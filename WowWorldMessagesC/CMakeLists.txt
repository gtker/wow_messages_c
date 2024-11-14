cmake_minimum_required(VERSION 3.28)
project(WowWorldMessagesC LANGUAGES C VERSION 0.1.0)

add_library(WowWorldMessagesC_WowWorldMessagesC src/vanilla.c src/all.c src/util.c)
target_sources(WowWorldMessagesC_WowWorldMessagesC
        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include "${CMAKE_CURRENT_BINARY_DIR}"
        FILES
        include/wow_world_messages/wow_world_messages.h
        "${CMAKE_CURRENT_BINARY_DIR}/wowworldmessagesc_export.h"
)
add_library(WowWorldMessagesC::WowWorldMessagesC ALIAS WowWorldMessagesC_WowWorldMessagesC)
target_compile_features(WowWorldMessagesC_WowWorldMessagesC PUBLIC c_std_90)
set_target_properties(WowWorldMessagesC_WowWorldMessagesC
        PROPERTIES
        C_EXTENSIONS OFF

        C_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
        # CMake target is prefixed with project to avoid clashes
        OUTPUT_NAME WowWorldMessages
        EXPORT_NAME WowWorldMessagesC

        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
)
include(GenerateExportHeader)
generate_export_header(WowWorldMessagesC_WowWorldMessagesC
        BASE_NAME WowWorldMessagesC
)

include(CMakePackageConfigHelpers)
install(TARGETS WowWorldMessagesC_WowWorldMessagesC EXPORT WowWorldMessagesC FILE_SET HEADERS)
install(EXPORT WowWorldMessagesC NAMESPACE WowWorldMessagesC:: DESTINATION lib/cmake/WowWorldMessagesC)
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/WowWorldMessagesCConfigVersion.cmake"
        VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
        COMPATIBILITY AnyNewerVersion
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/WowWorldMessagesCConfigVersion.cmake" DESTINATION lib/cmake/WowWorldMessagesC)

if (BUILD_TESTING)
    add_executable(WowWorldMessages_vanilla.test src/vanilla.test.c)
    target_link_libraries(WowWorldMessages_vanilla.test PRIVATE WowMessages_TestUtils)
    add_test(NAME WowWorldMessages_vanilla.test COMMAND WowWorldMessages_vanilla.test)
endif ()