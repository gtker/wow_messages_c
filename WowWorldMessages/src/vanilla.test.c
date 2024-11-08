#include "wow_world_messages/vanilla.h"
#include "test_utils.h"

int main(void) {
    /* CMSG_WORLD_TELEPORT */
    #define CMSG_WORLD_TELEPORT0 0, 28, 8, 0, 0, 0, 239, 190, 173, 222, 1, 0, 0, 0, 0, 0, 128, 63, 0, 0, 0, 64, 0, 0, 64, 64, 0, 0, 128, 64, 
    ASSERT_SUCCESS(CMSG_WORLD_TELEPORT, 0, CMSG_WORLD_TELEPORT, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    #define CMSG_WORLD_TELEPORT1 0, 28, 8, 0, 0, 0, 154, 61, 9, 2, 213, 1, 0, 0, 0, 0, 226, 67, 0, 176, 201, 69, 0, 128, 30, 69, 219, 15, 73, 64, 
    ASSERT_SUCCESS(CMSG_WORLD_TELEPORT, 1, CMSG_WORLD_TELEPORT, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* CMSG_TELEPORT_TO_UNIT */
    #define CMSG_TELEPORT_TO_UNIT0 0, 11, 9, 0, 0, 0, 86, 117, 114, 116, 110, 101, 0, 
    ASSERT_SUCCESS(CMSG_TELEPORT_TO_UNIT, 0, CMSG_TELEPORT_TO_UNIT, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* CMSG_CHAR_CREATE */
    #define CMSG_CHAR_CREATE0 0, 22, 54, 0, 0, 0, 68, 101, 97, 100, 98, 101, 101, 102, 0, 1, 1, 1, 8, 0, 14, 2, 4, 0, 
    ASSERT_SUCCESS(CMSG_CHAR_CREATE, 0, CMSG_CHAR_CREATE, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* CMSG_CHAR_ENUM */
    #define CMSG_CHAR_ENUM0 0, 4, 55, 0, 0, 0, 
    ASSERT_SUCCESS_EMPTY(CMSG_CHAR_ENUM, 0, CMSG_CHAR_ENUM, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* CMSG_CHAR_DELETE */
    #define CMSG_CHAR_DELETE0 0, 12, 56, 0, 0, 0, 239, 190, 173, 222, 0, 0, 0, 0, 
    ASSERT_SUCCESS(CMSG_CHAR_DELETE, 0, CMSG_CHAR_DELETE, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* SMSG_CHAR_CREATE */
    #define SMSG_CHAR_CREATE0 0, 3, 58, 0, 47, 
    ASSERT_SUCCESS(SMSG_CHAR_CREATE, 0, SMSG_CHAR_CREATE, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* SMSG_CHAR_ENUM */
    #define SMSG_CHAR_ENUM0 0, 3, 59, 0, 0, 
    ASSERT_SUCCESS(SMSG_CHAR_ENUM, 0, SMSG_CHAR_ENUM, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* SMSG_CHAR_DELETE */
    #define SMSG_CHAR_DELETE0 0, 3, 60, 0, 57, 
    ASSERT_SUCCESS(SMSG_CHAR_DELETE, 0, SMSG_CHAR_DELETE, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* CMSG_PLAYER_LOGIN */
    #define CMSG_PLAYER_LOGIN0 0, 12, 61, 0, 0, 0, 239, 190, 173, 222, 0, 0, 0, 0, 
    ASSERT_SUCCESS(CMSG_PLAYER_LOGIN, 0, CMSG_PLAYER_LOGIN, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* SMSG_NEW_WORLD */
    #define SMSG_NEW_WORLD0 0, 22, 62, 0, 1, 0, 0, 0, 0, 160, 186, 68, 0, 236, 137, 197, 205, 204, 184, 65, 205, 204, 76, 62, 
    ASSERT_SUCCESS(SMSG_NEW_WORLD, 0, SMSG_NEW_WORLD, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* SMSG_CHARACTER_LOGIN_FAILED */
    #define SMSG_CHARACTER_LOGIN_FAILED0 0, 3, 65, 0, 65, 
    ASSERT_SUCCESS(SMSG_CHARACTER_LOGIN_FAILED, 0, SMSG_CHARACTER_LOGIN_FAILED, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* SMSG_LOGIN_SETTIMESPEED */
    #define SMSG_LOGIN_SETTIMESPEED0 0, 10, 66, 0, 10, 50, 115, 22, 137, 136, 136, 60, 
    ASSERT_SUCCESS(SMSG_LOGIN_SETTIMESPEED, 0, SMSG_LOGIN_SETTIMESPEED, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* CMSG_PLAYER_LOGOUT */
    #define CMSG_PLAYER_LOGOUT0 0, 4, 74, 0, 0, 0, 
    ASSERT_SUCCESS_EMPTY(CMSG_PLAYER_LOGOUT, 0, CMSG_PLAYER_LOGOUT, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* CMSG_LOGOUT_REQUEST */
    #define CMSG_LOGOUT_REQUEST0 0, 4, 75, 0, 0, 0, 
    ASSERT_SUCCESS_EMPTY(CMSG_LOGOUT_REQUEST, 0, CMSG_LOGOUT_REQUEST, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* SMSG_LOGOUT_RESPONSE */
    #define SMSG_LOGOUT_RESPONSE0 0, 7, 76, 0, 0, 0, 0, 0, 1, 
    ASSERT_SUCCESS(SMSG_LOGOUT_RESPONSE, 0, SMSG_LOGOUT_RESPONSE, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* SMSG_LOGOUT_COMPLETE */
    #define SMSG_LOGOUT_COMPLETE0 0, 2, 77, 0, 
    ASSERT_SUCCESS_EMPTY(SMSG_LOGOUT_COMPLETE, 0, SMSG_LOGOUT_COMPLETE, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* SMSG_NAME_QUERY_RESPONSE */
    #define SMSG_NAME_QUERY_RESPONSE0 0, 28, 81, 0, 239, 190, 173, 222, 0, 0, 0, 0, 65, 115, 100, 102, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 
    ASSERT_SUCCESS(SMSG_NAME_QUERY_RESPONSE, 0, SMSG_NAME_QUERY_RESPONSE, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    #define SMSG_NAME_QUERY_RESPONSE1 0, 29, 81, 0, 239, 190, 173, 222, 0, 0, 0, 0, 65, 115, 100, 102, 0, 65, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 
    ASSERT_SUCCESS(SMSG_NAME_QUERY_RESPONSE, 1, SMSG_NAME_QUERY_RESPONSE, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* CMSG_PET_NAME_QUERY */
    #define CMSG_PET_NAME_QUERY0 0, 16, 82, 0, 0, 0, 239, 190, 173, 222, 239, 190, 173, 222, 222, 202, 250, 0, 
    ASSERT_SUCCESS(CMSG_PET_NAME_QUERY, 0, CMSG_PET_NAME_QUERY, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* SMSG_PET_NAME_QUERY_RESPONSE */
    #define SMSG_PET_NAME_QUERY_RESPONSE0 0, 17, 83, 0, 239, 190, 173, 222, 65, 66, 67, 68, 69, 70, 0, 222, 202, 250, 0, 
    ASSERT_SUCCESS(SMSG_PET_NAME_QUERY_RESPONSE, 0, SMSG_PET_NAME_QUERY_RESPONSE, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* SMSG_IGNORE_LIST */
    #define SMSG_IGNORE_LIST0 0, 11, 107, 0, 1, 239, 190, 173, 222, 254, 15, 220, 186, 
    ASSERT_SUCCESS(SMSG_IGNORE_LIST, 0, SMSG_IGNORE_LIST, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    #define SMSG_IGNORE_LIST1 0, 19, 107, 0, 2, 239, 190, 173, 222, 254, 15, 220, 186, 239, 190, 173, 222, 0, 0, 0, 0, 
    ASSERT_SUCCESS(SMSG_IGNORE_LIST, 1, SMSG_IGNORE_LIST, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* CMSG_MESSAGECHAT */
    #define CMSG_MESSAGECHAT0 0, 35, 149, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 84, 104, 105, 115, 32, 105, 115, 32, 97, 32, 115, 97, 121, 32, 109, 101, 115, 115, 97, 103, 101, 46, 0, 
    ASSERT_SUCCESS(CMSG_MESSAGECHAT, 0, CMSG_MESSAGECHAT, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* SMSG_MESSAGECHAT */
    #define SMSG_MESSAGECHAT0 0, 51, 150, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 84, 104, 105, 115, 32, 105, 115, 32, 97, 32, 115, 97, 121, 32, 109, 101, 115, 115, 97, 103, 101, 46, 0, 0, 
    ASSERT_SUCCESS(SMSG_MESSAGECHAT, 0, SMSG_MESSAGECHAT, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* CMSG_JOIN_CHANNEL */
    #define CMSG_JOIN_CHANNEL0 0, 29, 151, 0, 0, 0, 71, 101, 110, 101, 114, 97, 108, 32, 45, 32, 69, 108, 119, 121, 110, 110, 32, 70, 111, 114, 101, 115, 116, 0, 0, 
    ASSERT_SUCCESS(CMSG_JOIN_CHANNEL, 0, CMSG_JOIN_CHANNEL, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    #define CMSG_JOIN_CHANNEL1 0, 34, 151, 0, 0, 0, 76, 111, 99, 97, 108, 68, 101, 102, 101, 110, 115, 101, 32, 45, 32, 69, 108, 119, 121, 110, 110, 32, 70, 111, 114, 101, 115, 116, 0, 0, 
    ASSERT_SUCCESS(CMSG_JOIN_CHANNEL, 1, CMSG_JOIN_CHANNEL, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* SMSG_UPDATE_OBJECT */
    #define SMSG_UPDATE_OBJECT0 0, 97, 169, 0, 1, 0, 0, 0, 0, 3, 1, 4, 4, 49, 0, 0, 0, 0, 0, 0, 0, 0, 205, 215, 11, 198, 53, 126, 4, 195, 249, 15, 167, 66, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 63, 0, 0, 224, 64, 0, 0, 144, 64, 0, 0, 0, 0, 0, 0, 0, 0, 219, 15, 73, 64, 1, 0, 0, 0, 2, 7, 0, 64, 0, 16, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 25, 0, 0, 0, 100, 0, 0, 0, 1, 1, 1, 1, 
    ASSERT_SUCCESS(SMSG_UPDATE_OBJECT, 0, SMSG_UPDATE_OBJECT, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    #define SMSG_UPDATE_OBJECT1 0, 133, 169, 0, 1, 0, 0, 0, 0, 3, 1, 4, 4, 49, 0, 0, 0, 0, 0, 0, 0, 0, 205, 215, 11, 198, 53, 126, 4, 195, 249, 15, 167, 66, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 63, 0, 0, 224, 64, 0, 0, 144, 64, 0, 0, 0, 0, 0, 0, 0, 0, 219, 15, 73, 64, 1, 0, 0, 0, 5, 23, 0, 64, 16, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 25, 0, 0, 0, 0, 0, 128, 63, 100, 0, 0, 0, 100, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 50, 0, 0, 0, 50, 0, 0, 0, 
    ASSERT_SUCCESS(SMSG_UPDATE_OBJECT, 1, SMSG_UPDATE_OBJECT, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* SMSG_DESTROY_OBJECT */
    #define SMSG_DESTROY_OBJECT0 0, 10, 170, 0, 6, 0, 0, 0, 0, 0, 0, 0, 
    ASSERT_SUCCESS(SMSG_DESTROY_OBJECT, 0, SMSG_DESTROY_OBJECT, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* CMSG_USE_ITEM */
    #define CMSG_USE_ITEM0 0, 9, 171, 0, 0, 0, 255, 24, 0, 0, 0, 
    ASSERT_SUCCESS(CMSG_USE_ITEM, 0, CMSG_USE_ITEM, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* MSG_MOVE_START_FORWARD_Client */
    #define MSG_MOVE_START_FORWARD_Client0 0, 32, 181, 0, 0, 0, 1, 0, 0, 0, 99, 42, 122, 1, 115, 186, 11, 198, 24, 227, 238, 194, 148, 168, 165, 66, 44, 231, 248, 62, 133, 3, 0, 0, 
    ASSERT_SUCCESS(MSG_MOVE_START_FORWARD_Client, 0, MSG_MOVE_START_FORWARD, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* MSG_MOVE_START_FORWARD_Server */
    #define MSG_MOVE_START_FORWARD_Server0 0, 32, 181, 0, 1, 5, 1, 0, 0, 0, 99, 42, 122, 1, 115, 186, 11, 198, 24, 227, 238, 194, 148, 168, 165, 66, 44, 231, 248, 62, 133, 3, 0, 0, 
    ASSERT_SUCCESS(MSG_MOVE_START_FORWARD_Server, 0, MSG_MOVE_START_FORWARD, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* MSG_MOVE_START_BACKWARD_Client */
    #define MSG_MOVE_START_BACKWARD_Client0 0, 32, 182, 0, 0, 0, 2, 0, 0, 0, 16, 87, 91, 2, 117, 165, 11, 198, 111, 244, 244, 194, 189, 13, 165, 66, 107, 108, 146, 64, 0, 0, 0, 0, 
    ASSERT_SUCCESS(MSG_MOVE_START_BACKWARD_Client, 0, MSG_MOVE_START_BACKWARD, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* MSG_MOVE_START_BACKWARD_Server */
    #define MSG_MOVE_START_BACKWARD_Server0 0, 32, 182, 0, 1, 5, 2, 0, 0, 0, 16, 87, 91, 2, 117, 165, 11, 198, 111, 244, 244, 194, 189, 13, 165, 66, 107, 108, 146, 64, 0, 0, 0, 0, 
    ASSERT_SUCCESS(MSG_MOVE_START_BACKWARD_Server, 0, MSG_MOVE_START_BACKWARD, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* MSG_MOVE_STOP_Client */
    #define MSG_MOVE_STOP_Client0 0, 32, 183, 0, 0, 0, 0, 0, 0, 0, 242, 49, 122, 1, 36, 203, 11, 198, 48, 32, 223, 194, 61, 23, 166, 66, 3, 81, 36, 64, 133, 3, 0, 0, 
    ASSERT_SUCCESS(MSG_MOVE_STOP_Client, 0, MSG_MOVE_STOP, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* MSG_MOVE_STOP_Server */
    #define MSG_MOVE_STOP_Server0 0, 32, 183, 0, 1, 5, 0, 0, 0, 0, 242, 49, 122, 1, 36, 203, 11, 198, 48, 32, 223, 194, 61, 23, 166, 66, 3, 81, 36, 64, 133, 3, 0, 0, 
    ASSERT_SUCCESS(MSG_MOVE_STOP_Server, 0, MSG_MOVE_STOP, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* MSG_MOVE_START_STRAFE_LEFT_Client */
    #define MSG_MOVE_START_STRAFE_LEFT_Client0 0, 32, 184, 0, 0, 0, 5, 0, 0, 0, 159, 210, 121, 1, 238, 193, 11, 198, 253, 68, 8, 195, 36, 126, 167, 66, 184, 157, 194, 62, 127, 3, 0, 0, 
    ASSERT_SUCCESS(MSG_MOVE_START_STRAFE_LEFT_Client, 0, MSG_MOVE_START_STRAFE_LEFT, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* MSG_MOVE_START_STRAFE_LEFT_Server */
    #define MSG_MOVE_START_STRAFE_LEFT_Server0 0, 32, 184, 0, 1, 5, 5, 0, 0, 0, 159, 210, 121, 1, 238, 193, 11, 198, 253, 68, 8, 195, 36, 126, 167, 66, 184, 157, 194, 62, 127, 3, 0, 0, 
    ASSERT_SUCCESS(MSG_MOVE_START_STRAFE_LEFT_Server, 0, MSG_MOVE_START_STRAFE_LEFT, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* MSG_MOVE_START_STRAFE_RIGHT_Client */
    #define MSG_MOVE_START_STRAFE_RIGHT_Client0 0, 32, 185, 0, 0, 0, 9, 0, 0, 0, 159, 210, 121, 1, 238, 193, 11, 198, 253, 68, 8, 195, 36, 126, 167, 66, 184, 157, 194, 62, 127, 3, 0, 0, 
    ASSERT_SUCCESS(MSG_MOVE_START_STRAFE_RIGHT_Client, 0, MSG_MOVE_START_STRAFE_RIGHT, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* MSG_MOVE_START_STRAFE_RIGHT_Server */
    #define MSG_MOVE_START_STRAFE_RIGHT_Server0 0, 32, 185, 0, 1, 5, 9, 0, 0, 0, 159, 210, 121, 1, 238, 193, 11, 198, 253, 68, 8, 195, 36, 126, 167, 66, 184, 157, 194, 62, 127, 3, 0, 0, 
    ASSERT_SUCCESS(MSG_MOVE_START_STRAFE_RIGHT_Server, 0, MSG_MOVE_START_STRAFE_RIGHT, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* MSG_MOVE_STOP_STRAFE_Client */
    #define MSG_MOVE_STOP_STRAFE_Client0 0, 32, 186, 0, 0, 0, 1, 0, 0, 0, 70, 211, 121, 1, 22, 192, 11, 198, 248, 49, 7, 195, 115, 127, 167, 66, 184, 157, 194, 62, 127, 3, 0, 0, 
    ASSERT_SUCCESS(MSG_MOVE_STOP_STRAFE_Client, 0, MSG_MOVE_STOP_STRAFE, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* MSG_MOVE_STOP_STRAFE_Server */
    #define MSG_MOVE_STOP_STRAFE_Server0 0, 32, 186, 0, 1, 5, 1, 0, 0, 0, 70, 211, 121, 1, 22, 192, 11, 198, 248, 49, 7, 195, 115, 127, 167, 66, 184, 157, 194, 62, 127, 3, 0, 0, 
    ASSERT_SUCCESS(MSG_MOVE_STOP_STRAFE_Server, 0, MSG_MOVE_STOP_STRAFE, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* MSG_MOVE_JUMP_Client */
    #define MSG_MOVE_JUMP_Client0 0, 48, 187, 0, 0, 0, 1, 32, 0, 0, 32, 214, 121, 1, 27, 173, 11, 198, 157, 76, 5, 195, 209, 74, 167, 66, 184, 157, 194, 62, 0, 0, 0, 0, 216, 147, 254, 192, 77, 186, 109, 63, 159, 246, 189, 62, 0, 0, 224, 64, 
    ASSERT_SUCCESS(MSG_MOVE_JUMP_Client, 0, MSG_MOVE_JUMP, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* MSG_MOVE_JUMP_Server */
    #define MSG_MOVE_JUMP_Server0 0, 48, 187, 0, 1, 5, 1, 32, 0, 0, 32, 214, 121, 1, 27, 173, 11, 198, 157, 76, 5, 195, 209, 74, 167, 66, 184, 157, 194, 62, 0, 0, 0, 0, 216, 147, 254, 192, 77, 186, 109, 63, 159, 246, 189, 62, 0, 0, 224, 64, 
    ASSERT_SUCCESS(MSG_MOVE_JUMP_Server, 0, MSG_MOVE_JUMP, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* MSG_MOVE_START_TURN_LEFT_Client */
    #define MSG_MOVE_START_TURN_LEFT_Client0 0, 32, 188, 0, 0, 0, 16, 0, 0, 0, 251, 190, 121, 1, 205, 215, 11, 198, 53, 126, 4, 195, 249, 15, 167, 66, 0, 0, 0, 0, 0, 0, 0, 0, 
    ASSERT_SUCCESS(MSG_MOVE_START_TURN_LEFT_Client, 0, MSG_MOVE_START_TURN_LEFT, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* MSG_MOVE_START_TURN_LEFT_Server */
    #define MSG_MOVE_START_TURN_LEFT_Server0 0, 32, 188, 0, 1, 5, 16, 0, 0, 0, 251, 190, 121, 1, 205, 215, 11, 198, 53, 126, 4, 195, 249, 15, 167, 66, 0, 0, 0, 0, 0, 0, 0, 0, 
    ASSERT_SUCCESS(MSG_MOVE_START_TURN_LEFT_Server, 0, MSG_MOVE_START_TURN_LEFT, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* MSG_MOVE_START_TURN_RIGHT_Client */
    #define MSG_MOVE_START_TURN_RIGHT_Client0 0, 32, 189, 0, 0, 0, 32, 0, 0, 0, 251, 190, 121, 1, 205, 215, 11, 198, 53, 126, 4, 195, 249, 15, 167, 66, 0, 0, 0, 0, 0, 0, 0, 0, 
    ASSERT_SUCCESS(MSG_MOVE_START_TURN_RIGHT_Client, 0, MSG_MOVE_START_TURN_RIGHT, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* MSG_MOVE_START_TURN_RIGHT_Server */
    #define MSG_MOVE_START_TURN_RIGHT_Server0 0, 32, 189, 0, 1, 5, 32, 0, 0, 0, 251, 190, 121, 1, 205, 215, 11, 198, 53, 126, 4, 195, 249, 15, 167, 66, 0, 0, 0, 0, 0, 0, 0, 0, 
    ASSERT_SUCCESS(MSG_MOVE_START_TURN_RIGHT_Server, 0, MSG_MOVE_START_TURN_RIGHT, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* MSG_MOVE_STOP_TURN_Client */
    #define MSG_MOVE_STOP_TURN_Client0 0, 32, 190, 0, 0, 0, 0, 0, 0, 0, 151, 166, 91, 2, 37, 162, 11, 198, 57, 130, 248, 194, 222, 72, 165, 66, 16, 19, 156, 64, 0, 0, 0, 0, 
    ASSERT_SUCCESS(MSG_MOVE_STOP_TURN_Client, 0, MSG_MOVE_STOP_TURN, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* MSG_MOVE_STOP_TURN_Server */
    #define MSG_MOVE_STOP_TURN_Server0 0, 32, 190, 0, 1, 5, 0, 0, 0, 0, 151, 166, 91, 2, 37, 162, 11, 198, 57, 130, 248, 194, 222, 72, 165, 66, 16, 19, 156, 64, 0, 0, 0, 0, 
    ASSERT_SUCCESS(MSG_MOVE_STOP_TURN_Server, 0, MSG_MOVE_STOP_TURN, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* MSG_MOVE_SET_RUN_MODE_Client */
    #define MSG_MOVE_SET_RUN_MODE_Client0 0, 32, 194, 0, 0, 0, 1, 0, 0, 0, 65, 27, 91, 2, 85, 185, 11, 198, 248, 132, 1, 195, 173, 49, 167, 66, 46, 14, 195, 64, 0, 0, 0, 0, 
    ASSERT_SUCCESS(MSG_MOVE_SET_RUN_MODE_Client, 0, MSG_MOVE_SET_RUN_MODE, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* MSG_MOVE_SET_RUN_MODE_Server */
    #define MSG_MOVE_SET_RUN_MODE_Server0 0, 32, 194, 0, 1, 5, 1, 0, 0, 0, 65, 27, 91, 2, 85, 185, 11, 198, 248, 132, 1, 195, 173, 49, 167, 66, 46, 14, 195, 64, 0, 0, 0, 0, 
    ASSERT_SUCCESS(MSG_MOVE_SET_RUN_MODE_Server, 0, MSG_MOVE_SET_RUN_MODE, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* MSG_MOVE_SET_WALK_MODE_Client */
    #define MSG_MOVE_SET_WALK_MODE_Client0 0, 32, 195, 0, 0, 0, 1, 1, 0, 0, 154, 23, 91, 2, 2, 189, 11, 198, 78, 88, 1, 195, 38, 41, 167, 66, 46, 14, 195, 64, 0, 0, 0, 0, 
    ASSERT_SUCCESS(MSG_MOVE_SET_WALK_MODE_Client, 0, MSG_MOVE_SET_WALK_MODE, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* MSG_MOVE_SET_WALK_MODE_Server */
    #define MSG_MOVE_SET_WALK_MODE_Server0 0, 32, 195, 0, 1, 5, 1, 1, 0, 0, 154, 23, 91, 2, 2, 189, 11, 198, 78, 88, 1, 195, 38, 41, 167, 66, 46, 14, 195, 64, 0, 0, 0, 0, 
    ASSERT_SUCCESS(MSG_MOVE_SET_WALK_MODE_Server, 0, MSG_MOVE_SET_WALK_MODE, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* MSG_MOVE_TELEPORT_ACK_Client */
    #define MSG_MOVE_TELEPORT_ACK_Client0 0, 13, 199, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 
    ASSERT_SUCCESS(MSG_MOVE_TELEPORT_ACK_Client, 0, MSG_MOVE_TELEPORT_ACK, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* MSG_MOVE_TELEPORT_ACK_Server */
    #define MSG_MOVE_TELEPORT_ACK_Server0 0, 36, 199, 0, 1, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 135, 69, 0, 160, 37, 197, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    ASSERT_SUCCESS(MSG_MOVE_TELEPORT_ACK_Server, 0, MSG_MOVE_TELEPORT_ACK, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* MSG_MOVE_FALL_LAND_Client */
    #define MSG_MOVE_FALL_LAND_Client0 0, 32, 201, 0, 0, 0, 0, 0, 0, 0, 165, 217, 121, 1, 173, 149, 11, 198, 120, 245, 2, 195, 241, 246, 165, 66, 75, 71, 175, 61, 133, 3, 0, 0, 
    ASSERT_SUCCESS(MSG_MOVE_FALL_LAND_Client, 0, MSG_MOVE_FALL_LAND, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* MSG_MOVE_FALL_LAND_Server */
    #define MSG_MOVE_FALL_LAND_Server0 0, 32, 201, 0, 1, 5, 0, 0, 0, 0, 165, 217, 121, 1, 173, 149, 11, 198, 120, 245, 2, 195, 241, 246, 165, 66, 75, 71, 175, 61, 133, 3, 0, 0, 
    ASSERT_SUCCESS(MSG_MOVE_FALL_LAND_Server, 0, MSG_MOVE_FALL_LAND, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* MSG_MOVE_SET_FACING_Client */
    #define MSG_MOVE_SET_FACING_Client0 0, 32, 218, 0, 0, 0, 1, 0, 0, 0, 94, 45, 122, 1, 151, 175, 11, 198, 66, 10, 232, 194, 227, 37, 165, 66, 167, 79, 35, 64, 133, 3, 0, 0, 
    ASSERT_SUCCESS(MSG_MOVE_SET_FACING_Client, 0, MSG_MOVE_SET_FACING, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* MSG_MOVE_SET_FACING_Server */
    #define MSG_MOVE_SET_FACING_Server0 0, 32, 218, 0, 1, 5, 1, 0, 0, 0, 94, 45, 122, 1, 151, 175, 11, 198, 66, 10, 232, 194, 227, 37, 165, 66, 167, 79, 35, 64, 133, 3, 0, 0, 
    ASSERT_SUCCESS(MSG_MOVE_SET_FACING_Server, 0, MSG_MOVE_SET_FACING, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* SMSG_FORCE_RUN_SPEED_CHANGE */
    #define SMSG_FORCE_RUN_SPEED_CHANGE0 0, 12, 226, 0, 1, 6, 0, 0, 0, 0, 0, 0, 224, 64, 
    ASSERT_SUCCESS(SMSG_FORCE_RUN_SPEED_CHANGE, 0, SMSG_FORCE_RUN_SPEED_CHANGE, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* CMSG_FORCE_RUN_SPEED_CHANGE_ACK */
    #define CMSG_FORCE_RUN_SPEED_CHANGE_ACK0 0, 48, 227, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 23, 246, 1, 203, 171, 11, 198, 7, 134, 248, 194, 142, 209, 165, 66, 237, 153, 127, 64, 57, 3, 0, 0, 0, 0, 224, 64, 
    ASSERT_SUCCESS(CMSG_FORCE_RUN_SPEED_CHANGE_ACK, 0, CMSG_FORCE_RUN_SPEED_CHANGE_ACK, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* MSG_MOVE_HEARTBEAT_Client */
    #define MSG_MOVE_HEARTBEAT_Client0 0, 32, 238, 0, 0, 0, 1, 0, 0, 0, 70, 49, 122, 1, 25, 199, 11, 198, 254, 110, 224, 194, 26, 245, 165, 66, 3, 81, 36, 64, 133, 3, 0, 0, 
    ASSERT_SUCCESS(MSG_MOVE_HEARTBEAT_Client, 0, MSG_MOVE_HEARTBEAT, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* MSG_MOVE_HEARTBEAT_Server */
    #define MSG_MOVE_HEARTBEAT_Server0 0, 32, 238, 0, 1, 5, 1, 0, 0, 0, 70, 49, 122, 1, 25, 199, 11, 198, 254, 110, 224, 194, 26, 245, 165, 66, 3, 81, 36, 64, 133, 3, 0, 0, 
    ASSERT_SUCCESS(MSG_MOVE_HEARTBEAT_Server, 0, MSG_MOVE_HEARTBEAT, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* SMSG_TUTORIAL_FLAGS */
    #define SMSG_TUTORIAL_FLAGS0 0, 34, 253, 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
    ASSERT_SUCCESS(SMSG_TUTORIAL_FLAGS, 0, SMSG_TUTORIAL_FLAGS, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* CMSG_STANDSTATECHANGE */
    #define CMSG_STANDSTATECHANGE0 0, 8, 1, 1, 0, 0, 1, 0, 0, 0, 
    ASSERT_SUCCESS(CMSG_STANDSTATECHANGE, 0, CMSG_STANDSTATECHANGE, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* CMSG_TEXT_EMOTE */
    #define CMSG_TEXT_EMOTE0 0, 20, 4, 1, 0, 0, 34, 0, 0, 0, 255, 255, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 
    ASSERT_SUCCESS(CMSG_TEXT_EMOTE, 0, CMSG_TEXT_EMOTE, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* CMSG_AUTOEQUIP_ITEM */
    #define CMSG_AUTOEQUIP_ITEM0 0, 6, 10, 1, 0, 0, 255, 24, 
    ASSERT_SUCCESS(CMSG_AUTOEQUIP_ITEM, 0, CMSG_AUTOEQUIP_ITEM, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* CMSG_INITIATE_TRADE */
    #define CMSG_INITIATE_TRADE0 0, 12, 22, 1, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 
    ASSERT_SUCCESS(CMSG_INITIATE_TRADE, 0, CMSG_INITIATE_TRADE, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* CMSG_CANCEL_TRADE */
    #define CMSG_CANCEL_TRADE0 0, 4, 28, 1, 0, 0, 
    ASSERT_SUCCESS_EMPTY(CMSG_CANCEL_TRADE, 0, CMSG_CANCEL_TRADE, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* SMSG_INITIAL_SPELLS */
    #define SMSG_INITIAL_SPELLS0 0, 167, 42, 1, 0, 40, 0, 78, 0, 0, 0, 81, 0, 0, 0, 107, 0, 0, 0, 196, 0, 0, 0, 198, 0, 0, 0, 201, 0, 0, 0, 203, 0, 0, 0, 204, 0, 0, 0, 10, 2, 0, 0, 156, 2, 0, 0, 78, 9, 0, 0, 153, 9, 0, 0, 175, 9, 0, 0, 234, 11, 0, 0, 37, 13, 0, 0, 181, 20, 0, 0, 89, 24, 0, 0, 102, 24, 0, 0, 103, 24, 0, 0, 77, 25, 0, 0, 78, 25, 0, 0, 203, 25, 0, 0, 98, 28, 0, 0, 99, 28, 0, 0, 187, 28, 0, 0, 194, 32, 0, 0, 33, 34, 0, 0, 117, 35, 0, 0, 118, 35, 0, 0, 156, 35, 0, 0, 165, 35, 0, 0, 117, 80, 0, 0, 118, 80, 0, 0, 119, 80, 0, 0, 120, 80, 0, 0, 128, 81, 0, 0, 147, 84, 0, 0, 148, 84, 0, 0, 11, 86, 0, 0, 26, 89, 0, 0, 0, 0, 
    ASSERT_SUCCESS(SMSG_INITIAL_SPELLS, 0, SMSG_INITIAL_SPELLS, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* CMSG_CAST_SPELL */
    #define CMSG_CAST_SPELL0 0, 10, 46, 1, 0, 0, 120, 80, 0, 0, 0, 0, 
    ASSERT_SUCCESS(CMSG_CAST_SPELL, 0, CMSG_CAST_SPELL, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* CMSG_CANCEL_CAST */
    #define CMSG_CANCEL_CAST0 0, 8, 47, 1, 0, 0, 120, 80, 0, 0, 
    ASSERT_SUCCESS(CMSG_CANCEL_CAST, 0, CMSG_CANCEL_CAST, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    #define CMSG_CANCEL_CAST1 0, 8, 47, 1, 0, 0, 242, 33, 0, 0, 
    ASSERT_SUCCESS(CMSG_CANCEL_CAST, 1, CMSG_CANCEL_CAST, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* CMSG_SET_SELECTION */
    #define CMSG_SET_SELECTION0 0, 12, 61, 1, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 
    ASSERT_SUCCESS(CMSG_SET_SELECTION, 0, CMSG_SET_SELECTION, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* CMSG_ATTACKSWING */
    #define CMSG_ATTACKSWING0 0, 12, 65, 1, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 
    ASSERT_SUCCESS(CMSG_ATTACKSWING, 0, CMSG_ATTACKSWING, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* SMSG_ATTACKSTART */
    #define SMSG_ATTACKSTART0 0, 18, 67, 1, 23, 0, 0, 0, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 
    ASSERT_SUCCESS(SMSG_ATTACKSTART, 0, SMSG_ATTACKSTART, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* SMSG_ATTACKSTOP */
    #define SMSG_ATTACKSTOP0 0, 10, 68, 1, 1, 23, 1, 100, 0, 0, 0, 0, 
    ASSERT_SUCCESS(SMSG_ATTACKSTOP, 0, SMSG_ATTACKSTOP, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* SMSG_ATTACKERSTATEUPDATE */
    #define SMSG_ATTACKERSTATEUPDATE0 0, 51, 74, 1, 128, 0, 0, 0, 1, 23, 1, 100, 57, 5, 0, 0, 1, 0, 0, 0, 0, 0, 128, 166, 68, 52, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    ASSERT_SUCCESS(SMSG_ATTACKERSTATEUPDATE, 0, SMSG_ATTACKERSTATEUPDATE, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* SMSG_ITEM_PUSH_RESULT */
    #define SMSG_ITEM_PUSH_RESULT0 0, 43, 102, 1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 255, 24, 0, 0, 0, 96, 49, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 
    ASSERT_SUCCESS(SMSG_ITEM_PUSH_RESULT, 0, SMSG_ITEM_PUSH_RESULT, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* CMSG_QUERY_TIME */
    #define CMSG_QUERY_TIME0 0, 4, 206, 1, 0, 0, 
    ASSERT_SUCCESS_EMPTY(CMSG_QUERY_TIME, 0, CMSG_QUERY_TIME, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* SMSG_QUERY_TIME_RESPONSE */
    #define SMSG_QUERY_TIME_RESPONSE0 0, 6, 207, 1, 148, 152, 80, 97, 
    ASSERT_SUCCESS(SMSG_QUERY_TIME_RESPONSE, 0, SMSG_QUERY_TIME_RESPONSE, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* CMSG_PING */
    #define CMSG_PING0 0, 12, 220, 1, 0, 0, 239, 190, 173, 222, 222, 202, 250, 0, 
    ASSERT_SUCCESS(CMSG_PING, 0, CMSG_PING, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* SMSG_PONG */
    #define SMSG_PONG0 0, 6, 221, 1, 239, 190, 173, 222, 
    ASSERT_SUCCESS(SMSG_PONG, 0, SMSG_PONG, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* CMSG_SETSHEATHED */
    #define CMSG_SETSHEATHED0 0, 8, 224, 1, 0, 0, 1, 0, 0, 0, 
    ASSERT_SUCCESS(CMSG_SETSHEATHED, 0, CMSG_SETSHEATHED, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* SMSG_AUTH_CHALLENGE */
    #define SMSG_AUTH_CHALLENGE0 0, 6, 236, 1, 239, 190, 173, 222, 
    ASSERT_SUCCESS(SMSG_AUTH_CHALLENGE, 0, SMSG_AUTH_CHALLENGE, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* SMSG_AUTH_RESPONSE */
    #define SMSG_AUTH_RESPONSE0 0, 3, 238, 1, 13, 
    ASSERT_SUCCESS(SMSG_AUTH_RESPONSE, 0, SMSG_AUTH_RESPONSE, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    #define SMSG_AUTH_RESPONSE1 0, 7, 238, 1, 27, 239, 190, 173, 222, 
    ASSERT_SUCCESS(SMSG_AUTH_RESPONSE, 1, SMSG_AUTH_RESPONSE, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    #define SMSG_AUTH_RESPONSE2 0, 12, 238, 1, 12, 239, 190, 173, 222, 0, 0, 0, 0, 0, 
    ASSERT_SUCCESS(SMSG_AUTH_RESPONSE, 2, SMSG_AUTH_RESPONSE, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* CMSG_ZONEUPDATE */
    #define CMSG_ZONEUPDATE0 0, 8, 244, 1, 0, 0, 101, 6, 0, 0, 
    ASSERT_SUCCESS(CMSG_ZONEUPDATE, 0, CMSG_ZONEUPDATE, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    #define CMSG_ZONEUPDATE1 0, 8, 244, 1, 0, 0, 12, 0, 0, 0, 
    ASSERT_SUCCESS(CMSG_ZONEUPDATE, 1, CMSG_ZONEUPDATE, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* SMSG_ACCOUNT_DATA_TIMES */
    #define SMSG_ACCOUNT_DATA_TIMES0 0, 130, 9, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    ASSERT_SUCCESS(SMSG_ACCOUNT_DATA_TIMES, 0, SMSG_ACCOUNT_DATA_TIMES, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* CMSG_REQUEST_ACCOUNT_DATA */
    #define CMSG_REQUEST_ACCOUNT_DATA0 0, 8, 10, 2, 0, 0, 6, 0, 0, 0, 
    ASSERT_SUCCESS(CMSG_REQUEST_ACCOUNT_DATA, 0, CMSG_REQUEST_ACCOUNT_DATA, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* CMSG_GMTICKET_GETTICKET */
    #define CMSG_GMTICKET_GETTICKET0 0, 4, 17, 2, 0, 0, 
    ASSERT_SUCCESS_EMPTY(CMSG_GMTICKET_GETTICKET, 0, CMSG_GMTICKET_GETTICKET, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* SMSG_LOGIN_VERIFY_WORLD */
    #define SMSG_LOGIN_VERIFY_WORLD0 0, 22, 54, 2, 0, 0, 0, 0, 205, 215, 11, 198, 53, 126, 4, 195, 249, 15, 167, 66, 0, 0, 0, 0, 
    ASSERT_SUCCESS(SMSG_LOGIN_VERIFY_WORLD, 0, SMSG_LOGIN_VERIFY_WORLD, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* CMSG_LEARN_TALENT */
    #define CMSG_LEARN_TALENT0 0, 12, 81, 2, 0, 0, 158, 0, 0, 0, 0, 0, 0, 0, 
    ASSERT_SUCCESS(CMSG_LEARN_TALENT, 0, CMSG_LEARN_TALENT, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* MSG_AUCTION_HELLO_Client */
    #define MSG_AUCTION_HELLO_Client0 0, 12, 85, 2, 0, 0, 239, 190, 173, 222, 0, 0, 0, 0, 
    ASSERT_SUCCESS(MSG_AUCTION_HELLO_Client, 0, MSG_AUCTION_HELLO, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* CMSG_SET_ACTIVE_MOVER */
    #define CMSG_SET_ACTIVE_MOVER0 0, 12, 106, 2, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 
    ASSERT_SUCCESS(CMSG_SET_ACTIVE_MOVER, 0, CMSG_SET_ACTIVE_MOVER, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* MSG_QUERY_NEXT_MAIL_TIME_Client */
    #define MSG_QUERY_NEXT_MAIL_TIME_Client0 0, 4, 132, 2, 0, 0, 
    ASSERT_SUCCESS_EMPTY(MSG_QUERY_NEXT_MAIL_TIME_Client, 0, MSG_QUERY_NEXT_MAIL_TIME, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* CMSG_MEETINGSTONE_INFO */
    #define CMSG_MEETINGSTONE_INFO0 0, 4, 150, 2, 0, 0, 
    ASSERT_SUCCESS_EMPTY(CMSG_MEETINGSTONE_INFO, 0, CMSG_MEETINGSTONE_INFO, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* CMSG_CHAR_RENAME */
    #define CMSG_CHAR_RENAME0 0, 21, 199, 2, 0, 0, 239, 190, 173, 222, 0, 0, 0, 0, 68, 101, 97, 100, 98, 101, 101, 102, 0, 
    ASSERT_SUCCESS(CMSG_CHAR_RENAME, 0, CMSG_CHAR_RENAME, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* SMSG_CHAR_RENAME */
    #define SMSG_CHAR_RENAME0 0, 3, 200, 2, 71, 
    ASSERT_SUCCESS(SMSG_CHAR_RENAME, 0, SMSG_CHAR_RENAME, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    #define SMSG_CHAR_RENAME1 0, 20, 200, 2, 0, 239, 190, 173, 222, 0, 0, 0, 0, 68, 101, 97, 100, 98, 101, 101, 102, 0, 
    ASSERT_SUCCESS(SMSG_CHAR_RENAME, 1, SMSG_CHAR_RENAME, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    /* CMSG_REQUEST_RAID_INFO */
    #define CMSG_REQUEST_RAID_INFO0 0, 4, 205, 2, 0, 0, 
    ASSERT_SUCCESS_EMPTY(CMSG_REQUEST_RAID_INFO, 0, CMSG_REQUEST_RAID_INFO, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* CMSG_MOVE_TIME_SKIPPED */
    #define CMSG_MOVE_TIME_SKIPPED0 0, 16, 206, 2, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 
    ASSERT_SUCCESS(CMSG_MOVE_TIME_SKIPPED, 0, CMSG_MOVE_TIME_SKIPPED, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* CMSG_BATTLEFIELD_STATUS */
    #define CMSG_BATTLEFIELD_STATUS0 0, 4, 211, 2, 0, 0, 
    ASSERT_SUCCESS_EMPTY(CMSG_BATTLEFIELD_STATUS, 0, CMSG_BATTLEFIELD_STATUS, VanillaClientOpcodeContainer, vanilla, client, World, wwm);

    /* SMSG_SPLINE_SET_RUN_SPEED */
    #define SMSG_SPLINE_SET_RUN_SPEED0 0, 8, 254, 2, 1, 6, 0, 0, 224, 64, 
    ASSERT_SUCCESS(SMSG_SPLINE_SET_RUN_SPEED, 0, SMSG_SPLINE_SET_RUN_SPEED, VanillaServerOpcodeContainer, vanilla, server, World, wwm);

    return 0;
}
