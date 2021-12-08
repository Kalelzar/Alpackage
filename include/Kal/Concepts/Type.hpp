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

struct FalseType {
  inline constexpr      operator bool ( ) { return false; }
  constexpr static bool value = false;
};
struct TrueType {
  inline constexpr      operator bool ( ) { return true; }
  constexpr static bool value = true;
};

template<typename T> struct __IsPointer : FalseType { };
template<typename T> struct __IsPointer<T*> : TrueType { };

template<typename T> struct _IsPointer : __IsPointer<RemoveCV<T>> { };

template<class T> inline constexpr bool IsPointer = _IsPointer<T>::value;


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
inline constexpr bool IsSameCVR
  = IsSame<RemoveRef<RemoveCV<T>>, RemoveRef<RemoveCV<U>>>;
template<typename T> inline constexpr bool IsSameCVR<T, T> = true;

/// @defgroup TypeConcepts Type Concepts

/** @defgroup Same Same
 *  @ingroup TypeConcepts
 *
 * @brief Encodes the concept of type equality.
 *
 * @tparam T left
 * @tparam U right
 *
 * Only true when \c T and \c U are the \e EXACT same type
 *
 */
template<typename T, typename U>
concept Same = IsSame<T, U>;

/** @defgroup SameCVR SameCVR
 *  @ingroup TypeConcepts
 *
 * @brief Encodes the concept of type equality up to a const, volatile or
 * reference.
 * @see Same
 *
 * @tparam T left
 * @tparam U right
 *
 * Only true when \c T and \c U are the \e EXACT same type
 *
 */
template<typename T, typename U>
concept SameCVR = IsSameCVR<T, U>;

/** @defgroup Const Const
 *  @ingroup TypeConcepts
 *
 * @brief Encodes the concept of a constant type.
 *
 * @tparam T type
 *
 * @requires{T, Same} : \c T is the same as @ref AddConst <T>
 *
 * Only true when \c T is a constant type
 *
 * @see NotConst
 */
template<typename T>
concept Const = Same<T, AddConst<T>>;

/** @defgroup NotConst Not Const
 *  @ingroup TypeConcepts
 *
 * @brief Encodes the concept of a mutable type.
 *
 * @tparam T type
 *
 * @requires{T, Same} : \c T is the same as @ref RemoveConst <T>
 *
 * Only true when \c T is a non-const type.
 *
 * @see Const
 *
 */
template<typename T>
concept NotConst = Same<RemoveConst<T>, T>;

/** @defgroup Pointer Pointer
 *  @ingroup TypeConcepts
 *
 * @brief Encodes the concept of a pointer type.
 *
 * @tparam T type
 *
 * Only true when \c T is a pointer type.
 *
 */
template<typename T>
concept Pointer = IsPointer<T>;

/** @defgroup Nullable Nullable
 *  @ingroup TypeConcepts
 *
 * @brief Encodes the concept of a nullable type.
 *
 * @tparam T type
 *
 * Only true when \c T is such that \code{T t = nullptr} is valid.
 *
 */
template<typename T>
concept Nullable = requires (T t) {
  t = nullptr;
};

/** @defgroup NotNull Not Null
 *  @ingroup TypeConcepts
 *
 * @brief Encodes the concept of a non-nullable type.
 *
 * @tparam T type
 *
 * @requires<T, Nullable>
 *
 * Only true when @ref Nullable <T> is false
 *
 */
template<typename T>
concept NotNull = !Nullable<T>;
