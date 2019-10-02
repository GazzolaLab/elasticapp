#pragma once
#include <tuple>
#include <type_traits>

#include "DataStructures/Tags.hpp"
#include "Utilities/Requires.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits.hpp"

/*
TODO
1. Forwarding arguments
2. Utility namespace general
3. Documentation
4. noexcept specifiers
5. Cleanup for apply...
*/

namespace tuples {

  /*
   * Traits for identifying Simple and Container Tags
   */
  namespace detail {

    template <typename Tag, Requires<Tags::is_simple_tag_v<Tag>> = nullptr>
    struct SimpleTagTrait : Tag {
      using tag_type = Tag;
      using typename Tag::data_type;
    };

    /*
     * TODO
     * ContainerTagTrait doesn't derived from SimpleTagTrait because of
     * the Requires clause. Find a way around it in the future without
     * coding in explicitly
     * struct ContainerTagTrait: SimpleTag<Tag, template_error....>
     *
     */
    template <typename Tag, Requires<Tags::is_container_tag_v<Tag>> = nullptr>
    struct ContainerTagTrait : Tag {
      using tag_type = Tag;
      using Tag::data_type;
      using Tag::store_type;
    };

  }  // namespace detail

  using Tags::TagType;

  template <TagType, class MetaData>
  struct TagPolicy;

  template <class MetaData>
  struct TagPolicy<TagType::Simple, MetaData> {
    template <typename Tag>
    using tag_t = typename detail::SimpleTagTrait<Tag>::tag_type;
    template <typename Tag>
    using data_t = typename detail::SimpleTagTrait<Tag>::data_type;
  };

  template <class MetaData>
  struct TagPolicy<TagType::Container, MetaData> {
    template <typename Tag>
    using tag_t = typename detail::ContainerTagTrait<Tag>::tag_type;
    template <typename Tag>
    using storage_t = typename detail::ContainerTagTrait<
        Tag>::template store_type<MetaData::storage_option>;
    template <typename Tag>
    using data_t = typename detail::ContainerTagTrait<Tag>::template data_type<
        storage_t<Tag>, typename Tag::value_type, MetaData::BlockSize,
        MetaData::Dim>;
  };

  // Definition
  template <TagType TT, class MetaData, typename... Tags>
  class TaggedTuple
      : public std::tuple<
            typename TagPolicy<TT, MetaData>::template data_t<Tags>...> {
   public:
    using policy_type = TagPolicy<TT, MetaData>;

    using tag_type = std::tuple<typename policy_type::template tag_t<Tags>...>;
    using value_type =
        std::tuple<typename policy_type::template data_t<Tags>...>;

    using value_type::swap;
    using value_type::value_type;
    using value_type::operator=;

    constexpr TaggedTuple() = default;

    static constexpr size_t size() noexcept { return sizeof...(Tags); }

   private:
    // Check if any tags are duplicated
    static_assert(
        tt::is_all_unique_v<typename policy_type::template tag_t<Tags>...>,
        "Tags in TaggedTuple are not unique!");
  };

  // Indexing functions for TaggedTuple
  namespace detail {
    // Base case
    template <typename TagToBeFound>
    constexpr std::size_t get_index_impl(std::size_t /*index*/,
                                         std::size_t end) {
      return end;
    }

    // Peeling
    template <typename TagToBeFound, typename Tag, typename... Tags>
    constexpr std::size_t get_index_impl(std::size_t index, std::size_t end) {
      return std::is_same<TagToBeFound, Tag>::value
                 ? index
                 : get_index_impl<TagToBeFound, Tags...>(index + 1, end);
    }
  }  // namespace detail

  // Expose to world
  template <typename TagToBeFound, typename... Tags>
  constexpr std::size_t get_index() {
    return detail::get_index_impl<TagToBeFound, Tags...>(0,
                                                         sizeof...(Tags) + 1);
  }

}  // namespace tuples

namespace std {
  template <typename TagToBeFound, tuples::TagType TT, class MetaData,
            typename... Tags>
  constexpr auto& get(
      tuples::TaggedTuple<TT, MetaData, Tags...>& in_tuple) noexcept {
    return std::get<tuples::get_index<
        TagToBeFound, typename tuples::TaggedTuple<TT, MetaData, Tags...>::
                          policy_type::template tag_t<Tags>...>()>(in_tuple);
  }

  template <typename TagToBeFound, tuples::TagType TT, class MetaData,
            typename... Tags>
  constexpr const auto& get(
      const tuples::TaggedTuple<TT, MetaData, Tags...>& in_tuple) noexcept {
    return std::get<tuples::get_index<
        TagToBeFound, typename tuples::TaggedTuple<TT, MetaData, Tags...>::
                          policy_type::template tag_t<Tags>...>()>(in_tuple);
  }

}  // namespace std

namespace tuples {
  /*
   * If you feed in a tmpl::list of parameters, you get out a tuple containing
   * those values in the TT
   */
  template <typename... TagsToBeFound, tuples::TagType TT, class MetaData,
            typename... Tags>
  constexpr std::tuple<typename tuples::TaggedTuple<
      TT, MetaData, Tags...>::policy_type::template data_t<TagsToBeFound>...>
  get(tuples::TaggedTuple<TT, MetaData, Tags...>& in_tuple,
      tmpl::list<TagsToBeFound...> = {}) {
    return std::make_tuple(std::get<TagsToBeFound>(in_tuple)...);
  }
}  // namespace tuples

template <typename... Args>
using TaggedTuple = tuples::TaggedTuple<tuples::TagType::Simple, int, Args...>;

template <class MetaData, typename... Args>
using MetaTaggedTuple =
    tuples::TaggedTuple<tuples::TagType::Container, MetaData, Args...>;

// TagToBeFound, tuples::TaggedTuple<TT, MetaData, Tags...>::policy_type::
// retrieval

/*
template <typename... TagsToBeFound, typename... Tags, class Functor>
void apply_impl(TaggedTuple<Tags...>& in_tuple, Functor& f) {
  Functor temp(get<TagsToBeFound>(in_tuple)...);
  f = temp;
}

template <template <typename...> class TagList, typename... TagsToBeFound,
          typename... Tags, class Functor>
void apply_impl_what(TagList<TagsToBeFound...>&&,
                     TaggedTuple<Tags...>& in_tuple, Functor& f) {
  Functor temp(get<TagsToBeFound>(in_tuple)...);
  f = temp;
}

template <typename TagList, typename... Tags, class Functor>
void apply(TaggedTuple<Tags...>& in_tuple, Functor& f) {
  apply_impl_what(TagList{}, in_tuple, f);
}
*/
