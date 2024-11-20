#include "util.hpp"

#include "wow_login_messages_cpp/all.hpp"

namespace wow_login_messages::all {
Version Version_read(Reader& reader) {
    Version obj;

    obj.major = reader.read_u8();

    obj.minor = reader.read_u8();

    obj.patch = reader.read_u8();

    obj.build = reader.read_u16();

    return obj;
}

void Version_write(Writer& writer, const Version& obj) {
    writer.write_u8(obj.major);

    writer.write_u8(obj.minor);

    writer.write_u8(obj.patch);

    writer.write_u16(obj.build);

}

static size_t CMD_AUTH_LOGON_CHALLENGE_Client_size(const CMD_AUTH_LOGON_CHALLENGE_Client& obj) {
    return 30 + obj.account_name.size();
}

CMD_AUTH_LOGON_CHALLENGE_Client CMD_AUTH_LOGON_CHALLENGE_Client_read(Reader& reader) {
    CMD_AUTH_LOGON_CHALLENGE_Client obj;

    obj.protocol_version = static_cast<ProtocolVersion>(reader.read_u8());

    (void)reader.read_u16();

    (void)reader.read_u32();

    obj.version = Version_read(reader);

    obj.platform = static_cast<Platform>(reader.read_u32());

    obj.os = static_cast<Os>(reader.read_u32());

    obj.locale = static_cast<Locale>(reader.read_u32());

    obj.utc_timezone_offset = reader.read_i32();

    obj.client_ip_address = reader.read_u32();

    obj.account_name = reader.read_string();

    return obj;
}

std::vector<unsigned char> CMD_AUTH_LOGON_CHALLENGE_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(128);

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
    CMD_AUTH_RECONNECT_CHALLENGE_Client obj;

    obj.protocol_version = static_cast<ProtocolVersion>(reader.read_u8());

    (void)reader.read_u16();

    (void)reader.read_u32();

    obj.version = Version_read(reader);

    obj.platform = static_cast<Platform>(reader.read_u32());

    obj.os = static_cast<Os>(reader.read_u32());

    obj.locale = static_cast<Locale>(reader.read_u32());

    obj.utc_timezone_offset = reader.read_i32();

    obj.client_ip_address = reader.read_u32();

    obj.account_name = reader.read_string();

    return obj;
}

std::vector<unsigned char> CMD_AUTH_RECONNECT_CHALLENGE_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(128);

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

std::vector<unsigned char> write_opcode(const ClientOpcode& opcode) {
    if (const auto p = std::get_if<CMD_AUTH_LOGON_CHALLENGE_Client>(&opcode)) {
        return p->write();
    }
    if (const auto p = std::get_if<CMD_AUTH_RECONNECT_CHALLENGE_Client>(&opcode)) {
        return p->write();
    }

    return {}; /* unreachable */
}

std::optional<ClientOpcode> read_client_opcode(Reader& reader) {
    enum Opcode {
        CMD_AUTH_LOGON_CHALLENGE = 0x0000,
        CMD_AUTH_RECONNECT_CHALLENGE = 0x0002,
    };

    uint8_t opcode = reader.read_u8();

    if (opcode == Opcode::CMD_AUTH_LOGON_CHALLENGE) {
        return CMD_AUTH_LOGON_CHALLENGE_Client_read(reader);
    }
    if (opcode == Opcode::CMD_AUTH_RECONNECT_CHALLENGE) {
        return CMD_AUTH_RECONNECT_CHALLENGE_Client_read(reader);
    }

    return {};
}
} /* namespace wow_login_messages::all */
#include "wow_login_messages_cpp/version2.hpp"

namespace wow_login_messages::version2 {
static size_t Realm_size(const Realm& obj) {
    return 14 + obj.name.size() + obj.address.size();
}

Realm Realm_read(Reader& reader) {
    Realm obj;

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

void Realm_write(Writer& writer, const Realm& obj) {
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
    TelemetryKey obj;

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

void TelemetryKey_write(Writer& writer, const TelemetryKey& obj) {
    writer.write_u16(obj.unknown1);

    writer.write_u32(obj.unknown2);

    for (const auto& v : obj.unknown3) {
        writer.write_u8(v);
    }

    for (const auto& v : obj.cd_key_proof) {
        writer.write_u8(v);
    }

}

CMD_AUTH_LOGON_CHALLENGE_Server CMD_AUTH_LOGON_CHALLENGE_Server_read(Reader& reader) {
    CMD_AUTH_LOGON_CHALLENGE_Server obj;

    (void)reader.read_u8();

    obj.result = static_cast<LoginResult>(reader.read_u8());

    if (obj.result == LoginResult::SUCCESS) {
        for (auto i = 0; i < 32; ++i) {
            obj.server_public_key[i] = reader.read_u8();
        }

        obj.generator_length = reader.read_u8();

        for (auto i = 0; i < obj.generator_length; ++i) {
            obj.generator.push_back(reader.read_u8());
        }

        obj.large_safe_prime_length = reader.read_u8();

        for (auto i = 0; i < obj.large_safe_prime_length; ++i) {
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

std::vector<unsigned char> CMD_AUTH_LOGON_CHALLENGE_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(128);

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

CMD_AUTH_LOGON_PROOF_Client CMD_AUTH_LOGON_PROOF_Client_read(Reader& reader) {
    CMD_AUTH_LOGON_PROOF_Client obj;

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

    for (auto i = 0; i < obj.number_of_telemetry_keys; ++i) {
        obj.telemetry_keys.push_back(TelemetryKey_read(reader));
    }

    return obj;
}

std::vector<unsigned char> CMD_AUTH_LOGON_PROOF_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(128);

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

CMD_AUTH_LOGON_PROOF_Server CMD_AUTH_LOGON_PROOF_Server_read(Reader& reader) {
    CMD_AUTH_LOGON_PROOF_Server obj;

    obj.result = static_cast<LoginResult>(reader.read_u8());

    if (obj.result == LoginResult::SUCCESS) {
        for (auto i = 0; i < 20; ++i) {
            obj.server_proof[i] = reader.read_u8();
        }

        obj.hardware_survey_id = reader.read_u32();

    }
    return obj;
}

std::vector<unsigned char> CMD_AUTH_LOGON_PROOF_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(128);

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

CMD_AUTH_RECONNECT_CHALLENGE_Server CMD_AUTH_RECONNECT_CHALLENGE_Server_read(Reader& reader) {
    CMD_AUTH_RECONNECT_CHALLENGE_Server obj;

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

std::vector<unsigned char> CMD_AUTH_RECONNECT_CHALLENGE_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(128);

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
    CMD_AUTH_RECONNECT_PROOF_Server obj;

    obj.result = static_cast<LoginResult>(reader.read_u8());

    return obj;
}

std::vector<unsigned char> CMD_AUTH_RECONNECT_PROOF_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0001);

    writer.write_u8(0x03); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.result));

    return writer.m_buf;
}

CMD_AUTH_RECONNECT_PROOF_Client CMD_AUTH_RECONNECT_PROOF_Client_read(Reader& reader) {
    CMD_AUTH_RECONNECT_PROOF_Client obj;

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

std::vector<unsigned char> CMD_AUTH_RECONNECT_PROOF_Client::write() const {
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
    CMD_REALM_LIST_Server obj;

    (void)reader.read_u16();

    (void)reader.read_u32();

    obj.number_of_realms = reader.read_u8();

    for (auto i = 0; i < obj.number_of_realms; ++i) {
        obj.realms.push_back(Realm_read(reader));
    }

    (void)reader.read_u16();

    return obj;
}

std::vector<unsigned char> CMD_REALM_LIST_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(128);

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

std::vector<unsigned char> CMD_REALM_LIST_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u8(0x10); /* opcode */

    writer.write_u32(0);

    return writer.m_buf;
}

CMD_XFER_INITIATE CMD_XFER_INITIATE_read(Reader& reader) {
    CMD_XFER_INITIATE obj;

    obj.filename = reader.read_string();

    obj.file_size = reader.read_u64();

    for (auto i = 0; i < 16; ++i) {
        obj.file_md5[i] = reader.read_u8();
    }

    return obj;
}

std::vector<unsigned char> CMD_XFER_INITIATE::write() const {
    const auto& obj = *this;
    auto writer = Writer(128);

    writer.write_u8(0x30); /* opcode */

    writer.write_string(obj.filename);

    writer.write_u64(obj.file_size);

    for (const auto& v : obj.file_md5) {
        writer.write_u8(v);
    }

    return writer.m_buf;
}

CMD_XFER_DATA CMD_XFER_DATA_read(Reader& reader) {
    CMD_XFER_DATA obj;

    obj.size = reader.read_u16();

    for (auto i = 0; i < obj.size; ++i) {
        obj.data.push_back(reader.read_u8());
    }

    return obj;
}

std::vector<unsigned char> CMD_XFER_DATA::write() const {
    const auto& obj = *this;
    auto writer = Writer(128);

    writer.write_u8(0x31); /* opcode */

    writer.write_u16(obj.size);

    for (const auto& v : obj.data) {
        writer.write_u8(v);
    }

    return writer.m_buf;
}

std::vector<unsigned char> CMD_XFER_ACCEPT::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0000);

    writer.write_u8(0x32); /* opcode */

    return writer.m_buf;
}

CMD_XFER_RESUME CMD_XFER_RESUME_read(Reader& reader) {
    CMD_XFER_RESUME obj;

    obj.offset = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMD_XFER_RESUME::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u8(0x33); /* opcode */

    writer.write_u64(obj.offset);

    return writer.m_buf;
}

std::vector<unsigned char> CMD_XFER_CANCEL::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0000);

    writer.write_u8(0x34); /* opcode */

    return writer.m_buf;
}

std::vector<unsigned char> write_opcode(const ClientOpcode& opcode) {
    if (const auto p = std::get_if<CMD_AUTH_LOGON_PROOF_Client>(&opcode)) {
        return p->write();
    }
    if (const auto p = std::get_if<CMD_AUTH_RECONNECT_PROOF_Client>(&opcode)) {
        return p->write();
    }
    if (const auto p = std::get_if<CMD_XFER_RESUME>(&opcode)) {
        return p->write();
    }

    return {}; /* unreachable */
}

std::optional<ClientOpcode> read_client_opcode(Reader& reader) {
    enum Opcode {
        CMD_AUTH_LOGON_CHALLENGE = 0x0000,
        CMD_AUTH_LOGON_PROOF = 0x0001,
        CMD_AUTH_RECONNECT_CHALLENGE = 0x0002,
        CMD_AUTH_RECONNECT_PROOF = 0x0003,
        CMD_REALM_LIST = 0x0010,
        CMD_XFER_INITIATE = 0x0030,
        CMD_XFER_DATA = 0x0031,
        CMD_XFER_ACCEPT = 0x0032,
        CMD_XFER_RESUME = 0x0033,
        CMD_XFER_CANCEL = 0x0034,
    };

    uint8_t opcode = reader.read_u8();

    if (opcode == Opcode::CMD_AUTH_LOGON_PROOF) {
        return CMD_AUTH_LOGON_PROOF_Client_read(reader);
    }
    if (opcode == Opcode::CMD_AUTH_RECONNECT_PROOF) {
        return CMD_AUTH_RECONNECT_PROOF_Client_read(reader);
    }
    if (opcode == Opcode::CMD_REALM_LIST) {
        return wow_login_messages::version2::CMD_REALM_LIST_Client{};
    }
    if (opcode == Opcode::CMD_XFER_ACCEPT) {
        return wow_login_messages::version2::CMD_XFER_ACCEPT{};
    }
    if (opcode == Opcode::CMD_XFER_RESUME) {
        return CMD_XFER_RESUME_read(reader);
    }
    if (opcode == Opcode::CMD_XFER_CANCEL) {
        return wow_login_messages::version2::CMD_XFER_CANCEL{};
    }

    return {};
}
std::vector<unsigned char> write_opcode(const ServerOpcode& opcode) {
    if (const auto p = std::get_if<CMD_AUTH_LOGON_CHALLENGE_Server>(&opcode)) {
        return p->write();
    }
    if (const auto p = std::get_if<CMD_AUTH_LOGON_PROOF_Server>(&opcode)) {
        return p->write();
    }
    if (const auto p = std::get_if<CMD_AUTH_RECONNECT_CHALLENGE_Server>(&opcode)) {
        return p->write();
    }
    if (const auto p = std::get_if<CMD_AUTH_RECONNECT_PROOF_Server>(&opcode)) {
        return p->write();
    }
    if (const auto p = std::get_if<CMD_REALM_LIST_Server>(&opcode)) {
        return p->write();
    }
    if (const auto p = std::get_if<CMD_XFER_INITIATE>(&opcode)) {
        return p->write();
    }
    if (const auto p = std::get_if<CMD_XFER_DATA>(&opcode)) {
        return p->write();
    }

    return {}; /* unreachable */
}

std::optional<ServerOpcode> read_server_opcode(Reader& reader) {
    enum Opcode {
        CMD_AUTH_LOGON_CHALLENGE = 0x0000,
        CMD_AUTH_LOGON_PROOF = 0x0001,
        CMD_AUTH_RECONNECT_CHALLENGE = 0x0002,
        CMD_AUTH_RECONNECT_PROOF = 0x0003,
        CMD_REALM_LIST = 0x0010,
        CMD_XFER_INITIATE = 0x0030,
        CMD_XFER_DATA = 0x0031,
        CMD_XFER_ACCEPT = 0x0032,
        CMD_XFER_RESUME = 0x0033,
        CMD_XFER_CANCEL = 0x0034,
    };

    uint8_t opcode = reader.read_u8();

    if (opcode == Opcode::CMD_AUTH_LOGON_CHALLENGE) {
        return CMD_AUTH_LOGON_CHALLENGE_Server_read(reader);
    }
    if (opcode == Opcode::CMD_AUTH_LOGON_PROOF) {
        return CMD_AUTH_LOGON_PROOF_Server_read(reader);
    }
    if (opcode == Opcode::CMD_AUTH_RECONNECT_CHALLENGE) {
        return CMD_AUTH_RECONNECT_CHALLENGE_Server_read(reader);
    }
    if (opcode == Opcode::CMD_AUTH_RECONNECT_PROOF) {
        return CMD_AUTH_RECONNECT_PROOF_Server_read(reader);
    }
    if (opcode == Opcode::CMD_REALM_LIST) {
        return CMD_REALM_LIST_Server_read(reader);
    }
    if (opcode == Opcode::CMD_XFER_INITIATE) {
        return CMD_XFER_INITIATE_read(reader);
    }
    if (opcode == Opcode::CMD_XFER_DATA) {
        return CMD_XFER_DATA_read(reader);
    }

    return {};
}
} /* namespace wow_login_messages::version2 */
