cmake_minimum_required(VERSION 3.21)

project(
  hdtSMP-force-fields
  VERSION 0.9.1
  LANGUAGES CXX)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(FATAL_ERROR "In-source builds not allowed.")
endif()

# ============================================================
# Options
# ============================================================
option(ENABLE_SKYRIM_SE "Enable support for Skyrim SE" ON)
option(ENABLE_SKYRIM_AE "Enable support for Skyrim AE" ON)
option(ENABLE_SKYRIM_VR "Enable support for Skyrim VR" ON)

set(SKSE_SUPPORT_XBYAK ON)

# ============================================================
# Source files
# ============================================================
set(SOURCE_DIR "${PROJECT_SOURCE_DIR}/hdtSMP-force-fields")
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")

set(SOURCE_FILES
    "${SOURCE_DIR}/ForceFieldManager.cpp"
    "${SOURCE_DIR}/ForceFieldManager.h"
    "${SOURCE_DIR}/IForceField.h"
    "${SOURCE_DIR}/PluginHelper.h"
    "${SOURCE_DIR}/Settings.cpp"
    "${SOURCE_DIR}/Settings.h"
    "${SOURCE_DIR}/TForceField.h"
    "${SOURCE_DIR}/Timer.h"
    "${SOURCE_DIR}/Concepts.h"
    "${SOURCE_DIR}/get_member.h"
    "${SOURCE_DIR}/hooks.cpp"
    "${SOURCE_DIR}/hooks.h"
    "${SOURCE_DIR}/main.cpp"
    "${SOURCE_DIR}/memory.cpp"
    "${SOURCE_DIR}/memory.h"
    "${SOURCE_DIR}/objects.h"
    "${SOURCE_DIR}/utils.cpp"
    "${SOURCE_DIR}/utils.h"
    "${SOURCE_DIR}/concepts/Clock.h"
    "${SOURCE_DIR}/concepts/Force.h"
    "${SOURCE_DIR}/concepts/Geometry.h"
    "${SOURCE_DIR}/concepts/Transformable.h"
    "${SOURCE_DIR}/factories/Bomb.cpp"
    "${SOURCE_DIR}/factories/Bomb.h"
    "${SOURCE_DIR}/factories/Cylinder.cpp"
    "${SOURCE_DIR}/factories/Cylinder.h"
    "${SOURCE_DIR}/factories/Plane.h"
    "${SOURCE_DIR}/factories/Sphere.cpp"
    "${SOURCE_DIR}/factories/Sphere.h"
    "${SOURCE_DIR}/factories/Vortex.cpp"
    "${SOURCE_DIR}/factories/Vortex.h"
    "${SOURCE_DIR}/objects/Effect_traits.cpp"
    "${SOURCE_DIR}/objects/Effect_traits.h"
    "${SOURCE_DIR}/objects/Explosion_traits.h"
    "${SOURCE_DIR}/objects/Hazard_traits.h"
    "${SOURCE_DIR}/objects/ObjRef_traits.cpp"
    "${SOURCE_DIR}/objects/ObjRef_traits.h"
    "${SOURCE_DIR}/objects/Projectile_traits.h"
    "${SOURCE_DIR}/objects/obj_traits.h"
    "${INCLUDE_DIR}/PCH.h"
    "${INCLUDE_DIR}/hdtSMP64/PluginAPI.h")

source_group(TREE "${PROJECT_SOURCE_DIR}" FILES ${SOURCE_FILES})

# ============================================================
# Version header & resource
# ============================================================
set(VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/Plugin.h")
configure_file("${PROJECT_SOURCE_DIR}/cmake/Plugin.h.in" "${VERSION_HEADER}" @ONLY)

configure_file("${PROJECT_SOURCE_DIR}/cmake/version.rc.in"
               "${CMAKE_CURRENT_BINARY_DIR}/version.rc" @ONLY)

# ============================================================
# Target
# ============================================================
add_library(${PROJECT_NAME} SHARED
    ${SOURCE_FILES}
    "${VERSION_HEADER}"
    "${CMAKE_CURRENT_BINARY_DIR}/version.rc")

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    SKSE_SUPPORT_XBYAK
    BT_USE_SSE_IN_API
    _UNICODE
    UNICODE)

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE
      /utf-8
      /Zi
      /permissive-
      /Zc:preprocessor
      /bigobj
      "$<$<CONFIG:RELEASE>:/Zc:inline;/JMC-;/Ob3>")
  target_link_options(${PROJECT_NAME} PRIVATE
      "$<$<CONFIG:DEBUG>:/INCREMENTAL;/OPT:NOREF;/OPT:NOICF>"
      "$<$<CONFIG:RELEASE>:/INCREMENTAL:NO;/OPT:REF;/OPT:ICF;/DEBUG:FULL>")
endif()

# ============================================================
# CommonLibSSE – look in extern first, then env variable
# ============================================================
macro(find_dependency_path DEPENDENCY FILE)
  if(NOT ${DEPENDENCY} STREQUAL "")
    message(STATUS "Searching for ${DEPENDENCY} using file ${FILE}")
    find_path(PATH ${FILE}
      PATHS
        "${PROJECT_SOURCE_DIR}/../extern/${DEPENDENCY}"
        "${PROJECT_SOURCE_DIR}/extern/${DEPENDENCY}"
        "${PROJECT_SOURCE_DIR}/../external/${DEPENDENCY}"
        "${PROJECT_SOURCE_DIR}/external/${DEPENDENCY}")
    set("${DEPENDENCY}Path" "${PATH}")
    if("${${DEPENDENCY}Path}" STREQUAL "PATH-NOTFOUND")
      message(STATUS "Getting environment variable for ${DEPENDENCY}Path: $ENV{${DEPENDENCY}Path}")
      set("${DEPENDENCY}Path" "$ENV{${DEPENDENCY}Path}")
    endif()
    if(NOT "${${DEPENDENCY}Path}" STREQUAL "")
      message(STATUS "Found ${DEPENDENCY} in ${${DEPENDENCY}Path}; adding")
      add_subdirectory("${${DEPENDENCY}Path}" ${DEPENDENCY} EXCLUDE_FROM_ALL)
    endif()
  endif()
endmacro()

set(BUILD_TESTS OFF CACHE BOOL "" FORCE)

find_dependency_path(CommonLibSSE include/REL/Relocation.h)

if(("${CommonLibSSEPath}" STREQUAL "CommonLibSSEPath-NOTFOUND") OR "${CommonLibSSEPath}" STREQUAL "")
  if(NOT "$ENV{CommonLibSSEPath_NG}" STREQUAL "")
    message(STATUS "Found CommonLibSSE from CommonLibSSEPath_NG environment variable")
    add_subdirectory("$ENV{CommonLibSSEPath_NG}" CommonLibSSE EXCLUDE_FROM_ALL)
  else()
    message(FATAL_ERROR "CommonLibSSE not found. Set CommonLibSSEPath_NG or place it at extern/CommonLibSSE.")
  endif()
endif()

# ============================================================
# Vcpkg dependencies
# ============================================================
find_package(Bullet CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(xbyak REQUIRED CONFIG)
find_path(DETOURS_INCLUDE_DIRS "detours/detours.h")
find_path(CLIB_UTIL_INCLUDE_DIRS "ClibUtil/utils.hpp")

# Explicit release/debug detours to avoid vcpkg picking the wrong variant
if(DEFINED VCPKG_INSTALLED_DIR)
  set(_detours_installed "${VCPKG_INSTALLED_DIR}")
elseif(DEFINED _VCPKG_INSTALLED_DIR)
  set(_detours_installed "${_VCPKG_INSTALLED_DIR}")
endif()

if(DEFINED _detours_installed)
  find_library(DETOURS_LIBRARY_RELEASE NAMES detours NO_DEFAULT_PATH
    PATHS "${_detours_installed}/${VCPKG_TARGET_TRIPLET}/lib")
  find_library(DETOURS_LIBRARY_DEBUG NAMES detours NO_DEFAULT_PATH
    PATHS "${_detours_installed}/${VCPKG_TARGET_TRIPLET}/debug/lib")
  if(NOT DETOURS_LIBRARY_DEBUG)
    set(DETOURS_LIBRARY_DEBUG ${DETOURS_LIBRARY_RELEASE})
  endif()
  set(DETOURS_LINK optimized ${DETOURS_LIBRARY_RELEASE} debug ${DETOURS_LIBRARY_DEBUG})
else()
  find_library(DETOURS_LIBRARY detours REQUIRED)
  set(DETOURS_LINK ${DETOURS_LIBRARY})
endif()

# ============================================================
# Include dirs & link
# ============================================================
target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${SOURCE_DIR}"
    "${INCLUDE_DIR}"
    ${DETOURS_INCLUDE_DIRS}
    ${CLIB_UTIL_INCLUDE_DIRS}
    ${BULLET_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE
    CommonLibSSE::CommonLibSSE
    xbyak::xbyak
    spdlog::spdlog
    ${DETOURS_LINK}
    ${BULLET_LIBRARIES})

target_precompile_headers(${PROJECT_NAME} PRIVATE "${INCLUDE_DIR}/PCH.h")

# ============================================================
# Output name & install
# ============================================================
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "HDT-SMP Force Fields")

install(FILES "$<TARGET_FILE:${PROJECT_NAME}>" DESTINATION "SKSE/Plugins" COMPONENT "main")
install(FILES "$<TARGET_PDB_FILE:${PROJECT_NAME}>" DESTINATION "/" COMPONENT "pdbs")
