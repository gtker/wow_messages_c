#include "wow_login_messages/version6.h"
#include "test_utils.h"

int main(void) {
    /* CMD_REALM_LIST_Server */
    #define CMD_REALM_LIST_Server0 16, 81, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 84, 101, 115, 116, 32, 82, 101, 97, 108, 109, 50, 0, 108, 111, 99, 97, 108, 104, 111, 115, 116, 58, 56, 48, 56, 53, 0, 0, 0, 72, 67, 3, 1, 1, 0, 0, 0, 84, 101, 115, 116, 32, 82, 101, 97, 108, 109, 0, 108, 111, 99, 97, 108, 104, 111, 115, 116, 58, 56, 48, 56, 53, 0, 0, 0, 72, 67, 3, 2, 0, 0, 0, 
    ASSERT_SUCCESS(CMD_REALM_LIST_Server, 0, CMD_REALM_LIST, Version6ServerOpcodeContainer, version6, server, Login, wlm);

    return 0;
}
