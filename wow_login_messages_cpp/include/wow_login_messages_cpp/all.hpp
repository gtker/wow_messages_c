#ifndef WOW_LOGIN_MESSAGES_CPP_ALL_HPP
#define WOW_LOGIN_MESSAGES_CPP_ALL_HPP

/* clang-format off */

#include "wow_login_messages_cpp/wow_login_messages.hpp"

namespace wow_login_messages {
namespace all {

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
    all::Version version;
    Platform platform;
    Os os;
    Locale locale;
    int32_t utc_timezone_offset;
    uint32_t client_ip_address;
    std::string account_name;

    WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write() const;
};

struct CMD_AUTH_RECONNECT_CHALLENGE_Client {
    ProtocolVersion protocol_version;
    all::Version version;
    Platform platform;
    Os os;
    Locale locale;
    int32_t utc_timezone_offset;
    uint32_t client_ip_address;
    std::string account_name;

    WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write() const;
};

struct ClientOpcode {
    enum class Opcode {
        NONE = 0xFF,
        CMD_AUTH_LOGON_CHALLENGE = 0,
        CMD_AUTH_RECONNECT_CHALLENGE = 2,
    } opcode;

    union {
        all::CMD_AUTH_LOGON_CHALLENGE_Client CMD_AUTH_LOGON_CHALLENGE;
        all::CMD_AUTH_RECONNECT_CHALLENGE_Client CMD_AUTH_RECONNECT_CHALLENGE;
    };

    bool is_none() const noexcept {
        return opcode == Opcode::NONE;
    }

    explicit ClientOpcode() : opcode(Opcode::NONE), CMD_AUTH_LOGON_CHALLENGE() {}

    ClientOpcode(ClientOpcode&& other) noexcept {
        this->opcode = other.opcode;
        other.opcode = Opcode::NONE;
        if (opcode == Opcode::CMD_AUTH_LOGON_CHALLENGE) {
            this->CMD_AUTH_LOGON_CHALLENGE = std::move(other.CMD_AUTH_LOGON_CHALLENGE);
        }
        if (opcode == Opcode::CMD_AUTH_RECONNECT_CHALLENGE) {
            this->CMD_AUTH_RECONNECT_CHALLENGE = std::move(other.CMD_AUTH_RECONNECT_CHALLENGE);
        }
    }

    ~ClientOpcode() {
        if (opcode == Opcode::CMD_AUTH_LOGON_CHALLENGE) {
            this->CMD_AUTH_LOGON_CHALLENGE.~CMD_AUTH_LOGON_CHALLENGE_Client();
        }
        if (opcode == Opcode::CMD_AUTH_RECONNECT_CHALLENGE) {
            this->CMD_AUTH_RECONNECT_CHALLENGE.~CMD_AUTH_RECONNECT_CHALLENGE_Client();
        }
    }

    explicit ClientOpcode(all::CMD_AUTH_LOGON_CHALLENGE_Client&& obj) {
        opcode = Opcode::CMD_AUTH_LOGON_CHALLENGE;
        new (&this->CMD_AUTH_LOGON_CHALLENGE) all::CMD_AUTH_LOGON_CHALLENGE_Client (std::move(obj));
    }
    explicit ClientOpcode(all::CMD_AUTH_RECONNECT_CHALLENGE_Client&& obj) {
        opcode = Opcode::CMD_AUTH_RECONNECT_CHALLENGE;
        new (&this->CMD_AUTH_RECONNECT_CHALLENGE) all::CMD_AUTH_RECONNECT_CHALLENGE_Client (std::move(obj));
    }

    template<typename T>
    // NOLINTNEXTLINE
    WOW_LOGIN_MESSAGES_CPP_EXPORT T& get(); // All possible types have been specialized

    template<typename T>
    // NOLINTNEXTLINE
    WOW_LOGIN_MESSAGES_CPP_EXPORT T* get_if(); // All possible types have been specialized

    WOW_LOGIN_MESSAGES_CPP_EXPORT const char* to_string() const;
};

template<>
WOW_LOGIN_MESSAGES_CPP_EXPORT all::CMD_AUTH_LOGON_CHALLENGE_Client* ClientOpcode::get_if();
template<>
WOW_LOGIN_MESSAGES_CPP_EXPORT all::CMD_AUTH_LOGON_CHALLENGE_Client& ClientOpcode::get();
template<>
WOW_LOGIN_MESSAGES_CPP_EXPORT all::CMD_AUTH_RECONNECT_CHALLENGE_Client* ClientOpcode::get_if();
template<>
WOW_LOGIN_MESSAGES_CPP_EXPORT all::CMD_AUTH_RECONNECT_CHALLENGE_Client& ClientOpcode::get();

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write_opcode(const ClientOpcode& opcode);

WOW_LOGIN_MESSAGES_CPP_EXPORT ClientOpcode read_client_opcode(Reader& reader);

} // namespace all
} // namespace wow_login_messages
#endif /* WOW_LOGIN_MESSAGES_CPP_ALL_HPP */
