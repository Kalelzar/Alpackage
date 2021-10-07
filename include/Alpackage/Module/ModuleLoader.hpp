#pragma once

#include <Alpackage/Module/Module.hpp>

#include <boost/dll/import.hpp>

namespace Alpackage::Module {

class ModuleLoader {
  private:
  using ModulePtr = boost::shared_ptr<IAlpackageModule>;

  static ModulePtr load (std::string const& moduleName);

  static ModulePtr load (std::string const&          moduleName,
                         boost::dll::fs::path const& prefix);
  public:
  static ModulePtr getModule (std::string moduleName);
  static ModulePtr getModule (std::string                 moduleName,
                              boost::dll::fs::path const& prefix);
};


}     // namespace Alpackage::Module
