cmake_minimum_required(VERSION 3.10)
project(template)
include(CTest)

set(CMAKE_CXX_STANDARD 17)

# static checks
# if(MSVC)
#   add_compile_options(/W4 /WX)
# else()
#   add_compile_options(-Wall -Wextra -pedantic -Werror)
# endif()

# set(CMAKE_CXX_CLANG_TIDY
#   clang-tidy;
#   -checks=*;
# )

# set(CMAKE_CXX_CPPCHECK
#   cppcheck;
#   --error-exitcode=1;
# )


find_package(ZeroMQ REQUIRED)
find_package(cppzmq REQUIRED)


add_executable(chat_client src/chat_client.cpp)
add_executable(chat_server src/chat_server.cpp)
add_executable(main src/main.cpp)

target_link_libraries(chat_client cppzmq)
target_link_libraries(chat_server cppzmq)
include_directories(include)
