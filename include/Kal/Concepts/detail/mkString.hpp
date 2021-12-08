#pragma once
#include <Kal/Concepts/Iterator.hpp>
#include <Kal/Concepts/Stream.hpp>

#include <sstream>

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
  for (auto it = t.begin ( ); it != t.end ( ); ++it) {
    s << *it;
    if (std::next (it) != t.end ( )) s << delim;
  }
  s << after;
  return std::move (s.str ( ));
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
  auto it = t.cbegin ( );
  if (it != t.cend ( )) {
    s << *it;
    ++it;
  }
  for (; it != t.cend ( ); ++it) { s << delim << *it; }
  s << after;
  return std::move (s.str ( ));
}

template<AnyIterable T>
  requires OStreamable<typename IteratesWith<T>::type> &&(
    !OStreamable<T>) inline std::string mkString (T const& t) {
    return mkString (t, " ");
  }

  template<OStreamable C> std::string mkString (C const& t) {
    std::stringstream s;
    s << t;
    return s.str ( );
  }
