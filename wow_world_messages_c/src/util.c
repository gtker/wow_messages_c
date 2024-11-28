#include "util.h"

#include <memory.h>

#include "puff.c"

WowWorldResult wwm_read_uint8(WowWorldReader* stream, uint8_t* value)
{
    const size_t index = WWM_CHECK_LENGTH(1);

    *value = stream->source[index];

    return WWM_RESULT_SUCCESS;
}

WowWorldResult wwm_read_uint16(WowWorldReader* stream, uint16_t* value)
{
    const size_t index = WWM_CHECK_LENGTH(2);

    *value = (uint16_t)stream->source[index] | (uint16_t)stream->source[index + 1] << 8;

    return WWM_RESULT_SUCCESS;
}

WowWorldResult wwm_read_uint16_be(WowWorldReader* stream, uint16_t* value)
{
    const size_t index = WWM_CHECK_LENGTH(2);

    *value = (uint16_t)stream->source[index] << 8 | (uint16_t)stream->source[index + 1];

    return WWM_RESULT_SUCCESS;
}

WowWorldResult wwm_read_uint32(WowWorldReader* stream, uint32_t* value)
{
    const size_t index = WWM_CHECK_LENGTH(4);

    *value = (uint32_t)stream->source[index] | (uint32_t)stream->source[index + 1] << 8 |
        (uint32_t)stream->source[index + 2] << 16 | (uint32_t)stream->source[index + 3] << 24;

    return WWM_RESULT_SUCCESS;
}

WowWorldResult wwm_read_uint64(WowWorldReader* stream, uint64_t* value)
{
    const size_t index = WWM_CHECK_LENGTH(8);

    *value = stream->source[index] | (uint64_t)stream->source[index + 1] << 8 |
        (uint64_t)stream->source[index + 2] << 16 | (uint64_t)stream->source[index + 3] << 24 |
        (uint64_t)stream->source[index + 4] << 32 | (uint64_t)stream->source[index + 5] << 40 |
        (uint64_t)stream->source[index + 6] << 48 | (uint64_t)stream->source[index + 7] << 56;

    return WWM_RESULT_SUCCESS;
}

WowWorldResult wwm_read_int32(WowWorldReader* stream, int32_t* value)
{
    const size_t index = WWM_CHECK_LENGTH(4);

    *value = (int32_t)stream->source[index] | (int32_t)stream->source[index + 1] << 8 |
        (int32_t)stream->source[index + 2] << 16 | (int32_t)stream->source[index + 3] << 24;

    return WWM_RESULT_SUCCESS;
}

WowWorldResult wwm_write_uint8(WowWorldWriter* stream, const uint8_t value)
{
    const size_t index = WWM_CHECK_LENGTH(1);

    stream->destination[index] = (char)value;

    return WWM_RESULT_SUCCESS;
}

WowWorldResult wwm_write_uint16(WowWorldWriter* stream, const uint16_t value)
{
    const size_t index = WWM_CHECK_LENGTH(2);

    stream->destination[index] = (char)value;
    stream->destination[index + 1] = (char)(value >> 8);

    return WWM_RESULT_SUCCESS;
}

WowWorldResult wwm_write_uint16_be(WowWorldWriter* stream, const uint16_t value)
{
    const size_t index = WWM_CHECK_LENGTH(2);

    stream->destination[index] = (unsigned char)(value >> 8);
    stream->destination[index + 1] = (unsigned char)value;

    return WWM_RESULT_SUCCESS;
}

WowWorldResult wwm_write_uint32(WowWorldWriter* stream, const uint32_t value)
{
    const size_t index = WWM_CHECK_LENGTH(4);

    stream->destination[index] = (char)value;
    stream->destination[index + 1] = (char)(value >> 8);
    stream->destination[index + 2] = (char)(value >> 16);
    stream->destination[index + 3] = (char)(value >> 24);

    return WWM_RESULT_SUCCESS;
}

WowWorldResult wwm_write_uint64(WowWorldWriter* stream, const uint64_t value)
{
    const size_t index = WWM_CHECK_LENGTH(8);

    stream->destination[index] = (char)value;
    stream->destination[index + 1] = (char)(value >> 8);
    stream->destination[index + 2] = (char)(value >> 16);
    stream->destination[index + 3] = (char)(value >> 24);
    stream->destination[index + 4] = (char)(value >> 32);
    stream->destination[index + 5] = (char)(value >> 40);
    stream->destination[index + 6] = (char)(value >> 48);
    stream->destination[index + 7] = (char)(value >> 56);

    return WWM_RESULT_SUCCESS;
}

WowWorldResult wwm_write_int32(WowWorldWriter* stream, const int32_t value)
{
    const size_t index = WWM_CHECK_LENGTH(4);

    stream->destination[index] = (char)value;
    stream->destination[index + 1] = (char)(value >> 8);
    stream->destination[index + 2] = (char)(value >> 16);
    stream->destination[index + 3] = (char)(value >> 24);

    return WWM_RESULT_SUCCESS;
}

WowWorldResult wwm_read_string(WowWorldReader* stream, WowWorldString* string)
{
    size_t index = WWM_CHECK_LENGTH(1);
    string->length = stream->source[index];

    index = WWM_CHECK_LENGTH(string->length);

    string->string = malloc(string->length + 1);

    memcpy(string->string, &stream->source[index], string->length);
    string->string[string->length] = '\0';

    return WWM_RESULT_SUCCESS;
}

WowWorldResult wwm_write_string(WowWorldWriter* stream, const WowWorldString* string)
{
    const size_t index = WWM_CHECK_LENGTH(1 + string->length);

    stream->destination[index] = (uint8_t)string->length;
    memcpy(&stream->destination[index + 1], string->string, string->length);

    return WWM_RESULT_SUCCESS;
}

WowWorldResult wwm_read_cstring(WowWorldReader* stream, WowWorldString* string)
{
    const unsigned char* const start = &stream->source[stream->index];
    size_t index = WWM_CHECK_LENGTH(1);
    string->length = 0;

    while (stream->source[index] != '\0')
    {
        index = WWM_CHECK_LENGTH(1);
        string->length++;
    }

    string->string = malloc(string->length + 1);

    memcpy(string->string, start, string->length);
    string->string[string->length] = '\0';

    return WWM_RESULT_SUCCESS;
}

WowWorldResult wwm_write_cstring(WowWorldWriter* stream, const WowWorldString* string)
{
    const size_t index = WWM_CHECK_LENGTH(1 + string->length);

    memcpy(&stream->destination[index], string->string, string->length + 1);

    return WWM_RESULT_SUCCESS;
}

WowWorldResult wwm_read_sized_cstring(WowWorldReader* stream, WowWorldString* string)
{
    size_t index;

    WWM_CHECK_RETURN_CODE(wwm_read_uint32(stream, &string->length));

    index = WWM_CHECK_LENGTH(string->length);

    string->string = malloc(string->length + 1);
    memcpy(string->string, &stream->source[index], string->length);
    string->string[string->length] = '\0';

    return WWM_RESULT_SUCCESS;
}

WowWorldResult wwm_write_sized_cstring(WowWorldWriter* stream, const WowWorldString* string)
{
    size_t index;
    WWM_CHECK_RETURN_CODE(wwm_write_uint32(stream, string->length));

    index = WWM_CHECK_LENGTH(string->length + 1);

    memcpy(&stream->destination[index], string->string, string->length + 1);


    return WWM_RESULT_SUCCESS;
}

WowWorldResult wwm_read_float(WowWorldReader* stream, float* value)
{
    uint32_t v;
    const WowWorldResult result = wwm_read_uint32(stream, &v);

    memcpy(value, &v, sizeof(uint32_t));

    return result;
}

WowWorldResult wwm_write_float(WowWorldWriter* stream, const float value)
{
    uint32_t v;
    memcpy(&v, &value, sizeof(uint32_t));

    return wwm_write_uint32(stream, v);
}

WowWorldResult wwm_read_bool8(WowWorldReader* stream, bool* value)
{
    uint8_t v;

    WWM_CHECK_RETURN_CODE(wwm_read_uint8(stream, &v));

    *value = v == 1 ? true : false;

    return WWM_RESULT_SUCCESS;
}

WowWorldResult wwm_write_bool8(WowWorldWriter* stream, const bool value)
{
    const uint8_t v = value ? 1 : 0;

    WWM_CHECK_RETURN_CODE(wwm_write_uint8(stream, v));

    return WWM_RESULT_SUCCESS;
}

WowWorldResult wwm_read_bool32(WowWorldReader* stream, bool* value)
{
    uint32_t v;

    WWM_CHECK_RETURN_CODE(wwm_read_uint32(stream, &v));

    *value = v == 1 ? true : false;

    return WWM_RESULT_SUCCESS;
}

WowWorldResult wwm_write_bool32(WowWorldWriter* stream, const bool value)
{
    const uint32_t v = value ? 1 : 0;

    WWM_CHECK_RETURN_CODE(wwm_write_uint32(stream, v));

    return WWM_RESULT_SUCCESS;
}

void wwm_free_string(WowWorldString* string)
{
    free(string->string);

    string->string = NULL;
    string->length = 0;
}

size_t wwm_packed_guid_size(const uint64_t value)
{
    int i;
    size_t size = 1; /* first byte */

    for (i = 0; i < 8; ++i)
    {
        const uint8_t v = (value >> (i * 8)) & 0xFF;
        if (v != 0)
        {
            size += 1;
        }
    }

    return size;
}

WowWorldResult wwm_read_packed_guid(WowWorldReader* stream, uint64_t* value)
{
    int i;
    uint8_t header;
    size_t index = WWM_CHECK_LENGTH(1);

    header = stream->source[index];
    *value = 0;

    for (i = 0; i < 8; ++i)
    {
        const bool byte_has_value = (header & (1 << i)) != 0;
        if (byte_has_value)
        {
            index = WWM_CHECK_LENGTH(1);
            *value |= stream->source[index] << i * 8;
        }
    }

    return WWM_RESULT_SUCCESS;
}

WowWorldResult wwm_write_packed_guid(WowWorldWriter* stream, uint64_t value)
{
    int i;
    uint8_t output[8];
    int output_index = 0;
    uint8_t header = 0;
    const size_t index = WWM_CHECK_LENGTH(1);

    for (i = 0; i < 8; ++i)
    {
        const uint8_t v = (value >> i * 8) & 0xff;
        const bool byte_has_value = v != 0;
        if (byte_has_value)
        {
            (void)WWM_CHECK_LENGTH(1);
            output[output_index++] = v;
            header |= 1 << i;
        }
    }

    stream->destination[index] = header;
    for (i = 0; i < output_index; ++i)
    {
        stream->destination[index + 1 + i] = output[i];
    }

    return WWM_RESULT_SUCCESS;
}

static uint32_t to_packed_vector3d(const all_Vector3d* v)
{
    uint32_t packed = 0;
    packed |= ((uint32_t)(v->x / 0.25f) & 0x7FF);
    packed |= ((uint32_t)(v->y / 0.25f) & 0x7FF) << 11;
    packed |= ((uint32_t)(v->z / 0.25f) & 0x3FF) << 22;
    return packed;
}

static void from_packed(uint32_t p, all_Vector3d* vector)
{
    vector->x = (float)((p & 0x7FF) / 4);
    vector->y = (float)(((p >> 11) & 0x7FF) / 4);
    vector->z = (float)(((p >> 22) & 0x3FF) / 4);
}

WowWorldResult all_Vector3d_read(WowWorldReader* reader, all_Vector3d* object);

WowWorldResult all_Vector3d_write(WowWorldWriter* writer, const all_Vector3d* object);

WowWorldResult wwm_read_monster_move_spline(WowWorldReader* stream, MonsterMoveSpline* value)
{
    uint32_t i;
    WWM_CHECK_RETURN_CODE(wwm_read_uint32(stream, &value->amount_of_splines));

    if (value->amount_of_splines == 0)
    {
        value->splines = NULL;
        return WWM_RESULT_SUCCESS;
    }

    value->splines = malloc(12 * value->amount_of_splines);

    WWM_CHECK_RETURN_CODE(all_Vector3d_read(stream, &value->splines[0]));

    for (i = 1; i < value->amount_of_splines; ++i)
    {
        uint32_t packed;
        WWM_CHECK_RETURN_CODE(wwm_read_uint32(stream, &packed));
        from_packed(packed, &value->splines[i]);
    }

    return WWM_RESULT_SUCCESS;
}

WowWorldResult wwm_write_monster_move_spline(WowWorldWriter* stream, const MonsterMoveSpline* value)
{
    uint32_t i;

    WWM_CHECK_RETURN_CODE(wwm_write_uint32(stream, value->amount_of_splines));

    if (value->amount_of_splines == 0)
    {
        return WWM_RESULT_SUCCESS;
    }

    WWM_CHECK_RETURN_CODE(all_Vector3d_write(stream, &value->splines[0]));

    for (i = 1; i < value->amount_of_splines; ++i)
    {
        const uint32_t packed = to_packed_vector3d(&value->splines[i]);
        WWM_CHECK_RETURN_CODE(wwm_write_uint32(stream, packed));
    }

    return WWM_RESULT_SUCCESS;
}

size_t wwm_monster_move_spline_size(const MonsterMoveSpline* value)
{
    const size_t size = 4; /* header uint32_t */

    if (value->amount_of_splines == 0)
    {
        return size;
    }

    return size + 12 + (4 * (value->amount_of_splines - 1));
}

void wwm_monster_move_spline_free(const MonsterMoveSpline* value)
{
    free(value->splines);
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldReader wwm_create_reader(const unsigned char* const source, const size_t length)
{
    WowWorldReader reader;
    reader.source = source;
    reader.length = length;
    reader.index = 0;
    return reader;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldWriter wwm_create_writer(unsigned char* const destination, const size_t length)
{
    WowWorldWriter writer;
    writer.destination = destination;
    writer.length = length;
    writer.index = 0;
    return writer;
}

WOW_WORLD_MESSAGES_C_EXPORT const char* wwm_error_code_to_string(const WowWorldResult result)
{
    switch (result)
    {
        case WWM_RESULT_SUCCESS:
            return "Success";
        case WWM_RESULT_NOT_ENOUGH_BYTES:
            return "Not enough bytes";
    }

    return "";
}

uint32_t wwm_adler32(const unsigned char *data, const size_t len)
{

    uint32_t a = 1, b = 0;
    size_t index;

#define MOD_ADLER 65521
    for (index = 0; index < len; ++index)
    {
        a = (a + data[index]) % MOD_ADLER;
        b = (b + a) % MOD_ADLER;
    }
#undef MOD_ADLER

    return (b << 16) | a;
}

size_t wwm_compress_data(const unsigned char* src, const size_t src_length, unsigned char* dst, const size_t dst_length)
{
    size_t index = 0;
    uint32_t adler32;

    if (dst_length < (src_length + WWM_COMPRESS_EXTRA_LENGTH))
    {
        return 0;
    }

    /* zlib header */
    dst[index] = 0x78;
    index += 1;
    dst[index] = 0x01;
    index += 1;

    dst[index] = 0x01; /* BFINAL and no compression */
    index += 1;

    dst[index] = (unsigned char)src_length; /* LEN */
    index += 1;
    dst[index] = (unsigned char)(src_length >> 8); /* LEN */
    index += 1;

    dst[index] = ((unsigned char)src_length) ^ 0xff; /* NLEN */
    index += 1;
    dst[index] = ((unsigned char)(src_length >> 8)) ^ 0xff; /* NLEN */
    index += 1;

    memcpy(&dst[index], src, src_length);
    index += src_length;

    adler32 = wwm_adler32(src, src_length);

    dst[index] = (unsigned char)adler32;
    index += 1;
    dst[index] = (unsigned char)(adler32 >> 8);
    index += 1;
    dst[index] = (unsigned char)(adler32 >> 16);
    index += 1;
    dst[index] = (unsigned char)(adler32 >> 24);

    return index;
}

bool wwm_decompress_data(const unsigned char* src, const size_t src_length, unsigned char* dst, const size_t dst_length)
{
    unsigned long source_length = (unsigned long)(src_length - 2);
    unsigned long destination_length = (unsigned long)dst_length;

    /* Skip zlib header */
    const int ret = puff(dst, &destination_length, src + 2, &source_length);
    if (ret != 0)
    {
        return false;
    }

    return true;
}