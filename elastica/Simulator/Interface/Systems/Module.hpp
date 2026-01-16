//==============================================================================
/*!
//  \file
//  \brief Implementation of module for Systems
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

#include <algorithm>  // find
#include <array>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <tuple>
#include <utility>  // forward
#include <vector>

#define PRIMITIVE_POLICY_WORKAROUND                                 \
  "Workaround for WithPrimitivesExecutionPolicy needing to access " \
  "to_be_iterated_sequentially"

#ifdef PRIMITIVE_POLICY_WORKAROUND
#include "Simulator/Interface/Environments/Types.hpp"
#endif
/// All systems types
#include "Simulator/Interface/Systems/Types.hpp"
#include "Systems/Types.hpp"
///
/// Systems interface
#include "Systems/Block/Block.hpp"
#include "Systems/Block/Block/TypeTraits.hpp"
/// Interface methods
#include "Simulator/Interface/Systems/BlockRefs.hpp"
#include "Simulator/Interface/Systems/ExecutionPolicy.hpp"
// #include "Simulator/Interface/Protocols/Systems.hpp"
///
/// Serialization support
#include "Simulator/IO/Serialization/Types.hpp"
///
/// Profiling support
#include "ModuleSettings/Profiling.hpp"
///
#include "ErrorHandling/NestedExceptions.hpp"
#include "Utilities/CRTP.hpp"
#include "Utilities/Logging.hpp"
#include "Utilities/MakeArray.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/Requires.hpp"
#include "Utilities/Size.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace modules {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief The systems module
     * \ingroup modules systems
     *
     * The Systems module is a compulsary module in the \elastica Simulator
     * and is responsible for adding (aka registering) systems within the
     * Simulator
     *
     * \tparam SimTraits Options for configuration of the Simulator
     * \tparam Sim Simulator
     *
     * \usage
     * Sicne Systems is a compulsary member of Simulator, we have chosen to
     * integrate it directly into the simulator, so no work is necessary on
     * the consumer's side to use Systems. However, this module can also be
     * configured, and so it needs to be accompanied by an instantiation of
     * ::elastica::configuration::SystemsConfiguration as an argument to
     * ::elastica::make_simulator().
     *
     * \code
     * auto systems_configuration =
     * ::elastica::configuration::DefaultSystemsConfiguration{};
     *
     * auto sim = ::elastica::make_simulator<//no modules
     *                                      >(
     *  systems_configuration,
     * ::elastica::configuration::DefaultDomainConfiguration{},
     * );
     * \endcode
     * And the simulator `sim` now has capabilities of registering systems. The
     * sole entry point of registration of systems is the #emplace_back method
     * for which we show examples below.
     *
     * \see ::elastica::make_simulator()
     */
    template <class SimTraits, class Sim>
    class Systems : public CRTPHelper<Sim, Systems>,
                    // public ::tt::ConformsTo<
                    //    ::elastica::interface::protocols::Connections>,
                    public NonCopyable,
                    protected ::elastica::interface::systems::ExecutionPolicy<
                        SimTraits, Systems<SimTraits, Sim>> {
     private:
      //**Compile-time checks***************************************************
      //
      //************************************************************************

      //**Type definitions******************************************************
      //! This type
      using This = Systems<SimTraits, Sim>;
      //! CRTP Type
      using CRTP = CRTPHelper<Sim, Systems>;
      //! CRTP methods
      using CRTP::self;
      //! Internal size type
      using SizeType = typename SimTraits::SizeType;
      //! Internal type of index
      using IndexType = typename SimTraits::IndexType;
      //! Algorithm type
      using Algorithm =
          ::elastica::interface::systems::ExecutionPolicy<SimTraits, This>;
      //************************************************************************

     public:
      //**Type definitions******************************************************
      //! Type of simulation time
      using TimeType = typename SimTraits::TimeType;  // public for protocols
      //! Typelist of admissible plugins
      using AdmissibleSystemPlugins =
          typename SimTraits::AdmissibleSystemPlugins;
      //! Typelist of admissible systems
      //! \example <Sphere, Box, Cylinder, CosseratRod>
      using ListOfAdmissibleSystemCollections =
          typename SimTraits::ListOfAdmissibleSystemCollections;
      //! Policy type used for determining blocks
      using BlockingPolicy = typename SimTraits::BlockingPolicy;
      //! Virtualized type of blocking policy
      using RuntimeBlockingPolicy =
          decltype(::elastica::configuration::virtualize(
              std::declval<BlockingPolicy>()));
      //! Policy type used for determining iteration over and across systems
      using IterationPolicy = typename SimTraits::IterationPolicy;
      //************************************************************************

     protected:
      //************************************************************************
      /*!\name System Type definitions */
      //@{
      //! Metafunction for storage type of admissible systems
      template <typename SystemCollection>
      using AddSystemCollectionsType =
          typename SimTraits::template AddSystemStorageType<SystemCollection>;
      //! Tuple type of collection of admissible systems
      //! \example std::tuple<std::vector<Sphere>, std::vector<Box>, ... >
      using SystemCollections = tmpl::as_tuple<
          tmpl::transform<ListOfAdmissibleSystemCollections,
                          tmpl::bind<AddSystemCollectionsType, tmpl::_1>>>;
      //@}
      //************************************************************************

      //**Friendships***********************************************************
      //! Friend the Algorithm
      friend Algorithm;
#ifdef PRIMITIVE_POLICY_WORKAROUND
      friend ::elastica::interface::environments::WithPrimitivesExecutionPolicy<
          SimTraits, Sim>;
#endif
#undef PRIMITIVE_POLICY_WORKAROUND
      //! Friendship with Serialize
      friend struct ::elastica::io::Serialize<This>;
      //************************************************************************

     protected:
      //************************************************************************
      /*!\name Utility Metafunctions */
      //@{
      //! Metafunction for converting a Plugin to a Block
      template <typename Plugin>
      using as_block = blocks::Block<Plugin>;
      //! Generate blocks from plugins
      template <typename... Plugins>
      using as_block_wrapper = typename tmpl::list<as_block<Plugins>...>;
      //! Metafunction for converting Plugins to Blocks
      template <typename PluginsList>
      using as_blocks = tmpl::wrap<PluginsList, as_block_wrapper>;
      //! Metafunction to check if a plugin is valid
      template <typename Plugin>
      using an_admissible = std::bool_constant<
          // brigand's bool constant does not have constexpr bool() operator
          tmpl::list_contains<AdmissibleSystemPlugins, Plugin>::value>;
      //@}
      //************************************************************************

     protected:
      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{
      //************************************************************************
      /*!\brief Constructor of the Systems module.
       */
      Systems()
          : n_total_systems_(
                make_array<get_number_of_system_collections()>(0UL)),
            system_collection_containers_(),
            blocking_policy_(nullptr),
            iteration_policy_(nullptr) {
        using namespace ::elastica::logging;
        log<info>([](auto const& log) {
          log << "Successfully initialized the Systems module";
        });
      };
      //************************************************************************

      //************************************************************************
      /*!\brief Move constructor of the Systems class.
       *
       * \details This is not needed strictly (as it is ensured to be a
       * singleton class by the Simulator). However, while testing it is useful.
       */
      Systems(Systems&& other) noexcept
          : n_total_systems_(other.n_total_systems_),
            system_collection_containers_(
                std::move(other.system_collection_containers_)),
            blocking_policy_(std::move(other.blocking_policy_)),
            iteration_policy_(std::move(other.iteration_policy_)){};
      //************************************************************************

      //@}
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Get an invalid reference type for given a system type
       *
       * \details
       * Convenience function, used especially in case-studies, to get the type
       * and default value of a wrapped reference
       */
      template <typename Plugin, Requires<an_admissible<Plugin>{}> = nullptr>
      decltype(auto) get_invalid_ref() const noexcept {
        using RT = ::blocks::BlockSliceGenerator<Plugin>;
        return RT{};
      }
      //************************************************************************

     public:
      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      ~Systems() {
        using namespace ::elastica::logging;
        log<info>([](auto const& log) {
          log << "Successfully cleaned up the Systems module";
        });
      };
      //@}
      //************************************************************************

     public:  // TODO: Temporary change from protected
      //**Get methods***********************************************************
      /*!\name Get methods */
      //@{

      //************************************************************************
      /*!\brief Get method to return heterogeneous collection of systems
       *
       * \details
       * This method returns the entire storage, including all types registered
       * in the configuration. For internal use of \elastica developers, please
       * use with care.
       *
       * \return Mutable reference of the storage
       */
      inline constexpr auto get_system_collection_container() & noexcept
          -> SystemCollections& {
        return system_collection_containers_;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Get method to return heterogeneous collection of systems
       *
       * \details
       * This method returns the entire storage, including all types registered
       * in the configuration. For internal use of \elastica developers, please
       * use with care.
       *
       * \return Const reference of the storage
       */
      inline constexpr auto get_system_collection_container() const& noexcept
          -> SystemCollections const& {
        return system_collection_containers_;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Get method to return requested homogenous system collection
       *
       * \details
       * Get function for specific requested type of system container. For
       * internal use of \elastica developers, please use with care.
       *
       * \tparam SystemCollection Type of system collection requested
       * \return Mutable reference of the stored system collection
       */
      template <typename SystemCollection>
      inline constexpr auto get_system_collection_container() & noexcept
          -> AddSystemCollectionsType<SystemCollection>& {
        return std::get<AddSystemCollectionsType<SystemCollection>>(
            system_collection_containers_);
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Get method to return requested homogenous system collection
       *
       * \details
       * Get function for specific requested type of system container. For
       * internal use of \elastica developers, please use with care.
       *
       * \tparam SystemCollection Type of system collection requested
       * \return Const reference of the stored system collection
       */
      template <typename SystemCollection>
      inline constexpr auto get_system_collection_container() const& noexcept
          -> AddSystemCollectionsType<SystemCollection> const& {
        return std::get<AddSystemCollectionsType<SystemCollection>>(
            system_collection_containers_);
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Get method to return requested homogenous system collection
       *
       * \details
       * Get function for specific requested type of system container. For
       * internal use of \elastica developers, please use with care.
       *
       * \tparam SystemCollectionIdx Index of system collection requested
       * \return Mutable reference of the stored system collection
       */
      template <SizeType SystemCollectionIdx>
      inline constexpr auto get_system_collection_container() & noexcept
          -> std::tuple_element_t<SystemCollectionIdx, SystemCollections>& {
        return std::get<SystemCollectionIdx>(system_collection_containers_);
      }
      //************************************************************************

      //**Get type method*******************************************************
      /*!\brief Get method to return requested homogenous system collection
       *
       * \details
       * Get function for specific requested type of system container. For
       * internal use of \elastica developers, please use with care.
       *
       * \tparam SystemCollectionIdx Index of system collection requested
       * \return Const reference of the stored system collection
       *
       */
      template <SizeType SystemCollectionIdx>
      inline constexpr auto get_system_collection_container() const& noexcept
          -> std::tuple_element_t<SystemCollectionIdx,
                                  SystemCollections> const& {
        return std::get<SystemCollectionIdx>(system_collection_containers_);
      }
      //************************************************************************

      //@}
      //************************************************************************

      //**Interfaces************************************************************
      /*!\name Interface methods */
      //@{
     public:
      //**Get number of types***************************************************
      /*!\brief Get number of types of system (collections) in this Simulator
       */
      static constexpr SizeType get_number_of_system_collections() noexcept {
        return tmpl::size<ListOfAdmissibleSystemCollections>::value;
      }
      //************************************************************************

     protected:
      //************************************************************************
      /*!\brief Checks if the system is registered in the simulator
       *
       * \param system_slice_ref A ref to system to be checked for registration
       * \return True if system registered, else false
       */
      template <typename InterfaceType>
      inline auto check_if_registered_impl(
          InterfaceType const& system_ref) const noexcept -> bool {
        // two indirections here, to make check pass
        using Parent = typename InterfaceType::BlockType;

        const auto begin_ = get_system_collection_container<Parent>().cbegin();
        const auto end_ = get_system_collection_container<Parent>().cend();

        return std::find_if(
                   begin_, end_,
                   [&parent = std::as_const(system_ref.get().parent())](
                       Parent const& parent_in_collection) {
                     // compare references themselves
                     return &parent == &parent_in_collection;
                   }) != end_;
      }
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Checks if the system is registered in the simulator
       *
       * \param system_slice_ref A ref to system to be checked for registration
       * \return True if system registered, else false
       */
      template <typename SystemPlugin>
      inline auto check_if_registered(
          ::blocks::BlockSliceGenerator<SystemPlugin> const& system_slice_ref)
          const noexcept -> bool {
        return check_if_registered_impl(system_slice_ref);
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Checks if the system is registered in the simulator
       *
       * \param system_slice_ref A ref to system to be checked for registration
       * \return True if system registered, else false
       */
      template <typename SystemPlugin>
      inline auto check_if_registered(
          ::blocks::BlockViewGenerator<SystemPlugin> const& system_slice_ref)
          const noexcept -> bool {
        return check_if_registered_impl(system_slice_ref);
      }
      //************************************************************************

     protected:
      //************************************************************************
      /*!\brief Confirms if the system is registered in the simulator
       *
       * \param system A system to be checked for regisration
       * \exception std::runtime_error if the system is not registered in the
       * simulator
       */
      template <typename InterfaceType>
      inline auto confirm_as_registered(InterfaceType const& system) const
          -> void {
        if (not check_if_registered(system))
          throw std::runtime_error(
              "Input system was not found to be registered in the simulator");
      }
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Apply a callable over all system blocks
       *
       * \details
       * Applies a callable `func` over all system blocks
       * (across types and collections) registered in the current simulator.
       *
       * \note
       * This is meant for internal use mostly (as it applies `func` over
       * blocks instead of block slices), but is exposed to client code as it
       * can potentially improve speed.
       *
       * \param func Callable to be applied over system blocks
       */
      template <typename F>
      inline void for_each_block_untimed(F func) /*const noexcept*/ {
        using Alg = typename Algorithm::ApplyOverSystems;
        self().template get_algorithm<Alg>()(std::move(func));
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply a callable over all system blocks, with profiling enabled
       *
       * \copydetails for_each_block_untimed()
       */
      template <typename F>
      inline void for_each_block(F func) /*const noexcept*/ {
        using Alg = typename Algorithm::ApplyOverSystems;

        ELASTICA_PROFILING_SECTION { self().template get_timer<Alg>().start(); }
        for_each_block_untimed(std::move(func));
        ELASTICA_PROFILING_SECTION { self().template get_timer<Alg>().stop(); }
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply a callable over specific system blocks, with profiling
       * enabled
       *
       * \details
       * Applies a callable `func` over specific system block types given in
       * `BlocksTypeList`  registered in the current simulator. See
       * for_each_block_untimed() for a version over all blocks
       *
       * \note
       * This is meant for internal use mostly (as it applies `func` over
       * blocks instead of block slices), but is exposed to client code as it
       * can potentially improve speed.
       *
       * \tparam BlocksTypeList A `tmpl::list` of Block types associated with
       * the simulator
       * \param func Callable to be applied over system blocks
       */
      template <typename BlocksTypeList, typename F>
      inline void for_each_block_untimed(F func) /*const noexcept*/ {
        using Alg = typename Algorithm::ApplyOverSystems;
        self().template get_algorithm<Alg>().template
        operator()<BlocksTypeList>(std::move(func));
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply a callable over specific system blocks, with profiling
       * enabled
       *
       * \details
       * Applies a callable `func` over specific system block types given in
       * `BlocksTypeList`  registered in the current simulator with profiling
       * enabled.
       *
       * \note
       * This is meant for internal use mostly (as it applies `func` over
       * blocks instead of block slices), but is exposed to client code as it
       * can potentially improve speed.
       *
       * \tparam BlocksTypeList A `tmpl::list` of Block types associated with
       * the simulator
       * \param func Callable to be applied over system blocks
       */
      template <typename BlocksTypeList, typename F>
      inline void for_each_block(F func) /*const noexcept*/ {
        using Alg = typename Algorithm::ApplyOverSystems;

        ELASTICA_PROFILING_SECTION { self().template get_timer<Alg>().start(); }
        for_each_block_untimed<BlocksTypeList>(std::move(func));
        ELASTICA_PROFILING_SECTION { self().template get_timer<Alg>().stop(); }
      }
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Apply a callable over all systems
       *
       * \details
       * Applies a callable `func` over all systems
       * (across types and collections) registered in the current simulator.
       *
       * \param func Callable to be applied over systems
       */
      template <typename F>
      inline void for_each_system_untimed(F func) /*const noexcept*/ {
        for_each_block_untimed(
            ::blocks::for_each_slice_adapter(std::move(func)));
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply a callable over all systems, with profiling enabled
       * \copydetails for_each_system_untimed()
       */
      template <typename F>
      inline void for_each_system(F func) /*const noexcept*/ {
        for_each_block(::blocks::for_each_slice_adapter(std::move(func)));
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply a callable over a subset of systems
       *
       * \details
       * Applies a callable `func` over collections of the system type
       * `SystemsBlock` registered in the current simulator.
       *
       * \tparam PluginsTypeList A `tmpl::list` of Plugins over which to apply
       * the callable
       * \param func Callable to be applied over systems of type
       * `SystemsBlock`
       */
      template <typename PluginsTypeList, typename F>
      inline void for_each_system_untimed(F func) /*const noexcept*/ {
        for_each_block_untimed<as_blocks<PluginsTypeList>>(
            ::blocks::for_each_slice_adapter(std::move(func)));
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply a callable over a subset of systems, with profiling
       * enabled
       *
       * \details
       * Applies a callable `func` over collections of the system type
       * `SystemsBlock` registered in the current simulator.
       *
       * \tparam PluginsTypeList A `tmpl::list` of Plugins over which to apply
       * the callable
       * \param func Callable to be applied over systems of type
       * `SystemsBlock`
       */
      template <typename PluginsTypeList, typename F>
      inline void for_each_system(F func) /*const noexcept*/ {
        for_each_block<as_blocks<PluginsTypeList>>(
            ::blocks::for_each_slice_adapter(std::move(func)));
      }
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Apply a callable over all system blocks sequentially
       *
       * \details
       * Applies a callable `func` over all system blocks
       * (across types and collections) registered in the current simulator, but
       * sequentially
       *
       * \param func Callable to be applied over system blocks
       */
      template <typename F>
      inline void for_each_block_sequential_untimed(F func) /*const noexcept*/ {
        using Alg = typename Algorithm::ApplyOverSystemsSeq;
        // As we constrain our execution model to be  purely sequential , we
        // allow for non-const lambda execution via references.
        self().template get_algorithm<Alg>()(std::ref(func));
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply a callable over all system blocks sequentially, with
       * profiling enabled
       *
       * \copydetails for_each_block_untimed()
       */
      template <typename F>
      inline void for_each_block_sequential(F func) /*const noexcept*/ {
        using Alg = typename Algorithm::ApplyOverSystemsSeq;

        ELASTICA_PROFILING_SECTION { self().template get_timer<Alg>().start(); }
        for_each_block_sequential_untimed(std::move(func));
        ELASTICA_PROFILING_SECTION { self().template get_timer<Alg>().stop(); }
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply a callable over all systems sequentially
       *
       * \details
       * Applies a callable `func` over all systems
       * (across types and collections) registered in the current simulator.
       *
       * \param func Callable to be applied over systems
       */
      template <typename F>
      inline void for_each_system_sequential_untimed(
          F func) /*const noexcept*/ {
        // For_each adapter copies the func inside in an unmutable context
        // To allow for mutable functions, add a ref() here.
        for_each_block_sequential_untimed(
            ::blocks::for_each_slice_adapter(std::ref(func)));
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply a callable over all systems sequentially, with profiling
       * enabled \copydetails for_each_system_untimed()
       */
      template <typename F>
      inline void for_each_system_sequential(F func) /*const noexcept*/ {
        // For_each adapter copies the func inside in an unmutable context
        // To allow for mutable functions, add a ref() here.
        for_each_block_sequential(
            ::blocks::for_each_slice_adapter(std::ref(func)));
      }
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Update system kinematics
       *
       * \note
       * This is meant for internal use only.
       *
       * \param func Callable to be applied over systems
       */
      template <typename F>
      inline void for_each_block_update_kinematics(F func) /*const noexcept*/ {
        using Alg = typename Algorithm::UpdateKinematics;

        ELASTICA_PROFILING_SECTION { self().template get_timer<Alg>().start(); }
        self().template get_algorithm<Alg>()(std::move(func));
        ELASTICA_PROFILING_SECTION { self().template get_timer<Alg>().stop(); }
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Update system dynamics
       *
       * \note
       * This is meant for internal use only.
       *
       * \param func Callable to be applied over systems
       */
      template <typename F>
      inline void for_each_block_update_dynamics(F func) /*const noexcept*/ {
        using Alg = typename Algorithm::UpdateDynamics;

        ELASTICA_PROFILING_SECTION { self().template get_timer<Alg>().start(); }
        self().template get_algorithm<Alg>()(std::move(func));
        ELASTICA_PROFILING_SECTION { self().template get_timer<Alg>().stop(); }
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Computes internal system dynamics
       *
       * \note
       * This is meant for internal use only.
       *
       * \param func Callable to be applied over systems
       */
      template <typename F>
      inline void for_each_block_compute_internal_dynamics(
          F func) /*const noexcept*/ {
        using Alg = typename Algorithm::ComputeInternalDynamics;

        ELASTICA_PROFILING_SECTION { self().template get_timer<Alg>().start(); }
        self().template get_algorithm<Alg>()(std::move(func));
        ELASTICA_PROFILING_SECTION { self().template get_timer<Alg>().stop(); }
      }
      //************************************************************************

     protected:
      //************************************************************************
      /*!\brief Check if a collection should be iterated sequentially
       *
       * \param system_collection Collection of systems
       * \return bool true if collection should be iterated in a sequential
       * fashion, else else false
       */
      template <typename SystemCollection>
      inline auto to_be_iterated_sequentially(
          SystemCollection const& system_collection) const noexcept -> bool {
        return iteration_policy()(system_collection);
      }
      //************************************************************************

      //@}
      //************************************************************************

      //**Utility methods*******************************************************
      /*!\name Utility methods*/
      //@{
     public:
      //************************************************************************
      /*!\brief Access to the blocking policy
       *
       * \note Stealing resources from the return type using std::move() will
       * result in undefined behavior
       */
      inline auto blocking_policy() & noexcept -> RuntimeBlockingPolicy& {
        return blocking_policy_;
      }
      inline auto blocking_policy() const& noexcept
          -> RuntimeBlockingPolicy const& {
        return blocking_policy_;
      }
      //************************************************************************

     protected:
      //************************************************************************
      /*!\brief Access to the iteration, useful for serialization
       */
      inline auto iteration_policy() const& noexcept -> IterationPolicy const& {
        return *iteration_policy_;
      }
      inline auto iteration_policy() & noexcept -> IterationPolicy /*const*/& {
        return *iteration_policy_;
      }
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Sets the iteration policy, useful for initialization
       */
      inline auto iteration_policy_id() & noexcept
          -> std::unique_ptr<IterationPolicy /*const*/>& {
        return iteration_policy_;
      }
      //************************************************************************

      //@}
      //************************************************************************

     protected:
      //************************************************************************
      /*!\brief Display error message
       *
       * \tparam ImposedPrimitive Type of imposed primitive to emplace back
       */
      template <typename SystemCollection,
                bool B = tmpl::list_contains_v<
                    ListOfAdmissibleSystemCollections, SystemCollection>>
      static inline constexpr auto check_if_admissible() noexcept -> void {
        static_assert(B,
                      R"error(
The system that you are trying to add to the current Simulator is either unknown
to Elastica++ or not in the type-list of admitted systems! Please add them to
the above type-list, using SystemsConfiguration.
)error");
      }
      //************************************************************************

     public:
      //**Push back methods*****************************************************
      /*!\name Push back methods*/
      //@{

      //************************************************************************
      /*!\brief Interface for a push back of one system
       *
       * \details
       * Pushes back based on the `SystemCollection` passed. Does some static
       * checking for type safety.
       *
       * \param system \system to be pushed in
       * \return Mutable reference to the system pushed in
       */
      template <class SystemCollection,
                class D = std::decay_t<SystemCollection>>
      [[deprecated("Use emplace_back() instead")]] inline auto push_back(
          SystemCollection&& system) -> void {
        self().check_if_not_final();
        check_if_admissible<D>();

        auto& container = get_system_collection_container<D>();
        container.push_back(std::forward<SystemCollection>(system));
      }
      //************************************************************************

      //@}
      //************************************************************************

      //**Emplace methods*******************************************************
      /*!\name Emplace methods*/
      //@{

     private:
      //************************************************************************
      /*!\brief Implementation of emplace back for non-Block types
       *
       * \tparam SystemCollection Type of the system to be emplaced
       * \param args Arguments for the SystemCollection
       * \return Mutable reference to the system emplaced in
       */
      //      template <class SystemCollection, class... Args,
      //                Requires<not blocks::is_plugin_v<SystemCollection>> =
      //                nullptr>
      //      inline auto emplace_back_impl(Args&&... args) -> SystemCollection&
      //      {
      //        auto& system_collection_container =
      //            get_system_collection_container<SystemCollection>();
      //        system_collection_container.emplace_back(std::forward<Args>(args)...);
      //
      //        constexpr std::size_t n =
      //            tmpl::index_of<ListOfAdmissibleSystemCollections,
      //                           SystemCollection>::value;
      //        ++n_total_systems_[n];
      //
      //        // return non-owning handle to the System just pushed back
      //        return system_collection_container.back();
      //      }
      //************************************************************************

      //**Implementation of emplace back****************************************
      /*!\brief Implementation of emplace back for \elastica Block plugin types
       *
       * \tparam SystemPlugin Plugin to be emplaced in
       * \param args Arguments to the plugin
       * \return A view of the emplaced object
       */
      template <class SystemPlugin, class Initializer,
                Requires<blocks::is_plugin_v<SystemPlugin>> = nullptr>
      inline auto emplace_back_impl(Initializer&& initializer) {
        using Key = as_block<SystemPlugin>;
        auto& system_block_container = get_system_collection_container<Key>();
        auto system_block_it = blocking_policy_->get_iterator_to_block(
            system_block_container,
            ::elastica::size(std::forward<Initializer>(initializer)));

        // TODO : redundant
        constexpr std::size_t n =
            tmpl::index_of<ListOfAdmissibleSystemCollections, Key>::value;
        ++n_total_systems_[n];

        (void)(system_block_it->emplace_back(
            std::forward<Initializer>(initializer)));

        // block iterator
        using ::blocks::n_units;
        const auto index_of_block_in_collection = static_cast<std::size_t>(
            ::std::distance(system_block_container.begin(), system_block_it));
        ::blocks::BlockRefGenerator<SystemPlugin> ref_to_block(
            system_block_container, index_of_block_in_collection);
        return ::blocks::slice(ref_to_block, n_units(*system_block_it) - 1UL);
      }
      //************************************************************************

     protected:
      //************************************************************************
      //! Internal record of sizes for different system
      using NTotalSystems =
          std::array<SizeType, get_number_of_system_collections()>;
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Interface for emplacing a system
       *
       * \details
       * Constructs in place a system based on the explicitly passed
       * `SystemCollection` template. Does some static checking for type safety.
       * The usage is very similar to a C++17 std::vector's `emplace_back()`
       * method, with the major difference being the template parameter which
       * needs to be specified to know the type of the object to emplace
       * (mimicking the interface of std::make_unique, for example).
       * Another difference is in the return type---while a C++-17 vector's
       * `emplace_back()` returns a reference, here we might return a reference
       * or a view of the emplaced object.
       *
       * \tparam SystemCollection Type of the system to be emplaced
       * \param args Arguments for the system to be emplaced
       * \return A view/reference of the emplaced system, with the exact same
       *         interface
       */
      template <class SystemPlugin, class... Args,
                Requires<an_admissible<SystemPlugin>{}> = nullptr>
      inline decltype(auto) emplace_back(Args&&... args) {
        self().check_if_not_final();
        return emplace_back_impl<SystemPlugin>(std::forward<Args>(args)...);
      }
      //************************************************************************
      //@}
      //************************************************************************

      //**NSystems methods******************************************************
      /*!\name size methods*/
      //@{

     protected:
      //************************************************************************
      /*!\brief Access to the counting buffer, useful for serialization
       */
      inline auto n_total_systems() const& noexcept -> NTotalSystems const& {
        return n_total_systems_;
      }
      inline auto n_total_systems() & noexcept -> NTotalSystems /*const*/& {
        return n_total_systems_;
      }
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Gets the number of systems registered in the present
       * simulator
       */
      inline auto n_systems() const noexcept -> SizeType {
        return std::accumulate(std::begin(n_total_systems_),
                               std::end(n_total_systems_), 0UL);
      }
      //************************************************************************

      //@}
      //************************************************************************

     protected:
      //**Reset methods*********************************************************
      /*!\name Reset methods*/
      //@{

      //************************************************************************
      /*!\brief Resets the system module
       *
       * \param time_v The time of reset
       * \exception std::runtime_error
       */
      void reset_module(TimeType /*time_v*/) {
        std::fill(std::begin(n_total_systems()), std::end(n_total_systems()),
                  0UL);
        std::apply(
            [](auto&... system_container) {
              EXPAND_PACK_LEFT_TO_RIGHT(system_container.clear());
            },
            get_system_collection_container());
      }
      //************************************************************************

      //@}
      //************************************************************************

     private:
      //**Member variables******************************************************
      //! The total number of registered systems
      NTotalSystems n_total_systems_;
      //! Heterogeneous storage of homogeneous collection of systems
      SystemCollections system_collection_containers_;
      //// not marked const because we need serialization to work here and
      //// load up values
      //! Handle to blocking policy, initialized downstream!!
      RuntimeBlockingPolicy blocking_policy_;
      //! Handle to iteration policy, initialized downstream!!
      std::unique_ptr<IterationPolicy /*const*/> iteration_policy_;
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace modules

}  // namespace elastica
