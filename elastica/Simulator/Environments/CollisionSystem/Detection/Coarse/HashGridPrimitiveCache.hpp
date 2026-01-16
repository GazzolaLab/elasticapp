//
// Created by Tejaswin Parthasarathy on 7/28/21.
//

#pragma once

#include <cstddef>
#include <tuple>

#include "Utilities/Size.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace collision {

    namespace detection {

      namespace coarse {

        namespace detail {

          // TODO : refactor and use Offsets from Simulator/Utilities.hpp
          // more easier than having an array
          template <typename P>
          struct Size {
            using size_type = std::size_t;

            Size() : size_(0UL) {}
            Size(size_type s) : size_(s){};

            inline constexpr auto value() & noexcept -> size_type& {
              return size_;
            }
            inline constexpr auto value() const& noexcept -> size_type const& {
              return size_;
            }
            explicit operator size_type() const noexcept { return value(); }
            size_type size_;
          };

        }  // namespace detail

        template <typename AdmissiblePrimitives>
        struct CounterCache {
          template <typename P>
          struct AddSize {
            using type = detail::Size<P>;
          };
          template <typename P>
          static constexpr auto size_type() -> typename AddSize<P>::type;
          using Sizes = tmpl::as_tuple<
              tmpl::transform<AdmissiblePrimitives, AddSize<tmpl::_1>>>;

          CounterCache() = default;
          ~CounterCache() = default;

          template <typename Primitive>
          inline constexpr decltype(auto) get() & noexcept {
            return std::get<decltype(size_type<Primitive>())>(data()).value();
          }
          template <typename Primitive>
          inline constexpr decltype(auto) get() const& noexcept {
            return std::get<decltype(size_type<Primitive>())>(data()).value();
          }

          //**Data access*******************************************************
          /*!\name Data access */
          //@{

          //********************************************************************
          /*!\brief Access to the underlying data
           *
           * \return Mutable lvalue reference to the underlying data
           */
          inline constexpr Sizes& data() & noexcept { return sizes_; }
          //********************************************************************

          //********************************************************************
          /*!\brief Access to the underlying data
           *
           * \return Constant lvalue reference to the underlying data
           */
          inline constexpr Sizes const& data() const& noexcept {
            return sizes_;
          }
          //********************************************************************

          //@}
          //********************************************************************

          Sizes sizes_;
        };

        template <typename Primitive, typename Primitives>
        inline constexpr decltype(auto) get(
            CounterCache<Primitives>& counter_cache) noexcept {
          return counter_cache.template get<Primitive>();
        }
        template <typename Primitive, typename Primitives>
        inline constexpr decltype(auto) get(
            CounterCache<Primitives> const& counter_cache) noexcept {
          return counter_cache.template get<Primitive>();
        }

        template <typename AdmissiblePrimitives>
        struct Cache;

        template <typename Primitive, typename Primitives>
        inline constexpr decltype(auto) get(Cache<Primitives>& cache) noexcept;
        template <typename Primitive, typename Primitives>
        inline constexpr decltype(auto) get(
            Cache<Primitives> const& cache) noexcept;

        template <typename AdmissiblePrimitives>
        struct Cache {
          // template <typename P>
          // struct AddSize {
          //   using type = detail::Size<P>;
          // };
          template <typename P>
          struct AddStorage {
            using type = std::vector<P*>;
          };
          // using Sizes = tmpl::as_tuple<
          //     tmpl::transform<AdmissiblePrimitives, AddSize<tmpl::_1>>>;
          using PrimitiveBuffers = tmpl::as_tuple<
              tmpl::transform<AdmissiblePrimitives, AddStorage<tmpl::_1>>>;

          Cache() = default;
          ~Cache() = default;

          //          Cache() {
          //            // always initialize at least one so that when you
          //            destroy you
          //            // do the right thing
          //
          //            // tmpl::for_each<AdmissiblePrimitives>([this](auto p){
          //            //   using Primitive = tmpl::type_from<decltype(p)>;
          //            //   this->reserve<Primitive>(1UL);
          //            // });
          //          }

          //          ~Cache() {
          //            std::apply(
          //                [](auto&... pp) {
          //                  // pp is Primitive**&
          //                  EXPAND_PACK_LEFT_TO_RIGHT(delete[] pp);
          //                },
          //                buffers_);
          //          }

          template <typename P>
          static constexpr auto storage_t() -> typename AddStorage<P>::type;

          template <typename Primitive>
          inline constexpr decltype(auto) get() & noexcept {
            return std::get<decltype(storage_t<Primitive>())>(data());
          }
          template <typename Primitive>
          inline constexpr decltype(auto) get() const& noexcept {
            return std::get<decltype(storage_t<Primitive>())>(data());
          }

          template <typename P>
          inline auto size() const -> std::size_t {
            return ::elastica::size(get<P>());
          }

          //          template <typename P>
          //          void reserve(std::size_t size) {
          //            const auto old_size =
          //                std::get<decltype(size_type<P>())>(std::as_const(sizes_))
          //                    .value();
          //
          //            if (size <= old_size) {
          //              return;
          //            } else {
          //              using ::elastica::collision::detection::coarse::get;
          //
          //              // invalidate old and allocate new
          //              delete[] get<P>(*this);
          //
          //              get<P>(*this) = new P*[size];
          //              std::get<decltype(size_type<P>())>(sizes_).value() =
          //              size;
          //            }
          //          }

         public:
          //**Data access*******************************************************
          /*!\name Data access */
          //@{

          //********************************************************************
          /*!\brief Access to the underlying data
           *
           * \return Mutable lvalue reference to the underlying data
           */
          inline constexpr PrimitiveBuffers& data() & noexcept {
            return buffers_;
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Access to the underlying data
           *
           * \return Constant lvalue reference to the underlying data
           */
          inline constexpr PrimitiveBuffers const& data() const& noexcept {
            return buffers_;
          }
          //********************************************************************

          //@}
          //********************************************************************

          // PrimitiveType** all_primitives_from_current_grid =
          //     new PrimitiveType*[current_primitive_count];
          PrimitiveBuffers buffers_;
        };

        template <typename Primitive, typename Primitives>
        inline constexpr decltype(auto) get(Cache<Primitives>& cache) noexcept {
          return cache.template get<Primitive>();
        }
        template <typename Primitive, typename Primitives>
        inline constexpr decltype(auto) get(
            Cache<Primitives> const& cache) noexcept {
          return cache.template get<Primitive>();
        }

      }  // namespace coarse

    }  // namespace detection

  }  // namespace collision

}  // namespace elastica