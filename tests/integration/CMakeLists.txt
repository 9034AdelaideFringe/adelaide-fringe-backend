cmake_minimum_required(VERSION 3.5)

# 收集集成测试源文件
file(GLOB_RECURSE INTEGRATION_TEST_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp
)

# 创建集成测试可执行文件
add_executable(integration_tests ${INTEGRATION_TEST_SOURCES})

# 添加链接目录
target_link_directories(integration_tests PRIVATE
  "/opt/homebrew/opt/libpq/lib"
  "/opt/homebrew/opt/ossp-uuid/lib"
  "/opt/homebrew/opt/libpqxx/lib"
)

# 链接GoogleTest和项目库
target_link_libraries(integration_tests
  gtest
  gmock
  gtest_main
  pq
  OpenSSL::Crypto
  uuid
  pqxx
)

# 包含头文件目录
target_include_directories(integration_tests PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  "/opt/homebrew/opt/libpq/include"
  "/opt/homebrew/opt/ossp-uuid/include"
  "/opt/homebrew/opt/libpqxx/include"
)

# 添加测试
add_test(NAME integration_tests COMMAND integration_tests) 