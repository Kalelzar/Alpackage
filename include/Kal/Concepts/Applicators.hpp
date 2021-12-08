#pragma once

#include <Kal/Concepts/Collection.hpp>
#include <Kal/Concepts/Functional.hpp>
#include <Kal/Concepts/Iterator.hpp>
#include <Kal/Concepts/Stream.hpp>

#include <Kal/default.hpp>

#include <sstream>

#include "detail/aggregate.hpp"
#include "detail/filter.hpp"
#include "detail/forall.hpp"
#include "detail/foreach.hpp"
#include "detail/lift.hpp"
#include "detail/map.hpp"
#include "detail/mkString.hpp"
#include "detail/unique.hpp"
#include "detail/zip.hpp"


template<typename T> void swap (T& in, T& out) {
  T temp = in;
  in     = out;
  out    = in;
}

template<typename T> T exchange (T& target, T& value) {
  T temp = target;
  target = value;
  return temp;
}

template<typename T> T exchange (T& target, T&& value) {
  T temp = target;
  target = std::forward (value);
  return temp;
}
