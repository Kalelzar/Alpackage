#pragma once
#include <Kal/Concepts/Iterator.hpp>

#include <functional>

template<typename T, typename U>
  requires requires (T t) {
    ++t;
    { *t } -> SameCVR<U>;
    { t.atEnd ( ) } -> Same<bool>;
    t.reset ( );
  }
class KeepIterator {
  public:
  std::function<bool (U&)> f;
  T                        iter;

  KeepIterator (T& iter, std::function<bool (U&)> f) : iter (iter), f (f) {
    if (!iter.atEnd ( ) && !f (*iter)) ++(*this);
  }

  KeepIterator (KeepIterator const& other) : f (other.f), iter (other.iter) { }
  KeepIterator& operator= (KeepIterator const& other) {
    if (this != &other) {
      f    = other.f;
      iter = other.iter;
    }
    return *this;
  };

  U&            operator* ( ) { return *iter; }

  KeepIterator& operator++ ( ) {
    do { ++iter; } while (!iter.atEnd ( ) && !f (*iter));
    return *this;
  }

  void reset ( ) { iter.reset ( ); }

  bool atEnd ( ) { return iter.atEnd ( ); }
};
