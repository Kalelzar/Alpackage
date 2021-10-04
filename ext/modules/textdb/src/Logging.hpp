#pragma once

#include <reckless/file_writer.hpp>
#include <reckless/severity_log.hpp>
#include <reckless/stdout_writer.hpp>

#define LOGNAME "TextDB"

class Log {
  private:
  static Log l;
  Log ( )
      : filewriter (LOGNAME ".log")
      , filelog (&filewriter)
      , errwriter ( )
      , errlog (&errwriter) { }

  using log_t = reckless::
    severity_log<reckless::indent<2>, '\t', reckless::severity_field>;

  reckless::file_writer   filewriter;
  reckless::stderr_writer errwriter;
  log_t                   filelog;
  log_t                   errlog;

  public:
  template<typename... Args>
  static void debug (char const* fmt, Args&&... arg) {
    l.filelog.debug (fmt, arg...);
#ifndef NDEBUG
    l.errlog.debug (fmt, arg...);
#endif
  }
  template<typename... Args> static void info (char const* fmt, Args&&... arg) {
    l.filelog.info (fmt, arg...);
#ifndef NDEBUG
    l.errlog.info (fmt, arg...);
#endif
  }
  template<typename... Args> static void warn (char const* fmt, Args&&... arg) {
    l.filelog.warn (fmt, arg...);
    l.errlog.warn (fmt, arg...);
  }
  template<typename... Args>
  static void error (char const* fmt, Args&&... arg) {
    l.filelog.error (fmt, arg...);
    l.errlog.error (fmt, arg...);
  }
};

Log Log::l;
