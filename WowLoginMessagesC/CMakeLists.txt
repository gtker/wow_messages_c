cmake_minimum_required(VERSION 3.28)
project(WowWorldMessagesC LANGUAGES C VERSION 0.1.0)

add_library(WowLoginMessagesC_WowLoginMessagesC src/login.c)
target_sources(WowLoginMessagesC_WowLoginMessagesC
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
        "${CMAKE_CURRENT_BINARY_DIR}/wowloginmessagesc_export.h"
)
add_library(WowLoginMessagesC::WowLoginMessagesC ALIAS WowLoginMessagesC_WowLoginMessagesC)
target_compile_features(WowLoginMessagesC_WowLoginMessagesC PUBLIC c_std_90)
set_target_properties(WowLoginMessagesC_WowLoginMessagesC
        PROPERTIES
        C_EXTENSIONS OFF

        C_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
        # CMake target is prefixed with project to avoid clashes
        OUTPUT_NAME WowLoginMessagesC
        EXPORT_NAME WowLoginMessagesC

        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
)
include(GenerateExportHeader)
generate_export_header(WowLoginMessagesC_WowLoginMessagesC
        BASE_NAME WowLoginMessagesC
)

include(CMakePackageConfigHelpers)
install(TARGETS WowLoginMessagesC_WowLoginMessagesC EXPORT WowLoginMessagesC FILE_SET HEADERS)
install(EXPORT WowLoginMessagesC NAMESPACE WowLoginMessagesC:: DESTINATION lib/cmake/WowLoginMessagesC)
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/WowLoginMessagesCConfigVersion.cmake"
        VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
        COMPATIBILITY AnyNewerVersion
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/WowLoginMessagesCConfigVersion.cmake" DESTINATION lib/cmake/WowLoginMessagesC)

if (BUILD_TESTING)
    add_executable(WowLoginMessagesC_all.test src/all.test.c)
    target_link_libraries(WowLoginMessagesC_all.test PRIVATE WowMessages_TestUtils)
    add_test(NAME WowLoginMessagesC_all.test COMMAND WowLoginMessagesC_all.test)
endif ()
