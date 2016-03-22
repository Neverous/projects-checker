/* 2010
 * Maciej Szeptuch
 * XIV LO Wrocław
 */
#include "time.hpp"

using namespace Time;

TimeIt::TimeIt(void)
{
	clear();
	return;
}

TimeIt::~TimeIt(void)
{
	return;
}

bool TimeIt::start(void)
{
	if(state == STARTED)
		return false;

	struct timeb sysTime;
	ftime(&sysTime);
	startTime = (long long unsigned int)sysTime.time * 1000 +
		sysTime.millitm;
	state = STARTED;

	return true;
}

bool TimeIt::stop(void)
{
	if(state == STOPPED)
		return false;

	struct timeb sysTime;
	ftime(&sysTime);
	milliseconds += (long long unsigned int)sysTime.time * 1000 +
		sysTime.millitm - startTime;
	state = STOPPED;

	return true;
}

double TimeIt::getTime(void)
{
	if(state == STARTED)
		return -1;

	return (double)milliseconds / 1000;
}

void TimeIt::clear(void)
{
	milliseconds = 0;
	startTime = 0;
	state = STOPPED;

	return;
}

