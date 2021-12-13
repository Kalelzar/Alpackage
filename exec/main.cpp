#include <Alpackage/Module/ModuleLoader.hpp>
#include <Alpackage/Package.hpp>
#include <Alpackage/Util/ConfLine.hpp>
#include <Alpackage/Util/EntryReader.hpp>
#include <Alpackage/Util/Logging.hpp>

#include <Kal/Concepts/Applicators.hpp>

#include <Kal/Flag.hpp>
#include <Kal/View/detail/mkString.hpp>
#include <Kal/XDGBaseDir.hpp>

#include <boost/dll/import.hpp>
#include <iostream>
#include <ranges>
#include <stdexcept>

#define KAL_UNHANDLED_ERROR 255

ErrorOr<int> kalmain (int argc, char** argv) {
  auto textdb
    = TRY (Alpackage::Module::ModuleLoader::getModule ("AlpackageTextDB"));
  auto git = TRY (Alpackage::Module::ModuleLoader::getModule ("AlpackageGit"));

  std::cout << "Checking git packages status:" << std::endl
            << mkString (TRY (git->hasUpdates ( )), '\n');

  return 0;
}

int main (int argc, char** argv) {
  auto t = kalmain (argc, argv);


  if (t.isEmpty ( )) {
    Log::error ("%s", mkString (t));
    return KAL_UNHANDLED_ERROR;
  }

  return t.get ( );
}

#undef KAL_UNHANDLED_ERROR
