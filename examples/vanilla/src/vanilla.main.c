#include "server_socket.h"

#include "wow_login_messages/all.h"
#include "wow_login_messages/version3.h"

#include "wow_srp/server.h"
#include "wow_srp/vanilla.h"
#include "wow_srp/wow_srp.h"

#include "wow_world_messages/vanilla.h"

static uint8_t GENERATOR = {WOW_SRP_GENERATOR};

#define BUFFER_SIZE 4096
static unsigned char buffer[BUFFER_SIZE];

void expect_version3_opcode(WowLoginReader* reader,
                            ServerSocket* sock,
                            Version3ClientOpcodeContainer* opcodes,
                            WowLoginOpcode opcode)
{
    reader->index = 0;
    reader->length = server_recv(sock, buffer, BUFFER_SIZE);
    WowLoginResult result = version3_client_opcode_read(reader, opcodes);

    if (result != WLM_RESULT_SUCCESS)
    {
        puts("failed to read opcodes v3");
        abort();
    }

    if (opcodes->opcode != opcode)
    {
        printf("incorrect opcodes received, expected %d, received %d", opcode, opcodes->opcode);
        abort();
    }
}

void write_version3_opcode(WowLoginWriter* writer, ServerSocket* sock, Version3ServerOpcodeContainer* opcodes)
{
    writer->index = 0;
    version3_server_opcode_write(writer, opcodes);
    server_send(sock, buffer, writer->index);
}

void write_unencrypted_opcode(WowWorldWriter* writer, ServerSocket* sock, VanillaServerOpcodeContainer* opcodes)
{
    writer->index = 0;
    vanilla_server_opcode_write(writer, opcodes);
    server_send(sock, buffer, writer->index);
}

void write_opcode(WowWorldWriter* writer,
                  ServerSocket* sock,
                  VanillaServerOpcodeContainer* opcodes,
                  WowSrpVanillaHeaderCrypto* header_crypto)
{
    char error = 0;
    writer->index = 0;
    vanilla_server_opcode_write(writer, opcodes);
    wow_srp_vanilla_header_crypto_encrypt(header_crypto, writer->destination, WOW_SRP_VANILLA_SERVER_HEADER_LENGTH,
                                          &error);
    server_send(sock, buffer, writer->index);
}

void read_opcode(WowWorldReader* reader,
                 ServerSocket* sock,
                 VanillaClientOpcodeContainer* opcodes,
                 WowSrpVanillaHeaderCrypto* header_crypto)
{
    char error = 0;
    reader->index = 0;
    reader->length = server_recv(sock, buffer, BUFFER_SIZE);
    wow_srp_vanilla_header_crypto_decrypt(header_crypto, (uint8_t*)reader->source, WOW_SRP_CLIENT_HEADER_LENGTH,
                                          &error);
    WowWorldResult result = vanilla_client_opcode_read(reader, opcodes);

    if (result != WWM_RESULT_SUCCESS)
    {
        puts("failed to read opcodes vanilla");
        abort();
    }
}

void expect_opcode(WowWorldReader* reader,
                   ServerSocket* sock,
                   VanillaClientOpcodeContainer* opcodes,
                   WowVanillaWorldOpcode opcode,
                   WowSrpVanillaHeaderCrypto* header_crypto)
{
    read_opcode(reader, sock, opcodes, header_crypto);

    if (opcodes->opcode != opcode)
    {
        printf("incorrect opcodes received, expected %d, received %d", opcode, opcodes->opcode);
        abort();
    }
}


void expect_unencrypted_opcode(WowWorldReader* reader,
                               ServerSocket* sock,
                               VanillaClientOpcodeContainer* opcodes,
                               WowVanillaWorldOpcode opcode)
{
    reader->index = 0;
    reader->length = server_recv(sock, buffer, BUFFER_SIZE);
    WowWorldResult result = vanilla_client_opcode_read(reader, opcodes);

    if (result != WWM_RESULT_SUCCESS)
    {
        puts("failed to read opcodes vanilla");
        abort();
    }

    if (opcodes->opcode != opcode)
    {
        printf("incorrect opcodes received, expected %d, received %d", opcode, opcodes->opcode);
        abort();
    }
}

static version3_Realm realms = {VERSION2_REALM_TYPE_PLAYER_VS_ENVIRONMENT,
                                VERSION2_REALM_FLAG_NONE,
                                {sizeof("Test Name") - 1, "Test Name"},
                                {sizeof("localhost:8085") - 1, "localhost:8085"},
                                0.0f,
                                2,
                                VERSION2_REALM_CATEGORY_DEFAULT,
                                1};
static vanilla_CharacterGear gear[19] = {0};
static vanilla_Character characters = {
    1,
    {sizeof("TestChar") - 1, "TestChar"},
    VANILLA_RACE_HUMAN,
    VANILLA_CLASS_WARRIOR,
    VANILLA_GENDER_MALE,
    0,
    0,
    0,
    0,
    0,
    1,
    VANILLA_AREA_NORTHSHIRE_ABBEY,
    VANILLA_MAP_EASTERN_KINGDOMS,
    {0.0f, 0.0f, 0.0f},
    0,
    VANILLA_CHARACTER_FLAGS_NONE,
    false,
    0,
    0,
    0,
    &gear,
};

int main(void)
{
    int ret = 0;

    ServerSocket auth = initialize_socket("3724");

    server_accept(&auth);

    size_t bytes = server_recv(&auth, buffer, BUFFER_SIZE);

    AllClientOpcodeContainer opcodes;
    WowLoginReader reader = wlm_create_reader(buffer, bytes);

    WowLoginResult r = all_client_opcode_read(&reader, &opcodes);
    if (r != WLM_RESULT_SUCCESS && opcodes.opcode != CMD_AUTH_LOGON_CHALLENGE)
    {
        puts("client opcode read failed");
        ret = 1;
        goto exit_goto;
    }

    if (opcodes.body.CMD_AUTH_LOGON_CHALLENGE_Client.protocol_version != ALL_PROTOCOL_VERSION_THREE)
    {
        puts("incorrect protocol version");
        ret = 1;
        goto exit_goto;
    }

    char error;
    WowSrpVerifier* verifier = wow_srp_verifier_from_username_and_password("A", "A", &error);
    if (verifier == NULL)
    {
        puts("verifier is NULL");
        ret = 1;
        goto exit_goto;
    }
    WowSrpProof* proof = wow_srp_verifier_into_proof(verifier);
    verifier = NULL;
    if (proof == NULL)
    {
        puts("proof is NULL");
        ret = 1;
        goto exit_goto;
    }

    version3_CMD_AUTH_LOGON_CHALLENGE_Server c;
    c.result = VERSION2_LOGIN_RESULT_SUCCESS;
    c.server_public_key = (uint8_t(*)[32])wow_srp_proof_server_public_key(proof);
    c.generator_length = 1;
    c.generator = &GENERATOR;
    c.large_safe_prime_length = WOW_SRP_KEY_LENGTH;
    c.large_safe_prime = (uint8_t*)WOW_SRP_LARGE_SAFE_PRIME_LITTLE_ENDIAN;
    c.salt = (uint8_t(*)[32])wow_srp_proof_salt(proof);
    uint8_t crc_salt[16] = {0};
    c.crc_salt = &crc_salt;
    c.security_flag = VERSION3_SECURITY_FLAG_NONE;

    WowLoginWriter writer = wlm_create_writer(buffer, BUFFER_SIZE);

    version3_CMD_AUTH_LOGON_CHALLENGE_Server_write(&writer, &c);
    server_send(&auth, buffer, writer.index);

    Version3ClientOpcodeContainer v3_opcodes;
    expect_version3_opcode(&reader, &auth, &v3_opcodes, CMD_AUTH_LOGON_PROOF);

    puts("received logon proof");

    uint8_t server_proof[WOW_SRP_PROOF_LENGTH];
    WowSrpServer* server = wow_srp_proof_into_server(
        proof, (uint8_t*)v3_opcodes.body.CMD_AUTH_LOGON_PROOF_Client.client_public_key,
        (uint8_t*)v3_opcodes.body.CMD_AUTH_LOGON_PROOF_Client.client_proof, server_proof, &error);

    if (server == NULL)
    {
        printf("server is NULL");
        ret = 1;
        goto exit_goto;
    }
    unsigned char session_key[WOW_SRP_SESSION_KEY_LENGTH];
    const uint8_t* sk = wow_srp_server_session_key(server);
    memcpy(&session_key, sk, WOW_SRP_SESSION_KEY_LENGTH);

    puts("server is not null");
    Version3ServerOpcodeContainer s;
    s.opcode = CMD_AUTH_LOGON_PROOF;
    s.body.CMD_AUTH_LOGON_PROOF_Server.result = VERSION2_LOGIN_RESULT_SUCCESS;
    s.body.CMD_AUTH_LOGON_PROOF_Server.server_proof = (uint8_t(*)[20])server_proof;
    s.body.CMD_AUTH_LOGON_PROOF_Server.hardware_survey_id = 0;

    write_version3_opcode(&writer, &auth, &s);
    puts("Sent logon proof");

    expect_version3_opcode(&reader, &auth, &v3_opcodes, CMD_REALM_LIST);
    puts("Received realm list");

    ServerSocket world = initialize_socket("8085");

    s.opcode = CMD_REALM_LIST;
    s.body.CMD_REALM_LIST_Server.number_of_realms = 1;
    s.body.CMD_REALM_LIST_Server.realms = &realms;
    write_version3_opcode(&writer, &auth, &s);

    puts("sent realm list");

    server_accept(&world);
    puts("accepted world");

    WowSrpVanillaProofSeed* seed = wow_srp_vanilla_proof_seed_new();
    char err = 0;

    VanillaServerOpcodeContainer server_opcode;
    server_opcode.opcode = SMSG_AUTH_CHALLENGE;
    server_opcode.body.SMSG_AUTH_CHALLENGE.server_seed = wow_srp_vanilla_proof_seed(seed, &err);

    WowWorldWriter world_writer = wwm_create_writer(buffer, BUFFER_SIZE);

    write_unencrypted_opcode(&world_writer, &world, &server_opcode);

    WowWorldReader world_reader = wwm_create_reader(buffer, BUFFER_SIZE);
    VanillaClientOpcodeContainer client_opcode;
    expect_unencrypted_opcode(&world_reader, &world, &client_opcode, CMSG_AUTH_SESSION);
    printf("Accepted %s\n", client_opcode.body.CMSG_AUTH_SESSION.username.string);

    WowSrpVanillaHeaderCrypto* header_crypto = wow_srp_vanilla_proof_seed_into_server_header_crypto(
        seed, client_opcode.body.CMSG_AUTH_SESSION.username.string, sk,
        (uint8_t*)client_opcode.body.CMSG_AUTH_SESSION.client_proof, client_opcode.body.CMSG_AUTH_SESSION.client_seed,
        &err);

    if (header_crypto == NULL)
    {
        printf("header_crypto is NULL");
        ret = 1;
        goto exit_goto;
    }

    server_opcode.opcode = SMSG_AUTH_RESPONSE;
    server_opcode.body.SMSG_AUTH_RESPONSE.result = VANILLA_WORLD_RESULT_AUTH_OK;

    write_opcode(&world_writer, &world, &server_opcode, header_crypto);

    expect_opcode(&world_reader, &world, &client_opcode, CMSG_CHAR_ENUM, header_crypto);

    server_opcode.opcode = SMSG_CHAR_ENUM;
    server_opcode.body.SMSG_CHAR_ENUM.amount_of_characters = 1;
    server_opcode.body.SMSG_CHAR_ENUM.characters = &characters;

    write_opcode(&world_writer, &world, &server_opcode, header_crypto);

    while (client_opcode.opcode != CMSG_PLAYER_LOGIN)
    {
        read_opcode(&world_reader, &world, &client_opcode, header_crypto);

        printf("Received %s\n", vanilla_client_opcode_to_str(&client_opcode));

        if (client_opcode.opcode == CMSG_CHAR_ENUM)
        {
            server_opcode.opcode = SMSG_CHAR_ENUM;
            server_opcode.body.SMSG_CHAR_ENUM.amount_of_characters = 1;
            server_opcode.body.SMSG_CHAR_ENUM.characters = &characters;

            write_opcode(&world_writer, &world, &server_opcode, header_crypto);
        }
    }

    printf("Logging into %lu\n", client_opcode.body.CMSG_PLAYER_LOGIN.guid);

    server_opcode.opcode = SMSG_LOGIN_VERIFY_WORLD;
    server_opcode.body.SMSG_LOGIN_VERIFY_WORLD.map = VANILLA_MAP_EASTERN_KINGDOMS;
    server_opcode.body.SMSG_LOGIN_VERIFY_WORLD.orientation = 0.0f;
    server_opcode.body.SMSG_LOGIN_VERIFY_WORLD.position = (all_Vector3d){
        -8949.95,
        -132.493,
        83.5312,
    };
    write_opcode(&world_writer, &world, &server_opcode, header_crypto);

    uint32_t tutorial_data[8] = {
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    };

    server_opcode.opcode = SMSG_TUTORIAL_FLAGS;
    server_opcode.body.SMSG_TUTORIAL_FLAGS.tutorial_data = &tutorial_data;
    write_opcode(&world_writer, &world, &server_opcode, header_crypto);

    server_opcode.opcode = SMSG_UPDATE_OBJECT;
    server_opcode.body.SMSG_UPDATE_OBJECT.has_transport = 0;
    server_opcode.body.SMSG_UPDATE_OBJECT.amount_of_objects = 1;
    vanilla_Object objects = {0};
    server_opcode.body.SMSG_UPDATE_OBJECT.objects = &objects;

    objects.update_type = VANILLA_UPDATE_TYPE_CREATE_OBJECT2;
    objects.guid3 = 1;
    objects.object_type = VANILLA_OBJECT_TYPE_PLAYER;
    objects.movement2.update_flag = VANILLA_UPDATE_FLAG_SELF | VANILLA_UPDATE_FLAG_ALL | VANILLA_UPDATE_FLAG_LIVING;
    objects.movement2.flags = VANILLA_MOVEMENT_FLAGS_NONE;
    objects.movement2.timestamp = 0;
    objects.movement2.living_position = (all_Vector3d){
        -8949.95,
        -132.493,
        83.5312,
    };
    objects.movement2.living_orientation = 0.0;
    objects.movement2.fall_time = 0.0;
    objects.movement2.walking_speed = 1.0;
    objects.movement2.running_speed = 70.0;
    objects.movement2.backwards_running_speed = 4.5;
    objects.movement2.swimming_speed = 0.0;
    objects.movement2.backwards_swimming_speed = 0.0;
    objects.movement2.turn_rate = 3.1415;
    objects.movement2.unknown1 = 0;

    vanilla_update_mask_set(&objects.mask2, VANILLA_OBJECT_GUID, 1);
    vanilla_update_mask_set(&objects.mask2, VANILLA_OBJECT_GUID + 1, 0);

    vanilla_update_mask_set(&objects.mask2, VANILLA_OBJECT_SCALE_X, 1.0f);
    vanilla_update_mask_set(&objects.mask2, VANILLA_OBJECT_TYPE, 25);
    vanilla_update_mask_set(&objects.mask2, VANILLA_UNIT_BYTES_0, 0x01010101);
    vanilla_update_mask_set(&objects.mask2, VANILLA_UNIT_DISPLAYID, 50);
    vanilla_update_mask_set(&objects.mask2, VANILLA_UNIT_FACTIONTEMPLATE, 1);
    vanilla_update_mask_set(&objects.mask2, VANILLA_UNIT_HEALTH, 100);
    vanilla_update_mask_set(&objects.mask2, VANILLA_UNIT_LEVEL, 1);
    vanilla_update_mask_set(&objects.mask2, VANILLA_UNIT_NATIVEDISPLAYID, 50);

    write_opcode(&world_writer, &world, &server_opcode, header_crypto);

    while (true)
    {
    }

exit_goto:
    free_socket(&auth);
    all_client_opcode_free(&opcodes);
    return ret;
}
