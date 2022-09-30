//==============================================================================
/*!
//  \file
//  \brief Implementation of the 'Hierarchical Hash Grids' coarse collision
//  detection algorithm
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
//
//  This file contains implementation from the pe library with the following
//  copyrights
**
**  Project home: https://www.cs10.tf.fau.de/research/software/pe/
**
**  Copyright (C) 2009 Klaus Iglberger
**
**  This file is part of pe.
**
**  pe is free software: you can redistribute it and/or modify it under the
** terms of the GNU General Public License as published by the Free Software
** Foundation, either version 3 of the License, or (at your option) any later
** version.
**
**  pe is distributed in the hope that it will be useful, but WITHOUT ANY
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
** A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License along with
** pe. If not, see <http://www.gnu.org/licenses/>.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include "ErrorHandling/Assert.hpp"
#include "ErrorHandling/ExpectsAndEnsures.hpp"
//
#include "Simulator/Environments/CollisionSystem/Detection/Coarse/BoundingBox.hpp"  // length
#include "Simulator/Environments/CollisionSystem/Detection/Coarse/ExhaustiveSearch.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/Coarse/TypeTraits.hpp"
//
#include "Utilities/AsConst.hpp"
#include "Utilities/AsTaggedTuple.hpp"
#include "Utilities/CountingIterator.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/Invoke.hpp"
#include "Utilities/Requires.hpp"
#include "Utilities/Size.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TaggedTuple.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"

/// Customization
#include "Simulator/Environments/CollisionSystem/Detection/Coarse/HashGridPolicies/DomainPolicy.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/Coarse/HashGridPolicies/StoragePolicy.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/Coarse/HashGridsConfigurationParameters.hpp"
/// Internal
#include "Simulator/Environments/CollisionSystem/Detection/Coarse/HashGridPrimitiveCache.hpp"
//

#include <algorithm>  // for_each
#include <array>
#include <cmath>
#include <list>
#include <mutex>
#include <numeric>  // accumulate
#include <tuple>
#include <type_traits>
#include <unordered_map>

namespace elastica {

  namespace collision {

    namespace detection {

      namespace coarse {

        //======================================================================
        //
        //  CLASS DEFINITION
        //
        //======================================================================

        //**********************************************************************
        /*!\brief Implementation of the 'Hierarchical Hash Grids' coarse
         * collision detection algorithm.
         * \ingroup coarse_collision_detection
         *
         * \details
         * The 'Hierarchical Hash Grids' coarse collision detection algorithm is
         * based on a spatial partitioning scheme that uses a hierarchy of
         * uniform, hash storage based grids. Uniform grids subdivide the
         * simulation space into cubic cells. A hierarchy of spatially
         * superimposed grids provides a set of grids with each grid subdividing
         * the very same space, however possessing different and thus
         * unique-sized cells. Spatial partitioning is achieved by assigning
         * every rigid body to exactly one cell in exactly one grid - that is
         * the grid with the smallest cells that are larger than the rigid body
         * (more precisely cells that are larger than the longest edge of the
         * rigid body's axis-aligned bounding box). As a consequence, the
         * collision detection is reduced to only checking body's that are
         * assigned to spatially directly adjacent cells.
         *
         * Key features of this implementation are not only an **average-case
         * computational complexity of order O(N)** as well as a space
         * complexity of order O(N), but also a short actual runtime combined
         * with low memory consumption. Moreover, the data structure
         * representing the hierarchy of grids has the ability to, at runtime,
         * automatically and perfectly adapt to the bodies of the underlying
         * simulation. Also, arbitrary bodies can be added and removed in
         * constant time, O(1). Consequently, the coarse collision detection
         * algorithm based on these hierarchical hash grids is **especially
         * well-suited for large-scale simulations** involving very large
         * numbers of bodies.
         *
         * This is frequently the case with \elastica++ as the number of rigid
         * bodies for which contact detection needs to be resolved is usually
         * large. For N rods with an average of M elements each, the number of
         * bodies is O(MN), which quickly scales up the computational cost of
         * contact detection. Hence, other collision detection schemes like
         * exhaustive searching (see ::elastica::collision::detection::coarse::
         * ExhaustiveSearch) usually do not perform well except in the simplest
         * of scenarios involving O(1--5) rods.
         *
         * The core algorithm has been rediscovered over and over in many
         * communities (involving simulations of rigid bodies, particulate
         * matter, DEM and condensed matter, crowd simulations). Our
         * implementation is based on the pe library. For further information
         * and a much more detailed explanation of this algorithm see
         * http://www10.informatik.uni-erlangen.de/Publications/Theses/2009/Schornbaum_SA09.pdf
         *
         * \tparam Config_ Type of the collision configuration (enhanced from
         * the collision module with system properties)
         */
        template <typename Config_>
        class HashGrids : private NonCopyable,
                          public DomainPolicy<Config_>,
                          public StoragePolicy {
         public:
          //**Type definitions**************************************************
          //! Type of the configuration.
          using Config = Config_;
          //! Type of this HashGrids instance
          using This = HashGrids<Config>;
          //! Type of the domain policy
          using DomainPolicyType = DomainPolicy<Config>;
          //! Type of the simulation domain
          using DomainType = typename DomainPolicyType::DomainType;
          //! Type of the admissible primitives
          using AdmissiblePrimitives = typename Config::AdmissiblePrimitives;
          //! Combinations of primitive-primitive collision matrix
          using PrimitiveCombinations = typename Config::PrimitiveCombinations;
          //! Type of the primitive-imposed primitive collision matrix
          using PrimitiveImposedPrimitiveCombinations =
              typename Config::PrimitiveImposedPrimitiveCombinations;
          //! Type of runtime parametrization.
          using Parameters = HashGridsConfigurationParameters;
          //! Execution policy for the fine detection module
          using Exec = typename Config::KernelExecutionPolicy;

         private:
          //! Type of domain policy
          using DP = DomainPolicyType;
          //! Type of storage policy
          using SP = StoragePolicy;
          //********************************************************************

         private:
          //**Compile time checks***********************************************
          /*! \cond ELASTICA_INTERNAL */
          static_assert(
              cpp17::is_same_v<typename Config::DomainType, DomainType>,
              "Meta-failure in hashgrids, please contact the developers!");
          /*! \endcond */
          //********************************************************************

         public:
          //**Utility Metafunctions ********************************************
          /*!\name Utility meta-functions */
          //@{

          //********************************************************************
          /*!\brief Gets number of primitives as initialized in the
           * configuration
           */
          static constexpr auto get_number_of_primitive_types() noexcept
              -> std::size_t {
            return tmpl::size<AdmissiblePrimitives>::value;
          }
          //********************************************************************

          //@}
          //********************************************************************

         private:
          //**Type definitions**************************************************
          //! A heterogenous collection of pointers to vectors of primitive
          //! pointers

          //! A heterogenoous collection of a vector of imposed
          //! primitive pointers
          // AdmissiblePrimitives = tmpl::list<Sphere, Cylinder>
          // PrimitiveStorage = tuple<vector<Sphere>, vector<Cylinder>>
          // CellPrimities = tuple<vector<Sphere*>*, vector<Cylinder*>*>

          // Since its internal to HashGrids, we put the type-trait here
          // and do a tmpl::transform on it
          //! Primitives residing within a single cell of the hashgrid
          using CellPrimitives = tmpl::as_tagged_tuple<tmpl::transform<
              AdmissiblePrimitives,
              // make it explicit
              typename SP::template AsTagsToCellStorage<tmpl::_1>>>;
          //! Storage of IDs for occupied cells
          using OccupiedCellsIDs =
              std::array<std::size_t, get_number_of_primitive_types()>;
          //! Storage for keeping count of primitives
          using PrimitiveCounts =
              std::array<std::size_t, get_number_of_primitive_types()>;
          //********************************************************************

          //********************************************************************
          /*!\name Literate types */
          // \brief Types and variables for literate addition and removal
          //@{
          //! Type marking a destination
          struct to_t {};
          //! Type marking a source
          struct from_t {};
          //! Variables for literate addition
          static constexpr from_t from{};
          //! Variable for literate destination
          static constexpr to_t to{};
          //@}
          //********************************************************************

          //********************************************************************
          /*!\brief Implementation of the hash grid data structure.
           * \ingroup coarse_collision_detection
           */
          class HashGrid {
           protected:
            //**Forward declarations********************************************
            struct Cell;
            // template <typename PrimitiveType>
            // inline static constexpr auto index_of_() noexcept -> std::size_t;
            //******************************************************************

           private:
            //**Type definitions************************************************
            //! The signed integer type that is used for storing offsets to
            //! neighboring cells.
            using offset_t = long;
            //! Vector for storing pointers to (body-occupied) cells.
            using CellVector = typename std::vector<Cell*>;
            //! Tag type for indexing into a tagged tuple
            template <typename T>
            struct AsCellVector {
              using type = CellVector;
            };
            //! Tagged tuple of cell vectors
            using OccupiedCells = tmpl::as_tagged_tuple<tmpl::transform<
                AdmissiblePrimitives, tmpl::bind<AsCellVector, tmpl::_1>>>;
            //! Custom tag for dispatch
            using DifferentPrimitivesOrCellsTag = tt::DifferentPrimitivesTag;
            //******************************************************************

           protected:
            //******************************************************************
            /*!\brief Data structure for representing a cell in the hash grid
             * ,used by the 'Hierarchical  Hash Grids' coarse collision
             * detection algorithm.
             * \ingroup coarse_collision_detection
             *
             * \note
             * Here a choice was made between two competing designs:
             * 1) To have a Cell templated on each primitive type, which only
             * carries information associated with that primitive type. In this
             * case when we allocate a cell, we only allocate memory for one
             * pointer to a vector of pointers for storing the body (8 bytes),
             * one pointer for neighbor offsets (8 bytes) and one index into the
             * cell_ids vector (8 bytes).
             * 2) Have a cell that carries a tuple of pointers to vectors of
             * pointers to different rigid bodies (of size 8 * n_primitives),
             * one pointer for for neighbor offsets (8 bytes) and a tuple of
             * indices into the cell_ids vector (8 * * n_primitives).
             *
             * In the former design the hashgrid can carry a tuple of
             * templated Cell vectors. In the latter design we need to carry
             * only Cell vectors. Both approaches have advantages/disadvantages
             *
             * The benefit of this design is :
             *  - (1) has straightforward code than (2)
             *  - Allocating one cell in (1) only allocates 24 bytes. Allocating
             *  one cell in (2) allocates 8 + 16 * n_primitives bytes.
             *  However allocating M cells (M >> 1 usually) allocates
             *  M * 24 * n_primitives
             *  bytes in (1), while in (2) its
             *  M * (8 + 16 * n_primitives) bytes
             *  with a difference of 8 * M * (n_primitives - 1) bytes, saved by
             *  storing a common neihbor array. For n_primitives = 3
             *  (protoypical case with a Cosserat rod (capsules), spheres and
             *  cylinders) this is a savings of 16 * M bytes per grid, so a
             *  total of 16 * M * n_grids bytes which is considerable.
             *  - In (1) cells can be created on demand---if the grid does not
             * contain a sphere in one cell but contains a capsule in that cell,
             * memory needs to be allocated within the cell for creating a
             * vector of capsule ptrs (typically another 24 bytes, for start_,
             * capacity_ and end_ of std::vector.). In (2) this is not possible.
             * If you want allocation for some primitive within the cell, you
             * necessarily need to allocate for all primitives---this takes
             * 24 * n_primitive bytes. Usually, its rare to have all types of
             * primitives in all types of cells.
             *
             * The implications to performance of these choices do not seem
             * obvious. But to err on the side of caution we implement choice
             * (2) as there is at least a clear benefit of saving memory.
             *
             * \note
             * Its important that we keep the size of a single cell as low as
             * possible. Here it is
             * p = sizeof(Pointer) = 8
             * s = sizeof(std::size_t) = 8
             * o = sizeof(point to offset) = 8
             * Therefore sizeof(Cell) = n_primitive_types * (p + s) + o
             *
             */
            // TODO : While default constructing a Cell, maybe ensure that
            // occupiedCellIDs have SIZE_MAX as the default value for error
            // checking?
            struct Cell {
              //**Member variables**********************************************
              /*!\name Member variables */
              //@{
              //! The cell's body container that stores (handles to) all
              //! the bodies that are assigned to this cell.
              CellPrimitives cell_primitives_;
              /*!< Note that only a pointer to such a body container is
                   stored: in order to save memory, this container object
                   is only allocated as long as there are bodies assigned
                   to this cell. */
              //! Pointer to an array that is storing offsets that can be used
              //! to directly access all the neighboring cells in the hash grid
              offset_t* neighbor_offset_;
              //! A tuple which carries the indices of the current cell in the
              //! `occupiedCells_` tuple of the hash-grid
              OccupiedCellsIDs occupied_cells_ids_;
              //@}
              //****************************************************************

             protected:
              //**Type definitions**********************************************
              //! Metafunction to return handles to cell primitives
              template <typename PrimitiveType>
              using tag_t = AsIDToPrimitiveIDCollection<PrimitiveType>;
              //****************************************************************

             public:
              //****************************************************************
              /*!\name Utility functions */
              //@{

              //****************************************************************
              /*!\brief Gets handle to storage of a specific primitive type
               * among all cell primitives
               */
              template <typename PrimitiveType,
                        typename Tag = tag_t<PrimitiveType>>
              inline constexpr auto set_handle_of() noexcept ->
                  typename Tag::type& {
                // No reference explicitly, its a pointer already. The pointed
                // value can change, but the pointer itself isnt
                return tuples::get<Tag>(cell_primitives_);
              }
              //****************************************************************

              //****************************************************************
              /*!\brief Gets handle to storage of a specific primitive type
               * among all cell primitives
               */
              template <typename PrimitiveType,
                        typename Tag = tag_t<PrimitiveType>>
              inline constexpr auto get_handle_to() const noexcept ->
                  typename Tag::type {
                // No reference explicitly, its a pointer already. The pointed
                // value can change, but the pointer itself isnt
                // DONE Complete logic for CellPrimitives
                return tuples::get<Tag>(cell_primitives_);
              }
              //****************************************************************

              //****************************************************************
              /*!\brief Gets the occupied cell ID of a primitive type
               */
              template <typename PrimitiveType>  // Type of the primitive
              inline constexpr auto occupied_cellID() const& noexcept
                  -> std::size_t {
                return std::get<index_of_<PrimitiveType>()>(
                    occupied_cells_ids_);
              }
              //****************************************************************

              //****************************************************************
              /*!\brief Gets the occupied cell ID of a primitive type
               */
              template <typename PrimitiveType>  // Type of the primitive
              inline constexpr auto occupied_cellID() & noexcept
                  -> std::size_t& {
                return std::get<index_of_<PrimitiveType>()>(
                    occupied_cells_ids_);
              }
              //****************************************************************

              //@}
              //****************************************************************

              template <typename ListOfPrimitives = AdmissiblePrimitives>
              inline auto get_number_of_primitives() const noexcept
                  -> std::size_t {
                using L = tmpl::transform<ListOfPrimitives,
                                          AsTagsToCellStorage<tmpl::_1>>;
                auto count_primitives =
                    [](auto handle_to_primitives) -> std::size_t {
                  return handle_to_primitives
                             ? ::elastica::size(*handle_to_primitives)
                             : 0UL;
                };
                return tuples::apply<L>(
                    [=](const auto... handle_to_primitives) {
                      std::size_t running_count(0UL);
                      EXPAND_PACK_LEFT_TO_RIGHT(
                          running_count +=
                          count_primitives(handle_to_primitives));
                      return running_count;
                    },
                    cell_primitives_);
              }
            };
            //******************************************************************

           public:
            //**Constructor*****************************************************
            /*!\name Constructor */
            //@{
            explicit HashGrid(real_t cellSpan, Parameters const& parameters);
            //@}
            //******************************************************************

            //**Destructor******************************************************
            /*!\name Destructor */
            //@{
            ~HashGrid();
            //@}
            //******************************************************************

            //**Get functions***************************************************
            /*!\name Get functions */
            //@{

           public:
            //******************************************************************
            /*!\brief Getter for \a cellSpan_
             */
            real_t get_cell_span() const noexcept { return cellSpan_; }
            //******************************************************************

           protected:
            //******************************************************************
            /*!\brief Gets the index of a primitive type in the occupied_cells
             * buffer
             */
            template <typename Primitive>
            inline static constexpr auto impl_index_() noexcept
                -> tmpl::index_of<AdmissiblePrimitives, Primitive>;
            //******************************************************************

            //******************************************************************
            /*!\brief Gets the index of a primitive type in the occupied_cells
             * buffer
             */
            template <typename Primitive>
            inline static constexpr auto index_of_() noexcept {
              return decltype(impl_index_<Primitive>())::value;
            }
            //******************************************************************

            //******************************************************************
            /*!\brief Gets the suffix type list of primitives, from the position
             * of the current primitive
             *
             * \example
             * using AdmissiblePrimitives = tmpl::list<A, B, C, D, E>;
             * using L = decltype(suffix_to<B>());
             * // L = tmpl::list<B, C, D, E>
             */
            template <typename Primitive>
            inline static constexpr auto suffix_of_() noexcept
                -> tmpl::pop_front<AdmissiblePrimitives,
                                   decltype(impl_index_<Primitive>())>;
            //******************************************************************

           public:
            //******************************************************************
            /*!\brief Gets the count of one type of primitive in the current
             * hashgrid
             */
            template <typename PrimitiveType>
            inline constexpr auto get_count_of() & noexcept -> std::size_t& {
              // retrieving it from
              // Could be decltype(auto).
              return std::get<index_of_<PrimitiveType>()>(primitive_count_);
            }
            template <typename PrimitiveType>
            inline constexpr auto get_count_of() const& noexcept
                -> std::size_t {
              return std::get<index_of_<PrimitiveType>()>(primitive_count_);
            }
            //******************************************************************

            //******************************************************************
            /*!\brief Gets the count of primitives of type `Primitive` assigned
             * to the current hashgrid
             */
            template <typename Primitive>
            inline decltype(auto) get_number_of_primitives() & noexcept {
              return get_count_of<Primitive>();
            }
            template <typename Primitive>
            inline decltype(auto) get_number_of_primitives() const& noexcept {
              return get_count_of<Primitive>();
            }
            //******************************************************************

            // While this should remain private, we need it for communication
            // in between hash grids, so promote to public

            //******************************************************************
            /*!\brief Gets the total count of primitives assigned to the current
             * hashgrid
             */
            inline auto get_number_of_primitives() const noexcept
                -> std::size_t {
              return std::accumulate(primitive_count_.cbegin(),
                                     primitive_count_.cend(), 0UL);
            }
            //******************************************************************

           private:
            //******************************************************************
            /*!\brief Gets the count of a primitive type in a given cell of the
             * hashgrid
             */
            inline auto get_number_of_primitives(Cell* cell) const noexcept
                -> std::size_t {
              // all primitives
              return cell->template get_number_of_primitives<>();
            }
            //******************************************************************

            //******************************************************************
            /*!\brief Gets a collection of cells occupied by a primitive type in
             * the current hashgrid
             */
            template <typename PrimitiveType>
            inline constexpr auto get_cells_occupied_by() & noexcept
                -> CellVector& {
              return tuples::get<AsCellVector<PrimitiveType>>(occupiedCells_);
            }
            template <typename PrimitiveType>
            inline constexpr auto get_cells_occupied_by() const& noexcept
                -> CellVector const& {
              return tuples::get<AsCellVector<PrimitiveType>>(occupiedCells_);
            }
            //******************************************************************

            /* Disallow indexing based access for now
            template <std::size_t PrimitiveIndex,
                      Requires<(PrimitiveIndex <=
            get_number_of_primitive_types())> = nullptr> inline constexpr auto
            get_cells_occupied_by() noexcept
                -> CellVector& {
              return std::get<CellVectorStorageTag<
                  tmpl::at_c<AdmissiblePrimitives, PrimitiveIndex>>>(
                  occupiedCells_);
            }
             */

            //******************************************************************
            /*!\brief Gets the number of neighbors to a cell in the context of
             * the hashgrid.
             */
            inline static constexpr auto get_number_of_neighbors()
                -> std::size_t {
              return 27UL;
            }
            //******************************************************************

            //******************************************************************
            /*!\brief Gets half the number of neighbors to a cell in the context
             * of the hashgrid.
             */
            inline static constexpr auto get_half_number_of_neighbors()
                -> std::size_t {
              return 13UL;
            }
            //******************************************************************

            //@}
            //******************************************************************

            //**Add/remove functions********************************************
            /*!\name Add/remove functions */
            //@{
           public:
            template <typename PrimitiveType>  // Type of the primitive
            inline void add(PrimitiveType* primitive);
            template <typename PrimitiveType>  // Type of the primitive
            inline void remove(PrimitiveType* primitive);

           private:
            template <typename PrimitiveType>
            void add(PrimitiveType* primitive, to_t /* meta */, Cell* cell);

            template <typename PrimitiveType>
            void remove(PrimitiveType* primitive, from_t /* meta */,
                        Cell* cell);
            //@}
            //******************************************************************

            //**Utility functions***********************************************
            /*!\name Utility functions */
            //@{
           public:
            template <typename Primitive>
            void update(Primitive* primitive);
            void clear();

           private:
            void destroy();
            void clear_neighbor_offsets();
            void initialize_neighbor_offsets();
            void set_cell_primitives_to_null();

            template <typename Primitive>
            std::size_t hash(Primitive* primitive) const noexcept;

            void enlarge();
            //@}
            //******************************************************************

            //**Contact functions***********************************************
            /*!\name Contact functions */
            //@{
           public:
            // template <typename Contacts>
            // void process_grid(Contacts& contacts) const;
            template <typename Contacts>
            void process_v2(Contacts& contacts) const;

            template <typename PrimitiveType>
            std::size_t transfer_primitives(
                to_t /* meta */,
                PrimitiveType** output_primitives_buffer) const;

            template <typename FCP, typename Contacts>
            void generate_contacts_from_finer_grid(
                FCP** primitives_from_finer_grid,
                size_t n_primitives_from_finer_grid, Contacts& contacts) const;

            void check_invariance() const;

           private:
            // TODO These can be extracted as free functions and reused in
            // Exhaustive search too
            template <typename Contacts, typename FCPCollection,
                      typename SCPCollection>
            void generate_contacts(Contacts& contacts,
                                   FCPCollection& first_primitives,
                                   SCPCollection&, tt::SamePrimitivesTag) const;

            template <typename Contacts, typename FCPCollection,
                      typename SCPCollection>
            void generate_contacts(Contacts& contacts,
                                   FCPCollection& first_primitives,
                                   SCPCollection& second_primitives,
                                   DifferentPrimitivesOrCellsTag) const;

            template <typename PrimitiveToCollide, typename Contacts>
            void process_within(Cell const*, Contacts&) const;

            template <typename PrimitiveToCollide, typename Contacts>
            void process_between(Cell const* const, Cell const* const,
                                 Contacts&) const;

            //@}
            //******************************************************************

           private:
            //**Member variables************************************************
            /*!\name Member variables */
            //@{
            //! Linear array of cells representing the 3D hash grid.
            Cell* cell_;
            //! Number of cells allocated in x-axis direction.
            std::size_t xCellCount_;
            //! Number of cells allocated in y-axis direction.
            std::size_t yCellCount_;
            //! Number of cells allocated in z-axis direction.
            std::size_t zCellCount_;
            //! Bit-mask required for the hash calculation in x-axis direction
            std::size_t xHashMask_;
            //! Bit-mask required for the hash calculation in y-axis direction
            std::size_t yHashMask_;
            //! Bit-mask required for the hash calculation in z-axis direction
            std::size_t zHashMask_;
            /*! \brief Number of cells allocated in x-axis direction multiplied
             *  by the number of cells allocated in y-axis direction.
             */
            std::size_t xyCellCount_;
            //! Total number of allocated cells.
            std::size_t xyzCellCount_;

            /*! \brief The enlargement threshold---the moment the number of
             * assigned bodies exceeds this threshold, the size of this hash
             * grid is increased.
             */
            std::size_t enlargementThreshold_;
            //! The grid's cell size (edge length of the  cubic grid cells).
            real_t cellSpan_;
            ////////////////////////////////////////////////////////////////////
            //! The inverse cell size.
            real_t inverseCellSpan_;
            /* \devnote : This is required for replacing floating point
             * divisions with multiplications during the hash computation.
             */
            ////////////////////////////////////////////////////////////////////
            //! A grid-global list that keeps track of all body-occupied cells,
            //! according to the type of the primitive
            OccupiedCells occupiedCells_;
            /* \devnote : The list is required in order to avoid iterating
             * through all grid cells whenever all bodies that are stored in the
             * grid need to be addressed.
             */
            ////////////////////////////////////////////////////////////////////
            //! Number of primitives assigned to this hash grid, categorized
            //! according to the type of the primitive
            PrimitiveCounts primitive_count_;
            /*!\brief Array of offsets to the neighboring cells (valid for all
             * the inner cells of the hash grid).
             */
            offset_t stdNeighborOffset_[get_number_of_neighbors()];
            //! Handle to configuration parameters
            Parameters const* parameters_{nullptr};
            //@}
            //******************************************************************
          };
          //********************************************************************

          //**Type definitions**************************************************
          //! List for storing all the hash grids that are in use.
          /*! This data structure is used to represent the grid hierarchy. All
             hash grids are stored in ascending order by the size of their
             cells. */
          // NOTE: A std::forward_list becomes hard to deal with when insering
          // new bodies, so keep using std::list
          using GridList = typename std::list<HashGrid*>;
          //********************************************************************

         public:
          //**Constructor*******************************************************
          /*!\name Constructor */
          //@{
          HashGrids() : HashGrids(nullptr){};
          explicit HashGrids(DomainType const* domain);
          HashGrids(DomainType const* domain, Parameters parameters);
          //@}
          //********************************************************************

          //**Destructor********************************************************
          /*!\name Destructor */
          //@{
          ~HashGrids();
          //@}
          //********************************************************************

          //**Add/remove functions**********************************************
          /*!\name Add/remove functions */
          //@{
         public:
          template <typename Simulator_>
          void add_all_primitives(::elastica::collision::tags::from_t,
                                  Simulator_& simulator);

          template <typename PrimitiveType>  // Type of the primitive
          void add(PrimitiveType* primitive);

          template <typename PrimitiveType>  // Type of the primitive
          void remove(PrimitiveType* primitive);

         private:
          template <typename PrimitiveType>  // Type of the primitive
          void add_finite_primitives(PrimitiveType* primitive,
                                     real_t primitive_length);

          template <typename PrimitiveType>  // Type of the primitive
          void add_nonfinite_primitives(PrimitiveType* primitive,
                                        real_t primitive_length);
          //@}
          //********************************************************************

          //**Utility functions*************************************************
          /*!\name Utility functions */
          //@{
         public:
          void clear();

          //********************************************************************
          /*!\brief Performs state invariant checks on the grids.
           *
           * \return void
           */
          auto check_invariance() const -> void {
            if (not is_hashgrid_active())
              return;

            for (HashGrid* grid : grid_list_)
              grid->check_invariance();
          }
          //********************************************************************

          //@}
          //********************************************************************

         public:
          //**Get functions*****************************************************
          /*!\name Get functions */
          //@{

          //********************************************************************
          //! \brief Checks if hash-grid algorithm is active algorithm is active
          auto is_hashgrid_active() const noexcept -> bool {
            return grid_active_;
          }
          //********************************************************************

          //********************************************************************
          //! \brief Gets the configuration parameters
          auto parameters() & noexcept -> Parameters& { return parameters_; }
          //********************************************************************

          //********************************************************************
          //! \brief Gets the configuration parameters
          auto parameters() const& noexcept -> Parameters const& {
            return parameters_;
          }
          //********************************************************************

          //********************************************************************
          //! \brief Gets the configuration parameters
          auto grids() & noexcept -> GridList& { return grid_list_; }
          //********************************************************************

          //********************************************************************
          //! \brief Gets the configuration parameters
          auto grids() const& noexcept -> GridList const& { return grid_list_; }
          //********************************************************************

          //@}
          //********************************************************************

          //**Contact functions*************************************************
          /*!\name Contact functions */
         public:
          template <typename PrimitivesCollection_,
                    typename ImposedPrimitivesCollection_,
                    typename Contacts>  // Contact container type
          void operator()(
              PrimitivesCollection_& primitives_collection,
              ImposedPrimitivesCollection_& imposed_primitives_collection,
              Contacts& contacts);

         protected:
          template <typename FCP,                // First colliding primitive
                    typename SCP,                // Second colliding primitive
                    typename ContactsContainer>  // Contact container type
          static inline void generate_contact(ContactsContainer& contacts,
                                              FCP* first_primitive,
                                              SCP* second_primitive);
          //@}
          //********************************************************************

         private:
          //**Member variables**************************************************
          /*!\name Member variables */
          //@{
          //////////////////////////////////////////////////////////////////////
          /*!< This vector contains all bodies that are not assigned to any
               grid in the hierarchy. A body is not assigned to any grid if
               the body is infinite in size or if the detection algorithm
               based on hierarchical hash grids has not yet been activated
               (see \ref grid_active_). */
          // NonGridPrimitiveStorage
          //     non_grid_primitives_;  //!< Vector of all unassigned rigid
          //     bodies.
          //////////////////////////////////////////////////////////////////////

          //////////////////////////////////////////////////////////////////////
          /* The grids in this list are sorted in ascending order by the
           * size of their cells. */
          //!< List of all grids that form the hierarchy.
          GridList grid_list_;
          //////////////////////////////////////////////////////////////////////

          //////////////////////////////////////////////////////////////////////
          /* This flag indicates whether the detection algorithm based on
           * hierarchical hash grids is activated. If the simulation only
           * consists of a very small number of bodies, each body is simply
           * checked against every other body. This strategy proves to be
           * faster than involving the far more complex mechanisms of the
           * hierarchical hash grids.
           */
          //! Grid activation flag.
          bool grid_active_;
          //////////////////////////////////////////////////////////////////////
          //! Keeps track of primitives that get transferred between each grid
          Cache<AdmissiblePrimitives> transfer_cache_;
          //////////////////////////////////////////////////////////////////////
          //! Runtime parameters
          Parameters parameters_;
          //////////////////////////////////////////////////////////////////////
          /* Below a certain threshold on the number of primitives, we switch
           * over to the exhaustive search algorithm, see \ref grid_active_.
           */
          //! The exhaustive search algorithm
          ExhaustiveSearch<Config> exhaustive_search;
          //////////////////////////////////////////////////////////////////////

          //**Compile time checks***********************************************
          /*! \cond ELASTICA_INTERNAL */
          static_assert(
              std::is_same<This, typename Config::template CoarseDetectorFamily<
                                     Config>>::value,
              "Should not be reached!");
          /*! \endcond */
          //********************************************************************
        };
        //**********************************************************************

        //======================================================================
        //
        //  CONSTRUCTOR
        //
        //======================================================================

        //**********************************************************************
        /*!\brief Constructor for the HashGrid class.
         */
        template <typename C>
        HashGrids<C>::HashGrid::HashGrid(real_t cellSpan,
                                         Parameters const& parameters)
            : parameters_(&parameters) {
          // Initialization of all member variables and ...
          std::tie(xCellCount_, yCellCount_, zCellCount_) =
              DomainPolicyType::initial_cell_counts(cellSpan);
          /*
          xCellCount_ = initial_x_cell_count();
          yCellCount_ = initial_y_cell_count();
          zCellCount_ = initial_z_cell_count();
          */

          xHashMask_ = xCellCount_ - 1;
          yHashMask_ = yCellCount_ - 1;
          zHashMask_ = zCellCount_ - 1;

          xyCellCount_ = xCellCount_ * yCellCount_;
          xyzCellCount_ = xyCellCount_ * zCellCount_;

          enlargementThreshold_ =
              xyzCellCount_ / parameters.minimal_grid_density;

          // ... allocation of the linear array that represents the hash
          // grid.
          cell_ = new Cell[xyzCellCount_];

          // Initialization of each cell - i.e., initially setting the pointer
          // to the body container to NULL (=> no bodies are assigned to this
          // hash grid yet!) and ...
          set_cell_primitives_to_null();

          // ... setting up the neighborhood relationship (using the offset
          // array).
          initialize_neighbor_offsets();

          cellSpan_ = cellSpan;
          inverseCellSpan_ = static_cast<real_t>(1.0) / cellSpan;

          // this explicit to silence warnings in clang
          tmpl::for_each<AdmissiblePrimitives>([this](auto p) {
            using Primitive = tmpl::type_from<decltype(p)>;
            // rseerve memory
            this->template get_cells_occupied_by<Primitive>().reserve(
                this->parameters_->initial_occupied_cells_vector_size);
            // initialize to 0
            this->template get_count_of<Primitive>() = 0UL;
          });
        }
        //**********************************************************************

        //======================================================================
        //
        //  DESTRUCTOR
        //
        //======================================================================

        //**********************************************************************
        /*!\brief Destructor for the HashGrid class.
         */
        template <typename C>
        HashGrids<C>::HashGrid::~HashGrid() {
          destroy();
        }
        //**********************************************************************

        //======================================================================
        //
        //  ADD/REMOVE FUNCTIONS
        //
        //======================================================================

        //**********************************************************************
        /*!\brief Adds a body to this hash grid.
         *
         * \param body The body that is about to be added to this hash grid.
         * \return void
         *
         * This function is called every time a new rigid body is added to this
         * grid. If adding the body will cause the total number of bodies
         * assigned to this grid to exceed the enlargement threshold, the size
         * of this hash grid is increased in order to maintain a fixed minimal
         * grid density (= ratio of cells to bodies). This function may also be
         * called during the update phase.
         */
        template <typename C>
        template <typename Primitive>
        void HashGrids<C>::HashGrid::add(Primitive* primitive) {
          // TODO : You can exercise more control here, and add it as a policy
          // If adding the primitive will cause the total number of primitives
          // assigned to this grid to exceed the enlargement threshold, the size
          // of this hash grid must be increased.
          if (DP::should_enlarge(get_number_of_primitives(),
                                 enlargementThreshold_))
            HashGrid::enlarge();

          // Calculate (and store) the hash value (= the primitive's cell
          // association) and ...
          std::size_t h(hash(primitive));
          primitive->set_hash(h);

          // ... insert the primitive into the corresponding cell.
          Cell* cell = cell_ + h;
          add(primitive, to, cell);

          // Raise the corresponding primitive count
          ++get_count_of<Primitive>();
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Removes a body from this hash grid.
         *
         * \param body The body that is about to be removed from this hash grid.
         * \return void
         *
         * This function is called every time a rigid body is removed from this
         * grid. It may also be called during the update phase.
         */
        template <typename C>
        template <typename Primitive>
        void HashGrids<C>::HashGrid::remove(Primitive* primitive) {
          // The stored hash value (= the body's cell association) is used in
          // order to directly access the cell from which this body will be
          // removed.
          // TODO parallel
          // TODO Ensure cell is unique for every thread, and multiple threads
          //      don't access the same cell
          Cell* cell = cell_ + primitive->get_hash();
          remove(primitive, from, cell);

          // TODO concurrent array (this is not that important, if we know
          //      how many times the remove function is called for every
          //      primitive, we can update it later)
          --get_count_of<Primitive>();
        }
        //**********************************************************************

        //======================================================================
        //
        //  UTILITY FUNCTIONS
        //
        //======================================================================

        //**********************************************************************
        /*!\brief Updates the cell association of a primitive that is assigned
         * to this grid.
         *
         * \param primitive The primitive whose cell association is being
         * updated.
         * \return void
         *
         * Checks if a given rigid primitive can remain stored in its currently
         * assigned cell - and if not, removes the primitive from this cell and
         * reassigns it to another cell (which is identified by re-evaluating
         * the primitive's hash value).
         */
        template <typename C>
        template <typename Primitive>
        void HashGrids<C>::HashGrid::update(Primitive* primitive) {
          // DONE : primitive is parallel
          // The hash value is recomputed based on the primitive's current
          // spatial location.
          std::size_t const new_hash = hash(primitive);
          std::size_t const old_hash = primitive->get_hash();

          // If this new hash value is identical to the hash value of the
          // previous time step, the primitive
          // remains assigned to its current grid cell.
          if (new_hash == old_hash)
            return;

          // Only if the hash value changes, the cell association has to be
          // changed, too - meaning, the
          // primitive has to be removed from its currently assigned cell and
          // ...
          Cell* cell = cell_ + old_hash;
          // TODO parallel
          // TODO Ensure cell is unique for every thread, and multiple threads
          //      don't access the same cell
          //      1. Have a mutex around a cell pointer? (acquire and release)
          //         immediately after the call?
          remove(primitive, from, cell);

          primitive->set_hash(new_hash);

          // TODO Ensure cell is unique for every thread, and multiple threads
          //      don't access the same cell
          // ... stored in the cell that corresponds to the new hash value.
          cell = cell_ + new_hash;
          // TODO parallel
          add(primitive, to, cell);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Performs invariant checks on hash grid.
         *
         * \return void
         */
        template <typename C>
        void HashGrids<C>::HashGrid::check_invariance() const {
          // Count the number of primitives in each cell and see if it agrees
          // with the counter.
          tmpl::for_each<AdmissiblePrimitives>([this](auto p) {
            using Primitive = tmpl::type_from<decltype(p)>;

            // 1. Call destructor of each element
            auto& primitive_occupied_cells =
                this->get_cells_occupied_by<Primitive>();
            auto const actual = std::accumulate(
                std::cbegin(primitive_occupied_cells),
                std::cend(primitive_occupied_cells), 0UL,
                [](std::size_t sum, Cell* cell) {
                  auto ptr = cell->template get_handle_to<Primitive>();
                  Expects(ptr != nullptr);
                  sum += cpp17::size(*ptr);
                  return sum;
                });
            auto const expected = this->get_count_of<Primitive>();
            Ensures(actual == expected);
          });

          tmpl::for_each<AdmissiblePrimitives>([this](auto p) {
            using Primitive = tmpl::type_from<decltype(p)>;

            std::for_each(
                elastica::make_counting_iterator(0UL),
                elastica::make_counting_iterator(xyzCellCount_),
                [=](auto const expected_hash) {
                  Cell* cell = cell_ + expected_hash;
                  auto* primitives = cell->template get_handle_to<Primitive>();

                  if (primitives != nullptr) {
                    auto const primitives_have_expected_hash = std::all_of(
                        std::cbegin(*primitives), std::cend(*primitives),
                        [=](auto& primitive) {
                          return primitive->get_hash() == expected_hash;
                        });
                    Ensures(primitives_have_expected_hash);
                  }
                });
          });
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Clears the hash grid.
         *
         * \return void
         *
         * This function removes all (the handles to) the bodies that are
         * assigned to this hash grid from the body containers of the grid's
         * cells.
         */
        template <typename C>
        void HashGrids<C>::HashGrid::clear() {
          tmpl::for_each<AdmissiblePrimitives>([this](auto p) {
            using Primitive = tmpl::type_from<decltype(p)>;

            // 1. Call destructor of each element
            auto& primitive_occupied_cells =
                this->get_cells_occupied_by<Primitive>();

            for (Cell* cell : primitive_occupied_cells) {
              auto* primitives = cell->template get_handle_to<Primitive>();
              // clears only the pointers, leaves the pointed to objects
              // untouched
              primitives->clear();

              // In add(primitive, to, cell) we do a new PrimitiveContainer, so
              // we explicitly delete it.
              delete primitives;
              primitives = nullptr;
            }

            primitive_occupied_cells.clear();

            // 2. Set size to 0
            this->get_count_of<Primitive>() = 0UL;
          });
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Clears the offset array of the current hash-grid.
         *
         * \return void
         *
         * This function clears all the offset arrays in the cells of the
         * current hashgrid
         */
        template <typename C>
        void HashGrids<C>::HashGrid::clear_neighbor_offsets() {
          std::for_each(cell_, cell_ + xyzCellCount_, [this](Cell& c) {
            // if its not an interior cell
            if (c.neighbor_offset_ != stdNeighborOffset_)
              delete[] c.neighbor_offset_;
          });
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Destroys any and all allocations made inside the hashgrid,
         *
         * \details
         * Can be called in place of the destructor.
         *
         */
        template <typename C>
        void HashGrids<C>::HashGrid::destroy() {
          clear();
          clear_neighbor_offsets();
          delete[] cell_;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Sets up the neighborhood relationships for all grid cells.
         *
         * \return void
         *
         * This function is used to initialize the offset arrays of all grid
         * cells. The offsets are required for ensuring fast direct access to
         * all directly adjacent cells for each cell in the hash grid.
         */
        template <typename C>
        void HashGrids<C>::HashGrid::initialize_neighbor_offsets() {
          auto xc = static_cast<offset_t>(xCellCount_);
          auto yc = static_cast<offset_t>(yCellCount_);
          auto zc = static_cast<offset_t>(zCellCount_);
          auto xyc = static_cast<offset_t>(xyCellCount_);
          auto xyzc = static_cast<offset_t>(xyzCellCount_);

          // Initialization of the grid-global offset array that is valid for
          // all inner cells in the hash grid.
          unsigned int i = 0;
          for (offset_t zz = -xyc; zz <= xyc; zz += xyc) {
            for (offset_t yy = -xc; yy <= xc; yy += xc) {
              for (offset_t xx = -1; xx <= 1; ++xx, ++i) {
                stdNeighborOffset_[i] = xx + yy + zz;
              }
            }
          }

          // Allocation and initialization of the offset arrays of all the
          // border cells. All inner cells are set to point to the grid-global
          // offset array.
          Cell* c = cell_;
          for (offset_t z = 0; z < zc; ++z) {
            for (offset_t y = 0; y < yc; ++y) {
              for (offset_t x = 0; x < xc; ++x, ++c) {
                /* border cell */
                if (x == 0 or x == (xc - 1) or y == 0 or y == (yc - 1) or
                    z == 0 or z == (zc - 1)) {
                  c->neighbor_offset_ = new offset_t[get_number_of_neighbors()];

                  i = 0;
                  // 27 = 3 * 3 * 3

                  // loop 3
                  for (offset_t zz = -xyc; zz <= xyc; zz += xyc) {
                    offset_t zo = zz;
                    if (z == 0 && zz == -xyc) {
                      zo = xyzc - xyc;
                    } else if (z == (zc - 1) && zz == xyc) {
                      zo = xyc - xyzc;
                    }

                    // loop 3
                    for (offset_t yy = -xc; yy <= xc; yy += xc) {
                      offset_t yo = yy;
                      if (y == 0 && yy == -xc) {
                        yo = xyc - xc;
                      } else if (y == (yc - 1) && yy == xc) {
                        yo = xc - xyc;
                      }

                      // loop 3
                      for (offset_t xx = -1; xx <= 1; ++xx, ++i) {
                        offset_t xo = xx;
                        if (x == 0 && xx == -1) {
                          xo = xc - 1;
                        } else if (x == (xc - 1) && xx == 1) {
                          xo = 1 - xc;
                        }

                        c->neighbor_offset_[i] = xo + yo + zo;
                      }
                    }
                  }
                }
                /* inner cell */
                else {
                  c->neighbor_offset_ = stdNeighborOffset_;
                }
              }
            }
          }
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Sets the handle of each primitive storage across cells to
         * empty
         *
         * \return void
         */
        template <typename C>
        void HashGrids<C>::HashGrid::set_cell_primitives_to_null() {
          std::for_each(cell_, cell_ + xyzCellCount_, [](Cell& c) {
            tmpl::for_each<AdmissiblePrimitives>([&](auto p) {
              using Primitive = tmpl::type_from<decltype(p)>;
              c.template set_handle_of<Primitive>() = nullptr;
            });
          });
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Computes the hash value (= cell association) of a given rigid
         * body.
         *
         * \param body The body whose hash value is about to be calculated.
         * \return The hash value (=cell association) of the body.
         *
         * The hash calculation uses modulo operations in order to spatially map
         * entire blocks of connected cells to the origin of the coordinate
         * system. This block of cells at the origin of the coordinate system
         * that is filled with all the bodies of the simulation is referred to
         * as the hash grid. The key feature, and ultimately the advantage, of
         * hash grids is the fact that two adjacent cells that are located
         * anywhere in the simulation space are mapped to two cells that are
         * still adjacent in the hashed storage structure.
         *
         * Note that the modulo calculations are replaced with fast bitwise AND
         * operations - hence, the spatial dimensions of the hash grid must be
         * restricted to powers of two!
         */
        template <typename C>
        template <typename Primitive>
        std::size_t HashGrids<C>::HashGrid::hash(
            Primitive* primitive) const noexcept {
          auto const& aabb = primitive->get_AABB();
          const real_t x = aabb[0];
          const real_t y = aabb[1];
          const real_t z = aabb[2];

          std::size_t x_hash;
          std::size_t y_hash;
          std::size_t z_hash;

          if (x < 0) {
            real_t i = (-x) * inverseCellSpan_;
            x_hash =
                xCellCount_ - 1 - (static_cast<std::size_t>(i) & xHashMask_);
          } else {
            real_t i = x * inverseCellSpan_;
            x_hash = static_cast<std::size_t>(i) & xHashMask_;
          }

          if (y < 0) {
            real_t i = (-y) * inverseCellSpan_;
            y_hash =
                yCellCount_ - 1 - (static_cast<std::size_t>(i) & yHashMask_);
          } else {
            real_t i = y * inverseCellSpan_;
            y_hash = static_cast<std::size_t>(i) & yHashMask_;
          }

          if (z < 0) {
            real_t i = (-z) * inverseCellSpan_;
            z_hash =
                zCellCount_ - 1 - (static_cast<std::size_t>(i) & zHashMask_);
          } else {
            real_t i = z * inverseCellSpan_;
            z_hash = static_cast<std::size_t>(i) & zHashMask_;
          }

          return x_hash + y_hash * xCellCount_ + z_hash * xyCellCount_;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Adds a body to a specific cell in this hash grid.
         *
         * \param body The body that is about to be added to this hash grid.
         * \param cell The cell the body is assigend to.
         * \return void
         */
        template <typename C>
        template <typename Primitive>
        void HashGrids<C>::HashGrid::add(Primitive* primitive, to_t /* meta */,
                                         Cell* cell) {
          // TODO The first part is parallel provided cell pointer
          //      is uniquely accessed within this function

          // HEALTH WARNING : Here we obtain a pointer to the location
          // pointed by the underlying handle from get_handle. Chances are
          // when initializing the cell, its a nullptr. However, creating
          // new memory (using new[]) below and assigning it to the primtives
          // binds this memory to the local primitives variable and not the
          // underlying handle. Hence the program is ill-formed and eventually
          // memory leaks.
          // One can think that instead of copying the handle as a pointer,
          // why not get a reference to the handle directly and assign the
          // memory there? This is UB according to C++ standards
          // https://stackoverflow.com/questions/6421083/reference-pointing-to-a-null-object

          // Pointer to a vector of primitive pointers
          auto* primitives = cell->template get_handle_to<Primitive>();

          // If this cell is already occupied by other bodies, which means the
          // pointer to the body container holds a valid address and thus the
          // container itself is properly initialized, then the body is simply
          // added to this already existing body container. Note that the index
          // position is memorized (=> "body->setCellId()") in order to ensure
          // constant time removal.
          if (primitives != nullptr) {
            primitive->set_cell_ID(primitives->size());
            primitives->push_back(primitive);
          }

          // If, however, the cell is still empty, then the object container,
          // first of all, must be created (i.e., allocated) and properly
          // initialized (i.e., sufficient initial storage capacity must be
          // reserved). Furthermore, the cell must be inserted into the
          // grid-global vector 'occupiedCells_' in which all cells that are
          // currently occupied by bodies are recorded.
          else {
            //            // Prepare to see if the cell is already occupied
            //            const bool occupied =
            //                get_number_of_primitive_types_currently_in(cell) >
            //                0;

            using PrimitiveContainer =
                std::remove_pointer_t<decltype(primitives)>;
            cell->template set_handle_of<Primitive>() = new PrimitiveContainer;
            //            primitives = new PrimitiveContainer;
            primitives = cell->template get_handle_to<Primitive>();
            primitives->reserve(parameters_->initial_cell_vector_size);

            primitive->set_cell_ID(0);
            primitives->push_back(primitive);

            // TODO If parallel, these need to be concurrent vectors which may
            // be simultaneously modified
            // TODO Else have a thread local buffer that reduces the cells need
            //      to be added
            // Vector of cell pointers containing this primitive
            auto& occupied_cells = get_cells_occupied_by<Primitive>();
            cell->template occupied_cellID<Primitive>() = occupied_cells.size();
            occupied_cells.push_back(cell);
          }
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Removes a body from a specific cell in this hash grid.
         *
         * \param body The body that is about to be removed from this hash grid.
         * \param cell The cell the body is removed from.
         * \return void
         */
        template <typename C>
        template <typename Primitive>
        void HashGrids<C>::HashGrid::remove(Primitive* primitive,
                                            from_t /* meta */, Cell* cell) {
          // TODO The first part is parallel provided cell pointer
          //      is uniquely accessed within this function
          // Pointer to a vector of primitive pointers
          auto* primitives = cell->template get_handle_to<Primitive>();

          Expects(primitives != nullptr);
          Expects(primitives->size() > 0);

          // If the primitive is the last primitive that is stored in this cell
          // ...
          if (primitives->size() == 1) {
            // ... the primtives's container is destroyed and ...

            // HEALTH WARNING : Same issue as add here
            // Deletion of memory is succesful, however the pointer fails to be
            // set to null, thus the simulation thinks that there is a ghost
            // element in this particular vector and issues come up later!
            delete primitives;
            primitives = nullptr;
            //            delete cell->template get_handle_of<PrimitiveType>();
            cell->template set_handle_of<Primitive>() = nullptr;

            // ... the cell is removed from the grid-global vector
            // 'occupiedCells_' that records all body-occupied cells. Since the
            // cell memorized its index (=> 'occupied_cells_ids_') in this
            // vector, it can be removed in constant time, O(1).

            // Get values from cache
            // TODO If parallel, these need to be concurrent vectors which may
            // be simultaneously modified
            // TODO Else have a thread local buffer that reduces what cells need
            //      to be removed and remove after the step
            auto& occupied_cells = get_cells_occupied_by<Primitive>();
            const std::size_t occupied_cellID =
                cpp17::as_const(cell)->template occupied_cellID<Primitive>();

            if (occupied_cellID == occupied_cells.size() - 1) {
              occupied_cells.pop_back();
            } else {
              // Exchange pointers with the last cell
              Cell* last_cell = occupied_cells.back();
              // Removes the cell, so don't need to erase it
              occupied_cells.pop_back();
              last_cell->template occupied_cellID<Primitive>() =
                  occupied_cellID;
              occupied_cells[occupied_cellID] = last_cell;
              // after this the current cell is no longer in the occupied cells
            }
          }
          // If the body is *not* the last body that is stored in this cell ...
          else {
            // TODO This part is automatically parallel provided cell pointer
            //      is uniqely accessed
            const size_t cellID = primitive->get_cell_ID();

            // ... the body is removed from the cell's body container. Since the
            // body memorized its index (=> 'cellId') in this container, it can
            // be removed in constant time, O(1).
            if (cellID == primitives->size() - 1) {
              primitives->pop_back();
            } else {
              Primitive* last_element = primitives->back();
              primitives->pop_back();
              last_element->set_cell_ID(cellID);
              (*primitives)[cellID] = last_element;
              // after this the current primitive is no longer in the list of
              // primitives of the occupied cell
            }
          }
        }

        //**********************************************************************
        /*!\brief Increases the number of cells used by this hash grid.
         *
         * \details
         * In order to handle an initially unknown and ultimately arbitrary
         * number of bodies, the hash grid, starting with a rather small number
         * of cells at the time of its creation, must have the ability to grow
         * as new bodies are inserted. Therefore, if by inserting a body into
         * this hash grid the associated grid density - that is the ratio of
         * cells to bodies - drops below the threshold specified by \a
         * minimalGridDensity, or in other words, if the number of bodies grows
         * larger than specified by \a enlargementThreshold_, the number of
         * cells in each coordinate direction is doubled (thus the total number
         * of grid cells is increased by a factor of 8).
         *
         * \return void
         */
        template <typename C>
        void HashGrids<C>::HashGrid::enlarge() {
          // Not using smart pointers because smart pointer to smart pointers
          // is exhausting

          // Tuple of pointer to pointers (bodyIDs)
          using PrimitiveIDs =
              tmpl::transform<AdmissiblePrimitives, std::add_pointer<tmpl::_1>>;
          using PrimitiveIDHandleStorage = tmpl::as_tuple<
              tmpl::transform<PrimitiveIDs, std::add_pointer<tmpl::_1>>>;

          //! Storage of all previous primitives in the hash grid
          // Primitive** or PrimitiveID*
          PrimitiveIDHandleStorage primitives;
          //! Handle to a current primitive ID, helper variable
          // Primitive**
          PrimitiveIDHandleStorage curr_primitive;

          // Create memory to backup all primitives assigned to this grid
          // and get handle to the first pointer
          auto allocate = [this, &primitives, &curr_primitive](auto Is) {
            using Primitive = tmpl::at_c<AdmissiblePrimitives, Is>;
            using PrimitiveID = tmpl::at_c<PrimitiveIDs, Is>;
            // Primitive** primitives = new Primitive*[count]
            std::get<Is>(primitives) =
                new PrimitiveID[this->get_count_of<Primitive>()];
            // curr_primitive = primitives (sets to the beginning)
            std::get<Is>(curr_primitive) = std::get<Is>(primitives);
          };

          index_apply<get_number_of_primitive_types()>([allocate](auto... Is) {
            EXPAND_PACK_LEFT_TO_RIGHT(allocate(Is));
          });

          //          using PrimitiveType = tmpl::at_c<AdmissiblePrimitives,
          //          Is>; using PrimitivePointer =
          //          std::add_pointer_t<PrimitiveType>; PrimitivePointer*
          //          primitives =
          //              new PrimitivePointer[get_count<PrimitiveType>()];
          //          PrimitivePointer* curr_primitive = primitives;

          // Backup all primitives assigned to this grid currently, and
          // temporarily remove them.
          auto copier = [this, &curr_primitive](auto Is) {
            // can use Is, but this is more expressive
            using Primitive = tmpl::at_c<AdmissiblePrimitives, Is>;
            auto& occupied_cells = this->get_cells_occupied_by<Primitive>();

            // We copy the pointer here (not a reference) and so it isnt updated
            Primitive** curr = std::get<Is>(curr_primitive);

            for (Cell* cell : occupied_cells) {
              // Pointer to a vector of primitive pointers
              auto* cell_primitives = cell->template get_handle_to<Primitive>();

              // Copy pointers
              for (auto* cell_primitive : *cell_primitives) {
                //*(curr_primitive++) = *primitive;
                *curr = cell_primitive;
                ++curr;
              }
            }

            // Now copy the new pointer back onto curr_primitive
            std::get<Is>(curr_primitive) = curr;
            // after this curr_primitive points to the end of the primitiveID
            // pointer array
          };

          // All bodies that are assigned to this grid are temporarily
          // removed,
          index_apply<get_number_of_primitive_types()>(
              [&copier](auto... Is) { EXPAND_PACK_LEFT_TO_RIGHT(copier(Is)); });

          // ... the grid's current data structures are destroyed, ...
          destroy();

          // ... the number of cells is enlarged in each coordinate direction,
          // ...
          DomainPolicyType::enlarge(xCellCount_, yCellCount_, zCellCount_);

          xHashMask_ = xCellCount_ - 1;
          yHashMask_ = yCellCount_ - 1;
          zHashMask_ = zCellCount_ - 1;

          xyCellCount_ = xCellCount_ * yCellCount_;
          xyzCellCount_ = xyCellCount_ * zCellCount_;

          // ... a new threshold for enlarging this hash grid is set, ...
          enlargementThreshold_ =
              xyzCellCount_ / parameters_->minimal_grid_density;

          // ... a new linear array of cells representing this enlarged hash
          // grid is allocated and ...
          cell_ = new Cell[xyzCellCount_];

          // ... initialized to null ...
          set_cell_primitives_to_null();

          // ... with the neighbors setup, and finally ...
          initialize_neighbor_offsets();

          // At this stage all the current primitive points to the end
          auto reinserter = [this, &primitives, &curr_primitive](auto Is) {
            using PrimitiveType = tmpl::at_c<AdmissiblePrimitives, Is>;
            using PrimitivePointer = std::add_pointer_t<PrimitiveType>;

            // From before, curr_primitive points to the end
            auto end_ = std::get<Is>(curr_primitive);

            for (PrimitivePointer* p = std::get<Is>(primitives); p < end_;
                 ++p) {
              this->add(*p);
            }
          };

          // ... all previously removed bodies are reinserted.
          index_apply<get_number_of_primitive_types()>(
              [&reinserter](auto... Is) {
                EXPAND_PACK_LEFT_TO_RIGHT(reinserter(Is));
              });

          cpp17::apply(
              [](auto&... prim) {
                // delete a bunch of pointers we allpcated
                EXPAND_PACK_LEFT_TO_RIGHT(delete[](prim));
              },
              primitives);
        }
        //**********************************************************************

        //======================================================================
        //
        //  CONTACT FUNCTIONS
        //
        //======================================================================

        //**********************************************************************
        /*!\brief Contact generation between colliding rigid primitives of the
         * same type
         *
         * \details
         * This function generates all contacts between rigid primitives of the
         * same type contained in the simulation world. The contacts are added
         * to the contact container \a contacts.
         *
         * \param contacts Contact container for the generated contacts.
         * \return void
         */
        template <typename C>
        template <typename Contacts,       // Contact container type
                  typename FCPCollection,  // Storage of the first colliding
                                           // primitive
                  typename SCPCollection>  // Storage of the second colliding
                                           // primitive
        void HashGrids<C>::HashGrid::generate_contacts(
            Contacts& contacts, FCPCollection& first_primitives,
            SCPCollection& /*second_primitives*/,
            tt::SamePrimitivesTag /*meta*/) const {
          // FCPCollection and SCPCollection are contiguous collections of
          // pointers by design
          static_assert(cpp17::is_same_v<FCPCollection, SCPCollection>,
                        "Hashgrids Meta failure!");

          // Its const, so no need to bother extracting it :/
          auto const first_end = first_primitives.end();
          for (auto first_primitive_it = first_primitives.begin();
               first_primitive_it != first_end; ++first_primitive_it) {
            for (auto second_primitive_it = first_primitive_it + 1;
                 second_primitive_it != first_end; ++second_primitive_it) {
              // Derefence the iterator which returns a pointer to the body
              // which is aga
              HashGrids<C>::generate_contact(contacts, *first_primitive_it,
                                             *second_primitive_it);
            }
          }
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Contact generation between colliding rigid primitives of
         * different types
         *
         * \details
         * This function generates all contacts between rigid primitives of
         * different type contained in the simulation world. The contacts
         * generated are added to the container \a contacts.
         *
         * \param contacts Contact container for the generated contacts.
         * \return void
         */
        template <typename C>              // Type of the configuration
        template <typename Contacts,       // Contact container type
                  typename FCPCollection,  // Storage of the first colliding
                                           // primitive
                  typename SCPCollection>  // Storage of the second colliding
                                           // primitive
        void HashGrids<C>::HashGrid::generate_contacts(
            Contacts& contacts, FCPCollection& first_primitives,
            SCPCollection& second_primitives,
            DifferentPrimitivesOrCellsTag /*meta*/) const {
          // FCPCollection and SCPCollection are contiguous collections of
          // pointers by design
          // NOTE : Here the static assert is only useful for determining
          // that primitives WITHIN one cell are different. However, we reuse
          // the same code to check for collisions between neighbors, where
          // its perfectly valid for the primitives to be same, but the tag
          // is a DifferentPrimitivesTag (as they belong to different
          // collections). Maybe the tag should be renamed to better express the
          // intent?

          /*
          static_assert(cpp17::is_same_v<FCPCollection, SCPCollection>,
                        "Hashgrids Meta failure!");
          */

          for (auto& first_primitive : first_primitives)
            for (auto& second_primitive : second_primitives) {
              HashGrids<C>::generate_contact(contacts, first_primitive,
                                             second_primitive);
            }
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Processes colliding primitives within a cell for this hash
         * grid
         *
         * This function generates all contacts between rigid bodies that are
         * local to each cell assigned to this hashgrid. The contacts are added
         * to the contact container `contacts`.
         *
         * \param contacts Contact container for the generated contacts.
         * \tparam PrimitiveToCollide Type of the primitive whose collisions are
         * to be processed within the cells
         */
        template <typename C>
        template <typename PrimitiveToCollide, typename Contacts>
        void HashGrids<C>::HashGrid::process_within(Cell const* const cell,
                                                    Contacts& contacts) const {
          // auto here is std::vector<PrimitiveToCollide*>
          // so std::vector<PrimitiveToCollide*> * const
          auto* const first_primitives =
              cell->template get_handle_to<PrimitiveToCollide>();
          //
          Expects(first_primitives != nullptr);

          // Primitives to collide against also includes the current primitive.
          using PrimitivesToCollideAgainst =
              decltype(suffix_of_<PrimitiveToCollide>());

          Exec::template for_each_type<PrimitivesToCollideAgainst>(
              [this, cell, first_primitives, &contacts](auto op) {
                using PrimitiveToCollideAgainst = tmpl::type_from<decltype(op)>;
                using Dispatch = tt::IsSamePrimitive<PrimitiveToCollide,
                                                     PrimitiveToCollideAgainst>;

                auto* const second_primitives =
                    cell->template get_handle_to<PrimitiveToCollideAgainst>();

                // There might not be any second primitive belonging to the same
                // cell
                if (second_primitives != nullptr) {
                  this->generate_contacts(contacts, *first_primitives,
                                          *second_primitives, Dispatch{});
                }
              });
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Processes colliding primitives between a cell and a
         * neighboring for the current hash grid
         *
         * This function generates all contacts between rigid bodies that are
         * spread over one cell and one of its neighbor cells assigned to this
         * hashgrid. The contacts are added to the contact container `contacts`.
         *
         * \param contacts Contact container for the generated contacts.
         * \tparam PrimitiveToCollide Type of the primitive whose collisions are
         * to be processed within the cells
         */
        template <typename C>
        template <typename PrimitiveToCollide, typename Contacts>
        void HashGrids<C>::HashGrid::process_between(
            Cell const* const current_cell, Cell const* const neighboring_cell,
            Contacts& contacts) const {
          auto* const first_primitives =
              current_cell->template get_handle_to<PrimitiveToCollide>();
          //
          Expects(first_primitives != nullptr);

          // Note : here we should do processing for all primitives
          // and not only the lower half of the primitive matrix
          using PrimitivesToCollideAgainst = AdmissiblePrimitives;

          Exec::template for_each_type<PrimitivesToCollideAgainst>(
              [this, first_primitives, neighboring_cell, &contacts](auto op) {
                using PrimitiveToCollideAgainst = tmpl::type_from<decltype(op)>;
                using Dispatch = DifferentPrimitivesOrCellsTag;

                auto* const second_primitives =
                    neighboring_cell
                        ->template get_handle_to<PrimitiveToCollideAgainst>();

                // There might not be any second primitive belonging to the same
                // cell
                if (second_primitives != nullptr) {
                  this->generate_contacts(contacts, *first_primitives,
                                          *second_primitives, Dispatch{});
                }
              });
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Processes colliding primitives internal to one hash-grid
         *
         * This function generates all contacts between rigid bodies that are
         * local to the current hashgrid. The contacts are added to the contact
         * container `contacts`.
         *
         * \param contacts Contact container for the generated contacts.
         */
        template <typename C>         // Type of the configuration
        template <typename Contacts>  // Contact container type
        void HashGrids<C>::HashGrid::process_v2(Contacts& contacts) const {
          // Wraps process_within_cell and process_with_neighbors
          Exec::template for_each_type<AdmissiblePrimitives>(
              [this, &contacts](auto p) {
                using PrimitiveToCollide = tmpl::type_from<decltype(p)>;
                auto const& occupied_cells =
                    this->get_cells_occupied_by<PrimitiveToCollide>();

                Exec::for_each(
                    std::cbegin(occupied_cells), std::cend(occupied_cells),
                    [this, &contacts](Cell const* const cell) {
                      // each thread works on subset of cells
                      this->process_within<PrimitiveToCollide>(cell, contacts);

                      // loop for half number of neighbors
                      constexpr auto hnn = get_half_number_of_neighbors();

                      for (std::size_t i = 0; i < hnn; ++i) {
                        Cell const* const neighboring_cell =
                            cell + cell->neighbor_offset_[i];
                        this->process_between<PrimitiveToCollide>(
                            cell, neighboring_cell, contacts);
                      }  // end neighbor iteration
                    });
              });
          // At this point all operations within the grid are performed
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Transfer primitives from a finer grid to an intermediate
         * buffer, in order to transfer those to a coarser grid
         *
         * \details
         * Get pointers to primitives of a certain type residing in one
         * hash grid in an intermediate buffer, to detect contacts with hash
         * grids higher up the chain.
         *
         * The memory for output_primitives_buffer is supposed to be managed
         * from the caller scope (C style), as it's a pointer of pointers.
         *
         * We only copy a bunch of pointers so the cost is 8 * n_primitives in
         * grid
         *
         * \param output_primitives_buffer Output buffer to copy the primitives
         * \return Number of primitives transferred as meta-data
         */
        template <typename C>
        template <typename Primitive>
        std::size_t HashGrids<C>::HashGrid::transfer_primitives(
            to_t /* meta */, Primitive** output_primitives_buffer) const {
          // vector of cell pointers
          auto const& occupied_cells = get_cells_occupied_by<Primitive>();

          std::size_t count = 0UL;

          for (Cell const* const cell : occupied_cells) {
            // Get pointer to vector of pointers to primitives
            auto const* const primitives =
                cell->template get_handle_to<Primitive>();
            Expects(primitives != nullptr);

            // // v1
            // for (auto ptr : *primitives) {
            //   output_primitives_buffer[count] = ptr;
            //   ++count;  // I can also do output_primitives_buffer[count++]
            // }

            // v2, seems faster in quickbench.
            std::copy(primitives->cbegin(), primitives->cend(),
                      output_primitives_buffer + count);
            count += primitives->size();
          }

          // Cannot have no primitives.
          Ensures(count > 0UL);
          return count;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Processes (Generates contacts) for primitives from the
         * intermediate buffer populated by #transfer_primitives against those
         * stored in the current grid.
         *
         * \param primitives_from_finer_grid
         * \param n_primitives_from_finer_grid
         * \param contacts Contact container for the generated contacts.
         */
        template <typename C>
        template <typename PrimitiveFromFinerGrid, typename Contacts>
        void HashGrids<C>::HashGrid::generate_contacts_from_finer_grid(
            PrimitiveFromFinerGrid** primitives_from_finer_grid,
            std::size_t n_primitives_from_finer_grid,
            Contacts& contacts) const {
          /* Order of processing
           * Implemented : Data-friendly (?!) version, process each neighbor
           * cell for all primitive types before moving on
           *
           * For each primitive type from finer grid
           *  Iterate over primitives
           *   Calculate cell in which the primitives lie
           *    Iterate over this cell's neighbors
           *        For each colliding primitives
           *            iterate over neighbors' colliding primitive collection
           *
           *
           * Instruction friendly version : process each neighbor cell only
           * for a particular primitive type. Then repeat iterations over
           * neighbors for every primitive type. Seems wasteful at first
           * glance.
           */
          // Here we should do processing for all primitives
          // and not only the lower half of the primitive matrix as
          // they come from different collections
          using CollidingPrimitivesFromCurrentGrid = AdmissiblePrimitives;
          using PrimitiveID = PrimitiveFromFinerGrid*;

          PrimitiveID* const begin_fine_grid_primitives =
              primitives_from_finer_grid;
          PrimitiveID* const end_fine_grid_primitives =
              (primitives_from_finer_grid + n_primitives_from_finer_grid);

          // For each primitive 'p' that is stored in
          // 'primitives_from_finer_grid' ...
          Exec::for_each(
              begin_fine_grid_primitives, end_fine_grid_primitives,
              [this, &contacts](PrimitiveID fine_grid_primitive) {
                Cell const* const cell =
                    this->cell_ + this->hash(fine_grid_primitive);

                // for each neighbor cell
                for (auto i = 0UL; i < get_number_of_neighbors(); ++i) {
                  Cell const* const nbCell = cell + cell->neighbor_offset_[i];

                  // for each type
                  // this should be refactored
                  tmpl::for_each<CollidingPrimitivesFromCurrentGrid>(
                      [fine_grid_primitive, nbCell, &contacts](auto cp) {
                        using PotentiallyCollidingPrimitiveFromCurrentGrid =
                            tmpl::type_from<decltype(cp)>;
                        using PCPC =
                            PotentiallyCollidingPrimitiveFromCurrentGrid;

                        auto* const current_grid_primitives =
                            nbCell->template get_handle_to<PCPC>();

                        // the neighboring cells may not have any primitive
                        if (current_grid_primitives != nullptr) {
                          // for each colliding neighbor
                          auto const b = current_grid_primitives->begin();
                          auto const e = current_grid_primitives->end();

                          for (auto second_primitive = b; second_primitive != e;
                               ++second_primitive) {
                            HashGrids<C>::generate_contact(contacts,
                                                           fine_grid_primitive,
                                                           *second_primitive);
                          }  // second primitive
                        };   // null check
                      });    // type of second primitive
                }            // neighbor
              });            // first primitive
        }
        //**********************************************************************

        //======================================================================
        //
        //  CONSTRUCTOR
        //
        //======================================================================

        //**********************************************************************
        /*!\brief Constructor for the HashGrids class.
         */
        template <typename C>  // Type of the configuration
        HashGrids<C>::HashGrids(DomainType const* domain, Parameters parameters)
            : DomainPolicyType(domain),
              StoragePolicy(),
              grid_list_(),
              grid_active_(false),
              transfer_cache_(),
              parameters_(std::move(parameters)),
              exhaustive_search(domain) {
          // grid_active_ is set to false
          // As long as the number of bodies is less-or-equal than specified
          // by 'gridActivationThreshold', no hierarchy of hash grids is
          // constructed. Instead, pairwise collision checks are performed.
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief No-parameter constructor for the HashGrids class.
         */
        template <typename C>  // Type of the configuration
        HashGrids<C>::HashGrids(DomainType const* domain)
            : HashGrids(domain, Parameters{}) {}
        //**********************************************************************

        //======================================================================
        //
        //  DESTRUCTOR
        //
        //======================================================================

        //**********************************************************************
        /*!\brief Destructor for the HashGrids class.
         */
        template <typename C>  // Type of the configuration
        HashGrids<C>::~HashGrids() {
          // Delete all grids that are stored in the grid hierarchy (=>
          // gridList_).
          for (HashGrid* grid : grid_list_)
            delete grid;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Checks whether two bodies are colliding or not, and if so
         * generates the related points of contact.
         *
         * \param first_primitive The first body.
         * \param second_primitive The second body.
         * \param contacts Contact container for the generated contacts.
         * \return void
         */
        template <typename C>
        template <typename FirstPrimitive, typename SecondPrimitive,
                  typename Contacts>
        void HashGrids<C>::generate_contact(
            Contacts& contacts,
            FirstPrimitive* const __restrict__ first_primitive,
            SecondPrimitive* const __restrict__ second_primitive) {
          if (
              // Testing for overlapping bounding boxes, more probable
              overlaps(first_primitive->get_AABB(),
                       second_primitive->get_AABB())
              // // All fixed primitives to be checked are pushed to imposed
              // // bodies,  hence skip checkinh
              //  Ignoring contacts between two fixed bodies
              // // && (!first_primitive->is_fixed() or
              // !second_primitive->is_fixed())
          ) {
            contacts.emplace_back(*first_primitive,
                                  *second_primitive);  // modify
          }
        }
        //**********************************************************************

        //======================================================================
        //
        //  ADD/REMOVE FUNCTIONS
        //
        //======================================================================

        //**********************************************************************
        /*!\brief Registering all rigid primitives with the coarse collision
         * detector.
         *
         * This function is a requirement for all coarse collision detectors.
         * It is automatically called every time a new rigid primitive is
         * added to the simulation world.
         */
        template <typename C>
        template <typename Simulator_>
        void HashGrids<C>::add_all_primitives(
            ::elastica::collision::tags::from_t, Simulator_& simulator) {
          // Get total size of primitives
          std::size_t const primitives_count = simulator.n_primitives();

          if (primitives_count >= parameters_.hashgrid_activation_threshold) {
            // do hashgrid

            // Serialize execution, okay since its an initialization loop.
            std::mutex primitive_add_mutex;

            simulator.for_each_primitive([&](auto& primitive) {
              std::lock_guard<std::mutex> guard(primitive_add_mutex);
              this->add(&primitive);
            });

            // Here allocate resources for a memory block of pointers which
            // we use to transfer between grids in the detection step
            // This only works since we know that the number of primitives
            // in the simulation is constant, and set at initialization time.
            // Revisit if the number of primitives grow.
            tmpl::for_each<AdmissiblePrimitives>([&](auto p) {
              using Primitive = tmpl::type_from<decltype(p)>;
              // We may be reserving more memory than required, for the worst
              // case scenario when all primitives belong to one grid, such as
              // when all elements are uniformly sized.
              // The upper bound is always `n_primitives`.

              get<Primitive>(this->transfer_cache_)
                  .resize(simulator.template n_primitives<Primitive>(),
                          nullptr);
            });

            grid_active_ = true;

          } else {
            // do exhaustive detection
            grid_active_ = false;
          }
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Registering a rigid primitive with the coarse collision
         * detector.
         *
         * \details
         * This function is a requirement for all coarse collision detectors.
         * It is automatically called every time a new rigid primitive is
         * added to the simulation world.
         *
         * \param primitive The primitive to be registered with the coarse
         * collision detector.
         * \return void
         */
        template <typename C>
        template <typename PrimitiveType>
        void HashGrids<C>::add(PrimitiveType* primitive) {
          // only finite primitives should even reach this point
          Expects(primitive->is_finite());

          real_t primitive_length = length(primitive->get_AABB());
          add_finite_primitives(primitive, primitive_length);
        }
        //**********************************************************************

        /*
         * AddPrimitivesToGrid : assumes finite and directly adds to the grid
         * AddImposedPrimitivesToGrid : can have both finite and non-finite
         * variants For finite bodies we should consider adding to grid only
         * when its size is favorable else we should mark it as non-finite and
         * add only to grid
         *
         * For DRY and SRP, we use
         *
         * Add(prim*) => Detects size and calls one of the two functions below
         * AddFinitePrimitivesToGrid(prim*, size) => adds to HHG
         * AddNonFinitePrimitivesToGrid(prim*, size) => adds to
         * non-grid-bodies
         */

        //**********************************************************************
        /*!\brief Adds a primitive to the hierarchical hash grids data
         * structure.
         *
         * \details
         * If the usage of hierarchical hash grids is activated, this function
         * is called every time a new rigid primitive is added to the data
         * structure. It may also be called during the update phase.
         *
         * \param primitive The primitive that is about to be added to the
         * hierarchical hash grids data structure.
         * \param primitive_length Rough size of the primitive to determine the
         * hash grid level
         * \return void
         */
        template <typename C>
        template <typename PrimitiveType>
        void HashGrids<C>::add_finite_primitives(PrimitiveType* primitive,
                                                 real_t primitive_length) {
          auto const hierarchy_factor = parameters_.hierarchy_factor;

          // Assign to a grid with suitably primitive_lengthd cells.
          HashGrid* grid = nullptr;

          if (grid_list_.empty()) {
            // If no hash grid yet exists in the hierarchy, an initial hash
            // grid is created based on the primitive's primitive_length.

            grid = new HashGrid(
                DomainPolicyType::adjust_cell_span(primitive_length *
                                                   std::sqrt(hierarchy_factor)),
                parameters());
          } else {
            // Check the hierarchy for a hash grid with suitably
            // primitive_lengthd cells - if such a grid does not yet exist, it
            // will be created.

            real_t cell_span = 0.0;
            // Loop via an iterator to insert
            for (auto g = grid_list_.begin(); g != grid_list_.end(); ++g) {
              grid = *g;
              cell_span = grid->get_cell_span();

              if (primitive_length < cell_span) {
                cell_span /= hierarchy_factor;
                if (primitive_length < cell_span) {
                  // Primitive is less than the current span
                  // but also less than the span one below, in
                  // which case we create a new grid and add
                  // it to the list
                  while (primitive_length < cell_span)
                    cell_span /= hierarchy_factor;

                  // No need for size policy here as cell_span is already
                  // appropriately sized, since
                  // cell_span = appropraitely_sized_cell * factor**k
                  grid =
                      new HashGrid(cell_span * hierarchy_factor, parameters());
                  // Insert at the iterator g pushes behind it!
                  grid_list_.insert(g, grid);
                }

                // Primitive is less than the current span
                // but more than the span one below, so add
                grid->add(primitive);
                primitive->set_grid(static_cast<void*>(grid));

                return;
              }
            }

            // The current object is big for all the current grids. So create
            // a new grid that can accomodate this primitive
            while (primitive_length >= cell_span)
              cell_span *= hierarchy_factor;

            // No need for size policy here as cell_span is already
            // appropriately sized, since
            // cell_span = appropraitely_sized_cell * factor**k
            grid = new HashGrid(cell_span, parameters());
          }

          grid->add(primitive);
          primitive->set_grid(static_cast<void*>(grid));

          grid_list_.push_back(grid);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Adds a primitive to the non grid data structure.
         *
         * \param primitive The primitive that is about to be added to the non
         * grid data structure. \return void
         *
         * For primitives that are nonfinite (a plane for example), we add
         * them to a separate buffer which are checked for collision pairwise.
         */
        template <typename C>              // Type of the configuration
        template <typename PrimitiveType>  // Type of the primitive
        void HashGrids<C>::add_nonfinite_primitives(
            PrimitiveType* /*primitive*/, real_t /* primitive_length */) {
          // The primitive - which is infinite in size - is marked as being
          // added to 'nonGridBodies_' by setting the grid pointer to NULL and
          // setting the cell-ID to '1'. Additionally, the hash value is used
          // to memorize the primitive's index position in the
          // 'nonGridBodies_' vector.
          /*
          auto& non_grid_primitives =
              get_non_grid_collection_of<PrimitiveType>();

          primitive->set_grid(nullptr);
          primitive->set_hash(non_grid_primitives.size());
          primitive->set_cell_ID(1);

          non_grid_primitives.push_back(primitive);
           */
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Deregistering a rigid body from the coarse collision
         * detector.
         *
         * \param body The body to be deregistered from the coarse collision
         * detector. \return void
         *
         * This function is a requirement for all coarse collision detectors.
         * It is automatically called every time a rigid body is removed from
         * the simulation world.
         */
        template <typename C>
        template <typename PrimitiveType>
        void HashGrids<C>::remove(PrimitiveType* /*primitive*/) {
          // TODO : Complete
          /*
          HashGrid* grid = static_cast<HashGrid*>(body->getGrid());

          // The body is stored in a hash grid from which it must be removed.
          if (grid != NULL) {
            grid->remove(body);
          }
          // The body's grid pointer is equal to NULL.
          // => The body is either stored in 'bodiesToAdd_' (-> cell-ID = 0)
          or
          // 'nonGridBodies_' (-> cell-ID = 1).
          else {
            if (body->getCellId() == 0) {
              // the body's hash value => index of this body in 'bodiesToAdd_'
              if (body->getHash() == bodiesToAdd_.size() - 1) {
                bodiesToAdd_.pop_back();
              } else {
                BodyID lastElement = bodiesToAdd_.back();
                bodiesToAdd_.pop_back();
                lastElement->setHash(body->getHash());
                bodiesToAdd_[body->getHash()] = lastElement;
              }
            } else {
              // the body's hash value => index of this body in
          'nonGridBodies_' if (body->getHash() == nonGridBodies_.size() - 1) {
                nonGridBodies_.pop_back();
              } else {
                BodyID lastElement = nonGridBodies_.back();
                nonGridBodies_.pop_back();
                lastElement->setHash(body->getHash());
                nonGridBodies_[body->getHash()] = lastElement;
              }
            }
          }
           */
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Contact generation between colliding rigid bodies.
         *
         * \details
         * This function generates all contacts between the rigid bodies
         * contained in the simulation world. The contacts are added to the
         * contact container \a contacts.
         *
         * \param contacts Contact container for the generated contacts.
         * \return void
         */
        template <typename C>
        template <typename PrimitivesCollection_,
                  typename ImposedPrimitivesCollection_,
                  typename Contacts>  // Contact container type
        void HashGrids<C>::operator()(
            PrimitivesCollection_& primitives_collection,
            ImposedPrimitivesCollection_& imposed_primitives_collection,
            Contacts& contacts) {
          // Serial
          /* TODO : Parallelization of update
           *  Bottlenecks and fixes:
                1. grid->update and grid->remove methods
                  Fixes :
                  1) mutex around the cell that is currently accessed (wont be
           that bad performance wise) 2) To keep track of the cells removed from
           occupied cells we can have a thread local buffer that reduces what
           cells need to be removed and remove after the step (REFACTOR!)
                2. add_finite_primitives and grid->add
                  1) More problematic because grids can be added or removed
                  Fixes :
                  Invert dependency : instead of parallel over primitives and
                  adding over grids
                  0) Have an addition buffer and removal buffer of size grid
           each where we put the primtives from each grid to be added/removed 1)
           Scan through primitives, see which grid it is to be added removed,
           compute its corresponding hash, set its new grid and hash (in the
           primitives itself). If a new grid is needed, put these in a separate
           buffer 2) Add new grids to the list location serially, pop items from
           the temporary buffer and process one-by-one (can be serial) 3) Over
           the grids in parallel-invoke style : go through the respective
                  addition/removal buffers and added primitive pointers
                  . Only limited parallelism here of n_grids

              Requirements:
          */
          using elastica::collision::for_each;
          using elastica::collision::get_collection;

          // Pre-processing : O(N), currently serial.
          // TODO : profile.
          if (grid_active_) {
            // 1. Update all moving primitive associations
            for_each(primitives_collection, [this](auto& primitive) {
              auto grid = static_cast<HashGrid*>(primitive.get_grid());
              Expects(grid != nullptr);

              real_t const primitive_length = length(primitive.get_AABB());
              real_t const cell_span = grid->get_cell_span();

              if (primitive_length >= cell_span or
                  primitive_length <
                      (cell_span / parameters_.hierarchy_factor)) {
                // TODO parallel
                grid->remove(&primitive);
                // We know apriori that primitives_ are finite
                this->add_finite_primitives(&primitive, primitive_length);

              } else {
                // Even if the association of the grid doesn't change
                // the cell association within a grid might change
                // TODO parallel
                grid->update(&primitive);
              }
            });

            // 2. Detection step

            // Contact generation by traversing through all hash grids (which
            // are sorted in ascending order with respect to the size of their
            // cells).

            // First completely process all primitives within each grid
            Exec::for_each(std::cbegin(grid_list_), std::cend(grid_list_),
                           [&contacts](HashGrid const* const grid) {
                             grid->process_v2(contacts);
                           });

            // Next process between grids.
            // The strategy here is to
            // Parallellize for each grid
            //   Parallelize over types
            //     Parallelize collision over higher grids
            // This parallelization strategy needs a composable parallel
            // backend. For this we need to:

            // 1. Keep count of types of primitives transferred, for each grid
            // Grids are stored as lists, so for fastish lookup, we use a map
            // here. Ideally we would use a vector.
            using StartCounter = CounterCache<AdmissiblePrimitives>;
            using StopCounter = StartCounter;
            // Stored as [start, stop) so adjacent start, stops are continous
            std::unordered_map<HashGrid const*,
                               std::pair<StartCounter, StopCounter>>
                transfer_counts;

            auto const b = grid_list_.cbegin();
            auto const e = grid_list_.cend();

            // 2. Populate the start and stop counts serially.
            // Not using parallel execution here since the work is keeping track
            // of counters.
            tmpl::for_each<AdmissiblePrimitives>([&, b, e](auto p) {
              using Primitive = tmpl::type_from<decltype(p)>;

              std::for_each(
                  b, e, [&, start_idx = 0UL](HashGrid const* grid) mutable {
                    auto& counts = transfer_counts[grid];
                    std::size_t const current_primitive_count =
                        cpp17::as_const(grid)
                            ->template get_number_of_primitives<Primitive>();
                    // primitive count may also be zero : we check this case
                    // explicitly below
                    // start index in buffer
                    get<Primitive>(counts.first) = start_idx;
                    start_idx += current_primitive_count;
                    // stop index in buffer
                    get<Primitive>(counts.second) = start_idx;
                  });
            });

            // For each grid,
            const auto n_grids = grid_list_.size();
            // We need access to grid iterators inside the lambda, so do it via
            // counting instead
            Exec::for_each(
                make_counting_iterator(0UL), make_counting_iterator(n_grids),
                [this, &contacts,
                 &transfer_counts = cpp17::as_const(transfer_counts), b,
                 e](auto const grid_idx) {
                  auto const grid_it = std::next(b, long(grid_idx));
                  auto* const grid = *grid_it;
                  auto f_it = transfer_counts.find(grid);

                  Expects(f_it != transfer_counts.end());

                  // For each type of primitive in grid
                  Exec::template for_each_type<AdmissiblePrimitives>(
                      [&, grid, &counts = f_it->second](auto p) {
                        using Primitive = tmpl::type_from<decltype(p)>;
                        using ::elastica::collision::detection::coarse::get;
                        auto const start_idx = get<Primitive>(counts.first);
                        auto const stop_idx = get<Primitive>(counts.second);
                        auto const n_idx = stop_idx - start_idx;

                        // Check for case with no primitives
                        if (stop_idx > start_idx) {
                          // 1. Allocate buffer for all pointers using a memory
                          // resource
                          Primitive** buffer_all_primitives =
                              get<Primitive>(transfer_cache_).data();
                          Primitive** buffer_all_primitives_from_current_grid =
                              buffer_all_primitives + start_idx;

                          // 2. Transfer all primitives to a buffer
                          std::size_t count_transferred =
                              grid->transfer_primitives(
                                  to, buffer_all_primitives_from_current_grid);

                          Ensures(start_idx + count_transferred == stop_idx);
                        }

                        // 3. Collide the transferred primitives with all
                        // primitives in higher grids This step is fused with
                        // the transfer to hide the IO latency for copying
                        // pointers by adding some computation in the same task.
                        //
                        // this seems like a bad pattern as we complete all
                        // primitives from one grid before moving onto another.
                        // Good : In each iteration of the grid the load is
                        // expected to be evenly distributed between all the
                        // primitives

                        Exec::for_each(
                            std::next(grid_it), e,
                            [&](HashGrid const* const coarser_grid) {
                              Primitive** buffer_all_primitives =
                                  get<Primitive>(transfer_cache_).data();
                              Primitive**
                                  buffer_all_primitives_from_current_grid =
                                      buffer_all_primitives + start_idx;

                              coarser_grid->generate_contacts_from_finer_grid(
                                  buffer_all_primitives_from_current_grid,
                                  n_idx, contacts);
                            });  // for each higher grid
                      });        // for each primitive type
                });              // for each grid

            // TODO : make finite imposed primitives part of the HashGrids step.

            // Pairwise test (=> contact generation) for all primitives with
            // imposed bodies, which usually do not have a finite span
            exhaustive_search.process(primitives_collection,
                                      imposed_primitives_collection, contacts);
          } else {
            exhaustive_search(primitives_collection,
                              imposed_primitives_collection, contacts);
          }
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Clears the coarse collision detector.
         *
         * \return void
         */
        template <typename C>  // Type of the configuration
        void HashGrids<C>::clear() {
          for (HashGrid* grid : grid_list_)
            delete grid;

          grid_list_.clear();

          grid_active_ = false;

          exhaustive_search.clear();
        }
        //**********************************************************************

      }  // namespace coarse

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica

//! A heterogenoous collection of a vector of imposed
//! primitive pointers
// ImposedPrimitiveStorage = tuple<vector<Plane>, vector<Spline>>
// NonGridPrimitiveStorage = tuple<vector<Plane*>, vector<Spline*>>
// NOT POSSIBLE Complete logic for NonGridPrimitiveStorage
//          using NonGridPrimitiveStorage =
//              tmpl::transform<ImposedPrimitiveStorage,
//                              std::add_pointer<tmpl::_1>>;
