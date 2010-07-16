/*
 Copyright 2010 Greg Tener
 Released under the Lesser General Public License v3.
 */

/*
 A high resolution cross platform timer function.
 Used for relative timing, since I have no idea what the
 first call to this would mean.
 */

// start the calendar time #defines
#ifdef HAS_GETTIMEOFDAY

#include <sys/time.h>

#include <cstdio>
#include <cstdlib>

double calendar_time() {
  timeval tv;
  double dTime = 0;

  if (gettimeofday(&tv, NULL) == -1) {
    perror("Problem with gettimeofday()");
    exit(1);
  }

  dTime = tv.tv_sec;
  dTime += 1e-6 * tv.tv_usec;

  return dTime;
}

#elif HAS_QUERY_PERFORMANCE_COUNTER

#include <windows.h>  // NOLINT

double calendar_time() {
  double dTime = 0;
  LARGE_INTEGER nCount;
  LARGE_INTEGER nFreq;

  if (QueryPerformanceFrequency(&nFreq) == 0) {
    perror("Problem with QueryPerformanceFequency()");
    exit(1);
  }

  if (QueryPerformanceCounter(&nCount) == 0) {
    perror("Problem with QueryPerformanceCounter()");
    exit(1);
  }

  dTime = static_cast<double>(nCount.QuadPart) / nFreq.QuadPart;

  return dTime;
}

#else

#include <ctime>

// these are all terrible resolution clocks it seems

double calendar_time() {
  time_t dTime = 0;

  dTime = static_cast<double>(time(NULL) );

  return dTime;
}

#endif  // calendar time stuff
// start the cpu time #defines

#ifdef HAS_RUSAGE

#include <sys/time.h>  // NOLINT
#include <sys/resource.h>  // NOLINT

double cpu_time() {
  double dTime;
  struct rusage ruse;

  if (getrusage(RUSAGE_SELF, &ruse) != 0) {
    perror("Problem with QueryPerformanceFequency()");
    exit(1);
  }

  dTime = ruse.ru_utime.tv_sec + ruse.ru_stime.tv_sec + 1e-6
      * (ruse.ru_utime.tv_usec + ruse.ru_stime.tv_usec);

  return dTime;
}

#elif HAS_TIMES

#include <sys/times.h>  // NOLINT

double cpu_time() {
  double dTime = 0;
  struct tms timebuf;

  times(&timebuf);

  dTime = static_cast<double>(timebuf.tms_utime + timebuf.tms_stime)
  / CLOCKS_PER_SEC;

  return dTime;
}

#else

#include <ctime>  // NOLINT

double cpu_time() {
  double dTime = 0;

  dTime = static_cast<double>(clock() ) / CLOCKS_PER_SEC;

  return dTime;
}

#endif  // cpu time defines
#ifdef HRTIME_TEST

#include <cstdio>  // NOLINT

int main(int argc, char **argv) {
  int i = 0;
  int j = 0;
  double dCalendarTime = 0;
  double dCpuTime = 0;
  int n = 0;
  int m = 0;

  n = 100000;
  m = 10000;

  dCalendarTime = -calendar_time();
  dCpuTime = -cpu_time();

  for (i = 0; i < n; i++) {
    for (j = 0; j < m; j++) {
    }
  }

  dCalendarTime += calendar_time();
  dCpuTime += cpu_time();

  printf("Calendar Time: %.16lf\n", dCalendarTime);
  printf("CPU Time     : %.16lf\n", dCpuTime);

  return 0;
}

#endif  // HRTIME_TEST
