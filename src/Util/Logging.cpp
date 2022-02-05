#include <Alpackage/Util/Logging.hpp>

#include <Kal/XDGBaseDir.hpp>

const std::string Log::log_path
  = XDG_CACHE_HOME ( ).getOrElse (XDG_CACHE_HOME_DEFAULT) + "/Alpackage.log";
Log Log::l;
