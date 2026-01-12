//==============================================================================
/*!
//  \file
//  \brief Collection of primitives
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
#include <tuple>
#include <vector>

#include "Utilities/Invoke.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace collision {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    struct SequentialContainerPolicy {
      template <typename T>
      using type = std::vector<T>;
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*!\brief A heterogeneous collection of primitives
     * \ingroup primitives
     */
    template <typename PrimitiveList_,
              typename ContainerPolicy_ = SequentialContainerPolicy>
    class PrimitivesCollection {
     public:
      //**Type definitions******************************************************
      //! List of tags
      using PrimitiveList = PrimitiveList_;
      //! Container policy
      using ContainerPolicy = ContainerPolicy_;
      //! Container type
      using container_type = tmpl::as_tuple<tmpl::transform<
          PrimitiveList, tmpl::bind<ContainerPolicy::template type, tmpl::_1>>>;
      //************************************************************************

     protected:
      //**Type definitions******************************************************
      using Primitives_ = container_type;
      //************************************************************************

      //************************************************************************
      /*!\brief Metafunction to get type of collection given the primtive
       */
      template <typename Primitive>
      static constexpr auto collection_t() noexcept ->
          typename ContainerPolicy::template type<Primitive>;
      //************************************************************************

      // don't declare constructors

     public:
      //**Data access***********************************************************
      /*!\name Data access
       * \brief Access to the underlying data
       */
      //@{
      inline constexpr Primitives_& data() & noexcept { return primitives_; }
      inline constexpr Primitives_ const& data() const& noexcept {
        return primitives_;
      }
      // inline constexpr Primitives_&& data() && noexcept {
      //   return static_cast<Primitives_&&>(primitives_);
      // }
      // inline constexpr Primitives_ const&& data() const&& noexcept {
      //   return static_cast<Primitives_ const&&>(primitives_);
      // }
      //@}
      //************************************************************************

      //**Get functions*********************************************************
      /*!\name Get functions for PrimitiveCollection */
      //@{

      //************************************************************************
      /*!\brief Get number of types of primitive in this Simulator
       */
      static constexpr auto get_number_of_primitive_types() -> std::size_t {
        return tmpl::size<PrimitiveList>::value;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Get number of types of primitive in this Simulator
       */
      static constexpr auto size() -> std::size_t {
        return get_number_of_primitive_types();
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Returns the primitive collection corresponding to a primitive
       *
       * \tparam Primitive primitive to be accessed
       * \return Reference to the primitive collection.
       */
      template <typename Primitive,
                typename RT = decltype(collection_t<Primitive>())>
      inline constexpr auto get_collection() & noexcept -> RT& {
        return std::get<RT>(data());
      }
      template <typename Primitive,
                typename RT = decltype(collection_t<Primitive>())>
      inline constexpr auto get_collection() const& noexcept -> RT const& {
        return std::get<RT>(data());
      }
      //@}
      //************************************************************************

     private:
      //**Member variables******************************************************
      //! The storage of primitive containers
      Primitives_ primitives_;
      //************************************************************************
    };
    //**************************************************************************

    //**Get functions***********************************************************
    /*!\name Get functions for PrimitiveCollection */
    //@{
    template <typename Primitive, typename PrimitiveList_,
              typename ContainerPolicy>
    inline constexpr decltype(auto) get_collection(
        PrimitivesCollection<PrimitiveList_, ContainerPolicy>&
            primitives) noexcept {
      return primitives.template get_collection<Primitive>();
    }
    template <typename Primitive, typename PrimitiveList_,
              typename ContainerPolicy>
    inline constexpr decltype(auto) get_collection(
        PrimitivesCollection<PrimitiveList_, ContainerPolicy> const&
            primitives) noexcept {
      return primitives.template get_collection<Primitive>();
    }
    //@}
    //**************************************************************************

    namespace detail {

      //************************************************************************
      /*!\brief Apply a callable over all primitives
       *
       * \details
       * Applies a callable `func` over all primitives
       * (across types and collections) registered in the current simulator.
       *
       * \note
       * We need strong exception safety guarantee from `func`. In C++14 this
       * cannot be strongly checked, so the user needs to satisfy this
       * guarantee
       *
       * \tparam PrimitivesList A `tmpl::list` of Primitives over which to
       * apply the callable
       * \param func Callable to be applied over primitives
       */
      template <typename F, typename PrimitiveList_, typename ContainerPolicy,
                typename... Primitives>
      inline auto for_each_impl(
          PrimitivesCollection<PrimitiveList_, ContainerPolicy>& primitives,
          F func,
          tmpl::list<Primitives...> /*PrimitiveList*/ =
              PrimitiveList_{}) noexcept -> F {
        auto apply_per_primitive = [func =
                                        std::ref(func)](auto& prim_collection) {
          using std::begin;
          using std::end;
          std::for_each(begin(prim_collection), end(prim_collection), func);
        };

        std::apply(
            [apply_per_primitive](auto&... collection) {
              EXPAND_PACK_LEFT_TO_RIGHT(apply_per_primitive(collection));
            },
            std::tie(get_collection<Primitives>(primitives)...));

        return func;
      }
      //************************************************************************
    }  // namespace detail

    //**Functor methods*********************************************************
    /*!\name Functor methods */
    //@{

    //**************************************************************************
    /*!\brief Apply a callable over a subset of primitives
     *
     * \details
     * Applies a callable `func` over all primitives
     * (across types and collections)
     *
     * \note
     * We need strong exception safety guarantee from `func`. In C++14 this
     * cannot be strongly checked, so the user needs to satisfy this
     * guarantee
     *
     * \tparam PrimitivesList A `tmpl::list` of Primitives over which to
     * apply the callable
     * \param func Callable to be applied over primitives
     */
    template <typename PrimitivesList, typename PrimitiveList_,
              typename ContainerPolicy, typename F>
    inline auto for_each(
        PrimitivesCollection<PrimitiveList_, ContainerPolicy>& primitives,
        F func) noexcept -> F {
      return detail::for_each_impl(primitives, std::move(func),
                                   PrimitivesList{});
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Apply a callable over all primitives
     *
     * \details
     * Applies a callable `func` over all primitives
     * (across types and collections) registered in the current simulator.
     *
     * \note
     * We need strong exception safety guarantee from `func`. In C++14 this
     * cannot be strongly checked, so the user needs to satisfy this
     * guarantee
     *
     * \tparam PrimitivesList A `tmpl::list` of Primitives over which to
     * apply the callable
     * \param func Callable to be applied over primitives
     */
    template <typename PrimitiveList_, typename ContainerPolicy, typename F>
    inline auto for_each(
        PrimitivesCollection<PrimitiveList_, ContainerPolicy>& primitives,
        F func) noexcept -> F {
      return detail::for_each_impl(primitives, std::move(func),
                                   PrimitiveList_{});
    }
    //@}
    //**************************************************************************

  }  // namespace collision

}  // namespace elastica