/* 2010
 * Maciej Szeptuch
 * XIV LO Wrocław
 */
#include<string>
#include<vector>
#include<sys/stat.h>
#include<sstream>
#include<fstream>
#include<iostream>
#include "test.hpp"
#include "judge.hpp"
#include "command.hpp"
#include "time.hpp"

using namespace Judge;

JudgeLib::JudgeLib(void)
{
	source = "";
	testFile = "";
	group = 0;
	return;
}

JudgeLib::~JudgeLib(void)
{
	Command commander;
	commander.run("rm -r /tmp/easyjudge/");
	commander.cclose();
	return;
}

std::pair<bool, std::string> JudgeLib::validateOptions(std::string sSource,
	std::string sTestFile, unsigned int iGroup)
{
	struct stat st;
	std::ifstream file;
	std::string data;
	char *buffer = 0;
	int length = 0;
	if(stat(sSource.c_str(), &st))
		return std::pair<bool, std::string>(false, "Source file " + sSource
			+ " does not exist!");

	source = sSource;
	if(stat((sTestFile + ".tst").c_str(), &st))
	{
		if(stat((sTestFile + ".in").c_str(), &st) || stat((sTestFile +
					".out").c_str(), &st))
			return std::pair<bool, std::string>(false, "Test file " +
				sTestFile + "(.in/.out/.tst) does not exist!");

		testFile = sTestFile;
		tests.resize(1);
		tests[0].push_back(Test::TestCase(testFile));
	}

	else
	{
		file.open((sTestFile + ".tst").c_str());
		if(!file.good())
			return std::pair<bool, std::string>(false, "Cannot open test file " +
				sTestFile + ".tst!");

		file.seekg(0, std::ios::end);
		length = file.tellg();
		file.seekg(0);
		buffer = new char[length];
		file.read(buffer, length); //FIXME: Overflow...
		file.close();
		std::stringstream input(buffer);
		while(input.getline(buffer, 10240))
		{
			std::stringstream line(buffer);
			tests.push_back(std::vector<Test::TestCase>());
			while(line>>data)
				(*tests.rbegin()).push_back(Test::TestCase(data));
		}

		testFile = sTestFile;
	}

	if(iGroup)
	{
		if(tests.size() < iGroup)
			return std::pair<bool, std::string>(false, "No such group!");
	}

	group = iGroup;

	return std::pair<bool, std::string>(true, "");
}

std::pair<bool, std::string> JudgeLib::prepareEnvironment(void)
{
	Command commander;
	int result;
	std::string data;
	result = !commander.run("mkdir -p /tmp/easyjudge/");
	if(!result)
		data = commander.read(1024);

	result |= commander.cclose();
	if(result)
		return std::pair<bool, std::string>(false, data);

	return std::pair<bool, std::string>(true, "");
}

std::pair<bool, std::string> JudgeLib::compileSource(void)
{
	Command commander;
	int result;
	std::string data,
				one;
	result = !commander.run("cp " + source + " /tmp/easyjudge/source.cpp");
	if(!result)
		data = commander.read(1024);

	result |= commander.cclose();
	if(result)
		return std::pair<bool, std::string>(false, data);

	std::cerr<<"->Compilation \33[0;34m"<<source<<"\33[0m\t";
	commander.setTimeLimit(30);
	commander.setMemoryLimit(0);
	result = !commander.run("g++ -O2 --static -lm /tmp/easyjudge/source.cpp -o /tmp/easyjudge/source.e 2>&1");
	data = "";
	if(!result)
	{
		do
		{
			data += one = commander.read(1024);
		}
		while(!one.empty());
	}

	result |= commander.cclose();
	std::cerr<<"\33[0;37m"<<commander.getUserTime()<<"\33[1;37ms\33[0m";
	std::cerr<<"/\33[0;37m30\33[1;37ms\33[0m\t";
	if(result)
	{
		std::cerr<<"\t[\33[1;31mERROR\33[0m]"<<std::endl;
		return std::pair<bool, std::string>(false, data);
	}

	std::cerr<<"[\33[1;32mOK\33[0m]"<<std::endl;
	return std::pair<bool, std::string>(true, "");
}

std::pair<bool, std::string> JudgeLib::checkTests(void)
{
	std::pair<bool, std::string> result;
	for(unsigned int g = 0; g < tests.size(); ++ g)
		for(unsigned int t = 0; t < tests[g].size(); ++ t)
		{
			result = tests[g][t].validate();
			if(!result.first)
				return result;
		}

	return std::pair<bool, std::string>(true, "");
}

std::pair<int, int> JudgeLib::runTest(int Group, int number)
{
	Command commander;
	int result;
	std::string data;
	std::cerr<<"-->Test \33[0;34m"<< number + 1<<"\33[0m["<<tests[Group][number].getPoints()<<"]\t";
	result = !commander.run("cp " + tests[Group][number].getPath() + ".in /tmp/easyjudge/test.in 2>&1");
	if(!result)
		data = commander.read(1024);

	result |= commander.cclose();
	if(result)
	{
		std::cerr<<"\t[\33[1;31mERROR\33[0m]"<<std::endl;
		std::cerr<<data;
		return std::pair<int, int> (0, 0);
	}

	result = !commander.run("cat " + tests[Group][number].getPath() + ".out > /tmp/easyjudge/test.out 2>&1");
	data = "";
	if(!result)
		data = commander.read(1024);

	result |= commander.cclose();
	if(result)
	{
		std::cerr<<"\t[\33[1;31mERROR\33[0m]"<<std::endl;
		std::cerr<<data;
		return std::pair<int, int> (0, 0);
	}

	commander.setTimeLimit(tests[Group][number].getTimeLimit() * 1.5 + 1);
	commander.setMemoryLimit(tests[Group][number].getMemoryLimit() * 2 + 1024);
	result = !commander.run("/tmp/easyjudge/source.e < /tmp/easyjudge/test.in > /tmp/easyjudge/source.out");
	result |= commander.cclose();
	/* TIME */
	std::cerr<<"\33[0;37m"<<commander.getUserTime()<<"\33[1;37ms\33[0m";
	std::cerr<<"/\33[0;37m"<<tests[Group][number].getTimeLimit()<<"\33[1;37ms\33[0m\t";

	/* MEMORY */
	std::cerr<<"\33[0;37m"<<commander.getUserMemory()<<"\33[1;37mkb\33[0m";
	std::cerr<<"/\33[0;37m"<<tests[Group][number].getMemoryLimit()<<"\33[1;37mkb\33[0m\t";

	if(commander.getUserMemory() > tests[Group][number].getMemoryLimit())
		std::cerr<<"[\33[1;35mMLE\33[0m]"<<" {"<<tests[Group][number].getPath()<<"}"<<std::endl;

	else if(commander.getUserTime() > tests[Group][number].getTimeLimit())
		std::cerr<<"[\33[1;35mTLE\33[0m]"<<" {"<<tests[Group][number].getPath()<<"}"<<std::endl;

	else if(result)
		std::cerr<<"[\33[1;35mNZEC\33[0m] (\33[0;31m"<<result<<"\33[0m)"<<" {"<<tests[Group][number].getPath()<<"}"<<std::endl;

	else
	{
		std::stringstream points,
						  full;
		int time = (commander.getUserTime() - tests[Group][number].getTimeLimit() / 2) / (tests[Group][number].getTimeLimit() / 2 / tests[Group][number].getPoints());
		time = time > 0?time:0;
		points<<tests[Group][number].getPoints() - time;
		full<<tests[Group][number].getPoints();
		result = !commander.run("diff -iwbB /tmp/easyjudge/test.out /tmp/easyjudge/source.out");
		result |= commander.cclose();
		commander.run("rm /tmp/easyjudge/source.out");
		commander.cclose();
		if(!result && points.str() == full.str())
			std::cerr<<"[\33[1;32mAC\33[0m]";

		else
			std::cerr<<((!result)?"[\33[1;32m" + points.str() +
				"\33[0m]":"[\33[1;31mWA\33[0m]");

		if(result)
			std::cerr<<" {"<<tests[Group][number].getPath()<<"}";

		std::cerr<<std::endl;

		if(!result)
			return std::pair<int, int> (tests[Group][number].getPoints() - time, tests[Group][number].getPoints());
	}


	return std::pair<int, int> (0, tests[Group][number].getPoints());
}

void JudgeLib::runTests(void)
{
	int points = 0,
		maxPoints = 0,
		groupPoints = 0,
		maxGroupPoints = 0;
	bool zero = false;
	std::pair<int, int> result;
	for(unsigned int g = 0; g < tests.size(); ++ g)
	{
		if(group && g != group - 1)
			continue;

		groupPoints = 0;
		maxGroupPoints = 0;
		zero = false;
		std::cerr<<"------>GROUP \33[1;34m"<< g + 1<<"\33[0m"<<std::endl;
		for(unsigned int t = 0; t < tests[g].size(); ++ t)
		{
			result = runTest(g, t);
			if(!result.first && result.second)
				zero = true;

			groupPoints += result.first;
			maxGroupPoints += result.second;
		}

		if(zero)
			groupPoints = 0;

		points += groupPoints;
		maxPoints += maxGroupPoints;
		std::cerr<<"Group: \t\33[1;34m"<<groupPoints<<"\33[0m/\33[1;34m"<<maxGroupPoints<<"\33[0m"<<std::endl<<std::endl;
	}

	std::cerr<<"Overall: \t\33[1;34m"<<points<<"\33[0m/\33[1;34m"<<maxPoints<<"\33[0m"<<std::endl<<std::endl;
	return;
}
