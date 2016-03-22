/* 2010
 * Maciej Szeptuch
 * XIV LO Wrocław
 */
#include<iostream>
#include<cstdlib>
#include<string>
#include<sstream>
#include "time.hpp"
#include "judge.hpp"

Time::TimeIt timer;
Judge::JudgeLib judge;
unsigned int group;
std::pair<bool, std::string> result;

int main(int argc, char **argv)
{
	std::cerr.precision(3);
	std::cerr.setf(std::ios::fixed, std::ios::floatfield);
	timer.start();
	if(argc < 3)
	{
		std::cerr<<HELP_MSG;
		std::cerr<<"Not enough arguments!"<<std::endl;
		return 1;
	}

	if(argc > 3)
	{
		std::stringstream temp(argv[3]);
		temp>>group;
	}

	else
		group = 0;

	result = judge.validateOptions(argv[1], argv[2], group);
	if(!result.first)
	{
		std::cerr<<HELP_MSG;
		std::cerr<<result.second<<std::endl;
		return 1;
	}

	result = judge.prepareEnvironment();
	if(!result.first)
	{
		std::cerr<<HELP_MSG;
		std::cerr<<result.second<<std::endl;
		return 1;
	}

	std::cerr<<"TASK \33[1;37m"<<argv[2]<<"\33[0m"<<std::endl;
	result = judge.checkTests();
	if(!result.first)
	{
		std::cerr<<HELP_MSG;
		std::cerr<<result.second<<std::endl;
		return 1;
	}

	result = judge.compileSource();
	if(!result.first)
	{
		std::cerr<<HELP_MSG;
		std::cerr<<result.second<<std::endl;
		return 1;
	}

	judge.runTests();
	timer.stop();
	std::cerr<<"Judge execution time: \33[0;37m"<<timer.getTime()<<"\33[1;37ms\33[0m"<<std::endl;

	return 0;
}
