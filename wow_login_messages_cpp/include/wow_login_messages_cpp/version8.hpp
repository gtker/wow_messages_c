#ifndef WOW_LOGIN_MESSAGES_CPP_VERSION8_HPP
#define WOW_LOGIN_MESSAGES_CPP_VERSION8_HPP

/* clang-format off */

#include "wow_login_messages_cpp/wow_login_messages.hpp"

namespace wow_login_messages {
namespace version8 {

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
    FAIL_LOCKED_ENFORCED = 16,
};

typedef all::Os Os;

typedef all::Platform Platform;

typedef all::ProtocolVersion ProtocolVersion;

typedef version2::RealmCategory RealmCategory;

typedef version2::RealmType RealmType;

enum AccountFlag : uint32_t {
    ACCOUNT_FLAG_GM = 1,
    ACCOUNT_FLAG_TRIAL = 8,
    ACCOUNT_FLAG_PROPASS = 8388608,
};

enum RealmFlag : uint8_t {
    REALM_FLAG_NONE = 0,
    REALM_FLAG_INVALID = 1,
    REALM_FLAG_OFFLINE = 2,
    REALM_FLAG_SPECIFY_BUILD = 4,
    REALM_FLAG_FORCE_BLUE_RECOMMENDED = 32,
    REALM_FLAG_FORCE_GREEN_RECOMMENDED = 64,
    REALM_FLAG_FORCE_RED_FULL = 128,
};

enum SecurityFlag : uint8_t {
    SECURITY_FLAG_NONE = 0,
    SECURITY_FLAG_PIN = 1,
    SECURITY_FLAG_MATRIX_CARD = 2,
    SECURITY_FLAG_AUTHENTICATOR = 4,
};

typedef all::Version Version;

struct Realm {
    RealmType realm_type;
    bool locked;
    RealmFlag flag;
    std::string name;
    std::string address;
    float population;
    uint8_t number_of_characters_on_realm;
    RealmCategory category;
    uint8_t realm_id;
    all::Version version;
};

typedef version2::TelemetryKey TelemetryKey;

struct CMD_AUTH_LOGON_CHALLENGE_Server {
    LoginResult result;
    std::array<uint8_t, 32> server_public_key;
    uint8_t generator_length;
    std::vector<uint8_t> generator;
    uint8_t large_safe_prime_length;
    std::vector<uint8_t> large_safe_prime;
    std::array<uint8_t, 32> salt;
    std::array<uint8_t, 16> crc_salt;
    SecurityFlag security_flag;
    uint32_t pin_grid_seed;
    std::array<uint8_t, 16> pin_salt;
    uint8_t width;
    uint8_t height;
    uint8_t digit_count;
    uint8_t challenge_count;
    uint64_t seed;
    uint8_t required;

    WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write() const;
};

typedef all::CMD_AUTH_LOGON_CHALLENGE_Client CMD_AUTH_LOGON_CHALLENGE_Client;

struct CMD_AUTH_LOGON_PROOF_Client {
    std::array<uint8_t, 32> client_public_key;
    std::array<uint8_t, 20> client_proof;
    std::array<uint8_t, 20> crc_hash;
    uint8_t number_of_telemetry_keys;
    std::vector<version2::TelemetryKey> telemetry_keys;
    SecurityFlag security_flag;
    std::array<uint8_t, 16> pin_salt;
    std::array<uint8_t, 20> pin_hash;
    std::array<uint8_t, 20> matrix_card_proof;
    std::string authenticator;

    WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write() const;
};

struct CMD_AUTH_LOGON_PROOF_Server {
    LoginResult result;
    std::array<uint8_t, 20> server_proof;
    AccountFlag account_flag;
    uint32_t hardware_survey_id;
    uint16_t unknown;

    WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write() const;
};

struct CMD_AUTH_RECONNECT_CHALLENGE_Server {
    LoginResult result;
    std::array<uint8_t, 16> challenge_data;
    std::array<uint8_t, 16> checksum_salt;

    WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write() const;
};

typedef all::CMD_AUTH_RECONNECT_CHALLENGE_Client CMD_AUTH_RECONNECT_CHALLENGE_Client;

typedef version2::CMD_AUTH_RECONNECT_PROOF_Client CMD_AUTH_RECONNECT_PROOF_Client;

struct CMD_AUTH_RECONNECT_PROOF_Server {
    LoginResult result;

    WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write() const;
};

struct CMD_REALM_LIST_Client {

    WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write() const;
};

struct CMD_REALM_LIST_Server {
    uint16_t number_of_realms;
    std::vector<version8::Realm> realms;

    WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write() const;
};

typedef version2::CMD_XFER_INITIATE CMD_XFER_INITIATE;

typedef version2::CMD_XFER_DATA CMD_XFER_DATA;

struct CMD_XFER_ACCEPT {

    WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write() const;
};

typedef version2::CMD_XFER_RESUME CMD_XFER_RESUME;

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
        version8::CMD_AUTH_LOGON_PROOF_Client CMD_AUTH_LOGON_PROOF;
        version8::CMD_AUTH_RECONNECT_PROOF_Client CMD_AUTH_RECONNECT_PROOF;
        version8::CMD_REALM_LIST_Client CMD_REALM_LIST;
        version8::CMD_XFER_ACCEPT CMD_XFER_ACCEPT;
        version8::CMD_XFER_RESUME CMD_XFER_RESUME;
        version8::CMD_XFER_CANCEL CMD_XFER_CANCEL;
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

    explicit ClientOpcode(version8::CMD_AUTH_LOGON_PROOF_Client&& obj) {
        opcode = Opcode::CMD_AUTH_LOGON_PROOF;
        new (&this->CMD_AUTH_LOGON_PROOF) version8::CMD_AUTH_LOGON_PROOF_Client (std::move(obj));
    }
    explicit ClientOpcode(version8::CMD_AUTH_RECONNECT_PROOF_Client&& obj) {
        opcode = Opcode::CMD_AUTH_RECONNECT_PROOF;
        new (&this->CMD_AUTH_RECONNECT_PROOF) version8::CMD_AUTH_RECONNECT_PROOF_Client (std::move(obj));
    }
    explicit ClientOpcode(version8::CMD_REALM_LIST_Client&& obj) {
        opcode = Opcode::CMD_REALM_LIST;
        new (&this->CMD_REALM_LIST) version8::CMD_REALM_LIST_Client (std::move(obj));
    }
    explicit ClientOpcode(version8::CMD_XFER_ACCEPT&& obj) {
        opcode = Opcode::CMD_XFER_ACCEPT;
        new (&this->CMD_XFER_ACCEPT) version8::CMD_XFER_ACCEPT (std::move(obj));
    }
    explicit ClientOpcode(version8::CMD_XFER_RESUME&& obj) {
        opcode = Opcode::CMD_XFER_RESUME;
        new (&this->CMD_XFER_RESUME) version8::CMD_XFER_RESUME (std::move(obj));
    }
    explicit ClientOpcode(version8::CMD_XFER_CANCEL&& obj) {
        opcode = Opcode::CMD_XFER_CANCEL;
        new (&this->CMD_XFER_CANCEL) version8::CMD_XFER_CANCEL (std::move(obj));
    }

    template<typename T>
    // NOLINTNEXTLINE
    T& get(); // All possible types have been specialized

    template<typename T>
    // NOLINTNEXTLINE
    T* get_if(); // All possible types have been specialized
};

template<>
version8::CMD_AUTH_LOGON_PROOF_Client* ClientOpcode::get_if();
template<>
version8::CMD_AUTH_LOGON_PROOF_Client& ClientOpcode::get();
template<>
version8::CMD_AUTH_RECONNECT_PROOF_Client* ClientOpcode::get_if();
template<>
version8::CMD_AUTH_RECONNECT_PROOF_Client& ClientOpcode::get();
template<>
version8::CMD_REALM_LIST_Client* ClientOpcode::get_if();
template<>
version8::CMD_REALM_LIST_Client& ClientOpcode::get();
template<>
version8::CMD_XFER_ACCEPT* ClientOpcode::get_if();
template<>
version8::CMD_XFER_ACCEPT& ClientOpcode::get();
template<>
version8::CMD_XFER_RESUME* ClientOpcode::get_if();
template<>
version8::CMD_XFER_RESUME& ClientOpcode::get();
template<>
version8::CMD_XFER_CANCEL* ClientOpcode::get_if();
template<>
version8::CMD_XFER_CANCEL& ClientOpcode::get();

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
        version8::CMD_AUTH_LOGON_CHALLENGE_Server CMD_AUTH_LOGON_CHALLENGE;
        version8::CMD_AUTH_LOGON_PROOF_Server CMD_AUTH_LOGON_PROOF;
        version8::CMD_AUTH_RECONNECT_CHALLENGE_Server CMD_AUTH_RECONNECT_CHALLENGE;
        version8::CMD_AUTH_RECONNECT_PROOF_Server CMD_AUTH_RECONNECT_PROOF;
        version8::CMD_REALM_LIST_Server CMD_REALM_LIST;
        version8::CMD_XFER_INITIATE CMD_XFER_INITIATE;
        version8::CMD_XFER_DATA CMD_XFER_DATA;
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

    explicit ServerOpcode(version8::CMD_AUTH_LOGON_CHALLENGE_Server&& obj) {
        opcode = Opcode::CMD_AUTH_LOGON_CHALLENGE;
        new (&this->CMD_AUTH_LOGON_CHALLENGE) version8::CMD_AUTH_LOGON_CHALLENGE_Server (std::move(obj));
    }
    explicit ServerOpcode(version8::CMD_AUTH_LOGON_PROOF_Server&& obj) {
        opcode = Opcode::CMD_AUTH_LOGON_PROOF;
        new (&this->CMD_AUTH_LOGON_PROOF) version8::CMD_AUTH_LOGON_PROOF_Server (std::move(obj));
    }
    explicit ServerOpcode(version8::CMD_AUTH_RECONNECT_CHALLENGE_Server&& obj) {
        opcode = Opcode::CMD_AUTH_RECONNECT_CHALLENGE;
        new (&this->CMD_AUTH_RECONNECT_CHALLENGE) version8::CMD_AUTH_RECONNECT_CHALLENGE_Server (std::move(obj));
    }
    explicit ServerOpcode(version8::CMD_AUTH_RECONNECT_PROOF_Server&& obj) {
        opcode = Opcode::CMD_AUTH_RECONNECT_PROOF;
        new (&this->CMD_AUTH_RECONNECT_PROOF) version8::CMD_AUTH_RECONNECT_PROOF_Server (std::move(obj));
    }
    explicit ServerOpcode(version8::CMD_REALM_LIST_Server&& obj) {
        opcode = Opcode::CMD_REALM_LIST;
        new (&this->CMD_REALM_LIST) version8::CMD_REALM_LIST_Server (std::move(obj));
    }
    explicit ServerOpcode(version8::CMD_XFER_INITIATE&& obj) {
        opcode = Opcode::CMD_XFER_INITIATE;
        new (&this->CMD_XFER_INITIATE) version8::CMD_XFER_INITIATE (std::move(obj));
    }
    explicit ServerOpcode(version8::CMD_XFER_DATA&& obj) {
        opcode = Opcode::CMD_XFER_DATA;
        new (&this->CMD_XFER_DATA) version8::CMD_XFER_DATA (std::move(obj));
    }

    template<typename T>
    // NOLINTNEXTLINE
    T& get(); // All possible types have been specialized

    template<typename T>
    // NOLINTNEXTLINE
    T* get_if(); // All possible types have been specialized
};

template<>
version8::CMD_AUTH_LOGON_CHALLENGE_Server* ServerOpcode::get_if();
template<>
version8::CMD_AUTH_LOGON_CHALLENGE_Server& ServerOpcode::get();
template<>
version8::CMD_AUTH_LOGON_PROOF_Server* ServerOpcode::get_if();
template<>
version8::CMD_AUTH_LOGON_PROOF_Server& ServerOpcode::get();
template<>
version8::CMD_AUTH_RECONNECT_CHALLENGE_Server* ServerOpcode::get_if();
template<>
version8::CMD_AUTH_RECONNECT_CHALLENGE_Server& ServerOpcode::get();
template<>
version8::CMD_AUTH_RECONNECT_PROOF_Server* ServerOpcode::get_if();
template<>
version8::CMD_AUTH_RECONNECT_PROOF_Server& ServerOpcode::get();
template<>
version8::CMD_REALM_LIST_Server* ServerOpcode::get_if();
template<>
version8::CMD_REALM_LIST_Server& ServerOpcode::get();
template<>
version8::CMD_XFER_INITIATE* ServerOpcode::get_if();
template<>
version8::CMD_XFER_INITIATE& ServerOpcode::get();
template<>
version8::CMD_XFER_DATA* ServerOpcode::get_if();
template<>
version8::CMD_XFER_DATA& ServerOpcode::get();

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write_opcode(const ServerOpcode& opcode);

WOW_LOGIN_MESSAGES_CPP_EXPORT ServerOpcode read_server_opcode(Reader& reader);

} // namespace version8
} // namespace wow_login_messages
#endif /* WOW_LOGIN_MESSAGES_CPP_VERSION8_HPP */
