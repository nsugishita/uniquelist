#include <iostream>
#include <iterator> // std::back_inserter
#include <vector>

#include <gtest/gtest.h>

#include "uniquelist/uniquelist.h"

TEST(TestUtilsUniqueList, TestUniquelist) {
  {
    using T = double;
    uniquelist::uniquelist<T> list;

    {
      auto [pos, isnew] = list.push_back(3.9); // -> [3.9]
      EXPECT_EQ(pos, 0);
      EXPECT_EQ(isnew, 1);
    }

    {
      auto [pos, isnew] = list.push_back(-1.0); // -> [3.9, -1.0]
      EXPECT_EQ(pos, 1);
      EXPECT_EQ(isnew, 1);
    }

    {
      auto [pos, isnew] = list.push_back(0.0); // -> [3.9, -1.0, 0.0]
      EXPECT_EQ(pos, 2);
      EXPECT_EQ(isnew, 1);
    }

    {
      auto [pos, isnew] = list.push_back(-1.0); // -> [3.9, -1.0, 0.0]
      EXPECT_EQ(pos, 1);
      EXPECT_EQ(isnew, 0);
    }

    {
      auto it = std::begin(list);
      ++it;
      auto [pos, isnew] = list.insert(it, 0.0); // -> [3.9, -1.0, 0.0]
      EXPECT_EQ(pos, 2);
      EXPECT_EQ(isnew, 0);
    }

    {
      auto it = std::begin(list);
      ++it;
      ++it;
      auto [pos, isnew] = list.insert(it, 1.0); // -> [3.9, -1.0, 1.0, 0.0]
      EXPECT_EQ(pos, 2);
      EXPECT_EQ(isnew, 1);
    }

    {
      auto result = list.isin(-1.0);
      EXPECT_EQ(result, 1);
    }

    {
      auto result = list.isin(0.0);
      EXPECT_EQ(result, 1);
    }

    {
      auto result = list.isin(9.1);
      EXPECT_EQ(result, 0);
    }

    EXPECT_EQ(std::size(list), 4);

    {
      std::vector<T> out;
      std::copy(list.begin(), list.end(), std::back_inserter(out));
      std::vector<T> expected = {3.9, -1.0, 1.0, 0.0};
      EXPECT_EQ(out, expected);
    }

    {
      std::vector<T> out;
      std::copy(list.sbegin(), list.send(), std::back_inserter(out));
      std::vector<T> expected = {-1.0, 0.0, 1.0, 3.9};
      EXPECT_EQ(out, expected);
    }

    std::vector<int> flags = {false, true, false, false};
    list.erase_nonzero(std::size(flags), flags.data());

    EXPECT_EQ(std::size(list), 3);

    {
      auto it = std::begin(list);
      ++it;
      ++it;
      auto [pos, isnew] = list.insert(it, 1.0); // -> [3.9, 1.0, 0.0]
      EXPECT_EQ(pos, 1);
      EXPECT_EQ(isnew, 0);
    }

    {
      auto it = std::begin(list);
      ++it;
      ++it;
      auto [pos, isnew] = list.insert(it, -5.0); // -> [3.9, 1.0, -5.0, 0.0]
      EXPECT_EQ(pos, 2);
      EXPECT_EQ(isnew, 1);
    }

    {
      std::vector<T> out;
      std::copy(list.begin(), list.end(), std::back_inserter(out));
      std::vector<T> expected = {3.9, 1.0, -5.0, 0.0};
      EXPECT_EQ(out, expected);
    }

    {
      std::vector<T> out;
      std::copy(list.sbegin(), list.send(), std::back_inserter(out));
      std::vector<T> expected = {-5.0, 0.0, 1.0, 3.9};
      EXPECT_EQ(out, expected);
    }
  }
}
