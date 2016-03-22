/* 2010
 * Maciej Szeptuch
 * XIV LO Wrocław
 * ----------
 *  Command running class
 */
#ifndef _COMMAND
#define _COMMAND
#include<string>
#include<sys/resource.h>

enum CommandState
{
	STOPPED,
	WORKING,
};

class Command
{
	private:
		rusage *usage;
		pid_t pid;
		FILE *input,
			 *output;
		CommandState state;
		long long unsigned int timeLimit,
							   memoryLimit;

	public:
		Command(void);
		~Command(void);
		bool run(std::string command);
		int cclose(void);
		void setTimeLimit(long long unsigned int seconds);
		void setMemoryLimit(long long unsigned int kilobytes);
		double getUserTime(void);
		double getSystemTime(void);
		long long unsigned int getUserMemory(void);
		bool write(std::string data);
		std::string read(unsigned int size);
};

#endif
