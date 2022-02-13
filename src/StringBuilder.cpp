#include <Kal/StringBuilder.hpp>

#include <cassert>
#include <cstring>


StringBuilder::StringBuilder ( ) = default;
StringBuilder::~StringBuilder ( ) {
  if (buffer) {
    free (buffer);
    buffer         = nullptr;
    bufferCapacity = 0;
    bufferSize     = 0;
  }
}

StringBuilder::StringBuilder (StringBuilder&& other) noexcept
    : buffer (other.buffer)
    , bufferCapacity (other.bufferCapacity)
    , bufferSize (other.bufferSize) {
  if (&other == this) return;
  other.buffer         = nullptr;
  other.bufferCapacity = 0;
  other.bufferSize     = 0;
}

StringBuilder& StringBuilder::operator= (StringBuilder&& other) noexcept {
  if (&other != this) {
    buffer               = other.buffer;
    bufferCapacity       = other.bufferCapacity;
    bufferSize           = other.bufferSize;

    other.buffer         = nullptr;
    other.bufferCapacity = 0;
    other.bufferSize     = 0;
  }
  return *this;
}

StringBuilder::StringBuilder (StringBuilder const& other)
    : bufferCapacity (other.bufferCapacity)
    , bufferSize (other.bufferSize) {
  if (&other == this) return;
  if (buffer) { free (buffer); }
  if (other.buffer) {
    buffer = strdup (other.buffer);
  } else
    buffer = nullptr;
}

StringBuilder& StringBuilder::operator= (StringBuilder const& other) {
  if (&other != this) {
    if (buffer) free (buffer);
    if (other.buffer) {
      buffer = strdup (other.buffer);
    } else
      buffer = nullptr;
    bufferCapacity = other.bufferCapacity;
    bufferSize     = other.bufferSize;
  }
  return *this;
}

void StringBuilder::expand ( ) {
  if (bufferCapacity == 0) {
    if (buffer)
      buffer = (char*) realloc (buffer, 16);
    else
      buffer = (char*) malloc (16);
    if (!buffer) assert (false && "Out of memory");
    bufferCapacity = 16;
  } else {
    size_t newCapacity = bufferCapacity * 2;
    if (buffer)
      buffer = (char*) realloc (buffer, newCapacity);
    else
      buffer = (char*) malloc (newCapacity);
    if (!buffer) assert (false && "Out of memory");
    bufferCapacity = newCapacity;
  }
}

void StringBuilder::put (const char* str) { put (str, strlen (str)); }
void StringBuilder::putr (const char* str) { putr (str, strlen (str)); }


void StringBuilder::put (const char* str, size_t size) {
  if (size == 0 || !str) return;
  while (!buffer || size + bufferSize >= bufferCapacity - 1) {
    // FIXME: This can be improved by calculating a suitable capacity
    //        and only expanding once to that.
    expand ( );
  }


  strncpy (buffer + bufferSize, str, size);
  buffer[bufferSize + size] = '\0';
  bufferSize += size;
}

void StringBuilder::putr (const char* str, size_t size) {
  if (size == 0 || !str) return;
  while (!buffer || size + bufferSize >= bufferCapacity - 1) {
    // FIXME: This can be improved by calculating a suitable capacity
    //        and only expanding once to that.
    expand ( );
  }

  for (size_t i = 0; i < size; i++) {
    buffer[bufferSize + i] = str[size - i - 1];
  }

  buffer[bufferSize + size] = '\0';
  bufferSize += size;
}

void StringBuilder::put (std::string const& str) {
  put (str.c_str ( ), str.size ( ));
}

void StringBuilder::putr (std::string const& str) {
  putr (str.c_str ( ), str.size ( ));
}

std::string StringBuilder::get ( ) const {
  if (buffer)
    return {buffer};
  else
    return "";
}
std::string StringBuilder::getr ( ) const {
  // NOTE: This is far from ideal but it is what it is.
  if (buffer) {
    std::string buf (buffer);
    return {buf.rbegin ( ), buf.rend ( )};
  } else
    return "";
}
