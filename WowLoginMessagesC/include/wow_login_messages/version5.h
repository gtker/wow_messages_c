#ifndef WOW_LOGIN_MESSAGES_VERSION5_H
#define WOW_LOGIN_MESSAGES_VERSION5_H

#include "wow_login_messages/wow_login_messages.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include "wow_login_messages/all.h" /* type include */
#include "wow_login_messages/version2.h" /* type include */

typedef all_Locale version5_Locale;

typedef version2_LoginResult version5_LoginResult;

typedef all_Os version5_Os;

typedef all_Platform version5_Platform;

typedef all_ProtocolVersion version5_ProtocolVersion;

typedef version2_RealmCategory version5_RealmCategory;

typedef version2_RealmType version5_RealmType;

typedef version2_RealmFlag version5_RealmFlag;

typedef enum {
    VERSION5_SECURITY_FLAG_NONE = 0,
    VERSION5_SECURITY_FLAG_PIN = 1,
    VERSION5_SECURITY_FLAG_MATRIX_CARD = 2
} version5_SecurityFlag;

typedef struct {
    version5_RealmType realm_type;
    bool locked;
    version5_RealmFlag flag;
    WowLoginString name;
    WowLoginString address;
    float population;
    uint8_t number_of_characters_on_realm;
    version5_RealmCategory category;
    uint8_t realm_id;

} version5_Realm;

typedef all_Version version5_Version;

typedef version2_TelemetryKey version5_TelemetryKey;

typedef struct {
    version5_LoginResult result;
    uint8_t (*server_public_key)[32];
    uint8_t generator_length;
    uint8_t* generator;
    uint8_t large_safe_prime_length;
    uint8_t* large_safe_prime;
    uint8_t (*salt)[32];
    uint8_t (*crc_salt)[16];
    version5_SecurityFlag security_flag;
    uint32_t pin_grid_seed;
    uint8_t (*pin_salt)[16];
    uint8_t width;
    uint8_t height;
    uint8_t digit_count;
    uint8_t challenge_count;
    uint64_t seed;

} version5_CMD_AUTH_LOGON_CHALLENGE_Server;
WOWLOGINMESSAGESC_EXPORT WowLoginResult version5_CMD_AUTH_LOGON_CHALLENGE_Server_write(WowLoginWriter* writer, const version5_CMD_AUTH_LOGON_CHALLENGE_Server* object);
WOWLOGINMESSAGESC_EXPORT void version5_CMD_AUTH_LOGON_CHALLENGE_Server_free(version5_CMD_AUTH_LOGON_CHALLENGE_Server* object);

typedef all_CMD_AUTH_LOGON_CHALLENGE_Client version5_CMD_AUTH_LOGON_CHALLENGE_Client;

typedef struct {
    uint8_t (*client_public_key)[32];
    uint8_t (*client_proof)[20];
    uint8_t (*crc_hash)[20];
    uint8_t number_of_telemetry_keys;
    version2_TelemetryKey* telemetry_keys;
    version5_SecurityFlag security_flag;
    uint8_t (*pin_salt)[16];
    uint8_t (*pin_hash)[20];
    uint8_t (*matrix_card_proof)[20];

} version5_CMD_AUTH_LOGON_PROOF_Client;
WOWLOGINMESSAGESC_EXPORT WowLoginResult version5_CMD_AUTH_LOGON_PROOF_Client_write(WowLoginWriter* writer, const version5_CMD_AUTH_LOGON_PROOF_Client* object);
WOWLOGINMESSAGESC_EXPORT void version5_CMD_AUTH_LOGON_PROOF_Client_free(version5_CMD_AUTH_LOGON_PROOF_Client* object);

typedef struct {
    version5_LoginResult result;
    uint8_t (*server_proof)[20];
    uint32_t hardware_survey_id;
    uint16_t unknown;

} version5_CMD_AUTH_LOGON_PROOF_Server;
WOWLOGINMESSAGESC_EXPORT WowLoginResult version5_CMD_AUTH_LOGON_PROOF_Server_write(WowLoginWriter* writer, const version5_CMD_AUTH_LOGON_PROOF_Server* object);
WOWLOGINMESSAGESC_EXPORT void version5_CMD_AUTH_LOGON_PROOF_Server_free(version5_CMD_AUTH_LOGON_PROOF_Server* object);

typedef version2_CMD_AUTH_RECONNECT_CHALLENGE_Server version5_CMD_AUTH_RECONNECT_CHALLENGE_Server;

typedef all_CMD_AUTH_RECONNECT_CHALLENGE_Client version5_CMD_AUTH_RECONNECT_CHALLENGE_Client;

typedef version2_CMD_AUTH_RECONNECT_PROOF_Client version5_CMD_AUTH_RECONNECT_PROOF_Client;

typedef struct {
    version5_LoginResult result;

} version5_CMD_AUTH_RECONNECT_PROOF_Server;
WOWLOGINMESSAGESC_EXPORT WowLoginResult version5_CMD_AUTH_RECONNECT_PROOF_Server_write(WowLoginWriter* writer, const version5_CMD_AUTH_RECONNECT_PROOF_Server* object);

WOWLOGINMESSAGESC_EXPORT WowLoginResult version5_CMD_REALM_LIST_Client_write(WowLoginWriter* writer);

typedef struct {
    uint8_t number_of_realms;
    version5_Realm* realms;

} version5_CMD_REALM_LIST_Server;
WOWLOGINMESSAGESC_EXPORT WowLoginResult version5_CMD_REALM_LIST_Server_write(WowLoginWriter* writer, const version5_CMD_REALM_LIST_Server* object);
WOWLOGINMESSAGESC_EXPORT void version5_CMD_REALM_LIST_Server_free(version5_CMD_REALM_LIST_Server* object);

typedef version2_CMD_XFER_INITIATE version5_CMD_XFER_INITIATE;

typedef version2_CMD_XFER_DATA version5_CMD_XFER_DATA;

WOWLOGINMESSAGESC_EXPORT WowLoginResult version5_CMD_XFER_ACCEPT_write(WowLoginWriter* writer);

typedef version2_CMD_XFER_RESUME version5_CMD_XFER_RESUME;

WOWLOGINMESSAGESC_EXPORT WowLoginResult version5_CMD_XFER_CANCEL_write(WowLoginWriter* writer);

typedef struct {
    WowLoginOpcode opcode;

    union {
        version5_CMD_AUTH_LOGON_PROOF_Client CMD_AUTH_LOGON_PROOF_Client;
        version2_CMD_AUTH_RECONNECT_PROOF_Client CMD_AUTH_RECONNECT_PROOF_Client;
        version2_CMD_XFER_RESUME CMD_XFER_RESUME;
    } body;
} Version5ClientOpcodeContainer;

WOWLOGINMESSAGESC_EXPORT WowLoginResult version5_client_opcode_read(WowLoginReader* reader, Version5ClientOpcodeContainer* opcodes);

WOWLOGINMESSAGESC_EXPORT void version5_client_opcode_free(Version5ClientOpcodeContainer* opcodes);

typedef struct {
    WowLoginOpcode opcode;

    union {
        version5_CMD_AUTH_LOGON_CHALLENGE_Server CMD_AUTH_LOGON_CHALLENGE_Server;
        version5_CMD_AUTH_LOGON_PROOF_Server CMD_AUTH_LOGON_PROOF_Server;
        version2_CMD_AUTH_RECONNECT_CHALLENGE_Server CMD_AUTH_RECONNECT_CHALLENGE_Server;
        version5_CMD_AUTH_RECONNECT_PROOF_Server CMD_AUTH_RECONNECT_PROOF_Server;
        version5_CMD_REALM_LIST_Server CMD_REALM_LIST_Server;
        version2_CMD_XFER_INITIATE CMD_XFER_INITIATE;
        version2_CMD_XFER_DATA CMD_XFER_DATA;
    } body;
} Version5ServerOpcodeContainer;

WOWLOGINMESSAGESC_EXPORT WowLoginResult version5_server_opcode_read(WowLoginReader* reader, Version5ServerOpcodeContainer* opcodes);

WOWLOGINMESSAGESC_EXPORT void version5_server_opcode_free(Version5ServerOpcodeContainer* opcodes);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* WOW_LOGIN_MESSAGES_VERSION5_H */
