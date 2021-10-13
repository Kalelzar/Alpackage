#include <Alpackage/Module/Module.hpp>
#include <Alpackage/Module/ModuleLoader.hpp>
#include <Alpackage/Util/Logging.hpp>

#include <boost/dll/import.hpp>

namespace Alpackage::Module {

using ModulePtr = boost::shared_ptr<IAlpackageModule>;

ModulePtr ModuleLoader::load (std::string const& moduleName) {
  return load (moduleName, "/usr/lib/alpackage/");
}

ModulePtr ModuleLoader::load (std::string const&          moduleName,
                              boost::dll::fs::path const& prefix) {
  Log::info ("Loading %s from '%s'", moduleName, prefix.string ( ));


  boost::shared_ptr<Alpackage::Module::IAlpackageModule> module
    = boost::dll::import_symbol<Alpackage::Module::IAlpackageModule> (

      prefix / moduleName,
      "module",
      boost::dll::load_mode::append_decorations

    );

  if (!module) {
    Log::error ("Failed to load module: %s", moduleName);
    throw std::runtime_error ("Failed to load module");
  }


  auto result = module->init ( );

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

ModulePtr ModuleLoader::getModule (std::string moduleName) {
  return load (moduleName);
}

ModulePtr ModuleLoader::getModule (std::string                 moduleName,
                                   boost::dll::fs::path const& prefix) {
  return load (moduleName, prefix);
}

}     // namespace Alpackage::Module
