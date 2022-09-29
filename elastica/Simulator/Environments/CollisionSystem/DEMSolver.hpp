//==============================================================================
/*!
//  \file
//  \brief Implementation of a collision solver based on Discrete Element
//  Method (DEM)
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

#include <limits>
#include <type_traits>
#include <utility>

#include "ErrorHandling/Assert.hpp"
//
#include "ModuleSettings/Profiling.hpp"
// Common environments
#include "Simulator/Environments/Common.hpp"
// All types
#include "Simulator/Environments/CollisionSystem/Types.hpp"
// Batches
#include "Simulator/Environments/CollisionSystem/Batches/Batches.hpp"
// Contacts
#include "Simulator/Environments/CollisionSystem/Contact/Contact.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/ContactContainerTrait.hpp"
// Detection
#include "Simulator/Environments/CollisionSystem/Detection/ContactCache.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/Detection.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/PairwiseTMPL.hpp"
// Interface
#include "Simulator/Environments/CollisionSystem/Detection/Fine/FineDetectionInterface.hpp"
// Policies
#include "Simulator/Environments/CollisionSystem/Policy/DomainPolicy.hpp"
#include "Simulator/Environments/CollisionSystem/Policy/ExecutionPolicy.hpp"
// Response
#include "Simulator/Environments/CollisionSystem/Response/Response.hpp"
//
// TODO : refactor out later on
#include "Simulator/Environments/Transfer/Transferer.hpp"
// Module configuration
#include "Simulator/Environments/CollisionSystem/ModuleConfiguration/ModuleConfiguration.hpp"
//
#include "Simulator/Utilities/MemoryMeters.hpp"
#include "Simulator/Utilities/Timers.hpp"
//
#include "Time/SimulationTime.hpp"
//
#include "Utilities/AsConst.hpp"
#include "Utilities/AsTaggedTuple.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/Invoke.hpp"
#include "Utilities/Logging.hpp"
#include "Utilities/Logging/Logger.hpp"
#include "Utilities/PrettyType.hpp"
#include "Utilities/Singleton.hpp"
#include "Utilities/Size.hpp"
#include "Utilities/TMPL.hpp"
//

/*
 * TODO : refactor the successive call counters into separate entity
 */

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
                           logging::Logger>,
          public DomainPolicy<SimulatorConfig>,
          public ::tt::ConformsTo<
              ::elastica::environments::protocols::Environment> {
     public:
      //**Type definitions******************************************************
      //! Type of the configuration.
      using CollisionConfig = C<CD, FD, BG, response::DEMSolver>;
      //! Type of the coarse collision detection algorithm.
      using This = CollisionSystem<SimulatorConfig, CollisionConfig>;
      //! Type of the fine collision detector.
      /*! The type of the fine collision detection algorithm is selected by the
         setting of the ELASTICA_FINE_COLLISION_DETECTOR macro. */
      using FineDetector = FD;
      //! Type of the contacts.
      // using Contact = typename CollisionConfig::Contact;
      //! Typelist of admissible primitives in the simulator
      using AdmissiblePrimitives =
          typename SimulatorConfig::AdmissiblePrimitives;
      //! Typelist of admissible imposed primitives
      using AdmissibleImposedPrimitives =
          typename SimulatorConfig::AdmissibleImposedPrimitives;
      //! Execution policy
      using ExecutionPolicy = KernelExecutionPolicy<
          typename SimulatorConfig::ForEachExecutionPolicy,
          typename SimulatorConfig::OverPairsExecutionPolicy>;
      //************************************************************************

     private:
      //**Type definitions******************************************************
      using PrimitiveOrdering = typename FineDetector::PrimitiveOrdering;
      //************************************************************************

      //**Friendships***********************************************************
      //! Friendship with Singleton
      BEFRIEND_SINGLETON;
      //************************************************************************

     public:
      //**Type definitions******************************************************
      //! Type of the domain policy
      using DomainPolicyType = DomainPolicy<SimulatorConfig>;
      //! Type of the simulation domain
      using typename DomainPolicyType::DomainType;
      //! Metafunction that returns type of the simulation boundaries
      template <::elastica::detail::EulerianFrame::DirectionType N>
      using BoundaryTypeAlong =
          typename SimulatorConfig::template BoundaryTypeAlong<N>;
      //! Time Type
      using TimeType = typename SimulatorConfig::TimeType;
      //! Real type
      using Real = typename TimeType::Real;

     protected:
      //! A typelist of primitives ordered according to internal considerations
      using OrderedPrimitives = tmpl::sort<
          AdmissiblePrimitives,
          tmpl::less<tmpl::bind<tmpl::index_of, tmpl::pin<PrimitiveOrdering>,
                                tmpl::_1>,
                     tmpl::bind<tmpl::index_of, tmpl::pin<PrimitiveOrdering>,
                                tmpl::_2>>>;
      //! A typelist of imposed primitives ordered internally
      using OrderedImposedPrimitives = tmpl::sort<
          AdmissibleImposedPrimitives,
          tmpl::less<tmpl::bind<tmpl::index_of, tmpl::pin<PrimitiveOrdering>,
                                tmpl::_1>,
                     tmpl::bind<tmpl::index_of, tmpl::pin<PrimitiveOrdering>,
                                tmpl::_2>>>;
      /*
       * We use the lower-triangular matrix only since this
       * results in unique collision between primitives
       *
       * Assume PrimitivePrimitiveMatrix is the following:
       *
       * | Primitive | Cylinder | Box | Sphere |
       * |-----------|----------|-----|--------|
       * | Cylinder  | x        | x   | x      |
       * | Box       | x        | x   | x      |
       * | Sphere    | x        | x   | x      |
       *
       * If we iterate over all entries of the matrix, then there
       * are repeated collision checks :  for example a Cylinder-Box
       * check is the same as a Box-Cylinder check for the registered
       * cylinder and box primitives. To avoid double-counting then
       * its easy to extract the lower triangular component of the
       * matrix above:
       *
       * | Primitive | Cylinder | Box | Sphere |
       * |-----------|----------|-----|--------|
       * | Cylinder  | x        |     |        |
       * | Box       | x        | x   |        |
       * | Sphere    | x        | x   | x      |
       *
       * Note that the Primitives are ordered according to the
       * FineDetector's preferred ordering, so it helps reduce
       * template instantiations.
       */
      //! Combination of ordered primitives and ordered primitives
      using PrimitiveCombinations = combinations<OrderedPrimitives>;

      /*
       * In the case of the primitive-imposed primitive matrix, this
       * siutation doesn't arise as a collision between a
       * sphere primitive and an imposed box primitive is different
       * than a box primitive and an imposed sphere primitive, since
       * they correspond to different objects, although they use
       * the same fine-detection code i.e.
       *
       * | P \ IP   | ImposedCyl | ImposedBox | ImposedSphere |
       * |----------|------------|------------|---------------|
       * | Cylinder | x          | x          | x             |
       * | Box      | x          | x          | x             |
       * | Sphere   | x          | x          | x             |
       *
       * should all be processed as rows and columns are different
       */
      //! Combination of ordered primitives and ordered imposed primitives
      using PrimitiveImposedPrimitiveCombinations =
          combinations_with_replacement<OrderedPrimitives,
                                        OrderedImposedPrimitives>;
      //! All combinations
      using AllPrimitiveCombinations =
          tmpl::append<PrimitiveCombinations,
                       PrimitiveImposedPrimitiveCombinations>;

     public:
      //! Type of Contacts
      using Contacts = typename ::elastica::collision::ContactContainerTrait<
          typename ExecutionPolicy::ExecutionPolicy>::type;
      //! Contact Cache
      using ContactCache = ::elastica::ContactCache<
          AllPrimitiveCombinations,
          ::elastica::PairwiseCacheContainerPolicy<
              typename ExecutionPolicy::ExecutionPolicy>>;
      //! Batch type
      using Batches = typename batches::BatchContainerTrait<
          typename ExecutionPolicy::ExecutionPolicy>::type;

     protected:
      //! Internal configuration inside the collision system
      struct InternalCollisionConfig : public SimulatorConfig, CollisionConfig {
        // ///// Unused, dont pass
        //        using Primitives = This::Primitives;

        //!< Domain Type
        using DomainType = This::DomainType;

        //! Type of direction
        // using DirectionType = typename DomainType::DirectionType;

        //!< Metafunction that returns type of the simulation boundaries
        // template <DirectionType D>
        // using BoundaryTypeAlong = This::BoundaryTypeAlong<D>;

        //!< Combination Types
        using PrimitiveCombinations = This::PrimitiveCombinations;
        using PrimitiveImposedPrimitiveCombinations =
            This::PrimitiveImposedPrimitiveCombinations;

        //! Kernel
        using KernelExecutionPolicy = This::ExecutionPolicy;

        // Can add pointers here for a uniform interface
        // using PrimitiveStorage = This::PrimitiveStorage;
        // using ImposedPrimitiveStorage = This::ImposedPrimitiveStorage;
        // using AdmissiblePrimitives = This::AdmissiblePrimitives; // get from
        // simulator config template <typename T> using AddPrimitiveStorageType
        // = This::AddPrimitiveStorageType<T>; template <typename T> using
        // AddImposedPrimitiveStorageType =
        //     This::AddImposedPrimitiveStorageType<T>;
      };
      //************************************************************************

     public:
      //**Algorithm tags********************************************************
      /*!\name Algorithm tags */
      //@{
      //************************************************************************
      /*!\brief Tags that denote the algorithms
       */
      struct TransferInfoFromSimulationStep {
        using type = Transferer;
      };
      struct CoarseDetectionStep {
        //! Type of the coarse collision detection algorithm
        using type = CD<InternalCollisionConfig>;
      };
      struct FineDetectionStep {
        //! Type of the fine collision detection algorithm
        using type =
            detection::fine::FineDetectionInterface<InternalCollisionConfig>;
      };
      struct ContactBatchingStep {
        //! Type of the batch generation algorithm
        using type = BG<InternalCollisionConfig>;
      };
      struct ContactResolutionStep {
        //! Type of the contact resolution algorithm
        using type = response::DEMSolver<InternalCollisionConfig>;
      };
      struct TransferInfoToSimulatorStep {
        //! Type of algorithm to transfer
        using type = Transferer;
      };
      //! Empty
      struct AllCollisionSystemSteps;
      //************************************************************************

      //@}
      //************************************************************************

      //! Typelist of algorithms
      using AlgorithmSteps =
          tmpl::list<TransferInfoFromSimulationStep, CoarseDetectionStep,
                     FineDetectionStep, ContactBatchingStep,
                     ContactResolutionStep, TransferInfoToSimulatorStep>;
      //! Tuple containing all algorithms
      using Algorithms = tmpl::as_tagged_tuple<AlgorithmSteps>;
      //! Type of tags
      using Tags =
          tmpl::append<tmpl::list<AllCollisionSystemSteps>, AlgorithmSteps>;
      //! Type of timers
      using Timers = ::elastica::Timers<Tags>;
      //! Type of intrusive memory meters
      using MemoryMeters = ::elastica::MemoryMeters<Tags>;
      //************************************************************************

     protected:
      //************************************************************************
      /*!\brief Initialization logic for detection
       */
      static Real default_time_of_last_detection_call() {
        return std::numeric_limits<Real>::lowest();
      }
      //************************************************************************

     private:
      //**Constructor***********************************************************
      /*!\name Constructor */
      //@{
      //************************************************************************
      /*!\brief Constructor of the CollisionSystem class.
       */
      explicit CollisionSystem()
          :  // Initialization of the Singleton base object
            Singleton<CollisionSystem, logging::Logger>(),
            // Pointer to the simulation domain
            DomainPolicyType(nullptr),
            // The active collision algorithms
            algorithms_(),
            //! Batches of contacts for parallelization
            batches_(),
            //! The cache of contacts
            contact_cache_(),
            // The currently active contacts of the simulation world
            contacts_(5000),
            // Active memory profilers
            memory_meters_(),
            // Active time profilers
            timers_(),
            //! The time between sucessive calls to coarse detector
            time_interval_between_each_detection_(0.0),
            //! The time of previous call to coarse detector
            /* I initialize it to a highly negative number so that detection
             * will always be performed at the start*/
            time_of_last_detection_call_(
                default_time_of_last_detection_call()) {
        // Logging the successful setup of the collision system
        using namespace ::elastica::logging;
        log<info>([](auto const& log) {
          log << "Successfully initialized the collision system instance";
        });
      }
      //************************************************************************

      //@}
      //************************************************************************

     public:
      //**Destructor************************************************************
      /*!\name Destructor */
      //@{

      //************************************************************************
      /*!\brief Destructor of the CollisionSystem class.
       */
      ~CollisionSystem() {
        clear();

        using namespace ::elastica::logging;
        log<info>([](auto const& log) {
          log << "Successfully destroyed the collision system instance";
        });
      }
      //************************************************************************

      //@}
      //************************************************************************

      //**Get functions*********************************************************
      /*!\name Get functions */
      //@{

      //************************************************************************
      /*!\brief Gets the registered algorithms
       */
      inline constexpr auto get_algorithms() & noexcept -> Algorithms& {
        return algorithms_;
      }
      inline constexpr auto get_algorithms() const& noexcept
          -> Algorithms const& {
        return algorithms_;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Gets the algorithm correspondng to `AlgorithmStep`.
       *
       * \tparam AlgorithmStep Tag to fetch algorithm
       */
      template <typename AlgorithmStep>
      inline constexpr decltype(auto) get_algorithms() {
        return tuples::get<AlgorithmStep>(algorithms_);
      }
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Gets the currently active coarse detector.
       */
      inline constexpr decltype(auto) get_coarse_detector() noexcept {
        return get_algorithms<CoarseDetectionStep>();
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Gets the currently active fine detector.
       */
      inline constexpr decltype(auto) get_fine_detector() noexcept {
        return get_algorithms<FineDetectionStep>();
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Gets the currently active batching algorithm
       */
      inline constexpr decltype(auto) get_batcher() noexcept {
        return get_algorithms<ContactBatchingStep>();
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Gets the currently active contact resolution algorithm.
       */
      inline constexpr decltype(auto) get_contact_resolver() noexcept {
        return get_algorithms<ContactResolutionStep>();
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Gets the registered contacts.
       */
      inline constexpr auto contacts() & noexcept -> Contacts& {
        return contacts_;
      }
      inline constexpr auto contacts() const& noexcept -> Contacts const& {
        return contacts_;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Gets the batches.
       */
      inline constexpr auto batches() & noexcept -> Batches& {
        return batches_;
      }
      inline constexpr auto batches() const& noexcept -> Batches const& {
        return batches_;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Gets the contact cache.
       */
      inline constexpr auto contact_cache() & noexcept -> ContactCache& {
        return contact_cache_;
      }
      inline constexpr auto contact_cache() const& noexcept
          -> ContactCache const& {
        return contact_cache_;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Gets the registered timers.
       */
      inline constexpr auto timers() & noexcept -> Timers& { return timers_; }
      inline constexpr auto timers() const& noexcept -> Timers const& {
        return timers_;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Gets the registered memory_meters.
       */
      inline constexpr auto memory_meters() & noexcept -> MemoryMeters& {
        return memory_meters_;
      }
      inline constexpr auto memory_meters() const& noexcept
          -> MemoryMeters const& {
        return memory_meters_;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Gets the time interval between each detection.
       */
      constexpr auto time_interval_between_detection() & noexcept -> Real& {
        return time_interval_between_each_detection_;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Gets the time interval between each detection.
       */
      constexpr auto time_interval_between_detection() const& noexcept
          -> Real const& {
        return time_interval_between_each_detection_;
      }
      //************************************************************************

      //@}
      //************************************************************************

     protected:
      //**Functors**************************************************************
      /*!\name Functors */
      //@{

      //************************************************************************
      /*!\brief Prologue of an algorithmic step
       */
      template <typename Step>
      void prologue() /*const*/ noexcept {
        using namespace ::elastica::logging;
        using ::elastica::get_meter;

        log<debug>([](auto const& log) {
          log << "Performing " + ::pretty_type::name<Step>() + " ...\n";
        });

        ELASTICA_PROFILING_SECTION {
          get_meter<Step>(timers()).start();
          // TODO : Memory profiling is turned off because on M2 mac, it causes
          // signficant (2-10x) slowdown.
          //          get_meter<Step>(memory_meters()).start();
        }
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Epologue of an algorithmic step
       */
      template <typename Step>
      void epilogue() /*const*/ noexcept {
        using namespace ::elastica::logging;
        using ::elastica::get_meter;

        ELASTICA_PROFILING_SECTION {
          get_meter<Step>(timers()).stop();
          // TODO : Memory profiling is turned off because on M2 mac, it causes
          // signficant (2-10x) slowdown.
          //          get_meter<Step>(memory_meters()).stop();
        }

        log<debug>([](auto const& log) {
          log << "Finished " + ::pretty_type::name<Step>() + " ...\n";
        });
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Perform an algorithmic step while recording time take/memory
       * used
       */
      template <typename Step, typename... Args>
      void perform(Args&&... args) /* const noexcept*/ {
        prologue<Step>();
        tuples::get<Step>(get_algorithms())(std::forward<Args>(args)...);
        epilogue<Step>();
      }
      //************************************************************************

      //@}
      //************************************************************************

      //**Query functions*******************************************************
      /*!\name Query functions */
      //@{
      // void log_profiling_summary() const noexcept;

      //************************************************************************
      /*!\brief Should coarse detection be performed?
       * used
       */
      bool detection_is_to_be_performed(TimeType time_v) noexcept {
        auto elapsed_time_between_calls = time_v - time_of_last_detection_call_;
        bool b = static_cast<Real>(elapsed_time_between_calls) >=
                 time_interval_between_each_detection_;
        return b ? (time_of_last_detection_call_ = time_v, b) : b;
      }
      //************************************************************************

      //@}
      //************************************************************************

      //**Utility functions*****************************************************
      /*!\name Utility functions */
      //@{
     public:
      //************************************************************************
      /*!\brief Clears the collision system.
       *
       * \return void
       */
      inline void clear() {
        // Cleanup coarse detector
        get_coarse_detector().clear();
        // Cleanup data-structures
        contacts().clear();
        batches().clear();
        contact_cache().clear();
      };
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Initializes the collision detection environment from the
       * main simulator
       *
       * \param simulator Current simulation
       *
       * \note
       * `noexcept` not guaranteed since coarse detector has internal state
       */
      template <typename Simulator_>
      void initialize(Simulator_& simulator) /*const noexcept*/ {
        using namespace ::elastica::logging;
        log<debug>([](auto const& log) {
          log << "Start initialization of the CollisionSystem...\n";
        });

        // 0. Initialize the domain, may not be needed
        DomainPolicyType::domain_ = &cpp17::as_const(simulator).domain();
        get_coarse_detector().domain() = &cpp17::as_const(simulator).domain();

        // 1. Transfer body contents to primitives
        perform<TransferInfoFromSimulationStep>(elastica::collision::from,
                                                simulator);

        // 2. Add primitives to the coarse detector if it has some internal
        // state
        get_coarse_detector().add_all_primitives(elastica::collision::from,
                                                 simulator);

        // 3. For initialization, clearing all contacts to be safe
        clear();

        log<debug>([](auto const& log) {
          log << "Finished initialization of the CollisionSystem...\n";
        });
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Resolves all contacts for the current simulation world using
       * a Discerte Element Method approach
       *
       * \details
       * Serves as the main entry points to resolving the effect of a collision
       * environment within \elastica.
       *
       * \param simulator Current simulation.
       * \param time Current simulation time.
       * \return void
       *
       * TODO : noexcept guarnatees?
       */
      template <typename Simulator>
      void resolve(Simulator& simulator, TimeType time_v) {
        prologue<AllCollisionSystemSteps>();

        // TODO : does putting this in a pipeline aid parallelism?

        perform<TransferInfoFromSimulationStep>(elastica::collision::from,
                                                simulator);

        // need a counter here
        if (detection_is_to_be_performed(time_v)) {
          contact_cache().clear();
          perform<CoarseDetectionStep>(simulator.primitives(),
                                       simulator.imposed_primitives(),
                                       contact_cache());
        }

        {
          contacts().clear();
          perform<FineDetectionStep>(cpp17::as_const(contact_cache()),
                                     contacts());
        }

        {
          batches().clear();
          // cannot be const because sometimes the index is set inside :/
          perform<ContactBatchingStep>(contacts(), batches());
        }

        {
          // TODO : Figure out passing max_overlap to time-step limiters in
          //        explicit algorithms
          perform<ContactResolutionStep>(cpp17::as_const(batches()));
        }

        {
          perform<TransferInfoToSimulatorStep>(elastica::collision::to,
                                               simulator);
        }

        epilogue<AllCollisionSystemSteps>();
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Resets the current collision system
       *
       * \details
       * Resets the entire system
       */
      void reset(typename SimulatorConfig::TimeType) {
        using elastica::for_each;
        clear();
        for_each(timers(), [](auto& m) { m.reset(); });
        for_each(memory_meters(), [](auto& m) { m.reset(); });
        time_interval_between_each_detection_ = 0.0;
        time_of_last_detection_call_ =
            TimeType{default_time_of_last_detection_call()};
      }
      //************************************************************************

      //@}
      //************************************************************************

     private:
      //**Member variables******************************************************
      /*!\name Member variables */
      //@{
      //! All algorithms.
      Algorithms algorithms_;
      //! Batches of contacts for parallelization
      Batches batches_;
      //! The cache of contacts
      ContactCache contact_cache_;
      //! The currently active contacts of the simulation world
      Contacts contacts_;
      //! The intrusive memory meters
      MemoryMeters memory_meters_;
      //! The instrusive timers
      Timers timers_;
      //@}
      //************************************************************************
     protected:
      //**Member variables******************************************************
      /*!\name Member variables */
      //@{
      //! The time between sucessive calls to coarse detector
      Real time_interval_between_each_detection_;
      //! The time of previous call to coarse detector
      TimeType time_of_last_detection_call_;
      //@}
      //************************************************************************
    };
    //**************************************************************************

    //==========================================================================
    //
    //  FREE FUNCTIONS
    //
    //==========================================================================

    //************************************************************************
    /*!\brief Resets the current collision system
     * \ingroup environments
     *
     * \param field Gravity field to be reset
     * \param time_v Time value
     */
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
    void reset(CollisionSystem<SimulatorConfig,
                               C<CD, FD, BG, response::DEMSolver>>& system,
               ::elastica::Time time) {
      system.reset(time);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Initializes parameters of a gravity field from its configuration
     * \ingroup environments
     *
     * \param field Gravity field to be initialized
     * \param config Configuration of a gravity field
     */
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
        class C,
        typename Configuration_
        /*
        typename ForEachExecutionPolicy_, typename OverPairsExecutionPolicy_,
        typename ForceModel_
        */
        >
    void initialize_parameters(
        CollisionSystem<SimulatorConfig, C<CD, FD, BG, response::DEMSolver>>&
            system,
        /* Commented for protocol conformance
        configuration::CollisionSystemUserConfiguration<
            ForEachExecutionPolicy_, OverPairsExecutionPolicy_,
            ForceModel_>
        */
        Configuration_ const& config) noexcept {
      // copy construct the force model
      system.get_contact_resolver().force_model() = config.force_model();
    }
    //**************************************************************************

    //==========================================================================
    //
    //  UTILITY FUNCTIONS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Writes a profiling summary into the log file.
     *
     * \return void
     *
     * In case the profiling mode is enabled and the log level is at least
     * info this function writes a summary of profiling information to the log
     * files. The information is reduced over all previous time steps and in a
     * separate compilation over all time steps and all ranks.
     */
    //    template <
    //        // Simulator Configuration
    //        class SimulatorConfig,
    //        // Type of the coarse collision detection algorithm
    //        template <typename> class CD,
    //        // Type of the fine collision detection algorithm
    //        typename FD,
    //        // Type of the batch generation algorithm
    //        template <typename> class BG,
    //        template <
    //            // Template signature of the coarse collision detection
    //            algorithm template <typename> class,
    //            // Template signature of the fine collision detection
    //            algorithm typename,
    //            // Template signature of the batch generation algorithm
    //            template <typename> class,
    //            // Template signature of the collision response algorithm
    //            template <typename, typename, typename> class>
    //        class C>  // Type of the configuration
    //    void CollisionSystem<SimulatorConfig, C<CD, FD, BG,
    //    response::DEMSolver>>::
    //        log_profiling_summary() const noexcept {
    //      // Logging the profiling results
    //
    //      ELASTICA_PROFILING_SECTION {
    //        // 1. Time logging
    //        const auto white_space = std::string(3UL, ' ');
    //        auto fmt_unit = std::string(10UL, '-') + white_space;
    //        {
    //          using namespace ::elastica::logging;
    //          log<info>([this, fmt_unit, white_space](auto const& log) {
    //            // Look ma, on stack
    //            std::array<double, n_timers()> minValues, maxValues,
    //            totalValues; std::array<std::size_t, n_timers()> numValues;
    //            auto fmt_str = std::string(20UL, '-') + white_space +
    //            fmt_unit
    //            +
    //                           fmt_unit + fmt_unit + fmt_unit +
    //                           std::string(10UL, '-') + "\n";
    //
    //            auto populate_times = [this, &minValues, &maxValues,
    //            &totalValues,
    //                                   &numValues](auto Is) {
    //              auto const& timer = this->get_timer<tmpl::at_c<TimerTags,
    //              Is>>(); minValues[Is] = timer.min(); maxValues[Is] =
    //              timer.max(); totalValues[Is] = timer.total();
    //              numValues[Is] = timer.getCounter();
    //            };
    //
    //            // Compile time for-each, don't like brigands' type_ ID
    //            index_apply<n_timers>(
    //                [populate_times = std::move(populate_times)](auto... Is)
    //                {
    //                  EXPAND_PACK_LEFT_TO_RIGHT(populate_times(Is));
    //                });
    //
    //            // clang-format off
    //              log << "Timing results reduced over all time steps on
    //              current process:\n" << std::fixed << std::setprecision(4)
    //                  << "code part               min time     max time avg
    //                  time     total time   executions\n"
    //                  << fmt_str;
    //            // clang-format on
    //            // Now automatically format based on tags
    //
    //            auto pretty_printer = [this, &log, &minValues, &maxValues,
    //                                   &totalValues, &numValues,
    //                                   white_space](auto Is) {
    //              using Tag = tmpl::at_c<TimerTags, Is>;
    //              log << Tag::log_name() << std::setw(10) << minValues[Is]
    //                  << white_space << std::setw(10) << maxValues[Is]
    //                  << white_space << std::setw(10)
    //                  << totalValues[Is] / numValues[Is] << " = " <<
    //                  std::setw(10)
    //                  << totalValues[Is] << " / " << std::setw(10) <<
    //                  numValues[Is]
    //                  << "\n";
    //            };
    //
    //            // Compile time for-each, don't like brigands' type_ ID
    //            index_apply<n_timers>(
    //                [pretty_printer = std::move(pretty_printer)](auto... Is)
    //                {
    //                  EXPAND_PACK_LEFT_TO_RIGHT(printer(Is));
    //                });
    //
    //            log << fmt_str;
    //          });  // log
    //
    //        }  // Time logging
    //
    //        // 2. Memory meter logging
    //        {
    //          using namespace ::elastica::logging;
    //          log<info>([this, fmt_unit, white_space](auto const& log) {
    //            // Look ma, on stack
    //            std::array<double, n_memory_meters()> minAllocValues,
    //                maxAllocValues, totalAllocValues, minInUseValues,
    //                maxInUseValues, totalInUseValues;
    //            std::array<std::size_t, n_memory_meters()> numAllocValues;
    //            auto fmt_str = std::string(20UL, '-') + white_space +
    //            fmt_unit
    //            +
    //                           fmt_unit + fmt_unit + fmt_unit + fmt_unit +
    //                           fmt_unit + std::string(10UL, '-') + "\n";
    //
    //            auto populate_allocations = [&, this](auto Is) {
    //              auto const& memory_meter =
    //                  get_memory_meter<tmpl::at_c<MemoryMeterTags, Is>>();
    //              minAllocValues[Is] = memory_meter.minAllocation();
    //              maxAllocValues[Is] = memory_meter.maxAllocation();
    //              totalAllocValues[Is] = memory_meter.totalAllocation();
    //              minInUseValues[Is] = memory_meter.minInUse();
    //              maxInUseValues[Is] = memory_meter.maxInUse();
    //              totalInUseValues[Is] = memory_meter.totalInUse();
    //              numAllocValues[Is] = memory_meter.getCounter();
    //            };
    //
    //            index_apply<n_memory_meters>(
    //                [populate_allocations =
    //                     std::move(populate_allocations)](auto... Is) {
    //                  EXPAND_PACK_LEFT_TO_RIGHT(populate_allocations(Is));
    //                });
    //
    //            // clang-format off
    //              log << "Memory metering results reduced over all time
    //              steps on current process:\n"
    //                  << "code part               min alloc    max alloc sum
    //                  alloc    min in use   max in use   sum in use
    //                  executions\n"
    //                  << fmt_str;
    //            // clang-format on
    //
    //            auto printer = [&, this, white_space](auto Is) {
    //              using Tag = tmpl::at_c<MemoryMeterTags, Is>;
    //              log << Tag::log_name() << std::setw(10) <<
    //              minAllocValues[Is]
    //                  << white_space << std::setw(10) << maxAllocValues[Is]
    //                  << white_space << std::setw(10) <<
    //                  totalAllocValues[Is]
    //                  << white_space << std::setw(10) << minInUseValues[Is]
    //                  << white_space << std::setw(10) << maxInUseValues[Is]
    //                  << white_space << std::setw(10) <<
    //                  totalInUseValues[Is]
    //                  << white_space << std::setw(10) << numAllocValues[Is]
    //                  <<
    //                  "\n";
    //            };
    //
    //            // Compile time for-each, don't like brigands' type_ ID
    //            index_apply<n_memory_meters>(
    //                [printer = std::move(printer)](auto... Is) {
    //                  EXPAND_PACK_LEFT_TO_RIGHT(printer(Is));
    //                });
    //
    //            log << fmt_str;
    //          });  // log
    //
    //        }  // Memory meter logging
    //
    //      }  // PROFILING SECTION
    //    }
    //    //**************************************************************************

  }  // namespace collision

}  // namespace elastica
