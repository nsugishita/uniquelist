# uniquelist

C++/Python library which provides a list which stores unique elements/arrays.
This is similar to `set` in C++/Python, but this sorts the elements in the order of addition.
C++ and Python interfaces are available.

# Example

Below is an example use in Python.

```python3
>>> import uniquelistpy
>>> lst = uniquelistpy.UniqueList()
>>> lst.push_back(2)  # Append 2.
(0, True)
>>> lst.push_back(1)  # Append 1.
(1, True)
>>> lst.push_back(2)  # 2 is not appended since it is already in the list.
(0, False)
>>> lst.push_back(3)
(2, True)
>>> lst.push_back(5)
(3, True)
>>> lst.size()  # List has [2, 1, 3, 5]
4
>>> lst.index(2)
0
>>> lst.index(3)
2
>>> lst.index(4)
-1
>>> lst.erase_nonzero([0, 1, 0, 1])  # Remove the 2nd and 4th elements.
>>> lst.size()
2

```

`UniqueArrayList` handles arrays.

```python3
>>> import uniquelistpy
>>> lst = uniquelistpy.UniqueArrayList(3)
>>> lst.push_back([0, 1.5, 2])
(0, True)
>>> lst.push_back([2, 1, 2.1])
(1, True)
>>> lst.push_back([-1, 0.8, 0])
(2, True)
>>> lst.push_back([2, 1, 2.1])
(1, False)
>>> lst.push_back([-1, 0.8, -1])
(3, True)
>>> lst.size()
4
>>> lst.erase_nonzero([1, 0, 0, 1])
>>> lst.size()
2
>>> lst.push_back([2, 1, 2.1])
(0, False)
>>> lst.push_back([-1, 0.8, -1])
(2, True)
>>> lst.size()
3

```

# Install

This uses CMake and pybind11.
Typical commands to build the library are as follows.

```shell
$ cmake -S . -B build
$ cmake --build build
$ cmake --install build
```

To use the Python interface set `PYTHONPATH`.

```shell
$ export PYTHONPATH="$PYTHONPATH":"$(pwd)/build"
$ python tests/test.py
```
