#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include "OptimizationLevel.hpp"
#include "Utilities/Math/Linalg.hpp"  // Blaze linalg tools
#include "Utilities/NonCreatable.hpp"

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

      //************************************************************************
      // Batch Operations
      template<typename VT1, typename MT, typename VT2>
      static inline void batch_matvec(VT1& matvec_batch,
                                      const MT& matrix_batch,
                                      const VT2& vector_batch) {
        elastica::batch_matvec(matvec_batch, matrix_batch, vector_batch);
      }

      template<typename M1, typename M2, typename M3>
      static inline void batch_matmul(M1& matmul_batch,
                                      const M2& first_matrix_batch,
                                      const M3& second_matrix_batch) {
        elastica::batch_matvec(matmul_batch, first_matrix_batch, second_matrix_batch);
      }

      template<typename VR, typename V1, typename V2>
      static inline void batch_cross(VR& cross_batch,
                                     const V1& first_vector_batch,
                                     const V2& second_vector_batch) {
        elastica::batch_cross(cross_batch, first_vector_batch, second_vector_batch);
      }

      template<typename V1, typename V2>
      static auto batch_dot(const V1& first_vector_batch,
                            const V2& second_vector_batch){
        return elastica::batch_dot(first_vector_batch, second_vector_batch);
      }

      template<typename V>
      static auto batch_norm(const V& vector_batch) {
        return elastica::batch_norm(vector_batch);
      }

      //************************************************************************
      // Vector Operation
      template<typename VR, typename V>
      static inline void difference_kernel(VR& result_vector,
                                           const V& input_vector){
        elastica::difference_kernel(result_vector, input_vector);
      }

      template<typename VR, typename V>
      static inline void vornoi_length(VR& result_vector,
                                       const V& input_vector){
        elastica::voronoi_length(result_vector, input_vector);
      }
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization for optimized routines
     * \ingroup cosserat_rods
     */
    template<>
    struct Operations<OptimizationLevel::optimized> : private NonCreatable {
      static inline int add(int x, int y) { return x + y; }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace cosserat_rod

}  // namespace elastica
