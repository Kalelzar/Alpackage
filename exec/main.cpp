#include <Alpackage/Module/ModuleLoader.hpp>
#include <Alpackage/Package.hpp>
#include <Alpackage/Util/ConfLine.hpp>
#include <Alpackage/Util/EntryReader.hpp>
#include <Alpackage/Util/Logging.hpp>

#include <Kal/Concepts/Applicators.hpp>

#include <Kal/Flag.hpp>
#include <Kal/XDGBaseDir.hpp>

#include <boost/dll/import.hpp>
#include <iostream>
#include <stdexcept>

#define KAL_UNHANDLED_ERROR 255

ErrorOr<int> kalmain (int argc, char** argv) { return 0; }

int          main (int argc, char** argv) {
  auto t = kalmain (argc, argv);
  if (t.isEmpty ( )) {
    std::cerr << t << std::endl;
    return KAL_UNHANDLED_ERROR;
  }

  return t.get ( );
}
