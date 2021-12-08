#pragma once

#include <Kal/Concepts/Applicators.hpp>

#include <ranges>
#include <string_view>

namespace views {

namespace detail {
  struct mkStringClosure {
    const std::string_view delim;
    constexpr explicit mkStringClosure (std::string_view const& delim)
        : delim (delim){ };

    template<std::ranges::viewable_range R>
    constexpr auto operator( ) (R&& r) const {
      return ::mkString (std::forward<R> (r), delim);
    }
  };

  struct mkStringAdaptor {
    template<std::ranges::viewable_range R>
    constexpr auto operator( ) (R&& r, std::string_view const& delim) const {
      return ::mkString (std::forward<R> (r), delim);
    }

    constexpr auto operator( ) (std::string_view const& delim) const {
      return mkStringClosure (delim);
    }

    constexpr auto operator( ) ( ) const {
      using namespace std::literals;
      return mkStringClosure (" "sv);
    }
  };

  template<std::ranges::viewable_range R>
  constexpr auto operator| (R&& r, mkStringClosure const& a) {
    return a (std::forward<R> (r));
  }
}     // namespace detail
const detail::mkStringAdaptor mkString;
}     // namespace views
