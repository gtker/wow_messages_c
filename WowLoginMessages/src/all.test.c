#include "wow_login_messages/all.h"
#include "test_utils.h"

int main(void) {
    /* CMD_AUTH_LOGON_CHALLENGE_Client */
    #define CMD_AUTH_LOGON_CHALLENGE_Client0 0, 3, 31, 0, 87, 111, 87, 0, 1, 12, 1, 243, 22, 54, 56, 120, 0, 110, 105, 87, 0, 66, 71, 110, 101, 60, 0, 0, 0, 127, 0, 0, 1, 1, 65, 
    ASSERT_SUCCESS(CMD_AUTH_LOGON_CHALLENGE_Client, 0, CMD_AUTH_LOGON_CHALLENGE, AllClientOpcodeContainer, all, client, Login, wlm);

    /* CMD_AUTH_RECONNECT_CHALLENGE_Client */
    #define CMD_AUTH_RECONNECT_CHALLENGE_Client0 2, 2, 31, 0, 87, 111, 87, 0, 1, 12, 1, 243, 22, 54, 56, 120, 0, 110, 105, 87, 0, 66, 71, 110, 101, 60, 0, 0, 0, 127, 0, 0, 1, 1, 65, 
    ASSERT_SUCCESS(CMD_AUTH_RECONNECT_CHALLENGE_Client, 0, CMD_AUTH_RECONNECT_CHALLENGE, AllClientOpcodeContainer, all, client, Login, wlm);

    #define CMD_AUTH_RECONNECT_CHALLENGE_Client1 2, 2, 46, 0, 87, 111, 87, 0, 1, 12, 1, 243, 22, 54, 56, 120, 0, 110, 105, 87, 0, 66, 71, 110, 101, 60, 0, 0, 0, 127, 0, 0, 1, 16, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 
    ASSERT_SUCCESS(CMD_AUTH_RECONNECT_CHALLENGE_Client, 1, CMD_AUTH_RECONNECT_CHALLENGE, AllClientOpcodeContainer, all, client, Login, wlm);

    return 0;
}
