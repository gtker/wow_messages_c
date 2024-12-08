#ifndef WOW_LOGIN_MESSAGES_CPP_VERSION3_HPP
#define WOW_LOGIN_MESSAGES_CPP_VERSION3_HPP

/* clang-format off */

#include "wow_login_messages_cpp/wow_login_messages.hpp"

namespace wow_login_messages {
namespace version3 {

typedef all::Locale Locale;

typedef version2::LoginResult LoginResult;

typedef all::Os Os;

typedef all::Platform Platform;

typedef all::ProtocolVersion ProtocolVersion;

typedef version2::RealmCategory RealmCategory;

typedef version2::RealmType RealmType;

enum class SecurityFlag : uint8_t {
    NONE = 0,
    PIN = 1,
};

typedef version2::RealmFlag RealmFlag;

typedef version2::Realm Realm;

typedef all::Version Version;

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

    WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write() const;
};

typedef all::CMD_AUTH_LOGON_CHALLENGE_Client CMD_AUTH_LOGON_CHALLENGE_Client;

typedef version2::CMD_AUTH_LOGON_PROOF_Server CMD_AUTH_LOGON_PROOF_Server;

struct CMD_AUTH_LOGON_PROOF_Client {
    std::array<uint8_t, 32> client_public_key;
    std::array<uint8_t, 20> client_proof;
    std::array<uint8_t, 20> crc_hash;
    uint8_t number_of_telemetry_keys;
    std::vector<version2::TelemetryKey> telemetry_keys;
    SecurityFlag security_flag;
    std::array<uint8_t, 16> pin_salt;
    std::array<uint8_t, 20> pin_hash;

    WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write() const;
};

typedef all::CMD_AUTH_RECONNECT_CHALLENGE_Client CMD_AUTH_RECONNECT_CHALLENGE_Client;

struct CMD_SURVEY_RESULT {
    uint32_t survey_id;
    uint8_t error;
    uint16_t compressed_data_length;
    std::vector<uint8_t> data;

    WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write() const;
};

typedef version2::CMD_REALM_LIST_Server CMD_REALM_LIST_Server;

struct CMD_REALM_LIST_Client {

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
        CMD_SURVEY_RESULT = 4,
        CMD_REALM_LIST = 16,
        CMD_XFER_ACCEPT = 50,
        CMD_XFER_RESUME = 51,
        CMD_XFER_CANCEL = 52,
    } opcode;

    union {
        version3::CMD_AUTH_LOGON_PROOF_Client CMD_AUTH_LOGON_PROOF;
        version3::CMD_SURVEY_RESULT CMD_SURVEY_RESULT;
        version3::CMD_REALM_LIST_Client CMD_REALM_LIST;
        version3::CMD_XFER_ACCEPT CMD_XFER_ACCEPT;
        version3::CMD_XFER_RESUME CMD_XFER_RESUME;
        version3::CMD_XFER_CANCEL CMD_XFER_CANCEL;
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
        if (opcode == Opcode::CMD_SURVEY_RESULT) {
            this->CMD_SURVEY_RESULT = std::move(other.CMD_SURVEY_RESULT);
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
        if (opcode == Opcode::CMD_SURVEY_RESULT) {
            this->CMD_SURVEY_RESULT.~CMD_SURVEY_RESULT();
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

    explicit ClientOpcode(version3::CMD_AUTH_LOGON_PROOF_Client&& obj) {
        opcode = Opcode::CMD_AUTH_LOGON_PROOF;
        new (&this->CMD_AUTH_LOGON_PROOF) version3::CMD_AUTH_LOGON_PROOF_Client (std::move(obj));
    }
    explicit ClientOpcode(version3::CMD_SURVEY_RESULT&& obj) {
        opcode = Opcode::CMD_SURVEY_RESULT;
        new (&this->CMD_SURVEY_RESULT) version3::CMD_SURVEY_RESULT (std::move(obj));
    }
    explicit ClientOpcode(version3::CMD_REALM_LIST_Client&& obj) {
        opcode = Opcode::CMD_REALM_LIST;
        new (&this->CMD_REALM_LIST) version3::CMD_REALM_LIST_Client (std::move(obj));
    }
    explicit ClientOpcode(version3::CMD_XFER_ACCEPT&& obj) {
        opcode = Opcode::CMD_XFER_ACCEPT;
        new (&this->CMD_XFER_ACCEPT) version3::CMD_XFER_ACCEPT (std::move(obj));
    }
    explicit ClientOpcode(version3::CMD_XFER_RESUME&& obj) {
        opcode = Opcode::CMD_XFER_RESUME;
        new (&this->CMD_XFER_RESUME) version3::CMD_XFER_RESUME (std::move(obj));
    }
    explicit ClientOpcode(version3::CMD_XFER_CANCEL&& obj) {
        opcode = Opcode::CMD_XFER_CANCEL;
        new (&this->CMD_XFER_CANCEL) version3::CMD_XFER_CANCEL (std::move(obj));
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
WOW_LOGIN_MESSAGES_CPP_EXPORT version3::CMD_AUTH_LOGON_PROOF_Client* ClientOpcode::get_if();
template<>
WOW_LOGIN_MESSAGES_CPP_EXPORT version3::CMD_AUTH_LOGON_PROOF_Client& ClientOpcode::get();
template<>
WOW_LOGIN_MESSAGES_CPP_EXPORT version3::CMD_SURVEY_RESULT* ClientOpcode::get_if();
template<>
WOW_LOGIN_MESSAGES_CPP_EXPORT version3::CMD_SURVEY_RESULT& ClientOpcode::get();
template<>
WOW_LOGIN_MESSAGES_CPP_EXPORT version3::CMD_REALM_LIST_Client* ClientOpcode::get_if();
template<>
WOW_LOGIN_MESSAGES_CPP_EXPORT version3::CMD_REALM_LIST_Client& ClientOpcode::get();
template<>
WOW_LOGIN_MESSAGES_CPP_EXPORT version3::CMD_XFER_ACCEPT* ClientOpcode::get_if();
template<>
WOW_LOGIN_MESSAGES_CPP_EXPORT version3::CMD_XFER_ACCEPT& ClientOpcode::get();
template<>
WOW_LOGIN_MESSAGES_CPP_EXPORT version3::CMD_XFER_RESUME* ClientOpcode::get_if();
template<>
WOW_LOGIN_MESSAGES_CPP_EXPORT version3::CMD_XFER_RESUME& ClientOpcode::get();
template<>
WOW_LOGIN_MESSAGES_CPP_EXPORT version3::CMD_XFER_CANCEL* ClientOpcode::get_if();
template<>
WOW_LOGIN_MESSAGES_CPP_EXPORT version3::CMD_XFER_CANCEL& ClientOpcode::get();

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write_opcode(const ClientOpcode& opcode);

WOW_LOGIN_MESSAGES_CPP_EXPORT ClientOpcode read_client_opcode(Reader& reader);

struct ServerOpcode {
    enum class Opcode {
        NONE = 0xFF,
        CMD_AUTH_LOGON_CHALLENGE = 0,
        CMD_AUTH_LOGON_PROOF = 1,
        CMD_REALM_LIST = 16,
        CMD_XFER_INITIATE = 48,
        CMD_XFER_DATA = 49,
    } opcode;

    union {
        version3::CMD_AUTH_LOGON_CHALLENGE_Server CMD_AUTH_LOGON_CHALLENGE;
        version3::CMD_AUTH_LOGON_PROOF_Server CMD_AUTH_LOGON_PROOF;
        version3::CMD_REALM_LIST_Server CMD_REALM_LIST;
        version3::CMD_XFER_INITIATE CMD_XFER_INITIATE;
        version3::CMD_XFER_DATA CMD_XFER_DATA;
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

    explicit ServerOpcode(version3::CMD_AUTH_LOGON_CHALLENGE_Server&& obj) {
        opcode = Opcode::CMD_AUTH_LOGON_CHALLENGE;
        new (&this->CMD_AUTH_LOGON_CHALLENGE) version3::CMD_AUTH_LOGON_CHALLENGE_Server (std::move(obj));
    }
    explicit ServerOpcode(version3::CMD_AUTH_LOGON_PROOF_Server&& obj) {
        opcode = Opcode::CMD_AUTH_LOGON_PROOF;
        new (&this->CMD_AUTH_LOGON_PROOF) version3::CMD_AUTH_LOGON_PROOF_Server (std::move(obj));
    }
    explicit ServerOpcode(version3::CMD_REALM_LIST_Server&& obj) {
        opcode = Opcode::CMD_REALM_LIST;
        new (&this->CMD_REALM_LIST) version3::CMD_REALM_LIST_Server (std::move(obj));
    }
    explicit ServerOpcode(version3::CMD_XFER_INITIATE&& obj) {
        opcode = Opcode::CMD_XFER_INITIATE;
        new (&this->CMD_XFER_INITIATE) version3::CMD_XFER_INITIATE (std::move(obj));
    }
    explicit ServerOpcode(version3::CMD_XFER_DATA&& obj) {
        opcode = Opcode::CMD_XFER_DATA;
        new (&this->CMD_XFER_DATA) version3::CMD_XFER_DATA (std::move(obj));
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
WOW_LOGIN_MESSAGES_CPP_EXPORT version3::CMD_AUTH_LOGON_CHALLENGE_Server* ServerOpcode::get_if();
template<>
WOW_LOGIN_MESSAGES_CPP_EXPORT version3::CMD_AUTH_LOGON_CHALLENGE_Server& ServerOpcode::get();
template<>
WOW_LOGIN_MESSAGES_CPP_EXPORT version3::CMD_AUTH_LOGON_PROOF_Server* ServerOpcode::get_if();
template<>
WOW_LOGIN_MESSAGES_CPP_EXPORT version3::CMD_AUTH_LOGON_PROOF_Server& ServerOpcode::get();
template<>
WOW_LOGIN_MESSAGES_CPP_EXPORT version3::CMD_REALM_LIST_Server* ServerOpcode::get_if();
template<>
WOW_LOGIN_MESSAGES_CPP_EXPORT version3::CMD_REALM_LIST_Server& ServerOpcode::get();
template<>
WOW_LOGIN_MESSAGES_CPP_EXPORT version3::CMD_XFER_INITIATE* ServerOpcode::get_if();
template<>
WOW_LOGIN_MESSAGES_CPP_EXPORT version3::CMD_XFER_INITIATE& ServerOpcode::get();
template<>
WOW_LOGIN_MESSAGES_CPP_EXPORT version3::CMD_XFER_DATA* ServerOpcode::get_if();
template<>
WOW_LOGIN_MESSAGES_CPP_EXPORT version3::CMD_XFER_DATA& ServerOpcode::get();

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write_opcode(const ServerOpcode& opcode);

WOW_LOGIN_MESSAGES_CPP_EXPORT ServerOpcode read_server_opcode(Reader& reader);

} // namespace version3
} // namespace wow_login_messages
#endif /* WOW_LOGIN_MESSAGES_CPP_VERSION3_HPP */
