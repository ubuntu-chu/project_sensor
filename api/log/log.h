#ifndef __API_LOG_H__
#define __API_LOG_H__

#include "../../includes/includes.h"
#include "log_stream.h"
#include "timestamp.h"

#ifdef LOGGER

/*-----------------------------------------------
 *         default log level setting
 *----------------------------------------------*/

#define    DEFAULT_LOG_LEVEL        Logger::TRACE

class Logger
{
 public:
  enum LogLevel
  {
    TRACE = 0,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    NUM_LOG_LEVELS,
  };

  // compile time calculation of basename of source file
  class SourceFile
  {
   public:
    SourceFile(const char* filename)
      : data_(filename)
    {
      const char* slash = strrchr(filename, '\\');
      if (slash)
      {
        data_ = slash + 1;
      }
      size_ = static_cast<int>(strlen(data_));
    }

    const char* data_;
    int size_;
  };

  Logger(SourceFile file, int line);
  Logger(SourceFile file, int line, LogLevel level);
  Logger(SourceFile file, int line, LogLevel level, const char* func);
  Logger(SourceFile file, int line, bool toAbort);
  ~Logger();

  LogStream& stream() { return impl_.stream_; }

  static LogLevel logLevel();
  static void setLogLevel(LogLevel level);

  typedef void (*OutputFunc)(const char* msg, int len);
  typedef void (*FlushFunc)();
  static void setOutput(OutputFunc);
  static void setFlush(FlushFunc);

 private:

class Impl
{
 public:
  typedef Logger::LogLevel LogLevel;
  Impl(LogLevel level, int old_errno, const SourceFile& file, int line);
  void formatTime();
  void finish();

  Timestamp time_;
  LogStream stream_;
  LogLevel level_;
  int line_;
  SourceFile basename_;
};

  Impl impl_;

};

extern Logger::LogLevel g_logLevel;

inline Logger::LogLevel Logger::logLevel()
{
  return g_logLevel;
}

#define LOG_TRACE 		if (Logger::logLevel() <= Logger::TRACE) \
					  		Logger(__FILE__, __LINE__, Logger::TRACE, __func__).stream()
#define LOG_DEBUG 		if (Logger::logLevel() <= Logger::DEBUG) \
					  		Logger(__FILE__, __LINE__, Logger::DEBUG, __func__).stream()
#define LOG_INFO 		if (Logger::logLevel() <= Logger::INFO) \
					  		Logger(__FILE__, __LINE__).stream()
#define LOG_WARN 		Logger(__FILE__, __LINE__, Logger::WARN).stream()
#define LOG_ERROR 		Logger(__FILE__, __LINE__, Logger::ERROR).stream()
#define LOG_FATAL 		Logger(__FILE__, __LINE__, Logger::FATAL).stream()
#define LOG_SYSERR 		Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL 	Logger(__FILE__, __LINE__, true).stream()

#else

#define LOG_TRACE
#define LOG_DEBUG 
#define LOG_INFO 		
#define LOG_WARN 		
#define LOG_ERROR 		
#define LOG_FATAL 		
#define LOG_SYSERR 		
#define LOG_SYSFATAL

#endif

#endif


