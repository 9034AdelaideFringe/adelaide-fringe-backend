cmake_minimum_required(VERSION 3.5)

project(fringe LANGUAGES CXX)

# 添加Homebrew安装的库路径
# set(CMAKE_PREFIX_PATH 
#     "/opt/homebrew/opt/libpq"
#     "/opt/homebrew/opt/ossp-uuid"
#     "/opt/homebrew/opt/libpqxx"
#     ${CMAKE_PREFIX_PATH}
# )

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(server)

target_include_directories(server PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    # "/opt/homebrew/opt/libpq/include"
    # "/opt/homebrew/opt/ossp-uuid/include"
    # "/opt/homebrew/opt/libpqxx/include"
)

file(GLOB_RECURSE SRC ${CMAKE_SOURCE_DIR}/src/*.cpp)

target_sources(server PRIVATE ${SRC} main.cpp)

find_package(OpenSSL REQUIRED)
# find_package(UUID REQUIRED)

# target_link_directories(server PRIVATE
#     "/opt/homebrew/opt/libpq/lib"
#     "/opt/homebrew/opt/ossp-uuid/lib"
#     "/opt/homebrew/opt/libpqxx/lib"
# )

target_link_libraries(server PRIVATE pq OpenSSL::Crypto uuid pqxx)

# target_compile_definitions(server PRIVATE JWT_DISABLE_PICOJSON)

add_custom_command(TARGET server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:server> ${CMAKE_SOURCE_DIR}/server
    COMMENT "Copying server executable to the top-level directory"
)

# add_subdirectory(tests)

