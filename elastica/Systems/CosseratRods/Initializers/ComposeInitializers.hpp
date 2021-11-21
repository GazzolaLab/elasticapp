//==============================================================================
/*!
//  \file
//  \brief Quickly compose initializers together
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

#include "Systems/CosseratRods/Initializers/detail/Affordances.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace cosserat_rod {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Compose initializers from different components of a Cosserat rod
     * \ingroup cosserat_rod
     *
     * \details
     * ComposeInitializers provides a facade for quickly creating client-facing
     * initializers (i.e. those which accept POD types rather than lambda types)
     * from their simplest possible definitions.
     *
     * \tparam ModularizedInitializers The modularized initializers found in
     * ::elastica::cosserat_rod::Initializers
     */
    template <typename... ModularizedInitializers>
    class ComposeInitializers
        : public ModularizedInitializers...,
          public detail::CosseratRodInitializerInterface<
              ComposeInitializers<ModularizedInitializers...>> {
     protected:
      using This = ComposeInitializers<ModularizedInitializers...>;
      using Interface = detail::CosseratRodInitializerInterface<This>;
      friend Interface;

     public:
      using RequiredParameters =
          tmpl::append<typename ModularizedInitializers::RequiredParameters...>;
      using DefaultParameters =
          tmpl::append<typename ModularizedInitializers::DefaultParameters...>;
      using DefaultInitializers =
          tmpl::transform<DefaultParameters,
                          tmpl::bind<tmpl::type_from, tmpl::_1>>;
      using OptionsCache = tmpl::as_tuple<RequiredParameters>;
      using DefaultInitializersCache = tmpl::as_tuple<DefaultInitializers>;

     private:
      template <typename... Args, typename... RequiredTags,
                typename... DefaultTags>
      ComposeInitializers(std::tuple<Args...> tup,
                          tmpl::list<RequiredTags...> /*meta*/,
                          tmpl::list<DefaultTags...> /* meta*/)
          : ModularizedInitializers()...,
            Interface(),
            cache_(std::get<RequiredTags>(std::move(tup))...),
            default_initializers_cache_((std::move(DefaultTags{}.data))...) {}

     public:
      template <typename... Args>
      explicit ComposeInitializers(std::tuple<Args...> tup)
          : ComposeInitializers(std::move(tup), RequiredParameters{},
                                DefaultParameters{}) {}

      template <typename... Args>
      explicit ComposeInitializers(Args&&... args)
          : ComposeInitializers(std::make_tuple(std::forward<Args>(args)...)) {}

     public: // TODO : make it protected later on
      decltype(auto) get_required_parameters() const /*noexcept*/ {
        // pass directly as references
        // return detail::pass_as_wrapped<tmpl::list<NElement,
        // Radius>>(data());
        return std::tuple_cat(
            ModularizedInitializers::get_required_parameters(data())...);
      }

      // TODO : needs to be refactored as not public
      using Interface::get_optional_parameters;

     public:
      auto data() const& noexcept -> OptionsCache const& { return cache_; }
      auto data() & noexcept -> OptionsCache& { return cache_; }

      auto default_initializers() const& noexcept
          -> DefaultInitializersCache const& {
        return default_initializers_cache_;
      }
      auto default_initializers() & noexcept -> DefaultInitializersCache& {
        return default_initializers_cache_;
      }

     private:
      OptionsCache cache_;
      DefaultInitializersCache default_initializers_cache_;
    };
    //**************************************************************************

  }  // namespace cosserat_rod

}  // namespace elastica
