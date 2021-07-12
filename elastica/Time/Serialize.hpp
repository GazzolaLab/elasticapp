//==============================================================================
/*!
//  \file
//  \brief Serialization support for time-types
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
#include <stdexcept>
#include <string>

#include "Time/TimeStepID.hpp"
#include "Simulator/IO/Serialization/Serialize.hpp"
//
#include "Utilities/MakeString.hpp"

namespace elastica {

  namespace io {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for TimeStepID
     * \ingroup time
     */
    template <>
    struct Serialize<::elastica::TimeStepID> {
      using type = ::elastica::TimeStepID;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& time_step) {
        const auto step_threshold_from_config = time_step.step_threshold();
        typename type::SubStep step_threshold_from_file;
        ar(step_threshold_from_file);
        if (step_threshold_from_file != step_threshold_from_config) {
          throw std::runtime_error(
              std::string(
                  MakeString{}
                  << "The substep->step conversion threshold from the "
                     "timestepper in the executable: "
                  << step_threshold_from_config
                  << " does not match the threshold from the earlier saved "
                     "archive : "
                  << step_threshold_from_file));
        }
        ar(time_step.load_step());
        ar(time_step.load_substep());
      }

      template <typename InArchive>
      static inline void save(InArchive& ar, type const& time_step) {
        ar(time_step.step_threshold());
        ar(time_step.step());
        ar(time_step.substep());
      }
    };
    /*! \endcond */
    //**************************************************************************
  }  // namespace io
}  // namespace elastica
