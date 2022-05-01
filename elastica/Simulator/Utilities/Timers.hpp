//==============================================================================
/*!
//  \file
//  \brief
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
#include <string>

#include "Utilities/AsTaggedTuple.hpp"
#include "Utilities/PrettyType.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TaggedTuple.hpp"
#include "Utilities/Timing.h"

namespace elastica {

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  template <typename Tags_,
            typename TimingPolicy_ = ::elastica::timing::WcPolicy>
  struct Timers {
   protected:
    //**Type definitions********************************************************
    //! Tags type
    using Tags = Tags_;
    //! Timing policy
    using TimingPolicy = TimingPolicy_;
    //! Type of timer used
    using Timer = ::elastica::timing::Timer<TimingPolicy>;
    template <typename Tag>
    struct Store {
      using type = Timer;
      // static std::string name() {
      //   return "Time of " + pretty_type::get_name<>();
      // }
    };
    //! Type of timers
    using Timers_ = tmpl::as_tagged_tuple<
        tmpl::transform<Tags, tmpl::bind<Store, tmpl::_1>>>;
    //**************************************************************************

    template <typename Tag, typename LTags, typename TP>
    friend constexpr auto get_meter(Timers<LTags, TP>& timers) noexcept ->
        typename Timers<LTags, TP>::Timer&;
    template <typename Tag, typename LTags, typename TP>
    friend constexpr auto get_meter(Timers<LTags, TP> const& timers) noexcept ->
        typename Timers<LTags, TP>::Timer const&;

   public:
    //**Data access*************************************************************
    /*!\name Data access
     * \brief Access to the underlying data
     */
    //@{
    inline constexpr Timers_& data() & noexcept { return timers_; }
    inline constexpr Timers_ const& data() const& noexcept { return timers_; }
    //@}
    //**************************************************************************

    //************************************************************************
    /*!\brief Get the number of registered timers
     */
    static inline constexpr auto size() noexcept -> std::size_t {
      return tmpl::size<Tags>::value;
    }
    //************************************************************************

   private:
    //**Member variables********************************************************
    //! The storage of timers
    Timers_ timers_;
    //**************************************************************************
  };
  /*! \endcond */
  //****************************************************************************

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */

  //**Get functions*************************************************************
  /*!\name Get functions
   * \brief Get a meter corresponding to a tag
   * \tparam Tag Tag for obtaining the meter
   */
  //@{
  template <typename Tag, typename LTags, typename TP>
  constexpr auto get_meter(Timers<LTags, TP>& timers) noexcept ->
      typename Timers<LTags, TP>::Timer& {
    return tuples::get<typename Timers<LTags, TP>::template Store<Tag>>(
        timers.data());
  }
  template <typename Tag, typename LTags, typename TP>
  constexpr auto get_meter(Timers<LTags, TP> const& timers) noexcept ->
      typename Timers<LTags, TP>::Timer const& {
    return tuples::get<typename Timers<LTags, TP>::template Store<Tag>>(
        timers.data());
  }
  //@}
  //****************************************************************************

  //**Get functions*************************************************************
  /*!\name For each function
   * \brief Applies f for each tag
   */
  //@{
  template <typename LTags, typename TP, typename Func>
  auto for_each(Timers<LTags, TP> const& timers, Func f) -> Func {
    tmpl::for_each<LTags>([&](auto tag) {
      using Tag = tmpl::type_from<decltype(tag)>;
      f(get_meter<Tag>(timers));
    });
    return f;
  }
  template <typename LTags, typename TP, typename Func>
  auto for_each(Timers<LTags, TP>& timers, Func f) -> Func {
    tmpl::for_each<LTags>([&](auto tag) {
      using Tag = tmpl::type_from<decltype(tag)>;
      f(get_meter<Tag>(timers));
    });
    return f;
  }
  //@}
  //****************************************************************************

  /*! \endcond */
  //****************************************************************************

}  // namespace elastica