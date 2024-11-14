cmake_minimum_required(VERSION 3.28)
project(WowMessages LANGUAGES C)

if (BUILD_TESTING)
    include(CTest)

    add_library(WowMessages_TestUtils TestUtils/test_utils.c)
    target_include_directories(WowMessages_TestUtils PUBLIC TestUtils)
    target_link_libraries(WowMessages_TestUtils PUBLIC WowLoginMessagesC::WowLoginMessagesC WowWorldMessagesC::WowWorldMessagesC)

    if (BUILD_EXAMPLES)
        add_subdirectory(examples)
    endif ()
endif ()

add_subdirectory(WowLoginMessagesC)
add_subdirectory(WowWorldMessagesC)
