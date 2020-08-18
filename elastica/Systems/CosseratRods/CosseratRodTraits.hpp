#pragma once

///
#include "Utilities/DefineTypes.h"
///

#include <blaze/math/DynamicMatrix.h>
#include <blaze/math/DynamicVector.h>

#include "Math.hpp"
#include "Systems/CosseratRods/Math/Tags.hpp"
//#include "Systems/CosseratRods/Policies/Policies.hpp"
#include "Systems/CosseratRods/TypeTraits/detail.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits.hpp"

namespace elastica {

  namespace detail_cosserat_rod {

    // The default traits for the CosseratRod class
    // should not need much customziation, but it is possible
    // to customize if needed
    struct CosseratRodTraits {
      template <typename IdType, typename TagType>
      struct MatrixSlice;

      // can be in traits class
      template <typename TagType>
      struct MatrixTag {
        //
        template <typename IdType>
        using Slice = MatrixSlice<IdType, TagType>;

        // To prevent cross operations
        using data_type =
            blaze::DynamicMatrix<real_t, blaze::rowMajor,
                                 blaze::AlignedAllocator<real_t>, TagType>;

        // Why can't I just use get
        static inline constexpr auto get_dofs(std::size_t i) {
          return TagType::get_dofs(i);
        }

        // should really be in the Slice Tag class, but it ends up being
        // confusing as what we are really doing is taking a slice of a
        // slice but its abstracted away
        template <typename MT>
        static inline constexpr decltype(auto) slice(MT& matrix,
                                                     std::size_t i) {
          return blaze::column(matrix, i);
        }
      };

      // Useful aliases
      using NodeMatrix = MatrixTag<NodeTag>;
      using VoronoiMatrix = MatrixTag<VoronoiTag>;
      using ElementMatrix = MatrixTag<ElementTag>;

      template <typename IdType, typename TagType>
      struct MatrixSlice {
        using data_type = blaze::Columns_<typename MatrixTag<TagType>::type>;
      };

      //      // Useful aliases
      //      using NodeMatrixSlice = MatrixSliceTag<NodeTag>;
      //      using VoronoiMatrixSlice = MatrixSliceTag<VoronoiTag>;
      //      using ElementMatrixSlice = MatrixSliceTag<ElementTag>;
      //
      // can be in traits class

      template <typename IdType, typename TagType>
      struct VectorSlice;

      template <typename TagType>
      struct VectorTag {
        template <typename IdType>
        using Slice = VectorSlice<IdType, TagType>;

        // To prevent cross operations
        using data_type =
            blaze::DynamicVector<real_t, blaze::columnVector,
                                 blaze::AlignedAllocator<real_t>, TagType>;

        // Why can't I just use get
        static inline constexpr auto get_dofs(std::size_t i) {
          return TagType::get_dofs(i);
        }

        template <typename VT>
        static inline constexpr decltype(auto) slice(VT& vector,
                                                     std::size_t i) {
          return vector[i];
        }
      };

      // Useful aliases
      using NodeVector = VectorTag<NodeTag>;
      using VoronoiVector = VectorTag<VoronoiTag>;
      using ElementVector = VectorTag<ElementTag>;

      template <typename IdType, typename TagType>
      struct VectorSlice {
        using data_type = blaze::Subvector_<typename VectorTag<TagType>::type>;
      };

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

      template <typename T>
      using AddSliceType = typename tt::AddSliceType<T>::type;

      // Can be free functions, but better to restrict scope here
      template <typename CRMT, template <typename...> class Tuple,
                typename... Slices>
      static constexpr decltype(auto) get_slice(Tuple<Slices...>& t) noexcept {
        return std::get<AddSliceType<CRMT>>(t);
      }

      template <typename CRMT, template <typename...> class Tuple,
                typename... Initializers>
      static constexpr decltype(auto) get_initializer(
          Tuple<Initializers...>& t) noexcept {
        // TODO Is there no alternative than doing an O(n) search here?
        using InitializerTuple = Tuple<Initializers...>;
        using Checker = ::tt::detail::is_a<CRMT::template InitializerType>;
        using Initializer = tmpl::front<
            // Find
            tmpl::find<InitializerTuple,
                       // Does it match the template?
                       tmpl::bind<Checker::template check, tmpl::_1>>
            // End find
            >;

        return std::get<Initializer>(t);
      }

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
