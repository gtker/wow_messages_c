add_executable(wow_messages_vanilla src/vanilla.main.c)
target_link_libraries(wow_messages_vanilla
        PRIVATE
        wow_login_messages_c::wow_login_messages_c
        wow_world_messages_c::wow_world_messages_c
)
set_target_properties(wow_messages_vanilla
        PROPERTIES
        C_EXTENSIONS ON
)
target_compile_features(wow_messages_vanilla PUBLIC c_std_99)

include(FetchContent)
FetchContent_Declare(
        wow_srp
        GIT_REPOSITORY https://github.com/gtker/wow_srp_c.git
        GIT_TAG 95316394d2f60a8c7004aba88f7e9a8651a8bf38
)
FetchContent_MakeAvailable(wow_srp)

target_link_libraries(wow_messages_vanilla PRIVATE WowSrp::wow_srp $<$<PLATFORM_ID:Windows>:ws2_32> $<$<PLATFORM_ID:Windows>:Bcrypt>)
