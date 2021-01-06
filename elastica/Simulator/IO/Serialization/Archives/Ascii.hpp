//==============================================================================
/*!
//  \file
//  \brief Implementation of a non-portable ascii archive
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
#include <sstream>
#include <stdexcept>

/// Forward types first
#include "Simulator/IO/Serialization/Types.hpp"  // includes archive types
///
#include "Simulator/IO/Serialization/Serialization.hpp"

namespace elastica {

  namespace io {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Output archive for saving data in ascii representation
    // \ingroup serialization
    //
    // AsciiOutputArchive outputs data to a stream in a compact ascii
    // representation with as little extra metadata as possible.
    //
    // This archive does nothing to ensure that the endianness of the saved
    // and loaded data is the same.  If you need to have portability over
    // architectures with different endianness, use
    // PortableAsciiOutputArchive (WIP)
    //
    // When using a ascii archive and a file stream, you must use the
    // std::ios::ascii format flag to avoid having your data altered
    // inadvertently.
    //
    // \example
       \code
           std::stringstream ss; // any stream can be used
           {
             AsciiOutputArchive oarchive(ss); //  Create an output archive

             int i; float f; double d;
             oarchive(i, f, d); // Write the data to the archive
           } // archive goes out of scope, ensuring all contents are flushed
       \endcode
    //
    // \see OutputArchive
    */
    class AsciiOutputArchive : public OutputArchive<AsciiOutputArchive> {
     public:
      //**Constructors**********************************************************
      /*!\brief Sole Constructor for the AsciiOutputArchive class.
      //
      // \param stream The stream to output to. Can be a stringstream, a file
      //         stream, or even cout!
      */
      explicit AsciiOutputArchive(std::ostream& stream)
          : OutputArchive<AsciiOutputArchive>(), stream_(stream) {}
      //************************************************************************

      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      ~AsciiOutputArchive() = default;
      //@}
      //************************************************************************

     private:
      //**Save implementation***************************************************
      /*!\brief Writes size bytes of data to the output stream
      //
      // \param data Pointer to address of data being written
      // \param size Size of data
      // \return void
      //
      // This method implements the ascii save operation and is intended for
      // internal use of \elastica developers only.
      */
      void save_ascii(const void* data, std::streamsize size) {
        auto const written_size =
            stream_.rdbuf()->sputn(reinterpret_cast<const char*>(data), size);

        if (written_size != size)
          throw std::runtime_error("Failed to write " + std::to_string(size) +
                                   " bytes to output stream! Wrote " +
                                   std::to_string(written_size));
      }
      //************************************************************************

      // Give friend access for accessing save functions
      template <typename T>
      friend void save_internal(AsciiOutputArchive&, T const&);

     private:
      //**Member variables******************************************************
      std::ostream& stream_;  // !< Stream to write to
      //************************************************************************
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Input archive for loading data saved using AsciiOutputArchive
    // \ingroup serialization
    //
    // AsciiInputArchive loads data that has been previously saved using
    // AsciiOutputArchive, from am input stream
    //
    // This archive does nothing to ensure that the endianness of the saved
    // and loaded data is the same.  If you need to have portability over
    // architectures with different endianness, use
    // PortableAsciiInputArchive (WIP)
    //
    // When using a ascii archive and a file stream, you must use the
    // std::ios::ascii format flag to avoid having your data altered
    // inadvertently.
    //
    // \example
       \code
           std::stringstream ss; // any stream can be used
           {
             AsciiOutputArchive oarchive(ss); //  Create an output archive

             int i; float f; double d; MyClass m;
             oarchive(i, f, d, m); // Write the data to the archive
           } // archive goes out of scope, ensuring all contents are flushed

           {
             AsciiInputArchive iarchive(ss); //  Create an input archive

             int i; float f; double d; MyClass m;
             iarchive(i, f, d, m); // Read the data from the archive
           }
       \endcode
    //
    // \see InputArchive
    */
    class AsciiInputArchive : public InputArchive<AsciiInputArchive> {
     public:
      //**Constructors**********************************************************
      /*!\brief Sole Constructor for the AsciiInputArchive class.
      //
      // \param stream The stream to load from. Can be a stringstream, a file
      //         stream, or even cin!
      */
      explicit AsciiInputArchive(std::istream& stream)
          : InputArchive<AsciiInputArchive>(), stream_(stream) {}
      //************************************************************************

      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      ~AsciiInputArchive() = default;
      //************************************************************************

     private:
      //**Save implementation***************************************************
      /*!\brief Reads size bytes of data from the input stream
      //
      // \param data Pointer to address of data being loaded
      // \param size Size of data
      // \return void
      //
      // This method implements the ascii load operation and is intended for
      // internal use of \elastica developers only.
      */
      void load_ascii(void* const data, std::streamsize size) {
        auto const readSize =
            stream_.rdbuf()->sgetn(reinterpret_cast<char*>(data), size);

        if (readSize != size)
          throw std::runtime_error("Failed to read " + std::to_string(size) +
                                   " bytes from input stream! Read " +
                                   std::to_string(readSize));
      }
      //************************************************************************

      // Give friend access for accessing load functions
      template <typename T>
      friend void load_internal(AsciiInputArchive&, T&);

     private:
      //**Member variables******************************************************
      std::istream& stream_;  // !< Stream to read from
      //************************************************************************
    };
    //**************************************************************************

    //==========================================================================
    //
    //  Internal serialization functions
    //
    //==========================================================================

    //**Internal save***********************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Saves POD data types into a AsciiArchive
    // \ingroup serialization
    //
    // \param ar The archive that stores the data
    // \param t  The data being stored
    // \return void
    //
    // This method implements the Ascii save operation and is intended for
    // internal use of \elastica developers only.
    */
    template <class T>
    inline void save_internal(AsciiOutputArchive& ar, T const& t) {
      ar.save_ascii(std::addressof(t), sizeof(t));
    }
    /*! \endcond */
    //**************************************************************************

    //**Internal load***********************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Loads POD data types from a AsciiArchive
    // \ingroup serialization
    //
    // \param ar The archive that stores the data
    // \param t  The data being stored
    // \return void
    //
    // This method implements the Ascii load operation and is intended for
    // internal use of \elastica developers only.
    */
    template <class T>
    inline void load_internal(AsciiInputArchive& ar, T& t) {
      ar.load_ascii(std::addressof(t), sizeof(t));
    }
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
