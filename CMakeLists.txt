# Distributed under the MIT License (See accompanying file /LICENSE )

# CMake build : global project

cmake_minimum_required (VERSION 3.3)
project (header_only_lib VERSION 0.0.1 LANGUAGES CXX)
include(CMakeDependentOption)
include(GNUInstallDirs)

set_property (GLOBAL PROPERTY USE_FOLDERS ON)

#---------------------------------------------------------------------------------------
# compiler config
#---------------------------------------------------------------------------------------
set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

#---------------------------------------------------------------------------------------
# threads
#---------------------------------------------------------------------------------------
set (THREADS_PREFER_PTHREAD_FLAG ON)
find_package (Threads REQUIRED)

#---------------------------------------------------------------------------------------
# set default build to Debug
#---------------------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose Release or Debug" FORCE)
endif()

message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

add_library(header_only_lib INTERFACE)
add_library(header_only_lib::header_only_lib ALIAS header_only_lib)

# Check if header_only_lib is being used directly or via add_subdirectory
set(HOL_MASTER_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(HOL_MASTER_PROJECT ON)
endif()

option(HOL_BUILD_EXAMPLES "Build examples" ${HOL_MASTER_PROJECT})
option(HOL_BUILD_TESTS "Build tests" ${HOL_MASTER_PROJECT})
option(HOL_INSTALL "Generate the install target" ${HOL_MASTER_PROJECT})

target_include_directories(
    header_only_lib
    INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

add_subdirectory (third_party EXCLUDE_FROM_ALL)
include_directories (${THIRD_PARTY_INCLUDE_PATH})
message(STATUS "THIRD_PARTY_INCLUDE_PATH: " ${THIRD_PARTY_INCLUDE_PATH})

set(HEADER_BASE "${CMAKE_CURRENT_SOURCE_DIR}/include")
message(STATUS "HEADER_BASE: " ${HEADER_BASE})

if(HOL_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(HOL_BUILD_TESTS)
    include(CTest)
    add_subdirectory(tests)
	enable_testing()

	option (CODE_COVERAGE "Enable coverage reporting" OFF)
	if (CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
		# Add required flags (GCC & LLVM/Clang)
		target_compile_options(header_only_lib INTERFACE
		-O0        # no optimization
		-g         # generate debug info
		--coverage # sets all required flags
		)
		target_link_libraries(header_only_lib INTERFACE --coverage)
	endif()

endif()

enable_testing ()

#---------------------------------------------------------------------------------------
# Install/export targets and files
#---------------------------------------------------------------------------------------
if(HOL_INSTALL)
    # set files and directories
    set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
    set(include_install_dir "${CMAKE_INSTALL_INCLUDEDIR}")
    set(pkgconfig_install_dir "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
    set(version_config "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake")
    set(project_config "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake")
    set(targets_config "${PROJECT_NAME}Targets.cmake")
    set(pkg_config "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc")
    set(targets_export_name "${PROJECT_NAME}Targets")
    set(namespace "${PROJECT_NAME}::")

    # generate package version file
    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
        "${version_config}" COMPATIBILITY SameMajorVersion
    )

    # configure pkg config file
    configure_file("cmake/header_only_lib.pc.in" "${pkg_config}" @ONLY)
    # configure header_only_lib.cmake file
    configure_file("cmake/Config.cmake.in" "${project_config}" @ONLY)

    # install targets
    install(
        TARGETS header_only_lib
        EXPORT "${targets_export_name}"
    )

    # install headers
    install(
        DIRECTORY "${HEADER_BASE}/${PROJECT_NAME}"
        DESTINATION "${include_install_dir}"
    )

    # install project config and version file
    install(
        FILES "${project_config}" "${version_config}"
        DESTINATION "${config_install_dir}"
    )

    # install pkg config file
    install(
        FILES "${pkg_config}"
        DESTINATION "${pkgconfig_install_dir}"
    )

    # install targets config file
    install(
        EXPORT "${targets_export_name}"
        NAMESPACE "${namespace}"
        DESTINATION "${config_install_dir}"
        FILE ${targets_config}
    )

	# export build directory targets file
	export(
		EXPORT ${targets_export_name}
		NAMESPACE "${namespace}"
		FILE ${targets_config}
	)

	# register project in CMake user registry
	export(PACKAGE ${PROJECT_NAME})

endif()

file(GLOB_RECURSE header_only_lib_include_SRCS "${HEADER_BASE}/*.h")
add_custom_target(header_only_lib_headers_for_ide SOURCES ${header_only_lib_include_SRCS})

