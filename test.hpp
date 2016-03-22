/* 2010
 * Maciej Szeptuch
 * XIV LO Wrocław
 * ----------
 *  TestCase class
 */
#ifndef _TEST_CLASS
#define _TEST_CLASS
#define _NS_TEST namespace Test\
{
#define _NS_END }
#define VERSION 0.1
#define TIMELIMIT 30.0
#define MEMORYLIMIT 32768
#include<string>
#include<vector>

_NS_TEST

class TestCase
{
	private:
		std::string path;
		double timeLimit;
		long long unsigned int memoryLimit;
		unsigned int points;

	public:
		TestCase(std::string data = "");
		~TestCase(void);
		std::pair<bool, std::string> validate(void);
		double getTimeLimit(void);
		long long unsigned int getMemoryLimit(void);
		std::string getPath(void);
		unsigned int getPoints(void);
};

_NS_END
#endif
