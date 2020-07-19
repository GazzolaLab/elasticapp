#pragma once

#include <algorithm>
#include <array>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "Simulator/Environments/CollisionSystem/Contact/Contact.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/ContactVector.hpp"
//#include
//"Simulator/Environments/CollisionSystem/Primitives/SpherePrimitive.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/Coarse/Detectors.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/Fine/Detectors.hpp"
#include "Simulator/Environments/CollisionSystem/Response/DEMForces/TypeTraits.hpp"
#include "Simulator/Environments/CollisionSystem/Response/DEMSolver.hpp"
#include "Simulator/Environments/CollisionSystem/Tags.hpp"
#include "Transfer.hpp"
#include "Types.hpp"
#include "Utilities/MemoryMeter/MemoryMeter.hpp"
#include "elastica/DataStructures/TaggedTuple.hpp"
#include "elastica/ErrorHandling/Assert.hpp"
#include "elastica/Utilities/DefineTypes.h"
#include "elastica/Utilities/Logging/Logger.hpp"
#include "elastica/Utilities/MakeArray.hpp"
#include "elastica/Utilities/MemoryPool.hpp"
#include "elastica/Utilities/ProfilingSection.hpp"
#include "elastica/Utilities/Singleton.hpp"
#include "elastica/Utilities/StdHelpers.hpp"
#include "elastica/Utilities/TMPL.hpp"
#include "elastica/Utilities/TMPLDebugging.hpp"
#include "elastica/Utilities/Timing.h"

// TODO : Replace primitives with a tagged tuple
// TODO : std::get function for accessing primitives_? (not strictly needed)
// TODO : Symmetrize imposed primitives and primitives

namespace elastica {

  namespace collision {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Specialization of the collision system for the DEM solver.
     * \ingroup core
     *
     * This specialization of the CollisionSystem class template adapts the
     * collision system of the rigid body simulation world to the requirements
     * of the DEM solver.
     */
    // TODO : Note the public inheritance, this is to allow the Environemnts
    //        class to construct the CollisionSystem object. Consider another
    //        mechanism in the future
    template <
        // Type of the simulator configuration (everything except collision)
        class SimulatorConfig,
        // Type of the coarse collision detection algorithm
        template <typename> class CD,
        // Type of the fine collision detection algorithm
        typename FD,
        // Type of the batch generation algorithm
        template <typename> class BG,
        // Type of the configuration
        template <
            // Template signature of the coarse collision detection algorithm
            template <typename> class,
            // Template signature of the fine collision detection algorithm
            typename,
            // Template signature of the batch generation algorithm
            template <typename> class,
            // Template signature of the collision response algorithm
            template <typename, typename, typename> class>
        class C>
    class CollisionSystem<SimulatorConfig, C<CD, FD, BG, response::DEMSolver>>
        : public Singleton<CollisionSystem<SimulatorConfig,
                                           C<CD, FD, BG, response::DEMSolver>>,
                           logging::Logger> {
     public:
      //**Type definitions******************************************************
      using CollisionConfig =
          C<CD, FD, BG, response::DEMSolver>;  //!< Type of the configuration.
                                               //! Type of the coarse collision
                                               //! detection algorithm.

      using This = CollisionSystem<SimulatorConfig, CollisionConfig>;

      //! Type of the fine collision detector.
      /*! The type of the fine collision detection algorithm is selected by the
         setting of the pe::pe_FINE_COLLISION_DETECTOR macro. */
      using FineDetector = FD;

      //! Type of the batch generation algorithm.
      /*! The type of the batch generation algorithm is selected by the setting
         of the pe_BATCH_GENERATOR macro. */
      using BatchGenerator = BG<CollisionConfig>;

      // Other typedefs
      using ContactType =
          typename CollisionConfig::ContactType;  //!< Type of the contacts.
      using ContactID =
          typename CollisionConfig::ContactID;  //!< Handle to a contact.
      using ConstContactID =
          typename CollisionConfig::ConstContactID;  //!< Handle to a constant
                                                     //!< contact.
      // Specialization : stores ptrs of ContactType
      using Contacts = ContactVector<ContactType>;  //!< Container for contacts.

      using AdmissiblePrimitives =
          typename SimulatorConfig::AdmissiblePrimitives;
      //      using AdmissibleImposedPrimitives =
      //      typename SimulatorConfig::AdmissibleImposedPrimitives;

     private:
      //      // Some trait classes
      //      template <typename T>
      //      struct AddPrimitiveStorageType {
      //        // TODO : Customize
      //        using type = std::vector<T>;
      //      };

      //      using AdmissiblePrimitiveCollection =
      //          tmpl::transform<tmpl::remove_duplicates<AdmissiblePrimitives>,
      //                          AddPrimitiveStorageType<tmpl::_1>>;

      using PrimitiveOrdering = typename FineDetector::PrimitiveOrdering;

      template <typename S, typename E>
      struct RemoveRepeatedCartesianHelper;

      template <typename S, typename E1, typename E2>
      struct RemoveRepeatedCartesianHelper<S, tmpl::list<E1, E2>> {
        using type = typename std::conditional<
            std::is_same<tmpl::index_of<S, tmpl::list<E2, E1>>,
                         tmpl::no_such_type_>::value,
            tmpl::push_back<S, tmpl::list<E1, E2>>, S>::type;
      };

      // For the primitive - imposed primitive matrix
      // don't check only flips, also check for primitive ordering
      // TODO : FIX this, don't think its possible to fix too
      // WARNING : Not correct always
      // Only gives correct answers when both the lists being
      // cartesian product are same. In case one list is different
      // and we don't there exists a (E1, E2) pair but we want (E2, E1)
      // this doesn't even put it in the list and proceeds to compile
      template <typename S, typename E>
      struct OrderedCartesianHelper;

      template <typename S, typename E1, typename E2>
      struct OrderedCartesianHelper<S, tmpl::list<E1, E2>> {
        // Don't use conjunction, causes issue
        using type = typename std::conditional<
            // If reverse exists, don't bother
            std::is_same<tmpl::index_of<S, tmpl::list<E2, E1>>,
                         tmpl::no_such_type_>::value &&
                // If not in correct ordering don't bother
                tmpl::less_equal<tmpl::index_of<PrimitiveOrdering, E1>,
                                 tmpl::index_of<PrimitiveOrdering, E2>>::value,
            tmpl::push_back<S, tmpl::list<E1, E2>>, S>::type;
      };

     public:
      // DONE : Removing duplicates to be done upstream
      // Get a stable ordering so that Fine detection works with the available
      // methods
      // DONE : Push upstream

      // // OLD
      //      using Primitives = tmpl::sort<
      //          AdmissiblePrimitives,
      //          tmpl::less<tmpl::bind<tmpl::index_of,
      //          tmpl::pin<PrimitiveOrdering>,
      //                                tmpl::_1>,
      //                     tmpl::bind<tmpl::index_of,
      //                     tmpl::pin<PrimitiveOrdering>,
      //                                tmpl::_2>>>;
      //

      // Store only upper half to avoid redundancies while checking contact
      // DONE : Push upstream
      // ///// OLD
      //      using PrimitiveCollection =
      //          tmpl::transform<Primitives,
      //          AddPrimitiveStorageType<tmpl::_1>>;

      using PrimitiveCollection = typename SimulatorConfig::PrimitiveCollection;
      using ImposedPrimitiveCollection =
          typename SimulatorConfig::ImposedPrimitiveCollection;

      // Extract value type, sort the value types according to ordering,
      // and add storage type back again
      template <typename T>
      using RemovePrimitiveStorageType =
          typename SimulatorConfig::template RemovePrimitiveStorageType<T>;
      template <typename T>
      using AddPrimitiveStorageType =
          typename SimulatorConfig::template AddPrimitiveStorageType<T>;

      using OrderedPrimitiveCollection =
          // Inject storage type
          tmpl::transform<
              // Sort
              tmpl::sort<
                  // Extract type
                  tmpl::transform<
                      PrimitiveCollection,
                      tmpl::bind<RemovePrimitiveStorageType, tmpl::_1>>
                  // End Extract type,
                  ,
                  // sort key
                  tmpl::less<tmpl::bind<tmpl::index_of,
                                        tmpl::pin<PrimitiveOrdering>, tmpl::_1>,
                             tmpl::bind<tmpl::index_of,
                                        tmpl::pin<PrimitiveOrdering>, tmpl::_2>>
                  // End sort
                  >,
              tmpl::bind<AddPrimitiveStorageType, tmpl::_1>
              // End inject storage
              >;

      template <typename T>
      using RemoveImposedPrimitiveStorageType =
          typename SimulatorConfig::template RemoveImposedPrimitiveStorageType<
              T>;
      template <typename T>
      using AddImposedPrimitiveStorageType =
          typename SimulatorConfig::template AddImposedPrimitiveStorageType<T>;

      using OrderedImposedPrimitiveCollection =
          // Inject storage type
          tmpl::transform<
              // Sort
              tmpl::sort<
                  // Extract type
                  tmpl::transform<
                      ImposedPrimitiveCollection,
                      tmpl::bind<RemoveImposedPrimitiveStorageType, tmpl::_1>>
                  // End Extract type,
                  ,
                  // sort key
                  tmpl::less<tmpl::bind<tmpl::index_of,
                                        tmpl::pin<PrimitiveOrdering>, tmpl::_1>,
                             tmpl::bind<tmpl::index_of,
                                        tmpl::pin<PrimitiveOrdering>, tmpl::_2>>
                  // End sort
                  >,
              tmpl::bind<AddImposedPrimitiveStorageType, tmpl::_1>
              // End inject storage
              >;

      // Matrix types, its convenient to separate collision of imposed bodies
      // and non-imposed bodies for force reduction
      using PrimitivePrimitiveMatrix = tmpl::fold<
          tmpl::cartesian_product<OrderedPrimitiveCollection,
                                  OrderedPrimitiveCollection>,
          tmpl::list<>,
          RemoveRepeatedCartesianHelper<tmpl::_state, tmpl::_element>>;
      // SEE WARNING Above : Fix by having imposed primtiives always
      // trailing admissible primitives or making both equal and having
      // empty vectors
      //      TypeDisplayer<OrderedImposedPrimitiveCollection> td;
      using PrimitiveImposedPrimitiveMatrix = tmpl::fold<
          tmpl::cartesian_product<OrderedPrimitiveCollection,
                                  OrderedImposedPrimitiveCollection>,
          tmpl::list<>,
          RemoveRepeatedCartesianHelper<tmpl::_state, tmpl::_element>>;

      //  Storage types
      ////// OLD
      //// using PrimitiveStorage = tmpl::as_tuple<PrimitiveCollection>;
      using PrimitiveStorage = typename SimulatorConfig::PrimitiveStorage;
      using ImposedPrimitiveStorage =
          typename SimulatorConfig::ImposedPrimitiveStorage;

      // Stores the start indices of the currently filled primitives vector to
      // help with filling and unfilling them. Essentially maps the system to
      // the primitive
      // TODO Should ideally be a taggedTuple with std::size_t as member with
      // primitives as tags
      using AdmissibleSystemCollections =
          typename SimulatorConfig::AdmissibleSystemCollections;
      using CollectionIndices =
          std::array<std::size_t,
                     tmpl::size<AdmissibleSystemCollections>::value>;
      // Slightly dirty FIX :
      // Enhance the CollisionConfiguration with PrimitiveInformation to CD
      // This is fine since its internal to the collision system
      struct CoarseDetectorCollisionConfig : public CollisionConfig {
        // ///// Unused, dont pass
        //        using Primitives = This::Primitives;
        using PrimitivePrimitiveMatrix = This::PrimitivePrimitiveMatrix;
        using PrimitiveImposedPrimitiveMatrix =
            This::PrimitiveImposedPrimitiveMatrix;
        // Can add pointers here for a uniform interface
        using PrimitiveStorage = This::PrimitiveStorage;
        using ImposedPrimitiveStorage = This::ImposedPrimitiveStorage;
      };

      /*! The type of the coarse collision detection algorithm is selected by
         the setting of the pe::pe_COARSE_COLLISION_DETECTOR macro. */
      using CoarseDetector = CD<CoarseDetectorCollisionConfig>;

      struct ResponseCollisionConfig : public CollisionConfig {
        // If user doesn't want to override defaults, leave it be
        // else set to user overriden force model from client file

        //        template <typename T>
        //        struct FM_Helper_ {
        //          using type = typename T::ForceModel;
        //        };
        //

        // Cannot use conditional actively in a SFINAE context, hence enable
        // SFINAE lazily i.e. dont' do conditional<B, FM_Helper<A>::type,
        // FM_Helper<C>::type> but rather conditional<B, A, C>::NestedType;
        using ForceModel = typename std::conditional_t<
            response::tt::has_valid_force_model_member_v<SimulatorConfig>,
            SimulatorConfig, CollisionConfig>::ForceModel;
      };

      //! Type of the collision response algorithm.
      /*! The type of the collision response algorithm is selected by the
         setting of the pe::pe_CONTACT_SOLVER macro. */
      using ContactSolver = response::DEMSolver<ResponseCollisionConfig>;

     private:
      //**Constructor*********************************************************************************
      /*!\name Constructor */
      //@{
      explicit CollisionSystem();

      //@}
      //**********************************************************************************************

     public:
      //**Destructor**********************************************************************************
      /*!\name Destructor */
      //@{
      ~CollisionSystem();
      //@}
      //**********************************************************************************************

      //**Get
      // functions*******************************************************************************
      /*!\name Get functions */
      //@{
      inline auto get_coarse_detector() noexcept -> CoarseDetector&;
      inline auto get_contact_solver() noexcept -> ContactSolver&;
      static constexpr auto get_number_of_primitive_collections() noexcept
          -> std::size_t;
      // TODO : Functions seem common, maybe refactor?
      template <typename Primitive>
      inline constexpr auto get_primitive_collection() noexcept ->
          AddPrimitiveStorageType<Primitive>&;
      template <typename Primitive>
      inline constexpr auto get_primitive_collection() const noexcept ->
          AddPrimitiveStorageType<Primitive> const&;
      template <typename Primitive>
      inline constexpr auto get_offset_index() noexcept -> std::size_t&;
      template <typename Primitive>
      inline constexpr auto get_offset_index() const noexcept
          -> std::size_t const&;
      template <typename TimerTag>
      inline constexpr decltype(auto) get_timer() noexcept;
      static inline constexpr std::size_t get_number_of_timers() noexcept;
      template <typename MemoryMeterTag>
      inline constexpr decltype(auto) get_memory_meter() noexcept;
      static inline constexpr std::size_t
      get_number_of_memory_meters() noexcept;

      //@}
      //**********************************************************************************************

      //**Query
      // functions*****************************************************************************
      /*!\name Query functions */
      //@{
      void log_profiling_summary() const noexcept;
      //@}
      //**********************************************************************************************

      //**Utility
      // functions***************************************************************************
      /*!\name Utility functions */
      //@{
     public:
      inline void clear();
      inline void clear_contacts();
      template <template <typename...> class HeterogenousContainer,
                typename... CollectionContainers>
      void initialize(HeterogenousContainer<CollectionContainers...> const& cct,
                      ImposedPrimitiveStorage*);
      template <template <typename...> class HeterogenousContainer,
                typename TimeType, typename... CollectionContainers>
      void resolve(HeterogenousContainer<CollectionContainers...>& cct,
                   TimeType);

     private:
      template <typename Collection>
      void initialize_impl(Collection const&);
      // TODO noexcept?
      template <typename CollectionContainer>
      void transfer_info(tags::from_t /* meta */,
                         CollectionContainer const& collection_container);
      template <typename CollectionContainer>
      void transfer_info(tags::to_t /* meta */,
                         CollectionContainer& collection_container) const;

      //@}
      //**********************************************************************************************

     private:
      //**Member
      // variables****************************************************************************
      /*!\name Member variables */
      //@{
      PrimitiveStorage primitives_;  //!< The collection of primitives that are
                                     //!< updated as the simulation progresses
      ImposedPrimitiveStorage*
          imposed_primitives_;  //!< Pointer to the primitives that are fixed as
                                //!< the simulation progresses
      CollectionIndices offset_indices_to_primitives_;
      Contacts contacts_;  //!< The currently active contacts of the simulation
                           //!< world.
      CoarseDetector detector_;  //!< The active collision detection algorithm.
      ContactSolver solver_;     //!< The active collision response algorithm.

      // DONE Tagged tuple of timers
      using ElapsedTimeTags = tmpl::list<tags::TotalElapsedTimeTag,
                                         tags::TransferIntoElapsedTimeTag,
                                         tags::CollisionDetectionElapsedTimeTag,
                                         tags::CollisionResponseElapsedTimeTag,
                                         tags::TransferOutOfElapsedTimeTag>;
      using Timers = tmpl::as_tagged_tuple<ElapsedTimeTags>;
      Timers timers_;
      //      timing::WcTimer time_collision_detection_,
      //          time_collision_response_, time_transfer_from_,
      //          time_transfer_to_;
      //      std::vector<timing::WcTimer*> timers_;

      using MemoryMeterTags = tmpl::list<tags::CollisionDetectionMemoryTag,
                                         tags::CollisionResponseMemoryTag>;
      using MemoryMeters = tmpl::as_tagged_tuple<MemoryMeterTags>;
      MemoryMeters memory_meters_;

      //      MemoryMeter mem_collision_detection_, mem_collision_response_;
      //      std::vector<MemoryMeter*> memoryMeters_;

      BEFRIEND_SINGLETON;
    };

    //=================================================================================================
    //
    //  CONSTRUCTOR
    //
    //=================================================================================================

    //*************************************************************************************************
    /*!\brief Constructor of the CollisionSystem class.
     */
    template <
        // Simulator Configuration
        class SimulatorConfig,
        // Type of the coarse collision detection algorithm
        template <typename> class CD,
        // Type of the fine collision detection algorithm
        typename FD,
        // Type of the batch generation algorithm
        template <typename> class BG,
        template <
            // Template signature of the coarse collision detection algorithm
            template <typename> class,
            // Template signature of the fine collision detection algorithm
            typename,
            // Template signature of the batch generation algorithm
            template <typename> class,
            // Template signature of the collision response algorithm
            template <typename, typename, typename> class>
        class C>  // Type of the configuration
    CollisionSystem<SimulatorConfig,
                    C<CD, FD, BG, response::DEMSolver>>::CollisionSystem()
        :  // Initialization of the Singleton  base object
          Singleton<CollisionSystem, logging::Logger>(),
          // The list of primitives
          primitives_(),
          // Pointer to fixed primitives
          imposed_primitives_(nullptr),
          // The offset indices of the registered systems into the primitive
          // collection
          offset_indices_to_primitives_(
              make_array<tmpl::size<AdmissibleSystemCollections>::value>(0UL)),
          // The currently active contacts of the simulation world
          contacts_(5000),
          // The active collision detection algorithm
          detector_(primitives_, imposed_primitives_),
          // The active collision response algorithm
          solver_(),
          // TODO Does the tagged tuple constructor do the right thing?
          // Active time profilers
          timers_(),
          // Active memory profilers
          memory_meters_() {
      // Registering all timers
      //      timers_.push_back(&time_transfer_from_);
      //      timers_.push_back(&time_collision_detection_);
      //      timers_.push_back(&time_collision_response_);
      //      timers_.push_back(&time_transfer_to_);

      // Registering all memory meters
      //      memoryMeters_.push_back(&mem_collision_detection_);
      //      memoryMeters_.push_back(&mem_collision_response_);

      // Logging the successful setup of the collision system
      LOG_INFO_SECTION(log) {
        log << "Successfully initialized the collision system instance";
      }
    }
    //*************************************************************************************************

    //=================================================================================================
    //
    //  DESTRUCTOR
    //
    //=================================================================================================
    //*************************************************************************************************
    /*!\brief Destructor of the CollisionSystem class.
     */
    template <
        // Simulator Configuration
        class SimulatorConfig,
        // Type of the coarse collision detection algorithm
        template <typename> class CD,
        // Type of the fine collision detection algorithm
        typename FD,
        // Type of the batch generation algorithm
        template <typename> class BG,
        template <
            // Template signature of the coarse collision detection algorithm
            template <typename> class,
            // Template signature of the fine collision detection algorithm
            typename,
            // Template signature of the batch generation algorithm
            template <typename> class,
            // Template signature of the collision response algorithm
            template <typename, typename, typename> class>
        class C>  // Type of the configuration
    CollisionSystem<SimulatorConfig,
                    C<CD, FD, BG, response::DEMSolver>>::~CollisionSystem() {
      // Clearing the collision system
      clear();

      clear_contacts();

      // Non owning pointer, hence simply set it to null
      imposed_primitives_ = nullptr;

      // Logging the successful destruction of the collision system
      LOG_INFO_SECTION(log) {
        log << "Successfully destroyed the collision system instance";
      }
    }
    //*************************************************************************************************

    //=================================================================================================
    //
    //  GET FUNCTIONS
    //
    //=================================================================================================
    //*************************************************************************************************
    /*!\brief Returns the currently active coarse collision detector.
     *
     * \return Reference to the active coarse collision detector.
     *
     * With this function it is possible to access the currently active coarse
     collision detector.
     * The following code shows an example of how to use this function to
     acquire a reference to the
     * coarse collision detector:

       \code
       pe::CollisionSystemID collisionSystem = pe::theCollisionSystem();
       pe::CoarseDetector& coarseDetector =
     collisionSystem->getCoarseDetector(); \endcode
     */
    template <
        // Simulator Configuration
        class SimulatorConfig,
        // Type of the coarse collision detection algorithm
        template <typename> class CD,
        // Type of the fine collision detection algorithm
        typename FD,
        // Type of the batch generation algorithm
        template <typename> class BG,
        template <
            // Template signature of the coarse collision detection algorithm
            template <typename> class,
            // Template signature of the fine collision detection algorithm
            typename,
            // Template signature of the batch generation algorithm
            template <typename> class,
            // Template signature of the collision response algorithm
            template <typename, typename, typename> class>
        class C>  // Type of the configuration
    inline typename CollisionSystem<
        SimulatorConfig, C<CD, FD, BG, response::DEMSolver>>::CoarseDetector&
    CollisionSystem<SimulatorConfig, C<CD, FD, BG, response::DEMSolver>>::
        get_coarse_detector() noexcept {
      return detector_;
    }
    //*************************************************************************************************
    /*!\brief Returns the currently active contact solver.
     *
     * \return Reference to the active contact solver.
     *
     * With this function it is possible to access the currently active contact
     solver. The
     * following code shows an example of how to use this function to acquire a
     reference to
     * the contact solver:

       \code
       pe::CollisionSystemID collisionSystem = pe::theCollisionSystem();
       pe::ContactSolver& contactSolver = collisionSystem->getContactSolver();
       \endcode
     */
    template <
        // Simulator Configuration
        class SimulatorConfig,
        // Type of the coarse collision detection algorithm
        template <typename> class CD,
        // Type of the fine collision detection algorithm
        typename FD,
        // Type of the batch generation algorithm
        template <typename> class BG,
        template <
            // Template signature of the coarse collision detection algorithm
            template <typename> class,
            // Template signature of the fine collision detection algorithm
            typename,
            // Template signature of the batch generation algorithm
            template <typename> class,
            // Template signature of the collision response algorithm
            template <typename, typename, typename> class>
        class C>  // Type of the configuration
    inline typename CollisionSystem<
        SimulatorConfig, C<CD, FD, BG, response::DEMSolver>>::ContactSolver&
    CollisionSystem<SimulatorConfig, C<CD, FD, BG, response::DEMSolver>>::
        get_contact_solver() noexcept {
      return solver_;
    }
    //*************************************************************************************************

    template <
        // Simulator Configuration
        class SimulatorConfig,
        // Type of the coarse collision detection algorithm
        template <typename> class CD,
        // Type of the fine collision detection algorithm
        typename FD,
        // Type of the batch generation algorithm
        template <typename> class BG,
        template <
            // Template signature of the coarse collision detection algorithm
            template <typename> class,
            // Template signature of the fine collision detection algorithm
            typename,
            // Template signature of the batch generation algorithm
            template <typename> class,
            // Template signature of the collision response algorithm
            template <typename, typename, typename> class>
        class C>  // Type of the configuration
    constexpr std::size_t
    CollisionSystem<SimulatorConfig, C<CD, FD, BG, response::DEMSolver>>::
        get_number_of_primitive_collections() noexcept {
      return tmpl::size<PrimitiveStorage>::value;
    };

    template <
        // Simulator Configuration
        class SimulatorConfig,
        // Type of the coarse collision detection algorithm
        template <typename> class CD,
        // Type of the fine collision detection algorithm
        typename FD,
        // Type of the batch generation algorithm
        template <typename> class BG,
        template <
            // Template signature of the coarse collision detection algorithm
            template <typename> class,
            // Template signature of the fine collision detection algorithm
            typename,
            // Template signature of the batch generation algorithm
            template <typename> class,
            // Template signature of the collision response algorithm
            template <typename, typename, typename> class>
        class C>  // Type of the configuration
    template <typename Primitive>
    inline constexpr auto CollisionSystem<
        SimulatorConfig,
        C<CD, FD, BG, response::DEMSolver>>::get_primitive_collection() noexcept
        -> AddPrimitiveStorageType<Primitive>& {
      using PrimitiveCollectionType = AddPrimitiveStorageType<Primitive>;
      return std::get<PrimitiveCollectionType>(primitives_);
    }

    template <
        // Simulator Configuration
        class SimulatorConfig,
        // Type of the coarse collision detection algorithm
        template <typename> class CD,
        // Type of the fine collision detection algorithm
        typename FD,
        // Type of the batch generation algorithm
        template <typename> class BG,
        template <
            // Template signature of the coarse collision detection algorithm
            template <typename> class,
            // Template signature of the fine collision detection algorithm
            typename,
            // Template signature of the batch generation algorithm
            template <typename> class,
            // Template signature of the collision response algorithm
            template <typename, typename, typename> class>
        class C>  // Type of the configuration
    template <typename Primitive>
    inline constexpr auto
    CollisionSystem<SimulatorConfig, C<CD, FD, BG, response::DEMSolver>>::
        get_primitive_collection() const noexcept ->
        AddPrimitiveStorageType<Primitive> const& {
      using PrimitiveCollectionType = AddPrimitiveStorageType<Primitive>;
      return std::get<PrimitiveCollectionType>(primitives_);
    }

    template <
        // Simulator Configuration
        class SimulatorConfig,
        // Type of the coarse collision detection algorithm
        template <typename> class CD,
        // Type of the fine collision detection algorithm
        typename FD,
        // Type of the batch generation algorithm
        template <typename> class BG,
        template <
            // Template signature of the coarse collision detection algorithm
            template <typename> class,
            // Template signature of the fine collision detection algorithm
            typename,
            // Template signature of the batch generation algorithm
            template <typename> class,
            // Template signature of the collision response algorithm
            template <typename, typename, typename> class>
        class C>  // Type of the configuration
    template <typename Collection>
    inline constexpr auto CollisionSystem<
        SimulatorConfig,
        C<CD, FD, BG, response::DEMSolver>>::get_offset_index() noexcept
        -> std::size_t& {
      return std::get<
          tmpl::index_of<AdmissibleSystemCollections, Collection>::value>(
          offset_indices_to_primitives_);
    }

    template <
        // Simulator Configuration
        class SimulatorConfig,
        // Type of the coarse collision detection algorithm
        template <typename> class CD,
        // Type of the fine collision detection algorithm
        typename FD,
        // Type of the batch generation algorithm
        template <typename> class BG,
        template <
            // Template signature of the coarse collision detection algorithm
            template <typename> class,
            // Template signature of the fine collision detection algorithm
            typename,
            // Template signature of the batch generation algorithm
            template <typename> class,
            // Template signature of the collision response algorithm
            template <typename, typename, typename> class>
        class C>  // Type of the configuration
    template <typename Collection>
    inline constexpr auto CollisionSystem<
        SimulatorConfig, C<CD, FD, BG, response::DEMSolver>>::get_offset_index()
        const noexcept -> std::size_t const& {
      return std::get<
          tmpl::index_of<AdmissibleSystemCollections, Collection>::value>(
          offset_indices_to_primitives_);
    }

    //=================================================================================================
    //
    //  UTILITY FUNCTIONS
    //
    //=================================================================================================

    //*************************************************************************************************
    /*!\brief Clears the collision system.
     *
     * \return void
     */
    template <
        // Simulator Configuration
        class SimulatorConfig,
        // Type of the coarse collision detection algorithm
        template <typename> class CD,
        // Type of the fine collision detection algorithm
        typename FD,
        // Type of the batch generation algorithm
        template <typename> class BG,
        template <
            // Template signature of the coarse collision detection algorithm
            template <typename> class,
            // Template signature of the fine collision detection algorithm
            typename,
            // Template signature of the batch generation algorithm
            template <typename> class,
            // Template signature of the collision response algorithm
            template <typename, typename, typename> class>
        class C>  // Type of the configuration
    inline void CollisionSystem<SimulatorConfig,
                                C<CD, FD, BG, response::DEMSolver>>::clear() {
      // Cleanup coarse detector
      detector_.clear();

      constexpr std::size_t n_primitive_collections(
          get_number_of_primitive_collections());

      //      // v is a vector of pointers of type T
      //      const auto delete_and_clear = [](auto& v) {
      //        using vector_type = decltype(v);
      //        using value_type =
      //            std::remove_pointer_t<typename vector_type::value_type>;
      //        std::for_each(v.begin(), v.end(),
      //        std::default_delete<value_type>());
      //        // No need for setting to nullptr
      //        v.clear();
      //      };
      //
      //      // Clear the primitives stored in storage
      //      ::index_apply<n_primitive_collections>(
      //          [primitives_ = &(this->primitives_), delete_and_clear](auto...
      //          Is) {
      //            EXPAND_PACK_LEFT_TO_RIGHT(
      //                delete_and_clear(std::get<Is>(primitives_)));
      //          });

      ::index_apply<n_primitive_collections>([this](auto... Is) {
        EXPAND_PACK_LEFT_TO_RIGHT(std::get<Is>(primitives_).clear());
      });
    }
    //*************************************************************************************************

    //*************************************************************************************************
    /*!\brief Clearing the contacts and batches of the simulation world.
     *
     * \return void.
     *
     * This function destroys all currently active contacts of the simulation
     * world and resets the generated batches.
     */
    template <
        // Simulator Configuration
        class SimulatorConfig,
        // Type of the coarse collision detection algorithm
        template <typename> class CD,
        // Type of the fine collision detection algorithm
        typename FD,
        // Type of the batch generation algorithm
        template <typename> class BG,
        template <
            // Template signature of the coarse collision detection algorithm
            template <typename> class,
            // Template signature of the fine collision detection algorithm
            typename,
            // Template signature of the batch generation algorithm
            template <typename> class,
            // Template signature of the collision response algorithm
            template <typename, typename, typename> class>
        class C>  // Type of the configuration
    inline void CollisionSystem<
        SimulatorConfig, C<CD, FD, BG, response::DEMSolver>>::clear_contacts() {
      // Needs a ptr based implementation for memory pooling for now
      std::for_each(contacts_.begin(), contacts_.end(),
                    std::default_delete<ContactType>());
      contacts_.clear();
    }
    //*************************************************************************************************

    template <
        // Simulator Configuration
        class SimulatorConfig,
        // Type of the coarse collision detection algorithm
        template <typename> class CD,
        // Type of the fine collision detection algorithm
        typename FD,
        // Type of the batch generation algorithm
        template <typename> class BG,
        template <
            // Template signature of the coarse collision detection algorithm
            template <typename> class,
            // Template signature of the fine collision detection algorithm
            typename,
            // Template signature of the batch generation algorithm
            template <typename> class,
            // Template signature of the collision response algorithm
            template <typename, typename, typename> class>
        class C>  // Type of the configuration
    template <typename CollectionContainer>
    void CollisionSystem<SimulatorConfig, C<CD, FD, BG, response::DEMSolver>>::
        initialize_impl(CollectionContainer const& collection_container) {
      using CollectionType =
          std::remove_pointer_t<typename CollectionContainer::value_type>;
      using PrimitiveType = typename CollectionType::PrimitiveType;

      // 1. Get reference to a vector of primitives
      auto& primitive_collection_ = get_primitive_collection<PrimitiveType>();
      std::size_t n_primitives(primitive_collection_.size());

      // 2. Store the number of currently referenced primitives to the offset
      // indices
      get_offset_index<CollectionType>() = n_primitives;

      // 2. reserve storage based on collection of containers
      std::for_each(collection_container.cbegin(), collection_container.cend(),
                    [&n_primitives](auto const& collection) {
                      n_primitives += collection.size();
                    });

      // Needs the PrimitiveType to be move inserteable according to the
      // standards Our primitive types cannot (and should not be copied), hence
      // we provide an explicit move constructor for allocation of memory
      primitive_collection_.reserve(n_primitives);

      // 3. Create primitives, no need to transfer yet (achieved in first
      // simulation step)
      // TODO Refactor into separate ADD function
      std::for_each(collection_container.cbegin(), collection_container.cend(),
                    [&primitive_collection_](auto const& collection) {
                      std::size_t end = collection.size();
                      for (std::size_t i = 0UL; i < end; ++i) {
                        id_t sid(0UL), uid(0UL);
                        bool finite(true);
                        // TODO : Tentative, collection [i] doesn't work for now
                        // auto
                        // materialID(collection[i].get_material());
                        const auto materialID(collection.get_material(i));
                        primitive_collection_.emplace_back(sid, uid, finite,
                                                           materialID);
                      }
                    });
    }

    template <
        // Simulator Configuration
        class SimulatorConfig,
        // Type of the coarse collision detection algorithm
        template <typename> class CD,
        // Type of the fine collision detection algorithm
        typename FD,
        // Type of the batch generation algorithm
        template <typename> class BG,
        template <
            // Template signature of the coarse collision detection algorithm
            template <typename> class,
            // Template signature of the fine collision detection algorithm
            typename,
            // Template signature of the batch generation algorithm
            template <typename> class,
            // Template signature of the collision response algorithm
            template <typename, typename, typename> class>
        class C>  // Type of the configuration
    template <template <typename...> class HeterogenousContainer,
              typename... CollectionContainers>
    void CollisionSystem<SimulatorConfig, C<CD, FD, BG, response::DEMSolver>>::
        initialize(HeterogenousContainer<CollectionContainers...> const& cct,
                   ImposedPrimitiveStorage* ips) {
      // 1. Given the heterogenous container of a container of collections from
      // the main simulator, it can initialize all primitives for contact
      // detection NOTE: Expansion has to take place from left to right to
      // introduce ordering in the system in case there are multiple collections
      // with the same primitive type
      EXPAND_PACK_LEFT_TO_RIGHT(
          initialize_impl(std::get<CollectionContainers>(cct)));

      // 2. Also initialize the fixed primitives_
      imposed_primitives_ = ips;

      // 3. Pass the fixed prim

      // 3. Once initialized, start the system by clearning all contacts
      clear_contacts();
    }

    // TODO Remove function from collision detector, keep track of indices
    //      Do before the initialization step for ease of managing the blocks
    //      To get the ID of the object, its better to have a struct with the
    //      CollectionID and UID (or pointers alternatively)

    // Was initially templated for the MessageTag but we lost const correctness
    // so revert back to overloading
    template <
        // Simulator Configuration
        class SimulatorConfig,
        // Type of the coarse collision detection algorithm
        template <typename> class CD,
        // Type of the fine collision detection algorithm
        typename FD,
        // Type of the batch generation algorithm
        template <typename> class BG,
        template <
            // Template signature of the coarse collision detection algorithm
            template <typename> class,
            // Template signature of the fine collision detection algorithm
            typename,
            // Template signature of the batch generation algorithm
            template <typename> class,
            // Template signature of the collision response algorithm
            template <typename, typename, typename> class>
        class C>  // Type of the configuration
    template <typename CollectionContainer>
    void CollisionSystem<SimulatorConfig, C<CD, FD, BG, response::DEMSolver>>::
        transfer_info(tags::from_t /* meta */,
                      CollectionContainer const& collection_container) {
      using CollectionType =
          std::remove_pointer_t<typename CollectionContainer::value_type>;
      using PrimitiveType = typename CollectionType::PrimitiveType;

      // 1. Get reference to a vector of primitives
      auto& primitive_collection_ = get_primitive_collection<PrimitiveType>();

      // Get offset into the primitive array, makes it easily parallelizable and
      // thread safe instead of having an incremental counter here
      using IteratorType = decltype(primitive_collection_.begin());
      using DifferenceType =
          typename std::iterator_traits<IteratorType>::difference_type;
      // WARNING : The assumption here is that the offset index is not too big
      // for the iterator of the stored container of primtives, hence we static
      // cast it
      const auto offset =
          static_cast<DifferenceType>(get_offset_index<CollectionType>());
      auto start_iterator = (primitive_collection_.begin() + offset);

      // TODO : Parallel transfer here
      // For all collections in the collection vector
      for (auto const& collection : collection_container) {
        // 2. Calculate start and end iterators for the current collection
        auto end_iterator =
            start_iterator + static_cast<DifferenceType>(collection.size());
        ELASTICA_ASSERT(end_iterator <= primitive_collection_.end(),
                        "Invalid iterator beyond end of collection");

        // 3. Do transfer
        transfer(from, collection, to, start_iterator, end_iterator);

        // Update start iterator to point to the next element
        // Don't use next!
        std::exchange(start_iterator, end_iterator);
      }
    }

    template <
        // Simulator Configuration
        class SimulatorConfig,
        // Type of the coarse collision detection algorithm
        template <typename> class CD,
        // Type of the fine collision detection algorithm
        typename FD,
        // Type of the batch generation algorithm
        template <typename> class BG,
        template <
            // Template signature of the coarse collision detection algorithm
            template <typename> class,
            // Template signature of the fine collision detection algorithm
            typename,
            // Template signature of the batch generation algorithm
            template <typename> class,
            // Template signature of the collision response algorithm
            template <typename, typename, typename> class>
        class C>  // Type of the configuration
    template <typename CollectionContainer>
    void CollisionSystem<SimulatorConfig, C<CD, FD, BG, response::DEMSolver>>::
        transfer_info(tags::to_t /* meta */,
                      CollectionContainer& collection_container) const {
      using CollectionType =
          std::remove_pointer_t<typename CollectionContainer::value_type>;
      using PrimitiveType = typename CollectionType::PrimitiveType;

      // 1. Get reference to a vector of primitives
      auto& primitive_collection_ = get_primitive_collection<PrimitiveType>();

      // Get offset into the primitive array, makes it easily parallelizable and
      // thread safe instead of having an incremental counter here
      using IteratorType = decltype(primitive_collection_.cbegin());
      using DifferenceType =
          typename std::iterator_traits<IteratorType>::difference_type;
      // TODO : Safe convert
      //  WARNING : The assumption here is that the offset index is not too big
      // for the iterator of the stored container of primtives, hence we static
      // cast it
      const auto offset =
          static_cast<DifferenceType>(get_offset_index<CollectionType>());
      auto start_iterator = (primitive_collection_.cbegin() + offset);

      // TODO : Parallel transfer here
      // For all collections in the collection vector
      for (auto& collection : collection_container) {
        // 2. Calculate start and end iterators for the current collection
        auto end_iterator =
            start_iterator + static_cast<DifferenceType>(collection.size());
        ELASTICA_ASSERT(end_iterator <= primitive_collection_.cend(),
                        "Invalid iterator beyond end of collection");

        // 3. Do transfer
        transfer(to, collection, from, start_iterator, end_iterator);

        // Update start iterator to point to the next element
        // Don't use next!
        std::exchange(start_iterator, end_iterator);
      }
    }

    // TODO : noexcept guarnatees?
    template <
        // Simulator Configuration
        class SimulatorConfig,
        // Type of the coarse collision detection algorithm
        template <typename> class CD,
        // Type of the fine collision detection algorithm
        typename FD,
        // Type of the batch generation algorithm
        template <typename> class BG,
        template <
            // Template signature of the coarse collision detection algorithm
            template <typename> class,
            // Template signature of the fine collision detection algorithm
            typename,
            // Template signature of the batch generation algorithm
            template <typename> class,
            // Template signature of the collision response algorithm
            template <typename, typename, typename> class>
        class C>  // Type of the configuration
    template <template <typename...> class HeterogenousContainer,
              typename TimeType, typename... CollectionContainers>
    void CollisionSystem<SimulatorConfig, C<CD, FD, BG, response::DEMSolver>>::
        resolve(HeterogenousContainer<CollectionContainers...>& cct,
                TimeType /* time*/) {
      // NOTE : cct cannot be const in this scope because we also transfer
      // information to it at the end of the day
      // TODO : Parallel pipeline?
      using namespace tags;

      PROFILING_SECTION {
        get_timer<TotalElapsedTimeTag>().start();
        get_timer<TransferIntoElapsedTimeTag>().start();
      }

      // 1. Transfer body to primitive
      // TODO Parallel invoke on transfer?
      EXPAND_PACK_LEFT_TO_RIGHT(
          transfer_info(from, std::get<CollectionContainers>(cct)));

      // 2. Detect collision
      LOG_DEBUG_SECTION(log) { log << "Detecting contacts...\n"; }

      PROFILING_SECTION {
        get_timer<TransferIntoElapsedTimeTag>().stop();
        get_timer<CollisionDetectionElapsedTimeTag>().start();
        get_memory_meter<CollisionDetectionMemoryTag>().start();
      }

      detector_.find_contacts(contacts_);

      PROFILING_SECTION {
        get_timer<CollisionDetectionElapsedTimeTag>().stop();
        //        time_collision_detection_.stop();
        get_memory_meter<CollisionDetectionMemoryTag>().stop();
        //        mem_collision_detection_.stop();
        get_timer<CollisionResponseElapsedTimeTag>().start();
        //        time_collision_response_.start();
        get_memory_meter<CollisionResponseMemoryTag>().start();
        //        mem_collision_response_.start();
      }
      // 3. Resolve collision
      // TODO : Future : Figure out passing max_overlap to time-step limiters
      //                 algorithms
      // real_t max_overlap =
      solver_.resolve_contacts(contacts_);

      PROFILING_SECTION {
        get_timer<CollisionResponseElapsedTimeTag>().stop();
        //        time_collision_response_.stop();
        get_memory_meter<CollisionResponseMemoryTag>().stop();
        //        mem_collision_response_.stop();
        get_timer<TransferOutOfElapsedTimeTag>().start();
        //        time_transfer_to_.start();
      }

      // 4. Transfer primitive to body
      // TODO : By templating MessageTag we have lost const correctness in the
      //    to phase
      //    Is there a DRY way to correct this?
      EXPAND_PACK_LEFT_TO_RIGHT(
          transfer_info(to, std::get<CollectionContainers>(cct)));

      PROFILING_SECTION {
        get_timer<TransferOutOfElapsedTimeTag>().stop();
        get_timer<TotalElapsedTimeTag>().stop();
      }

      // 5. Clear contacts for next iteration
      clear_contacts();
    }

    //*************************************************************************************************
    /*!\brief Writes a profiling summary into the log file.
     *
     * \return void
     *
     * In case the profiling mode is enabled and the log level is at least info
     * this function writes a summary of profiling information to the log files.
     * The information is reduced over all previous time steps and in a separate
     * compilation over all time steps and all ranks.
     */

    template <
        // Simulator Configuration
        class SimulatorConfig,
        // Type of the coarse collision detection algorithm
        template <typename> class CD,
        // Type of the fine collision detection algorithm
        typename FD,
        // Type of the batch generation algorithm
        template <typename> class BG,
        template <
            // Template signature of the coarse collision detection algorithm
            template <typename> class,
            // Template signature of the fine collision detection algorithm
            typename,
            // Template signature of the batch generation algorithm
            template <typename> class,
            // Template signature of the collision response algorithm
            template <typename, typename, typename> class>
        class C>  // Type of the configuration
    void CollisionSystem<SimulatorConfig, C<CD, FD, BG, response::DEMSolver>>::
        log_profiling_summary() const noexcept {
      // Logging the profiling results
      PROFILING_SECTION {
        if (logging::loglevel >= logging::info) {
          // 1. Time logging
          {
            constexpr std::size_t n_timers(get_number_of_timers());
            // Look ma, on stack
            std::array<double, n_timers> minValues, maxValues, totalValues;
            std::array<std::size_t, n_timers> numValues;

            auto registrant = [this, &minValues, &maxValues, &totalValues,
                               &numValues](auto Is) {
              auto& timer = get_timer<tmpl::at_c<ElapsedTimeTags, Is>>();
              minValues[Is] = timer.min();
              maxValues[Is] = timer.max();
              totalValues[Is] = timer.total();
              numValues[Is] = timer.getCounter();
            };

            // Compile time for-each, don't like brigands' type_ ID
            index_apply<n_timers>([&registrant](auto... Is) {
              EXPAND_PACK_LEFT_TO_RIGHT(registrant(Is));
            });

            LOG_INFO_SECTION(log) {
              // clang-format off
              log << "Timing results reduced over all time steps on current process:\n" << std::fixed << std::setprecision(4)
                  << "code part              min time     max time     avg time     total time   executions\n"
                  << "--------------------   ----------   ----------   ----------   ----------   ----------\n";
              // clang-format on
              // Now automatically format based on tags
              auto printer = [this, &log, &minValues, &maxValues, &totalValues,
                              &numValues](auto Is) {
                using Tag = tmpl::at_c<ElapsedTimeTags, Is>;
                log << Tag::name() << std::setw(9) << minValues[Is] << "   "
                    << std::setw(10) << maxValues[Is] << "   " << std::setw(10)
                    << totalValues[Is] / numValues[Is] << " = " << std::setw(10)
                    << totalValues[Is] << " / " << std::setw(10)
                    << numValues[Is] << "\n";
              };

              // Compile time for-each, don't like brigands' type_ ID
              index_apply<n_timers>([&printer](auto... Is) {
                EXPAND_PACK_LEFT_TO_RIGHT(printer(Is));
              });

              // clang-format off
              log << "--------------------   ----------   ----------   ----------   ----------   ----------\n";
              // clang-format on
            }  // LOG

          }  // Time logging

          // 2. Memory meter logging
          {
            constexpr std::size_t n_memory_meters(
                get_number_of_memory_meters());
            // Look ma, on stack
            std::array<double, n_memory_meters> minAllocValues, maxAllocValues,
                totalAllocValues, minInUseValues, maxInUseValues,
                totalInUseValues;
            std::array<std::size_t, n_memory_meters> numAllocValues;

            auto registrant = [&, this](auto Is) {
              auto& memory_meter =
                  get_memory_meter<tmpl::at_c<MemoryMeterTags, Is>>();
              minAllocValues[Is] = memory_meter.minAllocation();
              maxAllocValues[Is] = memory_meter.maxAllocation();
              totalAllocValues[Is] = memory_meter.totalAllocation();
              minInUseValues[Is] = memory_meter.minInUse();
              maxInUseValues[Is] = memory_meter.maxInUse();
              totalInUseValues[Is] = memory_meter.totalInUse();
              numAllocValues[Is] = memory_meter.getCounter();
            };

            index_apply<n_memory_meters>([&registrant](auto... Is) {
              EXPAND_PACK_LEFT_TO_RIGHT(registrant(Is));
            });

            LOG_INFO_SECTION(log) {
              // clang-format off
              log << "Memory metering results reduced over all time steps on current process:\n"
                  << "code part              min alloc    max alloc    sum alloc    min in use   max in use   sum in use   executions\n"
                  << "--------------------   ----------   ----------   ----------   ----------   ----------   ----------   ----------\n" << std::fixed << std::setprecision(4);
              // clang-format on

              auto printer = [&, this](auto Is) {
                using Tag = tmpl::at_c<ElapsedTimeTags, Is>;
                log << Tag::name() << std::setw(9) << minAllocValues[Is]
                    << "   " << std::setw(10) << maxAllocValues[Is] << "   "
                    << std::setw(10) << totalAllocValues[Is] << "   "
                    << std::setw(10) << minInUseValues[Is] << "   "
                    << std::setw(10) << maxInUseValues[Is] << "   "
                    << std::setw(10) << totalInUseValues[Is] << "   "
                    << std::setw(10) << numAllocValues[Is] << "\n";
              };

              // Compile time for-each, don't like brigands' type_ ID
              index_apply<n_memory_meters>([&printer](auto... Is) {
                EXPAND_PACK_LEFT_TO_RIGHT(printer(Is));
              });

              // clang-format off
              log <<  "--------------------   ----------   ----------   ----------   ----------   ----------   ----------   ----------\n";
              // clang-format on

            }  // INFO SECTION

          }  // Memory meter logging

        }  // Info level

      }  // PROFILING SECTION
    }
    //*************************************************************************************************

    template <
        // Simulator Configuration
        class SimulatorConfig,
        // Type of the coarse collision detection algorithm
        template <typename> class CD,
        // Type of the fine collision detection algorithm
        typename FD,
        // Type of the batch generation algorithm
        template <typename> class BG,
        template <
            // Template signature of the coarse collision detection algorithm
            template <typename> class,
            // Template signature of the fine collision detection algorithm
            typename,
            // Template signature of the batch generation algorithm
            template <typename> class,
            // Template signature of the collision response algorithm
            template <typename, typename, typename> class>
        class C>  // Type of the configuration
    template <typename TimerTag>
    constexpr decltype(auto)
    CollisionSystem<SimulatorConfig,
                    C<CD, FD, BG, response::DEMSolver>>::get_timer() noexcept {
      return std::get<TimerTag>(timers_);
    }

    template <
        // Simulator Configuration
        class SimulatorConfig,
        // Type of the coarse collision detection algorithm
        template <typename> class CD,
        // Type of the fine collision detection algorithm
        typename FD,
        // Type of the batch generation algorithm
        template <typename> class BG,
        template <
            // Template signature of the coarse collision detection algorithm
            template <typename> class,
            // Template signature of the fine collision detection algorithm
            typename,
            // Template signature of the batch generation algorithm
            template <typename> class,
            // Template signature of the collision response algorithm
            template <typename, typename, typename> class>
        class C>  // Type of the configuration
    constexpr std::size_t CollisionSystem<
        SimulatorConfig,
        C<CD, FD, BG, response::DEMSolver>>::get_number_of_timers() noexcept {
      return tmpl::size<ElapsedTimeTags>::value;
    }

    template <
        // Simulator Configuration
        class SimulatorConfig,
        // Type of the coarse collision detection algorithm
        template <typename> class CD,
        // Type of the fine collision detection algorithm
        typename FD,
        // Type of the batch generation algorithm
        template <typename> class BG,
        template <
            // Template signature of the coarse collision detection algorithm
            template <typename> class,
            // Template signature of the fine collision detection algorithm
            typename,
            // Template signature of the batch generation algorithm
            template <typename> class,
            // Template signature of the collision response algorithm
            template <typename, typename, typename> class>
        class C>  // Type of the configuration
    template <typename MemoryMeterTag>
    constexpr decltype(auto) CollisionSystem<
        SimulatorConfig,
        C<CD, FD, BG, response::DEMSolver>>::get_memory_meter() noexcept {
      return std::get<MemoryMeterTag>(memory_meters_);
    }

    template <
        // Simulator Configuration
        class SimulatorConfig,
        // Type of the coarse collision detection algorithm
        template <typename> class CD,
        // Type of the fine collision detection algorithm
        typename FD,
        // Type of the batch generation algorithm
        template <typename> class BG,
        template <
            // Template signature of the coarse collision detection algorithm
            template <typename> class,
            // Template signature of the fine collision detection algorithm
            typename,
            // Template signature of the batch generation algorithm
            template <typename> class,
            // Template signature of the collision response algorithm
            template <typename, typename, typename> class>
        class C>  // Type of the configuration
    constexpr std::size_t
    CollisionSystem<SimulatorConfig, C<CD, FD, BG, response::DEMSolver>>::
        get_number_of_memory_meters() noexcept {
      return tmpl::size<MemoryMeterTags>::value;
    }

  }  // namespace collision

}  // namespace elastica
