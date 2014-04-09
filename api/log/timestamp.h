#ifndef _TIMESTAMP_H
#define _TIMESTAMP_H

#include "../../includes/includes.h"
#include <string.h> // memcpy
#include <string>

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
};LOG_IN_EMBEDED

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


#endif 
