cmake_minimum_required(VERSION 3.16) 
project (sliding VERSION 1.0.0 LANGUAGES CXX) 
set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (MSVC)
    add_compile_options(/W4)
else() 
    add_compile_options(-Wall -Wextra -pedantic)
endif() 

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_library( 
    sliding_core STATIC 
    src/core/graph.cpp
)

add_library( 
    sliding_utils STATIC 
    src/utils/fileIO.cpp
)

add_library(
    sliding_ui STATIC
    src/ui/snapshot.cpp
    src/ui/gameui.cpp
    src/ui/grideditor.cpp
)

add_executable(sliding src/main.cpp) 

target_include_directories(sliding_utils PUBLIC include)
target_include_directories(sliding_core PUBLIC include)
target_include_directories(sliding_ui PUBLIC include)

target_link_libraries(sliding_core PUBLIC sliding_utils)


target_link_libraries(sliding_ui PUBLIC sliding_core sfml-graphics sfml-window sfml-system)



target_link_libraries(sliding PRIVATE sliding_ui)