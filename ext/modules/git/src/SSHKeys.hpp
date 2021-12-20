#pragma once

#include <Kal/Iterators/KeepIterator.hpp>
#include <Kal/Iterators/MergeIterator.hpp>

#include <cstdio>
#include <filesystem>


template<IterableWith<const std::filesystem::directory_entry>... T>
using SSHKeyMergeIterator
  = MergeIterator<const std::filesystem::directory_entry,
                  T...,
                  std::filesystem::directory_iterator,
                  std::filesystem::directory_iterator>;

template<IterableWith<const std::filesystem::directory_entry>... T>
using SSHKeyIterator = KeepIterator<SSHKeyMergeIterator<T...>,
                                    const std::filesystem::directory_entry>;


inline bool isPublicKey (std::filesystem::directory_entry const& dir) {
  return dir.path ( ).filename ( ).generic_string ( ).ends_with (".pub");
}

template<IterableWith<const std::filesystem::directory_entry>... Ts>
SSHKeyIterator<Ts...> makeSSHKeyIterator (Ts... dirs) {
  auto*                               home = getenv ("HOME");
  std::filesystem::directory_iterator userKeys (format ("{}/.ssh", home));
  std::filesystem::directory_iterator globalKeys ("/etc/ssh/");
  SSHKeyMergeIterator<Ts...>          mi (dirs..., userKeys, globalKeys);
  return SSHKeyIterator<Ts...> (mi, isPublicKey);
}
