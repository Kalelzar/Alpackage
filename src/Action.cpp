#include <Kal/Action.hpp>

#include <cstdint>

namespace Kal::Action {

ErrorOr<uint32_t> ActionFactory::reserve (std::string const& name) {
  auto res = storage.has (name.c_str ( ));
  if (res.isDefined ( )) { return res.get ( ); }

  return TRY_WITH (storage.add (idCounter++, name.c_str ( )),
                   format ("Failed to reserve an ID for action '{}'", name));
}

ErrorOr<Action> ActionFactory::make (std::string const& name,
                                     std::string const& provider,
                                     std::vector<ActionArgument> const& args) {
  auto id = TRY (reserve (name));
  return Action{name, provider, args, id};
}

uint32_t      ActionFactory::idCounter = 0;
ActionFactory ActionFactory::instance;

}     // namespace Kal::Action
