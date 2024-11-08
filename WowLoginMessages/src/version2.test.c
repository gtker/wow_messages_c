#include "wow_login_messages/version2.h"
#include "test_utils.h"

int main(void) {
    /* CMD_AUTH_LOGON_CHALLENGE_Server */
    #define CMD_AUTH_LOGON_CHALLENGE_Server0 0, 0, 0, 73, 216, 194, 188, 104, 92, 43, 206, 74, 244, 250, 7, 10, 71, 147, 120, 88, 120, 70, 181, 131, 212, 65, 130, 158, 36, 216, 135, 206, 218, 52, 70, 1, 7, 32, 183, 155, 62, 42, 135, 130, 60, 171, 143, 94, 191, 191, 142, 177, 1, 8, 83, 80, 6, 41, 139, 91, 173, 189, 91, 83, 225, 137, 94, 100, 75, 137, 199, 9, 135, 125, 140, 101, 82, 102, 165, 125, 184, 101, 61, 110, 166, 43, 181, 84, 242, 11, 207, 116, 214, 74, 119, 167, 211, 61, 243, 48, 144, 135, 186, 163, 30, 153, 160, 11, 33, 87, 252, 55, 63, 179, 105, 205, 210, 241, 
    ASSERT_SUCCESS(CMD_AUTH_LOGON_CHALLENGE_Server, 0, CMD_AUTH_LOGON_CHALLENGE, Version2ServerOpcodeContainer, version2, server, Login, wlm);

    /* CMD_AUTH_LOGON_PROOF_Client */
    #define CMD_AUTH_LOGON_PROOF_Client0 1, 241, 62, 229, 209, 131, 196, 200, 169, 80, 14, 63, 90, 93, 138, 238, 78, 46, 69, 225, 247, 204, 143, 28, 245, 238, 142, 17, 206, 211, 29, 215, 8, 107, 30, 72, 27, 77, 4, 161, 24, 216, 242, 222, 92, 89, 213, 92, 129, 46, 101, 236, 62, 78, 245, 45, 225, 128, 94, 26, 103, 21, 236, 200, 65, 238, 184, 144, 138, 88, 187, 0, 208, 2, 255, 0, 239, 190, 173, 222, 1, 2, 3, 4, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 254, 0, 238, 190, 173, 222, 0, 1, 2, 3, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 
    ASSERT_SUCCESS(CMD_AUTH_LOGON_PROOF_Client, 0, CMD_AUTH_LOGON_PROOF, Version2ClientOpcodeContainer, version2, client, Login, wlm);

    #define CMD_AUTH_LOGON_PROOF_Client1 1, 241, 62, 229, 209, 131, 196, 200, 169, 80, 14, 63, 90, 93, 138, 238, 78, 46, 69, 225, 247, 204, 143, 28, 245, 238, 142, 17, 206, 211, 29, 215, 8, 107, 30, 72, 27, 77, 4, 161, 24, 216, 242, 222, 92, 89, 213, 92, 129, 46, 101, 236, 62, 78, 245, 45, 225, 128, 94, 26, 103, 21, 236, 200, 65, 238, 184, 144, 138, 88, 187, 0, 208, 1, 255, 0, 239, 190, 173, 222, 1, 2, 3, 4, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 
    ASSERT_SUCCESS(CMD_AUTH_LOGON_PROOF_Client, 1, CMD_AUTH_LOGON_PROOF, Version2ClientOpcodeContainer, version2, client, Login, wlm);

    #define CMD_AUTH_LOGON_PROOF_Client2 1, 241, 62, 229, 209, 131, 196, 200, 169, 80, 14, 63, 90, 93, 138, 238, 78, 46, 69, 225, 247, 204, 143, 28, 245, 238, 142, 17, 206, 211, 29, 215, 8, 107, 30, 72, 27, 77, 4, 161, 24, 216, 242, 222, 92, 89, 213, 92, 129, 46, 101, 236, 62, 78, 245, 45, 225, 128, 94, 26, 103, 21, 236, 200, 65, 238, 184, 144, 138, 88, 187, 0, 208, 0, 
    ASSERT_SUCCESS(CMD_AUTH_LOGON_PROOF_Client, 2, CMD_AUTH_LOGON_PROOF, Version2ClientOpcodeContainer, version2, client, Login, wlm);

    /* CMD_AUTH_LOGON_PROOF_Server */
    #define CMD_AUTH_LOGON_PROOF_Server0 1, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 239, 190, 173, 222, 
    ASSERT_SUCCESS(CMD_AUTH_LOGON_PROOF_Server, 0, CMD_AUTH_LOGON_PROOF, Version2ServerOpcodeContainer, version2, server, Login, wlm);

    /* CMD_AUTH_RECONNECT_CHALLENGE_Server */
    #define CMD_AUTH_RECONNECT_CHALLENGE_Server0 2, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 255, 254, 253, 252, 251, 250, 249, 248, 247, 246, 245, 244, 243, 242, 241, 240, 
    ASSERT_SUCCESS(CMD_AUTH_RECONNECT_CHALLENGE_Server, 0, CMD_AUTH_RECONNECT_CHALLENGE, Version2ServerOpcodeContainer, version2, server, Login, wlm);

    #define CMD_AUTH_RECONNECT_CHALLENGE_Server1 2, 3, 
    ASSERT_SUCCESS(CMD_AUTH_RECONNECT_CHALLENGE_Server, 1, CMD_AUTH_RECONNECT_CHALLENGE, Version2ServerOpcodeContainer, version2, server, Login, wlm);

    /* CMD_AUTH_RECONNECT_PROOF_Server */
    #define CMD_AUTH_RECONNECT_PROOF_Server0 3, 0, 
    ASSERT_SUCCESS(CMD_AUTH_RECONNECT_PROOF_Server, 0, CMD_AUTH_RECONNECT_PROOF, Version2ServerOpcodeContainer, version2, server, Login, wlm);

    #define CMD_AUTH_RECONNECT_PROOF_Server1 3, 14, 
    ASSERT_SUCCESS(CMD_AUTH_RECONNECT_PROOF_Server, 1, CMD_AUTH_RECONNECT_PROOF, Version2ServerOpcodeContainer, version2, server, Login, wlm);

    #define CMD_AUTH_RECONNECT_PROOF_Server2 3, 14, 
    ASSERT_SUCCESS(CMD_AUTH_RECONNECT_PROOF_Server, 2, CMD_AUTH_RECONNECT_PROOF, Version2ServerOpcodeContainer, version2, server, Login, wlm);

    /* CMD_AUTH_RECONNECT_PROOF_Client */
    #define CMD_AUTH_RECONNECT_PROOF_Client0 3, 234, 250, 185, 198, 24, 21, 11, 242, 249, 50, 206, 39, 98, 121, 150, 153, 107, 109, 26, 13, 243, 165, 158, 106, 56, 2, 231, 11, 225, 47, 5, 113, 186, 71, 140, 163, 40, 167, 158, 154, 36, 40, 230, 130, 237, 236, 199, 201, 232, 110, 241, 59, 123, 225, 224, 245, 0, 
    ASSERT_SUCCESS(CMD_AUTH_RECONNECT_PROOF_Client, 0, CMD_AUTH_RECONNECT_PROOF, Version2ClientOpcodeContainer, version2, client, Login, wlm);

    /* CMD_REALM_LIST_Server */
    #define CMD_REALM_LIST_Server0 16, 23, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 65, 0, 65, 0, 0, 0, 200, 67, 1, 0, 2, 0, 0, 
    ASSERT_SUCCESS(CMD_REALM_LIST_Server, 0, CMD_REALM_LIST, Version2ServerOpcodeContainer, version2, server, Login, wlm);

    #define CMD_REALM_LIST_Server1 16, 23, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 65, 0, 65, 0, 0, 0, 200, 67, 1, 0, 2, 0, 0, 
    ASSERT_SUCCESS(CMD_REALM_LIST_Server, 1, CMD_REALM_LIST, Version2ServerOpcodeContainer, version2, server, Login, wlm);

    /* CMD_REALM_LIST_Client */
    #define CMD_REALM_LIST_Client0 16, 0, 0, 0, 0, 
    ASSERT_SUCCESS_EMPTY(CMD_REALM_LIST_Client, 0, CMD_REALM_LIST, Version2ClientOpcodeContainer, version2, client, Login, wlm);

    /* CMD_XFER_INITIATE */
    #define CMD_XFER_INITIATE0 48, 5, 80, 97, 116, 99, 104, 188, 9, 0, 0, 0, 0, 0, 0, 17, 91, 85, 89, 127, 183, 223, 14, 134, 217, 179, 174, 90, 235, 203, 98, 
    ASSERT_SUCCESS(CMD_XFER_INITIATE, 0, CMD_XFER_INITIATE, Version2ServerOpcodeContainer, version2, server, Login, wlm);

    /* CMD_XFER_DATA */
    #define CMD_XFER_DATA0 49, 64, 0, 77, 80, 81, 26, 44, 0, 0, 0, 60, 224, 38, 0, 1, 0, 3, 0, 252, 217, 38, 0, 252, 221, 38, 0, 64, 0, 0, 0, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 120, 218, 236, 125, 123, 124, 84, 197, 245, 248, 110, 246, 134, 92, 116, 37, 11, 174, 184, 
    ASSERT_SUCCESS(CMD_XFER_DATA, 0, CMD_XFER_DATA, Version2ServerOpcodeContainer, version2, server, Login, wlm);

    /* CMD_XFER_ACCEPT */
    #define CMD_XFER_ACCEPT0 50, 
    ASSERT_SUCCESS_EMPTY(CMD_XFER_ACCEPT, 0, CMD_XFER_ACCEPT, Version2ClientOpcodeContainer, version2, client, Login, wlm);

    /* CMD_XFER_RESUME */
    #define CMD_XFER_RESUME0 51, 64, 0, 0, 0, 0, 0, 0, 0, 
    ASSERT_SUCCESS(CMD_XFER_RESUME, 0, CMD_XFER_RESUME, Version2ClientOpcodeContainer, version2, client, Login, wlm);

    /* CMD_XFER_CANCEL */
    #define CMD_XFER_CANCEL0 52, 
    ASSERT_SUCCESS_EMPTY(CMD_XFER_CANCEL, 0, CMD_XFER_CANCEL, Version2ClientOpcodeContainer, version2, client, Login, wlm);

    return 0;
}
