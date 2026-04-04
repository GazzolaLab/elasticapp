//==============================================================================
/*!
//  \file
//  \brief A set partition algorithm implementing Karmarkarp-Karp differencing
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <list>
#include <memory>
#include <ostream>
#include <vector>

#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/NonCopyable.hpp"

// namespace elastica {
//
//   struct TwoWayKarmarkarKarpPartitioner {
//     template <class BidirIt, class UnaryOperation>
//     BidirIt operator()(BidirIt first, BidirIt last, UnaryOperation&& op)
//     const {
//       using value_type = typename std::iterator_traits<BidirIt>::value_type;
//       using compare_type =
//           decltype(std::declval<UnaryOperation>()(std::declval<value_type>()));
//       //      using compare = std::greater<compare_type>;
//
//       struct compare_helper {
//         std::vector<compare_helper*> childs;
//         compare_type value;
//         // const
//         const compare_type initial_value;
//         const BidirIt it;
//
//         compare_helper(BidirIt iter, compare_type c)
//             : value(c), childs{}, initial_value(value), it(iter) {}
//       };
//
//       auto comparator = [compare = std::less<compare_type>{}](
//                             compare_helper const& first,
//                             compare_helper const& second) {
//         // can be made into a std::equal object
//         return (first.value == second.value)
//                    ? compare(first.initial_value, second.initial_value)
//                    : compare(first.value, second.value);
//       };
//       ignore_unused(comparator);
//
//       using Heap =
//           std::priority_queue<compare_helper, std::vector<compare_helper>,
//                               decltype(comparator)>;
//       Heap heap;
//       std::vector<compare_helper> processed;
//
//       for (auto it = first; it != last; ++it)
//         heap.emplace(it, op(*it));
//
//       // reserve (n) where n is the number of elements
//       auto n_elems = std::distance(first, last);
//       processed.reserve(n_elems);
//
//       auto collect = [&heap, &processed]() {
//         compare_helper better = heap.top();
//         heap.pop();
//         processed.push_back(heap.top());
//         heap.pop();
//         compare_helper& worser = processed.back();
//         better.value -= worser.value;
//         better.childs.push_back(&worser);
//         heap.push(better);
//       };
//       // runs (n) times
//       //        processed.push_back(heap.top());
//       //        compare_helper& better = processed.back();
//       // prority queue doesn't have an API to steal
//       //        compare_helper& worser = processed.back();
//       //        heap.pop();
//       //        heap.emplace(&better, &worser, nullptr, better.c - worser.c);
//       while (heap.size() > 1) {
//         collect();
//       }
//
//       // there is now only the difference element in the heap
//       // we do backtracking by traversing the tree
//
//       std::vector<compare_helper> first_subset;
//       std::vector<compare_helper> second_subset;
//       first_subset.reserve(n_elems / 2);
//       second_subset.reserve(n_elems / 2);
//
//       auto traverse = [&first_subset, &second_subset](compare_helper const&
//       c,
//                                                       int color = 0) {
//         if (color % 2) {
//           // odd levels
//
//         } else {
//           // even levels, add to another subset
//         }
//       };
//     }
//   };
//
// }  // namespace elastica

namespace elastica {

  namespace detail {

    template <typename T, typename = typename std::is_integral<T>::type>
    struct NumberLike;

    template <typename T>
    struct NumberLike<T, std::true_type> {
      using SumType = T;
      NumberLike(SumType const /*&t*/, SumType const sum) : cardinality(sum) {}
      auto value() const -> SumType { return cardinality; }
      auto data() && -> SumType { return cardinality; }
      SumType cardinality;
    };

    template <typename T>
    struct NumberLike<T, std::false_type> {
      using SumType = std::size_t;
      using Data = std::reference_wrapper<T const>;

      NumberLike(T const& v, SumType const sum) : set(v), cardinality(sum) {}
      auto value() const -> SumType { return cardinality; }
      auto data() && -> Data { return set; }

      Data set;
      SumType cardinality;
    };

    template <typename T>
    std::ostream& operator<<(std::ostream& stream,
                             const NumberLike<T>& number) {
      return stream << number.value();
      /*
          if (!number.set.get().empty()) {
          auto it{number.set.get().begin()};
          stream << *it;
          for (++it; it != number.set.get().end(); ++it) {
            stream << ',' << *it;
          }
        }
        return stream;
      */
    }

    template <typename Number>
    class Subset : public NonCopyable {
     public:
      using Numbers = std::list<Number>;
      using SumType = typename Number::SumType;

      Subset() : numbers_{}, sum_{} {}
      explicit Subset(const Number number)
          : numbers_{number}, sum_{number.value()} {}

      Subset(Subset&&) noexcept = default;
      Subset& operator=(Subset&&) noexcept = default;

      // inline auto numbers() const& -> const Numbers& { return numbers_; }
      inline auto numbers() && noexcept -> Numbers&& {
        return static_cast<Numbers&&>(numbers_);
      }

      auto sum() const -> SumType { return sum_; }

      void merge(Subset other) {
        numbers_.splice(numbers_.end(), std::move(other).numbers());
        sum_ += other.sum_;
      }

     private:
      Numbers numbers_;
      SumType sum_;
    };

    template <typename Number>
    std::ostream& operator<<(std::ostream& stream,
                             const Subset<Number>& subset) {
      stream << '[';
      if (!subset.numbers().empty()) {
        auto it{subset.numbers().begin()};
        stream << *it;
        for (++it; it != subset.numbers().end(); ++it) {
          stream << ',' << *it;
        }
      }
      stream << ']';
      return stream;
    }

    template <typename Number>
    struct SubsetSumGreater {
      bool operator()(const Subset<Number>& a, const Subset<Number>& b) const {
        return a.sum() > b.sum();
      }
    };

    template <typename Number>
    class Partition : public NonCopyable {
     public:
      using Subset_ = Subset<Number>;
      using SumType = typename Subset_::SumType;
      using Subsets = std::vector<Subset_>;

      Partition(Number number, const std::size_t k) : subsets_(k) {
        subsets_.front().merge(Subset_(std::move(number)));
      }

      Partition(Partition&&) noexcept = default;
      Partition& operator=(Partition&&) noexcept = default;

      auto subsets() && -> Subsets&& { return std::move(subsets_); }
      auto subsets() const& -> Subsets const& { return subsets_; }
      // auto subset(std::size_t idx) -> Subset_& { return subsets_[idx]; }
      auto subset(std::size_t idx) const& -> Subset_ const& {
        return subsets_[idx];
      }
      auto subset(std::size_t idx) && -> Subset_&& {
        return std::move(subsets_[idx]);
      }

      SumType difference() const {
        return subsets_.front().sum() - subsets_.back().sum();
      }

      void merge(Partition other) {
        assert(subsets_.size() == other.subsets_.size());
        auto it{subsets_.begin()};
        auto other_it{other.subsets_.rbegin()};
        while (it != subsets_.end() && other_it != other.subsets_.rend()) {
          it->merge(std::move(*other_it));
          ++it;
          ++other_it;
        }
        std::sort(subsets_.begin(), subsets_.end(), SubsetSumGreater<Number>{});
      }

     private:
      Subsets subsets_;
    };

    template <typename Number>
    std::ostream& operator<<(std::ostream& stream,
                             const Partition<Number>& partition) {
      stream << '[';
      if (!partition.subsets().empty()) {
        auto it{partition.subsets().begin()};
        stream << *it;
        for (++it; it != partition.subsets().end(); ++it) {
          stream << ',' << *it;
        }
      }
      stream << ']';
      return stream;
    }

    template <typename Number>
    struct DifferenceLess {
      bool operator()(const Partition<Number>& a,
                      const Partition<Number>& b) const {
        return a.difference() < b.difference();
      }
    };

    template <typename T>
    struct KarmarkarKarpResult {
      using Number = NumberLike<T>;
      using P = Partition<Number>;
      using SumType = typename P::SumType;
      using Sums = std::vector<SumType>;
      using Subset_ = std::vector<T>;
      using Subsets = std::vector<Subset_>;

      explicit KarmarkarKarpResult(P&& partition)
          : n_subsets_(partition.subsets().size()),
            subsets_(n_subsets_),
            sums_(n_subsets_) {
        for (std::size_t i_subset = 0UL; i_subset < n_subsets_; ++i_subset) {
          // no point in moving since this is a single number
          sums_[i_subset] = partition.subset(i_subset).sum();
          auto&& l(std::move(partition).subset(i_subset).numbers());
          subsets_[i_subset].reserve(l.size());

          // makes a copy
          std::transform(
              std::make_move_iterator(std::begin(l)),
              std::make_move_iterator(std::end(l)),
              std::back_inserter(subsets_[i_subset]),
              [](Number&& n) { return std::forward<Number>(n).data(); });
        }
      }

      auto n_subsets() const -> std::size_t { return n_subsets_; }
      auto subsets() & -> Subsets& { return subsets_; }
      auto subsets() const& -> Subsets const& { return subsets_; }
      auto subsets() && -> Subsets&& {
        return static_cast<Subsets&&>(subsets_);
      }
      auto subset(std::size_t idx) -> Subset_& { return subsets_[idx]; }
      auto subset(std::size_t idx) const -> Subset_ const& {
        return subsets_[idx];
      }

     private:
      std::size_t n_subsets_;
      Subsets subsets_;
      Sums sums_;
    };

    // only needs iterable
    // inspired from https://stackoverflow.com/a/68425000
    // see alos
    // https://stackoverflow.com/questions/32354215/better-results-in-set-partition-than-by-differencing
    template <typename T, typename Op>
    auto KarmarkarKarp(std::vector<T> const& inputs, const std::size_t k, Op op)
        -> KarmarkarKarpResult<T> {
      assert(!inputs.empty());

      using Number = NumberLike<T>;
      using P = Partition<Number>;

      std::vector<P> heap;
      heap.reserve(inputs.size());

      for (T const& input : inputs) {
        // keeps a ref inside
        heap.emplace_back(Number(input, op(input)), k);
      }

      constexpr DifferenceLess<Number> less{};
      std::make_heap(heap.begin(), heap.end(), less);
      while (heap.size() > 1) {
        std::pop_heap(heap.begin(), heap.end(), less);
        P partition{std::move(heap.back())};
        heap.pop_back();
        std::pop_heap(heap.begin(), heap.end(), less);
        heap.back().merge(std::move(partition));
        std::push_heap(heap.begin(), heap.end(), less);
      }
      using RT = KarmarkarKarpResult<T>;
      return RT{std::move(heap.front())};
    }

  }  // namespace detail

  //****************************************************************************
  /*!\brief Performs set differencing via the Karmarkar-Karp partitioning
   * algorithm for subset sums, using a custom access operator
   * \ingroup utils
   *
   * \details
   * Given a collection of items, perform set differencing upon them using the
   * result of evaluating `op` on each item as a key and return a collection of
   * item collections, each containing a partitioned set that all have
   * approximately the same sum of `op(item)`.
   *
   * \semantics
   * The key of each item, accessed by `op(item)`, should be logically
   * comparable (i.e. the type `Op(T)` should have a well defined semantics for
   * logical comparison operators <, >, ==, !=).
   *
   * \example
   * With the setup shown below for the input items
   * \snippet Test_KarmarkarKarpPartition.cpp kk_lambda_setup
   * the following results in a partioned set
   * \snippet Test_KarmarkarKarpPartition.cpp kk_lambda_eg
   *
   * \param inputs The input collection to be partitioned
   * \param n_partitions The number of requested partitions
   * \param op Unary callable returning the numeric key used for each item
   * \return Collection of input collections that are partitioned
   */
  template <typename T, typename Op>
  inline decltype(auto) karmarkar_karp_partition(std::vector<T> const& inputs,
                                                 std::size_t const n_partitions,
                                                 Op op) {
    return detail::KarmarkarKarp(inputs, n_partitions, std::move(op));
  }
  //****************************************************************************

  //****************************************************************************
  /*!\brief Performs set differencing via the Karmarkar-Karp partitioning
   * algorithm for subset sums
   * \ingroup utils
   *
   * \details
   * Given a collection of items of type `T`, perform set differencing upon them
   * using the item as a key and return a collection of item collections, each
   * containing a partitioned set that all have approximately the same sum.
   *
   * \semantics
   * Each item should be logically comparable (i.e. the type `T` should have a
   * well defined semantics for logical comparison operators <, >, ==, !=).
   *
   * \example
   * \snippet Test_KarmarkarKarpPartition.cpp kk_example
   *
   * \param items The input item collection to be partitioned
   * \param n_partitions The number of requested partitions
   * \return Collection of input collections that are partitioned
   */
  template <typename T>
  inline decltype(auto) karmarkar_karp_partition(
      std::vector<T> const& items, std::size_t const n_partitions) {
    return karmarkar_karp_partition(items, n_partitions,
                                    [](T const& t) { return t; });
  }
  //****************************************************************************

}  // namespace elastica
