#pragma once

#include "Systems/States/Groups/States/States.hpp"
#include "Utilities/StdHelpers.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  template <typename Plugin, typename ComputationalBlock>
  class SymplecticPolicy {
   private:
    using This = SymplecticPolicy<Plugin, ComputationalBlock>;

    using Self = ComputationalBlock;
    using Reference = Self&;
    using ConstReference = Self const&;

    inline constexpr Reference self() noexcept {
      return static_cast<Reference>(*this);
    }

    inline constexpr ConstReference self() const noexcept {
      return static_cast<ConstReference>(*this);
    }

   private:
    // reverse wrap
    template <typename... T>
    using state_wrapper = SymplecticState<T...>;

    template <typename L>
    using as_state = tmpl::wrap<L, state_wrapper>;

   protected:
    // How to get types of x, Q, v ,w ,a, alpha here?

    // The states have to use a pointer instead of a reference
    // in order to be valid,initializable and moveable
    struct SymplecticPositions {
      using initializer_type =
          tmpl::list<typename Plugin::PositionCollectionTag,
                     typename Plugin::DirectorCollectionTag>;
      using data_type = as_state<initializer_type>;
    };

    struct SymplecticVelocities {
      using initializer_type =
          tmpl::list<typename Plugin::VelocityCollectionTag,
                     typename Plugin::OmegaCollectionTag>;
      using data_type = as_state<initializer_type>;
    };

    struct SymplecticAccelerations {
      using initializer_type =
          tmpl::list<typename Plugin::AccelerationCollectionTag,
                     typename Plugin::AlphaCollectionTag>;
      using data_type = as_state<initializer_type>;
    };

    using MemberTags = tmpl::list<SymplecticPositions, SymplecticVelocities,
                                  SymplecticAccelerations>;

   public:
    void construct() {
      // Initialize references to all block objets
      tmpl::for_each<This::MemberTags>([this](
                                           auto generated_member_tag) -> void {
        using MemberTag = tmpl::type_from<decltype(generated_member_tag)>;
        using InitializerTags = typename MemberTag::initializer_type;

        // Ugly
        auto temp = [this](auto... Is) {
          return std::make_tuple(
              std::get<tmpl::at_c<InitializerTags, Is>>(self().data())...);
        };

        /*
        tmpl::for_each<InitializerTags>([this](auto initializer_tag){
          using InitializerTag = tmpl::type_from<decltype(initializer_tag)>;
          return std::get<InitializerTag>(self().data());
        });
         */
        std::get<MemberTag>(self().data())
            .construct(index_apply<tmpl::size<InitializerTags>::value>(temp));
      });
    }

   public:
    // Do not need const overloads : if its const, it should be a hard
    // error

    inline decltype(auto) kinematic_states() noexcept {
      return std::get<SymplecticPositions>(self().data());
    }

    inline decltype(auto) kinematic_rates(double /*time_v*/) {
      return dynamic_states();
    }

    inline decltype(auto) dynamic_states() {
      return std::get<SymplecticVelocities>(self().data());
    }

    inline decltype(auto) dynamic_rates(double time_v) {
      /* To be truly lazy, one should only compute acceleration
       * data and internal dynamics here.
       * but the cosserat rods need precomputing to account
       * for environment effects etc. so we can skip that here
       */
      return std::get<SymplecticAccelerations>(self().data());
    }
  };

}  // namespace elastica
