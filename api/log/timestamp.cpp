#include "timestamp.h"

Timestamp::Timestamp(time_t microSecondsSinceEpoch)
  : microSecondsSinceEpoch_(microSecondsSinceEpoch)
{
}

string Timestamp::toString() const
{
  char buf[32] = {0};
  time_t seconds = microSecondsSinceEpoch_ / kSecondsPerSecond;
  time_t microseconds = microSecondsSinceEpoch_ % kSecondsPerSecond;
  snprintf(buf, sizeof(buf)-1, "%d.%06d ", seconds, microseconds);
  return buf;
}

string Timestamp::toFormattedString() const
{
  char buf[32] = {0};
  time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / kSecondsPerSecond);
  int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kSecondsPerSecond);
  struct tm tm_time;
  gmtime_r(&seconds, &tm_time);

  snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
      tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
      tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
      microseconds);
  return buf;
}

Timestamp Timestamp::now()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  time_t seconds = tv.tv_sec;
  return Timestamp(seconds * kSecondsPerSecond + tv.tv_usec);
}

Timestamp Timestamp::invalid()
{
  return Timestamp();
}

