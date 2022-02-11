#pragma once

#include <Kal/Concepts/Functional.hpp>

#include <Kal/ErrorOr.hpp>
#include <Kal/Format.hpp>

#include <filesystem>
#include <vector>


template<typename T>
ErrorOr<std::vector<T>> mapDirectoryFiles (
  std::string                                                    dirpath,
  Transformer<std::filesystem::directory_entry, ErrorOr<T>> auto mapper) {
  std::filesystem::path dir (dirpath);

  std::vector<T>        res;

  for (auto const& dir_entry : std::filesystem::directory_iterator{dir}) {
    if (!dir_entry.is_directory ( )) {
      auto mapped = TRY_WITH (
        mapper (dir_entry),
        format ("Failed to map {} ", dir_entry.path ( ).string ( )));
      res.push_back (mapped);
    }
  }

  return res;
}
