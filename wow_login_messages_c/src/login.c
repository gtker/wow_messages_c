#include "util.h"

#include "wow_login_messages/all.h"

static WowLoginResult all_Version_read(WowLoginReader* reader, all_Version* object) {
    READ_U8(object->major);

    READ_U8(object->minor);

    READ_U8(object->patch);

    READ_U16(object->build);

    return WLM_RESULT_SUCCESS;
}

static WowLoginResult all_Version_write(WowLoginWriter* writer, const all_Version* object) {
    WRITE_U8(object->major);

    WRITE_U8(object->minor);

    WRITE_U8(object->patch);

    WRITE_U16(object->build);


    return WLM_RESULT_SUCCESS;
}

static size_t all_CMD_AUTH_LOGON_CHALLENGE_Client_size(const all_CMD_AUTH_LOGON_CHALLENGE_Client* object) {
    return 30 + STRING_SIZE(object->account_name);
}

static WowLoginResult all_CMD_AUTH_LOGON_CHALLENGE_Client_read(WowLoginReader* reader, all_CMD_AUTH_LOGON_CHALLENGE_Client* object) {
    object->protocol_version = 0;
    READ_U8(object->protocol_version);

    SKIP_FORWARD_BYTES(2);

    SKIP_FORWARD_BYTES(4);

    WLM_CHECK_RETURN_CODE(all_Version_read(reader, &object->version));

    object->platform = 0;
    READ_U32(object->platform);

    object->os = 0;
    READ_U32(object->os);

    object->locale = 0;
    READ_U32(object->locale);

    READ_I32(object->utc_timezone_offset);

    READ_U32(object->client_ip_address);

    READ_STRING(object->account_name);

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult all_CMD_AUTH_LOGON_CHALLENGE_Client_write(WowLoginWriter* writer, const all_CMD_AUTH_LOGON_CHALLENGE_Client* object) {
    WRITE_U8(0x00); /* opcode */

    WRITE_U8(object->protocol_version);

    WRITE_U16((uint16_t)all_CMD_AUTH_LOGON_CHALLENGE_Client_size(object));

    WRITE_U32(5730135);

    WLM_CHECK_RETURN_CODE(all_Version_write(writer, &object->version));

    WRITE_U32(object->platform);

    WRITE_U32(object->os);

    WRITE_U32(object->locale);

    WRITE_I32(object->utc_timezone_offset);

    WRITE_U32(object->client_ip_address);

    WRITE_STRING(object->account_name);


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void all_CMD_AUTH_LOGON_CHALLENGE_Client_free(all_CMD_AUTH_LOGON_CHALLENGE_Client* object) {
    FREE_STRING(object->account_name);

}

static size_t all_CMD_AUTH_RECONNECT_CHALLENGE_Client_size(const all_CMD_AUTH_RECONNECT_CHALLENGE_Client* object) {
    return 30 + STRING_SIZE(object->account_name);
}

static WowLoginResult all_CMD_AUTH_RECONNECT_CHALLENGE_Client_read(WowLoginReader* reader, all_CMD_AUTH_RECONNECT_CHALLENGE_Client* object) {
    object->protocol_version = 0;
    READ_U8(object->protocol_version);

    SKIP_FORWARD_BYTES(2);

    SKIP_FORWARD_BYTES(4);

    WLM_CHECK_RETURN_CODE(all_Version_read(reader, &object->version));

    object->platform = 0;
    READ_U32(object->platform);

    object->os = 0;
    READ_U32(object->os);

    object->locale = 0;
    READ_U32(object->locale);

    READ_I32(object->utc_timezone_offset);

    READ_U32(object->client_ip_address);

    READ_STRING(object->account_name);

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult all_CMD_AUTH_RECONNECT_CHALLENGE_Client_write(WowLoginWriter* writer, const all_CMD_AUTH_RECONNECT_CHALLENGE_Client* object) {
    WRITE_U8(0x02); /* opcode */

    WRITE_U8(object->protocol_version);

    WRITE_U16((uint16_t)all_CMD_AUTH_RECONNECT_CHALLENGE_Client_size(object));

    WRITE_U32(5730135);

    WLM_CHECK_RETURN_CODE(all_Version_write(writer, &object->version));

    WRITE_U32(object->platform);

    WRITE_U32(object->os);

    WRITE_U32(object->locale);

    WRITE_I32(object->utc_timezone_offset);

    WRITE_U32(object->client_ip_address);

    WRITE_STRING(object->account_name);


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void all_CMD_AUTH_RECONNECT_CHALLENGE_Client_free(all_CMD_AUTH_RECONNECT_CHALLENGE_Client* object) {
    FREE_STRING(object->account_name);

}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult all_client_opcode_read(WowLoginReader* reader, AllClientOpcodeContainer* opcodes) {
    opcodes->opcode = 0;
    READ_U8(opcodes->opcode);

    switch (opcodes->opcode) {
        case CMD_AUTH_LOGON_CHALLENGE:
            WLM_CHECK_RETURN_CODE(all_CMD_AUTH_LOGON_CHALLENGE_Client_read(reader, &opcodes->body.CMD_AUTH_LOGON_CHALLENGE_Client));
            break;
        case CMD_AUTH_RECONNECT_CHALLENGE:
            WLM_CHECK_RETURN_CODE(all_CMD_AUTH_RECONNECT_CHALLENGE_Client_read(reader, &opcodes->body.CMD_AUTH_RECONNECT_CHALLENGE_Client));
            break;
        default:
            break;
    }

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void all_client_opcode_free(AllClientOpcodeContainer* opcodes) {
    switch (opcodes->opcode) {
        case CMD_AUTH_LOGON_CHALLENGE:
            all_CMD_AUTH_LOGON_CHALLENGE_Client_free(&opcodes->body.CMD_AUTH_LOGON_CHALLENGE_Client);
            break;
        case CMD_AUTH_RECONNECT_CHALLENGE:
            all_CMD_AUTH_RECONNECT_CHALLENGE_Client_free(&opcodes->body.CMD_AUTH_RECONNECT_CHALLENGE_Client);
            break;
        default:
            break;
    }
}

#include "wow_login_messages/version2.h"

static size_t version2_Realm_size(const version2_Realm* object) {
    return 14 + STRING_SIZE(object->name) + STRING_SIZE(object->address);
}

static WowLoginResult version2_Realm_read(WowLoginReader* reader, version2_Realm* object) {
    object->realm_type = 0;
    READ_U32(object->realm_type);

    object->flag = 0;
    READ_U8(object->flag);

    READ_CSTRING(object->name);

    READ_CSTRING(object->address);

    READ_FLOAT(object->population);

    READ_U8(object->number_of_characters_on_realm);

    object->category = 0;
    READ_U8(object->category);

    READ_U8(object->realm_id);

    return WLM_RESULT_SUCCESS;
}

static WowLoginResult version2_Realm_write(WowLoginWriter* writer, const version2_Realm* object) {
    WRITE_U32(object->realm_type);

    WRITE_U8(object->flag);

    WRITE_CSTRING(object->name);

    WRITE_CSTRING(object->address);

    WRITE_FLOAT(object->population);

    WRITE_U8(object->number_of_characters_on_realm);

    WRITE_U8(object->category);

    WRITE_U8(object->realm_id);


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version2_Realm_free(version2_Realm* object) {
    FREE_STRING(object->name);

    FREE_STRING(object->address);

}

static WowLoginResult version2_TelemetryKey_read(WowLoginReader* reader, version2_TelemetryKey* object) {
    READ_U16(object->unknown1);

    READ_U32(object->unknown2);

    READ_ARRAY_ALLOCATE(object->unknown3, 4, sizeof(uint8_t));
    READ_ARRAY(object->unknown3, 4, READ_U8((*object->unknown3)[i]));

    READ_ARRAY_ALLOCATE(object->cd_key_proof, 20, sizeof(uint8_t));
    READ_ARRAY(object->cd_key_proof, 20, READ_U8((*object->cd_key_proof)[i]));

    return WLM_RESULT_SUCCESS;
}

static WowLoginResult version2_TelemetryKey_write(WowLoginWriter* writer, const version2_TelemetryKey* object) {
    WRITE_U16(object->unknown1);

    WRITE_U32(object->unknown2);

    WRITE_ARRAY(object->unknown3, 4, WRITE_U8((*object->unknown3)[i]));

    WRITE_ARRAY(object->cd_key_proof, 20, WRITE_U8((*object->cd_key_proof)[i]));


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version2_TelemetryKey_free(version2_TelemetryKey* object) {
    free(object->unknown3);
    object->unknown3 = NULL;
    free(object->cd_key_proof);
    object->cd_key_proof = NULL;
}

static WowLoginResult version2_CMD_AUTH_LOGON_CHALLENGE_Server_read(WowLoginReader* reader, version2_CMD_AUTH_LOGON_CHALLENGE_Server* object) {
    SKIP_FORWARD_BYTES(1);

    object->result = 0;
    READ_U8(object->result);

    if (object->result == VERSION2_LOGIN_RESULT_SUCCESS) {
        READ_ARRAY_ALLOCATE(object->server_public_key, 32, sizeof(uint8_t));
        READ_ARRAY(object->server_public_key, 32, READ_U8((*object->server_public_key)[i]));

        READ_U8(object->generator_length);

        READ_ARRAY_ALLOCATE(object->generator, object->generator_length, sizeof(uint8_t));
        READ_ARRAY(object->generator, object->generator_length, READ_U8(object->generator[i]));

        READ_U8(object->large_safe_prime_length);

        READ_ARRAY_ALLOCATE(object->large_safe_prime, object->large_safe_prime_length, sizeof(uint8_t));
        READ_ARRAY(object->large_safe_prime, object->large_safe_prime_length, READ_U8(object->large_safe_prime[i]));

        READ_ARRAY_ALLOCATE(object->salt, 32, sizeof(uint8_t));
        READ_ARRAY(object->salt, 32, READ_U8((*object->salt)[i]));

        READ_ARRAY_ALLOCATE(object->crc_salt, 16, sizeof(uint8_t));
        READ_ARRAY(object->crc_salt, 16, READ_U8((*object->crc_salt)[i]));

    }
    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version2_CMD_AUTH_LOGON_CHALLENGE_Server_write(WowLoginWriter* writer, const version2_CMD_AUTH_LOGON_CHALLENGE_Server* object) {
    WRITE_U8(0x00); /* opcode */

    WRITE_U8(0);

    WRITE_U8(object->result);

    if (object->result == VERSION2_LOGIN_RESULT_SUCCESS) {
        WRITE_ARRAY(object->server_public_key, 32, WRITE_U8((*object->server_public_key)[i]));

        WRITE_U8(object->generator_length);

        WRITE_ARRAY(object->generator, object->generator_length, WRITE_U8(object->generator[i]));

        WRITE_U8(object->large_safe_prime_length);

        WRITE_ARRAY(object->large_safe_prime, object->large_safe_prime_length, WRITE_U8(object->large_safe_prime[i]));

        WRITE_ARRAY(object->salt, 32, WRITE_U8((*object->salt)[i]));

        WRITE_ARRAY(object->crc_salt, 16, WRITE_U8((*object->crc_salt)[i]));

    }

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version2_CMD_AUTH_LOGON_CHALLENGE_Server_free(version2_CMD_AUTH_LOGON_CHALLENGE_Server* object) {
    if (object->result == VERSION2_LOGIN_RESULT_SUCCESS) {
        free(object->server_public_key);
        object->server_public_key = NULL;
        free(object->generator);
        object->generator = NULL;
        free(object->large_safe_prime);
        object->large_safe_prime = NULL;
        free(object->salt);
        object->salt = NULL;
        free(object->crc_salt);
        object->crc_salt = NULL;
    }
}

static WowLoginResult version2_CMD_AUTH_LOGON_PROOF_Client_read(WowLoginReader* reader, version2_CMD_AUTH_LOGON_PROOF_Client* object) {
    READ_ARRAY_ALLOCATE(object->client_public_key, 32, sizeof(uint8_t));
    READ_ARRAY(object->client_public_key, 32, READ_U8((*object->client_public_key)[i]));

    READ_ARRAY_ALLOCATE(object->client_proof, 20, sizeof(uint8_t));
    READ_ARRAY(object->client_proof, 20, READ_U8((*object->client_proof)[i]));

    READ_ARRAY_ALLOCATE(object->crc_hash, 20, sizeof(uint8_t));
    READ_ARRAY(object->crc_hash, 20, READ_U8((*object->crc_hash)[i]));

    READ_U8(object->number_of_telemetry_keys);

    READ_ARRAY_ALLOCATE(object->telemetry_keys, object->number_of_telemetry_keys, sizeof(version2_TelemetryKey));
    READ_ARRAY(object->telemetry_keys, object->number_of_telemetry_keys, WLM_CHECK_RETURN_CODE(version2_TelemetryKey_read(reader, &object->telemetry_keys[i])));

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version2_CMD_AUTH_LOGON_PROOF_Client_write(WowLoginWriter* writer, const version2_CMD_AUTH_LOGON_PROOF_Client* object) {
    WRITE_U8(0x01); /* opcode */

    WRITE_ARRAY(object->client_public_key, 32, WRITE_U8((*object->client_public_key)[i]));

    WRITE_ARRAY(object->client_proof, 20, WRITE_U8((*object->client_proof)[i]));

    WRITE_ARRAY(object->crc_hash, 20, WRITE_U8((*object->crc_hash)[i]));

    WRITE_U8(object->number_of_telemetry_keys);

    WRITE_ARRAY(object->telemetry_keys, object->number_of_telemetry_keys, WLM_CHECK_RETURN_CODE(version2_TelemetryKey_write(writer, &object->telemetry_keys[i])));


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version2_CMD_AUTH_LOGON_PROOF_Client_free(version2_CMD_AUTH_LOGON_PROOF_Client* object) {
    size_t i;

    free(object->client_public_key);
    object->client_public_key = NULL;
    free(object->client_proof);
    object->client_proof = NULL;
    free(object->crc_hash);
    object->crc_hash = NULL;
    for (i = 0; i < object->number_of_telemetry_keys; ++i) {
        version2_TelemetryKey_free(&((object->telemetry_keys)[i]));
    }
    free(object->telemetry_keys);
    object->telemetry_keys = NULL;
}

static WowLoginResult version2_CMD_AUTH_LOGON_PROOF_Server_read(WowLoginReader* reader, version2_CMD_AUTH_LOGON_PROOF_Server* object) {
    object->result = 0;
    READ_U8(object->result);

    if (object->result == VERSION2_LOGIN_RESULT_SUCCESS) {
        READ_ARRAY_ALLOCATE(object->server_proof, 20, sizeof(uint8_t));
        READ_ARRAY(object->server_proof, 20, READ_U8((*object->server_proof)[i]));

        READ_U32(object->hardware_survey_id);

    }
    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version2_CMD_AUTH_LOGON_PROOF_Server_write(WowLoginWriter* writer, const version2_CMD_AUTH_LOGON_PROOF_Server* object) {
    WRITE_U8(0x01); /* opcode */

    WRITE_U8(object->result);

    if (object->result == VERSION2_LOGIN_RESULT_SUCCESS) {
        WRITE_ARRAY(object->server_proof, 20, WRITE_U8((*object->server_proof)[i]));

        WRITE_U32(object->hardware_survey_id);

    }

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version2_CMD_AUTH_LOGON_PROOF_Server_free(version2_CMD_AUTH_LOGON_PROOF_Server* object) {
    if (object->result == VERSION2_LOGIN_RESULT_SUCCESS) {
        free(object->server_proof);
        object->server_proof = NULL;
    }
}

static WowLoginResult version2_CMD_AUTH_RECONNECT_CHALLENGE_Server_read(WowLoginReader* reader, version2_CMD_AUTH_RECONNECT_CHALLENGE_Server* object) {
    object->result = 0;
    READ_U8(object->result);

    if (object->result == VERSION2_LOGIN_RESULT_SUCCESS) {
        READ_ARRAY_ALLOCATE(object->challenge_data, 16, sizeof(uint8_t));
        READ_ARRAY(object->challenge_data, 16, READ_U8((*object->challenge_data)[i]));

        READ_ARRAY_ALLOCATE(object->checksum_salt, 16, sizeof(uint8_t));
        READ_ARRAY(object->checksum_salt, 16, READ_U8((*object->checksum_salt)[i]));

    }
    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version2_CMD_AUTH_RECONNECT_CHALLENGE_Server_write(WowLoginWriter* writer, const version2_CMD_AUTH_RECONNECT_CHALLENGE_Server* object) {
    WRITE_U8(0x02); /* opcode */

    WRITE_U8(object->result);

    if (object->result == VERSION2_LOGIN_RESULT_SUCCESS) {
        WRITE_ARRAY(object->challenge_data, 16, WRITE_U8((*object->challenge_data)[i]));

        WRITE_ARRAY(object->checksum_salt, 16, WRITE_U8((*object->checksum_salt)[i]));

    }

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version2_CMD_AUTH_RECONNECT_CHALLENGE_Server_free(version2_CMD_AUTH_RECONNECT_CHALLENGE_Server* object) {
    if (object->result == VERSION2_LOGIN_RESULT_SUCCESS) {
        free(object->challenge_data);
        object->challenge_data = NULL;
        free(object->checksum_salt);
        object->checksum_salt = NULL;
    }
}

static WowLoginResult version2_CMD_AUTH_RECONNECT_PROOF_Server_read(WowLoginReader* reader, version2_CMD_AUTH_RECONNECT_PROOF_Server* object) {
    object->result = 0;
    READ_U8(object->result);

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version2_CMD_AUTH_RECONNECT_PROOF_Server_write(WowLoginWriter* writer, const version2_CMD_AUTH_RECONNECT_PROOF_Server* object) {
    WRITE_U8(0x03); /* opcode */

    WRITE_U8(object->result);


    return WLM_RESULT_SUCCESS;
}

static WowLoginResult version2_CMD_AUTH_RECONNECT_PROOF_Client_read(WowLoginReader* reader, version2_CMD_AUTH_RECONNECT_PROOF_Client* object) {
    READ_ARRAY_ALLOCATE(object->proof_data, 16, sizeof(uint8_t));
    READ_ARRAY(object->proof_data, 16, READ_U8((*object->proof_data)[i]));

    READ_ARRAY_ALLOCATE(object->client_proof, 20, sizeof(uint8_t));
    READ_ARRAY(object->client_proof, 20, READ_U8((*object->client_proof)[i]));

    READ_ARRAY_ALLOCATE(object->client_checksum, 20, sizeof(uint8_t));
    READ_ARRAY(object->client_checksum, 20, READ_U8((*object->client_checksum)[i]));

    SKIP_FORWARD_BYTES(1);

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version2_CMD_AUTH_RECONNECT_PROOF_Client_write(WowLoginWriter* writer, const version2_CMD_AUTH_RECONNECT_PROOF_Client* object) {
    WRITE_U8(0x03); /* opcode */

    WRITE_ARRAY(object->proof_data, 16, WRITE_U8((*object->proof_data)[i]));

    WRITE_ARRAY(object->client_proof, 20, WRITE_U8((*object->client_proof)[i]));

    WRITE_ARRAY(object->client_checksum, 20, WRITE_U8((*object->client_checksum)[i]));

    WRITE_U8(0);


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version2_CMD_AUTH_RECONNECT_PROOF_Client_free(version2_CMD_AUTH_RECONNECT_PROOF_Client* object) {
    free(object->proof_data);
    object->proof_data = NULL;
    free(object->client_proof);
    object->client_proof = NULL;
    free(object->client_checksum);
    object->client_checksum = NULL;
}

static size_t version2_CMD_REALM_LIST_Server_size(const version2_CMD_REALM_LIST_Server* object) {
    size_t _size = 7;

    /* C89 scope to allow variable declarations */ {
        int i;
        for(i = 0; i < (int)object->number_of_realms; ++i) {
            _size += version2_Realm_size(&object->realms[i]);
        }
    }

    return _size;
}

static WowLoginResult version2_CMD_REALM_LIST_Server_read(WowLoginReader* reader, version2_CMD_REALM_LIST_Server* object) {
    SKIP_FORWARD_BYTES(2);

    SKIP_FORWARD_BYTES(4);

    READ_U8(object->number_of_realms);

    READ_ARRAY_ALLOCATE(object->realms, object->number_of_realms, sizeof(version2_Realm));
    READ_ARRAY(object->realms, object->number_of_realms, WLM_CHECK_RETURN_CODE(version2_Realm_read(reader, &object->realms[i])));

    SKIP_FORWARD_BYTES(2);

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version2_CMD_REALM_LIST_Server_write(WowLoginWriter* writer, const version2_CMD_REALM_LIST_Server* object) {
    WRITE_U8(0x10); /* opcode */

    WRITE_U16((uint16_t)version2_CMD_REALM_LIST_Server_size(object));

    WRITE_U32(0);

    WRITE_U8(object->number_of_realms);

    WRITE_ARRAY(object->realms, object->number_of_realms, WLM_CHECK_RETURN_CODE(version2_Realm_write(writer, &object->realms[i])));

    WRITE_U16(0);


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version2_CMD_REALM_LIST_Server_free(version2_CMD_REALM_LIST_Server* object) {
    size_t i;

    for (i = 0; i < object->number_of_realms; ++i) {
        version2_Realm_free(&((object->realms)[i]));
    }
    free(object->realms);
    object->realms = NULL;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version2_CMD_REALM_LIST_Client_write(WowLoginWriter* writer) {
    WRITE_U8(0x10); /* opcode */

    WRITE_U32(0);


    return WLM_RESULT_SUCCESS;
}

static WowLoginResult version2_CMD_XFER_INITIATE_read(WowLoginReader* reader, version2_CMD_XFER_INITIATE* object) {
    READ_STRING(object->filename);

    READ_U64(object->file_size);

    READ_ARRAY_ALLOCATE(object->file_md5, 16, sizeof(uint8_t));
    READ_ARRAY(object->file_md5, 16, READ_U8((*object->file_md5)[i]));

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version2_CMD_XFER_INITIATE_write(WowLoginWriter* writer, const version2_CMD_XFER_INITIATE* object) {
    WRITE_U8(0x30); /* opcode */

    WRITE_STRING(object->filename);

    WRITE_U64(object->file_size);

    WRITE_ARRAY(object->file_md5, 16, WRITE_U8((*object->file_md5)[i]));


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version2_CMD_XFER_INITIATE_free(version2_CMD_XFER_INITIATE* object) {
    FREE_STRING(object->filename);

    free(object->file_md5);
    object->file_md5 = NULL;
}

static WowLoginResult version2_CMD_XFER_DATA_read(WowLoginReader* reader, version2_CMD_XFER_DATA* object) {
    READ_U16(object->size);

    READ_ARRAY_ALLOCATE(object->data, object->size, sizeof(uint8_t));
    READ_ARRAY(object->data, object->size, READ_U8(object->data[i]));

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version2_CMD_XFER_DATA_write(WowLoginWriter* writer, const version2_CMD_XFER_DATA* object) {
    WRITE_U8(0x31); /* opcode */

    WRITE_U16(object->size);

    WRITE_ARRAY(object->data, object->size, WRITE_U8(object->data[i]));


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version2_CMD_XFER_DATA_free(version2_CMD_XFER_DATA* object) {
    free(object->data);
    object->data = NULL;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version2_CMD_XFER_ACCEPT_write(WowLoginWriter* writer) {
    WRITE_U8(0x32); /* opcode */


    return WLM_RESULT_SUCCESS;
}

static WowLoginResult version2_CMD_XFER_RESUME_read(WowLoginReader* reader, version2_CMD_XFER_RESUME* object) {
    READ_U64(object->offset);

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version2_CMD_XFER_RESUME_write(WowLoginWriter* writer, const version2_CMD_XFER_RESUME* object) {
    WRITE_U8(0x33); /* opcode */

    WRITE_U64(object->offset);


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version2_CMD_XFER_CANCEL_write(WowLoginWriter* writer) {
    WRITE_U8(0x34); /* opcode */


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version2_client_opcode_read(WowLoginReader* reader, Version2ClientOpcodeContainer* opcodes) {
    opcodes->opcode = 0;
    READ_U8(opcodes->opcode);

    switch (opcodes->opcode) {
        case CMD_AUTH_LOGON_PROOF:
            WLM_CHECK_RETURN_CODE(version2_CMD_AUTH_LOGON_PROOF_Client_read(reader, &opcodes->body.CMD_AUTH_LOGON_PROOF_Client));
            break;
        case CMD_AUTH_RECONNECT_PROOF:
            WLM_CHECK_RETURN_CODE(version2_CMD_AUTH_RECONNECT_PROOF_Client_read(reader, &opcodes->body.CMD_AUTH_RECONNECT_PROOF_Client));
            break;
        case CMD_XFER_RESUME:
            WLM_CHECK_RETURN_CODE(version2_CMD_XFER_RESUME_read(reader, &opcodes->body.CMD_XFER_RESUME));
            break;
        default:
            break;
    }

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version2_client_opcode_free(Version2ClientOpcodeContainer* opcodes) {
    switch (opcodes->opcode) {
        case CMD_AUTH_LOGON_PROOF:
            version2_CMD_AUTH_LOGON_PROOF_Client_free(&opcodes->body.CMD_AUTH_LOGON_PROOF_Client);
            break;
        case CMD_AUTH_RECONNECT_PROOF:
            version2_CMD_AUTH_RECONNECT_PROOF_Client_free(&opcodes->body.CMD_AUTH_RECONNECT_PROOF_Client);
            break;
        default:
            break;
    }
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version2_server_opcode_read(WowLoginReader* reader, Version2ServerOpcodeContainer* opcodes) {
    opcodes->opcode = 0;
    READ_U8(opcodes->opcode);

    switch (opcodes->opcode) {
        case CMD_AUTH_LOGON_CHALLENGE:
            WLM_CHECK_RETURN_CODE(version2_CMD_AUTH_LOGON_CHALLENGE_Server_read(reader, &opcodes->body.CMD_AUTH_LOGON_CHALLENGE_Server));
            break;
        case CMD_AUTH_LOGON_PROOF:
            WLM_CHECK_RETURN_CODE(version2_CMD_AUTH_LOGON_PROOF_Server_read(reader, &opcodes->body.CMD_AUTH_LOGON_PROOF_Server));
            break;
        case CMD_AUTH_RECONNECT_CHALLENGE:
            WLM_CHECK_RETURN_CODE(version2_CMD_AUTH_RECONNECT_CHALLENGE_Server_read(reader, &opcodes->body.CMD_AUTH_RECONNECT_CHALLENGE_Server));
            break;
        case CMD_AUTH_RECONNECT_PROOF:
            WLM_CHECK_RETURN_CODE(version2_CMD_AUTH_RECONNECT_PROOF_Server_read(reader, &opcodes->body.CMD_AUTH_RECONNECT_PROOF_Server));
            break;
        case CMD_REALM_LIST:
            WLM_CHECK_RETURN_CODE(version2_CMD_REALM_LIST_Server_read(reader, &opcodes->body.CMD_REALM_LIST_Server));
            break;
        case CMD_XFER_INITIATE:
            WLM_CHECK_RETURN_CODE(version2_CMD_XFER_INITIATE_read(reader, &opcodes->body.CMD_XFER_INITIATE));
            break;
        case CMD_XFER_DATA:
            WLM_CHECK_RETURN_CODE(version2_CMD_XFER_DATA_read(reader, &opcodes->body.CMD_XFER_DATA));
            break;
        default:
            break;
    }

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version2_server_opcode_free(Version2ServerOpcodeContainer* opcodes) {
    switch (opcodes->opcode) {
        case CMD_AUTH_LOGON_CHALLENGE:
            version2_CMD_AUTH_LOGON_CHALLENGE_Server_free(&opcodes->body.CMD_AUTH_LOGON_CHALLENGE_Server);
            break;
        case CMD_AUTH_LOGON_PROOF:
            version2_CMD_AUTH_LOGON_PROOF_Server_free(&opcodes->body.CMD_AUTH_LOGON_PROOF_Server);
            break;
        case CMD_AUTH_RECONNECT_CHALLENGE:
            version2_CMD_AUTH_RECONNECT_CHALLENGE_Server_free(&opcodes->body.CMD_AUTH_RECONNECT_CHALLENGE_Server);
            break;
        case CMD_REALM_LIST:
            version2_CMD_REALM_LIST_Server_free(&opcodes->body.CMD_REALM_LIST_Server);
            break;
        case CMD_XFER_INITIATE:
            version2_CMD_XFER_INITIATE_free(&opcodes->body.CMD_XFER_INITIATE);
            break;
        case CMD_XFER_DATA:
            version2_CMD_XFER_DATA_free(&opcodes->body.CMD_XFER_DATA);
            break;
        default:
            break;
    }
}

#include "wow_login_messages/version3.h"

static WowLoginResult version3_CMD_AUTH_LOGON_CHALLENGE_Server_read(WowLoginReader* reader, version3_CMD_AUTH_LOGON_CHALLENGE_Server* object) {
    SKIP_FORWARD_BYTES(1);

    object->result = 0;
    READ_U8(object->result);

    if (object->result == VERSION2_LOGIN_RESULT_SUCCESS) {
        READ_ARRAY_ALLOCATE(object->server_public_key, 32, sizeof(uint8_t));
        READ_ARRAY(object->server_public_key, 32, READ_U8((*object->server_public_key)[i]));

        READ_U8(object->generator_length);

        READ_ARRAY_ALLOCATE(object->generator, object->generator_length, sizeof(uint8_t));
        READ_ARRAY(object->generator, object->generator_length, READ_U8(object->generator[i]));

        READ_U8(object->large_safe_prime_length);

        READ_ARRAY_ALLOCATE(object->large_safe_prime, object->large_safe_prime_length, sizeof(uint8_t));
        READ_ARRAY(object->large_safe_prime, object->large_safe_prime_length, READ_U8(object->large_safe_prime[i]));

        READ_ARRAY_ALLOCATE(object->salt, 32, sizeof(uint8_t));
        READ_ARRAY(object->salt, 32, READ_U8((*object->salt)[i]));

        READ_ARRAY_ALLOCATE(object->crc_salt, 16, sizeof(uint8_t));
        READ_ARRAY(object->crc_salt, 16, READ_U8((*object->crc_salt)[i]));

        object->security_flag = 0;
        READ_U8(object->security_flag);

        if (object->security_flag == VERSION3_SECURITY_FLAG_PIN) {
            READ_U32(object->pin_grid_seed);

            READ_ARRAY_ALLOCATE(object->pin_salt, 16, sizeof(uint8_t));
            READ_ARRAY(object->pin_salt, 16, READ_U8((*object->pin_salt)[i]));

        }
    }
    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version3_CMD_AUTH_LOGON_CHALLENGE_Server_write(WowLoginWriter* writer, const version3_CMD_AUTH_LOGON_CHALLENGE_Server* object) {
    WRITE_U8(0x00); /* opcode */

    WRITE_U8(0);

    WRITE_U8(object->result);

    if (object->result == VERSION2_LOGIN_RESULT_SUCCESS) {
        WRITE_ARRAY(object->server_public_key, 32, WRITE_U8((*object->server_public_key)[i]));

        WRITE_U8(object->generator_length);

        WRITE_ARRAY(object->generator, object->generator_length, WRITE_U8(object->generator[i]));

        WRITE_U8(object->large_safe_prime_length);

        WRITE_ARRAY(object->large_safe_prime, object->large_safe_prime_length, WRITE_U8(object->large_safe_prime[i]));

        WRITE_ARRAY(object->salt, 32, WRITE_U8((*object->salt)[i]));

        WRITE_ARRAY(object->crc_salt, 16, WRITE_U8((*object->crc_salt)[i]));

        WRITE_U8(object->security_flag);

        if (object->security_flag == VERSION3_SECURITY_FLAG_PIN) {
            WRITE_U32(object->pin_grid_seed);

            WRITE_ARRAY(object->pin_salt, 16, WRITE_U8((*object->pin_salt)[i]));

        }
    }

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version3_CMD_AUTH_LOGON_CHALLENGE_Server_free(version3_CMD_AUTH_LOGON_CHALLENGE_Server* object) {
    if (object->result == VERSION2_LOGIN_RESULT_SUCCESS) {
        free(object->server_public_key);
        object->server_public_key = NULL;
        free(object->generator);
        object->generator = NULL;
        free(object->large_safe_prime);
        object->large_safe_prime = NULL;
        free(object->salt);
        object->salt = NULL;
        free(object->crc_salt);
        object->crc_salt = NULL;
        if (object->security_flag == VERSION3_SECURITY_FLAG_PIN) {
            free(object->pin_salt);
            object->pin_salt = NULL;
        }
    }
}

static WowLoginResult version3_CMD_AUTH_LOGON_PROOF_Client_read(WowLoginReader* reader, version3_CMD_AUTH_LOGON_PROOF_Client* object) {
    READ_ARRAY_ALLOCATE(object->client_public_key, 32, sizeof(uint8_t));
    READ_ARRAY(object->client_public_key, 32, READ_U8((*object->client_public_key)[i]));

    READ_ARRAY_ALLOCATE(object->client_proof, 20, sizeof(uint8_t));
    READ_ARRAY(object->client_proof, 20, READ_U8((*object->client_proof)[i]));

    READ_ARRAY_ALLOCATE(object->crc_hash, 20, sizeof(uint8_t));
    READ_ARRAY(object->crc_hash, 20, READ_U8((*object->crc_hash)[i]));

    READ_U8(object->number_of_telemetry_keys);

    READ_ARRAY_ALLOCATE(object->telemetry_keys, object->number_of_telemetry_keys, sizeof(version2_TelemetryKey));
    READ_ARRAY(object->telemetry_keys, object->number_of_telemetry_keys, WLM_CHECK_RETURN_CODE(version2_TelemetryKey_read(reader, &object->telemetry_keys[i])));

    object->security_flag = 0;
    READ_U8(object->security_flag);

    if (object->security_flag == VERSION3_SECURITY_FLAG_PIN) {
        READ_ARRAY_ALLOCATE(object->pin_salt, 16, sizeof(uint8_t));
        READ_ARRAY(object->pin_salt, 16, READ_U8((*object->pin_salt)[i]));

        READ_ARRAY_ALLOCATE(object->pin_hash, 20, sizeof(uint8_t));
        READ_ARRAY(object->pin_hash, 20, READ_U8((*object->pin_hash)[i]));

    }
    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version3_CMD_AUTH_LOGON_PROOF_Client_write(WowLoginWriter* writer, const version3_CMD_AUTH_LOGON_PROOF_Client* object) {
    WRITE_U8(0x01); /* opcode */

    WRITE_ARRAY(object->client_public_key, 32, WRITE_U8((*object->client_public_key)[i]));

    WRITE_ARRAY(object->client_proof, 20, WRITE_U8((*object->client_proof)[i]));

    WRITE_ARRAY(object->crc_hash, 20, WRITE_U8((*object->crc_hash)[i]));

    WRITE_U8(object->number_of_telemetry_keys);

    WRITE_ARRAY(object->telemetry_keys, object->number_of_telemetry_keys, WLM_CHECK_RETURN_CODE(version2_TelemetryKey_write(writer, &object->telemetry_keys[i])));

    WRITE_U8(object->security_flag);

    if (object->security_flag == VERSION3_SECURITY_FLAG_PIN) {
        WRITE_ARRAY(object->pin_salt, 16, WRITE_U8((*object->pin_salt)[i]));

        WRITE_ARRAY(object->pin_hash, 20, WRITE_U8((*object->pin_hash)[i]));

    }

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version3_CMD_AUTH_LOGON_PROOF_Client_free(version3_CMD_AUTH_LOGON_PROOF_Client* object) {
    size_t i;

    free(object->client_public_key);
    object->client_public_key = NULL;
    free(object->client_proof);
    object->client_proof = NULL;
    free(object->crc_hash);
    object->crc_hash = NULL;
    for (i = 0; i < object->number_of_telemetry_keys; ++i) {
        version2_TelemetryKey_free(&((object->telemetry_keys)[i]));
    }
    free(object->telemetry_keys);
    object->telemetry_keys = NULL;
    if (object->security_flag == VERSION3_SECURITY_FLAG_PIN) {
        free(object->pin_salt);
        object->pin_salt = NULL;
        free(object->pin_hash);
        object->pin_hash = NULL;
    }
}

static WowLoginResult version3_CMD_SURVEY_RESULT_read(WowLoginReader* reader, version3_CMD_SURVEY_RESULT* object) {
    READ_U32(object->survey_id);

    READ_U8(object->error);

    READ_U16(object->compressed_data_length);

    READ_ARRAY_ALLOCATE(object->data, object->compressed_data_length, sizeof(uint8_t));
    READ_ARRAY(object->data, object->compressed_data_length, READ_U8(object->data[i]));

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version3_CMD_SURVEY_RESULT_write(WowLoginWriter* writer, const version3_CMD_SURVEY_RESULT* object) {
    WRITE_U8(0x04); /* opcode */

    WRITE_U32(object->survey_id);

    WRITE_U8(object->error);

    WRITE_U16(object->compressed_data_length);

    WRITE_ARRAY(object->data, object->compressed_data_length, WRITE_U8(object->data[i]));


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version3_CMD_SURVEY_RESULT_free(version3_CMD_SURVEY_RESULT* object) {
    free(object->data);
    object->data = NULL;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version3_CMD_REALM_LIST_Client_write(WowLoginWriter* writer) {
    WRITE_U8(0x10); /* opcode */

    WRITE_U32(0);


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version3_CMD_XFER_ACCEPT_write(WowLoginWriter* writer) {
    WRITE_U8(0x32); /* opcode */


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version3_CMD_XFER_CANCEL_write(WowLoginWriter* writer) {
    WRITE_U8(0x34); /* opcode */


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version3_client_opcode_read(WowLoginReader* reader, Version3ClientOpcodeContainer* opcodes) {
    opcodes->opcode = 0;
    READ_U8(opcodes->opcode);

    switch (opcodes->opcode) {
        case CMD_AUTH_LOGON_PROOF:
            WLM_CHECK_RETURN_CODE(version3_CMD_AUTH_LOGON_PROOF_Client_read(reader, &opcodes->body.CMD_AUTH_LOGON_PROOF_Client));
            break;
        case CMD_SURVEY_RESULT:
            WLM_CHECK_RETURN_CODE(version3_CMD_SURVEY_RESULT_read(reader, &opcodes->body.CMD_SURVEY_RESULT));
            break;
        case CMD_XFER_RESUME:
            WLM_CHECK_RETURN_CODE(version2_CMD_XFER_RESUME_read(reader, &opcodes->body.CMD_XFER_RESUME));
            break;
        default:
            break;
    }

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version3_client_opcode_free(Version3ClientOpcodeContainer* opcodes) {
    switch (opcodes->opcode) {
        case CMD_AUTH_LOGON_PROOF:
            version3_CMD_AUTH_LOGON_PROOF_Client_free(&opcodes->body.CMD_AUTH_LOGON_PROOF_Client);
            break;
        case CMD_SURVEY_RESULT:
            version3_CMD_SURVEY_RESULT_free(&opcodes->body.CMD_SURVEY_RESULT);
            break;
        default:
            break;
    }
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version3_server_opcode_read(WowLoginReader* reader, Version3ServerOpcodeContainer* opcodes) {
    opcodes->opcode = 0;
    READ_U8(opcodes->opcode);

    switch (opcodes->opcode) {
        case CMD_AUTH_LOGON_CHALLENGE:
            WLM_CHECK_RETURN_CODE(version3_CMD_AUTH_LOGON_CHALLENGE_Server_read(reader, &opcodes->body.CMD_AUTH_LOGON_CHALLENGE_Server));
            break;
        case CMD_AUTH_LOGON_PROOF:
            WLM_CHECK_RETURN_CODE(version2_CMD_AUTH_LOGON_PROOF_Server_read(reader, &opcodes->body.CMD_AUTH_LOGON_PROOF_Server));
            break;
        case CMD_REALM_LIST:
            WLM_CHECK_RETURN_CODE(version2_CMD_REALM_LIST_Server_read(reader, &opcodes->body.CMD_REALM_LIST_Server));
            break;
        case CMD_XFER_INITIATE:
            WLM_CHECK_RETURN_CODE(version2_CMD_XFER_INITIATE_read(reader, &opcodes->body.CMD_XFER_INITIATE));
            break;
        case CMD_XFER_DATA:
            WLM_CHECK_RETURN_CODE(version2_CMD_XFER_DATA_read(reader, &opcodes->body.CMD_XFER_DATA));
            break;
        default:
            break;
    }

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version3_server_opcode_free(Version3ServerOpcodeContainer* opcodes) {
    switch (opcodes->opcode) {
        case CMD_AUTH_LOGON_CHALLENGE:
            version3_CMD_AUTH_LOGON_CHALLENGE_Server_free(&opcodes->body.CMD_AUTH_LOGON_CHALLENGE_Server);
            break;
        case CMD_AUTH_LOGON_PROOF:
            version2_CMD_AUTH_LOGON_PROOF_Server_free(&opcodes->body.CMD_AUTH_LOGON_PROOF_Server);
            break;
        case CMD_REALM_LIST:
            version2_CMD_REALM_LIST_Server_free(&opcodes->body.CMD_REALM_LIST_Server);
            break;
        case CMD_XFER_INITIATE:
            version2_CMD_XFER_INITIATE_free(&opcodes->body.CMD_XFER_INITIATE);
            break;
        case CMD_XFER_DATA:
            version2_CMD_XFER_DATA_free(&opcodes->body.CMD_XFER_DATA);
            break;
        default:
            break;
    }
}

#include "wow_login_messages/version5.h"

static size_t version5_Realm_size(const version5_Realm* object) {
    return 12 + STRING_SIZE(object->name) + STRING_SIZE(object->address);
}

static WowLoginResult version5_Realm_read(WowLoginReader* reader, version5_Realm* object) {
    object->realm_type = 0;
    READ_U8(object->realm_type);

    READ_BOOL8(object->locked);

    object->flag = 0;
    READ_U8(object->flag);

    READ_CSTRING(object->name);

    READ_CSTRING(object->address);

    READ_FLOAT(object->population);

    READ_U8(object->number_of_characters_on_realm);

    object->category = 0;
    READ_U8(object->category);

    READ_U8(object->realm_id);

    return WLM_RESULT_SUCCESS;
}

static WowLoginResult version5_Realm_write(WowLoginWriter* writer, const version5_Realm* object) {
    WRITE_U8(object->realm_type);

    WRITE_BOOL8(object->locked);

    WRITE_U8(object->flag);

    WRITE_CSTRING(object->name);

    WRITE_CSTRING(object->address);

    WRITE_FLOAT(object->population);

    WRITE_U8(object->number_of_characters_on_realm);

    WRITE_U8(object->category);

    WRITE_U8(object->realm_id);


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version5_Realm_free(version5_Realm* object) {
    FREE_STRING(object->name);

    FREE_STRING(object->address);

}

static WowLoginResult version5_CMD_AUTH_LOGON_CHALLENGE_Server_read(WowLoginReader* reader, version5_CMD_AUTH_LOGON_CHALLENGE_Server* object) {
    SKIP_FORWARD_BYTES(1);

    object->result = 0;
    READ_U8(object->result);

    if (object->result == VERSION2_LOGIN_RESULT_SUCCESS) {
        READ_ARRAY_ALLOCATE(object->server_public_key, 32, sizeof(uint8_t));
        READ_ARRAY(object->server_public_key, 32, READ_U8((*object->server_public_key)[i]));

        READ_U8(object->generator_length);

        READ_ARRAY_ALLOCATE(object->generator, object->generator_length, sizeof(uint8_t));
        READ_ARRAY(object->generator, object->generator_length, READ_U8(object->generator[i]));

        READ_U8(object->large_safe_prime_length);

        READ_ARRAY_ALLOCATE(object->large_safe_prime, object->large_safe_prime_length, sizeof(uint8_t));
        READ_ARRAY(object->large_safe_prime, object->large_safe_prime_length, READ_U8(object->large_safe_prime[i]));

        READ_ARRAY_ALLOCATE(object->salt, 32, sizeof(uint8_t));
        READ_ARRAY(object->salt, 32, READ_U8((*object->salt)[i]));

        READ_ARRAY_ALLOCATE(object->crc_salt, 16, sizeof(uint8_t));
        READ_ARRAY(object->crc_salt, 16, READ_U8((*object->crc_salt)[i]));

        object->security_flag = 0;
        READ_U8(object->security_flag);

        if ((object->security_flag & VERSION5_SECURITY_FLAG_PIN) != 0) {
            READ_U32(object->pin_grid_seed);

            READ_ARRAY_ALLOCATE(object->pin_salt, 16, sizeof(uint8_t));
            READ_ARRAY(object->pin_salt, 16, READ_U8((*object->pin_salt)[i]));

        }
        if ((object->security_flag & VERSION5_SECURITY_FLAG_MATRIX_CARD) != 0) {
            READ_U8(object->width);

            READ_U8(object->height);

            READ_U8(object->digit_count);

            READ_U8(object->challenge_count);

            READ_U64(object->seed);

        }
    }
    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version5_CMD_AUTH_LOGON_CHALLENGE_Server_write(WowLoginWriter* writer, const version5_CMD_AUTH_LOGON_CHALLENGE_Server* object) {
    WRITE_U8(0x00); /* opcode */

    WRITE_U8(0);

    WRITE_U8(object->result);

    if (object->result == VERSION2_LOGIN_RESULT_SUCCESS) {
        WRITE_ARRAY(object->server_public_key, 32, WRITE_U8((*object->server_public_key)[i]));

        WRITE_U8(object->generator_length);

        WRITE_ARRAY(object->generator, object->generator_length, WRITE_U8(object->generator[i]));

        WRITE_U8(object->large_safe_prime_length);

        WRITE_ARRAY(object->large_safe_prime, object->large_safe_prime_length, WRITE_U8(object->large_safe_prime[i]));

        WRITE_ARRAY(object->salt, 32, WRITE_U8((*object->salt)[i]));

        WRITE_ARRAY(object->crc_salt, 16, WRITE_U8((*object->crc_salt)[i]));

        WRITE_U8(object->security_flag);

        if ((object->security_flag & VERSION5_SECURITY_FLAG_PIN) != 0) {
            WRITE_U32(object->pin_grid_seed);

            WRITE_ARRAY(object->pin_salt, 16, WRITE_U8((*object->pin_salt)[i]));

        }
        if ((object->security_flag & VERSION5_SECURITY_FLAG_MATRIX_CARD) != 0) {
            WRITE_U8(object->width);

            WRITE_U8(object->height);

            WRITE_U8(object->digit_count);

            WRITE_U8(object->challenge_count);

            WRITE_U64(object->seed);

        }
    }

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version5_CMD_AUTH_LOGON_CHALLENGE_Server_free(version5_CMD_AUTH_LOGON_CHALLENGE_Server* object) {
    if (object->result == VERSION2_LOGIN_RESULT_SUCCESS) {
        free(object->server_public_key);
        object->server_public_key = NULL;
        free(object->generator);
        object->generator = NULL;
        free(object->large_safe_prime);
        object->large_safe_prime = NULL;
        free(object->salt);
        object->salt = NULL;
        free(object->crc_salt);
        object->crc_salt = NULL;
        if ((object->security_flag & VERSION5_SECURITY_FLAG_PIN) != 0) {
            free(object->pin_salt);
            object->pin_salt = NULL;
        }
        if ((object->security_flag & VERSION5_SECURITY_FLAG_MATRIX_CARD) != 0) {
        }
    }
}

static WowLoginResult version5_CMD_AUTH_LOGON_PROOF_Client_read(WowLoginReader* reader, version5_CMD_AUTH_LOGON_PROOF_Client* object) {
    READ_ARRAY_ALLOCATE(object->client_public_key, 32, sizeof(uint8_t));
    READ_ARRAY(object->client_public_key, 32, READ_U8((*object->client_public_key)[i]));

    READ_ARRAY_ALLOCATE(object->client_proof, 20, sizeof(uint8_t));
    READ_ARRAY(object->client_proof, 20, READ_U8((*object->client_proof)[i]));

    READ_ARRAY_ALLOCATE(object->crc_hash, 20, sizeof(uint8_t));
    READ_ARRAY(object->crc_hash, 20, READ_U8((*object->crc_hash)[i]));

    READ_U8(object->number_of_telemetry_keys);

    READ_ARRAY_ALLOCATE(object->telemetry_keys, object->number_of_telemetry_keys, sizeof(version2_TelemetryKey));
    READ_ARRAY(object->telemetry_keys, object->number_of_telemetry_keys, WLM_CHECK_RETURN_CODE(version2_TelemetryKey_read(reader, &object->telemetry_keys[i])));

    object->security_flag = 0;
    READ_U8(object->security_flag);

    if ((object->security_flag & VERSION5_SECURITY_FLAG_PIN) != 0) {
        READ_ARRAY_ALLOCATE(object->pin_salt, 16, sizeof(uint8_t));
        READ_ARRAY(object->pin_salt, 16, READ_U8((*object->pin_salt)[i]));

        READ_ARRAY_ALLOCATE(object->pin_hash, 20, sizeof(uint8_t));
        READ_ARRAY(object->pin_hash, 20, READ_U8((*object->pin_hash)[i]));

    }
    if ((object->security_flag & VERSION5_SECURITY_FLAG_MATRIX_CARD) != 0) {
        READ_ARRAY_ALLOCATE(object->matrix_card_proof, 20, sizeof(uint8_t));
        READ_ARRAY(object->matrix_card_proof, 20, READ_U8((*object->matrix_card_proof)[i]));

    }
    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version5_CMD_AUTH_LOGON_PROOF_Client_write(WowLoginWriter* writer, const version5_CMD_AUTH_LOGON_PROOF_Client* object) {
    WRITE_U8(0x01); /* opcode */

    WRITE_ARRAY(object->client_public_key, 32, WRITE_U8((*object->client_public_key)[i]));

    WRITE_ARRAY(object->client_proof, 20, WRITE_U8((*object->client_proof)[i]));

    WRITE_ARRAY(object->crc_hash, 20, WRITE_U8((*object->crc_hash)[i]));

    WRITE_U8(object->number_of_telemetry_keys);

    WRITE_ARRAY(object->telemetry_keys, object->number_of_telemetry_keys, WLM_CHECK_RETURN_CODE(version2_TelemetryKey_write(writer, &object->telemetry_keys[i])));

    WRITE_U8(object->security_flag);

    if ((object->security_flag & VERSION5_SECURITY_FLAG_PIN) != 0) {
        WRITE_ARRAY(object->pin_salt, 16, WRITE_U8((*object->pin_salt)[i]));

        WRITE_ARRAY(object->pin_hash, 20, WRITE_U8((*object->pin_hash)[i]));

    }
    if ((object->security_flag & VERSION5_SECURITY_FLAG_MATRIX_CARD) != 0) {
        WRITE_ARRAY(object->matrix_card_proof, 20, WRITE_U8((*object->matrix_card_proof)[i]));

    }

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version5_CMD_AUTH_LOGON_PROOF_Client_free(version5_CMD_AUTH_LOGON_PROOF_Client* object) {
    size_t i;

    free(object->client_public_key);
    object->client_public_key = NULL;
    free(object->client_proof);
    object->client_proof = NULL;
    free(object->crc_hash);
    object->crc_hash = NULL;
    for (i = 0; i < object->number_of_telemetry_keys; ++i) {
        version2_TelemetryKey_free(&((object->telemetry_keys)[i]));
    }
    free(object->telemetry_keys);
    object->telemetry_keys = NULL;
    if ((object->security_flag & VERSION5_SECURITY_FLAG_PIN) != 0) {
        free(object->pin_salt);
        object->pin_salt = NULL;
        free(object->pin_hash);
        object->pin_hash = NULL;
    }
    if ((object->security_flag & VERSION5_SECURITY_FLAG_MATRIX_CARD) != 0) {
        free(object->matrix_card_proof);
        object->matrix_card_proof = NULL;
    }
}

static WowLoginResult version5_CMD_AUTH_LOGON_PROOF_Server_read(WowLoginReader* reader, version5_CMD_AUTH_LOGON_PROOF_Server* object) {
    object->result = 0;
    READ_U8(object->result);

    if (object->result == VERSION2_LOGIN_RESULT_SUCCESS) {
        READ_ARRAY_ALLOCATE(object->server_proof, 20, sizeof(uint8_t));
        READ_ARRAY(object->server_proof, 20, READ_U8((*object->server_proof)[i]));

        READ_U32(object->hardware_survey_id);

        READ_U16(object->unknown);

    }
    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version5_CMD_AUTH_LOGON_PROOF_Server_write(WowLoginWriter* writer, const version5_CMD_AUTH_LOGON_PROOF_Server* object) {
    WRITE_U8(0x01); /* opcode */

    WRITE_U8(object->result);

    if (object->result == VERSION2_LOGIN_RESULT_SUCCESS) {
        WRITE_ARRAY(object->server_proof, 20, WRITE_U8((*object->server_proof)[i]));

        WRITE_U32(object->hardware_survey_id);

        WRITE_U16(object->unknown);

    }

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version5_CMD_AUTH_LOGON_PROOF_Server_free(version5_CMD_AUTH_LOGON_PROOF_Server* object) {
    if (object->result == VERSION2_LOGIN_RESULT_SUCCESS) {
        free(object->server_proof);
        object->server_proof = NULL;
    }
}

static WowLoginResult version5_CMD_AUTH_RECONNECT_PROOF_Server_read(WowLoginReader* reader, version5_CMD_AUTH_RECONNECT_PROOF_Server* object) {
    object->result = 0;
    READ_U8(object->result);

    SKIP_FORWARD_BYTES(2);

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version5_CMD_AUTH_RECONNECT_PROOF_Server_write(WowLoginWriter* writer, const version5_CMD_AUTH_RECONNECT_PROOF_Server* object) {
    WRITE_U8(0x03); /* opcode */

    WRITE_U8(object->result);

    WRITE_U16(0);


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version5_CMD_REALM_LIST_Client_write(WowLoginWriter* writer) {
    WRITE_U8(0x10); /* opcode */

    WRITE_U32(0);


    return WLM_RESULT_SUCCESS;
}

static size_t version5_CMD_REALM_LIST_Server_size(const version5_CMD_REALM_LIST_Server* object) {
    size_t _size = 7;

    /* C89 scope to allow variable declarations */ {
        int i;
        for(i = 0; i < (int)object->number_of_realms; ++i) {
            _size += version5_Realm_size(&object->realms[i]);
        }
    }

    return _size;
}

static WowLoginResult version5_CMD_REALM_LIST_Server_read(WowLoginReader* reader, version5_CMD_REALM_LIST_Server* object) {
    SKIP_FORWARD_BYTES(2);

    SKIP_FORWARD_BYTES(4);

    READ_U8(object->number_of_realms);

    READ_ARRAY_ALLOCATE(object->realms, object->number_of_realms, sizeof(version5_Realm));
    READ_ARRAY(object->realms, object->number_of_realms, WLM_CHECK_RETURN_CODE(version5_Realm_read(reader, &object->realms[i])));

    SKIP_FORWARD_BYTES(2);

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version5_CMD_REALM_LIST_Server_write(WowLoginWriter* writer, const version5_CMD_REALM_LIST_Server* object) {
    WRITE_U8(0x10); /* opcode */

    WRITE_U16((uint16_t)version5_CMD_REALM_LIST_Server_size(object));

    WRITE_U32(0);

    WRITE_U8(object->number_of_realms);

    WRITE_ARRAY(object->realms, object->number_of_realms, WLM_CHECK_RETURN_CODE(version5_Realm_write(writer, &object->realms[i])));

    WRITE_U16(0);


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version5_CMD_REALM_LIST_Server_free(version5_CMD_REALM_LIST_Server* object) {
    size_t i;

    for (i = 0; i < object->number_of_realms; ++i) {
        version5_Realm_free(&((object->realms)[i]));
    }
    free(object->realms);
    object->realms = NULL;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version5_CMD_XFER_ACCEPT_write(WowLoginWriter* writer) {
    WRITE_U8(0x32); /* opcode */


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version5_CMD_XFER_CANCEL_write(WowLoginWriter* writer) {
    WRITE_U8(0x34); /* opcode */


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version5_client_opcode_read(WowLoginReader* reader, Version5ClientOpcodeContainer* opcodes) {
    opcodes->opcode = 0;
    READ_U8(opcodes->opcode);

    switch (opcodes->opcode) {
        case CMD_AUTH_LOGON_PROOF:
            WLM_CHECK_RETURN_CODE(version5_CMD_AUTH_LOGON_PROOF_Client_read(reader, &opcodes->body.CMD_AUTH_LOGON_PROOF_Client));
            break;
        case CMD_AUTH_RECONNECT_PROOF:
            WLM_CHECK_RETURN_CODE(version2_CMD_AUTH_RECONNECT_PROOF_Client_read(reader, &opcodes->body.CMD_AUTH_RECONNECT_PROOF_Client));
            break;
        case CMD_XFER_RESUME:
            WLM_CHECK_RETURN_CODE(version2_CMD_XFER_RESUME_read(reader, &opcodes->body.CMD_XFER_RESUME));
            break;
        default:
            break;
    }

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version5_client_opcode_free(Version5ClientOpcodeContainer* opcodes) {
    switch (opcodes->opcode) {
        case CMD_AUTH_LOGON_PROOF:
            version5_CMD_AUTH_LOGON_PROOF_Client_free(&opcodes->body.CMD_AUTH_LOGON_PROOF_Client);
            break;
        case CMD_AUTH_RECONNECT_PROOF:
            version2_CMD_AUTH_RECONNECT_PROOF_Client_free(&opcodes->body.CMD_AUTH_RECONNECT_PROOF_Client);
            break;
        default:
            break;
    }
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version5_server_opcode_read(WowLoginReader* reader, Version5ServerOpcodeContainer* opcodes) {
    opcodes->opcode = 0;
    READ_U8(opcodes->opcode);

    switch (opcodes->opcode) {
        case CMD_AUTH_LOGON_CHALLENGE:
            WLM_CHECK_RETURN_CODE(version5_CMD_AUTH_LOGON_CHALLENGE_Server_read(reader, &opcodes->body.CMD_AUTH_LOGON_CHALLENGE_Server));
            break;
        case CMD_AUTH_LOGON_PROOF:
            WLM_CHECK_RETURN_CODE(version5_CMD_AUTH_LOGON_PROOF_Server_read(reader, &opcodes->body.CMD_AUTH_LOGON_PROOF_Server));
            break;
        case CMD_AUTH_RECONNECT_CHALLENGE:
            WLM_CHECK_RETURN_CODE(version2_CMD_AUTH_RECONNECT_CHALLENGE_Server_read(reader, &opcodes->body.CMD_AUTH_RECONNECT_CHALLENGE_Server));
            break;
        case CMD_AUTH_RECONNECT_PROOF:
            WLM_CHECK_RETURN_CODE(version5_CMD_AUTH_RECONNECT_PROOF_Server_read(reader, &opcodes->body.CMD_AUTH_RECONNECT_PROOF_Server));
            break;
        case CMD_REALM_LIST:
            WLM_CHECK_RETURN_CODE(version5_CMD_REALM_LIST_Server_read(reader, &opcodes->body.CMD_REALM_LIST_Server));
            break;
        case CMD_XFER_INITIATE:
            WLM_CHECK_RETURN_CODE(version2_CMD_XFER_INITIATE_read(reader, &opcodes->body.CMD_XFER_INITIATE));
            break;
        case CMD_XFER_DATA:
            WLM_CHECK_RETURN_CODE(version2_CMD_XFER_DATA_read(reader, &opcodes->body.CMD_XFER_DATA));
            break;
        default:
            break;
    }

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version5_server_opcode_free(Version5ServerOpcodeContainer* opcodes) {
    switch (opcodes->opcode) {
        case CMD_AUTH_LOGON_CHALLENGE:
            version5_CMD_AUTH_LOGON_CHALLENGE_Server_free(&opcodes->body.CMD_AUTH_LOGON_CHALLENGE_Server);
            break;
        case CMD_AUTH_LOGON_PROOF:
            version5_CMD_AUTH_LOGON_PROOF_Server_free(&opcodes->body.CMD_AUTH_LOGON_PROOF_Server);
            break;
        case CMD_AUTH_RECONNECT_CHALLENGE:
            version2_CMD_AUTH_RECONNECT_CHALLENGE_Server_free(&opcodes->body.CMD_AUTH_RECONNECT_CHALLENGE_Server);
            break;
        case CMD_REALM_LIST:
            version5_CMD_REALM_LIST_Server_free(&opcodes->body.CMD_REALM_LIST_Server);
            break;
        case CMD_XFER_INITIATE:
            version2_CMD_XFER_INITIATE_free(&opcodes->body.CMD_XFER_INITIATE);
            break;
        case CMD_XFER_DATA:
            version2_CMD_XFER_DATA_free(&opcodes->body.CMD_XFER_DATA);
            break;
        default:
            break;
    }
}

#include "wow_login_messages/version6.h"

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version6_CMD_REALM_LIST_Client_write(WowLoginWriter* writer) {
    WRITE_U8(0x10); /* opcode */

    WRITE_U32(0);


    return WLM_RESULT_SUCCESS;
}

static size_t version6_CMD_REALM_LIST_Server_size(const version6_CMD_REALM_LIST_Server* object) {
    size_t _size = 8;

    /* C89 scope to allow variable declarations */ {
        int i;
        for(i = 0; i < (int)object->number_of_realms; ++i) {
            _size += version5_Realm_size(&object->realms[i]);
        }
    }

    return _size;
}

static WowLoginResult version6_CMD_REALM_LIST_Server_read(WowLoginReader* reader, version6_CMD_REALM_LIST_Server* object) {
    SKIP_FORWARD_BYTES(2);

    SKIP_FORWARD_BYTES(4);

    READ_U16(object->number_of_realms);

    READ_ARRAY_ALLOCATE(object->realms, object->number_of_realms, sizeof(version5_Realm));
    READ_ARRAY(object->realms, object->number_of_realms, WLM_CHECK_RETURN_CODE(version5_Realm_read(reader, &object->realms[i])));

    SKIP_FORWARD_BYTES(2);

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version6_CMD_REALM_LIST_Server_write(WowLoginWriter* writer, const version6_CMD_REALM_LIST_Server* object) {
    WRITE_U8(0x10); /* opcode */

    WRITE_U16((uint16_t)version6_CMD_REALM_LIST_Server_size(object));

    WRITE_U32(0);

    WRITE_U16(object->number_of_realms);

    WRITE_ARRAY(object->realms, object->number_of_realms, WLM_CHECK_RETURN_CODE(version5_Realm_write(writer, &object->realms[i])));

    WRITE_U16(0);


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version6_CMD_REALM_LIST_Server_free(version6_CMD_REALM_LIST_Server* object) {
    size_t i;

    for (i = 0; i < object->number_of_realms; ++i) {
        version5_Realm_free(&((object->realms)[i]));
    }
    free(object->realms);
    object->realms = NULL;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version6_CMD_XFER_ACCEPT_write(WowLoginWriter* writer) {
    WRITE_U8(0x32); /* opcode */


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version6_CMD_XFER_CANCEL_write(WowLoginWriter* writer) {
    WRITE_U8(0x34); /* opcode */


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version6_client_opcode_read(WowLoginReader* reader, Version6ClientOpcodeContainer* opcodes) {
    opcodes->opcode = 0;
    READ_U8(opcodes->opcode);

    switch (opcodes->opcode) {
        case CMD_AUTH_LOGON_PROOF:
            WLM_CHECK_RETURN_CODE(version5_CMD_AUTH_LOGON_PROOF_Client_read(reader, &opcodes->body.CMD_AUTH_LOGON_PROOF_Client));
            break;
        case CMD_AUTH_RECONNECT_PROOF:
            WLM_CHECK_RETURN_CODE(version2_CMD_AUTH_RECONNECT_PROOF_Client_read(reader, &opcodes->body.CMD_AUTH_RECONNECT_PROOF_Client));
            break;
        case CMD_XFER_RESUME:
            WLM_CHECK_RETURN_CODE(version2_CMD_XFER_RESUME_read(reader, &opcodes->body.CMD_XFER_RESUME));
            break;
        default:
            break;
    }

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version6_client_opcode_free(Version6ClientOpcodeContainer* opcodes) {
    switch (opcodes->opcode) {
        case CMD_AUTH_LOGON_PROOF:
            version5_CMD_AUTH_LOGON_PROOF_Client_free(&opcodes->body.CMD_AUTH_LOGON_PROOF_Client);
            break;
        case CMD_AUTH_RECONNECT_PROOF:
            version2_CMD_AUTH_RECONNECT_PROOF_Client_free(&opcodes->body.CMD_AUTH_RECONNECT_PROOF_Client);
            break;
        default:
            break;
    }
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version6_server_opcode_read(WowLoginReader* reader, Version6ServerOpcodeContainer* opcodes) {
    opcodes->opcode = 0;
    READ_U8(opcodes->opcode);

    switch (opcodes->opcode) {
        case CMD_AUTH_LOGON_CHALLENGE:
            WLM_CHECK_RETURN_CODE(version5_CMD_AUTH_LOGON_CHALLENGE_Server_read(reader, &opcodes->body.CMD_AUTH_LOGON_CHALLENGE_Server));
            break;
        case CMD_AUTH_LOGON_PROOF:
            WLM_CHECK_RETURN_CODE(version5_CMD_AUTH_LOGON_PROOF_Server_read(reader, &opcodes->body.CMD_AUTH_LOGON_PROOF_Server));
            break;
        case CMD_AUTH_RECONNECT_CHALLENGE:
            WLM_CHECK_RETURN_CODE(version2_CMD_AUTH_RECONNECT_CHALLENGE_Server_read(reader, &opcodes->body.CMD_AUTH_RECONNECT_CHALLENGE_Server));
            break;
        case CMD_AUTH_RECONNECT_PROOF:
            WLM_CHECK_RETURN_CODE(version5_CMD_AUTH_RECONNECT_PROOF_Server_read(reader, &opcodes->body.CMD_AUTH_RECONNECT_PROOF_Server));
            break;
        case CMD_REALM_LIST:
            WLM_CHECK_RETURN_CODE(version6_CMD_REALM_LIST_Server_read(reader, &opcodes->body.CMD_REALM_LIST_Server));
            break;
        case CMD_XFER_INITIATE:
            WLM_CHECK_RETURN_CODE(version2_CMD_XFER_INITIATE_read(reader, &opcodes->body.CMD_XFER_INITIATE));
            break;
        case CMD_XFER_DATA:
            WLM_CHECK_RETURN_CODE(version2_CMD_XFER_DATA_read(reader, &opcodes->body.CMD_XFER_DATA));
            break;
        default:
            break;
    }

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version6_server_opcode_free(Version6ServerOpcodeContainer* opcodes) {
    switch (opcodes->opcode) {
        case CMD_AUTH_LOGON_CHALLENGE:
            version5_CMD_AUTH_LOGON_CHALLENGE_Server_free(&opcodes->body.CMD_AUTH_LOGON_CHALLENGE_Server);
            break;
        case CMD_AUTH_LOGON_PROOF:
            version5_CMD_AUTH_LOGON_PROOF_Server_free(&opcodes->body.CMD_AUTH_LOGON_PROOF_Server);
            break;
        case CMD_AUTH_RECONNECT_CHALLENGE:
            version2_CMD_AUTH_RECONNECT_CHALLENGE_Server_free(&opcodes->body.CMD_AUTH_RECONNECT_CHALLENGE_Server);
            break;
        case CMD_REALM_LIST:
            version6_CMD_REALM_LIST_Server_free(&opcodes->body.CMD_REALM_LIST_Server);
            break;
        case CMD_XFER_INITIATE:
            version2_CMD_XFER_INITIATE_free(&opcodes->body.CMD_XFER_INITIATE);
            break;
        case CMD_XFER_DATA:
            version2_CMD_XFER_DATA_free(&opcodes->body.CMD_XFER_DATA);
            break;
        default:
            break;
    }
}

#include "wow_login_messages/version7.h"

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version7_CMD_REALM_LIST_Client_write(WowLoginWriter* writer) {
    WRITE_U8(0x10); /* opcode */

    WRITE_U32(0);


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version7_CMD_XFER_ACCEPT_write(WowLoginWriter* writer) {
    WRITE_U8(0x32); /* opcode */


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version7_CMD_XFER_CANCEL_write(WowLoginWriter* writer) {
    WRITE_U8(0x34); /* opcode */


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version7_client_opcode_read(WowLoginReader* reader, Version7ClientOpcodeContainer* opcodes) {
    opcodes->opcode = 0;
    READ_U8(opcodes->opcode);

    switch (opcodes->opcode) {
        case CMD_AUTH_LOGON_PROOF:
            WLM_CHECK_RETURN_CODE(version5_CMD_AUTH_LOGON_PROOF_Client_read(reader, &opcodes->body.CMD_AUTH_LOGON_PROOF_Client));
            break;
        case CMD_AUTH_RECONNECT_PROOF:
            WLM_CHECK_RETURN_CODE(version2_CMD_AUTH_RECONNECT_PROOF_Client_read(reader, &opcodes->body.CMD_AUTH_RECONNECT_PROOF_Client));
            break;
        case CMD_XFER_RESUME:
            WLM_CHECK_RETURN_CODE(version2_CMD_XFER_RESUME_read(reader, &opcodes->body.CMD_XFER_RESUME));
            break;
        default:
            break;
    }

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version7_client_opcode_free(Version7ClientOpcodeContainer* opcodes) {
    switch (opcodes->opcode) {
        case CMD_AUTH_LOGON_PROOF:
            version5_CMD_AUTH_LOGON_PROOF_Client_free(&opcodes->body.CMD_AUTH_LOGON_PROOF_Client);
            break;
        case CMD_AUTH_RECONNECT_PROOF:
            version2_CMD_AUTH_RECONNECT_PROOF_Client_free(&opcodes->body.CMD_AUTH_RECONNECT_PROOF_Client);
            break;
        default:
            break;
    }
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version7_server_opcode_read(WowLoginReader* reader, Version7ServerOpcodeContainer* opcodes) {
    opcodes->opcode = 0;
    READ_U8(opcodes->opcode);

    switch (opcodes->opcode) {
        case CMD_AUTH_LOGON_CHALLENGE:
            WLM_CHECK_RETURN_CODE(version5_CMD_AUTH_LOGON_CHALLENGE_Server_read(reader, &opcodes->body.CMD_AUTH_LOGON_CHALLENGE_Server));
            break;
        case CMD_AUTH_LOGON_PROOF:
            WLM_CHECK_RETURN_CODE(version5_CMD_AUTH_LOGON_PROOF_Server_read(reader, &opcodes->body.CMD_AUTH_LOGON_PROOF_Server));
            break;
        case CMD_AUTH_RECONNECT_CHALLENGE:
            WLM_CHECK_RETURN_CODE(version2_CMD_AUTH_RECONNECT_CHALLENGE_Server_read(reader, &opcodes->body.CMD_AUTH_RECONNECT_CHALLENGE_Server));
            break;
        case CMD_AUTH_RECONNECT_PROOF:
            WLM_CHECK_RETURN_CODE(version5_CMD_AUTH_RECONNECT_PROOF_Server_read(reader, &opcodes->body.CMD_AUTH_RECONNECT_PROOF_Server));
            break;
        case CMD_REALM_LIST:
            WLM_CHECK_RETURN_CODE(version6_CMD_REALM_LIST_Server_read(reader, &opcodes->body.CMD_REALM_LIST_Server));
            break;
        case CMD_XFER_INITIATE:
            WLM_CHECK_RETURN_CODE(version2_CMD_XFER_INITIATE_read(reader, &opcodes->body.CMD_XFER_INITIATE));
            break;
        case CMD_XFER_DATA:
            WLM_CHECK_RETURN_CODE(version2_CMD_XFER_DATA_read(reader, &opcodes->body.CMD_XFER_DATA));
            break;
        default:
            break;
    }

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version7_server_opcode_free(Version7ServerOpcodeContainer* opcodes) {
    switch (opcodes->opcode) {
        case CMD_AUTH_LOGON_CHALLENGE:
            version5_CMD_AUTH_LOGON_CHALLENGE_Server_free(&opcodes->body.CMD_AUTH_LOGON_CHALLENGE_Server);
            break;
        case CMD_AUTH_LOGON_PROOF:
            version5_CMD_AUTH_LOGON_PROOF_Server_free(&opcodes->body.CMD_AUTH_LOGON_PROOF_Server);
            break;
        case CMD_AUTH_RECONNECT_CHALLENGE:
            version2_CMD_AUTH_RECONNECT_CHALLENGE_Server_free(&opcodes->body.CMD_AUTH_RECONNECT_CHALLENGE_Server);
            break;
        case CMD_REALM_LIST:
            version6_CMD_REALM_LIST_Server_free(&opcodes->body.CMD_REALM_LIST_Server);
            break;
        case CMD_XFER_INITIATE:
            version2_CMD_XFER_INITIATE_free(&opcodes->body.CMD_XFER_INITIATE);
            break;
        case CMD_XFER_DATA:
            version2_CMD_XFER_DATA_free(&opcodes->body.CMD_XFER_DATA);
            break;
        default:
            break;
    }
}

#include "wow_login_messages/version8.h"

static size_t version8_Realm_size(const version8_Realm* object) {
    size_t _size = 12 + STRING_SIZE(object->name) + STRING_SIZE(object->address);

    if ((object->flag & VERSION8_REALM_FLAG_SPECIFY_BUILD) != 0) {
        _size += 5;
    }

    return _size;
}

static WowLoginResult version8_Realm_read(WowLoginReader* reader, version8_Realm* object) {
    object->realm_type = 0;
    READ_U8(object->realm_type);

    READ_BOOL8(object->locked);

    object->flag = 0;
    READ_U8(object->flag);

    READ_CSTRING(object->name);

    READ_CSTRING(object->address);

    READ_FLOAT(object->population);

    READ_U8(object->number_of_characters_on_realm);

    object->category = 0;
    READ_U8(object->category);

    READ_U8(object->realm_id);

    if ((object->flag & VERSION8_REALM_FLAG_SPECIFY_BUILD) != 0) {
        WLM_CHECK_RETURN_CODE(all_Version_read(reader, &object->version));

    }
    return WLM_RESULT_SUCCESS;
}

static WowLoginResult version8_Realm_write(WowLoginWriter* writer, const version8_Realm* object) {
    WRITE_U8(object->realm_type);

    WRITE_BOOL8(object->locked);

    WRITE_U8(object->flag);

    WRITE_CSTRING(object->name);

    WRITE_CSTRING(object->address);

    WRITE_FLOAT(object->population);

    WRITE_U8(object->number_of_characters_on_realm);

    WRITE_U8(object->category);

    WRITE_U8(object->realm_id);

    if ((object->flag & VERSION8_REALM_FLAG_SPECIFY_BUILD) != 0) {
        WLM_CHECK_RETURN_CODE(all_Version_write(writer, &object->version));

    }

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version8_Realm_free(version8_Realm* object) {
    FREE_STRING(object->name);

    FREE_STRING(object->address);

    if ((object->flag & VERSION8_REALM_FLAG_SPECIFY_BUILD) != 0) {
    }
}

static WowLoginResult version8_CMD_AUTH_LOGON_CHALLENGE_Server_read(WowLoginReader* reader, version8_CMD_AUTH_LOGON_CHALLENGE_Server* object) {
    SKIP_FORWARD_BYTES(1);

    object->result = 0;
    READ_U8(object->result);

    if (object->result == VERSION8_LOGIN_RESULT_SUCCESS) {
        READ_ARRAY_ALLOCATE(object->server_public_key, 32, sizeof(uint8_t));
        READ_ARRAY(object->server_public_key, 32, READ_U8((*object->server_public_key)[i]));

        READ_U8(object->generator_length);

        READ_ARRAY_ALLOCATE(object->generator, object->generator_length, sizeof(uint8_t));
        READ_ARRAY(object->generator, object->generator_length, READ_U8(object->generator[i]));

        READ_U8(object->large_safe_prime_length);

        READ_ARRAY_ALLOCATE(object->large_safe_prime, object->large_safe_prime_length, sizeof(uint8_t));
        READ_ARRAY(object->large_safe_prime, object->large_safe_prime_length, READ_U8(object->large_safe_prime[i]));

        READ_ARRAY_ALLOCATE(object->salt, 32, sizeof(uint8_t));
        READ_ARRAY(object->salt, 32, READ_U8((*object->salt)[i]));

        READ_ARRAY_ALLOCATE(object->crc_salt, 16, sizeof(uint8_t));
        READ_ARRAY(object->crc_salt, 16, READ_U8((*object->crc_salt)[i]));

        object->security_flag = 0;
        READ_U8(object->security_flag);

        if ((object->security_flag & VERSION8_SECURITY_FLAG_PIN) != 0) {
            READ_U32(object->pin_grid_seed);

            READ_ARRAY_ALLOCATE(object->pin_salt, 16, sizeof(uint8_t));
            READ_ARRAY(object->pin_salt, 16, READ_U8((*object->pin_salt)[i]));

        }
        if ((object->security_flag & VERSION8_SECURITY_FLAG_MATRIX_CARD) != 0) {
            READ_U8(object->width);

            READ_U8(object->height);

            READ_U8(object->digit_count);

            READ_U8(object->challenge_count);

            READ_U64(object->seed);

        }
        if ((object->security_flag & VERSION8_SECURITY_FLAG_AUTHENTICATOR) != 0) {
            READ_U8(object->required);

        }
    }
    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version8_CMD_AUTH_LOGON_CHALLENGE_Server_write(WowLoginWriter* writer, const version8_CMD_AUTH_LOGON_CHALLENGE_Server* object) {
    WRITE_U8(0x00); /* opcode */

    WRITE_U8(0);

    WRITE_U8(object->result);

    if (object->result == VERSION8_LOGIN_RESULT_SUCCESS) {
        WRITE_ARRAY(object->server_public_key, 32, WRITE_U8((*object->server_public_key)[i]));

        WRITE_U8(object->generator_length);

        WRITE_ARRAY(object->generator, object->generator_length, WRITE_U8(object->generator[i]));

        WRITE_U8(object->large_safe_prime_length);

        WRITE_ARRAY(object->large_safe_prime, object->large_safe_prime_length, WRITE_U8(object->large_safe_prime[i]));

        WRITE_ARRAY(object->salt, 32, WRITE_U8((*object->salt)[i]));

        WRITE_ARRAY(object->crc_salt, 16, WRITE_U8((*object->crc_salt)[i]));

        WRITE_U8(object->security_flag);

        if ((object->security_flag & VERSION8_SECURITY_FLAG_PIN) != 0) {
            WRITE_U32(object->pin_grid_seed);

            WRITE_ARRAY(object->pin_salt, 16, WRITE_U8((*object->pin_salt)[i]));

        }
        if ((object->security_flag & VERSION8_SECURITY_FLAG_MATRIX_CARD) != 0) {
            WRITE_U8(object->width);

            WRITE_U8(object->height);

            WRITE_U8(object->digit_count);

            WRITE_U8(object->challenge_count);

            WRITE_U64(object->seed);

        }
        if ((object->security_flag & VERSION8_SECURITY_FLAG_AUTHENTICATOR) != 0) {
            WRITE_U8(object->required);

        }
    }

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version8_CMD_AUTH_LOGON_CHALLENGE_Server_free(version8_CMD_AUTH_LOGON_CHALLENGE_Server* object) {
    if (object->result == VERSION8_LOGIN_RESULT_SUCCESS) {
        free(object->server_public_key);
        object->server_public_key = NULL;
        free(object->generator);
        object->generator = NULL;
        free(object->large_safe_prime);
        object->large_safe_prime = NULL;
        free(object->salt);
        object->salt = NULL;
        free(object->crc_salt);
        object->crc_salt = NULL;
        if ((object->security_flag & VERSION8_SECURITY_FLAG_PIN) != 0) {
            free(object->pin_salt);
            object->pin_salt = NULL;
        }
        if ((object->security_flag & VERSION8_SECURITY_FLAG_MATRIX_CARD) != 0) {
        }
        if ((object->security_flag & VERSION8_SECURITY_FLAG_AUTHENTICATOR) != 0) {
        }
    }
}

static WowLoginResult version8_CMD_AUTH_LOGON_PROOF_Client_read(WowLoginReader* reader, version8_CMD_AUTH_LOGON_PROOF_Client* object) {
    READ_ARRAY_ALLOCATE(object->client_public_key, 32, sizeof(uint8_t));
    READ_ARRAY(object->client_public_key, 32, READ_U8((*object->client_public_key)[i]));

    READ_ARRAY_ALLOCATE(object->client_proof, 20, sizeof(uint8_t));
    READ_ARRAY(object->client_proof, 20, READ_U8((*object->client_proof)[i]));

    READ_ARRAY_ALLOCATE(object->crc_hash, 20, sizeof(uint8_t));
    READ_ARRAY(object->crc_hash, 20, READ_U8((*object->crc_hash)[i]));

    READ_U8(object->number_of_telemetry_keys);

    READ_ARRAY_ALLOCATE(object->telemetry_keys, object->number_of_telemetry_keys, sizeof(version2_TelemetryKey));
    READ_ARRAY(object->telemetry_keys, object->number_of_telemetry_keys, WLM_CHECK_RETURN_CODE(version2_TelemetryKey_read(reader, &object->telemetry_keys[i])));

    object->security_flag = 0;
    READ_U8(object->security_flag);

    if ((object->security_flag & VERSION8_SECURITY_FLAG_PIN) != 0) {
        READ_ARRAY_ALLOCATE(object->pin_salt, 16, sizeof(uint8_t));
        READ_ARRAY(object->pin_salt, 16, READ_U8((*object->pin_salt)[i]));

        READ_ARRAY_ALLOCATE(object->pin_hash, 20, sizeof(uint8_t));
        READ_ARRAY(object->pin_hash, 20, READ_U8((*object->pin_hash)[i]));

    }
    if ((object->security_flag & VERSION8_SECURITY_FLAG_MATRIX_CARD) != 0) {
        READ_ARRAY_ALLOCATE(object->matrix_card_proof, 20, sizeof(uint8_t));
        READ_ARRAY(object->matrix_card_proof, 20, READ_U8((*object->matrix_card_proof)[i]));

    }
    if ((object->security_flag & VERSION8_SECURITY_FLAG_AUTHENTICATOR) != 0) {
        READ_STRING(object->authenticator);

    }
    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version8_CMD_AUTH_LOGON_PROOF_Client_write(WowLoginWriter* writer, const version8_CMD_AUTH_LOGON_PROOF_Client* object) {
    WRITE_U8(0x01); /* opcode */

    WRITE_ARRAY(object->client_public_key, 32, WRITE_U8((*object->client_public_key)[i]));

    WRITE_ARRAY(object->client_proof, 20, WRITE_U8((*object->client_proof)[i]));

    WRITE_ARRAY(object->crc_hash, 20, WRITE_U8((*object->crc_hash)[i]));

    WRITE_U8(object->number_of_telemetry_keys);

    WRITE_ARRAY(object->telemetry_keys, object->number_of_telemetry_keys, WLM_CHECK_RETURN_CODE(version2_TelemetryKey_write(writer, &object->telemetry_keys[i])));

    WRITE_U8(object->security_flag);

    if ((object->security_flag & VERSION8_SECURITY_FLAG_PIN) != 0) {
        WRITE_ARRAY(object->pin_salt, 16, WRITE_U8((*object->pin_salt)[i]));

        WRITE_ARRAY(object->pin_hash, 20, WRITE_U8((*object->pin_hash)[i]));

    }
    if ((object->security_flag & VERSION8_SECURITY_FLAG_MATRIX_CARD) != 0) {
        WRITE_ARRAY(object->matrix_card_proof, 20, WRITE_U8((*object->matrix_card_proof)[i]));

    }
    if ((object->security_flag & VERSION8_SECURITY_FLAG_AUTHENTICATOR) != 0) {
        WRITE_STRING(object->authenticator);

    }

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version8_CMD_AUTH_LOGON_PROOF_Client_free(version8_CMD_AUTH_LOGON_PROOF_Client* object) {
    size_t i;

    free(object->client_public_key);
    object->client_public_key = NULL;
    free(object->client_proof);
    object->client_proof = NULL;
    free(object->crc_hash);
    object->crc_hash = NULL;
    for (i = 0; i < object->number_of_telemetry_keys; ++i) {
        version2_TelemetryKey_free(&((object->telemetry_keys)[i]));
    }
    free(object->telemetry_keys);
    object->telemetry_keys = NULL;
    if ((object->security_flag & VERSION8_SECURITY_FLAG_PIN) != 0) {
        free(object->pin_salt);
        object->pin_salt = NULL;
        free(object->pin_hash);
        object->pin_hash = NULL;
    }
    if ((object->security_flag & VERSION8_SECURITY_FLAG_MATRIX_CARD) != 0) {
        free(object->matrix_card_proof);
        object->matrix_card_proof = NULL;
    }
    if ((object->security_flag & VERSION8_SECURITY_FLAG_AUTHENTICATOR) != 0) {
        FREE_STRING(object->authenticator);

    }
}

static WowLoginResult version8_CMD_AUTH_LOGON_PROOF_Server_read(WowLoginReader* reader, version8_CMD_AUTH_LOGON_PROOF_Server* object) {
    object->result = 0;
    READ_U8(object->result);

    if (object->result == VERSION8_LOGIN_RESULT_SUCCESS) {
        READ_ARRAY_ALLOCATE(object->server_proof, 20, sizeof(uint8_t));
        READ_ARRAY(object->server_proof, 20, READ_U8((*object->server_proof)[i]));

        object->account_flag = 0;
        READ_U32(object->account_flag);

        READ_U32(object->hardware_survey_id);

        READ_U16(object->unknown);

    }
    else if (object->result == VERSION8_LOGIN_RESULT_FAIL_UNKNOWN0|| object->result == VERSION8_LOGIN_RESULT_FAIL_UNKNOWN1|| object->result == VERSION8_LOGIN_RESULT_FAIL_BANNED|| object->result == VERSION8_LOGIN_RESULT_FAIL_UNKNOWN_ACCOUNT|| object->result == VERSION8_LOGIN_RESULT_FAIL_INCORRECT_PASSWORD|| object->result == VERSION8_LOGIN_RESULT_FAIL_ALREADY_ONLINE|| object->result == VERSION8_LOGIN_RESULT_FAIL_NO_TIME|| object->result == VERSION8_LOGIN_RESULT_FAIL_DB_BUSY|| object->result == VERSION8_LOGIN_RESULT_FAIL_VERSION_INVALID|| object->result == VERSION8_LOGIN_RESULT_LOGIN_DOWNLOAD_FILE|| object->result == VERSION8_LOGIN_RESULT_FAIL_INVALID_SERVER|| object->result == VERSION8_LOGIN_RESULT_FAIL_SUSPENDED|| object->result == VERSION8_LOGIN_RESULT_FAIL_NO_ACCESS|| object->result == VERSION8_LOGIN_RESULT_SUCCESS_SURVEY|| object->result == VERSION8_LOGIN_RESULT_FAIL_PARENTALCONTROL|| object->result == VERSION8_LOGIN_RESULT_FAIL_LOCKED_ENFORCED) {
        SKIP_FORWARD_BYTES(2);

    }
    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version8_CMD_AUTH_LOGON_PROOF_Server_write(WowLoginWriter* writer, const version8_CMD_AUTH_LOGON_PROOF_Server* object) {
    WRITE_U8(0x01); /* opcode */

    WRITE_U8(object->result);

    if (object->result == VERSION8_LOGIN_RESULT_SUCCESS) {
        WRITE_ARRAY(object->server_proof, 20, WRITE_U8((*object->server_proof)[i]));

        WRITE_U32(object->account_flag);

        WRITE_U32(object->hardware_survey_id);

        WRITE_U16(object->unknown);

    }
    else if (object->result == VERSION8_LOGIN_RESULT_FAIL_UNKNOWN0|| object->result == VERSION8_LOGIN_RESULT_FAIL_UNKNOWN1|| object->result == VERSION8_LOGIN_RESULT_FAIL_BANNED|| object->result == VERSION8_LOGIN_RESULT_FAIL_UNKNOWN_ACCOUNT|| object->result == VERSION8_LOGIN_RESULT_FAIL_INCORRECT_PASSWORD|| object->result == VERSION8_LOGIN_RESULT_FAIL_ALREADY_ONLINE|| object->result == VERSION8_LOGIN_RESULT_FAIL_NO_TIME|| object->result == VERSION8_LOGIN_RESULT_FAIL_DB_BUSY|| object->result == VERSION8_LOGIN_RESULT_FAIL_VERSION_INVALID|| object->result == VERSION8_LOGIN_RESULT_LOGIN_DOWNLOAD_FILE|| object->result == VERSION8_LOGIN_RESULT_FAIL_INVALID_SERVER|| object->result == VERSION8_LOGIN_RESULT_FAIL_SUSPENDED|| object->result == VERSION8_LOGIN_RESULT_FAIL_NO_ACCESS|| object->result == VERSION8_LOGIN_RESULT_SUCCESS_SURVEY|| object->result == VERSION8_LOGIN_RESULT_FAIL_PARENTALCONTROL|| object->result == VERSION8_LOGIN_RESULT_FAIL_LOCKED_ENFORCED) {
        WRITE_U16(0);

    }

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version8_CMD_AUTH_LOGON_PROOF_Server_free(version8_CMD_AUTH_LOGON_PROOF_Server* object) {
    if (object->result == VERSION8_LOGIN_RESULT_SUCCESS) {
        free(object->server_proof);
        object->server_proof = NULL;
    }
    else if (object->result == VERSION8_LOGIN_RESULT_FAIL_UNKNOWN0|| object->result == VERSION8_LOGIN_RESULT_FAIL_UNKNOWN1|| object->result == VERSION8_LOGIN_RESULT_FAIL_BANNED|| object->result == VERSION8_LOGIN_RESULT_FAIL_UNKNOWN_ACCOUNT|| object->result == VERSION8_LOGIN_RESULT_FAIL_INCORRECT_PASSWORD|| object->result == VERSION8_LOGIN_RESULT_FAIL_ALREADY_ONLINE|| object->result == VERSION8_LOGIN_RESULT_FAIL_NO_TIME|| object->result == VERSION8_LOGIN_RESULT_FAIL_DB_BUSY|| object->result == VERSION8_LOGIN_RESULT_FAIL_VERSION_INVALID|| object->result == VERSION8_LOGIN_RESULT_LOGIN_DOWNLOAD_FILE|| object->result == VERSION8_LOGIN_RESULT_FAIL_INVALID_SERVER|| object->result == VERSION8_LOGIN_RESULT_FAIL_SUSPENDED|| object->result == VERSION8_LOGIN_RESULT_FAIL_NO_ACCESS|| object->result == VERSION8_LOGIN_RESULT_SUCCESS_SURVEY|| object->result == VERSION8_LOGIN_RESULT_FAIL_PARENTALCONTROL|| object->result == VERSION8_LOGIN_RESULT_FAIL_LOCKED_ENFORCED) {
    }
}

static WowLoginResult version8_CMD_AUTH_RECONNECT_CHALLENGE_Server_read(WowLoginReader* reader, version8_CMD_AUTH_RECONNECT_CHALLENGE_Server* object) {
    object->result = 0;
    READ_U8(object->result);

    if (object->result == VERSION8_LOGIN_RESULT_SUCCESS) {
        READ_ARRAY_ALLOCATE(object->challenge_data, 16, sizeof(uint8_t));
        READ_ARRAY(object->challenge_data, 16, READ_U8((*object->challenge_data)[i]));

        READ_ARRAY_ALLOCATE(object->checksum_salt, 16, sizeof(uint8_t));
        READ_ARRAY(object->checksum_salt, 16, READ_U8((*object->checksum_salt)[i]));

    }
    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version8_CMD_AUTH_RECONNECT_CHALLENGE_Server_write(WowLoginWriter* writer, const version8_CMD_AUTH_RECONNECT_CHALLENGE_Server* object) {
    WRITE_U8(0x02); /* opcode */

    WRITE_U8(object->result);

    if (object->result == VERSION8_LOGIN_RESULT_SUCCESS) {
        WRITE_ARRAY(object->challenge_data, 16, WRITE_U8((*object->challenge_data)[i]));

        WRITE_ARRAY(object->checksum_salt, 16, WRITE_U8((*object->checksum_salt)[i]));

    }

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version8_CMD_AUTH_RECONNECT_CHALLENGE_Server_free(version8_CMD_AUTH_RECONNECT_CHALLENGE_Server* object) {
    if (object->result == VERSION8_LOGIN_RESULT_SUCCESS) {
        free(object->challenge_data);
        object->challenge_data = NULL;
        free(object->checksum_salt);
        object->checksum_salt = NULL;
    }
}

static WowLoginResult version8_CMD_AUTH_RECONNECT_PROOF_Server_read(WowLoginReader* reader, version8_CMD_AUTH_RECONNECT_PROOF_Server* object) {
    object->result = 0;
    READ_U8(object->result);

    SKIP_FORWARD_BYTES(2);

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version8_CMD_AUTH_RECONNECT_PROOF_Server_write(WowLoginWriter* writer, const version8_CMD_AUTH_RECONNECT_PROOF_Server* object) {
    WRITE_U8(0x03); /* opcode */

    WRITE_U8(object->result);

    WRITE_U16(0);


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version8_CMD_REALM_LIST_Client_write(WowLoginWriter* writer) {
    WRITE_U8(0x10); /* opcode */

    WRITE_U32(0);


    return WLM_RESULT_SUCCESS;
}

static size_t version8_CMD_REALM_LIST_Server_size(const version8_CMD_REALM_LIST_Server* object) {
    size_t _size = 8;

    /* C89 scope to allow variable declarations */ {
        int i;
        for(i = 0; i < (int)object->number_of_realms; ++i) {
            _size += version8_Realm_size(&object->realms[i]);
        }
    }

    return _size;
}

static WowLoginResult version8_CMD_REALM_LIST_Server_read(WowLoginReader* reader, version8_CMD_REALM_LIST_Server* object) {
    SKIP_FORWARD_BYTES(2);

    SKIP_FORWARD_BYTES(4);

    READ_U16(object->number_of_realms);

    READ_ARRAY_ALLOCATE(object->realms, object->number_of_realms, sizeof(version8_Realm));
    READ_ARRAY(object->realms, object->number_of_realms, WLM_CHECK_RETURN_CODE(version8_Realm_read(reader, &object->realms[i])));

    SKIP_FORWARD_BYTES(2);

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version8_CMD_REALM_LIST_Server_write(WowLoginWriter* writer, const version8_CMD_REALM_LIST_Server* object) {
    WRITE_U8(0x10); /* opcode */

    WRITE_U16((uint16_t)version8_CMD_REALM_LIST_Server_size(object));

    WRITE_U32(0);

    WRITE_U16(object->number_of_realms);

    WRITE_ARRAY(object->realms, object->number_of_realms, WLM_CHECK_RETURN_CODE(version8_Realm_write(writer, &object->realms[i])));

    WRITE_U16(0);


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version8_CMD_REALM_LIST_Server_free(version8_CMD_REALM_LIST_Server* object) {
    size_t i;

    for (i = 0; i < object->number_of_realms; ++i) {
        version8_Realm_free(&((object->realms)[i]));
    }
    free(object->realms);
    object->realms = NULL;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version8_CMD_XFER_ACCEPT_write(WowLoginWriter* writer) {
    WRITE_U8(0x32); /* opcode */


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version8_CMD_XFER_CANCEL_write(WowLoginWriter* writer) {
    WRITE_U8(0x34); /* opcode */


    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version8_client_opcode_read(WowLoginReader* reader, Version8ClientOpcodeContainer* opcodes) {
    opcodes->opcode = 0;
    READ_U8(opcodes->opcode);

    switch (opcodes->opcode) {
        case CMD_AUTH_LOGON_PROOF:
            WLM_CHECK_RETURN_CODE(version8_CMD_AUTH_LOGON_PROOF_Client_read(reader, &opcodes->body.CMD_AUTH_LOGON_PROOF_Client));
            break;
        case CMD_AUTH_RECONNECT_PROOF:
            WLM_CHECK_RETURN_CODE(version2_CMD_AUTH_RECONNECT_PROOF_Client_read(reader, &opcodes->body.CMD_AUTH_RECONNECT_PROOF_Client));
            break;
        case CMD_XFER_RESUME:
            WLM_CHECK_RETURN_CODE(version2_CMD_XFER_RESUME_read(reader, &opcodes->body.CMD_XFER_RESUME));
            break;
        default:
            break;
    }

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version8_client_opcode_free(Version8ClientOpcodeContainer* opcodes) {
    switch (opcodes->opcode) {
        case CMD_AUTH_LOGON_PROOF:
            version8_CMD_AUTH_LOGON_PROOF_Client_free(&opcodes->body.CMD_AUTH_LOGON_PROOF_Client);
            break;
        case CMD_AUTH_RECONNECT_PROOF:
            version2_CMD_AUTH_RECONNECT_PROOF_Client_free(&opcodes->body.CMD_AUTH_RECONNECT_PROOF_Client);
            break;
        default:
            break;
    }
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginResult version8_server_opcode_read(WowLoginReader* reader, Version8ServerOpcodeContainer* opcodes) {
    opcodes->opcode = 0;
    READ_U8(opcodes->opcode);

    switch (opcodes->opcode) {
        case CMD_AUTH_LOGON_CHALLENGE:
            WLM_CHECK_RETURN_CODE(version8_CMD_AUTH_LOGON_CHALLENGE_Server_read(reader, &opcodes->body.CMD_AUTH_LOGON_CHALLENGE_Server));
            break;
        case CMD_AUTH_LOGON_PROOF:
            WLM_CHECK_RETURN_CODE(version8_CMD_AUTH_LOGON_PROOF_Server_read(reader, &opcodes->body.CMD_AUTH_LOGON_PROOF_Server));
            break;
        case CMD_AUTH_RECONNECT_CHALLENGE:
            WLM_CHECK_RETURN_CODE(version8_CMD_AUTH_RECONNECT_CHALLENGE_Server_read(reader, &opcodes->body.CMD_AUTH_RECONNECT_CHALLENGE_Server));
            break;
        case CMD_AUTH_RECONNECT_PROOF:
            WLM_CHECK_RETURN_CODE(version8_CMD_AUTH_RECONNECT_PROOF_Server_read(reader, &opcodes->body.CMD_AUTH_RECONNECT_PROOF_Server));
            break;
        case CMD_REALM_LIST:
            WLM_CHECK_RETURN_CODE(version8_CMD_REALM_LIST_Server_read(reader, &opcodes->body.CMD_REALM_LIST_Server));
            break;
        case CMD_XFER_INITIATE:
            WLM_CHECK_RETURN_CODE(version2_CMD_XFER_INITIATE_read(reader, &opcodes->body.CMD_XFER_INITIATE));
            break;
        case CMD_XFER_DATA:
            WLM_CHECK_RETURN_CODE(version2_CMD_XFER_DATA_read(reader, &opcodes->body.CMD_XFER_DATA));
            break;
        default:
            break;
    }

    return WLM_RESULT_SUCCESS;
}

WOW_LOGIN_MESSAGES_C_EXPORT void version8_server_opcode_free(Version8ServerOpcodeContainer* opcodes) {
    switch (opcodes->opcode) {
        case CMD_AUTH_LOGON_CHALLENGE:
            version8_CMD_AUTH_LOGON_CHALLENGE_Server_free(&opcodes->body.CMD_AUTH_LOGON_CHALLENGE_Server);
            break;
        case CMD_AUTH_LOGON_PROOF:
            version8_CMD_AUTH_LOGON_PROOF_Server_free(&opcodes->body.CMD_AUTH_LOGON_PROOF_Server);
            break;
        case CMD_AUTH_RECONNECT_CHALLENGE:
            version8_CMD_AUTH_RECONNECT_CHALLENGE_Server_free(&opcodes->body.CMD_AUTH_RECONNECT_CHALLENGE_Server);
            break;
        case CMD_REALM_LIST:
            version8_CMD_REALM_LIST_Server_free(&opcodes->body.CMD_REALM_LIST_Server);
            break;
        case CMD_XFER_INITIATE:
            version2_CMD_XFER_INITIATE_free(&opcodes->body.CMD_XFER_INITIATE);
            break;
        case CMD_XFER_DATA:
            version2_CMD_XFER_DATA_free(&opcodes->body.CMD_XFER_DATA);
            break;
        default:
            break;
    }
}
