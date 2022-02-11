#pragma once

#include <Kal/Action.hpp>
#include <Kal/ErrorOr.hpp>

namespace Kal::Action {

class IActionProvider {
  private:
  protected:
  [[nodiscard]] virtual ErrorOr<std::vector<Action>> generateProvideList ( )
    = 0;
  public:
  // FIXME: Could this return an error?
  //        Should we return a const& to the vector instead?
  [[nodiscard]] virtual ErrorOr<std::vector<Action>> provides ( ) const = 0;
};

}     // namespace Kal::Action
