#ifndef WWM_UTIL_H
#define WWM_UTIL_H

#include "wow_world_messages/wow_world_messages.h"

#include <malloc.h>
#include <stdint.h>

#define WWM_CHECK_RETURN_CODE(action)                    \
    do                                                   \
    {                                                    \
        int result_variable_macro = action;              \
        if (result_variable_macro != WWM_RESULT_SUCCESS) \
        {                                                \
            return result_variable_macro;                \
        }                                                \
    }                                                    \
    while (0)

#define WWM_CHECK_LENGTH(type_size)                       \
    stream->index;                                        \
    do                                                    \
    {                                                     \
        if (stream->index + (type_size) > stream->length) \
        {                                                 \
            return WWM_RESULT_NOT_ENOUGH_BYTES;           \
        }                                                 \
        stream->index += (type_size);                     \
    }                                                     \
    while (0)

#define SKIP_FORWARD_BYTES(type_size)                     \
    do                                                    \
    {                                                     \
        if (reader->index + (type_size) > reader->length) \
        {                                                 \
            return WWM_RESULT_NOT_ENOUGH_BYTES;           \
        }                                                 \
        reader->index += (type_size);                     \
    }                                                     \
    while (0)

WowWorldResult wwm_read_uint8(WowWorldReader* stream, uint8_t* value);

#define READ_U8(variable) WWM_CHECK_RETURN_CODE(wwm_read_uint8(reader, (uint8_t*)&variable))

WowWorldResult wwm_read_uint16(WowWorldReader* stream, uint16_t* value);

#define READ_U16(variable) WWM_CHECK_RETURN_CODE(wwm_read_uint16(reader, (uint16_t*)&variable))

WowWorldResult wwm_read_uint16_be(WowWorldReader* stream, uint16_t* value);

#define READ_U16_BE(variable) WWM_CHECK_RETURN_CODE(wwm_read_uint16_be(reader, (uint16_t*)&variable))

WowWorldResult wwm_read_uint32(WowWorldReader* stream, uint32_t* value);

#define READ_U32(variable) WWM_CHECK_RETURN_CODE(wwm_read_uint32(reader, (uint32_t*)&variable))

WowWorldResult wwm_read_uint64(WowWorldReader* stream, uint64_t* value);

#define READ_U64(variable) WWM_CHECK_RETURN_CODE(wwm_read_uint64(reader, (uint64_t*)&variable))

WowWorldResult wwm_read_int32(WowWorldReader* stream, int32_t* value);

#define READ_I32(variable) WWM_CHECK_RETURN_CODE(wwm_read_int32(reader, (int32_t*)&variable))

WowWorldResult wwm_write_uint8(WowWorldWriter* stream, uint8_t value);

#define WRITE_U8(variable) WWM_CHECK_RETURN_CODE(wwm_write_uint8(writer, variable))

WowWorldResult wwm_write_uint16(WowWorldWriter* stream, uint16_t value);

#define WRITE_U16(variable) WWM_CHECK_RETURN_CODE(wwm_write_uint16(writer, variable))

WowWorldResult wwm_write_uint16_be(WowWorldWriter* stream, uint16_t value);

#define WRITE_U16_BE(variable) WWM_CHECK_RETURN_CODE(wwm_write_uint16_be(writer, variable))

WowWorldResult wwm_write_uint32(WowWorldWriter* stream, uint32_t value);

#define WRITE_U32(variable) WWM_CHECK_RETURN_CODE(wwm_write_uint32(writer, variable))

WowWorldResult wwm_write_uint64(WowWorldWriter* stream, uint64_t value);

#define WRITE_U64(variable) WWM_CHECK_RETURN_CODE(wwm_write_uint64(writer, variable))

WowWorldResult wwm_write_int32(WowWorldWriter* stream, int32_t value);

#define WRITE_I32(variable) WWM_CHECK_RETURN_CODE(wwm_write_int32(writer, variable))

WowWorldResult wwm_read_string(WowWorldReader* stream, WowWorldString* string);

#define READ_STRING(variable) WWM_CHECK_RETURN_CODE(wwm_read_string(reader, &variable));

WowWorldResult wwm_write_string(WowWorldWriter* stream, const WowWorldString* string);

#define WRITE_STRING(variable) WWM_CHECK_RETURN_CODE(wwm_write_string(writer, &variable))

WowWorldResult wwm_read_cstring(WowWorldReader* stream, WowWorldString* string);

#define READ_CSTRING(variable) WWM_CHECK_RETURN_CODE(wwm_read_cstring(reader, &variable));

WowWorldResult wwm_write_cstring(WowWorldWriter* stream, const WowWorldString* string);

#define WRITE_CSTRING(variable) WWM_CHECK_RETURN_CODE(wwm_write_cstring(writer, &variable))

WowWorldResult wwm_read_sized_cstring(WowWorldReader* stream, WowWorldString* string);

#define READ_SIZED_CSTRING(variable) WWM_CHECK_RETURN_CODE(wwm_read_sized_cstring(reader, &variable));

WowWorldResult wwm_write_sized_cstring(WowWorldWriter* stream, const WowWorldString* string);

#define WRITE_SIZED_CSTRING(variable) WWM_CHECK_RETURN_CODE(wwm_write_sized_cstring(writer, &variable))

WowWorldResult wwm_read_float(WowWorldReader* stream, float* value);

#define READ_FLOAT(variable) WWM_CHECK_RETURN_CODE(wwm_read_float(reader, &variable))

WowWorldResult wwm_write_float(WowWorldWriter* stream, float value);

#define WRITE_FLOAT(variable) WWM_CHECK_RETURN_CODE(wwm_write_float(writer, variable))

WowWorldResult wwm_read_bool8(WowWorldReader* stream, bool* value);

#define READ_BOOL8(variable) WWM_CHECK_RETURN_CODE(wwm_read_bool8(reader, &variable))

WowWorldResult wwm_write_bool8(WowWorldWriter* stream, bool value);

#define WRITE_BOOL8(variable) WWM_CHECK_RETURN_CODE(wwm_write_bool8(writer, variable))

WowWorldResult wwm_read_bool32(WowWorldReader* stream, bool* value);

#define READ_BOOL32(variable) WWM_CHECK_RETURN_CODE(wwm_read_bool32(reader, &variable))

WowWorldResult wwm_write_bool32(WowWorldWriter* stream, bool value);

#define WRITE_BOOL32(variable) WWM_CHECK_RETURN_CODE(wwm_write_bool32(writer, variable))

void wwm_free_string(WowWorldString* string);

#define FREE_STRING(s) wwm_free_string(&s)

#define STRING_SIZE(s) (s.length)

size_t wwm_packed_guid_size(uint64_t);

WowWorldResult wwm_read_packed_guid(WowWorldReader* stream, uint64_t* value);

#define READ_PACKED_GUID(variable) WWM_CHECK_RETURN_CODE(wwm_read_packed_guid(reader, &variable))

WowWorldResult wwm_write_packed_guid(WowWorldWriter* stream, uint64_t value);

#define WRITE_PACKED_GUID(variable) WWM_CHECK_RETURN_CODE(wwm_write_packed_guid(writer, variable))

WowWorldResult wwm_read_monster_move_spline(WowWorldReader* stream, MonsterMoveSpline* value);

#define READ_MONSTER_MOVE_SPLINE(variable) WWM_CHECK_RETURN_CODE(wwm_read_monster_move_spline(reader, &variable))

WowWorldResult wwm_write_monster_move_spline(WowWorldWriter* stream, const MonsterMoveSpline* value);

#define WRITE_MONSTER_MOVE_SPLINE(variable) WWM_CHECK_RETURN_CODE(wwm_write_monster_move_spline(writer, &variable))

size_t wwm_monster_move_spline_size(const MonsterMoveSpline* value);

void wwm_monster_move_spline_free(const MonsterMoveSpline* value);

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
        for (i = 0; i < (int)arrayLength; ++i)        \
        {                                             \
            readAction;                               \
        }                                             \
    }                                                 \
    while (0)

#define WRITE_ARRAY(variable, arrayLength, writeAction) \
    do                                                  \
    {                                                   \
        int i;                                          \
        for (i = 0; i < (int)arrayLength; ++i)          \
        {                                               \
            writeAction;                                \
        }                                               \
    }                                                   \
    while (0)

#endif
