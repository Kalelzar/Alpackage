#pragma once

#include <Kal/Concepts/Collection.hpp>
#include <Kal/Concepts/Functional.hpp>
#include <Kal/Concepts/Iterator.hpp>
#include <Kal/Concepts/Stream.hpp>
#include <Kal/default.hpp>
#include <sstream>

template<IndexableT C, typename T = typename IndexReturnType<C, SizeT>::type>
  requires WithDefaultValue<T> &&(!Iterable<C>) &&SizeableT<C> T
    aggregate (const C collection, Aggregator<T> auto aggregator) {
    T total = defaultValue<T>;
    for (SizeT i = 0; i < collection.size ( ); i++) {
      total = aggregator (total, collection[i]);
    }
    return total;
  }

  template<Iterable C,
           typename T  = typename IteratesWith<C>::type,
           typename T2 = RemoveConst<RemoveRef<T>>>
  T2 aggregate (const C collection, Aggregator<T> auto aggregator) {
    T2 total = defaultValue<T2>;
    for (const T& it : collection) { total = aggregator (total, it); }
    return total;
  }

  template<typename T, template<typename> class C>
    requires IndexableT<C<T>> &&(!Iterable<C<T>>) &&SizeableT<
      C<T>>&& Appendable<C, T> C<T> filter (const C<T>        collection,
                                            Predicate<T> auto predicate) {
      C<T> newCollection = defaultValue<C<T>>;
      for (SizeT i = 0; i < collection.size ( ); i++) {
        if (predicate (collection[i])) newCollection.push_back (collection[i]);
      }

      return newCollection;
    }

    template<typename T, template<typename> class C>
      requires Iterable<C<T>> && Appendable<C, T> C<T>
        filter (const C<T> collection, Predicate<T> auto predicate) {
        C<T> newCollection = defaultValue<C<T>>;
        for (const T& it : collection) {
          if (predicate (it)) newCollection.push_back (it);
        }

        return newCollection;
      }

    template<typename T, template<typename> class C>
      requires Iterable<C<T>> && Insertable<C, T> C<T>
        filter (const C<T> collection, Predicate<T> auto predicate) {
        C<T> newCollection = defaultValue<C<T>>;
        for (const T& it : collection) {
          if (predicate (it)) newCollection.insert (it);
        }

        return newCollection;
      }

    template<typename T, template<typename> class C>
      requires Iterable<C<T>> && Reversible<C<T>> && Prependable<C, T> &&(
        !Appendable<C, T>) C<T> filter (const C<T>        collection,
                                        Predicate<T> auto predicate) {
        C<T> newCollection = defaultValue<C<T>>;
        for (const T& it : collection) {
          if (predicate (it)) newCollection.push_front (it);
        }

        newCollection.reverse ( );

        return newCollection;
      }

      template<typename U, typename T, template<typename> class C>
        requires IndexableT<C<T>> &&(!Iterable<C<T>>) &&SizeableT<
          C<T>>&& Appendable<C,
                             T>&& Appendable<C,
                                             U> C<U> map (const C<T> collection,
                                                          Transformer<T, U> auto
                                                            transformer) {
          C<U> newCollection = defaultValue<C<U>>;
          for (SizeT i = 0; i < collection.size ( ); i++) {
            newCollection.push_back (transformer (collection[i]));
          }

          return newCollection;
        }

        template<typename U, typename T, template<typename> class C>
          requires Mappable<C, T, U> C<U>
            map (const C<T> collection, Transformer<T, U> auto transformer) {
            return collection.map (transformer);
          }

        template<typename U, typename T, template<typename> class C>
          requires Iterable<C<T>> && Appendable<C, U> C<U>
            map (const C<T> collection, Transformer<T, U> auto transformer) {
            C<U> newCollection = defaultValue<C<U>>;
            for (const T& it : collection) {
              newCollection.push_back (transformer (it));
            }

            return newCollection;
          }

        template<typename U, typename T, template<typename> class C>
          requires Iterable<C<T>> && Insertable<C, U> C<U>
            map (const C<T> collection, Transformer<T, U> auto transformer) {
            C<U> newCollection = defaultValue<C<U>>;
            for (const T& it : collection) {
              newCollection.insert (transformer (it));
            }

            return newCollection;
          }

        template<typename U, typename T, template<typename> class C>
          requires Iterable<C<T>> && Reversible<C<U>> && Prependable<C, U> &&(
            !Appendable<C, U>) C<U> map (const C<T>             collection,
                                         Transformer<T, U> auto transformer) {
            C<U> newCollection = defaultValue<C<U>>;
            for (const T& it : collection) {
              newCollection.push_front (transform (it));
            }

            newCollection.reverse ( );

            return newCollection;
          }

          template<typename T, typename C>
            requires IndexableT<C> &&(!Iterable<C>) &&SizeableT<
              C> void foreach (const C collection, Consumer<T> auto consumer) {
              for (SizeT i = 0; i < collection.size ( ); i++) {
                consumer (collection[i]);
              }
            }

            template<typename T>
            void foreach (const Iterable auto collection,
                          Consumer<T> auto    consumer) {
              for (const T& it : collection) { consumer (it); }
            }

            template<template<typename A> class Kind, typename T>
            Kind<T> lift (T);
            template<template<typename A> class Kind, typename T>
              requires ConstructibleFrom<Kind<T>, T> Kind<T> lift (T t) {
                return {t};
              }

            template<template<typename A> class Kind, typename T>
              requires Liftable<Kind, T> Kind<T> lift (T t) {
                return Kind<T>::lift (t);
              }

            template<template<typename> class Kind,
                     template<typename>
                     class C,
                     typename T>
            C<Kind<T>> liftAll (const C<T> collection) {
              return map<Kind<T>, T, C> (collection, [&] (const T& it) {
                return lift<Kind> (it);
              });
            }


            template<Iterable T>
              requires (!ConstIterable<T>)
            &&OStreamable<typename IteratesWith<T>::type> std::string
              mkString (T t, std::string delim) {
              std::stringstream s;
              s << "[ ";
              for (auto it = t.begin ( ); it != t.end ( ); ++it) {
                s << *it;
                if (std::next (it) != t.end ( )) s << delim;
              }
              s << " ]";
              return s.str ( );
            }


            template<ConstIterable T>
              requires OStreamable<typename IteratesWith<T>::type> std::string
                mkString (T t, std::string delim) {
                std::stringstream s;
                s << "[ ";
                for (auto it = t.cbegin ( ); it != t.cend ( ); ++it) {
                  s << *it;
                  if (std::next (it) != t.cend ( )) s << delim;
                }
                s << " ]";
                return s.str ( );
              }

            template<AnyIterable T>
              requires OStreamable<typename IteratesWith<T>::type> std::string
                mkString (T t) {
                return mkString (t, " ");
              }

            template<OStreamable C> std::string mkString (C t) {
              std::stringstream s;
              s << t;
              return s.str ( );
            }

            template<typename T> void swap (T& in, T& out) {
              T temp = in;
              in     = out;
              out    = in;
            }

            template<typename T> T exchange (T& target, T& value) {
              T temp = target;
              target = value;
              return temp;
            }

            template<typename T> T exchange (T& target, T&& value) {
              T temp = target;
              target = std::move (value);
              return temp;
            }
