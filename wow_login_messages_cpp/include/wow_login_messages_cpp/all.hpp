#ifndef WOW_LOGIN_MESSAGES_CPP_ALL_HPP
#define WOW_LOGIN_MESSAGES_CPP_ALL_HPP

#include "wow_login_messages_cpp/wow_login_messages.hpp"

namespace wow_login_messages::all {

enum class Locale : uint32_t {
    EN_GB = 1701726018,
    EN_US = 1701729619,
    ES_MX = 1702055256,
    PT_BR = 1886667346,
    FR_FR = 1718765138,
    DE_DE = 1684358213,
    ES_ES = 1702053203,
    PT_PT = 1886670932,
    IT_IT = 1769228628,
    RU_RU = 1920291413,
    KO_KR = 1802455890,
    ZH_TW = 2053657687,
    EN_TW = 1701729367,
    EN_CN = 1701725006,
};

enum class Os : uint32_t {
    WINDOWS = 5728622,
    MAC_OS_X = 5198680,
};

enum class Platform : uint32_t {
    X86 = 7878710,
    POWER_PC = 5263427,
};

enum class ProtocolVersion : uint8_t {
    TWO = 2,
    THREE = 3,
    FIVE = 5,
    SIX = 6,
    SEVEN = 7,
    EIGHT = 8,
};

struct Version {
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
    uint16_t build;
};

struct CMD_AUTH_LOGON_CHALLENGE_Client {
    ProtocolVersion protocol_version;
    Version version;
    Platform platform;
    Os os;
    Locale locale;
    int32_t utc_timezone_offset;
    uint32_t client_ip_address;
    std::string account_name;

    std::vector<unsigned char> write() const;
};

struct CMD_AUTH_RECONNECT_CHALLENGE_Client {
    ProtocolVersion protocol_version;
    Version version;
    Platform platform;
    Os os;
    Locale locale;
    int32_t utc_timezone_offset;
    uint32_t client_ip_address;
    std::string account_name;

    std::vector<unsigned char> write() const;
};

using ClientOpcode = std::variant<
    CMD_AUTH_LOGON_CHALLENGE_Client,
    CMD_AUTH_RECONNECT_CHALLENGE_Client
>;

std::vector<unsigned char> write_opcode(const ClientOpcode& opcode);

std::optional<ClientOpcode> read_client_opcode(Reader& reader);

} /* namespace wow_login_messages::all */
#endif /* WOW_LOGIN_MESSAGES_ALL_HPP */
