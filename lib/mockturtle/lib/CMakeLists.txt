add_library(sparsepp INTERFACE) # v1.1
target_include_directories(sparsepp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/sparsepp)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "8.1.0")
  target_compile_options(sparsepp INTERFACE -Wno-class-memaccess)
endif()

add_library(ez INTERFACE) # v1.0
target_include_directories(ez INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/ez)

add_library(mockturtle_fmt INTERFACE)
target_include_directories(mockturtle_fmt INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/fmt)
target_compile_definitions(mockturtle_fmt INTERFACE FMT_HEADER_ONLY)

add_library(kitty INTERFACE) # v0.4
target_include_directories(kitty INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/kitty)

add_library(rang INTERFACE)
target_include_directories(rang INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/rang)

add_library(lorina INTERFACE) # v0.1
target_include_directories(lorina INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/lorina)
target_link_libraries(lorina INTERFACE rang mockturtle_fmt)
