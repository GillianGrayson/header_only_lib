project("header_only_lib-test" CXX)

#set includes
include_directories(${TEST_THIRD_PARTY_INCLUDE_PATH})

#set test sources
file (GLOB TEST_SOURCE_FILES "*.cpp")

#set target executable
add_executable (${PROJECT_NAME} ${TEST_SOURCE_FILES})

#add the library
target_link_libraries (${PROJECT_NAME} PRIVATE Threads::Threads)
target_link_libraries(${PROJECT_NAME} PRIVATE header_only_lib::header_only_lib)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/logs")

# Turn on CMake testing capabilities
enable_testing()

#parse catch tests
ParseAndAddCatchTests (${PROJECT_NAME})
