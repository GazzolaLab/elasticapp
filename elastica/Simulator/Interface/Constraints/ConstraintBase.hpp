//==============================================================================
/*!
//  \file
//  \brief Constraint base class
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
#include "Simulator/Interface/Constraints/Types.hpp"
///
#include "Simulator/Constraints/Constraint.hpp"
#include "Simulator/Constraints/Types.hpp"
#include "Simulator/Interface/Constraints/ConstrainerBase.hpp"
#include "Simulator/Interface/Constraints/ConstraintUUID.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/NumericAt.hpp"

namespace elastica {

  namespace interface {

    namespace constraints {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Base class for Constraint
       * \ingroup constraints
       *
       * ConstraintBase is a helper class to ease the storage of Constraints
       * which has specialized System information, into a uniform vector based
       * on pointers, in the Constraints add-on in \elastica
       *
       * \tparam ConstraintRegistrars Registrars for all constraints
       *
       * \see Constraint
       */
      template <typename ConstraintRegistrars>
      class ConstraintBase : public NonCopyable {
       protected:
        //**Type definitions****************************************************
        //! Type of the final constraint applicator
        using ConstrainerID = std::unique_ptr<ConstrainerBase>;
        //! Type of the Unique ID
        using UUIDType = ConstraintUUID;
        //! Type of the index
        using IndexType = typename UUIDType::IndexType;
        //**********************************************************************

        // Friend declaration with the main Constraints class (for finalize)
        template <class SimTraits,  // Simulator options
                  class Sim>        // Simulator class
        friend class Constraints;

        //**Constructors********************************************************
        /*!\name Constructors */
        //@{

        //**********************************************************************
        /*!\brief The sole constructor.
         *
         * \param id An integer ID of the current constraint
         * \param name Name of the current Constraint
         */
        ConstraintBase(IndexType id, std::string&& name)
            : constraint_id_(id, std::move(name)),
              concrete_constraint_type_(),
              concrete_system_type_(),
              constrainer_generator_(nullptr){};
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
        virtual ~ConstraintBase() = default;
        //**********************************************************************
        //@}
        //**********************************************************************

       public:
        //**********************************************************************
        /*!\brief The finalize method.
         *
         * \details
         * The finalize method is the main interface to get the finalized
         * Constrainer that will act on systems within \elastica
         */
        virtual ConstrainerID finalize() const = 0;
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
        UUIDType id() const { return constraint_id_; }
        //**********************************************************************

       protected:
        //**********************************************************************
        /*!\brief Sets the name of the constraint
         *
         * \param name of the constraint
         * \return void
         */
        inline constexpr auto set_name() & noexcept -> std::string& {
          /*
           * Dev note:
           * 1. noexcept because of copy-and-swap
           * 2. This takes in an argument because this is exposed as an
           * interface to the constraint class
           */
          return constraint_id_.name;
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
        /*!\brief Set pretty name of the type of constraint being applied to the
         * system
         *
         * \details
         * Explicitly have a "set" in the interface but thats okay since its for
         * internal use only
         *
         * \return std::string
         */
        inline constexpr auto set_concrete_constraint_type() & noexcept
            -> std::string& {
          return concrete_constraint_type_;
        }
        //**********************************************************************

       public:
        //**********************************************************************
        /*!\brief Get pretty name of the type of constraint being applied to the
         * system
         *
         * \return std::string
         */
        inline constexpr auto concrete_constraint_type() const& noexcept
            -> std::string const& {
          return concrete_constraint_type_;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Check if the current Constraint has all necessary information
         * for making a constrainer
         *
         * \return bool
        */
        virtual bool is_not_valid() const noexcept = 0;
        //**********************************************************************

       protected:
        //**Member variables****************************************************
        //! A unique id for each constraint, for debugging purposes
        UUIDType constraint_id_;
        //! Pretty name of the type of constraint
        std::string concrete_constraint_type_;
        //! Pretty name of the type of system being constrained
        std::string concrete_system_type_;
        //! Generator for the final constrainer object
        std::function<ConstrainerID()> constrainer_generator_;
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace constraints

  }  // namespace interface

}  // namespace elastica
