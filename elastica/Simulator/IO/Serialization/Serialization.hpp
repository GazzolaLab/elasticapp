//==============================================================================
/*!
//  \file
//  \brief Main serialization classes with interface
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

#include "Simulator/IO/Serialization/SerializeWizard.hpp"
#include "Utilities/CRTP.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits.hpp"

namespace elastica {

  namespace io {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief The base output archive class
    // \ingroup serialization
    //
    // \details
    // Base class archive for all output archives. It provides the expected
    // interface and support functions, and allows extension to different
    // archive formats using the Curiously Recurring Template Pattern (CRTP).
    //
    // This class provides all of the functionality necessary to
    // properly forward data to the correct serialization functions.
    //
    // Individual archives can use a combination of prologue and
    // epilogue functions together with specializations of save, and load to
    // alter the functionality of their serialization.
    //
    // \subsection Customization
    // If you create a custom archive class, it should derive from this, passing
    // itself as a template parameter for the ArchiveType.
    //
       \code
         using namespace elastica::io;
         class MyOwnArchive : public OutputArchive<MyOwnArchive>{
         // implementation details //
         }
       \endcode
    //
    // Each custom output archive should be accompanied by a specialization of
    // the save_internal free function with the following signature.
    //
       \code
         template <typename T>
         void save_internal(MyOwnArchive& ar, T const& t){ // implementation //}
       \endcode
    //
    // \tparam ArchiveType The archive type that derives from OutputArchive
    */
    template <class ArchiveType>
    class OutputArchive : public NonCopyable,
                          public CRTPHelper<ArchiveType, OutputArchive> {
     private:
      //**Type definitions******************************************************
      using CRTP = CRTPHelper<ArchiveType, OutputArchive>;  //<! CRTP Type
      using CRTP::self;                                     //<! CRTP methods
      //************************************************************************

     protected:
      //**Constructors**********************************************************
      /*!\brief Default Constructor for the OutputArchive class.
       */
      OutputArchive() = default;
      //************************************************************************

     public:
      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      ~OutputArchive() = default;
      //@}
      //************************************************************************

     public:
      //**Functor interface*****************************************************
      /*!\brief Serializes all passed in data
      //
      // \details
      // This is the primary interface for serializing data with an archive.
      //
      // \return ArchiveType& the reference to the archive type
      */
      template <class... Args>
      inline ArchiveType& operator()(Args&&... args) {
        EXPAND_PACK_LEFT_TO_RIGHT(self().process(std::forward<Args>(args)));
        return self();
      }
      //************************************************************************

     private:
      //**Serialization processing**********************************************
      /*!\brief Serializes all passed in data
      //
      // \details
      // Serializes \c data based on the downcasted implementation after calling
      // prologue, then calls epilogue.
      //
      // \return void
      */
      template <class T>
      inline void process(T&& data) {
        using ActualType = cpp20::remove_cvref_t<T>;
        SerializePrePost<ActualType>::prologue(self(), data);
        SerializeWizard<ActualType>::save(self(), data);
        SerializePrePost<ActualType>::epilogue(self(), data);
      }
      //************************************************************************
    };
    //**************************************************************************

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief The base input archive class
    // \ingroup serialization
    //
    // \details
    // Base class archive for all input archives. It provides the expected
    // interface and support functions, and allows extension to different
    // archive formats using the Curiously Recurring Template Pattern (CRTP).
    //
    // This class provides all of the functionality necessary to
    // properly forward data to the correct serialization functions.
    //
    // Individual archives can use a combination of prologue and
    // epilogue functions together with specializations of load, and load to
    // alter the functionality of their serialization.
    //
    // \subsection Customization
    // If you create a custom archive class, it should derive from this, passing
    // itself as a template parameter for the ArchiveType.
    //
       \code
         using namespace elastica::io;
         class MyOwnArchive : public InputArchive<MyOwnArchive>{
         // implementation details //
         }
       \endcode
    //
    // Each custom input archive should be accompanied by a specialization of
    // the load_internal free function with the following signature.
    //
       \code
         template <typename T>
         void load_internal(MyOwnArchive& ar, T const& t){ // implementation //}
       \endcode
    //
    // \tparam ArchiveType The archive type that derives from InputArchive
    */
    template <class ArchiveType>
    class InputArchive : public NonCopyable,
                         public CRTPHelper<ArchiveType, InputArchive> {
     private:
      //**Type definitions******************************************************
      using CRTP = CRTPHelper<ArchiveType, InputArchive>;  //<! CRTP Type
      using CRTP::self;                                    //<! CRTP methods
      //************************************************************************

     protected:
      //**Constructors**********************************************************
      /*!\brief Default Constructor for the InputArchive class.
       */
      InputArchive() = default;
      //************************************************************************

     public:
      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      ~InputArchive() = default;
      //@}
      //************************************************************************

     public:
      //**Functor interface*****************************************************
      /*!\brief Serializes all passed in data
      //
      // \details
      // This is the primary interface for serializing data with an archive.
      //
      // \return ArchiveType& the reference to the archive type
      */
      template <class... Types>
      inline ArchiveType& operator()(Types&&... args) {
        EXPAND_PACK_LEFT_TO_RIGHT(self().process(std::forward<Types>(args)));
        return self();
      }
      //************************************************************************

     private:
      //**Serialization processing**********************************************
      /*!\brief Serializes all passed in data
      //
      // \details
      // Serializes \c data based on the downcasted implementation after calling
      // prologue, then calls epilogue.
      //
      // \return void
      */
      template <class T>
      inline void process(T&& data) {
        using ActualType = cpp20::remove_cvref_t<T>;
        SerializePrePost<ActualType>::prologue(self(), data);
        SerializeWizard<ActualType>::load(self(), data);
        SerializePrePost<ActualType>::epilogue(self(), data);
      }
      //************************************************************************
    };

  }  // namespace io

}  // namespace elastica