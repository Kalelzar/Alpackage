#pragma once

#include <Kal/Data/RadixTree.hpp>
#include <Kal/ErrorOr.hpp>

#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

namespace Kal::Action {


constexpr uint8_t ALIST    = 0b1000000U;
constexpr uint8_t ASTRING  = 0b0000001U;
constexpr uint8_t AINT     = 0b0000010U;
constexpr uint8_t ADOUBLE  = 0b0000100U;
constexpr uint8_t ABOOLEAN = 0b0001000U;
constexpr uint8_t AUNKNOWN = 0b0010000U;


struct ActionArgument {
  const std::string name;
  const uint8_t     type;
  ActionArgument (std::string const& name, const uint8_t type)
      : name (name)
      , type (type) { }
  ActionArgument (std::string&& name, const uint8_t type)
      : name (std::forward<std::string> (name))
      , type (type) { }

  std::string toString ( ) const;
};

struct Action {
  std::string                 name;
  std::string                 provider;
  std::vector<ActionArgument> args;
  uint32_t                    id;
  [[nodiscard]] std::string   toString ( ) const;
};

class ActionFactory {
  private:
  static ActionFactory           instance;
  uint32_t                       idCounter{ };
  Kal::Data::RadixTree<uint32_t> storage;
  public:
  static inline ActionFactory& the ( ) { return instance; }

  ErrorOr<uint32_t>            reserve (std::string const& name) {
    auto res = storage.lookup (name);
    if (res.isDefined ( )) { return res.get ( ); }
    TRY_WITH (storage.add (name, idCounter++),
              format ("Failed to reserve an ID for action '{}'", name));
    return idCounter;
  }
  inline ErrorOr<Action> make (std::string&                 name,
                               std::string&                 provider,
                               std::vector<ActionArgument>& args) {
    return make (std::move (name), std::move (provider), std::move (args));
  }

  ErrorOr<Action> make (std::string&&                 name,
                        std::string&&                 provider,
                        std::vector<ActionArgument>&& args) {
    auto id = TRY (reserve (name));
    return Action{std::forward<std::string> (name),
                  std::forward<std::string> (provider),
                  std::forward<std::vector<ActionArgument>> (args),
                  id};
  }
};

}     // namespace Kal::Action
