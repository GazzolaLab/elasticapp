//==============================================================================
/*!
//  \file
//  \brief Kinematics component of @ref rigid_body
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
#include <cstdint>  // for size_t
#include <utility>  // for move

#include "Simulator/Materials.hpp"
//
#include "Systems/Block.hpp"
//
#include "Systems/common/Components.hpp"
//
#include "Systems/RigidBody/Components/Initialization.hpp"
#include "Systems/RigidBody/Components/Tags.hpp"
#include "Systems/RigidBody/Components/Types.hpp"
#include "Systems/common/Cardinality.hpp"
//
#include "Time/SimulationTime.hpp"
//
#include "Utilities/CRTP.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace rigid_body {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Variables corresponding to a rigid body kinematics
     * \ingroup rigid_body_component
     *
     * \details
     * WithRigidBodyKinematicsVariables contains the definitions of variables
     * used within the Blocks framework for the kinematics of rigid body
     * data-structure
     *
     * \tparam RBT A valid Rigid body Traits class
     */
    template <typename RBT>
    class WithRigidBodyKinematicsVariables {
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
      /*!\brief Variable marking velocity
       */
      struct Velocity : public Traits::template RigidBodyInitializedVariable<
                            ::elastica::tags::Velocity,  //
                            typename Traits::DataType::Vector> {};
      //************************************************************************

      //************************************************************************
      /*!\brief Variable marking angular velocity
       */
      struct AngularVelocity
          : public Traits::template RigidBodyInitializedVariable<
                ::elastica::tags::AngularVelocity,  //
                typename Traits::DataType::Vector> {};
      //************************************************************************

      //************************************************************************
      /*!\brief Variable marking acceleration
       */
      struct Acceleration
          : public Traits::template RigidBodyInitializedVariable<
                ::elastica::tags::Acceleration,  //
                typename Traits::DataType::Vector> {};
      //************************************************************************

      //************************************************************************
      /*!\brief Variable marking angular acceleration
       */
      struct AngularAcceleration
          : public Traits::template RigidBodyInitializedVariable<
                ::elastica::tags::AngularAcceleration,  //
                typename Traits::DataType::Vector> {};
      //************************************************************************

      //************************************************************************
      /*!\brief Variable marking external loads
       */
      struct ExternalLoads : public Traits::template RigidBodyVariable<
                                 ::elastica::tags::ExternalLoads,  //
                                 typename Traits::DataType::Vector> {};
      //************************************************************************

      //************************************************************************
      /*!\brief Variable marking external loads
       */
      struct ExternalTorques : public Traits::template RigidBodyVariable<
                                   ::elastica::tags::ExternalTorques,  //
                                   typename Traits::DataType::Vector> {};
      //************************************************************************

      //************************************************************************
      /*!\brief Variable marking material
       */
      struct Material : public Traits::template RigidBodyInitializedVariable<
                            ::elastica::tags::Material,  //
                            typename Traits::DataType::Index> {};
      //************************************************************************

      //************************************************************************
      /*!\brief Variable marking scalar mass
       */
      struct Mass : public Traits::template RigidBodyVariable<
                        ::elastica::tags::Mass,  //
                        typename Traits::DataType::Scalar> {};
      //************************************************************************

      //************************************************************************
      /*!\brief Variable marking external loads
       */
      struct InvMassSecondMomentOfInertia
          : public Traits::template RigidBodyVariable<
                ::elastica::tags::InvMassSecondMomentOfInertia,  //
                typename Traits::DataType::Matrix> {};
      //************************************************************************

      //@}
      //************************************************************************

      //**Type definitions******************************************************
      //! List of computed variables
      using ComputedVariables = tmpl::list<ExternalLoads, ExternalTorques, Mass,
                                           InvMassSecondMomentOfInertia>;
      //! List of initialized variables
      using InitializedVariables =
          tmpl::list<Velocity, AngularVelocity, Acceleration,
                     AngularAcceleration, Material>;
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
    /*!\brief Component corresponding to a rigid body kinematics
     * \ingroup rigid_body_component
     *
     * \details
     * WithRigidBodyKinematics implements geometrical quantities
     * for use within RigidBodies implemented in the Blocks framework.
     *
     * \tparam RBT A valid Rigid Body Traits class
     * \tparam ComputationalBlock The final block which is derived from the
     * current component
     */
    template <typename RBT, typename ComputationalBlock>
    class WithRigidBodyKinematics
        : public CRTPHelper<ComputationalBlock, WithRigidBodyKinematics>,
          public WithRigidBodyKinematicsVariables<RBT>,
          public ::elastica::KinematicsComponent<
              WithRigidBodyKinematics<RBT, ComputationalBlock>> {
     private:
      //**Type definitions******************************************************
      //! Traits type
      using Traits = RBT;
      //! This type
      using This = WithRigidBodyKinematics<Traits, ComputationalBlock>;
      //! Type of Variable definitions
      using VariableDefinitions = WithRigidBodyKinematicsVariables<Traits>;
      //! CRTP Type
      using CRTP = CRTPHelper<ComputationalBlock, WithRigidBodyKinematics>;
      //! Real number type
      using real_type = typename Traits::real_type;
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
      using typename VariableDefinitions::Acceleration;
      using typename VariableDefinitions::AngularAcceleration;
      using typename VariableDefinitions::AngularVelocity;
      using typename VariableDefinitions::ExternalLoads;
      using typename VariableDefinitions::ExternalTorques;
      using typename VariableDefinitions::InvMassSecondMomentOfInertia;
      using typename VariableDefinitions::Mass;
      using typename VariableDefinitions::Material;
      using typename VariableDefinitions::Velocity;
      //************************************************************************

      //********************************************************************
      /*!\copydoc ComponentInitializationDocStub
       */
      template <typename BlockLike, typename RigidBodyInitializer>
      static void initialize(
          WithRigidBodyKinematics<Traits, BlockLike>& this_component,
          RigidBodyInitializer&& initializer) {
        // 1. Initialize required variables
        initialize_component<
            typename VariableDefinitions::InitializedVariables>(
            this_component.self(),
            std::forward<RigidBodyInitializer>(initializer));

        constexpr std::size_t at_only_index = UnitCardinality::index();

        /* mass second moment of inertia */
        {
          using Variable = InvMassSecondMomentOfInertia;
          using Tag = blocks::parameter_t<Variable>;

          auto&& variable(blocks::get<Tag>(this_component.self()));

          // already initialized
          auto&& material(blocks::get<tags::Material>(this_component.self()));

          // (I_1, I_2, I_3)
          Variable::slice(variable, UnitCardinality::index()) = real_type(0.0);
          // in lagrangian coordinates
          const ::elastica::Vec3 I_at_only_index =
              1.0 /
              (this_component.self().get_unscaled_moment_of_inertia(
                   at_only_index) *
               ::elastica::Material::get_density(
                   Traits::DataType::Index::slice(material, at_only_index)));
          Variable::diagonal_assign(variable, at_only_index, I_at_only_index);
        }

        /* mass computation */
        /* \todo
         * Mass can be computed from geometry. Here we fill in the
         * actual mass, considering the volume of the rigid bodies*/
        {
          auto&& mass(
              blocks::get<::elastica::tags::Mass>(this_component.self()));
          auto&& material(this_component.self().get_material());

          Mass::slice(mass, at_only_index) =
              ::elastica::Material::get_density(
                  This::Material::slice(material, at_only_index)) *
              this_component.self().get_volume(at_only_index);
        }

        /* Reset forces and torques */
        {
          auto&& external_loads(blocks::get<::elastica::tags::ExternalLoads>(
              this_component.self()));
          auto&& external_torques(
              blocks::get<::elastica::tags::ExternalTorques>(
                  this_component.self()));
          external_loads = 0.0;
          external_torques = 0.0;
        }
      }
      //************************************************************************

     public:
      //**CRTP method***********************************************************
      /*!\name CRTP method*/
      //@{
      using CRTP::self;
      //@}
      //************************************************************************

#define STAMP_GETTERS(Var, func)                                         \
  inline constexpr decltype(auto) func()& noexcept {                     \
    return blocks::get<::elastica::tags::Var>(self());                   \
  }                                                                      \
  inline constexpr decltype(auto) func() const& noexcept {               \
    return blocks::get<::elastica::tags::Var>(self());                   \
  }                                                                      \
  inline constexpr decltype(auto) func(index_type idx)& noexcept {       \
    return Var::slice(func(), idx);                                      \
  }                                                                      \
  inline constexpr decltype(auto) func(index_type idx) const& noexcept { \
    return Var::slice(func(), idx);                                      \
  }

      //**Get methods***********************************************************
      /*!\name Get methods*/
      //@{

      //************************************************************************
      /*!\name Velocity methods*/
      //@{
      /*!\brief Gets velocity of the current rigid body
       */
      STAMP_GETTERS(Velocity, get_velocity)
      //@}
      //************************************************************************

      //************************************************************************
      /*!\name Angular Velocity methods*/
      //@{
      /*!\brief Gets angular velocity of the current rigid body
       */
      STAMP_GETTERS(AngularVelocity, get_angular_velocity)
      //@}
      //************************************************************************

      //************************************************************************
      /*!\name Aceeleration methods*/
      //@{
      /*!\brief Gets acceleration of the current rigid body
       */
      STAMP_GETTERS(Acceleration, get_acceleration)
      //@}
      //************************************************************************

      //************************************************************************
      /*!\name Angular acceleration methods*/
      //@{
      /*!\brief Gets angular acceleration of the current rigid body
       */
      STAMP_GETTERS(AngularAcceleration, get_angular_acceleration)
      //@}
      //************************************************************************

      //************************************************************************
      /*!\name Material methods*/
      //@{
      /*!\brief Gets material of the current rigid body
       */
      STAMP_GETTERS(Material, get_material)
      //@}
      //************************************************************************

      //************************************************************************
      /*!\name Mass methods*/
      //@{
      /*!\brief Gets mass of the current rod
       */
      STAMP_GETTERS(Mass, get_mass)
      //@}
      //************************************************************************

      //************************************************************************
      /*!\name External Loads methods*/
      //@{
      /*!\brief Gets external loads of the current rigid body
       */
      STAMP_GETTERS(ExternalLoads, get_external_loads)
      //@}
      //************************************************************************

      //************************************************************************
      /*!\name External Torques methods*/
      //@{
      /*!\brief Gets external torques of the current rigid body
       */
      STAMP_GETTERS(ExternalTorques, get_external_torques)
      //@}
      //************************************************************************

      //************************************************************************
      /*!\name Inverse Mass SMOI methods*/
      //@{
      /*!\brief Gets inverse fo mass second moment of inertia of the current
       * rod
       */
      STAMP_GETTERS(InvMassSecondMomentOfInertia,
                    get_inverse_mass_second_moment_of_inertia)
      //@}
      //************************************************************************

#undef STAMP_GETTERS

      //@}
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Compute internal dynamics
       *
       * \details
       * No internal dynamics in case of a rigid body
       *
       * \return void/None
       *
       */
      inline void compute_internal_dynamics(::elastica::Time) {}
      //************************************************************************

      //@}
      //************************************************************************
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Update acceleration based on the current state
     *
     * \details
     * Updates accelerations of the system (translational and angular) given
     * the state of the system. \n
     * acceleration_batch{i} =
     * (internal_force_batch{i} + external_force_batch{i})
     * / mass_batch{i} \n
     * alpha_batch{i} = inv_mass_second_moment_of_inertia_batch{i} *
     * (internal_torque_batch{i} + external_torque_batch{i}) *
     * dilatation_batch{i} \n
     * Also resets external_force_batch and external_torque_batch = 0.0
     *
     * \example
     * The following shows a typical use of the update_acceleration()
     * function with the expected (correct) result also shown.
     * \snippet test_gov_eqns.cpp update_acceleration_example
     *
     * \return void/None
     *
     * \see fill later?
     */
    template <typename Traits, typename ComputationalBlock>
    void update_acceleration(
        WithRigidBodyKinematics<Traits, ComputationalBlock>&
            block_like) noexcept {
      auto&& mass(blocks::get<tags::Mass>(block_like.self()));
      auto&& inv_mass_SMOI(
          blocks::get<tags::InvMassSecondMomentOfInertia>(block_like.self()));
      auto&& acceleration(blocks::get<tags::Acceleration>(block_like.self()));
      auto&& angular_acceleration(
          blocks::get<tags::AngularAcceleration>(block_like.self()));
      auto&& external_loads(
          blocks::get<tags::ExternalLoads>(block_like.self()));
      auto&& external_torques(
          blocks::get<tags::ExternalTorques>(block_like.self()));

      // TODO: Revisit division vs power+multiplication
      // TODO: remove transpose if possible
      // acceleration = total_loads /
      // Traits::Operations::expand_for_broadcast(mass);
      Traits::Operations::batch_division_matvec(acceleration, external_loads,
                                                mass);

      Traits::Operations::batch_matvec(angular_acceleration, inv_mass_SMOI,
                                       external_torques);
    }
    //**************************************************************************

  }  // namespace rigid_body

}  // namespace elastica