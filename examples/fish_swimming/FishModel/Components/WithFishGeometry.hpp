#pragma once

#include <cmath>
#include <elastica/Systems/CosseratRods/Components/Geometry/Protocols.hpp>
#include <elastica/Systems/CosseratRods/Components/Geometry/detail/CircleCrossSectionOperations.hpp>
#include <elastica/Systems/CosseratRods/Components/Geometry/detail/CosseratRodCrossSectionInterface.hpp>
#include <elastica/Systems/CosseratRods/Components/helpers.hpp>
//
#include <elastica/Utilities/Math/Vec3.hpp>
#include <elastica/Utilities/Math/Sqrt.hpp>
//
#include <examples/fish_swimming/FishModel/Tags.hpp>

namespace fish {

  namespace component {

    namespace details {

      template <typename CRT>
      struct FishGeometryVariables {
       private:
        using Traits = CRT;

       protected:
        struct SemiMajorAxis : public Traits::template CosseratRodVariable<
                                   ::fish::tags::SemiMajorAxis,
                                   typename Traits::DataType::Scalar,
                                   typename Traits::Place::OnElement> {};
        struct SemiMinorAxis : public Traits::template CosseratRodVariable<
                                   ::fish::tags::SemiMinorAxis,
                                   typename Traits::DataType::Scalar,
                                   typename Traits::Place::OnElement> {};

        using ComputedVariables = tmpl::list<SemiMajorAxis, SemiMinorAxis>;
        using Variables = ComputedVariables;
      };

    }  // namespace details

    template <typename CRT, typename ComputationalBlock>
    class WithFishGeometry
        : public ::elastica::CRTPHelper<ComputationalBlock, WithFishGeometry>,
          public ::elastica::cosserat_rod::component::detail::
              CosseratRodCrossSectionInterface<
                  CRT, ComputationalBlock,
                  ::elastica::cosserat_rod::component::detail::
                      CircleCrossSectionOperations<CRT>>,
          public details::FishGeometryVariables<CRT>,
          public ::tt::ConformsTo<
              ::elastica::cosserat_rod::component::protocols::Geometry1D>,
          public ::elastica::GeometryComponent<
              WithFishGeometry<CRT, ComputationalBlock>> {
     private:
      using Traits = CRT;
      using CRTP = ::elastica::CRTPHelper<ComputationalBlock, WithFishGeometry>;
      using CrossSection = ::elastica::cosserat_rod::component::detail::
          CircleCrossSectionOperations<Traits>;
      using This = WithFishGeometry<Traits, ComputationalBlock>;
      using Parent = ::elastica::cosserat_rod::component::detail::
          CosseratRodCrossSectionInterface<Traits, ComputationalBlock,
                                           CrossSection>;
      using VariableDefinitions =
          typename details::FishGeometryVariables<Traits>;
      using real_t = typename Traits::real_type;
      using index_t = typename Traits::index_type;

      // sanity check cross section classes
      static_assert(
          cpp17::is_same_v<typename Parent::CrossSection, CrossSection>,
          "Cross section assertion failure, contact the developers!");

     protected:
      using typename Parent::DirectionType;
      using typename Parent::Frame;
      using typename Parent::InitializedVariables;
      using typename VariableDefinitions::SemiMajorAxis;
      using typename VariableDefinitions::SemiMinorAxis;
      using ComputedVariables =
          tmpl::append<typename Parent::ComputedVariables,
                       typename VariableDefinitions::ComputedVariables>;
      using Variables = tmpl::append<typename Parent::Variables,
                                     typename VariableDefinitions::Variables>;

      template <typename BlockLike, typename CosseratInitializer>
      static void initialize(
          WithFishGeometry<Traits, BlockLike>& this_component,
          CosseratInitializer&& initializer) {
        // 1. Initialize parent
        Parent::initialize(this_component,
                           std::forward<CosseratInitializer>(initializer));

        // 2. Initialize required variables
        initialize_component<InitializedVariables>(
            this_component.self(),
            std::forward<CosseratInitializer>(initializer));

        // 3. Compute other variables
        {
          auto&& reference_lengths(
              blocks::get<::elastica::tags::ReferenceElementLength>(
                  this_component.self()));
          auto&& semi_major_axis(
              blocks::get<::fish::tags::SemiMajorAxis>(this_component.self()));
          auto&& semi_minor_axis(
              blocks::get<::fish::tags::SemiMinorAxis>(this_component.self()));

          const real_t base_length = ::blaze::sum(reference_lengths);
          const index_t n_elements = reference_lengths.size();

          const real_t s_b{0.04 * base_length};
          const real_t s_t{0.95 * base_length};
          const real_t w_h{0.04 * base_length};
          const real_t w_t{0.01 * base_length};
          const real_t a{0.51 * base_length};
          const real_t b{0.08 * base_length};

          const real_t dx{1.0 / n_elements};

          for (index_t i = 0; i < n_elements; ++i) {
            real_t s = (static_cast<real_t>(i) + 0.5) * dx;
            if (s >= 0 && s <= s_b) {
              semi_minor_axis[i] = std::sqrt(2 * w_h * s - s * s);
            } else if (s_b < s && s <= s_t) {
              semi_minor_axis[i] =
                  w_h - (w_h - w_t) * std::pow((s - s_b) / (s_t - s_b), 2);
            } else if (s_t < s && s <= base_length) {
              semi_minor_axis[i] =
                  w_t * (base_length - s) / (base_length - s_t);
            } else {
              semi_minor_axis[i] = 0.0;
            }

            semi_major_axis[i] = b * std::sqrt(1 - std::pow((s - a) / a, 2));
          }

          auto&& element_dimensions(
              blocks::get<::elastica::tags::ElementDimension>(
                  this_component.self()));
          auto&& volumes(blocks::get<::elastica::tags::ElementVolume>(
              this_component.self()));
          volumes =
              M_PI * semi_major_axis * semi_minor_axis * reference_lengths;
          element_dimensions = ::elastica::sqrt(volumes / reference_lengths / M_PI);
        }
      }

     public:
      using CRTP::self;

      // Get methods: SemiMajorAxis
      inline constexpr decltype(auto) get_semi_major_axis() & noexcept {
        return blocks::get<::fish::tags::SemiMajorAxis>(self());
      }
      inline constexpr decltype(auto) get_semi_major_axis() const& noexcept {
        return blocks::get<::fish::tags::SemiMajorAxis>(self());
      }
      inline constexpr decltype(auto) get_semi_major_axis(
          index_t idx) & noexcept {
        return SemiMajorAxis::slice(get_semi_major_axis(), idx);
      }
      inline constexpr decltype(auto) get_semi_major_axis(
          index_t idx) const& noexcept {
        return SemiMajorAxis::slice(get_semi_major_axis(), idx);
      }

      // Get methods: SemiMinorAxis
      inline constexpr decltype(auto) get_semi_minor_axis() & noexcept {
        return blocks::get<::fish::tags::SemiMinorAxis>(self());
      }
      inline constexpr decltype(auto) get_semi_minor_axis() const& noexcept {
        return blocks::get<::fish::tags::SemiMinorAxis>(self());
      }
      inline constexpr decltype(auto) get_semi_minor_axis(
          index_t idx) & noexcept {
        return SemiMinorAxis::slice(get_semi_minor_axis(), idx);
      }
      inline constexpr decltype(auto) get_semi_minor_axis(
          index_t idx) const& noexcept {
        return SemiMinorAxis::slice(get_semi_minor_axis(), idx);
      }

      // Get methods: Area
      inline constexpr auto get_area() const& COSSERATROD_LIB_NOEXCEPT {
        return M_PI * get_semi_major_axis() * get_semi_minor_axis();
      }
      inline constexpr auto get_area(
          index_t idx) const& COSSERATROD_LIB_NOEXCEPT {
        return M_PI * get_semi_major_axis(idx) * get_semi_minor_axis(idx);
      }

      // Get methods: Second Moment of Area
      template <DirectionType Dim, Requires<(Dim == Frame::D1)> = nullptr>
      inline constexpr decltype(auto) get_second_moment_of_area(
          index_t idx) const COSSERATROD_LIB_NOEXCEPT {
        // 0.25 * pi * w^3 * h
        return 0.25 * M_PI * get_semi_major_axis(idx) *
               std::pow(get_semi_minor_axis(idx), 3);
      }
      template <DirectionType Dim, Requires<(Dim == Frame::D2)> = nullptr>
      inline constexpr decltype(auto) get_second_moment_of_area(
          index_t idx) const COSSERATROD_LIB_NOEXCEPT {
        // 0.25 * pi * w * h^3
        return 0.25 * M_PI * get_semi_minor_axis(idx) *
               std::pow(get_semi_major_axis(idx), 3);
      }
      template <DirectionType Dim, Requires<(Dim == Frame::D3)> = nullptr>
      inline constexpr decltype(auto) get_second_moment_of_area(
          index_t idx) const COSSERATROD_LIB_NOEXCEPT {
        // 0.25 * pi * w * h * (w^2 + h^2)
        return This::get_second_moment_of_area<Frame::D1>(idx) +
               This::get_second_moment_of_area<Frame::D2>(idx);
      }
      inline constexpr auto get_second_moment_of_area(index_t idx) const
          COSSERATROD_LIB_NOEXCEPT {
        return ::elastica::Vec3{
            real_t(This::get_second_moment_of_area<Frame::D1>(idx)),
            real_t(This::get_second_moment_of_area<Frame::D2>(idx)),
            real_t(This::get_second_moment_of_area<Frame::D3>(idx))};
      }

      static inline constexpr auto shape_factor() COSSERATROD_LIB_NOEXCEPT
          -> real_t {
        return 27.0 / 28.0;
      }
    };

  }  // namespace component

}  // namespace fish
