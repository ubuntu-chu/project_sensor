#ifndef _TIMESTAMP_H
#define _TIMESTAMP_H

#include "../includes/includes-low.h"
#include <string.h> // memcpy
#include <string>

using std::string;
using std::snprintf;

#ifdef LOG_IN_EMBEDED
static const long int kMilliSecondsPerSecond = 1000;
static const long int kSecondsPerSecond      = kMilliSecondsPerSecond;
#else
static const long int kMicroSecondsPerSecond = 1000*1000;
static const long int kSecondsPerSecond      = kMicroSecondsPerSecond;
#endif

class Timestamp : public copyable
{
 public:
  Timestamp()
    : microSecondsSinceEpoch_(0)
  {
  }

  explicit Timestamp(time_t microSecondsSinceEpoch);

  void swap(Timestamp& that)
  {
      time_t  tmp = that.microSecondsSinceEpoch_;
      
      that.microSecondsSinceEpoch_ = microSecondsSinceEpoch_;
      microSecondsSinceEpoch_      = tmp;
  }

  string toString() const;
  string toFormattedString() const;

  bool valid() const { return microSecondsSinceEpoch_ > 0; }

  time_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }
  time_t secondsSinceEpoch() const
  { return static_cast<time_t>(microSecondsSinceEpoch_ / kSecondsPerSecond); }

  ///
  /// Get time of now.
  ///
  static Timestamp now();
  static Timestamp invalid();

 private:
  time_t microSecondsSinceEpoch_;
};

inline bool operator<(Timestamp lhs, Timestamp rhs)
{
  return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator==(Timestamp lhs, Timestamp rhs)
{
  return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

///
/// Gets time difference of two timestamps, result in seconds.
///
/// @param high, low
/// @return (high-low) in seconds
/// @c double has 52-bit precision, enough for one-microseciond
/// resolution for next 100 years.
inline double timeDifference(Timestamp high, Timestamp low)
{
  time_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
  return static_cast<double>(diff) / kSecondsPerSecond;
}

///
/// Add @c seconds to given timestamp.
///
/// @return timestamp+seconds as Timestamp
///
inline Timestamp addTime(Timestamp timestamp, double seconds)
{
  time_t delta = static_cast<time_t>(seconds * kSecondsPerSecond);
  return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}


#ifdef  def_MINI_TIME_LIBRARY

#if 0
/*
 * Structure returned by gettimeofday(2) system call,
 * and used in other calls.
 */
struct timeval {
	long	tv_sec;		/* seconds */
	long	tv_usec;	/* and microseconds */
};

/*
 * Structure defined by POSIX.1b to be like a timeval.
 */
struct timespec {
	time_t	tv_sec;		/* seconds */
	long	tv_nsec;	/* and nanoseconds */
};

struct timezone {
  int tz_minuteswest;	/* minutes west of Greenwich */
  int tz_dsttime;	/* type of dst correction */
};

struct tm {
  int tm_sec;			/* Seconds.	[0-60] (1 leap second) */
  int tm_min;			/* Minutes.	[0-59] */
  int tm_hour;			/* Hours.	[0-23] */
  int tm_mday;			/* Day.		[1-31] */
  int tm_mon;			/* Month.	[0-11] */
  int tm_year;			/* Year - 1900. */
  int tm_wday;			/* Day of week.	[0-6] */
  int tm_yday;			/* Days in year.[0-365]	*/
  int tm_isdst;			/* DST.		[-1/0/1]*/

  long int tm_gmtoff;		/* Seconds east of UTC.  */
  const char *tm_zone;		/* Timezone abbreviation.  */
};

#endif

struct tm *gmtime_r(const time_t *timep, struct tm *r);
struct tm* localtime_r(const time_t* t, struct tm* r);
struct tm* localtime(const time_t* t);
time_t mktime(struct tm * const t);
char *asctime_r(const struct tm *t, char *buf);
char *asctime(const struct tm *timeptr);
char *ctime(const time_t *timep);
int gettimeofday(struct timeval *tp, void *ignore);

#endif


#endif 
