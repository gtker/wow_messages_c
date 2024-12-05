#include "server_socket.h"

#include "wow_login_messages/all.h"
#include "wow_login_messages/version3.h"
#include "wow_srp/server.h"
#include "wow_srp/wow_srp.h"

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
    version3_server_write_opcode(writer, opcodes);
    server_send(sock, buffer, writer->index + 1);
}

static version3_Realm realms = {VERSION2_REALM_TYPE_PLAYER_VS_ENVIRONMENT,
                                VERSION2_REALM_FLAG_NONE,
                                {sizeof("Test Name"), "Test Name"},
                                {sizeof("localhost"), "localhost"},
                                0.0f,
                                2,
                                VERSION2_REALM_CATEGORY_DEFAULT,
                                1};

int main(void)
{
    int ret = 0;

    ServerSocket sock = initialize_socket("3724");

    size_t bytes = server_recv(&sock, buffer, BUFFER_SIZE);

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
    server_send(&sock, buffer, writer.index);

    Version3ClientOpcodeContainer v3_opcodes;
    expect_version3_opcode(&reader, &sock, &v3_opcodes, CMD_AUTH_LOGON_PROOF);

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

    puts("server is not null");
    Version3ServerOpcodeContainer s;
    s.opcode = CMD_AUTH_LOGON_PROOF;
    s.body.CMD_AUTH_LOGON_PROOF_Server.result = VERSION2_LOGIN_RESULT_SUCCESS;
    s.body.CMD_AUTH_LOGON_PROOF_Server.server_proof = (uint8_t(*)[20])server_proof;
    s.body.CMD_AUTH_LOGON_PROOF_Server.hardware_survey_id = 0;

    write_version3_opcode(&writer, &sock, &s);
    puts("Sent logon proof");

    expect_version3_opcode(&reader, &sock, &v3_opcodes, CMD_REALM_LIST);
    puts("Received realm list");

    s.body.CMD_REALM_LIST_Server.number_of_realms = 1;
    s.body.CMD_REALM_LIST_Server.realms = &realms;

    write_version3_opcode(&writer, &sock, &s);
    puts("sent realm list");

    while (true)
    {
    }

exit_goto:
    free_socket(&sock);
    all_client_opcode_free(&opcodes);
    return ret;
}
