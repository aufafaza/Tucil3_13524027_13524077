cmake_minimum_required(VERSION 3.16) 
project (sliding VERSION 1.0.0 LANGUAGES CXX) 
set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (MSVC)
    add_compile_options(/W4)
else() 
    add_compile_options(-Wall -Wextra -pedantic)
endif() 

add_library( 
    sliding_core STATIC 
    src/core/graph.cpp
)

add_library( 
    sliding_utils STATIC 
    src/utils/fileIO.cpp
)

add_executable(sliding src/main.cpp) 

target_include_directories(sliding_utils PUBLIC include)
target_include_directories(sliding_core PUBLIC include)

target_link_libraries(sliding_core PUBLIC sliding_utils)

add_executable(test_graph tests/test_graph.cpp)
target_link_libraries(test_graph PRIVATE sliding_core)

target_link_libraries(sliding PRIVATE sliding_core)