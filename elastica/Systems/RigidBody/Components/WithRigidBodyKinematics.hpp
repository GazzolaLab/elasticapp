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
#include "Systems/RigidBody/Components/Tags.hpp"
#include "Systems/RigidBody/Components/Types.hpp"
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
      //! CRTP methods
      using CRTP::self;
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

      //************************************************************************
      /*!\brief Initialize method for the current component
       *
       * \details
       * initialize() is responsible for initializing the variables present
       * in the current component in the final block of data. For
       * InitializedVariables, it fetches the corresponding initializer from
       * the set of `initializers` to fill in the data. For
       * ComputedVariables the default value is usually set. In this way,
       * each component is responsible for initializing its own data in a
       * manner that does not intrude with initialization from other
       * components.
       *
       * \tparam DownstreamBlock The final block-like object which is
       * derived from the current component
       * \tparam RigidBodyInitializer An Initializer corresponding to the
       * RigidBody rod hierarchy
       */
      template <typename DownstreamBlock, typename RigidBodyInitializer>
      static void initialize(DownstreamBlock& downstream_block,
                             RigidBodyInitializer&& initializers) {
        // TODO : refactor
        constexpr std::size_t at_only_index = 0UL;

        //        https://stackoverflow.com/a/53144382
        struct {
          RigidBodyInitializer value;
        } cap{std::forward<RigidBodyInitializer>(initializers)};
        // Note : need a mutable lambda as we can now "move" from cap
        tmpl::for_each<This::InitializedVariables>(
            [&downstream_block,
             //               cap = std::move(cap)
             &cap](auto v) mutable -> void {
              using Variable = tmpl::type_from<decltype(v)>;
              using Tag = blocks::initializer_t<Variable>;

              auto& variable(blocks::get<Tag>(downstream_block));
              auto&& initializer(blocks::get<Tag>(std::move(cap.value)));

              // invoke directly, assuming this is the template here
              Variable::slice(variable, at_only_index) = initializer();
            });

        /* mass second moment of inertia */
        {
          using Variable = InvMassSecondMomentOfInertia;
          using Tag = blocks::parameter_t<Variable>;

          auto& variable(blocks::get<Tag>(downstream_block));

          // already initialized
          auto& material(blocks::get<tags::Material>(downstream_block));

          // (I_1, I_2, I_3)
          Variable::slice(variable, at_only_index) = real_type(0.0);
          // in lagrangian coordinates
          const ::elastica::Vec3 I_at_only_index =
              1.0 /
              (downstream_block.get_unscaled_moment_of_inertia(at_only_index) *
               ::elastica::Material::get_density(
                   Traits::DataType::Index::slice(material, at_only_index)));
          Variable::diagonal_assign(variable, at_only_index, I_at_only_index);
        }

        /* mass computation */
        /* \todo
         * Mass can be computed from geometry. Here we fill in the
         * actual mass, considering the volume of the rigid bodies*/
        {
          auto& mass(blocks::get<::elastica::tags::Mass>(downstream_block));
          auto& material(downstream_block.get_material());

          Mass::slice(mass, at_only_index) =
              ::elastica::Material::get_density(
                  This::Material::slice(material, at_only_index)) *
              downstream_block.get_volume(at_only_index);
        }

        /* Reset forces and torques */
        {
          auto& external_loads(
              blocks::get<::elastica::tags::ExternalLoads>(downstream_block));
          auto& external_torques(
              blocks::get<::elastica::tags::ExternalTorques>(downstream_block));
          external_loads = 0.0;
          external_torques = 0.0;
        }
      }
      //************************************************************************

     public:
      //**Get methods***********************************************************
      /*!\name Get methods*/
      //@{

      //************************************************************************
      /*!\name Velocity methods*/
      //@{
      /*!\brief Gets velocity of the current rod
       */
      inline constexpr decltype(auto) get_velocity() & noexcept {
        return blocks::get<tags::Velocity>(self());
      }
      inline constexpr decltype(auto) get_velocity() const& noexcept {
        return blocks::get<tags::Velocity>(self());
      }
      inline constexpr decltype(auto) get_velocity(index_type idx) & noexcept {
        return Velocity::slice(get_velocity(), idx);
      }
      inline constexpr decltype(auto) get_velocity(
          index_type idx) const& noexcept {
        return Velocity::slice(get_velocity(), idx);
      }
      //@}
      //************************************************************************

      //************************************************************************
      /*!\name Angular Velocity methods*/
      //@{
      /*!\brief Gets angular velocity of the current rod
       */
      inline constexpr decltype(auto) get_angular_velocity() & noexcept {
        return blocks::get<tags::AngularVelocity>(self());
      }
      inline constexpr decltype(auto) get_angular_velocity() const& noexcept {
        return blocks::get<tags::AngularVelocity>(self());
      }
      inline constexpr decltype(auto) get_angular_velocity(
          index_type idx) & noexcept {
        return AngularVelocity::slice(get_angular_velocity(), idx);
      }
      inline constexpr decltype(auto) get_angular_velocity(
          index_type idx) const& noexcept {
        return AngularVelocity::slice(get_angular_velocity(), idx);
      }
      //@}
      //************************************************************************

      //************************************************************************
      /*!\name Aceeleration methods*/
      //@{
      /*!\brief Gets acceleration of the current rod
       */
      inline constexpr decltype(auto) get_acceleration() & noexcept {
        return blocks::get<tags::Acceleration>(self());
      }
      inline constexpr decltype(auto) get_acceleration() const& noexcept {
        return blocks::get<tags::Acceleration>(self());
      }
      //@}
      //************************************************************************

      //************************************************************************
      /*!\name Angular acceleration methods*/
      //@{
      /*!\brief Gets angular acceleration of the current rod
       */
      inline constexpr decltype(auto) get_angular_acceleration() & noexcept {
        return blocks::get<tags::AngularAcceleration>(self());
      }
      inline constexpr decltype(auto) get_angular_acceleration()
          const& noexcept {
        return blocks::get<tags::AngularAcceleration>(self());
      }
      //@}
      //************************************************************************

      //************************************************************************
      /*!\name Material methods*/
      //@{
      /*!\brief Gets material of the current rod
       */
      inline constexpr decltype(auto) get_material() & noexcept {
        return blocks::get<tags::Material>(self());
      }
      inline constexpr decltype(auto) get_material() const& noexcept {
        return blocks::get<tags::Material>(self());
      }
      inline constexpr decltype(auto) get_material(std::size_t idx) & noexcept {
        return Material::slice(get_material(), idx);
      }
      inline constexpr decltype(auto) get_material(
          std::size_t idx) const& noexcept {
        return Material::slice(get_material(), idx);
      }
      //@}
      //************************************************************************

      //************************************************************************
      /*!\name External Loads methods*/
      //@{
      /*!\brief Gets external loads of the current rod
       */
      inline constexpr decltype(auto) get_external_loads() & noexcept {
        return blocks::get<tags::ExternalLoads>(self());
      }
      inline constexpr decltype(auto) get_external_loads() const& noexcept {
        return blocks::get<tags::ExternalLoads>(self());
      }
      inline constexpr decltype(auto) get_external_loads(
          index_type idx) & noexcept {
        return ExternalLoads::slice(get_external_loads(), idx);
      }
      inline constexpr decltype(auto) get_external_loads(
          index_type idx) const& noexcept {
        return ExternalLoads::slice(get_external_loads(), idx);
      }
      //@}
      //************************************************************************

      //************************************************************************
      /*!\name External Torques methods*/
      //@{
      /*!\brief Gets external torques of the current rod
       */
      inline constexpr decltype(auto) get_external_torques() & noexcept {
        return blocks::get<tags::ExternalTorques>(self());
      }
      inline constexpr decltype(auto) get_external_torques() const& noexcept {
        return blocks::get<tags::ExternalTorques>(self());
      }
      inline constexpr decltype(auto) get_external_torques(
          index_type idx) & noexcept {
        return ExternalTorques::slice(get_external_torques(), idx);
      }
      inline constexpr decltype(auto) get_external_torques(
          index_type idx) const& noexcept {
        return ExternalTorques::slice(get_external_torques(), idx);
      }
      //@}
      //************************************************************************

      //@}
      //************************************************************************

     public:  // TODO: temporary change from protected
      //************************************************************************
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

      // TODO: double check how external force and external torque comes into
      // play
      inline void update_acceleration() noexcept {
        auto& mass(blocks::get<tags::Mass>(self()));
        auto& inv_mass_SMOI(
            blocks::get<tags::InvMassSecondMomentOfInertia>(self()));
        auto& acceleration(blocks::get<tags::Acceleration>(self()));
        auto& angular_acceleration(
            blocks::get<tags::AngularAcceleration>(self()));
        auto& external_loads(blocks::get<tags::ExternalLoads>(self()));
        auto& external_torques(blocks::get<tags::ExternalTorques>(self()));

        // TODO: Revisit division vs power+multiplication
        // TODO: remove transpose if possible
        // acceleration = total_loads /
        // Traits::Operations::expand_for_broadcast(mass);
        Traits::Operations::batch_division_matvec(acceleration, external_loads,
                                                  mass);

        Traits::Operations::batch_matvec(angular_acceleration, inv_mass_SMOI,
                                         external_torques);
      }
      //************************************************************************

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

  }  // namespace rigid_body

}  // namespace elastica