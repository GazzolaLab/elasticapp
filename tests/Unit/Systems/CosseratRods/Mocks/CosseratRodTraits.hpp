//
// Created by Tejaswin Parthasarathy on 4/20/21.
//

#pragma once

#include <functional>  // ref_wrapper

#include "ErrorHandling/Assert.hpp"
#include "Systems/Block/Block.hpp"
#include "Systems/CosseratRods/Traits/DataOpsTraits.hpp"
#include "Systems/CosseratRods/Traits/PlacementTrait.hpp"
#include "Systems/CosseratRods/Traits/Protocols.hpp"
#include "Utilities/NonCreatable.hpp"

// because Vec3 is a blaze vector we need to include constructors for it
#include <blaze/math/expressions/DVecScalarMultExpr.h>
#include <vector>

// Serializtaino suppot
#include "Simulator/IO/Serialization.hpp"

namespace mock {

  template <typename T>
  struct ScalarType {
    using type = T;
    using slice_type = T&;
    using const_slice_type = T const&;
    using ghost_type = T;

    // for initialization
    static inline slice_type slice(type& data, std::size_t /*index*/) {
      return data;
    }
    static inline const_slice_type slice(type const& data,
                                         std::size_t /*index*/) {
      return data;
    }
    static inline void diagonal_assign(type const&, std::size_t, type const&) {}
  };

  template <std::size_t>
  struct PlacementMarker {
    inline static auto get_dofs(std::size_t n_elems) noexcept -> std::size_t {
      ELASTICA_ASSERT(n_elems > 0UL, "prescribed number of elements incorrect");
      // the actual return value here doesn't matter as it does not get used
      return 1UL;
    }
    inline static auto n_ghosts() noexcept -> std::size_t { return 1UL; }
  };

  struct CustomDouble {
    // constexpr here for literal types like pi
    constexpr CustomDouble() : d_(0.0){};
    // explicit not marked because some operations return a real type
    constexpr /*explicit*/ CustomDouble(double d) : d_(d){};
    constexpr CustomDouble(CustomDouble const& other) : d_(other.d_){};
    constexpr CustomDouble& operator=(CustomDouble other) {
      d_ = other.d_;
      return *this;
    };

    ~CustomDouble() = default;
    // Overloaded assignment
    // no self-assignment
    CustomDouble& operator=(double d) {
      // do the copy
      d_ = d;
      // return the existing object so we can chain this operator
      return *this;
    }

    // overload for blaze expression types
    template <typename VT, typename ST, bool TF>
    CustomDouble& operator=(blaze::DVecScalarMultExpr<VT, ST, TF> const& expr) {
      // do the copy
      d_ = expr[0];
      // return the existing object so we can chain this operator
      return *this;
    }

    /* explicit*/ operator double() const { return d_; }

    double d_;
  };

  // need to provide equality operator for comparison
  inline auto operator==(CustomDouble const& lhs,
                         CustomDouble const& rhs) noexcept -> bool {
    return static_cast<double>(lhs) == static_cast<double>(rhs);
  }

}  // namespace mock

namespace elastica {

  namespace io {

    // serialize a double
    template <>
    struct Serialize<::mock::CustomDouble> {
      using type = ::mock::CustomDouble;

      // 1. check if blocking is the same, else abort
      template <typename InArchive>
      static inline void load(InArchive& ar, type& val) {
        ar(val.d_);
      }

      template <typename InArchive>
      static inline void save(InArchive& ar, type const& val) {
        ar(val.d_);
      }
    };

  }  // namespace io

}  // namespace elastica

namespace mock {

  // Test only data, do not test placement, etc
  struct BasicCosseratRodTraits : private ::elastica::NonCreatable {
    using real_type = CustomDouble;
    using index_type = std::size_t;
    using size_type = std::size_t;

    struct DataType {
      using Scalar = ScalarType<real_type>;
      using Vector = ScalarType<real_type>;
      using Matrix = ScalarType<real_type>;
      using Index = ScalarType<index_type>;
    };

    struct Place {
      // to prevent  error: multiple overloads of 'n_ghosts' instantiate to the
      // same signature 'auto
      // (elastica::cosserat_rod::CosseratRodPlugin<mock::BasicCosseratRodTraits,
      // DummyBlock, WithCircularCosseratRod>::OnNode) noexcept ->
      // elastica::cosserat_rod::CosseratRodPlugin<mock::BasicCosseratRodTraits,
      // DummyBlock, WithCircularCosseratRod>::size_type' (aka 'auto
      // (mock::PlacementMarker) noexcept -> unsigned long')
      using OnNode = PlacementMarker<0UL>;
      using OnElement = PlacementMarker<1UL>;
      using OnVoronoi = PlacementMarker<2UL>;
      using OnRod = PlacementMarker<3UL>;
    };

    //    struct Operations {
    //      template<typename A>
    //      static auto voronoi_length(A& a){return 1UL;}
    //      template<typename A, typename B>
    //      static void quadrature_kernel(A, B){}
    //      template<typename A, typename B>
    //      static void subtract_quadrature(A, B){}
    //      template<typename A, typename B>
    //      static void difference_kernel(A, B){}
    //      template<typename B>
    //      static auto difference_kernel(B){return 1UL;}
    //      template<typename A, typename B, typename C>
    //      static void batch_matvec(A, B, C){}
    //      template<typename A>
    //      static auto batch_norm(A){return 1UL;}
    //      template<typename A, typename B>
    //      static void batch_inv_rotate(A, B){}
    //      template<typename A, typename B, typename C>
    //      static void batch_inv_rotate_and_divide(A, B, C){}
    //      template<typename A>
    //      static void batch_subtract_z_unit(A){}
    //    };

    // declare variables
    template <typename ParameterTag, typename RankTag, typename StaggerTag,
              typename... Tags>
    struct CosseratRodVariable
        : public ::blocks::Variable<ParameterTag, RankTag, StaggerTag,
                                    Tags...> {
      using Stagger = StaggerTag;
    };
    template <typename ParameterTag, typename RankTag, typename StaggerTag,
              typename... Tags>
    struct CosseratRodInitializedVariable
        : public ::blocks::InitializedVariable<ParameterTag, RankTag,
                                               StaggerTag, Tags...> {
      using Stagger = StaggerTag;
    };
  };

}  // namespace mock

namespace mock {

  template <typename T>
  class custom_reference_wrapper : public std::reference_wrapper<T> {
   private:
    using Parent = std::reference_wrapper<T>;
    using typename Parent::type;

   public:
    using Parent::Parent;
    //     assignment
    custom_reference_wrapper& operator=(const T& x) noexcept {
      this->get() = x;
      return *this;
    };
  };

  /// [vector_datatrait]
  template <typename T>
  struct VectorType
      : public ::tt::ConformsTo<elastica::cosserat_rod::protocols::DataTrait> {
    using type = std::vector<T>;
    using slice_type = std::vector<std::reference_wrapper<T>>;
    using const_slice_type = const std::vector<std::reference_wrapper<const T>>;
    using reference_type = T&;
    using const_reference_type = T const&;
    using reference_slice_type = T&;
    using const_reference_slice_type = T const&;
    using ghost_type = T;

    // no params
    static inline auto ghost_value() -> ghost_type {
      return static_cast<ghost_type>(3.14159);
    }

    // for initialization
    static inline reference_type slice(type& data, std::size_t index) {
      return data[index];
    }
    static inline const_reference_type slice(type const& data,
                                             std::size_t index) {
      return data[index];
    }

    // for slicing a slice in initializer
    static inline reference_slice_type slice(slice_type& slice,
                                             std::size_t index) {
      return slice[index];
    }
    // sometimes you need this overload (for example in check()) where the
    // context is const
    static inline const_reference_slice_type slice(slice_type const& slice,
                                                   std::size_t index) {
      return slice[index];
    }

    static inline const_reference_slice_type slice(
        const_slice_type const& slice, std::size_t index) {
      return slice[index];
    }

    static inline void resize(type& data, std::size_t new_dofs) {
      ELASTICA_ASSERT(data.size() <= new_dofs,
                      "Contract violation, block shrinks");
      return data.resize(new_dofs);
    }

    // for slicing whole blocks together
    static inline slice_type slice(type& data, std::size_t start,
                                   std::size_t size) {
      return slice_type(&data[start], &data[start + size]);
    }
    static inline const_slice_type slice(type const& data, std::size_t start,
                                         std::size_t size) {
      return const_slice_type(&data[start], &data[start + size]);
    }
  };
  /// [vector_datatrait]

  struct NodePlacementMarker
      : public ::elastica::cosserat_rod::placement_tags::OnNode {};
  struct ElementPlacementMarker
      : public ::elastica::cosserat_rod::placement_tags::OnElement {};
  struct VoronoiPlacementMarker
      : public ::elastica::cosserat_rod::placement_tags::OnVoronoi {};
  struct RodPlacementMarker
      : public ::elastica::cosserat_rod::placement_tags::OnRod {};

  //  struct NodePlacementMarker
  //      : /* public NonCreatable*/ public ::tt::ConformsTo<
  //            elastica::cosserat_rod::protocols::PlacementTrait> {
  //    inline static auto get_dofs(std::size_t n_elems) -> std::size_t {
  //      ELASTICA_ASSERT(n_elems > 0UL, "prescribed number of elements
  //      incorrect");
  //      // the actual return value here doesn't matter as it does not get used
  //      return n_elems + 1UL;
  //    }
  //  };
  //
  //  struct ElementPlacementMarker
  //      : /* public NonCreatable*/ public ::tt::ConformsTo<
  //            elastica::cosserat_rod::protocols::PlacementTrait> {
  //    inline static auto get_dofs(std::size_t n_elems) -> std::size_t {
  //      ELASTICA_ASSERT(n_elems > 0UL, "prescribed number of elements
  //      incorrect");
  //      // the actual return value here doesn't matter as it does not get used
  //      return n_elems;
  //    }
  //  };
  //
  //  struct VoronoiPlacementMarker
  //      : /* public NonCreatable*/ public ::tt::ConformsTo<
  //            elastica::cosserat_rod::protocols::PlacementTrait> {
  //    inline static auto get_dofs(std::size_t n_elems) -> std::size_t {
  //      ELASTICA_ASSERT(n_elems > 0UL, "prescribed number of elements
  //      incorrect");
  //      // the actual return value here doesn't matter as it does not get used
  //      return n_elems - 1UL;
  //    }
  //  };
  //
  //  struct RodPlacementMarker
  //      : /* public NonCreatable*/ public ::tt::ConformsTo<
  //            elastica::cosserat_rod::protocols::PlacementTrait> {
  //    inline static auto get_dofs(std::size_t) noexcept -> std::size_t {
  //      // ELASTICA_ASSERT(n_elems > 0UL, "prescribed number of elements
  //      // incorrect"); the actual return value here doesn't matter as it does
  //      not
  //      // get used
  //      return 1UL;
  //    }
  //  };

  // Test only data, do not test placement, etc
  struct MinimalCosseratRodTraits : private ::elastica::NonCreatable {
    using real_type = CustomDouble;
    using index_type = std::size_t;
    using size_type = std::size_t;

    struct DataType {
      using Scalar = VectorType<real_type>;
      using Vector = VectorType<real_type>;
      using Matrix = VectorType<real_type>;
      using Index = VectorType<index_type>;
    };

    struct Place {
      using OnNode = NodePlacementMarker;
      using OnElement = ElementPlacementMarker;
      using OnVoronoi = VoronoiPlacementMarker;
      using OnRod = RodPlacementMarker;
    };

    template <typename Var>
    struct IsOnNode : std::is_base_of<NodePlacementMarker, Var> {};
    template <typename Var>
    struct IsOnElement : std::is_base_of<ElementPlacementMarker, Var> {};
    template <typename Var>
    struct IsOnVoronoi : std::is_base_of<VoronoiPlacementMarker, Var> {};
    template <typename Var>
    struct IsOnRod : std::is_base_of<RodPlacementMarker, Var> {};

    // declare variables
    template <typename ParameterTag, typename RankTag, typename StaggerTag,
              typename... Tags>
    struct CosseratRodVariable
        : public ::blocks::Variable<ParameterTag, RankTag, StaggerTag,
                                    Tags...> {
      using Stagger = StaggerTag;
    };
    template <typename ParameterTag, typename RankTag, typename StaggerTag,
              typename... Tags>
    struct CosseratRodInitializedVariable
        : public ::blocks::InitializedVariable<ParameterTag, RankTag,
                                               StaggerTag, Tags...> {
      using Stagger = StaggerTag;
    };
  };

}  // namespace mock