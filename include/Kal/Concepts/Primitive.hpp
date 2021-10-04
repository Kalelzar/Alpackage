#pragma once

#include <Kal/Concepts/Type.hpp>


template<typename T> inline constexpr bool __IsIntegral                 = false;

template<> inline constexpr bool           __IsIntegral<bool>           = true;
template<> inline constexpr bool           __IsIntegral<unsigned char>  = true;
template<> inline constexpr bool           __IsIntegral<char8_t>        = true;
template<> inline constexpr bool           __IsIntegral<char16_t>       = true;
template<> inline constexpr bool           __IsIntegral<char32_t>       = true;
template<> inline constexpr bool           __IsIntegral<unsigned short> = true;
template<> inline constexpr bool           __IsIntegral<unsigned int>   = true;
template<> inline constexpr bool           __IsIntegral<unsigned long>  = true;
template<> inline constexpr bool __IsIntegral<unsigned long long>       = true;

template<typename T>
inline constexpr bool IsIntegral = __IsIntegral<MakeUnsigned<RemoveCV<T>>>;

template<typename T>
concept Integral = IsIntegral<T>;
