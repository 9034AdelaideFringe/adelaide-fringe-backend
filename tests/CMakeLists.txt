cmake_minimum_required(VERSION 3.5)
project(fringe_tests LANGUAGES CXX)

# 启用测试
enable_testing()

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加Homebrew安装的库路径
set(CMAKE_PREFIX_PATH 
    "/opt/homebrew/opt/libpq"
    "/opt/homebrew/opt/ossp-uuid"
    "/opt/homebrew/opt/libpqxx"
    ${CMAKE_PREFIX_PATH}
)

# 添加GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.11.0
)
FetchContent_MakeAvailable(googletest)

# 包含头文件目录
include_directories(
  ${CMAKE_SOURCE_DIR}/include
  ${gtest_SOURCE_DIR}/include
  ${gmock_SOURCE_DIR}/include
  "/opt/homebrew/opt/libpq/include"
  "/opt/homebrew/opt/ossp-uuid/include"
  "/opt/homebrew/opt/libpqxx/include"
)

# 单元测试
# 收集测试源文件
file(GLOB_RECURSE TEST_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/unit/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/unit/**/*.cpp
)

# 创建测试可执行文件
add_executable(unit_tests ${TEST_SOURCES} main.cpp)

# 添加链接目录
target_link_directories(unit_tests PRIVATE
  "/opt/homebrew/opt/libpq/lib"
  "/opt/homebrew/opt/ossp-uuid/lib"
  "/opt/homebrew/opt/libpqxx/lib"
)

# 链接GoogleTest和项目库
target_link_libraries(unit_tests
  gtest
  gmock
  gtest_main
  pq
  OpenSSL::Crypto
  uuid
  pqxx
)

# 添加测试
add_test(NAME unit_tests COMMAND unit_tests)

# 创建简单测试可执行文件
add_executable(simple_test simple_test.cpp)

# 添加链接目录
target_link_directories(simple_test PRIVATE
  "/opt/homebrew/opt/libpq/lib"
  "/opt/homebrew/opt/ossp-uuid/lib"
  "/opt/homebrew/opt/libpqxx/lib"
)

# 链接GoogleTest
target_link_libraries(simple_test
  gtest
  gmock
  gtest_main
)

# 添加简单测试
add_test(NAME simple_test COMMAND simple_test)

# 添加集成测试子目录
add_subdirectory(integration) 