#ifndef WOW_LOGIN_MESSAGES_CPP_VERSION2_HPP
#define WOW_LOGIN_MESSAGES_CPP_VERSION2_HPP

#include "wow_login_messages_cpp/wow_login_messages.hpp"

namespace wow_login_messages {
namespace version2 {

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

enum RealmFlag : uint8_t {
    REALM_FLAG_NONE = 0,
    REALM_FLAG_INVALID = 1,
    REALM_FLAG_OFFLINE = 2,
    REALM_FLAG_FORCE_BLUE_RECOMMENDED = 32,
    REALM_FLAG_FORCE_GREEN_RECOMMENDED = 64,
    REALM_FLAG_FORCE_RED_FULL = 128,
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

    WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write() const;
};

typedef all::CMD_AUTH_LOGON_CHALLENGE_Client CMD_AUTH_LOGON_CHALLENGE_Client;

struct CMD_AUTH_LOGON_PROOF_Client {
    std::array<uint8_t, 32> client_public_key;
    std::array<uint8_t, 20> client_proof;
    std::array<uint8_t, 20> crc_hash;
    uint8_t number_of_telemetry_keys;
    std::vector<version2::TelemetryKey> telemetry_keys;

    WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write() const;
};

struct CMD_AUTH_LOGON_PROOF_Server {
    LoginResult result;
    std::array<uint8_t, 20> server_proof;
    uint32_t hardware_survey_id;

    WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write() const;
};

struct CMD_AUTH_RECONNECT_CHALLENGE_Server {
    LoginResult result;
    std::array<uint8_t, 16> challenge_data;
    std::array<uint8_t, 16> checksum_salt;

    WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write() const;
};

typedef all::CMD_AUTH_RECONNECT_CHALLENGE_Client CMD_AUTH_RECONNECT_CHALLENGE_Client;

struct CMD_AUTH_RECONNECT_PROOF_Server {
    LoginResult result;

    WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write() const;
};

struct CMD_AUTH_RECONNECT_PROOF_Client {
    std::array<uint8_t, 16> proof_data;
    std::array<uint8_t, 20> client_proof;
    std::array<uint8_t, 20> client_checksum;

    WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write() const;
};

struct CMD_REALM_LIST_Server {
    uint8_t number_of_realms;
    std::vector<version2::Realm> realms;

    WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write() const;
};

struct CMD_REALM_LIST_Client {

    WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write() const;
};

struct CMD_XFER_INITIATE {
    std::string filename;
    uint64_t file_size;
    std::array<uint8_t, 16> file_md5;

    WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write() const;
};

struct CMD_XFER_DATA {
    uint16_t size;
    std::vector<uint8_t> data;

    WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write() const;
};

struct CMD_XFER_ACCEPT {

    WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write() const;
};

struct CMD_XFER_RESUME {
    uint64_t offset;

    WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write() const;
};

struct CMD_XFER_CANCEL {

    WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write() const;
};

struct ClientOpcode {
    enum class Opcode {
        NONE = 0xFF,
        CMD_AUTH_LOGON_PROOF = 1,
        CMD_AUTH_RECONNECT_PROOF = 3,
        CMD_REALM_LIST = 16,
        CMD_XFER_ACCEPT = 50,
        CMD_XFER_RESUME = 51,
        CMD_XFER_CANCEL = 52,
    } opcode;

    union {
        version2::CMD_AUTH_LOGON_PROOF_Client CMD_AUTH_LOGON_PROOF;
        version2::CMD_AUTH_RECONNECT_PROOF_Client CMD_AUTH_RECONNECT_PROOF;
        version2::CMD_REALM_LIST_Client CMD_REALM_LIST;
        version2::CMD_XFER_ACCEPT CMD_XFER_ACCEPT;
        version2::CMD_XFER_RESUME CMD_XFER_RESUME;
        version2::CMD_XFER_CANCEL CMD_XFER_CANCEL;
    };

    bool is_none() const noexcept {
        return opcode == Opcode::NONE;
    }

    explicit ClientOpcode() : opcode(Opcode::NONE), CMD_AUTH_LOGON_PROOF() {}

    ClientOpcode(ClientOpcode&& other) noexcept {
        this->opcode = other.opcode;
        other.opcode = Opcode::NONE;
        if (opcode == Opcode::CMD_AUTH_LOGON_PROOF) {
            this->CMD_AUTH_LOGON_PROOF = std::move(other.CMD_AUTH_LOGON_PROOF);
        }
        if (opcode == Opcode::CMD_AUTH_RECONNECT_PROOF) {
            this->CMD_AUTH_RECONNECT_PROOF = std::move(other.CMD_AUTH_RECONNECT_PROOF);
        }
        if (opcode == Opcode::CMD_REALM_LIST) {
            this->CMD_REALM_LIST = std::move(other.CMD_REALM_LIST);
        }
        if (opcode == Opcode::CMD_XFER_ACCEPT) {
            this->CMD_XFER_ACCEPT = std::move(other.CMD_XFER_ACCEPT);
        }
        if (opcode == Opcode::CMD_XFER_RESUME) {
            this->CMD_XFER_RESUME = std::move(other.CMD_XFER_RESUME);
        }
        if (opcode == Opcode::CMD_XFER_CANCEL) {
            this->CMD_XFER_CANCEL = std::move(other.CMD_XFER_CANCEL);
        }
    }

    ~ClientOpcode() {
        if (opcode == Opcode::CMD_AUTH_LOGON_PROOF) {
            this->CMD_AUTH_LOGON_PROOF.~CMD_AUTH_LOGON_PROOF_Client();
        }
        if (opcode == Opcode::CMD_AUTH_RECONNECT_PROOF) {
            this->CMD_AUTH_RECONNECT_PROOF.~CMD_AUTH_RECONNECT_PROOF_Client();
        }
        if (opcode == Opcode::CMD_REALM_LIST) {
            this->CMD_REALM_LIST.~CMD_REALM_LIST_Client();
        }
        if (opcode == Opcode::CMD_XFER_ACCEPT) {
            this->CMD_XFER_ACCEPT.~CMD_XFER_ACCEPT();
        }
        if (opcode == Opcode::CMD_XFER_RESUME) {
            this->CMD_XFER_RESUME.~CMD_XFER_RESUME();
        }
        if (opcode == Opcode::CMD_XFER_CANCEL) {
            this->CMD_XFER_CANCEL.~CMD_XFER_CANCEL();
        }
    }

    explicit ClientOpcode(version2::CMD_AUTH_LOGON_PROOF_Client&& obj) {
        opcode = Opcode::CMD_AUTH_LOGON_PROOF;
        new (&this->CMD_AUTH_LOGON_PROOF) version2::CMD_AUTH_LOGON_PROOF_Client (std::move(obj));
    }
    explicit ClientOpcode(version2::CMD_AUTH_RECONNECT_PROOF_Client&& obj) {
        opcode = Opcode::CMD_AUTH_RECONNECT_PROOF;
        new (&this->CMD_AUTH_RECONNECT_PROOF) version2::CMD_AUTH_RECONNECT_PROOF_Client (std::move(obj));
    }
    explicit ClientOpcode(version2::CMD_REALM_LIST_Client&& obj) {
        opcode = Opcode::CMD_REALM_LIST;
        new (&this->CMD_REALM_LIST) version2::CMD_REALM_LIST_Client (std::move(obj));
    }
    explicit ClientOpcode(version2::CMD_XFER_ACCEPT&& obj) {
        opcode = Opcode::CMD_XFER_ACCEPT;
        new (&this->CMD_XFER_ACCEPT) version2::CMD_XFER_ACCEPT (std::move(obj));
    }
    explicit ClientOpcode(version2::CMD_XFER_RESUME&& obj) {
        opcode = Opcode::CMD_XFER_RESUME;
        new (&this->CMD_XFER_RESUME) version2::CMD_XFER_RESUME (std::move(obj));
    }
    explicit ClientOpcode(version2::CMD_XFER_CANCEL&& obj) {
        opcode = Opcode::CMD_XFER_CANCEL;
        new (&this->CMD_XFER_CANCEL) version2::CMD_XFER_CANCEL (std::move(obj));
    }

    template<typename T>
    // NOLINTNEXTLINE
    T& get(); // All possible types have been specialized

    template<typename T>
    // NOLINTNEXTLINE
    T* get_if(); // All possible types have been specialized
};

template<>
version2::CMD_AUTH_LOGON_PROOF_Client* ClientOpcode::get_if();
template<>
version2::CMD_AUTH_LOGON_PROOF_Client& ClientOpcode::get();
template<>
version2::CMD_AUTH_RECONNECT_PROOF_Client* ClientOpcode::get_if();
template<>
version2::CMD_AUTH_RECONNECT_PROOF_Client& ClientOpcode::get();
template<>
version2::CMD_REALM_LIST_Client* ClientOpcode::get_if();
template<>
version2::CMD_REALM_LIST_Client& ClientOpcode::get();
template<>
version2::CMD_XFER_ACCEPT* ClientOpcode::get_if();
template<>
version2::CMD_XFER_ACCEPT& ClientOpcode::get();
template<>
version2::CMD_XFER_RESUME* ClientOpcode::get_if();
template<>
version2::CMD_XFER_RESUME& ClientOpcode::get();
template<>
version2::CMD_XFER_CANCEL* ClientOpcode::get_if();
template<>
version2::CMD_XFER_CANCEL& ClientOpcode::get();

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write_opcode(const ClientOpcode& opcode);

WOW_LOGIN_MESSAGES_CPP_EXPORT ClientOpcode read_client_opcode(Reader& reader);

struct ServerOpcode {
    enum class Opcode {
        NONE = 0xFF,
        CMD_AUTH_LOGON_CHALLENGE = 0,
        CMD_AUTH_LOGON_PROOF = 1,
        CMD_AUTH_RECONNECT_CHALLENGE = 2,
        CMD_AUTH_RECONNECT_PROOF = 3,
        CMD_REALM_LIST = 16,
        CMD_XFER_INITIATE = 48,
        CMD_XFER_DATA = 49,
    } opcode;

    union {
        version2::CMD_AUTH_LOGON_CHALLENGE_Server CMD_AUTH_LOGON_CHALLENGE;
        version2::CMD_AUTH_LOGON_PROOF_Server CMD_AUTH_LOGON_PROOF;
        version2::CMD_AUTH_RECONNECT_CHALLENGE_Server CMD_AUTH_RECONNECT_CHALLENGE;
        version2::CMD_AUTH_RECONNECT_PROOF_Server CMD_AUTH_RECONNECT_PROOF;
        version2::CMD_REALM_LIST_Server CMD_REALM_LIST;
        version2::CMD_XFER_INITIATE CMD_XFER_INITIATE;
        version2::CMD_XFER_DATA CMD_XFER_DATA;
    };

    bool is_none() const noexcept {
        return opcode == Opcode::NONE;
    }

    explicit ServerOpcode() : opcode(Opcode::NONE), CMD_AUTH_LOGON_CHALLENGE() {}

    ServerOpcode(ServerOpcode&& other) noexcept {
        this->opcode = other.opcode;
        other.opcode = Opcode::NONE;
        if (opcode == Opcode::CMD_AUTH_LOGON_CHALLENGE) {
            this->CMD_AUTH_LOGON_CHALLENGE = std::move(other.CMD_AUTH_LOGON_CHALLENGE);
        }
        if (opcode == Opcode::CMD_AUTH_LOGON_PROOF) {
            this->CMD_AUTH_LOGON_PROOF = std::move(other.CMD_AUTH_LOGON_PROOF);
        }
        if (opcode == Opcode::CMD_AUTH_RECONNECT_CHALLENGE) {
            this->CMD_AUTH_RECONNECT_CHALLENGE = std::move(other.CMD_AUTH_RECONNECT_CHALLENGE);
        }
        if (opcode == Opcode::CMD_AUTH_RECONNECT_PROOF) {
            this->CMD_AUTH_RECONNECT_PROOF = std::move(other.CMD_AUTH_RECONNECT_PROOF);
        }
        if (opcode == Opcode::CMD_REALM_LIST) {
            this->CMD_REALM_LIST = std::move(other.CMD_REALM_LIST);
        }
        if (opcode == Opcode::CMD_XFER_INITIATE) {
            this->CMD_XFER_INITIATE = std::move(other.CMD_XFER_INITIATE);
        }
        if (opcode == Opcode::CMD_XFER_DATA) {
            this->CMD_XFER_DATA = std::move(other.CMD_XFER_DATA);
        }
    }

    ~ServerOpcode() {
        if (opcode == Opcode::CMD_AUTH_LOGON_CHALLENGE) {
            this->CMD_AUTH_LOGON_CHALLENGE.~CMD_AUTH_LOGON_CHALLENGE_Server();
        }
        if (opcode == Opcode::CMD_AUTH_LOGON_PROOF) {
            this->CMD_AUTH_LOGON_PROOF.~CMD_AUTH_LOGON_PROOF_Server();
        }
        if (opcode == Opcode::CMD_AUTH_RECONNECT_CHALLENGE) {
            this->CMD_AUTH_RECONNECT_CHALLENGE.~CMD_AUTH_RECONNECT_CHALLENGE_Server();
        }
        if (opcode == Opcode::CMD_AUTH_RECONNECT_PROOF) {
            this->CMD_AUTH_RECONNECT_PROOF.~CMD_AUTH_RECONNECT_PROOF_Server();
        }
        if (opcode == Opcode::CMD_REALM_LIST) {
            this->CMD_REALM_LIST.~CMD_REALM_LIST_Server();
        }
        if (opcode == Opcode::CMD_XFER_INITIATE) {
            this->CMD_XFER_INITIATE.~CMD_XFER_INITIATE();
        }
        if (opcode == Opcode::CMD_XFER_DATA) {
            this->CMD_XFER_DATA.~CMD_XFER_DATA();
        }
    }

    explicit ServerOpcode(version2::CMD_AUTH_LOGON_CHALLENGE_Server&& obj) {
        opcode = Opcode::CMD_AUTH_LOGON_CHALLENGE;
        new (&this->CMD_AUTH_LOGON_CHALLENGE) version2::CMD_AUTH_LOGON_CHALLENGE_Server (std::move(obj));
    }
    explicit ServerOpcode(version2::CMD_AUTH_LOGON_PROOF_Server&& obj) {
        opcode = Opcode::CMD_AUTH_LOGON_PROOF;
        new (&this->CMD_AUTH_LOGON_PROOF) version2::CMD_AUTH_LOGON_PROOF_Server (std::move(obj));
    }
    explicit ServerOpcode(version2::CMD_AUTH_RECONNECT_CHALLENGE_Server&& obj) {
        opcode = Opcode::CMD_AUTH_RECONNECT_CHALLENGE;
        new (&this->CMD_AUTH_RECONNECT_CHALLENGE) version2::CMD_AUTH_RECONNECT_CHALLENGE_Server (std::move(obj));
    }
    explicit ServerOpcode(version2::CMD_AUTH_RECONNECT_PROOF_Server&& obj) {
        opcode = Opcode::CMD_AUTH_RECONNECT_PROOF;
        new (&this->CMD_AUTH_RECONNECT_PROOF) version2::CMD_AUTH_RECONNECT_PROOF_Server (std::move(obj));
    }
    explicit ServerOpcode(version2::CMD_REALM_LIST_Server&& obj) {
        opcode = Opcode::CMD_REALM_LIST;
        new (&this->CMD_REALM_LIST) version2::CMD_REALM_LIST_Server (std::move(obj));
    }
    explicit ServerOpcode(version2::CMD_XFER_INITIATE&& obj) {
        opcode = Opcode::CMD_XFER_INITIATE;
        new (&this->CMD_XFER_INITIATE) version2::CMD_XFER_INITIATE (std::move(obj));
    }
    explicit ServerOpcode(version2::CMD_XFER_DATA&& obj) {
        opcode = Opcode::CMD_XFER_DATA;
        new (&this->CMD_XFER_DATA) version2::CMD_XFER_DATA (std::move(obj));
    }

    template<typename T>
    // NOLINTNEXTLINE
    T& get(); // All possible types have been specialized

    template<typename T>
    // NOLINTNEXTLINE
    T* get_if(); // All possible types have been specialized
};

template<>
version2::CMD_AUTH_LOGON_CHALLENGE_Server* ServerOpcode::get_if();
template<>
version2::CMD_AUTH_LOGON_CHALLENGE_Server& ServerOpcode::get();
template<>
version2::CMD_AUTH_LOGON_PROOF_Server* ServerOpcode::get_if();
template<>
version2::CMD_AUTH_LOGON_PROOF_Server& ServerOpcode::get();
template<>
version2::CMD_AUTH_RECONNECT_CHALLENGE_Server* ServerOpcode::get_if();
template<>
version2::CMD_AUTH_RECONNECT_CHALLENGE_Server& ServerOpcode::get();
template<>
version2::CMD_AUTH_RECONNECT_PROOF_Server* ServerOpcode::get_if();
template<>
version2::CMD_AUTH_RECONNECT_PROOF_Server& ServerOpcode::get();
template<>
version2::CMD_REALM_LIST_Server* ServerOpcode::get_if();
template<>
version2::CMD_REALM_LIST_Server& ServerOpcode::get();
template<>
version2::CMD_XFER_INITIATE* ServerOpcode::get_if();
template<>
version2::CMD_XFER_INITIATE& ServerOpcode::get();
template<>
version2::CMD_XFER_DATA* ServerOpcode::get_if();
template<>
version2::CMD_XFER_DATA& ServerOpcode::get();

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write_opcode(const ServerOpcode& opcode);

WOW_LOGIN_MESSAGES_CPP_EXPORT ServerOpcode read_server_opcode(Reader& reader);

} // namespace version2
} // namespace wow_login_messages
#endif /* WOW_LOGIN_MESSAGES_CPP_VERSION2_HPP */
