#pragma once

#include <type_traits>

//
#include "Systems/Block/Block/TypeTraits.hpp"
#include "Systems/States/States.hpp"
#include "Systems/common/Tags.hpp"
#include "Systems/common/Types.hpp"
//
#include "Time/SimulationTime.hpp"
//
#include "Utilities/TMPL.hpp"

namespace elastica {

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  namespace detail {

    template <typename... X>
    struct Inherit : public X... {
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

    template <typename B, typename Variable>
    using SymplecticDataView =
        std::conditional_t<::blocks::IsBlockSlice<B>::value ||
                               ::blocks::IsBlockView<B>::value,
                           typename Variable::SliceType::type,
                           typename Variable::ConstSliceType::type>;

    // TODO : can be optimized and refactored
    template <typename B, typename Variable>
    using SymplecticData =
        std::conditional_t<::blocks::IsBlock<B>::value, typename Variable::type,
                           SymplecticDataView<B, Variable>>;
  }  // namespace detail

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
  class SymplecticPolicy
      : public detail::Inherit<OtherComponents<Traits, Block>...> {
   private:
    //**Type definitions********************************************************
    //! Parent type
    using P = detail::Inherit<OtherComponents<Traits, Block>...>;
    //! This type
    using This = SymplecticPolicy<Traits, Block, OtherComponents...>;
    //**************************************************************************

   private:
    //**************************************************************************
    // CRTP section
    // \note : cant use CRTP helper because it does not expect template
    // template parameters
    //! Type of the bottom level derived class
    using Self = Block;
    //! Reference type of the bottom level derived class
    using Reference = Self&;
    //! const reference type of the bottom level derived class
    using ConstReference = Self const&;
    //**************************************************************************

   protected:
    //**Type definitions********************************************************
    //! List of computed variables
    using typename P::ComputedVariables;
    //! List of initialized variables
    using typename P::InitializedVariables;
    //! List of all variables
    using typename P::Variables;
    //**************************************************************************

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
    using PositionData = detail::SymplecticData<Block, Position>;
    // std::remove_reference_t<decltype(std::declval<P>().get_position())>;
    using PositionState =
        ::elastica::states::SE3<PositionData,
                                ::elastica::states::tags::PrimitiveTag>;
    using VelocityData = detail::SymplecticData<Block, Velocity>;
    using VelocityState =
        ::elastica::states::SE3<VelocityData,
                                ::elastica::states::tags::DerivativeTag>;
    using AccelerationData = detail::SymplecticData<Block, Acceleration>;
    using AccelerationState =
        ::elastica::states::SE3<AccelerationData,
                                ::elastica::states::tags::DoubleDerivativeTag>;

    using DirectorData = detail::SymplecticData<Block, Director>;
    using DirectorState =
        ::elastica::states::SO3<DirectorData,
                                ::elastica::states::tags::PrimitiveTag>;
    using AngularVelocityData = detail::SymplecticData<Block, AngularVelocity>;
    using AngularVelocityState =
        ::elastica::states::SO3<AngularVelocityData,
                                ::elastica::states::tags::DerivativeTag>;

    using AngularAccelerationData =
        detail::SymplecticData<Block, AngularAcceleration>;
    using AngularAccelerationState =
        ::elastica::states::SO3<AngularAccelerationData,
                                ::elastica::states::tags::DoubleDerivativeTag>;

    //! Collection of all kinematic states
    using KinematicState =
        ::elastica::states::States<PositionState, DirectorState>;
    //! Collection of all dynamic states
    using DynamicState =
        ::elastica::states::States<VelocityState, AngularVelocityState>;
    //! Collection of all dynamic rates
    using DynamicRate =
        ::elastica::states::States<AccelerationState, AngularAccelerationState>;
   
    //@}
    //**************************************************************************

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
    //**************************************************************************
    /*!\name CRTP methods */
    //@{

    //**Self method*************************************************************
    /*!\brief Access to the underlying derived
     *
     * \return Mutable reference to the underlying derived
     *
     * Safely down-casts this module to the underlying derived type, using
     * the Curiously Recurring Template Pattern (CRTP).
     */
    inline constexpr auto self() & noexcept -> Reference {
      return static_cast<Reference>(*this);
    }
    //**************************************************************************

    //**Self method*************************************************************
    /*!\brief Access to the underlying derived
     *
     * \return Const reference to the underlying derived
     *
     * Safely down-casts this module to the underlying derived type, using
     * the Curiously Recurring Template Pattern (CRTP).
     */
    inline constexpr auto self() const& noexcept -> ConstReference {
      return static_cast<ConstReference>(*this);
    }
    //**************************************************************************

    //@}
    //**************************************************************************

   public:
    // Do not need const overloads : const here should be a hard compile error
    //**************************************************************************
    /*!\name State functions */
    //@{

    //**************************************************************************
    /*!\brief Access to kinematic states
     */
    inline auto kinematic_states() & noexcept -> KinematicState& {
      return kinematic_states_;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Access to kinematic rates
     */
    inline auto kinematic_rates(::elastica::Time /*time_v*/) const noexcept
        -> DynamicState const& {
      return dynamic_states();
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Access to dynamic states
     */
    inline auto dynamic_states() & noexcept -> DynamicState& {
      return dynamic_states_;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Access to immutable dynamic states
     */
    inline auto dynamic_states() const& noexcept -> DynamicState const& {
      return dynamic_states_;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Access to dynamic rates
     */
    inline auto dynamic_rates(::elastica::Time /*time_v*/) /*const*/ noexcept
        -> DynamicRate const& {
      /* To be truly lazy, one should only compute acceleration
       * data and internal dynamics here.
       * but the cosserat rods need precomputing to account
       * for environment effects etc. so we can skip that here
       */
      update_acceleration(self());
      return dynamic_rates_;
    }
    //**************************************************************************

    //@}
    //**************************************************************************

   private:
    //! Kinematic state information of the system.
    KinematicState kinematic_states_;
    //! Dynamic state information of the system.
    DynamicState dynamic_states_;
    //! Dynamic rate information of the system.
    DynamicRate dynamic_rates_;
  };
  //****************************************************************************

}  // namespace elastica
