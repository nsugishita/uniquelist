/**
 * @file
 * @author Nagisa Sugishita <s1576972@ed.ac.uk>
 * @version 1.0
 *
 * List to keep unique elements.
 *
 * An extension of std::list which only takes unique elements.
 */

#ifndef UNIQUELIST_UNIQUELIST_H
#define UNIQUELIST_UNIQUELIST_H

#include <list>  // std::list
#include <map>  // std::map
#include <type_traits>  // std::is_same_v
#include <memory>  // std::shared_ptr
#include <utility> // std::pair

namespace uniquelist {

/**
 * @brief Linked list which only keeps unique elements
 *
 * This is a linked list which only contains unique elements.
 * Elements in this list must not be modified after they
 * are added until they are removed from the list.
 *
 * Internally, this uses a list and a map to sort elements
 * in the order of addition and in the increasing order,
 * respectively. In this way, membership of an element
 * can be tested quickly while it maintains the order of addition.
 *
 * When a new item is added, a new entry is added to the map and
 * the list. The key of the entry of the map is the newly added
 * item, while the value is a link to the entry in the list.
 * Similarly, the entry in the list is a link to the entry in
 * the map.
 *
 * ```
 * map : [
 *     <smallest input value, link to the corresponding entry in the list>,
 *     <2nd smallest input value, link to the corresponding entry in the list>,
 *     ...
 *     <largest input value, link to the corresponding entry in the list>
 * ]
 *
 * list : [
 *     <link to the entry of the 1st input in the map>,
 *     <link to the entry of the 2nd input in the map>,
 *     ...
 *     <link to the entry of the last input in the map>
 * ]
 * ```
 *
 * When one wants to retrieve the item at position n in terms
 * of the addition, first the nth entry in the list is obtained.
 * Then the link of the nth entry in the list is used to retrieve
 * the corresponding entry in the map. The key of the entry in
 * the map is the value looked for.
 *
 * When one wants to check if an item is already added or not,
 * one can check the item is in the map as a key or not.
 *
 */
template <typename T, typename Compare = std::less<T>> struct uniquelist {

protected:
  struct map_item_type;

  /**
   * @brief Type of items added in the underlying list
   *
   * Instances of this struct are added to the underlying list.
   * Each instance has an iterator to an item in the map
   * and that item in the map has an iterator pointing back
   * to the original element in the list.
   */
  struct list_item_type {
    typename std::map<T, map_item_type, Compare>::iterator link;
  };

  /**
   * @brief Type of items added in the underlying map
   *
   * This type is used as a value_type for the underlying map.
   * Each instance has an iterator to an item in the list
   * and that item in the list has an iterator pointint back
   * to the original element (and its key) in the map.
   */
  struct map_item_type {
    typename std::list<list_item_type>::iterator link;
  };

  /**
   * @brief Type of the underlying list
   */
  using list_type = std::list<list_item_type>;

  /**
   * @brief Type of the underlying map
   */
  using map_type = std::map<T, map_item_type, Compare>;

  /**
   * @brief Iterator to iterate elements in the order they are added
   *
   * This is a bidirectional iterator to iterate over
   * elements in the order they are added.
   *
   * The template parameter S must be an iterator type
   * of list_type.  This keeps an iterator of the list_type
   * internally * and surrogate all operations.  When an
   * iterator is dereferenced, the key of the corresponding
   * map elements are returned.
   */
  template <typename S> struct iterator_wrapper {
    using iterator_category = typename S::iterator_category;
    using value_type = S;
    using difference_type = typename S::difference_type;
    using pointer = value_type *;
    using reference = value_type &;

    constexpr static bool is_list_iterator = (
        std::is_same_v<S, typename list_type::iterator>
        || std::is_same_v<S, typename list_type::const_iterator>
    );

    explicit iterator_wrapper(const S &it) noexcept : it{it} {}

    auto &operator++() noexcept {
      ++it;
      return *this;
    }

    auto &operator--() noexcept {
      --it;
      return *this;
    }

    auto operator++(int) noexcept {
      auto buf = *this;
      ++*this;
      return buf;
    }

    auto operator--(int) noexcept {
      auto buf = *this;
      --*this;
      return buf;
    }

    template <typename U> auto operator==(const U &other) const noexcept {
      return it == other.it;
    }

    template <typename U> auto operator!=(const U &other) const noexcept {
      return !(*this == other);
    }

    const auto &operator*() {
      if constexpr (is_list_iterator) {
        return it->link->first;
      } else {
        return it->first;
      }
    }

    const auto &operator*() const {
      if constexpr (is_list_iterator) {
        return it->link->first;
      } else {
        return it->first;
      }
    }

    const auto *operator->() {
      if constexpr (is_list_iterator) {
        return &it->link->first;
      } else {
        return &it->first;
      }
    }

    const auto *operator->() const {
      if constexpr (is_list_iterator) {
        return &it->link->first;
      } else {
        return &it->first;
      }
    }

    auto get_list_iterator() noexcept {
      return it;
      if constexpr (is_list_iterator) {
        return it;
      } else {
        return it->second;
      }
    }
    auto get_map_iterator() noexcept { return it->link; }

  private:
    S it;
  };

public:
  using value_type = T;
  using reference = value_type &;
  using const_reference = const value_type &;
  using pointer = value_type *;
  using const_pointer = const value_type *;
  using list_iterator_wrapper = iterator_wrapper<typename list_type::iterator>;
  using const_list_iterator = iterator_wrapper<typename list_type::const_iterator>;
  using map_iterator_wrapper = iterator_wrapper<typename map_type::iterator>;
  using const_map_iterator = iterator_wrapper<typename map_type::const_iterator>;

  /* Member functions */

  /* Iterators */

  /**
   * @brief Returns an iterator pointing to the first element
   *
   * @return An iterator to the beginning of the sequence container.
   */
  auto begin() noexcept { return list_iterator_wrapper(std::begin(list)); }

  /**
   * @brief Returns an iterator pointing to the first element
   *
   * @return An iterator to the beginning of the sequence container.
   */
  auto begin() const noexcept { return const_list_iterator(std::begin(list)); }

  /**
   * @brief Returns an iterator referring to the past-the-end element
   *
   * @return An iterator to the element past the end of the sequence.
   */
  auto end() noexcept { return list_iterator_wrapper(std::end(list)); }

  /**
   * @brief Returns an iterator referring to the past-the-end element
   *
   * @return An iterator to the element past the end of the sequence.
   */
  auto end() const noexcept { return const_list_iterator(std::end(list)); }

  /**
   * @brief Returns an iterator pointing to the first element
   *
   * @return An iterator to the beginning of the sequence container.
   */
  auto sbegin() noexcept { return map_iterator_wrapper(std::begin(map)); }

  /**
   * @brief Returns an iterator pointing to the first element
   *
   * @return An iterator to the beginning of the sequence container.
   */
  auto sbegin() const noexcept { return const_map_iterator_wrapper(std::begin(map)); }

  /**
   * @brief Returns an iterator referring to the past-the-end element
   *
   * @return An iterator to the element past the end of the sequence.
   */
  auto send() noexcept { return map_iterator_wrapper(std::end(map)); }

  /**
   * @brief Returns an iterator referring to the past-the-end element
   *
   * @return An iterator to the element past the end of the sequence.
   */
  auto send() const noexcept { return const_map_iterator_wrapper(std::end(map)); }

  /* Capacity */

  /**
   * @brief Test whether container is empty
   *
   * @return true if the container size is 0, false otherwise.
   */
  auto empty() const noexcept {
    return list.empty();
  }

  /**
   * @brief Return size
   *
   * @return The number of elements in the container.
   */
  auto size() const noexcept { return list.size(); }

  /**
   * @brief Return maximum size
   *
   * @return The maximum number of elements the object can hold as content.
   */
  auto max_size() const noexcept {
    return std::min(list.max_size(), map.max_size());
  }

  /* Modifiers */

  /**
   * @brief Add a new item to the end if it is not in the list
   *
   * @param [in] Value to be added
   *
   * @return Pair of the position of the given item in the list
   *     and status.  status = true indicates that the item is added
   *     as a new one and false indicates that the item is already
   *     in the list.
   */
  auto push_back(const T &key) { return insert(std::end(*this), key); }

  /**
   * @brief Add a new item to the end if it is not in the list
   *
   * @param [in] Value to be added
   *
   * @return Pair of the position of the given item in the list
   *     and status.  status = true indicates that the item is added
   *     as a new one and false indicates that the item is already
   *     in the list.
   */
  auto push_back(T &&key) { return insert(std::end(*this), std::move(key)); }

  /**
   * @brief Add a new item to the end if it is not in the list
   *
   * @param [in] Value to be added
   * @param [in] Hook called when the value is added. This may be
   *     useful when we want to deepcopy the value only if
   *     the item is new.
   *
   * @return Pair of the position of the given item in the list
   *     and status.  status = true indicates that the item is added
   *     as a new one and false indicates that the item is already
   *     in the list.
   */
  template <typename F> auto push_back_with_hook(const T &key, const F &f) {
    return insert_with_hook(std::end(*this), key, f);
  }

  /**
   * @brief Insert a new element before the the specified position
   *
   * @param [in] Position in the container where
   *     the new elements are inserted.
   * @param [in] Value to be added
   *
   * @return Pair of the position of the given item in the list
   *     and status.  status = true indicates that the item is added
   *     as a new one and false indicates that the item is already
   *     in the list.
   */
  template <typename S>
  auto insert(iterator_wrapper<S> position, const value_type &val) {
    auto [it, status] = map.insert(std::pair(val, map_item_type{}));
    if (status) {
      it->second.link = list.insert(position.get_list_iterator(), list_item_type{it});
    }
    return std::pair<size_t, bool>(
        std::distance(std::begin(list), it->second.link), status);
  }

  /**
   * @brief Insert a new element before the the specified position
   *
   * @param [in] Position in the container where
   *     the new elements are inserted.
   * @param [in] Value to be added
   *
   * @return Pair of the position of the given item in the list
   *     and status.  status = true indicates that the item is added
   *     as a new one and false indicates that the item is already
   *     in the list.
   */
  template <typename S>
  auto insert(iterator_wrapper<S> position, value_type &&val) {
    auto [it, status] = map.try_emplace(std::move(val), map_item_type{});
    if (status) {
      it->second.link = list.insert(position.get_list_iterator(), list_item_type{it});
    }
    return std::pair<size_t, bool>(
        std::distance(std::begin(list), it->second.link), status);
  }

  /**
   * @brief Insert a new item to the end if it is not in the list
   *
   * @param [in] Position in the container where
   *     the new elements are inserted.
   * @param [in] Value to be added
   * @param [in] Hook called when the value is added. This may be
   *     useful when we want to deepcopy the value only if
   *     the item is new.
   *
   * @return Pair of the position of the given item in the list
   *     and status.  status = true indicates that the item is added
   *     as a new one and false indicates that the item is already
   *     in the list.
   */
  template <typename S, typename F>
  auto insert_with_hook(iterator_wrapper<S> position, const value_type &val,
                        const F &f) {
    // First, add the item without calling the hook.
    auto [try_it, try_status] = map.insert(std::pair(val, map_item_type{}));
    if (try_status) { // If the given item is new.
      // Remove the item added above and add the one returned by the hook.
      auto hint = try_it; // Pos just before the newly added element.
      ++hint;
      map.erase(try_it); // Remove the item added above.
      // Call the hook and re-insert the result to the map.
      auto it = map.emplace_hint(hint, f(val), map_item_type{});
      it->second.link = list.insert(position.get_list_iterator(), list_item_type{it});
      return std::pair<size_t, bool>(
          std::distance(std::begin(list), it->second.link), try_status);
    } else { // If the given item is not new.
      return std::pair<size_t, bool>(
          std::distance(std::begin(list), try_it->second.link), try_status);
    }
  }

  /**
   * @brief Remove an element
   *
   * This removes an element pointed by a given iterator.
   *
   * @param [in] it Iterator pointint to an element to be removed
   *
   * @return An iterator pointint to the element that followed
   *     the element erased by the function call.
   */
  template <typename S>
  auto erase(iterator_wrapper<S> it) {
    if constexpr (iterator_wrapper<S>::is_list_iterator) {
      map.erase(it.get_map_iterator());
      return iterator_wrapper<S>(list.erase(it.get_list_iterator()));
    } else {
      list.erase(it.get_list_iterator());
      return iterator_wrapper<S>(map.erase(it.get_map_iterator()));
    }
  }

  /**
   * @brief Erase an element at a given position
   *
   * This removes an element at a given position.
   *
   * @param [in] index Index of element to be removed.
   *
   * @return An iterator pointint to the element that followed
   *     the element erased by the function call.
   */
  auto erase(size_t index) {
    auto it = std::begin(*this);
    std::advance(it, index);
    return erase(it);
  }

  /**
   * @brief Erase elements at given positions
   *
   * Remove elements at given positions.  `indexes` must be sorted
   * in the increasing order.
   *
   * @param [in] n Number of elements to be removed
   * @param [in] indexes Array of indexes of elements to be removed.
   *     The indexes must be sorted in the increasing order.  size: n
   */
  template <typename U> auto erase(size_t n, const U *indexes) {
    auto prev_pos = 0;
    auto cursor = ++std::begin(*this);
    for (size_t i = 0; i < n; ++i) {
      std::advance(cursor, indexes[i] - prev_pos - 1);
      cursor = erase(cursor);
      prev_pos = indexes[i];
    }
  }

  /**
   * @brief Erase elements at the positions of nonzero elements
   *
   * Remove elements at the positions of nonzero elements.
   * Elements which are evaluated as true is considered
   * to be nonzero.
   *
   * @param [in] n Size of `flags`
   * @param [in] flag Array of flags whose nonzero elements
   *     indicate the removal of the corresponding elements.  size: n
   */
  template <typename U> auto erase_nonzero(size_t n, const U *flag) {
    auto cursor = std::begin(*this);
    for (size_t i = 0; i < n; ++i) {
      if (flag[i]) {
        cursor = erase(cursor);
      } else {
        ++cursor;
      }
    }
  }

  /**
   * @brief Remove all elements
   */
  auto clear() noexcept {
    list.clear();
    map.clear();
  }

  /**
   * @brief Test if the given item is in the list or not
   *
   * @param [in] val Value to search for.
   *
   * @return true if contained and 0 otherwise.
   */
  auto isin(const T &val) const noexcept { return map.count(val) > 0; }

private:
  /**
   * @brief Actual list to maintain elements.
   *
   * This keeps all elements in this uniquelist.
   * More precisely, all elements are kept as keys
   * in the map and this list contains iterators
   * to the key, value pairs of map.  The order
   * of elements in this list is the same as
   * the order of the elements added, and traversing
   * this list allows us to travese elements in
   * the insertion order.
   */
  list_type list{};

  /**
   * @brief Map to maintain given elements in the sorted order
   *
   * This keeps all given elements as keys.  The corresponding
   * values are iterators to the elements in list.  From
   * a key, value pair of this map, one can use the iterator
   * to get the element in the list, and compute the index
   * based on their insertion, say.
   */
  map_type map{};

}; // struct uniquelist

} // namespace uniquelist

#endif // UNIQUELIST_UNIQUELIST_H
