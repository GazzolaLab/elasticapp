//
// Created by Tejaswin Parthasarathy on 3/23/22.
//

#pragma once

#include <iterator>
#include <limits>

namespace elastica {

  template <typename IntType>
  class CountingIterator {
    static_assert(
        std::numeric_limits<IntType>::is_integer,
        "Cannot instantiate CountingIterator with a non-integer type");

   public:
    using difference_type = typename std::make_signed<IntType>::type;
    using value_type = IntType;
    using pointer = const IntType*;
    using reference = const IntType&;
    using iterator_category =
        std::random_access_iterator_tag;  // interop with tbb

    CountingIterator() : my_counter() {}
    explicit CountingIterator(IntType init) : my_counter(init) {}

    auto operator*() const -> reference { return my_counter; }
    auto operator[](difference_type i) const -> value_type {
      return *(*this + i);
    }

    auto operator-(CountingIterator const& it) const -> difference_type {
      return static_cast<difference_type>(my_counter) -
             static_cast<difference_type>(it.my_counter);
    }

    auto operator+=(difference_type forward) -> CountingIterator& {
      my_counter += forward;
      return *this;
    }
    auto operator-=(difference_type backward) -> CountingIterator& {
      return *this += -backward;
    }
    auto operator++() -> CountingIterator& { return *this += 1; }
    auto operator--() -> CountingIterator& { return *this -= 1; }

    auto operator++(int) -> CountingIterator const {
      CountingIterator it(*this);
      ++(*this);
      return it;
    }
    auto operator--(int) -> CountingIterator const {
      CountingIterator it(*this);
      --(*this);
      return it;
    }

    auto operator-(difference_type backward) const -> CountingIterator {
      return CountingIterator(my_counter - backward);
    }
    auto operator+(difference_type forward) const -> CountingIterator {
      return CountingIterator(my_counter + forward);
    }
    friend auto operator+(difference_type forward, CountingIterator const it)
        -> CountingIterator {
      return it + forward;
    }

    auto operator==(CountingIterator const& it) const -> bool {
      return *this - it == 0;
    }
    auto operator!=(CountingIterator const& it) const -> bool {
      return !(*this == it);
    }
    auto operator<(CountingIterator const& it) const -> bool {
      return *this - it < 0;
    }
    auto operator>(CountingIterator const& it) const -> bool {
      return it < *this;
    }
    auto operator<=(CountingIterator const& it) const -> bool {
      return !(*this > it);
    }
    auto operator>=(CountingIterator const& it) const -> bool {
      return !(*this < it);
    }

   private:
    IntType my_counter;
  };

  template <class IntType, typename result_t = CountingIterator<IntType>>
  inline auto make_counting_iterator(IntType x) -> result_t {
    return result_t{std::move(x)};
  }

}  // namespace elastica
