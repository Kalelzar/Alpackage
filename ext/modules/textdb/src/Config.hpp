#pragma once

//#include "Logging.hpp"
#include <Alpackage/Package.hpp>
#include <Alpackage/Util/EntryReader.hpp>
#include <Alpackage/Util/Logging.hpp>

#include <cstring>
#include <fstream>
#include <istream>
#include <stdexcept>


static std::istream& operator>> (std::istream& in, Alpackage::Package& p) {
  if (in.eof ( )) {
    Log::error ("Unexpected end of file while reading a Package.");
    throw std::runtime_error (
      "Unexpected end of file while reading a Package.");
  }

  static const int nameSize    = 64;
  static const int pkgmngrSize = 16;
  static const int versionSize = 64;
  static const int descSize    = 256;

  char             name[nameSize];
  char             pkgmanager[pkgmngrSize];
  char             version[versionSize];
  char             description[descSize];

  static const int bufferSize = 512;

  char             buffer[bufferSize];
  in.getline (buffer, bufferSize);
  if (in.fail ( )) {
    Log::error ("(%s:%d) Failed to get line from buffer: %s",
                __FILE__,
                __LINE__);
    throw std::runtime_error ("Failed to get line from buffer.");
  }
  int read  = strlen (buffer);

  int point = 0;
  int mark  = 0;

  while (point < read && buffer[point] != ';') point++;
  strncpy (name, buffer + mark, point - mark);
  name[point - mark] = '\0';
  point++;
  mark = point;

  while (point < read && buffer[point] != ';') point++;
  strncpy (pkgmanager, buffer + mark, point - mark);
  pkgmanager[point - mark] = '\0';
  point++;
  mark = point;

  while (point < read && buffer[point] != ';') point++;
  strncpy (version, buffer + mark, point - mark);
  version[point - mark] = '\0';
  point++;
  mark = point;

  strncpy (description, buffer + mark, read - mark);
  description[read - mark] = '\0';


  p                        = Alpackage::Package (name, pkgmanager, true);

  return in;
}


std::vector<Alpackage::Package> readConfig (std::istream* in) {
  return EntryReader<Alpackage::Package>::parse (in);
}

// FIX: This fails if there is a newline after the last entry in a file
std::set<Alpackage::Package> readConfig (std::string configPath) {
  std::ifstream file (configPath);

  if (!file.is_open ( )) {
    Log::error ("(%s:%d) Failed to open file for read: %s",
                __FILE__,
                __LINE__,
                configPath);
    throw std::runtime_error ("Failed to open file for read");
  }
  auto res = readConfig (&file);

  file.close ( );

  return std::set<Alpackage::Package> (res.begin ( ), res.end ( ));
}
