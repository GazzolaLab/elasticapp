//==============================================================================
/*!
//  \file
//  \brief Internal maps from UUID to systems and vice-versa
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
#include <cstdint>
#include <unordered_map>
#include <vector>

#include "Simulator/Interface/Wrappers/ToWrapper.hpp"
#include "Simulator/Interface/Wrappers/UUID.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/Size.hpp"

namespace elastica {

  namespace interface {

    namespace detail {

      template <typename UID>
      class UUIDMap;

      template <typename UID>
      class UUIDMap {
       public:
        //**Type definitions****************************************************
        // Key type
        using Key = std::size_t;
        // ID type
        using ID = UID;
        // IDs type
        using IDs = std::vector<ID>;
        // Type of the map used
        using Map = std::unordered_map<Key, IDs>;
        //**********************************************************************

        UUIDMap() = default;
        ~UUIDMap() = default;

       protected:
        void map_to(Key k, ID const& id) { system_mapping_[k].push_back(id); }

       public:
        auto map(Key k) noexcept {
          return make_wrapped_to([this, k](ID const& id) mutable -> void {
            return this->map_to(k, id);
          });
        }

       public:
        auto as_vector() && -> std::vector<IDs> {
          std::vector<IDs> batches;
          // how many?
          batches.reserve(system_mapping_.size());

          for (auto&& s : system_mapping_)
            batches.push_back(std::move(s.second));

          return batches;
        }

        void clear() noexcept { system_mapping_.clear(); }
        inline auto size() const noexcept -> std::size_t {
          return system_mapping_.size();
        }

       private:
        //**Member variables****************************************************
        Map system_mapping_;  //!< Mapping data
        //**********************************************************************
      };

      template <typename UID>
      class UUIDToBatch {
       public:
        //**Type definitions****************************************************
        // ID type
        using ID = UID;
        // Key type
        using Key = ID;
        // IDs type
        using ValueType = std::size_t;
        // Type of the map used
        using Map = std::unordered_map<Key, ValueType>;
        //**********************************************************************

        explicit UUIDToBatch() : batch_mapping_(){};
        explicit UUIDToBatch(std::vector<std::vector<ID>> partitioned_ids) {
          // for fast lookup we first put it into a batch
          const std::size_t n_partitions = ::elastica::size(partitioned_ids);

          for (std::size_t partition_id = 0UL; partition_id < n_partitions;
               ++partition_id) {
            const std::size_t value = partition_id;
            for (auto&& key : partitioned_ids[value]) {
              batch_mapping_[std::move(key)] = value;
            }
          }
        };

        ~UUIDToBatch() = default;

        inline auto size() const noexcept -> std::size_t {
          return batch_mapping_.size();
        }
        inline auto data() & -> Map& { return batch_mapping_; }
        inline auto data() const& -> Map const& { return batch_mapping_; }

        // using S = std::vector<std::tuple<typename Key::IndexType,
        // std::string, ValueType>>;
        using P = std::tuple<typename Key::IndexType, std::string>;
        using K = std::vector<P>;
        using V = std::vector<ValueType>;

        auto from_vectors(std::tuple<K, V> as_vector_results) {
          K keys = std::get<0UL>(std::move(as_vector_results));
          V values = std::get<1UL>(std::move(as_vector_results));

          batch_mapping_.clear();
          const std::size_t restored_size = keys.size();

          // Put keys to form UUIDs
          std::vector<ID> ids;
          ids.reserve(restored_size);
          std::transform(std::make_move_iterator(std::begin(keys)),
                         std::make_move_iterator(std::end(keys)),
                         std::back_inserter(ids), [](P k) -> ID {
                           return {std::get<0UL>(std::move(k)),
                                   std::get<1UL>(std::move(k))};
                         });

          std::vector<std::pair<Key, ValueType>> kvs;
          kvs.reserve(restored_size);
          std::transform(std::make_move_iterator(std::begin(ids)),
                         std::make_move_iterator(std::end(ids)),
                         std::begin(values), std::back_inserter(kvs),
                         [](ID id, std::size_t batch) {
                           return std::make_pair(std::move(id), batch);
                         });

          batch_mapping_ = Map(std::make_move_iterator(kvs.begin()),
                               std::make_move_iterator(kvs.end()));
        }

        auto as_vectors() const& {
          K split_keys;
          V values;

          // how many?
          split_keys.reserve(size());
          values.reserve(size());

          for (auto const& b : batch_mapping_) {
            split_keys.push_back(std::make_tuple(id(b.first), name(b.first)));
            values.push_back(b.second);
          }

          return std::make_tuple(split_keys, values);
        }

        ValueType operator[](const Key& key) const {
          // const needed!
          return batch_mapping_.at(key);
        };

        void clear() noexcept { batch_mapping_.clear(); }

        Map batch_mapping_;
      };

      template <typename UID>
      auto make_uuid_to_batch_map(std::vector<std::vector<UID>> partitioned_ids)
          -> UUIDToBatch<UID> {
        return UUIDToBatch<UID>(std::move(partitioned_ids));
      }

    }  // namespace detail

  }  // namespace interface

}  // namespace elastica
