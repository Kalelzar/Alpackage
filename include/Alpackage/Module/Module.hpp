#pragma once

#include <Alpackage/Package.hpp>
#include <Alpackage/Util/Logging.hpp>

#include <Kal/Concepts/TypeConversion.hpp>

#include <Kal/Option.hpp>
#include <Kal/default.hpp>

#include <boost/config.hpp>
#include <sstream>
#include <stdexcept>
#include <string>

namespace Alpackage::Module {

enum class ModuleError {
  NONE = -1,
  UNIMPLEMENTED,
  UNSUPPORTED,
};

template<typename T> class ModuleErrorOr {
  private:
  const T           value;
  const ModuleError error = ModuleError::NONE;

  public:
  ModuleErrorOr (const char* str) {
    std::ostringstream ss;
    ss << str;
    ModuleErrorOr (ss.str ( ));
  }

  template<typename U> U to ( ) const requires Same<T, U> { return value; }

  template<typename U>
    requires CanConvertTo<T, U> &&(!Same<T, U>) U to ( ) const {
      return value.template to<U> ( );
    }

    template<typename U>
      requires ConstructibleFrom<U, T> &&(!Same<T, U>) U to ( ) const {
        return U (value);
      }

      constexpr ModuleErrorOr (T const& t) : value (t) { }
      constexpr ModuleErrorOr (T&& t) : value (std::move (t)) { }
      constexpr ModuleErrorOr (ModuleError e)
          : error (e)
          , value (defaultValue<T>) { }

      const T getValue ( ) const {
        if (isError ( )) {
          Log::error ("Accessing an empty value in ModuleErrorOr");
          throw std::runtime_error (
            "Accessing an empty value in ModuleErrorOr");
        }
        return value;
      }

      const Option<T> get ( ) const {
        if (isError ( )) {
          Log::warn ("Accessing an empty value in ModuleErrorOr");
          return { };
        }
        return value;
      }
      const ModuleError getError ( ) const { return error; }
      bool              isError ( ) const { return error != ModuleError::NONE; }
};

class BOOST_SYMBOL_VISIBLE IAlpackageModule {

  public:
  virtual constexpr bool                   canSearch ( ) const  = 0;
  virtual constexpr bool                   canFind ( ) const    = 0;
  virtual constexpr bool                   canInstall ( ) const = 0;
  virtual constexpr bool                   canList ( ) const    = 0;

  virtual constexpr const char*            version ( ) const    = 0;
  virtual constexpr const char*            name ( ) const       = 0;

  virtual ModuleError                      init ( )             = 0;

  virtual ModuleErrorOr<std::set<Package>> installed ( ) const  = 0;
  virtual ModuleErrorOr<std::set<Package>>
                                 search (std::string const& query) const = 0;
  virtual ModuleErrorOr<Package> find (std::string const& pkgName) const = 0;
  virtual ModuleError            install (std::string const& pkgName)    = 0;

  virtual ModuleError            install (Package const& pkg) {
    return install (pkg.name);
  }


  virtual ~IAlpackageModule ( ){ };
};
}     // namespace Alpackage::Module
