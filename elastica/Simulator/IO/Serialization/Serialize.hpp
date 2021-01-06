//==============================================================================
/*!
//  \file
//  \brief Defines the Serialize struct, the customization point of \elastica 's
//  serialization module
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
#include <type_traits>
#include <utility>

#include "Simulator/IO/Serialization/Helpers.hpp"
#include "Simulator/IO/Serialization/Types.hpp"
#include "Utilities/NonCreatable.hpp"

namespace elastica {

  namespace io {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Customization point for prologue and epilogue for serializing
    // \ingroup serialization
    //
    // \details
    // SerializePrePost provides the sole customization point for adding
    // messages/tags pre- and post- serialization. The struct can be customized
    // for user-defined types, achieved typically via the `epilogue` (for pre-
    // serialization) and `prologue` (for post-serialization) static functions,
    // as shown in the code snippet below.
    //
    // \example
       \code
       TODO
       \endcode
    //
    // \tparam T The type that needs to be serialized
    //
    // \note
    // T needs to be a concrete type here, and references/const/volatile etc
    // need to be removed prior
    */
    template <typename T>
    struct SerializePrePost : public NonCreatable {
      //**Serialization interface***********************************************
      /*!\brief Adds a prologue to a serialized message
       */
      template <class Archive>
      static inline void prologue(Archive& /* unused */,
                                  T const& /* unused data */) {}
      //************************************************************************

      //************************************************************************
      /*!\brief Adds an epilogue to a serialized message
       */
      template <class Archive>
      static inline void epilogue(Archive& /* unused */,
                                  T const& /* unused data */) {}
      //************************************************************************
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Customization point for serialization of user-defined types
    // \ingroup serialization
    //
    // \details
    // Serialize is the sole customization point for adding serialization
    // support in \elastica for user-defined types. The struct needs to be
    // customized for user-defined types, achieved typically via the `load`
    // and `save` static functions, as shown in the code snippet below.
    //
    // \example
    // \snippet Test_Serialize.cpp custom_serialize
    //
    // which can then be used in any serialization archive supported by
    // \elastica like shown below
    //
    // \example
    // \snippet Test_Serialize.cpp custom_serialize_usage
    //
    // \tparam T The type that needs to be serialized
    //
    // \note
    // T needs to be a concrete type here, and references/const/volatile etc
    // need to be removed prior
    */
    template <typename T>
    struct Serialize;
    //**************************************************************************

    // enable if internal data type obtained from the Types.h file
    // Only here have access to an internal free function that does the
    // right thing using ADL

    //**Internal load***********************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for POD types
    // \ingroup serialization
    //
    // Developer note:
    // none of these can be noexecpt because the stream can fail
    */
#define STAMP_OUT_FUNDAMENTAL_SERIALIZERS(T)                       \
  template <>                                                      \
  struct Serialize<T> : NonCreatable {                             \
    using type = T;                                                \
                                                                   \
    template <typename InputArchive>                               \
    static inline void load(InputArchive& ar, type& data) {        \
      detail::load_internal(ar, data);                             \
    }                                                              \
                                                                   \
    template <typename OutputArchive>                              \
    static inline void save(OutputArchive& ar, type const& data) { \
      detail::save_internal(ar, data);                             \
    }                                                              \
  }
    /*! \endcond */
    //**************************************************************************

    // you can define a type-list and use macros REPEAT_8
    // to stamp it out too!
    STAMP_OUT_FUNDAMENTAL_SERIALIZERS(bool);
    STAMP_OUT_FUNDAMENTAL_SERIALIZERS(char);
    STAMP_OUT_FUNDAMENTAL_SERIALIZERS(signed char);    // int_8
    STAMP_OUT_FUNDAMENTAL_SERIALIZERS(unsigned char);  // uint8_t
    STAMP_OUT_FUNDAMENTAL_SERIALIZERS(wchar_t);
    STAMP_OUT_FUNDAMENTAL_SERIALIZERS(char16_t);
    STAMP_OUT_FUNDAMENTAL_SERIALIZERS(char32_t);
    STAMP_OUT_FUNDAMENTAL_SERIALIZERS(short);           // int16_t
    STAMP_OUT_FUNDAMENTAL_SERIALIZERS(unsigned short);  // uint16_t
    STAMP_OUT_FUNDAMENTAL_SERIALIZERS(int);             // int32_t
    STAMP_OUT_FUNDAMENTAL_SERIALIZERS(unsigned int);    // uint32_t
    STAMP_OUT_FUNDAMENTAL_SERIALIZERS(long);
    STAMP_OUT_FUNDAMENTAL_SERIALIZERS(unsigned long);       // std::size_t
    STAMP_OUT_FUNDAMENTAL_SERIALIZERS(long long);           // int64_t
    STAMP_OUT_FUNDAMENTAL_SERIALIZERS(unsigned long long);  // uint64_t
    STAMP_OUT_FUNDAMENTAL_SERIALIZERS(float);
    STAMP_OUT_FUNDAMENTAL_SERIALIZERS(double);
    STAMP_OUT_FUNDAMENTAL_SERIALIZERS(long double);

#undef STAMP_OUT_FUNDAMENTAL_SERIALIZERS
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for SizeTag
    // \ingroup serialization
    */
    template <typename T>
    struct Serialize<SizeTag<T>> : NonCreatable {
      using type = SizeTag<T>;

      template <typename InputArchive>
      static inline void load(InputArchive& ar, type& data) {
        ar(data.size);
      }

      template <typename OutputArchive>
      static inline void save(OutputArchive& ar, type const& data) {
        ar(data.size);
      }
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for BinaryData
    // \ingroup serialization
    */
    template <typename T>
    struct Serialize<BinaryData<T>> : NonCreatable {
      using type = BinaryData<T>;

      template <typename InputArchive>
      static inline void load(InputArchive& ar, type& data) {
        detail::load_internal(ar, data);
      }

      template <typename OutputArchive>
      static inline void save(OutputArchive& ar, type const& data) {
        detail::save_internal(ar, data);
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
