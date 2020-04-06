cmake_minimum_required(VERSION 3.2)

if($ENV{TRAVIS_BUILD_NUMBER})
    project(helloworld VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})
else()
    project(helloworld VERSION 0.0.42)
endif()

#====================================================================
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
endif()
#====================================================================

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

configure_file(version.h.in version.h)
configure_file(description.json.in description.json)

add_executable(helloworld_cli main.cpp)
add_library(helloworld STATIC lib.cpp)
add_executable(test_version test_version.cpp)
add_executable(gtest_version gtest_version.cpp)

set_target_properties(helloworld_cli helloworld test_version gtest_version PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)  

target_include_directories(helloworld
    PRIVATE "${CMAKE_BINARY_DIR}"
)

target_include_directories(gtest_version
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/googletest-src/googletest/include/
)

set_target_properties(test_version PROPERTIES
    COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
    INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
)

target_link_libraries(helloworld_cli PRIVATE
    helloworld
)

target_link_libraries(gtest_version PRIVATE 
    gtest_main gtest helloworld
)

target_link_libraries(test_version 
    ${Boost_LIBRARIES}
    helloworld
)

if(MSVC)
    target_compile_options(helloworld_cli PRIVATE
        /W4
    )
    target_compile_options(helloworld PRIVATE
        /W4
    )
    target_compile_options(test_version PRIVATE
        /W4
    )
    target_compile_options(gtest_version PRIVATE
        /W4
    )
else()
    target_compile_options(helloworld_cli PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(helloworld PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(test_version PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(gtest_version PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()

install(TARGETS helloworld_cli RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)

enable_testing()

add_test(test_version test_version)
add_test(gtest_version gtest_version)