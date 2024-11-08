#ifndef WWM_WOW_WORLD_MESSAGES_H
#define WWM_WOW_WORLD_MESSAGES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "wow_world_messages/all.h"

#include "wowworldmessages_export.h"

typedef struct
{
    const unsigned char* const source;
    size_t length;
    size_t index;
} WowWorldReader;

typedef struct
{
    unsigned char* const destination;
    size_t length;
    size_t index;
} WowWorldWriter;

typedef enum
{
    WWM_RESULT_SUCCESS = 0,
    WWM_RESULT_NOT_ENOUGH_BYTES,
} WowWorldResult;

typedef struct
{
    uint32_t length;
    char* string;
} WowWorldString;

typedef struct
{
    uint32_t amount_of_splines;
    all_Vector3d* splines;
} MonsterMoveSpline;

WOWWORLDMESSAGES_EXPORT const char* wwm_error_code_to_string(WowWorldResult result);

WOWWORLDMESSAGES_EXPORT WowWorldReader wwm_create_reader(const unsigned char* const source, const size_t length);

WOWWORLDMESSAGES_EXPORT WowWorldWriter wwm_create_writer(unsigned char* const destination, const size_t length);

#endif /* WWM_WOW_WORLD_MESSAGES_H */
