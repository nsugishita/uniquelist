
#include <vector>
#include <iostream>
#include <iterator>  // std::back_inserter

#include <gtest/gtest.h>

#include "uniquelist/uniquelist.h"
#include "uniquelist/sized_ptr.h"

TEST(TestUtilsUniqueList, TestUniquelistWithSizedPtr) {
  {
    using array = uniquelist::sized_ptr<std::shared_ptr<double[]>>;
    uniquelist::uniquelist<array, uniquelist::strictly_less> list;

    {
      auto a = uniquelist::as_sized_ptr({2.9, -1.0, 4.9});
      auto [pos, isnew] = list.push_back(a);
      EXPECT_EQ(pos, 0);
      EXPECT_EQ(isnew, 1);
    }

    {
      auto a = uniquelist::as_sized_ptr({3.4, 1.0, 4.9});
      auto [pos, isnew] = list.push_back_with_hook(a, uniquelist::deepcopy<std::shared_ptr<double[]>>);
      EXPECT_EQ(pos, 1);
      EXPECT_EQ(isnew, 1);
    }

    {
      auto a = uniquelist::as_sized_ptr({5.5, 5.0, 0.0});
      auto [pos, isnew] = list.push_back(a);
      EXPECT_EQ(pos, 2);
      EXPECT_EQ(isnew, 1);
    }

    {
      auto a = uniquelist::as_sized_ptr({3.4, 1.0, 4.8999999999});
      auto [pos, isnew] = list.push_back(a);
      EXPECT_EQ(pos, 1);
      EXPECT_EQ(isnew, 0);
    }

    {
      auto a = uniquelist::as_sized_ptr({5.5, 5.0, 0.0});
      auto it = std::begin(list);
      ++it;
      auto [pos, isnew] = list.insert(it, a);
      EXPECT_EQ(pos, 2);
      EXPECT_EQ(isnew, 0);
    }

    {
      auto a = uniquelist::as_sized_ptr({1.5, 1.0, 0.1});
      auto it = std::begin(list);
      ++it;
      auto [pos, isnew] = list.insert(it, a);
      EXPECT_EQ(pos, 1);
      EXPECT_EQ(isnew, 1);
    }

    {
      auto a = uniquelist::as_sized_ptr({5.5, 5.0, 0.0});
      auto result = list.isin(a);
      EXPECT_EQ(result, 1);
    }

    {
      auto a = uniquelist::as_sized_ptr({1.5, 1.0, 0.1});
      auto result = list.isin(a);
      EXPECT_EQ(result, 1);
    }

    {
      auto a = uniquelist::as_sized_ptr({1.5, 1.4, 4.0});
      auto result = list.isin(a);
      EXPECT_EQ(result, 0);
    }

    EXPECT_EQ(std::size(list), 4);

    std::vector<int> flags = {false, true};
    list.erase_nonzero(std::size(flags), flags.data());

    EXPECT_EQ(std::size(list), 3);
  }
}
