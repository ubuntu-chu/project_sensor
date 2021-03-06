#include "log.h"

#ifdef LOGGER

portBASE_TYPE 	errno	= 0;

const char *strerror_array[] = {
    "Einit",
};

char t_time[32];
time_t t_lastSecond = static_cast<time_t>(-1);

const char* strerror_tl(int savedErrno)
{
  return strerror_array[savedErrno];
}

Logger::LogLevel initLogLevel()
{
    return Logger::TRACE;
}

Logger::LogLevel g_logLevel = initLogLevel();

const char* LogLevelName[Logger::NUM_LOG_LEVELS] =
{
  "TRACE ",
  "DEBUG ",
  "INFO  ",
  "WARN  ",
  "ERROR ",
  "FATAL ",
};

// helper class for known string length at compile time
class T
{
 public:
  T(const char* str, unsigned len)
    :str_(str),
     len_(len)
  {
    //assert(strlen(str) == len_);
  }

  const char* str_;
  const unsigned len_;
};

inline LogStream& operator<<(LogStream& s, T v)
{
  s.append(v.str_, v.len_);
  return s;
}

inline LogStream& operator<<(LogStream& s, const Logger::SourceFile& v)
{
  s.append(v.data_, v.size_);
  return s;
}

void defaultOutput(const char* msg, int len)
{
    
}

void defaultFlush()
{
    
}

Logger::OutputFunc g_output = defaultOutput;
Logger::FlushFunc g_flush = defaultFlush;

Logger::Impl::Impl(LogLevel level, int savedErrno, const SourceFile& file, int line)
  : time_(Timestamp::now()),
    stream_(),
    level_(level),
    line_(line),
    basename_(file)
{
  formatTime();
  stream_ << T("pid = 1 ", 8);
  stream_ << T(LogLevelName[level], 6);
  if (savedErrno != 0)
  {
    stream_ << strerror_tl(savedErrno) << " (errno=" << savedErrno << ") ";
  }
}

void Logger::Impl::formatTime()
{
    time_t microSecondsSinceEpoch = time_.microSecondsSinceEpoch();
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / kAccuracyPerSecond);
    int microseconds = static_cast<int>(microSecondsSinceEpoch % kAccuracyPerSecond);
    
    if (seconds != t_lastSecond){
        t_lastSecond = seconds;
        struct tm tm_time;
        gmtime_r(&seconds, &tm_time); // FIXME TimeZone::fromUtcTime

        int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
            tm_time.tm_year + TM_YEAR_BASE, tm_time.tm_mon + 1, tm_time.tm_mday,
            tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
        //ASSERT(len == 17); (void)len;
    }
#ifdef LOG_IN_EMBEDED
    //精确到ms
    Fmt us(".%03d ", microseconds);
    stream_ << T(t_time, 17) << T(us.data(), 5);
#else
    //精确到us
    Fmt us(".%06d ", microseconds);
    //Fmt us(".%06dZ ", microseconds);
    //ASSERT(us.length() == 9);
    stream_ << T(t_time, 17) << T(us.data(), 8);
#endif
}

void Logger::Impl::finish()
{
  stream_ << " - " << basename_ << ':' << line_ << '\n';
}

Logger::Logger(SourceFile file, int line)
  : impl_(INFO, 0, file, line)
{
}

Logger::Logger(SourceFile file, int line, LogLevel level, const char* func)
  : impl_(level, 0, file, line)
{
  impl_.stream_ << func << ' ';
}

Logger::Logger(SourceFile file, int line, LogLevel level)
  : impl_(level, 0, file, line)
{
}

Logger::Logger(SourceFile file, int line, bool toAbort)
  : impl_(toAbort?FATAL:ERROR, errno, file, line)
{
}

Logger::~Logger()
{
  impl_.finish();
  const LogStream::Buffer& buf(stream().buffer());
  g_output(buf.data(), buf.length());
  if (impl_.level_ == FATAL)
  {
    g_flush();
    //while (1);
  }
}

void Logger::setLogLevel(Logger::LogLevel level)
{
    g_logLevel = level;
}

void Logger::setOutput(OutputFunc out)
{
  g_output = out;
}

void Logger::setFlush(FlushFunc flush)
{
  g_flush = flush;
}

#else


#endif

