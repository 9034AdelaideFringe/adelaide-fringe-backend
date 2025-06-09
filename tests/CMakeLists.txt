cmake_minimum_required(VERSION 3.10)
project(fringe_tests LANGUAGES CXX)

# 启用测试
enable_testing()

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 启用代码覆盖率
option(ENABLE_COVERAGE "Enable coverage reporting" ON)
if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

# 添加Homebrew安装的库路径
set(CMAKE_PREFIX_PATH 
    "/opt/homebrew/opt/libpq"
    "/opt/homebrew/opt/ossp-uuid"
    "/opt/homebrew/opt/libpqxx"
    ${CMAKE_PREFIX_PATH}
)

# 查找OpenSSL
find_package(OpenSSL REQUIRED)

# 添加GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

# 包含头文件目录
include_directories(
  ${CMAKE_SOURCE_DIR}/../include
  ${gtest_SOURCE_DIR}/include
  ${gmock_SOURCE_DIR}/include
  "/opt/homebrew/opt/libpq/include"
  "/opt/homebrew/opt/ossp-uuid/include"
  "/opt/homebrew/opt/libpqxx/include"
  "/opt/homebrew/opt/crow/include"
  "/opt/homebrew/opt/asio/include"
  "/opt/homebrew/opt/libjwt/include"
)

# 单元测试
# 收集测试源文件 (包含工具类和控制器测试)
file(GLOB_RECURSE TEST_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/unit/utils/HashTest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/unit/utils/ConfigTest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/unit/controller/EventControllerTest.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/unit/controller/TicketControllerTest.cpp
)

# 创建测试可执行文件
add_executable(unit_tests ${TEST_SOURCES} main.cpp
    ${CMAKE_SOURCE_DIR}/../src/utils/Hash.cpp
    ${CMAKE_SOURCE_DIR}/../src/utils/Config.cpp
    ${CMAKE_SOURCE_DIR}/../src/controller/event/EventController.cpp
    ${CMAKE_SOURCE_DIR}/../src/controller/ticket/TicketController.cpp
    ${CMAKE_SOURCE_DIR}/../src/service/EventService.cpp
    ${CMAKE_SOURCE_DIR}/../src/service/UploadService.cpp
    ${CMAKE_SOURCE_DIR}/../src/utils/jwt.cpp
    ${CMAKE_SOURCE_DIR}/../src/utils/uuid.cpp
    ${CMAKE_SOURCE_DIR}/../src/utils/response.cpp
    ${CMAKE_SOURCE_DIR}/../src/utils/Singleton.cpp
    ${CMAKE_SOURCE_DIR}/../src/service/UserService.cpp
    ${CMAKE_SOURCE_DIR}/../src/service/LoginService.cpp
    ${CMAKE_SOURCE_DIR}/../src/service/AuthService.cpp
    ${CMAKE_SOURCE_DIR}/../src/utils/PostgresConnection.cpp
)

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

# 创建独立工具类测试 (不依赖Crow)
add_executable(standalone_utils_test standalone_utils_test.cpp 
    ${CMAKE_SOURCE_DIR}/../src/utils/Config.cpp
    ${CMAKE_SOURCE_DIR}/../src/utils/Hash.cpp
)

target_link_libraries(standalone_utils_test
  gtest
  gmock
  gtest_main
  OpenSSL::Crypto
)

# 添加独立工具测试
add_test(NAME standalone_utils_test COMMAND standalone_utils_test)

# 添加集成测试子目录
add_subdirectory(integration) 