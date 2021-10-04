#pragma once

#include <Kal/Concepts/Type.hpp>
#include <iostream>

template<typename T>
concept OStreamable = requires (T t) {
  { std::cout << t } -> Same<decltype (std::cout)&>;
};

template<typename T>
concept IStreamable = requires (T t) {
  { std::cin >> t } -> Same<decltype (std::cin)&>;
};

template<typename T>
concept IOStreamable = IStreamable<T> && OStreamable<T>;
