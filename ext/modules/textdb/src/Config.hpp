#pragma once

#include <Alpackage/Package.hpp>
#include <Alpackage/Util/EntryReader.hpp>

#include <Kal/ErrorOr.hpp>
#include <Kal/Format.hpp>

#include <fstream>
#include <istream>


ErrorOr<std::vector<Alpackage::Package>> readConfig (std::istream* in) {
  return std::move (EntryReader<Alpackage::Package>::parse (in));
}

// FIX: This fails if there is a newline after the last entry in a file
ErrorOr<std::set<Alpackage::Package>> readConfig (std::string configPath) {
  std::ifstream file (configPath);

  if (!file.is_open ( )) {
    return format ("Failed to open file for read: {}", std::move (configPath));
  }

  auto res
    = TRY_WITH (readConfig (&file),
                format ("Failed to read TextDB config from '{}'", configPath));

  file.close ( );

  return std::set<Alpackage::Package> (res.begin ( ), res.end ( ));
}
