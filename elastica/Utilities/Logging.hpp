//==============================================================================
/*!
//  \file
//  \brief The logging module of \elastica
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
#include <iostream>  // for tee, but is not a good dependency to have

#include "Utilities/Logging/LoggingLevel.hpp"
#include "Utilities/Logging/LoggingSection.hpp"
#include "Utilities/Requires.hpp"
#include "Utilities/TMPL.hpp"

//==============================================================================
//
//  DOXYGEN DOCUMENTATION
//
//==============================================================================

//******************************************************************************
/*!\defgroup logging Logging
 *
 * The logging module contains a lightweight logger (without additional fluff)
 */
//******************************************************************************

namespace elastica {

  /*!\brief Logging utilities
   * \ingroup logging
   */
  namespace logging {}

}  // namespace elastica

namespace elastica {

  namespace logging {

#define LOG_INACTIVE_SECTION(NAME)               \
  if (elastica::logging::log_level >=            \
      elastica::logging::LoggingLevel::inactive) \
    if (elastica::logging::LoggingSection NAME = \
            elastica::logging::LoggingLevel::inactive)

#define LOG_ERROR_SECTION(NAME)                                               \
  if (elastica::logging::log_level >= elastica::logging::LoggingLevel::error) \
    if (elastica::logging::LoggingSection NAME =                              \
            elastica::logging::LoggingLevel::error)

#define LOG_WARNING_SECTION(NAME)                \
  if (elastica::logging::log_level >=            \
      elastica::logging::LoggingLevel::warning)  \
    if (elastica::logging::LoggingSection NAME = \
            elastica::logging::LoggingLevel::warning)

#define LOG_INFO_SECTION(NAME)                                               \
  if (elastica::logging::log_level >= elastica::logging::LoggingLevel::info) \
    if (elastica::logging::LoggingSection NAME =                             \
            elastica::logging::LoggingLevel::info)

#define LOG_DEBUG_SECTION(NAME)                                               \
  if (elastica::logging::log_level >= elastica::logging::LoggingLevel::debug) \
    if (elastica::logging::LoggingSection NAME =                              \
            elastica::logging::LoggingLevel::debug)

    //**************************************************************************
    /*!\brief Log a function object to streams
     *
     * \param f       Functor object to log
     * \param stream  The first stream to log onto
     * \param streams The other streams to log onto
     *
     * \see log()
     */
    template <typename F, typename Stream, typename... Streams>
    inline auto log_to_stream(F&& f, Stream& stream,
                              Streams&... streams) noexcept -> void {
      // what are the semantics here if f returns?
      expand_pack(std::forward<F>(f)(stream), std::forward<F>(f)(streams)...);
    }
    //**************************************************************************

    namespace detail {

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Log a function object to streams
       *
       * \details
       * We split it into an interface function to save on the number of class
       * template instantiations
       *
       * \tparam L The LoggingLevel to log
       */
      template <LoggingLevel L, bool B = to_be_logged<L>()>
      struct LogInterface;
      /*! \endcond */
      //************************************************************************

      namespace {
        constexpr bool needs_to_be_logged = true;
        constexpr bool need_not_be_logged = false;
      }  // namespace

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Log a function object to streams
       *
       * \details
       * Specialization for when a logging level meets the criteria to be
       * logged.
       *
       * \tparam L The LoggingLevel to log
       */
      template <LoggingLevel L>
      struct LogInterface<L, needs_to_be_logged> {
        template <typename F, typename... Streams>
        inline auto operator()(F&& f, const Streams&... streams) const noexcept
            -> void {
          elastica::logging::LoggingSection log_section(L);
          log_to_stream(std::forward<F>(f), log_section, streams...);
        }
      };
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Log a function object to streams
       *
       * \details
       * No-op specialization for when a logging level does not meet the
       * criteria to be logged.
       *
       * \tparam L The LoggingLevel to log
       */
      template <LoggingLevel L>
      struct LogInterface<L, need_not_be_logged> {
        template <typename F, typename... Streams>
        inline auto operator()(F&& /*f*/,
                               const Streams&... /*streams*/) const noexcept
            -> void {}
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace detail

    //**************************************************************************
    /*!\brief Log a function object to streams
     *
     * \tparam L The LoggingLevel to log
     *
     * \param f     The function object to log
     * \param funcs The other function objects to log
     *
     * \see log_to_stream(), tee()
     */
    template <LoggingLevel L, typename F, typename... Funcs>
    inline auto log(F&& f, Funcs&&... funcs) noexcept -> void {
      detail::LogInterface<L> logger;
      EXPAND_PACK_LEFT_TO_RIGHT(logger(std::forward<F>(f)),
                                logger(std::forward<Funcs>(funcs)));
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief `Tee` a function object, aka log to both stdout and a log stream
     *
     * \tparam L The LoggingLevel to log
     *
     * \param f     The function object to log
     * \param funcs The other function objects to log
     *
     * \see log()
     */
    template <LoggingLevel L, typename F, typename... Funcs>
    inline auto tee(F&& f, Funcs&&... funcs) noexcept -> void {
      detail::LogInterface<L> logger;
      EXPAND_PACK_LEFT_TO_RIGHT(logger(std::forward<F>(f), std::cout),
                                logger(std::forward<Funcs>(funcs), std::cout));
    }
    //**************************************************************************

  }  // namespace logging

}  // namespace elastica
