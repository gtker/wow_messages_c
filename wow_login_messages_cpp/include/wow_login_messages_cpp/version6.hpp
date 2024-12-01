#ifndef WOW_LOGIN_MESSAGES_CPP_VERSION6_HPP
#define WOW_LOGIN_MESSAGES_CPP_VERSION6_HPP

/* clang-format off */

#include "wow_login_messages_cpp/wow_login_messages.hpp"

namespace wow_login_messages {
namespace version6 {

typedef all::Locale Locale;

typedef version2::LoginResult LoginResult;

typedef all::Os Os;

typedef all::Platform Platform;

typedef all::ProtocolVersion ProtocolVersion;

typedef version2::RealmCategory RealmCategory;

typedef version2::RealmType RealmType;

typedef version2::RealmFlag RealmFlag;

typedef version5::SecurityFlag SecurityFlag;

typedef version5::Realm Realm;

typedef all::Version Version;

typedef version2::TelemetryKey TelemetryKey;

typedef version5::CMD_AUTH_LOGON_CHALLENGE_Server CMD_AUTH_LOGON_CHALLENGE_Server;

typedef all::CMD_AUTH_LOGON_CHALLENGE_Client CMD_AUTH_LOGON_CHALLENGE_Client;

typedef version5::CMD_AUTH_LOGON_PROOF_Client CMD_AUTH_LOGON_PROOF_Client;

typedef version5::CMD_AUTH_LOGON_PROOF_Server CMD_AUTH_LOGON_PROOF_Server;

typedef version2::CMD_AUTH_RECONNECT_CHALLENGE_Server CMD_AUTH_RECONNECT_CHALLENGE_Server;

typedef all::CMD_AUTH_RECONNECT_CHALLENGE_Client CMD_AUTH_RECONNECT_CHALLENGE_Client;

typedef version2::CMD_AUTH_RECONNECT_PROOF_Client CMD_AUTH_RECONNECT_PROOF_Client;

typedef version5::CMD_AUTH_RECONNECT_PROOF_Server CMD_AUTH_RECONNECT_PROOF_Server;

struct CMD_REALM_LIST_Client {

    WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write() const;
};

struct CMD_REALM_LIST_Server {
    uint16_t number_of_realms;
    std::vector<version5::Realm> realms;

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
        version6::CMD_AUTH_LOGON_PROOF_Client CMD_AUTH_LOGON_PROOF;
        version6::CMD_AUTH_RECONNECT_PROOF_Client CMD_AUTH_RECONNECT_PROOF;
        version6::CMD_REALM_LIST_Client CMD_REALM_LIST;
        version6::CMD_XFER_ACCEPT CMD_XFER_ACCEPT;
        version6::CMD_XFER_RESUME CMD_XFER_RESUME;
        version6::CMD_XFER_CANCEL CMD_XFER_CANCEL;
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

    explicit ClientOpcode(version6::CMD_AUTH_LOGON_PROOF_Client&& obj) {
        opcode = Opcode::CMD_AUTH_LOGON_PROOF;
        new (&this->CMD_AUTH_LOGON_PROOF) version6::CMD_AUTH_LOGON_PROOF_Client (std::move(obj));
    }
    explicit ClientOpcode(version6::CMD_AUTH_RECONNECT_PROOF_Client&& obj) {
        opcode = Opcode::CMD_AUTH_RECONNECT_PROOF;
        new (&this->CMD_AUTH_RECONNECT_PROOF) version6::CMD_AUTH_RECONNECT_PROOF_Client (std::move(obj));
    }
    explicit ClientOpcode(version6::CMD_REALM_LIST_Client&& obj) {
        opcode = Opcode::CMD_REALM_LIST;
        new (&this->CMD_REALM_LIST) version6::CMD_REALM_LIST_Client (std::move(obj));
    }
    explicit ClientOpcode(version6::CMD_XFER_ACCEPT&& obj) {
        opcode = Opcode::CMD_XFER_ACCEPT;
        new (&this->CMD_XFER_ACCEPT) version6::CMD_XFER_ACCEPT (std::move(obj));
    }
    explicit ClientOpcode(version6::CMD_XFER_RESUME&& obj) {
        opcode = Opcode::CMD_XFER_RESUME;
        new (&this->CMD_XFER_RESUME) version6::CMD_XFER_RESUME (std::move(obj));
    }
    explicit ClientOpcode(version6::CMD_XFER_CANCEL&& obj) {
        opcode = Opcode::CMD_XFER_CANCEL;
        new (&this->CMD_XFER_CANCEL) version6::CMD_XFER_CANCEL (std::move(obj));
    }

    template<typename T>
    // NOLINTNEXTLINE
    T& get(); // All possible types have been specialized

    template<typename T>
    // NOLINTNEXTLINE
    T* get_if(); // All possible types have been specialized
};

template<>
version6::CMD_AUTH_LOGON_PROOF_Client* ClientOpcode::get_if();
template<>
version6::CMD_AUTH_LOGON_PROOF_Client& ClientOpcode::get();
template<>
version6::CMD_AUTH_RECONNECT_PROOF_Client* ClientOpcode::get_if();
template<>
version6::CMD_AUTH_RECONNECT_PROOF_Client& ClientOpcode::get();
template<>
version6::CMD_REALM_LIST_Client* ClientOpcode::get_if();
template<>
version6::CMD_REALM_LIST_Client& ClientOpcode::get();
template<>
version6::CMD_XFER_ACCEPT* ClientOpcode::get_if();
template<>
version6::CMD_XFER_ACCEPT& ClientOpcode::get();
template<>
version6::CMD_XFER_RESUME* ClientOpcode::get_if();
template<>
version6::CMD_XFER_RESUME& ClientOpcode::get();
template<>
version6::CMD_XFER_CANCEL* ClientOpcode::get_if();
template<>
version6::CMD_XFER_CANCEL& ClientOpcode::get();

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
        version6::CMD_AUTH_LOGON_CHALLENGE_Server CMD_AUTH_LOGON_CHALLENGE;
        version6::CMD_AUTH_LOGON_PROOF_Server CMD_AUTH_LOGON_PROOF;
        version6::CMD_AUTH_RECONNECT_CHALLENGE_Server CMD_AUTH_RECONNECT_CHALLENGE;
        version6::CMD_AUTH_RECONNECT_PROOF_Server CMD_AUTH_RECONNECT_PROOF;
        version6::CMD_REALM_LIST_Server CMD_REALM_LIST;
        version6::CMD_XFER_INITIATE CMD_XFER_INITIATE;
        version6::CMD_XFER_DATA CMD_XFER_DATA;
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

    explicit ServerOpcode(version6::CMD_AUTH_LOGON_CHALLENGE_Server&& obj) {
        opcode = Opcode::CMD_AUTH_LOGON_CHALLENGE;
        new (&this->CMD_AUTH_LOGON_CHALLENGE) version6::CMD_AUTH_LOGON_CHALLENGE_Server (std::move(obj));
    }
    explicit ServerOpcode(version6::CMD_AUTH_LOGON_PROOF_Server&& obj) {
        opcode = Opcode::CMD_AUTH_LOGON_PROOF;
        new (&this->CMD_AUTH_LOGON_PROOF) version6::CMD_AUTH_LOGON_PROOF_Server (std::move(obj));
    }
    explicit ServerOpcode(version6::CMD_AUTH_RECONNECT_CHALLENGE_Server&& obj) {
        opcode = Opcode::CMD_AUTH_RECONNECT_CHALLENGE;
        new (&this->CMD_AUTH_RECONNECT_CHALLENGE) version6::CMD_AUTH_RECONNECT_CHALLENGE_Server (std::move(obj));
    }
    explicit ServerOpcode(version6::CMD_AUTH_RECONNECT_PROOF_Server&& obj) {
        opcode = Opcode::CMD_AUTH_RECONNECT_PROOF;
        new (&this->CMD_AUTH_RECONNECT_PROOF) version6::CMD_AUTH_RECONNECT_PROOF_Server (std::move(obj));
    }
    explicit ServerOpcode(version6::CMD_REALM_LIST_Server&& obj) {
        opcode = Opcode::CMD_REALM_LIST;
        new (&this->CMD_REALM_LIST) version6::CMD_REALM_LIST_Server (std::move(obj));
    }
    explicit ServerOpcode(version6::CMD_XFER_INITIATE&& obj) {
        opcode = Opcode::CMD_XFER_INITIATE;
        new (&this->CMD_XFER_INITIATE) version6::CMD_XFER_INITIATE (std::move(obj));
    }
    explicit ServerOpcode(version6::CMD_XFER_DATA&& obj) {
        opcode = Opcode::CMD_XFER_DATA;
        new (&this->CMD_XFER_DATA) version6::CMD_XFER_DATA (std::move(obj));
    }

    template<typename T>
    // NOLINTNEXTLINE
    T& get(); // All possible types have been specialized

    template<typename T>
    // NOLINTNEXTLINE
    T* get_if(); // All possible types have been specialized
};

template<>
version6::CMD_AUTH_LOGON_CHALLENGE_Server* ServerOpcode::get_if();
template<>
version6::CMD_AUTH_LOGON_CHALLENGE_Server& ServerOpcode::get();
template<>
version6::CMD_AUTH_LOGON_PROOF_Server* ServerOpcode::get_if();
template<>
version6::CMD_AUTH_LOGON_PROOF_Server& ServerOpcode::get();
template<>
version6::CMD_AUTH_RECONNECT_CHALLENGE_Server* ServerOpcode::get_if();
template<>
version6::CMD_AUTH_RECONNECT_CHALLENGE_Server& ServerOpcode::get();
template<>
version6::CMD_AUTH_RECONNECT_PROOF_Server* ServerOpcode::get_if();
template<>
version6::CMD_AUTH_RECONNECT_PROOF_Server& ServerOpcode::get();
template<>
version6::CMD_REALM_LIST_Server* ServerOpcode::get_if();
template<>
version6::CMD_REALM_LIST_Server& ServerOpcode::get();
template<>
version6::CMD_XFER_INITIATE* ServerOpcode::get_if();
template<>
version6::CMD_XFER_INITIATE& ServerOpcode::get();
template<>
version6::CMD_XFER_DATA* ServerOpcode::get_if();
template<>
version6::CMD_XFER_DATA& ServerOpcode::get();

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write_opcode(const ServerOpcode& opcode);

WOW_LOGIN_MESSAGES_CPP_EXPORT ServerOpcode read_server_opcode(Reader& reader);

} // namespace version6
} // namespace wow_login_messages
#endif /* WOW_LOGIN_MESSAGES_CPP_VERSION6_HPP */
