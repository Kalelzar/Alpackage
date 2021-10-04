#include "Alpackage/Package.hpp"
#include <Alpackage/Module/ModuleLoader.hpp>
#include <Alpackage/Util/Logging.hpp>
#include <Kal/Concepts/Applicators.hpp>
#include <Kal/XDGBaseDir.hpp>
#include <boost/dll/import.hpp>
#include <iostream>
#include <stdexcept>

int main (int argc, char** argv) {
  try {
#ifdef NDEBUG
    boost::dll::fs::path path = "/home/kalelzar/Code/cpp/Alpackage/build/ninja/"
                                "ext/modules/textdb/Release";
#else
    boost::dll::fs::path path = "/home/kalelzar/Code/cpp/Alpackage/build/ninja/"
                                "ext/modules/textdb/Debug";
#endif

    auto module
      = Alpackage::Module::ModuleLoader::getModule ("AlpackageTextDB", path);

    auto pkgs = module->installed ( );

    //    std::cout << mkString (pkgs.get ( ), ",\n") << std::endl;
    std::cout << "XDG_CACHE_HOME: " << XDG_CACHE_HOME ( ) << std::endl;
    std::cout << "XDG_DATA_HOME: " << XDG_DATA_HOME ( ) << std::endl;
    std::cout << "XDG_CONFIG_HOME: " << XDG_CONFIG_HOME ( ) << std::endl;
    std::cout << "XDG_STATE_HOME: " << XDG_STATE_HOME ( ) << std::endl;
    std::cout << "XDG_EXEC_HOME: " << XDG_EXEC_HOME ( ) << std::endl;
    std::cout << "XDG_RUNTIME_DIR: " << XDG_RUNTIME_DIR ( ) << std::endl;
    std::cout << "XDG_DATA_DIRS: " << mkString (XDG_DATA_DIRS ( ), " : ")
              << std::endl;
    std::cout << "XDG_CONFIG_DIRS: " << mkString (XDG_CONFIG_DIRS ( ), " : ")
              << std::endl;
    std::cout << "autostart?: " << findFirstFile (XDG_CONFIG_DIRS ( ), "")
              << std::endl;

  } catch (std::runtime_error& e) {
    std::cerr << "Encountered unrecoverable error: " << e.what ( ) << std::endl
              << "See logs for more details" << std::endl;
    return 1;
  }
}
