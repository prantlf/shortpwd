cmake_minimum_required(VERSION 3.10)

file(READ "CHANGELOG.md" VER)
string(REGEX MATCH "## ([.0-9]*)" _ ${VER})
set(VER ${CMAKE_MATCH_1})
project(shortpwd VERSION ${VER})

if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  set(default_build_type "Debug")
else()
  set(default_build_type "Release")
endif()
 if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

configure_file(src/${CMAKE_PROJECT_NAME}.h.in ${CMAKE_PROJECT_NAME}.h)
include_directories(${CMAKE_BINARY_DIR})
add_executable(${CMAKE_PROJECT_NAME} src/${CMAKE_PROJECT_NAME}.c)
if(MSVC)
  target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC /MT)
  target_link_options(${CMAKE_PROJECT_NAME} PUBLIC /INCREMENTAL:NO /NODEFAULTLIB:MSVCRT)
elseif(NOT APPLE)
  target_link_libraries(${CMAKE_PROJECT_NAME} -static)
endif()

install(TARGETS ${CMAKE_PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES src/${CMAKE_PROJECT_NAME}.1 DESTINATION share/man/man1)

if(WIN32)
  set(CPACK_GENERATOR "ZIP")
  set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${VER}-windows-x86_64")
elseif(APPLE)
  set(CPACK_GENERATOR "TGZ")
  set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${VER}-darwin-x86_64")
else()
  set(CPACK_GENERATOR "TXZ")
  set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${VER}-linux-x86_64")
endif()
set(CPACK_PACKAGE_CHECKSUM "SHA256")
set(CPACK_STRIP_FILES TRUE)
include(CPack)

add_test(NAME all
  COMMAND ${CMAKE_COMMAND} "-DSRC=${PROJECT_SOURCE_DIR}" "-DBIN=${PROJECT_BINARY_DIR}" "-DEXE=${CMAKE_PROJECT_NAME}" -P "${PROJECT_SOURCE_DIR}/test/test.cmake")
include(CTest)

set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY ADDITIONAL_CLEAN_FILES
  ${CMAKE_PROJECT_NAME}.dSYM _CPack_Packages Testing actual.txt)
