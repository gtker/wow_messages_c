#ifndef WOW_LOGIN_MESSAGES_VERSION3_H
#define WOW_LOGIN_MESSAGES_VERSION3_H

#include "wow_login_messages/wow_login_messages.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include "wow_login_messages/all.h" /* type include */
#include "wow_login_messages/version2.h" /* type include */

typedef all_Locale version3_Locale;

typedef version2_LoginResult version3_LoginResult;

typedef all_Os version3_Os;

typedef all_Platform version3_Platform;

typedef all_ProtocolVersion version3_ProtocolVersion;

typedef version2_RealmCategory version3_RealmCategory;

typedef version2_RealmType version3_RealmType;

typedef enum {
    VERSION3_SECURITY_FLAG_NONE = 0,
    VERSION3_SECURITY_FLAG_PIN = 1,
} version3_SecurityFlag;

typedef version2_RealmFlag version3_RealmFlag;

typedef version2_Realm version3_Realm;

typedef all_Version version3_Version;

typedef version2_TelemetryKey version3_TelemetryKey;

typedef struct {
    version3_LoginResult result;
    uint8_t* server_public_key;
    uint8_t generator_length;
    uint8_t* generator;
    uint8_t large_safe_prime_length;
    uint8_t* large_safe_prime;
    uint8_t* salt;
    uint8_t* crc_salt;
    version3_SecurityFlag security_flag;
    uint32_t pin_grid_seed;
    uint8_t* pin_salt;

} version3_CMD_AUTH_LOGON_CHALLENGE_Server;
WOWLOGINMESSAGES_EXPORT WowLoginResult version3_CMD_AUTH_LOGON_CHALLENGE_Server_write(WowLoginWriter* writer, const version3_CMD_AUTH_LOGON_CHALLENGE_Server* object);
WOWLOGINMESSAGES_EXPORT void version3_CMD_AUTH_LOGON_CHALLENGE_Server_free(version3_CMD_AUTH_LOGON_CHALLENGE_Server* object);

typedef all_CMD_AUTH_LOGON_CHALLENGE_Client version3_CMD_AUTH_LOGON_CHALLENGE_Client;

typedef version2_CMD_AUTH_LOGON_PROOF_Server version3_CMD_AUTH_LOGON_PROOF_Server;

typedef struct {
    uint8_t* client_public_key;
    uint8_t* client_proof;
    uint8_t* crc_hash;
    uint8_t number_of_telemetry_keys;
    version2_TelemetryKey* telemetry_keys;
    version3_SecurityFlag security_flag;
    uint8_t* pin_salt;
    uint8_t* pin_hash;

} version3_CMD_AUTH_LOGON_PROOF_Client;
WOWLOGINMESSAGES_EXPORT WowLoginResult version3_CMD_AUTH_LOGON_PROOF_Client_write(WowLoginWriter* writer, const version3_CMD_AUTH_LOGON_PROOF_Client* object);
WOWLOGINMESSAGES_EXPORT void version3_CMD_AUTH_LOGON_PROOF_Client_free(version3_CMD_AUTH_LOGON_PROOF_Client* object);

typedef all_CMD_AUTH_RECONNECT_CHALLENGE_Client version3_CMD_AUTH_RECONNECT_CHALLENGE_Client;

typedef struct {
    uint32_t survey_id;
    uint8_t error;
    uint16_t compressed_data_length;
    uint8_t* data;

} version3_CMD_SURVEY_RESULT;
WOWLOGINMESSAGES_EXPORT WowLoginResult version3_CMD_SURVEY_RESULT_write(WowLoginWriter* writer, const version3_CMD_SURVEY_RESULT* object);
WOWLOGINMESSAGES_EXPORT void version3_CMD_SURVEY_RESULT_free(version3_CMD_SURVEY_RESULT* object);

typedef version2_CMD_REALM_LIST_Server version3_CMD_REALM_LIST_Server;

WOWLOGINMESSAGES_EXPORT WowLoginResult version3_CMD_REALM_LIST_Client_write(WowLoginWriter* writer);

typedef version2_CMD_XFER_INITIATE version3_CMD_XFER_INITIATE;

typedef version2_CMD_XFER_DATA version3_CMD_XFER_DATA;

WOWLOGINMESSAGES_EXPORT WowLoginResult version3_CMD_XFER_ACCEPT_write(WowLoginWriter* writer);

typedef version2_CMD_XFER_RESUME version3_CMD_XFER_RESUME;

WOWLOGINMESSAGES_EXPORT WowLoginResult version3_CMD_XFER_CANCEL_write(WowLoginWriter* writer);

typedef struct {
    WowLoginOpcode opcode;

    union {
        version3_CMD_AUTH_LOGON_PROOF_Client CMD_AUTH_LOGON_PROOF_Client;
        version3_CMD_SURVEY_RESULT CMD_SURVEY_RESULT;
        version2_CMD_XFER_RESUME CMD_XFER_RESUME;
    } body;
} Version3ClientOpcodeContainer;

WOWLOGINMESSAGES_EXPORT WowLoginResult version3_client_opcode_read(WowLoginReader* reader, Version3ClientOpcodeContainer* opcodes);

WOWLOGINMESSAGES_EXPORT void version3_client_opcode_free(Version3ClientOpcodeContainer* opcodes);

typedef struct {
    WowLoginOpcode opcode;

    union {
        version3_CMD_AUTH_LOGON_CHALLENGE_Server CMD_AUTH_LOGON_CHALLENGE_Server;
        version2_CMD_AUTH_LOGON_PROOF_Server CMD_AUTH_LOGON_PROOF_Server;
        version2_CMD_REALM_LIST_Server CMD_REALM_LIST_Server;
        version2_CMD_XFER_INITIATE CMD_XFER_INITIATE;
        version2_CMD_XFER_DATA CMD_XFER_DATA;
    } body;
} Version3ServerOpcodeContainer;

WOWLOGINMESSAGES_EXPORT WowLoginResult version3_server_opcode_read(WowLoginReader* reader, Version3ServerOpcodeContainer* opcodes);

WOWLOGINMESSAGES_EXPORT void version3_server_opcode_free(Version3ServerOpcodeContainer* opcodes);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* WOW_LOGIN_MESSAGES_VERSION3_H */
