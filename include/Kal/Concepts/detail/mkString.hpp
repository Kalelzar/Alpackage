#pragma once
#include <Kal/Concepts/Iterator.hpp>
#include <Kal/Concepts/Stream.hpp>

#include <sstream>

template<Iterable T>
  requires (!ConstIterable<T>)
&&OStreamable<typename IteratesWith<T>::type> std::string
  mkString (T t, std::string delim) {
  std::stringstream s;
  s << "[ ";
  for (auto it = t.begin ( ); it != t.end ( ); ++it) {
    s << *it;
    if (std::next (it) != t.end ( )) s << delim;
  }
  s << " ]";
  return s.str ( );
}


template<ConstIterable T>
  requires OStreamable<typename IteratesWith<T>::type> std::string
    mkString (T t, std::string delim) {
    std::stringstream s;
    s << "[ ";
    for (auto it = t.cbegin ( ); it != t.cend ( ); ++it) {
      s << *it;
      if (std::next (it) != t.cend ( )) s << delim;
    }
    s << " ]";
    return s.str ( );
  }

template<AnyIterable T>
  requires OStreamable<typename IteratesWith<T>::type> std::string
                                                       mkString (T t) {
    return mkString (t, " ");
  }

template<OStreamable C> std::string mkString (C t) {
  std::stringstream s;
  s << t;
  return s.str ( );
}
