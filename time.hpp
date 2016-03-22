/* 2010
 * Maciej Szeptuch
 * XIV LO Wrocław
 * ----------
 *  Measure time with milliseconds precise
 */
#ifndef _TIME_MEASURE
#define _TIME_MEASURE
#define _NS_TIME namespace Time\
{
#define _NS_END }
#include<sys/timeb.h>

_NS_TIME

enum TimerState
{
	STARTED,
	STOPPED,
};

class TimeIt
{
	public:
		TimeIt(void);
		~TimeIt(void);
		bool start(void);
		bool stop(void);
		double getTime(void);
		void clear(void);

	private:
		long long unsigned int milliseconds,
							   startTime;
		TimerState state;
};

_NS_END
#endif
