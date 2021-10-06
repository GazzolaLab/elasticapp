#pragma once

#include "Systems/States/States.hpp"
#include "Systems/common/Tags.hpp"
#include "Time/SimulationTime.hpp"

namespace elastica {

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

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
  template <typename BlockCache>
  class SymplecticPolicy : public BlockCache {
   private:
    //**Type definitions********************************************************
    //! This type
    using This = SymplecticPolicy<BlockCache>;
    //! Variable map
    using typename BlockCache::VariableMap;
    //! Obtains type of data from Tag
    template <typename Tag>
    using DataType = typename VariableMap::template variable_from<Tag>::type;
    //**************************************************************************

    //**************************************************************************
    /*!\name Variable data */
    //@{
    using PositionData = DataType<::elastica::tags::Position>;
    using VelocityData = DataType<::elastica::tags::Velocity>;
    using AccelerationData = DataType<::elastica::tags::Acceleration>;
    using DirectorData = DataType<::elastica::tags::Director>;
    using AngularVelocityData = DataType<::elastica::tags::AngularVelocity>;
    using AngularAccelerationData =
        DataType<::elastica::tags::AngularAcceleration>;
    //@}
    //**************************************************************************

    //**************************************************************************
    /*!\name State types */
    //@{
    using PositionState =
        ::elastica::states::SE3<PositionData,
                                ::elastica::states::tags::PrimitiveTag>;
    using VelocityState =
        ::elastica::states::SE3<VelocityData,
                                ::elastica::states::tags::DerivativeTag>;
    using AccelerationState =
        ::elastica::states::SE3<AccelerationData,
                                ::elastica::states::tags::DoubleDerivativeTag>;
    using DirectorState =
        ::elastica::states::SO3<DirectorData,
                                ::elastica::states::tags::PrimitiveTag>;
    using AngularVelocityState =
        ::elastica::states::SO3<AngularVelocityData,
                                ::elastica::states::tags::DerivativeTag>;
    using AngularAccelerationState =
        ::elastica::states::SE3<AngularAccelerationData,
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
        : kinematic_states_(&BlockCache::get_position(),
                            &BlockCache::get_director()),
          dynamic_states_(&BlockCache::get_velocity(),
                          &BlockCache::get_angular_velocity()),
          dynamic_rates_(&BlockCache::get_acceleration(),
                         &BlockCache::get_angular_acceleration()) {}

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

    inline auto dynamic_rates(::elastica::Time /*time_v*/) const noexcept
        -> DynamicRate const& {
      /* To be truly lazy, one should only compute acceleration
       * data and internal dynamics here.
       * but the cosserat rods need precomputing to account
       * for environment effects etc. so we can skip that here
       */
      // return std::get<SymplecticAccelerations>(self().data());
      return dynamic_rates_;
    }

   private:
    KinematicState kinematic_states_;
    DynamicState dynamic_states_;
    DynamicRate dynamic_rates_;
  };

}  // namespace elastica
