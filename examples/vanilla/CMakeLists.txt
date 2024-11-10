if (UNIX)
    add_executable(WowMessages_vanilla src/vanilla.main.c)
    target_link_libraries(WowMessages_vanilla PRIVATE WowLoginMessages::WowLoginMessages)
    set_target_properties(WowMessages_vanilla
            PROPERTIES
            C_EXTENSIONS ON
    )

    include(FetchContent)
    FetchContent_Declare(
            wow_srp
            GIT_REPOSITORY https://github.com/gtker/wow_srp_c.git
            GIT_TAG 0a41248d8ae493c9960503449509f5009aa46e62
    )
    FetchContent_MakeAvailable(wow_srp)

    target_link_libraries(WowMessages_vanilla PRIVATE WowSrp::wow_srp)

endif ()