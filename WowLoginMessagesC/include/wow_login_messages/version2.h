#ifndef WOW_LOGIN_MESSAGES_VERSION2_H
#define WOW_LOGIN_MESSAGES_VERSION2_H

#include "wow_login_messages/wow_login_messages.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include "wow_login_messages/all.h" /* type include */

typedef all_Locale version2_Locale;

typedef enum {
    VERSION2_LOGIN_RESULT_SUCCESS = 0,
    VERSION2_LOGIN_RESULT_FAIL_UNKNOWN0 = 1,
    VERSION2_LOGIN_RESULT_FAIL_UNKNOWN1 = 2,
    VERSION2_LOGIN_RESULT_FAIL_BANNED = 3,
    VERSION2_LOGIN_RESULT_FAIL_UNKNOWN_ACCOUNT = 4,
    VERSION2_LOGIN_RESULT_FAIL_INCORRECT_PASSWORD = 5,
    VERSION2_LOGIN_RESULT_FAIL_ALREADY_ONLINE = 6,
    VERSION2_LOGIN_RESULT_FAIL_NO_TIME = 7,
    VERSION2_LOGIN_RESULT_FAIL_DB_BUSY = 8,
    VERSION2_LOGIN_RESULT_FAIL_VERSION_INVALID = 9,
    VERSION2_LOGIN_RESULT_LOGIN_DOWNLOAD_FILE = 10,
    VERSION2_LOGIN_RESULT_FAIL_INVALID_SERVER = 11,
    VERSION2_LOGIN_RESULT_FAIL_SUSPENDED = 12,
    VERSION2_LOGIN_RESULT_FAIL_NO_ACCESS = 13,
    VERSION2_LOGIN_RESULT_SUCCESS_SURVEY = 14,
    VERSION2_LOGIN_RESULT_FAIL_PARENTALCONTROL = 15
} version2_LoginResult;

typedef all_Os version2_Os;

typedef all_Platform version2_Platform;

typedef all_ProtocolVersion version2_ProtocolVersion;

typedef enum {
    VERSION2_REALM_CATEGORY_DEFAULT = 0,
    VERSION2_REALM_CATEGORY_ONE = 1,
    VERSION2_REALM_CATEGORY_TWO = 2,
    VERSION2_REALM_CATEGORY_THREE = 3,
    VERSION2_REALM_CATEGORY_FIVE = 5
} version2_RealmCategory;

typedef enum {
    VERSION2_REALM_TYPE_PLAYER_VS_ENVIRONMENT = 0,
    VERSION2_REALM_TYPE_PLAYER_VS_PLAYER = 1,
    VERSION2_REALM_TYPE_ROLEPLAYING = 6,
    VERSION2_REALM_TYPE_ROLEPLAYING_PLAYER_VS_PLAYER = 8
} version2_RealmType;

typedef enum {
    VERSION2_REALM_FLAG_NONE = 0,
    VERSION2_REALM_FLAG_INVALID = 1,
    VERSION2_REALM_FLAG_OFFLINE = 2,
    VERSION2_REALM_FLAG_FORCE_BLUE_RECOMMENDED = 32,
    VERSION2_REALM_FLAG_FORCE_GREEN_RECOMMENDED = 64,
    VERSION2_REALM_FLAG_FORCE_RED_FULL = 128
} version2_RealmFlag;

typedef struct {
    version2_RealmType realm_type;
    version2_RealmFlag flag;
    WowLoginString name;
    WowLoginString address;
    float population;
    uint8_t number_of_characters_on_realm;
    version2_RealmCategory category;
    uint8_t realm_id;

} version2_Realm;

typedef all_Version version2_Version;

typedef struct {
    uint16_t unknown1;
    uint32_t unknown2;
    uint8_t (*unknown3)[4];
    uint8_t (*cd_key_proof)[20];

} version2_TelemetryKey;

typedef struct {
    version2_LoginResult result;
    uint8_t (*server_public_key)[32];
    uint8_t generator_length;
    uint8_t* generator;
    uint8_t large_safe_prime_length;
    uint8_t* large_safe_prime;
    uint8_t (*salt)[32];
    uint8_t (*crc_salt)[16];

} version2_CMD_AUTH_LOGON_CHALLENGE_Server;
WOWLOGINMESSAGESC_EXPORT WowLoginResult version2_CMD_AUTH_LOGON_CHALLENGE_Server_write(WowLoginWriter* writer, const version2_CMD_AUTH_LOGON_CHALLENGE_Server* object);
WOWLOGINMESSAGESC_EXPORT void version2_CMD_AUTH_LOGON_CHALLENGE_Server_free(version2_CMD_AUTH_LOGON_CHALLENGE_Server* object);

typedef all_CMD_AUTH_LOGON_CHALLENGE_Client version2_CMD_AUTH_LOGON_CHALLENGE_Client;

typedef struct {
    uint8_t (*client_public_key)[32];
    uint8_t (*client_proof)[20];
    uint8_t (*crc_hash)[20];
    uint8_t number_of_telemetry_keys;
    version2_TelemetryKey* telemetry_keys;

} version2_CMD_AUTH_LOGON_PROOF_Client;
WOWLOGINMESSAGESC_EXPORT WowLoginResult version2_CMD_AUTH_LOGON_PROOF_Client_write(WowLoginWriter* writer, const version2_CMD_AUTH_LOGON_PROOF_Client* object);
WOWLOGINMESSAGESC_EXPORT void version2_CMD_AUTH_LOGON_PROOF_Client_free(version2_CMD_AUTH_LOGON_PROOF_Client* object);

typedef struct {
    version2_LoginResult result;
    uint8_t (*server_proof)[20];
    uint32_t hardware_survey_id;

} version2_CMD_AUTH_LOGON_PROOF_Server;
WOWLOGINMESSAGESC_EXPORT WowLoginResult version2_CMD_AUTH_LOGON_PROOF_Server_write(WowLoginWriter* writer, const version2_CMD_AUTH_LOGON_PROOF_Server* object);
WOWLOGINMESSAGESC_EXPORT void version2_CMD_AUTH_LOGON_PROOF_Server_free(version2_CMD_AUTH_LOGON_PROOF_Server* object);

typedef struct {
    version2_LoginResult result;
    uint8_t (*challenge_data)[16];
    uint8_t (*checksum_salt)[16];

} version2_CMD_AUTH_RECONNECT_CHALLENGE_Server;
WOWLOGINMESSAGESC_EXPORT WowLoginResult version2_CMD_AUTH_RECONNECT_CHALLENGE_Server_write(WowLoginWriter* writer, const version2_CMD_AUTH_RECONNECT_CHALLENGE_Server* object);
WOWLOGINMESSAGESC_EXPORT void version2_CMD_AUTH_RECONNECT_CHALLENGE_Server_free(version2_CMD_AUTH_RECONNECT_CHALLENGE_Server* object);

typedef all_CMD_AUTH_RECONNECT_CHALLENGE_Client version2_CMD_AUTH_RECONNECT_CHALLENGE_Client;

typedef struct {
    version2_LoginResult result;

} version2_CMD_AUTH_RECONNECT_PROOF_Server;
WOWLOGINMESSAGESC_EXPORT WowLoginResult version2_CMD_AUTH_RECONNECT_PROOF_Server_write(WowLoginWriter* writer, const version2_CMD_AUTH_RECONNECT_PROOF_Server* object);

typedef struct {
    uint8_t (*proof_data)[16];
    uint8_t (*client_proof)[20];
    uint8_t (*client_checksum)[20];

} version2_CMD_AUTH_RECONNECT_PROOF_Client;
WOWLOGINMESSAGESC_EXPORT WowLoginResult version2_CMD_AUTH_RECONNECT_PROOF_Client_write(WowLoginWriter* writer, const version2_CMD_AUTH_RECONNECT_PROOF_Client* object);
WOWLOGINMESSAGESC_EXPORT void version2_CMD_AUTH_RECONNECT_PROOF_Client_free(version2_CMD_AUTH_RECONNECT_PROOF_Client* object);

typedef struct {
    uint8_t number_of_realms;
    version2_Realm* realms;

} version2_CMD_REALM_LIST_Server;
WOWLOGINMESSAGESC_EXPORT WowLoginResult version2_CMD_REALM_LIST_Server_write(WowLoginWriter* writer, const version2_CMD_REALM_LIST_Server* object);
WOWLOGINMESSAGESC_EXPORT void version2_CMD_REALM_LIST_Server_free(version2_CMD_REALM_LIST_Server* object);

WOWLOGINMESSAGESC_EXPORT WowLoginResult version2_CMD_REALM_LIST_Client_write(WowLoginWriter* writer);

typedef struct {
    WowLoginString filename;
    uint64_t file_size;
    uint8_t (*file_md5)[16];

} version2_CMD_XFER_INITIATE;
WOWLOGINMESSAGESC_EXPORT WowLoginResult version2_CMD_XFER_INITIATE_write(WowLoginWriter* writer, const version2_CMD_XFER_INITIATE* object);
WOWLOGINMESSAGESC_EXPORT void version2_CMD_XFER_INITIATE_free(version2_CMD_XFER_INITIATE* object);

typedef struct {
    uint16_t size;
    uint8_t* data;

} version2_CMD_XFER_DATA;
WOWLOGINMESSAGESC_EXPORT WowLoginResult version2_CMD_XFER_DATA_write(WowLoginWriter* writer, const version2_CMD_XFER_DATA* object);
WOWLOGINMESSAGESC_EXPORT void version2_CMD_XFER_DATA_free(version2_CMD_XFER_DATA* object);

WOWLOGINMESSAGESC_EXPORT WowLoginResult version2_CMD_XFER_ACCEPT_write(WowLoginWriter* writer);

typedef struct {
    uint64_t offset;

} version2_CMD_XFER_RESUME;
WOWLOGINMESSAGESC_EXPORT WowLoginResult version2_CMD_XFER_RESUME_write(WowLoginWriter* writer, const version2_CMD_XFER_RESUME* object);

WOWLOGINMESSAGESC_EXPORT WowLoginResult version2_CMD_XFER_CANCEL_write(WowLoginWriter* writer);

typedef struct {
    WowLoginOpcode opcode;

    union {
        version2_CMD_AUTH_LOGON_PROOF_Client CMD_AUTH_LOGON_PROOF_Client;
        version2_CMD_AUTH_RECONNECT_PROOF_Client CMD_AUTH_RECONNECT_PROOF_Client;
        version2_CMD_XFER_RESUME CMD_XFER_RESUME;
    } body;
} Version2ClientOpcodeContainer;

WOWLOGINMESSAGESC_EXPORT WowLoginResult version2_client_opcode_read(WowLoginReader* reader, Version2ClientOpcodeContainer* opcodes);

WOWLOGINMESSAGESC_EXPORT void version2_client_opcode_free(Version2ClientOpcodeContainer* opcodes);

typedef struct {
    WowLoginOpcode opcode;

    union {
        version2_CMD_AUTH_LOGON_CHALLENGE_Server CMD_AUTH_LOGON_CHALLENGE_Server;
        version2_CMD_AUTH_LOGON_PROOF_Server CMD_AUTH_LOGON_PROOF_Server;
        version2_CMD_AUTH_RECONNECT_CHALLENGE_Server CMD_AUTH_RECONNECT_CHALLENGE_Server;
        version2_CMD_AUTH_RECONNECT_PROOF_Server CMD_AUTH_RECONNECT_PROOF_Server;
        version2_CMD_REALM_LIST_Server CMD_REALM_LIST_Server;
        version2_CMD_XFER_INITIATE CMD_XFER_INITIATE;
        version2_CMD_XFER_DATA CMD_XFER_DATA;
    } body;
} Version2ServerOpcodeContainer;

WOWLOGINMESSAGESC_EXPORT WowLoginResult version2_server_opcode_read(WowLoginReader* reader, Version2ServerOpcodeContainer* opcodes);

WOWLOGINMESSAGESC_EXPORT void version2_server_opcode_free(Version2ServerOpcodeContainer* opcodes);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* WOW_LOGIN_MESSAGES_VERSION2_H */
