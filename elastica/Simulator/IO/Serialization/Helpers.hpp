//==============================================================================
/*!
//  \file
//  \brief Defines convenience helpers for serialization
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

// Reused from cereal : https://uscilab.github.io/cereal/index.html
// Distributed under the BSD License.
// See https://github.com/USCiLab/cereal/blob/master/LICENSE for details.

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <cstdint>
#include <string>
#include <type_traits>
#include <utility>

#include "Simulator/IO/Serialization/Types.hpp"
#include "Utilities/NonCopyable.hpp"

namespace elastica {

  namespace io {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief A wrapper around size metadata
    // \ingroup serialization
    //
    // \details
    // This class provides a way for archives to have more flexibility over how
    // they choose to serialize size metadata for containers.  For some archive
    // types, the size may be implicitly encoded in the output (e.g. JSON) and
    // not need an explicit entry.  Specializing serialize or load/save for
    // your archive and SizeTags allows you to choose what happens.
    //
    // \tparam T The integral size type
    */
    template <class T>
    class SizeTag : NonCopyable {
     private:
      // Store a reference if passed an lvalue reference, otherwise
      // make a copy of the data
      using type =
          typename std::conditional<std::is_lvalue_reference<T>::value, T,
                                    typename std::decay<T>::type>::type;

     public:
      explicit SizeTag(T&& sz) : size(std::forward<T>(sz)) {}

      type size;
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief A wrapper around data that can be serialized in a binary fashion
    // \ingroup serialization
    //
    // \details
    //
    // This class is used to demarcate data that can safely be serialized
    // as a binary chunk of data.  Individual archives can then choose how
    // best represent this during serialization.
    //
    // \tparam T The type of data being pointed to
    */
    template <class T>
    struct BinaryData {
      //! Internally store the pointer as a void *, keeping const if created
      //! with a const pointer
      using PT = typename std::conditional<
          std::is_const<typename std::remove_pointer<
              typename std::remove_reference<T>::type>::type>::value,
          const void*, void*>::type;

      BinaryData(T&& d, std::uint64_t s) : data(std::forward<T>(d)), size(s) {}

      PT data;             //!< pointer to beginning of data
      std::uint64_t size;  //!< size in bytes
    };
    //**************************************************************************

    namespace detail {
      struct NameValuePairCore {};  //!< Traits struct for NVPs
    }                               // namespace detail

    //**************************************************************************
    /*!\brief Holds name value pairs
     * \ingroup serialization
     *
     * \details
     * Pairs a name (some string) with some value such that an archive
     * can potentially take advantage of the pairing.
     */
    template <class T>
    class NameValuePair : detail::NameValuePairCore, NonCopyable {
     private:
      // If we get passed an array, keep the type as is, otherwise store
      // a reference if we were passed an l value reference, else copy the value
      using Type = typename std::conditional<
          std::is_array<typename std::remove_reference<T>::type>::value,
          typename std::remove_cv<T>::type,
          typename std::conditional<std::is_lvalue_reference<T>::value, T,
                                    typename std::decay<T>::type>::type>::type;

      // prevent nested nvps
      static_assert(!std::is_base_of<detail::NameValuePairCore, T>::value,
                    "Cannot pair a name to a NameValuePair");

     public:
      //************************************************************************
      /*!\brief Constructs a new NameValuePair
       *
       * \details
       * Ideally this should be an l-value
       * reference so that the value can be both loaded and saved to.  If you
       * pass an r-value reference, the NameValuePair will store a copy of it
       * instead of a reference.  Thus you should only pass r-values in cases
       * where this makes sense, such as the result of some size() call.
       *
       * \param n The name of the pair
       * \param v The value to pair.
       */
      NameValuePair(char const* n, T&& v)
          : name(n), value(std::forward<T>(v)) {}
      //************************************************************************

      char const* name;
      Type value;
    };
    //**************************************************************************

    //==========================================================================
    //
    //  GLOBAL FUNCTIONS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Creates a size tag from some variable.
    // \ingroup serialization
    //
    // \details
    // Will normally be used to serialize size (e.g. size()) information for
    // variable size containers.  If you have a variable sized container,
    // the very first thing it serializes should be its size, wrapped in
    // a SizeTag.
    //
    // \param sz Size of data
    */
    template <class T>
    inline SizeTag<T> make_size_tag(T&& sz) {
      return SizeTag<T>{std::forward<T>(sz)};
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Convenience function to create binary data for both const and non
    // const pointers
    // \ingroup serialization
    //
    // \details
    //
    // \param data Pointer to beginning of the data
    // \param size The size in bytes of the data
    */
    template <class T>
    inline BinaryData<T> binary_data(T&& data, size_t size) {
      return {std::forward<T>(data), size};
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Creates a name value pair
     * \ingroup serialization
     *
     * \see NameValuePair
     */
    template <class T>
    inline NameValuePair<T> make_nvp(std::string const& name, T&& value) {
      return {name.c_str(), std::forward<T>(value)};
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Creates a name value pair
     * \ingroup serialization
     *
     * \see NameValuePair
     */
    template <class T>
    inline NameValuePair<T> make_nvp(const char* name, T&& value) {
      return {name, std::forward<T>(value)};
    }
    //**************************************************************************

    //**************************************************************************
    //! A specialization of make_nvp<> that bypasses creating an nvp for
    //! binary archives
    template <class Archive, class T>
    inline typename std::enable_if<
        std::is_same<Archive, ::elastica::io::BinaryInputArchive>::value ||
            std::is_same<Archive, ::elastica::io::BinaryOutputArchive>::value,
        T&&>::type
    make_nvp(const char*, T&& value) {
      return std::forward<T>(value);
    }
    //**************************************************************************

    //**************************************************************************
    //! A specialization of make_nvp<> that actually creates an nvp for
    //! non-binary archives
    template <class Archive, class T>
    inline typename std::enable_if<
        !(std::is_same<Archive, ::elastica::io::BinaryInputArchive>::value ||
          std::is_same<Archive, ::elastica::io::BinaryOutputArchive>::value),
        NameValuePair<T> >::type
    make_nvp(const char* name, T&& value) {
      return {name, std::forward<T>(value)};
    }
    //**************************************************************************

//! Creates a name value pair for the variable T with the same name as the
//! variable
#define MAKE_NVP(T) ::elastica::io::make_nvp(#T, T)

    //**************************************************************************
    //!\brief A wrapper around a key and value for serializing data into maps.
    //!\see make_map_item
    template <class Key, class Value>
    struct MapItem : NonCopyable {
      using KeyType =
          typename std::conditional<std::is_lvalue_reference<Key>::value, Key,
                                    typename std::decay<Key>::type>::type;

      using ValueType =
          typename std::conditional<std::is_lvalue_reference<Value>::value,
                                    Value,
                                    typename std::decay<Value>::type>::type;

      //! Construct a MapItem from a key and a value
      MapItem(Key&& key_, Value&& value_)
          : key(std::forward<Key>(key_)), value(std::forward<Value>(value_)) {}

      KeyType key;
      ValueType value;
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Create a MapItem so that human readable archives will group keys
     * and values together
     * \see MapItem
     */
    template <class KeyType, class ValueType>
    inline MapItem<KeyType, ValueType> make_map_item(KeyType&& key,
                                                     ValueType&& value) {
      return {std::forward<KeyType>(key), std::forward<ValueType>(value)};
    }
    //**************************************************************************

  }  // namespace io

}  // namespace elastica