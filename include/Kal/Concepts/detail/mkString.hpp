#pragma once
#include <Kal/Concepts/Iterator.hpp>
#include <Kal/Concepts/Stream.hpp>

#include <sstream>

template<typename T>
concept ToStringable = requires (T t) {
  { t.toString ( ) } -> Same<std::string>;
};

template<Iterable    T,
         OStreamable Delim,
         OStreamable Before = std::string,
         OStreamable After  = std::string>
  requires (!ConstIterable<T>)
&&OStreamable<typename IteratesWith<T>::type> std::string
                                              mkString (T const&      t,
                                                        Delim const&  delim,
                                                        Before const& before = "[ ",
                                                        After const&  after = " ]") {
  std::stringstream s;
  s << before;
  for (auto& it = begin (t); it != end (t); ++it) {
    s << *it;
    if (std::next (it) != end (t)) s << delim;
  }
  s << after;
  return s.str ( );
}


template<ConstIterable T,
         OStreamable   Delim,
         OStreamable   Before = std::string,
         OStreamable   After  = std::string>
  requires OStreamable<typename IteratesWith<T>::type>
inline std::string mkString (T const&      t,
                             Delim const&  delim,
                             Before const& before = "[ ",
                             After const&  after  = " ]") {
  std::stringstream s;
  s << before;
  auto it = cbegin (t);
  if (it != cend (t)) {
    s << *it;
    ++it;
  }
  for (; it != cend (t); ++it) { s << delim << *it; }
  s << after;
  return s.str ( );
}

template<Iterable    T,
         OStreamable Delim,
         OStreamable Before = std::string,
         OStreamable After  = std::string>
  requires (!ConstIterable<T>)
&&ToStringable<typename IteratesWith<T>::type> std::string
                                               mkString (T const&      t,
                                                         Delim const&  delim,
                                                         Before const& before = "[ ",
                                                         After const&  after = " ]") {
  std::stringstream s;
  s << before;
  for (auto& it = begin (t); it != end (t); ++it) {
    s << (*it).toString ( );
    if (std::next (it) != end (t)) s << delim;
  }
  s << after;
  return s.str ( );
}


template<ConstIterable T,
         OStreamable   Delim,
         OStreamable   Before = std::string,
         OStreamable   After  = std::string>
  requires ToStringable<typename IteratesWith<T>::type>
inline std::string mkString (T const&      t,
                             Delim const&  delim,
                             Before const& before = "[ ",
                             After const&  after  = " ]") {
  std::stringstream s;
  s << before;
  auto it = cbegin (t);
  if (it != cend (t)) {
    s << (*it).toString ( );
    ++it;
  }
  for (; it != cend (t); ++it) { s << delim << (*it).toString ( ); }
  s << after;
  return s.str ( );
}

template<AnyIterable T>
  requires OStreamable<typename IteratesWith<T>::type> &&(
    !OStreamable<T>) inline std::string mkString (T const& t) {
    return mkString (t, " ");
  }

  template<ToStringable T> inline std::string mkString (T const& t) {
    return t.toString ( );
  }

  template<AnyIterable T>
    requires ToStringable<typename IteratesWith<T>::type>
  inline std::string mkString (T const& t) { return mkString (t, " "); }

  template<OStreamable C> std::string mkString (C const& t) {
    std::stringstream s;
    s << t;
    return s.str ( );
  }
