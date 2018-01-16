#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#else
  #ifndef __USE_BSD
    #define __USE_BSD
  #endif
  #include <sys/time.h>
#endif

#ifdef WIN32
double PCFreq = 0.0;
__int64 timerStart = 0;
#else
struct timeval timerStart;
double totalTime;

double totalTime_min,totalTime_max, totalTime_tot;

#endif



void StartTimer()
{
#ifdef WIN32
  LARGE_INTEGER li;
  if(!QueryPerformanceFrequency(&li))
    printf("QueryPerformanceFrequency failed!\n");

  PCFreq = (double)li.QuadPart/1000.0;

  QueryPerformanceCounter(&li);
  timerStart = li.QuadPart;
#else
  totalTime = 0.0; 
  gettimeofday(&timerStart, NULL);
#endif
}

void ReStartTimer()
{
#ifdef WIN32
  LARGE_INTEGER li;
  if(!QueryPerformanceFrequency(&li))
    printf("QueryPerformanceFrequency failed!\n");

  PCFreq = (double)li.QuadPart/1000.0;

  QueryPerformanceCounter(&li);
  timerStart = li.QuadPart;
#else
  gettimeofday(&timerStart, NULL);
#endif
}


void StopTimer()
{
#ifdef WIN32
  LARGE_INTEGER li;
  QueryPerformanceCounter(&li);
  return (double)(li.QuadPart-timerStart)/PCFreq;
#else
  struct timeval timerStop, timerElapsed;
  gettimeofday(&timerStop, NULL);
  timersub(&timerStop, &timerStart, &timerElapsed);

  totalTime += timerElapsed.tv_sec*1000.0+timerElapsed.tv_usec/1000.0;

    /* return totalTime; */

#endif
}
void GetTimerStats(int n)
{
 printf("min = %0.3f ms\n", totalTime_min);
 printf("max = %0.3f ms\n", totalTime_max);
 printf("avg = %0.3f ms\n", totalTime_tot/n);
}
void ResetTimerStats()
{

 totalTime_min = 9999999 ;
 totalTime_max= 0.0;
 totalTime_tot= 0.0;
}

// time elapsed in ms
void GetTimer()
{
#ifdef WIN32
  LARGE_INTEGER li;
  QueryPerformanceCounter(&li);
  return (double)(li.QuadPart-timerStart)/PCFreq;
#else
  struct timeval timerStop, timerElapsed;
  gettimeofday(&timerStop, NULL);
  timersub(&timerStop, &timerStart, &timerElapsed);

  totalTime += timerElapsed.tv_sec*1000.0+timerElapsed.tv_usec/1000.0;
  

  //printf("%0.3f ms\n", totalTime);
  totalTime_min = (totalTime_min> totalTime)? totalTime: totalTime_min;
  totalTime_max= (totalTime_max< totalTime)? totalTime: totalTime_max;
  totalTime_tot += totalTime;

    /* return totalTime; */

#endif
}

