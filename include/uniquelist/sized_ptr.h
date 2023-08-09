/**
 * @file
 * @author Nagisa Sugishita <s1576972@ed.ac.uk>
 * @version 1.0
 *
 * Shared ptr with size
 */

#ifndef UNIQUELIST_SIZED_PTR_H
#define UNIQUELIST_SIZED_PTR_H

#include <concepts>
#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <type_traits>
#include <vector>

namespace uniquelist {

/**
 * @brief Shared ptr with size
 *
 * This struct keeps a shared pointer and the size of the pointed 
 * memory block.
 *
 * One can compare two sized_ptrs in the shortlex order using `<` operator.
 * Furthermore, one can use `strictly_less` to compare two sized_ptrs.
 *
 * `deepcopy` can deepcopy a sized_ptr
 *
 */
template <typename P> struct sized_ptr {
  using smart_pointer_type = P;

  size_t size;
  P ptr;

  /**
   * @brief Compare two sized_ptr instances
   *
   * This compares two sized_ptr lexicographically.
   * If `l.size < r.size`, `l < r`.
   * If `l.size > r.size`, `l > r`.
   * If both have the same size, entries are compared one by one.
   * Let `i` be the first index where `l[i] != r[i]`.
   * If `l[i] < r[i]`, then `l < r`.
   * If `l[i] > r[i]`, then `l > r`.
   * If `l.size == r.size` and `l[i] == r[i]` for all i, then `l == r`.
   *
   */
  template <typename Q>
  friend bool operator<(const sized_ptr<P> &l, const sized_ptr<Q> &r) {
    if (l.size < r.size) {
      return true;
    } else if (l.size > r.size) {
      return false;
    } else {
      auto p = l.ptr.get();
      auto q = r.ptr.get();
      for (size_t i = 0; i < l.size; ++i, ++p, ++q) {
        if ((*p) < (*q)) {
          return true;
        } else if ((*p) > (*q)) {
          return false;
        }
      }
    }
    return false;
  }
};

/**
 * @brief Deepcopy a sized_ptr
 *
 * This deepcopies a sized_ptr instance. The returned sized_ptr
 * does not shared a memory block.
 */
template <typename P> auto deepcopy(const sized_ptr<P> &p) {
  using nonconst_element_type =
      std::remove_const_t<std::remove_extent_t<typename P::element_type>>;
  std::unique_ptr<nonconst_element_type[]> out_p{
      new nonconst_element_type[p.size]};
  std::copy(p.ptr.get(), p.ptr.get() + p.size, out_p.get());
  return sized_ptr<P>{p.size, P{out_p.release()}};
}

/**
 * @brief Compare two numbers with a tolerance
 */
struct strictly_less {
  double rtol;
  double atol;

  strictly_less(double rtol = 1e-6, double atol = 1e-6)
      : rtol{rtol}, atol{atol} {}

  template <typename T>
  bool operator()(T a, T b) const
    requires std::integral<T> || std::floating_point<T>
  {
    return a < b - ((b > 0) ? b : -b) * this->rtol - this->atol;
  }

  template <typename S, typename T>
  bool operator()(const sized_ptr<S> &a, const sized_ptr<T> &b) const {
    if (a.size < b.size) {
      return true;
    } else if (a.size > b.size) {
      return false;
    } else {
      auto p = a.ptr.get();
      auto q = b.ptr.get();
      for (size_t i = 0; i < a.size; ++i, ++p, ++q) {
        if ((*this)(*p, *q)) {
          return true;
        } else if ((*this)(*q, *p)) {
          return false;
        }
      }
      return false;
    }
  }
};

/**
 * @brief Allocate a memory and initialise by sequence of numbers
 *
 * This allocates a memory block and initialise it with a sequence
 * of increasing numbers. The result is returned as a shared pointer.
 */
template <typename T> auto range(int n) {
  using element_type = std::remove_extent_t<T>;
  using nonconst_element_type = std::remove_const_t<std::remove_extent_t<T>>;
  if (n <= 0) {
    return std::shared_ptr<element_type[]>{nullptr};
  }
  std::shared_ptr<nonconst_element_type[]> p{
      new nonconst_element_type[static_cast<size_t>(n)]()};
  for (int i = 0; i < n; ++i) {
    p[i] = i;
  }
  return std::shared_ptr<element_type[]>{p};
}

/**
 * @brief Allocate a memory block and create a shared pointer
 *
 * This allocates a memory block and copies the data passed in
 * the initializer list. The result is returned as a shared pointer.
 */
template <typename T> auto as_shared_ptr(std::initializer_list<T> &&l) {
  using element_type = std::remove_extent_t<T>;
  using nonconst_element_type = std::remove_const_t<std::remove_extent_t<T>>;
  std::shared_ptr<nonconst_element_type[]> p{
      new nonconst_element_type[std::size(l)]()};
  auto it = std::begin(l);
  for (size_t i = 0; i < std::size(l); ++i, ++it) {
    p[static_cast<long>(i)] = *it;
  }
  return std::shared_ptr<element_type[]>{p};
}

/**
 * @brief Deleter for a view
 *
 * This is a deleter which does not release any data.
 * This may be used for a shared_ptr which is a view of a pointer.
 */
template <typename T> struct no_delete {
  void operator()(T *p) const { (void)p; }
};

/**
 * @brief Deleter for a view
 *
 * This is a deleter which does not release any data.
 * This may be used for a shared_ptr which is a view of a pointer.
 */
template <typename T> struct no_delete<T[]> {
  void operator()(T *p) const { (void)p; }
};

/**
 * @brief Create a shared pointer without the ownership
 *
 * This returns a shared pointer which keeps the given pointer.
 * However, the shared pointer does not possess the ownership.
 * That is, the deleter does not release any memory.
 */
template <typename T> auto shared_ptr_without_ownership(T *p) {
  using element_type = std::remove_extent_t<T>;
  return std::shared_ptr<element_type[]>(p, no_delete<element_type[]>());
}

/**
 * @brief Allocate a memory block and create a sized pointer
 *
 * This allocates a memory block and copies the data passed in
 * the initializer list. The result is returned as a sized pointer.
 */
template <typename T> auto as_sized_ptr(std::initializer_list<T> &&l) {
  using element_type = std::remove_extent_t<T>;
  using nonconst_element_type = std::remove_const_t<std::remove_extent_t<T>>;
  std::shared_ptr<nonconst_element_type[]> p{
      new nonconst_element_type[std::size(l)]()};
  auto it = std::begin(l);
  for (size_t i = 0; i < std::size(l); ++i, ++it) {
    p[static_cast<long>(i)] = *it;
  }
  return sized_ptr<std::shared_ptr<T[]>>{std::size(l), std::shared_ptr<element_type[]>{p}};
}


}

#endif
