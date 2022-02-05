#pragma once

#include <Kal/Action.hpp>
#include <Kal/ErrorOr.hpp>

namespace Kal::Action {

class IActionConsumer {

  protected:
  [[nodiscard]] virtual ErrorOr<void> dispatch (Action const&) = 0;
  public:
  [[nodiscard]] ErrorOr<void> consume (Action const& c) {
    return dispatch (c);
  };
};
}     // namespace Kal::Action
