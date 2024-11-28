#include "wow_world_messages_cpp/vanilla.hpp"
#include "test_utils.h"

class ByteReader final : public wow_world_messages::Reader
{
public:
    explicit ByteReader(std::vector<unsigned char> buf) : m_buf(std::move(buf)) { }

    uint8_t read_u8() override
    {
        const uint8_t value = m_buf[m_index];
        m_index += 1;

        return value;
    }

    std::vector<unsigned char> m_buf;
    size_t m_index = 0;
};

int main(void) {
    /* CMSG_WORLD_TELEPORT */ {
        const std::vector<unsigned char> buffer = {0, 28, 8, 0, 0, 0, 239, 190, 173, 222, 1, 0, 0, 0, 0, 0, 128, 63, 0, 0, 0, 64, 0, 0, 64, 64, 0, 0, 128, 64, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_WORLD_TELEPORT 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_WORLD_TELEPORT) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_WORLD_TELEPORT 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_WORLD_TELEPORT 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* CMSG_WORLD_TELEPORT */ {
        const std::vector<unsigned char> buffer = {0, 28, 8, 0, 0, 0, 154, 61, 9, 2, 213, 1, 0, 0, 0, 0, 226, 67, 0, 176, 201, 69, 0, 128, 30, 69, 219, 15, 73, 64, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_WORLD_TELEPORT 1 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_WORLD_TELEPORT) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_WORLD_TELEPORT 1 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_WORLD_TELEPORT 1", TEST_UTILS_SIDE_CLIENT);
    }
    /* CMSG_TELEPORT_TO_UNIT */ {
        const std::vector<unsigned char> buffer = {0, 11, 9, 0, 0, 0, 86, 117, 114, 116, 110, 101, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_TELEPORT_TO_UNIT 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_TELEPORT_TO_UNIT) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_TELEPORT_TO_UNIT 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_TELEPORT_TO_UNIT 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* CMSG_CHAR_CREATE */ {
        const std::vector<unsigned char> buffer = {0, 22, 54, 0, 0, 0, 68, 101, 97, 100, 98, 101, 101, 102, 0, 1, 1, 1, 8, 0, 14, 2, 4, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_CHAR_CREATE 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_CHAR_CREATE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_CHAR_CREATE 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_CHAR_CREATE 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* CMSG_CHAR_ENUM */ {
        const std::vector<unsigned char> buffer = {0, 4, 55, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_CHAR_ENUM 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_CHAR_ENUM) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_CHAR_ENUM 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_CHAR_ENUM 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* CMSG_CHAR_DELETE */ {
        const std::vector<unsigned char> buffer = {0, 12, 56, 0, 0, 0, 239, 190, 173, 222, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_CHAR_DELETE 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_CHAR_DELETE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_CHAR_DELETE 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_CHAR_DELETE 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* SMSG_CHAR_CREATE */ {
        const std::vector<unsigned char> buffer = {0, 3, 58, 0, 47, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_CHAR_CREATE 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_CHAR_CREATE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_CHAR_CREATE 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_CHAR_CREATE 0", TEST_UTILS_SIDE_SERVER);
    }
    /* SMSG_CHAR_ENUM */ {
        const std::vector<unsigned char> buffer = {0, 3, 59, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_CHAR_ENUM 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_CHAR_ENUM) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_CHAR_ENUM 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_CHAR_ENUM 0", TEST_UTILS_SIDE_SERVER);
    }
    /* SMSG_CHAR_DELETE */ {
        const std::vector<unsigned char> buffer = {0, 3, 60, 0, 57, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_CHAR_DELETE 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_CHAR_DELETE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_CHAR_DELETE 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_CHAR_DELETE 0", TEST_UTILS_SIDE_SERVER);
    }
    /* CMSG_PLAYER_LOGIN */ {
        const std::vector<unsigned char> buffer = {0, 12, 61, 0, 0, 0, 239, 190, 173, 222, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_PLAYER_LOGIN 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_PLAYER_LOGIN) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_PLAYER_LOGIN 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_PLAYER_LOGIN 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* SMSG_NEW_WORLD */ {
        const std::vector<unsigned char> buffer = {0, 22, 62, 0, 1, 0, 0, 0, 0, 160, 186, 68, 0, 236, 137, 197, 205, 204, 184, 65, 205, 204, 76, 62, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_NEW_WORLD 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_NEW_WORLD) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_NEW_WORLD 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_NEW_WORLD 0", TEST_UTILS_SIDE_SERVER);
    }
    /* SMSG_TRANSFER_PENDING */ {
        const std::vector<unsigned char> buffer = {0, 6, 63, 0, 1, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_TRANSFER_PENDING 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_TRANSFER_PENDING) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_TRANSFER_PENDING 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_TRANSFER_PENDING 0", TEST_UTILS_SIDE_SERVER);
    }
    /* SMSG_CHARACTER_LOGIN_FAILED */ {
        const std::vector<unsigned char> buffer = {0, 3, 65, 0, 65, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_CHARACTER_LOGIN_FAILED 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_CHARACTER_LOGIN_FAILED) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_CHARACTER_LOGIN_FAILED 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_CHARACTER_LOGIN_FAILED 0", TEST_UTILS_SIDE_SERVER);
    }
    /* SMSG_LOGIN_SETTIMESPEED */ {
        const std::vector<unsigned char> buffer = {0, 10, 66, 0, 10, 50, 115, 22, 137, 136, 136, 60, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_LOGIN_SETTIMESPEED 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_LOGIN_SETTIMESPEED) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_LOGIN_SETTIMESPEED 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_LOGIN_SETTIMESPEED 0", TEST_UTILS_SIDE_SERVER);
    }
    /* CMSG_PLAYER_LOGOUT */ {
        const std::vector<unsigned char> buffer = {0, 4, 74, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_PLAYER_LOGOUT 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_PLAYER_LOGOUT) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_PLAYER_LOGOUT 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_PLAYER_LOGOUT 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* CMSG_LOGOUT_REQUEST */ {
        const std::vector<unsigned char> buffer = {0, 4, 75, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_LOGOUT_REQUEST 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_LOGOUT_REQUEST) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_LOGOUT_REQUEST 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_LOGOUT_REQUEST 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* SMSG_LOGOUT_RESPONSE */ {
        const std::vector<unsigned char> buffer = {0, 7, 76, 0, 0, 0, 0, 0, 1, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_LOGOUT_RESPONSE 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_LOGOUT_RESPONSE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_LOGOUT_RESPONSE 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_LOGOUT_RESPONSE 0", TEST_UTILS_SIDE_SERVER);
    }
    /* SMSG_LOGOUT_COMPLETE */ {
        const std::vector<unsigned char> buffer = {0, 2, 77, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_LOGOUT_COMPLETE 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_LOGOUT_COMPLETE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_LOGOUT_COMPLETE 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_LOGOUT_COMPLETE 0", TEST_UTILS_SIDE_SERVER);
    }
    /* SMSG_NAME_QUERY_RESPONSE */ {
        const std::vector<unsigned char> buffer = {0, 28, 81, 0, 239, 190, 173, 222, 0, 0, 0, 0, 65, 115, 100, 102, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_NAME_QUERY_RESPONSE 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_NAME_QUERY_RESPONSE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_NAME_QUERY_RESPONSE 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_NAME_QUERY_RESPONSE 0", TEST_UTILS_SIDE_SERVER);
    }
    /* SMSG_NAME_QUERY_RESPONSE */ {
        const std::vector<unsigned char> buffer = {0, 29, 81, 0, 239, 190, 173, 222, 0, 0, 0, 0, 65, 115, 100, 102, 0, 65, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_NAME_QUERY_RESPONSE 1 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_NAME_QUERY_RESPONSE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_NAME_QUERY_RESPONSE 1 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_NAME_QUERY_RESPONSE 1", TEST_UTILS_SIDE_SERVER);
    }
    /* CMSG_PET_NAME_QUERY */ {
        const std::vector<unsigned char> buffer = {0, 16, 82, 0, 0, 0, 239, 190, 173, 222, 239, 190, 173, 222, 222, 202, 250, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_PET_NAME_QUERY 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_PET_NAME_QUERY) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_PET_NAME_QUERY 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_PET_NAME_QUERY 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* SMSG_PET_NAME_QUERY_RESPONSE */ {
        const std::vector<unsigned char> buffer = {0, 17, 83, 0, 239, 190, 173, 222, 65, 66, 67, 68, 69, 70, 0, 222, 202, 250, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_PET_NAME_QUERY_RESPONSE 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_PET_NAME_QUERY_RESPONSE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_PET_NAME_QUERY_RESPONSE 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_PET_NAME_QUERY_RESPONSE 0", TEST_UTILS_SIDE_SERVER);
    }
    /* SMSG_ITEM_QUERY_SINGLE_RESPONSE */ {
        const std::vector<unsigned char> buffer = {1, 224, 88, 0, 62, 28, 0, 0, 2, 0, 0, 0, 5, 0, 0, 0, 83, 109, 105, 116, 101, 39, 115, 32, 77, 105, 103, 104, 116, 121, 32, 72, 97, 109, 109, 101, 114, 0, 0, 0, 0, 154, 76, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 155, 60, 0, 0, 31, 12, 0, 0, 17, 0, 0, 0, 223, 5, 0, 0, 255, 1, 0, 0, 23, 0, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 11, 0, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 92, 66, 0, 0, 166, 66, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 172, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_ITEM_QUERY_SINGLE_RESPONSE 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_ITEM_QUERY_SINGLE_RESPONSE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_ITEM_QUERY_SINGLE_RESPONSE 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_ITEM_QUERY_SINGLE_RESPONSE 0", TEST_UTILS_SIDE_SERVER);
    }
    /* SMSG_CREATURE_QUERY_RESPONSE */ {
        const std::vector<unsigned char> buffer = {0, 46, 97, 0, 69, 0, 0, 0, 84, 104, 105, 110, 103, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_CREATURE_QUERY_RESPONSE 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_CREATURE_QUERY_RESPONSE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_CREATURE_QUERY_RESPONSE 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_CREATURE_QUERY_RESPONSE 0", TEST_UTILS_SIDE_SERVER);
    }
    /* SMSG_IGNORE_LIST */ {
        const std::vector<unsigned char> buffer = {0, 11, 107, 0, 1, 239, 190, 173, 222, 254, 15, 220, 186, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_IGNORE_LIST 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_IGNORE_LIST) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_IGNORE_LIST 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_IGNORE_LIST 0", TEST_UTILS_SIDE_SERVER);
    }
    /* SMSG_IGNORE_LIST */ {
        const std::vector<unsigned char> buffer = {0, 19, 107, 0, 2, 239, 190, 173, 222, 254, 15, 220, 186, 239, 190, 173, 222, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_IGNORE_LIST 1 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_IGNORE_LIST) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_IGNORE_LIST 1 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_IGNORE_LIST 1", TEST_UTILS_SIDE_SERVER);
    }
    /* CMSG_MESSAGECHAT */ {
        const std::vector<unsigned char> buffer = {0, 35, 149, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 84, 104, 105, 115, 32, 105, 115, 32, 97, 32, 115, 97, 121, 32, 109, 101, 115, 115, 97, 103, 101, 46, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_MESSAGECHAT 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_MESSAGECHAT) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_MESSAGECHAT 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_MESSAGECHAT 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* SMSG_MESSAGECHAT */ {
        const std::vector<unsigned char> buffer = {0, 51, 150, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 84, 104, 105, 115, 32, 105, 115, 32, 97, 32, 115, 97, 121, 32, 109, 101, 115, 115, 97, 103, 101, 46, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_MESSAGECHAT 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_MESSAGECHAT) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_MESSAGECHAT 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_MESSAGECHAT 0", TEST_UTILS_SIDE_SERVER);
    }
    /* CMSG_JOIN_CHANNEL */ {
        const std::vector<unsigned char> buffer = {0, 29, 151, 0, 0, 0, 71, 101, 110, 101, 114, 97, 108, 32, 45, 32, 69, 108, 119, 121, 110, 110, 32, 70, 111, 114, 101, 115, 116, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_JOIN_CHANNEL 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_JOIN_CHANNEL) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_JOIN_CHANNEL 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_JOIN_CHANNEL 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* CMSG_JOIN_CHANNEL */ {
        const std::vector<unsigned char> buffer = {0, 34, 151, 0, 0, 0, 76, 111, 99, 97, 108, 68, 101, 102, 101, 110, 115, 101, 32, 45, 32, 69, 108, 119, 121, 110, 110, 32, 70, 111, 114, 101, 115, 116, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_JOIN_CHANNEL 1 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_JOIN_CHANNEL) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_JOIN_CHANNEL 1 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_JOIN_CHANNEL 1", TEST_UTILS_SIDE_CLIENT);
    }
    /* SMSG_UPDATE_OBJECT */ {
        const std::vector<unsigned char> buffer = {0, 97, 169, 0, 1, 0, 0, 0, 0, 3, 1, 4, 4, 49, 0, 0, 0, 0, 0, 0, 0, 0, 205, 215, 11, 198, 53, 126, 4, 195, 249, 15, 167, 66, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 63, 0, 0, 224, 64, 0, 0, 144, 64, 0, 0, 0, 0, 0, 0, 0, 0, 219, 15, 73, 64, 1, 0, 0, 0, 2, 7, 0, 64, 0, 16, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 25, 0, 0, 0, 100, 0, 0, 0, 1, 1, 1, 1, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_UPDATE_OBJECT 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_UPDATE_OBJECT) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_UPDATE_OBJECT 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_UPDATE_OBJECT 0", TEST_UTILS_SIDE_SERVER);
    }
    /* SMSG_UPDATE_OBJECT */ {
        const std::vector<unsigned char> buffer = {0, 133, 169, 0, 1, 0, 0, 0, 0, 3, 1, 4, 4, 49, 0, 0, 0, 0, 0, 0, 0, 0, 205, 215, 11, 198, 53, 126, 4, 195, 249, 15, 167, 66, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 63, 0, 0, 224, 64, 0, 0, 144, 64, 0, 0, 0, 0, 0, 0, 0, 0, 219, 15, 73, 64, 1, 0, 0, 0, 5, 23, 0, 64, 16, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 25, 0, 0, 0, 0, 0, 128, 63, 100, 0, 0, 0, 100, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 50, 0, 0, 0, 50, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_UPDATE_OBJECT 1 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_UPDATE_OBJECT) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_UPDATE_OBJECT 1 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_UPDATE_OBJECT 1", TEST_UTILS_SIDE_SERVER);
    }
    /* SMSG_DESTROY_OBJECT */ {
        const std::vector<unsigned char> buffer = {0, 10, 170, 0, 6, 0, 0, 0, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_DESTROY_OBJECT 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_DESTROY_OBJECT) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_DESTROY_OBJECT 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_DESTROY_OBJECT 0", TEST_UTILS_SIDE_SERVER);
    }
    /* CMSG_USE_ITEM */ {
        const std::vector<unsigned char> buffer = {0, 9, 171, 0, 0, 0, 255, 24, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_USE_ITEM 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_USE_ITEM) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_USE_ITEM 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_USE_ITEM 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* MSG_MOVE_START_FORWARD_Client */ {
        const std::vector<unsigned char> buffer = {0, 32, 181, 0, 0, 0, 1, 0, 0, 0, 99, 42, 122, 1, 115, 186, 11, 198, 24, 227, 238, 194, 148, 168, 165, 66, 44, 231, 248, 62, 133, 3, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_FORWARD_Client 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::MSG_MOVE_START_FORWARD) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_FORWARD_Client 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_FORWARD_Client 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* MSG_MOVE_START_FORWARD_Server */ {
        const std::vector<unsigned char> buffer = {0, 32, 181, 0, 1, 5, 1, 0, 0, 0, 99, 42, 122, 1, 115, 186, 11, 198, 24, 227, 238, 194, 148, 168, 165, 66, 44, 231, 248, 62, 133, 3, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_FORWARD_Server 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::MSG_MOVE_START_FORWARD) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_FORWARD_Server 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_FORWARD_Server 0", TEST_UTILS_SIDE_SERVER);
    }
    /* MSG_MOVE_START_BACKWARD_Client */ {
        const std::vector<unsigned char> buffer = {0, 32, 182, 0, 0, 0, 2, 0, 0, 0, 16, 87, 91, 2, 117, 165, 11, 198, 111, 244, 244, 194, 189, 13, 165, 66, 107, 108, 146, 64, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_BACKWARD_Client 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::MSG_MOVE_START_BACKWARD) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_BACKWARD_Client 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_BACKWARD_Client 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* MSG_MOVE_START_BACKWARD_Server */ {
        const std::vector<unsigned char> buffer = {0, 32, 182, 0, 1, 5, 2, 0, 0, 0, 16, 87, 91, 2, 117, 165, 11, 198, 111, 244, 244, 194, 189, 13, 165, 66, 107, 108, 146, 64, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_BACKWARD_Server 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::MSG_MOVE_START_BACKWARD) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_BACKWARD_Server 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_BACKWARD_Server 0", TEST_UTILS_SIDE_SERVER);
    }
    /* MSG_MOVE_STOP_Client */ {
        const std::vector<unsigned char> buffer = {0, 32, 183, 0, 0, 0, 0, 0, 0, 0, 242, 49, 122, 1, 36, 203, 11, 198, 48, 32, 223, 194, 61, 23, 166, 66, 3, 81, 36, 64, 133, 3, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_STOP_Client 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::MSG_MOVE_STOP) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_STOP_Client 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_STOP_Client 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* MSG_MOVE_STOP_Server */ {
        const std::vector<unsigned char> buffer = {0, 32, 183, 0, 1, 5, 0, 0, 0, 0, 242, 49, 122, 1, 36, 203, 11, 198, 48, 32, 223, 194, 61, 23, 166, 66, 3, 81, 36, 64, 133, 3, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_STOP_Server 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::MSG_MOVE_STOP) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_STOP_Server 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_STOP_Server 0", TEST_UTILS_SIDE_SERVER);
    }
    /* MSG_MOVE_START_STRAFE_LEFT_Client */ {
        const std::vector<unsigned char> buffer = {0, 32, 184, 0, 0, 0, 5, 0, 0, 0, 159, 210, 121, 1, 238, 193, 11, 198, 253, 68, 8, 195, 36, 126, 167, 66, 184, 157, 194, 62, 127, 3, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_STRAFE_LEFT_Client 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::MSG_MOVE_START_STRAFE_LEFT) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_STRAFE_LEFT_Client 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_STRAFE_LEFT_Client 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* MSG_MOVE_START_STRAFE_LEFT_Server */ {
        const std::vector<unsigned char> buffer = {0, 32, 184, 0, 1, 5, 5, 0, 0, 0, 159, 210, 121, 1, 238, 193, 11, 198, 253, 68, 8, 195, 36, 126, 167, 66, 184, 157, 194, 62, 127, 3, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_STRAFE_LEFT_Server 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::MSG_MOVE_START_STRAFE_LEFT) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_STRAFE_LEFT_Server 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_STRAFE_LEFT_Server 0", TEST_UTILS_SIDE_SERVER);
    }
    /* MSG_MOVE_START_STRAFE_RIGHT_Client */ {
        const std::vector<unsigned char> buffer = {0, 32, 185, 0, 0, 0, 9, 0, 0, 0, 159, 210, 121, 1, 238, 193, 11, 198, 253, 68, 8, 195, 36, 126, 167, 66, 184, 157, 194, 62, 127, 3, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_STRAFE_RIGHT_Client 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::MSG_MOVE_START_STRAFE_RIGHT) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_STRAFE_RIGHT_Client 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_STRAFE_RIGHT_Client 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* MSG_MOVE_START_STRAFE_RIGHT_Server */ {
        const std::vector<unsigned char> buffer = {0, 32, 185, 0, 1, 5, 9, 0, 0, 0, 159, 210, 121, 1, 238, 193, 11, 198, 253, 68, 8, 195, 36, 126, 167, 66, 184, 157, 194, 62, 127, 3, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_STRAFE_RIGHT_Server 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::MSG_MOVE_START_STRAFE_RIGHT) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_STRAFE_RIGHT_Server 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_STRAFE_RIGHT_Server 0", TEST_UTILS_SIDE_SERVER);
    }
    /* MSG_MOVE_STOP_STRAFE_Client */ {
        const std::vector<unsigned char> buffer = {0, 32, 186, 0, 0, 0, 1, 0, 0, 0, 70, 211, 121, 1, 22, 192, 11, 198, 248, 49, 7, 195, 115, 127, 167, 66, 184, 157, 194, 62, 127, 3, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_STOP_STRAFE_Client 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::MSG_MOVE_STOP_STRAFE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_STOP_STRAFE_Client 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_STOP_STRAFE_Client 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* MSG_MOVE_STOP_STRAFE_Server */ {
        const std::vector<unsigned char> buffer = {0, 32, 186, 0, 1, 5, 1, 0, 0, 0, 70, 211, 121, 1, 22, 192, 11, 198, 248, 49, 7, 195, 115, 127, 167, 66, 184, 157, 194, 62, 127, 3, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_STOP_STRAFE_Server 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::MSG_MOVE_STOP_STRAFE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_STOP_STRAFE_Server 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_STOP_STRAFE_Server 0", TEST_UTILS_SIDE_SERVER);
    }
    /* MSG_MOVE_JUMP_Client */ {
        const std::vector<unsigned char> buffer = {0, 48, 187, 0, 0, 0, 1, 32, 0, 0, 32, 214, 121, 1, 27, 173, 11, 198, 157, 76, 5, 195, 209, 74, 167, 66, 184, 157, 194, 62, 0, 0, 0, 0, 216, 147, 254, 192, 77, 186, 109, 63, 159, 246, 189, 62, 0, 0, 224, 64, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_JUMP_Client 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::MSG_MOVE_JUMP) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_JUMP_Client 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_JUMP_Client 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* MSG_MOVE_JUMP_Server */ {
        const std::vector<unsigned char> buffer = {0, 48, 187, 0, 1, 5, 1, 32, 0, 0, 32, 214, 121, 1, 27, 173, 11, 198, 157, 76, 5, 195, 209, 74, 167, 66, 184, 157, 194, 62, 0, 0, 0, 0, 216, 147, 254, 192, 77, 186, 109, 63, 159, 246, 189, 62, 0, 0, 224, 64, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_JUMP_Server 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::MSG_MOVE_JUMP) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_JUMP_Server 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_JUMP_Server 0", TEST_UTILS_SIDE_SERVER);
    }
    /* MSG_MOVE_START_TURN_LEFT_Client */ {
        const std::vector<unsigned char> buffer = {0, 32, 188, 0, 0, 0, 16, 0, 0, 0, 251, 190, 121, 1, 205, 215, 11, 198, 53, 126, 4, 195, 249, 15, 167, 66, 0, 0, 0, 0, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_TURN_LEFT_Client 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::MSG_MOVE_START_TURN_LEFT) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_TURN_LEFT_Client 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_TURN_LEFT_Client 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* MSG_MOVE_START_TURN_LEFT_Server */ {
        const std::vector<unsigned char> buffer = {0, 32, 188, 0, 1, 5, 16, 0, 0, 0, 251, 190, 121, 1, 205, 215, 11, 198, 53, 126, 4, 195, 249, 15, 167, 66, 0, 0, 0, 0, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_TURN_LEFT_Server 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::MSG_MOVE_START_TURN_LEFT) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_TURN_LEFT_Server 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_TURN_LEFT_Server 0", TEST_UTILS_SIDE_SERVER);
    }
    /* MSG_MOVE_START_TURN_RIGHT_Client */ {
        const std::vector<unsigned char> buffer = {0, 32, 189, 0, 0, 0, 32, 0, 0, 0, 251, 190, 121, 1, 205, 215, 11, 198, 53, 126, 4, 195, 249, 15, 167, 66, 0, 0, 0, 0, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_TURN_RIGHT_Client 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::MSG_MOVE_START_TURN_RIGHT) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_TURN_RIGHT_Client 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_TURN_RIGHT_Client 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* MSG_MOVE_START_TURN_RIGHT_Server */ {
        const std::vector<unsigned char> buffer = {0, 32, 189, 0, 1, 5, 32, 0, 0, 0, 251, 190, 121, 1, 205, 215, 11, 198, 53, 126, 4, 195, 249, 15, 167, 66, 0, 0, 0, 0, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_TURN_RIGHT_Server 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::MSG_MOVE_START_TURN_RIGHT) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_TURN_RIGHT_Server 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_START_TURN_RIGHT_Server 0", TEST_UTILS_SIDE_SERVER);
    }
    /* MSG_MOVE_STOP_TURN_Client */ {
        const std::vector<unsigned char> buffer = {0, 32, 190, 0, 0, 0, 0, 0, 0, 0, 151, 166, 91, 2, 37, 162, 11, 198, 57, 130, 248, 194, 222, 72, 165, 66, 16, 19, 156, 64, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_STOP_TURN_Client 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::MSG_MOVE_STOP_TURN) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_STOP_TURN_Client 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_STOP_TURN_Client 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* MSG_MOVE_STOP_TURN_Server */ {
        const std::vector<unsigned char> buffer = {0, 32, 190, 0, 1, 5, 0, 0, 0, 0, 151, 166, 91, 2, 37, 162, 11, 198, 57, 130, 248, 194, 222, 72, 165, 66, 16, 19, 156, 64, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_STOP_TURN_Server 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::MSG_MOVE_STOP_TURN) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_STOP_TURN_Server 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_STOP_TURN_Server 0", TEST_UTILS_SIDE_SERVER);
    }
    /* MSG_MOVE_SET_RUN_MODE_Client */ {
        const std::vector<unsigned char> buffer = {0, 32, 194, 0, 0, 0, 1, 0, 0, 0, 65, 27, 91, 2, 85, 185, 11, 198, 248, 132, 1, 195, 173, 49, 167, 66, 46, 14, 195, 64, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_SET_RUN_MODE_Client 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::MSG_MOVE_SET_RUN_MODE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_SET_RUN_MODE_Client 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_SET_RUN_MODE_Client 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* MSG_MOVE_SET_RUN_MODE_Server */ {
        const std::vector<unsigned char> buffer = {0, 32, 194, 0, 1, 5, 1, 0, 0, 0, 65, 27, 91, 2, 85, 185, 11, 198, 248, 132, 1, 195, 173, 49, 167, 66, 46, 14, 195, 64, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_SET_RUN_MODE_Server 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::MSG_MOVE_SET_RUN_MODE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_SET_RUN_MODE_Server 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_SET_RUN_MODE_Server 0", TEST_UTILS_SIDE_SERVER);
    }
    /* MSG_MOVE_SET_WALK_MODE_Client */ {
        const std::vector<unsigned char> buffer = {0, 32, 195, 0, 0, 0, 1, 1, 0, 0, 154, 23, 91, 2, 2, 189, 11, 198, 78, 88, 1, 195, 38, 41, 167, 66, 46, 14, 195, 64, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_SET_WALK_MODE_Client 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::MSG_MOVE_SET_WALK_MODE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_SET_WALK_MODE_Client 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_SET_WALK_MODE_Client 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* MSG_MOVE_SET_WALK_MODE_Server */ {
        const std::vector<unsigned char> buffer = {0, 32, 195, 0, 1, 5, 1, 1, 0, 0, 154, 23, 91, 2, 2, 189, 11, 198, 78, 88, 1, 195, 38, 41, 167, 66, 46, 14, 195, 64, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_SET_WALK_MODE_Server 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::MSG_MOVE_SET_WALK_MODE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_SET_WALK_MODE_Server 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_SET_WALK_MODE_Server 0", TEST_UTILS_SIDE_SERVER);
    }
    /* MSG_MOVE_TELEPORT_ACK_Client */ {
        const std::vector<unsigned char> buffer = {0, 13, 199, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_TELEPORT_ACK_Client 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::MSG_MOVE_TELEPORT_ACK) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_TELEPORT_ACK_Client 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_TELEPORT_ACK_Client 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* MSG_MOVE_TELEPORT_ACK_Server */ {
        const std::vector<unsigned char> buffer = {0, 36, 199, 0, 1, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 135, 69, 0, 160, 37, 197, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_TELEPORT_ACK_Server 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::MSG_MOVE_TELEPORT_ACK) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_TELEPORT_ACK_Server 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_TELEPORT_ACK_Server 0", TEST_UTILS_SIDE_SERVER);
    }
    /* MSG_MOVE_FALL_LAND_Client */ {
        const std::vector<unsigned char> buffer = {0, 32, 201, 0, 0, 0, 0, 0, 0, 0, 165, 217, 121, 1, 173, 149, 11, 198, 120, 245, 2, 195, 241, 246, 165, 66, 75, 71, 175, 61, 133, 3, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_FALL_LAND_Client 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::MSG_MOVE_FALL_LAND) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_FALL_LAND_Client 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_FALL_LAND_Client 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* MSG_MOVE_FALL_LAND_Server */ {
        const std::vector<unsigned char> buffer = {0, 32, 201, 0, 1, 5, 0, 0, 0, 0, 165, 217, 121, 1, 173, 149, 11, 198, 120, 245, 2, 195, 241, 246, 165, 66, 75, 71, 175, 61, 133, 3, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_FALL_LAND_Server 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::MSG_MOVE_FALL_LAND) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_FALL_LAND_Server 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_FALL_LAND_Server 0", TEST_UTILS_SIDE_SERVER);
    }
    /* MSG_MOVE_SET_FACING_Client */ {
        const std::vector<unsigned char> buffer = {0, 32, 218, 0, 0, 0, 1, 0, 0, 0, 94, 45, 122, 1, 151, 175, 11, 198, 66, 10, 232, 194, 227, 37, 165, 66, 167, 79, 35, 64, 133, 3, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_SET_FACING_Client 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::MSG_MOVE_SET_FACING) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_SET_FACING_Client 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_SET_FACING_Client 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* MSG_MOVE_SET_FACING_Server */ {
        const std::vector<unsigned char> buffer = {0, 32, 218, 0, 1, 5, 1, 0, 0, 0, 94, 45, 122, 1, 151, 175, 11, 198, 66, 10, 232, 194, 227, 37, 165, 66, 167, 79, 35, 64, 133, 3, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_SET_FACING_Server 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::MSG_MOVE_SET_FACING) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_SET_FACING_Server 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_SET_FACING_Server 0", TEST_UTILS_SIDE_SERVER);
    }
    /* SMSG_FORCE_RUN_SPEED_CHANGE */ {
        const std::vector<unsigned char> buffer = {0, 12, 226, 0, 1, 6, 0, 0, 0, 0, 0, 0, 224, 64, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_FORCE_RUN_SPEED_CHANGE 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_FORCE_RUN_SPEED_CHANGE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_FORCE_RUN_SPEED_CHANGE 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_FORCE_RUN_SPEED_CHANGE 0", TEST_UTILS_SIDE_SERVER);
    }
    /* CMSG_FORCE_RUN_SPEED_CHANGE_ACK */ {
        const std::vector<unsigned char> buffer = {0, 48, 227, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 23, 246, 1, 203, 171, 11, 198, 7, 134, 248, 194, 142, 209, 165, 66, 237, 153, 127, 64, 57, 3, 0, 0, 0, 0, 224, 64, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_FORCE_RUN_SPEED_CHANGE_ACK 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_FORCE_RUN_SPEED_CHANGE_ACK) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_FORCE_RUN_SPEED_CHANGE_ACK 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_FORCE_RUN_SPEED_CHANGE_ACK 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* MSG_MOVE_HEARTBEAT_Client */ {
        const std::vector<unsigned char> buffer = {0, 32, 238, 0, 0, 0, 1, 0, 0, 0, 70, 49, 122, 1, 25, 199, 11, 198, 254, 110, 224, 194, 26, 245, 165, 66, 3, 81, 36, 64, 133, 3, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_HEARTBEAT_Client 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::MSG_MOVE_HEARTBEAT) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_HEARTBEAT_Client 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_HEARTBEAT_Client 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* MSG_MOVE_HEARTBEAT_Server */ {
        const std::vector<unsigned char> buffer = {0, 32, 238, 0, 1, 5, 1, 0, 0, 0, 70, 49, 122, 1, 25, 199, 11, 198, 254, 110, 224, 194, 26, 245, 165, 66, 3, 81, 36, 64, 133, 3, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_HEARTBEAT_Server 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::MSG_MOVE_HEARTBEAT) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_HEARTBEAT_Server 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_MOVE_HEARTBEAT_Server 0", TEST_UTILS_SIDE_SERVER);
    }
    /* SMSG_TUTORIAL_FLAGS */ {
        const std::vector<unsigned char> buffer = {0, 34, 253, 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_TUTORIAL_FLAGS 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_TUTORIAL_FLAGS) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_TUTORIAL_FLAGS 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_TUTORIAL_FLAGS 0", TEST_UTILS_SIDE_SERVER);
    }
    /* CMSG_STANDSTATECHANGE */ {
        const std::vector<unsigned char> buffer = {0, 8, 1, 1, 0, 0, 1, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_STANDSTATECHANGE 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_STANDSTATECHANGE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_STANDSTATECHANGE 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_STANDSTATECHANGE 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* CMSG_TEXT_EMOTE */ {
        const std::vector<unsigned char> buffer = {0, 20, 4, 1, 0, 0, 34, 0, 0, 0, 255, 255, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_TEXT_EMOTE 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_TEXT_EMOTE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_TEXT_EMOTE 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_TEXT_EMOTE 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* CMSG_AUTOEQUIP_ITEM */ {
        const std::vector<unsigned char> buffer = {0, 6, 10, 1, 0, 0, 255, 24, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_AUTOEQUIP_ITEM 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_AUTOEQUIP_ITEM) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_AUTOEQUIP_ITEM 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_AUTOEQUIP_ITEM 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* CMSG_INITIATE_TRADE */ {
        const std::vector<unsigned char> buffer = {0, 12, 22, 1, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_INITIATE_TRADE 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_INITIATE_TRADE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_INITIATE_TRADE 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_INITIATE_TRADE 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* CMSG_CANCEL_TRADE */ {
        const std::vector<unsigned char> buffer = {0, 4, 28, 1, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_CANCEL_TRADE 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_CANCEL_TRADE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_CANCEL_TRADE 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_CANCEL_TRADE 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* SMSG_INITIAL_SPELLS */ {
        const std::vector<unsigned char> buffer = {0, 167, 42, 1, 0, 40, 0, 78, 0, 0, 0, 81, 0, 0, 0, 107, 0, 0, 0, 196, 0, 0, 0, 198, 0, 0, 0, 201, 0, 0, 0, 203, 0, 0, 0, 204, 0, 0, 0, 10, 2, 0, 0, 156, 2, 0, 0, 78, 9, 0, 0, 153, 9, 0, 0, 175, 9, 0, 0, 234, 11, 0, 0, 37, 13, 0, 0, 181, 20, 0, 0, 89, 24, 0, 0, 102, 24, 0, 0, 103, 24, 0, 0, 77, 25, 0, 0, 78, 25, 0, 0, 203, 25, 0, 0, 98, 28, 0, 0, 99, 28, 0, 0, 187, 28, 0, 0, 194, 32, 0, 0, 33, 34, 0, 0, 117, 35, 0, 0, 118, 35, 0, 0, 156, 35, 0, 0, 165, 35, 0, 0, 117, 80, 0, 0, 118, 80, 0, 0, 119, 80, 0, 0, 120, 80, 0, 0, 128, 81, 0, 0, 147, 84, 0, 0, 148, 84, 0, 0, 11, 86, 0, 0, 26, 89, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_INITIAL_SPELLS 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_INITIAL_SPELLS) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_INITIAL_SPELLS 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_INITIAL_SPELLS 0", TEST_UTILS_SIDE_SERVER);
    }
    /* CMSG_CAST_SPELL */ {
        const std::vector<unsigned char> buffer = {0, 10, 46, 1, 0, 0, 120, 80, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_CAST_SPELL 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_CAST_SPELL) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_CAST_SPELL 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_CAST_SPELL 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* CMSG_CANCEL_CAST */ {
        const std::vector<unsigned char> buffer = {0, 8, 47, 1, 0, 0, 120, 80, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_CANCEL_CAST 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_CANCEL_CAST) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_CANCEL_CAST 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_CANCEL_CAST 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* CMSG_CANCEL_CAST */ {
        const std::vector<unsigned char> buffer = {0, 8, 47, 1, 0, 0, 242, 33, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_CANCEL_CAST 1 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_CANCEL_CAST) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_CANCEL_CAST 1 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_CANCEL_CAST 1", TEST_UTILS_SIDE_CLIENT);
    }
    /* CMSG_SET_SELECTION */ {
        const std::vector<unsigned char> buffer = {0, 12, 61, 1, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_SET_SELECTION 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_SET_SELECTION) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_SET_SELECTION 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_SET_SELECTION 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* CMSG_ATTACKSWING */ {
        const std::vector<unsigned char> buffer = {0, 12, 65, 1, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_ATTACKSWING 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_ATTACKSWING) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_ATTACKSWING 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_ATTACKSWING 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* SMSG_ATTACKSTART */ {
        const std::vector<unsigned char> buffer = {0, 18, 67, 1, 23, 0, 0, 0, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_ATTACKSTART 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_ATTACKSTART) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_ATTACKSTART 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_ATTACKSTART 0", TEST_UTILS_SIDE_SERVER);
    }
    /* SMSG_ATTACKSTOP */ {
        const std::vector<unsigned char> buffer = {0, 10, 68, 1, 1, 23, 1, 100, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_ATTACKSTOP 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_ATTACKSTOP) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_ATTACKSTOP 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_ATTACKSTOP 0", TEST_UTILS_SIDE_SERVER);
    }
    /* SMSG_ATTACKERSTATEUPDATE */ {
        const std::vector<unsigned char> buffer = {0, 51, 74, 1, 128, 0, 0, 0, 1, 23, 1, 100, 57, 5, 0, 0, 1, 0, 0, 0, 0, 0, 128, 166, 68, 52, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_ATTACKERSTATEUPDATE 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_ATTACKERSTATEUPDATE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_ATTACKERSTATEUPDATE 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_ATTACKERSTATEUPDATE 0", TEST_UTILS_SIDE_SERVER);
    }
    /* SMSG_ITEM_PUSH_RESULT */ {
        const std::vector<unsigned char> buffer = {0, 43, 102, 1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 255, 24, 0, 0, 0, 96, 49, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_ITEM_PUSH_RESULT 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_ITEM_PUSH_RESULT) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_ITEM_PUSH_RESULT 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_ITEM_PUSH_RESULT 0", TEST_UTILS_SIDE_SERVER);
    }
    /* CMSG_QUERY_TIME */ {
        const std::vector<unsigned char> buffer = {0, 4, 206, 1, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_QUERY_TIME 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_QUERY_TIME) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_QUERY_TIME 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_QUERY_TIME 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* SMSG_QUERY_TIME_RESPONSE */ {
        const std::vector<unsigned char> buffer = {0, 6, 207, 1, 148, 152, 80, 97, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_QUERY_TIME_RESPONSE 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_QUERY_TIME_RESPONSE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_QUERY_TIME_RESPONSE 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_QUERY_TIME_RESPONSE 0", TEST_UTILS_SIDE_SERVER);
    }
    /* CMSG_PING */ {
        const std::vector<unsigned char> buffer = {0, 12, 220, 1, 0, 0, 239, 190, 173, 222, 222, 202, 250, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_PING 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_PING) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_PING 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_PING 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* SMSG_PONG */ {
        const std::vector<unsigned char> buffer = {0, 6, 221, 1, 239, 190, 173, 222, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_PONG 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_PONG) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_PONG 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_PONG 0", TEST_UTILS_SIDE_SERVER);
    }
    /* CMSG_SETSHEATHED */ {
        const std::vector<unsigned char> buffer = {0, 8, 224, 1, 0, 0, 1, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_SETSHEATHED 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_SETSHEATHED) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_SETSHEATHED 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_SETSHEATHED 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* SMSG_AUTH_CHALLENGE */ {
        const std::vector<unsigned char> buffer = {0, 6, 236, 1, 239, 190, 173, 222, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_AUTH_CHALLENGE 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_AUTH_CHALLENGE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_AUTH_CHALLENGE 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_AUTH_CHALLENGE 0", TEST_UTILS_SIDE_SERVER);
    }
    /* SMSG_AUTH_RESPONSE */ {
        const std::vector<unsigned char> buffer = {0, 3, 238, 1, 13, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_AUTH_RESPONSE 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_AUTH_RESPONSE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_AUTH_RESPONSE 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_AUTH_RESPONSE 0", TEST_UTILS_SIDE_SERVER);
    }
    /* SMSG_AUTH_RESPONSE */ {
        const std::vector<unsigned char> buffer = {0, 7, 238, 1, 27, 239, 190, 173, 222, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_AUTH_RESPONSE 1 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_AUTH_RESPONSE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_AUTH_RESPONSE 1 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_AUTH_RESPONSE 1", TEST_UTILS_SIDE_SERVER);
    }
    /* SMSG_AUTH_RESPONSE */ {
        const std::vector<unsigned char> buffer = {0, 12, 238, 1, 12, 239, 190, 173, 222, 0, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_AUTH_RESPONSE 2 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_AUTH_RESPONSE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_AUTH_RESPONSE 2 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_AUTH_RESPONSE 2", TEST_UTILS_SIDE_SERVER);
    }
    /* CMSG_ZONEUPDATE */ {
        const std::vector<unsigned char> buffer = {0, 8, 244, 1, 0, 0, 101, 6, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_ZONEUPDATE 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_ZONEUPDATE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_ZONEUPDATE 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_ZONEUPDATE 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* CMSG_ZONEUPDATE */ {
        const std::vector<unsigned char> buffer = {0, 8, 244, 1, 0, 0, 12, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_ZONEUPDATE 1 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_ZONEUPDATE) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_ZONEUPDATE 1 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_ZONEUPDATE 1", TEST_UTILS_SIDE_CLIENT);
    }
    /* SMSG_ACCOUNT_DATA_TIMES */ {
        const std::vector<unsigned char> buffer = {0, 130, 9, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_ACCOUNT_DATA_TIMES 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_ACCOUNT_DATA_TIMES) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_ACCOUNT_DATA_TIMES 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_ACCOUNT_DATA_TIMES 0", TEST_UTILS_SIDE_SERVER);
    }
    /* CMSG_REQUEST_ACCOUNT_DATA */ {
        const std::vector<unsigned char> buffer = {0, 8, 10, 2, 0, 0, 6, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_REQUEST_ACCOUNT_DATA 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_REQUEST_ACCOUNT_DATA) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_REQUEST_ACCOUNT_DATA 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_REQUEST_ACCOUNT_DATA 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* CMSG_GMTICKET_GETTICKET */ {
        const std::vector<unsigned char> buffer = {0, 4, 17, 2, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_GMTICKET_GETTICKET 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_GMTICKET_GETTICKET) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_GMTICKET_GETTICKET 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_GMTICKET_GETTICKET 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* SMSG_LOGIN_VERIFY_WORLD */ {
        const std::vector<unsigned char> buffer = {0, 22, 54, 2, 0, 0, 0, 0, 205, 215, 11, 198, 53, 126, 4, 195, 249, 15, 167, 66, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_LOGIN_VERIFY_WORLD 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_LOGIN_VERIFY_WORLD) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_LOGIN_VERIFY_WORLD 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_LOGIN_VERIFY_WORLD 0", TEST_UTILS_SIDE_SERVER);
    }
    /* CMSG_LEARN_TALENT */ {
        const std::vector<unsigned char> buffer = {0, 12, 81, 2, 0, 0, 158, 0, 0, 0, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_LEARN_TALENT 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_LEARN_TALENT) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_LEARN_TALENT 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_LEARN_TALENT 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* MSG_AUCTION_HELLO_Client */ {
        const std::vector<unsigned char> buffer = {0, 12, 85, 2, 0, 0, 239, 190, 173, 222, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_AUCTION_HELLO_Client 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::MSG_AUCTION_HELLO) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_AUCTION_HELLO_Client 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_AUCTION_HELLO_Client 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* CMSG_SET_ACTIVE_MOVER */ {
        const std::vector<unsigned char> buffer = {0, 12, 106, 2, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_SET_ACTIVE_MOVER 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_SET_ACTIVE_MOVER) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_SET_ACTIVE_MOVER 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_SET_ACTIVE_MOVER 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* MSG_QUERY_NEXT_MAIL_TIME_Client */ {
        const std::vector<unsigned char> buffer = {0, 4, 132, 2, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_QUERY_NEXT_MAIL_TIME_Client 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::MSG_QUERY_NEXT_MAIL_TIME) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " MSG_QUERY_NEXT_MAIL_TIME_Client 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " MSG_QUERY_NEXT_MAIL_TIME_Client 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* CMSG_MEETINGSTONE_INFO */ {
        const std::vector<unsigned char> buffer = {0, 4, 150, 2, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_MEETINGSTONE_INFO 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_MEETINGSTONE_INFO) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_MEETINGSTONE_INFO 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_MEETINGSTONE_INFO 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* CMSG_CHAR_RENAME */ {
        const std::vector<unsigned char> buffer = {0, 21, 199, 2, 0, 0, 239, 190, 173, 222, 0, 0, 0, 0, 68, 101, 97, 100, 98, 101, 101, 102, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_CHAR_RENAME 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_CHAR_RENAME) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_CHAR_RENAME 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_CHAR_RENAME 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* SMSG_CHAR_RENAME */ {
        const std::vector<unsigned char> buffer = {0, 3, 200, 2, 71, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_CHAR_RENAME 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_CHAR_RENAME) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_CHAR_RENAME 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_CHAR_RENAME 0", TEST_UTILS_SIDE_SERVER);
    }
    /* SMSG_CHAR_RENAME */ {
        const std::vector<unsigned char> buffer = {0, 20, 200, 2, 0, 239, 190, 173, 222, 0, 0, 0, 0, 68, 101, 97, 100, 98, 101, 101, 102, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_CHAR_RENAME 1 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_CHAR_RENAME) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_CHAR_RENAME 1 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_CHAR_RENAME 1", TEST_UTILS_SIDE_SERVER);
    }
    /* CMSG_REQUEST_RAID_INFO */ {
        const std::vector<unsigned char> buffer = {0, 4, 205, 2, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_REQUEST_RAID_INFO 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_REQUEST_RAID_INFO) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_REQUEST_RAID_INFO 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_REQUEST_RAID_INFO 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* CMSG_MOVE_TIME_SKIPPED */ {
        const std::vector<unsigned char> buffer = {0, 16, 206, 2, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_MOVE_TIME_SKIPPED 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_MOVE_TIME_SKIPPED) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_MOVE_TIME_SKIPPED 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_MOVE_TIME_SKIPPED 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* CMSG_BATTLEFIELD_STATUS */ {
        const std::vector<unsigned char> buffer = {0, 4, 211, 2, 0, 0, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_client_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_BATTLEFIELD_STATUS 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ClientOpcode::Opcode::CMSG_BATTLEFIELD_STATUS) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " CMSG_BATTLEFIELD_STATUS 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " CMSG_BATTLEFIELD_STATUS 0", TEST_UTILS_SIDE_CLIENT);
    }
    /* SMSG_SPLINE_SET_RUN_SPEED */ {
        const std::vector<unsigned char> buffer = {0, 8, 254, 2, 1, 6, 0, 0, 224, 64, };
        auto reader = ByteReader(buffer);

        const auto opcode = ::wow_world_messages::vanilla::read_server_opcode(reader);
        if (opcode.is_none()) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_SPLINE_SET_RUN_SPEED 0 read invalid opcode");
            return 1;
        }

        if (opcode.opcode != ::wow_world_messages::vanilla::ServerOpcode::Opcode::SMSG_SPLINE_SET_RUN_SPEED) {
            printf(__FILE__ ":" STRINGIFY(__LINE__) " SMSG_SPLINE_SET_RUN_SPEED 0 read invalid opcode");
            return 1;
        }

        const std::vector<unsigned char> write_buffer = ::wow_world_messages::vanilla::write_opcode(opcode);

        world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " SMSG_SPLINE_SET_RUN_SPEED 0", TEST_UTILS_SIDE_SERVER);
    }
    return 0;
}
