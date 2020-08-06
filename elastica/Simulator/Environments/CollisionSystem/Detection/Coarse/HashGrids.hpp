//=================================================================================================
/*!
 *  \file pe/core/detection/coarse/HashGrids.h
 *  \brief Implementation of the 'Hierarchical Hash Grids' coarse collision
 * detection algorithm
 *
 *  Copyright (C) 2009 Klaus Iglberger
 *
 *  This file is part of pe.
 *
 *  pe is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 *  pe is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with
 * pe. If not, see <http://www.gnu.org/licenses/>.
 */
//=================================================================================================

#pragma once

//#include <algorithm>
#include <array>
#include <cmath>
#include <list>
#include <tuple>
#include <type_traits>

#include "elastica/DataStructures/TaggedTuple.hpp"
#include "elastica/ErrorHandling/Assert.hpp"
#include "elastica/ModuleSettings/HashGrids.hpp"
#include "elastica/Simulator/Environments/CollisionSystem/Detection/Coarse/ExhaustiveSearch.hpp"
#include "elastica/Simulator/Environments/CollisionSystem/Detection/Coarse/HashGridPolicies/HashGridsStoragePolicy.hpp"
#include "elastica/Simulator/Environments/CollisionSystem/Detection/Coarse/Tags.hpp"
#include "elastica/Simulator/Environments/CollisionSystem/Detection/Coarse/TypeTraits.hpp"
#include "elastica/Utilities/DefineTypes.h"
#include "elastica/Utilities/Requires.hpp"
#include "elastica/Utilities/StdHelpers.hpp"
#include "elastica/Utilities/TMPL.hpp"
#include "elastica/Utilities/TMPLDebugging.hpp"

namespace elastica {

  namespace collision {

    namespace detection {

      namespace coarse {

        template <typename C>  // Type of the configuration
        class HashGrids : private NonCopyable,
                          HashGridsStoragePolicy /*,
                            HashGridsSpanPolicy<typename C::DomainType> */
        {
         public:
          //**Type definitions**************************************************
          using EnhancedCollisionConfig = C;  //!< Type of the configuration.
          using This =
              HashGrids<EnhancedCollisionConfig>;  //!< Type of this HashGrids
                                                   //!< instance.

          //!< Types of primitives in the current simulation
          using AdmissiblePrimitives =
              typename EnhancedCollisionConfig::AdmissiblePrimitives;

          // Storage types
          //!< Type of the primitive storage.
          using PrimitiveStorage =
              typename EnhancedCollisionConfig::PrimitiveStorage;
          //!< Type of the imposed primitive storage.
          using ImposedPrimitiveStorage =
              typename EnhancedCollisionConfig::ImposedPrimitiveStorage;

          // Matrix types
          //!< Type of the primitive-primitive collision matrix.
          using PrimitivePrimitiveMatrix =
              typename EnhancedCollisionConfig::PrimitivePrimitiveMatrix;
          //!< Type of the primitive-imposed primitive collision matrix.
          using PrimitiveImposedPrimitiveMatrix =
              typename EnhancedCollisionConfig::PrimitiveImposedPrimitiveMatrix;
          //!< Lower triangular part of the primitive-primitive collision
          //!< matrix.
          using LowerTriangularPrimitivePrimitiveMatrix =
              typename EnhancedCollisionConfig::
                  LowerTriangularPrimitivePrimitiveMatrix;

          //          using BodyID =
          //              typename EnhancedCollisionConfig::BodyID;  //!< Handle
          //              for a rigid
          //!< body.

          //!< Type of the fine collision detector.
          using FineDetector = typename EnhancedCollisionConfig::FineDetector;
          //********************************************************************

          static constexpr auto get_number_of_primitives() noexcept
              -> std::size_t {
            return tmpl::size<AdmissiblePrimitives>::value;
          }

          static constexpr auto get_number_of_imposed_primitives() noexcept
              -> std::size_t {
            return tmpl::size<ImposedPrimitiveStorage>::value;
          }

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

          // DONE Complete logic for CellPrimitives
          using CellPrimitives = tmpl::as_tuple<tmpl::transform<
              AdmissiblePrimitives, AddCellPrimitivesStorage<tmpl::_1>>>;

          //          TypeDisplayer<CellPrimitives> aeou;

          using OccupiedCellsIDs =
              std::array<std::size_t, get_number_of_primitives()>;

          using PrimitiveCounts =
              std::array<std::size_t, get_number_of_primitives()>;

          //! A heterogenoous collection of a vector of imposed
          //! primitive pointers
          // ImposedPrimitiveStorage = tuple<vector<Plane>, vector<Spline>>
          // NonGridPrimitiveStorage = tuple<vector<Plane*>, vector<Spline*>>

          // NOT POSSIBLE Complete logic for NonGridPrimitiveStorage
          //          using NonGridPrimitiveStorage =
          //              tmpl::transform<ImposedPrimitiveStorage,
          //                              std::add_pointer<tmpl::_1>>;

          // Types for literate addition and removal
          struct to_t {};
          struct from_t {};

          // Variables for literate addition and removal
          static constexpr from_t from{};
          static constexpr to_t to{};

          //**********************************************************************************************
          /*!\brief Implementation of the hash grid data structure.
           */
          class HashGrid {
           private:
            //**Type definitions************************************************
            //! The signed integer type that is used for storing offsets to
            //! neighboring cells.
            using offset_t = long;
            //******************************************************************

            //******************************************************************
            /*!\brief Data structure for representing a cell in the hash grid
            (used by the 'Hierarchical
            //        Hash Grids' coarse collision detection algorithm).
            */
            // sizof(Cell) = n_primitive_types * (8 (=pointer) + 8
            // (=std::size_t))
            // + 8 (pointer to offset)

            // TODO : While default constructing a Cell, maybe ensure that
            // occupiedCellIDs have SIZE_MAX as the default value for error
            // checking?
            struct Cell {
              CellPrimitives cell_primitives_; /*!< \brief The cell's body
                                      container that stores (handles to) all the
                                      bodies that are assigned to this cell. */
              /*!< Note that only a pointer to such a body container is
                   stored: in order to save memory, this container object
                   is only allocated as long as there are bodies assigned
                   to this cell. */
              offset_t* neighborOffset_; /*!< \brief Pointer to an array that is
                                            storing offsets that can be used to
                                            directly access all the neighboring
                                              cells in the hash grid. */
              OccupiedCellsIDs
                  occupiedCellsIds_;  //!< The cell's index in the
                                      //!< \a occupiedCells_ vector.

              // Only constexpr here
              // No reference explicitly, its a pointer already. The pointed
              // value can change, but the pointer itself isnt
              template <typename PrimitiveType>  // Type of the primitive
              inline constexpr auto get_handle_of() noexcept ->
                  typename AddCellPrimitivesStorage<PrimitiveType>::type {
                // DONE Complete logic for CellPrimitives
                using CellPrimitiveType =
                    typename AddCellPrimitivesStorage<PrimitiveType>::type;
                return std::get<CellPrimitiveType>(cell_primitives_);
              }

              template <typename PrimitiveType>  // Type of the primitive
              inline constexpr auto set_handle_of() noexcept ->
                  typename AddCellPrimitivesStorage<PrimitiveType>::type& {
                // DONE Complete logic for CellPrimitives
                using CellPrimitiveType =
                    typename AddCellPrimitivesStorage<PrimitiveType>::type;
                return std::get<CellPrimitiveType>(cell_primitives_);
              }

              template <typename PrimitiveType>  // Type of the primitive
              inline constexpr auto get_occupied_cellID() const noexcept
                  -> std::size_t {
                return std::get<
                    tmpl::index_of<AdmissiblePrimitives, PrimitiveType>::value>(
                    occupiedCellsIds_);
              }

              template <typename PrimitiveType>  // Type of the primitive
              inline constexpr auto set_occupied_cellID() noexcept
                  -> std::size_t& {
                return std::get<
                    tmpl::index_of<AdmissiblePrimitives, PrimitiveType>::value>(
                    occupiedCellsIds_);
              }
            };

            //**Type definitions************************************************
            //! Vector for storing pointers to (body-occupied) cells.
            using CellVector = typename std::vector<Cell*>;
            //******************************************************************

            // Tagged tuple of cell vectors, with tags being the admissible
            // primitives
            template <typename T>
            struct CellVectorStorageTag {
              using data_type = CellVector;
            };

            template <typename Primitive>
            struct AddCellVectorStorage {
              using type = CellVectorStorageTag<Primitive>;
            };

            using OccupiedCellStorage = tmpl::as_tagged_tuple<tmpl::transform<
                AdmissiblePrimitives, AddCellVectorStorage<tmpl::_1>>>;

           public:
            //**Constructor*****************************************************
            /*!\name Constructor */
            //@{
            explicit HashGrid(real_t cellSpan);
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
            real_t get_cell_span() const noexcept {
              return cellSpan_;
            }  //!< Getter for \a cellSpan_.

            // While this should remain private, we need it for communication
            // in between hash grids, so promote to public
            inline auto get_total_primitive_count() const noexcept
                -> std::size_t {
              std::size_t total_count_primitives(0UL);

              // this explicit to silence warnings in clang
              auto reducer = [this, &total_count_primitives](auto Is) {
                total_count_primitives += this->get_count<Is>();
              };
              index_apply<get_number_of_primitives()>([&reducer](auto... Is) {
                EXPAND_PACK_LEFT_TO_RIGHT(reducer(Is));
              });

              return total_count_primitives;
            }

            // Needed for transferring primitives in HashGrids, so promoted to
            // public
            template <typename PrimitiveType,
                      Requires<tmpl::list_contains_v<AdmissiblePrimitives,
                                                     PrimitiveType>> = nullptr>
            inline constexpr auto get_count() noexcept -> std::size_t& {
              // Could be decltype(auto).
              return std::get<
                  tmpl::index_of<AdmissiblePrimitives, PrimitiveType>::value>(
                  primitive_count_);
            }

            template <typename PrimitiveType,
                      Requires<tmpl::list_contains_v<AdmissiblePrimitives,
                                                     PrimitiveType>> = nullptr>
            inline constexpr auto get_count() const noexcept -> std::size_t {
              return std::get<
                  tmpl::index_of<AdmissiblePrimitives, PrimitiveType>::value>(
                  primitive_count_);
            }

            template <std::size_t PrimitiveIndex,
                      Requires<(PrimitiveIndex <= get_number_of_primitives())> =
                          nullptr>
            inline constexpr decltype(auto) get_count() /*const*/
                noexcept
            /*-> std::size_t&*/ {
              return std::get<PrimitiveIndex>(primitive_count_);
            }

            template <std::size_t PrimitiveIndex,
                      Requires<(PrimitiveIndex <= get_number_of_primitives())> =
                          nullptr>
            inline constexpr auto get_count() const noexcept -> std::size_t {
              return std::get<PrimitiveIndex>(primitive_count_);
            }

            /*
            template <typename PrimitiveType,
                      Requires<tmpl::list_contains_v<AdmissiblePrimitives,
                                                     PrimitiveType>> = nullptr>
            inline constexpr auto set_count() noexcept -> std::size_t& {
              return std::get<
                  tmpl::index_of<AdmissiblePrimitives, PrimitiveType>::value>(
                  primitive_count_);
            }

            template <std::size_t PrimitiveIndex,
                      Requires<(PrimitiveIndex <= get_number_of_primitives())> =
                          nullptr>
            inline constexpr auto set_count() noexcept -> std::size_t& {
              return std::get<PrimitiveIndex>(primitive_count_);
            }
             */

            //@}
            //******************************************************************

            //**Add/remove functions********************************************
            /*!\name Add/remove functions */
            //@{
            template <typename PrimitiveType>  // Type of the primitive
            inline void add(PrimitiveType* primitive);
            template <typename PrimitiveType>  // Type of the primitive
            inline void remove(PrimitiveType* primitive);
            //@}
            //******************************************************************

            //**Utility functions***********************************************
            /*!\name Utility functions */
            //@{
            template <typename PrimitiveType>  // Type of the primitive
            void update(PrimitiveType* primitive);

            void clear();
            //@}
            //******************************************************************

            //**Contact functions***********************************************
            /*!\name Contact functions */

            template <typename Contacts>
            void process_grid(Contacts& contacts) const;

            template <typename PrimitiveType>
            std::size_t transfer_primitives(
                to_t /* meta */,
                PrimitiveType** output_primitives_buffer) const;

            template <typename FCP, typename Contacts>
            void process_primitives_from_finer_grid(
                FCP** primitives_from_finer_grid,
                size_t n_primitives_from_finer_grid, Contacts& contacts) const;
            //@}
            //******************************************************************

           private:
            //**Utility
            // functions********************************************************
            /*!\name Utility functions */
            //@{
            void initializeNeighborOffsets();

            template <typename PrimitiveType>  // Type of the primitive
            std::size_t hash(PrimitiveType* primitive) const noexcept;

            void enlarge();
            //@}
            //******************************************************************

            //**Add/remove functions********************************************
            /*!\name Add/remove functions */
            //@{

            template <typename PrimitiveType>  // Type of the primitive
            void add(PrimitiveType* primitive, to_t /* meta */, Cell* cell);

            template <typename PrimitiveType>  // Type of the primitive
            void remove(PrimitiveType* primitive, from_t /* meta */,
                        Cell* cell);

            //@}
            //******************************************************************

            //**Contact functions***********************************************
            /*!\name Contact functions */

            // TODO These can be extracted as free functions and reused in
            // Exhaustive search too
            template <typename Contacts, typename FCPS, typename SCPS>
            void process_primitives(Contacts& contacts, FCPS& first_primitives,
                                    SCPS&, tags::SamePrimitivesTag) const;

            template <typename Contacts, typename FCPS, typename SCPS>
            void process_primitives(Contacts& contacts, FCPS& first_primitives,
                                    SCPS& second_primitives,
                                    tags::DifferentPrimitivesTag) const;

            template <typename FCP, typename Contacts>
            void process_within_cell(Contacts& contacts) const;

            template <typename FCP, typename Contacts>
            void process_neighboring_cells(Contacts& contacts) const;

            template <typename FCP, typename Contacts>
            void process_neighboring_cells_version_two(
                Contacts& contacts) const;

            //@}
            //******************************************************************

            //**Get functions***************************************************
            /*!\name Get functions */
            //@{

            inline auto get_number_of_primitives_currently_in(
                Cell* cell) const noexcept -> std::size_t {
              // Iterate over the tuple and reduce
              std::size_t n_primitives(0UL);
              auto reducer = [&n_primitives](auto* primitive_vector) {
                n_primitives +=
                    primitive_vector == nullptr ? 0 : primitive_vector->size();
              };
              index_apply<get_number_of_primitives()>(
                  [cell, &reducer](auto... Is) {
                    EXPAND_PACK_LEFT_TO_RIGHT(
                        reducer(std::get<Is>(cell->cell_primitives_)));
                  });
              return n_primitives;
            }

            template <typename PrimitiveType,
                      Requires<tmpl::list_contains_v<AdmissiblePrimitives,
                                                     PrimitiveType>> = nullptr>
            inline constexpr auto get_cells_occupied_by() noexcept
                -> CellVector& {
              return std::get<CellVectorStorageTag<PrimitiveType>>(
                  occupiedCells_);
            }

            template <typename PrimitiveType,
                      Requires<tmpl::list_contains_v<AdmissiblePrimitives,
                                                     PrimitiveType>> = nullptr>
            inline constexpr auto get_cells_occupied_by() const noexcept
                -> CellVector const& {
              return std::get<CellVectorStorageTag<PrimitiveType>>(
                  occupiedCells_);
            }

            /* Disallow indexing based access for now
            template <std::size_t PrimitiveIndex,
                      Requires<(PrimitiveIndex <= get_number_of_primitives())> =
                          nullptr>
            inline constexpr auto get_cells_occupied_by() noexcept
                -> CellVector& {
              return std::get<CellVectorStorageTag<
                  tmpl::at_c<AdmissiblePrimitives, PrimitiveIndex>>>(
                  occupiedCells_);
            }
             */

            inline static constexpr auto number_of_neighbors() -> std::size_t {
              return 27UL;
            }

            inline static constexpr auto half_number_of_neighbors()
                -> std::size_t {
              return 13UL;
            }
            //@}
            //******************************************************************

           private:
            //**Member
            // variables*************************************************************************
            /*!\name Member variables */
            //@{
            Cell* cell_;  //!< Linear array of cells representing the
                          //!< three-dimensional hash grid.

            std::size_t xCellCount_;  //!< Number of cells allocated in x-axis
                                      //!< direction.
            std::size_t yCellCount_;  //!< Number of cells allocated in y-axis
                                      //!< direction.
            std::size_t zCellCount_;  //!< Number of cells allocated in z-axis
                                      //!< direction.

            std::size_t
                xHashMask_;  //!< Bit-mask required for the hash calculation
                             //!< in x-axis direction (\a xCellCount_ - 1).
            std::size_t
                yHashMask_;  //!< Bit-mask required for the hash calculation
                             //!< in y-axis direction (\a yCellCount_ - 1).
            std::size_t
                zHashMask_;  //!< Bit-mask required for the hash calculation
                             //!< in z-axis direction (\a zCellCount_ - 1).

            std::size_t
                xyCellCount_; /*!< \brief Number of cells allocated in x-axis
                                 direction multiplied by the number of cells
                                 allocated in y-axis direction. */
            std::size_t xyzCellCount_;  //!< Total number of allocated cells.

            std::size_t
                enlargementThreshold_; /*!< \brief The enlargement threshold
                                     - the moment the number of assigned
                                     bodies exceeds this threshold, the
                                              size of this hash grid is
                                     increased. */

            real_t cellSpan_;  //!< The grid's cell size (edge length of the
                               //!< underlying cubic grid cells).
            real_t inverseCellSpan_;  //!< The inverse cell size.
            /*!< Required for replacing floating point divisions with
               multiplications during the hash computation. */

            OccupiedCellStorage
                occupiedCells_;  //!< A grid-global list that keeps track
                                 //!< of all body-occupied cells.
            /*!< The list is required in order to avoid iterating through all
                 grid cells whenever all bodies that are stored in the grid
                 need to be addressed.  */

            PrimitiveCounts primitive_count_;  //!< Number of primitives
                                               //!< assigned to this hash grid.

            offset_t
                stdNeighborOffset_[number_of_neighbors()]; /*!< \brief Array
                                           of offsets to the neighboring cells
                                           (valid for all the inner cells of the
                                           hash grid). */
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
          HashGrids(PrimitiveStorage& primitivestorage,
                    ImposedPrimitiveStorage*& imposed_primitive_storage);
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
          void add_all_primitives();

          template <typename PrimitiveType>  // Type of the primitive
          void add(PrimitiveType* primitive);

          template <typename PrimitiveType>  // Type of the primitive
          void remove(PrimitiveType* primitive);

          //@}
          //********************************************************************

          //**Utility functions*************************************************
          /*!\name Utility functions */
          void clear();
          //@}
          //********************************************************************

          //**Contact functions*************************************************
          /*!\name Contact functions */

          template <typename Contacts>
          void find_contacts(Contacts& contacts);

          //@}
          //********************************************************************

         protected:
          //**Contact functions*************************************************
          /*!\name Contact functions */
          //@{
          template <typename FirstPrimitive, typename SecondPrimitive,
                    typename Contacts>
          static inline void collide(FirstPrimitive* first_primitive,
                                     SecondPrimitive* second_primitive,
                                     Contacts& contacts);
          //@}
          //********************************************************************

          //**Get functions*****************************************************
          /*!\name Get/remove functions */
          //@{
          inline auto get_total_primitive_count_in_simulation() const noexcept
              -> std::size_t {
            std::size_t total_count_primitives(0UL);

            auto reducer = [this, &total_count_primitives](auto Is) {
              total_count_primitives += std::get<Is>(primitives_).size();
            };

            index_apply<get_number_of_primitives()>([&reducer](auto... Is) {
              EXPAND_PACK_LEFT_TO_RIGHT(reducer(Is));
            });

            return total_count_primitives;
          }

          /*
          template <typename PrimitiveType>  // Type of the primitive
          inline constexpr auto get_non_grid_collection_of() noexcept
              -> AddPrimitiveStorageType<PrimitiveType>& {
            // TODO
            //            using PrimitiveCollectionType =
            //            AddPrimitiveStorageType<Primitive>; return
            //            std::get<PrimitiveCollectionType>(primitives_);
          }
           */

          template <std::size_t PrimitiveIdx>
          inline constexpr decltype(auto) get_primitive_collection() noexcept {
            return std::get<PrimitiveIdx>(primitives_);
          }

          //@}
          //********************************************************************
         private:
          // TODO : Put in policy class
          real_t adjust_span(real_t requested_cell_span) const noexcept;

          template <typename PrimitiveType>  // Type of the primitive
          void add_finite_primitives(PrimitiveType* primitive,
                                     real_t primitive_size);

          template <typename PrimitiveType>  // Type of the primitive
          void add_nonfinite_primitives(PrimitiveType* primitive,
                                        real_t primitive_size);

         private:
          //**Member variables**************************************************
          /*!\name Member variables */
          //@{
          // The lifetime of the referred variable will exceed the
          // CoarseDetectors lifetime Hence this operation is valid
          PrimitiveStorage&
              primitives_;  //!< Reference to the primitive storage.
          ImposedPrimitiveStorage*&
              imposed_primitives_;  //!< Pointer to the primitives that are
                                    //!< fixed as the simulation progresses
          // NonGridPrimitiveStorage
          //     non_grid_primitives_;  //!< Vector of all unassigned rigid
          //     bodies.
          /*!< This vector contains all bodies that are not assigned to any
               grid in the hierarchy. A body is not assigned to any grid if
               the body is infinite in size or if the detection algorithm
               based on hierarchical hash grids has not yet been activated
               (see \a gridActive_). */

          GridList grid_list_;  //!< List of all grids that form the hierarchy.
          /*!< The grids in this list are sorted in ascending order by the
               size of their cells. */
          bool grid_active_;  //!< Grid activation flag.
          /*!< This flag indicates whether the detection algorithm based on
               hierarchical hash grids is activated. If the simulation only
               consists of a very small number of bodies, each body is simply
               checked against every other body. This strategy proves to be
               faster than involving the far more complex mechanisms of the
               hierarchical hash grids. */
          ExhaustiveSearch<EnhancedCollisionConfig> exhaustive_search;

          //**Compile time checks***********************************************
          /*! \cond ELASTICA_INTERNAL */
          static_assert(
              std::is_same<
                  This, typename CollisionConfig::template CoarseDetectorFamily<
                            EnhancedCollisionConfig>>::value,
              "Should not be reached!");
          /*! \endcond */
          //********************************************************************
        };

        //=================================================================================================
        //
        //  CONSTRUCTOR
        //
        //=================================================================================================

        //*************************************************************************************************
        /*!\brief Constructor for the HashGrid class.
         */
        template <typename C>  // Type of the configuration
        HashGrids<C>::HashGrid::HashGrid(real_t cellSpan) {
          using namespace elastica::collision::detection::coarse::hhg;

          // Initialization of all member variables and ...
          xCellCount_ = initial_x_cell_count();
          yCellCount_ = initial_y_cell_count();
          zCellCount_ = initial_z_cell_count();

          xHashMask_ = xCellCount_ - 1;
          yHashMask_ = yCellCount_ - 1;
          zHashMask_ = zCellCount_ - 1;

          xyCellCount_ = xCellCount_ * yCellCount_;
          xyzCellCount_ = xyCellCount_ * zCellCount_;

          enlargementThreshold_ = xyzCellCount_ / minimal_grid_density();

          // ... allocation of the linear array that is representing the hash
          // grid.
          cell_ = new Cell[xyzCellCount_];

          // Initialization of each cell - i.e., initially setting the pointer
          // to the body container to NULL (=> no bodies are assigned to this
          // hash grid yet!) and ...

          {
            Cell* c = cell_;

            auto null_initializer = [&c](auto Is) {
              using PrimitiveType = tmpl::at_c<AdmissiblePrimitives, Is>;
              c->template set_handle_of<PrimitiveType>() = nullptr;
            };

            for (; c < cell_ + xyzCellCount_; ++c) {
              index_apply<get_number_of_primitives()>(
                  [&null_initializer](auto... Is) {
                    EXPAND_PACK_LEFT_TO_RIGHT(null_initializer(Is));
                  });
            }
          }

          // ... setting up the neighborhood relationship (using the offset
          // array).
          initializeNeighborOffsets();

          cellSpan_ = cellSpan;
          inverseCellSpan_ = static_cast<real_t>(1) / cellSpan;

          // this explicit to silence warnings in clang
          index_apply<get_number_of_primitives()>([this](auto... Is) {
            EXPAND_PACK_LEFT_TO_RIGHT(
                this->get_cells_occupied_by<
                        tmpl::at_c<AdmissiblePrimitives, Is>>()
                    .reserve(initial_occupied_cells_vector_size()));
          });

          // Set all primitive counts to 0
          // this explicit to silence warnings in clang
          index_apply<get_number_of_primitives()>([this](auto... Is) {
            EXPAND_PACK_LEFT_TO_RIGHT(this->get_count<Is>() = 0UL);
          });
        }
        //*************************************************************************************************

        //=================================================================================================
        //
        //  DESTRUCTOR
        //
        //=================================================================================================

        //*************************************************************************************************
        /*!\brief Destructor for the HashGrid class.
         */
        template <typename C>  // Type of the configuration
        HashGrids<C>::HashGrid::~HashGrid() {
          clear();

          for (Cell* c = cell_; c < cell_ + xyzCellCount_; ++c) {
            // if its not an interior cell
            if (c->neighborOffset_ != stdNeighborOffset_)
              delete[] c->neighborOffset_;
          }
          delete[] cell_;
        }
        //*************************************************************************************************

        //=================================================================================================
        //
        //  ADD/REMOVE FUNCTIONS
        //
        //=================================================================================================

        //*************************************************************************************************
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
        template <typename C>              // Type of the configuration
        template <typename PrimitiveType>  // Type of the primitive
        void HashGrids<C>::HashGrid::add(PrimitiveType* primitive) {
          // TODO : You can exercise more control here, and add it as a policy
          // If adding the primitive will cause the total number of primitives
          // assigned to this grid to exceed the enlargement threshold, the size
          // of this hash grid must be increased.
          if (get_total_primitive_count() == enlargementThreshold_)
            HashGrid::enlarge();

          // Calculate (and store) the hash value (= the primitive's cell
          // association) and ...
          std::size_t h(hash(primitive));
          primitive->set_hash(h);

          // ... insert the primitive into the corresponding cell.
          Cell* cell = cell_ + h;
          add(primitive, to, cell);

          // Raise the corresponding primitive count
          ++get_count<PrimitiveType>();
        }
        //*************************************************************************************************

        //*************************************************************************************************
        /*!\brief Removes a body from this hash grid.
         *
         * \param body The body that is about to be removed from this hash grid.
         * \return void
         *
         * This function is called every time a rigid body is removed from this
         * grid. It may also be called during the update phase.
         */
        template <typename C>              // Type of the configuration
        template <typename PrimitiveType>  // Type of the primitive
        void HashGrids<C>::HashGrid::remove(PrimitiveType* primitive) {
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
          --get_count<PrimitiveType>();
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
         * updated. \return void
         *
         * Checks if a given rigid primitive can remain stored in its currently
         * assigned cell - and if not, removes the primitive from this cell and
         * reassigns it to another cell (which is identified by re-evaluating
         * the primitive's hash value).
         */
        template <typename C>
        template <typename PrimitiveType>  // Type of the primitive
        void HashGrids<C>::HashGrid::update(PrimitiveType* primitive) {
          // DONE : primitive is parallel
          // The hash value is recomputed based on the primitive's current
          // spatial location.
          std::size_t new_hash = hash(primitive);
          std::size_t old_hash = primitive->get_hash();

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
        /*!\brief Clears the hash grid.
         *
         * \return void
         *
         * This function removes all (the handles to) the bodies that are
         * assigned to this hash grid from the body containers of the grid's
         * cells.
         */
        template <typename C>  // Type of the configuration
        void HashGrids<C>::HashGrid::clear() {
          auto clearer = [this](auto Idx) {
            using PrimitiveType = tmpl::at_c<AdmissiblePrimitives, Idx>;
            auto& primitive_occupied_cells =
                get_cells_occupied_by<PrimitiveType>();
            for (Cell* cell : primitive_occupied_cells) {
              auto* primitives = cell->template get_handle_of<PrimitiveType>();
              delete primitives;
              primitives = nullptr;
            }
            primitive_occupied_cells.clear();
            std::get<Idx>(primitive_count_) = 0;
          };

          index_apply<get_number_of_primitives()>([&clearer](auto... Is) {
            EXPAND_PACK_LEFT_TO_RIGHT(clearer(Is));
          });
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
        template <typename C>  // Type of the configuration
        void HashGrids<C>::HashGrid::initializeNeighborOffsets() {
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
                  c->neighborOffset_ = new offset_t[27];

                  i = 0;
                  for (offset_t zz = -xyc; zz <= xyc; zz += xyc) {
                    offset_t zo = zz;
                    if (z == 0 && zz == -xyc) {
                      zo = xyzc - xyc;
                    } else if (z == (zc - 1) && zz == xyc) {
                      zo = xyc - xyzc;
                    }

                    for (offset_t yy = -xc; yy <= xc; yy += xc) {
                      offset_t yo = yy;
                      if (y == 0 && yy == -xc) {
                        yo = xyc - xc;
                      } else if (y == (yc - 1) && yy == xc) {
                        yo = xc - xyc;
                      }

                      for (offset_t xx = -1; xx <= 1; ++xx, ++i) {
                        offset_t xo = xx;
                        if (x == 0 && xx == -1) {
                          xo = xc - 1;
                        } else if (x == (xc - 1) && xx == 1) {
                          xo = 1 - xc;
                        }

                        c->neighborOffset_[i] = xo + yo + zo;
                      }
                    }
                  }
                }
                /* inner cell */
                else {
                  c->neighborOffset_ = stdNeighborOffset_;
                }
              }
            }
          }
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
        template <typename C>              // Type of the configuration
        template <typename PrimitiveType>  // Type of the primitive
        std::size_t HashGrids<C>::HashGrid::hash(
            PrimitiveType* primitive) const noexcept {
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
        //******************************************************************************

        //*************************************************************************************************
        /*!\brief Adds a body to a specific cell in this hash grid.
         *
         * \param body The body that is about to be added to this hash grid.
         * \param cell The cell the body is assigend to.
         * \return void
         */
        template <typename C>              // Type of the configuration
        template <typename PrimitiveType>  // Type of the primitive
        void HashGrids<C>::HashGrid::add(PrimitiveType* primitive,
                                         to_t /* meta */, Cell* cell) {
          // TODO The first part is parallel provided cell pointer
          //      is uniquely accessed within this function
          using namespace elastica::collision::detection::coarse::hhg;

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
          auto* primitives = cell->template get_handle_of<PrimitiveType>();

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
            //                get_number_of_primitives_currently_in(cell) > 0;

            using PrimitiveContainer =
                std::remove_pointer_t<decltype(primitives)>;
            cell->template set_handle_of<PrimitiveType>() =
                new PrimitiveContainer;
            //            primitives = new PrimitiveContainer;
            primitives = cell->template get_handle_of<PrimitiveType>();
            primitives->reserve(initial_cell_vector_size());

            primitive->set_cell_ID(0);
            primitives->push_back(primitive);

            // TODO If parallel, these need to be concurrent vectors which may be
            //      simultaneously modified
            // TODO Else have a thread local buffer that reduces the cells need
            //      to be added
            // Vector of cell pointers containing this primitive
            auto& occupied_cells = get_cells_occupied_by<PrimitiveType>();
            cell->template set_occupied_cellID<PrimitiveType>() =
                occupied_cells.size();
            occupied_cells.push_back(cell);

            //            // If cell is already occupied by primitives, leave
            //            alone
            //            // Else it is not in occupied cells, because no
            //            primitives
            //            // are present, and we insert into occupiedCells_
            //            if (!occupied) {
            //              cell->occupiedCellsIds_ = occupiedCells_.size();
            //              occupiedCells_.push_back(cell);
            //            }
          }
        }
        //*************************************************************************************************

        //*************************************************************************************************
        /*!\brief Removes a body from a specific cell in this hash grid.
         *
         * \param body The body that is about to be removed from this hash grid.
         * \param cell The cell the body is removed from.
         * \return void
         */
        template <typename C>              // Type of the configuration
        template <typename PrimitiveType>  // Type of the primitive
        void HashGrids<C>::HashGrid::remove(PrimitiveType* primitive,
                                            from_t /* meta */, Cell* cell) {
          // TODO The first part is parallel provided cell pointer
          //      is uniquely accessed within this function
          // Pointer to a vector of primitive pointers
          auto* primitives = cell->template get_handle_of<PrimitiveType>();

          ELASTICA_ASSERT(primitives != nullptr, "Shouldn't reach this point!");
          ELASTICA_ASSERT(primitives->size() > 0, "Shouldn't reach this point!");

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
            cell->template set_handle_of<PrimitiveType>() = nullptr;

            // ... the cell is removed from the grid-global vector
            // 'occupiedCells_' that records all body-occupied cells. Since the
            // cell memorized its index (=> 'occupiedCellsIds_') in this vector,
            // it can be removed in constant time, O(1).

            // Get values from cache
            // TODO If parallel, these need to be concurrent vectors which may be
            //      simultaneously modified
            // TODO Else have a thread local buffer that reduces what cells need
            //      to be removed and remove after the step
            auto& occupied_cells = get_cells_occupied_by<PrimitiveType>();
            const std::size_t occupied_cellID =
                cell->template get_occupied_cellID<PrimitiveType>();

            if (occupied_cellID == occupied_cells.size() - 1) {
              occupied_cells.pop_back();
            } else {
              // Exchange pointers with the last cell
              Cell* last_cell = occupied_cells.back();
              // Removes the cell, so don't need to erase it
              occupied_cells.pop_back();
              last_cell->template set_occupied_cellID<PrimitiveType>() =
                  occupied_cellID;
              occupied_cells[occupied_cellID] = last_cell;
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
              PrimitiveType* last_element = primitives->back();
              primitives->pop_back();
              last_element->set_cell_ID(cellID);
              (*primitives)[cellID] = last_element;
            }
          }
        }

        template <typename C>  // Type of the configuration
        void HashGrids<C>::HashGrid::enlarge() {
          using namespace elastica::collision::detection::coarse::hhg;

          // Not using smart pointers because smart pointer to smart pointers
          // is exhausting

          // Tuple of pointer to pointers (bodyIDs)
          using PrimitiveIDs =
              tmpl::transform<AdmissiblePrimitives, std::add_pointer<tmpl::_1>>;
          using PrimitiveIDHandleStorage = tmpl::as_tuple<
              tmpl::transform<PrimitiveIDs, std::add_pointer<tmpl::_1>>>;

          //! Storage of all previous primitives in the hash grid
          PrimitiveIDHandleStorage primitives;
          //! Handle to a current primitive ID, helper variable
          PrimitiveIDHandleStorage curr_primitive;

          // Create memory to backup all primitives assigned to this grid
          // and get handle to the first pointer
          auto allocator = [this, &primitives, &curr_primitive](auto Is) {
            using PrimitiveType = tmpl::at_c<AdmissiblePrimitives, Is>;
            using PrimitiveID = tmpl::at_c<PrimitiveIDs, Is>;
            std::get<Is>(primitives) =
                new PrimitiveID[this->get_count<PrimitiveType>()];
            std::get<Is>(curr_primitive) = std::get<Is>(primitives);
          };

          index_apply<get_number_of_primitives()>([&allocator](auto... Is) {
            EXPAND_PACK_LEFT_TO_RIGHT(allocator(Is));
          });

          //          using PrimitiveType = tmpl::at_c<AdmissiblePrimitives,
          //          Is>; using PrimitivePointer =
          //          std::add_pointer_t<PrimitiveType>; PrimitivePointer*
          //          primitives =
          //              new PrimitivePointer[get_count<PrimitiveType>()];
          //          PrimitivePointer* curr_primitive = primitives;

          // Do backup all primitives assigned to this grid
          auto copier = [this, &curr_primitive](auto Is) {
            // can use Is, but this is more expressive
            using PrimitiveType = tmpl::at_c<AdmissiblePrimitives, Is>;
            auto& occupied_cells = this->get_cells_occupied_by<PrimitiveType>();

            // We copy the pointer here (not a reference) and so it isnt updated
            PrimitiveType** curr = std::get<Is>(curr_primitive);

            for (Cell* cell : occupied_cells) {
              // Pointer to a vector of primitive pointers
              auto* cell_primitives = cell->template get_handle_of<PrimitiveType>();

              // Copy pointers
              for (auto* cell_primitive : *cell_primitives) {
                //*(curr_primitive++) = *primitive;
                *curr = cell_primitive;
                ++curr;
              }
            }

            // Now copy the new pointer back onto curr_primitive
            std::get<Is>(curr_primitive) = curr;
          };

          // All bodies that are assigned to this grid are temporarily
          // removed,
          index_apply<get_number_of_primitives()>(
              [&copier](auto... Is) { EXPAND_PACK_LEFT_TO_RIGHT(copier(Is)); });

          // ... the grid's current data structures are deleted, ...
          clear();

          for (Cell* c = cell_; c < cell_ + xyzCellCount_; ++c) {
            if (c->neighborOffset_ != stdNeighborOffset_)
              delete[] c->neighborOffset_;
          }
          delete[] cell_;

          // ... the number of cells is doubled in each coordinate direction,
          // ...
          xCellCount_ *= 2;
          yCellCount_ *= 2;
          zCellCount_ *= 2;

          xHashMask_ = xCellCount_ - 1;
          yHashMask_ = yCellCount_ - 1;
          zHashMask_ = zCellCount_ - 1;

          xyCellCount_ = xCellCount_ * yCellCount_;
          xyzCellCount_ = xyCellCount_ * zCellCount_;

          // ... a new threshold for enlarging this hash grid is set, ...
          enlargementThreshold_ = xyzCellCount_ / minimal_grid_density();

          // ... a new linear array of cells representing this enlarged hash
          // grid is allocated and ...
          cell_ = new Cell[xyzCellCount_];

          // ... initialized, and finally ...
          {
            Cell* c = cell_;

            auto null_initializer = [&c](auto Is) {
              using PrimitiveType = tmpl::at_c<AdmissiblePrimitives, Is>;
              c->template set_handle_of<PrimitiveType>() = nullptr;
            };

            for (; c < cell_ + xyzCellCount_; ++c) {
              index_apply<get_number_of_primitives()>(
                  [&null_initializer](auto... Is) {
                    EXPAND_PACK_LEFT_TO_RIGHT(null_initializer(Is));
                  });
            }
          }

          initializeNeighborOffsets();

          // At this stage all the current primitive points to the end
          auto reinserter = [this, &primitives, &curr_primitive](auto Is) {
            using PrimitiveType = tmpl::at_c<AdmissiblePrimitives, Is>;
            using PrimitivePointer = std::add_pointer_t<PrimitiveType>;

            auto end_ = std::get<Is>(curr_primitive);

            for (PrimitivePointer* p = std::get<Is>(primitives); p < end_;
                 ++p) {
              this->add(*p);
            }
          };

          // ... all previously removed bodies are reinserted.
          index_apply<get_number_of_primitives()>([&reinserter](auto... Is) {
            EXPAND_PACK_LEFT_TO_RIGHT(reinserter(Is));
          });

          // Don't forget to clear memory
          auto deleter = [&primitives](auto Is) {
            delete[](std::get<Is>(primitives));
          };

          index_apply<get_number_of_primitives()>([&deleter](auto... Is) {
            EXPAND_PACK_LEFT_TO_RIGHT(deleter(Is));
          });
        }

        //======================================================================
        //
        //  CONTACT FUNCTIONS
        //
        //======================================================================

        template <typename C>         // Type of the configuration
        template <typename Contacts,  // Contact container type
                  typename FCPS,  // Storage of the first colliding primitive
                  typename SCPS>  // Storage of the second colliding primitive
        void HashGrids<C>::HashGrid::process_primitives(
            Contacts& contacts, FCPS& first_primitives,
            SCPS& /*second_primitives*/,
            tags::SamePrimitivesTag /*meta*/) const {
          static_assert(std::is_same<FCPS, SCPS>::value, "Oopsie!");

          // Its const, so no need to bother extracting it :/
          auto first_end = first_primitives.end();
          for (auto first_primitive_it = first_primitives.begin();
               first_primitive_it != first_end; ++first_primitive_it) {
            for (auto second_primitive_it = first_primitive_it + 1;
                 second_primitive_it != first_end; ++second_primitive_it) {
              // Derefence the iterator which returns a pointer to the body
              // which is aga
              HashGrids<C>::collide(*first_primitive_it, *second_primitive_it,
                                    contacts);
            }
          }
        }

        template <typename C>         // Type of the configuration
        template <typename Contacts,  // Contact container type
                  typename FCPS,  // Storage of the first colliding primitive
                  typename SCPS>  // Storage of the second colliding primitive
        void HashGrids<C>::HashGrid::process_primitives(
            Contacts& contacts, FCPS& first_primitives, SCPS& second_primitives,
            tags::DifferentPrimitivesTag /*meta*/) const {
          // TODO : Rename the tags
          // NOTE : Here the static assert is only useful for determining
          // that primitives WITHIN one cell are different. However, we reuse
          // the same code to check for collisions between neighbors, where
          // its perfectly valid for the primitives to be same, but the tag
          // is a DifferentPrimitivesTag (as they belong to different
          // collections). Maybe the tag should be renamed to better express the
          // intent?

          //          static_assert(!std::is_same<FCPS, SCPS>::value,
          //          "Oopsie!");

          // Its const, so no need to bother extracting it :/
          auto first_end = first_primitives.end();
          auto second_end = second_primitives.end();

          for (auto first_primitive_it = first_primitives.begin();
               first_primitive_it != first_end; ++first_primitive_it) {
            for (auto second_primitive_it = second_primitives.begin();
                 second_primitive_it != second_end; ++second_primitive_it) {
              // Derefence the iterator which returns a pointer to the primitive
              HashGrids<C>::collide(*first_primitive_it, *second_primitive_it,
                                    contacts);
            }
          }
        }

        //**********************************************************************
        /*!\brief Processes this hash grid for colliding bodies.
         *
         * \param gridBodies Linear array of (handles to) all the bodies stored
         * in this hash grid. \param contacts Contact container for the
         * generated contacts. \return The number of bodies stored in this grid
         * - which is the number of bodies stored in \a gridBodies.
         *
         * This function generates all contacts between all rigid bodies that
         * are assigned to this grid. The contacts are added to the contact
         * container \a contacts. Moreover, a linear array that contains
         * (handles to) all bodies that are stored in this grid is returned in
         * order to being able to check these bodies against other bodies that
         * are stored in grids with larger sized cells.
         */
        template <typename C>         // Type of the configuration
        template <typename FCP,       // Type of the first colliding primitive
                  typename Contacts>  // Contact container type
        void HashGrids<C>::HashGrid::process_within_cell(
            Contacts& contacts) const {
          // Extract a tmpl list of all primitives colliding with the
          // current primitive in the half matrix This operation is
          // beneficial from a memory cache perspective as we process all
          // these other primitives in one go without iterating over the
          // first primitives list again However due to heterogenous
          // instructions, ILP may suffer.
          using SecondCollidingPrimitives =
              tmpl::fold<LowerTriangularPrimitivePrimitiveMatrix, tmpl::list<>,
                         typename tt::ExtractSecondPrimitive<FCP>::
                             template apply<tmpl::_state, tmpl::_element>>;
          constexpr auto n_second_colliding_primitives =
              tmpl::size<SecondCollidingPrimitives>::value;
          // END META

          // Only get the occupied cells, so the first_primitives below is
          // guaranteed to be not null
          auto const& occupied_cells = get_cells_occupied_by<FCP>();

          // Iterate through all cells that are occupied by bodies (=>
          // 'occupiedCells_') and ...
          auto const cend = occupied_cells.cend();
          auto cell_it = occupied_cells.cbegin();

          // Expand here, is this inefficient?
          // ... perform pairwise collision checks within each of these cells.
          auto intra_cell_collider = [this, &cell_it, &contacts](auto Is) {
            using SCP = tmpl::at_c<SecondCollidingPrimitives, Is>;
            using Dispatcher = tt::PrimitivesDispatchType<FCP, SCP>;

            auto* first_primitives = (*cell_it)->template get_handle_of<FCP>();
            ELASTICA_ASSERT(first_primitives != nullptr, "Oopsie!");

            auto* second_primitives = (*cell_it)->template get_handle_of<SCP>();

            // There might not be any second primitive belonging to the same
            // cell
            if (second_primitives != nullptr) {
              this->process_primitives(contacts, *first_primitives,
                                       *second_primitives, Dispatcher{});
            }
          };

          for (; cell_it != cend; ++cell_it) {
            index_apply<n_second_colliding_primitives>(
                [&intra_cell_collider](auto... Is) {
                  EXPAND_PACK_LEFT_TO_RIGHT(intra_cell_collider(Is));
                });
          }
        }

        template <typename C>         // Type of the configuration
        template <typename FCP,       // Type of the first colliding primitive
                  typename Contacts>  // Contact container type
        void HashGrids<C>::HashGrid::process_neighboring_cells(
            Contacts& contacts) const {
          // TODO : Which version is more performant?
          // chaning the order of the loops?
          // Currently we have
          // For each primitive
          //  iterate over contanining cell
          //    For each colliding primitives
          //      iterate over neighbors
          //        iterate over primitive's collection
          //          iterate over neighbors' colliding primitive collection

          // Note : here we should do processing for all primitives
          // and not only the lower half of the primitive matrix
          using SecondCollidingPrimitives =
              tmpl::fold<PrimitivePrimitiveMatrix, tmpl::list<>,
                         typename tt::ExtractSecondPrimitive<FCP>::
                             template apply<tmpl::_state, tmpl::_element>>;
          constexpr auto n_second_colliding_primitives =
              tmpl::size<SecondCollidingPrimitives>::value;

          auto const& occupied_cells = get_cells_occupied_by<FCP>();

          // Iterate through all cells that are occupied by bodies (=>
          // 'occupiedCells_') and ...
          auto const cend = occupied_cells.cend();
          auto cell_it = occupied_cells.cbegin();

          // Expand here, is this inefficient?
          // ... perform pairwise collision checks within each of these cells.
          auto inter_cell_collider = [this, &cell_it, &contacts](auto Is) {
            using SCP = tmpl::at_c<SecondCollidingPrimitives, Is>;
            // Although one can have FCP = SCP, these come from different
            // collections (one from this cell, one from the neighbor), hence
            // we always consider them different primitives and iterate over
            // the full collection
            using Dispatcher = tags::DifferentPrimitivesTag;

            auto* first_primitives = (*cell_it)->template get_handle_of<FCP>();
            ELASTICA_ASSERT(first_primitives != nullptr, "Oopsie!");

            // Moreover, check all the bodies that are stored in the currently
            // processed cell against all
            // bodies that are stored in the first half of all directly adjacent
            // cells.
            for (unsigned int i = 0; i < half_number_of_neighbors(); ++i) {
              Cell* nbCell = (*cell_it) + (*cell_it)->neighborOffset_[i];
              auto* second_primitives =
                  (*nbCell)->template get_handle_of<SCP>();

              // the neighboring cells may not have any primitive
              if (second_primitives != nullptr) {
                process_primitives(contacts, *first_primitives,
                                   *second_primitives, Dispatcher{});
              }
            }
          };

          for (; cell_it != cend; ++cell_it) {
            index_apply<n_second_colliding_primitives>(
                [&inter_cell_collider](auto... Is) {
                  EXPAND_PACK_LEFT_TO_RIGHT(inter_cell_collider(Is));
                });
          }
        }

        template <typename C>         // Type of the configuration
        template <typename FCP,       // Type of the first colliding primitive
                  typename Contacts>  // Contact container type
        void HashGrids<C>::HashGrid::process_neighboring_cells_version_two(
            Contacts& contacts) const {
          //  For each primitive
          //  iterate over cell
          //    iterate over neighbors
          //        For each colliding primitives
          //          iterate over primitive's collection
          //            iterate over neighbors' colliding primitive collection

          // Note : here we should do processing for all primitives
          // and not only the lower half of the primitive matrix

          using SecondCollidingPrimitives = AdmissiblePrimitives;
          //          using SecondCollidingPrimitives =
          //              tmpl::fold<PrimitivePrimitiveMatrix, tmpl::list<>,
          //                         typename tt::ExtractSecondPrimitive<FCP>::
          //                             template apply<tmpl::_state,
          //                             tmpl::_element>>;
          constexpr auto n_second_colliding_primitives =
              tmpl::size<SecondCollidingPrimitives>::value;

          auto const& occupied_cells = get_cells_occupied_by<FCP>();

          // Iterate through all cells that are occupied by bodies (=>
          // 'occupiedCells_') and ...
          auto const cend = occupied_cells.cend();
          auto cell_it = occupied_cells.cbegin();

          // The value from get_handle is a reference to the original pointer to
          // vector of primitives so that changes are reflected there
          using PointerToPrimitiveStorage = std::remove_reference_t<decltype(
              (*cell_it)->template get_handle_of<FCP>())>;

          // Easy to check with a nullptr
          PointerToPrimitiveStorage first_primitives = nullptr;
          Cell* nbCell = nullptr;

          auto inter_cell_collider = [this, &first_primitives, &nbCell,
                                      &contacts](auto Is) {
            using SCP = tmpl::at_c<SecondCollidingPrimitives, Is>;
            // Although one can have FCP = SCP, these come from different
            // collections (one from this cell, one from the neighbor), hence
            // we always consider them different primitives and iterate over
            // the full collection
            using Dispatcher = tags::DifferentPrimitivesTag;

            auto* second_primitives = nbCell->template get_handle_of<SCP>();

            // the neighboring cells may not have any primitive
            if (second_primitives != nullptr) {
              this->process_primitives(contacts, *first_primitives,
                                       *second_primitives, Dispatcher{});
            }
          };

          for (; cell_it != cend; ++cell_it) {
            first_primitives = (*cell_it)->template get_handle_of<FCP>();
            ELASTICA_ASSERT(first_primitives != nullptr, "Oopsie!");

            // Check all the bodies that are stored in the currently
            // processed cell against all
            // bodies that are stored in the first half of all directly adjacent
            // cells.
            for (unsigned int i = 0; i < half_number_of_neighbors(); ++i) {
              nbCell = (*cell_it) + (*cell_it)->neighborOffset_[i];

              index_apply<n_second_colliding_primitives>(
                  [&inter_cell_collider](auto... Is) {
                    EXPAND_PACK_LEFT_TO_RIGHT(inter_cell_collider(Is));
                  });
            }  // end neighbor iteration
          }    // end cell iteration
        }

        template <typename C>         // Type of the configuration
        template <typename Contacts>  // Contact container type
        void HashGrids<C>::HashGrid::process_grid(Contacts& contacts) const {
          // Wraps process_within_cell and process_with_neighbors
          // TODO : Most of the index apply calls should really be apply calls
          // TODO : It may be more beneficial to have a loop here rather than
          // pushing it to the inner calls? Such as:
          //
          // For all primitives
          // Loop over cells containing said primitive
          //    Delegated : Process within this call
          //    Delegated : Process with neighboring cells
          // instead of looping in each delegated call

          // First process within cells for each primitive
          index_apply<get_number_of_primitives()>([this,
                                                   &contacts](auto... Is) {
            EXPAND_PACK_LEFT_TO_RIGHT(
                this->process_within_cell<tmpl::at_c<AdmissiblePrimitives, Is>>(
                    contacts));
          });

          // Then do neighbors
          index_apply<get_number_of_primitives()>(
              [this, &contacts](auto... Is) {
                EXPAND_PACK_LEFT_TO_RIGHT(
                    this->process_neighboring_cells_version_two<
                        tmpl::at_c<AdmissiblePrimitives, Is>>(contacts));
              });

          // At this point all operations within the grid are performed
        }

        template <typename C>              // Type of the configuration
        template <typename PrimitiveType>  // Type of the primitive
        std::size_t HashGrids<C>::HashGrid::transfer_primitives(
            to_t /* meta */, PrimitiveType** output_primitives_buffer) const {
          // Get pointers to primitives of a certain type residing in one
          // hash grid, to detect contacts with hash grids higher
          // up the chain
          // The space for output_primitives_buffer is supposed to be already
          // allocated from the caller scope, which takes care of memory
          // management

          //          PrimitiveType** out_primitives_it =
          //          *output_primitives_buffer;

          // vector of cell pointers
          auto const& occupied_cells = get_cells_occupied_by<PrimitiveType>();

          auto const cend = occupied_cells.cend();

          std::size_t count = 0UL;

          for (auto cell_it = occupied_cells.cbegin(); cell_it != cend;
               ++cell_it) {
            // Get pointer to vector of pointers to primitives
            auto const* primitives =
                (*cell_it)->template get_handle_of<PrimitiveType>();
            ELASTICA_ASSERT(primitives != nullptr, "Oopsie!");

            // Its const, so no need to bother extracting it :/
            auto primitives_end = primitives->cend();

            for (auto primitive_it = primitives->cbegin();
                 primitive_it != primitives_end; ++primitive_it, ++count) {
              // Pointer copy
              output_primitives_buffer[count] = (*primitive_it);
            }
            //            std::copy(primitives->cbegin(), primitives->cend(),
            //            output_primitives_buffer);
          }

          return count;
        }

        // TODO : Not very type-safe and RAII. Can allocate a vector each and
        // every time, but it might be a performance hit, but simplifies the
        // code
        template <typename C>         // Type of the configuration
        template <typename FCP,       // Type of the first colliding primitive
                  typename Contacts>  // Contact container type
        void HashGrids<C>::HashGrid::process_primitives_from_finer_grid(
            FCP** primitives_from_finer_grid,
            std::size_t n_primitives_from_finer_grid,
            Contacts& contacts) const {
          /* Order of processing
           * Implemented : Data-friendly (?!) version, process each neighbor
           * cell for all primitive types before moving on
           *
           * For each primitive type from finer grid
           *  Iterate over primitives
           *   Calculate cell in which the primitives
           *    Iterate over this cell's neighbors
           *        For each colliding primitives
           *            iterate over neighbors' colliding primitive collection
           *
           *
           *  Instruction friendly version : process each neighbor cell only
           *  for a particular primitive type. Then repeat iterations over
           *  neighbors for every primitive type. Seems wasteful at first
           * glance.
           */

          // Note : here we should do processing for all primitives
          // and not only the lower half of the primitive matrix as
          // they come from different collections
          using SecondCollidingPrimitives = AdmissiblePrimitives;
          constexpr auto n_second_colliding_primitives =
              tmpl::size<SecondCollidingPrimitives>::value;
          using PrimitiveID = FCP*;

          PrimitiveID* first_primitive_it = primitives_from_finer_grid;
          Cell* nbCell = nullptr;

          auto coarse_grid_collider = [&first_primitive_it, &nbCell,
                                       &contacts](auto Is) {
            using SCP = tmpl::at_c<SecondCollidingPrimitives, Is>;

            auto* second_primitives = nbCell->template get_handle_of<SCP>();

            // the neighboring cells may not have any primitive
            if (second_primitives != nullptr) {
              for (auto second_primitive_it = second_primitives->begin();
                   second_primitive_it != second_primitives->end();
                   ++second_primitive_it) {
                HashGrids<C>::collide(*first_primitive_it, *second_primitive_it,
                                      contacts);
              }
            };
          };

          // For each primitive 'p' that is stored in
          // 'primitives_from_finer_grid' ...
          PrimitiveID* end_primitives =
              (primitives_from_finer_grid + n_primitives_from_finer_grid);

          for (; first_primitive_it < end_primitives; ++first_primitive_it) {
            // ... calculate the body's cell association (=> "hash()")
            // within this hash grid and ...
            Cell* cell = cell_ + hash(*first_primitive_it);

            // ... check 'p' against every primitive that is stored in
            // this or in any of the directly adjacent cells. Note: one
            // entry in the offset array of a cell is always referring
            // back to the cell itself. As a consequence, a specific cell
            // X and all of its neighbors can be addressed by simply
            // iterating through all entries of X's offset array!
            for (unsigned int i = 0; i < number_of_neighbors(); ++i) {
              nbCell = cell + cell->neighborOffset_[i];

              index_apply<n_second_colliding_primitives>(
                  [&coarse_grid_collider](auto... Is) {
                    EXPAND_PACK_LEFT_TO_RIGHT(coarse_grid_collider(Is));
                  });
            }
          }
        }

        //======================================================================
        //
        //  CONSTRUCTOR
        //
        //======================================================================
        //**********************************************************************
        /*!\brief Constructor for the HashGrids class.
         *
         * \param bodystorage Reference to the general body storage.
         *
         * Note that all (local, global and remote) must be contained in the
         * body storage.
         */
        template <typename C>  // Type of the configuration
        HashGrids<C>::HashGrids(
            PrimitiveStorage& primitivestorage,
            ImposedPrimitiveStorage*& imposed_primitive_storage)
            : primitives_(primitivestorage),
              imposed_primitives_(imposed_primitive_storage),
              exhaustive_search(primitivestorage, imposed_primitive_storage) {
          //          using namespace
          //          elastica::collision::detection::coarse::hhg;

          // As long as the number of bodies is less-or-equal than specified
          // by 'gridActivationThreshold', no hierarchy of hash grids is
          // constructed. Instead, pairwise collision checks are performed.
          grid_active_ = false;
        }
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

        //*************************************************************************************************
        /*!\brief Checks whether two bodies are colliding or not, and if so
         * generates the related points of contact.
         *
         * \param first_primitive The first body.
         * \param second_primitive The second body.
         * \param contacts Contact container for the generated contacts.
         * \return void
         */
        template <typename C>                    // Type of the configuration
        template <typename FirstPrimitiveType,   // Type of the first primitive
                  typename SecondPrimitiveType,  // Type of the second
                                                 // primitive
                  typename Contacts>             // Contact container type
        void HashGrids<C>::collide(
            FirstPrimitiveType* const __restrict__ first_primitive,
            SecondPrimitiveType* const __restrict__ second_primitive,
            Contacts& contacts) {
          if (
              // Testing for overlapping bounding boxes, more probable
              (first_primitive->get_AABB().overlaps(
                  second_primitive->get_AABB()))
              // // All fixed primitives to be checked are pushed to imposed
              // // bodies,  hence skip checkinh
              //  Ignoring contacts between two fixed bodies
              // // && (!first_primitive->is_fixed() or
              // !second_primitive->is_fixed())
          ) {
            FineDetector::collide(*first_primitive, *second_primitive,
                                  contacts);
          }
        }
        //*************************************************************************************************

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
        template <typename C>  // Type of the configuration
        void HashGrids<C>::add_all_primitives() {
          using namespace elastica::collision::detection::coarse::hhg;

          // Get total size of primitives
          std::size_t all_primitives_count =
              get_total_primitive_count_in_simulation();

          // If greater than the activation threshold, add
          if (all_primitives_count >= hashgrid_activation_threshold()) {
            auto primitive_adder = [this](auto Is) {
              auto& primitive_collection = std::get<Is>(primitives_);
              // For each primitive in this primitive collection, add to
              // grid
              for (auto& primitive : primitive_collection) {
                // Add a pointer
                this->add(&primitive);
              }
            };

            index_apply<get_number_of_primitives()>(
                [&primitive_adder](auto... Is) {
                  EXPAND_PACK_LEFT_TO_RIGHT(primitive_adder(Is));
                });

            // TODO : The imposed primitive strategy
            //    should be determined by a policy class
            /*
            auto imposed_primitive_adder = [this](auto Is) {
              auto& imposed_primitive_collection =
                  std::get<Is>(*imposed_primitives_);
              // For each primitive in this primitive collection, add to
              // grid
              for (auto& imposed_primitive : imposed_primitive_collection) {
                // Add a pointer
                this->add(&imposed_primitive);
              }
            };

            index_apply<get_number_of_imposed_primitives()>(
                [&imposed_primitive_adder](auto... Is) {
                  EXPAND_PACK_LEFT_TO_RIGHT(imposed_primitive_adder(Is));
                });
             */

            grid_active_ = true;

            // Here allocate resources for a memory block of pointers which
            // we use in the detection step
            //            void* memory_resource_ = std::malloc(sizeof(void*) *
            //            all_primitives_count);

          }
          // else do exhaustive detection
          else {
            // pointless statement given initializaion, but leave for
            // readability
            grid_active_ = false;
          }
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Registering a rigid primitive with the coarse collision
         * detector.
         *
         * \param primitive The primitive to be registered with the coarse
         * collision detector. \return void
         *
         * This function is a requirement for all coarse collision detectors.
         * It is automatically called every time a new rigid primitive is
         * added to the simulation world.
         */
        template <typename C>              // Type of the configuration
        template <typename PrimitiveType>  // Type of the primitive
        void HashGrids<C>::add(PrimitiveType* primitive) {
          real_t primitive_size = (primitive->is_finite())
                                      ? primitive->get_AABB_size()
                                      : static_cast<real_t>(-1);

          // only finite primitives should even reach this point
          ELASTICA_ASSERT(primitive_size > 0,
                          "Logic error : Infinite primitive in a function "
                          "meant for finite types!");
          add_finite_primitives(primitive, primitive_size);

          /*
          if (primitive_size > 0) {
            add_finite_primitives(primitive, primitive_size);
          } else {
            // TODO : The imposed primitive strategy
            //    should be determined by a policy class
            // TODO : This depends on the strategy, for doing imposed
            // primitives, its better to rely on the exhaustive search
            // algorithm rather than having a separate data structure
            // here that adds to the complication
            add_nonfinite_primitives(primitive, primitive_size);
          }
          */
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

        //*************************************************************************************************
        /*!\brief Adds a primitive to the hierarchical hash grids data
         * structure.
         *
         * \param primitive The primitive that is about to be added to the
         * hierarchical hash grids data structure. \return void
         *
         * If the usage of hierarchical hash grids is activated, this function
         * is called every time a new rigid primitive is added to the data
         * structure. It may also be called during the update phase.
         */
        template <typename C>              // Type of the configuration
        template <typename PrimitiveType>  // Type of the primitive
        void HashGrids<C>::add_finite_primitives(PrimitiveType* primitive,
                                                 real_t primitive_size) {
          using namespace elastica::collision::detection::coarse::hhg;

          // Assign to a grid with suitably primitive_sized cells.
          HashGrid* grid = nullptr;

          if (grid_list_.empty()) {
            // If no hash grid yet exists in the hierarchy, an initial hash
            // grid is created based on the primitive's primitive_size.

            grid = new HashGrid(
                adjust_span(primitive_size * std::sqrt(hierarchy_factor())));
          } else {
            // Check the hierarchy for a hash grid with suitably
            // primitive_sized cells - if such a grid does not yet exist, it
            // will be created.

            real_t cell_span = 0;
            for (auto g = grid_list_.begin(); g != grid_list_.end(); ++g) {
              grid = *g;
              cell_span = grid->get_cell_span();

              if (primitive_size < cell_span) {
                cell_span /= hierarchy_factor();
                if (primitive_size < cell_span) {
                  // Primitive is less than the current span
                  // but also less than the span one below, in
                  // which case we create a new grid and add
                  // it to the list
                  while (primitive_size < cell_span)
                    cell_span /= hierarchy_factor();

                  // No need for size policy here as cell_span is already
                  // appropriately sized
                  grid = new HashGrid(cell_span * hierarchy_factor());
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
            while (primitive_size >= cell_span)
              cell_span *= hierarchy_factor();
            grid = new HashGrid(cell_span);
          }

          grid->add(primitive);
          primitive->set_grid(static_cast<void*>(grid));

          grid_list_.push_back(grid);
        }
        //*************************************************************************************************

        //*************************************************************************************************
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
            PrimitiveType* /*primitive*/, real_t /* primitive_size */) {
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

        //*************************************************************************************************
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
        template <typename C>  // Type of the configuration
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
        //*************************************************************************************************

        template <typename C>         // Type of the configuration
        template <typename Contacts>  // Contact container type
        void HashGrids<C>::find_contacts(Contacts& contacts) {
          using namespace elastica::collision::detection::coarse::hhg;
          // Serial
          LOG_DEBUG_SECTION(log) {
            log << "   Finding the contacts via the hash grids "
                   "algorithm...";
          }

          /* TODO : Parallelization of update
           *  Bottlenecks and fixes:
                1. grid->update and grid->remove methods
                  Fixes :
                  1) mutex around the cell that is currently accessed (wont be that bad performance wise)
                  2) To keep track of the cells removed from occupied cells we can have a
                  thread local buffer that reduces what cells need to be removed and
                  remove after the step (REFACTOR!)
                2. add_finite_primitives and grid->add
                  1) More problematic because grids can be added or removed
                  Fixes :
                  Invert dependency : instead of parallel over primitives and
                  adding over grids
                  0) Have an addition buffer and removal buffer of size grid each
                  where we put the primtives from each grid to be added/removed
                  1) Scan through primitives, see which grid it is to be added
                  removed, compute its corresponding hash, set its new grid and hash
                  (in the primitives itself). If a new grid is needed, put these in
                  a separate buffer
                  2) Add new grids to the list location serially, pop items from the
                  temporary buffer and process one-by-one (can be serial)
                  3) Over the grids in parallel-invoke style : go through the respective
                  addition/removal buffers and added primitive pointers
                  . Only limited parallelism here of n_grids

              Requirements:
          */

          if (grid_active_) {
            // 1. Update all moving primitive associations
            auto updater = [this](auto Is) {
              auto& curr_primitives = this->get_primitive_collection<Is>();
              for (auto& primitive : curr_primitives) {
                auto grid = static_cast<HashGrid*>(primitive.get_grid());
                ELASTICA_ASSERT(grid!=nullptr, "Oopise, not assigned!");
                if (grid != nullptr) {
                  real_t primitive_size = primitive.get_AABB_size();
                  real_t cell_span = grid->get_cell_span();

                  if (primitive_size >= cell_span or
                      primitive_size < (cell_span / hierarchy_factor())) {
                    // TODO parallel
                    grid->remove(&primitive);
                    // We know apriori that primitives_ are finite
                    add_finite_primitives(&primitive, primitive_size);

                  } else {
                    // Even if the association of the grid doesn't change
                    // the cell association within a grid might change
                    // TODO parallel
                    grid->update(&primitive);
                  }
                }
              }
            };

            index_apply<get_number_of_primitives()>([&updater](auto... Is) {
              EXPAND_PACK_LEFT_TO_RIGHT(updater(Is));
            });

            // 2. Detection step

            // Contact generation by traversing through all hash grids (which
            // are sorted in ascending order
            // with respect to the size of their cells).
            std::vector<std::size_t> total_primitives_in_grid(grid_list_.size(),
                                                              0UL);
            std::size_t i_grid(0UL);

            // First completely process all primitives within each grid
            for (HashGrid* grid : grid_list_) {
              // TODO : Parallel
              //  This can be made parallel WLO correctness and performant
              //  (provided that contacts
              //  is a concurrent vector) since the grid and cells are not modified
              //  But to be sure, we should revisit the const correctness of the
              //  code to ensure that even the pointers/objects it points to
              //  cannot be modified
              grid->process_grid(contacts);
              total_primitives_in_grid[i_grid] =
                  grid->get_total_primitive_count();
              ++i_grid;
            }

            // Next process between grids, for which we need to:
            auto grid_it = grid_list_.begin();
            auto grid_end = grid_list_.end();

            auto inter_grid_collider = [&grid_it, &grid_end,
                                        &contacts](auto Is) {
              using PrimitiveType = tmpl::at_c<AdmissiblePrimitives, Is>;

              std::size_t current_primitive_count =
                  (*grid_it)->template get_count<PrimitiveType>();

              if (current_primitive_count > 0) {
                // 1. Allocate buffer for all pointers using a memory resource
                // TODO NOTE : Since we know that the number of primitives in
                // the simulation is constant for now, we can allocate a block
                // of memory for pointers during the initialization step and
                // keep on reusing it.
                PrimitiveType** all_primitives_from_current_grid =
                    new PrimitiveType*[current_primitive_count];

                // 2. Transfer all primitives to a buffer
                std::size_t count_transferred = (*grid_it)->transfer_primitives(
                    to, all_primitives_from_current_grid);

                ELASTICA_ASSERT(count_transferred == current_primitive_count,
                                "Wrong count!");

                auto next_grid_it = std::next(grid_it);

                // 3. Test all bodies stored in 'grid_it' against bodies
                // stored in grids with larger sized cells.
                for (; next_grid_it != grid_end; ++next_grid_it) {
                  // TODO : Parallel
                  //  This can be made parallel WLO correctness and performant
                  //  (provided that contacts
                  //  is a concurrent vector) since the grid and cells are not modified
                  //  But to be sure, we should revisit the const correctness of the
                  //  code to ensure that even the pointers/objects it points to
                  //  cannot be modified
                  (*next_grid_it)
                      ->process_primitives_from_finer_grid(
                          all_primitives_from_current_grid,
                          current_primitive_count, contacts);
                }

                // 4. Delete any allocation
                delete[] all_primitives_from_current_grid;
              }
            };

            for (; grid_it != grid_end; ++grid_it) {
              index_apply<get_number_of_primitives()>(
                  [&inter_grid_collider](auto... Is) {
                    EXPAND_PACK_LEFT_TO_RIGHT(inter_grid_collider(Is));
                  });
            }

            // Pairwise test (=> contact generation) for all primitives with
            // imposed bodies, which usually do not have a finite span
            exhaustive_search.find_contacts_with_imposed_primitives(
                contacts, PrimitiveImposedPrimitiveMatrix{});

          } else {
            exhaustive_search.find_contacts(contacts);
          }

          // Serial
          LOG_DEBUG_SECTION(log) {
            if (contacts.empty())
              log << "      no contacts found!";
            else {
              for (auto const& c : contacts) {
                log << "      State of the contacts:\n";
                log << "         Contact " << c->get_ID() << " ("
                    << c->get_contact_type() << "):"
                    << " pos = " << c->get_position()
                    << " , n = " << c->get_normal()
                    << " , dist = " << c->get_distance()
                    << " , vrel = " << c->get_normal_relative_velocity()
                    << " , cor = " << c->get_restitution() << "\n";
              }
            }
          }
        }

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

        // TODO : Can be a policy class
        template <typename C>  // Type of the configuration
        real_t HashGrids<C>::adjust_span(
            real_t requested_cell_span) const noexcept {
          // TODO
          // The hash calculation, for an infinite grid, always puts
          // the object in a domain of [0, cell_count * cell_size]
          // where cell_size is determined from the inserted object

          // However, for a periodic domain, one can enforce strict
          // requirements for the cell spans in the x, y or z directions
          // (requiring that the grid fits snugly within the domain)

          // If x, y and z have different dimensions, then in an ideal
          // world we will have three different cell_spans which we use
          // in the hash calculation. So the hashed grids are not cubes
          // but cuboids. We skip this anisotropy for now, but rather
          // enforce that if someone gives a periodic domain in two or
          // three directions, their lengths mus be equal (square domain)

          // If thats the case, then the cell span is min(requested_cell_span,
          // l_x / 2**k) with L / 2**k < requested_cell_span < L / 2**(k-1)
          // ie. k = ceil(log2(L / requested_cell_span))
          // return L / 2**k
          // This is because every object is assigned to one single grid in
          // the hierarchy only, which happens to be the first grid with a
          // cell size larger than the size of the object.

          return requested_cell_span;
        }

      }  // namespace coarse

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica
