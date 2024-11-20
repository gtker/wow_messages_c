#ifndef WOW_LOGIN_MESSAGES_CPP_VERSION2_HPP
#define WOW_LOGIN_MESSAGES_CPP_VERSION2_HPP

#include "wow_login_messages_cpp/wow_login_messages.hpp"

namespace wow_login_messages::version2 {
#include "wow_login_messages_cpp/all.hpp" /* type include */

typedef all::Locale Locale;

enum class LoginResult : uint8_t {
    SUCCESS = 0,
    FAIL_UNKNOWN0 = 1,
    FAIL_UNKNOWN1 = 2,
    FAIL_BANNED = 3,
    FAIL_UNKNOWN_ACCOUNT = 4,
    FAIL_INCORRECT_PASSWORD = 5,
    FAIL_ALREADY_ONLINE = 6,
    FAIL_NO_TIME = 7,
    FAIL_DB_BUSY = 8,
    FAIL_VERSION_INVALID = 9,
    LOGIN_DOWNLOAD_FILE = 10,
    FAIL_INVALID_SERVER = 11,
    FAIL_SUSPENDED = 12,
    FAIL_NO_ACCESS = 13,
    SUCCESS_SURVEY = 14,
    FAIL_PARENTALCONTROL = 15,
};

typedef all::Os Os;

typedef all::Platform Platform;

typedef all::ProtocolVersion ProtocolVersion;

enum class RealmCategory : uint8_t {
    DEFAULT = 0,
    ONE = 1,
    TWO = 2,
    THREE = 3,
    FIVE = 5,
};

enum class RealmType : uint8_t {
    PLAYER_VS_ENVIRONMENT = 0,
    PLAYER_VS_PLAYER = 1,
    ROLEPLAYING = 6,
    ROLEPLAYING_PLAYER_VS_PLAYER = 8,
};

enum class RealmFlag : uint8_t {
    NONE = 0,
    INVALID = 1,
    OFFLINE = 2,
    FORCE_BLUE_RECOMMENDED = 32,
    FORCE_GREEN_RECOMMENDED = 64,
    FORCE_RED_FULL = 128,
};

struct Realm {
    RealmType realm_type;
    RealmFlag flag;
    std::string name;
    std::string address;
    float population;
    uint8_t number_of_characters_on_realm;
    RealmCategory category;
    uint8_t realm_id;
};

typedef all::Version Version;

struct TelemetryKey {
    uint16_t unknown1;
    uint32_t unknown2;
    std::array<uint8_t, 4> unknown3;
    std::array<uint8_t, 20> cd_key_proof;
};

struct CMD_AUTH_LOGON_CHALLENGE_Server {
    LoginResult result;
    std::array<uint8_t, 32> server_public_key;
    uint8_t generator_length;
    std::vector<uint8_t> generator;
    uint8_t large_safe_prime_length;
    std::vector<uint8_t> large_safe_prime;
    std::array<uint8_t, 32> salt;
    std::array<uint8_t, 16> crc_salt;

    std::vector<unsigned char> write() const;
};

typedef all::CMD_AUTH_LOGON_CHALLENGE_Client CMD_AUTH_LOGON_CHALLENGE_Client;

struct CMD_AUTH_LOGON_PROOF_Client {
    std::array<uint8_t, 32> client_public_key;
    std::array<uint8_t, 20> client_proof;
    std::array<uint8_t, 20> crc_hash;
    uint8_t number_of_telemetry_keys;
    std::vector<TelemetryKey> telemetry_keys;

    std::vector<unsigned char> write() const;
};

struct CMD_AUTH_LOGON_PROOF_Server {
    LoginResult result;
    std::array<uint8_t, 20> server_proof;
    uint32_t hardware_survey_id;

    std::vector<unsigned char> write() const;
};

struct CMD_AUTH_RECONNECT_CHALLENGE_Server {
    LoginResult result;
    std::array<uint8_t, 16> challenge_data;
    std::array<uint8_t, 16> checksum_salt;

    std::vector<unsigned char> write() const;
};

typedef all::CMD_AUTH_RECONNECT_CHALLENGE_Client CMD_AUTH_RECONNECT_CHALLENGE_Client;

struct CMD_AUTH_RECONNECT_PROOF_Server {
    LoginResult result;

    std::vector<unsigned char> write() const;
};

struct CMD_AUTH_RECONNECT_PROOF_Client {
    std::array<uint8_t, 16> proof_data;
    std::array<uint8_t, 20> client_proof;
    std::array<uint8_t, 20> client_checksum;

    std::vector<unsigned char> write() const;
};

struct CMD_REALM_LIST_Server {
    uint8_t number_of_realms;
    std::vector<Realm> realms;

    std::vector<unsigned char> write() const;
};

struct CMD_REALM_LIST_Client {

    std::vector<unsigned char> write() const;
};

struct CMD_XFER_INITIATE {
    std::string filename;
    uint64_t file_size;
    std::array<uint8_t, 16> file_md5;

    std::vector<unsigned char> write() const;
};

struct CMD_XFER_DATA {
    uint16_t size;
    std::vector<uint8_t> data;

    std::vector<unsigned char> write() const;
};

struct CMD_XFER_ACCEPT {

    std::vector<unsigned char> write() const;
};

struct CMD_XFER_RESUME {
    uint64_t offset;

    std::vector<unsigned char> write() const;
};

struct CMD_XFER_CANCEL {

    std::vector<unsigned char> write() const;
};

using ClientOpcode = std::variant<
    CMD_AUTH_LOGON_PROOF_Client,
    CMD_AUTH_RECONNECT_PROOF_Client,
    CMD_REALM_LIST_Client,
    CMD_XFER_ACCEPT,
    CMD_XFER_RESUME,
    CMD_XFER_CANCEL
>;

std::vector<unsigned char> write_opcode(const ClientOpcode& opcode);

std::optional<ClientOpcode> read_client_opcode(Reader& reader);

using ServerOpcode = std::variant<
    CMD_AUTH_LOGON_CHALLENGE_Server,
    CMD_AUTH_LOGON_PROOF_Server,
    CMD_AUTH_RECONNECT_CHALLENGE_Server,
    CMD_AUTH_RECONNECT_PROOF_Server,
    CMD_REALM_LIST_Server,
    CMD_XFER_INITIATE,
    CMD_XFER_DATA
>;

std::vector<unsigned char> write_opcode(const ServerOpcode& opcode);

std::optional<ServerOpcode> read_server_opcode(Reader& reader);

} /* namespace wow_login_messages::version2 */
#endif /* WOW_LOGIN_MESSAGES_VERSION2_HPP */
