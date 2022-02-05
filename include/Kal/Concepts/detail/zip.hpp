#pragma once

#include <Kal/Concepts/Collection.hpp>
#include <Kal/Concepts/Functional.hpp>
#include <Kal/Concepts/Iterator.hpp>

#include <Kal/default.hpp>

#include <utility>

template<typename L,
         typename R,
         template<typename>
         class Left,
         template<typename>
         class Right,
         template<typename>
         class Result>
  requires ConstIterableWith<Left<L>, L> && ConstIterableWith<
    Right<R>,
    R> && IterableWith<Result<std::pair<L, R>>,
                       std::pair<L, R>> && Appendable<Result, std::pair<L, R>>
    Result<std::pair<L, R>> zip (Left<L> const& l, Right<R> const& r) {
    auto ret = defaultValue<Result<std::pair<L, R>>>;

    if (l.size ( ) == 0 || r.size ( ) == 0) return ret;

    decltype (l.cbegin ( )) li;
    decltype (r.cbegin ( )) ri;
    for (li = l.cbegin ( ), ri = r.cbegin ( );
         li < l.cend ( ) && ri < r.cend ( );
         ++ri, ++li) {
      ret.push_back ({*li, *ri});
    };

    return ret;
  }
