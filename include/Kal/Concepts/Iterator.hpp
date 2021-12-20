#pragma once

#include <Kal/Concepts/Type.hpp>

#include <iterator>

///@defgroup IteratorConcepts Iterator Concepts

/** @defgroup IteratorPair Iterator Pair
 * @ingroup IteratorConcepts
 *
 * @tparam Begin the type of the beginning iterator
 * @tparam End the type of the end iterator
 *
 * \par Syntactic Requirements:
 *   \c Begin::operator*() \n
 *   \c operator++(Begin&) \n
 *   \c Begin::operator!=(End)
 *
 * @requires{Same}
 * \li
 * Sanity check. Makes sure that \c Begin::operator!=(End)
 * returns \c bool
 */
template<typename Begin, typename End>
concept IteratorPair = requires (Begin begin, End end) {
  *begin;
  { begin != end } -> Same<bool>;
  ++begin;
};

/** @defgroup Iterable Iterable
 * @ingroup IteratorConcepts
 *
 * @brief An \c Iterable is any object that implements an @ref IteratorPair via
 * its \c begin() and \c end() methods
 *
 *
 *
 * @requires{T::begin(), IteratorPair}
 * \li The return type of \c T::begin() is an IteratorPair with \c T::end()
 *
 *
 */
template<typename T>
concept Iterable = requires (T t) {
  { begin (t) } -> IteratorPair<decltype (end (t))>;
};

/** @defgroup ConstIterable Const Iterable
 * @ingroup IteratorConcepts
 *
 * @brief A \c ConstIterable is any object that implements an @ref IteratorPair
 * via its \c cbegin() and \c cend() methods
 *
 *
 *
 * @requires{T::cbegin(), IteratorPair}
 * \li The return type of \c T::cbegin() is an IteratorPair with \c T::cend()
 *
 *
 */
template<typename T>
concept ConstIterable = requires (T t) {
  { cbegin (t) } -> IteratorPair<decltype (cend (t))>;
};

/** @defgroup AnyIterable Any Iterable
 * @ingroup IteratorConcepts
 *
 * @brief An \c AnyIterable is any object that is either an @ref Iterable or a
 * @ref ConstIterable
 * @par Either one of:
 * \li @requires{T, Iterable}
 * \li @requires{T, ConstIterable}
 *
 */
template<typename T>
concept AnyIterable = ConstIterable<T> || Iterable<T>;

template<typename T> struct IteratesWith;

template<typename T>
  requires Iterable<T> && ConstIterable<T>
struct IteratesWith<T> {
  private:
  T t;
  public:
  using type = typename std::iterator_traits<decltype (begin (t))>::value_type;
  using ctype =
    typename std::iterator_traits<decltype (cbegin (t))>::value_type;
};

template<Iterable T> struct IteratesWith<T> {
  private:
  T t;
  public:
  using type = typename std::iterator_traits<decltype (begin (t))>::value_type;
};


template<ConstIterable T> struct IteratesWith<T> {
  private:
  T t;
  public:
  using ctype =
    typename std::iterator_traits<decltype (cbegin (t))>::value_type;
};

/** @defgroup IterableWith IterableWith
 * @ingroup IteratorConcepts
 *
 * @tparam T the iterable
 * @tparam U the value held by the iterator
 *
 * @brief An \c IterableWith is any object that implements an @ref Iterable
 * whose iterator holds a value of type \c U
 *
 * @requires{T, Iterable}
 * @requires{*T::begin(), Same}
 *  \li It must have the @ref Same type as \c U&
 *
 *
 *
 */
template<typename T, typename U>
concept IterableWith = Iterable<T> && Same<typename IteratesWith<T>::type, U>;

template<typename T, typename U>
concept IterableWithConst
  = Iterable<T> && SameCVR<typename IteratesWith<T>::type, U>;

template<typename T, typename U>
concept ConstIterableWith
  = ConstIterable<T> && SameCVR<typename IteratesWith<T>::type, U>;

template<typename T, typename U>
concept AnyIterableWith
  = ConstIterableWith<T, U> || IterableWithConst<T, U> || IterableWith<T, U>;
