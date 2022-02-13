#pragma once

#include <Kal/ErrorOr.hpp>
#include <Kal/Format.hpp>
#include <Kal/Option.hpp>

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace Kal::Data {

template<typename T> class RadixTree {
  class Node {
    private:
    char*             prefix = nullptr;
    size_t            length = 0;
    T*                data   = nullptr;
    std::vector<Node> children{ };

    ErrorOr<void>
      breakUpAt (size_t prefixSize, const char* newSuffix, T newData) {
      char* originalSuffix = strdup (prefix + prefixSize);
      if (!originalSuffix) {
        return format ("Failed to duplicate '{}'", prefix + prefixSize);
      }
      prefix[prefixSize] = '\0';
      length             = prefixSize;

      Node original (originalSuffix, data);

      original.children = std::move (children);
      children          = { };

      data              = nullptr;

      children.push_back (std::move (original));

      char* suffixAlloc = strdup (newSuffix + prefixSize);
      if (!suffixAlloc) {
        return format ("Failed to duplicate '{}'", newSuffix + prefixSize);
      }

      Node newer (suffixAlloc, newData);


      children.push_back (std::move (newer));
      return { };
    }

    // TODO: Add templating to support storing primitives on the stack.
    Node (char* s, T t) : prefix (s), data ((T*) malloc (sizeof (T))) {
      if (data) {
        new (data) T (t);
      } else {
        assert (false && "Ran out of memory");
      }
      length = strlen (s);
    }

    Node (char* s, T* t) : prefix (s), data (t), length (strlen (s)) { }

    Option<T> contains (const char* pre, size_t len) const {
      if (len > length) {
        if (strncmp (pre, prefix, length) != 0) return { };

        const auto* nextPre = pre + length;
        for (auto& child : children) {
          if (child.prefix && child.prefix[0] == nextPre[0]) {
            return child.contains (nextPre, len - length);
          }
        }
        return { };
      }

      if (len == length && strcmp (pre, prefix) == 0 && data) { return *data; }
      return { };
    }

    Option<T> remove (const char* pre, size_t len) {
      if (len > length) {
        if (strncmp (pre, prefix, length) != 0) return { };

        const auto* nextPre = pre + length;
        for (size_t i = 0; i < children.size ( ); i++) {
          auto& child = children[i];
          if (child.prefix && child.prefix[0] == nextPre[0]) {
            auto res = std::move (child.remove (nextPre, len - length));
            if (res && !child.data && child.children.size ( ) == 0) {
              children.erase (children.begin ( ) + i);
            }
            if (child.data && !data && children.size ( ) == 1) {
              consolidate ( );
            }
            return res;
          }
        }
        return { };
      }

      if (len == length && strcmp (pre, prefix) == 0 && data) {
        T res = *data;
        free (data);
        data = nullptr;
        return res;
      }

      return { };
    }

    inline ErrorOr<bool>
      setWhenEmptyString (const char* s, T const& t, size_t newlen) {
      if (newlen == 0 && length == 0) {
        if (data) {
          new (data) T (t);
        } else {
          data = (T*) malloc (sizeof (T));
          if (!data) {
            return format ("Failed to allocate '{}' bytes", sizeof (T));
          }
          new (data) T (t);
        }
        return true;
      }
      return false;
    }

    ErrorOr<void> setValue (T const& t) {
      if (!data) {
        data = (T*) malloc (sizeof (T));
        if (!data) {
          return format ("Failed to allocate '{}' bytes", sizeof (T));
        }
      }
      new (data) T (t);
      return { };
    }

    ErrorOr<void> addSubstring (T const& t, size_t substringLength) {
      if (data) {
        T* newData = (T*) malloc (sizeof (T));
        if (!newData) {
          return format ("Failed to allocate '{}' bytes", sizeof (T));
        }
        new (newData) T (t);
        Node newChild (strdup (prefix + substringLength + 1), data);
        data = newData;
        children.push_back (std::move (newChild));
      } else {
        data = (T*) malloc (sizeof (T));
        if (!data) {
          return format ("Failed to allocate '{}' bytes", sizeof (T));
        }
        new (data) T (t);
        Node newChild (strdup (prefix + substringLength + 1), nullptr);
        children.push_back (std::move (newChild));
      }
      prefix[substringLength + 1] = '\0';
      length                      = substringLength + 1;

      return { };
    }

    public:
    ErrorOr<void> add (const char* s, T t) {
      auto newlen = strlen (s);
      if (TRY (setWhenEmptyString (s, t, newlen))) return { };

      for (auto& child : children) {
        auto   prefixLength = child.length;

        size_t i            = 0;
        for (; i < prefixLength; i++) {
          if (child.prefix[i] != s[i]) { break; }

          if (i == newlen - 1) {
            if (newlen == length) { child.setValue (t); }
            child.addSubstring (t, i);
          }

          if (i == prefixLength - 1) { return child.add (s + i + 1, t); }
        }

        if (i != 0) { return child.breakUpAt (i, s, t); }
      }

      char* suffixAlloc = strdup (s);
      if (!suffixAlloc) return format ("Failed to duplicate '{}'", s);

      Node newer (suffixAlloc, t);

      children.push_back (std::move (newer));
      return { };
    }


    Option<T> contains (const char* pre) const {
      return contains (pre, strlen (pre));
    }

    explicit Node (const char* s) : prefix (strdup (s)) {
      if (!prefix) assert (false && "Ran out of memory");
      length = strlen (s);
    }

    ~Node ( ) {
      if (prefix) {
        free (prefix);
        length = 0;
      }
      if (data) {
        data->~T ( );
        free (data);
      }
    }

    T* getData ( ) const { return data; }

    Node (Node const& other) {
      if (&other != this) {
        children = other.children;
        prefix   = strdup (other.prefix);
        length   = other.length;
        if (!prefix) assert (false && "Ran out of memory");
        if (other.data) {
          data = (T*) malloc (sizeof (T));
          if (!data) assert (false && "Ran out of memory");
          new (data) T (*other.data);
        }
      }
    }

    Node& operator= (Node const& other) {
      if (&other != this) {
        children = other.children;
        prefix   = strdup (other.prefix);
        length   = other.length;
        if (!prefix) assert (false && "Ran out of memory");
        if (other.data) {
          data = (T*) malloc (sizeof (T));
          if (!data) assert (false && "Ran out of memory");
          new (data) T (*other.data);
        }
      }
      return *this;
    }

    Node (Node&& other) noexcept
        : prefix (other.prefix)
        , data (other.data)
        , length (other.length) {
      if (&other != this) {
        children     = std::move (other.children);
        other.data   = nullptr;
        other.prefix = nullptr;
        other.length = 0;
      }
    }

    Node& operator= (Node&& other) noexcept {
      if (&other != this) {
        prefix       = other.prefix;
        data         = other.data;
        length       = other.length;
        children     = std::move (other.children);
        other.data   = nullptr;
        other.prefix = nullptr;
        other.length = 0;
      }
      return *this;
    }

    ErrorOr<void> consolidate ( ) {
      for (auto& child : children) {
        TRY (child.consolidate ( ));
        if (child.data) continue;
        if (child.children.size ( ) == 1) {
          auto  other              = child.children.front ( );
          auto  parentN            = child.length;
          auto  childN             = other.length;

          char* consolidatedPrefix = (char*) malloc (parentN + childN + 1);

          if (!consolidatedPrefix) {
            return format ("Failed to allocate '{}' bytes",
                           parentN + childN + 1);
          }

          strncpy (consolidatedPrefix, child.prefix, parentN);
          strncpy (consolidatedPrefix + parentN, other.prefix, childN);

          consolidatedPrefix[parentN + childN] = '\0';

          child.data                           = other.data;
          other.data                           = nullptr;
          if (other.children.size ( ) > 0) {
            child.children = std::move (other.children);
          } else {
            child.children.erase (child.children.begin ( ));
          }
          free (child.prefix);
          child.prefix = consolidatedPrefix;
          child.length = parentN + childN;
        }
      }
      return { };
    }

    Option<T> remove (const char* s) { return remove (s, strlen (s)); }
  };


  private:
  Node root;
  public:
  Option<T> lookup (std::string const& s) {
    return root.contains (s.c_str ( ));
  }

  bool contains (std::string const& s) {
    return root.contains (s.c_str ( )).isDefined ( );
  }

  ErrorOr<T> remove (std::string const& s) {
    if (!contains (s)) return format ("Key '{}' not in tree.", s);
    auto res = root.remove (s.c_str ( ));
    return res;
  }

  ErrorOr<void> add (std::string const& s, T t) {
    const char* cstr = s.c_str ( );
    ({
      auto result = std ::move (root.add (cstr, t));
      if (result.isEmpty ( )) { return result; }
      result.get ( );
    });
    return { };
  }

  bool contains (const char* s) { return root.contains (s).isDefined ( ); }

  ErrorOr<T> remove (const char* s) {
    if (!contains (s)) return format ("Key '{}' not in tree.", s);
    auto res = root.remove (s);
    return res;
  }

  ErrorOr<void> add (const char* s, T t) {
    TRY (root.add (s, t));
    return { };
  }


  void clear ( ) { root = Node (""); }

  RadixTree ( ) : root (""){ };

  RadixTree (RadixTree const& other) : root (other.root) { }
  RadixTree (RadixTree&& other) noexcept : root (std::move (other.root)) { }

  RadixTree& operator= (RadixTree const& other) {
    if (&other != this) { root = other.root; }
    return *this;
  }

  RadixTree& operator= (RadixTree&& other) noexcept {
    if (&other != this) { root = std::move (other.root); }
    return *this;
  }

  ~RadixTree ( ) = default;
};

}     // namespace Kal::Data
