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

#include "Time/Types.hpp"
//
#include "Time/SimulationTime.hpp"
#include "Time/TimeMetadata.hpp"
#include "Time/TimeStepID.hpp"
//
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
        typename type::SubStep step_threshold_from_archive;
        ar(make_nvp<InArchive>("threshold", step_threshold_from_archive));
        if (step_threshold_from_archive != step_threshold_from_config) {
          throw std::runtime_error(
              std::string(
                  MakeString{}
                  << "The substep->step conversion threshold from the "
                     "timestepper in the executable: "
                  << step_threshold_from_config
                  << " does not match the threshold from the earlier saved "
                     "archive : "
                  << step_threshold_from_archive));
        }
        ar(make_nvp<InArchive>("step", time_step.step()));
        ar(make_nvp<InArchive>("substep", time_step.substep()));
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& time_step) {
        ar(make_nvp<OutArchive>("threshold", time_step.step_threshold()));
        ar(make_nvp<OutArchive>("step", time_step.step()));
        ar(make_nvp<OutArchive>("substep", time_step.substep()));
      }
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for SimulationTime
     * \ingroup time
     *
     * \tparam Real Value type of time
     */
    template <typename Real>
    struct Serialize<::elastica::impl::SimulationTime<Real>> {
      using type = ::elastica::impl::SimulationTime<Real>;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& time) {
        ar(time.value());
      }

      template <typename InArchive>
      static inline void save(InArchive& ar, type const& time) {
        ar(time.value());
      }
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for SimulationTimeDelta
     * \ingroup time
     *
     * \tparam Real Value type of time
     */
    template <typename Real>
    struct Serialize<::elastica::impl::SimulationTimeDelta<Real>> {
      using type = ::elastica::impl::SimulationTimeDelta<Real>;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& time_delta) {
        ar(time_delta.value());
      }

      template <typename InArchive>
      static inline void save(InArchive& ar, type const& time_delta) {
        ar(time_delta.value());
      }
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for TimeMetaData
     * \ingroup time
     */
    template <>
    struct Serialize<::elastica::TimeMetadata> {
      using type = ::elastica::TimeMetadata;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& time_metadata) {
        ar(make_nvp("Time", time_metadata.time_v));
        ar(make_nvp("TimeDelta", time_metadata.dt));
        ar(make_nvp("TimeStepID", time_metadata.id));
      }

      template <typename InArchive>
      static inline void save(InArchive& ar, type const& time_metadata) {
        ar(make_nvp("Time", time_metadata.time_v));
        ar(make_nvp("TimeDelta", time_metadata.dt));
        ar(make_nvp("TimeStepID", time_metadata.id));
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
