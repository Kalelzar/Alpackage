#include <Kal/Action.hpp>

#include <cstdint>

namespace Kal::Action {


ActionFactory ActionFactory::instance;

std::string   ActionArgument::toString ( ) const {
  bool isList = (type & ALIST) != 0U;

  switch ((uint8_t) (type | ALIST) ^ ALIST) {
    case ASTRING:
      return format ("{}: {}", name, isList ? "[String]" : "String");
    case ABOOLEAN:
      return format ("{}: {}", name, isList ? "[Boolean]" : "Boolean");
    case AINT: return format ("{}: {}", name, isList ? "[Int]" : "Int");
    case ADOUBLE:
      return format ("{}: {}", name, isList ? "[Double]" : "Double");
    default: return format ("{}: <{}>", name, (uint16_t) type);
  }
}

std::string Action::toString ( ) const {
  return format ("[{}] {}::{}({})",
                 id,
                 provider,
                 name,
                 mkString (args, ", ", "", ""));
}

}     // namespace Kal::Action
