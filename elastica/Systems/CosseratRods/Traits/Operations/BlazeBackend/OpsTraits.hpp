#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include <blaze/Blaze.h>
#include <blaze_tensor/Blaze.h>

// Utils
#include "Utilities/NonCreatable.hpp"

/// Add BlazeBackend code here
#include "ErrorHandling/Assert.hpp"
#include "Systems/CosseratRods/Traits/Operations/BlazeBackend/InvRotateDivide/SIMD.hpp"
#include "Systems/CosseratRods/Traits/Operations/BlazeBackend/OptimizationLevel.hpp"
#include "Utilities/Math/BlazeDetail/BlazeCalculus.hpp"
#include "Utilities/Math/BlazeDetail/BlazeLinearAlgebra.hpp"
#include "Utilities/Math/BlazeDetail/BlazeRotation.hpp"

namespace elastica {

  namespace cosserat_rod {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Rod Operations specialized for blaze data structures
     * \ingroup cosserat_rods
     */
    template <OptimizationLevel O>
    struct OpsTraits : private NonCreatable {};
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond Utility */
    /*!\brief useful overloads
     * \ingroup cosserat_rods
     */
    template <typename VT, bool TF>
    static inline decltype(auto) to_matrix(const ::blaze::Vector<VT, TF>& vec) {
      return ::blaze::expand<1UL>(vec);
    }
    template <typename VT, bool TF>
    static inline std::size_t get_batch_elems(
        ::blaze::Vector<VT, TF> const& scalar_batch) {
      // return ::blaze::rows(*scalar_batch);
      return (*scalar_batch).size();
    }
    template <typename MT, bool SO>
    static inline std::size_t get_batch_elems(
        ::blaze::Matrix<MT, SO> const& vector_batch) {
      // return ::blaze::columns(*vector_batch);
      return (*vector_batch).columns();
    }
    template <typename TT>
    static inline std::size_t get_batch_elems(
        ::blaze::Tensor<TT> const& matrix_batch) {
      // return ::blaze::columns(*matrix_batch);
      return (*matrix_batch).columns();
    }
    template <typename VT, bool TF>
    static inline auto batch_slice(::blaze::Vector<VT, TF> const& scalar_batch,
                                   const std::size_t start_index,
                                   const std::size_t size) {
      return ::blaze::subvector(*scalar_batch, start_index, size);
    }
    template <typename MT, bool SO>
    static inline auto batch_slice(::blaze::Matrix<MT, SO> const& vector_batch,
                                   const std::size_t start_index,
                                   const std::size_t size) {
      return ::blaze::submatrix(*vector_batch, 0UL, start_index, 3UL, size);
    }

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization for non-optimized routines
     * \ingroup cosserat_rods
     */
    template <>
    struct OpsTraits<OptimizationLevel::basic> : private NonCreatable {
      static inline int add(int x, int y) { return x + y; }

      // add axis=0 with size 3
      template <typename T>
      static inline auto expand_for_broadcast(const T& a) {
        return blaze::expand(a, 3UL);
      }
      template <typename T>
      static inline auto inverse_cube(const T& a) {
        return blaze::pow(a, -3);
      }
      template <typename T, typename A>
      static inline auto transpose(const T& tensor, const A& order) {
        return blaze::trans(tensor, order);
      }
      template <typename T>
      static inline auto transpose(const T& tensor) {
        return blaze::trans(tensor);
      }

      //************************************************************************
      // Batch Operations
      // TODO: Documentation
      template <typename MT1, typename MT2, typename VT>
      static inline void batch_division_matvec(MT1& out_matrix,
                                               const MT2& denominator,
                                               const VT& numerator) {
        // const std::size_t n_elems = denominator.columns();
        auto tp_numerator = transpose(numerator);
        blaze::row(out_matrix, 0UL) =
            blaze::row(denominator, 0UL) / tp_numerator;
        blaze::row(out_matrix, 1UL) =
            blaze::row(denominator, 1UL) / tp_numerator;
        blaze::row(out_matrix, 2UL) =
            blaze::row(denominator, 2UL) / tp_numerator;
      }

      // TODO: Documentation
      template <typename VT1, typename MT, typename VT2>
      static inline void batch_matvec(VT1& matvec_batch, const MT& matrix_batch,
                                      const VT2& vector_batch) {
        using ::elastica::batch_matvec;
        batch_matvec(matvec_batch, matrix_batch, vector_batch);
      }
      template <typename MT1,  // blaze Matrix expression type 1
                typename TT,   // blaze Tensor expression type
                typename MT2>  // blaze Matrix expression type 2
      static inline void batch_mattranspvec(MT1& matvec_batch,
                                            const TT& matrix_batch,
                                            const MT2& vector_batch) {
        constexpr std::size_t dimension(3UL);
        for (std::size_t i(0UL); i < dimension; ++i) {
          auto val = blaze::trans(blaze::rowslice(matrix_batch, i));
          blaze::row(matvec_batch, i) =
              blaze::sum<blaze::columnwise>(val % vector_batch);
        }
      }

      template <typename M1, typename M2, typename M3>
      static inline void batch_matmul(M1& matmul_batch,
                                      const M2& first_matrix_batch,
                                      const M3& second_matrix_batch) {
        using ::elastica::batch_matmul;
        batch_matvec(matmul_batch, first_matrix_batch, second_matrix_batch);
      }

      template <typename VR, typename V1, typename V2>
      static inline void batch_cross(VR& cross_batch,
                                     const V1& first_vector_batch,
                                     const V2& second_vector_batch) {
        using ::elastica::batch_cross;
        batch_cross(cross_batch, first_vector_batch, second_vector_batch);
      }

      template <typename V1, typename V2>
      static inline auto batch_dot(const V1& first_vector_batch,
                                   const V2& second_vector_batch) {
        using ::elastica::batch_dot;
        return batch_dot(first_vector_batch, second_vector_batch);
      }

      template <typename V>
      static inline auto batch_norm(const V& vector_batch) {
        using ::elastica::batch_norm;
        return batch_norm(vector_batch);
      }

      //************************************************************************
      // Vector Operation
      // TODO: Documentation
      template <typename V>
      static inline auto difference_kernel(const V& input_vector) {
        using ::elastica::difference_kernel;
        return difference_kernel(input_vector);
      }

      template <typename MT1, typename MT2>
      static inline void two_point_difference_kernel(MT1& input_vector,
                                                     const MT2& output_vector) {
        using ::elastica::two_point_difference_kernel;
        two_point_difference_kernel(input_vector, output_vector);
      }

      //************************************************************************
      /*!\brief Average kernal computation
       *
       * Equivalent to 'node_to_element_pos_or_vel' in pyelastica
       *
       * @tparam MT1
       * @tparam MT2
       * @param in_matrix
       */
      template <typename MT>
      static inline auto average_kernel(const MT& arr) {
        using ValueType = typename MT::ElementType;
        const std::size_t n_elems = get_batch_elems(arr);

        // 0.5*(ref_length[1:] + ref_length[:-1])
        return ValueType(0.5) * (batch_slice(arr, 0UL, n_elems - 1) +
                                 batch_slice(arr, 1UL, n_elems - 1));
      }
      //************************************************************************

      template <typename MT1, typename MT2>
      static inline void quadrature_kernel(MT1& out_matrix,
                                           const MT2& in_matrix) {
        using ::elastica::quadrature_kernel;
        quadrature_kernel(out_matrix, in_matrix);
      }

      //************************************************************************
      // Batch Rotation Operation
      // TODO: Documentation
      template <typename MT,  // blaze Matrix expression type
                typename TT>  // blaze Tensor expression type
      static inline void batch_inv_rotate(MT& rot_axis_vector_batch,
                                          const TT& rot_matrix_batch) {
        using ::elastica::batch_inv_rotate;
        batch_inv_rotate(rot_axis_vector_batch, rot_matrix_batch);
      }

      //************************************************************************
      // Batch Rotation (exponential) Operation
      // TODO: Documentation
      template <typename MT,  // blaze Matrix expression type
                typename TT>  // blaze Tensor expression type
      static inline void batch_mat_exp(MT& rot_matrix_batch,
                                       const TT& rot_axis_vector_batch) {
        using ::elastica::exp_batch;
        exp_batch(rot_matrix_batch, rot_axis_vector_batch);
      }

      //************************************************************************
      // Spanwise Inverse Rotation + Division Operation
      // specifically used for compute-curvature
      // TODO: Documentation
      template <typename MT,  // blaze Matrix expression type
                typename TT,  // blaze Tensor expression type
                typename VT>  // blaze Matrix expression type
      static inline void spanwise_inv_rotate_and_divide(
          MT& rot_axis_vector, const TT& rot_matrix,
          const VT& normalize_vector) {
        constexpr std::size_t dimension(3UL);
        ELASTICA_ASSERT(rot_matrix.pages() == dimension,
                        "Incorrect dimension, matrix must be 3D");
        ELASTICA_ASSERT(rot_matrix.rows() == dimension,
                        "Incorrect dimension, matrix must be 3D");
        ELASTICA_ASSERT(rot_axis_vector.rows() == dimension,
                        "Incorrect dimension, vector must be 3D");
        ELASTICA_ASSERT(
            (rot_axis_vector.columns() + 1UL) == rot_matrix.columns(),
            "Invariance in in number of elements and voronoi regions in "
            "Cosserat Rod violated");
        ELASTICA_ASSERT(
            rot_axis_vector.columns() == normalize_vector.size(),
            "Invariance in number of dofs in voronoi-regions violated");

        using Op = detail::InvRotateDivideOp<detail::InvRotateDivideKind::simd>;
        Op::apply(rot_axis_vector, rot_matrix, normalize_vector);
      }

      //************************************************************************
      // Misc Operation
      // TODO: Try to use slice instead
      template <typename MT>
      static inline void batch_subtract_z_unit(MT& matrix) {
        using ValueType = typename MT::ElementType;
        blaze::row(matrix, 2UL) -= ValueType(1.0);
      }
      template <typename MT>
      static inline void batch_add_z_unit(MT& matrix) {
        using ValueType = typename MT::ElementType;
        blaze::row(matrix, 2UL) += ValueType(1.0);
      }
    };
    /*! \endcond */
    //**************************************************************************

    //    //********************************************************************
    //    /*! \cond ELASTICA_INTERNAL */
    //    /*!\brief Specialization for optimized routines
    //     * \ingroup cosserat_rods
    //     */
    //    template<>
    //    struct OpsTraits<OptimizationLevel::A> : private NonCreatable
    //    {};
    //    /*! \endcond */
    //    //********************************************************************

  }  // namespace cosserat_rod

}  // namespace elastica
