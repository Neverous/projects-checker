/* 2010
 * Maciej Szeptuch
 * XIV LO Wrocław
 */
#include<string>
#include<sstream>
#include<vector>
#include<sys/stat.h>
#include "test.hpp"

using namespace Test;

TestCase::TestCase(std::string data)
{
	path = "";
	timeLimit = TIMELIMIT;
	memoryLimit = MEMORYLIMIT;
	points = 1;
	if(data.empty())
		return;

	std::stringstream input(data);
	std::string sTime,
				sMemory,
				sPoints;

	getline(input, path, ';');
	getline(input, sTime, ';');
	getline(input, sMemory, ';');
	getline(input, sPoints, ';');

	std::stringstream time(sTime);
	time>>timeLimit;

	std::stringstream memory(sMemory);
	memory>>memoryLimit;

	std::stringstream point(sPoints);
	point>>points;
	return;
}

TestCase::~TestCase(void)
{
	return;
}

std::pair<bool, std::string> TestCase::validate(void)
{
	struct stat st;
	if(stat((path + ".in").c_str(), &st))
		return std::pair<bool, std::string>(false, "Test file " + path +
			".in does not exist!");

	if(stat((path + ".out").c_str(), &st))
		return std::pair<bool, std::string>(false, "Test file " + path +
			".out does not exist!");

	return std::pair<bool, std::string>(true, "");
}

double TestCase::getTimeLimit(void)
{
	return timeLimit;
}

long long unsigned int TestCase::getMemoryLimit(void)
{
	return memoryLimit;
}

std::string TestCase::getPath(void)
{
	return path;
}

unsigned int TestCase::getPoints(void)
{
	return points;
}
