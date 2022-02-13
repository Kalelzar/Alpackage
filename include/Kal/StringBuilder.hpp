#pragma once

#include <Kal/Format.hpp>

class StringBuilder {

  private:
  char*  buffer         = nullptr;
  size_t bufferSize     = 0;
  size_t bufferCapacity = 0;

  void   expand ( );

  public:
  StringBuilder ( );
  // NOTE: Preallocate enough space?
  explicit StringBuilder (std::string const& s) { put (s); }
  StringBuilder (std::string const& s, bool reverse) {
    if (reverse) {
      putr (s);
    } else {
      put (s);
    }
  }
  ~StringBuilder ( );
  StringBuilder (StringBuilder&&) noexcept;
  StringBuilder& operator= (StringBuilder&&) noexcept;
  StringBuilder (StringBuilder const&);
  StringBuilder& operator= (StringBuilder const&);


  void           put (const char*);
  void           put (const char*, size_t size);
  void           put (std::string const&);
  void           putr (const char*);
  void           putr (const char*, size_t size);
  void           putr (std::string const&);

  template<typename T>
    requires (
      ToStringable<
        T> || OStreamable<T> || (AnyIterable<T> && (OStreamable<typename IteratesWith<T>::type> || ToStringable<typename IteratesWith<T>::type>) ))
  void put (T const& t) { put (mkString (t)); }

  template<typename T>
    requires (
      ToStringable<
        T> || OStreamable<T> || (AnyIterable<T> && (OStreamable<typename IteratesWith<T>::type> || ToStringable<typename IteratesWith<T>::type>) ))
  void                         putr (T const& t) { putr (mkString (t)); }


  template<typename... T> void putf (std::string const& formatString, T... t) {
    put (format (formatString, t...));
  }

  template<typename... T> void putrf (std::string const& formatString, T... t) {
    putr (format (formatString, t...));
  }

  [[nodiscard]] std::string get ( ) const;
  [[nodiscard]] std::string getr ( ) const;
};
