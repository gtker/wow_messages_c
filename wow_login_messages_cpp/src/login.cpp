#include "util.hpp"

#include "wow_login_messages_cpp/all.hpp"

namespace wow_login_messages {
namespace all {
Version Version_read(Reader& reader) {
    Version obj{};

    obj.major = reader.read_u8();

    obj.minor = reader.read_u8();

    obj.patch = reader.read_u8();

    obj.build = reader.read_u16();

    return obj;
}

static void Version_write(Writer& writer, const Version& obj) {
    writer.write_u8(obj.major);

    writer.write_u8(obj.minor);

    writer.write_u8(obj.patch);

    writer.write_u16(obj.build);

}

static size_t CMD_AUTH_LOGON_CHALLENGE_Client_size(const CMD_AUTH_LOGON_CHALLENGE_Client& obj) {
    return 30 + obj.account_name.size();
}

CMD_AUTH_LOGON_CHALLENGE_Client CMD_AUTH_LOGON_CHALLENGE_Client_read(Reader& reader) {
    CMD_AUTH_LOGON_CHALLENGE_Client obj{};

    obj.protocol_version = static_cast<ProtocolVersion>(reader.read_u8());

    (void)reader.read_u16();

    (void)reader.read_u32();

    obj.version = ::wow_login_messages::all::Version_read(reader);

    obj.platform = static_cast<Platform>(reader.read_u32());

    obj.os = static_cast<Os>(reader.read_u32());

    obj.locale = static_cast<Locale>(reader.read_u32());

    obj.utc_timezone_offset = reader.read_i32();

    obj.client_ip_address = reader.read_u32();

    obj.account_name = reader.read_string();

    return obj;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_AUTH_LOGON_CHALLENGE_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMD_AUTH_LOGON_CHALLENGE_Client_size(obj));

    writer.write_u8(0x00); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.protocol_version));

    writer.write_u16(static_cast<uint16_t>(CMD_AUTH_LOGON_CHALLENGE_Client_size(obj)));

    writer.write_u32(5730135);

    Version_write(writer, obj.version);

    writer.write_u32(static_cast<uint32_t>(obj.platform));

    writer.write_u32(static_cast<uint32_t>(obj.os));

    writer.write_u32(static_cast<uint32_t>(obj.locale));

    writer.write_i32(obj.utc_timezone_offset);

    writer.write_u32(obj.client_ip_address);

    writer.write_string(obj.account_name);

    return writer.m_buf;
}

static size_t CMD_AUTH_RECONNECT_CHALLENGE_Client_size(const CMD_AUTH_RECONNECT_CHALLENGE_Client& obj) {
    return 30 + obj.account_name.size();
}

CMD_AUTH_RECONNECT_CHALLENGE_Client CMD_AUTH_RECONNECT_CHALLENGE_Client_read(Reader& reader) {
    CMD_AUTH_RECONNECT_CHALLENGE_Client obj{};

    obj.protocol_version = static_cast<ProtocolVersion>(reader.read_u8());

    (void)reader.read_u16();

    (void)reader.read_u32();

    obj.version = ::wow_login_messages::all::Version_read(reader);

    obj.platform = static_cast<Platform>(reader.read_u32());

    obj.os = static_cast<Os>(reader.read_u32());

    obj.locale = static_cast<Locale>(reader.read_u32());

    obj.utc_timezone_offset = reader.read_i32();

    obj.client_ip_address = reader.read_u32();

    obj.account_name = reader.read_string();

    return obj;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_AUTH_RECONNECT_CHALLENGE_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMD_AUTH_RECONNECT_CHALLENGE_Client_size(obj));

    writer.write_u8(0x02); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.protocol_version));

    writer.write_u16(static_cast<uint16_t>(CMD_AUTH_RECONNECT_CHALLENGE_Client_size(obj)));

    writer.write_u32(5730135);

    Version_write(writer, obj.version);

    writer.write_u32(static_cast<uint32_t>(obj.platform));

    writer.write_u32(static_cast<uint32_t>(obj.os));

    writer.write_u32(static_cast<uint32_t>(obj.locale));

    writer.write_i32(obj.utc_timezone_offset);

    writer.write_u32(obj.client_ip_address);

    writer.write_string(obj.account_name);

    return writer.m_buf;
}

template <>
all::CMD_AUTH_LOGON_CHALLENGE_Client* ClientOpcode::get_if<CMD_AUTH_LOGON_CHALLENGE_Client>() {
    if (opcode == Opcode::CMD_AUTH_LOGON_CHALLENGE) {
        return &CMD_AUTH_LOGON_CHALLENGE;
    }
    return nullptr;
}
template <>
all::CMD_AUTH_LOGON_CHALLENGE_Client& ClientOpcode::get<CMD_AUTH_LOGON_CHALLENGE_Client>() {
    auto p = ClientOpcode::get_if<all::CMD_AUTH_LOGON_CHALLENGE_Client>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
all::CMD_AUTH_RECONNECT_CHALLENGE_Client* ClientOpcode::get_if<CMD_AUTH_RECONNECT_CHALLENGE_Client>() {
    if (opcode == Opcode::CMD_AUTH_RECONNECT_CHALLENGE) {
        return &CMD_AUTH_RECONNECT_CHALLENGE;
    }
    return nullptr;
}
template <>
all::CMD_AUTH_RECONNECT_CHALLENGE_Client& ClientOpcode::get<CMD_AUTH_RECONNECT_CHALLENGE_Client>() {
    auto p = ClientOpcode::get_if<all::CMD_AUTH_RECONNECT_CHALLENGE_Client>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}


WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write_opcode(const ClientOpcode& opcode) {
    if (opcode.opcode == ClientOpcode::Opcode::CMD_AUTH_LOGON_CHALLENGE) {
        return opcode.CMD_AUTH_LOGON_CHALLENGE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_AUTH_RECONNECT_CHALLENGE) {
        return opcode.CMD_AUTH_RECONNECT_CHALLENGE.write();;
    }

    return {}; /* unreachable */
}

WOW_LOGIN_MESSAGES_CPP_EXPORT ClientOpcode read_client_opcode(Reader& reader) {
    const uint8_t opcode = reader.read_u8();

    ClientOpcode op;

    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_AUTH_LOGON_CHALLENGE)) {
        return ClientOpcode(::wow_login_messages::all::CMD_AUTH_LOGON_CHALLENGE_Client_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_AUTH_RECONNECT_CHALLENGE)) {
        return ClientOpcode(::wow_login_messages::all::CMD_AUTH_RECONNECT_CHALLENGE_Client_read(reader));
    }

    return op;
}
} // namespace all
} // namespace wow_login_messages
#include "wow_login_messages_cpp/version2.hpp"

namespace wow_login_messages {
namespace version2 {
static size_t Realm_size(const Realm& obj) {
    return 14 + obj.name.size() + obj.address.size();
}

Realm Realm_read(Reader& reader) {
    Realm obj{};

    obj.realm_type = static_cast<RealmType>(reader.read_u32());

    obj.flag = static_cast<RealmFlag>(reader.read_u8());

    obj.name = reader.read_cstring();

    obj.address = reader.read_cstring();

    obj.population = reader.read_float();

    obj.number_of_characters_on_realm = reader.read_u8();

    obj.category = static_cast<RealmCategory>(reader.read_u8());

    obj.realm_id = reader.read_u8();

    return obj;
}

static void Realm_write(Writer& writer, const Realm& obj) {
    writer.write_u32(static_cast<uint32_t>(obj.realm_type));

    writer.write_u8(static_cast<uint8_t>(obj.flag));

    writer.write_cstring(obj.name);

    writer.write_cstring(obj.address);

    writer.write_float(obj.population);

    writer.write_u8(obj.number_of_characters_on_realm);

    writer.write_u8(static_cast<uint8_t>(obj.category));

    writer.write_u8(obj.realm_id);

}

TelemetryKey TelemetryKey_read(Reader& reader) {
    TelemetryKey obj{};

    obj.unknown1 = reader.read_u16();

    obj.unknown2 = reader.read_u32();

    for (auto i = 0; i < 4; ++i) {
        obj.unknown3[i] = reader.read_u8();
    }

    for (auto i = 0; i < 20; ++i) {
        obj.cd_key_proof[i] = reader.read_u8();
    }

    return obj;
}

static void TelemetryKey_write(Writer& writer, const TelemetryKey& obj) {
    writer.write_u16(obj.unknown1);

    writer.write_u32(obj.unknown2);

    for (const auto& v : obj.unknown3) {
        writer.write_u8(v);
    }

    for (const auto& v : obj.cd_key_proof) {
        writer.write_u8(v);
    }

}

static size_t CMD_AUTH_LOGON_CHALLENGE_Server_size(const CMD_AUTH_LOGON_CHALLENGE_Server& obj) {
    size_t _size = 2;

    if (obj.result == LoginResult::SUCCESS) {
        _size += 82 + 1 * obj.generator.size() + 1 * obj.large_safe_prime.size();
    }

    return _size;
}

CMD_AUTH_LOGON_CHALLENGE_Server CMD_AUTH_LOGON_CHALLENGE_Server_read(Reader& reader) {
    CMD_AUTH_LOGON_CHALLENGE_Server obj{};

    (void)reader.read_u8();

    obj.result = static_cast<LoginResult>(reader.read_u8());

    if (obj.result == LoginResult::SUCCESS) {
        for (auto i = 0; i < 32; ++i) {
            obj.server_public_key[i] = reader.read_u8();
        }

        obj.generator_length = reader.read_u8();

        for (uint8_t i = 0; i < obj.generator_length; ++i) {
            obj.generator.push_back(reader.read_u8());
        }

        obj.large_safe_prime_length = reader.read_u8();

        for (uint8_t i = 0; i < obj.large_safe_prime_length; ++i) {
            obj.large_safe_prime.push_back(reader.read_u8());
        }

        for (auto i = 0; i < 32; ++i) {
            obj.salt[i] = reader.read_u8();
        }

        for (auto i = 0; i < 16; ++i) {
            obj.crc_salt[i] = reader.read_u8();
        }

    }
    return obj;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_AUTH_LOGON_CHALLENGE_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMD_AUTH_LOGON_CHALLENGE_Server_size(obj));

    writer.write_u8(0x00); /* opcode */

    writer.write_u8(0);

    writer.write_u8(static_cast<uint8_t>(obj.result));

    if (obj.result == LoginResult::SUCCESS) {
        for (const auto& v : obj.server_public_key) {
            writer.write_u8(v);
        }

        writer.write_u8(obj.generator_length);

        for (const auto& v : obj.generator) {
            writer.write_u8(v);
        }

        writer.write_u8(obj.large_safe_prime_length);

        for (const auto& v : obj.large_safe_prime) {
            writer.write_u8(v);
        }

        for (const auto& v : obj.salt) {
            writer.write_u8(v);
        }

        for (const auto& v : obj.crc_salt) {
            writer.write_u8(v);
        }

    }
    return writer.m_buf;
}

static size_t CMD_AUTH_LOGON_PROOF_Client_size(const CMD_AUTH_LOGON_PROOF_Client& obj) {
    return 73 + 30 * obj.telemetry_keys.size();
}

CMD_AUTH_LOGON_PROOF_Client CMD_AUTH_LOGON_PROOF_Client_read(Reader& reader) {
    CMD_AUTH_LOGON_PROOF_Client obj{};

    for (auto i = 0; i < 32; ++i) {
        obj.client_public_key[i] = reader.read_u8();
    }

    for (auto i = 0; i < 20; ++i) {
        obj.client_proof[i] = reader.read_u8();
    }

    for (auto i = 0; i < 20; ++i) {
        obj.crc_hash[i] = reader.read_u8();
    }

    obj.number_of_telemetry_keys = reader.read_u8();

    for (uint8_t i = 0; i < obj.number_of_telemetry_keys; ++i) {
        obj.telemetry_keys.push_back(::wow_login_messages::version2::TelemetryKey_read(reader));
    }

    return obj;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_AUTH_LOGON_PROOF_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMD_AUTH_LOGON_PROOF_Client_size(obj));

    writer.write_u8(0x01); /* opcode */

    for (const auto& v : obj.client_public_key) {
        writer.write_u8(v);
    }

    for (const auto& v : obj.client_proof) {
        writer.write_u8(v);
    }

    for (const auto& v : obj.crc_hash) {
        writer.write_u8(v);
    }

    writer.write_u8(obj.number_of_telemetry_keys);

    for (const auto& v : obj.telemetry_keys) {
        TelemetryKey_write(writer, v);
    }

    return writer.m_buf;
}

static size_t CMD_AUTH_LOGON_PROOF_Server_size(const CMD_AUTH_LOGON_PROOF_Server& obj) {
    size_t _size = 1;

    if (obj.result == LoginResult::SUCCESS) {
        _size += 24;
    }

    return _size;
}

CMD_AUTH_LOGON_PROOF_Server CMD_AUTH_LOGON_PROOF_Server_read(Reader& reader) {
    CMD_AUTH_LOGON_PROOF_Server obj{};

    obj.result = static_cast<LoginResult>(reader.read_u8());

    if (obj.result == LoginResult::SUCCESS) {
        for (auto i = 0; i < 20; ++i) {
            obj.server_proof[i] = reader.read_u8();
        }

        obj.hardware_survey_id = reader.read_u32();

    }
    return obj;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_AUTH_LOGON_PROOF_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMD_AUTH_LOGON_PROOF_Server_size(obj));

    writer.write_u8(0x01); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.result));

    if (obj.result == LoginResult::SUCCESS) {
        for (const auto& v : obj.server_proof) {
            writer.write_u8(v);
        }

        writer.write_u32(obj.hardware_survey_id);

    }
    return writer.m_buf;
}

static size_t CMD_AUTH_RECONNECT_CHALLENGE_Server_size(const CMD_AUTH_RECONNECT_CHALLENGE_Server& obj) {
    size_t _size = 1;

    if (obj.result == LoginResult::SUCCESS) {
        _size += 32;
    }

    return _size;
}

CMD_AUTH_RECONNECT_CHALLENGE_Server CMD_AUTH_RECONNECT_CHALLENGE_Server_read(Reader& reader) {
    CMD_AUTH_RECONNECT_CHALLENGE_Server obj{};

    obj.result = static_cast<LoginResult>(reader.read_u8());

    if (obj.result == LoginResult::SUCCESS) {
        for (auto i = 0; i < 16; ++i) {
            obj.challenge_data[i] = reader.read_u8();
        }

        for (auto i = 0; i < 16; ++i) {
            obj.checksum_salt[i] = reader.read_u8();
        }

    }
    return obj;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_AUTH_RECONNECT_CHALLENGE_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMD_AUTH_RECONNECT_CHALLENGE_Server_size(obj));

    writer.write_u8(0x02); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.result));

    if (obj.result == LoginResult::SUCCESS) {
        for (const auto& v : obj.challenge_data) {
            writer.write_u8(v);
        }

        for (const auto& v : obj.checksum_salt) {
            writer.write_u8(v);
        }

    }
    return writer.m_buf;
}

CMD_AUTH_RECONNECT_PROOF_Server CMD_AUTH_RECONNECT_PROOF_Server_read(Reader& reader) {
    CMD_AUTH_RECONNECT_PROOF_Server obj{};

    obj.result = static_cast<LoginResult>(reader.read_u8());

    return obj;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_AUTH_RECONNECT_PROOF_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0001);

    writer.write_u8(0x03); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.result));

    return writer.m_buf;
}

CMD_AUTH_RECONNECT_PROOF_Client CMD_AUTH_RECONNECT_PROOF_Client_read(Reader& reader) {
    CMD_AUTH_RECONNECT_PROOF_Client obj{};

    for (auto i = 0; i < 16; ++i) {
        obj.proof_data[i] = reader.read_u8();
    }

    for (auto i = 0; i < 20; ++i) {
        obj.client_proof[i] = reader.read_u8();
    }

    for (auto i = 0; i < 20; ++i) {
        obj.client_checksum[i] = reader.read_u8();
    }

    (void)reader.read_u8();

    return obj;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_AUTH_RECONNECT_PROOF_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0039);

    writer.write_u8(0x03); /* opcode */

    for (const auto& v : obj.proof_data) {
        writer.write_u8(v);
    }

    for (const auto& v : obj.client_proof) {
        writer.write_u8(v);
    }

    for (const auto& v : obj.client_checksum) {
        writer.write_u8(v);
    }

    writer.write_u8(0);

    return writer.m_buf;
}

static size_t CMD_REALM_LIST_Server_size(const CMD_REALM_LIST_Server& obj) {
    size_t _size = 7;

    for(const auto& v : obj.realms) {
        _size += Realm_size(v);
    }

    return _size;
}

CMD_REALM_LIST_Server CMD_REALM_LIST_Server_read(Reader& reader) {
    CMD_REALM_LIST_Server obj{};

    (void)reader.read_u16();

    (void)reader.read_u32();

    obj.number_of_realms = reader.read_u8();

    for (uint8_t i = 0; i < obj.number_of_realms; ++i) {
        obj.realms.push_back(::wow_login_messages::version2::Realm_read(reader));
    }

    (void)reader.read_u16();

    return obj;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_REALM_LIST_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMD_REALM_LIST_Server_size(obj));

    writer.write_u8(0x10); /* opcode */

    writer.write_u16(static_cast<uint16_t>(CMD_REALM_LIST_Server_size(obj)));

    writer.write_u32(0);

    writer.write_u8(obj.number_of_realms);

    for (const auto& v : obj.realms) {
        Realm_write(writer, v);
    }

    writer.write_u16(0);

    return writer.m_buf;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_REALM_LIST_Client::write() const {
    auto writer = Writer(0x0004);

    writer.write_u8(0x10); /* opcode */

    writer.write_u32(0);

    return writer.m_buf;
}

static size_t CMD_XFER_INITIATE_size(const CMD_XFER_INITIATE& obj) {
    return 25 + obj.filename.size();
}

CMD_XFER_INITIATE CMD_XFER_INITIATE_read(Reader& reader) {
    CMD_XFER_INITIATE obj{};

    obj.filename = reader.read_string();

    obj.file_size = reader.read_u64();

    for (auto i = 0; i < 16; ++i) {
        obj.file_md5[i] = reader.read_u8();
    }

    return obj;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_XFER_INITIATE::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMD_XFER_INITIATE_size(obj));

    writer.write_u8(0x30); /* opcode */

    writer.write_string(obj.filename);

    writer.write_u64(obj.file_size);

    for (const auto& v : obj.file_md5) {
        writer.write_u8(v);
    }

    return writer.m_buf;
}

static size_t CMD_XFER_DATA_size(const CMD_XFER_DATA& obj) {
    return 2 + 1 * obj.data.size();
}

CMD_XFER_DATA CMD_XFER_DATA_read(Reader& reader) {
    CMD_XFER_DATA obj{};

    obj.size = reader.read_u16();

    for (uint16_t i = 0; i < obj.size; ++i) {
        obj.data.push_back(reader.read_u8());
    }

    return obj;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_XFER_DATA::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMD_XFER_DATA_size(obj));

    writer.write_u8(0x31); /* opcode */

    writer.write_u16(obj.size);

    for (const auto& v : obj.data) {
        writer.write_u8(v);
    }

    return writer.m_buf;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_XFER_ACCEPT::write() const {
    auto writer = Writer(0x0000);

    writer.write_u8(0x32); /* opcode */

    return writer.m_buf;
}

CMD_XFER_RESUME CMD_XFER_RESUME_read(Reader& reader) {
    CMD_XFER_RESUME obj{};

    obj.offset = reader.read_u64();

    return obj;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_XFER_RESUME::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u8(0x33); /* opcode */

    writer.write_u64(obj.offset);

    return writer.m_buf;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_XFER_CANCEL::write() const {
    auto writer = Writer(0x0000);

    writer.write_u8(0x34); /* opcode */

    return writer.m_buf;
}

template <>
version2::CMD_AUTH_LOGON_PROOF_Client* ClientOpcode::get_if<CMD_AUTH_LOGON_PROOF_Client>() {
    if (opcode == Opcode::CMD_AUTH_LOGON_PROOF) {
        return &CMD_AUTH_LOGON_PROOF;
    }
    return nullptr;
}
template <>
version2::CMD_AUTH_LOGON_PROOF_Client& ClientOpcode::get<CMD_AUTH_LOGON_PROOF_Client>() {
    auto p = ClientOpcode::get_if<version2::CMD_AUTH_LOGON_PROOF_Client>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version2::CMD_AUTH_RECONNECT_PROOF_Client* ClientOpcode::get_if<CMD_AUTH_RECONNECT_PROOF_Client>() {
    if (opcode == Opcode::CMD_AUTH_RECONNECT_PROOF) {
        return &CMD_AUTH_RECONNECT_PROOF;
    }
    return nullptr;
}
template <>
version2::CMD_AUTH_RECONNECT_PROOF_Client& ClientOpcode::get<CMD_AUTH_RECONNECT_PROOF_Client>() {
    auto p = ClientOpcode::get_if<version2::CMD_AUTH_RECONNECT_PROOF_Client>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version2::CMD_REALM_LIST_Client* ClientOpcode::get_if<CMD_REALM_LIST_Client>() {
    if (opcode == Opcode::CMD_REALM_LIST) {
        return &CMD_REALM_LIST;
    }
    return nullptr;
}
template <>
version2::CMD_REALM_LIST_Client& ClientOpcode::get<CMD_REALM_LIST_Client>() {
    auto p = ClientOpcode::get_if<version2::CMD_REALM_LIST_Client>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version2::CMD_XFER_ACCEPT* ClientOpcode::get_if<CMD_XFER_ACCEPT>() {
    if (opcode == Opcode::CMD_XFER_ACCEPT) {
        return &CMD_XFER_ACCEPT;
    }
    return nullptr;
}
template <>
version2::CMD_XFER_ACCEPT& ClientOpcode::get<CMD_XFER_ACCEPT>() {
    auto p = ClientOpcode::get_if<version2::CMD_XFER_ACCEPT>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version2::CMD_XFER_RESUME* ClientOpcode::get_if<CMD_XFER_RESUME>() {
    if (opcode == Opcode::CMD_XFER_RESUME) {
        return &CMD_XFER_RESUME;
    }
    return nullptr;
}
template <>
version2::CMD_XFER_RESUME& ClientOpcode::get<CMD_XFER_RESUME>() {
    auto p = ClientOpcode::get_if<version2::CMD_XFER_RESUME>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version2::CMD_XFER_CANCEL* ClientOpcode::get_if<CMD_XFER_CANCEL>() {
    if (opcode == Opcode::CMD_XFER_CANCEL) {
        return &CMD_XFER_CANCEL;
    }
    return nullptr;
}
template <>
version2::CMD_XFER_CANCEL& ClientOpcode::get<CMD_XFER_CANCEL>() {
    auto p = ClientOpcode::get_if<version2::CMD_XFER_CANCEL>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}


WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write_opcode(const ClientOpcode& opcode) {
    if (opcode.opcode == ClientOpcode::Opcode::CMD_AUTH_LOGON_PROOF) {
        return opcode.CMD_AUTH_LOGON_PROOF.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_AUTH_RECONNECT_PROOF) {
        return opcode.CMD_AUTH_RECONNECT_PROOF.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_REALM_LIST) {
        return opcode.CMD_REALM_LIST.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_XFER_ACCEPT) {
        return opcode.CMD_XFER_ACCEPT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_XFER_RESUME) {
        return opcode.CMD_XFER_RESUME.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_XFER_CANCEL) {
        return opcode.CMD_XFER_CANCEL.write();;
    }

    return {}; /* unreachable */
}

WOW_LOGIN_MESSAGES_CPP_EXPORT ClientOpcode read_client_opcode(Reader& reader) {
    const uint8_t opcode = reader.read_u8();

    ClientOpcode op;

    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_AUTH_LOGON_PROOF)) {
        return ClientOpcode(::wow_login_messages::version2::CMD_AUTH_LOGON_PROOF_Client_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_AUTH_RECONNECT_PROOF)) {
        return ClientOpcode(::wow_login_messages::version2::CMD_AUTH_RECONNECT_PROOF_Client_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_REALM_LIST)) {
        return ClientOpcode(::wow_login_messages::version2::CMD_REALM_LIST_Client{});
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_XFER_ACCEPT)) {
        return ClientOpcode(::wow_login_messages::version2::CMD_XFER_ACCEPT{});
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_XFER_RESUME)) {
        return ClientOpcode(::wow_login_messages::version2::CMD_XFER_RESUME_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_XFER_CANCEL)) {
        return ClientOpcode(::wow_login_messages::version2::CMD_XFER_CANCEL{});
    }

    return op;
}
template <>
version2::CMD_AUTH_LOGON_CHALLENGE_Server* ServerOpcode::get_if<CMD_AUTH_LOGON_CHALLENGE_Server>() {
    if (opcode == Opcode::CMD_AUTH_LOGON_CHALLENGE) {
        return &CMD_AUTH_LOGON_CHALLENGE;
    }
    return nullptr;
}
template <>
version2::CMD_AUTH_LOGON_CHALLENGE_Server& ServerOpcode::get<CMD_AUTH_LOGON_CHALLENGE_Server>() {
    auto p = ServerOpcode::get_if<version2::CMD_AUTH_LOGON_CHALLENGE_Server>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version2::CMD_AUTH_LOGON_PROOF_Server* ServerOpcode::get_if<CMD_AUTH_LOGON_PROOF_Server>() {
    if (opcode == Opcode::CMD_AUTH_LOGON_PROOF) {
        return &CMD_AUTH_LOGON_PROOF;
    }
    return nullptr;
}
template <>
version2::CMD_AUTH_LOGON_PROOF_Server& ServerOpcode::get<CMD_AUTH_LOGON_PROOF_Server>() {
    auto p = ServerOpcode::get_if<version2::CMD_AUTH_LOGON_PROOF_Server>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version2::CMD_AUTH_RECONNECT_CHALLENGE_Server* ServerOpcode::get_if<CMD_AUTH_RECONNECT_CHALLENGE_Server>() {
    if (opcode == Opcode::CMD_AUTH_RECONNECT_CHALLENGE) {
        return &CMD_AUTH_RECONNECT_CHALLENGE;
    }
    return nullptr;
}
template <>
version2::CMD_AUTH_RECONNECT_CHALLENGE_Server& ServerOpcode::get<CMD_AUTH_RECONNECT_CHALLENGE_Server>() {
    auto p = ServerOpcode::get_if<version2::CMD_AUTH_RECONNECT_CHALLENGE_Server>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version2::CMD_AUTH_RECONNECT_PROOF_Server* ServerOpcode::get_if<CMD_AUTH_RECONNECT_PROOF_Server>() {
    if (opcode == Opcode::CMD_AUTH_RECONNECT_PROOF) {
        return &CMD_AUTH_RECONNECT_PROOF;
    }
    return nullptr;
}
template <>
version2::CMD_AUTH_RECONNECT_PROOF_Server& ServerOpcode::get<CMD_AUTH_RECONNECT_PROOF_Server>() {
    auto p = ServerOpcode::get_if<version2::CMD_AUTH_RECONNECT_PROOF_Server>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version2::CMD_REALM_LIST_Server* ServerOpcode::get_if<CMD_REALM_LIST_Server>() {
    if (opcode == Opcode::CMD_REALM_LIST) {
        return &CMD_REALM_LIST;
    }
    return nullptr;
}
template <>
version2::CMD_REALM_LIST_Server& ServerOpcode::get<CMD_REALM_LIST_Server>() {
    auto p = ServerOpcode::get_if<version2::CMD_REALM_LIST_Server>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version2::CMD_XFER_INITIATE* ServerOpcode::get_if<CMD_XFER_INITIATE>() {
    if (opcode == Opcode::CMD_XFER_INITIATE) {
        return &CMD_XFER_INITIATE;
    }
    return nullptr;
}
template <>
version2::CMD_XFER_INITIATE& ServerOpcode::get<CMD_XFER_INITIATE>() {
    auto p = ServerOpcode::get_if<version2::CMD_XFER_INITIATE>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version2::CMD_XFER_DATA* ServerOpcode::get_if<CMD_XFER_DATA>() {
    if (opcode == Opcode::CMD_XFER_DATA) {
        return &CMD_XFER_DATA;
    }
    return nullptr;
}
template <>
version2::CMD_XFER_DATA& ServerOpcode::get<CMD_XFER_DATA>() {
    auto p = ServerOpcode::get_if<version2::CMD_XFER_DATA>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}


WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write_opcode(const ServerOpcode& opcode) {
    if (opcode.opcode == ServerOpcode::Opcode::CMD_AUTH_LOGON_CHALLENGE) {
        return opcode.CMD_AUTH_LOGON_CHALLENGE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_AUTH_LOGON_PROOF) {
        return opcode.CMD_AUTH_LOGON_PROOF.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_AUTH_RECONNECT_CHALLENGE) {
        return opcode.CMD_AUTH_RECONNECT_CHALLENGE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_AUTH_RECONNECT_PROOF) {
        return opcode.CMD_AUTH_RECONNECT_PROOF.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_REALM_LIST) {
        return opcode.CMD_REALM_LIST.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_XFER_INITIATE) {
        return opcode.CMD_XFER_INITIATE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_XFER_DATA) {
        return opcode.CMD_XFER_DATA.write();;
    }

    return {}; /* unreachable */
}

WOW_LOGIN_MESSAGES_CPP_EXPORT ServerOpcode read_server_opcode(Reader& reader) {
    const uint8_t opcode = reader.read_u8();

    ServerOpcode op;

    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_AUTH_LOGON_CHALLENGE)) {
        return ServerOpcode(::wow_login_messages::version2::CMD_AUTH_LOGON_CHALLENGE_Server_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_AUTH_LOGON_PROOF)) {
        return ServerOpcode(::wow_login_messages::version2::CMD_AUTH_LOGON_PROOF_Server_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_AUTH_RECONNECT_CHALLENGE)) {
        return ServerOpcode(::wow_login_messages::version2::CMD_AUTH_RECONNECT_CHALLENGE_Server_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_AUTH_RECONNECT_PROOF)) {
        return ServerOpcode(::wow_login_messages::version2::CMD_AUTH_RECONNECT_PROOF_Server_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_REALM_LIST)) {
        return ServerOpcode(::wow_login_messages::version2::CMD_REALM_LIST_Server_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_XFER_INITIATE)) {
        return ServerOpcode(::wow_login_messages::version2::CMD_XFER_INITIATE_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_XFER_DATA)) {
        return ServerOpcode(::wow_login_messages::version2::CMD_XFER_DATA_read(reader));
    }

    return op;
}
} // namespace version2
} // namespace wow_login_messages
#include "wow_login_messages_cpp/version3.hpp"

namespace wow_login_messages {
namespace version3 {
static size_t CMD_AUTH_LOGON_CHALLENGE_Server_size(const CMD_AUTH_LOGON_CHALLENGE_Server& obj) {
    size_t _size = 2;

    if (obj.result == LoginResult::SUCCESS) {
        _size += 83 + 1 * obj.generator.size() + 1 * obj.large_safe_prime.size();

        if (obj.security_flag == SecurityFlag::PIN) {
            _size += 20;
        }

    }

    return _size;
}

CMD_AUTH_LOGON_CHALLENGE_Server CMD_AUTH_LOGON_CHALLENGE_Server_read(Reader& reader) {
    CMD_AUTH_LOGON_CHALLENGE_Server obj{};

    (void)reader.read_u8();

    obj.result = static_cast<LoginResult>(reader.read_u8());

    if (obj.result == LoginResult::SUCCESS) {
        for (auto i = 0; i < 32; ++i) {
            obj.server_public_key[i] = reader.read_u8();
        }

        obj.generator_length = reader.read_u8();

        for (uint8_t i = 0; i < obj.generator_length; ++i) {
            obj.generator.push_back(reader.read_u8());
        }

        obj.large_safe_prime_length = reader.read_u8();

        for (uint8_t i = 0; i < obj.large_safe_prime_length; ++i) {
            obj.large_safe_prime.push_back(reader.read_u8());
        }

        for (auto i = 0; i < 32; ++i) {
            obj.salt[i] = reader.read_u8();
        }

        for (auto i = 0; i < 16; ++i) {
            obj.crc_salt[i] = reader.read_u8();
        }

        obj.security_flag = static_cast<SecurityFlag>(reader.read_u8());

        if (obj.security_flag == SecurityFlag::PIN) {
            obj.pin_grid_seed = reader.read_u32();

            for (auto i = 0; i < 16; ++i) {
                obj.pin_salt[i] = reader.read_u8();
            }

        }
    }
    return obj;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_AUTH_LOGON_CHALLENGE_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMD_AUTH_LOGON_CHALLENGE_Server_size(obj));

    writer.write_u8(0x00); /* opcode */

    writer.write_u8(0);

    writer.write_u8(static_cast<uint8_t>(obj.result));

    if (obj.result == LoginResult::SUCCESS) {
        for (const auto& v : obj.server_public_key) {
            writer.write_u8(v);
        }

        writer.write_u8(obj.generator_length);

        for (const auto& v : obj.generator) {
            writer.write_u8(v);
        }

        writer.write_u8(obj.large_safe_prime_length);

        for (const auto& v : obj.large_safe_prime) {
            writer.write_u8(v);
        }

        for (const auto& v : obj.salt) {
            writer.write_u8(v);
        }

        for (const auto& v : obj.crc_salt) {
            writer.write_u8(v);
        }

        writer.write_u8(static_cast<uint8_t>(obj.security_flag));

        if (obj.security_flag == SecurityFlag::PIN) {
            writer.write_u32(obj.pin_grid_seed);

            for (const auto& v : obj.pin_salt) {
                writer.write_u8(v);
            }

        }
    }
    return writer.m_buf;
}

static size_t CMD_AUTH_LOGON_PROOF_Client_size(const CMD_AUTH_LOGON_PROOF_Client& obj) {
    size_t _size = 74 + 30 * obj.telemetry_keys.size();

    if (obj.security_flag == SecurityFlag::PIN) {
        _size += 36;
    }

    return _size;
}

CMD_AUTH_LOGON_PROOF_Client CMD_AUTH_LOGON_PROOF_Client_read(Reader& reader) {
    CMD_AUTH_LOGON_PROOF_Client obj{};

    for (auto i = 0; i < 32; ++i) {
        obj.client_public_key[i] = reader.read_u8();
    }

    for (auto i = 0; i < 20; ++i) {
        obj.client_proof[i] = reader.read_u8();
    }

    for (auto i = 0; i < 20; ++i) {
        obj.crc_hash[i] = reader.read_u8();
    }

    obj.number_of_telemetry_keys = reader.read_u8();

    for (uint8_t i = 0; i < obj.number_of_telemetry_keys; ++i) {
        obj.telemetry_keys.push_back(::wow_login_messages::version2::TelemetryKey_read(reader));
    }

    obj.security_flag = static_cast<SecurityFlag>(reader.read_u8());

    if (obj.security_flag == SecurityFlag::PIN) {
        for (auto i = 0; i < 16; ++i) {
            obj.pin_salt[i] = reader.read_u8();
        }

        for (auto i = 0; i < 20; ++i) {
            obj.pin_hash[i] = reader.read_u8();
        }

    }
    return obj;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_AUTH_LOGON_PROOF_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMD_AUTH_LOGON_PROOF_Client_size(obj));

    writer.write_u8(0x01); /* opcode */

    for (const auto& v : obj.client_public_key) {
        writer.write_u8(v);
    }

    for (const auto& v : obj.client_proof) {
        writer.write_u8(v);
    }

    for (const auto& v : obj.crc_hash) {
        writer.write_u8(v);
    }

    writer.write_u8(obj.number_of_telemetry_keys);

    for (const auto& v : obj.telemetry_keys) {
        TelemetryKey_write(writer, v);
    }

    writer.write_u8(static_cast<uint8_t>(obj.security_flag));

    if (obj.security_flag == SecurityFlag::PIN) {
        for (const auto& v : obj.pin_salt) {
            writer.write_u8(v);
        }

        for (const auto& v : obj.pin_hash) {
            writer.write_u8(v);
        }

    }
    return writer.m_buf;
}

static size_t CMD_SURVEY_RESULT_size(const CMD_SURVEY_RESULT& obj) {
    return 7 + 1 * obj.data.size();
}

CMD_SURVEY_RESULT CMD_SURVEY_RESULT_read(Reader& reader) {
    CMD_SURVEY_RESULT obj{};

    obj.survey_id = reader.read_u32();

    obj.error = reader.read_u8();

    obj.compressed_data_length = reader.read_u16();

    for (uint16_t i = 0; i < obj.compressed_data_length; ++i) {
        obj.data.push_back(reader.read_u8());
    }

    return obj;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_SURVEY_RESULT::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMD_SURVEY_RESULT_size(obj));

    writer.write_u8(0x04); /* opcode */

    writer.write_u32(obj.survey_id);

    writer.write_u8(obj.error);

    writer.write_u16(obj.compressed_data_length);

    for (const auto& v : obj.data) {
        writer.write_u8(v);
    }

    return writer.m_buf;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_REALM_LIST_Client::write() const {
    auto writer = Writer(0x0004);

    writer.write_u8(0x10); /* opcode */

    writer.write_u32(0);

    return writer.m_buf;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_XFER_ACCEPT::write() const {
    auto writer = Writer(0x0000);

    writer.write_u8(0x32); /* opcode */

    return writer.m_buf;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_XFER_CANCEL::write() const {
    auto writer = Writer(0x0000);

    writer.write_u8(0x34); /* opcode */

    return writer.m_buf;
}

template <>
version3::CMD_AUTH_LOGON_PROOF_Client* ClientOpcode::get_if<CMD_AUTH_LOGON_PROOF_Client>() {
    if (opcode == Opcode::CMD_AUTH_LOGON_PROOF) {
        return &CMD_AUTH_LOGON_PROOF;
    }
    return nullptr;
}
template <>
version3::CMD_AUTH_LOGON_PROOF_Client& ClientOpcode::get<CMD_AUTH_LOGON_PROOF_Client>() {
    auto p = ClientOpcode::get_if<version3::CMD_AUTH_LOGON_PROOF_Client>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version3::CMD_SURVEY_RESULT* ClientOpcode::get_if<CMD_SURVEY_RESULT>() {
    if (opcode == Opcode::CMD_SURVEY_RESULT) {
        return &CMD_SURVEY_RESULT;
    }
    return nullptr;
}
template <>
version3::CMD_SURVEY_RESULT& ClientOpcode::get<CMD_SURVEY_RESULT>() {
    auto p = ClientOpcode::get_if<version3::CMD_SURVEY_RESULT>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version3::CMD_REALM_LIST_Client* ClientOpcode::get_if<CMD_REALM_LIST_Client>() {
    if (opcode == Opcode::CMD_REALM_LIST) {
        return &CMD_REALM_LIST;
    }
    return nullptr;
}
template <>
version3::CMD_REALM_LIST_Client& ClientOpcode::get<CMD_REALM_LIST_Client>() {
    auto p = ClientOpcode::get_if<version3::CMD_REALM_LIST_Client>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version3::CMD_XFER_ACCEPT* ClientOpcode::get_if<CMD_XFER_ACCEPT>() {
    if (opcode == Opcode::CMD_XFER_ACCEPT) {
        return &CMD_XFER_ACCEPT;
    }
    return nullptr;
}
template <>
version3::CMD_XFER_ACCEPT& ClientOpcode::get<CMD_XFER_ACCEPT>() {
    auto p = ClientOpcode::get_if<version3::CMD_XFER_ACCEPT>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version3::CMD_XFER_RESUME* ClientOpcode::get_if<CMD_XFER_RESUME>() {
    if (opcode == Opcode::CMD_XFER_RESUME) {
        return &CMD_XFER_RESUME;
    }
    return nullptr;
}
template <>
version3::CMD_XFER_RESUME& ClientOpcode::get<CMD_XFER_RESUME>() {
    auto p = ClientOpcode::get_if<version3::CMD_XFER_RESUME>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version3::CMD_XFER_CANCEL* ClientOpcode::get_if<CMD_XFER_CANCEL>() {
    if (opcode == Opcode::CMD_XFER_CANCEL) {
        return &CMD_XFER_CANCEL;
    }
    return nullptr;
}
template <>
version3::CMD_XFER_CANCEL& ClientOpcode::get<CMD_XFER_CANCEL>() {
    auto p = ClientOpcode::get_if<version3::CMD_XFER_CANCEL>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}


WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write_opcode(const ClientOpcode& opcode) {
    if (opcode.opcode == ClientOpcode::Opcode::CMD_AUTH_LOGON_PROOF) {
        return opcode.CMD_AUTH_LOGON_PROOF.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_SURVEY_RESULT) {
        return opcode.CMD_SURVEY_RESULT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_REALM_LIST) {
        return opcode.CMD_REALM_LIST.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_XFER_ACCEPT) {
        return opcode.CMD_XFER_ACCEPT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_XFER_RESUME) {
        return opcode.CMD_XFER_RESUME.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_XFER_CANCEL) {
        return opcode.CMD_XFER_CANCEL.write();;
    }

    return {}; /* unreachable */
}

WOW_LOGIN_MESSAGES_CPP_EXPORT ClientOpcode read_client_opcode(Reader& reader) {
    const uint8_t opcode = reader.read_u8();

    ClientOpcode op;

    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_AUTH_LOGON_PROOF)) {
        return ClientOpcode(::wow_login_messages::version3::CMD_AUTH_LOGON_PROOF_Client_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_SURVEY_RESULT)) {
        return ClientOpcode(::wow_login_messages::version3::CMD_SURVEY_RESULT_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_REALM_LIST)) {
        return ClientOpcode(::wow_login_messages::version3::CMD_REALM_LIST_Client{});
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_XFER_ACCEPT)) {
        return ClientOpcode(::wow_login_messages::version3::CMD_XFER_ACCEPT{});
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_XFER_RESUME)) {
        return ClientOpcode(::wow_login_messages::version2::CMD_XFER_RESUME_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_XFER_CANCEL)) {
        return ClientOpcode(::wow_login_messages::version3::CMD_XFER_CANCEL{});
    }

    return op;
}
template <>
version3::CMD_AUTH_LOGON_CHALLENGE_Server* ServerOpcode::get_if<CMD_AUTH_LOGON_CHALLENGE_Server>() {
    if (opcode == Opcode::CMD_AUTH_LOGON_CHALLENGE) {
        return &CMD_AUTH_LOGON_CHALLENGE;
    }
    return nullptr;
}
template <>
version3::CMD_AUTH_LOGON_CHALLENGE_Server& ServerOpcode::get<CMD_AUTH_LOGON_CHALLENGE_Server>() {
    auto p = ServerOpcode::get_if<version3::CMD_AUTH_LOGON_CHALLENGE_Server>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version3::CMD_AUTH_LOGON_PROOF_Server* ServerOpcode::get_if<CMD_AUTH_LOGON_PROOF_Server>() {
    if (opcode == Opcode::CMD_AUTH_LOGON_PROOF) {
        return &CMD_AUTH_LOGON_PROOF;
    }
    return nullptr;
}
template <>
version3::CMD_AUTH_LOGON_PROOF_Server& ServerOpcode::get<CMD_AUTH_LOGON_PROOF_Server>() {
    auto p = ServerOpcode::get_if<version3::CMD_AUTH_LOGON_PROOF_Server>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version3::CMD_REALM_LIST_Server* ServerOpcode::get_if<CMD_REALM_LIST_Server>() {
    if (opcode == Opcode::CMD_REALM_LIST) {
        return &CMD_REALM_LIST;
    }
    return nullptr;
}
template <>
version3::CMD_REALM_LIST_Server& ServerOpcode::get<CMD_REALM_LIST_Server>() {
    auto p = ServerOpcode::get_if<version3::CMD_REALM_LIST_Server>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version3::CMD_XFER_INITIATE* ServerOpcode::get_if<CMD_XFER_INITIATE>() {
    if (opcode == Opcode::CMD_XFER_INITIATE) {
        return &CMD_XFER_INITIATE;
    }
    return nullptr;
}
template <>
version3::CMD_XFER_INITIATE& ServerOpcode::get<CMD_XFER_INITIATE>() {
    auto p = ServerOpcode::get_if<version3::CMD_XFER_INITIATE>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version3::CMD_XFER_DATA* ServerOpcode::get_if<CMD_XFER_DATA>() {
    if (opcode == Opcode::CMD_XFER_DATA) {
        return &CMD_XFER_DATA;
    }
    return nullptr;
}
template <>
version3::CMD_XFER_DATA& ServerOpcode::get<CMD_XFER_DATA>() {
    auto p = ServerOpcode::get_if<version3::CMD_XFER_DATA>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}


WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write_opcode(const ServerOpcode& opcode) {
    if (opcode.opcode == ServerOpcode::Opcode::CMD_AUTH_LOGON_CHALLENGE) {
        return opcode.CMD_AUTH_LOGON_CHALLENGE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_AUTH_LOGON_PROOF) {
        return opcode.CMD_AUTH_LOGON_PROOF.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_REALM_LIST) {
        return opcode.CMD_REALM_LIST.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_XFER_INITIATE) {
        return opcode.CMD_XFER_INITIATE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_XFER_DATA) {
        return opcode.CMD_XFER_DATA.write();;
    }

    return {}; /* unreachable */
}

WOW_LOGIN_MESSAGES_CPP_EXPORT ServerOpcode read_server_opcode(Reader& reader) {
    const uint8_t opcode = reader.read_u8();

    ServerOpcode op;

    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_AUTH_LOGON_CHALLENGE)) {
        return ServerOpcode(::wow_login_messages::version3::CMD_AUTH_LOGON_CHALLENGE_Server_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_AUTH_LOGON_PROOF)) {
        return ServerOpcode(::wow_login_messages::version2::CMD_AUTH_LOGON_PROOF_Server_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_REALM_LIST)) {
        return ServerOpcode(::wow_login_messages::version2::CMD_REALM_LIST_Server_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_XFER_INITIATE)) {
        return ServerOpcode(::wow_login_messages::version2::CMD_XFER_INITIATE_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_XFER_DATA)) {
        return ServerOpcode(::wow_login_messages::version2::CMD_XFER_DATA_read(reader));
    }

    return op;
}
} // namespace version3
} // namespace wow_login_messages
#include "wow_login_messages_cpp/version5.hpp"

namespace wow_login_messages {
namespace version5 {
static size_t Realm_size(const Realm& obj) {
    return 12 + obj.name.size() + obj.address.size();
}

Realm Realm_read(Reader& reader) {
    Realm obj{};

    obj.realm_type = static_cast<RealmType>(reader.read_u8());

    obj.locked = reader.read_bool8();

    obj.flag = static_cast<RealmFlag>(reader.read_u8());

    obj.name = reader.read_cstring();

    obj.address = reader.read_cstring();

    obj.population = reader.read_float();

    obj.number_of_characters_on_realm = reader.read_u8();

    obj.category = static_cast<RealmCategory>(reader.read_u8());

    obj.realm_id = reader.read_u8();

    return obj;
}

static void Realm_write(Writer& writer, const Realm& obj) {
    writer.write_u8(static_cast<uint8_t>(obj.realm_type));

    writer.write_bool8(obj.locked);

    writer.write_u8(static_cast<uint8_t>(obj.flag));

    writer.write_cstring(obj.name);

    writer.write_cstring(obj.address);

    writer.write_float(obj.population);

    writer.write_u8(obj.number_of_characters_on_realm);

    writer.write_u8(static_cast<uint8_t>(obj.category));

    writer.write_u8(obj.realm_id);

}

static size_t CMD_AUTH_LOGON_CHALLENGE_Server_size(const CMD_AUTH_LOGON_CHALLENGE_Server& obj) {
    size_t _size = 2;

    if (obj.result == LoginResult::SUCCESS) {
        _size += 83 + 1 * obj.generator.size() + 1 * obj.large_safe_prime.size();

        if ((obj.security_flag & SECURITY_FLAG_PIN) != 0) {
            _size += 20;
        }

        if ((obj.security_flag & SECURITY_FLAG_MATRIX_CARD) != 0) {
            _size += 12;
        }

    }

    return _size;
}

CMD_AUTH_LOGON_CHALLENGE_Server CMD_AUTH_LOGON_CHALLENGE_Server_read(Reader& reader) {
    CMD_AUTH_LOGON_CHALLENGE_Server obj{};

    (void)reader.read_u8();

    obj.result = static_cast<LoginResult>(reader.read_u8());

    if (obj.result == LoginResult::SUCCESS) {
        for (auto i = 0; i < 32; ++i) {
            obj.server_public_key[i] = reader.read_u8();
        }

        obj.generator_length = reader.read_u8();

        for (uint8_t i = 0; i < obj.generator_length; ++i) {
            obj.generator.push_back(reader.read_u8());
        }

        obj.large_safe_prime_length = reader.read_u8();

        for (uint8_t i = 0; i < obj.large_safe_prime_length; ++i) {
            obj.large_safe_prime.push_back(reader.read_u8());
        }

        for (auto i = 0; i < 32; ++i) {
            obj.salt[i] = reader.read_u8();
        }

        for (auto i = 0; i < 16; ++i) {
            obj.crc_salt[i] = reader.read_u8();
        }

        obj.security_flag = static_cast<SecurityFlag>(reader.read_u8());

        if ((obj.security_flag & SECURITY_FLAG_PIN) != 0) {
            obj.pin_grid_seed = reader.read_u32();

            for (auto i = 0; i < 16; ++i) {
                obj.pin_salt[i] = reader.read_u8();
            }

        }
        if ((obj.security_flag & SECURITY_FLAG_MATRIX_CARD) != 0) {
            obj.width = reader.read_u8();

            obj.height = reader.read_u8();

            obj.digit_count = reader.read_u8();

            obj.challenge_count = reader.read_u8();

            obj.seed = reader.read_u64();

        }
    }
    return obj;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_AUTH_LOGON_CHALLENGE_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMD_AUTH_LOGON_CHALLENGE_Server_size(obj));

    writer.write_u8(0x00); /* opcode */

    writer.write_u8(0);

    writer.write_u8(static_cast<uint8_t>(obj.result));

    if (obj.result == LoginResult::SUCCESS) {
        for (const auto& v : obj.server_public_key) {
            writer.write_u8(v);
        }

        writer.write_u8(obj.generator_length);

        for (const auto& v : obj.generator) {
            writer.write_u8(v);
        }

        writer.write_u8(obj.large_safe_prime_length);

        for (const auto& v : obj.large_safe_prime) {
            writer.write_u8(v);
        }

        for (const auto& v : obj.salt) {
            writer.write_u8(v);
        }

        for (const auto& v : obj.crc_salt) {
            writer.write_u8(v);
        }

        writer.write_u8(static_cast<uint8_t>(obj.security_flag));

        if ((obj.security_flag & SECURITY_FLAG_PIN) != 0) {
            writer.write_u32(obj.pin_grid_seed);

            for (const auto& v : obj.pin_salt) {
                writer.write_u8(v);
            }

        }
        if ((obj.security_flag & SECURITY_FLAG_MATRIX_CARD) != 0) {
            writer.write_u8(obj.width);

            writer.write_u8(obj.height);

            writer.write_u8(obj.digit_count);

            writer.write_u8(obj.challenge_count);

            writer.write_u64(obj.seed);

        }
    }
    return writer.m_buf;
}

static size_t CMD_AUTH_LOGON_PROOF_Client_size(const CMD_AUTH_LOGON_PROOF_Client& obj) {
    size_t _size = 74 + 30 * obj.telemetry_keys.size();

    if ((obj.security_flag & SECURITY_FLAG_PIN) != 0) {
        _size += 36;
    }

    if ((obj.security_flag & SECURITY_FLAG_MATRIX_CARD) != 0) {
        _size += 20;
    }

    return _size;
}

CMD_AUTH_LOGON_PROOF_Client CMD_AUTH_LOGON_PROOF_Client_read(Reader& reader) {
    CMD_AUTH_LOGON_PROOF_Client obj{};

    for (auto i = 0; i < 32; ++i) {
        obj.client_public_key[i] = reader.read_u8();
    }

    for (auto i = 0; i < 20; ++i) {
        obj.client_proof[i] = reader.read_u8();
    }

    for (auto i = 0; i < 20; ++i) {
        obj.crc_hash[i] = reader.read_u8();
    }

    obj.number_of_telemetry_keys = reader.read_u8();

    for (uint8_t i = 0; i < obj.number_of_telemetry_keys; ++i) {
        obj.telemetry_keys.push_back(::wow_login_messages::version2::TelemetryKey_read(reader));
    }

    obj.security_flag = static_cast<SecurityFlag>(reader.read_u8());

    if ((obj.security_flag & SECURITY_FLAG_PIN) != 0) {
        for (auto i = 0; i < 16; ++i) {
            obj.pin_salt[i] = reader.read_u8();
        }

        for (auto i = 0; i < 20; ++i) {
            obj.pin_hash[i] = reader.read_u8();
        }

    }
    if ((obj.security_flag & SECURITY_FLAG_MATRIX_CARD) != 0) {
        for (auto i = 0; i < 20; ++i) {
            obj.matrix_card_proof[i] = reader.read_u8();
        }

    }
    return obj;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_AUTH_LOGON_PROOF_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMD_AUTH_LOGON_PROOF_Client_size(obj));

    writer.write_u8(0x01); /* opcode */

    for (const auto& v : obj.client_public_key) {
        writer.write_u8(v);
    }

    for (const auto& v : obj.client_proof) {
        writer.write_u8(v);
    }

    for (const auto& v : obj.crc_hash) {
        writer.write_u8(v);
    }

    writer.write_u8(obj.number_of_telemetry_keys);

    for (const auto& v : obj.telemetry_keys) {
        TelemetryKey_write(writer, v);
    }

    writer.write_u8(static_cast<uint8_t>(obj.security_flag));

    if ((obj.security_flag & SECURITY_FLAG_PIN) != 0) {
        for (const auto& v : obj.pin_salt) {
            writer.write_u8(v);
        }

        for (const auto& v : obj.pin_hash) {
            writer.write_u8(v);
        }

    }
    if ((obj.security_flag & SECURITY_FLAG_MATRIX_CARD) != 0) {
        for (const auto& v : obj.matrix_card_proof) {
            writer.write_u8(v);
        }

    }
    return writer.m_buf;
}

static size_t CMD_AUTH_LOGON_PROOF_Server_size(const CMD_AUTH_LOGON_PROOF_Server& obj) {
    size_t _size = 1;

    if (obj.result == LoginResult::SUCCESS) {
        _size += 26;
    }

    return _size;
}

CMD_AUTH_LOGON_PROOF_Server CMD_AUTH_LOGON_PROOF_Server_read(Reader& reader) {
    CMD_AUTH_LOGON_PROOF_Server obj{};

    obj.result = static_cast<LoginResult>(reader.read_u8());

    if (obj.result == LoginResult::SUCCESS) {
        for (auto i = 0; i < 20; ++i) {
            obj.server_proof[i] = reader.read_u8();
        }

        obj.hardware_survey_id = reader.read_u32();

        obj.unknown = reader.read_u16();

    }
    return obj;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_AUTH_LOGON_PROOF_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMD_AUTH_LOGON_PROOF_Server_size(obj));

    writer.write_u8(0x01); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.result));

    if (obj.result == LoginResult::SUCCESS) {
        for (const auto& v : obj.server_proof) {
            writer.write_u8(v);
        }

        writer.write_u32(obj.hardware_survey_id);

        writer.write_u16(obj.unknown);

    }
    return writer.m_buf;
}

CMD_AUTH_RECONNECT_PROOF_Server CMD_AUTH_RECONNECT_PROOF_Server_read(Reader& reader) {
    CMD_AUTH_RECONNECT_PROOF_Server obj{};

    obj.result = static_cast<LoginResult>(reader.read_u8());

    (void)reader.read_u16();

    return obj;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_AUTH_RECONNECT_PROOF_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0003);

    writer.write_u8(0x03); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.result));

    writer.write_u16(0);

    return writer.m_buf;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_REALM_LIST_Client::write() const {
    auto writer = Writer(0x0004);

    writer.write_u8(0x10); /* opcode */

    writer.write_u32(0);

    return writer.m_buf;
}

static size_t CMD_REALM_LIST_Server_size(const CMD_REALM_LIST_Server& obj) {
    size_t _size = 7;

    for(const auto& v : obj.realms) {
        _size += Realm_size(v);
    }

    return _size;
}

CMD_REALM_LIST_Server CMD_REALM_LIST_Server_read(Reader& reader) {
    CMD_REALM_LIST_Server obj{};

    (void)reader.read_u16();

    (void)reader.read_u32();

    obj.number_of_realms = reader.read_u8();

    for (uint8_t i = 0; i < obj.number_of_realms; ++i) {
        obj.realms.push_back(::wow_login_messages::version5::Realm_read(reader));
    }

    (void)reader.read_u16();

    return obj;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_REALM_LIST_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMD_REALM_LIST_Server_size(obj));

    writer.write_u8(0x10); /* opcode */

    writer.write_u16(static_cast<uint16_t>(CMD_REALM_LIST_Server_size(obj)));

    writer.write_u32(0);

    writer.write_u8(obj.number_of_realms);

    for (const auto& v : obj.realms) {
        Realm_write(writer, v);
    }

    writer.write_u16(0);

    return writer.m_buf;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_XFER_ACCEPT::write() const {
    auto writer = Writer(0x0000);

    writer.write_u8(0x32); /* opcode */

    return writer.m_buf;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_XFER_CANCEL::write() const {
    auto writer = Writer(0x0000);

    writer.write_u8(0x34); /* opcode */

    return writer.m_buf;
}

template <>
version5::CMD_AUTH_LOGON_PROOF_Client* ClientOpcode::get_if<CMD_AUTH_LOGON_PROOF_Client>() {
    if (opcode == Opcode::CMD_AUTH_LOGON_PROOF) {
        return &CMD_AUTH_LOGON_PROOF;
    }
    return nullptr;
}
template <>
version5::CMD_AUTH_LOGON_PROOF_Client& ClientOpcode::get<CMD_AUTH_LOGON_PROOF_Client>() {
    auto p = ClientOpcode::get_if<version5::CMD_AUTH_LOGON_PROOF_Client>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version5::CMD_AUTH_RECONNECT_PROOF_Client* ClientOpcode::get_if<CMD_AUTH_RECONNECT_PROOF_Client>() {
    if (opcode == Opcode::CMD_AUTH_RECONNECT_PROOF) {
        return &CMD_AUTH_RECONNECT_PROOF;
    }
    return nullptr;
}
template <>
version5::CMD_AUTH_RECONNECT_PROOF_Client& ClientOpcode::get<CMD_AUTH_RECONNECT_PROOF_Client>() {
    auto p = ClientOpcode::get_if<version5::CMD_AUTH_RECONNECT_PROOF_Client>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version5::CMD_REALM_LIST_Client* ClientOpcode::get_if<CMD_REALM_LIST_Client>() {
    if (opcode == Opcode::CMD_REALM_LIST) {
        return &CMD_REALM_LIST;
    }
    return nullptr;
}
template <>
version5::CMD_REALM_LIST_Client& ClientOpcode::get<CMD_REALM_LIST_Client>() {
    auto p = ClientOpcode::get_if<version5::CMD_REALM_LIST_Client>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version5::CMD_XFER_ACCEPT* ClientOpcode::get_if<CMD_XFER_ACCEPT>() {
    if (opcode == Opcode::CMD_XFER_ACCEPT) {
        return &CMD_XFER_ACCEPT;
    }
    return nullptr;
}
template <>
version5::CMD_XFER_ACCEPT& ClientOpcode::get<CMD_XFER_ACCEPT>() {
    auto p = ClientOpcode::get_if<version5::CMD_XFER_ACCEPT>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version5::CMD_XFER_RESUME* ClientOpcode::get_if<CMD_XFER_RESUME>() {
    if (opcode == Opcode::CMD_XFER_RESUME) {
        return &CMD_XFER_RESUME;
    }
    return nullptr;
}
template <>
version5::CMD_XFER_RESUME& ClientOpcode::get<CMD_XFER_RESUME>() {
    auto p = ClientOpcode::get_if<version5::CMD_XFER_RESUME>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version5::CMD_XFER_CANCEL* ClientOpcode::get_if<CMD_XFER_CANCEL>() {
    if (opcode == Opcode::CMD_XFER_CANCEL) {
        return &CMD_XFER_CANCEL;
    }
    return nullptr;
}
template <>
version5::CMD_XFER_CANCEL& ClientOpcode::get<CMD_XFER_CANCEL>() {
    auto p = ClientOpcode::get_if<version5::CMD_XFER_CANCEL>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}


WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write_opcode(const ClientOpcode& opcode) {
    if (opcode.opcode == ClientOpcode::Opcode::CMD_AUTH_LOGON_PROOF) {
        return opcode.CMD_AUTH_LOGON_PROOF.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_AUTH_RECONNECT_PROOF) {
        return opcode.CMD_AUTH_RECONNECT_PROOF.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_REALM_LIST) {
        return opcode.CMD_REALM_LIST.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_XFER_ACCEPT) {
        return opcode.CMD_XFER_ACCEPT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_XFER_RESUME) {
        return opcode.CMD_XFER_RESUME.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_XFER_CANCEL) {
        return opcode.CMD_XFER_CANCEL.write();;
    }

    return {}; /* unreachable */
}

WOW_LOGIN_MESSAGES_CPP_EXPORT ClientOpcode read_client_opcode(Reader& reader) {
    const uint8_t opcode = reader.read_u8();

    ClientOpcode op;

    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_AUTH_LOGON_PROOF)) {
        return ClientOpcode(::wow_login_messages::version5::CMD_AUTH_LOGON_PROOF_Client_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_AUTH_RECONNECT_PROOF)) {
        return ClientOpcode(::wow_login_messages::version2::CMD_AUTH_RECONNECT_PROOF_Client_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_REALM_LIST)) {
        return ClientOpcode(::wow_login_messages::version5::CMD_REALM_LIST_Client{});
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_XFER_ACCEPT)) {
        return ClientOpcode(::wow_login_messages::version5::CMD_XFER_ACCEPT{});
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_XFER_RESUME)) {
        return ClientOpcode(::wow_login_messages::version2::CMD_XFER_RESUME_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_XFER_CANCEL)) {
        return ClientOpcode(::wow_login_messages::version5::CMD_XFER_CANCEL{});
    }

    return op;
}
template <>
version5::CMD_AUTH_LOGON_CHALLENGE_Server* ServerOpcode::get_if<CMD_AUTH_LOGON_CHALLENGE_Server>() {
    if (opcode == Opcode::CMD_AUTH_LOGON_CHALLENGE) {
        return &CMD_AUTH_LOGON_CHALLENGE;
    }
    return nullptr;
}
template <>
version5::CMD_AUTH_LOGON_CHALLENGE_Server& ServerOpcode::get<CMD_AUTH_LOGON_CHALLENGE_Server>() {
    auto p = ServerOpcode::get_if<version5::CMD_AUTH_LOGON_CHALLENGE_Server>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version5::CMD_AUTH_LOGON_PROOF_Server* ServerOpcode::get_if<CMD_AUTH_LOGON_PROOF_Server>() {
    if (opcode == Opcode::CMD_AUTH_LOGON_PROOF) {
        return &CMD_AUTH_LOGON_PROOF;
    }
    return nullptr;
}
template <>
version5::CMD_AUTH_LOGON_PROOF_Server& ServerOpcode::get<CMD_AUTH_LOGON_PROOF_Server>() {
    auto p = ServerOpcode::get_if<version5::CMD_AUTH_LOGON_PROOF_Server>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version5::CMD_AUTH_RECONNECT_CHALLENGE_Server* ServerOpcode::get_if<CMD_AUTH_RECONNECT_CHALLENGE_Server>() {
    if (opcode == Opcode::CMD_AUTH_RECONNECT_CHALLENGE) {
        return &CMD_AUTH_RECONNECT_CHALLENGE;
    }
    return nullptr;
}
template <>
version5::CMD_AUTH_RECONNECT_CHALLENGE_Server& ServerOpcode::get<CMD_AUTH_RECONNECT_CHALLENGE_Server>() {
    auto p = ServerOpcode::get_if<version5::CMD_AUTH_RECONNECT_CHALLENGE_Server>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version5::CMD_AUTH_RECONNECT_PROOF_Server* ServerOpcode::get_if<CMD_AUTH_RECONNECT_PROOF_Server>() {
    if (opcode == Opcode::CMD_AUTH_RECONNECT_PROOF) {
        return &CMD_AUTH_RECONNECT_PROOF;
    }
    return nullptr;
}
template <>
version5::CMD_AUTH_RECONNECT_PROOF_Server& ServerOpcode::get<CMD_AUTH_RECONNECT_PROOF_Server>() {
    auto p = ServerOpcode::get_if<version5::CMD_AUTH_RECONNECT_PROOF_Server>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version5::CMD_REALM_LIST_Server* ServerOpcode::get_if<CMD_REALM_LIST_Server>() {
    if (opcode == Opcode::CMD_REALM_LIST) {
        return &CMD_REALM_LIST;
    }
    return nullptr;
}
template <>
version5::CMD_REALM_LIST_Server& ServerOpcode::get<CMD_REALM_LIST_Server>() {
    auto p = ServerOpcode::get_if<version5::CMD_REALM_LIST_Server>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version5::CMD_XFER_INITIATE* ServerOpcode::get_if<CMD_XFER_INITIATE>() {
    if (opcode == Opcode::CMD_XFER_INITIATE) {
        return &CMD_XFER_INITIATE;
    }
    return nullptr;
}
template <>
version5::CMD_XFER_INITIATE& ServerOpcode::get<CMD_XFER_INITIATE>() {
    auto p = ServerOpcode::get_if<version5::CMD_XFER_INITIATE>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version5::CMD_XFER_DATA* ServerOpcode::get_if<CMD_XFER_DATA>() {
    if (opcode == Opcode::CMD_XFER_DATA) {
        return &CMD_XFER_DATA;
    }
    return nullptr;
}
template <>
version5::CMD_XFER_DATA& ServerOpcode::get<CMD_XFER_DATA>() {
    auto p = ServerOpcode::get_if<version5::CMD_XFER_DATA>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}


WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write_opcode(const ServerOpcode& opcode) {
    if (opcode.opcode == ServerOpcode::Opcode::CMD_AUTH_LOGON_CHALLENGE) {
        return opcode.CMD_AUTH_LOGON_CHALLENGE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_AUTH_LOGON_PROOF) {
        return opcode.CMD_AUTH_LOGON_PROOF.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_AUTH_RECONNECT_CHALLENGE) {
        return opcode.CMD_AUTH_RECONNECT_CHALLENGE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_AUTH_RECONNECT_PROOF) {
        return opcode.CMD_AUTH_RECONNECT_PROOF.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_REALM_LIST) {
        return opcode.CMD_REALM_LIST.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_XFER_INITIATE) {
        return opcode.CMD_XFER_INITIATE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_XFER_DATA) {
        return opcode.CMD_XFER_DATA.write();;
    }

    return {}; /* unreachable */
}

WOW_LOGIN_MESSAGES_CPP_EXPORT ServerOpcode read_server_opcode(Reader& reader) {
    const uint8_t opcode = reader.read_u8();

    ServerOpcode op;

    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_AUTH_LOGON_CHALLENGE)) {
        return ServerOpcode(::wow_login_messages::version5::CMD_AUTH_LOGON_CHALLENGE_Server_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_AUTH_LOGON_PROOF)) {
        return ServerOpcode(::wow_login_messages::version5::CMD_AUTH_LOGON_PROOF_Server_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_AUTH_RECONNECT_CHALLENGE)) {
        return ServerOpcode(::wow_login_messages::version2::CMD_AUTH_RECONNECT_CHALLENGE_Server_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_AUTH_RECONNECT_PROOF)) {
        return ServerOpcode(::wow_login_messages::version5::CMD_AUTH_RECONNECT_PROOF_Server_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_REALM_LIST)) {
        return ServerOpcode(::wow_login_messages::version5::CMD_REALM_LIST_Server_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_XFER_INITIATE)) {
        return ServerOpcode(::wow_login_messages::version2::CMD_XFER_INITIATE_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_XFER_DATA)) {
        return ServerOpcode(::wow_login_messages::version2::CMD_XFER_DATA_read(reader));
    }

    return op;
}
} // namespace version5
} // namespace wow_login_messages
#include "wow_login_messages_cpp/version6.hpp"

namespace wow_login_messages {
namespace version6 {
WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_REALM_LIST_Client::write() const {
    auto writer = Writer(0x0004);

    writer.write_u8(0x10); /* opcode */

    writer.write_u32(0);

    return writer.m_buf;
}

static size_t CMD_REALM_LIST_Server_size(const CMD_REALM_LIST_Server& obj) {
    size_t _size = 8;

    for(const auto& v : obj.realms) {
        _size += Realm_size(v);
    }

    return _size;
}

CMD_REALM_LIST_Server CMD_REALM_LIST_Server_read(Reader& reader) {
    CMD_REALM_LIST_Server obj{};

    (void)reader.read_u16();

    (void)reader.read_u32();

    obj.number_of_realms = reader.read_u16();

    for (uint16_t i = 0; i < obj.number_of_realms; ++i) {
        obj.realms.push_back(::wow_login_messages::version5::Realm_read(reader));
    }

    (void)reader.read_u16();

    return obj;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_REALM_LIST_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMD_REALM_LIST_Server_size(obj));

    writer.write_u8(0x10); /* opcode */

    writer.write_u16(static_cast<uint16_t>(CMD_REALM_LIST_Server_size(obj)));

    writer.write_u32(0);

    writer.write_u16(obj.number_of_realms);

    for (const auto& v : obj.realms) {
        Realm_write(writer, v);
    }

    writer.write_u16(0);

    return writer.m_buf;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_XFER_ACCEPT::write() const {
    auto writer = Writer(0x0000);

    writer.write_u8(0x32); /* opcode */

    return writer.m_buf;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_XFER_CANCEL::write() const {
    auto writer = Writer(0x0000);

    writer.write_u8(0x34); /* opcode */

    return writer.m_buf;
}

template <>
version6::CMD_AUTH_LOGON_PROOF_Client* ClientOpcode::get_if<CMD_AUTH_LOGON_PROOF_Client>() {
    if (opcode == Opcode::CMD_AUTH_LOGON_PROOF) {
        return &CMD_AUTH_LOGON_PROOF;
    }
    return nullptr;
}
template <>
version6::CMD_AUTH_LOGON_PROOF_Client& ClientOpcode::get<CMD_AUTH_LOGON_PROOF_Client>() {
    auto p = ClientOpcode::get_if<version6::CMD_AUTH_LOGON_PROOF_Client>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version6::CMD_AUTH_RECONNECT_PROOF_Client* ClientOpcode::get_if<CMD_AUTH_RECONNECT_PROOF_Client>() {
    if (opcode == Opcode::CMD_AUTH_RECONNECT_PROOF) {
        return &CMD_AUTH_RECONNECT_PROOF;
    }
    return nullptr;
}
template <>
version6::CMD_AUTH_RECONNECT_PROOF_Client& ClientOpcode::get<CMD_AUTH_RECONNECT_PROOF_Client>() {
    auto p = ClientOpcode::get_if<version6::CMD_AUTH_RECONNECT_PROOF_Client>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version6::CMD_REALM_LIST_Client* ClientOpcode::get_if<CMD_REALM_LIST_Client>() {
    if (opcode == Opcode::CMD_REALM_LIST) {
        return &CMD_REALM_LIST;
    }
    return nullptr;
}
template <>
version6::CMD_REALM_LIST_Client& ClientOpcode::get<CMD_REALM_LIST_Client>() {
    auto p = ClientOpcode::get_if<version6::CMD_REALM_LIST_Client>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version6::CMD_XFER_ACCEPT* ClientOpcode::get_if<CMD_XFER_ACCEPT>() {
    if (opcode == Opcode::CMD_XFER_ACCEPT) {
        return &CMD_XFER_ACCEPT;
    }
    return nullptr;
}
template <>
version6::CMD_XFER_ACCEPT& ClientOpcode::get<CMD_XFER_ACCEPT>() {
    auto p = ClientOpcode::get_if<version6::CMD_XFER_ACCEPT>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version6::CMD_XFER_RESUME* ClientOpcode::get_if<CMD_XFER_RESUME>() {
    if (opcode == Opcode::CMD_XFER_RESUME) {
        return &CMD_XFER_RESUME;
    }
    return nullptr;
}
template <>
version6::CMD_XFER_RESUME& ClientOpcode::get<CMD_XFER_RESUME>() {
    auto p = ClientOpcode::get_if<version6::CMD_XFER_RESUME>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version6::CMD_XFER_CANCEL* ClientOpcode::get_if<CMD_XFER_CANCEL>() {
    if (opcode == Opcode::CMD_XFER_CANCEL) {
        return &CMD_XFER_CANCEL;
    }
    return nullptr;
}
template <>
version6::CMD_XFER_CANCEL& ClientOpcode::get<CMD_XFER_CANCEL>() {
    auto p = ClientOpcode::get_if<version6::CMD_XFER_CANCEL>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}


WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write_opcode(const ClientOpcode& opcode) {
    if (opcode.opcode == ClientOpcode::Opcode::CMD_AUTH_LOGON_PROOF) {
        return opcode.CMD_AUTH_LOGON_PROOF.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_AUTH_RECONNECT_PROOF) {
        return opcode.CMD_AUTH_RECONNECT_PROOF.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_REALM_LIST) {
        return opcode.CMD_REALM_LIST.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_XFER_ACCEPT) {
        return opcode.CMD_XFER_ACCEPT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_XFER_RESUME) {
        return opcode.CMD_XFER_RESUME.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_XFER_CANCEL) {
        return opcode.CMD_XFER_CANCEL.write();;
    }

    return {}; /* unreachable */
}

WOW_LOGIN_MESSAGES_CPP_EXPORT ClientOpcode read_client_opcode(Reader& reader) {
    const uint8_t opcode = reader.read_u8();

    ClientOpcode op;

    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_AUTH_LOGON_PROOF)) {
        return ClientOpcode(::wow_login_messages::version5::CMD_AUTH_LOGON_PROOF_Client_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_AUTH_RECONNECT_PROOF)) {
        return ClientOpcode(::wow_login_messages::version2::CMD_AUTH_RECONNECT_PROOF_Client_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_REALM_LIST)) {
        return ClientOpcode(::wow_login_messages::version6::CMD_REALM_LIST_Client{});
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_XFER_ACCEPT)) {
        return ClientOpcode(::wow_login_messages::version6::CMD_XFER_ACCEPT{});
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_XFER_RESUME)) {
        return ClientOpcode(::wow_login_messages::version2::CMD_XFER_RESUME_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_XFER_CANCEL)) {
        return ClientOpcode(::wow_login_messages::version6::CMD_XFER_CANCEL{});
    }

    return op;
}
template <>
version6::CMD_AUTH_LOGON_CHALLENGE_Server* ServerOpcode::get_if<CMD_AUTH_LOGON_CHALLENGE_Server>() {
    if (opcode == Opcode::CMD_AUTH_LOGON_CHALLENGE) {
        return &CMD_AUTH_LOGON_CHALLENGE;
    }
    return nullptr;
}
template <>
version6::CMD_AUTH_LOGON_CHALLENGE_Server& ServerOpcode::get<CMD_AUTH_LOGON_CHALLENGE_Server>() {
    auto p = ServerOpcode::get_if<version6::CMD_AUTH_LOGON_CHALLENGE_Server>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version6::CMD_AUTH_LOGON_PROOF_Server* ServerOpcode::get_if<CMD_AUTH_LOGON_PROOF_Server>() {
    if (opcode == Opcode::CMD_AUTH_LOGON_PROOF) {
        return &CMD_AUTH_LOGON_PROOF;
    }
    return nullptr;
}
template <>
version6::CMD_AUTH_LOGON_PROOF_Server& ServerOpcode::get<CMD_AUTH_LOGON_PROOF_Server>() {
    auto p = ServerOpcode::get_if<version6::CMD_AUTH_LOGON_PROOF_Server>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version6::CMD_AUTH_RECONNECT_CHALLENGE_Server* ServerOpcode::get_if<CMD_AUTH_RECONNECT_CHALLENGE_Server>() {
    if (opcode == Opcode::CMD_AUTH_RECONNECT_CHALLENGE) {
        return &CMD_AUTH_RECONNECT_CHALLENGE;
    }
    return nullptr;
}
template <>
version6::CMD_AUTH_RECONNECT_CHALLENGE_Server& ServerOpcode::get<CMD_AUTH_RECONNECT_CHALLENGE_Server>() {
    auto p = ServerOpcode::get_if<version6::CMD_AUTH_RECONNECT_CHALLENGE_Server>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version6::CMD_AUTH_RECONNECT_PROOF_Server* ServerOpcode::get_if<CMD_AUTH_RECONNECT_PROOF_Server>() {
    if (opcode == Opcode::CMD_AUTH_RECONNECT_PROOF) {
        return &CMD_AUTH_RECONNECT_PROOF;
    }
    return nullptr;
}
template <>
version6::CMD_AUTH_RECONNECT_PROOF_Server& ServerOpcode::get<CMD_AUTH_RECONNECT_PROOF_Server>() {
    auto p = ServerOpcode::get_if<version6::CMD_AUTH_RECONNECT_PROOF_Server>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version6::CMD_REALM_LIST_Server* ServerOpcode::get_if<CMD_REALM_LIST_Server>() {
    if (opcode == Opcode::CMD_REALM_LIST) {
        return &CMD_REALM_LIST;
    }
    return nullptr;
}
template <>
version6::CMD_REALM_LIST_Server& ServerOpcode::get<CMD_REALM_LIST_Server>() {
    auto p = ServerOpcode::get_if<version6::CMD_REALM_LIST_Server>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version6::CMD_XFER_INITIATE* ServerOpcode::get_if<CMD_XFER_INITIATE>() {
    if (opcode == Opcode::CMD_XFER_INITIATE) {
        return &CMD_XFER_INITIATE;
    }
    return nullptr;
}
template <>
version6::CMD_XFER_INITIATE& ServerOpcode::get<CMD_XFER_INITIATE>() {
    auto p = ServerOpcode::get_if<version6::CMD_XFER_INITIATE>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version6::CMD_XFER_DATA* ServerOpcode::get_if<CMD_XFER_DATA>() {
    if (opcode == Opcode::CMD_XFER_DATA) {
        return &CMD_XFER_DATA;
    }
    return nullptr;
}
template <>
version6::CMD_XFER_DATA& ServerOpcode::get<CMD_XFER_DATA>() {
    auto p = ServerOpcode::get_if<version6::CMD_XFER_DATA>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}


WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write_opcode(const ServerOpcode& opcode) {
    if (opcode.opcode == ServerOpcode::Opcode::CMD_AUTH_LOGON_CHALLENGE) {
        return opcode.CMD_AUTH_LOGON_CHALLENGE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_AUTH_LOGON_PROOF) {
        return opcode.CMD_AUTH_LOGON_PROOF.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_AUTH_RECONNECT_CHALLENGE) {
        return opcode.CMD_AUTH_RECONNECT_CHALLENGE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_AUTH_RECONNECT_PROOF) {
        return opcode.CMD_AUTH_RECONNECT_PROOF.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_REALM_LIST) {
        return opcode.CMD_REALM_LIST.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_XFER_INITIATE) {
        return opcode.CMD_XFER_INITIATE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_XFER_DATA) {
        return opcode.CMD_XFER_DATA.write();;
    }

    return {}; /* unreachable */
}

WOW_LOGIN_MESSAGES_CPP_EXPORT ServerOpcode read_server_opcode(Reader& reader) {
    const uint8_t opcode = reader.read_u8();

    ServerOpcode op;

    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_AUTH_LOGON_CHALLENGE)) {
        return ServerOpcode(::wow_login_messages::version5::CMD_AUTH_LOGON_CHALLENGE_Server_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_AUTH_LOGON_PROOF)) {
        return ServerOpcode(::wow_login_messages::version5::CMD_AUTH_LOGON_PROOF_Server_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_AUTH_RECONNECT_CHALLENGE)) {
        return ServerOpcode(::wow_login_messages::version2::CMD_AUTH_RECONNECT_CHALLENGE_Server_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_AUTH_RECONNECT_PROOF)) {
        return ServerOpcode(::wow_login_messages::version5::CMD_AUTH_RECONNECT_PROOF_Server_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_REALM_LIST)) {
        return ServerOpcode(::wow_login_messages::version6::CMD_REALM_LIST_Server_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_XFER_INITIATE)) {
        return ServerOpcode(::wow_login_messages::version2::CMD_XFER_INITIATE_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_XFER_DATA)) {
        return ServerOpcode(::wow_login_messages::version2::CMD_XFER_DATA_read(reader));
    }

    return op;
}
} // namespace version6
} // namespace wow_login_messages
#include "wow_login_messages_cpp/version7.hpp"

namespace wow_login_messages {
namespace version7 {
WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_REALM_LIST_Client::write() const {
    auto writer = Writer(0x0004);

    writer.write_u8(0x10); /* opcode */

    writer.write_u32(0);

    return writer.m_buf;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_XFER_ACCEPT::write() const {
    auto writer = Writer(0x0000);

    writer.write_u8(0x32); /* opcode */

    return writer.m_buf;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_XFER_CANCEL::write() const {
    auto writer = Writer(0x0000);

    writer.write_u8(0x34); /* opcode */

    return writer.m_buf;
}

template <>
version7::CMD_AUTH_LOGON_PROOF_Client* ClientOpcode::get_if<CMD_AUTH_LOGON_PROOF_Client>() {
    if (opcode == Opcode::CMD_AUTH_LOGON_PROOF) {
        return &CMD_AUTH_LOGON_PROOF;
    }
    return nullptr;
}
template <>
version7::CMD_AUTH_LOGON_PROOF_Client& ClientOpcode::get<CMD_AUTH_LOGON_PROOF_Client>() {
    auto p = ClientOpcode::get_if<version7::CMD_AUTH_LOGON_PROOF_Client>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version7::CMD_AUTH_RECONNECT_PROOF_Client* ClientOpcode::get_if<CMD_AUTH_RECONNECT_PROOF_Client>() {
    if (opcode == Opcode::CMD_AUTH_RECONNECT_PROOF) {
        return &CMD_AUTH_RECONNECT_PROOF;
    }
    return nullptr;
}
template <>
version7::CMD_AUTH_RECONNECT_PROOF_Client& ClientOpcode::get<CMD_AUTH_RECONNECT_PROOF_Client>() {
    auto p = ClientOpcode::get_if<version7::CMD_AUTH_RECONNECT_PROOF_Client>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version7::CMD_REALM_LIST_Client* ClientOpcode::get_if<CMD_REALM_LIST_Client>() {
    if (opcode == Opcode::CMD_REALM_LIST) {
        return &CMD_REALM_LIST;
    }
    return nullptr;
}
template <>
version7::CMD_REALM_LIST_Client& ClientOpcode::get<CMD_REALM_LIST_Client>() {
    auto p = ClientOpcode::get_if<version7::CMD_REALM_LIST_Client>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version7::CMD_XFER_ACCEPT* ClientOpcode::get_if<CMD_XFER_ACCEPT>() {
    if (opcode == Opcode::CMD_XFER_ACCEPT) {
        return &CMD_XFER_ACCEPT;
    }
    return nullptr;
}
template <>
version7::CMD_XFER_ACCEPT& ClientOpcode::get<CMD_XFER_ACCEPT>() {
    auto p = ClientOpcode::get_if<version7::CMD_XFER_ACCEPT>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version7::CMD_XFER_RESUME* ClientOpcode::get_if<CMD_XFER_RESUME>() {
    if (opcode == Opcode::CMD_XFER_RESUME) {
        return &CMD_XFER_RESUME;
    }
    return nullptr;
}
template <>
version7::CMD_XFER_RESUME& ClientOpcode::get<CMD_XFER_RESUME>() {
    auto p = ClientOpcode::get_if<version7::CMD_XFER_RESUME>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version7::CMD_XFER_CANCEL* ClientOpcode::get_if<CMD_XFER_CANCEL>() {
    if (opcode == Opcode::CMD_XFER_CANCEL) {
        return &CMD_XFER_CANCEL;
    }
    return nullptr;
}
template <>
version7::CMD_XFER_CANCEL& ClientOpcode::get<CMD_XFER_CANCEL>() {
    auto p = ClientOpcode::get_if<version7::CMD_XFER_CANCEL>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}


WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write_opcode(const ClientOpcode& opcode) {
    if (opcode.opcode == ClientOpcode::Opcode::CMD_AUTH_LOGON_PROOF) {
        return opcode.CMD_AUTH_LOGON_PROOF.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_AUTH_RECONNECT_PROOF) {
        return opcode.CMD_AUTH_RECONNECT_PROOF.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_REALM_LIST) {
        return opcode.CMD_REALM_LIST.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_XFER_ACCEPT) {
        return opcode.CMD_XFER_ACCEPT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_XFER_RESUME) {
        return opcode.CMD_XFER_RESUME.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_XFER_CANCEL) {
        return opcode.CMD_XFER_CANCEL.write();;
    }

    return {}; /* unreachable */
}

WOW_LOGIN_MESSAGES_CPP_EXPORT ClientOpcode read_client_opcode(Reader& reader) {
    const uint8_t opcode = reader.read_u8();

    ClientOpcode op;

    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_AUTH_LOGON_PROOF)) {
        return ClientOpcode(::wow_login_messages::version5::CMD_AUTH_LOGON_PROOF_Client_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_AUTH_RECONNECT_PROOF)) {
        return ClientOpcode(::wow_login_messages::version2::CMD_AUTH_RECONNECT_PROOF_Client_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_REALM_LIST)) {
        return ClientOpcode(::wow_login_messages::version7::CMD_REALM_LIST_Client{});
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_XFER_ACCEPT)) {
        return ClientOpcode(::wow_login_messages::version7::CMD_XFER_ACCEPT{});
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_XFER_RESUME)) {
        return ClientOpcode(::wow_login_messages::version2::CMD_XFER_RESUME_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_XFER_CANCEL)) {
        return ClientOpcode(::wow_login_messages::version7::CMD_XFER_CANCEL{});
    }

    return op;
}
template <>
version7::CMD_AUTH_LOGON_CHALLENGE_Server* ServerOpcode::get_if<CMD_AUTH_LOGON_CHALLENGE_Server>() {
    if (opcode == Opcode::CMD_AUTH_LOGON_CHALLENGE) {
        return &CMD_AUTH_LOGON_CHALLENGE;
    }
    return nullptr;
}
template <>
version7::CMD_AUTH_LOGON_CHALLENGE_Server& ServerOpcode::get<CMD_AUTH_LOGON_CHALLENGE_Server>() {
    auto p = ServerOpcode::get_if<version7::CMD_AUTH_LOGON_CHALLENGE_Server>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version7::CMD_AUTH_LOGON_PROOF_Server* ServerOpcode::get_if<CMD_AUTH_LOGON_PROOF_Server>() {
    if (opcode == Opcode::CMD_AUTH_LOGON_PROOF) {
        return &CMD_AUTH_LOGON_PROOF;
    }
    return nullptr;
}
template <>
version7::CMD_AUTH_LOGON_PROOF_Server& ServerOpcode::get<CMD_AUTH_LOGON_PROOF_Server>() {
    auto p = ServerOpcode::get_if<version7::CMD_AUTH_LOGON_PROOF_Server>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version7::CMD_AUTH_RECONNECT_CHALLENGE_Server* ServerOpcode::get_if<CMD_AUTH_RECONNECT_CHALLENGE_Server>() {
    if (opcode == Opcode::CMD_AUTH_RECONNECT_CHALLENGE) {
        return &CMD_AUTH_RECONNECT_CHALLENGE;
    }
    return nullptr;
}
template <>
version7::CMD_AUTH_RECONNECT_CHALLENGE_Server& ServerOpcode::get<CMD_AUTH_RECONNECT_CHALLENGE_Server>() {
    auto p = ServerOpcode::get_if<version7::CMD_AUTH_RECONNECT_CHALLENGE_Server>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version7::CMD_AUTH_RECONNECT_PROOF_Server* ServerOpcode::get_if<CMD_AUTH_RECONNECT_PROOF_Server>() {
    if (opcode == Opcode::CMD_AUTH_RECONNECT_PROOF) {
        return &CMD_AUTH_RECONNECT_PROOF;
    }
    return nullptr;
}
template <>
version7::CMD_AUTH_RECONNECT_PROOF_Server& ServerOpcode::get<CMD_AUTH_RECONNECT_PROOF_Server>() {
    auto p = ServerOpcode::get_if<version7::CMD_AUTH_RECONNECT_PROOF_Server>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version7::CMD_REALM_LIST_Server* ServerOpcode::get_if<CMD_REALM_LIST_Server>() {
    if (opcode == Opcode::CMD_REALM_LIST) {
        return &CMD_REALM_LIST;
    }
    return nullptr;
}
template <>
version7::CMD_REALM_LIST_Server& ServerOpcode::get<CMD_REALM_LIST_Server>() {
    auto p = ServerOpcode::get_if<version7::CMD_REALM_LIST_Server>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version7::CMD_XFER_INITIATE* ServerOpcode::get_if<CMD_XFER_INITIATE>() {
    if (opcode == Opcode::CMD_XFER_INITIATE) {
        return &CMD_XFER_INITIATE;
    }
    return nullptr;
}
template <>
version7::CMD_XFER_INITIATE& ServerOpcode::get<CMD_XFER_INITIATE>() {
    auto p = ServerOpcode::get_if<version7::CMD_XFER_INITIATE>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version7::CMD_XFER_DATA* ServerOpcode::get_if<CMD_XFER_DATA>() {
    if (opcode == Opcode::CMD_XFER_DATA) {
        return &CMD_XFER_DATA;
    }
    return nullptr;
}
template <>
version7::CMD_XFER_DATA& ServerOpcode::get<CMD_XFER_DATA>() {
    auto p = ServerOpcode::get_if<version7::CMD_XFER_DATA>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}


WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write_opcode(const ServerOpcode& opcode) {
    if (opcode.opcode == ServerOpcode::Opcode::CMD_AUTH_LOGON_CHALLENGE) {
        return opcode.CMD_AUTH_LOGON_CHALLENGE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_AUTH_LOGON_PROOF) {
        return opcode.CMD_AUTH_LOGON_PROOF.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_AUTH_RECONNECT_CHALLENGE) {
        return opcode.CMD_AUTH_RECONNECT_CHALLENGE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_AUTH_RECONNECT_PROOF) {
        return opcode.CMD_AUTH_RECONNECT_PROOF.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_REALM_LIST) {
        return opcode.CMD_REALM_LIST.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_XFER_INITIATE) {
        return opcode.CMD_XFER_INITIATE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_XFER_DATA) {
        return opcode.CMD_XFER_DATA.write();;
    }

    return {}; /* unreachable */
}

WOW_LOGIN_MESSAGES_CPP_EXPORT ServerOpcode read_server_opcode(Reader& reader) {
    const uint8_t opcode = reader.read_u8();

    ServerOpcode op;

    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_AUTH_LOGON_CHALLENGE)) {
        return ServerOpcode(::wow_login_messages::version5::CMD_AUTH_LOGON_CHALLENGE_Server_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_AUTH_LOGON_PROOF)) {
        return ServerOpcode(::wow_login_messages::version5::CMD_AUTH_LOGON_PROOF_Server_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_AUTH_RECONNECT_CHALLENGE)) {
        return ServerOpcode(::wow_login_messages::version2::CMD_AUTH_RECONNECT_CHALLENGE_Server_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_AUTH_RECONNECT_PROOF)) {
        return ServerOpcode(::wow_login_messages::version5::CMD_AUTH_RECONNECT_PROOF_Server_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_REALM_LIST)) {
        return ServerOpcode(::wow_login_messages::version6::CMD_REALM_LIST_Server_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_XFER_INITIATE)) {
        return ServerOpcode(::wow_login_messages::version2::CMD_XFER_INITIATE_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_XFER_DATA)) {
        return ServerOpcode(::wow_login_messages::version2::CMD_XFER_DATA_read(reader));
    }

    return op;
}
} // namespace version7
} // namespace wow_login_messages
#include "wow_login_messages_cpp/version8.hpp"

namespace wow_login_messages {
namespace version8 {
static size_t Realm_size(const Realm& obj) {
    size_t _size = 12 + obj.name.size() + obj.address.size();

    if ((obj.flag & REALM_FLAG_SPECIFY_BUILD) != 0) {
        _size += 5;
    }

    return _size;
}

Realm Realm_read(Reader& reader) {
    Realm obj{};

    obj.realm_type = static_cast<RealmType>(reader.read_u8());

    obj.locked = reader.read_bool8();

    obj.flag = static_cast<RealmFlag>(reader.read_u8());

    obj.name = reader.read_cstring();

    obj.address = reader.read_cstring();

    obj.population = reader.read_float();

    obj.number_of_characters_on_realm = reader.read_u8();

    obj.category = static_cast<RealmCategory>(reader.read_u8());

    obj.realm_id = reader.read_u8();

    if ((obj.flag & REALM_FLAG_SPECIFY_BUILD) != 0) {
        obj.version = ::wow_login_messages::all::Version_read(reader);

    }
    return obj;
}

static void Realm_write(Writer& writer, const Realm& obj) {
    writer.write_u8(static_cast<uint8_t>(obj.realm_type));

    writer.write_bool8(obj.locked);

    writer.write_u8(static_cast<uint8_t>(obj.flag));

    writer.write_cstring(obj.name);

    writer.write_cstring(obj.address);

    writer.write_float(obj.population);

    writer.write_u8(obj.number_of_characters_on_realm);

    writer.write_u8(static_cast<uint8_t>(obj.category));

    writer.write_u8(obj.realm_id);

    if ((obj.flag & REALM_FLAG_SPECIFY_BUILD) != 0) {
        Version_write(writer, obj.version);

    }
}

static size_t CMD_AUTH_LOGON_CHALLENGE_Server_size(const CMD_AUTH_LOGON_CHALLENGE_Server& obj) {
    size_t _size = 2;

    if (obj.result == LoginResult::SUCCESS) {
        _size += 83 + 1 * obj.generator.size() + 1 * obj.large_safe_prime.size();

        if ((obj.security_flag & SECURITY_FLAG_PIN) != 0) {
            _size += 20;
        }

        if ((obj.security_flag & SECURITY_FLAG_MATRIX_CARD) != 0) {
            _size += 12;
        }

        if ((obj.security_flag & SECURITY_FLAG_AUTHENTICATOR) != 0) {
            _size += 1;
        }

    }

    return _size;
}

CMD_AUTH_LOGON_CHALLENGE_Server CMD_AUTH_LOGON_CHALLENGE_Server_read(Reader& reader) {
    CMD_AUTH_LOGON_CHALLENGE_Server obj{};

    (void)reader.read_u8();

    obj.result = static_cast<LoginResult>(reader.read_u8());

    if (obj.result == LoginResult::SUCCESS) {
        for (auto i = 0; i < 32; ++i) {
            obj.server_public_key[i] = reader.read_u8();
        }

        obj.generator_length = reader.read_u8();

        for (uint8_t i = 0; i < obj.generator_length; ++i) {
            obj.generator.push_back(reader.read_u8());
        }

        obj.large_safe_prime_length = reader.read_u8();

        for (uint8_t i = 0; i < obj.large_safe_prime_length; ++i) {
            obj.large_safe_prime.push_back(reader.read_u8());
        }

        for (auto i = 0; i < 32; ++i) {
            obj.salt[i] = reader.read_u8();
        }

        for (auto i = 0; i < 16; ++i) {
            obj.crc_salt[i] = reader.read_u8();
        }

        obj.security_flag = static_cast<SecurityFlag>(reader.read_u8());

        if ((obj.security_flag & SECURITY_FLAG_PIN) != 0) {
            obj.pin_grid_seed = reader.read_u32();

            for (auto i = 0; i < 16; ++i) {
                obj.pin_salt[i] = reader.read_u8();
            }

        }
        if ((obj.security_flag & SECURITY_FLAG_MATRIX_CARD) != 0) {
            obj.width = reader.read_u8();

            obj.height = reader.read_u8();

            obj.digit_count = reader.read_u8();

            obj.challenge_count = reader.read_u8();

            obj.seed = reader.read_u64();

        }
        if ((obj.security_flag & SECURITY_FLAG_AUTHENTICATOR) != 0) {
            obj.required = reader.read_u8();

        }
    }
    return obj;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_AUTH_LOGON_CHALLENGE_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMD_AUTH_LOGON_CHALLENGE_Server_size(obj));

    writer.write_u8(0x00); /* opcode */

    writer.write_u8(0);

    writer.write_u8(static_cast<uint8_t>(obj.result));

    if (obj.result == LoginResult::SUCCESS) {
        for (const auto& v : obj.server_public_key) {
            writer.write_u8(v);
        }

        writer.write_u8(obj.generator_length);

        for (const auto& v : obj.generator) {
            writer.write_u8(v);
        }

        writer.write_u8(obj.large_safe_prime_length);

        for (const auto& v : obj.large_safe_prime) {
            writer.write_u8(v);
        }

        for (const auto& v : obj.salt) {
            writer.write_u8(v);
        }

        for (const auto& v : obj.crc_salt) {
            writer.write_u8(v);
        }

        writer.write_u8(static_cast<uint8_t>(obj.security_flag));

        if ((obj.security_flag & SECURITY_FLAG_PIN) != 0) {
            writer.write_u32(obj.pin_grid_seed);

            for (const auto& v : obj.pin_salt) {
                writer.write_u8(v);
            }

        }
        if ((obj.security_flag & SECURITY_FLAG_MATRIX_CARD) != 0) {
            writer.write_u8(obj.width);

            writer.write_u8(obj.height);

            writer.write_u8(obj.digit_count);

            writer.write_u8(obj.challenge_count);

            writer.write_u64(obj.seed);

        }
        if ((obj.security_flag & SECURITY_FLAG_AUTHENTICATOR) != 0) {
            writer.write_u8(obj.required);

        }
    }
    return writer.m_buf;
}

static size_t CMD_AUTH_LOGON_PROOF_Client_size(const CMD_AUTH_LOGON_PROOF_Client& obj) {
    size_t _size = 74 + 30 * obj.telemetry_keys.size();

    if ((obj.security_flag & SECURITY_FLAG_PIN) != 0) {
        _size += 36;
    }

    if ((obj.security_flag & SECURITY_FLAG_MATRIX_CARD) != 0) {
        _size += 20;
    }

    if ((obj.security_flag & SECURITY_FLAG_AUTHENTICATOR) != 0) {
        _size += 1 + obj.authenticator.size();
    }

    return _size;
}

CMD_AUTH_LOGON_PROOF_Client CMD_AUTH_LOGON_PROOF_Client_read(Reader& reader) {
    CMD_AUTH_LOGON_PROOF_Client obj{};

    for (auto i = 0; i < 32; ++i) {
        obj.client_public_key[i] = reader.read_u8();
    }

    for (auto i = 0; i < 20; ++i) {
        obj.client_proof[i] = reader.read_u8();
    }

    for (auto i = 0; i < 20; ++i) {
        obj.crc_hash[i] = reader.read_u8();
    }

    obj.number_of_telemetry_keys = reader.read_u8();

    for (uint8_t i = 0; i < obj.number_of_telemetry_keys; ++i) {
        obj.telemetry_keys.push_back(::wow_login_messages::version2::TelemetryKey_read(reader));
    }

    obj.security_flag = static_cast<SecurityFlag>(reader.read_u8());

    if ((obj.security_flag & SECURITY_FLAG_PIN) != 0) {
        for (auto i = 0; i < 16; ++i) {
            obj.pin_salt[i] = reader.read_u8();
        }

        for (auto i = 0; i < 20; ++i) {
            obj.pin_hash[i] = reader.read_u8();
        }

    }
    if ((obj.security_flag & SECURITY_FLAG_MATRIX_CARD) != 0) {
        for (auto i = 0; i < 20; ++i) {
            obj.matrix_card_proof[i] = reader.read_u8();
        }

    }
    if ((obj.security_flag & SECURITY_FLAG_AUTHENTICATOR) != 0) {
        obj.authenticator = reader.read_string();

    }
    return obj;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_AUTH_LOGON_PROOF_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMD_AUTH_LOGON_PROOF_Client_size(obj));

    writer.write_u8(0x01); /* opcode */

    for (const auto& v : obj.client_public_key) {
        writer.write_u8(v);
    }

    for (const auto& v : obj.client_proof) {
        writer.write_u8(v);
    }

    for (const auto& v : obj.crc_hash) {
        writer.write_u8(v);
    }

    writer.write_u8(obj.number_of_telemetry_keys);

    for (const auto& v : obj.telemetry_keys) {
        TelemetryKey_write(writer, v);
    }

    writer.write_u8(static_cast<uint8_t>(obj.security_flag));

    if ((obj.security_flag & SECURITY_FLAG_PIN) != 0) {
        for (const auto& v : obj.pin_salt) {
            writer.write_u8(v);
        }

        for (const auto& v : obj.pin_hash) {
            writer.write_u8(v);
        }

    }
    if ((obj.security_flag & SECURITY_FLAG_MATRIX_CARD) != 0) {
        for (const auto& v : obj.matrix_card_proof) {
            writer.write_u8(v);
        }

    }
    if ((obj.security_flag & SECURITY_FLAG_AUTHENTICATOR) != 0) {
        writer.write_string(obj.authenticator);

    }
    return writer.m_buf;
}

static size_t CMD_AUTH_LOGON_PROOF_Server_size(const CMD_AUTH_LOGON_PROOF_Server& obj) {
    size_t _size = 1;

    if (obj.result == LoginResult::SUCCESS) {
        _size += 30;
    }
    else if (obj.result == LoginResult::FAIL_UNKNOWN0|| obj.result == LoginResult::FAIL_UNKNOWN1|| obj.result == LoginResult::FAIL_BANNED|| obj.result == LoginResult::FAIL_UNKNOWN_ACCOUNT|| obj.result == LoginResult::FAIL_INCORRECT_PASSWORD|| obj.result == LoginResult::FAIL_ALREADY_ONLINE|| obj.result == LoginResult::FAIL_NO_TIME|| obj.result == LoginResult::FAIL_DB_BUSY|| obj.result == LoginResult::FAIL_VERSION_INVALID|| obj.result == LoginResult::LOGIN_DOWNLOAD_FILE|| obj.result == LoginResult::FAIL_INVALID_SERVER|| obj.result == LoginResult::FAIL_SUSPENDED|| obj.result == LoginResult::FAIL_NO_ACCESS|| obj.result == LoginResult::SUCCESS_SURVEY|| obj.result == LoginResult::FAIL_PARENTALCONTROL|| obj.result == LoginResult::FAIL_LOCKED_ENFORCED) {
        _size += 2;
    }

    return _size;
}

CMD_AUTH_LOGON_PROOF_Server CMD_AUTH_LOGON_PROOF_Server_read(Reader& reader) {
    CMD_AUTH_LOGON_PROOF_Server obj{};

    obj.result = static_cast<LoginResult>(reader.read_u8());

    if (obj.result == LoginResult::SUCCESS) {
        for (auto i = 0; i < 20; ++i) {
            obj.server_proof[i] = reader.read_u8();
        }

        obj.account_flag = static_cast<AccountFlag>(reader.read_u32());

        obj.hardware_survey_id = reader.read_u32();

        obj.unknown = reader.read_u16();

    }
    else if (obj.result == LoginResult::FAIL_UNKNOWN0|| obj.result == LoginResult::FAIL_UNKNOWN1|| obj.result == LoginResult::FAIL_BANNED|| obj.result == LoginResult::FAIL_UNKNOWN_ACCOUNT|| obj.result == LoginResult::FAIL_INCORRECT_PASSWORD|| obj.result == LoginResult::FAIL_ALREADY_ONLINE|| obj.result == LoginResult::FAIL_NO_TIME|| obj.result == LoginResult::FAIL_DB_BUSY|| obj.result == LoginResult::FAIL_VERSION_INVALID|| obj.result == LoginResult::LOGIN_DOWNLOAD_FILE|| obj.result == LoginResult::FAIL_INVALID_SERVER|| obj.result == LoginResult::FAIL_SUSPENDED|| obj.result == LoginResult::FAIL_NO_ACCESS|| obj.result == LoginResult::SUCCESS_SURVEY|| obj.result == LoginResult::FAIL_PARENTALCONTROL|| obj.result == LoginResult::FAIL_LOCKED_ENFORCED) {
        (void)reader.read_u16();

    }
    return obj;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_AUTH_LOGON_PROOF_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMD_AUTH_LOGON_PROOF_Server_size(obj));

    writer.write_u8(0x01); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.result));

    if (obj.result == LoginResult::SUCCESS) {
        for (const auto& v : obj.server_proof) {
            writer.write_u8(v);
        }

        writer.write_u32(static_cast<uint32_t>(obj.account_flag));

        writer.write_u32(obj.hardware_survey_id);

        writer.write_u16(obj.unknown);

    }
    else if (obj.result == LoginResult::FAIL_UNKNOWN0|| obj.result == LoginResult::FAIL_UNKNOWN1|| obj.result == LoginResult::FAIL_BANNED|| obj.result == LoginResult::FAIL_UNKNOWN_ACCOUNT|| obj.result == LoginResult::FAIL_INCORRECT_PASSWORD|| obj.result == LoginResult::FAIL_ALREADY_ONLINE|| obj.result == LoginResult::FAIL_NO_TIME|| obj.result == LoginResult::FAIL_DB_BUSY|| obj.result == LoginResult::FAIL_VERSION_INVALID|| obj.result == LoginResult::LOGIN_DOWNLOAD_FILE|| obj.result == LoginResult::FAIL_INVALID_SERVER|| obj.result == LoginResult::FAIL_SUSPENDED|| obj.result == LoginResult::FAIL_NO_ACCESS|| obj.result == LoginResult::SUCCESS_SURVEY|| obj.result == LoginResult::FAIL_PARENTALCONTROL|| obj.result == LoginResult::FAIL_LOCKED_ENFORCED) {
        writer.write_u16(0);

    }
    return writer.m_buf;
}

static size_t CMD_AUTH_RECONNECT_CHALLENGE_Server_size(const CMD_AUTH_RECONNECT_CHALLENGE_Server& obj) {
    size_t _size = 1;

    if (obj.result == LoginResult::SUCCESS) {
        _size += 32;
    }

    return _size;
}

CMD_AUTH_RECONNECT_CHALLENGE_Server CMD_AUTH_RECONNECT_CHALLENGE_Server_read(Reader& reader) {
    CMD_AUTH_RECONNECT_CHALLENGE_Server obj{};

    obj.result = static_cast<LoginResult>(reader.read_u8());

    if (obj.result == LoginResult::SUCCESS) {
        for (auto i = 0; i < 16; ++i) {
            obj.challenge_data[i] = reader.read_u8();
        }

        for (auto i = 0; i < 16; ++i) {
            obj.checksum_salt[i] = reader.read_u8();
        }

    }
    return obj;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_AUTH_RECONNECT_CHALLENGE_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMD_AUTH_RECONNECT_CHALLENGE_Server_size(obj));

    writer.write_u8(0x02); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.result));

    if (obj.result == LoginResult::SUCCESS) {
        for (const auto& v : obj.challenge_data) {
            writer.write_u8(v);
        }

        for (const auto& v : obj.checksum_salt) {
            writer.write_u8(v);
        }

    }
    return writer.m_buf;
}

CMD_AUTH_RECONNECT_PROOF_Server CMD_AUTH_RECONNECT_PROOF_Server_read(Reader& reader) {
    CMD_AUTH_RECONNECT_PROOF_Server obj{};

    obj.result = static_cast<LoginResult>(reader.read_u8());

    (void)reader.read_u16();

    return obj;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_AUTH_RECONNECT_PROOF_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0003);

    writer.write_u8(0x03); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.result));

    writer.write_u16(0);

    return writer.m_buf;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_REALM_LIST_Client::write() const {
    auto writer = Writer(0x0004);

    writer.write_u8(0x10); /* opcode */

    writer.write_u32(0);

    return writer.m_buf;
}

static size_t CMD_REALM_LIST_Server_size(const CMD_REALM_LIST_Server& obj) {
    size_t _size = 8;

    for(const auto& v : obj.realms) {
        _size += Realm_size(v);
    }

    return _size;
}

CMD_REALM_LIST_Server CMD_REALM_LIST_Server_read(Reader& reader) {
    CMD_REALM_LIST_Server obj{};

    (void)reader.read_u16();

    (void)reader.read_u32();

    obj.number_of_realms = reader.read_u16();

    for (uint16_t i = 0; i < obj.number_of_realms; ++i) {
        obj.realms.push_back(::wow_login_messages::version8::Realm_read(reader));
    }

    (void)reader.read_u16();

    return obj;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_REALM_LIST_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMD_REALM_LIST_Server_size(obj));

    writer.write_u8(0x10); /* opcode */

    writer.write_u16(static_cast<uint16_t>(CMD_REALM_LIST_Server_size(obj)));

    writer.write_u32(0);

    writer.write_u16(obj.number_of_realms);

    for (const auto& v : obj.realms) {
        Realm_write(writer, v);
    }

    writer.write_u16(0);

    return writer.m_buf;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_XFER_ACCEPT::write() const {
    auto writer = Writer(0x0000);

    writer.write_u8(0x32); /* opcode */

    return writer.m_buf;
}

WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> CMD_XFER_CANCEL::write() const {
    auto writer = Writer(0x0000);

    writer.write_u8(0x34); /* opcode */

    return writer.m_buf;
}

template <>
version8::CMD_AUTH_LOGON_PROOF_Client* ClientOpcode::get_if<CMD_AUTH_LOGON_PROOF_Client>() {
    if (opcode == Opcode::CMD_AUTH_LOGON_PROOF) {
        return &CMD_AUTH_LOGON_PROOF;
    }
    return nullptr;
}
template <>
version8::CMD_AUTH_LOGON_PROOF_Client& ClientOpcode::get<CMD_AUTH_LOGON_PROOF_Client>() {
    auto p = ClientOpcode::get_if<version8::CMD_AUTH_LOGON_PROOF_Client>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version8::CMD_AUTH_RECONNECT_PROOF_Client* ClientOpcode::get_if<CMD_AUTH_RECONNECT_PROOF_Client>() {
    if (opcode == Opcode::CMD_AUTH_RECONNECT_PROOF) {
        return &CMD_AUTH_RECONNECT_PROOF;
    }
    return nullptr;
}
template <>
version8::CMD_AUTH_RECONNECT_PROOF_Client& ClientOpcode::get<CMD_AUTH_RECONNECT_PROOF_Client>() {
    auto p = ClientOpcode::get_if<version8::CMD_AUTH_RECONNECT_PROOF_Client>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version8::CMD_REALM_LIST_Client* ClientOpcode::get_if<CMD_REALM_LIST_Client>() {
    if (opcode == Opcode::CMD_REALM_LIST) {
        return &CMD_REALM_LIST;
    }
    return nullptr;
}
template <>
version8::CMD_REALM_LIST_Client& ClientOpcode::get<CMD_REALM_LIST_Client>() {
    auto p = ClientOpcode::get_if<version8::CMD_REALM_LIST_Client>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version8::CMD_XFER_ACCEPT* ClientOpcode::get_if<CMD_XFER_ACCEPT>() {
    if (opcode == Opcode::CMD_XFER_ACCEPT) {
        return &CMD_XFER_ACCEPT;
    }
    return nullptr;
}
template <>
version8::CMD_XFER_ACCEPT& ClientOpcode::get<CMD_XFER_ACCEPT>() {
    auto p = ClientOpcode::get_if<version8::CMD_XFER_ACCEPT>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version8::CMD_XFER_RESUME* ClientOpcode::get_if<CMD_XFER_RESUME>() {
    if (opcode == Opcode::CMD_XFER_RESUME) {
        return &CMD_XFER_RESUME;
    }
    return nullptr;
}
template <>
version8::CMD_XFER_RESUME& ClientOpcode::get<CMD_XFER_RESUME>() {
    auto p = ClientOpcode::get_if<version8::CMD_XFER_RESUME>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version8::CMD_XFER_CANCEL* ClientOpcode::get_if<CMD_XFER_CANCEL>() {
    if (opcode == Opcode::CMD_XFER_CANCEL) {
        return &CMD_XFER_CANCEL;
    }
    return nullptr;
}
template <>
version8::CMD_XFER_CANCEL& ClientOpcode::get<CMD_XFER_CANCEL>() {
    auto p = ClientOpcode::get_if<version8::CMD_XFER_CANCEL>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}


WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write_opcode(const ClientOpcode& opcode) {
    if (opcode.opcode == ClientOpcode::Opcode::CMD_AUTH_LOGON_PROOF) {
        return opcode.CMD_AUTH_LOGON_PROOF.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_AUTH_RECONNECT_PROOF) {
        return opcode.CMD_AUTH_RECONNECT_PROOF.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_REALM_LIST) {
        return opcode.CMD_REALM_LIST.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_XFER_ACCEPT) {
        return opcode.CMD_XFER_ACCEPT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_XFER_RESUME) {
        return opcode.CMD_XFER_RESUME.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMD_XFER_CANCEL) {
        return opcode.CMD_XFER_CANCEL.write();;
    }

    return {}; /* unreachable */
}

WOW_LOGIN_MESSAGES_CPP_EXPORT ClientOpcode read_client_opcode(Reader& reader) {
    const uint8_t opcode = reader.read_u8();

    ClientOpcode op;

    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_AUTH_LOGON_PROOF)) {
        return ClientOpcode(::wow_login_messages::version8::CMD_AUTH_LOGON_PROOF_Client_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_AUTH_RECONNECT_PROOF)) {
        return ClientOpcode(::wow_login_messages::version2::CMD_AUTH_RECONNECT_PROOF_Client_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_REALM_LIST)) {
        return ClientOpcode(::wow_login_messages::version8::CMD_REALM_LIST_Client{});
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_XFER_ACCEPT)) {
        return ClientOpcode(::wow_login_messages::version8::CMD_XFER_ACCEPT{});
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_XFER_RESUME)) {
        return ClientOpcode(::wow_login_messages::version2::CMD_XFER_RESUME_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ClientOpcode::Opcode::CMD_XFER_CANCEL)) {
        return ClientOpcode(::wow_login_messages::version8::CMD_XFER_CANCEL{});
    }

    return op;
}
template <>
version8::CMD_AUTH_LOGON_CHALLENGE_Server* ServerOpcode::get_if<CMD_AUTH_LOGON_CHALLENGE_Server>() {
    if (opcode == Opcode::CMD_AUTH_LOGON_CHALLENGE) {
        return &CMD_AUTH_LOGON_CHALLENGE;
    }
    return nullptr;
}
template <>
version8::CMD_AUTH_LOGON_CHALLENGE_Server& ServerOpcode::get<CMD_AUTH_LOGON_CHALLENGE_Server>() {
    auto p = ServerOpcode::get_if<version8::CMD_AUTH_LOGON_CHALLENGE_Server>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version8::CMD_AUTH_LOGON_PROOF_Server* ServerOpcode::get_if<CMD_AUTH_LOGON_PROOF_Server>() {
    if (opcode == Opcode::CMD_AUTH_LOGON_PROOF) {
        return &CMD_AUTH_LOGON_PROOF;
    }
    return nullptr;
}
template <>
version8::CMD_AUTH_LOGON_PROOF_Server& ServerOpcode::get<CMD_AUTH_LOGON_PROOF_Server>() {
    auto p = ServerOpcode::get_if<version8::CMD_AUTH_LOGON_PROOF_Server>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version8::CMD_AUTH_RECONNECT_CHALLENGE_Server* ServerOpcode::get_if<CMD_AUTH_RECONNECT_CHALLENGE_Server>() {
    if (opcode == Opcode::CMD_AUTH_RECONNECT_CHALLENGE) {
        return &CMD_AUTH_RECONNECT_CHALLENGE;
    }
    return nullptr;
}
template <>
version8::CMD_AUTH_RECONNECT_CHALLENGE_Server& ServerOpcode::get<CMD_AUTH_RECONNECT_CHALLENGE_Server>() {
    auto p = ServerOpcode::get_if<version8::CMD_AUTH_RECONNECT_CHALLENGE_Server>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version8::CMD_AUTH_RECONNECT_PROOF_Server* ServerOpcode::get_if<CMD_AUTH_RECONNECT_PROOF_Server>() {
    if (opcode == Opcode::CMD_AUTH_RECONNECT_PROOF) {
        return &CMD_AUTH_RECONNECT_PROOF;
    }
    return nullptr;
}
template <>
version8::CMD_AUTH_RECONNECT_PROOF_Server& ServerOpcode::get<CMD_AUTH_RECONNECT_PROOF_Server>() {
    auto p = ServerOpcode::get_if<version8::CMD_AUTH_RECONNECT_PROOF_Server>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version8::CMD_REALM_LIST_Server* ServerOpcode::get_if<CMD_REALM_LIST_Server>() {
    if (opcode == Opcode::CMD_REALM_LIST) {
        return &CMD_REALM_LIST;
    }
    return nullptr;
}
template <>
version8::CMD_REALM_LIST_Server& ServerOpcode::get<CMD_REALM_LIST_Server>() {
    auto p = ServerOpcode::get_if<version8::CMD_REALM_LIST_Server>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version8::CMD_XFER_INITIATE* ServerOpcode::get_if<CMD_XFER_INITIATE>() {
    if (opcode == Opcode::CMD_XFER_INITIATE) {
        return &CMD_XFER_INITIATE;
    }
    return nullptr;
}
template <>
version8::CMD_XFER_INITIATE& ServerOpcode::get<CMD_XFER_INITIATE>() {
    auto p = ServerOpcode::get_if<version8::CMD_XFER_INITIATE>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}

template <>
version8::CMD_XFER_DATA* ServerOpcode::get_if<CMD_XFER_DATA>() {
    if (opcode == Opcode::CMD_XFER_DATA) {
        return &CMD_XFER_DATA;
    }
    return nullptr;
}
template <>
version8::CMD_XFER_DATA& ServerOpcode::get<CMD_XFER_DATA>() {
    auto p = ServerOpcode::get_if<version8::CMD_XFER_DATA>();
    if (p) {
        return *p;
    }
    throw bad_opcode_access{};
}


WOW_LOGIN_MESSAGES_CPP_EXPORT std::vector<unsigned char> write_opcode(const ServerOpcode& opcode) {
    if (opcode.opcode == ServerOpcode::Opcode::CMD_AUTH_LOGON_CHALLENGE) {
        return opcode.CMD_AUTH_LOGON_CHALLENGE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_AUTH_LOGON_PROOF) {
        return opcode.CMD_AUTH_LOGON_PROOF.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_AUTH_RECONNECT_CHALLENGE) {
        return opcode.CMD_AUTH_RECONNECT_CHALLENGE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_AUTH_RECONNECT_PROOF) {
        return opcode.CMD_AUTH_RECONNECT_PROOF.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_REALM_LIST) {
        return opcode.CMD_REALM_LIST.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_XFER_INITIATE) {
        return opcode.CMD_XFER_INITIATE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::CMD_XFER_DATA) {
        return opcode.CMD_XFER_DATA.write();;
    }

    return {}; /* unreachable */
}

WOW_LOGIN_MESSAGES_CPP_EXPORT ServerOpcode read_server_opcode(Reader& reader) {
    const uint8_t opcode = reader.read_u8();

    ServerOpcode op;

    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_AUTH_LOGON_CHALLENGE)) {
        return ServerOpcode(::wow_login_messages::version8::CMD_AUTH_LOGON_CHALLENGE_Server_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_AUTH_LOGON_PROOF)) {
        return ServerOpcode(::wow_login_messages::version8::CMD_AUTH_LOGON_PROOF_Server_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_AUTH_RECONNECT_CHALLENGE)) {
        return ServerOpcode(::wow_login_messages::version8::CMD_AUTH_RECONNECT_CHALLENGE_Server_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_AUTH_RECONNECT_PROOF)) {
        return ServerOpcode(::wow_login_messages::version8::CMD_AUTH_RECONNECT_PROOF_Server_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_REALM_LIST)) {
        return ServerOpcode(::wow_login_messages::version8::CMD_REALM_LIST_Server_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_XFER_INITIATE)) {
        return ServerOpcode(::wow_login_messages::version2::CMD_XFER_INITIATE_read(reader));
    }
    if (opcode == static_cast<uint8_t>(ServerOpcode::Opcode::CMD_XFER_DATA)) {
        return ServerOpcode(::wow_login_messages::version2::CMD_XFER_DATA_read(reader));
    }

    return op;
}
} // namespace version8
} // namespace wow_login_messages
