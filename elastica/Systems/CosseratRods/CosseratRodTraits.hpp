#pragma once

#include <type_traits>

///
#include "Utilities/DefineTypes.h"
///

// Vector types
#include <blaze/math/DynamicVector.h>
#include <blaze/math/StaticVector.h>
#include <blaze/math/views/Subvector.h>

// matrix types
#include <blaze/math/DynamicMatrix.h>
//#include <blaze/math/StaticMatrix.h>
#include <blaze/math/views/Column.h>
#include <blaze/math/views/Submatrix.h>

// Tensor types
#include <blaze_tensor/math/DynamicTensor.h>
#include <blaze_tensor/math/StaticTensor.h>
#include <blaze_tensor/math/SubTensor.h>
#include <blaze_tensor/math/views/ColumnSlice.h>

// Elastica
#include "Systems/CosseratRods/Math.hpp"
#include "Systems/CosseratRods/Types.hpp"
//#include "Systems/CosseratRods/Policies/Policies.hpp"
#include "ErrorHandling/Assert.hpp"
#include "Systems/CosseratRods/TypeTraits.hpp"
#include "Utilities/NonCreatable.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits.hpp"

namespace elastica {

  namespace detail_cosserat_rod {

    // The default traits for the CosseratRod class
    // should not need much customziation, but it is possible
    // to customize if needed
    struct CosseratRodTraits : private NonCreatable {
      using NodeTag = NodeTagImpl;
      using ElementTag = ElementTagImpl;
      using VoronoiTag = VoronoiTagImpl;

      //      static constexpr std::size_t get_dimensions() noexcept { return
      //      3UL; }

      /* Initialization helpers */
      template <typename IdType, typename StaggeredTag,
                template <typename> class RankTag>
      struct RegisterComputedType : public RankTag<StaggeredTag> {
        using This = RegisterComputedType<IdType, StaggeredTag, RankTag>;
        using Parent = RankTag<StaggeredTag>;
        using SliceType = typename Parent::template Slice<This>;
      };

      template <typename IdType, typename StaggeredTag,
                template <typename> class RankTag, typename InitTag>
      struct RegisterInitializedType : public RankTag<StaggeredTag> {
        using This = RegisterComputedType<IdType, StaggeredTag, RankTag>;
        using Parent = RankTag<StaggeredTag>;
        using SliceType = typename Parent::template Slice<This>;
        using InitializerTag = InitTag;
      };

      template <typename IdType, typename TagType>
      struct TensorSlice;

      // can be in traits class
      template <typename StaggeredTag>
      struct TensorTag {
        static constexpr std::size_t get_dimensions() noexcept { return 3UL; }

        using TagType = StaggeredTag;

        //
        template <typename IdType>
        using Slice = TensorSlice<IdType, TagType>;

        // To prevent cross operations
        using data_type = blaze::DynamicTensor<real_t>;
        using slice_type = blaze::Subtensor_<data_type>;
        // A concrete slice type
        // NOTE : blaze tensor cant initialize with a matrix type
        // so the ghosts for now are tensor types which involve
        // more work than necessary
        using ghost_type = blaze::StaticTensor<real_t, get_dimensions(),
                                               get_dimensions(), 1UL>;
        // blaze::DynamicMatrix<real_t, blaze::rowMajor>;
        //        blaze::StaticMatrix<real_t, 3UL, 3UL, blaze::rowMajor>;

        // Why can't I just use get
        static inline constexpr auto get_dofs(std::size_t n_elems) {
          return TagType::get_dofs(n_elems);
        }

        // Default ghosts have this value, can be overriden
        // This should not be a real_t (implicit conversion) but
        // rather a slice_t object (like a matrix for example)
        static inline constexpr auto ghost_value() -> ghost_type {
          // cannot be constexpr as it involves allocation :/
          return ghost_type{{{1.0}, {0.0}, {0.0}}, {{0.0}, {1.0}, {0.0}},
                 {{0.0}, {0.0}, {1.0}}};
        }

        // should really be in the Slice Tag class, but it ends up being
        // confusing as what we are really doing is taking a slice of a
        // slice but its abstracted away
        //        template <typename MT>
        //        static inline constexpr decltype(auto) slice(MT& matrix,
        //                                                     std::size_t i) {

        // TODO decltype expression is same as SliceType
        // TODO This is internal function, need not be exposed
        // This overload is used in the block to generate slices to be
        // filled in by the upper hierarchy of classes
        static inline constexpr decltype(auto) slice(data_type& tensor,
                                                     std::size_t start,
                                                     std::size_t size) {
          // (page, row, column), (o, m, n)
          return blaze::subtensor(tensor, 0UL, 0UL, start, get_dimensions(),
                                  get_dimensions(), size);
        }

        // How to slice the data-type?
        // This overload is used in the upper hierarchy of classes for
        // initialization
        static inline constexpr decltype(auto) slice(slice_type& tensor_slice,
                                                     std::size_t idx) {
          return blaze::subtensor(tensor_slice, 0UL, 0UL, idx, get_dimensions(),
                                  get_dimensions(), 1UL);
          //          return blaze::columnslice(tensor_slice, idx);
        }

        // This overload is used in the block to generate slices to be
        // used when filling the ghosts for example
        static inline constexpr decltype(auto) slice(data_type& tensor,
                                                     std::size_t idx) {
          return blaze::subtensor(tensor, 0UL, 0UL, idx, get_dimensions(),
                                  get_dimensions(), 1UL);
          //          return blaze::columnslice(tensor, idx);
        }

        // How much to resize the data-type?
        static inline void resize(data_type& tensor, std::size_t new_dofs) {
          ELASTICA_ASSERT(tensor.columns() <= new_dofs,
                          "Contract violation, block shrinks");
          // (o, m, n)
          return tensor.resize(get_dimensions(), get_dimensions(), new_dofs,
                               true);
        }
      };

      template <typename IdType, typename StaggeredTag>
      struct TensorSlice {
        using TagType = StaggeredTag;
        using type = typename TensorTag<StaggeredTag>::slice_type;
        constexpr type& get() noexcept { return d_; }
        type d_;
      };

      template <typename IdType, typename TagType>
      struct MatrixSlice;

      // can be in traits class
      template <typename StaggeredTag>
      struct MatrixTag {
        static constexpr std::size_t get_dimensions() noexcept { return 3UL; }
        using TagType = StaggeredTag;

        //
        template <typename IdType>
        using Slice = MatrixSlice<IdType, TagType>;

        // To prevent cross operations
        using data_type =
            blaze::DynamicMatrix<real_t, blaze::rowMajor,
                                 blaze::AlignedAllocator<real_t> /*, TagType*/>;
        using slice_type = blaze::Submatrix_<data_type>;
        // A concrete slice type
        using ghost_type =
            blaze::StaticVector<real_t, 3UL, blaze::columnVector>;

        // Why can't I just use get
        static inline constexpr auto get_dofs(std::size_t n_elems) {
          return TagType::get_dofs(n_elems);
        }

        // Default ghosts have this value, can be overriden
        static inline constexpr auto ghost_value() -> ghost_type {
          return {0.0, 0.0, 0.0};
        }

        // should really be in the Slice Tag class, but it ends up being
        // confusing as what we are really doing is taking a slice of a
        // slice but its abstracted away
        //        template <typename MT>
        //        static inline constexpr decltype(auto) slice(MT& matrix,
        //                                                     std::size_t i) {

        // TODO decltype expression is same as SliceType
        // TODO This is internal function, need not be exposed
        // This overload is used in the block to generate slices to be
        // filled in by the upper hierarchy of classes
        static inline constexpr decltype(auto) slice(data_type& matrix,
                                                     std::size_t start,
                                                     std::size_t size) {
          return blaze::submatrix(matrix, 0UL, start, get_dimensions(), size);
        }

        // How to slice the data-type?
        // This overload is used in the upper hierarchy of classes
        static inline constexpr decltype(auto) slice(slice_type& slice,
                                                     std::size_t idx) {
          return blaze::column(slice, idx);
        }

        // This overload is used in the block to generate slices to be
        // used when filling the ghosts for example
        static inline constexpr decltype(auto) slice(data_type& matrix,
                                                     std::size_t idx) {
          return blaze::column(matrix, idx);
        }

        // How much to resize the data-type?
        static inline void resize(data_type& matrix, std::size_t new_dofs) {
          ELASTICA_ASSERT(matrix.columns() <= new_dofs,
                          "Contract violation, block shrinks");
          return matrix.resize(get_dimensions(), new_dofs, true);
        }
      };

      // Useful aliases
      using NodeMatrix = MatrixTag<NodeTag>;
      using VoronoiMatrix = MatrixTag<VoronoiTag>;
      using ElementMatrix = MatrixTag<ElementTag>;

      template <typename IdType, typename StaggeredTag>
      struct MatrixSlice {
        using TagType = StaggeredTag;
        using type = typename MatrixTag<StaggeredTag>::slice_type;
        constexpr type& get() noexcept { return d_; }
        type d_;
      };

      //      // Useful aliases
      //      using NodeMatrixSlice = MatrixSliceTag<NodeTag>;
      //      using VoronoiMatrixSlice = MatrixSliceTag<VoronoiTag>;
      //      using ElementMatrixSlice = MatrixSliceTag<ElementTag>;
      //
      // can be in traits class

      template <typename IdType, typename TagType>
      struct VectorSlice;

      template <typename StaggeredTag>
      struct VectorTag {
        using TagType = StaggeredTag;

        template <typename IdType>
        using Slice = VectorSlice<IdType, TagType>;

        // To prevent cross operations
        using data_type =
            blaze::DynamicVector<real_t, blaze::columnVector,
                                 blaze::AlignedAllocator<real_t> /*, TagType*/>;
        using slice_type = blaze::Subvector_<data_type>;
        // A concrete slice type
        using ghost_type = real_t;

        // Why can't I just use get
        static inline constexpr auto get_dofs(std::size_t n_elems) {
          return TagType::get_dofs(n_elems);
        }

        // Default ghosts have this value, can be overriden
        static inline constexpr auto ghost_value() -> real_t { return 0.0; }

        // TODO decltype expression is same as SliceType
        // TODO This is internal function, need not be exposed
        static inline constexpr decltype(auto) slice(data_type& vector,
                                                     std::size_t start,
                                                     std::size_t size) {
          return blaze::subvector(vector, start, size);
        }

        //        template <typename VT>
        //        static inline constexpr decltype(auto) slice(VT& vector,
        //                                                     std::size_t i) {
        static inline constexpr decltype(auto) slice(data_type& vector,
                                                     std::size_t i) {
          return vector[i];
        }

        static inline constexpr decltype(auto) slice(slice_type& slice,
                                                     std::size_t idx) {
          return slice[idx];
        }

        // How much to resize the data-type?
        static inline void resize(data_type& vector, std::size_t new_dofs) {
          ELASTICA_ASSERT(vector.size() <= new_dofs,
                          "Contract violation, block shrinks");
          return vector.resize(new_dofs, true);
        }
      };

      // Useful aliases
      using NodeVector = VectorTag<NodeTag>;
      using VoronoiVector = VectorTag<VoronoiTag>;
      using ElementVector = VectorTag<ElementTag>;

      template <typename IdType, typename StaggeredTag>
      struct VectorSlice {
        using TagType = StaggeredTag;
        using type = typename VectorTag<StaggeredTag>::slice_type;
        constexpr type& get() noexcept { return d_; }
        type d_;
      };

      /*
      template <blaze::AlignmentFlag AF,  // Alignment flag
                size_t I,          // Index of the first subvector element
                size_t N,          // Size of the subvector
                typename VT,       // Type of the vector
                bool TF,           // Transpose flag
                typename... RSAs>  // Optional subvector arguments
      inline decltype(auto) custom_subvector(ElementVector::data_type& vector,
                                             RSAs... args) {
        using ReturnType = Subvector_<VT, AF, I, N>;
        return ReturnType(*vector, args...);
      }
        */
      //*************************************************************************************************

      /*
      template <typename... RSAs>  // Optional subvector arguments
      static inline decltype(auto) custom_subvector(
          blaze::Vector<typename ElementVector::data_type,
                        blaze::columnVector> const& vector,
          std::size_t index, std::size_t size, RSAs... args) {
        using ReturnType =
            const blaze::Subvector_<const VoronoiVector::data_type,
                                    blaze::unaligned>;
        return ReturnType(*vector, index, size, args...);
      }
       */
      //*************************************************************************************************

      // TODO: Having different tag types seem painful for conversion of
      //  operators from node->element->voronoi and vice-versa. Postpone for
      //  now.

      template <typename InputTagType = ElementVector,
                typename OutputTagType = VoronoiVector>
      static inline auto calculate_voronoi_length(
          typename InputTagType::data_type const& phys) ->
          typename OutputTagType::data_type {
        // convolve with weights 0.5, 0.5
        auto n_voronoi(OutputTagType::get_dofs(phys.size()));
        return real_t(0.5) * (blaze::subvector(phys, 1UL, n_voronoi) +
                              blaze::subvector(phys, 0UL, n_voronoi));
      }

      //      // Useful aliases
      //      using NodeVectorSlice = VectorSliceTag<NodeTag>;
      //      using VoronoiVectorSlice = VectorSliceTag<VoronoiTag>;
      //      using ElementVectorSlice = VectorSliceTag<ElementTag>;
      //
      // can be in traits class
      struct ScalarTag {
        // To prevent cross operations
        using data_type = std::vector<real_t>;
        // Why can't I just use get
        static inline constexpr auto get_dofs(std::size_t /*i*/)
            -> std::size_t {
          return 1UL;
        }
      };

      struct ScalarSliceTag {
        using data_type = real_t&;

        // Why can't I just use get
        static inline constexpr auto get_dofs(std::size_t /*i*/)
            -> std::size_t {
          return 1UL;
        }
      };

      // useful typedefs
      template <typename T>
      using slice_type_t = tt::slice_type_t<T>;
//      template <typename T>
//      using get_slice_ty = typename tt::GetSliceType<T>::type;

      template <typename T>
      using initializer_tag_t = typename tt::initializer_tag_t<T>;

      template <typename T>
      using parameter_type_t = typename tt::parameter_type_t<T>;

      template <typename T>
      using IsNodeStaggered = tt::IsNodeStaggered<T>;

      template <typename T>
      using IsElementStaggered = tt::IsElementStaggered<T>;

      template <typename T>
      using IsVoronoiStaggered = tt::IsVoronoiStaggered<T>;
      //

      // CRMT :-> CosseratRodMemberTag
      // Can be free functions, but better to restrict scope here
//      template <typename CRMT, template <typename...> class Tuple,
//                typename... Slices>
//      static constexpr decltype(auto) get_slice(Tuple<Slices...>& t) noexcept {
//        return std::get<GetSliceType<CRMT>>(t).get();
//      }

      // Couple of lessons learnt here:
      /*
       * Initially the design was to use an alias template inside the tags
       * to look at the initializer type, simply like so:
       *
       *
        struct MemberTag : Traits::VoronoiVector {

        template <typename Func>
        using InitializerType = tags::ReferenceCurvatureProfile<Func>;
        }

        And then in the call site, where we are passed in a tuple of
        these (instantiated initializer types with some lambda function
        Func), one can compare whether its an instantiation of the MemberTags
        Initializer template type
        like so

        // Alias for checking whether a tpye is an instantiaion of
       InitializerType using Checker = ::tt::detail::is_a<CRMT::template
       InitializerType>;

        // The templated check struct checks for an instantiation
        using Initializer = tmpl::front<
            // Find
            tmpl::find<InitializerTuple,
                       // Does it match the template?
                       tmpl::bind<Checker::template check, tmpl::_1>>
            //                       typename Checker::template check<tmpl::_1>>
            // End find
            >;

        This approach is volatile and causes an issue if CRMT::template
        InitializerType is an alias type as documented here :

        https://gcc.gnu.org/bugzilla/show_bug.cgi?id=59498

        While there is no clear fix (and I don't know what the standard
        says about this), one  fix is to not use an alias iniside the class
        but rather a struct itself. This leads to a counter intutive design
        as we initialize the struct inside the Tag, which the initializer
       factory (for creating cosserat rods) needs to know about, which is less
       than ideal.


        The alternate route which I have gone below is to simply mark the
       MemberTag with an initializer Tag like so: struct PositionTag :
       Traits::NodeMatrix{ usign InitializerTag = tags::PositionProfileTag;
        }

        Then when make the NamedGenerator with the same Tag
        ####### stands for some demangled name which we dont care about
        template <typename Func>
        aka PositionProgile##### = NamedType<Func, PositionProfileTag>;

        Now its possible to compare MemberTag::InitialzierTag to
        Initializer::ParameterType for a match
       */

//      template <typename CRMT, template <typename...> class Tuple,
//                typename... Initializers>
//      static constexpr decltype(auto) get_initializer(
//          Tuple<Initializers...>&& t) noexcept {
//        // Here initializers can be a ref too (in the case its not an
//        // immediate lambda, hence we first remove reference here
//
//        // TODO Is there no alternative than doing an O(n) search here?
//        using InitializerTuple = Tuple<Initializers...>;
//        using find_result =
//            tmpl::find<InitializerTuple,
//                       std::is_same<tmpl::pin<GetInitializerTagType<CRMT>>,
//                                    tt::GetParameterType<
//                                        std::remove_reference<tmpl::_1>>>>;
//
//        // TODO : Better display of failed component
//        static_assert(tmpl::size<find_result>::value, "Initializer not found!");
//
//        using CRMTInitializer = tmpl::front<find_result>;
//
//        // The trailing get is to return the generator from the enclosing
//        // strong type
//        return std::get<CRMTInitializer>(std::forward<InitializerTuple>(t))
//            .get();
//      }

      /*
      using NodeVector = blaze::DynamicVector<real_t, blaze::columnVector,
                                              blaze::AlignedAllocator<real_t>,
                                              detail_cosserat_rod::NodeTag>;
      using NodeMatrix = blaze::DynamicMatrix<real_t, blaze::rowMajor,
                                              blaze::AlignedAllocator<real_t>,
                                              detail_cosserat_rod::NodeTag>;

      //
      using ElementVector = blaze::DynamicVector<real_t, blaze::columnVector,
                                                 blaze::AlignedAllocator<real_t>,
                                                 detail_cosserat_rod::ElementTag>;
      using ElementMatrix = blaze::DynamicMatrix<real_t, blaze::rowMajor,
                                                 blaze::AlignedAllocator<real_t>,
                                                 detail_cosserat_rod::ElementTag>;

      //
      using VoronoiMatrix = blaze::DynamicMatrix<real_t, blaze::rowMajor,
                                                 blaze::AlignedAllocator<real_t>,
                                                 detail_cosserat_rod::VoronoiTag>;
                                                 */
    };

  }  // namespace detail_cosserat_rod

}  // namespace elastica
