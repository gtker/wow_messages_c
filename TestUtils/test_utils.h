#ifndef WOW_LOGIN_MESSAGES_TEST_UTILS_H
#define WOW_LOGIN_MESSAGES_TEST_UTILS_H

#include <stdio.h>
#include <stdlib.h>

#include "wow_login_messages/wow_login_messages.h"

#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)

void wlm_test_compare_buffers(const unsigned char* buffer,
                              const unsigned char* write_buffer,
                              size_t buffer_size,
                              const char* location);

#define ASSERT_SUCCESS(name, id, opcodeId, opcodeType, functionVersion, functionSide, libraryType, libraryPrefix) \
    do                                                                                                            \
    {                                                                                                             \
        unsigned char buffer[] = {name##id};                                                                      \
        Wow##libraryType##Reader reader = libraryPrefix##_create_reader(buffer, sizeof(buffer));                  \
                                                                                                                  \
        opcodeType opcode;                                                                                        \
        int result = (int)functionVersion##_##functionSide##_opcode_read(&reader, &opcode);                       \
                                                                                                                  \
        if (result != 0)                                                                                          \
        {                                                                                                         \
            printf(__FILE__ ":" STRINGIFY(__LINE__) " " #name " " STRINGIFY(id) " failed read: '%s'\n",           \
                   libraryPrefix##_error_code_to_string(result));                                                 \
            exit(EXIT_FAILURE);                                                                                   \
        }                                                                                                         \
        if (opcode.opcode != opcodeId)                                                                            \
        {                                                                                                         \
            printf(__FILE__                                                                                       \
                   ":" STRINGIFY(__LINE__) " " #name                                                              \
                                           " " STRINGIFY(id) " read wrong opcode: '0x%x' instead of '0x%x'\n",    \
                   opcode.opcode, opcodeId);                                                                      \
            exit(EXIT_FAILURE);                                                                                   \
        }                                                                                                         \
        unsigned char write_buffer[sizeof(buffer)] = {0};                                                         \
        Wow##libraryType##Writer writer = libraryPrefix##_create_writer(write_buffer, sizeof(write_buffer));      \
        result = functionVersion##_##name##_write(&writer, &opcode.body.name);                                    \
                                                                                                                  \
        wlm_test_compare_buffers(buffer, write_buffer, sizeof(write_buffer),                                      \
                                 __FILE__ ":" STRINGIFY(__LINE__) " " #name " " STRINGIFY(id));                   \
    }                                                                                                             \
    while (0)

#define ASSERT_SUCCESS_EMPTY(name, id, opcodeId, opcodeType, functionVersion, functionSide, libraryType,       \
                             libraryPrefix)                                                                    \
    do                                                                                                         \
    {                                                                                                          \
        unsigned char buffer[] = {name##id};                                                                   \
        Wow##libraryType##Reader reader = libraryPrefix##_create_reader(buffer, sizeof(buffer));               \
                                                                                                               \
        opcodeType opcode;                                                                                     \
        int result = (int)functionVersion##_##functionSide##_opcode_read(&reader, &opcode);                    \
                                                                                                               \
        if (result != 0)                                                                                       \
        {                                                                                                      \
            printf(__FILE__ ":" STRINGIFY(__LINE__) " " #name " " STRINGIFY(id) " failed read: '%s'\n",        \
                   libraryPrefix##_error_code_to_string(result));                                              \
            exit(EXIT_FAILURE);                                                                                \
        }                                                                                                      \
        if (opcode.opcode != opcodeId)                                                                         \
        {                                                                                                      \
            printf(__FILE__                                                                                    \
                   ":" STRINGIFY(__LINE__) " " #name                                                           \
                                           " " STRINGIFY(id) " read wrong opcode: '0x%x' instead of '0x%x'\n", \
                   opcode.opcode, opcodeId);                                                                   \
            exit(EXIT_FAILURE);                                                                                \
        }                                                                                                      \
        unsigned char write_buffer[sizeof(buffer)] = {0};                                                      \
        Wow##libraryType##Writer writer = libraryPrefix##_create_writer(write_buffer, sizeof(write_buffer));   \
        result = functionVersion##_##name##_write(&writer);                                                    \
                                                                                                               \
        wlm_test_compare_buffers(buffer, write_buffer, sizeof(write_buffer),                                   \
                                 __FILE__ ":" STRINGIFY(__LINE__) " " #name " " STRINGIFY(id));                \
    }                                                                                                          \
    while (0)

#endif
