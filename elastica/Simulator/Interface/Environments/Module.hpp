//==============================================================================
/*!
//  \file
//  \brief Implementation of module for Environments
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

#include <memory>
#include <tuple>
#include <utility>
#include <vector>

/// Environment types
#include "Simulator/Environments/Common.hpp"
#include "Simulator/Environments/TypeTraits.hpp"
//
#include "Simulator/Interface/Protocols/Environments.hpp"
/// Interface methods
#include "Simulator/Interface/Environments/Types.hpp"
///
#include "Simulator/Interface/Affordances/detail/EmptyBatchingAlgorithm.hpp"
#include "Simulator/Interface/Environments/Configuration.hpp"
#include "Simulator/Interface/Environments/ExecutionPolicy.hpp"
#include "Simulator/Interface/Environments/WithImposedPrimitives.hpp"
#include "Simulator/Interface/Environments/WithPrimitives.hpp"
///
///
#include "Simulator/Utilities/Timers.hpp"
/// Serialization support
#include "Simulator/IO/Serialization/Types.hpp"
/// Utilities
#include "Utilities/CRTP.hpp"
#include "Utilities/Invoke.hpp"
#include "Utilities/Logging.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/Requires.hpp"
#include "Utilities/Singleton.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace modules {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    // TODO : tmpl::list of allowable environment types (basic, all, temporally
    // evolving)
    // TODO : protocol/requirement check for default constructible from
    // environments side

    //**************************************************************************
    /*!\brief The environment module
     * \ingroup interface environment
     *
     * The Environment module is an enhancement to the \elastica Simulator
     * and is responsible for applying forces from the environment acting on
     * each system within the current simulation.
     *
     * \tparam SimTraits Options for configuration of the Simulator
     * \tparam Sim Simulator
     *
     * \see make_simulator(), Simulator
     */
    template <class SimTraits, class Sim>
    class Environments
        : public CRTPHelper<Sim, Environments>,
          public ::tt::ConformsTo<
              ::elastica::interface::protocols::Environments>,
          private detail::HasEnvironmentsConfiguration<SimTraits>,
          public detail::EmptyBatchingAlgorithm<SimTraits,
                                                Environments<SimTraits, Sim>>,
          public ::elastica::interface::environments::WithImposedPrimitives<
              SimTraits, Sim>,  // is non-copyable
          public ::elastica::interface::environments::WithPrimitives<
              SimTraits, Sim>,  // is non-copyable
          protected ::elastica::interface::environments::ExecutionPolicy<
              SimTraits, Sim> {
     private:
      //**Compile-time checks***************************************************
      //
      //************************************************************************

      //**Type definitions******************************************************
      //! This type
      using This = Environments<SimTraits, Sim>;
      //! CRTP Type
      using CRTP = CRTPHelper<Sim, Environments>;
      //! CRTP methods
      using CRTP::self;
      //! Internal size type
      using SizeType = typename SimTraits::SizeType;
      //! Internal type of index
      using IndexType = typename SimTraits::IndexType;
      //! Algorithm type
      using Algorithm =
          ::elastica::interface::environments::ExecutionPolicy<SimTraits, Sim>;
      //! Imposed primitives interface
      using ImposedPrimitivesInterface =
          ::elastica::interface::environments::WithImposedPrimitives<SimTraits,
                                                                     Sim>;
      //! Primitives interface
      using PrimitivesInterface =
          ::elastica::interface::environments::WithPrimitives<SimTraits, Sim>;
      //************************************************************************

     protected:
      //**Algorithmic order*****************************************************
      /*!\name Algorithmic steps within a temporal loop*/
      //@{
      //! Algorithms to be carried out as an update step
      using typename PrimitivesInterface::UpdateSteps;
      //! Algorithms for environments
      using typename Algorithm::PreDynamicUpdateSteps;
      //!
      using typename ImposedPrimitivesInterface::PostKinematicUpdateSteps;
      //!
      using typename ImposedPrimitivesInterface::PostDynamicUpdateSteps;
      //@}
      //************************************************************************

     public:
      //**Type definitions******************************************************
      //! Tags of admissible environments
      using AdmissibleEnvironmentTags =
          typename SimTraits::AdmissibleEnvironmentTags;
      //! Type of simulation time
      using TimeType = typename SimTraits::TimeType;  // public for protocol
      //! Submodules of the current module
      using SubModules = tmpl::append<
          tmpl::list<ImposedPrimitivesInterface, PrimitivesInterface>,
          typename ImposedPrimitivesInterface::SubModules,
          typename PrimitivesInterface ::SubModules>;
      //************************************************************************

     private:
      //**Type definitions******************************************************
      struct EnvironmentTraits : public SimTraits,
                                 public PrimitivesInterface::Traits {};
      //! Typelist of admissible environments
      using ListOfAdmissibleEnvironments =
          tmpl::transform<AdmissibleEnvironmentTags,
                          ::elastica::environments::tt::GetEnvironmentType<
                              tmpl::pin<EnvironmentTraits>, tmpl::_1>>;
      //! Tuple of singletons environments, accessed via shared_ptrs
      // TODO: rename to as singleton
      using AdmissibleEnvironments = tmpl::as_tuple<
          tmpl::transform<ListOfAdmissibleEnvironments,
                          ::elastica::tt::AddSingletonStorage<tmpl::_1>>>;
      template <typename Tag>
      struct Tag2EnvStorage {
        using type = typename ::elastica::tt::AddSingletonStorage<
            typename ::elastica::environments::tt::GetEnvironmentType<
                EnvironmentTraits, Tag>::type>::type;
      };
      //************************************************************************

      //**Friendships***********************************************************
      //! Friend the Algorithm
      friend Algorithm;
      //! Friend the imposed interface, not needed strictly except for symmetry
      friend ImposedPrimitivesInterface;
      //! Friend the interface
      friend PrimitivesInterface;
      //! Friendship with Serialize
      friend struct ::elastica::io::Serialize<This>;
      //************************************************************************

     protected:
      //************************************************************************
      /*!\name Profiler type definitions */
      //@{
      //! Cache for timers for profiling the code
      using EnvTimers = ::elastica::Timers<AdmissibleEnvironmentTags>;
      //@}
      //************************************************************************

     private:
      //************************************************************************
      /*!\brief Instantiate singletons
       *
       * \details
       * Instantiate a tuple of singletons
       */
      template <typename... X>
      static constexpr inline auto instantiate_from(tmpl::list<X...> /*meta*/) {
        return std::make_tuple(X::instance()...);
      }
      //************************************************************************

     protected:
      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{

      //************************************************************************
      /*!\brief Constructor of the Environments class.
       *
       */
      Environments()
          : ImposedPrimitivesInterface(),
            environments_(
                This::instantiate_from(ListOfAdmissibleEnvironments{})),
            environment_timers_() {
        using namespace ::elastica::logging;
        log<info>([](auto const& log) {
          log << "Successfully initialized the Environments module";
        });
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Move constructor of the Environments class.
       *
       * \details This is not needed strictly (as it is ensured to be a
       * singleton class by the Simulator). However, while testing it is useful.
       */
      Environments(Environments&& other) noexcept
          : ImposedPrimitivesInterface(std::move(other)),
            environments_(std::move(other.environments_)),
            environment_timers_(std::move(other.environment_timers_)) {}
      //************************************************************************

      //@}
      //************************************************************************

     public:
      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      ~Environments() {
        using namespace ::elastica::logging;
        log<info>([](auto const& log) {
          log << "Successfully cleaned up the Environment module";
        });
      };
      //@}
      //************************************************************************

     public:
      //**Get methods***********************************************************
      /*!\name Get methods */
      //@{

      //**Buffer functions******************************************************
      /*!\name Buffers */
      //@{
      /*!\brief Gets the environments buffer
       */
      inline constexpr auto environments() & noexcept
          -> AdmissibleEnvironments& {
        return environments_;
      }
      inline constexpr auto environments() const& noexcept
          -> AdmissibleEnvironments const& {
        return environments_;
      }
      //@}
      //************************************************************************

      //**Buffer functions******************************************************
      /*!\name Buffers */
      //@{
      /*!\brief Gets an environment
       * \tparam Tag Tag to get environment from
       */
      template <typename Tag>
      inline constexpr decltype(auto) get_environment() & noexcept {
        using E = typename Tag2EnvStorage<Tag>::type;
        return std::get<E>(environments_);
      }
      template <typename Tag>
      inline constexpr decltype(auto) get_environment() const& noexcept {
        using E = typename Tag2EnvStorage<Tag>::type;
        return std::get<E>(environments_);
      }
      //@}
      //************************************************************************

      //@}
      //************************************************************************

     public:
      //**Interfaces************************************************************
      /*!\name Interface methods */
      //@{

      //************************************************************************
      /*!\brief Gets the number of environments in the present simulator
       */
      static inline constexpr auto n_environments() noexcept -> SizeType {
        return tmpl::size<ListOfAdmissibleEnvironments>::value;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply environmental effects on all registered systems
       *
       * \param time_v The current time of simulation
       */
      inline auto apply_environmental_effects(TimeType time_v) const -> void {
        // this is internal so a member function suffices.
        auto const& algorithm =
            self()
                .template get_algorithm<
                    typename Algorithm::ApplyEnvironmentForces>();
        algorithm(time_v);
      }
      //************************************************************************

      //@}
      //************************************************************************

     public:
      //**Profiling methods*****************************************************
      /*!\name Profiling methods*/
      //@{

      //************************************************************************
      /*!\name Timer functions */
      //@{
      /*!\brief Gets the registered timers.
       *
       * \note
       * If elastica_profiling_mode() is disabled, the values within the timer
       * may not be what is expected.
       */
      inline constexpr auto environment_timers() & noexcept -> EnvTimers& {
        return environment_timers_;
      }
      inline constexpr auto environment_timers() const& noexcept
          -> EnvTimers const& {
        return environment_timers_;
      }
      //@}
      //************************************************************************

      //************************************************************************
      /*!\name Timer functions */
      //@{
      /*!\brief Get a timer corresponding to an algorithm
       *
       * \tparam EnvironmentTag Tag for obtaining the environment
       * \return Algorithm callable
       */
      template <typename EnvironmentTag>
      inline constexpr decltype(auto) get_environment_timer() & noexcept {
        using ::elastica::get_meter;
        return get_meter<EnvironmentTag>(environment_timers());
      }
      template <typename EnvironmentTag>
      inline constexpr decltype(auto) get_environment_timer() const& noexcept {
        using ::elastica::get_meter;
        return get_meter<EnvironmentTag>(environment_timers());
      }
      //@}
      //************************************************************************

      //@}
      //************************************************************************

     protected:
      //************************************************************************
      /*!\brief The finalize method.
       *
       * \details
       * The finalize method is the interface to finalize the environments
       * module of elastica before launching into temporal integration.
       *
       * \param time_v The time to finalize, usually 0.0
       * \exception std::runtime_error If any constraint information is
       * missing/faulty
       *
       *
       * \note This is not marked const because for some reason imposed
       * primitives cannot be const in an interior scope
       */
      void finalize(TimeType time_v) {
        // Call initialize on the more complex environments, passing the
        // current system storage and fixed primitives

        PrimitivesInterface::finalize(time_v);

        // needs access to the domain() member here
        std::apply(
            [&](auto const&... envs) {
              // let all be pointers for a uniform interface
              EXPAND_PACK_LEFT_TO_RIGHT(
                  // the self here needs to be const, but then we cannot use
                  // for_each_block
                  envs->initialize(self()));
            },
            environments_);
      }
      //************************************************************************

     protected:
      //**Reset methods*********************************************************
      /*!\name Reset methods*/
      //@{

      //************************************************************************
      /*!\brief Resets the environment module
       *
       * \param time_v The time of reset
       * \exception std::runtime_error
       */
      void reset_module(TimeType time_v) {
        ImposedPrimitivesInterface::reset_module(time_v);
        PrimitivesInterface::reset_module(time_v);
        std::apply(
            [time_v](auto&... environment) {
              using ::elastica::environments::reset;
              EXPAND_PACK_LEFT_TO_RIGHT(reset(*environment, time_v));
            },
            environments());

        using ::elastica::for_each;
        for_each(environment_timers(), [](auto& meter) { meter.reset(); });
      }
      //************************************************************************

      //@}
      //************************************************************************

     private:
      //**Member variables******************************************************
      //! The cache for all admissible environments
      AdmissibleEnvironments environments_;
      //! Timer caches
      EnvTimers environment_timers_;
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace modules

}  // namespace elastica