#pragma once

#include <Kal/Concepts/Type.hpp>

#include <inttypes.h>


template<typename A, typename B = A>
concept Equal = requires (A a, B b) {
  { a == b } -> Same<bool>;
};

template<typename A, typename B>
concept MutuallyEqual = Equal<A, B> && Equal<B, A>;

template<typename A, typename B = A>
concept Greater = requires (A a, B b) {
  { a > b } -> Same<bool>;
};

template<typename A, typename B = A>
concept GreaterOrEqual = Greater<A, B> && Equal<A, B>;

template<typename A, typename B = A>
concept Lesser = requires (A a, B b) {
  { a > b } -> Same<bool>;
};

template<typename A, typename B = A>
concept LesserOrEqual = Lesser<A, B> && Equal<A, B>;

template<typename A, typename B = A>
concept TotallyComparable = LesserOrEqual<A, B> && GreaterOrEqual<A, B>;

template<typename A> constexpr A minBound;

template<typename A>
concept LowerBound = requires (A a) {
  { minBound<A> } -> Same<AddConst<A>>;
};


template<typename A> constexpr A maxBound;

template<typename A>
concept UpperBound = requires (A a) {
  { maxBound<A> } -> Same<AddConst<A>>;
};

template<typename A>
concept Bounded = LowerBound<A> && UpperBound<A>;


template<typename A, typename B>
concept Comparator = requires (A c, B a, B b) {
  { c.equal (a, b) } -> Same<bool>;
  { c.greater (a, b) } -> Same<bool>;
  { c.lesser (a, b) } -> Same<bool>;
  { c.greaterEqual (a, b) } -> Same<bool>;
  { c.lesserEqual (a, b) } -> Same<bool>;
  { c.compare (a, b) } -> Same<int8_t>;
};

template<typename T> class IComparator {

  protected:
  IComparator ( ) = default;

  public:
  virtual bool greater (T const& a, T const& b) const {
    return !lesserEqual (a, b);
  }

  virtual bool lesser (T const& a, T const& b) const {
    return !greaterEqual (a, b);
  }
  virtual bool equal (T const& a, T const& b) const {
    return !(lesser (a, b) || greater (a, b));
  }

  virtual bool greaterEqual (T const& a, T const& b) const {
    return greater (a, b) || equal (a, b);
  }
  virtual bool lesserEqual (T const& a, T const& b) const {
    return lesser (a, b) || equal (a, b);
  }

  int8_t compare (T const& a, T const& b) const {
    if (greater (a, b)) {
      return +1;
    } else if (lesser (a, b)) {
      return -1;
    } else {
      return 0;
    }
  }
};

template<TotallyComparable T> struct OperatorComparator {

  public:
  virtual bool greater (T const& a, T const& b) const { return a > b; }

  virtual bool lesser (T const& a, T const& b) const { return a < b; }
  virtual bool equal (T const& a, T const& b) const { return a == b; }

  virtual bool greaterEqual (T const& a, T const& b) const { return a >= b; }
  virtual bool lesserEqual (T const& a, T const& b) const { return a <= b; }

  int8_t       compare (T const& a, T const& b) const {
    if (greater (a, b)) {
      return +1;
    } else if (lesser (a, b)) {
      return -1;
    } else {
      return 0;
    }
  }
};
