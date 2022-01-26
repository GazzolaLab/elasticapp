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

/// All systems types
#include "Simulator/Interface/Systems/Types.hpp"
#include "Systems/Types.hpp"
///
/// Systems interface
#include "Systems/Block.hpp"  // needed?
#include "Systems/Block/Block/TypeTraits.hpp"
/// Interface methods
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
#include "Utilities/AsConst.hpp"
#include "Utilities/CRTP.hpp"
#include "Utilities/GSL.hpp"
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
      //************************************************************************

     protected:
      //************************************************************************
      /*!\name System Type definitions */
      //@{
      //! Metafunction for storage type of admissible systems
      template <typename SystemCollection>
      using AddSystemCollectionsType =
          typename SimTraits::template AddSystemStorageType<SystemCollection>;
      //! Typelist of admissible systems
      //! \example <Sphere, Box, Cylinder, CosseratRod>
      using ListOfAdmissibleSystemCollections =
          typename SimTraits::ListOfAdmissibleSystemCollections;
      //! Tuple type of collection of admissible systems
      //! \example std::tuple<std::vector<Sphere>, std::vector<Box>, ... >
      using SystemCollections = tmpl::as_tuple<
          tmpl::transform<ListOfAdmissibleSystemCollections,
                          tmpl::bind<AddSystemCollectionsType, tmpl::_1>>>;
      //! Policy type used for determining blocks
      using BlockingPolicy = typename SimTraits::BlockingPolicy;
      //! Policy type used for determining iteration over and across systems
      using IterationPolicy = typename SimTraits::IterationPolicy;
      //@}
      //************************************************************************

      //**Friendships***********************************************************
      //! Friend the Algorithm
      friend Algorithm;
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

     protected:
      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <typename Plugin>
      class LazyBlockSliceGenerator {
       private:
        friend This;
        using B = ::blocks::Block<Plugin>;
        using SourceCollection =
            AddSystemCollectionsType<B>;  // can be customized

       protected:
        LazyBlockSliceGenerator()
            : src_(nullptr),
              index_(std::numeric_limits<std::size_t>::max()),
              slice_index_(std::numeric_limits<std::size_t>::max()) {}

        LazyBlockSliceGenerator(SourceCollection& v, std::size_t index,
                                std::size_t slice_index)
            : src_(&v), index_(index), slice_index_(slice_index){};

       public:
        // generates a slice
        auto operator*() const {
          gsl::not_null<SourceCollection*> ptr_to_collection(src_);
          return *(::blocks::BlockIterator<Plugin>{
              &((*ptr_to_collection)[index_]), slice_index_});
        }
        auto operator->() const {
          gsl::not_null<SourceCollection*> ptr_to_collection(src_);
          return *(::blocks::BlockIterator<Plugin>{
              &((*ptr_to_collection)[index_]), slice_index_});
        }

       private:
        SourceCollection* src_;
        std::size_t index_;
        std::size_t slice_index_;
      };
      /*! \endcond */
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Get an invalid simulation iterator given a system type
       *
       * \details
       * For convenience, one can get an
       */
      template <typename Plugin>
      decltype(auto) get_invalid_iterator() const noexcept {
        using RT = LazyBlockSliceGenerator<Plugin>;
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

      //************************************************************************
      /*!\brief Checks if the system is registered in the simulator
       *
       * \param system_slice A system to be checked for registration
       * \return True if system registered, else false
       */
      template <typename SystemSlice>
      inline auto check_if_registered(
          SystemSlice const& system_slice) const noexcept -> bool {
        // todo alias it
        using Parent = typename SystemSlice::ParentBlock;

        const auto begin_ = get_system_collection_container<Parent>().cbegin();
        const auto end_ = get_system_collection_container<Parent>().cend();

        return std::find_if(begin_, end_,
                            [&parent = cpp17::as_const(system_slice.parent())](
                                Parent const& parent_in_collection) {
                              // compare references themselves
                              return &parent == &parent_in_collection;
                            }) != end_;
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
      template <typename SystemCollection>
      inline auto confirm_as_registered(SystemCollection const& system) const
          -> void {
        if (not check_if_registered(system))
          throw std::runtime_error(
              "Input system was not found to be registered in the simulator");
      }
      //************************************************************************

     protected:
      //************************************************************************
      /*!\brief Apply a callable over all system blocks
       *
       * \details
       * Applies a callable `func` over all system blocks
       * (across types and collections) registererd in the current simulator.
       *
       * \note
       * 1. We need strong exception safety guarantee from `func`. In C++14 this
       * cannot be strongly checked, so the user needs to satisfy this guarantee
       * 2. This is meant for internal use mostly (as it applies `func` over
       * blocks instead of block slices), but can be exposed to consumers if
       * needed.
       *
       * \param func Callable to be applied over system blocks
       */
      template <typename F>
      inline decltype(auto) for_each_block(F func) const noexcept {
        using Alg = typename Algorithm::ApplyOverSystems;

        ELASTICA_PROFILING_SECTION { self().template get_timer<Alg>().start(); }
        auto const& algorithm = self().template get_algorithm<Alg>();
        F another_func(algorithm(std::move(func)));
        ELASTICA_PROFILING_SECTION { self().template get_timer<Alg>().stop(); }

        return another_func;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply a callable over specific system blocks
       *
       * \details
       * Applies a callable `func` over specific system block types given in
       * `BlocksTypeList` (but still over the collection within this list)
       * registererd in the current simulator.
       *
       * \note
       * 1. We need strong exception safety guarantee from `func`. In C++14 this
       * cannot be strongly checked, so the user needs to satisfy this guarantee
       * 2. This is meant for internal use mostly (as it applies `func` over
       * blocks instead of block slices), but can be exposed to consumers if
       * needed.
       *
       * \tparam BlocksTypeList A `tmpl::list` of Block types associated with
       * the simulator
       * \param func Callable to be applied over system blocks
       */
      template <typename BlocksTypeList, typename F>
      inline decltype(auto) for_each_block(F func) const noexcept {
        using Alg = typename Algorithm::ApplyOverSystems;

        ELASTICA_PROFILING_SECTION { self().template get_timer<Alg>().start(); }
        auto const& algorithm = self().template get_algorithm<Alg>();
        F another_func(
            algorithm.template operator()<BlocksTypeList>(std::move(func)));
        ELASTICA_PROFILING_SECTION { self().template get_timer<Alg>().stop(); }

        return another_func;
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
       * \note
       * We need strong exception safety guarantee from `func`. In C++14 this
       * cannot be strongly checked, so the user needs to satisfy this guarantee
       *
       * \param func Callable to be applied over systems
       */
      template <typename F>
      inline decltype(auto) for_each_system(F func) const noexcept {
        (void)for_each_block(::blocks::for_each_slice_adapter(std::ref(func)));
        return func;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply a callable over all systems
       *
       * \details
       * Applies a callable `func` over collections of the system type
       * `SystemsBlock` registered in the current simulator.
       *
       * \note
       * We need strong exception safety guarantee from `func`. In C++14 this
       * cannot be strongly checked, so the user needs to satisfy this guarantee
       *
       * \tparam PluginsTypeList A `tmpl::list` of Plugins over which to apply
       * the callable
       * \param func Callable to be applied over systems of type
       * `SystemsBlock`
       */
      template <typename PluginsTypeList, typename F>
      inline decltype(auto) for_each_system(F func) const noexcept {
        (void)for_each_block<as_blocks<PluginsTypeList>>(
            ::blocks::for_each_slice_adapter(std::ref(func)));
        return func;
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
     protected:
      //************************************************************************
      /*!\brief Access to the blocking, useful for serialization
       */
      inline auto blocking_policy() const& noexcept -> BlockingPolicy const& {
        return *blocking_policy_;
      }
      inline auto blocking_policy() & noexcept -> BlockingPolicy /*const*/& {
        return *blocking_policy_;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Sets the blocking policy, useful for initialization
       */
      inline auto blocking_policy_id() & noexcept
          -> std::unique_ptr<BlockingPolicy /*const*/>& {
        return blocking_policy_;
      }
      //************************************************************************

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
      template <class SystemCollection, class... Args,
                Requires<not blocks::is_plugin_v<SystemCollection>> = nullptr>
      inline auto emplace_back_impl(Args&&... args) -> SystemCollection& {
        auto& system_collection_container =
            get_system_collection_container<SystemCollection>();
        system_collection_container.emplace_back(std::forward<Args>(args)...);

        constexpr std::size_t n =
            tmpl::index_of<ListOfAdmissibleSystemCollections,
                           SystemCollection>::value;
        ++n_total_systems_[n];

        // return non-owning handle to the System just pushed back
        return system_collection_container.back();
      }
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
        check_if_admissible<Key>();
        auto& system_block_container = get_system_collection_container<Key>();
        auto system_block_it = blocking_policy().get_iterator_to_block(
            system_block_container,
            ::elastica::size(std::forward<Initializer>(initializer)));

        // TODO : redundant
        constexpr std::size_t n =
            tmpl::index_of<ListOfAdmissibleSystemCollections, Key>::value;
        ++n_total_systems_[n];

        (void)(system_block_it->emplace_back(
            std::forward<Initializer>(initializer)));

        // block iterator
        const auto index_of_block_in_collection = static_cast<std::size_t>(
            ::std::distance(system_block_container.begin(), system_block_it));
        return LazyBlockSliceGenerator<SystemPlugin>(
            system_block_container, index_of_block_in_collection,
            // TODO : This code has linker issues, although it is the SOLID code
            // ::blocks::n_units(*system_block_it) - 1UL
            (*system_block_it).n_units() - 1UL);
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
      template <class SystemCollection, class... Args>
      inline decltype(auto) emplace_back(Args&&... args) {
        self().check_if_not_final();
        return emplace_back_impl<SystemCollection>(std::forward<Args>(args)...);
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
      void reset(TimeType /*time_v*/) {
        std::fill(std::begin(n_total_systems()), std::end(n_total_systems()),
                  0UL);
        cpp17::apply(
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
      std::unique_ptr<BlockingPolicy /*const*/> blocking_policy_;
      //! Handle to iteration policy, initialized downstream!!
      std::unique_ptr<IterationPolicy /*const*/> iteration_policy_;
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace modules

}  // namespace elastica
