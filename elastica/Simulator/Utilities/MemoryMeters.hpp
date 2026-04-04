//==============================================================================
/*!
//  \file
//  \brief
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include "Utilities/AsTaggedTuple.hpp"
#include "Utilities/MemoryMeter.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  template <typename Tags_>
  struct MemoryMeters {
   protected:
    //**Type definitions********************************************************
    //! Tags type
    using Tags = Tags_;
    //! Type of meter used
    using MemoryMeter = ::elastica::MemoryMeter;

    template <typename Tag>
    struct Store {
      using type = MemoryMeter;
    };

    //! Type of memory meters
    using MemoryMeters_ = tmpl::as_tagged_tuple<
        tmpl::transform<Tags, tmpl::bind<Store, tmpl::_1>>>;
    //**************************************************************************

    template <typename Tag, typename LTags>
    friend constexpr auto get_meter(MemoryMeters<LTags>& memory_meters) noexcept
        -> typename MemoryMeters<LTags>::MemoryMeter&;
    template <typename Tag, typename LTags>
    friend constexpr auto get_meter(
        MemoryMeters<LTags> const& memory_meters) noexcept ->
        typename MemoryMeters<LTags>::MemoryMeter const&;

   public:
    //**Data access*************************************************************
    /*!\name Data access
     * \brief Access to the underlying data
     */
    //@{
    inline constexpr MemoryMeters_& data() & noexcept { return memory_meters_; }
    inline constexpr MemoryMeters_ const& data() const& noexcept {
      return memory_meters_;
    }
    //@}
    //**************************************************************************

    //************************************************************************
    /*!\brief Get the number of registered memory meters
     */
    static inline constexpr auto size() noexcept -> std::size_t {
      return tmpl::size<Tags>::value;
    }
    //************************************************************************

   private:
    //**Member variables********************************************************
    //! The storage of memory_meters
    MemoryMeters_ memory_meters_;
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
  template <typename Tag, typename LTags>
  constexpr auto get_meter(MemoryMeters<LTags>& memory_meters) noexcept ->
      typename MemoryMeters<LTags>::MemoryMeter& {
    return tuples::get<typename MemoryMeters<LTags>::template Store<Tag>>(
        memory_meters.data());
  }
  template <typename Tag, typename LTags>
  constexpr auto get_meter(MemoryMeters<LTags> const& memory_meters) noexcept ->
      typename MemoryMeters<LTags>::MemoryMeter const& {
    return tuples::get<typename MemoryMeters<LTags>::template Store<Tag>>(
        memory_meters.data());
  }
  //@}
  //****************************************************************************

  //**Get functions*************************************************************
  /*!\name For each function
   * \brief Applies f for each tag
   */
  //@{
  template <typename LTags, typename Func>
  auto for_each(MemoryMeters<LTags> const& meters, Func f) -> Func {
    tmpl::for_each<LTags>([&](auto tag) {
      using Tag = tmpl::type_from<decltype(tag)>;
      f(get_meter<Tag>(meters));
    });
    return f;
  }
  template <typename LTags, typename Func>
  auto for_each(MemoryMeters<LTags>& meters, Func f) -> Func {
    tmpl::for_each<LTags>([&](auto tag) {
      using Tag = tmpl::type_from<decltype(tag)>;
      f(get_meter<Tag>(meters));
    });
    return f;
  }
  //@}
  //****************************************************************************

  /*! \endcond */
  //****************************************************************************

}  // namespace elastica
