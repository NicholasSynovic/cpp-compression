#ifndef __timer_h
#define __timer_h

#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include <time.h>
#include <sys/time.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define HighResTimer
#ifdef HighResTimer

   typedef struct timespec TimerType;

#else

   typedef struct timeval TimerType;

#endif

TimerType getTimeStamp(void);
uint64_t getClockTicks(void);
double getElapsedTime (TimerType t0, TimerType t1);
double getTicksPerSecond (void);

#ifdef __cplusplus
struct Timer
{
   TimerType timer_start, timer_stop;

   Timer() : timer_start(), timer_stop() {}
   Timer(const Timer& timer) : timer_start(timer.timer_start), timer_stop(timer.timer_stop) {}

   void start(void) { this->timer_start = getTimeStamp(); }
   void stop(void) { this->timer_stop = getTimeStamp(); }
   uint64_t ticks(void) const { return this->time() * getTicksPerSecond(); }
   double time(void) const { return getElapsedTime( this->timer_start, this->timer_stop ); }
};
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __timer_h
