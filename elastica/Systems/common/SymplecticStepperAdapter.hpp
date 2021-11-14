#pragma once

#include <type_traits>

#include "Systems/Block/Block/TypeTraits.hpp"
#include "Systems/States/States.hpp"
#include "Systems/common/Tags.hpp"
#include "Time/SimulationTime.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  template <typename... X>
  struct Der : public X... {
    //**Block type definitions************************************************
    //! List of computed variables
    using ComputedVariables = tmpl::append<typename X::ComputedVariables...>;
    //! List of initialized variables
    using InitializedVariables =
        tmpl::append<typename X::InitializedVariables...>;
    //! List of all variables
    using Variables = tmpl::append<typename X::Variables...>;
    //************************************************************************

    template <class DownstreamBlock, class CosseratInitializer>
    static void initialize(DownstreamBlock& downstream_block,
                           CosseratInitializer&& initializers) {
      EXPAND_PACK_LEFT_TO_RIGHT(X::initialize(
          downstream_block, std::forward<CosseratInitializer>(initializers)));
    }
  };

  namespace {

    // TODO : can be optimized and refactored
    template <typename B, typename Variable>
    using SymplecticData = std::conditional_t<
        ::blocks::IsBlock<B>::value, typename Variable::type,
        // can be BlockSlice or ConstBlockSlice
        std::conditional_t<::blocks::IsBlockSlice<B>::value,
                           typename Variable::SliceType::type,
                           typename Variable::ConstSliceType::type>>;
  }  // namespace

  //****************************************************************************
  /*!\brief Adapter for enabling symplectic temporal integration
   * \ingroup systems
   *
   * \details
   * Adapter for system block hierarchies (i.e. conforming to
   * protocols::PhysicalSystem) to interface with symplectic time integrators
   * steppers
   *
   * \tparam BlockCache The block (along with data)
   *
   * \example
   * \snippet Test_SymplecticStepperAdapter.cpp
   */
  template <typename Traits, typename Block,
            template <typename, typename> class... OtherComponents>
  class SymplecticPolicy : public Der<OtherComponents<Traits, Block>...> {
   private:
    //**Type definitions********************************************************
    //! Parent type
    using P = Der<OtherComponents<Traits, Block>...>;
    //! This type
    using This = SymplecticPolicy<Traits, Block, OtherComponents...>;

   protected:
    using typename P::ComputedVariables;
    //! List of initialized variables
    using typename P::InitializedVariables;
    //! List of all variables
    using typename P::Variables;

    //**************************************************************************
    /*!\name Variable data */
    //@{
    using typename P::Acceleration;
    using typename P::AngularAcceleration;
    using typename P::AngularVelocity;
    using typename P::Director;
    using typename P::Position;
    using typename P::Velocity;
    //@}
    //**************************************************************************

   private:
    //**************************************************************************
    /*!\name State types */
    //@{
    using PositionData = SymplecticData<Block, Position>;
    // std::remove_reference_t<decltype(std::declval<P>().get_position())>;
    using PositionState =
        ::elastica::states::SE3<PositionData,
                                ::elastica::states::tags::PrimitiveTag>;
    using VelocityData = SymplecticData<Block, Velocity>;
    using VelocityState =
        ::elastica::states::SE3<VelocityData,
                                ::elastica::states::tags::DerivativeTag>;
    using AccelerationData = SymplecticData<Block, Acceleration>;
    using AccelerationState =
        ::elastica::states::SE3<AccelerationData,
                                ::elastica::states::tags::DoubleDerivativeTag>;

    using DirectorData = SymplecticData<Block, Director>;
    using DirectorState =
        ::elastica::states::SO3<DirectorData,
                                ::elastica::states::tags::PrimitiveTag>;
    using AngularVelocityData = SymplecticData<Block, AngularVelocity>;
    using AngularVelocityState =
        ::elastica::states::SO3<AngularVelocityData,
                                ::elastica::states::tags::DerivativeTag>;

    using AngularAccelerationData = SymplecticData<Block, AngularAcceleration>;
    using AngularAccelerationState =
        ::elastica::states::SO3<AngularAccelerationData,
                                ::elastica::states::tags::DoubleDerivativeTag>;
    //@}
    //**************************************************************************

    //! Collection of all kinematic states
    using KinematicState =
        ::elastica::states::States<PositionState, DirectorState>;
    //! Collection of all dynamic states
    using DynamicState =
        ::elastica::states::States<VelocityState, AngularVelocityState>;
    //! Collection of all dynamic rates
    using DynamicRate =
        ::elastica::states::States<AccelerationState, AngularAccelerationState>;

   protected:
    // At this point the facade would be instantiated so hopefully
    // retrieving the data stored in the tuple will not be a problem.
    SymplecticPolicy()
        : P(),
          kinematic_states_(PositionState(&P::get_position()),
                            DirectorState(&P::get_director())),
          dynamic_states_(VelocityState(&P::get_velocity()),
                          AngularVelocityState(&P::get_angular_velocity())),
          dynamic_rates_(
              AccelerationState(&P::get_acceleration()),
              AngularAccelerationState(&P::get_angular_acceleration())) {}

    SymplecticPolicy(SymplecticPolicy&& other) noexcept
        : P(std::move(other)),
          kinematic_states_(std::move(other.kinematic_states_)),
          dynamic_states_(std::move(other.dynamic_states_)),
          dynamic_rates_(std::move(other.dynamic_rates_)) {}

   public:
    // Do not need const overloads : if its const, it should be a hard
    // error

    inline auto kinematic_states() & noexcept -> KinematicState& {
      return kinematic_states_;
    }

    inline auto kinematic_rates(::elastica::Time /*time_v*/) const noexcept
        -> DynamicState const& {
      return dynamic_states();
    }

    inline auto dynamic_states() & noexcept -> DynamicState& {
      return dynamic_states_;
    }

    inline auto dynamic_states() const& noexcept -> DynamicState const& {
      return dynamic_states_;
    }

    inline auto dynamic_rates(::elastica::Time /*time_v*/) /*const*/ noexcept
        -> DynamicRate const& {
      /* To be truly lazy, one should only compute acceleration
       * data and internal dynamics here.
       * but the cosserat rods need precomputing to account
       * for environment effects etc. so we can skip that here
       */
      // return std::get<SymplecticAccelerations>(self().data());
      P::update_acceleration();
      return dynamic_rates_;
    }

   private:
    KinematicState kinematic_states_;
    DynamicState dynamic_states_;
    DynamicRate dynamic_rates_;
  };

}  // namespace elastica
