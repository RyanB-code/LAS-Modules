cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_CXX_STANDARD		23)
set(CMAKE_CXX_STANDARD_REQUIRED	True)

set(PROJECT_NAME		"ShooterCentral")
set(LIB_NAME			"${PROJECT_NAME}.lasm")

project(${PROJECT_NAME} VERSION 0.6.0)


# All CPP files
file(GLOB sources CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp" )

# Add lib
add_library(${PROJECT_NAME} SHARED ${sources})

set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY	"${CMAKE_SOURCE_DIR}/bin")  # Set output dir
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_NAME "name")                            # Leave as name.so to be renamed later

# Add headers
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include")

# Link to libLAS-SDK-dev.so libraries
target_link_libraries(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/lib/libLAS-SDK-dev.so")

# Post build commands
add_custom_command( 
    TARGET ${PROJECT_NAME}
    POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E rename "${CMAKE_SOURCE_DIR}/bin/libname.so" "${CMAKE_SOURCE_DIR}/bin/${PROJECT_NAME}.lasm"     # Rename temporary file 
    COMMENT "Renaming library file after build"
)
