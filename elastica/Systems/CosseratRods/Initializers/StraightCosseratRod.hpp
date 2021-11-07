//
// Created by Tejaswin Parthasarathy on 5/20/21.
//

#pragma once

#include <string>

#include "Systems/CosseratRods/Tags.hpp"
#include "Utilities/DefineTypes.h"  // for real_t
#include "Utilities/TMPL.hpp"
///
#include "Systems/CosseratRods/Initializers/ComposeInitializers.hpp"
#include "Systems/CosseratRods/Initializers/Initializers.hpp"

namespace elastica {

  namespace cosserat_rod {

    namespace detail {

      struct CosseratRodOptionGroup {
        static constexpr Options::String help = {
            "Options for initializing Cosserat rods"};
      };

    }  // namespace detail

    class StraightCosseratRod
        : public ComposeInitializers<
              Initializers::CosseratRod,            //
              Initializers::Geometry::StraightRod,  //
              Initializers::Kinematics::DefaultRodKinematics> {
      //     public:
      //      using P = detail::ComposeInitializers<
      //          typename Initializers::CommonInitializerToAllCosseratRods,
      //          typename Initializers::Geometry::StraightRod,
      //          typename Initializers::Kinematics::DefaultRodKinematics>;
      //      using P::P;

     public:
      using options = RequiredParameters;
    };

    //    // Final class, inheriting from many
    //    struct StraightCosseratRod
    //        : public CosseratRodInitializerInterface<StraightCosseratRod> {
    //      using NElement = detail::option_tags::NElement;
    //      using Radius = detail::option_tags::Radius;
    //
    //      using RequiredParameters = tmpl::list<NElement, Radius>;
    //      using DefaultParameters = tmpl::list<detail::VelocityInitializer>;
    //      using DefaultInitializers =
    //          tmpl::transform<DefaultParameters,
    //                          tmpl::bind<tmpl::type_from, tmpl::_1>>;
    //      // for each DI
    //      // defatul_cache_(DI::data...);
    //      //  std::get<Tag>(DefaultInitializer) =  DI::data;
    //
    //      using options = RequiredParameters;
    //
    //      static constexpr Options::String help = {
    //          "Initializes a straight Cosserat rod from origin to
    //          destination"};
    //
    //      using OptionsCache = tmpl::as_tuple<options>;
    //      using DefaultInitializersCache =
    //      tmpl::as_tuple<DefaultInitializers>;
    //      //      template <typename >
    //      //      StraightCosseratRod(tmpl::list<ApplyTags...> = {},
    //      NElement&& n,
    //      //      Radius&& r)
    //      //          : cache(std::move(n), std::move(r)) {}
    //
    //      //      template <typename... Args>
    //      //      StraightCosseratRod(std::tuple<Args...>&& tup)
    //      //          :
    //      StraightCosseratRod(std::get<NElement>(std::move(tup)),
    //      //                                std::get<Radius>(std::move(tup)))
    //      {}
    //      //
    //      //      // there is a copy here
    //      //      template <typename... Args>
    //      //      StraightCosseratRod(Args&&... args)
    //      //          : A(std::make_tuple(std::forward<Args>(args)...)) {}
    //      //
    //
    //      //      template <typename ... Tags>
    //      //      StraightCosseratRod(tmpl::list<Tags...>, NElement&& n,
    //      Radius&& r)
    //      //          : cache(std::move(n), std::move(r)) {}
    //
    //     private:
    //      template <typename... Args, typename... RequiredTags,
    //                typename... DefaultTags>
    //      StraightCosseratRod(std::tuple<Args...> tup,
    //                          tmpl::list<RequiredTags...> /*meta*/,
    //                          tmpl::list<DefaultTags...> /* meta*/)
    //          : cache(std::get<RequiredTags>(std::move(tup))...),
    //            default_initializers_cache_((std::move(DefaultTags{}.data))...)
    //            {}
    //
    //     public:
    //      template <typename... Args>
    //      StraightCosseratRod(std::tuple<Args...> tup)
    //          : StraightCosseratRod(std::move(tup), RequiredParameters{},
    //                                DefaultParameters{}) {}
    //
    //      template <typename... Args>
    //      StraightCosseratRod(Args&&... args)
    //          :
    //          StraightCosseratRod(std::make_tuple(std::forward<Args>(args)...))
    //          {}
    //
    //     protected:
    //      decltype(auto) get_required_parameters() const noexcept {
    //        // pass directly as references
    //        return detail::pass_as_wrapped<tmpl::list<NElement,
    //        Radius>>(data());
    //      }
    //
    //     public:
    //      auto data() const& noexcept -> OptionsCache const& { return cache_;
    //      } auto data() & noexcept -> OptionsCache& { return cache_; }
    //
    //      auto default_initializers() const& noexcept
    //          -> DefaultInitializersCache const& {
    //        return default_initializers_cache_;
    //      }
    //      auto default_initializers() & noexcept -> DefaultInitializersCache&
    //      {
    //        return default_initializers_cache_;
    //      }
    //
    //     private:
    //      OptionsCache cache_;
    //      DefaultInitializersCache default_initializers_cache_;
    //    };

    struct straight_rod {
      using type = StraightCosseratRod;
      static constexpr Options::String help = {"Initialize a straight rod"};
      using group = detail::CosseratRodOptionGroup;
    };

    //    struct StraightCosseratRodWithTwist : public StraightCosseratRod {
    //      using Parent = StraightCosseratRod;
    //      using Twist = detail::option_tags::Twist;
    //
    //      using this_options = tmpl::list<Twist>;
    //      using options = tmpl::append<typename Parent::options,
    //      this_options>;
    //
    //     private:
    //      template <typename... OrderedTags, typename... Args>
    //      StraightCosseratRodWithTwist(std::tuple<Args...>&& tup,
    //                                   tmpl::list<Tags...> /*meta*/)
    //          : Parent(std::move(tup)),
    //            cache(std::get<OrderedTags>(std::move(tup))...) {}
    //
    //     public:
    //      template <typename... Args>
    //      StraightCosseratRodWithTwist(std::tuple<Args...>&& tup)
    //          : StraightCosseratRodWithTwist(std::move(tup), this_options{})
    //          {}
    //
    //      template <typename... Args>
    //      StraightCosseratRodWithTwist(Args&&... args)
    //          : StraightCosseratRodWithTwist(
    //                std::make_tuple(std::forward<Args>(args)...),
    //                this_options{}) {}
    //    };

    //    template <typename ...Args>
    //    straight_rod(Args&& ...args){
    //
    //      StraightCosseratRod(std::forward<Args>(args)...);
    //    }

  }  // namespace cosserat_rod

}  // namespace elastica