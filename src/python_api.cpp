#include <iostream>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

#include "uniquelist/sized_ptr.h"
#include "uniquelist/uniquelist.h"

namespace py = pybind11;

using intlist = uniquelist::uniquelist<int>;
using sized_ptr = uniquelist::sized_ptr<std::shared_ptr<double[]>>;
using arraylist = uniquelist::uniquelist<sized_ptr, uniquelist::strictly_less>;

// TODO Make UniqueList pickable.

PYBIND11_MODULE(uniquelistpy, m) {
  m.doc() = "uniquelist extension";

  py::class_<intlist>(m, "UniqueList")
      .def(py::init<>())
      .def("size", &intlist::size, "Return the number of items in the list")
      .def(
          "push_back", [](intlist &a, int x) { return a.push_back(x); },
          "Add an item at the end of the list if it's new")
      .def(
          "erase_nonzero",
          [](intlist &a, py::array_t<int> removed) {
            auto removed_ = removed.request();
            return a.erase_nonzero(removed_.shape[0],
                                   static_cast<int *>(removed_.ptr));
          },
          "Erase items at given positions")
      .def(
          "index",
          [](const intlist &a, int x) {
            int i = 0;
            for (auto item : a) {
              if (item == x) {
                return i;
              }
              ++i;
            }
            return -1;
          },
          "Search a give item in the list and return its index")
      .def(
          "display",
          [](const intlist &a) {
            for (auto item : a) {
              std::cout << item << " ";
            }
            std::cout << std::endl;
          },
          "Print the items");

  py::class_<arraylist>(m, "UniqueArrayList")
      .def(py::init<>())
      .def("size", &arraylist::size, "Return the number of items in the list")
      .def(
          "push_back",
          [](arraylist &a, py::array_t<double> array) {
            auto array_ = array.request();
            if (array_.ndim != 1) {
              std::stringstream ss;
              ss << "expected 1 dimensional but got " << array_.ndim
                 << " dimensional";
              throw std::invalid_argument(ss.str());
            }
            auto view = uniquelist::shared_ptr_without_ownership(
                static_cast<double *>(array_.ptr));
            sized_ptr sized_ptr_view{static_cast<size_t>(array_.shape[0]),
                                     view};
            return a.push_back_with_hook(
                sized_ptr_view,
                uniquelist::deepcopy<std::shared_ptr<double[]>>);
          },
          "Add an item at the end of the list if its' new")
      .def(
          "erase",
          [](arraylist &a, py::array_t<int> removed) {
            auto removed_ = removed.request();
            if (removed_.ndim != 1) {
              std::stringstream ss;
              ss << "expected 1 dimensional but got " << removed_.ndim
                 << " dimensional";
              throw std::invalid_argument(ss.str());
            }
            return a.erase(removed_.shape[0], static_cast<int *>(removed_.ptr));
          },
          "Erase items at given indexes")
      .def(
          "erase_nonzero",
          [](arraylist &a, py::array_t<int> removed) {
            auto removed_ = removed.request();
            if (removed_.ndim != 1) {
              std::stringstream ss;
              ss << "expected 1 dimensional but got " << removed_.ndim
                 << " dimensional";
              throw std::invalid_argument(ss.str());
            }
            if (removed_.shape[0] != a.size()) {
              std::stringstream ss;
              ss << "expected size " << a.size() << " but got "
                 << removed_.shape[0];
              throw std::invalid_argument(ss.str());
            }
            return a.erase_nonzero(removed_.shape[0],
                                   static_cast<int *>(removed_.ptr));
          },
          "Erase items at positions where flags are nonzeros");
}
