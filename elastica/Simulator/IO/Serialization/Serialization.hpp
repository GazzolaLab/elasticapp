//==============================================================================
/*!
//  \file
//  \brief Main serialization classes with interface
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
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
     * \ingroup serialization
     *
     * \details
     * Base class archive for all output archives. It provides the expected
     * interface and support functions, and allows extension to different
     * archive formats using the Curiously Recurring Template Pattern (CRTP).
     *
     * This class provides all of the functionality necessary to
     * properly forward data to the correct serialization functions.
     *
     * Individual archives can use a combination of prologue and
     * epilogue functions together with specializations of save, and load to
     * alter the functionality of their serialization.
     *
     * \section serialization_output_customization Customization
     * If you create a custom archive class, it should derive from this, passing
     * itself as a template parameter for the ArchiveType.
     * \code
     *   using namespace elastica::io;
     *   class MyOwnArchive : public OutputArchive<MyOwnArchive>{
     *   // implementation details //
     *   }
     * \endcode
     *
     * Each custom output archive should be accompanied by a specialization of
     * the save_internal free function with the following signature.
     * \code
     *   template <typename T>
     *   void save_internal(MyOwnArchive& ar, T const& t){ // implementation //}
     * \endcode
     *
     * \tparam ArchiveType The archive type that derives from OutputArchive
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
      /*!\name Constructors */
      //@{

      //************************************************************************
      /*!\brief Default Constructor for the OutputArchive class.
       */
      OutputArchive() = default;
      //************************************************************************

      //************************************************************************
      /*!\brief Move constructor for the OutputArchive class.
       */
      OutputArchive(OutputArchive&&) noexcept = default;
      //************************************************************************

      //@}
      //************************************************************************

     public:
      //**Destructor************************************************************
      /*!\name Destructor */
      //@{

      //************************************************************************
      /*!\brief Destructor
       */
      ~OutputArchive() = default;
      //************************************************************************

      //@}
      //************************************************************************

     public:
      //**Functor interface*****************************************************
      /*!\brief Serializes all passed in data
       *
       * \details
       * This is the primary interface for serializing data with an archive.
       *
       * \example
       * \code
       *  int i; float f; double d; std::string s;
       *
       *  auto output_archive = // your favorite archive //;
       *
       *  // save all data
       *  output_archive(i, f, d, s);
       * \endcode
       *
       * \param to_be_serialized parameters to be serialized
       * \return ArchiveType& the reference to the archive type
       */
      template <class... Args>
      inline ArchiveType& operator()(Args&&... to_be_serialized) {
        // Do NOT abort here, the callning scope might be further up the chain
        // and so context will be lost
        EXPAND_PACK_LEFT_TO_RIGHT(
            self().process(std::forward<Args>(to_be_serialized)));

        return self();
      }
      //************************************************************************

     private:
      //**Serialization processing**********************************************
      /*!\brief Serializes all passed in data
       *
       * \details
       * Serializes \c data based on the downcasted implementation after calling
       * prologue, then calls epilogue.
       *
       * \return void
       */
      template <class T>
      inline void process(T&& data) {
        using ActualType = std::remove_cvref_t<T>;
        prologue(self(), data);
        SerializeWizard<ActualType>::save(self(), data);
        epilogue(self(), data);
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
     * \ingroup serialization
     *
     * \details
     * Base class archive for all input archives. It provides the expected
     * interface and support functions, and allows extension to different
     * archive formats using the Curiously Recurring Template Pattern (CRTP).
     *
     * This class provides all of the functionality necessary to
     * properly forward data to the correct serialization functions.
     *
     * Individual archives can use a combination of prologue and
     * epilogue functions together with specializations of load, and load to
     * alter the functionality of their serialization.
     *
     * \section serialization_input_customization Customization
     * If you create a custom archive class, it should derive from this, passing
     * itself as a template parameter for the ArchiveType.
     *
     * \code
     *   using namespace elastica::io;
     *   class MyOwnArchive : public InputArchive<MyOwnArchive>{
     *   // implementation details //
     *   }
     * \endcode
     *
     * Each custom input archive should be accompanied by a specialization of
     * the load_internal free function with the following signature.
     *
     * \code
     *   template <typename T>
     *   void load_internal(MyOwnArchive& ar, T const& t){ // implementation //}
     * \endcode
     *
     * \tparam ArchiveType The archive type that derives from InputArchive
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
      /*!\name Constructors */
      //@{
      //************************************************************************
      /*!\brief Default Constructor for the InputArchive class.
       */
      InputArchive() = default;
      //************************************************************************

      //************************************************************************
      /*!\brief Move constructor for the InputArchive class.
       */
      InputArchive(InputArchive&&) noexcept = default;
      //************************************************************************

      //@}
      //************************************************************************

     public:
      //**Destructor************************************************************
      /*!\name Destructor */
      //@{

      //************************************************************************
      /*!\brief Destructor
       */
      ~InputArchive() = default;
      //************************************************************************

      //@}
      //************************************************************************

     public:
      //**Functor interface*****************************************************
      /*!\brief Serializes all passed in data
       *
       * \details
       * This is the primary interface for serializing data with an archive.
       *
       * \example
       * \code
       *  int i; float f; double d; std::string s;
       *
       *  auto input_archive = // your favorite archive //;
       *
       *  // load all data
       *  input_archive(i, f, d, s);
       * \endcode
       *
       * \param to_be_serialized parameters to be serialized
       * \return ArchiveType& the reference to the archive type
       */
      template <class... Args>
      inline ArchiveType& operator()(Args&&... to_be_serialized) {
        // Do NOT abort here, the calling scope might be further up the chain
        // and so context will be lost

        EXPAND_PACK_LEFT_TO_RIGHT(
            self().process(std::forward<Args>(to_be_serialized)));
        return self();
      }
      //************************************************************************

     private:
      //**Serialization processing**********************************************
      /*!\brief Serializes all passed in data
       *
       * \details
       * Serializes \c data based on the downcasted implementation after calling
       * prologue, then calls epilogue.
       *
       * \return void
       */
      template <class T>
      inline void process(T&& data) {
        using ActualType = std::remove_cvref_t<T>;
        prologue(self(), data);
        SerializeWizard<ActualType>::load(self(), data);
        epilogue(self(), data);
      }
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace io

}  // namespace elastica