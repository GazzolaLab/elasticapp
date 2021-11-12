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
    /*! \cond Utility */
    /*!\brief useful overloads
     * \ingroup cosserat_rods
     */
    template <typename VT, bool TF>
    static inline decltype(auto) toMatrix(const ::blaze::Vector<VT, TF>& vec){
      return ::blaze::expand<1UL>(vec);
    }
    template <typename VT, bool TF>
    static inline std::size_t get_batch_elems(::blaze::Vector<VT, TF> const& scalar_batch){
      // return ::blaze::rows(*scalar_batch);
      return (*scalar_batch).size();
    }
    template <typename MT, bool SO>
    static inline std::size_t get_batch_elems(::blaze::Matrix<MT, SO> const& vector_batch){
      // return ::blaze::columns(*vector_batch);
      return (*vector_batch).columns();
    }
    template <typename TT>
    static inline std::size_t get_batch_elems(::blaze::Tensor<TT> const& matrix_batch){
      //return ::blaze::columns(*matrix_batch);
      return (*matrix_batch).columns();
    }
    template <typename VT, bool TF>
    static inline auto batch_slice(::blaze::Vector<VT, TF> const& scalar_batch, const std::size_t start_index, const std::size_t size){
      return ::blaze::subvector(*scalar_batch, start_index, size);
    }
    template <typename MT, bool SO>
    static inline auto batch_slice(::blaze::Matrix<MT, SO> const& vector_batch, const std::size_t start_index, const std::size_t size){
      return ::blaze::submatrix(*vector_batch, 0UL, start_index, 3UL, size);
    }

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization for non-optimized routines
     * \ingroup cosserat_rods
     */
    template <>
    struct Operations<OptimizationLevel::basic> : private NonCreatable {
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
      static inline void batch_division_matvec(MT1& out_matrix, const MT2& denominator, const VT& numerator){
        //const std::size_t n_elems = denominator.columns();
        blaze::row(out_matrix, 0UL) = blaze::row(denominator, 0UL) / transpose(numerator);
        blaze::row(out_matrix, 1UL) = blaze::row(denominator, 1UL) / transpose(numerator);
        blaze::row(out_matrix, 2UL) = blaze::row(denominator, 2UL) / transpose(numerator);
      }
//      template <typename MT, typename VT>
//      static inline void batch_division_matvec(MT& out_matrix, const VT& numerator){
//        using ValueType = typename MT::ElementType;
//        const std::size_t n_elems = out_matrix.columns();
//        blaze::row(out_matrix, 0UL) = blaze::row(out_matrix, 0UL) / transpose(numerator);
//        blaze::row(out_matrix, 1UL) = blaze::row(out_matrix, 1UL) / transpose(numerator);
//        blaze::row(out_matrix, 2UL) = blaze::row(out_matrix, 2UL) / transpose(numerator);
//      }

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
          auto val1 = blaze::trans(blaze::rowslice(matrix_batch, i));
          auto val2 = blaze::sum<blaze::columnwise>(val1 % vector_batch);
          blaze::row(matvec_batch, i) = val2;
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


      //**************************************************************************
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
        return ValueType(0.5) * (batch_slice(arr, 0UL, n_elems-1) +
                                 batch_slice(arr, 1UL, n_elems-1));
      };
      //**************************************************************************

      template<typename MT1, typename MT2>
      static inline void quadrature_kernel(MT1& out_matrix, const MT2& in_matrix){
        using ::elastica::quadrature_kernel;
        quadrature_kernel(out_matrix, in_matrix);
      }

      //************************************************************************
      // Batch Rotation Operation
      // TODO: Documentation
//      template <typename MT,  // blaze Matrix expression type
//                typename TT>  // blaze Tensor expression type
//      static inline void batch_inv_rotate(MT& rot_axis_vector_batch, const TT& rot_matrix_batch) {
//        using ::elastica::batch_inv_rotate;
//        batch_inv_rotate(rot_axis_vector_batch, rot_matrix_batch);
//      }

      //************************************************************************
      // Batch Rotation (exponential) Operation
      // TODO: Documentation
      template <typename MT,  // blaze Matrix expression type
                typename TT>  // blaze Tensor expression type
      static inline void batch_mat_exp(MT& rot_matrix_batch, const TT& rot_axis_vector_batch) {
        using ::elastica::exp_batch;
        exp_batch(rot_matrix_batch, rot_axis_vector_batch);
      }

      //************************************************************************
      // Spanwise Inverse Rotation + Division Operation (specific for compute-curvature)
      // Inverse rodrigues and division always comes together.
      // TODO: Avoid repeating code. Try to reuse inv-rotate
      // TODO: Documentation
      template <typename MT,  // blaze Matrix expression type
                typename TT, // blaze Tensor expression type
                typename VT>  // blaze Matrix expression type
      static inline void spanwise_inv_rotate_and_divide(MT& rot_axis_vector_batch,
                                                     const TT& rot_matrix_batch,
                                                     const VT& normalize_vector_batch) {
        constexpr std::size_t dimension(3UL);
        const std::size_t blocksize = rot_axis_vector_batch.columns();
        using ValueType = typename TT::ElementType;
        assert(rot_matrix_batch.pages() == dimension);
        assert(rot_matrix_batch.rows() == dimension);
        assert(rot_axis_vector_batch.rows() == dimension);

        /* hardcoded to avoid external memory passing */
        /* also now operates only on views */
        // TODO: Revisit - try to optimize using reduced operations (if any)
        /*
         * R = Q_t Q_{t+1} (elementwise)
         * theta = acos((tr(R) - 1) / 2)
         */
        auto q_next = blaze::subtensor(rot_matrix_batch, 0UL, 0UL, 1UL, dimension, dimension, blocksize);
        auto q = blaze::subtensor(rot_matrix_batch, 0UL, 0UL, 0UL, dimension, dimension, blocksize);
        auto qqt = q % q_next;
        auto rot_matrix_trace_batch =
            blaze::row(blaze::pageslice(qqt, 0UL), 0UL) +
            blaze::row(blaze::pageslice(qqt, 0UL), 1UL) +
            blaze::row(blaze::pageslice(qqt, 0UL), 2UL) +
            blaze::row(blaze::pageslice(qqt, 1UL), 0UL) +
            blaze::row(blaze::pageslice(qqt, 1UL), 1UL) +
            blaze::row(blaze::pageslice(qqt, 1UL), 2UL) +
            blaze::row(blaze::pageslice(qqt, 2UL), 0UL) +
            blaze::row(blaze::pageslice(qqt, 2UL), 1UL) +
            blaze::row(blaze::pageslice(qqt, 2UL), 2UL);
        auto theta_batch = blaze::acos(ValueType(0.5) * rot_matrix_trace_batch - ValueType(0.5) -
                                        ValueType(1e-12));  // TODO refactor 1e-12

        auto R_minus_RT =
            blaze::subtensor(rot_matrix_batch, 0UL, 0UL, 0UL, dimension, dimension, blocksize) -
            blaze::trans(
                blaze::subtensor(rot_matrix_batch, 0UL, 0UL, 1UL, dimension, dimension, blocksize),
                {1, 0, 2});
        auto trans_normalize_vector_batch = blaze::trans(normalize_vector_batch);
        /* theta (u) = -theta * inv_skew([R - RT]) / 2 sin(theta) */
        for (std::size_t i(0UL); i < dimension; ++i) {
          auto idx1 = (i + 1UL) % dimension;
          auto idx2 = (i + 2UL) % dimension;
          blaze::row(rot_axis_vector_batch, i) =
              blaze::sum<blaze::rowwise>(blaze::pageslice(q_next, idx2) % blaze::pageslice(q, idx1) -
                                         blaze::pageslice(q_next, idx1) % blaze::pageslice(q, idx2)) *
              ( ValueType(-0.5) * theta_batch * (blaze::sin(theta_batch)+ValueType(1e-12)) ) / // TODO: refactor 1e-12
              trans_normalize_vector_batch;
        }
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
