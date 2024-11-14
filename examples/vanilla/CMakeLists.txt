add_executable(WowMessages_vanilla src/vanilla.main.c)
target_link_libraries(WowMessages_vanilla PRIVATE WowLoginMessagesC::WowLoginMessagesC)
set_target_properties(WowMessages_vanilla
        PROPERTIES
        C_EXTENSIONS ON
)
target_compile_features(WowMessages_vanilla PUBLIC c_std_99)

include(FetchContent)
FetchContent_Declare(
        wow_srp
        GIT_REPOSITORY https://github.com/gtker/wow_srp_c.git
        GIT_TAG 0a41248d8ae493c9960503449509f5009aa46e62
)
FetchContent_MakeAvailable(wow_srp)

target_link_libraries(WowMessages_vanilla PRIVATE WowSrp::wow_srp $<$<PLATFORM_ID:Windows>:ws2_32> $<$<PLATFORM_ID:Windows>:Bcrypt>)