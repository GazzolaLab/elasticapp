//==============================================================================
/*!
//  \file
//  \brief Implementation of a non-portable binary archive
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
#include <sstream>
#include <stdexcept>

/// Forward types first
#include "Simulator/IO/Serialization/Types.hpp"  // includes archive types
///
#include "Simulator/IO/Serialization/Serialization.hpp"
//
#include "Utilities/Requires.hpp"

namespace elastica {

  namespace io {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Output archive for saving data in binary representation
    // \ingroup serialization
    //
    // BinaryOutputArchive outputs data to a stream in a compact binary
    // representation with as little extra metadata as possible.
    //
    // This archive does nothing to ensure that the endianness of the saved
    // and loaded data is the same.  If you need to have portability over
    // architectures with different endianness, use
    // PortableBinaryOutputArchive (WIP)
    //
    // When using a binary archive and a file stream, you must use the
    // std::ios::binary format flag to avoid having your data altered
    // inadvertently.
    //
    // \example
    // \snippet Serialization/Test_Serialize.cpp binary_archive
    //
    // \see OutputArchive, BinaryInputArchive
    */
    class BinaryOutputArchive : public OutputArchive<BinaryOutputArchive> {
     public:
      //**Constructors**********************************************************
      /*!\brief Sole Constructor for the BinaryOutputArchive class.
      //
      // \param stream The stream to output to. Can be a stringstream, a file
      //         stream, or even cout!
      */
      explicit BinaryOutputArchive(std::ostream& stream)
          : OutputArchive<BinaryOutputArchive>(), stream_(stream) {}
      //************************************************************************

      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      /*!\brief Destructor
       */
      ~BinaryOutputArchive() = default;
      //@}
      //************************************************************************

     public:
      //**Save implementation***************************************************
      /*!\brief Writes size bytes of data to the output stream
      //
      // \param data Pointer to address of data being written
      // \param size Size of data
      // \return void
      //
      // This method implements the binary save operation and is intended for
      // internal use of \elastica developers only.
      */
      void save_binary(const void* data, std::streamsize size) {
        auto const written_size =
            stream_.rdbuf()->sputn(reinterpret_cast<const char*>(data), size);

        if (written_size != size)
          throw std::runtime_error("Failed to write " + std::to_string(size) +
                                   " bytes to output stream! Wrote " +
                                   std::to_string(written_size));
      }
      //************************************************************************

      // Give friend access for accessing save functions
      // template <typename T>
      // friend void detail::save_internal(BinaryOutputArchive&, T const&);
      // template <typename T>
      // friend void detail::save_internal(BinaryOutputArchive&,
      //                                   BinaryData<T> const&);

     private:
      //**Member variables******************************************************
      std::ostream& stream_;  // !< Stream to write to
      //************************************************************************
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Input archive for loading data saved using BinaryOutputArchive
    // \ingroup serialization
    //
    // BinaryInputArchive loads data that has been previously saved using
    // BinaryOutputArchive, from am input stream
    //
    // This archive does nothing to ensure that the endianness of the saved
    // and loaded data is the same.  If you need to have portability over
    // architectures with different endianness, use
    // PortableBinaryInputArchive (WIP)
    //
    // When using a binary archive and a file stream, you must use the
    // std::ios::binary format flag to avoid having your data altered
    // inadvertently.
    //
    // \example
    // \snippet Serialization/Test_Serialize.cpp binary_archive
    //
    // \see InputArchive, BinaryOutputArchive
    */
    class BinaryInputArchive : public InputArchive<BinaryInputArchive> {
     public:
      //**Constructors**********************************************************
      /*!\brief Sole Constructor for the BinaryInputArchive class.
      //
      // \param stream The stream to load from. Can be a stringstream, a file
      //         stream, or even cin!
      */
      explicit BinaryInputArchive(std::istream& stream)
          : InputArchive<BinaryInputArchive>(), stream_(stream) {}
      //************************************************************************

      //**Destructor************************************************************
      /*!\brief Destructor
       */
      ~BinaryInputArchive() = default;
      //************************************************************************

     public:
      //**Save implementation***************************************************
      /*!\brief Reads size bytes of data from the input stream
      //
      // \param data Pointer to address of data being loaded
      // \param size Size of data
      // \return void
      //
      // This method implements the binary load operation and is intended for
      // internal use of \elastica developers only.
      */
      void load_binary(void* const data, std::streamsize size) {
        auto const readSize =
            stream_.rdbuf()->sgetn(reinterpret_cast<char*>(data), size);

        if (readSize != size)
          throw std::runtime_error("Failed to read " + std::to_string(size) +
                                   " bytes from input stream! Read " +
                                   std::to_string(readSize));
      }
      //************************************************************************

      // Give friend access for accessing load functions
      // template <typename T>
      // friend void detail::load_internal(BinaryInputArchive&, T&);
      // template <typename T>
      // friend void detail::load_internal(BinaryInputArchive&, BinaryData<T>&);

     private:
      //**Member variables******************************************************
      std::istream& stream_;  // !< Stream to read from
      //************************************************************************
    };
    //**************************************************************************

    namespace detail {

      //========================================================================
      //
      //  Internal serialization functions
      //
      //========================================================================

      //**Internal save*********************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Saves POD data types into a BinaryArchive
      // \ingroup serialization
      //
      // \param ar The archive that stores the data
      // \param t  The data being stored
      // \return void
      //
      // This method implements the Binary save operation and is intended for
      // internal_dispatch use of \elastica developers only.
      */
      template <class T, Requires<std::is_arithmetic<T>::value> = nullptr>
      inline void save_internal_dispatch(BinaryOutputArchive& ar, T const& t) {
        ar.save_binary(std::addressof(t), sizeof(t));
      }
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Saves BinaryData data types into a BinaryArchive
      // \ingroup serialization
      //
      // \details
      // Binary data can be efficiently serialized instead of looping over
      // elements, and so we have an overloaded method
      //
      // \param ar The archive that stores the data
      // \param t  The BinaryData being stored
      // \return void
      //
      // This method implements the Binary save operation and is intended for
      // internal_dispatch use of \elastica developers only.
      */
      template <class T>
      inline void save_internal_dispatch(BinaryOutputArchive& ar,
                                         BinaryData<T> const& t) {
        ar.save_binary(t.data, static_cast<std::streamsize>(t.size));
      }
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <class T>
      inline void save_internal_dispatch(BinaryOutputArchive& ar,
                                         SizeTag<T> const& data) {
        save_internal_dispatch(ar, data.size);
      }
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <class T>
      inline void save_internal_dispatch(BinaryOutputArchive& ar,
                                         NameValuePair<T> const& data) {
        ar(data.value);
      }
      /*! \endcond */
      //************************************************************************

      //**Internal load*********************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Loads POD data types from a BinaryArchive
      // \ingroup serialization
      //
      // \param ar The archive that loads the data
      // \param t  The data being loaded
      // \return void
      //
      // This method implements the Binary load operation and is intended for
      // internal_dispatch use of \elastica developers only.
      */
      template <class T, Requires<std::is_arithmetic<T>::value> = nullptr>
      inline void load_internal_dispatch(BinaryInputArchive& ar, T& t) {
        ar.load_binary(std::addressof(t), sizeof(t));
      }
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Loads BinaryData data types from a BinaryArchive
      // \ingroup serialization
      //
      // \details
      // Binary data can be efficiently serialized instead of looping over
      // elements, and so we have an overloaded method
      //
      // \param ar The archive that loads the data
      // \param t  The data being loaded
      // \return void
      //
      // This method implements the Binary load operation and is intended for
      // internal_dispatch use of \elastica developers only.
      */
      template <class T>
      inline void load_internal_dispatch(BinaryInputArchive& ar,
                                         BinaryData<T>& t) {
        ar.load_binary(t.data, static_cast<std::streamsize>(t.size));
      }
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <class T>
      inline void load_internal_dispatch(BinaryInputArchive& ar,
                                         SizeTag<T> const& data) {
        load_internal_dispatch(ar, data.size);
      }
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <class T>
      inline void load_internal_dispatch(BinaryInputArchive& ar,
                                         NameValuePair<T> const& data) {
        ar(data.value);
      }
      /*! \endcond */
      //************************************************************************

      template <class T>
      inline void save_internal(BinaryOutputArchive& ar, T const& t) {
        save_internal_dispatch(ar, t);
      }

      template <class T>
      inline void load_internal(BinaryInputArchive& ar, T& t) {
        load_internal_dispatch(ar, t);
      }

    }  // namespace detail

  }  // namespace io

}  // namespace elastica
