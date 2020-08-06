#pragma once

#include <type_traits>

#include "elastica/Simulator/Environments/CollisionSystem/Detection/Coarse/Tags.hpp"
#include "elastica/Utilities/TMPL.hpp"

namespace elastica {

  namespace collision {

    namespace detection {

      namespace coarse {

        namespace tt {

          namespace ecdc = elastica::collision::detection::coarse;

          namespace detail {
            template <typename FCP, typename SCP = void>
            struct PrimitivesDispatch
                : std::conditional<std::is_same<FCP, SCP>::value,
                                   ecdc::tags::SamePrimitivesTag,
                                   ecdc::tags::DifferentPrimitivesTag> {};

            template <typename FCP, typename SCP>
            struct PrimitivesDispatch<tmpl::list<FCP, SCP>>
                : PrimitivesDispatch<FCP, SCP> {};
          }  // namespace detail

          template <typename L1, typename L2 = void>
          using PrimitivesDispatchType =
              typename detail::PrimitivesDispatch<L1, L2>::type;

          /*Helps extract the second primitives from a matrix of primitives
           *  using FCP = int;
              using PrimitivePrimitiveMatrix =
                  tmpl::list<tmpl::list<int, float>, tmpl::list<int, char>,
                             tmpl::list<char, float>, tmpl::list<char, double>>;

              using ListOfSecondPrimitives =
                  tmpl::fold<PrimitivePrimitiveMatrix, tmpl::list<>,
                            SecondPrimtive<FCP>::Extractor<tmpl::_state,
           tmpl::_element>>;

              ListOfSecondPrimitives <=> tmpl::list<float, char>;
           */
          template <
              typename FirstPrimitiveType>  // Primitive whose second primitives
                                            // are to be extracted
                                            // from a typelist
          struct ExtractSecondPrimitive {
            template <typename S, typename E>
            struct apply;

            template <typename S, typename FP, typename SP>
            struct apply<S, tmpl::list<FP, SP>> {
              using type = typename std::conditional<
                  std::is_same<FirstPrimitiveType, FP>::value,
                  tmpl::push_back<S, SP>, S>::type;
            };
          };

        }  // namespace tt

      }  // namespace coarse

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica