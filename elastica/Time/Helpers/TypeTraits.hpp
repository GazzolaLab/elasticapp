//==============================================================================
/*!
//  \file
//  \brief Time detail type traits
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
///
#include "Time/Helpers/Types.hpp"
///
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits/IsCRTP.hpp"

namespace elastica {

  namespace tt {

    namespace detail {

      //**************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <typename Up>
      struct IsKinematicUpdate
          : ::tt::IsCRTP<::elastica::timestepper::detail::KinematicUpdate, Up> {
      };

      template <typename Up>
      struct IsDynamicUpdate
          : ::tt::IsCRTP<::elastica::timestepper::detail::DynamicUpdate, Up> {};

      template <typename Up>
      struct IsTimeUpdate
          : ::tt::IsCRTP<::elastica::timestepper::detail::TimeUpdate, Up> {};

      template <typename ListOfSubSteps>
      struct Counter {
        template <template <typename> class Predicate>
        using impl_ =
            tmpl::size<tmpl::filter<ListOfSubSteps, Predicate<tmpl::_1>>>;

        static constexpr std::size_t total_n_steps =
            tmpl::size<ListOfSubSteps>::value;
        static constexpr std::size_t n_feval_per_step =
            impl_<IsDynamicUpdate>::value;
        static constexpr std::size_t n_substeps =
            total_n_steps - impl_<IsTimeUpdate>::value;
      };
      /*! \endcond */
      //**************************************************************************
    }  // namespace detail

    //**************************************************************************
    /*!\brief Obtains the total number of steps in a time-stepping algorithm
     *
     * \tparam ListOfSubSteps List of algorithmic update substeps within the
     * time-stepping algorithm
     */
    template <typename ListOfSubSteps>
    inline static constexpr auto total_n_steps() -> std::size_t {
      return detail::Counter<ListOfSubSteps>::total_n_steps;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Obtains the total number of function evaluations in performing a
     * single step of the time-stepping algorithm
     *
     * \tparam ListOfSubSteps List of algorithmic update substeps within the
     * time-stepping algorithm
     */
    template <typename ListOfSubSteps>
    inline static constexpr auto n_feval_per_step() -> std::size_t {
      return detail::Counter<ListOfSubSteps>::n_feval_per_step;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Obtains the total number of substeps in the time-stepping
     * algorithm
     *
     * \tparam ListOfSubSteps List of algorithmic update substeps within the
     * time-stepping algorithm
     */
    template <typename ListOfSubSteps>
    inline static constexpr auto n_substeps() -> std::size_t {
      return detail::Counter<ListOfSubSteps>::n_substeps;
    }
    //**************************************************************************

  }  // namespace tt

}  // namespace elastica
