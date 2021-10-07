#pragma once

#include <Kal/Concepts/Type.hpp>

template<typename T, typename U = T>
concept Equal = requires (T t, U u) {
  { t == u } -> Same<bool>;
};
