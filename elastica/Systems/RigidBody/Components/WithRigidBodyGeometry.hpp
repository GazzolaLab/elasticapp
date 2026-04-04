//==============================================================================
/*!
//  \file
//  \brief Geometry component of @ref rigid_body
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
#include <cstdint>  // for size_t
#include <utility>  // for move

#include "Systems/Block.hpp"
//
#include "Systems/RigidBody/Components/Initialization.hpp"
#include "Systems/RigidBody/Components/Tags.hpp"
#include "Systems/RigidBody/Components/Types.hpp"
//
#include "Utilities/CRTP.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace rigid_body {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Variables corresponding to a rigid body geometry
     * \ingroup rigid_body_component
     *
     * \details
     * WithRigidBodyGeometryVariables contains the definitions of variables
     * used within the Blocks framework for the geometry of rigid body
     * data-structure
     *
     * \tparam RBT A valid Rigid body Traits class
     */
    template <typename RBT>
    class WithRigidBodyGeometryVariables {
     private:
      //**Type definitions******************************************************
      //! Traits type
      using Traits = RBT;
      //************************************************************************

     protected:
      //**Variable definitions**************************************************
      /*!\name Variable definitions*/
      //@{

      //************************************************************************
      /*!\brief Variable marking Positions
       */
      struct Position : public Traits::template RigidBodyInitializedVariable<
                            ::elastica::tags::Position,  //
                            typename Traits::DataType::Vector> {};
      //************************************************************************

      //************************************************************************
      /*!\brief Variable marking Director(Q)
       */
      struct Director : public Traits::template RigidBodyInitializedVariable<
                            ::elastica::tags::Director,  //
                            typename Traits::DataType::Matrix> {};
      //********************************************************************

      //@}
      //************************************************************************

      //**Type definitions******************************************************
      //! List of computed variables
      using ComputedVariables = tmpl::list<>;
      //! List of initialized variables
      using InitializedVariables = tmpl::list<Position, Director>;
      //! List of all variables
      using Variables = tmpl::append<InitializedVariables, ComputedVariables>;
      //************************************************************************
    };
    //**************************************************************************

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Component corresponding to a rigid body geometry
     * \ingroup rigid_body_component
     *
     * \details
     * WithRigidBodyGeometry implements geometrical quantities
     * for use within RigidBodies implemented in the Blocks framework.
     *
     * \tparam RBT A valid Rigid Body Traits class
     * \tparam ComputationalBlock The final block which is derived from the
     * current component
     */
    template <typename RBT, typename ComputationalBlock>
    class WithRigidBodyGeometry
        : public CRTPHelper<ComputationalBlock, WithRigidBodyGeometry>,
          public WithRigidBodyGeometryVariables<RBT> {
     private:
      //**Type definitions******************************************************
      //! Traits type
      using Traits = RBT;
      //! This type
      using This = WithRigidBodyGeometry<Traits, ComputationalBlock>;
      //! Type of Variable definitions
      using VariableDefinitions = WithRigidBodyGeometryVariables<Traits>;
      //! CRTP Type
      using CRTP = CRTPHelper<ComputationalBlock, WithRigidBodyGeometry>;
      //! Index type
      using index_type = typename Traits::index_type;
      //************************************************************************

     protected:
      //**Type definitions******************************************************
      //! List of computed variables
      using typename VariableDefinitions::ComputedVariables;
      //! List of initialized variables
      using typename VariableDefinitions::InitializedVariables;
      //! List of all variables
      using typename VariableDefinitions::Variables;

      //! Current Variable definitions
      using typename VariableDefinitions::Director;
      using typename VariableDefinitions::Position;
      //************************************************************************

      //************************************************************************
      /*!\copydoc elastica::cosserat_rod::ComponentInitializationDocStub
       */
      template <typename BlockLike, typename RigidBodyInitializer>
      static inline auto initialize(
          WithRigidBodyGeometry<RBT, BlockLike>& this_component,
          RigidBodyInitializer&& initializer) -> void {
        initialize_component<typename This::InitializedVariables>(
            this_component.self(),
            std::forward<RigidBodyInitializer>(initializer));
      }
      //************************************************************************

     public:
      //**CRTP method***********************************************************
      /*!\name CRTP method*/
      //@{
      using CRTP::self;
      //@}
      //************************************************************************

      //**Get methods***********************************************************
      /*!\name Get methods*/
      //@{

      //************************************************************************
      /*!\name Position methods*/
      //@{
      /*!\brief Gets position of the current rod
       */
      inline constexpr decltype(auto) get_position() & noexcept {
        return blocks::get<tags::Position>(self());
      }
      inline constexpr decltype(auto) get_position() const& noexcept {
        return blocks::get<tags::Position>(self());
      }
      inline constexpr decltype(auto) get_position(index_type idx) & noexcept {
        return Position::slice(get_position(), idx);
      }
      inline constexpr decltype(auto) get_position(
          index_type idx) const& noexcept {
        return Position::slice(get_position(), idx);
      }
      //@}
      //************************************************************************

      //************************************************************************
      /*!\name Director methods*/
      //@{
      /*!\brief Gets director of the current rod
       */
      inline constexpr decltype(auto) get_director() & noexcept {
        return blocks::get<tags::Director>(self());
      }
      inline constexpr decltype(auto) get_director() const& noexcept {
        return blocks::get<tags::Director>(self());
      }
      inline constexpr decltype(auto) get_director(index_type idx) & noexcept {
        return Director::slice(get_director(), idx);
      }
      inline constexpr decltype(auto) get_director(
          index_type idx) const& noexcept {
        return Director::slice(get_director(), idx);
      }
      //@}
      //************************************************************************

      //@}
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace rigid_body

}  // namespace elastica