cmake_minimum_required(VERSION 3.29)
project(WowLoginMessages LANGUAGES C VERSION 0.1.0)

add_library(WowLoginMessages_WowLoginMessages src/login.c)
target_sources(WowLoginMessages_WowLoginMessages
        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include "${CMAKE_CURRENT_BINARY_DIR}"
        FILES
        include/wow_login_messages/wow_login_messages.h
        include/wow_login_messages/all.h
        include/wow_login_messages/version2.h
        include/wow_login_messages/version3.h
        include/wow_login_messages/version5.h
        include/wow_login_messages/version6.h
        include/wow_login_messages/version7.h
        include/wow_login_messages/version8.h
        "${CMAKE_CURRENT_BINARY_DIR}/wowloginmessages_export.h"
)
add_library(WowLoginMessages::WowLoginMessages ALIAS WowLoginMessages_WowLoginMessages)
target_compile_features(WowLoginMessages_WowLoginMessages PUBLIC c_std_99)
set_target_properties(WowLoginMessages_WowLoginMessages
        PROPERTIES
        C_EXTENSIONS OFF

        C_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
        # CMake target is prefixed with project to avoid clashes
        OUTPUT_NAME WowLoginMessages
)
include(GenerateExportHeader)
generate_export_header(WowLoginMessages_WowLoginMessages
        BASE_NAME WowLoginMessages
)

if (BUILD_TESTING)
    add_executable(WowLoginMessages_all.test src/all.test.c)
    target_link_libraries(WowLoginMessages_all.test PRIVATE WowMessages_TestUtils)
    add_test(NAME WowLoginMessages_all.test COMMAND WowLoginMessages_all.test)

    add_executable(WowLoginMessages_version2.test src/version2.test.c)
    target_link_libraries(WowLoginMessages_version2.test PRIVATE WowMessages_TestUtils)
    add_test(NAME WowLoginMessages_version2.test COMMAND WowLoginMessages_version2.test)

    add_executable(WowLoginMessages_version3.test src/version3.test.c)
    target_link_libraries(WowLoginMessages_version3.test PRIVATE WowMessages_TestUtils)
    add_test(NAME WowLoginMessages_version3.test COMMAND WowLoginMessages_version3.test)

    add_executable(WowLoginMessages_version5.test src/version5.test.c)
    target_link_libraries(WowLoginMessages_version5.test PRIVATE WowMessages_TestUtils)
    add_test(NAME WowLoginMessages_version5.test COMMAND WowLoginMessages_version5.test)

    add_executable(WowLoginMessages_version6.test src/version6.test.c)
    target_link_libraries(WowLoginMessages_version6.test PRIVATE WowMessages_TestUtils)
    add_test(NAME WowLoginMessages_version6.test COMMAND WowLoginMessages_version6.test)

    add_executable(WowLoginMessages_version7.test src/version7.test.c)
    target_link_libraries(WowLoginMessages_version7.test PRIVATE WowMessages_TestUtils)
    add_test(NAME WowLoginMessages_version7.test COMMAND WowLoginMessages_version7.test)

    add_executable(WowLoginMessages_version8.test src/version8.test.c)
    target_link_libraries(WowLoginMessages_version8.test PRIVATE WowMessages_TestUtils)
    add_test(NAME WowLoginMessages_version8.test COMMAND WowLoginMessages_version8.test)
endif ()