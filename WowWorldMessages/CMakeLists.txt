cmake_minimum_required(VERSION 3.29)
project(WowWorldMessages C)

add_library(WowWorldMessages_WowWorldMessages src/vanilla.c src/all.c src/util.c)
target_sources(WowWorldMessages_WowWorldMessages
        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include "${CMAKE_CURRENT_BINARY_DIR}"
        FILES
        include/wow_world_messages/wow_world_messages.h
        "${CMAKE_CURRENT_BINARY_DIR}/wowworldmessages_export.h"
)
add_library(WowWorldMessages::WowWorldMessages ALIAS WowWorldMessages_WowWorldMessages)
target_compile_features(WowWorldMessages_WowWorldMessages PUBLIC c_std_99)
set_target_properties(WowWorldMessages_WowWorldMessages
        PROPERTIES
        C_EXTENSIONS OFF

        C_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
        # CMake target is prefixed with project to avoid clashes
        OUTPUT_NAME WowWorldMessages
)
include(GenerateExportHeader)
generate_export_header(WowWorldMessages_WowWorldMessages
        BASE_NAME WowWorldMessages
)

if (BUILD_TESTING)
    add_executable(WowWorldMessages_vanilla.test src/vanilla.test.c)
    target_link_libraries(WowWorldMessages_vanilla.test PRIVATE WowMessages_TestUtils)
    add_test(NAME WowWorldMessages_vanilla.test COMMAND WowWorldMessages_vanilla.test)
endif ()