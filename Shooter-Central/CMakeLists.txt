cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_CXX_STANDARD		23)
set(CMAKE_CXX_STANDARD_REQUIRED	True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # So clangd lsp can find compile commands

set(PROJECT_NAME		"ShooterCentral")
set(LIB_NAME			"${PROJECT_NAME}.lasm")

project(${PROJECT_NAME} VERSION 0.7.0)

# Setup config file with versioning
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/Config.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/include/Config.h")

# All CPP files
file(GLOB sources1 CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp" )
file(GLOB sources2 CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Backend/*.cpp" )
file(GLOB sources3 CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/UI/*.cpp" )
file(GLOB sources4 CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/UI/Screens/*.cpp" )


# Add lib
add_library(${PROJECT_NAME} SHARED ${sources1} ${sources2} ${sources3} ${sources4} )

set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY	"${CMAKE_SOURCE_DIR}/bin")  # Set output dir
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_NAME "name")                            # Leave as name.so to be renamed later

# Add headers
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    option(DEBUG "DEBUG" ON)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(DEBUG)
    # Link to libLAS-SDK-dev.so libraries
    target_link_libraries(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/lib/libLAS-SDK-dev.so")
else()
    # Link to libLAS-SDK-dev.so libraries
    target_link_libraries(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/lib/libLAS-SDK.so")
endif()


if(DEBUG)
    add_custom_command( 
        TARGET ${PROJECT_NAME}
        POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E rename "${CMAKE_SOURCE_DIR}/bin/libname.so" "${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME}-dev.lasm"  # Rename temporary file 
        COMMAND ${CMAKE_COMMAND} -E rm -f   "${CMAKE_SOURCE_DIR}/include/Config.h"                                                  # Remove temporary file 
        COMMENT "Removed temporary Config.h file and renamed shared object file"
    )
else()
    add_custom_command( 
        TARGET ${PROJECT_NAME}
        POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E rename "${CMAKE_SOURCE_DIR}/bin/libname.so" "${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME}.lasm"      # Rename temporary file 
        COMMAND ${CMAKE_COMMAND} -E rm -f   "${CMAKE_SOURCE_DIR}/include/Config.h"                                                  # Remove temporary file 
        COMMENT "Removed temporary Config.h file and renamed shared object file"
    )
endif()
