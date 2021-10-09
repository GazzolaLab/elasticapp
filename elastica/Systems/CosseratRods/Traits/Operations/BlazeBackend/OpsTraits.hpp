#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include "Utilities/NonCreatable.hpp"

#include "elastica/Utilities/Math/BlazeDetail/BlazeLinearAlgebra.hpp"
#include "elastica/Utilities/Math/BlazeDetail/BlazeCalculus.hpp"
#include "elastica/Utilities/Math/BlazeDetail/BlazeRotation.hpp"

#include "elastica/Systems/CosseratRods/Traits/Operations/BlazeBackend/OptimizationLevel.hpp"

#include "blaze/Blaze.h"
#include "blaze_tensor/Blaze.h"

/// Add BlazeBackend code here

namespace elastica {

  namespace cosserat_rod {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Rod Operations specialized for blaze data structures
     * \ingroup cosserat_rods
     */
    template <OptimizationLevel O>
    struct Operations : private NonCreatable{
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization for non-optimized routines
     * \ingroup cosserat_rods
     */
    template <>
    struct Operations<OptimizationLevel::basic> : private NonCreatable {

      static inline int add(int x, int y) { return x + y; }
      template<typename T>
      static inline auto expand_for_broadcast(const T& a)
      { return blaze::expand<3UL>(a); }
      template<typename T>
      static inline auto inverse_cube(const T& a)
      { return blaze::pow(a, -3); }
      template<typename T, typename A>
      static inline auto transpose(const T& tensor, const A& order)
      { return blaze::trans(tensor, order); }

      //************************************************************************
      // Batch Operations
      // TODO: Documentation
      template<typename VT1, typename MT, typename VT2>
      static inline void batch_matvec(VT1& matvec_batch,
                                      const MT& matrix_batch,
                                      const VT2& vector_batch) {
        using ::elastica::batch_matvec;
        batch_matvec(matvec_batch, matrix_batch, vector_batch);
      }

      template<typename M1, typename M2, typename M3>
      static inline void batch_matmul(M1& matmul_batch,
                                      const M2& first_matrix_batch,
                                      const M3& second_matrix_batch) {
        using ::elastica::batch_matmul;
        batch_matvec(matmul_batch, first_matrix_batch, second_matrix_batch);
      }

      template<typename VR, typename V1, typename V2>
      static inline void batch_cross(VR& cross_batch,
                                     const V1& first_vector_batch,
                                     const V2& second_vector_batch) {
        using ::elastica::batch_cross;
        batch_cross(cross_batch, first_vector_batch, second_vector_batch);
      }

      template<typename V1, typename V2>
      static inline auto batch_dot(const V1& first_vector_batch,
                            const V2& second_vector_batch){
        using ::elastica::batch_dot;
        return batch_dot(first_vector_batch, second_vector_batch);
      }

      template<typename V>
      static inline auto batch_norm(const V& vector_batch) {
        using ::elastica::batch_norm;
        return batch_norm(vector_batch);
      }

      //************************************************************************
      // Vector Operation
      // TODO: Documentation
      template<typename V>
      static inline auto difference_kernel(const V& input_vector){
        using ::elastica::difference_kernel;
        return difference_kernel(input_vector);
      }

      template<typename MT1, typename MT2>
      static inline void two_point_difference_kernel(MT1& input_vector, const MT2& output_vector){
        using ::elastica::two_point_difference_kernel;
        two_point_difference_kernel(input_vector, output_vector);
      }

      // Node to element (use for voronoi)
      template<typename V>
      static inline auto average_kernel(const V& input_vector){
        using ::elastica::average_kernel;
        return average_kernel(input_vector);
      }

      template<typename MT1, typename MT2>
      static inline void quadrature_kernel(MT1& out_matrix, const MT2& in_matrix){
        using ::elastica::quadrature_kernel;
        quadrature_kernel(out_matrix, in_matrix);
      }

      // TODO: Avoid repeating code. Try to reuse inv-rotate
      template <typename MT1, typename MT2>
      static inline void subtract_quadrature(MT1& out_matrix, const MT2& in_matrix) {
        constexpr std::size_t dimension(3UL);
        const std::size_t n_elems = in_matrix.columns();
        assert(in_matrix.rows() == dimension);
        assert(out_matrix.rows() == dimension);
        const std::size_t n_nodes = n_elems + 1UL;
        assert(out_matrix.columns() == n_nodes);
        using ValueType = typename MT1::ElementType;

        blaze::column(out_matrix, 0UL) -= ValueType(0.5) * blaze::column(in_matrix, 0UL);
        blaze::column(out_matrix, n_nodes - 1UL) -= ValueType(0.5) * blaze::column(in_matrix, n_elems - 1UL);
        blaze::submatrix(out_matrix, 0UL, 1UL, dimension, n_elems - 1UL) -=
            ValueType(0.5) *
            (blaze::submatrix(in_matrix, 0UL, 1UL, dimension, n_elems - 1UL) +
             blaze::submatrix(in_matrix, 0UL, 0UL, dimension, n_elems - 1UL));
      }

      //************************************************************************
      // Batch Rotation Operation
      // TODO: Documentation
      template <typename MT,  // blaze Matrix expression type
                typename TT>  // blaze Tensor expression type
      static inline void batch_inv_rotate(MT& rot_axis_vector_batch, const TT& rot_matrix_batch) {
        using ::elastica::batch_inv_rotate;
        batch_inv_rotate(rot_axis_vector_batch, rot_matrix_batch);
      }

      //************************************************************************
      // Batch Rotation + Division Operation (specific for compute-curvature)
      // Inverse rodrigues and division always comes together.
      // TODO: Avoid repeating code. Try to reuse inv-rotate
      // TODO: Documentation
      template <typename MT,  // blaze Matrix expression type
                typename TT, // blaze Tensor expression type
                typename VT>  // blaze Tensor expression type
      static inline void batch_inv_rotate_and_divide(MT& rot_axis_vector_batch,
                                                     const TT& rot_matrix_batch,
                                                     const VT& normalize_vector_batch) {
        constexpr std::size_t dimension(3UL);
        const std::size_t n_elems = rot_matrix_batch.columns();
        using ValueType = typename TT::ElementType;
        assert(rot_matrix_batch.pages() == dimension);
        assert(rot_matrix_batch.rows() == dimension);
        assert(rot_axis_vector_batch.rows() == dimension);
        assert(rot_axis_vector_batch.columns() == n_elems);
        assert(normalize_vector_batch.rows() == dimension);
        assert(normalize_vector_batch.columns() == n_elems);

        /* hardcoded to avoid external memory passing */
        /* also now operates only on views */
        auto rot_matrix_trace_batch =
            blaze::row(blaze::pageslice(rot_matrix_batch, 0UL), 0UL) +
            blaze::row(blaze::pageslice(rot_matrix_batch, 1UL), 1UL) +
            blaze::row(blaze::pageslice(rot_matrix_batch, 2UL), 2UL);
        /* theta = acos((tr(R) - 1) / 2) */
        auto theta_batch = blaze::acos(ValueType(0.5) *
                                       (rot_matrix_trace_batch - ValueType(1.0) -
                                        ValueType(1e-12)));  // TODO refactor 1e-12
        auto R_minus_RT =
            rot_matrix_batch - blaze::trans(rot_matrix_batch, {1, 0, 2});
        /* theta (u) = -theta * inv_skew([R - RT]) / 2 sin(theta) */
        for (std::size_t i(0UL); i < dimension; ++i) {
          auto inv_skew_symmetric_map = blaze::row(
              blaze::pageslice(R_minus_RT, (i + 2UL) % dimension),
              (i + 1UL) % dimension);  // % for the cyclic rotation of indices
          blaze::row(rot_axis_vector_batch, i) =
              ValueType(-0.5) * theta_batch * inv_skew_symmetric_map /
              (blaze::sin(theta_batch) + ValueType(1e-14)) / // TODO refactor 1e-14
              blaze::row(normalize_vector_batch, i);
        }
      }

      //************************************************************************
      // Misc Operation
      // TODO: Documentation
      template <typename T>
      static inline void batch_subtract_z_unit(T& vector) {
        using ValueType = typename T::ElementType;
        blaze::row(vector, 2UL) -= ValueType(1.0);
      }

    };
    /*! \endcond */
    //**************************************************************************

//    //**************************************************************************
//    /*! \cond ELASTICA_INTERNAL */
//    /*!\brief Specialization for optimized routines
//     * \ingroup cosserat_rods
//     */
//    template<>
//    struct Operations<OptimizationLevel::optimized> : private NonCreatable {};
//    /*! \endcond */
//    //**************************************************************************

  }  // namespace cosserat_rod

}  // namespace elastica
