#include <Alpackage/Module/ModuleLoader.hpp>
#include <Alpackage/Package.hpp>
#include <Alpackage/Util/ConfLine.hpp>
#include <Alpackage/Util/EntryReader.hpp>
#include <Alpackage/Util/Logging.hpp>

#include <Kal/Concepts/Applicators.hpp>
#include <Kal/Concepts/TypeConversion.hpp>

#include <Kal/XDGBaseDir.hpp>

#include <boost/dll/import.hpp>
#include <iostream>
#include <stdexcept>


int main (int argc, char** argv) {
  try {
  } catch (std::runtime_error& e) {
    std::cerr << "Encountered unrecoverable error: " << e.what ( ) << std::endl
              << "See logs for more details" << std::endl;


    return 1;
  }
}
