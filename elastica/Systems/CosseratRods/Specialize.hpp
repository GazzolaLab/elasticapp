//==============================================================================
/*!
//  \file
//  \brief Specalize a plugin for Cosserat Rods
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

/// Forward declarations
#include "Systems/CosseratRods/Types.hpp"

namespace elastica {

  namespace cosserat_rod {

    namespace detail {

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <typename Plugin, typename Tag>
      struct Specialize;

      template <typename CRT, template <typename> class ComputationalBlock,
                template <typename /*CRT*/, typename /* ComputationalBlock */>
                class... Components,
                typename Tag>
      struct Specialize<
          CosseratRodPlugin<CRT, ComputationalBlock, Components...>, Tag> {
        using type = TaggedCosseratRodPlugin<CRT, ComputationalBlock, Tag,
                                             Components...>;
      };

      template <typename CRT, template <typename> class ComputationalBlock,
                typename OldTag,
                template <typename /*CRT*/, typename /* ComputationalBlock */>
                class... Components,
                typename Tag>
      struct Specialize<TaggedCosseratRodPlugin<CRT, ComputationalBlock, OldTag,
                                                Components...>,
                        Tag> {
        using type = TaggedCosseratRodPlugin<CRT, ComputationalBlock, Tag,
                                             Components...>;
      };
      /*! \endcond */
      //************************************************************************
    }  // namespace detail

    //**************************************************************************
    /*!\brief Specializes a CosseratRodPlugin `Plugin` based on a `Tag` type
     * \ingroup cosserat_rod
     *
     * \details
     * Stamps a new Plugin type tagging the `Plugin` with the `Tag`. The new
     * type has the same semantics as the `Plugin` in generating blocks and
     * slices, however the types are not the same. Hence any generated blocks
     * and slices are registered as different types in the simulator. Since the
     * types are different function overloading works---so algorithms can be
     * customized based on the new Plugin type.
     *
     * \usage
     * For a plugin type `Plugin` and a tag type `Tag` (which only needs to be
     * declared)
     * \code
     * using result = specialize<Plugin, Tag>;
     * \endcode
     *
     * \metareturns
     * A new type TaggedPlugin
     *
     * \example
     * \snippet Test_Specialized.cpp specialize_eg
     *
     * \tparam Plugin A cosserat rod Plugin type (maybe tagged or untagged)
     * \tparam Tag A tag type to tag the Plugin above
     */
    template <typename Plugin, typename Tag>
    using specialize = typename detail::Specialize<Plugin, Tag>::type;
    //**************************************************************************
    // using Snake = specialize<CosseratRod, SnakeTag>;
    // using Snake = decltype(specialize(CosseratRod, using, SnakeTag));

  }  // namespace cosserat_rod

}  // namespace elastica