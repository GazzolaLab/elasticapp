//==============================================================================
/*!
//  \file
//  \brief Support for types found in \<tuple\>
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

// Reused from cereal : https://uscilab.github.io/cereal/index.html
// Distributed under the BSD License.
// See https://github.com/USCiLab/cereal/blob/master/LICENSE for details.

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <tuple>

#include "Simulator/IO/Serialization/Helpers.hpp"
#include "Simulator/IO/Serialization/Serialize.hpp"
#include "Utilities/Invoke.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace io {

    namespace detail {

      //************************************************************************
      //! Creates a c string from a sequence of characters
      /*! The c string created will always be prefixed by "tuple_element"
          Based on code from: http://stackoverflow/a/20973438/710791
           */
      template <char... Cs>
      struct char_seq_to_c_str {
        // Size of array for the word: tuple_element
        static constexpr int size = 14;
        typedef const char (&arr_type)[sizeof...(Cs) + size];
        static constexpr char str[sizeof...(Cs) + size] = {
            't', 'u', 'p', 'l', 'e', '_',   'e', 'l',
            'e', 'm', 'e', 'n', 't', Cs..., '\0'};
      };
      //************************************************************************

      template <char... Cs>
      constexpr char char_seq_to_c_str<Cs...>::str[sizeof...(Cs) + size];

      //************************************************************************
      /*! \brief Converts a number into a sequence of characters
       *  @tparam Q The quotient of dividing the original number by 10
       *  @tparam R The remainder of dividing the original number by 10
       *  @tparam C The sequence built so far
       */
      template <size_t Q, size_t R, char... C>
      struct to_string_impl {
        using type =
            typename to_string_impl<Q / 10, Q % 10,
                                    static_cast<char>(R + std::size_t{'0'}),
                                    C...>::type;
      };
      //************************************************************************

      //************************************************************************
      //! Base case with no quotient
      template <size_t R, char... C>
      struct to_string_impl<0, R, C...> {
        using type =
            char_seq_to_c_str<static_cast<char>(R + std::size_t{'0'}), C...>;
      };
      //************************************************************************

      //! Generates a c string for a given index of a tuple
      /*! Example use:
          @code{cpp}
          tuple_element_name<3>::c_str();// returns "tuple_element3"
          @endcode
          @internal */
      template <size_t T>
      struct tuple_element_name {
        using type = typename to_string_impl<T / 10, T % 10>::type;
        static constexpr typename type::arr_type c_str() { return type::str; }
      };

      // unwinds a tuple to save it
      template <size_t Height>
      struct tuple_serialize {
        template <class Archive, class... Types>
        inline static void apply(Archive& ar, std::tuple<Types...>& tuple) {
          tuple_serialize<Height - 1>::apply(ar, tuple);
          ar(make_nvp<Archive>(tuple_element_name<Height - 1>::c_str(),
                               std::get<Height - 1>(tuple)));
        }

        template <class Archive, class... Types>
        inline static void apply(Archive& ar,
                                 std::tuple<Types...> const& tuple) {
          tuple_serialize<Height - 1>::apply(ar, tuple);
          ar(make_nvp<Archive>(tuple_element_name<Height - 1>::c_str(),
                               std::get<Height - 1>(tuple)));
        }
      };

      // Zero height specialization - nothing to do here
      template <>
      struct tuple_serialize<0> {
        template <class Archive, class... Types>
        inline static void apply(Archive&, std::tuple<Types...> const&) {}
      };

    }  // namespace detail

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for std::tuple
    // \ingroup stl_support
    */
    template <class... Types>
    struct Serialize<std::tuple<Types...>> {
      using type = std::tuple<Types...>;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& tuple) {
        detail::tuple_serialize<
            std::tuple_size<std::tuple<Types...>>::value>::apply(ar, tuple);
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& tuple) {
        detail::tuple_serialize<
            std::tuple_size<std::tuple<Types...>>::value>::apply(ar, tuple);
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
