#pragma once

#include <Alpackage/Package.hpp>
#include <Alpackage/Util/EntryReader.hpp>
#include <Alpackage/Util/Logging.hpp>

#include <Kal/ErrorOr.hpp>
#include <Kal/Format.hpp>

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
  size_t read  = strlen (buffer);

  int    point = 0;
  int    mark  = 0;

  // FIXME: Check that output buffers are large enough to hold what is being put
  // in them
  //       Better yet dynamically scale their size to fit their data.

  while (point < read && buffer[point] != ';') { point++; }
  strncpy (name, buffer + mark, point - mark);
  name[point - mark] = '\0';
  point++;
  mark = point;

  while (point < read && buffer[point] != ';') { point++; }
  strncpy (pkgmanager, buffer + mark, point - mark);
  pkgmanager[point - mark] = '\0';
  point++;
  mark = point;

  while (point < read && buffer[point] != ';') { point++; }
  strncpy (version, buffer + mark, point - mark);
  version[point - mark] = '\0';
  point++;
  mark = point;
  if (mark <= read) {
    strncpy (description, buffer + mark, read - mark);
    description[read - mark] = '\0';
  }

  p = Alpackage::Package (name, pkgmanager, true);

  return in;
}


ErrorOr<std::vector<Alpackage::Package>> readConfig (std::istream* in) {
  return EntryReader<Alpackage::Package>::parse (in);
}

// FIX: This fails if there is a newline after the last entry in a file
ErrorOr<std::set<Alpackage::Package>> readConfig (std::string configPath) {
  std::ifstream file (configPath);

  if (!file.is_open ( )) {
    return format ("Failed to open file for read: {}", std::move (configPath));
  }

  auto res = TRY (readConfig (&file));

  file.close ( );

  return std::set<Alpackage::Package> (res.begin ( ), res.end ( ));
}
