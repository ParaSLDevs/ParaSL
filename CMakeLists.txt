project(parasl)

cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

add_subdirectory(syntax_tree_nodes)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_definitions( -std=c++11 )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_definitions( -ftemplate-depth=300 )
    add_definitions( -std=c++0x )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    add_definitions( /wd4996 )
endif()

find_package(Boost 1.60 REQUIRED)
if(Boost_FOUND)
    # Enable V3 of Phoenix by default. V2 has some issues with C++11
    # compilers.
    add_definitions( -DBOOST_SPIRIT_USE_PHOENIX_V3=1 )
    include_directories( SYSTEM ${Boost_INCLUDE_DIRS} )
else()
    message(WARNING "Boost not found. Cannot compile parser.")
endif()

add_subdirectory(parser)

add_executable(parasl main.cpp)
target_link_libraries(parasl parser ast)
