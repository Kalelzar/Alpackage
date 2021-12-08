#pragma once


#include <Alpackage/Util/Logging.hpp>

#include <Kal/Concepts/Type.hpp>

#include <Kal/ErrorOr.hpp>
#include <Kal/Format.hpp>

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace Kal {

using FlagType                        = uint32_t;
inline constexpr FlagType List        = 0b10000000U;
inline constexpr FlagType None        = 0b00000000U;
inline constexpr FlagType Int         = 0b00000001U;
inline constexpr FlagType Double      = 0b00000010U;
inline constexpr FlagType Bool        = 0b00000100U;
inline constexpr FlagType String      = 0b00001000U;
inline constexpr FlagType Unsupported = 0b11111111U;

template<typename T> struct FlagTypeV {
  constexpr static FlagType value = Unsupported;
};

template<> struct FlagTypeV<bool> { constexpr static FlagType value = Bool; };

template<> struct FlagTypeV<int> { constexpr static FlagType value = Int; };

template<> struct FlagTypeV<double> {
  constexpr static FlagType value = Double;
};

template<> struct FlagTypeV<void> { constexpr static FlagType value = None; };

template<> struct FlagTypeV<char*> {
  constexpr static FlagType value = String;
};

struct FlagEntry {
  FlagType    ftype;
  void*       flagv;
  std::string name;
  std::string usage;
  bool        required = false;
};

inline constexpr uint64_t MOVED_POINTER = 0xbadc0de;

template<typename T> class FlagRef {
  private:
  T*         ptr;
  FlagEntry* flagEntry;

  public:
  FlagRef ( ) : ptr (nullptr), flagEntry (nullptr) { }

  FlagRef (T* ptr, FlagEntry* flagEntry) : ptr (ptr), flagEntry (flagEntry) { }

  FlagRef (const FlagRef<T>&) = default;
  FlagRef& operator= (const FlagRef<T>&) = default;

  FlagRef (FlagRef<T>&& other) noexcept
      : ptr (other.ptr)
      , flagEntry (other.flagEntry) {
    other.ptr       = (T*) MOVED_POINTER;
    other.flagEntry = (FlagEntry*) MOVED_POINTER;
  }

  FlagRef& operator= (FlagRef<T>&& other) noexcept {
    if (this != &other) {
      ptr             = other.ptr;
      flagEntry       = other.flagEntry;
      other.ptr       = (T*) MOVED_POINTER;
      other.flagEntry = (FlagEntry*) MOVED_POINTER;
    }
    return *this;
  }

  ~FlagRef ( ) = default;

  friend void require (FlagRef<T>& ref) { ref.flagEntry->required = true; }
  void        require ( ) { flagEntry->required = true; }

  ErrorOr<T&> operator* ( ) {
    if (ptr == ((T*) MOVED_POINTER)) {
      return format ("Pointer has been moved.");
    }
    if (!ptr) { return format ("Dereferencing null pointer."); }
    return *ptr;
  }
};


template<template<typename, typename...> class C = std::vector> class Flag {

  private:
  std::vector<FlagEntry*> flags;
  std::string             desc;

  void                    print (std::ostream& s, void* v, FlagType t) const {
    FlagType p = (t | List) ^ List;
    switch (p) {
      case Int:
        if ((t & List) != 0) {
          for (auto& i : *((C<int>*) v)) {
            print (s, &i, p);
            s << " ";
          }
        } else {
          s << *((int*) v);
        }
        break;
      case Double:
        if ((t & List) != 0) {
          for (auto& i : *((C<double>*) v)) {
            print (s, &i, p);
            s << " ";
          }
        } else {
          s << *((double*) v);
        }
        break;
      case None:
      case Bool:
        if ((t & List) != 0) {
          for (bool i : *((C<bool>*) v)) {
            print (s, &i, p);
            s << " ";
          }
        } else {
          s << *((bool*) v);
        }
        break;
      case String: s << *((char**) v); break;
      default: s << "Printing a value of unsupported type.";
    }
  }

  ErrorOr<int> readList (char* str, FlagEntry* v) {
    std::stringstream ss;
    ss << str;
    switch (v->ftype ^ List) {
      case None: return 0;
      case Int: {
        C<int>& f = *(C<int>*) v->flagv;
        while (!ss.eof ( )) {
          int i = 0;
          ss >> i;
          if (!ss.fail ( )) {
            f.push_back (i);
          } else {
            return format ("While parsing '{}', failed to parse int...", str);
          }
        }
      } break;
      case Double: {
        C<double>& f = *(C<double>*) v->flagv;

        while (!ss.eof ( )) {
          double i = 0;
          ss >> i;
          if (!ss.fail ( )) {
            f.push_back (i);
          } else {
            return format ("While parsing '%s', failed to parse double...",
                           str);
          }
        }
      } break;
      case Bool: {
        C<bool>& f = *(C<bool>*) v->flagv;
        while (!ss.eof ( )) {
          bool i = false;
          ss >> i;
          if (!ss.fail ( )) {
            f.push_back (i);
          } else {
            return format ("While parsing '%s', failed to parse bool...", str);
          }
        }
      } break;
      case String: {
        assert (false && "Unimplemented");
      } break;
      default: return format ("Cannot read a value of unsupported type", str);
    }
    if (!ss.fail ( )) { return 0; }

    return format ("Something went wrong while reading a list from '{}'", str);
  }

  ErrorOr<int> readValue (char* str, FlagEntry* v) {
    std::stringstream ss;
    ss << str;
    switch (v->ftype) {
      case None: return 0;
      case Int: {
        int f = 0;
        ss >> f;
        *(int*) v->flagv = f;
      } break;
      case Double: {
        double f = 0;
        ss >> f;
        *(double*) v->flagv = f;
      } break;
      case Bool: {
        bool f = false;
        if (strcmp (str, "true") == 0) {
          f = true;
        } else if (strcmp (str, "false") == 0) {
          f = false;
        } else {
          ss >> f;
        }
        *(bool*) v->flagv = f;
      } break;
      case String: {
        *(char**) v->flagv = str;
        return 0;
      } break;
      default: return format ("Cannot read a value of unsupported type", str);
    }
    if (!ss.fail ( )) { return 0; }

    return format ("Something went wrong while reading a value from '{}'", str);
  }

  ErrorOr<int> read (char* str, FlagEntry* v) {
    if ((v->ftype & List) != 0) { return readList (str, v); }

    return readValue (str, v);
  }

  ErrorOr<int> verifyRequired ( ) {
    for (auto* flag : flags) {
      if (flag->required) {
        return format (
          "'-{0}' is a required flag. Please provide it to the program",
          flag->name);
      }
    }
    return 0;
  }

  public:
  Flag& operator= (Flag const&) = delete;
  Flag& operator                = (Flag&& other) noexcept {
    if (this != &other) { flags = std::move (other.flags); }
    return *this;
  };

  Flag (Flag const&) = delete;
  Flag (Flag&& other) noexcept : flags (std::move (other.flags)){ };

  Flag (std::string desc) : desc (std::move (desc)){ };

  ErrorOr<FlagRef<bool>> flag (std::string name, std::string usage) {
    void* t = malloc (sizeof (bool));

    if (t == nullptr) {
      return format ("Failed to allocate '{}' bytes of contiguous memory.",
                     sizeof (bool));
    }

    auto* entr   = new FlagEntry{FlagTypeV<void>::value,
                               t,
                               std::move (name),
                               std::move (usage)};

    *((bool*) t) = false;
    flags.push_back (entr);
    FlagRef<bool> ref ((bool*) t, flags.back ( ));
    return ref;
  }

  template<typename T>
    requires (!SameContainer<T, C>)
  ErrorOr<FlagRef<T>> flag (std::string name, T def, std::string usage) {
    static_assert (FlagTypeV<T>::value != Unsupported,
                   "Flags of that type are not supported!");

    void* t = malloc (sizeof (T));

    if (!t) {
      return format ("Failed to allocate '{}' bytes of contiguous memory.",
                     sizeof (T));
    }

    auto* entr = new FlagEntry{FlagTypeV<T>::value,
                               t,
                               std::move (name),
                               std::move (usage)};

    *((T*) t)  = def;
    flags.push_back (entr);
    FlagRef<T> ref ((T*) t, flags.back ( ));
    return ref;
  }

  template<typename T, typename U = typename T::value_type>
    requires SameContainer<T, C> ErrorOr<FlagRef<T>>
      flag (std::string name, T def, std::string usage) {
      static_assert (FlagTypeV<U>::value != Unsupported,
                     "Flags of that type are not supported!");

      void* _t = malloc (sizeof (T));
      T*    t  = new (_t) T ( );

      if (!t) {
        return format ("Failed to allocate '{}' bytes of contiguous memory.",
                       sizeof (T));
      }

      auto* entr = new FlagEntry{FlagTypeV<U>::value | List,
                                 (void*) t,
                                 std::move (name),
                                 std::move (usage)};

      *t         = def;
      flags.push_back (entr);
      FlagRef<T> ref (t, flags.back ( ));
      return ref;
    }

  void usage ( ) const {
    std::cerr << desc << std::endl;
    for (auto* it : flags) {
      std::cerr << "\t-" << it->name << "\t" << it->usage << "\t(default: ";
      print (std::cerr, it->flagv, it->ftype);
      std::cerr << ")";
      if (it->required) { std::cerr << " [required]"; }
      std::cerr << std::endl;
    }
  }

  ~Flag ( ) {
    for (auto* it : flags) {
      if ((it->ftype & List) != 0) {
        switch (it->ftype ^ List) {
          case Int: ((C<int>*) it->flagv)->~C<int> ( ); break;
          case Double: ((C<double>*) it->flagv)->~C<double> ( ); break;
          case Bool: ((C<bool>*) it->flagv)->~C<bool> ( ); break;
        }
      }
      free (it->flagv);
      delete it;
    }
  }

  ErrorOr<int> parseArgument (size_t& i, size_t argc, char** argv) {
    char* cur = argv[i];
    cur++;
    size_t     len = strlen (cur);
    FlagEntry* f   = nullptr;
    for (auto* flag : flags) {
      if (len == flag->name.size ( ) && flag->name == cur) {
        f              = flag;
        flag->required = false;
      } else {
        return 0;
      }
    }
    if (f == nullptr) {
      return format (
        "Unrecognised command-line argument '{}' at position '{}'. ",
        cur,
        i);
    }
    if (f->ftype == None) {
      *(bool*) f->flagv = true;
      return 0;
    }
    if (i + 1 >= argc) {
      return format ("Failed to read command-line argument value for '{}' at "
                     "position '{}'. Reached end of arguments.",
                     cur,
                     i);
    }
    auto* next = argv[++i];
    TRY_WITH (read (next, f),
              format ("Failed to read command-line argument value for '{}' at "
                      "position '{}'.",
                      cur,
                      i));
    return 0;
  }

  ErrorOr<size_t> parse (size_t argc, char** argv) {
    for (size_t i = 0; i < argc; i++) {
      auto* cur = argv[i];
      if (*cur == '-') {
        TRY_WITH (parseArgument (i, argc, argv),
                  format ("Reading CMD flags failed."));
      } else {
        TRY (verifyRequired ( ));
        return i;
      }
    }
    TRY (verifyRequired ( ));
    return argc;
  }
};
}     // namespace Kal
