#pragma once
#include <Kal/Concepts/Iterator.hpp>

template<typename T, IterableWithConst<T>... Iters> class MergeIterator;

template<typename T, IterableWithConst<T> Current>
class MergeIterator<T, Current> {
  public:
  using HeadType     = decltype (begin (Current ( )));
  using SentinelType = decltype (end (Current ( )));
  using EndType      = SentinelType;
  Current  current;
  HeadType head;
  EndType  endv;

  MergeIterator (Current& current)
      : current (current)
      , head (begin (current))
      , endv (end (current)){ };
  MergeIterator (MergeIterator const& other)
      : current (other.current)
      , head (other.head)
      , endv (other.endv) { }
  MergeIterator& operator= (MergeIterator const& other) {
    if (this != &other) {
      current = other.current;
      head    = other.head;
      endv    = other.endv;
    }
    return *this;
  };

  void           reset ( ) { head = begin (current); };
  bool           atEnd ( ) { return head == endv; }

  /// This currently silently fails when the end of the iterator is reached.
  /// What could be done about this? Except throwing an exception that is.
  MergeIterator& operator++ ( ) {
    if (head != endv) { ++head; }
    return *this;
  }

  T& operator* ( ) {
    if (head != endv) { return *head; }
    throw 0;     // HACK: FIXME: FIXME: DO NOT DO THIS IN ANY WAY, SHAPE OR
                 // FORM!!!
  }
};

template<typename T, IterableWithConst<T> Current, Iterable... Rest>
class MergeIterator<T, Current, Rest...> {
  public:
  using HeadType     = decltype (begin (Current ( )));
  using SentinelType = decltype (end (Current ( )));
  using SuperType    = MergeIterator<T, Rest...>;
  using EndType      = typename SuperType::EndType;
  Current      current;
  HeadType     head;
  SentinelType tail;
  SuperType    super;
  EndType      endv;

  MergeIterator (Current& current, Rest&... rest)
      : current (current)
      , head (begin (current))
      , tail (end (current))
      , super (rest...)
      , endv (super.endv){ };


  MergeIterator (MergeIterator const& other)
      : current (other.current)
      , head (begin (current))
      , tail (end (current))
      , super (other.super)
      , endv (super.endv) { }
  MergeIterator& operator= (MergeIterator const& other) {
    if (this != &other) {
      current = other.current;
      head    = begin (current);
      tail    = end (current);
      super   = other.super;
      endv    = super.endv;
    }
    return *this;
  };

  void           reset ( ) { head = begin (current); };

  bool           atEnd ( ) { return head == tail && super.atEnd ( ); }

  /// This currently silently fails when the end of the iterator is reached.
  /// What could be done about this? Except throwing an exception that is.
  MergeIterator& operator++ ( ) {
    if (head != tail) {
      ++head;
    } else if (!atEnd ( )) {
      ++super;
    }
    return *this;
  }

  T& operator* ( ) {
    if (head != tail) { return *head; }

    return *super;
  }
};
