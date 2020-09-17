#pragma once

#include <tuple>

/// Forward declarations
#include "Systems/CosseratRods/Types.hpp"
///

#include "DataStructures/TaggedTuple.hpp"
#include "ErrorHandling/Assert.hpp"
#include "Systems/Block/Block.hpp"
#include "Systems/CosseratRods/BlockInitializer.hpp"
#include "Systems/CosseratRods/BlockSlice.hpp"
#include "Systems/CosseratRods/CosseratRodPlugin.hpp"
#include "Systems/CosseratRods/TypeTraits/BlockInitializer.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/Requires.hpp"
#include "Utilities/StdHelpers.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TMPLDebugging.hpp"

namespace elastica {

  //    template <typename CRT,                 // Cosserat Rod Traits
  //              template <typename> ComputationalBlock,  // Block with
  //              implementation
  //            template <typename /*CRT*/, typename /* CosseratRodPlugin */>
  //            class... Layers>  // Skills
  //  class CosseratRodPlugin;

  // template <typename> Block

  //  template <template <typename /*Traits*/, typename /* Block */,
  //                      template <typename /*Traits*/, typename /* Block */>
  //                      class... /* Skills*/>
  //            class Plugin>
  //  class Block;

  // Specialization of blocks for the cosserat rods class
  // TODO : Blocking strategy :
  //  1. Default : free blocks
  //  2. fixed size blocks (compile time-parameter)

  //  template <template <typename, typename...> class TT, typename SENDER,
  //            typename... ARGS>
  //  class CRTPBase<TT<SENDER, ARGS...>> {
  //    // ...
  //  };

  // Specialization for a Cosserat Rod plugin
  //  template <template <typename /*Traits*/, typename /* Block */,
  //                      template <typename /*Traits*/, typename /* Block */>
  //                      class... /* Skills*/>
  //            class CRPlugin,
  //            typename CRT, template <typename, typename> class... Skills>
  //  class Block<CRPlugin<CRT, Block>>;

  template <typename CRT,                  // Cosserat Rod Traits
            template <typename, typename>  // Skills
            class... Layers>
  class Block<CosseratRodPlugin<CRT, Block, Layers...>>
      : public CosseratRodPlugin<CRT, Block, Layers...>, private NonCopyable {
    // Friend it to prevent annoying private member messages
    //    template <typename B>
    //    friend class CosseratRodPlugin;

    using Parent = CosseratRodPlugin<CRT, Block, Layers...>;
    friend Parent;
    using This = Block<Parent>;
#define REPEAT_2(M, N) M(N) M(N + 1)
#define REPEAT_4(M, N) REPEAT_2(M, N) REPEAT_2(M, N + 2)
#define REPEAT_8(M, N) REPEAT_4(M, N) REPEAT_4(M, N + 4)
#define FRIEND(N)                                                       \
  friend std::tuple_element_t<std::min(static_cast<std::size_t>(N + 1), \
                                       sizeof...(Layers)),              \
                              std::tuple<void, Layers<CRT, This>...>>;

    // Friend also all the layers called in parents
    REPEAT_8(FRIEND, 0)

#undef FRIEND
#undef REPEAT_8
#undef REPEAT_4
#undef REPEAT_2

    template <typename... Args>
    using InitializerType = CosseratBlockInitializer<Args...>;

    // Import staggering tags
    using typename Parent::ElementTag;
    using typename Parent::NodeTag;
    using typename Parent::VoronoiTag;

    //
    using typename Parent::ElementMemberTags;
    using typename Parent::MemberTags;
    using typename Parent::NodeMemberTags;
    using typename Parent::VoronoiMemberTags;

    //
    using typename Parent::SliceTags;
    template <typename T>
    using GetSliceType = typename Parent::template GetSliceType<T>;
    using Slices = tmpl::as_tagged_tuple<SliceTags>;

    using CosseratRodProperties = tmpl::as_tagged_tuple<MemberTags>;
    using BlockSlice = BlockSlice<Parent>;

    // No need scalable allocator
    using SizeType = std::size_t;
    using TotalElementCount = SizeType;
    using GhostNodeIndices = std::vector<SizeType>;
    using ElementCounts = std::vector<SizeType>;

    // Import from parent
    using Parent::n_elements;
    using Parent::n_nodes;
    using Parent::n_voronois;

    /* Really needed?
    static constexpr std::size_t get_number_of_node_members() {
      return tmpl::size<NodeMemberTags>::value;
    }

    static constexpr std::size_t get_number_of_element_members() {
      return tmpl::size<ElementMemberTags>::value;
    }

    static constexpr std::size_t get_number_of_voronoi_members() {
      return tmpl::size<VoronoiMemberTags>::value;
    }
    */

    // InitializerPacks are pack objects (a tuple) each having a bunch of
    // generator expressions to initialize a single cosserat rod
    //    template <typename... InitializerPacks>
    //    Block(ElementCounts&& elements,
    //          std::tuple<InitializerPacks...>&& initializer_packs)
    //        : elements_(std::move(elements)){};

   private:
    // Returns number of units stored in the block
    inline auto size() const noexcept -> SizeType {
      return element_counts_.size();
    }

    // We can have a ghost policy here, but its unnecessary as only
    // one feasible implementation exists
    static inline constexpr auto n_ghosts(ElementTag /*meta*/) noexcept
        -> SizeType {
      return 2UL;
    };

    static inline constexpr auto n_ghosts(NodeTag /* meta */) noexcept
        -> SizeType {
      return n_ghosts(ElementTag{}) - 1UL;
    };

    static inline constexpr auto n_ghosts(VoronoiTag /* meta */) noexcept
        -> SizeType {
      return n_ghosts(ElementTag{}) + 1UL;
    };

    inline constexpr auto data() noexcept -> CosseratRodProperties& {
      return rod_properties_;
    };

    inline constexpr auto data() const noexcept
        -> CosseratRodProperties const& {
      return rod_properties_;
    };

    // TODO : Unused
    // metafunction behaves as associative array of TagTypes and Members

    /* NOTE: An explicit specialization of structs inside class scope
     does not work with gcc-14 as its not standard C++-14 (but clang has no
     issues due to some retroactive fix). Even specializing thes structs
     outside of the class scope does not seem to help.

    template <typename T>
    struct GetMembersMarkedWithHelper;

    template <>
    struct GetMembersMarkedWithHelper<NodeTag> {
      using type = This::NodeMemberTags;
    };

    template <>
    struct GetMembersMarkedWithHelper<ElementTag> {
      using type = This::ElementMemberTags;
    };

    template <>
    struct GetMembersMarkedWithHelper<VoronoiTag> {
      using type = This::VoronoiMemberTags;
    };

    template <typename StaggeredTag>
    using get_members_marked_with =
        typename GetMembersMarkedWithHelper<StaggeredTag>::type;
    */

    // To work past that im using these undefined functions
    static constexpr This::VoronoiMemberTags GetMembersMarkedWithHelper(
        VoronoiTag);
    static constexpr This::NodeMemberTags GetMembersMarkedWithHelper(NodeTag);
    static constexpr This::ElementMemberTags GetMembersMarkedWithHelper(
        ElementTag);

    template <typename StaggeredTag>
    using get_members_marked_with =
        decltype(GetMembersMarkedWithHelper(std::declval<StaggeredTag>()));

    template <typename StaggeredTag>
    void resize_members_marked_with() {
      using StaggeredMemberTags = get_members_marked_with<StaggeredTag>;
      tmpl::for_each<StaggeredMemberTags>(
          [this, resized = Parent::template get_dofs<StaggeredTag>()](
              auto staggered_member_tag) {
            using StaggeredMemberTag =
                tmpl::type_from<decltype(staggered_member_tag)>;
            auto& staggered_member =
                std::get<StaggeredMemberTag>(rod_properties_);
            StaggeredMemberTag::resize(staggered_member, resized);
          });
    }

    template <typename StaggeredTag>
    void initialize_ghosts_marked_with() {
      using StaggeredMemberTags = get_members_marked_with<StaggeredTag>;
      tmpl::for_each<StaggeredMemberTags>([this,
                                           deficit = n_ghosts(StaggeredTag{})](
                                              auto staggered_member_tag) {
        using StaggeredMemberTag =
            tmpl::type_from<decltype(staggered_member_tag)>;
        auto& staggered_member = std::get<StaggeredMemberTag>(rod_properties_);
        const auto ghost_value = StaggeredMemberTag::ghost_value();

        // Here we initialize ALL ghosts, whether or not the
        // elements are preserved before and after reordering
        // TODO : Policy class
        // Resize dofs irrespective of the type
        for (SizeType ghost_node_idx : ghost_node_indices_) {
          SizeType ghost_stop_idx = ghost_node_idx;
          ELASTICA_ASSERT(ghost_stop_idx > deficit,
                          "Ghost invariant violated!");
          SizeType ghost_start_idx = ghost_stop_idx - deficit;

          for (SizeType ghost_idx = ghost_stop_idx; ghost_idx > ghost_start_idx;
               --ghost_idx) {
            StaggeredMemberTag::slice(staggered_member, ghost_idx) =
                ghost_value;
          }
        }
      });
    }

    template <typename StaggeredTag>
    void make_slices_of_members_marked_with_deprecated(Slices& slices) {
      // Always one behind is where the ghost starts according to the indexing
      const SizeType start_slice = ghost_node_indices_.empty()
                                       ? 0UL
                                       : (ghost_node_indices_.back() + 1UL);

      ELASTICA_ASSERT(not element_counts_.empty(),
                      "No elements to make slices for in the Block!");

      const SizeType size_slice =
          StaggeredTag::get_dofs(element_counts_.back());

      using StaggeredMemberTags = get_members_marked_with<StaggeredTag>;

      tmpl::for_each<StaggeredMemberTags>(
          [this, &slices, start_slice, size_slice](auto staggered_member_tag) {
            using StaggeredMemberTag =
                tmpl::type_from<decltype(staggered_member_tag)>;
            using StaggeredMemberSliceTag = GetSliceType<StaggeredMemberTag>;

            // Unsliced
            auto& full = std::get<StaggeredMemberTag>(rod_properties_);
            auto& sliced = std::get<StaggeredMemberSliceTag>(slices);

            // Sliced for initialization : from ghost_idx + 1 to last element
            sliced = StaggeredMemberTag::slice(full, start_slice, size_slice);
          });
    }

    // Zero-based indexing
    // This template is not needed, but kept for symmetry with size_of_slice
    template <typename StaggeredTag>
    inline auto get_start_of_slice(SizeType const unit) const noexcept
        -> std::size_t {
      ELASTICA_ASSERT(ghost_node_indices_.size() >= unit,
                      "Wrong number of units requested");
      // unit - 1 cannot be negative
      return unit ? (ghost_node_indices_[unit - 1UL] + 1UL) : 0UL;
    }

    // Zero-based indexing
    template <typename StaggeredTag>
    inline auto get_size_of_slice(SizeType const unit) const noexcept
        -> std::size_t {
      ELASTICA_ASSERT(ghost_node_indices_.size() >= unit,
                      "Wrong number of units requested");
      // unit - 1 cannot be negative
      return StaggeredTag::get_dofs(element_counts_[unit]);
    }

    template <typename StaggeredTag>
    constexpr auto make_slices_of_members_marked_with(const SizeType unit) {
      // Always one behind is where the ghost starts according to the indexing
      const SizeType start_slice = get_start_of_slice<StaggeredTag>(unit);
      const SizeType size_slice = get_size_of_slice<StaggeredTag>(unit);
      // StaggeredTag::get_dofs(elements_.back());

      using StaggeredMemberTags = get_members_marked_with<StaggeredTag>;

      auto applier = [this, start_slice, size_slice](auto Is) {
        using StaggeredMemberTag = tmpl::at_c<StaggeredMemberTags, Is>;
        using StaggeredMemberSliceTag = GetSliceType<StaggeredMemberTag>;

        // Unsliced
        auto& full = std::get<StaggeredMemberTag>(rod_properties_);
        // Sliced for initialization : from ghost_idx + 1 to last
        // element
        return StaggeredMemberSliceTag{
            StaggeredMemberTag::slice(full, start_slice, size_slice)};
      };

      return index_apply<tmpl::size<StaggeredMemberTags>::value>(
          [&applier](auto... Is) { return std::make_tuple(applier(Is)...); });
    }

    //    void resize(NodeTag /* meta */) {
    //      tmpl::for_each<NodeMemberTags>(
    //          [this, resized = n_nodes()](auto node_member_tag) {
    //            using NodeMemberTag =
    //            tmpl::type_from<decltype(node_member_tag)>; auto& node_member
    //            = std::get<NodeMemberTag>(rod_properties_);
    //            // Resize dofs irrespective of the type
    //            NodeMemberTag::resize(node_member, resized);
    //          });
    //    }
    //
    //    void resize(ElementTag /* meta */) {
    //      tmpl::for_each<ElementMemberTags>([this, resized = n_elements()](
    //                                            auto element_member_tag) {
    //        using ElementMemberTag =
    //        tmpl::type_from<decltype(element_member_tag)>; auto&
    //        element_member = std::get<ElementMemberTag>(rod_properties_);
    //        // Resize dofs irrespective of the type
    //        ElementMemberTag::resize(element_member, resized);
    //      });
    //    }
    //
    //    void resize(VoronoiTag /* meta */) {
    //      tmpl::for_each<VoronoiMemberTags>([this, resized = n_voronois()](
    //                                            auto voronoi_member_tag) {
    //        using VoronoiMemberTag =
    //        tmpl::type_from<decltype(voronoi_member_tag)>; auto&
    //        voronoi_member = std::get<VoronoiMemberTag>(rod_properties_);
    //        // Resize dofs irrespective of the type
    //        VoronoiMemberTag::resize(voronoi_member, resized);
    //      });
    //    }

    void resize() {
      resize_members_marked_with<NodeTag>();
      resize_members_marked_with<ElementTag>();
      resize_members_marked_with<VoronoiTag>();
    }

    void initialize_ghosts() {
      initialize_ghosts_marked_with<NodeTag>();
      initialize_ghosts_marked_with<ElementTag>();
      initialize_ghosts_marked_with<VoronoiTag>();
    }

    /*
    Slices make_slices_deprecated() {
      // IMPOSSIBLE : Submatrices and subvectors disallow default
      // construction, hence do in a round about way
      Slices slices{};
      make_slices_of_members_marked_with_deprecated<NodeTag>(slices);
      make_slices_of_members_marked_with_deprecated<ElementTag>(slices);
      make_slices_of_members_marked_with_deprecated<VoronoiTag>(slices);
      return slices;
    }
     */

    auto make_slices(SizeType const unit) {
      ELASTICA_ASSERT(not element_counts_.empty(),
                      "No elements to make slices for in the Block!");
//      TypeDisplayer<decltype(make_slices_of_members_marked_with<ElementTag>(unit))> td{};
      return std::tuple_cat(
          make_slices_of_members_marked_with<NodeTag>(unit),
          make_slices_of_members_marked_with<ElementTag>(unit),
          make_slices_of_members_marked_with<VoronoiTag>(unit));
    }

    // To resize
    void prepare_for_resize(std::size_t n_elems) {
      ELASTICA_ASSERT(n_elems > 1UL, "Cannot have rods with < 2 elements!");
      const auto n_units = size();

      // A. increase units count
      element_counts_.push_back(n_elems);

      // B. resize dofs
      if (n_units) {
        // pre-existing block, so

        // 1. push back a new ghost node (based on zero-indexing)
        ghost_node_indices_.push_back(n_nodes());

        // 2. calculate the new number of elements including ghosts
        n_elements_ += (n_elems + n_ghosts(ElementTag{}));

      } else {
        // Fresh block, so
        ELASTICA_ASSERT(ghost_node_indices_.empty(),
                        "Block assert failed, contact developers");
        // ghost_node_indices_.push_back(void) => No ghosts in this case

        // 1. Increment total number of elements
        n_elements_ += n_elems;
      }
    }

    // NOTE : The public interface should more or less come from the
    //  Cosserat Rod plugin class per design
   public:
    Block()
        : rod_properties_(),
          ghost_node_indices_(),
          element_counts_(),
          n_elements_(0UL){};

    //// Deleted by construction
    // Block(const Block& other) = delete;
    ////

    Block(Block&& other) noexcept
        :  // TODO : improve Tagged tuple move, for example what does move do?
          rod_properties_(std::move(other.rod_properties_)),
          ghost_node_indices_(std::move(other.element_counts_)),
          element_counts_(std::move(other.element_counts_)),
          n_elements_(other.n_elements_){};

    // TODO : Check if all are valid blocks
    template <typename... BlockInitializers,
              Requires<cpp17::conjunction_v<
                  tt::IsBlockInitializer<BlockInitializers>...>> = nullptr>
    auto emplace_back(BlockInitializers&&... block_initializers) -> BlockSlice {
      // 0. number of preexisting rods
      const auto n_units = size();
      constexpr auto n_units_added = sizeof...(BlockInitializers);

      // 1. Increase units count, linear
      EXPAND_PACK_LEFT_TO_RIGHT(prepare_for_resize(block_initializers.n_elems));

      // 2. Bulk resize
      resize();

      // 3. Bulk fill all ghosts
      initialize_ghosts();

      auto initializer = [n_units, this,
                          cap = std::forward_as_tuple(block_initializers...)](
                             auto Is) {
        auto&& block_initializer(std::get<Is>(cap));

        const SizeType unit = n_units + Is;

        // 4. Make slices for each rod
        auto slices(make_slices(unit));

        ELASTICA_ASSERT(element_counts_[unit] == (block_initializer.n_elems),
                        "Invariant violation detected, contact the developers");

        //        Parent::print(std::forward<decltype(block_initializer.initializers)>(
        //                block_initializer.initializers));

        // ... and initialize them ...
        Parent::initialize(block_initializer.n_elems, slices,
                           std::move(block_initializer.initializers));
      };

      // by going through the rods that are just added
      index_apply<n_units_added>([&initializer](auto... Is) {
        EXPAND_PACK_LEFT_TO_RIGHT(initializer(Is));
      });

      // return BlockSlice(slices);
    }

    // TODO : Check for initializer requirements here
    template <typename... UnitInitializers>
    auto emplace_back(std::size_t n_elems,
                      UnitInitializers&&... unit_initializers) {
      /*
      // A.
      prepare_for_resize(n_elems);

      // B. Resize memory for all the member types based on new n_elements_
      resize();

      // C. Initialize ghosts based on new n_elements_
      // no-op if ghost_node_indices has no elements
      initialize_ghosts();

      // D. Initialize new dofs for the new cosserat rod (the one just added)
      auto slices(make_slices_v2(size()));

      Parent::initialize(
          n_elems, slices,
          std::forward<std::tuple<InitializerPack...>>(initializer_pack));

      // At this point the total number of elements and the accumulate
      // of individual elements should match
       */
      return emplace_back(make_cosserat_rod(
          n_elems, std::forward<UnitInitializers>(unit_initializers)...));
    }

    /*
    template <typename IT>
    void emplace_back(IT&& block) {
      // forward to the multiple version, doesnt work
      emplace_back(std::forward<IT>(block));
    }
    */

    ~Block() = default;

   private:
    //**Member variables********************************************************
    /*!\name Member variables */
    //@{
    //!< All members of CosseratRod
    CosseratRodProperties rod_properties_;
    //    TypeDisplayer<CosseratRodProperties> td{};
    //@}
    //**************************************************************************

    //**Additional member variables*********************************************
    /*!\name Member variables */
    //@{
    /// Extra members for book-keeping
    //!< Indices of nodal ghosts across
    GhostNodeIndices ghost_node_indices_;
    //!< Number of elements within each unit
    ElementCounts element_counts_;
    //!< Total number of elements, for polymorphism as a Cosserat rod
    TotalElementCount n_elements_;
    //@}
    //**************************************************************************
  };

}  // namespace elastica