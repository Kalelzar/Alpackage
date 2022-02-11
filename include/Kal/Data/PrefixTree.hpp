#pragma once


#include <Kal/ErrorOr.hpp>
#include <Kal/Format.hpp>

#include <cassert>
#include <cstddef>
#include <cstring>

namespace Kal::Data {

template<typename T> class PrefixTree {
  private:
  class Node {
    private:
    char           matcher = '\0';
    Option<T>      value{ };
    Node**         children      = nullptr;
    size_t         childrenCount = 0;
    size_t         capacity      = 0;

    ErrorOr<Node*> push (char next) {
      for (size_t i = 0; i < childrenCount; i++) {
        if (children[i] && children[i]->matcher == next) { return children[i]; }
      }

      if (childrenCount + 1 > capacity) { TRY (expand ( )); }

      Node* child = (Node*) malloc (sizeof (Node));

      if (!child) {
        return format ("Failed to allocate '{}' bytes.", sizeof (Node));
      }

      children[childrenCount] = child;
      childrenCount++;
      new (child) Node (next);

      return child;
    }

    ErrorOr<Node*> add (T const& value, const char* tail, size_t n) {
      if (n == 0) { return format ("Cannot add empty string to tree"); }

      auto* node = TRY_WITH (push (tail[0]),
                             format ("Failed to add '{}' to tree", tail));
      if (n == 1) {
        node->value = value;
        return node;
      } else {
        if (node)
          return node->add (value, tail + 1, n - 1);
        else
          return format (
            "Dereferencing a pointer to null while adding '{}' to tree.",
            tail);
      }
    }

    ErrorOr<T> get (const char* tail, size_t n) {
      if (n == 0) {
        return format ("Cannot get the value of an empty prefix.");
      }

      auto* node = TRY (push (tail[0]));
      if (n == 1) {
        if (node->value.isDefined ( )) { return node->value.get ( ); }

        return format ("Trying to get an empty value.");
      }

      if (node)
        return node->get (tail + 1, n - 1);
      else
        return format (
          "Dereferencing a pointer to null while adding '{}' to tree.",
          tail);
    }

    ErrorOr<size_t> expand ( ) {
      if (children) {
        size_t newCapacity = capacity * 2;
        children
          = (Node**) reallocarray (children, newCapacity, sizeof (void*));
        if (!children) {
          return format ("Failed to allocate '{}' bytes.",
                         sizeof (void*) * newCapacity);
        }
        capacity = newCapacity;
        return newCapacity;
      }

      static const size_t startingCapacity = 8;
      children = (Node**) calloc (startingCapacity, sizeof (void*));
      if (!children) {
        return format ("Failed to allocate '{}' bytes.",
                       sizeof (void*) * startingCapacity);
      }

      capacity = startingCapacity;
      return startingCapacity;
    }

    ErrorOr<std::string> toString (int depth, bool single) {
      std::ostringstream ss;
      for (int i = 0; i < depth; i++) {
        if (single)
          ss << " ";
        else
          ss << "|";
      }

      ss << matcher << " : " << value << std::endl;

      for (size_t i = 0; i < childrenCount; i++) {
        if (children[i])
          ss << children[i]->toString (depth + 1, childrenCount == 1);
      }
      return ss.str ( );
    }

    Option<T> has (const char* tail, size_t n) {
      if (n == 0) { return value; }

      for (size_t i = 0; i < childrenCount; i++) {
        if (children[i] && children[i]->matcher == tail[0]) {
          return children[i]->has (tail + 1, n - 1);
        }
      }

      return { };
    }

    public:
    ErrorOr<Node*> add (T const& value, const char* tail) {
      return add (value, tail, strlen (tail));
    }

    ErrorOr<T> get (const char* tail) { return get (tail, strlen (tail)); }

    Option<T>  has (const char* tail) { return has (tail, strlen (tail)); }

    ErrorOr<std::string> toString ( ) {
      return toString (0, childrenCount == 1);
    }

    Option<T> get ( ) { return value; }

    explicit Node (char next) : matcher (next) { }

    Node (Node const& other)
        : matcher (other.next)
        , childrenCount (other.childrenCount) {
      if (&other != this) {
        capacity = other.childrenCount;
        children = calloc (childrenCount, sizeof (T));
        if (children) {
          memcpy (children, other.children, other.childrenCount);
        } else
          assert (false);
      }
    }

    Node& operator= (Node const& other) {
      if (&other != this) {
        matcher       = other.next;
        childrenCount = other.childrenCount;
        capacity      = other.childrenCount;
        children      = calloc (childrenCount, sizeof (T));
        if (children) {
          memcpy (children, other.children, other.childrenCount);
        } else
          assert (false);
      }
      return *this;
    }

    Node (Node&& other) noexcept
        : matcher (other.next)
        , childrenCount (other.childrenCount)
        , capacity (other.capacity)
        , children (other.children) {
      if (&other != this) {
        other.children      = nullptr;
        other.childrenCount = 0;
        other.capacity      = 0;
      }
    }

    Node& operator= (Node&& other) noexcept {
      if (&other != this) {
        matcher             = other.next;
        childrenCount       = other.childrenCount;
        capacity            = other.capacity;
        children            = other.children;
        other.children      = nullptr;
        other.childrenCount = 0;
        other.capacity      = 0;
      }
      return *this;
    }


    Node ( )  = delete;

    ~Node ( ) = default;

    void free ( ) {
      if (children) {
        for (size_t i = 0; i < childrenCount; i++) {
          if (children[i]) {
            children[i]->free ( );
            ::free (children[i]);
          }
        }
        ::free (children);
      }

      this->~Node ( );
    }
  };

  Node* root = nullptr;

  public:
  PrefixTree ( ) : root ((Node*) malloc (sizeof (Node))) {
    if (!root) {     // BAD!
      // TODO: Log Error
      exit (1);
    }
    new (root) Node ('\0');
  }

  // FIXME: Implement copy/move constructors and operators

  ErrorOr<std::string> toString ( ) { return root->toString ( ); }

  ErrorOr<T>           get (const char* lookup) { return root->get (lookup); }
  Option<T>            has (const char* lookup) { return root->has (lookup); }

  ErrorOr<T>           add (T const& value, const char* lookup) {
    return TRY (root->add (value, lookup))->get ( );
  }


  ~PrefixTree ( ) {
    root->free ( );
    ::free (root);
  }
};

}     // namespace Kal::Data
