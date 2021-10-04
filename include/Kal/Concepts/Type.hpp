#pragma once


template<class T> struct __AddConst { using Type = const T; };
template<class T> struct __AddConst<const T> { using Type = const T; };
template<class T> struct __AddConst<const T&> { using Type = const T&; };
template<class T> using AddConst = typename __AddConst<T>::Type;

template<class T> struct __RemoveConst { using Type = T; };
template<class T> struct __RemoveConst<const T> { using Type = T; };
template<class T> struct __RemoveConst<const T&> { using Type = T&; };
template<class T> using RemoveConst = typename __RemoveConst<T>::Type;

template<class T> struct __RemoveRef { using Type = T; };
template<class T> struct __RemoveRef<T&> { using Type = T; };
template<class T> struct __RemoveRef<const T&> { using Type = const T; };
template<class T> using RemoveRef = typename __RemoveRef<T>::Type;

template<class T> struct __RemoveVolatile { using Type = T; };

template<class T> struct __RemoveVolatile<volatile T> { using Type = T; };

template<typename T> using RemoveVolatile = typename __RemoveVolatile<T>::Type;

template<class T> using RemoveCV          = RemoveVolatile<RemoveConst<T>>;

template<typename T> struct __MakeUnsigned { using Type = void; };
template<> struct __MakeUnsigned<signed char> { using Type = unsigned char; };
template<> struct __MakeUnsigned<short> { using Type = unsigned short; };
template<> struct __MakeUnsigned<int> { using Type = unsigned int; };
template<> struct __MakeUnsigned<long> { using Type = unsigned long; };
template<> struct __MakeUnsigned<long long> {
  using Type = unsigned long long;
};
template<> struct __MakeUnsigned<unsigned char> { using Type = unsigned char; };
template<> struct __MakeUnsigned<unsigned short> {
  using Type = unsigned short;
};
template<> struct __MakeUnsigned<unsigned int> { using Type = unsigned int; };
template<> struct __MakeUnsigned<unsigned long> { using Type = unsigned long; };
template<> struct __MakeUnsigned<unsigned long long> {
  using Type = unsigned long long;
};
template<> struct __MakeUnsigned<char> { using Type = unsigned char; };
template<> struct __MakeUnsigned<char8_t> { using Type = char8_t; };
template<> struct __MakeUnsigned<char16_t> { using Type = char16_t; };
template<> struct __MakeUnsigned<char32_t> { using Type = char32_t; };
template<> struct __MakeUnsigned<bool> { using Type = bool; };

template<typename T> using MakeUnsigned = typename __MakeUnsigned<T>::Type;

template<typename T> struct __MakeSigned { using Type = void; };
template<> struct __MakeSigned<signed char> { using Type = signed char; };
template<> struct __MakeSigned<short> { using Type = short; };
template<> struct __MakeSigned<int> { using Type = int; };
template<> struct __MakeSigned<long> { using Type = long; };
template<> struct __MakeSigned<long long> { using Type = long long; };
template<> struct __MakeSigned<unsigned char> { using Type = char; };
template<> struct __MakeSigned<unsigned short> { using Type = short; };
template<> struct __MakeSigned<unsigned int> { using Type = int; };
template<> struct __MakeSigned<unsigned long> { using Type = long; };
template<> struct __MakeSigned<unsigned long long> { using Type = long long; };
template<> struct __MakeSigned<char> { using Type = char; };

template<typename T> using MakeSigned = typename __MakeSigned<T>::Type;

template<typename T, typename U> inline constexpr bool IsSame       = false;
template<typename T> inline constexpr bool             IsSame<T, T> = true;

template<typename T, typename U>
concept Same = IsSame<T, U>;

template<typename T>
concept Const = IsSame<T, AddConst<T>>;

template<typename T>
concept NotConst = IsSame<RemoveConst<T>, T>;

template<typename T>
concept Nullable = requires (T t) {
  t = nullptr;
};

template<typename T>
concept NotNull = !Nullable<T>;

template<typename T, typename U>
concept Convertible = requires (T t) {
  { t.template to<U> ( ) } -> Same<U>;
};

template<typename T, typename U>
concept Interchangeable = Convertible<T, U> && Convertible<U, T>;
