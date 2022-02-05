#pragma once

#include <cstring>
#include <iostream>
#include <reckless/crash_handler.hpp>
#include <reckless/file_writer.hpp>
#include <reckless/policy_log.hpp>
#include <reckless/stdout_writer.hpp>

#define LOGNAME "Alpackage"

namespace detail {

class SeverityField {
  private:
  const char* severity;
  public:
  constexpr explicit SeverityField (const char* severity)
      : severity (severity){ };

  void format (reckless::output_buffer* poutputBuffer) const {
    size_t len = strlen (severity);
    char*  p   = poutputBuffer->reserve (len);
    strncpy (p, severity, len);
    poutputBuffer->commit (len);
  }
};

class CategoryField {
  private:
  const char* category;
  public:
  constexpr explicit CategoryField (const char* category)
      : category (category){ };

  void format (reckless::output_buffer* poutputBuffer) const {
    size_t len = strlen (category);
    auto*  p   = poutputBuffer->reserve (len + 2);
    *p         = '[';
    strncpy (p + 1, category, len);
    *(p + 1 + len) = ']';
    poutputBuffer->commit (len + 2);
  }
};

template<class HeaderField>
HeaderField constructHeaderField (const char* severity, const char* category) {
  return HeaderField ( );
}

template<>
inline SeverityField
  constructHeaderField<SeverityField> (const char* severity,
                                       const char* category) {
  return SeverityField (severity);
}

template<>
inline CategoryField
  constructHeaderField<CategoryField> (const char* severity,
                                       const char* category) {
  return CategoryField (category);
}

template<class IndentPolicy, char FieldSeparator, class... HeaderFields>
class CategorizedSeverityLog : public reckless::basic_log {
  public:
  using reckless::basic_log::basic_log;

  template<typename... Args>
  inline void debug (const char* category, char const* fmt, Args&&... args) {
    write ("DEBUG", category, fmt, std::forward<Args> (args)...);
  }
  template<typename... Args>
  inline void info (const char* category, char const* fmt, Args&&... args) {
    write ("INFO", category, fmt, std::forward<Args> (args)...);
  }
  template<typename... Args>
  inline void warn (const char* category, char const* fmt, Args&&... args) {
    write ("WARN", category, fmt, std::forward<Args> (args)...);
  }
  template<typename... Args>
  inline void error (const char* category, char const* fmt, Args&&... args) {
    write ("ERROR", category, fmt, std::forward<Args> (args)...);
  }

  private:
  template<typename... Args>
  inline void write (const char* severity,
                     const char* category,
                     char const* fmt,
                     Args&&... args) {
    reckless::basic_log::write<reckless::policy_formatter<IndentPolicy,
                                                          FieldSeparator,
                                                          HeaderFields...>> (
      constructHeaderField<HeaderFields> (severity, category)...,
      IndentPolicy ( ),
      fmt,
      std::forward<Args> (args)...);
  }
};

}     // namespace detail


class Log {
  private:
  static const std::string log_path;
  static Log
    l;     // NOLINT(cppcoreguidelines-avoid-non-const-global-variables):
           // Needs to call vendor non-const code
  Log ( )
      : filewriter (log_path.c_str ( ))
      , filelog (&filewriter)
      , errlog (&errwriter) { }

  using log_t = detail::CategorizedSeverityLog<reckless::indent<2>,
                                               ' ',
                                               detail::SeverityField,
                                               detail::CategoryField>;

  reckless::file_writer   filewriter;
  reckless::stderr_writer errwriter;
  log_t                   filelog;
  log_t                   errlog;

  public:
  ~Log ( ) = default;

  static inline void flush ( ) {
    l.filelog.flush ( );
    l.errlog.flush ( );
  }

  template<typename... Args>
  static inline void debug (char const* fmt, Args&&... arg) {
#ifndef NDEBUG
    l.filelog.debug (LOGNAME, fmt, arg...);
    l.errlog.debug (LOGNAME, fmt, arg...);
#endif
  }
  template<typename... Args>
  static inline void info (char const* fmt, Args&&... arg) {
    l.filelog.info (LOGNAME, fmt, arg...);
#ifndef NDEBUG
    l.errlog.info (LOGNAME, fmt, arg...);
#endif
  }
  template<typename... Args>
  static inline void warn (char const* fmt, Args&&... arg) {
    l.filelog.warn (LOGNAME, fmt, arg...);
    l.errlog.warn (LOGNAME, fmt, arg...);
  }
  template<typename... Args>
  static inline void error (char const* fmt, Args&&... arg) {
    l.filelog.error (LOGNAME, fmt, arg...);
    l.errlog.error (LOGNAME, fmt, arg...);
  }
};

#undef LOGNAME
