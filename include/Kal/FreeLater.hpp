#pragma once

#include <cstdlib>


/**
 * A simple utility class that calls free() on the provided pointer after it
 * goes out of scope.
 */
class FreeLater {
  private:
  void** ptr;
  public:
  explicit FreeLater (void** ptr) : ptr (ptr) { }
  ~FreeLater ( ) {
    if (*ptr) {
      free (*ptr);
      *ptr = nullptr;
    }
  }

  FreeLater (FreeLater const&) = delete;
  FreeLater& operator= (FreeLater const&) = delete;

  FreeLater (FreeLater&& other) noexcept {
    if (this != &other) {
      ptr       = other.ptr;
      other.ptr = nullptr;
    }
  }
  FreeLater& operator= (FreeLater&& other) noexcept {
    if (this != &other) {
      ptr       = other.ptr;
      other.ptr = nullptr;
    }
    return *this;
  }


  /**
   *   Releases the held pointer, so that it won't be freed.
   */
  void release ( ) { ptr = nullptr; }
};
