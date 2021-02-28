//==============================================================================
/*!
//  \file
//  \brief A helper class for logging
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

#include <new>
#include <sstream>

///
#include "Utilities/Logging/Types.hpp"
///
#include "Utilities/Logging/LoggingLevel.hpp"

namespace elastica {

  namespace logging {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief A section of logging
     * \ingroup logging
     *
     * \details
     * A LoggingSection implements a single section of the logging utility. It
     * is responsible for taking in an input streamable and passing it (after
     * proper checking, reformatting) to the Logger class.
     *
     * \see log(), Logger
     */
    class LoggingSection {
     public:
      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{

      //************************************************************************
      /*!\brief Level constructor.
       *
       * \details
       * Allows implicit conversion of level to a LoggingSection for use in
       * Singleton Macros
       *
       * \param level The level of logging
       */
      /* Clang-Tidy: Single-argument constructors must be marked explicit to
       * avoid unintentional implicit conversions */
      LoggingSection(LoggingLevel level);  // NOLINT
      //************************************************************************

      //************************************************************************
      /*!\brief Copy constructor.
       *
       */
      inline LoggingSection(const LoggingSection& ls)
          : logging_level_(ls.logging_level_) {}
      //************************************************************************

      //************************************************************************
      /*!\brief Move constructor.
       *
       */
      inline LoggingSection(LoggingSection&& ls) noexcept
          : logging_level_(ls.logging_level_),
            message_(std::move(ls.message_)) {}
      //************************************************************************

      //@}
      //************************************************************************

      //**Assignment Operators**************************************************
      /*!\name Assignment Operators
       * Deleted assignments to prevent the user explicitly calling a
       * LoggingSection
       */
      //@{
      LoggingSection& operator=(const LoggingSection& ls) = delete;
      LoggingSection& operator=(LoggingSection&& ls) = delete;
      //@}
      //************************************************************************

      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      ~LoggingSection();
      //@}
      //************************************************************************

      //************************************************************************
      /*!\brief Conversion operator to `bool`.
       *
       * \details
       * The conversion operator always returns `true` to indicate that the
       * logging section is active. It finds use in macros where the
       * LoggingSection is created in the predicate of an if condition, like so
       * if (LoggingSection log = debug){...
       *
       * where we need the if to always evaluate to true
       */
      /* Clang-Tidy: 'operator bool' must be marked explicit to avoid
       * unintentional implicit conversions
       */
      inline operator bool() const noexcept { return true; }  // NOLINT
      //************************************************************************

      //************************************************************************
      /*!\brief The logging entry-point
       *
       * \example
       * \snippet TODO
       *
       * \param message The message to be logged. Should be streamable.
       */
      template <typename Type>  // Type of the log message
      inline auto log(Type&& message) noexcept -> void {
        message_ << message;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Commits the log(message), when finished, to the underlying
       * Logger
       */
      auto commit() -> void;
      //************************************************************************

      //**Memory allocation operators*******************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\name Memory allocation Operators
       * Disable dynamic memory allocation for LoggingSection objects using the
       * Concrete Data Type idiom :
       * https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Concrete_Data_Type
       */
      void* operator new(std::size_t count) = delete;
      void* operator new[](std::size_t count) = delete;
      void* operator new(std::size_t count,
                         const std::nothrow_t& tag) noexcept = delete;
      void* operator new[](std::size_t count,
                           const std::nothrow_t& tag) noexcept = delete;
      void* operator new(std::size_t count, void* ptr) = delete;
      void* operator new[](std::size_t count, void* ptr) = delete;
      void operator delete(void* ptr) noexcept = delete;
      void operator delete[](void* ptr) noexcept = delete;
      void operator delete(void* ptr, std::size_t size) noexcept = delete;
      void operator delete[](void* ptr, std::size_t size) noexcept = delete;
      void operator delete(void* ptr,
                           const std::nothrow_t& tag) noexcept = delete;
      void operator delete[](void* ptr,
                             const std::nothrow_t& tag) noexcept = delete;
      void operator delete(void* ptr, void* place) noexcept = delete;
      void operator delete[](void* ptr, void* place) noexcept = delete;
      /*! \endcond */
      //************************************************************************

     private:
      //**Member variables******************************************************
      //! The level of logging
      LoggingLevel logging_level_;
      //! Intermediate buffer for storing log messages
      std::stringstream message_;
      //************************************************************************
    };
    //**************************************************************************

    //==========================================================================
    //
    //  FREE FUNCTIONS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Stream operator for LoggingSection
     *
     * \param log_section The section class
     * \param message The message to log
     * \return log_section
     */
    template <typename Type>  // Type of the log message
    inline LoggingSection& operator<<(LoggingSection& log_section,
                                      Type&& message) {
      log_section.log(std::forward<Type>(message));
      return log_section;
    }
    //**************************************************************************

  }  // namespace logging

}  // namespace elastica
