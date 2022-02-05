#pragma once

#include "Kal/Data/LookUpTree.hpp"
#include "Kal/Data/PrefixTree.hpp"
#include "Kal/ErrorOr.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace Kal::Action {


enum ActionArgumentType {
  LIST    = 0b1000000,
  STRING  = 0b0000001,
  INT     = 0b0000010,
  DOUBLE  = 0b0000100,
  BOOLEAN = 0b0001000,
  UNKNOWN = 0b0010000,
};

struct ActionArgument {
  std::string        name;
  ActionArgumentType type;
};

struct Action {
  std::string                 name;
  std::string                 provider;
  std::vector<ActionArgument> args;
  uint32_t                    id;
};

class ActionFactory {
  private:
  static ActionFactory            instance;
  static uint32_t                 idCounter;
  Kal::Data::PrefixTree<uint32_t> storage;
  public:
  static inline ActionFactory the ( ) { return instance; }

  inline ErrorOr<uint32_t>    reserve (std::string const&);
  inline ErrorOr<Action>      make (std::string const&                 name,
                                    std::string const&                 provider,
                                    std::vector<ActionArgument> const& args);
};

}     // namespace Kal::Action
