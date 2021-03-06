#include <Alpackage/Module/Module.hpp>
#include <Alpackage/Module/ModuleLoader.hpp>
#include <Alpackage/Util/Logging.hpp>

#include <Kal/Format.hpp>

#include <boost/dll/import.hpp>
#include <boost/dll/shared_library.hpp>

namespace Alpackage::Module {

using ModulePtr = boost::shared_ptr<IAlpackageModule>;

ErrorOr<ModulePtr> ModuleLoader::load (std::string const& moduleName) {
  return TRY (load (moduleName, "/usr/lib/alpackage/"));
}

ErrorOr<ModulePtr> ModuleLoader::load (std::string const&          moduleName,
                                       boost::dll::fs::path const& prefix) {
  Log::info ("Loading %s from '%s'", moduleName, prefix.string ( ));


  try {
    reckless::scoped_indent    indent;
    boost::dll::shared_library lib (prefix / moduleName,
                                    boost::dll::load_mode::append_decorations);

    if (!lib.is_loaded ( )) {
      return format ("Failed to load module: %s", moduleName);
    }

    if (!lib.has ("module")) {
      return format (
        "Invalid module: %s. Module doesn't expose a 'module' object.",
        moduleName);
    }

    boost::shared_ptr<Alpackage::Module::IAlpackageModule> module
      = boost::dll::import_symbol<Alpackage::Module::IAlpackageModule> (
        lib,
        "module");
    if (!module) { return format ("Failed to import module: %s", moduleName); }
    Log::info ("Import: success");

    Log::flush ( );

    auto result = TRY_WITH (
      module->init ( ),
      format ("Could not load {} from {}.", moduleName, prefix.string ( )));

    if (!result) {
      return format ("Failed to initialize module: {}", moduleName);
    }

    Log::info ("Init: success");


    Log::debug ("Module %s v%s", module->name ( ), module->version ( ));

    auto actions = TRY (module->provides ( ));
    Log::info ("Actions: ");
    for (auto& action : actions) {
      reckless::scoped_indent indent;
      Log::info ("%s", action.toString ( ));
    }

    Log::info ("Successfully loaded: %s", moduleName);
    return module;

  } catch (boost::system::system_error& e) {
    return format ("Exception caught (boost::system::system_error) : {}",
                   e.what ( ));
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
