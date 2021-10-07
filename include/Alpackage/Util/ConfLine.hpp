#pragma once

#include <Alpackage/Util/Logging.hpp>

#include <cstring>
#include <istream>
#include <sstream>
#include <stdexcept>
#include <string>

struct ConfLine {
  std::string name;
  std::string value;

  bool        operator== (ConfLine const& other) const {
    return name == other.name && value == other.value;
  }

  friend std::ostream& operator<< (std::ostream&   out,
                                   ConfLine const& instance) {
    out << "{ \"name\": \"" << instance.name << "\", \"value\": \""
        << instance.value << "\" }";
    return out;
  }


  friend std::istream& operator>> (std::istream& in, ConfLine& instance) {
    Log::debug ("Parsing ConfLine.");
    reckless::scoped_indent indent;
    char                    buffer[512];
    std::stringstream       name;
    std::stringstream       value;
    int                     point = 0;
    int                     mark  = 0;
    int                     len;
    do {
      in.getline (buffer, 512);
      len = strlen (buffer);
    } while (len == 0);
    Log::debug ("Acquired line of length %d: %s", len, buffer);
    while (point < len && isWhitespace (buffer[point])) point++;
    Log::debug ("Discarded '%d' white space chars: %s", point, buffer + point);
    mark = point;
    while (point < len && !isWhitespace (buffer[point]) && buffer[point] != '=')
      point++;
    Log::debug ("Skipped '%d' non-whitespace chars: %s",
                point - mark,
                buffer + point);
    char cbuf[512];
    std::strncpy (cbuf, buffer + mark, point - mark);
    cbuf[point - mark] = '\0';
    Log::debug ("Copy skipped chars in separate buffer: %s", cbuf);
    name << cbuf;
    instance.name = name.str ( );
    mark          = point;
    while (point < len && isWhitespace (buffer[point])) point++;
    Log::debug ("Discarded '%d' white space chars: %s",
                point - mark,
                buffer + point);
    if (point < len && buffer[point] != '=') {
      Log::error ("Invalid ConfLine; While parsing name, found: '%s' where "
                  "'=' was expected.",
                  buffer[point]);
      Log::flush ( );
      throw std::runtime_error ("Invalid ConfLine");
    } else
      point++;
    Log::debug ("Skipped '=': %s", cbuf);
    mark = point;
    while (point < len && isWhitespace (buffer[point])) point++;
    Log::debug ("Discarded '%d' white space chars: %s",
                point - mark,
                buffer + point);
    value << (buffer + point);
    instance.value = value.str ( );
    Log::debug ("Pass rest of buffer to value: %s", buffer + point);
    Log::flush ( );
    return in;
  }

  private:
  static bool isWhitespace (char c) {
    switch (c) {
      case ' ':
      case '\n':
      case '\r':
      case '\t': return true;
      default: return false;
    }
  }
};
