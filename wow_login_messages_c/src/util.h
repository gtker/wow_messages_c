#ifndef WLM_UTIL_H
#define WLM_UTIL_H
/*
 * Manually written utility functions for login.c
 */

#include "wow_login_messages/wow_login_messages.h"

#include <malloc.h>
#include <memory.h>
#include <stdint.h>

#define WLM_CHECK_RETURN_CODE(action)                    \
    do                                                   \
    {                                                    \
        int result_variable_macro = action;              \
        if (result_variable_macro != WLM_RESULT_SUCCESS) \
        {                                                \
            return result_variable_macro;                \
        }                                                \
    }                                                    \
    while (0)

#define WLM_CHECK_LENGTH(type_size)                       \
    stream->index;                                        \
    do                                                    \
    {                                                     \
        if (stream->index + (type_size) > stream->length) \
        {                                                 \
            return WLM_RESULT_NOT_ENOUGH_BYTES;           \
        }                                                 \
        stream->index += (type_size);                     \
    }                                                     \
    while (0)

#define SKIP_FORWARD_BYTES(type_size)                     \
    do                                                    \
    {                                                     \
        if (reader->index + (type_size) > reader->length) \
        {                                                 \
            return WLM_RESULT_NOT_ENOUGH_BYTES;           \
        }                                                 \
        reader->index += (type_size);                     \
    }                                                     \
    while (0)

static WowLoginResult wlm_read_uint8(WowLoginReader* stream, uint8_t* value)
{
    const size_t index = WLM_CHECK_LENGTH(1);

    *value = stream->source[index];

    return WLM_RESULT_SUCCESS;
}

#define READ_U8(variable) WLM_CHECK_RETURN_CODE(wlm_read_uint8(reader, (uint8_t*)&variable))

static WowLoginResult wlm_read_uint16(WowLoginReader* stream, uint16_t* value)
{
    const size_t index = WLM_CHECK_LENGTH(2);

    *value = (uint16_t)stream->source[index] | (uint16_t)stream->source[index + 1] << 8;

    return WLM_RESULT_SUCCESS;
}

#define READ_U16(variable) WLM_CHECK_RETURN_CODE(wlm_read_uint16(reader, (uint16_t*)&variable))

static WowLoginResult wlm_read_uint32(WowLoginReader* stream, uint32_t* value)
{
    const size_t index = WLM_CHECK_LENGTH(4);

    *value = (uint32_t)stream->source[index] | (uint32_t)stream->source[index + 1] << 8 |
        (uint32_t)stream->source[index + 2] << 16 | (uint32_t)stream->source[index + 3] << 24;

    return WLM_RESULT_SUCCESS;
}

#define READ_U32(variable) WLM_CHECK_RETURN_CODE(wlm_read_uint32(reader, (uint32_t*)&variable))

static WowLoginResult wlm_read_uint64(WowLoginReader* stream, uint64_t* value)
{
    const size_t index = WLM_CHECK_LENGTH(8);

    *value = stream->source[index] | (uint64_t)stream->source[index + 1] << 8 |
        (uint64_t)stream->source[index + 2] << 16 | (uint64_t)stream->source[index + 3] << 24 |
        (uint64_t)stream->source[index + 4] << 32 | (uint64_t)stream->source[index + 5] << 40 |
        (uint64_t)stream->source[index + 6] << 48 | (uint64_t)stream->source[index + 7] << 56;

    return WLM_RESULT_SUCCESS;
}

#define READ_U64(variable) WLM_CHECK_RETURN_CODE(wlm_read_uint64(reader, (uint64_t*)&variable))

static WowLoginResult wlm_read_int32(WowLoginReader* stream, int32_t* value)
{
    const size_t index = WLM_CHECK_LENGTH(4);

    *value = (int32_t)stream->source[index] | (int32_t)stream->source[index + 1] << 8 |
        (int32_t)stream->source[index + 2] << 16 | (int32_t)stream->source[index + 3] << 24;

    return WLM_RESULT_SUCCESS;
}

#define READ_I32(variable) WLM_CHECK_RETURN_CODE(wlm_read_int32(reader, (int32_t*)&variable))

static WowLoginResult wlm_write_uint8(WowLoginWriter* stream, const uint8_t value)
{
    const size_t index = WLM_CHECK_LENGTH(1);

    stream->destination[index] = (char)value;

    return WLM_RESULT_SUCCESS;
}

#define WRITE_U8(variable) WLM_CHECK_RETURN_CODE(wlm_write_uint8(writer, variable))

static WowLoginResult wlm_write_uint16(WowLoginWriter* stream, const uint16_t value)
{
    const size_t index = WLM_CHECK_LENGTH(2);

    stream->destination[index] = (char)value;
    stream->destination[index + 1] = (char)(value >> 8);

    return WLM_RESULT_SUCCESS;
}

#define WRITE_U16(variable) WLM_CHECK_RETURN_CODE(wlm_write_uint16(writer, variable))

static WowLoginResult wlm_write_uint32(WowLoginWriter* stream, const uint32_t value)
{
    const size_t index = WLM_CHECK_LENGTH(4);

    stream->destination[index] = (char)value;
    stream->destination[index + 1] = (char)(value >> 8);
    stream->destination[index + 2] = (char)(value >> 16);
    stream->destination[index + 3] = (char)(value >> 24);

    return WLM_RESULT_SUCCESS;
}

#define WRITE_U32(variable) WLM_CHECK_RETURN_CODE(wlm_write_uint32(writer, variable))

static WowLoginResult wlm_write_uint64(WowLoginWriter* stream, const uint64_t value)
{
    const size_t index = WLM_CHECK_LENGTH(8);

    stream->destination[index] = (char)value;
    stream->destination[index + 1] = (char)(value >> 8);
    stream->destination[index + 2] = (char)(value >> 16);
    stream->destination[index + 3] = (char)(value >> 24);
    stream->destination[index + 4] = (char)(value >> 32);
    stream->destination[index + 5] = (char)(value >> 40);
    stream->destination[index + 6] = (char)(value >> 48);
    stream->destination[index + 7] = (char)(value >> 56);

    return WLM_RESULT_SUCCESS;
}

#define WRITE_U64(variable) WLM_CHECK_RETURN_CODE(wlm_write_uint64(writer, variable))

static WowLoginResult wlm_write_int32(WowLoginWriter* stream, const int32_t value)
{
    const size_t index = WLM_CHECK_LENGTH(4);

    stream->destination[index] = (char)value;
    stream->destination[index + 1] = (char)(value >> 8);
    stream->destination[index + 2] = (char)(value >> 16);
    stream->destination[index + 3] = (char)(value >> 24);

    return WLM_RESULT_SUCCESS;
}

#define WRITE_I32(variable) WLM_CHECK_RETURN_CODE(wlm_write_int32(writer, variable))

static WowLoginResult wlm_read_string(WowLoginReader* stream, WowLoginString* string)
{
    size_t index = WLM_CHECK_LENGTH(1);
    string->length = stream->source[index];

    index = WLM_CHECK_LENGTH(string->length);

    string->string = malloc(string->length + 1);

    memcpy(string->string, &stream->source[index], string->length);
    string->string[string->length] = '\0';

    return WLM_RESULT_SUCCESS;
}

#define READ_STRING(variable) WLM_CHECK_RETURN_CODE(wlm_read_string(reader, &variable))

static WowLoginResult wlm_write_string(WowLoginWriter* stream, const WowLoginString* string)
{
    const size_t index = WLM_CHECK_LENGTH(1 + string->length);

    stream->destination[index] = string->length;
    memcpy(&stream->destination[index + 1], string->string, string->length);

    return WLM_RESULT_SUCCESS;
}

#define WRITE_STRING(variable) WLM_CHECK_RETURN_CODE(wlm_write_string(writer, &variable))

static WowLoginResult wlm_read_cstring(WowLoginReader* stream, WowLoginString* string)
{
    const unsigned char* const start = &stream->source[stream->index];
    size_t index = WLM_CHECK_LENGTH(1);
    string->length = 0;

    while (stream->source[index] != '\0')
    {
        index = WLM_CHECK_LENGTH(1);
        string->length++;
    }

    string->string = malloc(string->length + 1);

    memcpy(string->string, start, string->length);
    string->string[string->length] = '\0';

    return WLM_RESULT_SUCCESS;
}

#define READ_CSTRING(variable) WLM_CHECK_RETURN_CODE(wlm_read_cstring(reader, &variable))

static WowLoginResult wlm_write_cstring(WowLoginWriter* stream, const WowLoginString* string)
{
    const size_t index = WLM_CHECK_LENGTH(1 + string->length);

    memcpy(&stream->destination[index], string->string, string->length + 1);

    return WLM_RESULT_SUCCESS;
}

#define WRITE_CSTRING(variable) WLM_CHECK_RETURN_CODE(wlm_write_cstring(writer, &variable))

static WowLoginResult wlm_read_float(WowLoginReader* stream, float* value)
{
    uint32_t v;
    const WowLoginResult result = wlm_read_uint32(stream, &v);

    memcpy(value, &v, sizeof(uint32_t));

    return result;
}

#define READ_FLOAT(variable) WLM_CHECK_RETURN_CODE(wlm_read_float(reader, &variable))

static WowLoginResult wlm_write_float(WowLoginWriter* stream, const float value)
{
    uint32_t v;
    memcpy(&v, &value, sizeof(uint32_t));

    return wlm_write_uint32(stream, v);
}

#define WRITE_FLOAT(variable) WLM_CHECK_RETURN_CODE(wlm_write_float(writer, variable))

static WowLoginResult wlm_read_bool8(WowLoginReader* stream, bool* value)
{
    uint8_t v;

    WLM_CHECK_RETURN_CODE(wlm_read_uint8(stream, &v));

    *value = v == 1 ? true : false;

    return WLM_RESULT_SUCCESS;
}

#define READ_BOOL8(variable) WLM_CHECK_RETURN_CODE(wlm_read_bool8(reader, &variable))

static WowLoginResult wlm_write_bool8(WowLoginWriter* stream, const bool value)
{
    const uint8_t v = value ? 1 : 0;

    WLM_CHECK_RETURN_CODE(wlm_write_uint8(stream, v));

    return WLM_RESULT_SUCCESS;
}

#define WRITE_BOOL8(variable) WLM_CHECK_RETURN_CODE(wlm_write_bool8(writer, variable))

static void wlm_free_string(WowLoginString* string)
{
    free(string->string);

    string->string = NULL;
    string->length = 0;
}

#define FREE_STRING(s) wlm_free_string(&s)

#define STRING_SIZE(s) (s.length)


#define READ_ARRAY_ALLOCATE(variable, arrayLength, elementSize) \
    do                                                          \
    {                                                           \
        size_t calculated_size = (elementSize * arrayLength);   \
        variable = malloc(calculated_size);                     \
    }                                                           \
    while (0)

#define READ_ARRAY(variable, arrayLength, readAction) \
    do                                                \
    {                                                 \
        int i;                                        \
        for (i = 0; i < arrayLength; ++i)             \
        {                                             \
            readAction;                               \
        }                                             \
    }                                                 \
    while (0)

#define WRITE_ARRAY(variable, arrayLength, writeAction) \
    do                                                  \
    {                                                   \
        int i;                                          \
        for (i = 0; i < arrayLength; ++i)               \
        {                                               \
            writeAction;                                \
        }                                               \
    }                                                   \
    while (0)

#undef WLM_CHECK_LENGTH


WOW_LOGIN_MESSAGES_C_EXPORT WowLoginReader wlm_create_reader(const unsigned char* const source, const size_t length)
{
    WowLoginReader reader;
    reader.source = source;
    reader.length = length;
    reader.index = 0;

    return reader;
}

WOW_LOGIN_MESSAGES_C_EXPORT WowLoginWriter wlm_create_writer(unsigned char* destination, const size_t length)
{
    WowLoginWriter writer;
    writer.destination = destination;
    writer.length = length;
    writer.index = 0;

    return writer;
}

WOW_LOGIN_MESSAGES_C_EXPORT const char* wlm_error_code_to_string(const WowLoginResult result)
{
    switch (result)
    {
        case WLM_RESULT_SUCCESS:
            return "Success";
        case WLM_RESULT_NOT_ENOUGH_BYTES:
            return "Not enough bytes";
        case WLM_RESULT_MALLOC_FAIL:
            return "Malloc fail";
    }

    return "";
}

#endif
