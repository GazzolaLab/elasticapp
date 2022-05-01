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
#include <vector>

#include "Utilities/AsTaggedTuple.hpp"
#include "Utilities/Invoke.hpp"
#include "Utilities/RepeatAsTuple.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  template <typename Tags_>
  struct Offsets {
   protected:
    //**Type definitions********************************************************
    //! Tags type
    using Tags = Tags_;
    //! Offset type
    using Offset = std::size_t;
    //! Collection
    using container_type = Offset;
    //! internal storage
    template <typename Tag>
    struct Store {
      using type = container_type;
    };
    //! Type of offsets
    using Offsets_ = tmpl::as_tagged_tuple<
        tmpl::transform<Tags, tmpl::bind<Store, tmpl::_1>>>;
    //**************************************************************************

   public:
    Offsets() noexcept
        : offsets_(cpp17::apply(
              [](auto... values) { return Offsets_(values...); },
              repeat_as_tuple<tuples::tuple_size<Offsets_>::value>(0UL))) {}
    ~Offsets() = default;

   public:
    //**Data access*************************************************************
    /*!\name Data access
     * \brief Access to the underlying data
     */
    //@{
    inline constexpr Offsets_& data() & noexcept { return offsets_; }
    inline constexpr Offsets_ const& data() const& noexcept { return offsets_; }
    //@}
    //**************************************************************************

    template <typename Tag, typename LTags>
    friend constexpr auto get_offset(Offsets<LTags>& offsets) noexcept ->
        typename Offsets<LTags>::container_type&;
    template <typename Tag, typename LTags>
    friend constexpr auto get_offset(Offsets<LTags> const& offsets) noexcept ->
        typename Offsets<LTags>::container_type const&;

   private:
    //**Member variables********************************************************
    //! The storage of offsets
    Offsets_ offsets_;
    //**************************************************************************
  };
  /*! \endcond */
  //****************************************************************************

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */

  //**Get functions*************************************************************
  /*!\name Get functions
   * \brief Get an offset corresponding to a system
   */
  //@{
  template <typename Tag, typename Tags_>
  constexpr auto get_offset(Offsets<Tags_>& offsets) noexcept ->
      typename Offsets<Tags_>::container_type& {
    return tuples::get<typename Offsets<Tags_>::template Store<Tag>>(
        offsets.data());
  }
  template <typename Tag, typename Tags_>
  constexpr auto get_offset(Offsets<Tags_> const& offsets) noexcept ->
      typename Offsets<Tags_>::container_type const& {
    return tuples::get<typename Offsets<Tags_>::template Store<Tag>>(
        offsets.data());
  }
  //@}
  //****************************************************************************

  /*! \endcond */
  //****************************************************************************

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  template <typename Tags_>
  struct OffsetsMap {
   protected:
    //**Type definitions********************************************************
    //! Tags type
    using Tags = Tags_;
    //! Offset type
    using Offset = std::size_t;
    //! internal storage
    template <typename Tag>
    struct Store {
      using type = std::unordered_map<Tag const*, Offset>;
    };
    //! Type of offsets
    using OffsetsMap_ = tmpl::as_tagged_tuple<
        tmpl::transform<Tags, tmpl::bind<Store, tmpl::_1>>>;
    //**************************************************************************

   public:
    OffsetsMap() noexcept : offsets_() {}
    ~OffsetsMap() = default;

   public:
    //**Data access*************************************************************
    /*!\name Data access
     * \brief Access to the underlying data
     */
    //@{
    inline constexpr OffsetsMap_& data() & noexcept { return offsets_; }
    inline constexpr OffsetsMap_ const& data() const& noexcept {
      return offsets_;
    }
    //@}
    //**************************************************************************

    template <typename Tag, typename LTags>
    friend constexpr decltype(auto) get_offset(
        OffsetsMap<LTags>& offsets) noexcept;
    template <typename Tag, typename LTags>
    friend constexpr decltype(auto) get_offset(
        OffsetsMap<LTags> const& offsets) noexcept;

   private:
    //**Member variables********************************************************
    //! The storage of offsets
    OffsetsMap_ offsets_;
    //**************************************************************************
  };
  /*! \endcond */
  //****************************************************************************

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */

  //**Get functions*************************************************************
  /*!\name Get functions
   * \brief Get an offset corresponding to a system
   */
  //@{
  template <typename Tag, typename Tags_>
  constexpr decltype(auto) get_offset(OffsetsMap<Tags_>& offsets) noexcept {
    return tuples::get<typename OffsetsMap<Tags_>::template Store<Tag>>(
        offsets.data());
  }
  template <typename Tag, typename Tags_>
  constexpr decltype(auto) get_offset(
      OffsetsMap<Tags_> const& offsets) noexcept {
    return tuples::get<typename OffsetsMap<Tags_>::template Store<Tag>>(
        offsets.data());
  }
  //@}
  //****************************************************************************

  /*! \endcond */
  //****************************************************************************

}  // namespace elastica