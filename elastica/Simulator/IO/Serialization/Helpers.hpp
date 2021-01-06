//==============================================================================
/*!
//  \file
//  \brief Defines convenience helpers for serialization
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

// Reused from cereal : https://uscilab.github.io/cereal/index.html
// Distributed under the BSD License.
// See https://github.com/USCiLab/cereal/blob/master/LICENSE for details.

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

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
    const pointers
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

  }  // namespace io

}  // namespace elastica