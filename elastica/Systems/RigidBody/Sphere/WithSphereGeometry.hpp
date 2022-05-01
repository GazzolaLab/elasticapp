//
// Created by Tejaswin Parthasarathy on 4/21/22.
//

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include <cstdint>  // for size_t
#include <utility>  // for move

#include "Simulator/Frames.hpp"  // smells
//
#include "Systems/Block.hpp"
//
#include "Systems/RigidBody/Components/WithRigidBodyGeometry.hpp"
//
#include "Systems/RigidBody/Sphere/Tags.hpp"
#include "Systems/RigidBody/Sphere/Types.hpp"
//
#include "Systems/common/Components.hpp"  // smells
//
#include "Utilities/CRTP.hpp"
#include "Utilities/Math/Square.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace rigid_body {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Variables corresponding to a rigid sphere
     * \ingroup rigid_body_component
     *
     * \details
     * WithSphereGeometryVariables contains the definitions of variables
     * used within the Blocks framework for a rigid sphere
     *
     * \tparam RBT A valid Rigid body Traits class
     */
    template <typename RBT>
    class WithSphereGeometryVariables {
     private:
      //**Type definitions******************************************************
      //! Traits type
      using Traits = RBT;
      //************************************************************************

     protected:
      //**Variable definitions**************************************************
      /*!\name Variable definitions*/
      //@{

      //********************************************************************
      /*!\brief Variable marking Positions
       */
      struct Radius : public Traits::template RigidBodyInitializedVariable<
                          ::elastica::tags::Radius,  //
                          typename Traits::DataType::Scalar> {};
      //********************************************************************

      //@}
      //********************************************************************

      //**Type definitions**************************************************
      //! List of computed variables
      using ComputedVariables = tmpl::list<>;
      //! List of initialized variables
      using InitializedVariables = tmpl::list<Radius>;
      //! List of all variables
      using Variables = tmpl::append<InitializedVariables, ComputedVariables>;
      //********************************************************************
    };
    //**************************************************************************

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Component corresponding to a rigid sphere
     * \ingroup rigid_body_component
     *
     * \details
     * WithSphereGeometry implements geometrical quantities
     * for use within RigidBodies implemented in the Blocks framework.
     *
     * \tparam RBT A valid Rigid Body Traits class
     * \tparam ComputationalBlock The final block which is derived from the
     * current component
     */
    template <typename RBT, typename ComputationalBlock>
    class WithSphereGeometry
        : public WithRigidBodyGeometry<RBT, ComputationalBlock>,
          public WithSphereGeometryVariables<RBT>,
          public ::elastica::GeometryComponent<
              WithSphereGeometry<RBT, ComputationalBlock>>,
          public CRTPHelper<ComputationalBlock, WithSphereGeometry> {
     private:
      //**Type definitions******************************************************
      //! Traits type
      using Traits = RBT;
      //! This type
      using This = WithSphereGeometry<Traits, ComputationalBlock>;
      //! Type of Variable definitions
      using VariableDefinitions = WithSphereGeometryVariables<Traits>;
      //! Parent type
      using Parent = WithRigidBodyGeometry<Traits, ComputationalBlock>;
      //! CRTP Type
      using CRTP = CRTPHelper<ComputationalBlock, WithSphereGeometry>;
      //! CRTP methods
      using CRTP::self;
      //! Real number type
      using real_type = typename Traits::real_type;
      //! Index type
      using index_type = typename Traits::index_type;
      //************************************************************************

     protected:
      //**Type definitions******************************************************
      //! Frame type
      using Frame = ::elastica::LagrangianFrame;
      //! Frame direction type
      using DirectionType = typename Frame::DirectionType;
      //************************************************************************

     protected:
      //**Type definitions******************************************************
      //! List of initialized variables
      using InitializedVariables =
          tmpl::append<typename Parent::InitializedVariables,
                       typename VariableDefinitions::InitializedVariables>;
      //! List of computed variables
      using ComputedVariables =
          tmpl::append<typename Parent::ComputedVariables,
                       typename VariableDefinitions::ComputedVariables>;
      //! List of all variables
      using Variables = tmpl::append<typename Parent::Variables,
                                     typename VariableDefinitions::Variables>;
      //! Current Variable definitions
      using typename VariableDefinitions::Radius;
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
        // 1. Initialize parent
        Parent::initialize(downstream_block,
                           std::forward<RigidBodyInitializer>(initializers));

        //        https://stackoverflow.com/a/53144382
        struct {
          RigidBodyInitializer value;
        } cap{std::forward<RigidBodyInitializer>(initializers)};
        // Note : need a mutable lambda as we can now "move" from cap
        tmpl::for_each<typename VariableDefinitions::InitializedVariables>(
            [&downstream_block,
             //               cap = std::move(cap)
             &cap](auto v) mutable -> void {
              using Variable = tmpl::type_from<decltype(v)>;
              using Tag = blocks::initializer_t<Variable>;

              auto& variable(blocks::get<Tag>(downstream_block));
              auto&& initializer(blocks::get<Tag>(std::move(cap.value)));

              // invoke directly, assuming this is the template here
              variable = initializer();
            });
      }
      //************************************************************************

     public:
      //**Static members********************************************************
      //! unique ID for D1 direction
      static constexpr DirectionType D1 = Frame::D1;
      //! unique ID for D2 direction
      static constexpr DirectionType D2 = Frame::D2;
      //! unique ID for D3 direction
      static constexpr DirectionType D3 = Frame::D3;
      //************************************************************************

     public:
      //**Get methods***********************************************************
      /*!\name Get methods*/
      //@{

      //************************************************************************
      /*!\name Radius methods*/
      //@{
      /*!\brief Gets radius of the current sphere
       */
      inline constexpr decltype(auto) get_radius() & noexcept {
        return blocks::get<tags::Radius>(self());
      }
      inline constexpr decltype(auto) get_radius() const& noexcept {
        return blocks::get<tags::Radius>(self());
      }
      inline constexpr decltype(auto) get_radius(index_type idx) & noexcept {
        return Radius::slice(get_radius(), idx);
      }
      inline constexpr decltype(auto) get_radius(
          index_type idx) const& noexcept {
        return Radius::slice(get_radius(), idx);
      }
      //@}
      //************************************************************************

     protected:
      //************************************************************************
      /*!\brief computes volume
       *
       * \param radius radius of the circle
       */
      template <typename Radius>
      static inline decltype(auto) compute_volume(
          Radius const& radius) noexcept {
        return static_cast<real_type>(4.0) / static_cast<real_type>(3.0) *
               M_PI * radius * radius * radius;
      }
      //************************************************************************

     public:
      //************************************************************************
      /*!\name volume methods*/
      //@{
      /*!\brief Gets volume of the current rod
       */
      inline constexpr auto get_volume() /*noexcept*/ {
        return compute_volume(get_radius());
      }
      inline constexpr auto get_volume(index_type idx) noexcept {
        return compute_volume(get_radius(idx));
      }
      //@}
      //************************************************************************

     protected:
      //************************************************************************
      /*!\brief computes volume
       *
       * \param radius radius of the circle
       */
      template <typename Radius>
      static inline decltype(auto) compute_unscaled_moment_of_inertia(
          Radius const& radius) noexcept {
        return (static_cast<real_type>(2.0) / static_cast<real_type>(5.0))
               /* density not multiple because unscaled*/
               * compute_volume(radius) * sq(radius);
      }
      //************************************************************************

     public:
      //************************************************************************
      /*!\name Moment of inertia methods*/
      //@{
      /*!\brief Gets second moment of area of the current rod
       */
      template <DirectionType Dim, Requires<(Dim < Frame::D3)> = nullptr>
      inline constexpr decltype(auto) get_unscaled_moment_of_inertia(
          index_type idx) const
      /*noexcept*/ {
        // Should give out a VecVecExpr rather than Dimensions object
        // for expression template mechanisms
        return compute_unscaled_moment_of_inertia(This::get_radius(idx));
      }
      template <DirectionType Dim, Requires<(Dim == Frame::D3)> = nullptr>
      inline constexpr decltype(auto) get_unscaled_moment_of_inertia(
          index_type idx) const
      /*noexcept*/ {
        // Should give out a VecVecExpr rather than Dimensions object
        // for expression template mechanisms
        return compute_unscaled_moment_of_inertia(This::get_radius(idx));
      }
      // do not have APIs return vec3, seems to reduce customizability and
      // promotes bad code
      inline constexpr auto get_unscaled_moment_of_inertia(index_type idx) const
      /*noexcept*/ {
        return Vec3{real_t(This::get_unscaled_moment_of_inertia<D1>(idx)),
                    real_t(This::get_unscaled_moment_of_inertia<D2>(idx)),
                    real_t(This::get_unscaled_moment_of_inertia<D3>(idx))};
      }

      //@}
      //************************************************************************

      //@}
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace rigid_body

}  // namespace elastica