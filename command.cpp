/* 2010
 * Maciej Szeptuch
 * XIV LO Wrocław
 */
#include<string>
#include<cstdlib>
#include<cstring>
#include<cstdio>
#include<unistd.h>
#include<sys/wait.h>
#include "command.hpp"

Command::Command(void)
{
	this->state = STOPPED;
	this->usage = NULL;
	this->input = NULL;
	this->output = NULL;
	this->pid = -1;
	this->timeLimit = 0;
	this->memoryLimit = 0;
	return;
}

Command::~Command(void)
{
	if(this->usage)
	{
		delete this->usage;
		this->usage = NULL;
	}

	return;
}

void Command::setTimeLimit(long long unsigned int seconds)
{
	this->timeLimit = seconds;
	return;
}

void Command::setMemoryLimit(long long unsigned int kilobytes)
{
	this->memoryLimit = kilobytes;
	return;
}

double Command::getUserTime(void)
{
	if(!this->usage || this->state != STOPPED)
		return -1.0;

	return (double)this->usage->ru_utime.tv_sec + (double)this->usage->ru_utime.tv_usec / 1000000.0;
}

double Command::getSystemTime(void)
{
	if(!this->usage || this->state != STOPPED)
		return -1.0;

	return (double)this->usage->ru_stime.tv_sec + (double)this->usage->ru_stime.tv_usec / 1000000.0;
}

long long unsigned int Command::getUserMemory(void)
{
	if(!this->usage || this->state != STOPPED)
		return 0;

	return (long long unsigned int)this->usage->ru_maxrss;
}

bool Command::run(std::string command)
{
	if(this->state != STOPPED)
		return false;

	int inp[2],
		outp[2];
	if(pipe(inp))
		return false;

	if(pipe(outp))
		return false;

	switch(this->pid = fork())
	{
		case -1:
			close(inp[0]);
			close(inp[1]);
			close(outp[0]);
			close(outp[1]);
			return false;
			break;

		case 0: /* CHILD */
			/* STDERR */
			dup2(STDOUT_FILENO, STDERR_FILENO);
			close(STDERR_FILENO);

			/* STDIN */
			close(inp[1]);
			dup2(inp[0], STDIN_FILENO);
			close(inp[0]);

			/* STDOUT */
			close(outp[0]);
			dup2(outp[1], STDOUT_FILENO);
			close(outp[1]);

			if(this->timeLimit)
			{
				rlimit limit;
				getrlimit(RLIMIT_CPU, &limit);
				limit.rlim_cur = this->timeLimit;
				setrlimit(RLIMIT_CPU, &limit);
			}

			if(this->memoryLimit)
			{
				rlimit limit;
				getrlimit(RLIMIT_AS, &limit);
				limit.rlim_cur = this->memoryLimit * 1024;
				setrlimit(RLIMIT_AS, &limit);

				getrlimit(RLIMIT_STACK, &limit);
				limit.rlim_cur = this->memoryLimit * 1024;
				setrlimit(RLIMIT_STACK, &limit);
			}

			execlp("sh", "sh", "-c", command.c_str(), NULL);
			_exit(127);
			break;
	}

	close(inp[0]);
	this->input = fdopen(inp[1], "w");
	//close(inp[1]);

	close(outp[1]);
	this->output = fdopen(outp[0], "r");
	//close(outp[0]);
	this->state = WORKING;
	return true;
}

bool Command::write(std::string data)
{
	if(this->state != WORKING)
		return false;

	return fwrite(data.c_str(), sizeof(char), data.length(), this->input);
}

std::string Command::read(unsigned int size)
{
	if(this->state != WORKING)
		return "";

	std::string result;
	char buffer[1024];
	while(fgets(buffer, 1024, this->output) != 0 && result.length() < size)
		result += buffer;

	return result;
}

int Command::cclose(void)
{
	if(this->state != WORKING)
		return 1;

	int status;
	fclose(this->input);
	fclose(this->output);
	this->input = NULL;
	this->output = NULL;
	if(this->usage)
		delete this->usage;

	this->usage = new rusage();
	wait4(this->pid, &status, 0, this->usage);
	this->pid = -1;
	this->state = STOPPED;
	if(WIFEXITED(status))
		return WEXITSTATUS(status);

	if(WIFSIGNALED(status))
		return WTERMSIG(status);

	return -1;
}
