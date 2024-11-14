#if defined(_MSC_VER)
#define WIN32_LEAN_AND_MEAN
#include <basetsd.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SSIZE_T ssize_t;
typedef int socklen_t;
typedef SOCKET fd_type;
typedef char socket_opt_type;
typedef int socket_length_type;

#define close closesocket
#define WINDOWS_INT_CAST (int)

#else
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

typedef int fd_type;
typedef int socket_opt_type;
typedef int socket_length_type;
#define WINDOWS_INT_CAST
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "wow_login_messages/all.h"
#include "wow_login_messages/version3.h"
#include "wow_srp/server.h"
#include "wow_srp/wow_srp.h"

#define LISTEN_BACKLOG 10

static uint8_t GENERATOR = {WOW_SRP_GENERATOR};

int main(void)
{
    struct addrinfo hints = {0};
    struct addrinfo* res = NULL;
    fd_type sockfd = 0;
    fd_type result = 0;
    fd_type clientfd = 0;
    struct sockaddr_storage storage = {0};
    socklen_t socklen = sizeof(storage);
    int ret = 0;

#if defined(_MSC_VER)
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        fprintf(stderr, "WSAStartup failed.\n");
        exit(1);
    }

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        fprintf(stderr, "Versiion 2.2 of Winsock is not available.\n");
        WSACleanup();
        exit(2);
    }
#endif

    hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;  // fill in my IP for me

    result = getaddrinfo(NULL, "3724", &hints, &res);
    if (result == -1)
    {
        puts("getaddrinfo returned failure.");
        ret = 1;
        goto exit_goto;
    }

    puts("Binding to 3724");

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    // bind it to the port we passed in to getaddrinfo():

    socket_opt_type one = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

    result = bind(sockfd, res->ai_addr, WINDOWS_INT_CAST res->ai_addrlen);
    if (result == -1)
    {
        puts("bind failed.");
        ret = 1;
        goto exit_goto;
    }

    result = listen(sockfd, LISTEN_BACKLOG);
    if (result == -1)
    {
        puts("listen failed.");
        ret = 1;
        goto exit_goto;
    }

    clientfd = accept(sockfd, (struct sockaddr*)&storage, &socklen);
    if (clientfd == -1)
    {
        puts("accept failed");
        exit(EXIT_FAILURE);
    }

    unsigned char buf[512] = {0};
    ssize_t bytes = recv(clientfd, (char*)buf, 512, 0);
    if (bytes == -1)
    {
        puts("recv failed");
        ret = 1;
        goto exit_goto;
    }

    AllClientOpcodeContainer opcodes;
    WowLoginReader reader = wlm_create_reader(buf, bytes);

    WowLoginResult r = all_client_opcode_read(&reader, &opcodes);
    if (r != WLM_RESULT_SUCCESS)
    {
        puts("client opcode read failed");
        ret = 1;
        goto exit_goto;
    }

    if (opcodes.opcode != CMD_AUTH_LOGON_CHALLENGE)
    {
        puts("incorrect opcode");
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

    WowLoginWriter writer = wlm_create_writer(buf, 512);

    version3_CMD_AUTH_LOGON_CHALLENGE_Server_write(&writer, &c);
    send(clientfd, (char*)buf, WINDOWS_INT_CAST writer.index, 0);

    bytes = recv(clientfd, (char*)buf, 512, 0);
    if (bytes == -1)
    {
        puts("recv failed");
        ret = 1;
        goto exit_goto;
    }

    reader.index = 0;
    reader.length = bytes;

    Version3ClientOpcodeContainer v3_opcodes;
    r = version3_client_opcode_read(&reader, &v3_opcodes);
    if (r != WLM_RESULT_SUCCESS)
    {
        puts("failed to read opcode v3");
        ret = 1;
        goto exit_goto;
    }

    if (v3_opcodes.opcode != CMD_AUTH_LOGON_PROOF)
    {
        printf("incorrect opcode received %u", v3_opcodes.opcode);
        ret = 1;
        goto exit_goto;
    }

    puts("last one 1");


    uint8_t server_proof[WOW_SRP_PROOF_LENGTH];
    WowSrpServer* server = wow_srp_proof_into_server(
        proof, (uint8_t*)v3_opcodes.body.CMD_AUTH_LOGON_PROOF_Client.client_public_key,
        (uint8_t*)v3_opcodes.body.CMD_AUTH_LOGON_PROOF_Client.client_proof, server_proof, &error);

    puts("last one 1");
    if (server == NULL)
    {
        printf("server is NULL");
        ret = 1;
        goto exit_goto;
    }

    puts("last one 1");
    version3_CMD_AUTH_LOGON_PROOF_Server s;
    s.result = VERSION2_LOGIN_RESULT_SUCCESS;
    s.server_proof = (uint8_t(*)[20])server_proof;
    s.hardware_survey_id = 0;

    writer.index = 0;

    version2_CMD_AUTH_LOGON_PROOF_Server_write(&writer, &s);
    send(clientfd, (char*)buf, WINDOWS_INT_CAST writer.index, 0);
    puts("last one 1");

    bytes = recv(clientfd, (char*)buf, 512, 0);
    if (bytes == -1)
    {
        puts("recv failed");
        ret = 1;
        goto exit_goto;
    }
    puts("last one 1");

    for (int i = 0; i < bytes; ++i)
    {
        printf("%02x ", buf[i]);
    }
    puts("last one 1");

    while (true)
    {
    }

exit_goto:
    all_client_opcode_free(&opcodes);
    close(sockfd);
    close(clientfd);
    return ret;
}
