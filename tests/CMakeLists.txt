cmake_minimum_required(VERSION 3.4...3.18)
project(uniquelist_test VERSION 0.1.0)


include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

add_executable(
    ${PROJECT_NAME}
    test_v1_utils_uniquelist.cpp
    test_v1_utils_unique_array_list.cpp
)
target_link_libraries(
    ${PROJECT_NAME}
    GTest::gtest_main
    uniquelist::uniquelist
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
