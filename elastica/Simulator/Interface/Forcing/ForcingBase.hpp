//==============================================================================
/*!
//  \file
//  \brief Forcing base class
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
#include <functional>
#include <memory>
#include <string>

///
#include "Simulator/Interface/Forcing/Types.hpp"
///
#include "Simulator/Forcing/Forcing.hpp"
#include "Simulator/Forcing/Types.hpp"
#include "Simulator/Interface/Forcing/ForcerBase.hpp"
#include "Simulator/Interface/Forcing/ForcingUUID.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/NumericAt.hpp"

namespace elastica {

  namespace interface {

    namespace forcing {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Base class for Forcing
       * \ingroup forcing
       *
       * ForcingBase is a helper class to ease the storage of Forcing
       * which has specialized System information, into a uniform pointer
       * vector, in the Forcing add-on in \elastica
       *
       * \tparam ForcingRegistrars Registrars for all forcing
       *
       * \see Forcing
       */
      template <typename ForcingRegistrars>
      class ForcingBase : public NonCopyable {
       protected:
        //**Type definitions****************************************************
        //! Type of the final forcing applicator
        using ForcerID = std::unique_ptr<ForcerBase>;
        //! Type of the Unique ID
        using UUIDType = ForcingUUID;
        //! Type of the index
        using IndexType = typename UUIDType::IndexType;
        //**********************************************************************

        // Friend declaration with the main Forcing class (for finalize)
        template <class SimTraits,  // Simulator options
                  class Sim>        // Simulator class
        friend class Forcing;

        //**Constructors********************************************************
        /*!\name Constructors */
        //@{

        //**********************************************************************
        /*!\brief The sole constructor.
         *
         * \param id An integer ID of the current forcing
         * \param name Name of the current Forcing
         */
        ForcingBase(IndexType id, std::string&& name)
            : forcing_id_(id, std::move(name)),
              concrete_forcing_type_(),
              concrete_system_type_(),
              forcer_generator_(nullptr){};
        //**********************************************************************

        //@}
        //**********************************************************************

       public:
        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        //**********************************************************************
        /*!\brief Destructor
         */
        virtual ~ForcingBase() = default;
        //**********************************************************************
        //@}
        //**********************************************************************

       public:
        //**********************************************************************
        /*!\brief The finalize method.
         *
         * \details
         * The finalize method is the main interface to get the finalized
         * Forcer that will act on systems within \elastica
         */
        virtual ForcerID finalize() const = 0;
        //**********************************************************************

       public:
        //**********************************************************************
        /*!\brief Get ID
         *
         * \return UUID
         *
         * \note
         * Returns by value, doesn't usually matter as its cheap with small
         * string optimization
         */
        UUIDType id() const { return forcing_id_; }
        //**********************************************************************

       protected:
        //**********************************************************************
        /*!\brief Sets the name of the forcing
         *
         * \param name of the forcing
         * \return void
         */
        inline constexpr auto set_name() & noexcept -> std::string& {
          /*
           * Dev note:
           * 1. noexcept because of copy-and-swap
           * 2. This takes in an argument because this is exposed as an
           * interface to the forcing class
           */
          return forcing_id_.name;
        }
        //**********************************************************************

       protected:
        //**********************************************************************
        /*!\brief Set pretty name of the system being constrained
         *
         * \details
         * Explicitly have a "set" in the interface but thats okay since its for
         * internal use only
         *
         * \return Mutable reference to std::string
         */
        inline constexpr auto set_concrete_system_type() & noexcept
            -> std::string& {
          return concrete_system_type_;
        }
        //**********************************************************************

       public:
        //**********************************************************************
        /*!\brief Get pretty name of the type of system being constrained
         *
         * \return std::string
         */
        inline constexpr auto concrete_system_type() const& noexcept
            -> std::string const& {
          return concrete_system_type_;
        }
        //**********************************************************************

       protected:
        //**********************************************************************
        /*!\brief Set pretty name of the type of forcing being applied to the
         * system
         *
         * \details
         * Explicitly have a "set" in the interface but thats okay since its for
         * internal use only
         *
         * \return std::string
         */
        inline constexpr auto set_concrete_forcing_type() & noexcept
            -> std::string& {
          return concrete_forcing_type_;
        }
        //**********************************************************************

       public:
        //**********************************************************************
        /*!\brief Get pretty name of the type of forcing being applied to the
         * system
         *
         * \return std::string
         */
        inline constexpr auto concrete_forcing_type() const& noexcept
            -> std::string const& {
          return concrete_forcing_type_;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Check if the current Forcing has all necessary information
         * for making a forcer
         *
         * \return bool
         */
        virtual bool is_not_valid() const noexcept = 0;
        //**********************************************************************

       protected:
        //**Member variables****************************************************
        //! A unique id for each forcing, for debugging purposes
        UUIDType forcing_id_;
        //! Pretty name of the type of forcing
        std::string concrete_forcing_type_;
        //! Pretty name of the type of system being forced
        std::string concrete_system_type_;
        //! Generator for the final forcer object
        std::function<ForcerID()> forcer_generator_;
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace forcing

  }  // namespace interface

}  // namespace elastica
