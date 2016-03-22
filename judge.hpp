/* 2010
 * Maciej Szeptuch
 * XIV LO Wrocław
 * ----------
 *  Easy Judge Library
 */
#ifndef _JUDGE
#define _JUDGE
#define _NS_JUDGE namespace Judge\
{
#define _NS_END }
#define JUDGE_VERSION 0.200
#define HELP_MSG "Easy judge v"<<JUDGE_VERSION<<" Copyright (c) Maciej Szeptuch 2009-12\nUsage judge <source> <tests> <number>\n\
Where:\n\
	* <source> is source code\n\
	* <tests> name of test group(.tst file) or simple test(without extension, .in and .out files required)\n\
	* <number> subgroup number\n\
\n\
Example:\n\
	judge add.cpp add 5\n\n"
#include<string>
#include<vector>
#include "test.hpp"

_NS_JUDGE

class JudgeLib
{
	private:
		std::vector<std::vector<Test::TestCase> > tests;
		std::string source,
					testFile;
		unsigned int group;

	public:
		JudgeLib(void);
		~JudgeLib(void);
		std::pair<bool, std::string> validateOptions(std::string sSource, std::string sTestFile, unsigned int iGroup = 0);
		std::pair<bool, std::string> prepareEnvironment(void);
		std::pair<bool, std::string> compileSource(void);
		std::pair<bool, std::string> checkTests(void);
		std::pair<int, int> runTest(int group, int number);
		void runTests(void);
};

_NS_END

#endif
