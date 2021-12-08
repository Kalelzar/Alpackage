#include <Alpackage/Module/Module.hpp>
#include <Alpackage/Module/ModuleLoader.hpp>
#include <Alpackage/Util/Logging.hpp>

#include <Kal/Format.hpp>

#include <boost/dll/import.hpp>

namespace Alpackage::Module {

using ModulePtr = boost::shared_ptr<IAlpackageModule>;

ErrorOr<ModulePtr> ModuleLoader::load (std::string const& moduleName) {
  return TRY (load (moduleName, "/usr/lib/alpackage/"));
}

ErrorOr<ModulePtr> ModuleLoader::load (std::string const&          moduleName,
                                       boost::dll::fs::path const& prefix) {
  Log::info ("Loading %s from '%s'", moduleName, prefix.string ( ));

  boost::shared_ptr<Alpackage::Module::IAlpackageModule> module;

  try {
    module = boost::dll::import_symbol<Alpackage::Module::IAlpackageModule> (

      prefix / moduleName,
      "module",
      boost::dll::load_mode::append_decorations);
  } catch (boost::system::system_error& e) {
    return format ("Exception caught (boost::system::system_error) : {}",
                   e.what ( ));
  }

  if (!module) { return format ("Failed to load module: %s", moduleName); }


  auto result = TRY_WITH (
    module->init ( ),
    format ("Could not load {} from {}.", moduleName, prefix.string ( )));

  if (result != ModuleError::NONE) {
    Log::error ("Failed to initialize module: %s", moduleName);
    throw std::runtime_error ("Failed to initialize module.");
  }

  Log::info ("Successfully initialized: %s", moduleName);

  Log::debug ("Module %s v%s", module->name ( ), module->version ( ));
  {
    reckless::scoped_indent indent;
    Log::debug ("Install: %s", module->canInstall ( ) ? "yes" : "no");
    Log::debug ("Search: %s", module->canSearch ( ) ? "yes" : "no");
    Log::debug ("Find: %s", module->canFind ( ) ? "yes" : "no");
    Log::debug ("List: %s", module->canList ( ) ? "yes" : "no");

    return module;
  }
}

ErrorOr<ModulePtr> ModuleLoader::getModule (std::string moduleName) {
  return TRY_WITH (load (moduleName),
                   format ("Loading '{}' failed.", moduleName));
}

ErrorOr<ModulePtr>
  ModuleLoader::getModule (std::string                 moduleName,
                           boost::dll::fs::path const& prefix) {
  return TRY (load (moduleName, prefix));
}

}     // namespace Alpackage::Module
