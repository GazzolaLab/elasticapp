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

// TODO : Refactor this guy to simple environment within environment
//#include "Simulator/Environments/Environments.hpp"
/// Environment types
#include "Simulator/Environments/Common.hpp"
#include "Simulator/Environments/Tags.hpp"
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
      //**Algorithmic order***************************************************
      /*!\name Algorithmic steps within a temporal loop*/
      //@{
      //! Algorithms to be carried out as an update step
      using typename PrimitivesInterface::UpdateSteps;
      // All others are imported from Execution Policy
      //@}
      //**********************************************************************

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
      //! Friend the interface
      friend PrimitivesInterface;
      //! Friendship with Serialize
      friend struct ::elastica::io::Serialize<This>;
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
                This::instantiate_from(ListOfAdmissibleEnvironments{})) {
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
            environments_(std::move(other.environments_)) {}
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

      //************************************************************************
      /*!\brief Sets a uniform gravity of the simulation world if the gravity
       * environment is present.
       */
      template <typename Tag = ::elastica::environments::tags::GravityTag,
                bool B = tmpl::list_contains_v<AdmissibleEnvironmentTags, Tag>,
                Requires<B> = nullptr>
      inline auto set_gravity(::elastica::Vec3 const& g) noexcept -> void {
        get_environment<Tag>()->gravity() = g;
      }
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
        cpp17::apply(
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
        cpp17::apply(
            [time_v](auto&... environment) {
              using ::elastica::environments::reset;
              EXPAND_PACK_LEFT_TO_RIGHT(reset(*environment, time_v));
            },
            environments());
      }
      //************************************************************************

      //@}
      //************************************************************************

     private:
      //**Member variables******************************************************
      //! The cache for all admissible environments
      AdmissibleEnvironments environments_;
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace modules

}  // namespace elastica

// struct Environments : public
// ::tt::ConformsTo<::elastica::interface::protocols::Environments>, public
// NonCopyable {
//   using SizeType = std::size_t;
//   using TimeType = typename SimulatorConfig::TimeType;
//   /*
//   template <typename T>
//   using SystemInterfaceType =
//       typename SimulatorConfig::template SystemInterfaceType<T>;
//   */
//   using EnvironmentID =
//   std::unique_ptr<::elastica::interface::environments::EnvironmentBase>;
//   //      using EnvironmentID =
//   ::elastica::interface::environments::EnvironmentBase*;
//
//   using AdmissibleEnvironmentTags =
//   typename SimulatorConfig::AdmissibleEnvironmentTags;
//   using AdmissibleEnvironments =
//   tmpl::transform<AdmissibleEnvironmentTags,
//       ::elastica::interface::environments::tt::GetEnvironmentType<
//       tmpl::pin<SimulatorConfig>, tmpl::_1>>;
//   // Singletons, tuple of shared_ptrs of environments
//   using AdmissibleEnvironmentsStorage = tmpl::as_tuple<
//       tmpl::transform<AdmissibleEnvironments,
//           ::elastica::tt::AddSingletonStorage<tmpl::_1>>>;
//   //      using AdmissibleEnvironments = tmpl::as_tuple<tmpl::transform<
//   //          AdmissibleEnvironmentTags,
//   //
//   ::elastica::interface::environments::tags::GetEnvironmentType<collision::CollisionConfig
//   //          , tmpl::_1>>>;
//
//   template <typename ImposedPrimitive>
//   using AddImposedPrimitiveStorageType =
//   typename SimulatorConfig::template AddImposedPrimitiveStorageType<
//       ImposedPrimitive>;
//
//   using ImposedPrimitiveStorage =
//   typename SimulatorConfig::ImposedPrimitiveStorage;
//
//   //=================================================================================================
//   //
//   //  CONSTRUCTOR
//   //
//   //=================================================================================================
//
//   //*************************************************************************************************
//   /*!\brief Constructor of the Constraints class.
//    */
//   // TODO : mark protected
//   // TODO : environments designed as singleton for now, revisit later
//   Environments() : NonCopyable(), simple_envs_(), imposed_primitives_() {
//     index_apply<get_number_of_environments()>([this](auto... Is) {
//       EXPAND_PACK_LEFT_TO_RIGHT(
//           std::get<Is>(environments_) =
//               tmpl::at_c<AdmissibleEnvironments, Is>::instance());
//     });
//     // We do not need to worry about the lifetime of the
//     // logging class, the main simulator instance takes care of it
//     LOG_INFO_SECTION(log) {
//         log << "Successfully initialized the environment instance";
//       }
//   }
//
//   ~Environments() {
//     LOG_INFO_SECTION(log) {
//         log << "Successfully destroyed the environment instance";
//       }
//   }
//
//   template <typename EnvironmentType, typename... Args>
//   auto add_simple_environment(Args&&... args) -> EnvironmentType* {
//     static_assert(std::is_base_of<::elastica::interface::environments::EnvironmentBase,
//                       EnvironmentType>::value,
//                   "Given environment does not satisfy interface "
//                   "requirements, "
//                   "did you forget to derive from EnvironmentBase?");
//     // emplace back leaks https://stackoverflow.com/a/15784813
//     simple_envs_.push_back(
//         std::make_unique<EnvironmentType>(std::forward<Args>(args)...));
//     return static_cast<EnvironmentType*>(simple_envs_.back().get());
//     //        simple_envs_.push_back(
//     //
//     static_cast<::elastica::interface::environments::EnvironmentBase*>(env.get()));
//   }
//
//   template <typename EnvironmentType>
//   auto add_simple_environment(std::unique_ptr<EnvironmentType> env)
//   -> EnvironmentType* {
//     static_assert(std::is_base_of<::elastica::interface::environments::EnvironmentBase,
//                       EnvironmentType>::value,
//                   "Given environment does not satisfy interface "
//                   "requirements, "
//                   "did you forget to derive from EnvironmentBase?");
//     simple_envs_.push_back(std::move(env));
//     return static_cast<EnvironmentType*>(simple_envs_.back().get());
//   }
//
//   //      template <typename EnvironmentType>
//   //      void add_simple_environment(const
//   //      std::shared_ptr<EnvironmentType>& env) {
//   //
//   static_assert(std::is_base_of<::elastica::interface::environments::EnvironmentBase,
//   //                                      EnvironmentType>::value,
//   //                      "Given environment does not satisfy interface "
//   //                      "requirements, "
//   //                      "did you forget to derive from EnvironmentBase?");
//   //        simple_envs_.push_back(
//   //
//   static_cast<::elastica::interface::environments::EnvironmentBase*>(env.get()));
//   //      }
//   //
//   constexpr static SizeType get_number_of_environments() noexcept {
//     return tmpl::size<AdmissibleEnvironments>::value;
//   }
//
//  private:
//   template <typename ImposedPrimitiveType>
//   void apply_simple_environment_forces(
//       std::vector<ImposedPrimitiveType>& system_collection,
//       TimeType time_v) {
//     for (auto& simple_env : simple_envs_) {
//       for (ImposedPrimitiveType& sys_ptr : system_collection)
//         simple_env->template apply<ImposedPrimitiveType>(sys_ptr, time_v);
//     }
//   }
//
//   constexpr Sim const& self() const noexcept {
//     return static_cast<const Sim&>(*this);
//   }
//
//   //      template <typename ImposedPrimitiveType>
//   //      void apply_environment_forces(TimeType time_v) {
//   //        auto& collection_containers_tuple =
//   //            static_cast<Sim*>(this)->get_system_collection_storage();
//   //        simple_env->template apply<ImposedPrimitiveType>(sys_ptr,
//   //        time_v);
//   //        }
//   //      }
//   //
//  public:
//   void resolve_external_effects(TimeType time_v) {
//     // Resolve simple enviroments first
//     // Earlier I was doing self().get_number_of_system_collections(), both
//     // marked constexpr. It works in gcc and not in clang, so changed to
//     // this version (static) instead
//     constexpr SizeType n_system_collections(
//         Sim::get_number_of_system_collections());
//     index_apply<n_system_collections>([this, time_v](auto... Is) {
//       EXPAND_PACK_LEFT_TO_RIGHT(this->apply_simple_environment_forces(
//           static_cast<Sim*>(this)
//               ->template get_system_collection_container<Is>(),
//           time_v));
//     });
//
//     // More complex environments, expansion of system collection of
//     // parameter packs done inside
//     auto& collection_containers_tuple =
//         static_cast<Sim*>(this)->get_system_collection_storage();
//     index_apply<get_number_of_environments()>(
//         [this, &collection_containers_tuple, time_v](auto... Is) {
//           EXPAND_PACK_LEFT_TO_RIGHT(
//               std::get<Is>(environments_)
//                   ->resolve(collection_containers_tuple, time_v));
//         });
//     //          using expander = int[];
//     //          (void)expander{
//     //              0, ((void)(this->apply_simple_environment_forces(
//     //                      static_cast<Sim*>(this)
//     //                          ->template
//     //                          get_system_collection_container<Is>(),
//     //                      time_v)),
//     //                  0)...};
//     //        });
//   }
//
//   // TODO Make private?
//   void finalize(TimeType /*time_v*/) {
//     // 1. Call initialize on the more complex environments, passing the
//     // current system storage and fixed primitives
//     index_apply<get_number_of_environments()>([this](auto... Is) {
//       EXPAND_PACK_LEFT_TO_RIGHT(
//           std::get<Is>(environments_)
//               ->initialize(&self().domain_,
//                            self().get_system_collection_storage(),
//                            &imposed_primitives_));
//     });
//   }
//
//   // TODO : Private (outside world has no business accessing the vector of
//   //        collections,
//   template <typename ImposedPrimitive>
//   inline constexpr auto get_imposed_primitive_container() noexcept
//   -> AddImposedPrimitiveStorageType<ImposedPrimitive>& {
//     return std::get<AddImposedPrimitiveStorageType<ImposedPrimitive>>(
//         imposed_primitives_);
//   }
//
//   template <typename ImposedPrimitive>
//   inline constexpr auto get_imposed_primitive_container() const noexcept
//   -> AddImposedPrimitiveStorageType<ImposedPrimitive> const& {
//     return std::get<AddImposedPrimitiveStorageType<ImposedPrimitive>>(
//         imposed_primitives_);
//   }
//
//   template <class ImposedPrimitive>
//   inline auto push_back_imposed_impl(ImposedPrimitive&& system)
//   -> ImposedPrimitive* {
//     // Propogate the fixed primitive to all (?!) the environments
//     // while getting a pointer
//     auto& imposed_primitive_container =
//         get_imposed_primitive_container<ImposedPrimitive>();
//     imposed_primitive_container.push_back(
//         std::forward<ImposedPrimitive>(system));
//     // return non-owning handle to the System just pushed back
//     return &imposed_primitive_container.back();
//   }
//
//   /* For fixed objects (finite/non-finite) inserted in the simulator
//    * Only used for 1) environments and 2) Visualization*/
//   template <class ImposedPrimitive>
//   inline auto push_back_imposed(ImposedPrimitive&& imposed_primitive) {
//     // TODO proper error message
//     static_assert(tmpl::list_contains_v<
//                       typename SimulatorConfig::AdmissibleImposedPrimitives,
//                       ImposedPrimitive> != 0,
//                   "The imposed primitive that you are trying to add to the "
//                   "Simulator is either unknown to elastica or not in the "
//                   "list of admitted primitives! "
//                   "Please add them to the above list if possible");
//
//     return push_back_imposed_impl(
//         std::forward<ImposedPrimitive>(imposed_primitive));
//   }
//
//   template <class ImposedPrimitive, class... Args>
//   inline auto emplace_back_imposed_impl(Args&&... args)
//   -> ImposedPrimitive* {
//     auto& imposed_primitive_container =
//         get_imposed_primitive_container<ImposedPrimitive>();
//     imposed_primitive_container.emplace_back(std::forward<Args>(args)...);
//     // return non-owning handle to the System just pushed back
//     return &imposed_primitive_container.back();
//   }
//
//   template <class ImposedPrimitive, class... Args>
//   inline auto emplace_back_imposed(Args&&... args) {
//     // TODO proper error message
//     static_assert(tmpl::list_contains_v<
//                       typename SimulatorConfig::AdmissibleImposedPrimitives,
//                       ImposedPrimitive> != 0,
//                   "The imposed primitive that you are trying to add to the "
//                   "Simulator is either unknown to elastica or not in the "
//                   "list of admitted primitives! "
//                   "Please add them to the above list if possible");
//
//     return emplace_back_imposed_impl<ImposedPrimitive>(
//         std::forward<Args>(args)...);
//   }
//
//  public:
//   // TODO : Double indirection for slicing, not nice
//   std::vector<EnvironmentID> simple_envs_;
//   ImposedPrimitiveStorage imposed_primitives_;
//   AdmissibleEnvironmentsStorage environments_;
// };
