//==============================================================================
/*!
//  \file
//  \brief Adapter for internal torques
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

///
#include "ErrorHandling/Assert.hpp"
//
#include "Simulator/Forcing/Forcing.hpp"
//
#include "Systems/CosseratRods.hpp"
//
#include "Time/SimulationTime.hpp"
//
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/Registration.hpp"
//
#include <algorithm>

namespace elastica {

  namespace forcing {

    template <typename F, typename CRT,
              template <typename> class ComputationalBlock,
              template <typename /*CRT*/, typename /*InitializedBlock*/>
              class... Components>
    void apply_as_internal_torques(
        F const& func, elastica::Time time_v,
        elastica::cosserat_rod::CosseratRodPlugin<CRT, ComputationalBlock,
                                                  Components...>& block_like,
        std::size_t start, std::size_t stop) {
      // Compute total length of system
      auto&& rest_length =
          ::blocks::get<::elastica::tags::ReferenceElementLength>(
              block_like.self());

      // stop() is non-inclusive, so it is already one past the end
      auto const total_length = std::accumulate(rest_length.begin() + start,
                                                rest_length.begin() + stop,
                                                typename CRT::real_type{0.0});
      auto const inv_total_length = typename CRT::real_type(1.0) / total_length;
      // Expects(rest_length.begin() + stop == rest_length.end());

      // start() is inclusive, so go one past the value.
      auto const start_length = std::accumulate(
          rest_length.begin(), rest_length.begin() + start + 1UL,
          typename CRT::real_type{0.0});

      auto const t_val = time_v.value();
      auto s_val = start_length * inv_total_length;

      // start() and stop() are designed to be element indices
      // s_val never reaches the last element : so s_val goes over the voronoi
      // indices.
      // First and last elements therefore receive more weightage : so ensure
      // that here the value is ~0.
      for (auto idx = (start + 1); idx < stop; ++idx) {
        // Don't perform partial sum, rather eval it lazily here
        Vec3 const f_val = func(s_val, t_val);  // at node idx
        ::elastica::external_torques(block_like, idx - 1) -=
            ::elastica::director(block_like, idx - 1) * f_val;
        ::elastica::external_torques(block_like, idx) +=
            ::elastica::director(block_like, idx) * f_val;

        s_val += (rest_length[idx] * inv_total_length);
      }

      // Ensures(s_val == approx(1.0));
    }

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <typename TorqueFunc, typename Registrars>
    class InternalTorquesAdapter;
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    namespace Registrars {
      //! Registration type for InternalTorquesAdapter
      template <typename TorqueFunc>
      using InternalTorquesAdapter =
          ::Registration::Registrar<InternalTorquesAdapter, TorqueFunc>;
    }  // namespace Registrars
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*!\brief Internal torque adapter for arbitrary spatio-temporal activations.
     * \ingroup forcing
     *
     * \details
     * For rapidly prototyping activation functions within \elastica, we utilize
     * a InternalTorquesAdapter instead of creating custom forcing classes which
     * require a lot of boiler-plate code.
     *
     * \tparam TorqueFunc Type of torque functor
     * \tparam Registrars Registration helpers
     *
     * \see as_internal_torques()
     */
    template <typename TorqueFunc,
              typename Registrars =
                  tmpl::list<Registrars::InternalTorquesAdapter<TorqueFunc>>>
    class InternalTorquesAdapter final
        : public Forcing<Registrars,
                         InternalTorquesAdapter<TorqueFunc, Registrars>> {
     public:
      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{

      //************************************************************************
      /*!\brief SFINAE Constructor for copyable functors
       */
      explicit InternalTorquesAdapter(TorqueFunc rr) : torque_(std::move(rr)) {}
      //************************************************************************

      //************************************************************************
      /*!\brief The move constructor.
       */
      InternalTorquesAdapter(InternalTorquesAdapter&& other) noexcept
          : torque_(std::move(other.torque_)) {}
      //@}
      //************************************************************************

     public:
      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      //************************************************************************
      /*!\brief destructor
       */
      ~InternalTorquesAdapter() final = default;
      //@}
      //************************************************************************

      //**Forcing functions**************************************************
      /*!\name Forcing functions */
      //@{

      //************************************************************************
      /*!\brief Apply forces
       *
       * \see make_force()
       */
      template <typename TimeType, typename System>
      inline auto apply_forces(TimeType, System&, int) const -> void {}
      //************************************************************************

      template <typename TimeType, typename System>
      inline auto apply_forces_on_range(TimeType, System&, std::size_t,
                                        std::size_t) const -> void {}

      //************************************************************************
      /*!\brief Apply torques
       */
      template <typename TimeType, typename System>
      inline auto apply_torques(TimeType, System&, int) const -> void {
        ELASTICA_ASSERT(false, "Dont call the index function");
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply torques
       */
      template <typename TimeType, typename System>
      inline auto apply_torques_on_range(TimeType t, System& sys,
                                         std::size_t start,
                                         std::size_t stop) const -> void {
        apply_as_internal_torques(torque_, t, sys, start, stop);
      }
      //************************************************************************

      //@}
      //************************************************************************

     private:
      //**Member variables******************************************************
      //! Functor applying torques
      TorqueFunc torque_;
      //************************************************************************
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Makes an internal torque function
     * \ingroup forcing
     *
     * \details
     * Takes a forcing function with signature (s, t) (where s is the rod
     * centerline as a floating point number, and t is the time as a floating
     * point number) and adapts it to apply the forces as internal torques on
     * the rod.
     *
     * \param f A functor with signature (s, t) that applies torques
     * \return Forcing
     */
    template <typename F>
    decltype(auto) as_internal_torques(F func) {
      using RT = InternalTorquesAdapter<F>;
      return RT(std::move(func));
    }
    //**************************************************************************

  }  // namespace forcing

}  // namespace elastica