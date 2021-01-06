//==============================================================================
/*!
//  \file
//  \brief Defines SerializeWizard, a safe interface for Serialize
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include <utility>

#include "Simulator/IO/Serialization/Serialize.hpp"
#include "Utilities/NonCreatable.hpp"
#include "Utilities/TypeTraits/IsDefined.hpp"

namespace elastica {

  namespace io {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Wizard class for accessing serialization functions
    // \ingroup serialization
    //
    // \details
    // SerializeWizard provides a safe interface for accessing serialization
    // functions, including some type-checking and nice error messages. This
    // class should be the preferred way of calling Serialize from any
    // OutputArchive or InputArchive types.
    //
    // \tparam T The type that needs to be serialized
    //
    // \note
    // T needs to be a concrete type here, and references/const/volatile etc
    // need to be removed prior
    */
    template <typename T>
    struct SerializeWizard : public NonCreatable {
      //**Static variables******************************************************
      //! Check for customization of Serialize struct
      static constexpr bool is_serialize_defined =
          ::tt::is_defined_v<Serialize<T>>;
      //************************************************************************

      //**Serialization interface***********************************************
      /*!\brief Serializes all passed in data
      //
      // \details
      // This is the primary interface for loading serialized data with an
      // archive.
      */
      template <class Archive>
      static inline void load(Archive& ar, T& data) {
        // check for specialization
        static_assert(is_serialize_defined,
                      R"error(
Could not find the load serialization function for the provided type and
archive combination. Types must specialize the Serialize struct and provide a
load/save function pair. Load function generally has the following signature:

template<class Archive>
void load(Archive & ar, T& t)
{
  ar( t.member1, t.member2, t.member3 );
} )error");
        Serialize<T>::load(ar, data);
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Serializes all passed in data
      //
      // \details
      // This is the primary interface for saving serialized data with an
      // archive.
      */
      template <class Archive>
      static inline void save(Archive& ar, T const& data) {
        // 1. Check if Serialize() is specialized.
        static_assert(is_serialize_defined,
                      R"error(
Could not find the save serialization function for the provided type and archive
 combination. Types must specialize the Serialize struct and provide a load/save
 function pair. Save function generally has the following signature:

template<class Archive>
void save(Archive & ar, T const& t)
{
  ar( t.member1, t.member2, t.member3 );
} )error");
        Serialize<T>::save(ar, data);
      }
      //************************************************************************
    };

  }  // namespace io

}  // namespace elastica
