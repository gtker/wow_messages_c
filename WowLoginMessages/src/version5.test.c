#include "wow_login_messages/version5.h"
#include "test_utils.h"

int main(void) {
    /* CMD_AUTH_LOGON_CHALLENGE_Server */
    #define CMD_AUTH_LOGON_CHALLENGE_Server0 0, 0, 0, 58, 43, 237, 162, 169, 101, 37, 78, 69, 4, 195, 168, 246, 106, 134, 201, 81, 114, 215, 99, 107, 54, 137, 237, 192, 63, 252, 193, 66, 165, 121, 50, 1, 7, 32, 183, 155, 62, 42, 135, 130, 60, 171, 143, 94, 191, 191, 142, 177, 1, 8, 83, 80, 6, 41, 139, 91, 173, 189, 91, 83, 225, 137, 94, 100, 75, 137, 174, 120, 124, 96, 218, 20, 21, 219, 130, 36, 67, 72, 71, 108, 63, 211, 188, 22, 60, 89, 21, 128, 86, 5, 146, 59, 82, 46, 114, 18, 41, 82, 70, 15, 184, 237, 114, 71, 169, 255, 31, 242, 228, 96, 253, 255, 127, 249, 3, 0, 0, 0, 0, 89, 29, 166, 11, 52, 253, 100, 94, 56, 108, 84, 192, 24, 182, 167, 47, 8, 8, 2, 1, 194, 216, 23, 56, 5, 251, 84, 143, 
    ASSERT_SUCCESS(CMD_AUTH_LOGON_CHALLENGE_Server, 0, CMD_AUTH_LOGON_CHALLENGE, Version5ServerOpcodeContainer, version5, server, Login, wlm);

    /* CMD_AUTH_LOGON_PROOF_Client */
    #define CMD_AUTH_LOGON_PROOF_Client0 1, 4, 73, 87, 221, 32, 81, 98, 245, 250, 254, 179, 103, 7, 114, 9, 81, 86, 32, 8, 8, 32, 193, 38, 202, 200, 247, 59, 70, 251, 136, 50, 6, 130, 201, 151, 96, 66, 228, 117, 249, 124, 96, 98, 228, 84, 102, 166, 254, 220, 233, 170, 124, 254, 116, 218, 112, 136, 204, 118, 36, 196, 40, 136, 181, 239, 196, 29, 180, 107, 197, 44, 251, 0, 3, 221, 105, 240, 247, 88, 76, 88, 240, 134, 54, 58, 26, 190, 110, 30, 77, 90, 78, 192, 86, 88, 136, 230, 41, 1, 108, 191, 61, 247, 142, 130, 147, 111, 29, 190, 229, 105, 52, 205, 8, 130, 148, 239, 93, 15, 150, 159, 252, 23, 11, 228, 66, 8, 46, 209, 16, 
    ASSERT_SUCCESS(CMD_AUTH_LOGON_PROOF_Client, 0, CMD_AUTH_LOGON_PROOF, Version5ClientOpcodeContainer, version5, client, Login, wlm);

    /* CMD_AUTH_LOGON_PROOF_Server */
    #define CMD_AUTH_LOGON_PROOF_Server0 1, 0, 25, 255, 233, 250, 100, 169, 155, 175, 246, 179, 141, 156, 17, 171, 220, 174, 128, 196, 210, 231, 0, 0, 0, 0, 0, 0, 
    ASSERT_SUCCESS(CMD_AUTH_LOGON_PROOF_Server, 0, CMD_AUTH_LOGON_PROOF, Version5ServerOpcodeContainer, version5, server, Login, wlm);

    /* CMD_AUTH_RECONNECT_PROOF_Server */
    #define CMD_AUTH_RECONNECT_PROOF_Server0 3, 0, 0, 0, 
    ASSERT_SUCCESS(CMD_AUTH_RECONNECT_PROOF_Server, 0, CMD_AUTH_RECONNECT_PROOF, Version5ServerOpcodeContainer, version5, server, Login, wlm);

    /* CMD_REALM_LIST_Server */
    #define CMD_REALM_LIST_Server0 16, 80, 0, 0, 0, 0, 0, 2, 0, 0, 0, 84, 101, 115, 116, 32, 82, 101, 97, 108, 109, 50, 0, 108, 111, 99, 97, 108, 104, 111, 115, 116, 58, 56, 48, 56, 53, 0, 0, 0, 72, 67, 3, 1, 1, 0, 0, 0, 84, 101, 115, 116, 32, 82, 101, 97, 108, 109, 0, 108, 111, 99, 97, 108, 104, 111, 115, 116, 58, 56, 48, 56, 53, 0, 0, 0, 72, 67, 3, 2, 0, 0, 0, 
    ASSERT_SUCCESS(CMD_REALM_LIST_Server, 0, CMD_REALM_LIST, Version5ServerOpcodeContainer, version5, server, Login, wlm);

    return 0;
}
