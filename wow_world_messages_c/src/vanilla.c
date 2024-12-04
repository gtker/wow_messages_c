/* clang-format off */

#include "util.h"

#include "wow_world_messages/vanilla.h"

#include <string.h> /* memset */

/* forward declare all struct read/write */
WowWorldResult all_Vector3d_read(WowWorldReader* reader, all_Vector3d* object);
WowWorldResult all_Vector3d_write(WowWorldWriter* writer, const all_Vector3d* object);

/* forward declare all struct read/write */
WowWorldResult all_Vector2d_read(WowWorldReader* reader, all_Vector2d* object);
WowWorldResult all_Vector2d_write(WowWorldWriter* writer, const all_Vector2d* object);


WOW_WORLD_MESSAGES_C_EXPORT void vanilla_update_mask_set(vanilla_UpdateMask* mask, vanilla_UpdateMaskValues offset, uint32_t value) {
    uint32_t block = offset / 32;
    uint32_t bit = offset % 32;

    mask->headers[block] |= 1 << bit;
    mask->values[offset] = value;
}

WOW_WORLD_MESSAGES_C_EXPORT uint32_t vanilla_update_mask_get(vanilla_UpdateMask* mask, vanilla_UpdateMaskValues offset) {
    uint32_t block = offset / 32;
    uint32_t bit = offset % 32;

    if((mask->headers[block] & 1 << bit) != 0) {
        return mask->values[offset];
    }

    return 0;
}

static WowWorldResult vanilla_update_mask_write(WowWorldWriter* stream, const vanilla_UpdateMask* mask) {
    uint8_t i;
    uint8_t j;
    uint8_t amount_of_headers = 0;

    for (i = 0; i < VANILLA_HEADERS_LENGTH; ++i) {
        uint32_t header = mask->headers[i];
        if (header != 0) {
            amount_of_headers = i + 1;
        }
    }

    WWM_CHECK_RETURN_CODE(wwm_write_uint8(stream, amount_of_headers));

    for (i = 0; i < amount_of_headers; ++i) {
        WWM_CHECK_RETURN_CODE(wwm_write_uint32(stream, mask->headers[i]));
    }

    for (i = 0; i < amount_of_headers; ++i) {
        uint32_t header = mask->headers[i];
        for (j = 0; j < 32; ++j) {
            if ((header & (1 << j)) != 0) {
                WWM_CHECK_RETURN_CODE(wwm_write_uint32(stream, mask->values[i * 32 + j]));
            }
        }
    }

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_update_mask_read(WowWorldReader* stream, vanilla_UpdateMask* mask) {
    uint8_t i;
    uint8_t j;

    uint8_t amount_of_headers;

    memset(mask->headers, 0, sizeof(mask->headers));
    memset(mask->values, 0, sizeof(mask->values));

    WWM_CHECK_RETURN_CODE(wwm_read_uint8(stream, &amount_of_headers));

    for (i = 0; i < amount_of_headers; ++i) {
        WWM_CHECK_RETURN_CODE(wwm_read_uint32(stream, &mask->headers[i]));
    }

    for (i = 0; i < amount_of_headers; ++i) {
        uint32_t header = mask->headers[i];
        for (j = 0; j < 32; ++j) {
            if ((header & (1 << j)) != 0) {
                WWM_CHECK_RETURN_CODE(wwm_read_uint32(stream, &mask->values[i * 32 + j]));
            }
        }
    }

    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_update_mask_size(const vanilla_UpdateMask* mask) {
    size_t i;
    size_t j;
    size_t max_header = 0;
    size_t amount_of_values = 0;

    size_t size = 1; /* initial u8 */

    for(i = 0; i < VANILLA_HEADERS_LENGTH; ++i) {
        uint32_t header = mask->headers[i];
        for(j = 0; j < 32; ++j) {
            if((header & (1 << j)) != 0) {
                max_header = i + 1;
                amount_of_values += 4;
            }
        }
    }

    return size + amount_of_values + (max_header * 4);
}
static size_t vanilla_Addon_size(const vanilla_Addon* object) {
    size_t _size = 3;

    if (object->info_block == VANILLA_INFO_BLOCK_AVAILABLE) {
        _size += 5;

        if (object->key_version == VANILLA_KEY_VERSION_ONE|| object->key_version == VANILLA_KEY_VERSION_TWO|| object->key_version == VANILLA_KEY_VERSION_THREE|| object->key_version == VANILLA_KEY_VERSION_FOUR|| object->key_version == VANILLA_KEY_VERSION_FIVE|| object->key_version == VANILLA_KEY_VERSION_SIX|| object->key_version == VANILLA_KEY_VERSION_SEVEN|| object->key_version == VANILLA_KEY_VERSION_EIGHT|| object->key_version == VANILLA_KEY_VERSION_NINE) {
            _size += 256;
        }

    }

    if (object->url_info == VANILLA_URL_INFO_AVAILABLE) {
        _size += 1 + STRING_SIZE(object->url);
    }

    return _size;
}

static WowWorldResult vanilla_Addon_read(WowWorldReader* reader, vanilla_Addon* object) {
    object->addon_type = 0;
    READ_U8(object->addon_type);

    object->info_block = 0;
    READ_U8(object->info_block);

    if (object->info_block == VANILLA_INFO_BLOCK_AVAILABLE) {
        object->key_version = 0;
        READ_U8(object->key_version);

        if (object->key_version == VANILLA_KEY_VERSION_ONE|| object->key_version == VANILLA_KEY_VERSION_TWO|| object->key_version == VANILLA_KEY_VERSION_THREE|| object->key_version == VANILLA_KEY_VERSION_FOUR|| object->key_version == VANILLA_KEY_VERSION_FIVE|| object->key_version == VANILLA_KEY_VERSION_SIX|| object->key_version == VANILLA_KEY_VERSION_SEVEN|| object->key_version == VANILLA_KEY_VERSION_EIGHT|| object->key_version == VANILLA_KEY_VERSION_NINE) {
            READ_ARRAY_ALLOCATE(object->public_key, 256, sizeof(uint8_t));
            READ_ARRAY(object->public_key, 256, READ_U8((*object->public_key)[i]));

        }
        READ_U32(object->update_available_flag);

    }
    object->url_info = 0;
    READ_U8(object->url_info);

    if (object->url_info == VANILLA_URL_INFO_AVAILABLE) {
        READ_CSTRING(object->url);

    }
    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_Addon_write(WowWorldWriter* writer, const vanilla_Addon* object) {
    WRITE_U8(object->addon_type);

    WRITE_U8(object->info_block);

    if (object->info_block == VANILLA_INFO_BLOCK_AVAILABLE) {
        WRITE_U8(object->key_version);

        if (object->key_version == VANILLA_KEY_VERSION_ONE|| object->key_version == VANILLA_KEY_VERSION_TWO|| object->key_version == VANILLA_KEY_VERSION_THREE|| object->key_version == VANILLA_KEY_VERSION_FOUR|| object->key_version == VANILLA_KEY_VERSION_FIVE|| object->key_version == VANILLA_KEY_VERSION_SIX|| object->key_version == VANILLA_KEY_VERSION_SEVEN|| object->key_version == VANILLA_KEY_VERSION_EIGHT|| object->key_version == VANILLA_KEY_VERSION_NINE) {
            WRITE_ARRAY(object->public_key, 256, WRITE_U8((*object->public_key)[i]));

        }
        WRITE_U32(object->update_available_flag);

    }
    WRITE_U8(object->url_info);

    if (object->url_info == VANILLA_URL_INFO_AVAILABLE) {
        WRITE_CSTRING(object->url);

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_Addon_free(vanilla_Addon* object) {
    if (object->info_block == VANILLA_INFO_BLOCK_AVAILABLE) {
        if (object->key_version == VANILLA_KEY_VERSION_ONE|| object->key_version == VANILLA_KEY_VERSION_TWO|| object->key_version == VANILLA_KEY_VERSION_THREE|| object->key_version == VANILLA_KEY_VERSION_FOUR|| object->key_version == VANILLA_KEY_VERSION_FIVE|| object->key_version == VANILLA_KEY_VERSION_SIX|| object->key_version == VANILLA_KEY_VERSION_SEVEN|| object->key_version == VANILLA_KEY_VERSION_EIGHT|| object->key_version == VANILLA_KEY_VERSION_NINE) {
            free(object->public_key);
            object->public_key = NULL;
        }
    }
    if (object->url_info == VANILLA_URL_INFO_AVAILABLE) {
        FREE_STRING(object->url);

    }
}

static size_t vanilla_AddonInfo_size(const vanilla_AddonInfo* object) {
    return 10 + STRING_SIZE(object->addon_name);
}

static WowWorldResult vanilla_AddonInfo_read(WowWorldReader* reader, vanilla_AddonInfo* object) {
    READ_CSTRING(object->addon_name);

    READ_U8(object->addon_has_signature);

    READ_U32(object->addon_crc);

    READ_U32(object->addon_extra_crc);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_AddonInfo_write(WowWorldWriter* writer, const vanilla_AddonInfo* object) {
    WRITE_CSTRING(object->addon_name);

    WRITE_U8(object->addon_has_signature);

    WRITE_U32(object->addon_crc);

    WRITE_U32(object->addon_extra_crc);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_AddonInfo_free(vanilla_AddonInfo* object) {
    FREE_STRING(object->addon_name);

}

static WowWorldResult vanilla_AuctionListItem_read(WowWorldReader* reader, vanilla_AuctionListItem* object) {
    READ_U32(object->id);

    READ_U32(object->item);

    READ_U32(object->item_enchantment);

    READ_U32(object->item_random_property_id);

    READ_U32(object->item_suffix_factor);

    READ_U32(object->item_count);

    READ_U32(object->item_charges);

    READ_U64(object->item_owner);

    READ_U32(object->start_bid);

    READ_U32(object->minimum_bid);

    READ_U32(object->buyout_amount);

    READ_U32(object->time_left);

    READ_U64(object->highest_bidder);

    READ_U32(object->highest_bid);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_AuctionListItem_write(WowWorldWriter* writer, const vanilla_AuctionListItem* object) {
    WRITE_U32(object->id);

    WRITE_U32(object->item);

    WRITE_U32(object->item_enchantment);

    WRITE_U32(object->item_random_property_id);

    WRITE_U32(object->item_suffix_factor);

    WRITE_U32(object->item_count);

    WRITE_U32(object->item_charges);

    WRITE_U64(object->item_owner);

    WRITE_U32(object->start_bid);

    WRITE_U32(object->minimum_bid);

    WRITE_U32(object->buyout_amount);

    WRITE_U32(object->time_left);

    WRITE_U64(object->highest_bidder);

    WRITE_U32(object->highest_bid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_AuraLog_size(const vanilla_AuraLog* object) {
    size_t _size = 4;

    if (object->aura_type == VANILLA_AURA_TYPE_PERIODIC_DAMAGE|| object->aura_type == VANILLA_AURA_TYPE_PERIODIC_DAMAGE_PERCENT) {
        _size += 13;
    }
    else if (object->aura_type == VANILLA_AURA_TYPE_PERIODIC_HEAL|| object->aura_type == VANILLA_AURA_TYPE_OBS_MOD_HEALTH) {
        _size += 4;
    }
    else if (object->aura_type == VANILLA_AURA_TYPE_OBS_MOD_MANA|| object->aura_type == VANILLA_AURA_TYPE_PERIODIC_ENERGIZE) {
        _size += 8;
    }
    else if (object->aura_type == VANILLA_AURA_TYPE_PERIODIC_MANA_LEECH) {
        _size += 12;
    }

    return _size;
}

static WowWorldResult vanilla_AuraLog_read(WowWorldReader* reader, vanilla_AuraLog* object) {
    object->aura_type = 0;
    READ_U32(object->aura_type);

    if (object->aura_type == VANILLA_AURA_TYPE_PERIODIC_DAMAGE|| object->aura_type == VANILLA_AURA_TYPE_PERIODIC_DAMAGE_PERCENT) {
        READ_U32(object->damage1);

        object->school = 0;
        READ_U8(object->school);

        READ_U32(object->absorbed);

        READ_U32(object->resisted);

    }
    else if (object->aura_type == VANILLA_AURA_TYPE_PERIODIC_HEAL|| object->aura_type == VANILLA_AURA_TYPE_OBS_MOD_HEALTH) {
        READ_U32(object->damage2);

    }
    else if (object->aura_type == VANILLA_AURA_TYPE_OBS_MOD_MANA|| object->aura_type == VANILLA_AURA_TYPE_PERIODIC_ENERGIZE) {
        READ_U32(object->misc_value1);

        READ_U32(object->damage3);

    }
    else if (object->aura_type == VANILLA_AURA_TYPE_PERIODIC_MANA_LEECH) {
        READ_U32(object->misc_value2);

        READ_U32(object->damage);

        READ_FLOAT(object->gain_multiplier);

    }
    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_AuraLog_write(WowWorldWriter* writer, const vanilla_AuraLog* object) {
    WRITE_U32(object->aura_type);

    if (object->aura_type == VANILLA_AURA_TYPE_PERIODIC_DAMAGE|| object->aura_type == VANILLA_AURA_TYPE_PERIODIC_DAMAGE_PERCENT) {
        WRITE_U32(object->damage1);

        WRITE_U8(object->school);

        WRITE_U32(object->absorbed);

        WRITE_U32(object->resisted);

    }
    else if (object->aura_type == VANILLA_AURA_TYPE_PERIODIC_HEAL|| object->aura_type == VANILLA_AURA_TYPE_OBS_MOD_HEALTH) {
        WRITE_U32(object->damage2);

    }
    else if (object->aura_type == VANILLA_AURA_TYPE_OBS_MOD_MANA|| object->aura_type == VANILLA_AURA_TYPE_PERIODIC_ENERGIZE) {
        WRITE_U32(object->misc_value1);

        WRITE_U32(object->damage3);

    }
    else if (object->aura_type == VANILLA_AURA_TYPE_PERIODIC_MANA_LEECH) {
        WRITE_U32(object->misc_value2);

        WRITE_U32(object->damage);

        WRITE_FLOAT(object->gain_multiplier);

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_AuraLog_free(vanilla_AuraLog* object) {
    if (object->aura_type == VANILLA_AURA_TYPE_PERIODIC_DAMAGE|| object->aura_type == VANILLA_AURA_TYPE_PERIODIC_DAMAGE_PERCENT) {
    }
    else if (object->aura_type == VANILLA_AURA_TYPE_PERIODIC_HEAL|| object->aura_type == VANILLA_AURA_TYPE_OBS_MOD_HEALTH) {
    }
    else if (object->aura_type == VANILLA_AURA_TYPE_OBS_MOD_MANA|| object->aura_type == VANILLA_AURA_TYPE_PERIODIC_ENERGIZE) {
    }
    else if (object->aura_type == VANILLA_AURA_TYPE_PERIODIC_MANA_LEECH) {
    }
}

static size_t vanilla_BattlegroundPlayer_size(const vanilla_BattlegroundPlayer* object) {
    return 32 + 4 * object->amount_of_extra_fields;
}

static WowWorldResult vanilla_BattlegroundPlayer_read(WowWorldReader* reader, vanilla_BattlegroundPlayer* object) {
    READ_U64(object->player);

    object->rank = 0;
    READ_U32(object->rank);

    READ_U32(object->killing_blows);

    READ_U32(object->honorable_kills);

    READ_U32(object->deaths);

    READ_U32(object->bonus_honor);

    READ_U32(object->amount_of_extra_fields);

    READ_ARRAY_ALLOCATE(object->fields, object->amount_of_extra_fields, sizeof(uint32_t));
    READ_ARRAY(object->fields, object->amount_of_extra_fields, READ_U32(object->fields[i]));

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_BattlegroundPlayer_write(WowWorldWriter* writer, const vanilla_BattlegroundPlayer* object) {
    WRITE_U64(object->player);

    WRITE_U32(object->rank);

    WRITE_U32(object->killing_blows);

    WRITE_U32(object->honorable_kills);

    WRITE_U32(object->deaths);

    WRITE_U32(object->bonus_honor);

    WRITE_U32(object->amount_of_extra_fields);

    WRITE_ARRAY(object->fields, object->amount_of_extra_fields, WRITE_U32(object->fields[i]));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_BattlegroundPlayer_free(vanilla_BattlegroundPlayer* object) {
    free(object->fields);
    object->fields = NULL;
}

static WowWorldResult vanilla_BattlegroundPlayerPosition_read(WowWorldReader* reader, vanilla_BattlegroundPlayerPosition* object) {
    READ_U64(object->player);

    READ_FLOAT(object->position_x);

    READ_FLOAT(object->position_y);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_BattlegroundPlayerPosition_write(WowWorldWriter* writer, const vanilla_BattlegroundPlayerPosition* object) {
    WRITE_U64(object->player);

    WRITE_FLOAT(object->position_x);

    WRITE_FLOAT(object->position_y);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_ChannelMember_read(WowWorldReader* reader, vanilla_ChannelMember* object) {
    READ_U64(object->guid);

    object->member_flags = 0;
    READ_U8(object->member_flags);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_ChannelMember_write(WowWorldWriter* writer, const vanilla_ChannelMember* object) {
    WRITE_U64(object->guid);

    WRITE_U8(object->member_flags);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CharacterGear_read(WowWorldReader* reader, vanilla_CharacterGear* object) {
    READ_U32(object->equipment_display_id);

    object->inventory_type = 0;
    READ_U8(object->inventory_type);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CharacterGear_write(WowWorldWriter* writer, const vanilla_CharacterGear* object) {
    WRITE_U32(object->equipment_display_id);

    WRITE_U8(object->inventory_type);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_Character_size(const vanilla_Character* object) {
    return 159 + STRING_SIZE(object->name);
}

static WowWorldResult vanilla_Character_read(WowWorldReader* reader, vanilla_Character* object) {
    READ_U64(object->guid);

    READ_CSTRING(object->name);

    object->race = 0;
    READ_U8(object->race);

    object->class_type = 0;
    READ_U8(object->class_type);

    object->gender = 0;
    READ_U8(object->gender);

    READ_U8(object->skin);

    READ_U8(object->face);

    READ_U8(object->hair_style);

    READ_U8(object->hair_color);

    READ_U8(object->facial_hair);

    READ_U8(object->level);

    object->area = 0;
    READ_U32(object->area);

    object->map = 0;
    READ_U32(object->map);

    WWM_CHECK_RETURN_CODE(all_Vector3d_read(reader, &object->position));

    READ_U32(object->guild_id);

    object->flags = 0;
    READ_U32(object->flags);

    READ_BOOL8(object->first_login);

    READ_U32(object->pet_display_id);

    READ_U32(object->pet_level);

    object->pet_family = 0;
    READ_U32(object->pet_family);

    READ_ARRAY_ALLOCATE(object->equipment, 19, sizeof(vanilla_CharacterGear));
    READ_ARRAY(object->equipment, 19, WWM_CHECK_RETURN_CODE(vanilla_CharacterGear_read(reader, &(*object->equipment)[i])));

    SKIP_FORWARD_BYTES(4);

    SKIP_FORWARD_BYTES(1);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_Character_write(WowWorldWriter* writer, const vanilla_Character* object) {
    WRITE_U64(object->guid);

    WRITE_CSTRING(object->name);

    WRITE_U8(object->race);

    WRITE_U8(object->class_type);

    WRITE_U8(object->gender);

    WRITE_U8(object->skin);

    WRITE_U8(object->face);

    WRITE_U8(object->hair_style);

    WRITE_U8(object->hair_color);

    WRITE_U8(object->facial_hair);

    WRITE_U8(object->level);

    WRITE_U32(object->area);

    WRITE_U32(object->map);

    WWM_CHECK_RETURN_CODE(all_Vector3d_write(writer, &object->position));

    WRITE_U32(object->guild_id);

    WRITE_U32(object->flags);

    WRITE_BOOL8(object->first_login);

    WRITE_U32(object->pet_display_id);

    WRITE_U32(object->pet_level);

    WRITE_U32(object->pet_family);

    WRITE_ARRAY(object->equipment, 19, WWM_CHECK_RETURN_CODE(vanilla_CharacterGear_write(writer, &(*object->equipment)[i])));

    WRITE_U32(0);

    WRITE_U8(0);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_Character_free(vanilla_Character* object) {
    FREE_STRING(object->name);

    free(object->equipment);
    object->equipment = NULL;
}

static size_t vanilla_MonsterMove_size(const vanilla_MonsterMove* object) {
    size_t _size = 17;

    if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_TARGET) {
        _size += 8;
    }
    else if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_ANGLE) {
        _size += 4;
    }
    else if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_SPOT) {
        _size += 12;
    }

    if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_NORMAL|| object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_SPOT|| object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_TARGET|| object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_ANGLE) {
        _size += 8 + wwm_monster_move_spline_size(&object->splines);
    }

    return _size;
}

static WowWorldResult vanilla_MonsterMove_read(WowWorldReader* reader, vanilla_MonsterMove* object) {
    WWM_CHECK_RETURN_CODE(all_Vector3d_read(reader, &object->spline_point));

    READ_U32(object->spline_id);

    object->move_type = 0;
    READ_U8(object->move_type);

    if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_TARGET) {
        READ_U64(object->target);

    }
    else if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_ANGLE) {
        READ_FLOAT(object->angle);

    }
    else if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_SPOT) {
        WWM_CHECK_RETURN_CODE(all_Vector3d_read(reader, &object->position));

    }
    if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_NORMAL|| object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_SPOT|| object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_TARGET|| object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_ANGLE) {
        object->spline_flags = 0;
        READ_U32(object->spline_flags);

        READ_U32(object->duration);

        READ_MONSTER_MOVE_SPLINE(object->splines);

    }
    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_MonsterMove_write(WowWorldWriter* writer, const vanilla_MonsterMove* object) {
    WWM_CHECK_RETURN_CODE(all_Vector3d_write(writer, &object->spline_point));

    WRITE_U32(object->spline_id);

    WRITE_U8(object->move_type);

    if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_TARGET) {
        WRITE_U64(object->target);

    }
    else if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_ANGLE) {
        WRITE_FLOAT(object->angle);

    }
    else if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_SPOT) {
        WWM_CHECK_RETURN_CODE(all_Vector3d_write(writer, &object->position));

    }
    if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_NORMAL|| object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_SPOT|| object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_TARGET|| object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_ANGLE) {
        WRITE_U32(object->spline_flags);

        WRITE_U32(object->duration);

        WRITE_MONSTER_MOVE_SPLINE(object->splines);

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MonsterMove_free(vanilla_MonsterMove* object) {
    if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_TARGET) {
    }
    else if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_ANGLE) {
    }
    else if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_SPOT) {
    }
    if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_NORMAL|| object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_SPOT|| object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_TARGET|| object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_ANGLE) {
        wwm_monster_move_spline_free(&object->splines);

    }
}

static size_t vanilla_CompressedMove_size(const vanilla_CompressedMove* object) {
    size_t _size = 3 + wwm_packed_guid_size(object->guid);

    if (object->opcode == VANILLA_COMPRESSED_MOVE_OPCODE_SMSG_SPLINE_SET_RUN_SPEED) {
        _size += 4;
    }
    else if (object->opcode == VANILLA_COMPRESSED_MOVE_OPCODE_SMSG_MONSTER_MOVE) {
        _size += 0 + vanilla_MonsterMove_size(&object->monster_move);
    }
    else if (object->opcode == VANILLA_COMPRESSED_MOVE_OPCODE_SMSG_MONSTER_MOVE_TRANSPORT) {
        _size += 0 + wwm_packed_guid_size(object->transport) + vanilla_MonsterMove_size(&object->monster_move_transport);
    }

    return _size;
}

static WowWorldResult vanilla_CompressedMove_read(WowWorldReader* reader, vanilla_CompressedMove* object) {
    SKIP_FORWARD_BYTES(1);

    object->opcode = 0;
    READ_U16(object->opcode);

    READ_PACKED_GUID(object->guid);

    if (object->opcode == VANILLA_COMPRESSED_MOVE_OPCODE_SMSG_SPLINE_SET_RUN_SPEED) {
        READ_FLOAT(object->speed);

    }
    else if (object->opcode == VANILLA_COMPRESSED_MOVE_OPCODE_SMSG_MONSTER_MOVE) {
        WWM_CHECK_RETURN_CODE(vanilla_MonsterMove_read(reader, &object->monster_move));

    }
    else if (object->opcode == VANILLA_COMPRESSED_MOVE_OPCODE_SMSG_MONSTER_MOVE_TRANSPORT) {
        READ_PACKED_GUID(object->transport);

        WWM_CHECK_RETURN_CODE(vanilla_MonsterMove_read(reader, &object->monster_move_transport));

    }
    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CompressedMove_write(WowWorldWriter* writer, const vanilla_CompressedMove* object) {
    WRITE_U8((uint8_t)vanilla_CompressedMove_size(object));

    WRITE_U16(object->opcode);

    WRITE_PACKED_GUID(object->guid);

    if (object->opcode == VANILLA_COMPRESSED_MOVE_OPCODE_SMSG_SPLINE_SET_RUN_SPEED) {
        WRITE_FLOAT(object->speed);

    }
    else if (object->opcode == VANILLA_COMPRESSED_MOVE_OPCODE_SMSG_MONSTER_MOVE) {
        WWM_CHECK_RETURN_CODE(vanilla_MonsterMove_write(writer, &object->monster_move));

    }
    else if (object->opcode == VANILLA_COMPRESSED_MOVE_OPCODE_SMSG_MONSTER_MOVE_TRANSPORT) {
        WRITE_PACKED_GUID(object->transport);

        WWM_CHECK_RETURN_CODE(vanilla_MonsterMove_write(writer, &object->monster_move_transport));

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CompressedMove_free(vanilla_CompressedMove* object) {
    if (object->opcode == VANILLA_COMPRESSED_MOVE_OPCODE_SMSG_SPLINE_SET_RUN_SPEED) {
    }
    else if (object->opcode == VANILLA_COMPRESSED_MOVE_OPCODE_SMSG_MONSTER_MOVE) {
        vanilla_MonsterMove_free(&object->monster_move);
    }
    else if (object->opcode == VANILLA_COMPRESSED_MOVE_OPCODE_SMSG_MONSTER_MOVE_TRANSPORT) {
        vanilla_MonsterMove_free(&object->monster_move_transport);
    }
}

static WowWorldResult vanilla_CooldownSpell_read(WowWorldReader* reader, vanilla_CooldownSpell* object) {
    READ_U16(object->spell_id);

    READ_U16(object->item_id);

    READ_U16(object->spell_category);

    READ_U32(object->cooldown);

    READ_U32(object->category_cooldown);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CooldownSpell_write(WowWorldWriter* writer, const vanilla_CooldownSpell* object) {
    WRITE_U16(object->spell_id);

    WRITE_U16(object->item_id);

    WRITE_U16(object->spell_category);

    WRITE_U32(object->cooldown);

    WRITE_U32(object->category_cooldown);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_DamageInfo_read(WowWorldReader* reader, vanilla_DamageInfo* object) {
    READ_U32(object->spell_school_mask);

    READ_FLOAT(object->damage_float);

    READ_U32(object->damage_uint);

    READ_U32(object->absorb);

    READ_U32(object->resist);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_DamageInfo_write(WowWorldWriter* writer, const vanilla_DamageInfo* object) {
    WRITE_U32(object->spell_school_mask);

    WRITE_FLOAT(object->damage_float);

    WRITE_U32(object->damage_uint);

    WRITE_U32(object->absorb);

    WRITE_U32(object->resist);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_FactionInitializer_read(WowWorldReader* reader, vanilla_FactionInitializer* object) {
    object->flag = 0;
    READ_U8(object->flag);

    READ_U32(object->standing);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_FactionInitializer_write(WowWorldWriter* writer, const vanilla_FactionInitializer* object) {
    WRITE_U8(object->flag);

    WRITE_U32(object->standing);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_FactionStanding_read(WowWorldReader* reader, vanilla_FactionStanding* object) {
    object->faction = 0;
    READ_U16(object->faction);

    READ_U32(object->standing);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_FactionStanding_write(WowWorldWriter* writer, const vanilla_FactionStanding* object) {
    WRITE_U16(object->faction);

    WRITE_U32(object->standing);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_ForcedReaction_read(WowWorldReader* reader, vanilla_ForcedReaction* object) {
    object->faction = 0;
    READ_U16(object->faction);

    READ_U32(object->reputation_rank);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_ForcedReaction_write(WowWorldWriter* writer, const vanilla_ForcedReaction* object) {
    WRITE_U16(object->faction);

    WRITE_U32(object->reputation_rank);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_Friend_size(const vanilla_Friend* object) {
    size_t _size = 9;

    if (object->status == VANILLA_FRIEND_STATUS_ONLINE|| object->status == VANILLA_FRIEND_STATUS_AFK|| object->status == VANILLA_FRIEND_STATUS_UNKNOWN3|| object->status == VANILLA_FRIEND_STATUS_DND) {
        _size += 12;
    }

    return _size;
}

static WowWorldResult vanilla_Friend_read(WowWorldReader* reader, vanilla_Friend* object) {
    READ_U64(object->guid);

    object->status = 0;
    READ_U8(object->status);

    if (object->status == VANILLA_FRIEND_STATUS_ONLINE|| object->status == VANILLA_FRIEND_STATUS_AFK|| object->status == VANILLA_FRIEND_STATUS_UNKNOWN3|| object->status == VANILLA_FRIEND_STATUS_DND) {
        object->area = 0;
        READ_U32(object->area);

        READ_U32(object->level);

        object->class_type = 0;
        READ_U32(object->class_type);

    }
    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_Friend_write(WowWorldWriter* writer, const vanilla_Friend* object) {
    WRITE_U64(object->guid);

    WRITE_U8(object->status);

    if (object->status == VANILLA_FRIEND_STATUS_ONLINE|| object->status == VANILLA_FRIEND_STATUS_AFK|| object->status == VANILLA_FRIEND_STATUS_UNKNOWN3|| object->status == VANILLA_FRIEND_STATUS_DND) {
        WRITE_U32(object->area);

        WRITE_U32(object->level);

        WRITE_U32(object->class_type);

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_Friend_free(vanilla_Friend* object) {
    if (object->status == VANILLA_FRIEND_STATUS_ONLINE|| object->status == VANILLA_FRIEND_STATUS_AFK|| object->status == VANILLA_FRIEND_STATUS_UNKNOWN3|| object->status == VANILLA_FRIEND_STATUS_DND) {
    }
}

static size_t vanilla_GmSurveyQuestion_size(const vanilla_GmSurveyQuestion* object) {
    return 6 + STRING_SIZE(object->comment);
}

static WowWorldResult vanilla_GmSurveyQuestion_read(WowWorldReader* reader, vanilla_GmSurveyQuestion* object) {
    READ_U32(object->question_id);

    READ_U8(object->answer);

    READ_CSTRING(object->comment);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_GmSurveyQuestion_write(WowWorldWriter* writer, const vanilla_GmSurveyQuestion* object) {
    WRITE_U32(object->question_id);

    WRITE_U8(object->answer);

    WRITE_CSTRING(object->comment);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_GmSurveyQuestion_free(vanilla_GmSurveyQuestion* object) {
    FREE_STRING(object->comment);

}

static size_t vanilla_GossipItem_size(const vanilla_GossipItem* object) {
    return 7 + STRING_SIZE(object->message);
}

static WowWorldResult vanilla_GossipItem_read(WowWorldReader* reader, vanilla_GossipItem* object) {
    READ_U32(object->id);

    READ_U8(object->item_icon);

    READ_BOOL8(object->coded);

    READ_CSTRING(object->message);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_GossipItem_write(WowWorldWriter* writer, const vanilla_GossipItem* object) {
    WRITE_U32(object->id);

    WRITE_U8(object->item_icon);

    WRITE_BOOL8(object->coded);

    WRITE_CSTRING(object->message);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_GossipItem_free(vanilla_GossipItem* object) {
    FREE_STRING(object->message);

}

static size_t vanilla_GroupListMember_size(const vanilla_GroupListMember* object) {
    return 11 + STRING_SIZE(object->name);
}

static WowWorldResult vanilla_GroupListMember_read(WowWorldReader* reader, vanilla_GroupListMember* object) {
    READ_CSTRING(object->name);

    READ_U64(object->guid);

    READ_BOOL8(object->is_online);

    READ_U8(object->flags);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_GroupListMember_write(WowWorldWriter* writer, const vanilla_GroupListMember* object) {
    WRITE_CSTRING(object->name);

    WRITE_U64(object->guid);

    WRITE_BOOL8(object->is_online);

    WRITE_U8(object->flags);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_GroupListMember_free(vanilla_GroupListMember* object) {
    FREE_STRING(object->name);

}

static size_t vanilla_GuildMember_size(const vanilla_GuildMember* object) {
    size_t _size = 22 + STRING_SIZE(object->name) + STRING_SIZE(object->public_note) + STRING_SIZE(object->officer_note);

    if (object->status == VANILLA_GUILD_MEMBER_STATUS_OFFLINE) {
        _size += 4;
    }

    return _size;
}

static WowWorldResult vanilla_GuildMember_read(WowWorldReader* reader, vanilla_GuildMember* object) {
    READ_U64(object->guid);

    object->status = 0;
    READ_U8(object->status);

    READ_CSTRING(object->name);

    READ_U32(object->rank);

    READ_U8(object->level);

    object->class_type = 0;
    READ_U8(object->class_type);

    object->area = 0;
    READ_U32(object->area);

    if (object->status == VANILLA_GUILD_MEMBER_STATUS_OFFLINE) {
        READ_FLOAT(object->time_offline);

    }
    READ_CSTRING(object->public_note);

    READ_CSTRING(object->officer_note);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_GuildMember_write(WowWorldWriter* writer, const vanilla_GuildMember* object) {
    WRITE_U64(object->guid);

    WRITE_U8(object->status);

    WRITE_CSTRING(object->name);

    WRITE_U32(object->rank);

    WRITE_U8(object->level);

    WRITE_U8(object->class_type);

    WRITE_U32(object->area);

    if (object->status == VANILLA_GUILD_MEMBER_STATUS_OFFLINE) {
        WRITE_FLOAT(object->time_offline);

    }
    WRITE_CSTRING(object->public_note);

    WRITE_CSTRING(object->officer_note);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_GuildMember_free(vanilla_GuildMember* object) {
    FREE_STRING(object->name);

    if (object->status == VANILLA_GUILD_MEMBER_STATUS_OFFLINE) {
    }
    FREE_STRING(object->public_note);

    FREE_STRING(object->officer_note);

}

static WowWorldResult vanilla_InitialSpell_read(WowWorldReader* reader, vanilla_InitialSpell* object) {
    READ_U16(object->spell_id);

    READ_U16(object->unknown1);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_InitialSpell_write(WowWorldWriter* writer, const vanilla_InitialSpell* object) {
    WRITE_U16(object->spell_id);

    WRITE_U16(object->unknown1);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_ItemDamageType_read(WowWorldReader* reader, vanilla_ItemDamageType* object) {
    READ_FLOAT(object->damage_minimum);

    READ_FLOAT(object->damage_maximum);

    object->school = 0;
    READ_U32(object->school);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_ItemDamageType_write(WowWorldWriter* writer, const vanilla_ItemDamageType* object) {
    WRITE_FLOAT(object->damage_minimum);

    WRITE_FLOAT(object->damage_maximum);

    WRITE_U32(object->school);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_ItemSpells_read(WowWorldReader* reader, vanilla_ItemSpells* object) {
    READ_U32(object->spell);

    object->spell_trigger = 0;
    READ_U32(object->spell_trigger);

    READ_I32(object->spell_charges);

    READ_I32(object->spell_cooldown);

    READ_U32(object->spell_category);

    READ_I32(object->spell_category_cooldown);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_ItemSpells_write(WowWorldWriter* writer, const vanilla_ItemSpells* object) {
    WRITE_U32(object->spell);

    WRITE_U32(object->spell_trigger);

    WRITE_I32(object->spell_charges);

    WRITE_I32(object->spell_cooldown);

    WRITE_U32(object->spell_category);

    WRITE_I32(object->spell_category_cooldown);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_ItemStat_read(WowWorldReader* reader, vanilla_ItemStat* object) {
    object->stat_type = 0;
    READ_U32(object->stat_type);

    READ_I32(object->value);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_ItemStat_write(WowWorldWriter* writer, const vanilla_ItemStat* object) {
    WRITE_U32(object->stat_type);

    WRITE_I32(object->value);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_ListInventoryItem_read(WowWorldReader* reader, vanilla_ListInventoryItem* object) {
    READ_U32(object->item_stack_count);

    READ_U32(object->item);

    READ_U32(object->item_display_id);

    READ_U32(object->max_items);

    READ_U32(object->price);

    READ_U32(object->max_durability);

    READ_U32(object->durability);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_ListInventoryItem_write(WowWorldWriter* writer, const vanilla_ListInventoryItem* object) {
    WRITE_U32(object->item_stack_count);

    WRITE_U32(object->item);

    WRITE_U32(object->item_display_id);

    WRITE_U32(object->max_items);

    WRITE_U32(object->price);

    WRITE_U32(object->max_durability);

    WRITE_U32(object->durability);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_LootItem_read(WowWorldReader* reader, vanilla_LootItem* object) {
    READ_U8(object->index);

    READ_U32(object->item);

    object->ty = 0;
    READ_U8(object->ty);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_LootItem_write(WowWorldWriter* writer, const vanilla_LootItem* object) {
    WRITE_U8(object->index);

    WRITE_U32(object->item);

    WRITE_U8(object->ty);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_Mail_size(const vanilla_Mail* object) {
    size_t _size = 67 + STRING_SIZE(object->subject);

    if (object->message_type == VANILLA_MAIL_TYPE_NORMAL) {
        _size += 8;
    }
    else if (object->message_type == VANILLA_MAIL_TYPE_CREATURE|| object->message_type == VANILLA_MAIL_TYPE_GAMEOBJECT) {
        _size += 4;
    }
    else if (object->message_type == VANILLA_MAIL_TYPE_AUCTION) {
        _size += 4;
    }

    return _size;
}

static WowWorldResult vanilla_Mail_read(WowWorldReader* reader, vanilla_Mail* object) {
    READ_U32(object->message_id);

    object->message_type = 0;
    READ_U8(object->message_type);

    if (object->message_type == VANILLA_MAIL_TYPE_NORMAL) {
        READ_U64(object->sender);

    }
    else if (object->message_type == VANILLA_MAIL_TYPE_CREATURE|| object->message_type == VANILLA_MAIL_TYPE_GAMEOBJECT) {
        READ_U32(object->sender_id);

    }
    else if (object->message_type == VANILLA_MAIL_TYPE_AUCTION) {
        READ_U32(object->auction_id);

    }
    READ_CSTRING(object->subject);

    READ_U32(object->item_text_id);

    READ_U32(object->unknown1);

    READ_U32(object->stationery);

    READ_U32(object->item);

    READ_U32(object->item_enchant_id);

    READ_U32(object->item_random_property_id);

    READ_U32(object->item_suffix_factor);

    READ_U8(object->item_stack_size);

    READ_U32(object->item_spell_charges);

    READ_U32(object->max_durability);

    READ_U32(object->durability);

    READ_U32(object->money);

    READ_U32(object->cash_on_delivery_amount);

    READ_U32(object->checked_timestamp);

    READ_FLOAT(object->expiration_time);

    READ_U32(object->mail_template_id);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_Mail_write(WowWorldWriter* writer, const vanilla_Mail* object) {
    WRITE_U32(object->message_id);

    WRITE_U8(object->message_type);

    if (object->message_type == VANILLA_MAIL_TYPE_NORMAL) {
        WRITE_U64(object->sender);

    }
    else if (object->message_type == VANILLA_MAIL_TYPE_CREATURE|| object->message_type == VANILLA_MAIL_TYPE_GAMEOBJECT) {
        WRITE_U32(object->sender_id);

    }
    else if (object->message_type == VANILLA_MAIL_TYPE_AUCTION) {
        WRITE_U32(object->auction_id);

    }
    WRITE_CSTRING(object->subject);

    WRITE_U32(object->item_text_id);

    WRITE_U32(object->unknown1);

    WRITE_U32(object->stationery);

    WRITE_U32(object->item);

    WRITE_U32(object->item_enchant_id);

    WRITE_U32(object->item_random_property_id);

    WRITE_U32(object->item_suffix_factor);

    WRITE_U8(object->item_stack_size);

    WRITE_U32(object->item_spell_charges);

    WRITE_U32(object->max_durability);

    WRITE_U32(object->durability);

    WRITE_U32(object->money);

    WRITE_U32(object->cash_on_delivery_amount);

    WRITE_U32(object->checked_timestamp);

    WRITE_FLOAT(object->expiration_time);

    WRITE_U32(object->mail_template_id);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_Mail_free(vanilla_Mail* object) {
    if (object->message_type == VANILLA_MAIL_TYPE_NORMAL) {
    }
    else if (object->message_type == VANILLA_MAIL_TYPE_CREATURE|| object->message_type == VANILLA_MAIL_TYPE_GAMEOBJECT) {
    }
    else if (object->message_type == VANILLA_MAIL_TYPE_AUCTION) {
    }
    FREE_STRING(object->subject);

}

static size_t vanilla_MovementBlock_size(const vanilla_MovementBlock* object) {
    size_t _size = 1;

    if ((object->update_flag & VANILLA_UPDATE_FLAG_LIVING) != 0) {
        _size += 52;

        if ((object->flags & VANILLA_MOVEMENT_FLAGS_ON_TRANSPORT) != 0) {
            _size += 16 + wwm_packed_guid_size(object->transport_guid);
        }

        if ((object->flags & VANILLA_MOVEMENT_FLAGS_SWIMMING) != 0) {
            _size += 4;
        }

        if ((object->flags & VANILLA_MOVEMENT_FLAGS_JUMPING) != 0) {
            _size += 16;
        }

        if ((object->flags & VANILLA_MOVEMENT_FLAGS_SPLINE_ELEVATION) != 0) {
            _size += 4;
        }

        if ((object->flags & VANILLA_MOVEMENT_FLAGS_SPLINE_ENABLED) != 0) {
            _size += 32 + 12 * object->amount_of_nodes;

            if ((object->spline_flags & VANILLA_SPLINE_FLAG_FINAL_ANGLE) != 0) {
                _size += 4;
            }
            else if ((object->spline_flags & VANILLA_SPLINE_FLAG_FINAL_TARGET) != 0) {
                _size += 8;
            }
            else if ((object->spline_flags & VANILLA_SPLINE_FLAG_FINAL_POINT) != 0) {
                _size += 12;
            }

        }

    }
    else if ((object->update_flag & VANILLA_UPDATE_FLAG_HAS_POSITION) != 0) {
        _size += 16;
    }

    if ((object->update_flag & VANILLA_UPDATE_FLAG_HIGH_GUID) != 0) {
        _size += 4;
    }

    if ((object->update_flag & VANILLA_UPDATE_FLAG_ALL) != 0) {
        _size += 4;
    }

    if ((object->update_flag & VANILLA_UPDATE_FLAG_MELEE_ATTACKING) != 0) {
        _size += 0 + wwm_packed_guid_size(object->guid);
    }

    if ((object->update_flag & VANILLA_UPDATE_FLAG_TRANSPORT) != 0) {
        _size += 4;
    }

    return _size;
}

static WowWorldResult vanilla_MovementBlock_read(WowWorldReader* reader, vanilla_MovementBlock* object) {
    object->update_flag = 0;
    READ_U8(object->update_flag);

    if ((object->update_flag & VANILLA_UPDATE_FLAG_LIVING) != 0) {
        object->flags = 0;
        READ_U32(object->flags);

        READ_U32(object->timestamp);

        WWM_CHECK_RETURN_CODE(all_Vector3d_read(reader, &object->living_position));

        READ_FLOAT(object->living_orientation);

        if ((object->flags & VANILLA_MOVEMENT_FLAGS_ON_TRANSPORT) != 0) {
            READ_PACKED_GUID(object->transport_guid);

            WWM_CHECK_RETURN_CODE(all_Vector3d_read(reader, &object->transport_position));

            READ_FLOAT(object->transport_orientation);

        }
        if ((object->flags & VANILLA_MOVEMENT_FLAGS_SWIMMING) != 0) {
            READ_FLOAT(object->pitch);

        }
        READ_FLOAT(object->fall_time);

        if ((object->flags & VANILLA_MOVEMENT_FLAGS_JUMPING) != 0) {
            READ_FLOAT(object->z_speed);

            READ_FLOAT(object->cos_angle);

            READ_FLOAT(object->sin_angle);

            READ_FLOAT(object->xy_speed);

        }
        if ((object->flags & VANILLA_MOVEMENT_FLAGS_SPLINE_ELEVATION) != 0) {
            READ_FLOAT(object->spline_elevation);

        }
        READ_FLOAT(object->walking_speed);

        READ_FLOAT(object->running_speed);

        READ_FLOAT(object->backwards_running_speed);

        READ_FLOAT(object->swimming_speed);

        READ_FLOAT(object->backwards_swimming_speed);

        READ_FLOAT(object->turn_rate);

        if ((object->flags & VANILLA_MOVEMENT_FLAGS_SPLINE_ENABLED) != 0) {
            object->spline_flags = 0;
            READ_U32(object->spline_flags);

            if ((object->spline_flags & VANILLA_SPLINE_FLAG_FINAL_ANGLE) != 0) {
                READ_FLOAT(object->angle);

            }
            else if ((object->spline_flags & VANILLA_SPLINE_FLAG_FINAL_TARGET) != 0) {
                READ_U64(object->target);

            }
            else if ((object->spline_flags & VANILLA_SPLINE_FLAG_FINAL_POINT) != 0) {
                WWM_CHECK_RETURN_CODE(all_Vector3d_read(reader, &object->spline_final_point));

            }
            READ_U32(object->time_passed);

            READ_U32(object->duration);

            READ_U32(object->id);

            READ_U32(object->amount_of_nodes);

            READ_ARRAY_ALLOCATE(object->nodes, object->amount_of_nodes, sizeof(all_Vector3d));
            READ_ARRAY(object->nodes, object->amount_of_nodes, WWM_CHECK_RETURN_CODE(all_Vector3d_read(reader, &object->nodes[i])));

            WWM_CHECK_RETURN_CODE(all_Vector3d_read(reader, &object->final_node));

        }
    }
    else if ((object->update_flag & VANILLA_UPDATE_FLAG_HAS_POSITION) != 0) {
        WWM_CHECK_RETURN_CODE(all_Vector3d_read(reader, &object->position));

        READ_FLOAT(object->orientation);

    }
    if ((object->update_flag & VANILLA_UPDATE_FLAG_HIGH_GUID) != 0) {
        READ_U32(object->unknown0);

    }
    if ((object->update_flag & VANILLA_UPDATE_FLAG_ALL) != 0) {
        READ_U32(object->unknown1);

    }
    if ((object->update_flag & VANILLA_UPDATE_FLAG_MELEE_ATTACKING) != 0) {
        READ_PACKED_GUID(object->guid);

    }
    if ((object->update_flag & VANILLA_UPDATE_FLAG_TRANSPORT) != 0) {
        READ_U32(object->transport_progress_in_ms);

    }
    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_MovementBlock_write(WowWorldWriter* writer, const vanilla_MovementBlock* object) {
    WRITE_U8(object->update_flag);

    if ((object->update_flag & VANILLA_UPDATE_FLAG_LIVING) != 0) {
        WRITE_U32(object->flags);

        WRITE_U32(object->timestamp);

        WWM_CHECK_RETURN_CODE(all_Vector3d_write(writer, &object->living_position));

        WRITE_FLOAT(object->living_orientation);

        if ((object->flags & VANILLA_MOVEMENT_FLAGS_ON_TRANSPORT) != 0) {
            WRITE_PACKED_GUID(object->transport_guid);

            WWM_CHECK_RETURN_CODE(all_Vector3d_write(writer, &object->transport_position));

            WRITE_FLOAT(object->transport_orientation);

        }
        if ((object->flags & VANILLA_MOVEMENT_FLAGS_SWIMMING) != 0) {
            WRITE_FLOAT(object->pitch);

        }
        WRITE_FLOAT(object->fall_time);

        if ((object->flags & VANILLA_MOVEMENT_FLAGS_JUMPING) != 0) {
            WRITE_FLOAT(object->z_speed);

            WRITE_FLOAT(object->cos_angle);

            WRITE_FLOAT(object->sin_angle);

            WRITE_FLOAT(object->xy_speed);

        }
        if ((object->flags & VANILLA_MOVEMENT_FLAGS_SPLINE_ELEVATION) != 0) {
            WRITE_FLOAT(object->spline_elevation);

        }
        WRITE_FLOAT(object->walking_speed);

        WRITE_FLOAT(object->running_speed);

        WRITE_FLOAT(object->backwards_running_speed);

        WRITE_FLOAT(object->swimming_speed);

        WRITE_FLOAT(object->backwards_swimming_speed);

        WRITE_FLOAT(object->turn_rate);

        if ((object->flags & VANILLA_MOVEMENT_FLAGS_SPLINE_ENABLED) != 0) {
            WRITE_U32(object->spline_flags);

            if ((object->spline_flags & VANILLA_SPLINE_FLAG_FINAL_ANGLE) != 0) {
                WRITE_FLOAT(object->angle);

            }
            else if ((object->spline_flags & VANILLA_SPLINE_FLAG_FINAL_TARGET) != 0) {
                WRITE_U64(object->target);

            }
            else if ((object->spline_flags & VANILLA_SPLINE_FLAG_FINAL_POINT) != 0) {
                WWM_CHECK_RETURN_CODE(all_Vector3d_write(writer, &object->spline_final_point));

            }
            WRITE_U32(object->time_passed);

            WRITE_U32(object->duration);

            WRITE_U32(object->id);

            WRITE_U32(object->amount_of_nodes);

            WRITE_ARRAY(object->nodes, object->amount_of_nodes, WWM_CHECK_RETURN_CODE(all_Vector3d_write(writer, &object->nodes[i])));

            WWM_CHECK_RETURN_CODE(all_Vector3d_write(writer, &object->final_node));

        }
    }
    else if ((object->update_flag & VANILLA_UPDATE_FLAG_HAS_POSITION) != 0) {
        WWM_CHECK_RETURN_CODE(all_Vector3d_write(writer, &object->position));

        WRITE_FLOAT(object->orientation);

    }
    if ((object->update_flag & VANILLA_UPDATE_FLAG_HIGH_GUID) != 0) {
        WRITE_U32(object->unknown0);

    }
    if ((object->update_flag & VANILLA_UPDATE_FLAG_ALL) != 0) {
        WRITE_U32(object->unknown1);

    }
    if ((object->update_flag & VANILLA_UPDATE_FLAG_MELEE_ATTACKING) != 0) {
        WRITE_PACKED_GUID(object->guid);

    }
    if ((object->update_flag & VANILLA_UPDATE_FLAG_TRANSPORT) != 0) {
        WRITE_U32(object->transport_progress_in_ms);

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MovementBlock_free(vanilla_MovementBlock* object) {
    if ((object->update_flag & VANILLA_UPDATE_FLAG_LIVING) != 0) {
        if ((object->flags & VANILLA_MOVEMENT_FLAGS_ON_TRANSPORT) != 0) {
        }
        if ((object->flags & VANILLA_MOVEMENT_FLAGS_SWIMMING) != 0) {
        }
        if ((object->flags & VANILLA_MOVEMENT_FLAGS_JUMPING) != 0) {
        }
        if ((object->flags & VANILLA_MOVEMENT_FLAGS_SPLINE_ELEVATION) != 0) {
        }
        if ((object->flags & VANILLA_MOVEMENT_FLAGS_SPLINE_ENABLED) != 0) {
            if ((object->spline_flags & VANILLA_SPLINE_FLAG_FINAL_ANGLE) != 0) {
            }
            else if ((object->spline_flags & VANILLA_SPLINE_FLAG_FINAL_TARGET) != 0) {
            }
            else if ((object->spline_flags & VANILLA_SPLINE_FLAG_FINAL_POINT) != 0) {
            }
            free(object->nodes);
            object->nodes = NULL;
        }
    }
    else if ((object->update_flag & VANILLA_UPDATE_FLAG_HAS_POSITION) != 0) {
    }
    if ((object->update_flag & VANILLA_UPDATE_FLAG_HIGH_GUID) != 0) {
    }
    if ((object->update_flag & VANILLA_UPDATE_FLAG_ALL) != 0) {
    }
    if ((object->update_flag & VANILLA_UPDATE_FLAG_MELEE_ATTACKING) != 0) {
    }
    if ((object->update_flag & VANILLA_UPDATE_FLAG_TRANSPORT) != 0) {
    }
}

static size_t vanilla_TransportInfo_size(const vanilla_TransportInfo* object) {
    return 20 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_TransportInfo_read(WowWorldReader* reader, vanilla_TransportInfo* object) {
    READ_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(all_Vector3d_read(reader, &object->position));

    READ_FLOAT(object->orientation);

    READ_U32(object->timestamp);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_TransportInfo_write(WowWorldWriter* writer, const vanilla_TransportInfo* object) {
    WRITE_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(all_Vector3d_write(writer, &object->position));

    WRITE_FLOAT(object->orientation);

    WRITE_U32(object->timestamp);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_MovementInfo_size(const vanilla_MovementInfo* object) {
    size_t _size = 28;

    if ((object->flags & VANILLA_MOVEMENT_FLAGS_ON_TRANSPORT) != 0) {
        _size += 0 + vanilla_TransportInfo_size(&object->transport);
    }

    if ((object->flags & VANILLA_MOVEMENT_FLAGS_SWIMMING) != 0) {
        _size += 4;
    }

    if ((object->flags & VANILLA_MOVEMENT_FLAGS_JUMPING) != 0) {
        _size += 16;
    }

    if ((object->flags & VANILLA_MOVEMENT_FLAGS_SPLINE_ELEVATION) != 0) {
        _size += 4;
    }

    return _size;
}

static WowWorldResult vanilla_MovementInfo_read(WowWorldReader* reader, vanilla_MovementInfo* object) {
    object->flags = 0;
    READ_U32(object->flags);

    READ_U32(object->timestamp);

    WWM_CHECK_RETURN_CODE(all_Vector3d_read(reader, &object->position));

    READ_FLOAT(object->orientation);

    if ((object->flags & VANILLA_MOVEMENT_FLAGS_ON_TRANSPORT) != 0) {
        WWM_CHECK_RETURN_CODE(vanilla_TransportInfo_read(reader, &object->transport));

    }
    if ((object->flags & VANILLA_MOVEMENT_FLAGS_SWIMMING) != 0) {
        READ_FLOAT(object->pitch);

    }
    READ_FLOAT(object->fall_time);

    if ((object->flags & VANILLA_MOVEMENT_FLAGS_JUMPING) != 0) {
        READ_FLOAT(object->z_speed);

        READ_FLOAT(object->cos_angle);

        READ_FLOAT(object->sin_angle);

        READ_FLOAT(object->xy_speed);

    }
    if ((object->flags & VANILLA_MOVEMENT_FLAGS_SPLINE_ELEVATION) != 0) {
        READ_FLOAT(object->spline_elevation);

    }
    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_MovementInfo_write(WowWorldWriter* writer, const vanilla_MovementInfo* object) {
    WRITE_U32(object->flags);

    WRITE_U32(object->timestamp);

    WWM_CHECK_RETURN_CODE(all_Vector3d_write(writer, &object->position));

    WRITE_FLOAT(object->orientation);

    if ((object->flags & VANILLA_MOVEMENT_FLAGS_ON_TRANSPORT) != 0) {
        WWM_CHECK_RETURN_CODE(vanilla_TransportInfo_write(writer, &object->transport));

    }
    if ((object->flags & VANILLA_MOVEMENT_FLAGS_SWIMMING) != 0) {
        WRITE_FLOAT(object->pitch);

    }
    WRITE_FLOAT(object->fall_time);

    if ((object->flags & VANILLA_MOVEMENT_FLAGS_JUMPING) != 0) {
        WRITE_FLOAT(object->z_speed);

        WRITE_FLOAT(object->cos_angle);

        WRITE_FLOAT(object->sin_angle);

        WRITE_FLOAT(object->xy_speed);

    }
    if ((object->flags & VANILLA_MOVEMENT_FLAGS_SPLINE_ELEVATION) != 0) {
        WRITE_FLOAT(object->spline_elevation);

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MovementInfo_free(vanilla_MovementInfo* object) {
    if ((object->flags & VANILLA_MOVEMENT_FLAGS_ON_TRANSPORT) != 0) {
    }
    if ((object->flags & VANILLA_MOVEMENT_FLAGS_SWIMMING) != 0) {
    }
    if ((object->flags & VANILLA_MOVEMENT_FLAGS_JUMPING) != 0) {
    }
    if ((object->flags & VANILLA_MOVEMENT_FLAGS_SPLINE_ELEVATION) != 0) {
    }
}

static WowWorldResult vanilla_NpcTextUpdateEmote_read(WowWorldReader* reader, vanilla_NpcTextUpdateEmote* object) {
    READ_U32(object->delay);

    READ_U32(object->emote);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_NpcTextUpdateEmote_write(WowWorldWriter* writer, const vanilla_NpcTextUpdateEmote* object) {
    WRITE_U32(object->delay);

    WRITE_U32(object->emote);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_NpcTextUpdate_size(const vanilla_NpcTextUpdate* object) {
    size_t _size = 32;

    /* C89 scope to allow variable declarations */ {
        int i;
        for(i = 0; i < 2; ++i) {
            _size += STRING_SIZE((*object->texts[i])) + 1;
        }
    }

    return _size;
}

static WowWorldResult vanilla_NpcTextUpdate_read(WowWorldReader* reader, vanilla_NpcTextUpdate* object) {
    READ_FLOAT(object->probability);

    READ_ARRAY_ALLOCATE(object->texts, 2, sizeof(WowWorldString));
    READ_ARRAY(object->texts, 2, READ_CSTRING((*object->texts)[i]));

    object->language = 0;
    READ_U32(object->language);

    READ_ARRAY_ALLOCATE(object->emotes, 3, sizeof(vanilla_NpcTextUpdateEmote));
    READ_ARRAY(object->emotes, 3, WWM_CHECK_RETURN_CODE(vanilla_NpcTextUpdateEmote_read(reader, &(*object->emotes)[i])));

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_NpcTextUpdate_write(WowWorldWriter* writer, const vanilla_NpcTextUpdate* object) {
    WRITE_FLOAT(object->probability);

    WRITE_ARRAY(object->texts, 2, WRITE_CSTRING((*object->texts)[i]));

    WRITE_U32(object->language);

    WRITE_ARRAY(object->emotes, 3, WWM_CHECK_RETURN_CODE(vanilla_NpcTextUpdateEmote_write(writer, &(*object->emotes)[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_NpcTextUpdate_free(vanilla_NpcTextUpdate* object) {
    size_t i;

    for (i = 0; i < 2; ++i) {
        FREE_STRING(((*object->texts)[i]));
    }
    free(object->texts);
    object->texts = NULL;
    free(object->emotes);
    object->emotes = NULL;
}

static size_t vanilla_Object_size(const vanilla_Object* object) {
    size_t _size = 1;

    if (object->update_type == VANILLA_UPDATE_TYPE_VALUES) {
        _size += 0 + wwm_packed_guid_size(object->guid1) + vanilla_update_mask_size(&object->mask1);
    }
    else if (object->update_type == VANILLA_UPDATE_TYPE_MOVEMENT) {
        _size += 0 + wwm_packed_guid_size(object->guid2) + vanilla_MovementBlock_size(&object->movement1);
    }
    else if (object->update_type == VANILLA_UPDATE_TYPE_CREATE_OBJECT|| object->update_type == VANILLA_UPDATE_TYPE_CREATE_OBJECT2) {
        _size += 1 + wwm_packed_guid_size(object->guid3) + vanilla_MovementBlock_size(&object->movement2) + vanilla_update_mask_size(&object->mask2);
    }
    else if (object->update_type == VANILLA_UPDATE_TYPE_OUT_OF_RANGE_OBJECTS|| object->update_type == VANILLA_UPDATE_TYPE_NEAR_OBJECTS) {
        _size += 4;

        /* C89 scope to allow variable declarations */ {
            int i;
            for(i = 0; i < (int)object->count; ++i) {
                _size += wwm_packed_guid_size(object->guids[i]);
            }
        }

    }

    return _size;
}

static WowWorldResult vanilla_Object_read(WowWorldReader* reader, vanilla_Object* object) {
    object->update_type = 0;
    READ_U8(object->update_type);

    if (object->update_type == VANILLA_UPDATE_TYPE_VALUES) {
        READ_PACKED_GUID(object->guid1);

        vanilla_update_mask_read(reader, &object->mask1);

    }
    else if (object->update_type == VANILLA_UPDATE_TYPE_MOVEMENT) {
        READ_PACKED_GUID(object->guid2);

        WWM_CHECK_RETURN_CODE(vanilla_MovementBlock_read(reader, &object->movement1));

    }
    else if (object->update_type == VANILLA_UPDATE_TYPE_CREATE_OBJECT|| object->update_type == VANILLA_UPDATE_TYPE_CREATE_OBJECT2) {
        READ_PACKED_GUID(object->guid3);

        object->object_type = 0;
        READ_U8(object->object_type);

        WWM_CHECK_RETURN_CODE(vanilla_MovementBlock_read(reader, &object->movement2));

        vanilla_update_mask_read(reader, &object->mask2);

    }
    else if (object->update_type == VANILLA_UPDATE_TYPE_OUT_OF_RANGE_OBJECTS|| object->update_type == VANILLA_UPDATE_TYPE_NEAR_OBJECTS) {
        READ_U32(object->count);

        READ_ARRAY_ALLOCATE(object->guids, object->count, sizeof(uint64_t));
        READ_ARRAY(object->guids, object->count, READ_PACKED_GUID(object->guids[i]));

    }
    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_Object_write(WowWorldWriter* writer, const vanilla_Object* object) {
    WRITE_U8(object->update_type);

    if (object->update_type == VANILLA_UPDATE_TYPE_VALUES) {
        WRITE_PACKED_GUID(object->guid1);

        vanilla_update_mask_write(writer, &object->mask1);

    }
    else if (object->update_type == VANILLA_UPDATE_TYPE_MOVEMENT) {
        WRITE_PACKED_GUID(object->guid2);

        WWM_CHECK_RETURN_CODE(vanilla_MovementBlock_write(writer, &object->movement1));

    }
    else if (object->update_type == VANILLA_UPDATE_TYPE_CREATE_OBJECT|| object->update_type == VANILLA_UPDATE_TYPE_CREATE_OBJECT2) {
        WRITE_PACKED_GUID(object->guid3);

        WRITE_U8(object->object_type);

        WWM_CHECK_RETURN_CODE(vanilla_MovementBlock_write(writer, &object->movement2));

        vanilla_update_mask_write(writer, &object->mask2);

    }
    else if (object->update_type == VANILLA_UPDATE_TYPE_OUT_OF_RANGE_OBJECTS|| object->update_type == VANILLA_UPDATE_TYPE_NEAR_OBJECTS) {
        WRITE_U32(object->count);

        WRITE_ARRAY(object->guids, object->count, WWM_CHECK_RETURN_CODE(wwm_write_packed_guid(writer, object->guids[i])));

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_Object_free(vanilla_Object* object) {
    if (object->update_type == VANILLA_UPDATE_TYPE_VALUES) {
    }
    else if (object->update_type == VANILLA_UPDATE_TYPE_MOVEMENT) {
        vanilla_MovementBlock_free(&object->movement1);
    }
    else if (object->update_type == VANILLA_UPDATE_TYPE_CREATE_OBJECT|| object->update_type == VANILLA_UPDATE_TYPE_CREATE_OBJECT2) {
        vanilla_MovementBlock_free(&object->movement2);
    }
    else if (object->update_type == VANILLA_UPDATE_TYPE_OUT_OF_RANGE_OBJECTS|| object->update_type == VANILLA_UPDATE_TYPE_NEAR_OBJECTS) {
        free(object->guids);
        object->guids = NULL;
    }
}

static WowWorldResult vanilla_PetSpellCooldown_read(WowWorldReader* reader, vanilla_PetSpellCooldown* object) {
    READ_U16(object->spell);

    READ_U16(object->spell_category);

    READ_U32(object->cooldown);

    READ_U32(object->category_cooldown);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_PetSpellCooldown_write(WowWorldWriter* writer, const vanilla_PetSpellCooldown* object) {
    WRITE_U16(object->spell);

    WRITE_U16(object->spell_category);

    WRITE_U32(object->cooldown);

    WRITE_U32(object->category_cooldown);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_PetitionShowlist_read(WowWorldReader* reader, vanilla_PetitionShowlist* object) {
    READ_U32(object->index);

    READ_U32(object->charter_entry);

    READ_U32(object->charter_display_id);

    READ_U32(object->guild_charter_cost);

    READ_U32(object->unknown1);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_PetitionShowlist_write(WowWorldWriter* writer, const vanilla_PetitionShowlist* object) {
    WRITE_U32(object->index);

    WRITE_U32(object->charter_entry);

    WRITE_U32(object->charter_display_id);

    WRITE_U32(object->guild_charter_cost);

    WRITE_U32(object->unknown1);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_PetitionSignature_read(WowWorldReader* reader, vanilla_PetitionSignature* object) {
    READ_U64(object->signer);

    READ_U32(object->unknown1);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_PetitionSignature_write(WowWorldWriter* writer, const vanilla_PetitionSignature* object) {
    WRITE_U64(object->signer);

    WRITE_U32(object->unknown1);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_QuestDetailsEmote_read(WowWorldReader* reader, vanilla_QuestDetailsEmote* object) {
    READ_U32(object->emote);

    READ_U32(object->emote_delay);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_QuestDetailsEmote_write(WowWorldWriter* writer, const vanilla_QuestDetailsEmote* object) {
    WRITE_U32(object->emote);

    WRITE_U32(object->emote_delay);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_QuestItem_size(const vanilla_QuestItem* object) {
    return 13 + STRING_SIZE(object->title);
}

static WowWorldResult vanilla_QuestItem_read(WowWorldReader* reader, vanilla_QuestItem* object) {
    READ_U32(object->quest_id);

    READ_U32(object->quest_icon);

    READ_U32(object->level);

    READ_CSTRING(object->title);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_QuestItem_write(WowWorldWriter* writer, const vanilla_QuestItem* object) {
    WRITE_U32(object->quest_id);

    WRITE_U32(object->quest_icon);

    WRITE_U32(object->level);

    WRITE_CSTRING(object->title);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_QuestItem_free(vanilla_QuestItem* object) {
    FREE_STRING(object->title);

}

static WowWorldResult vanilla_QuestItemRequirement_read(WowWorldReader* reader, vanilla_QuestItemRequirement* object) {
    READ_U32(object->item);

    READ_U32(object->item_count);

    READ_U32(object->item_display_id);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_QuestItemRequirement_write(WowWorldWriter* writer, const vanilla_QuestItemRequirement* object) {
    WRITE_U32(object->item);

    WRITE_U32(object->item_count);

    WRITE_U32(object->item_display_id);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_QuestItemReward_read(WowWorldReader* reader, vanilla_QuestItemReward* object) {
    READ_U32(object->item);

    READ_U32(object->item_count);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_QuestItemReward_write(WowWorldWriter* writer, const vanilla_QuestItemReward* object) {
    WRITE_U32(object->item);

    WRITE_U32(object->item_count);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_QuestObjective_read(WowWorldReader* reader, vanilla_QuestObjective* object) {
    READ_U32(object->creature_id);

    READ_U32(object->kill_count);

    READ_U32(object->required_item_id);

    READ_U32(object->required_item_count);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_QuestObjective_write(WowWorldWriter* writer, const vanilla_QuestObjective* object) {
    WRITE_U32(object->creature_id);

    WRITE_U32(object->kill_count);

    WRITE_U32(object->required_item_id);

    WRITE_U32(object->required_item_count);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_RaidInfo_read(WowWorldReader* reader, vanilla_RaidInfo* object) {
    object->map = 0;
    READ_U32(object->map);

    READ_U32(object->reset_time);

    READ_U32(object->instance_id);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_RaidInfo_write(WowWorldWriter* writer, const vanilla_RaidInfo* object) {
    WRITE_U32(object->map);

    WRITE_U32(object->reset_time);

    WRITE_U32(object->instance_id);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_RaidTargetUpdate_read(WowWorldReader* reader, vanilla_RaidTargetUpdate* object) {
    object->index = 0;
    READ_U8(object->index);

    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_RaidTargetUpdate_write(WowWorldWriter* writer, const vanilla_RaidTargetUpdate* object) {
    WRITE_U8(object->index);

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SpellCastTargets_size(const vanilla_SpellCastTargets* object) {
    size_t _size = 2;

    if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_UNIT) != 0) {
        _size += 0 + wwm_packed_guid_size(object->unit_target);
    }

    if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_GAMEOBJECT) != 0) {
        _size += 0 + wwm_packed_guid_size(object->gameobject);
    }
    else if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_OBJECT_UNK) != 0) {
        _size += 0 + wwm_packed_guid_size(object->object_unk);
    }

    if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_ITEM) != 0) {
        _size += 0 + wwm_packed_guid_size(object->item);
    }
    else if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_TRADE_ITEM) != 0) {
        _size += 0 + wwm_packed_guid_size(object->trade_item);
    }

    if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_SOURCE_LOCATION) != 0) {
        _size += 12;
    }

    if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_DEST_LOCATION) != 0) {
        _size += 12;
    }

    if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_STRING) != 0) {
        _size += 1 + STRING_SIZE(object->target_string);
    }

    if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_CORPSE) != 0) {
        _size += 0 + wwm_packed_guid_size(object->corpse);
    }
    else if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_PVP_CORPSE) != 0) {
        _size += 0 + wwm_packed_guid_size(object->pvp_corpse);
    }

    return _size;
}

static WowWorldResult vanilla_SpellCastTargets_read(WowWorldReader* reader, vanilla_SpellCastTargets* object) {
    object->target_flags = 0;
    READ_U16(object->target_flags);

    if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_UNIT) != 0) {
        READ_PACKED_GUID(object->unit_target);

    }
    if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_GAMEOBJECT) != 0) {
        READ_PACKED_GUID(object->gameobject);

    }
    else if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_OBJECT_UNK) != 0) {
        READ_PACKED_GUID(object->object_unk);

    }
    if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_ITEM) != 0) {
        READ_PACKED_GUID(object->item);

    }
    else if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_TRADE_ITEM) != 0) {
        READ_PACKED_GUID(object->trade_item);

    }
    if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_SOURCE_LOCATION) != 0) {
        WWM_CHECK_RETURN_CODE(all_Vector3d_read(reader, &object->source));

    }
    if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_DEST_LOCATION) != 0) {
        WWM_CHECK_RETURN_CODE(all_Vector3d_read(reader, &object->destination));

    }
    if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_STRING) != 0) {
        READ_CSTRING(object->target_string);

    }
    if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_CORPSE) != 0) {
        READ_PACKED_GUID(object->corpse);

    }
    else if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_PVP_CORPSE) != 0) {
        READ_PACKED_GUID(object->pvp_corpse);

    }
    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SpellCastTargets_write(WowWorldWriter* writer, const vanilla_SpellCastTargets* object) {
    WRITE_U16(object->target_flags);

    if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_UNIT) != 0) {
        WRITE_PACKED_GUID(object->unit_target);

    }
    if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_GAMEOBJECT) != 0) {
        WRITE_PACKED_GUID(object->gameobject);

    }
    else if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_OBJECT_UNK) != 0) {
        WRITE_PACKED_GUID(object->object_unk);

    }
    if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_ITEM) != 0) {
        WRITE_PACKED_GUID(object->item);

    }
    else if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_TRADE_ITEM) != 0) {
        WRITE_PACKED_GUID(object->trade_item);

    }
    if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_SOURCE_LOCATION) != 0) {
        WWM_CHECK_RETURN_CODE(all_Vector3d_write(writer, &object->source));

    }
    if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_DEST_LOCATION) != 0) {
        WWM_CHECK_RETURN_CODE(all_Vector3d_write(writer, &object->destination));

    }
    if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_STRING) != 0) {
        WRITE_CSTRING(object->target_string);

    }
    if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_CORPSE) != 0) {
        WRITE_PACKED_GUID(object->corpse);

    }
    else if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_PVP_CORPSE) != 0) {
        WRITE_PACKED_GUID(object->pvp_corpse);

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SpellCastTargets_free(vanilla_SpellCastTargets* object) {
    if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_UNIT) != 0) {
    }
    if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_GAMEOBJECT) != 0) {
    }
    else if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_OBJECT_UNK) != 0) {
    }
    if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_ITEM) != 0) {
    }
    else if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_TRADE_ITEM) != 0) {
    }
    if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_SOURCE_LOCATION) != 0) {
    }
    if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_DEST_LOCATION) != 0) {
    }
    if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_STRING) != 0) {
        FREE_STRING(object->target_string);

    }
    if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_CORPSE) != 0) {
    }
    else if ((object->target_flags & VANILLA_SPELL_CAST_TARGET_FLAGS_PVP_CORPSE) != 0) {
    }
}

static WowWorldResult vanilla_SpellCooldownStatus_read(WowWorldReader* reader, vanilla_SpellCooldownStatus* object) {
    READ_U32(object->id);

    READ_U32(object->cooldown_time);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SpellCooldownStatus_write(WowWorldWriter* writer, const vanilla_SpellCooldownStatus* object) {
    WRITE_U32(object->id);

    WRITE_U32(object->cooldown_time);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SpellLog_size(const vanilla_SpellLog* object) {
    size_t _size = 8;

    if (object->effect == VANILLA_SPELL_EFFECT_POWER_DRAIN) {
        _size += 20;
    }
    else if (object->effect == VANILLA_SPELL_EFFECT_HEAL|| object->effect == VANILLA_SPELL_EFFECT_HEAL_MAX_HEALTH) {
        _size += 16;
    }
    else if (object->effect == VANILLA_SPELL_EFFECT_ENERGIZE) {
        _size += 16;
    }
    else if (object->effect == VANILLA_SPELL_EFFECT_ADD_EXTRA_ATTACKS) {
        _size += 12;
    }
    else if (object->effect == VANILLA_SPELL_EFFECT_CREATE_ITEM) {
        _size += 4;
    }
    else if (object->effect == VANILLA_SPELL_EFFECT_INTERRUPT_CAST) {
        _size += 12;
    }
    else if (object->effect == VANILLA_SPELL_EFFECT_DURABILITY_DAMAGE) {
        _size += 16;
    }
    else if (object->effect == VANILLA_SPELL_EFFECT_FEED_PET) {
        _size += 4;
    }
    else if (object->effect == VANILLA_SPELL_EFFECT_INSTAKILL|| object->effect == VANILLA_SPELL_EFFECT_RESURRECT|| object->effect == VANILLA_SPELL_EFFECT_DISPEL|| object->effect == VANILLA_SPELL_EFFECT_THREAT|| object->effect == VANILLA_SPELL_EFFECT_DISTRACT|| object->effect == VANILLA_SPELL_EFFECT_SANCTUARY|| object->effect == VANILLA_SPELL_EFFECT_THREAT_ALL|| object->effect == VANILLA_SPELL_EFFECT_DISPEL_MECHANIC|| object->effect == VANILLA_SPELL_EFFECT_RESURRECT_NEW|| object->effect == VANILLA_SPELL_EFFECT_ATTACK_ME|| object->effect == VANILLA_SPELL_EFFECT_SKIN_PLAYER_CORPSE|| object->effect == VANILLA_SPELL_EFFECT_MODIFY_THREAT_PERCENT|| object->effect == VANILLA_SPELL_EFFECT_UNKNOWN126|| object->effect == VANILLA_SPELL_EFFECT_OPEN_LOCK|| object->effect == VANILLA_SPELL_EFFECT_OPEN_LOCK_ITEM|| object->effect == VANILLA_SPELL_EFFECT_DISMISS_PET|| object->effect == VANILLA_SPELL_EFFECT_TRANS_DOOR|| object->effect == VANILLA_SPELL_EFFECT_SUMMON|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_PET|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_WILD|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_GUARDIAN|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_TOTEM_SLOT1|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_TOTEM_SLOT2|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_TOTEM_SLOT3|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_TOTEM_SLOT4|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_POSSESSED|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_TOTEM|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_CRITTER|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_OBJECT_WILD|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_OBJECT_SLOT1|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_OBJECT_SLOT2|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_OBJECT_SLOT3|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_OBJECT_SLOT4|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_DEMON) {
        _size += 8;
    }

    return _size;
}

static WowWorldResult vanilla_SpellLog_read(WowWorldReader* reader, vanilla_SpellLog* object) {
    object->effect = 0;
    READ_U32(object->effect);

    SKIP_FORWARD_BYTES(4);

    if (object->effect == VANILLA_SPELL_EFFECT_POWER_DRAIN) {
        READ_U64(object->target1);

        READ_U32(object->amount);

        object->power = 0;
        READ_U32(object->power);

        READ_FLOAT(object->multiplier);

    }
    else if (object->effect == VANILLA_SPELL_EFFECT_HEAL|| object->effect == VANILLA_SPELL_EFFECT_HEAL_MAX_HEALTH) {
        READ_U64(object->target2);

        READ_U32(object->heal_amount);

        READ_U32(object->heal_critical);

    }
    else if (object->effect == VANILLA_SPELL_EFFECT_ENERGIZE) {
        READ_U64(object->target3);

        READ_U32(object->energize_amount);

        READ_U32(object->energize_power);

    }
    else if (object->effect == VANILLA_SPELL_EFFECT_ADD_EXTRA_ATTACKS) {
        READ_U64(object->target4);

        READ_U32(object->extra_attacks);

    }
    else if (object->effect == VANILLA_SPELL_EFFECT_CREATE_ITEM) {
        READ_U32(object->item);

    }
    else if (object->effect == VANILLA_SPELL_EFFECT_INTERRUPT_CAST) {
        READ_U64(object->target5);

        READ_U32(object->interrupted_spell);

    }
    else if (object->effect == VANILLA_SPELL_EFFECT_DURABILITY_DAMAGE) {
        READ_U64(object->target6);

        READ_U32(object->item_to_damage);

        READ_U32(object->unknown5);

    }
    else if (object->effect == VANILLA_SPELL_EFFECT_FEED_PET) {
        READ_U32(object->feed_pet_item);

    }
    else if (object->effect == VANILLA_SPELL_EFFECT_INSTAKILL|| object->effect == VANILLA_SPELL_EFFECT_RESURRECT|| object->effect == VANILLA_SPELL_EFFECT_DISPEL|| object->effect == VANILLA_SPELL_EFFECT_THREAT|| object->effect == VANILLA_SPELL_EFFECT_DISTRACT|| object->effect == VANILLA_SPELL_EFFECT_SANCTUARY|| object->effect == VANILLA_SPELL_EFFECT_THREAT_ALL|| object->effect == VANILLA_SPELL_EFFECT_DISPEL_MECHANIC|| object->effect == VANILLA_SPELL_EFFECT_RESURRECT_NEW|| object->effect == VANILLA_SPELL_EFFECT_ATTACK_ME|| object->effect == VANILLA_SPELL_EFFECT_SKIN_PLAYER_CORPSE|| object->effect == VANILLA_SPELL_EFFECT_MODIFY_THREAT_PERCENT|| object->effect == VANILLA_SPELL_EFFECT_UNKNOWN126|| object->effect == VANILLA_SPELL_EFFECT_OPEN_LOCK|| object->effect == VANILLA_SPELL_EFFECT_OPEN_LOCK_ITEM|| object->effect == VANILLA_SPELL_EFFECT_DISMISS_PET|| object->effect == VANILLA_SPELL_EFFECT_TRANS_DOOR|| object->effect == VANILLA_SPELL_EFFECT_SUMMON|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_PET|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_WILD|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_GUARDIAN|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_TOTEM_SLOT1|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_TOTEM_SLOT2|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_TOTEM_SLOT3|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_TOTEM_SLOT4|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_POSSESSED|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_TOTEM|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_CRITTER|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_OBJECT_WILD|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_OBJECT_SLOT1|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_OBJECT_SLOT2|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_OBJECT_SLOT3|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_OBJECT_SLOT4|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_DEMON) {
        READ_U64(object->target7);

    }
    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SpellLog_write(WowWorldWriter* writer, const vanilla_SpellLog* object) {
    WRITE_U32(object->effect);

    WRITE_U32(1);

    if (object->effect == VANILLA_SPELL_EFFECT_POWER_DRAIN) {
        WRITE_U64(object->target1);

        WRITE_U32(object->amount);

        WRITE_U32(object->power);

        WRITE_FLOAT(object->multiplier);

    }
    else if (object->effect == VANILLA_SPELL_EFFECT_HEAL|| object->effect == VANILLA_SPELL_EFFECT_HEAL_MAX_HEALTH) {
        WRITE_U64(object->target2);

        WRITE_U32(object->heal_amount);

        WRITE_U32(object->heal_critical);

    }
    else if (object->effect == VANILLA_SPELL_EFFECT_ENERGIZE) {
        WRITE_U64(object->target3);

        WRITE_U32(object->energize_amount);

        WRITE_U32(object->energize_power);

    }
    else if (object->effect == VANILLA_SPELL_EFFECT_ADD_EXTRA_ATTACKS) {
        WRITE_U64(object->target4);

        WRITE_U32(object->extra_attacks);

    }
    else if (object->effect == VANILLA_SPELL_EFFECT_CREATE_ITEM) {
        WRITE_U32(object->item);

    }
    else if (object->effect == VANILLA_SPELL_EFFECT_INTERRUPT_CAST) {
        WRITE_U64(object->target5);

        WRITE_U32(object->interrupted_spell);

    }
    else if (object->effect == VANILLA_SPELL_EFFECT_DURABILITY_DAMAGE) {
        WRITE_U64(object->target6);

        WRITE_U32(object->item_to_damage);

        WRITE_U32(object->unknown5);

    }
    else if (object->effect == VANILLA_SPELL_EFFECT_FEED_PET) {
        WRITE_U32(object->feed_pet_item);

    }
    else if (object->effect == VANILLA_SPELL_EFFECT_INSTAKILL|| object->effect == VANILLA_SPELL_EFFECT_RESURRECT|| object->effect == VANILLA_SPELL_EFFECT_DISPEL|| object->effect == VANILLA_SPELL_EFFECT_THREAT|| object->effect == VANILLA_SPELL_EFFECT_DISTRACT|| object->effect == VANILLA_SPELL_EFFECT_SANCTUARY|| object->effect == VANILLA_SPELL_EFFECT_THREAT_ALL|| object->effect == VANILLA_SPELL_EFFECT_DISPEL_MECHANIC|| object->effect == VANILLA_SPELL_EFFECT_RESURRECT_NEW|| object->effect == VANILLA_SPELL_EFFECT_ATTACK_ME|| object->effect == VANILLA_SPELL_EFFECT_SKIN_PLAYER_CORPSE|| object->effect == VANILLA_SPELL_EFFECT_MODIFY_THREAT_PERCENT|| object->effect == VANILLA_SPELL_EFFECT_UNKNOWN126|| object->effect == VANILLA_SPELL_EFFECT_OPEN_LOCK|| object->effect == VANILLA_SPELL_EFFECT_OPEN_LOCK_ITEM|| object->effect == VANILLA_SPELL_EFFECT_DISMISS_PET|| object->effect == VANILLA_SPELL_EFFECT_TRANS_DOOR|| object->effect == VANILLA_SPELL_EFFECT_SUMMON|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_PET|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_WILD|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_GUARDIAN|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_TOTEM_SLOT1|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_TOTEM_SLOT2|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_TOTEM_SLOT3|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_TOTEM_SLOT4|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_POSSESSED|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_TOTEM|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_CRITTER|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_OBJECT_WILD|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_OBJECT_SLOT1|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_OBJECT_SLOT2|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_OBJECT_SLOT3|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_OBJECT_SLOT4|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_DEMON) {
        WRITE_U64(object->target7);

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SpellLog_free(vanilla_SpellLog* object) {
    if (object->effect == VANILLA_SPELL_EFFECT_POWER_DRAIN) {
    }
    else if (object->effect == VANILLA_SPELL_EFFECT_HEAL|| object->effect == VANILLA_SPELL_EFFECT_HEAL_MAX_HEALTH) {
    }
    else if (object->effect == VANILLA_SPELL_EFFECT_ENERGIZE) {
    }
    else if (object->effect == VANILLA_SPELL_EFFECT_ADD_EXTRA_ATTACKS) {
    }
    else if (object->effect == VANILLA_SPELL_EFFECT_CREATE_ITEM) {
    }
    else if (object->effect == VANILLA_SPELL_EFFECT_INTERRUPT_CAST) {
    }
    else if (object->effect == VANILLA_SPELL_EFFECT_DURABILITY_DAMAGE) {
    }
    else if (object->effect == VANILLA_SPELL_EFFECT_FEED_PET) {
    }
    else if (object->effect == VANILLA_SPELL_EFFECT_INSTAKILL|| object->effect == VANILLA_SPELL_EFFECT_RESURRECT|| object->effect == VANILLA_SPELL_EFFECT_DISPEL|| object->effect == VANILLA_SPELL_EFFECT_THREAT|| object->effect == VANILLA_SPELL_EFFECT_DISTRACT|| object->effect == VANILLA_SPELL_EFFECT_SANCTUARY|| object->effect == VANILLA_SPELL_EFFECT_THREAT_ALL|| object->effect == VANILLA_SPELL_EFFECT_DISPEL_MECHANIC|| object->effect == VANILLA_SPELL_EFFECT_RESURRECT_NEW|| object->effect == VANILLA_SPELL_EFFECT_ATTACK_ME|| object->effect == VANILLA_SPELL_EFFECT_SKIN_PLAYER_CORPSE|| object->effect == VANILLA_SPELL_EFFECT_MODIFY_THREAT_PERCENT|| object->effect == VANILLA_SPELL_EFFECT_UNKNOWN126|| object->effect == VANILLA_SPELL_EFFECT_OPEN_LOCK|| object->effect == VANILLA_SPELL_EFFECT_OPEN_LOCK_ITEM|| object->effect == VANILLA_SPELL_EFFECT_DISMISS_PET|| object->effect == VANILLA_SPELL_EFFECT_TRANS_DOOR|| object->effect == VANILLA_SPELL_EFFECT_SUMMON|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_PET|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_WILD|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_GUARDIAN|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_TOTEM_SLOT1|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_TOTEM_SLOT2|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_TOTEM_SLOT3|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_TOTEM_SLOT4|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_POSSESSED|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_TOTEM|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_CRITTER|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_OBJECT_WILD|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_OBJECT_SLOT1|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_OBJECT_SLOT2|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_OBJECT_SLOT3|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_OBJECT_SLOT4|| object->effect == VANILLA_SPELL_EFFECT_SUMMON_DEMON) {
    }
}

static WowWorldResult vanilla_SpellLogMiss_read(WowWorldReader* reader, vanilla_SpellLogMiss* object) {
    READ_U64(object->target);

    object->miss_info = 0;
    READ_U8(object->miss_info);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SpellLogMiss_write(WowWorldWriter* writer, const vanilla_SpellLogMiss* object) {
    WRITE_U64(object->target);

    WRITE_U8(object->miss_info);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SpellMiss_read(WowWorldReader* reader, vanilla_SpellMiss* object) {
    READ_U64(object->target);

    object->miss_info = 0;
    READ_U8(object->miss_info);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SpellMiss_write(WowWorldWriter* writer, const vanilla_SpellMiss* object) {
    WRITE_U64(object->target);

    WRITE_U8(object->miss_info);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_StabledPet_size(const vanilla_StabledPet* object) {
    return 18 + STRING_SIZE(object->name);
}

static WowWorldResult vanilla_StabledPet_read(WowWorldReader* reader, vanilla_StabledPet* object) {
    READ_U32(object->pet_number);

    READ_U32(object->entry);

    READ_U32(object->level);

    READ_CSTRING(object->name);

    READ_U32(object->loyalty);

    READ_U8(object->slot);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_StabledPet_write(WowWorldWriter* writer, const vanilla_StabledPet* object) {
    WRITE_U32(object->pet_number);

    WRITE_U32(object->entry);

    WRITE_U32(object->level);

    WRITE_CSTRING(object->name);

    WRITE_U32(object->loyalty);

    WRITE_U8(object->slot);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_StabledPet_free(vanilla_StabledPet* object) {
    FREE_STRING(object->name);

}

static WowWorldResult vanilla_TradeSlot_read(WowWorldReader* reader, vanilla_TradeSlot* object) {
    READ_U8(object->trade_slot_number);

    READ_U32(object->item);

    READ_U32(object->display_id);

    READ_U32(object->stack_count);

    READ_BOOL32(object->wrapped);

    READ_U64(object->gift_wrapper);

    READ_U32(object->enchantment);

    READ_U64(object->item_creator);

    READ_U32(object->spell_charges);

    READ_U32(object->item_suffix_factor);

    READ_U32(object->item_random_properties_id);

    READ_U32(object->lock_id);

    READ_U32(object->max_durability);

    READ_U32(object->durability);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_TradeSlot_write(WowWorldWriter* writer, const vanilla_TradeSlot* object) {
    WRITE_U8(object->trade_slot_number);

    WRITE_U32(object->item);

    WRITE_U32(object->display_id);

    WRITE_U32(object->stack_count);

    WRITE_BOOL32(object->wrapped);

    WRITE_U64(object->gift_wrapper);

    WRITE_U32(object->enchantment);

    WRITE_U64(object->item_creator);

    WRITE_U32(object->spell_charges);

    WRITE_U32(object->item_suffix_factor);

    WRITE_U32(object->item_random_properties_id);

    WRITE_U32(object->lock_id);

    WRITE_U32(object->max_durability);

    WRITE_U32(object->durability);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_TrainerSpell_read(WowWorldReader* reader, vanilla_TrainerSpell* object) {
    READ_U32(object->spell);

    object->state = 0;
    READ_U8(object->state);

    READ_U32(object->spell_cost);

    READ_U32(object->talent_point_cost);

    READ_U32(object->first_rank);

    READ_U8(object->required_level);

    object->required_skill = 0;
    READ_U32(object->required_skill);

    READ_U32(object->required_skill_value);

    READ_ARRAY_ALLOCATE(object->required_spells, 3, sizeof(uint32_t));
    READ_ARRAY(object->required_spells, 3, READ_U32((*object->required_spells)[i]));

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_TrainerSpell_write(WowWorldWriter* writer, const vanilla_TrainerSpell* object) {
    WRITE_U32(object->spell);

    WRITE_U8(object->state);

    WRITE_U32(object->spell_cost);

    WRITE_U32(object->talent_point_cost);

    WRITE_U32(object->first_rank);

    WRITE_U8(object->required_level);

    WRITE_U32(object->required_skill);

    WRITE_U32(object->required_skill_value);

    WRITE_ARRAY(object->required_spells, 3, WRITE_U32((*object->required_spells)[i]));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_TrainerSpell_free(vanilla_TrainerSpell* object) {
    free(object->required_spells);
    object->required_spells = NULL;
}

static size_t vanilla_WhoPlayer_size(const vanilla_WhoPlayer* object) {
    return 16 + STRING_SIZE(object->name) + STRING_SIZE(object->guild);
}

static WowWorldResult vanilla_WhoPlayer_read(WowWorldReader* reader, vanilla_WhoPlayer* object) {
    READ_CSTRING(object->name);

    READ_CSTRING(object->guild);

    READ_U32(object->level);

    object->class_type = 0;
    READ_U8(object->class_type);

    object->race = 0;
    READ_U8(object->race);

    object->area = 0;
    READ_U32(object->area);

    READ_U32(object->party_status);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_WhoPlayer_write(WowWorldWriter* writer, const vanilla_WhoPlayer* object) {
    WRITE_CSTRING(object->name);

    WRITE_CSTRING(object->guild);

    WRITE_U32(object->level);

    WRITE_U8(object->class_type);

    WRITE_U8(object->race);

    WRITE_U32(object->area);

    WRITE_U32(object->party_status);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_WhoPlayer_free(vanilla_WhoPlayer* object) {
    FREE_STRING(object->name);

    FREE_STRING(object->guild);

}

static WowWorldResult vanilla_WorldState_read(WowWorldReader* reader, vanilla_WorldState* object) {
    READ_U32(object->state);

    READ_U32(object->value);

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_WorldState_write(WowWorldWriter* writer, const vanilla_WorldState* object) {
    WRITE_U32(object->state);

    WRITE_U32(object->value);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_aura_mask_size(const vanilla_AuraMask* object) {
    size_t size = 4; /* uint32_t header */
    size_t i;

    for(i = 0; i < VANILLA_AURA_MASK_SIZE; ++i) {
        if(object->auras[i] != 0) {
            size += 2; /* uint16_t members */
        }
    }

    return size;
}

static WowWorldResult vanilla_aura_mask_read(WowWorldReader* reader, vanilla_AuraMask* mask) {
    uint32_t header;
    uint32_t i;
    WWM_CHECK_RETURN_CODE(wwm_read_uint32(reader, &header));

    for(i = 0; i < VANILLA_AURA_MASK_SIZE; ++i) {
        mask->auras[i] = 0; /* initialize to 0 */

        if ((header & (1 << i)) != 0) {
            WWM_CHECK_RETURN_CODE(wwm_read_uint16(reader, &mask->auras[i]));
        }
    }

    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_aura_mask_write(WowWorldWriter* writer, const vanilla_AuraMask* mask) {
    uint32_t header = 0;
    uint32_t i;

    for(i = 0; i < VANILLA_AURA_MASK_SIZE; ++i) {
        if (mask->auras[i] != 0) {
            header |= 1 << i;
        }
    }

    WWM_CHECK_RETURN_CODE(wwm_write_uint32(writer, header));

    for(i = 0; i < VANILLA_AURA_MASK_SIZE; ++i) {
        if (mask->auras[i] != 0) {
            WWM_CHECK_RETURN_CODE(wwm_write_uint16(writer, mask->auras[i]));
        }
    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_BOOTME_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x00000001); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_DBLOOKUP_size(const vanilla_CMSG_DBLOOKUP* object) {
    return 1 + STRING_SIZE(object->query);
}

static WowWorldResult vanilla_CMSG_DBLOOKUP_read(WowWorldReader* reader, vanilla_CMSG_DBLOOKUP* object) {
    READ_CSTRING(object->query);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_DBLOOKUP_write(WowWorldWriter* writer, const vanilla_CMSG_DBLOOKUP* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_DBLOOKUP_size(object) + 4); /* size */

    WRITE_U32(0x00000002); /* opcode */

    WRITE_CSTRING(object->query);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_DBLOOKUP_free(vanilla_CMSG_DBLOOKUP* object) {
    FREE_STRING(object->query);

}

static WowWorldResult vanilla_CMSG_WORLD_TELEPORT_read(WowWorldReader* reader, vanilla_CMSG_WORLD_TELEPORT* object) {
    READ_U32(object->time);

    object->map = 0;
    READ_U32(object->map);

    WWM_CHECK_RETURN_CODE(all_Vector3d_read(reader, &object->position));

    READ_FLOAT(object->orientation);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_WORLD_TELEPORT_write(WowWorldWriter* writer, const vanilla_CMSG_WORLD_TELEPORT* object) {
    WRITE_U16_BE(0x0018 + 4); /* size */

    WRITE_U32(0x00000008); /* opcode */

    WRITE_U32(object->time);

    WRITE_U32(object->map);

    WWM_CHECK_RETURN_CODE(all_Vector3d_write(writer, &object->position));

    WRITE_FLOAT(object->orientation);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_TELEPORT_TO_UNIT_size(const vanilla_CMSG_TELEPORT_TO_UNIT* object) {
    return 1 + STRING_SIZE(object->name);
}

static WowWorldResult vanilla_CMSG_TELEPORT_TO_UNIT_read(WowWorldReader* reader, vanilla_CMSG_TELEPORT_TO_UNIT* object) {
    READ_CSTRING(object->name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_TELEPORT_TO_UNIT_write(WowWorldWriter* writer, const vanilla_CMSG_TELEPORT_TO_UNIT* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_TELEPORT_TO_UNIT_size(object) + 4); /* size */

    WRITE_U32(0x00000009); /* opcode */

    WRITE_CSTRING(object->name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_TELEPORT_TO_UNIT_free(vanilla_CMSG_TELEPORT_TO_UNIT* object) {
    FREE_STRING(object->name);

}

static size_t vanilla_CMSG_CHAR_CREATE_size(const vanilla_CMSG_CHAR_CREATE* object) {
    return 10 + STRING_SIZE(object->name);
}

static WowWorldResult vanilla_CMSG_CHAR_CREATE_read(WowWorldReader* reader, vanilla_CMSG_CHAR_CREATE* object) {
    READ_CSTRING(object->name);

    object->race = 0;
    READ_U8(object->race);

    object->class_type = 0;
    READ_U8(object->class_type);

    object->gender = 0;
    READ_U8(object->gender);

    READ_U8(object->skin_color);

    READ_U8(object->face);

    READ_U8(object->hair_style);

    READ_U8(object->hair_color);

    READ_U8(object->facial_hair);

    SKIP_FORWARD_BYTES(1);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_CHAR_CREATE_write(WowWorldWriter* writer, const vanilla_CMSG_CHAR_CREATE* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_CHAR_CREATE_size(object) + 4); /* size */

    WRITE_U32(0x00000036); /* opcode */

    WRITE_CSTRING(object->name);

    WRITE_U8(object->race);

    WRITE_U8(object->class_type);

    WRITE_U8(object->gender);

    WRITE_U8(object->skin_color);

    WRITE_U8(object->face);

    WRITE_U8(object->hair_style);

    WRITE_U8(object->hair_color);

    WRITE_U8(object->facial_hair);

    WRITE_U8(0);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_CHAR_CREATE_free(vanilla_CMSG_CHAR_CREATE* object) {
    FREE_STRING(object->name);

}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_CHAR_ENUM_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x00000037); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_CHAR_DELETE_read(WowWorldReader* reader, vanilla_CMSG_CHAR_DELETE* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_CHAR_DELETE_write(WowWorldWriter* writer, const vanilla_CMSG_CHAR_DELETE* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x00000038); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_CHAR_CREATE_read(WowWorldReader* reader, vanilla_SMSG_CHAR_CREATE* object) {
    object->result = 0;
    READ_U8(object->result);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_CHAR_CREATE_write(WowWorldWriter* writer, const vanilla_SMSG_CHAR_CREATE* object) {
    WRITE_U16_BE(0x0001 + 2); /* size */

    WRITE_U16(0x0000003a); /* opcode */

    WRITE_U8(object->result);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_CHAR_ENUM_size(const vanilla_SMSG_CHAR_ENUM* object) {
    size_t _size = 1;

    /* C89 scope to allow variable declarations */ {
        int i;
        for(i = 0; i < (int)object->amount_of_characters; ++i) {
            _size += vanilla_Character_size(&object->characters[i]);
        }
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_CHAR_ENUM_read(WowWorldReader* reader, vanilla_SMSG_CHAR_ENUM* object) {
    READ_U8(object->amount_of_characters);

    READ_ARRAY_ALLOCATE(object->characters, object->amount_of_characters, sizeof(vanilla_Character));
    READ_ARRAY(object->characters, object->amount_of_characters, WWM_CHECK_RETURN_CODE(vanilla_Character_read(reader, &object->characters[i])));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_CHAR_ENUM_write(WowWorldWriter* writer, const vanilla_SMSG_CHAR_ENUM* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_CHAR_ENUM_size(object) + 2); /* size */

    WRITE_U16(0x0000003b); /* opcode */

    WRITE_U8(object->amount_of_characters);

    WRITE_ARRAY(object->characters, object->amount_of_characters, WWM_CHECK_RETURN_CODE(vanilla_Character_write(writer, &object->characters[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_CHAR_ENUM_free(vanilla_SMSG_CHAR_ENUM* object) {
    size_t i;

    for (i = 0; i < object->amount_of_characters; ++i) {
        vanilla_Character_free(&((object->characters)[i]));
    }
    free(object->characters);
    object->characters = NULL;
}

static WowWorldResult vanilla_SMSG_CHAR_DELETE_read(WowWorldReader* reader, vanilla_SMSG_CHAR_DELETE* object) {
    object->result = 0;
    READ_U8(object->result);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_CHAR_DELETE_write(WowWorldWriter* writer, const vanilla_SMSG_CHAR_DELETE* object) {
    WRITE_U16_BE(0x0001 + 2); /* size */

    WRITE_U16(0x0000003c); /* opcode */

    WRITE_U8(object->result);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_PLAYER_LOGIN_read(WowWorldReader* reader, vanilla_CMSG_PLAYER_LOGIN* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_PLAYER_LOGIN_write(WowWorldWriter* writer, const vanilla_CMSG_PLAYER_LOGIN* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x0000003d); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_NEW_WORLD_read(WowWorldReader* reader, vanilla_SMSG_NEW_WORLD* object) {
    object->map = 0;
    READ_U32(object->map);

    WWM_CHECK_RETURN_CODE(all_Vector3d_read(reader, &object->position));

    READ_FLOAT(object->orientation);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_NEW_WORLD_write(WowWorldWriter* writer, const vanilla_SMSG_NEW_WORLD* object) {
    WRITE_U16_BE(0x0014 + 2); /* size */

    WRITE_U16(0x0000003e); /* opcode */

    WRITE_U32(object->map);

    WWM_CHECK_RETURN_CODE(all_Vector3d_write(writer, &object->position));

    WRITE_FLOAT(object->orientation);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_TRANSFER_PENDING_size(const vanilla_SMSG_TRANSFER_PENDING* object) {
    size_t _size = 4;

    if(object->has_transport) {
        _size += 8;
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_TRANSFER_PENDING_read(WowWorldReader* reader, vanilla_SMSG_TRANSFER_PENDING* object, size_t body_size) {
    size_t _size = 0;

    object->map = 0;
    READ_U32(object->map);
    _size += 4;

    object->has_transport = NULL;
    if (_size < body_size) {
        object->has_transport = malloc(sizeof(vanilla_SMSG_TRANSFER_PENDING_has_transport));

        READ_U32(object->has_transport->transport);

        object->has_transport->transport_map = 0;
        READ_U32(object->has_transport->transport_map);

    }
    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_TRANSFER_PENDING_write(WowWorldWriter* writer, const vanilla_SMSG_TRANSFER_PENDING* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_TRANSFER_PENDING_size(object) + 2); /* size */

    WRITE_U16(0x0000003f); /* opcode */

    WRITE_U32(object->map);

    if(object->has_transport) {
        WRITE_U32(object->has_transport->transport);

        WRITE_U32(object->has_transport->transport_map);

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_TRANSFER_PENDING_free(vanilla_SMSG_TRANSFER_PENDING* object) {
    free(object->has_transport);
}

static WowWorldResult vanilla_SMSG_TRANSFER_ABORTED_read(WowWorldReader* reader, vanilla_SMSG_TRANSFER_ABORTED* object) {
    object->map = 0;
    READ_U32(object->map);

    object->reason = 0;
    READ_U8(object->reason);

    READ_U8(object->argument);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_TRANSFER_ABORTED_write(WowWorldWriter* writer, const vanilla_SMSG_TRANSFER_ABORTED* object) {
    WRITE_U16_BE(0x0006 + 2); /* size */

    WRITE_U16(0x00000040); /* opcode */

    WRITE_U32(object->map);

    WRITE_U8(object->reason);

    WRITE_U8(object->argument);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_CHARACTER_LOGIN_FAILED_read(WowWorldReader* reader, vanilla_SMSG_CHARACTER_LOGIN_FAILED* object) {
    object->result = 0;
    READ_U8(object->result);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_CHARACTER_LOGIN_FAILED_write(WowWorldWriter* writer, const vanilla_SMSG_CHARACTER_LOGIN_FAILED* object) {
    WRITE_U16_BE(0x0001 + 2); /* size */

    WRITE_U16(0x00000041); /* opcode */

    WRITE_U8(object->result);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_LOGIN_SETTIMESPEED_read(WowWorldReader* reader, vanilla_SMSG_LOGIN_SETTIMESPEED* object) {
    READ_U32(object->datetime);

    READ_FLOAT(object->timescale);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_LOGIN_SETTIMESPEED_write(WowWorldWriter* writer, const vanilla_SMSG_LOGIN_SETTIMESPEED* object) {
    WRITE_U16_BE(0x0008 + 2); /* size */

    WRITE_U16(0x00000042); /* opcode */

    WRITE_U32(object->datetime);

    WRITE_FLOAT(object->timescale);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_PLAYER_LOGOUT_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x0000004a); /* opcode */


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_LOGOUT_REQUEST_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x0000004b); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_LOGOUT_RESPONSE_read(WowWorldReader* reader, vanilla_SMSG_LOGOUT_RESPONSE* object) {
    object->result = 0;
    READ_U32(object->result);

    object->speed = 0;
    READ_U8(object->speed);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_LOGOUT_RESPONSE_write(WowWorldWriter* writer, const vanilla_SMSG_LOGOUT_RESPONSE* object) {
    WRITE_U16_BE(0x0005 + 2); /* size */

    WRITE_U16(0x0000004c); /* opcode */

    WRITE_U32(object->result);

    WRITE_U8(object->speed);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_LOGOUT_COMPLETE_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 2); /* size */

    WRITE_U16(0x0000004d); /* opcode */


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_LOGOUT_CANCEL_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x0000004e); /* opcode */


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_LOGOUT_CANCEL_ACK_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 2); /* size */

    WRITE_U16(0x0000004f); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_NAME_QUERY_read(WowWorldReader* reader, vanilla_CMSG_NAME_QUERY* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_NAME_QUERY_write(WowWorldWriter* writer, const vanilla_CMSG_NAME_QUERY* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x00000050); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_NAME_QUERY_RESPONSE_size(const vanilla_SMSG_NAME_QUERY_RESPONSE* object) {
    return 22 + STRING_SIZE(object->character_name) + STRING_SIZE(object->realm_name);
}

static WowWorldResult vanilla_SMSG_NAME_QUERY_RESPONSE_read(WowWorldReader* reader, vanilla_SMSG_NAME_QUERY_RESPONSE* object) {
    READ_U64(object->guid);

    READ_CSTRING(object->character_name);

    READ_CSTRING(object->realm_name);

    object->race = 0;
    READ_U32(object->race);

    object->gender = 0;
    READ_U32(object->gender);

    object->class_type = 0;
    READ_U32(object->class_type);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_NAME_QUERY_RESPONSE_write(WowWorldWriter* writer, const vanilla_SMSG_NAME_QUERY_RESPONSE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_NAME_QUERY_RESPONSE_size(object) + 2); /* size */

    WRITE_U16(0x00000051); /* opcode */

    WRITE_U64(object->guid);

    WRITE_CSTRING(object->character_name);

    WRITE_CSTRING(object->realm_name);

    WRITE_U32(object->race);

    WRITE_U32(object->gender);

    WRITE_U32(object->class_type);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_NAME_QUERY_RESPONSE_free(vanilla_SMSG_NAME_QUERY_RESPONSE* object) {
    FREE_STRING(object->character_name);

    FREE_STRING(object->realm_name);

}

static WowWorldResult vanilla_CMSG_PET_NAME_QUERY_read(WowWorldReader* reader, vanilla_CMSG_PET_NAME_QUERY* object) {
    READ_U32(object->pet_number);

    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_PET_NAME_QUERY_write(WowWorldWriter* writer, const vanilla_CMSG_PET_NAME_QUERY* object) {
    WRITE_U16_BE(0x000c + 4); /* size */

    WRITE_U32(0x00000052); /* opcode */

    WRITE_U32(object->pet_number);

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_PET_NAME_QUERY_RESPONSE_size(const vanilla_SMSG_PET_NAME_QUERY_RESPONSE* object) {
    return 9 + STRING_SIZE(object->name);
}

static WowWorldResult vanilla_SMSG_PET_NAME_QUERY_RESPONSE_read(WowWorldReader* reader, vanilla_SMSG_PET_NAME_QUERY_RESPONSE* object) {
    READ_U32(object->pet_number);

    READ_CSTRING(object->name);

    READ_U32(object->pet_name_timestamp);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PET_NAME_QUERY_RESPONSE_write(WowWorldWriter* writer, const vanilla_SMSG_PET_NAME_QUERY_RESPONSE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_PET_NAME_QUERY_RESPONSE_size(object) + 2); /* size */

    WRITE_U16(0x00000053); /* opcode */

    WRITE_U32(object->pet_number);

    WRITE_CSTRING(object->name);

    WRITE_U32(object->pet_name_timestamp);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_PET_NAME_QUERY_RESPONSE_free(vanilla_SMSG_PET_NAME_QUERY_RESPONSE* object) {
    FREE_STRING(object->name);

}

static WowWorldResult vanilla_CMSG_GUILD_QUERY_read(WowWorldReader* reader, vanilla_CMSG_GUILD_QUERY* object) {
    READ_U32(object->guild_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GUILD_QUERY_write(WowWorldWriter* writer, const vanilla_CMSG_GUILD_QUERY* object) {
    WRITE_U16_BE(0x0004 + 4); /* size */

    WRITE_U32(0x00000054); /* opcode */

    WRITE_U32(object->guild_id);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_GUILD_QUERY_RESPONSE_size(const vanilla_SMSG_GUILD_QUERY_RESPONSE* object) {
    size_t _size = 25 + STRING_SIZE(object->name);

    /* C89 scope to allow variable declarations */ {
        int i;
        for(i = 0; i < 10; ++i) {
            _size += STRING_SIZE((*object->rank_names[i])) + 1;
        }
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_GUILD_QUERY_RESPONSE_read(WowWorldReader* reader, vanilla_SMSG_GUILD_QUERY_RESPONSE* object) {
    READ_U32(object->id);

    READ_CSTRING(object->name);

    READ_ARRAY_ALLOCATE(object->rank_names, 10, sizeof(WowWorldString));
    READ_ARRAY(object->rank_names, 10, READ_CSTRING((*object->rank_names)[i]));

    READ_U32(object->emblem_style);

    READ_U32(object->emblem_color);

    READ_U32(object->border_style);

    READ_U32(object->border_color);

    READ_U32(object->background_color);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_GUILD_QUERY_RESPONSE_write(WowWorldWriter* writer, const vanilla_SMSG_GUILD_QUERY_RESPONSE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_GUILD_QUERY_RESPONSE_size(object) + 2); /* size */

    WRITE_U16(0x00000055); /* opcode */

    WRITE_U32(object->id);

    WRITE_CSTRING(object->name);

    WRITE_ARRAY(object->rank_names, 10, WRITE_CSTRING((*object->rank_names)[i]));

    WRITE_U32(object->emblem_style);

    WRITE_U32(object->emblem_color);

    WRITE_U32(object->border_style);

    WRITE_U32(object->border_color);

    WRITE_U32(object->background_color);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_GUILD_QUERY_RESPONSE_free(vanilla_SMSG_GUILD_QUERY_RESPONSE* object) {
    size_t i;

    FREE_STRING(object->name);

    for (i = 0; i < 10; ++i) {
        FREE_STRING(((*object->rank_names)[i]));
    }
    free(object->rank_names);
    object->rank_names = NULL;
}

static WowWorldResult vanilla_CMSG_ITEM_QUERY_SINGLE_read(WowWorldReader* reader, vanilla_CMSG_ITEM_QUERY_SINGLE* object) {
    READ_U32(object->item);

    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_ITEM_QUERY_SINGLE_write(WowWorldWriter* writer, const vanilla_CMSG_ITEM_QUERY_SINGLE* object) {
    WRITE_U16_BE(0x000c + 4); /* size */

    WRITE_U32(0x00000056); /* opcode */

    WRITE_U32(object->item);

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_ITEM_QUERY_SINGLE_RESPONSE_size(const vanilla_SMSG_ITEM_QUERY_SINGLE_RESPONSE* object) {
    size_t _size = 4;

    if(object->found) {
        _size += 453 + STRING_SIZE(object->found->name1) + STRING_SIZE(object->found->name2) + STRING_SIZE(object->found->name3) + STRING_SIZE(object->found->name4) + STRING_SIZE(object->found->description);
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_ITEM_QUERY_SINGLE_RESPONSE_read(WowWorldReader* reader, vanilla_SMSG_ITEM_QUERY_SINGLE_RESPONSE* object, size_t body_size) {
    size_t _size = 0;

    READ_U32(object->item);
    _size += 4;

    object->found = NULL;
    if (_size < body_size) {
        object->found = malloc(sizeof(vanilla_SMSG_ITEM_QUERY_SINGLE_RESPONSE_found));

        object->found->class_and_sub_class = 0;
        READ_U64(object->found->class_and_sub_class);

        READ_CSTRING(object->found->name1);

        READ_CSTRING(object->found->name2);

        READ_CSTRING(object->found->name3);

        READ_CSTRING(object->found->name4);

        READ_U32(object->found->display_id);

        object->found->quality = 0;
        READ_U32(object->found->quality);

        object->found->flags = 0;
        READ_U32(object->found->flags);

        READ_U32(object->found->buy_price);

        READ_U32(object->found->sell_price);

        object->found->inventory_type = 0;
        READ_U32(object->found->inventory_type);

        object->found->allowed_class = 0;
        READ_U32(object->found->allowed_class);

        object->found->allowed_race = 0;
        READ_U32(object->found->allowed_race);

        READ_U32(object->found->item_level);

        READ_U32(object->found->required_level);

        object->found->required_skill = 0;
        READ_U32(object->found->required_skill);

        READ_U32(object->found->required_skill_rank);

        READ_U32(object->found->required_spell);

        READ_U32(object->found->required_honor_rank);

        READ_U32(object->found->required_city_rank);

        object->found->required_faction = 0;
        READ_U32(object->found->required_faction);

        READ_U32(object->found->required_faction_rank);

        READ_U32(object->found->max_count);

        READ_U32(object->found->stackable);

        READ_U32(object->found->container_slots);

        READ_ARRAY_ALLOCATE(object->found->stats, 10, sizeof(vanilla_ItemStat));
        READ_ARRAY(object->found->stats, 10, WWM_CHECK_RETURN_CODE(vanilla_ItemStat_read(reader, &(*object->found->stats)[i])));

        READ_ARRAY_ALLOCATE(object->found->damages, 5, sizeof(vanilla_ItemDamageType));
        READ_ARRAY(object->found->damages, 5, WWM_CHECK_RETURN_CODE(vanilla_ItemDamageType_read(reader, &(*object->found->damages)[i])));

        READ_I32(object->found->armor);

        READ_I32(object->found->holy_resistance);

        READ_I32(object->found->fire_resistance);

        READ_I32(object->found->nature_resistance);

        READ_I32(object->found->frost_resistance);

        READ_I32(object->found->shadow_resistance);

        READ_I32(object->found->arcane_resistance);

        READ_U32(object->found->delay);

        READ_U32(object->found->ammo_type);

        READ_FLOAT(object->found->ranged_range_modification);

        READ_ARRAY_ALLOCATE(object->found->spells, 5, sizeof(vanilla_ItemSpells));
        READ_ARRAY(object->found->spells, 5, WWM_CHECK_RETURN_CODE(vanilla_ItemSpells_read(reader, &(*object->found->spells)[i])));

        object->found->bonding = 0;
        READ_U32(object->found->bonding);

        READ_CSTRING(object->found->description);

        READ_U32(object->found->page_text);

        object->found->language = 0;
        READ_U32(object->found->language);

        object->found->page_text_material = 0;
        READ_U32(object->found->page_text_material);

        READ_U32(object->found->start_quest);

        READ_U32(object->found->lock_id);

        READ_U32(object->found->material);

        object->found->sheathe_type = 0;
        READ_U32(object->found->sheathe_type);

        READ_U32(object->found->random_property);

        READ_U32(object->found->block);

        object->found->item_set = 0;
        READ_U32(object->found->item_set);

        READ_U32(object->found->max_durability);

        object->found->area = 0;
        READ_U32(object->found->area);

        object->found->map = 0;
        READ_U32(object->found->map);

        object->found->bag_family = 0;
        READ_U32(object->found->bag_family);

    }
    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_ITEM_QUERY_SINGLE_RESPONSE_write(WowWorldWriter* writer, const vanilla_SMSG_ITEM_QUERY_SINGLE_RESPONSE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_ITEM_QUERY_SINGLE_RESPONSE_size(object) + 2); /* size */

    WRITE_U16(0x00000058); /* opcode */

    WRITE_U32(object->item);

    if(object->found) {
        WRITE_U64(object->found->class_and_sub_class);

        WRITE_CSTRING(object->found->name1);

        WRITE_CSTRING(object->found->name2);

        WRITE_CSTRING(object->found->name3);

        WRITE_CSTRING(object->found->name4);

        WRITE_U32(object->found->display_id);

        WRITE_U32(object->found->quality);

        WRITE_U32(object->found->flags);

        WRITE_U32(object->found->buy_price);

        WRITE_U32(object->found->sell_price);

        WRITE_U32(object->found->inventory_type);

        WRITE_U32(object->found->allowed_class);

        WRITE_U32(object->found->allowed_race);

        WRITE_U32(object->found->item_level);

        WRITE_U32(object->found->required_level);

        WRITE_U32(object->found->required_skill);

        WRITE_U32(object->found->required_skill_rank);

        WRITE_U32(object->found->required_spell);

        WRITE_U32(object->found->required_honor_rank);

        WRITE_U32(object->found->required_city_rank);

        WRITE_U32(object->found->required_faction);

        WRITE_U32(object->found->required_faction_rank);

        WRITE_U32(object->found->max_count);

        WRITE_U32(object->found->stackable);

        WRITE_U32(object->found->container_slots);

        WRITE_ARRAY(object->found->stats, 10, WWM_CHECK_RETURN_CODE(vanilla_ItemStat_write(writer, &(*object->found->stats)[i])));

        WRITE_ARRAY(object->found->damages, 5, WWM_CHECK_RETURN_CODE(vanilla_ItemDamageType_write(writer, &(*object->found->damages)[i])));

        WRITE_I32(object->found->armor);

        WRITE_I32(object->found->holy_resistance);

        WRITE_I32(object->found->fire_resistance);

        WRITE_I32(object->found->nature_resistance);

        WRITE_I32(object->found->frost_resistance);

        WRITE_I32(object->found->shadow_resistance);

        WRITE_I32(object->found->arcane_resistance);

        WRITE_U32(object->found->delay);

        WRITE_U32(object->found->ammo_type);

        WRITE_FLOAT(object->found->ranged_range_modification);

        WRITE_ARRAY(object->found->spells, 5, WWM_CHECK_RETURN_CODE(vanilla_ItemSpells_write(writer, &(*object->found->spells)[i])));

        WRITE_U32(object->found->bonding);

        WRITE_CSTRING(object->found->description);

        WRITE_U32(object->found->page_text);

        WRITE_U32(object->found->language);

        WRITE_U32(object->found->page_text_material);

        WRITE_U32(object->found->start_quest);

        WRITE_U32(object->found->lock_id);

        WRITE_U32(object->found->material);

        WRITE_U32(object->found->sheathe_type);

        WRITE_U32(object->found->random_property);

        WRITE_U32(object->found->block);

        WRITE_U32(object->found->item_set);

        WRITE_U32(object->found->max_durability);

        WRITE_U32(object->found->area);

        WRITE_U32(object->found->map);

        WRITE_U32(object->found->bag_family);

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_ITEM_QUERY_SINGLE_RESPONSE_free(vanilla_SMSG_ITEM_QUERY_SINGLE_RESPONSE* object) {
    FREE_STRING(object->found->name1);

    FREE_STRING(object->found->name2);

    FREE_STRING(object->found->name3);

    FREE_STRING(object->found->name4);

    free(object->found->stats);
    object->found->stats = NULL;
    free(object->found->damages);
    object->found->damages = NULL;
    free(object->found->spells);
    object->found->spells = NULL;
    FREE_STRING(object->found->description);

    free(object->found);
}

static WowWorldResult vanilla_CMSG_PAGE_TEXT_QUERY_read(WowWorldReader* reader, vanilla_CMSG_PAGE_TEXT_QUERY* object) {
    READ_U32(object->page_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_PAGE_TEXT_QUERY_write(WowWorldWriter* writer, const vanilla_CMSG_PAGE_TEXT_QUERY* object) {
    WRITE_U16_BE(0x0004 + 4); /* size */

    WRITE_U32(0x0000005a); /* opcode */

    WRITE_U32(object->page_id);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_PAGE_TEXT_QUERY_RESPONSE_size(const vanilla_SMSG_PAGE_TEXT_QUERY_RESPONSE* object) {
    return 9 + STRING_SIZE(object->text);
}

static WowWorldResult vanilla_SMSG_PAGE_TEXT_QUERY_RESPONSE_read(WowWorldReader* reader, vanilla_SMSG_PAGE_TEXT_QUERY_RESPONSE* object) {
    READ_U32(object->page_id);

    READ_CSTRING(object->text);

    READ_U32(object->next_page_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PAGE_TEXT_QUERY_RESPONSE_write(WowWorldWriter* writer, const vanilla_SMSG_PAGE_TEXT_QUERY_RESPONSE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_PAGE_TEXT_QUERY_RESPONSE_size(object) + 2); /* size */

    WRITE_U16(0x0000005b); /* opcode */

    WRITE_U32(object->page_id);

    WRITE_CSTRING(object->text);

    WRITE_U32(object->next_page_id);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_PAGE_TEXT_QUERY_RESPONSE_free(vanilla_SMSG_PAGE_TEXT_QUERY_RESPONSE* object) {
    FREE_STRING(object->text);

}

static WowWorldResult vanilla_CMSG_QUEST_QUERY_read(WowWorldReader* reader, vanilla_CMSG_QUEST_QUERY* object) {
    READ_U32(object->quest_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_QUEST_QUERY_write(WowWorldWriter* writer, const vanilla_CMSG_QUEST_QUERY* object) {
    WRITE_U16_BE(0x0004 + 4); /* size */

    WRITE_U32(0x0000005c); /* opcode */

    WRITE_U32(object->quest_id);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_QUEST_QUERY_RESPONSE_size(const vanilla_SMSG_QUEST_QUERY_RESPONSE* object) {
    size_t _size = 220 + STRING_SIZE(object->title) + STRING_SIZE(object->objective_text) + STRING_SIZE(object->details) + STRING_SIZE(object->end_text);

    /* C89 scope to allow variable declarations */ {
        int i;
        for(i = 0; i < 4; ++i) {
            _size += STRING_SIZE((*object->objective_texts[i])) + 1;
        }
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_QUEST_QUERY_RESPONSE_read(WowWorldReader* reader, vanilla_SMSG_QUEST_QUERY_RESPONSE* object) {
    READ_U32(object->quest_id);

    READ_U32(object->quest_method);

    READ_U32(object->quest_level);

    READ_U32(object->zone_or_sort);

    READ_U32(object->quest_type);

    object->reputation_objective_faction = 0;
    READ_U16(object->reputation_objective_faction);

    READ_U32(object->reputation_objective_value);

    object->required_opposite_faction = 0;
    READ_U16(object->required_opposite_faction);

    READ_U32(object->required_opposite_reputation_value);

    READ_U32(object->next_quest_in_chain);

    READ_U32(object->money_reward);

    READ_U32(object->max_level_money_reward);

    READ_U32(object->reward_spell);

    READ_U32(object->source_item_id);

    READ_U32(object->quest_flags);

    READ_ARRAY_ALLOCATE(object->rewards, 4, sizeof(vanilla_QuestItemReward));
    READ_ARRAY(object->rewards, 4, WWM_CHECK_RETURN_CODE(vanilla_QuestItemReward_read(reader, &(*object->rewards)[i])));

    READ_ARRAY_ALLOCATE(object->choice_rewards, 6, sizeof(vanilla_QuestItemReward));
    READ_ARRAY(object->choice_rewards, 6, WWM_CHECK_RETURN_CODE(vanilla_QuestItemReward_read(reader, &(*object->choice_rewards)[i])));

    READ_U32(object->point_map_id);

    WWM_CHECK_RETURN_CODE(all_Vector2d_read(reader, &object->position));

    READ_U32(object->point_opt);

    READ_CSTRING(object->title);

    READ_CSTRING(object->objective_text);

    READ_CSTRING(object->details);

    READ_CSTRING(object->end_text);

    READ_ARRAY_ALLOCATE(object->objectives, 4, sizeof(vanilla_QuestObjective));
    READ_ARRAY(object->objectives, 4, WWM_CHECK_RETURN_CODE(vanilla_QuestObjective_read(reader, &(*object->objectives)[i])));

    READ_ARRAY_ALLOCATE(object->objective_texts, 4, sizeof(WowWorldString));
    READ_ARRAY(object->objective_texts, 4, READ_CSTRING((*object->objective_texts)[i]));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_QUEST_QUERY_RESPONSE_write(WowWorldWriter* writer, const vanilla_SMSG_QUEST_QUERY_RESPONSE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_QUEST_QUERY_RESPONSE_size(object) + 2); /* size */

    WRITE_U16(0x0000005d); /* opcode */

    WRITE_U32(object->quest_id);

    WRITE_U32(object->quest_method);

    WRITE_U32(object->quest_level);

    WRITE_U32(object->zone_or_sort);

    WRITE_U32(object->quest_type);

    WRITE_U16(object->reputation_objective_faction);

    WRITE_U32(object->reputation_objective_value);

    WRITE_U16(object->required_opposite_faction);

    WRITE_U32(object->required_opposite_reputation_value);

    WRITE_U32(object->next_quest_in_chain);

    WRITE_U32(object->money_reward);

    WRITE_U32(object->max_level_money_reward);

    WRITE_U32(object->reward_spell);

    WRITE_U32(object->source_item_id);

    WRITE_U32(object->quest_flags);

    WRITE_ARRAY(object->rewards, 4, WWM_CHECK_RETURN_CODE(vanilla_QuestItemReward_write(writer, &(*object->rewards)[i])));

    WRITE_ARRAY(object->choice_rewards, 6, WWM_CHECK_RETURN_CODE(vanilla_QuestItemReward_write(writer, &(*object->choice_rewards)[i])));

    WRITE_U32(object->point_map_id);

    WWM_CHECK_RETURN_CODE(all_Vector2d_write(writer, &object->position));

    WRITE_U32(object->point_opt);

    WRITE_CSTRING(object->title);

    WRITE_CSTRING(object->objective_text);

    WRITE_CSTRING(object->details);

    WRITE_CSTRING(object->end_text);

    WRITE_ARRAY(object->objectives, 4, WWM_CHECK_RETURN_CODE(vanilla_QuestObjective_write(writer, &(*object->objectives)[i])));

    WRITE_ARRAY(object->objective_texts, 4, WRITE_CSTRING((*object->objective_texts)[i]));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_QUEST_QUERY_RESPONSE_free(vanilla_SMSG_QUEST_QUERY_RESPONSE* object) {
    size_t i;

    free(object->rewards);
    object->rewards = NULL;
    free(object->choice_rewards);
    object->choice_rewards = NULL;
    FREE_STRING(object->title);

    FREE_STRING(object->objective_text);

    FREE_STRING(object->details);

    FREE_STRING(object->end_text);

    free(object->objectives);
    object->objectives = NULL;
    for (i = 0; i < 4; ++i) {
        FREE_STRING(((*object->objective_texts)[i]));
    }
    free(object->objective_texts);
    object->objective_texts = NULL;
}

static WowWorldResult vanilla_CMSG_GAMEOBJECT_QUERY_read(WowWorldReader* reader, vanilla_CMSG_GAMEOBJECT_QUERY* object) {
    READ_U32(object->entry_id);

    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GAMEOBJECT_QUERY_write(WowWorldWriter* writer, const vanilla_CMSG_GAMEOBJECT_QUERY* object) {
    WRITE_U16_BE(0x000c + 4); /* size */

    WRITE_U32(0x0000005e); /* opcode */

    WRITE_U32(object->entry_id);

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_GAMEOBJECT_QUERY_RESPONSE_size(const vanilla_SMSG_GAMEOBJECT_QUERY_RESPONSE* object) {
    size_t _size = 4;

    if(object->found) {
        _size += 37 + STRING_SIZE(object->found->name1) + STRING_SIZE(object->found->name2) + STRING_SIZE(object->found->name3) + STRING_SIZE(object->found->name4) + STRING_SIZE(object->found->name5);
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_GAMEOBJECT_QUERY_RESPONSE_read(WowWorldReader* reader, vanilla_SMSG_GAMEOBJECT_QUERY_RESPONSE* object, size_t body_size) {
    size_t _size = 0;

    READ_U32(object->entry_id);
    _size += 4;

    object->found = NULL;
    if (_size < body_size) {
        object->found = malloc(sizeof(vanilla_SMSG_GAMEOBJECT_QUERY_RESPONSE_found));

        READ_U32(object->found->info_type);

        READ_U32(object->found->display_id);

        READ_CSTRING(object->found->name1);

        READ_CSTRING(object->found->name2);

        READ_CSTRING(object->found->name3);

        READ_CSTRING(object->found->name4);

        READ_CSTRING(object->found->name5);

        READ_ARRAY_ALLOCATE(object->found->raw_data, 6, sizeof(uint32_t));
        READ_ARRAY(object->found->raw_data, 6, READ_U32((*object->found->raw_data)[i]));

    }
    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_GAMEOBJECT_QUERY_RESPONSE_write(WowWorldWriter* writer, const vanilla_SMSG_GAMEOBJECT_QUERY_RESPONSE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_GAMEOBJECT_QUERY_RESPONSE_size(object) + 2); /* size */

    WRITE_U16(0x0000005f); /* opcode */

    WRITE_U32(object->entry_id);

    if(object->found) {
        WRITE_U32(object->found->info_type);

        WRITE_U32(object->found->display_id);

        WRITE_CSTRING(object->found->name1);

        WRITE_CSTRING(object->found->name2);

        WRITE_CSTRING(object->found->name3);

        WRITE_CSTRING(object->found->name4);

        WRITE_CSTRING(object->found->name5);

        WRITE_ARRAY(object->found->raw_data, 6, WRITE_U32((*object->found->raw_data)[i]));

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_GAMEOBJECT_QUERY_RESPONSE_free(vanilla_SMSG_GAMEOBJECT_QUERY_RESPONSE* object) {
    FREE_STRING(object->found->name1);

    FREE_STRING(object->found->name2);

    FREE_STRING(object->found->name3);

    FREE_STRING(object->found->name4);

    FREE_STRING(object->found->name5);

    free(object->found->raw_data);
    object->found->raw_data = NULL;
    free(object->found);
}

static WowWorldResult vanilla_CMSG_CREATURE_QUERY_read(WowWorldReader* reader, vanilla_CMSG_CREATURE_QUERY* object) {
    READ_U32(object->creature);

    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_CREATURE_QUERY_write(WowWorldWriter* writer, const vanilla_CMSG_CREATURE_QUERY* object) {
    WRITE_U16_BE(0x000c + 4); /* size */

    WRITE_U32(0x00000060); /* opcode */

    WRITE_U32(object->creature);

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_CREATURE_QUERY_RESPONSE_size(const vanilla_SMSG_CREATURE_QUERY_RESPONSE* object) {
    size_t _size = 4;

    if(object->found) {
        _size += 35 + STRING_SIZE(object->found->name1) + STRING_SIZE(object->found->name2) + STRING_SIZE(object->found->name3) + STRING_SIZE(object->found->name4) + STRING_SIZE(object->found->sub_name);
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_CREATURE_QUERY_RESPONSE_read(WowWorldReader* reader, vanilla_SMSG_CREATURE_QUERY_RESPONSE* object, size_t body_size) {
    size_t _size = 0;

    READ_U32(object->creature_entry);
    _size += 4;

    object->found = NULL;
    if (_size < body_size) {
        object->found = malloc(sizeof(vanilla_SMSG_CREATURE_QUERY_RESPONSE_found));

        READ_CSTRING(object->found->name1);

        READ_CSTRING(object->found->name2);

        READ_CSTRING(object->found->name3);

        READ_CSTRING(object->found->name4);

        READ_CSTRING(object->found->sub_name);

        READ_U32(object->found->type_flags);

        READ_U32(object->found->creature_type);

        object->found->creature_family = 0;
        READ_U32(object->found->creature_family);

        READ_U32(object->found->creature_rank);

        READ_U32(object->found->unknown0);

        READ_U32(object->found->spell_data_id);

        READ_U32(object->found->display_id);

        READ_U8(object->found->civilian);

        READ_U8(object->found->racial_leader);

    }
    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_CREATURE_QUERY_RESPONSE_write(WowWorldWriter* writer, const vanilla_SMSG_CREATURE_QUERY_RESPONSE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_CREATURE_QUERY_RESPONSE_size(object) + 2); /* size */

    WRITE_U16(0x00000061); /* opcode */

    WRITE_U32(object->creature_entry);

    if(object->found) {
        WRITE_CSTRING(object->found->name1);

        WRITE_CSTRING(object->found->name2);

        WRITE_CSTRING(object->found->name3);

        WRITE_CSTRING(object->found->name4);

        WRITE_CSTRING(object->found->sub_name);

        WRITE_U32(object->found->type_flags);

        WRITE_U32(object->found->creature_type);

        WRITE_U32(object->found->creature_family);

        WRITE_U32(object->found->creature_rank);

        WRITE_U32(object->found->unknown0);

        WRITE_U32(object->found->spell_data_id);

        WRITE_U32(object->found->display_id);

        WRITE_U8(object->found->civilian);

        WRITE_U8(object->found->racial_leader);

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_CREATURE_QUERY_RESPONSE_free(vanilla_SMSG_CREATURE_QUERY_RESPONSE* object) {
    FREE_STRING(object->found->name1);

    FREE_STRING(object->found->name2);

    FREE_STRING(object->found->name3);

    FREE_STRING(object->found->name4);

    FREE_STRING(object->found->sub_name);

    free(object->found);
}

static size_t vanilla_CMSG_WHO_size(const vanilla_CMSG_WHO* object) {
    size_t _size = 26 + STRING_SIZE(object->player_name) + STRING_SIZE(object->guild_name) + 4 * object->amount_of_zones;

    /* C89 scope to allow variable declarations */ {
        int i;
        for(i = 0; i < (int)object->amount_of_strings; ++i) {
            _size += STRING_SIZE(object->search_strings[i]) + 1;
        }
    }

    return _size;
}

static WowWorldResult vanilla_CMSG_WHO_read(WowWorldReader* reader, vanilla_CMSG_WHO* object) {
    READ_U32(object->minimum_level);

    READ_U32(object->maximum_level);

    READ_CSTRING(object->player_name);

    READ_CSTRING(object->guild_name);

    READ_U32(object->race_mask);

    READ_U32(object->class_mask);

    READ_U32(object->amount_of_zones);

    READ_ARRAY_ALLOCATE(object->zones, object->amount_of_zones, sizeof(uint32_t));
    READ_ARRAY(object->zones, object->amount_of_zones, READ_U32(object->zones[i]));

    READ_U32(object->amount_of_strings);

    READ_ARRAY_ALLOCATE(object->search_strings, object->amount_of_strings, sizeof(WowWorldString));
    READ_ARRAY(object->search_strings, object->amount_of_strings, READ_CSTRING(object->search_strings[i]));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_WHO_write(WowWorldWriter* writer, const vanilla_CMSG_WHO* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_WHO_size(object) + 4); /* size */

    WRITE_U32(0x00000062); /* opcode */

    WRITE_U32(object->minimum_level);

    WRITE_U32(object->maximum_level);

    WRITE_CSTRING(object->player_name);

    WRITE_CSTRING(object->guild_name);

    WRITE_U32(object->race_mask);

    WRITE_U32(object->class_mask);

    WRITE_U32(object->amount_of_zones);

    WRITE_ARRAY(object->zones, object->amount_of_zones, WRITE_U32(object->zones[i]));

    WRITE_U32(object->amount_of_strings);

    WRITE_ARRAY(object->search_strings, object->amount_of_strings, WRITE_CSTRING(object->search_strings[i]));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_WHO_free(vanilla_CMSG_WHO* object) {
    size_t i;

    FREE_STRING(object->player_name);

    FREE_STRING(object->guild_name);

    free(object->zones);
    object->zones = NULL;
    for (i = 0; i < object->amount_of_strings; ++i) {
        FREE_STRING(((object->search_strings)[i]));
    }
    free(object->search_strings);
    object->search_strings = NULL;
}

static size_t vanilla_SMSG_WHO_size(const vanilla_SMSG_WHO* object) {
    size_t _size = 8;

    /* C89 scope to allow variable declarations */ {
        int i;
        for(i = 0; i < (int)object->listed_players; ++i) {
            _size += vanilla_WhoPlayer_size(&object->players[i]);
        }
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_WHO_read(WowWorldReader* reader, vanilla_SMSG_WHO* object) {
    READ_U32(object->listed_players);

    READ_U32(object->online_players);

    READ_ARRAY_ALLOCATE(object->players, object->listed_players, sizeof(vanilla_WhoPlayer));
    READ_ARRAY(object->players, object->listed_players, WWM_CHECK_RETURN_CODE(vanilla_WhoPlayer_read(reader, &object->players[i])));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_WHO_write(WowWorldWriter* writer, const vanilla_SMSG_WHO* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_WHO_size(object) + 2); /* size */

    WRITE_U16(0x00000063); /* opcode */

    WRITE_U32(object->listed_players);

    WRITE_U32(object->online_players);

    WRITE_ARRAY(object->players, object->listed_players, WWM_CHECK_RETURN_CODE(vanilla_WhoPlayer_write(writer, &object->players[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_WHO_free(vanilla_SMSG_WHO* object) {
    size_t i;

    for (i = 0; i < object->listed_players; ++i) {
        vanilla_WhoPlayer_free(&((object->players)[i]));
    }
    free(object->players);
    object->players = NULL;
}

static size_t vanilla_CMSG_WHOIS_size(const vanilla_CMSG_WHOIS* object) {
    return 1 + STRING_SIZE(object->character);
}

static WowWorldResult vanilla_CMSG_WHOIS_read(WowWorldReader* reader, vanilla_CMSG_WHOIS* object) {
    READ_CSTRING(object->character);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_WHOIS_write(WowWorldWriter* writer, const vanilla_CMSG_WHOIS* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_WHOIS_size(object) + 4); /* size */

    WRITE_U32(0x00000064); /* opcode */

    WRITE_CSTRING(object->character);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_WHOIS_free(vanilla_CMSG_WHOIS* object) {
    FREE_STRING(object->character);

}

static size_t vanilla_SMSG_WHOIS_size(const vanilla_SMSG_WHOIS* object) {
    return 1 + STRING_SIZE(object->message);
}

static WowWorldResult vanilla_SMSG_WHOIS_read(WowWorldReader* reader, vanilla_SMSG_WHOIS* object) {
    READ_CSTRING(object->message);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_WHOIS_write(WowWorldWriter* writer, const vanilla_SMSG_WHOIS* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_WHOIS_size(object) + 2); /* size */

    WRITE_U16(0x00000065); /* opcode */

    WRITE_CSTRING(object->message);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_WHOIS_free(vanilla_SMSG_WHOIS* object) {
    FREE_STRING(object->message);

}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_FRIEND_LIST_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x00000066); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_FRIEND_LIST_size(const vanilla_SMSG_FRIEND_LIST* object) {
    size_t _size = 1;

    /* C89 scope to allow variable declarations */ {
        int i;
        for(i = 0; i < (int)object->amount_of_friends; ++i) {
            _size += vanilla_Friend_size(&object->friends[i]);
        }
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_FRIEND_LIST_read(WowWorldReader* reader, vanilla_SMSG_FRIEND_LIST* object) {
    READ_U8(object->amount_of_friends);

    READ_ARRAY_ALLOCATE(object->friends, object->amount_of_friends, sizeof(vanilla_Friend));
    READ_ARRAY(object->friends, object->amount_of_friends, WWM_CHECK_RETURN_CODE(vanilla_Friend_read(reader, &object->friends[i])));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_FRIEND_LIST_write(WowWorldWriter* writer, const vanilla_SMSG_FRIEND_LIST* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_FRIEND_LIST_size(object) + 2); /* size */

    WRITE_U16(0x00000067); /* opcode */

    WRITE_U8(object->amount_of_friends);

    WRITE_ARRAY(object->friends, object->amount_of_friends, WWM_CHECK_RETURN_CODE(vanilla_Friend_write(writer, &object->friends[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_FRIEND_LIST_free(vanilla_SMSG_FRIEND_LIST* object) {
    size_t i;

    for (i = 0; i < object->amount_of_friends; ++i) {
        vanilla_Friend_free(&((object->friends)[i]));
    }
    free(object->friends);
    object->friends = NULL;
}

static WowWorldResult vanilla_SMSG_FRIEND_STATUS_read(WowWorldReader* reader, vanilla_SMSG_FRIEND_STATUS* object) {
    object->result = 0;
    READ_U8(object->result);

    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_FRIEND_STATUS_write(WowWorldWriter* writer, const vanilla_SMSG_FRIEND_STATUS* object) {
    WRITE_U16_BE(0x0009 + 2); /* size */

    WRITE_U16(0x00000068); /* opcode */

    WRITE_U8(object->result);

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_ADD_FRIEND_size(const vanilla_CMSG_ADD_FRIEND* object) {
    return 1 + STRING_SIZE(object->name);
}

static WowWorldResult vanilla_CMSG_ADD_FRIEND_read(WowWorldReader* reader, vanilla_CMSG_ADD_FRIEND* object) {
    READ_CSTRING(object->name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_ADD_FRIEND_write(WowWorldWriter* writer, const vanilla_CMSG_ADD_FRIEND* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_ADD_FRIEND_size(object) + 4); /* size */

    WRITE_U32(0x00000069); /* opcode */

    WRITE_CSTRING(object->name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_ADD_FRIEND_free(vanilla_CMSG_ADD_FRIEND* object) {
    FREE_STRING(object->name);

}

static WowWorldResult vanilla_CMSG_DEL_FRIEND_read(WowWorldReader* reader, vanilla_CMSG_DEL_FRIEND* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_DEL_FRIEND_write(WowWorldWriter* writer, const vanilla_CMSG_DEL_FRIEND* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x0000006a); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_IGNORE_LIST_size(const vanilla_SMSG_IGNORE_LIST* object) {
    return 1 + 8 * object->amount_of_ignored;
}

static WowWorldResult vanilla_SMSG_IGNORE_LIST_read(WowWorldReader* reader, vanilla_SMSG_IGNORE_LIST* object) {
    READ_U8(object->amount_of_ignored);

    READ_ARRAY_ALLOCATE(object->ignored, object->amount_of_ignored, sizeof(uint64_t));
    READ_ARRAY(object->ignored, object->amount_of_ignored, READ_U64(object->ignored[i]));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_IGNORE_LIST_write(WowWorldWriter* writer, const vanilla_SMSG_IGNORE_LIST* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_IGNORE_LIST_size(object) + 2); /* size */

    WRITE_U16(0x0000006b); /* opcode */

    WRITE_U8(object->amount_of_ignored);

    WRITE_ARRAY(object->ignored, object->amount_of_ignored, WRITE_U64(object->ignored[i]));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_IGNORE_LIST_free(vanilla_SMSG_IGNORE_LIST* object) {
    free(object->ignored);
    object->ignored = NULL;
}

static size_t vanilla_CMSG_ADD_IGNORE_size(const vanilla_CMSG_ADD_IGNORE* object) {
    return 1 + STRING_SIZE(object->name);
}

static WowWorldResult vanilla_CMSG_ADD_IGNORE_read(WowWorldReader* reader, vanilla_CMSG_ADD_IGNORE* object) {
    READ_CSTRING(object->name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_ADD_IGNORE_write(WowWorldWriter* writer, const vanilla_CMSG_ADD_IGNORE* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_ADD_IGNORE_size(object) + 4); /* size */

    WRITE_U32(0x0000006c); /* opcode */

    WRITE_CSTRING(object->name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_ADD_IGNORE_free(vanilla_CMSG_ADD_IGNORE* object) {
    FREE_STRING(object->name);

}

static WowWorldResult vanilla_CMSG_DEL_IGNORE_read(WowWorldReader* reader, vanilla_CMSG_DEL_IGNORE* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_DEL_IGNORE_write(WowWorldWriter* writer, const vanilla_CMSG_DEL_IGNORE* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x0000006d); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_GROUP_INVITE_size(const vanilla_CMSG_GROUP_INVITE* object) {
    return 1 + STRING_SIZE(object->name);
}

static WowWorldResult vanilla_CMSG_GROUP_INVITE_read(WowWorldReader* reader, vanilla_CMSG_GROUP_INVITE* object) {
    READ_CSTRING(object->name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GROUP_INVITE_write(WowWorldWriter* writer, const vanilla_CMSG_GROUP_INVITE* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_GROUP_INVITE_size(object) + 4); /* size */

    WRITE_U32(0x0000006e); /* opcode */

    WRITE_CSTRING(object->name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_GROUP_INVITE_free(vanilla_CMSG_GROUP_INVITE* object) {
    FREE_STRING(object->name);

}

static size_t vanilla_SMSG_GROUP_INVITE_size(const vanilla_SMSG_GROUP_INVITE* object) {
    return 1 + STRING_SIZE(object->name);
}

static WowWorldResult vanilla_SMSG_GROUP_INVITE_read(WowWorldReader* reader, vanilla_SMSG_GROUP_INVITE* object) {
    READ_CSTRING(object->name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_GROUP_INVITE_write(WowWorldWriter* writer, const vanilla_SMSG_GROUP_INVITE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_GROUP_INVITE_size(object) + 2); /* size */

    WRITE_U16(0x0000006f); /* opcode */

    WRITE_CSTRING(object->name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_GROUP_INVITE_free(vanilla_SMSG_GROUP_INVITE* object) {
    FREE_STRING(object->name);

}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GROUP_ACCEPT_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x00000072); /* opcode */


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GROUP_DECLINE_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x00000073); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_GROUP_DECLINE_size(const vanilla_SMSG_GROUP_DECLINE* object) {
    return 1 + STRING_SIZE(object->name);
}

static WowWorldResult vanilla_SMSG_GROUP_DECLINE_read(WowWorldReader* reader, vanilla_SMSG_GROUP_DECLINE* object) {
    READ_CSTRING(object->name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_GROUP_DECLINE_write(WowWorldWriter* writer, const vanilla_SMSG_GROUP_DECLINE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_GROUP_DECLINE_size(object) + 2); /* size */

    WRITE_U16(0x00000074); /* opcode */

    WRITE_CSTRING(object->name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_GROUP_DECLINE_free(vanilla_SMSG_GROUP_DECLINE* object) {
    FREE_STRING(object->name);

}

static size_t vanilla_CMSG_GROUP_UNINVITE_size(const vanilla_CMSG_GROUP_UNINVITE* object) {
    return 1 + STRING_SIZE(object->name);
}

static WowWorldResult vanilla_CMSG_GROUP_UNINVITE_read(WowWorldReader* reader, vanilla_CMSG_GROUP_UNINVITE* object) {
    READ_CSTRING(object->name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GROUP_UNINVITE_write(WowWorldWriter* writer, const vanilla_CMSG_GROUP_UNINVITE* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_GROUP_UNINVITE_size(object) + 4); /* size */

    WRITE_U32(0x00000075); /* opcode */

    WRITE_CSTRING(object->name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_GROUP_UNINVITE_free(vanilla_CMSG_GROUP_UNINVITE* object) {
    FREE_STRING(object->name);

}

static WowWorldResult vanilla_CMSG_GROUP_UNINVITE_GUID_read(WowWorldReader* reader, vanilla_CMSG_GROUP_UNINVITE_GUID* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GROUP_UNINVITE_GUID_write(WowWorldWriter* writer, const vanilla_CMSG_GROUP_UNINVITE_GUID* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x00000076); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_GROUP_UNINVITE_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 2); /* size */

    WRITE_U16(0x00000077); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_GROUP_SET_LEADER_read(WowWorldReader* reader, vanilla_CMSG_GROUP_SET_LEADER* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GROUP_SET_LEADER_write(WowWorldWriter* writer, const vanilla_CMSG_GROUP_SET_LEADER* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x00000078); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_GROUP_SET_LEADER_size(const vanilla_SMSG_GROUP_SET_LEADER* object) {
    return 1 + STRING_SIZE(object->name);
}

static WowWorldResult vanilla_SMSG_GROUP_SET_LEADER_read(WowWorldReader* reader, vanilla_SMSG_GROUP_SET_LEADER* object) {
    READ_CSTRING(object->name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_GROUP_SET_LEADER_write(WowWorldWriter* writer, const vanilla_SMSG_GROUP_SET_LEADER* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_GROUP_SET_LEADER_size(object) + 2); /* size */

    WRITE_U16(0x00000079); /* opcode */

    WRITE_CSTRING(object->name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_GROUP_SET_LEADER_free(vanilla_SMSG_GROUP_SET_LEADER* object) {
    FREE_STRING(object->name);

}

static WowWorldResult vanilla_CMSG_LOOT_METHOD_read(WowWorldReader* reader, vanilla_CMSG_LOOT_METHOD* object) {
    object->loot_setting = 0;
    READ_U32(object->loot_setting);

    READ_U64(object->loot_master);

    object->loot_threshold = 0;
    READ_U32(object->loot_threshold);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_LOOT_METHOD_write(WowWorldWriter* writer, const vanilla_CMSG_LOOT_METHOD* object) {
    WRITE_U16_BE(0x0010 + 4); /* size */

    WRITE_U32(0x0000007a); /* opcode */

    WRITE_U32(object->loot_setting);

    WRITE_U64(object->loot_master);

    WRITE_U32(object->loot_threshold);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GROUP_DISBAND_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x0000007b); /* opcode */


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_GROUP_DESTROYED_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 2); /* size */

    WRITE_U16(0x0000007c); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_GROUP_LIST_size(const vanilla_SMSG_GROUP_LIST* object) {
    size_t _size = 14;

    /* C89 scope to allow variable declarations */ {
        int i;
        for(i = 0; i < (int)object->amount_of_members; ++i) {
            _size += vanilla_GroupListMember_size(&object->members[i]);
        }
    }

    if(object->group_not_empty) {
        _size += 10;
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_GROUP_LIST_read(WowWorldReader* reader, vanilla_SMSG_GROUP_LIST* object, size_t body_size) {
    size_t _size = 0;

    object->group_type = 0;
    READ_U8(object->group_type);
    _size += 1;

    READ_U8(object->flags);
    _size += 1;

    READ_U32(object->amount_of_members);
    _size += 4;

    READ_ARRAY_ALLOCATE(object->members, object->amount_of_members, sizeof(vanilla_GroupListMember));
    READ_ARRAY(object->members, object->amount_of_members, WWM_CHECK_RETURN_CODE(vanilla_GroupListMember_read(reader, &object->members[i]));_size += vanilla_GroupListMember_size(&object->members[i]););

    READ_U64(object->leader);
    _size += 8;

    object->group_not_empty = NULL;
    if (_size < body_size) {
        object->group_not_empty = malloc(sizeof(vanilla_SMSG_GROUP_LIST_group_not_empty));

        object->group_not_empty->loot_setting = 0;
        READ_U8(object->group_not_empty->loot_setting);

        READ_U64(object->group_not_empty->master_loot);

        object->group_not_empty->loot_threshold = 0;
        READ_U8(object->group_not_empty->loot_threshold);

    }
    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_GROUP_LIST_write(WowWorldWriter* writer, const vanilla_SMSG_GROUP_LIST* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_GROUP_LIST_size(object) + 2); /* size */

    WRITE_U16(0x0000007d); /* opcode */

    WRITE_U8(object->group_type);

    WRITE_U8(object->flags);

    WRITE_U32(object->amount_of_members);

    WRITE_ARRAY(object->members, object->amount_of_members, WWM_CHECK_RETURN_CODE(vanilla_GroupListMember_write(writer, &object->members[i])));

    WRITE_U64(object->leader);

    if(object->group_not_empty) {
        WRITE_U8(object->group_not_empty->loot_setting);

        WRITE_U64(object->group_not_empty->master_loot);

        WRITE_U8(object->group_not_empty->loot_threshold);

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_GROUP_LIST_free(vanilla_SMSG_GROUP_LIST* object) {
    size_t i;

    for (i = 0; i < object->amount_of_members; ++i) {
        vanilla_GroupListMember_free(&((object->members)[i]));
    }
    free(object->members);
    object->members = NULL;
    free(object->group_not_empty);
}

static size_t vanilla_SMSG_PARTY_MEMBER_STATS_size(const vanilla_SMSG_PARTY_MEMBER_STATS* object) {
    size_t _size = 4 + wwm_packed_guid_size(object->guid);

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_STATUS) != 0) {
        _size += 1;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_CUR_HP) != 0) {
        _size += 2;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_MAX_HP) != 0) {
        _size += 2;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_POWER_TYPE) != 0) {
        _size += 1;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_CUR_POWER) != 0) {
        _size += 2;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_MAX_POWER) != 0) {
        _size += 2;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_LEVEL) != 0) {
        _size += 2;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_ZONE) != 0) {
        _size += 4;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_POSITION) != 0) {
        _size += 4;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_AURAS) != 0) {
        _size += 0 + vanilla_aura_mask_size(&object->auras);
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_AURAS_2) != 0) {
        _size += 0 + vanilla_aura_mask_size(&object->negative_auras);
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_GUID) != 0) {
        _size += 8;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_NAME) != 0) {
        _size += 1 + STRING_SIZE(object->pet_name);
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_MODEL_ID) != 0) {
        _size += 2;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_CUR_HP) != 0) {
        _size += 2;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_MAX_HP) != 0) {
        _size += 2;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_POWER_TYPE) != 0) {
        _size += 1;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_CUR_POWER) != 0) {
        _size += 2;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_MAX_POWER) != 0) {
        _size += 2;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_AURAS) != 0) {
        _size += 0 + vanilla_aura_mask_size(&object->pet_auras);
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_PARTY_MEMBER_STATS_read(WowWorldReader* reader, vanilla_SMSG_PARTY_MEMBER_STATS* object) {
    READ_PACKED_GUID(object->guid);

    object->mask = 0;
    READ_U32(object->mask);

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_STATUS) != 0) {
        object->status = 0;
        READ_U8(object->status);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_CUR_HP) != 0) {
        READ_U16(object->current_health);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_MAX_HP) != 0) {
        READ_U16(object->max_health);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_POWER_TYPE) != 0) {
        object->power = 0;
        READ_U8(object->power);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_CUR_POWER) != 0) {
        READ_U16(object->current_power);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_MAX_POWER) != 0) {
        READ_U16(object->max_power);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_LEVEL) != 0) {
        READ_U16(object->level);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_ZONE) != 0) {
        object->area = 0;
        READ_U32(object->area);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_POSITION) != 0) {
        READ_U16(object->position_x);

        READ_U16(object->position_y);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_AURAS) != 0) {
        WWM_CHECK_RETURN_CODE(vanilla_aura_mask_read(reader, &object->auras));

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_AURAS_2) != 0) {
        WWM_CHECK_RETURN_CODE(vanilla_aura_mask_read(reader, &object->negative_auras));

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_GUID) != 0) {
        READ_U64(object->pet);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_NAME) != 0) {
        READ_CSTRING(object->pet_name);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_MODEL_ID) != 0) {
        READ_U16(object->pet_display_id);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_CUR_HP) != 0) {
        READ_U16(object->pet_current_health);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_MAX_HP) != 0) {
        READ_U16(object->pet_max_health);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_POWER_TYPE) != 0) {
        object->pet_power_type = 0;
        READ_U8(object->pet_power_type);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_CUR_POWER) != 0) {
        READ_U16(object->pet_current_power);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_MAX_POWER) != 0) {
        READ_U16(object->pet_max_power);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_AURAS) != 0) {
        WWM_CHECK_RETURN_CODE(vanilla_aura_mask_read(reader, &object->pet_auras));

    }
    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PARTY_MEMBER_STATS_write(WowWorldWriter* writer, const vanilla_SMSG_PARTY_MEMBER_STATS* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_PARTY_MEMBER_STATS_size(object) + 2); /* size */

    WRITE_U16(0x0000007e); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WRITE_U32(object->mask);

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_STATUS) != 0) {
        WRITE_U8(object->status);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_CUR_HP) != 0) {
        WRITE_U16(object->current_health);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_MAX_HP) != 0) {
        WRITE_U16(object->max_health);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_POWER_TYPE) != 0) {
        WRITE_U8(object->power);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_CUR_POWER) != 0) {
        WRITE_U16(object->current_power);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_MAX_POWER) != 0) {
        WRITE_U16(object->max_power);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_LEVEL) != 0) {
        WRITE_U16(object->level);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_ZONE) != 0) {
        WRITE_U32(object->area);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_POSITION) != 0) {
        WRITE_U16(object->position_x);

        WRITE_U16(object->position_y);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_AURAS) != 0) {
        WWM_CHECK_RETURN_CODE(vanilla_aura_mask_write(writer, &object->auras));

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_AURAS_2) != 0) {
        WWM_CHECK_RETURN_CODE(vanilla_aura_mask_write(writer, &object->negative_auras));

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_GUID) != 0) {
        WRITE_U64(object->pet);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_NAME) != 0) {
        WRITE_CSTRING(object->pet_name);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_MODEL_ID) != 0) {
        WRITE_U16(object->pet_display_id);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_CUR_HP) != 0) {
        WRITE_U16(object->pet_current_health);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_MAX_HP) != 0) {
        WRITE_U16(object->pet_max_health);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_POWER_TYPE) != 0) {
        WRITE_U8(object->pet_power_type);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_CUR_POWER) != 0) {
        WRITE_U16(object->pet_current_power);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_MAX_POWER) != 0) {
        WRITE_U16(object->pet_max_power);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_AURAS) != 0) {
        WWM_CHECK_RETURN_CODE(vanilla_aura_mask_write(writer, &object->pet_auras));

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_PARTY_MEMBER_STATS_free(vanilla_SMSG_PARTY_MEMBER_STATS* object) {
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_STATUS) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_CUR_HP) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_MAX_HP) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_POWER_TYPE) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_CUR_POWER) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_MAX_POWER) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_LEVEL) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_ZONE) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_POSITION) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_AURAS) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_AURAS_2) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_GUID) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_NAME) != 0) {
        FREE_STRING(object->pet_name);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_MODEL_ID) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_CUR_HP) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_MAX_HP) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_POWER_TYPE) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_CUR_POWER) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_MAX_POWER) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_AURAS) != 0) {
    }
}

static size_t vanilla_SMSG_PARTY_COMMAND_RESULT_size(const vanilla_SMSG_PARTY_COMMAND_RESULT* object) {
    return 9 + STRING_SIZE(object->member);
}

static WowWorldResult vanilla_SMSG_PARTY_COMMAND_RESULT_read(WowWorldReader* reader, vanilla_SMSG_PARTY_COMMAND_RESULT* object) {
    object->operation = 0;
    READ_U32(object->operation);

    READ_CSTRING(object->member);

    object->result = 0;
    READ_U32(object->result);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PARTY_COMMAND_RESULT_write(WowWorldWriter* writer, const vanilla_SMSG_PARTY_COMMAND_RESULT* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_PARTY_COMMAND_RESULT_size(object) + 2); /* size */

    WRITE_U16(0x0000007f); /* opcode */

    WRITE_U32(object->operation);

    WRITE_CSTRING(object->member);

    WRITE_U32(object->result);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_PARTY_COMMAND_RESULT_free(vanilla_SMSG_PARTY_COMMAND_RESULT* object) {
    FREE_STRING(object->member);

}

static size_t vanilla_CMSG_GUILD_CREATE_size(const vanilla_CMSG_GUILD_CREATE* object) {
    return 1 + STRING_SIZE(object->guild_name);
}

static WowWorldResult vanilla_CMSG_GUILD_CREATE_read(WowWorldReader* reader, vanilla_CMSG_GUILD_CREATE* object) {
    READ_CSTRING(object->guild_name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GUILD_CREATE_write(WowWorldWriter* writer, const vanilla_CMSG_GUILD_CREATE* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_GUILD_CREATE_size(object) + 4); /* size */

    WRITE_U32(0x00000081); /* opcode */

    WRITE_CSTRING(object->guild_name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_GUILD_CREATE_free(vanilla_CMSG_GUILD_CREATE* object) {
    FREE_STRING(object->guild_name);

}

static size_t vanilla_CMSG_GUILD_INVITE_size(const vanilla_CMSG_GUILD_INVITE* object) {
    return 1 + STRING_SIZE(object->invited_player);
}

static WowWorldResult vanilla_CMSG_GUILD_INVITE_read(WowWorldReader* reader, vanilla_CMSG_GUILD_INVITE* object) {
    READ_CSTRING(object->invited_player);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GUILD_INVITE_write(WowWorldWriter* writer, const vanilla_CMSG_GUILD_INVITE* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_GUILD_INVITE_size(object) + 4); /* size */

    WRITE_U32(0x00000082); /* opcode */

    WRITE_CSTRING(object->invited_player);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_GUILD_INVITE_free(vanilla_CMSG_GUILD_INVITE* object) {
    FREE_STRING(object->invited_player);

}

static size_t vanilla_SMSG_GUILD_INVITE_size(const vanilla_SMSG_GUILD_INVITE* object) {
    return 2 + STRING_SIZE(object->player_name) + STRING_SIZE(object->guild_name);
}

static WowWorldResult vanilla_SMSG_GUILD_INVITE_read(WowWorldReader* reader, vanilla_SMSG_GUILD_INVITE* object) {
    READ_CSTRING(object->player_name);

    READ_CSTRING(object->guild_name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_GUILD_INVITE_write(WowWorldWriter* writer, const vanilla_SMSG_GUILD_INVITE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_GUILD_INVITE_size(object) + 2); /* size */

    WRITE_U16(0x00000083); /* opcode */

    WRITE_CSTRING(object->player_name);

    WRITE_CSTRING(object->guild_name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_GUILD_INVITE_free(vanilla_SMSG_GUILD_INVITE* object) {
    FREE_STRING(object->player_name);

    FREE_STRING(object->guild_name);

}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GUILD_ACCEPT_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x00000084); /* opcode */


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GUILD_DECLINE_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x00000085); /* opcode */


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GUILD_INFO_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x00000087); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_GUILD_INFO_size(const vanilla_SMSG_GUILD_INFO* object) {
    return 21 + STRING_SIZE(object->guild_name);
}

static WowWorldResult vanilla_SMSG_GUILD_INFO_read(WowWorldReader* reader, vanilla_SMSG_GUILD_INFO* object) {
    READ_CSTRING(object->guild_name);

    READ_U32(object->created_day);

    READ_U32(object->created_month);

    READ_U32(object->created_year);

    READ_U32(object->amount_of_characters_in_guild);

    READ_U32(object->amount_of_accounts_in_guild);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_GUILD_INFO_write(WowWorldWriter* writer, const vanilla_SMSG_GUILD_INFO* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_GUILD_INFO_size(object) + 2); /* size */

    WRITE_U16(0x00000088); /* opcode */

    WRITE_CSTRING(object->guild_name);

    WRITE_U32(object->created_day);

    WRITE_U32(object->created_month);

    WRITE_U32(object->created_year);

    WRITE_U32(object->amount_of_characters_in_guild);

    WRITE_U32(object->amount_of_accounts_in_guild);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_GUILD_INFO_free(vanilla_SMSG_GUILD_INFO* object) {
    FREE_STRING(object->guild_name);

}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GUILD_ROSTER_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x00000089); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_GUILD_ROSTER_size(const vanilla_SMSG_GUILD_ROSTER* object) {
    size_t _size = 10 + STRING_SIZE(object->motd) + STRING_SIZE(object->guild_info) + 4 * object->amount_of_rights;

    /* C89 scope to allow variable declarations */ {
        int i;
        for(i = 0; i < (int)object->amount_of_members; ++i) {
            _size += vanilla_GuildMember_size(&object->members[i]);
        }
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_GUILD_ROSTER_read(WowWorldReader* reader, vanilla_SMSG_GUILD_ROSTER* object) {
    READ_U32(object->amount_of_members);

    READ_CSTRING(object->motd);

    READ_CSTRING(object->guild_info);

    READ_U32(object->amount_of_rights);

    READ_ARRAY_ALLOCATE(object->rights, object->amount_of_rights, sizeof(uint32_t));
    READ_ARRAY(object->rights, object->amount_of_rights, READ_U32(object->rights[i]));

    READ_ARRAY_ALLOCATE(object->members, object->amount_of_members, sizeof(vanilla_GuildMember));
    READ_ARRAY(object->members, object->amount_of_members, WWM_CHECK_RETURN_CODE(vanilla_GuildMember_read(reader, &object->members[i])));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_GUILD_ROSTER_write(WowWorldWriter* writer, const vanilla_SMSG_GUILD_ROSTER* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_GUILD_ROSTER_size(object) + 2); /* size */

    WRITE_U16(0x0000008a); /* opcode */

    WRITE_U32(object->amount_of_members);

    WRITE_CSTRING(object->motd);

    WRITE_CSTRING(object->guild_info);

    WRITE_U32(object->amount_of_rights);

    WRITE_ARRAY(object->rights, object->amount_of_rights, WRITE_U32(object->rights[i]));

    WRITE_ARRAY(object->members, object->amount_of_members, WWM_CHECK_RETURN_CODE(vanilla_GuildMember_write(writer, &object->members[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_GUILD_ROSTER_free(vanilla_SMSG_GUILD_ROSTER* object) {
    size_t i;

    FREE_STRING(object->motd);

    FREE_STRING(object->guild_info);

    free(object->rights);
    object->rights = NULL;
    for (i = 0; i < object->amount_of_members; ++i) {
        vanilla_GuildMember_free(&((object->members)[i]));
    }
    free(object->members);
    object->members = NULL;
}

static size_t vanilla_CMSG_GUILD_PROMOTE_size(const vanilla_CMSG_GUILD_PROMOTE* object) {
    return 1 + STRING_SIZE(object->player_name);
}

static WowWorldResult vanilla_CMSG_GUILD_PROMOTE_read(WowWorldReader* reader, vanilla_CMSG_GUILD_PROMOTE* object) {
    READ_CSTRING(object->player_name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GUILD_PROMOTE_write(WowWorldWriter* writer, const vanilla_CMSG_GUILD_PROMOTE* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_GUILD_PROMOTE_size(object) + 4); /* size */

    WRITE_U32(0x0000008b); /* opcode */

    WRITE_CSTRING(object->player_name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_GUILD_PROMOTE_free(vanilla_CMSG_GUILD_PROMOTE* object) {
    FREE_STRING(object->player_name);

}

static size_t vanilla_CMSG_GUILD_DEMOTE_size(const vanilla_CMSG_GUILD_DEMOTE* object) {
    return 1 + STRING_SIZE(object->player_name);
}

static WowWorldResult vanilla_CMSG_GUILD_DEMOTE_read(WowWorldReader* reader, vanilla_CMSG_GUILD_DEMOTE* object) {
    READ_CSTRING(object->player_name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GUILD_DEMOTE_write(WowWorldWriter* writer, const vanilla_CMSG_GUILD_DEMOTE* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_GUILD_DEMOTE_size(object) + 4); /* size */

    WRITE_U32(0x0000008c); /* opcode */

    WRITE_CSTRING(object->player_name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_GUILD_DEMOTE_free(vanilla_CMSG_GUILD_DEMOTE* object) {
    FREE_STRING(object->player_name);

}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GUILD_LEAVE_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x0000008d); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_GUILD_REMOVE_size(const vanilla_CMSG_GUILD_REMOVE* object) {
    return 1 + STRING_SIZE(object->player_name);
}

static WowWorldResult vanilla_CMSG_GUILD_REMOVE_read(WowWorldReader* reader, vanilla_CMSG_GUILD_REMOVE* object) {
    READ_CSTRING(object->player_name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GUILD_REMOVE_write(WowWorldWriter* writer, const vanilla_CMSG_GUILD_REMOVE* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_GUILD_REMOVE_size(object) + 4); /* size */

    WRITE_U32(0x0000008e); /* opcode */

    WRITE_CSTRING(object->player_name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_GUILD_REMOVE_free(vanilla_CMSG_GUILD_REMOVE* object) {
    FREE_STRING(object->player_name);

}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GUILD_DISBAND_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x0000008f); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_GUILD_LEADER_size(const vanilla_CMSG_GUILD_LEADER* object) {
    return 1 + STRING_SIZE(object->new_guild_leader_name);
}

static WowWorldResult vanilla_CMSG_GUILD_LEADER_read(WowWorldReader* reader, vanilla_CMSG_GUILD_LEADER* object) {
    READ_CSTRING(object->new_guild_leader_name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GUILD_LEADER_write(WowWorldWriter* writer, const vanilla_CMSG_GUILD_LEADER* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_GUILD_LEADER_size(object) + 4); /* size */

    WRITE_U32(0x00000090); /* opcode */

    WRITE_CSTRING(object->new_guild_leader_name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_GUILD_LEADER_free(vanilla_CMSG_GUILD_LEADER* object) {
    FREE_STRING(object->new_guild_leader_name);

}

static size_t vanilla_CMSG_GUILD_MOTD_size(const vanilla_CMSG_GUILD_MOTD* object) {
    return 1 + STRING_SIZE(object->message_of_the_day);
}

static WowWorldResult vanilla_CMSG_GUILD_MOTD_read(WowWorldReader* reader, vanilla_CMSG_GUILD_MOTD* object) {
    READ_CSTRING(object->message_of_the_day);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GUILD_MOTD_write(WowWorldWriter* writer, const vanilla_CMSG_GUILD_MOTD* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_GUILD_MOTD_size(object) + 4); /* size */

    WRITE_U32(0x00000091); /* opcode */

    WRITE_CSTRING(object->message_of_the_day);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_GUILD_MOTD_free(vanilla_CMSG_GUILD_MOTD* object) {
    FREE_STRING(object->message_of_the_day);

}

static size_t vanilla_SMSG_GUILD_EVENT_size(const vanilla_SMSG_GUILD_EVENT* object) {
    size_t _size = 2;

    /* C89 scope to allow variable declarations */ {
        int i;
        for(i = 0; i < (int)object->amount_of_events; ++i) {
            _size += STRING_SIZE(object->event_descriptions[i]) + 1;
        }
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_GUILD_EVENT_read(WowWorldReader* reader, vanilla_SMSG_GUILD_EVENT* object) {
    object->event = 0;
    READ_U8(object->event);

    READ_U8(object->amount_of_events);

    READ_ARRAY_ALLOCATE(object->event_descriptions, object->amount_of_events, sizeof(WowWorldString));
    READ_ARRAY(object->event_descriptions, object->amount_of_events, READ_CSTRING(object->event_descriptions[i]));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_GUILD_EVENT_write(WowWorldWriter* writer, const vanilla_SMSG_GUILD_EVENT* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_GUILD_EVENT_size(object) + 2); /* size */

    WRITE_U16(0x00000092); /* opcode */

    WRITE_U8(object->event);

    WRITE_U8(object->amount_of_events);

    WRITE_ARRAY(object->event_descriptions, object->amount_of_events, WRITE_CSTRING(object->event_descriptions[i]));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_GUILD_EVENT_free(vanilla_SMSG_GUILD_EVENT* object) {
    size_t i;

    for (i = 0; i < object->amount_of_events; ++i) {
        FREE_STRING(((object->event_descriptions)[i]));
    }
    free(object->event_descriptions);
    object->event_descriptions = NULL;
}

static size_t vanilla_SMSG_GUILD_COMMAND_RESULT_size(const vanilla_SMSG_GUILD_COMMAND_RESULT* object) {
    return 9 + STRING_SIZE(object->string);
}

static WowWorldResult vanilla_SMSG_GUILD_COMMAND_RESULT_read(WowWorldReader* reader, vanilla_SMSG_GUILD_COMMAND_RESULT* object) {
    object->command = 0;
    READ_U32(object->command);

    READ_CSTRING(object->string);

    object->result = 0;
    READ_U32(object->result);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_GUILD_COMMAND_RESULT_write(WowWorldWriter* writer, const vanilla_SMSG_GUILD_COMMAND_RESULT* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_GUILD_COMMAND_RESULT_size(object) + 2); /* size */

    WRITE_U16(0x00000093); /* opcode */

    WRITE_U32(object->command);

    WRITE_CSTRING(object->string);

    WRITE_U32(object->result);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_GUILD_COMMAND_RESULT_free(vanilla_SMSG_GUILD_COMMAND_RESULT* object) {
    FREE_STRING(object->string);

}

static size_t vanilla_CMSG_MESSAGECHAT_size(const vanilla_CMSG_MESSAGECHAT* object) {
    size_t _size = 9 + STRING_SIZE(object->message);

    if (object->chat_type == VANILLA_CHAT_TYPE_WHISPER) {
        _size += 1 + STRING_SIZE(object->target_player);
    }
    else if (object->chat_type == VANILLA_CHAT_TYPE_CHANNEL) {
        _size += 1 + STRING_SIZE(object->channel);
    }

    return _size;
}

static WowWorldResult vanilla_CMSG_MESSAGECHAT_read(WowWorldReader* reader, vanilla_CMSG_MESSAGECHAT* object) {
    object->chat_type = 0;
    READ_U32(object->chat_type);

    object->language = 0;
    READ_U32(object->language);

    if (object->chat_type == VANILLA_CHAT_TYPE_WHISPER) {
        READ_CSTRING(object->target_player);

    }
    else if (object->chat_type == VANILLA_CHAT_TYPE_CHANNEL) {
        READ_CSTRING(object->channel);

    }
    READ_CSTRING(object->message);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_MESSAGECHAT_write(WowWorldWriter* writer, const vanilla_CMSG_MESSAGECHAT* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_MESSAGECHAT_size(object) + 4); /* size */

    WRITE_U32(0x00000095); /* opcode */

    WRITE_U32(object->chat_type);

    WRITE_U32(object->language);

    if (object->chat_type == VANILLA_CHAT_TYPE_WHISPER) {
        WRITE_CSTRING(object->target_player);

    }
    else if (object->chat_type == VANILLA_CHAT_TYPE_CHANNEL) {
        WRITE_CSTRING(object->channel);

    }
    WRITE_CSTRING(object->message);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_MESSAGECHAT_free(vanilla_CMSG_MESSAGECHAT* object) {
    if (object->chat_type == VANILLA_CHAT_TYPE_WHISPER) {
        FREE_STRING(object->target_player);

    }
    else if (object->chat_type == VANILLA_CHAT_TYPE_CHANNEL) {
        FREE_STRING(object->channel);

    }
    FREE_STRING(object->message);

}

static size_t vanilla_SMSG_MESSAGECHAT_size(const vanilla_SMSG_MESSAGECHAT* object) {
    size_t _size = 10 + STRING_SIZE(object->message);

    if (object->chat_type == VANILLA_CHAT_TYPE_MONSTER_WHISPER|| object->chat_type == VANILLA_CHAT_TYPE_RAID_BOSS_EMOTE|| object->chat_type == VANILLA_CHAT_TYPE_MONSTER_EMOTE) {
        _size += 12 + STRING_SIZE(object->monster_name);
    }
    else if (object->chat_type == VANILLA_CHAT_TYPE_SAY|| object->chat_type == VANILLA_CHAT_TYPE_PARTY|| object->chat_type == VANILLA_CHAT_TYPE_YELL) {
        _size += 16;
    }
    else if (object->chat_type == VANILLA_CHAT_TYPE_MONSTER_SAY|| object->chat_type == VANILLA_CHAT_TYPE_MONSTER_YELL) {
        _size += 20 + STRING_SIZE(object->sender_name);
    }
    else if (object->chat_type == VANILLA_CHAT_TYPE_CHANNEL) {
        _size += 13 + STRING_SIZE(object->channel_name);
    }
    else if (object->chat_type == VANILLA_CHAT_TYPE_RAID|| object->chat_type == VANILLA_CHAT_TYPE_GUILD|| object->chat_type == VANILLA_CHAT_TYPE_OFFICER|| object->chat_type == VANILLA_CHAT_TYPE_WHISPER|| object->chat_type == VANILLA_CHAT_TYPE_WHISPER_INFORM|| object->chat_type == VANILLA_CHAT_TYPE_EMOTE|| object->chat_type == VANILLA_CHAT_TYPE_TEXT_EMOTE|| object->chat_type == VANILLA_CHAT_TYPE_SYSTEM|| object->chat_type == VANILLA_CHAT_TYPE_CHANNEL_JOIN|| object->chat_type == VANILLA_CHAT_TYPE_CHANNEL_LEAVE|| object->chat_type == VANILLA_CHAT_TYPE_CHANNEL_LIST|| object->chat_type == VANILLA_CHAT_TYPE_CHANNEL_NOTICE|| object->chat_type == VANILLA_CHAT_TYPE_CHANNEL_NOTICE_USER|| object->chat_type == VANILLA_CHAT_TYPE_AFK|| object->chat_type == VANILLA_CHAT_TYPE_DND|| object->chat_type == VANILLA_CHAT_TYPE_IGNORED|| object->chat_type == VANILLA_CHAT_TYPE_SKILL|| object->chat_type == VANILLA_CHAT_TYPE_LOOT|| object->chat_type == VANILLA_CHAT_TYPE_BG_SYSTEM_NEUTRAL|| object->chat_type == VANILLA_CHAT_TYPE_BG_SYSTEM_ALLIANCE|| object->chat_type == VANILLA_CHAT_TYPE_BG_SYSTEM_HORDE|| object->chat_type == VANILLA_CHAT_TYPE_RAID_LEADER|| object->chat_type == VANILLA_CHAT_TYPE_RAID_WARNING|| object->chat_type == VANILLA_CHAT_TYPE_RAID_BOSS_WHISPER|| object->chat_type == VANILLA_CHAT_TYPE_BATTLEGROUND|| object->chat_type == VANILLA_CHAT_TYPE_BATTLEGROUND_LEADER) {
        _size += 8;
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_MESSAGECHAT_read(WowWorldReader* reader, vanilla_SMSG_MESSAGECHAT* object) {
    object->chat_type = 0;
    READ_U8(object->chat_type);

    object->language = 0;
    READ_U32(object->language);

    if (object->chat_type == VANILLA_CHAT_TYPE_MONSTER_WHISPER|| object->chat_type == VANILLA_CHAT_TYPE_RAID_BOSS_EMOTE|| object->chat_type == VANILLA_CHAT_TYPE_MONSTER_EMOTE) {
        READ_SIZED_CSTRING(object->monster_name);

        READ_U64(object->monster);

    }
    else if (object->chat_type == VANILLA_CHAT_TYPE_SAY|| object->chat_type == VANILLA_CHAT_TYPE_PARTY|| object->chat_type == VANILLA_CHAT_TYPE_YELL) {
        READ_U64(object->speech_bubble_credit);

        READ_U64(object->chat_credit);

    }
    else if (object->chat_type == VANILLA_CHAT_TYPE_MONSTER_SAY|| object->chat_type == VANILLA_CHAT_TYPE_MONSTER_YELL) {
        READ_U64(object->sender1);

        READ_SIZED_CSTRING(object->sender_name);

        READ_U64(object->target);

    }
    else if (object->chat_type == VANILLA_CHAT_TYPE_CHANNEL) {
        READ_CSTRING(object->channel_name);

        READ_U32(object->player_rank);

        READ_U64(object->player);

    }
    else if (object->chat_type == VANILLA_CHAT_TYPE_RAID|| object->chat_type == VANILLA_CHAT_TYPE_GUILD|| object->chat_type == VANILLA_CHAT_TYPE_OFFICER|| object->chat_type == VANILLA_CHAT_TYPE_WHISPER|| object->chat_type == VANILLA_CHAT_TYPE_WHISPER_INFORM|| object->chat_type == VANILLA_CHAT_TYPE_EMOTE|| object->chat_type == VANILLA_CHAT_TYPE_TEXT_EMOTE|| object->chat_type == VANILLA_CHAT_TYPE_SYSTEM|| object->chat_type == VANILLA_CHAT_TYPE_CHANNEL_JOIN|| object->chat_type == VANILLA_CHAT_TYPE_CHANNEL_LEAVE|| object->chat_type == VANILLA_CHAT_TYPE_CHANNEL_LIST|| object->chat_type == VANILLA_CHAT_TYPE_CHANNEL_NOTICE|| object->chat_type == VANILLA_CHAT_TYPE_CHANNEL_NOTICE_USER|| object->chat_type == VANILLA_CHAT_TYPE_AFK|| object->chat_type == VANILLA_CHAT_TYPE_DND|| object->chat_type == VANILLA_CHAT_TYPE_IGNORED|| object->chat_type == VANILLA_CHAT_TYPE_SKILL|| object->chat_type == VANILLA_CHAT_TYPE_LOOT|| object->chat_type == VANILLA_CHAT_TYPE_BG_SYSTEM_NEUTRAL|| object->chat_type == VANILLA_CHAT_TYPE_BG_SYSTEM_ALLIANCE|| object->chat_type == VANILLA_CHAT_TYPE_BG_SYSTEM_HORDE|| object->chat_type == VANILLA_CHAT_TYPE_RAID_LEADER|| object->chat_type == VANILLA_CHAT_TYPE_RAID_WARNING|| object->chat_type == VANILLA_CHAT_TYPE_RAID_BOSS_WHISPER|| object->chat_type == VANILLA_CHAT_TYPE_BATTLEGROUND|| object->chat_type == VANILLA_CHAT_TYPE_BATTLEGROUND_LEADER) {
        READ_U64(object->sender2);

    }
    READ_SIZED_CSTRING(object->message);

    object->tag = 0;
    READ_U8(object->tag);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_MESSAGECHAT_write(WowWorldWriter* writer, const vanilla_SMSG_MESSAGECHAT* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_MESSAGECHAT_size(object) + 2); /* size */

    WRITE_U16(0x00000096); /* opcode */

    WRITE_U8(object->chat_type);

    WRITE_U32(object->language);

    if (object->chat_type == VANILLA_CHAT_TYPE_MONSTER_WHISPER|| object->chat_type == VANILLA_CHAT_TYPE_RAID_BOSS_EMOTE|| object->chat_type == VANILLA_CHAT_TYPE_MONSTER_EMOTE) {
        WRITE_SIZED_CSTRING(object->monster_name);

        WRITE_U64(object->monster);

    }
    else if (object->chat_type == VANILLA_CHAT_TYPE_SAY|| object->chat_type == VANILLA_CHAT_TYPE_PARTY|| object->chat_type == VANILLA_CHAT_TYPE_YELL) {
        WRITE_U64(object->speech_bubble_credit);

        WRITE_U64(object->chat_credit);

    }
    else if (object->chat_type == VANILLA_CHAT_TYPE_MONSTER_SAY|| object->chat_type == VANILLA_CHAT_TYPE_MONSTER_YELL) {
        WRITE_U64(object->sender1);

        WRITE_SIZED_CSTRING(object->sender_name);

        WRITE_U64(object->target);

    }
    else if (object->chat_type == VANILLA_CHAT_TYPE_CHANNEL) {
        WRITE_CSTRING(object->channel_name);

        WRITE_U32(object->player_rank);

        WRITE_U64(object->player);

    }
    else if (object->chat_type == VANILLA_CHAT_TYPE_RAID|| object->chat_type == VANILLA_CHAT_TYPE_GUILD|| object->chat_type == VANILLA_CHAT_TYPE_OFFICER|| object->chat_type == VANILLA_CHAT_TYPE_WHISPER|| object->chat_type == VANILLA_CHAT_TYPE_WHISPER_INFORM|| object->chat_type == VANILLA_CHAT_TYPE_EMOTE|| object->chat_type == VANILLA_CHAT_TYPE_TEXT_EMOTE|| object->chat_type == VANILLA_CHAT_TYPE_SYSTEM|| object->chat_type == VANILLA_CHAT_TYPE_CHANNEL_JOIN|| object->chat_type == VANILLA_CHAT_TYPE_CHANNEL_LEAVE|| object->chat_type == VANILLA_CHAT_TYPE_CHANNEL_LIST|| object->chat_type == VANILLA_CHAT_TYPE_CHANNEL_NOTICE|| object->chat_type == VANILLA_CHAT_TYPE_CHANNEL_NOTICE_USER|| object->chat_type == VANILLA_CHAT_TYPE_AFK|| object->chat_type == VANILLA_CHAT_TYPE_DND|| object->chat_type == VANILLA_CHAT_TYPE_IGNORED|| object->chat_type == VANILLA_CHAT_TYPE_SKILL|| object->chat_type == VANILLA_CHAT_TYPE_LOOT|| object->chat_type == VANILLA_CHAT_TYPE_BG_SYSTEM_NEUTRAL|| object->chat_type == VANILLA_CHAT_TYPE_BG_SYSTEM_ALLIANCE|| object->chat_type == VANILLA_CHAT_TYPE_BG_SYSTEM_HORDE|| object->chat_type == VANILLA_CHAT_TYPE_RAID_LEADER|| object->chat_type == VANILLA_CHAT_TYPE_RAID_WARNING|| object->chat_type == VANILLA_CHAT_TYPE_RAID_BOSS_WHISPER|| object->chat_type == VANILLA_CHAT_TYPE_BATTLEGROUND|| object->chat_type == VANILLA_CHAT_TYPE_BATTLEGROUND_LEADER) {
        WRITE_U64(object->sender2);

    }
    WRITE_SIZED_CSTRING(object->message);

    WRITE_U8(object->tag);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_MESSAGECHAT_free(vanilla_SMSG_MESSAGECHAT* object) {
    if (object->chat_type == VANILLA_CHAT_TYPE_MONSTER_WHISPER|| object->chat_type == VANILLA_CHAT_TYPE_RAID_BOSS_EMOTE|| object->chat_type == VANILLA_CHAT_TYPE_MONSTER_EMOTE) {
        FREE_STRING(object->monster_name);

    }
    else if (object->chat_type == VANILLA_CHAT_TYPE_SAY|| object->chat_type == VANILLA_CHAT_TYPE_PARTY|| object->chat_type == VANILLA_CHAT_TYPE_YELL) {
    }
    else if (object->chat_type == VANILLA_CHAT_TYPE_MONSTER_SAY|| object->chat_type == VANILLA_CHAT_TYPE_MONSTER_YELL) {
        FREE_STRING(object->sender_name);

    }
    else if (object->chat_type == VANILLA_CHAT_TYPE_CHANNEL) {
        FREE_STRING(object->channel_name);

    }
    else if (object->chat_type == VANILLA_CHAT_TYPE_RAID|| object->chat_type == VANILLA_CHAT_TYPE_GUILD|| object->chat_type == VANILLA_CHAT_TYPE_OFFICER|| object->chat_type == VANILLA_CHAT_TYPE_WHISPER|| object->chat_type == VANILLA_CHAT_TYPE_WHISPER_INFORM|| object->chat_type == VANILLA_CHAT_TYPE_EMOTE|| object->chat_type == VANILLA_CHAT_TYPE_TEXT_EMOTE|| object->chat_type == VANILLA_CHAT_TYPE_SYSTEM|| object->chat_type == VANILLA_CHAT_TYPE_CHANNEL_JOIN|| object->chat_type == VANILLA_CHAT_TYPE_CHANNEL_LEAVE|| object->chat_type == VANILLA_CHAT_TYPE_CHANNEL_LIST|| object->chat_type == VANILLA_CHAT_TYPE_CHANNEL_NOTICE|| object->chat_type == VANILLA_CHAT_TYPE_CHANNEL_NOTICE_USER|| object->chat_type == VANILLA_CHAT_TYPE_AFK|| object->chat_type == VANILLA_CHAT_TYPE_DND|| object->chat_type == VANILLA_CHAT_TYPE_IGNORED|| object->chat_type == VANILLA_CHAT_TYPE_SKILL|| object->chat_type == VANILLA_CHAT_TYPE_LOOT|| object->chat_type == VANILLA_CHAT_TYPE_BG_SYSTEM_NEUTRAL|| object->chat_type == VANILLA_CHAT_TYPE_BG_SYSTEM_ALLIANCE|| object->chat_type == VANILLA_CHAT_TYPE_BG_SYSTEM_HORDE|| object->chat_type == VANILLA_CHAT_TYPE_RAID_LEADER|| object->chat_type == VANILLA_CHAT_TYPE_RAID_WARNING|| object->chat_type == VANILLA_CHAT_TYPE_RAID_BOSS_WHISPER|| object->chat_type == VANILLA_CHAT_TYPE_BATTLEGROUND|| object->chat_type == VANILLA_CHAT_TYPE_BATTLEGROUND_LEADER) {
    }
    FREE_STRING(object->message);

}

static size_t vanilla_CMSG_JOIN_CHANNEL_size(const vanilla_CMSG_JOIN_CHANNEL* object) {
    return 2 + STRING_SIZE(object->channel_name) + STRING_SIZE(object->channel_password);
}

static WowWorldResult vanilla_CMSG_JOIN_CHANNEL_read(WowWorldReader* reader, vanilla_CMSG_JOIN_CHANNEL* object) {
    READ_CSTRING(object->channel_name);

    READ_CSTRING(object->channel_password);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_JOIN_CHANNEL_write(WowWorldWriter* writer, const vanilla_CMSG_JOIN_CHANNEL* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_JOIN_CHANNEL_size(object) + 4); /* size */

    WRITE_U32(0x00000097); /* opcode */

    WRITE_CSTRING(object->channel_name);

    WRITE_CSTRING(object->channel_password);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_JOIN_CHANNEL_free(vanilla_CMSG_JOIN_CHANNEL* object) {
    FREE_STRING(object->channel_name);

    FREE_STRING(object->channel_password);

}

static size_t vanilla_CMSG_LEAVE_CHANNEL_size(const vanilla_CMSG_LEAVE_CHANNEL* object) {
    return 1 + STRING_SIZE(object->channel_name);
}

static WowWorldResult vanilla_CMSG_LEAVE_CHANNEL_read(WowWorldReader* reader, vanilla_CMSG_LEAVE_CHANNEL* object) {
    READ_CSTRING(object->channel_name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_LEAVE_CHANNEL_write(WowWorldWriter* writer, const vanilla_CMSG_LEAVE_CHANNEL* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_LEAVE_CHANNEL_size(object) + 4); /* size */

    WRITE_U32(0x00000098); /* opcode */

    WRITE_CSTRING(object->channel_name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_LEAVE_CHANNEL_free(vanilla_CMSG_LEAVE_CHANNEL* object) {
    FREE_STRING(object->channel_name);

}

static size_t vanilla_SMSG_CHANNEL_NOTIFY_size(const vanilla_SMSG_CHANNEL_NOTIFY* object) {
    return 2 + STRING_SIZE(object->channel_name);
}

static WowWorldResult vanilla_SMSG_CHANNEL_NOTIFY_read(WowWorldReader* reader, vanilla_SMSG_CHANNEL_NOTIFY* object) {
    object->notify_type = 0;
    READ_U8(object->notify_type);

    READ_CSTRING(object->channel_name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_CHANNEL_NOTIFY_write(WowWorldWriter* writer, const vanilla_SMSG_CHANNEL_NOTIFY* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_CHANNEL_NOTIFY_size(object) + 2); /* size */

    WRITE_U16(0x00000099); /* opcode */

    WRITE_U8(object->notify_type);

    WRITE_CSTRING(object->channel_name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_CHANNEL_NOTIFY_free(vanilla_SMSG_CHANNEL_NOTIFY* object) {
    FREE_STRING(object->channel_name);

}

static size_t vanilla_CMSG_CHANNEL_LIST_size(const vanilla_CMSG_CHANNEL_LIST* object) {
    return 1 + STRING_SIZE(object->channel_name);
}

static WowWorldResult vanilla_CMSG_CHANNEL_LIST_read(WowWorldReader* reader, vanilla_CMSG_CHANNEL_LIST* object) {
    READ_CSTRING(object->channel_name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_CHANNEL_LIST_write(WowWorldWriter* writer, const vanilla_CMSG_CHANNEL_LIST* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_CHANNEL_LIST_size(object) + 4); /* size */

    WRITE_U32(0x0000009a); /* opcode */

    WRITE_CSTRING(object->channel_name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_CHANNEL_LIST_free(vanilla_CMSG_CHANNEL_LIST* object) {
    FREE_STRING(object->channel_name);

}

static size_t vanilla_SMSG_CHANNEL_LIST_size(const vanilla_SMSG_CHANNEL_LIST* object) {
    return 6 + STRING_SIZE(object->channel_name) + 9 * object->amount_of_members;
}

static WowWorldResult vanilla_SMSG_CHANNEL_LIST_read(WowWorldReader* reader, vanilla_SMSG_CHANNEL_LIST* object) {
    READ_CSTRING(object->channel_name);

    object->channel_flags = 0;
    READ_U8(object->channel_flags);

    READ_U32(object->amount_of_members);

    READ_ARRAY_ALLOCATE(object->members, object->amount_of_members, sizeof(vanilla_ChannelMember));
    READ_ARRAY(object->members, object->amount_of_members, WWM_CHECK_RETURN_CODE(vanilla_ChannelMember_read(reader, &object->members[i])));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_CHANNEL_LIST_write(WowWorldWriter* writer, const vanilla_SMSG_CHANNEL_LIST* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_CHANNEL_LIST_size(object) + 2); /* size */

    WRITE_U16(0x0000009b); /* opcode */

    WRITE_CSTRING(object->channel_name);

    WRITE_U8(object->channel_flags);

    WRITE_U32(object->amount_of_members);

    WRITE_ARRAY(object->members, object->amount_of_members, WWM_CHECK_RETURN_CODE(vanilla_ChannelMember_write(writer, &object->members[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_CHANNEL_LIST_free(vanilla_SMSG_CHANNEL_LIST* object) {
    FREE_STRING(object->channel_name);

    free(object->members);
    object->members = NULL;
}

static size_t vanilla_CMSG_CHANNEL_PASSWORD_size(const vanilla_CMSG_CHANNEL_PASSWORD* object) {
    return 2 + STRING_SIZE(object->channel_name) + STRING_SIZE(object->channel_password);
}

static WowWorldResult vanilla_CMSG_CHANNEL_PASSWORD_read(WowWorldReader* reader, vanilla_CMSG_CHANNEL_PASSWORD* object) {
    READ_CSTRING(object->channel_name);

    READ_CSTRING(object->channel_password);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_CHANNEL_PASSWORD_write(WowWorldWriter* writer, const vanilla_CMSG_CHANNEL_PASSWORD* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_CHANNEL_PASSWORD_size(object) + 4); /* size */

    WRITE_U32(0x0000009c); /* opcode */

    WRITE_CSTRING(object->channel_name);

    WRITE_CSTRING(object->channel_password);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_CHANNEL_PASSWORD_free(vanilla_CMSG_CHANNEL_PASSWORD* object) {
    FREE_STRING(object->channel_name);

    FREE_STRING(object->channel_password);

}

static size_t vanilla_CMSG_CHANNEL_SET_OWNER_size(const vanilla_CMSG_CHANNEL_SET_OWNER* object) {
    return 2 + STRING_SIZE(object->channel_name) + STRING_SIZE(object->new_owner);
}

static WowWorldResult vanilla_CMSG_CHANNEL_SET_OWNER_read(WowWorldReader* reader, vanilla_CMSG_CHANNEL_SET_OWNER* object) {
    READ_CSTRING(object->channel_name);

    READ_CSTRING(object->new_owner);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_CHANNEL_SET_OWNER_write(WowWorldWriter* writer, const vanilla_CMSG_CHANNEL_SET_OWNER* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_CHANNEL_SET_OWNER_size(object) + 4); /* size */

    WRITE_U32(0x0000009d); /* opcode */

    WRITE_CSTRING(object->channel_name);

    WRITE_CSTRING(object->new_owner);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_CHANNEL_SET_OWNER_free(vanilla_CMSG_CHANNEL_SET_OWNER* object) {
    FREE_STRING(object->channel_name);

    FREE_STRING(object->new_owner);

}

static size_t vanilla_CMSG_CHANNEL_OWNER_size(const vanilla_CMSG_CHANNEL_OWNER* object) {
    return 1 + STRING_SIZE(object->channel_name);
}

static WowWorldResult vanilla_CMSG_CHANNEL_OWNER_read(WowWorldReader* reader, vanilla_CMSG_CHANNEL_OWNER* object) {
    READ_CSTRING(object->channel_name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_CHANNEL_OWNER_write(WowWorldWriter* writer, const vanilla_CMSG_CHANNEL_OWNER* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_CHANNEL_OWNER_size(object) + 4); /* size */

    WRITE_U32(0x0000009e); /* opcode */

    WRITE_CSTRING(object->channel_name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_CHANNEL_OWNER_free(vanilla_CMSG_CHANNEL_OWNER* object) {
    FREE_STRING(object->channel_name);

}

static size_t vanilla_CMSG_CHANNEL_MODERATOR_size(const vanilla_CMSG_CHANNEL_MODERATOR* object) {
    return 2 + STRING_SIZE(object->channel_name) + STRING_SIZE(object->player_name);
}

static WowWorldResult vanilla_CMSG_CHANNEL_MODERATOR_read(WowWorldReader* reader, vanilla_CMSG_CHANNEL_MODERATOR* object) {
    READ_CSTRING(object->channel_name);

    READ_CSTRING(object->player_name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_CHANNEL_MODERATOR_write(WowWorldWriter* writer, const vanilla_CMSG_CHANNEL_MODERATOR* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_CHANNEL_MODERATOR_size(object) + 4); /* size */

    WRITE_U32(0x0000009f); /* opcode */

    WRITE_CSTRING(object->channel_name);

    WRITE_CSTRING(object->player_name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_CHANNEL_MODERATOR_free(vanilla_CMSG_CHANNEL_MODERATOR* object) {
    FREE_STRING(object->channel_name);

    FREE_STRING(object->player_name);

}

static size_t vanilla_CMSG_CHANNEL_UNMODERATOR_size(const vanilla_CMSG_CHANNEL_UNMODERATOR* object) {
    return 2 + STRING_SIZE(object->channel_name) + STRING_SIZE(object->player_name);
}

static WowWorldResult vanilla_CMSG_CHANNEL_UNMODERATOR_read(WowWorldReader* reader, vanilla_CMSG_CHANNEL_UNMODERATOR* object) {
    READ_CSTRING(object->channel_name);

    READ_CSTRING(object->player_name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_CHANNEL_UNMODERATOR_write(WowWorldWriter* writer, const vanilla_CMSG_CHANNEL_UNMODERATOR* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_CHANNEL_UNMODERATOR_size(object) + 4); /* size */

    WRITE_U32(0x000000a0); /* opcode */

    WRITE_CSTRING(object->channel_name);

    WRITE_CSTRING(object->player_name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_CHANNEL_UNMODERATOR_free(vanilla_CMSG_CHANNEL_UNMODERATOR* object) {
    FREE_STRING(object->channel_name);

    FREE_STRING(object->player_name);

}

static size_t vanilla_CMSG_CHANNEL_MUTE_size(const vanilla_CMSG_CHANNEL_MUTE* object) {
    return 2 + STRING_SIZE(object->channel_name) + STRING_SIZE(object->player_name);
}

static WowWorldResult vanilla_CMSG_CHANNEL_MUTE_read(WowWorldReader* reader, vanilla_CMSG_CHANNEL_MUTE* object) {
    READ_CSTRING(object->channel_name);

    READ_CSTRING(object->player_name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_CHANNEL_MUTE_write(WowWorldWriter* writer, const vanilla_CMSG_CHANNEL_MUTE* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_CHANNEL_MUTE_size(object) + 4); /* size */

    WRITE_U32(0x000000a1); /* opcode */

    WRITE_CSTRING(object->channel_name);

    WRITE_CSTRING(object->player_name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_CHANNEL_MUTE_free(vanilla_CMSG_CHANNEL_MUTE* object) {
    FREE_STRING(object->channel_name);

    FREE_STRING(object->player_name);

}

static size_t vanilla_CMSG_CHANNEL_UNMUTE_size(const vanilla_CMSG_CHANNEL_UNMUTE* object) {
    return 2 + STRING_SIZE(object->channel_name) + STRING_SIZE(object->player_name);
}

static WowWorldResult vanilla_CMSG_CHANNEL_UNMUTE_read(WowWorldReader* reader, vanilla_CMSG_CHANNEL_UNMUTE* object) {
    READ_CSTRING(object->channel_name);

    READ_CSTRING(object->player_name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_CHANNEL_UNMUTE_write(WowWorldWriter* writer, const vanilla_CMSG_CHANNEL_UNMUTE* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_CHANNEL_UNMUTE_size(object) + 4); /* size */

    WRITE_U32(0x000000a2); /* opcode */

    WRITE_CSTRING(object->channel_name);

    WRITE_CSTRING(object->player_name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_CHANNEL_UNMUTE_free(vanilla_CMSG_CHANNEL_UNMUTE* object) {
    FREE_STRING(object->channel_name);

    FREE_STRING(object->player_name);

}

static size_t vanilla_CMSG_CHANNEL_INVITE_size(const vanilla_CMSG_CHANNEL_INVITE* object) {
    return 2 + STRING_SIZE(object->channel_name) + STRING_SIZE(object->player_name);
}

static WowWorldResult vanilla_CMSG_CHANNEL_INVITE_read(WowWorldReader* reader, vanilla_CMSG_CHANNEL_INVITE* object) {
    READ_CSTRING(object->channel_name);

    READ_CSTRING(object->player_name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_CHANNEL_INVITE_write(WowWorldWriter* writer, const vanilla_CMSG_CHANNEL_INVITE* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_CHANNEL_INVITE_size(object) + 4); /* size */

    WRITE_U32(0x000000a3); /* opcode */

    WRITE_CSTRING(object->channel_name);

    WRITE_CSTRING(object->player_name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_CHANNEL_INVITE_free(vanilla_CMSG_CHANNEL_INVITE* object) {
    FREE_STRING(object->channel_name);

    FREE_STRING(object->player_name);

}

static size_t vanilla_CMSG_CHANNEL_KICK_size(const vanilla_CMSG_CHANNEL_KICK* object) {
    return 2 + STRING_SIZE(object->channel_name) + STRING_SIZE(object->player_name);
}

static WowWorldResult vanilla_CMSG_CHANNEL_KICK_read(WowWorldReader* reader, vanilla_CMSG_CHANNEL_KICK* object) {
    READ_CSTRING(object->channel_name);

    READ_CSTRING(object->player_name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_CHANNEL_KICK_write(WowWorldWriter* writer, const vanilla_CMSG_CHANNEL_KICK* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_CHANNEL_KICK_size(object) + 4); /* size */

    WRITE_U32(0x000000a4); /* opcode */

    WRITE_CSTRING(object->channel_name);

    WRITE_CSTRING(object->player_name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_CHANNEL_KICK_free(vanilla_CMSG_CHANNEL_KICK* object) {
    FREE_STRING(object->channel_name);

    FREE_STRING(object->player_name);

}

static size_t vanilla_CMSG_CHANNEL_BAN_size(const vanilla_CMSG_CHANNEL_BAN* object) {
    return 2 + STRING_SIZE(object->channel_name) + STRING_SIZE(object->player_name);
}

static WowWorldResult vanilla_CMSG_CHANNEL_BAN_read(WowWorldReader* reader, vanilla_CMSG_CHANNEL_BAN* object) {
    READ_CSTRING(object->channel_name);

    READ_CSTRING(object->player_name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_CHANNEL_BAN_write(WowWorldWriter* writer, const vanilla_CMSG_CHANNEL_BAN* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_CHANNEL_BAN_size(object) + 4); /* size */

    WRITE_U32(0x000000a5); /* opcode */

    WRITE_CSTRING(object->channel_name);

    WRITE_CSTRING(object->player_name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_CHANNEL_BAN_free(vanilla_CMSG_CHANNEL_BAN* object) {
    FREE_STRING(object->channel_name);

    FREE_STRING(object->player_name);

}

static size_t vanilla_CMSG_CHANNEL_UNBAN_size(const vanilla_CMSG_CHANNEL_UNBAN* object) {
    return 2 + STRING_SIZE(object->channel_name) + STRING_SIZE(object->player_name);
}

static WowWorldResult vanilla_CMSG_CHANNEL_UNBAN_read(WowWorldReader* reader, vanilla_CMSG_CHANNEL_UNBAN* object) {
    READ_CSTRING(object->channel_name);

    READ_CSTRING(object->player_name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_CHANNEL_UNBAN_write(WowWorldWriter* writer, const vanilla_CMSG_CHANNEL_UNBAN* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_CHANNEL_UNBAN_size(object) + 4); /* size */

    WRITE_U32(0x000000a6); /* opcode */

    WRITE_CSTRING(object->channel_name);

    WRITE_CSTRING(object->player_name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_CHANNEL_UNBAN_free(vanilla_CMSG_CHANNEL_UNBAN* object) {
    FREE_STRING(object->channel_name);

    FREE_STRING(object->player_name);

}

static size_t vanilla_CMSG_CHANNEL_ANNOUNCEMENTS_size(const vanilla_CMSG_CHANNEL_ANNOUNCEMENTS* object) {
    return 1 + STRING_SIZE(object->channel_name);
}

static WowWorldResult vanilla_CMSG_CHANNEL_ANNOUNCEMENTS_read(WowWorldReader* reader, vanilla_CMSG_CHANNEL_ANNOUNCEMENTS* object) {
    READ_CSTRING(object->channel_name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_CHANNEL_ANNOUNCEMENTS_write(WowWorldWriter* writer, const vanilla_CMSG_CHANNEL_ANNOUNCEMENTS* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_CHANNEL_ANNOUNCEMENTS_size(object) + 4); /* size */

    WRITE_U32(0x000000a7); /* opcode */

    WRITE_CSTRING(object->channel_name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_CHANNEL_ANNOUNCEMENTS_free(vanilla_CMSG_CHANNEL_ANNOUNCEMENTS* object) {
    FREE_STRING(object->channel_name);

}

static size_t vanilla_CMSG_CHANNEL_MODERATE_size(const vanilla_CMSG_CHANNEL_MODERATE* object) {
    return 1 + STRING_SIZE(object->channel_name);
}

static WowWorldResult vanilla_CMSG_CHANNEL_MODERATE_read(WowWorldReader* reader, vanilla_CMSG_CHANNEL_MODERATE* object) {
    READ_CSTRING(object->channel_name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_CHANNEL_MODERATE_write(WowWorldWriter* writer, const vanilla_CMSG_CHANNEL_MODERATE* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_CHANNEL_MODERATE_size(object) + 4); /* size */

    WRITE_U32(0x000000a8); /* opcode */

    WRITE_CSTRING(object->channel_name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_CHANNEL_MODERATE_free(vanilla_CMSG_CHANNEL_MODERATE* object) {
    FREE_STRING(object->channel_name);

}

static size_t vanilla_SMSG_UPDATE_OBJECT_size(const vanilla_SMSG_UPDATE_OBJECT* object) {
    size_t _size = 5;

    /* C89 scope to allow variable declarations */ {
        int i;
        for(i = 0; i < (int)object->amount_of_objects; ++i) {
            _size += vanilla_Object_size(&object->objects[i]);
        }
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_UPDATE_OBJECT_read(WowWorldReader* reader, vanilla_SMSG_UPDATE_OBJECT* object) {
    READ_U32(object->amount_of_objects);

    READ_U8(object->has_transport);

    READ_ARRAY_ALLOCATE(object->objects, object->amount_of_objects, sizeof(vanilla_Object));
    READ_ARRAY(object->objects, object->amount_of_objects, WWM_CHECK_RETURN_CODE(vanilla_Object_read(reader, &object->objects[i])));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_UPDATE_OBJECT_write(WowWorldWriter* writer, const vanilla_SMSG_UPDATE_OBJECT* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_UPDATE_OBJECT_size(object) + 2); /* size */

    WRITE_U16(0x000000a9); /* opcode */

    WRITE_U32(object->amount_of_objects);

    WRITE_U8(object->has_transport);

    WRITE_ARRAY(object->objects, object->amount_of_objects, WWM_CHECK_RETURN_CODE(vanilla_Object_write(writer, &object->objects[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_UPDATE_OBJECT_free(vanilla_SMSG_UPDATE_OBJECT* object) {
    size_t i;

    for (i = 0; i < object->amount_of_objects; ++i) {
        vanilla_Object_free(&((object->objects)[i]));
    }
    free(object->objects);
    object->objects = NULL;
}

static WowWorldResult vanilla_SMSG_DESTROY_OBJECT_read(WowWorldReader* reader, vanilla_SMSG_DESTROY_OBJECT* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_DESTROY_OBJECT_write(WowWorldWriter* writer, const vanilla_SMSG_DESTROY_OBJECT* object) {
    WRITE_U16_BE(0x0008 + 2); /* size */

    WRITE_U16(0x000000aa); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_USE_ITEM_size(const vanilla_CMSG_USE_ITEM* object) {
    return 3 + vanilla_SpellCastTargets_size(&object->targets);
}

static WowWorldResult vanilla_CMSG_USE_ITEM_read(WowWorldReader* reader, vanilla_CMSG_USE_ITEM* object) {
    READ_U8(object->bag_index);

    READ_U8(object->bag_slot);

    READ_U8(object->spell_index);

    WWM_CHECK_RETURN_CODE(vanilla_SpellCastTargets_read(reader, &object->targets));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_USE_ITEM_write(WowWorldWriter* writer, const vanilla_CMSG_USE_ITEM* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_USE_ITEM_size(object) + 4); /* size */

    WRITE_U32(0x000000ab); /* opcode */

    WRITE_U8(object->bag_index);

    WRITE_U8(object->bag_slot);

    WRITE_U8(object->spell_index);

    WWM_CHECK_RETURN_CODE(vanilla_SpellCastTargets_write(writer, &object->targets));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_USE_ITEM_free(vanilla_CMSG_USE_ITEM* object) {
    vanilla_SpellCastTargets_free(&object->targets);
}

static WowWorldResult vanilla_CMSG_OPEN_ITEM_read(WowWorldReader* reader, vanilla_CMSG_OPEN_ITEM* object) {
    READ_U8(object->bag_index);

    READ_U8(object->slot);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_OPEN_ITEM_write(WowWorldWriter* writer, const vanilla_CMSG_OPEN_ITEM* object) {
    WRITE_U16_BE(0x0002 + 4); /* size */

    WRITE_U32(0x000000ac); /* opcode */

    WRITE_U8(object->bag_index);

    WRITE_U8(object->slot);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_READ_ITEM_read(WowWorldReader* reader, vanilla_CMSG_READ_ITEM* object) {
    READ_U8(object->bag_index);

    READ_U8(object->slot);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_READ_ITEM_write(WowWorldWriter* writer, const vanilla_CMSG_READ_ITEM* object) {
    WRITE_U16_BE(0x0002 + 4); /* size */

    WRITE_U32(0x000000ad); /* opcode */

    WRITE_U8(object->bag_index);

    WRITE_U8(object->slot);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_READ_ITEM_OK_read(WowWorldReader* reader, vanilla_SMSG_READ_ITEM_OK* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_READ_ITEM_OK_write(WowWorldWriter* writer, const vanilla_SMSG_READ_ITEM_OK* object) {
    WRITE_U16_BE(0x0008 + 2); /* size */

    WRITE_U16(0x000000ae); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_READ_ITEM_FAILED_read(WowWorldReader* reader, vanilla_SMSG_READ_ITEM_FAILED* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_READ_ITEM_FAILED_write(WowWorldWriter* writer, const vanilla_SMSG_READ_ITEM_FAILED* object) {
    WRITE_U16_BE(0x0008 + 2); /* size */

    WRITE_U16(0x000000af); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_ITEM_COOLDOWN_read(WowWorldReader* reader, vanilla_SMSG_ITEM_COOLDOWN* object) {
    READ_U64(object->guid);

    READ_U32(object->id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_ITEM_COOLDOWN_write(WowWorldWriter* writer, const vanilla_SMSG_ITEM_COOLDOWN* object) {
    WRITE_U16_BE(0x000c + 2); /* size */

    WRITE_U16(0x000000b0); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->id);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_GAMEOBJ_USE_read(WowWorldReader* reader, vanilla_CMSG_GAMEOBJ_USE* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GAMEOBJ_USE_write(WowWorldWriter* writer, const vanilla_CMSG_GAMEOBJ_USE* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x000000b1); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_GAMEOBJECT_CUSTOM_ANIM_read(WowWorldReader* reader, vanilla_SMSG_GAMEOBJECT_CUSTOM_ANIM* object) {
    READ_U64(object->guid);

    READ_U32(object->animation_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_GAMEOBJECT_CUSTOM_ANIM_write(WowWorldWriter* writer, const vanilla_SMSG_GAMEOBJECT_CUSTOM_ANIM* object) {
    WRITE_U16_BE(0x000c + 2); /* size */

    WRITE_U16(0x000000b3); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->animation_id);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_AREATRIGGER_read(WowWorldReader* reader, vanilla_CMSG_AREATRIGGER* object) {
    READ_U32(object->trigger_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_AREATRIGGER_write(WowWorldWriter* writer, const vanilla_CMSG_AREATRIGGER* object) {
    WRITE_U16_BE(0x0004 + 4); /* size */

    WRITE_U32(0x000000b4); /* opcode */

    WRITE_U32(object->trigger_id);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_MSG_MOVE_START_FORWARD_Client_size(const vanilla_MSG_MOVE_START_FORWARD_Client* object) {
    return 0 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_START_FORWARD_Client_read(WowWorldReader* reader, vanilla_MSG_MOVE_START_FORWARD_Client* object) {
    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_START_FORWARD_Client_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_START_FORWARD_Client* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_START_FORWARD_Client_size(object) + 4); /* size */

    WRITE_U32(0x000000b5); /* opcode */

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_START_FORWARD_Client_free(vanilla_MSG_MOVE_START_FORWARD_Client* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_START_FORWARD_Server_size(const vanilla_MSG_MOVE_START_FORWARD_Server* object) {
    return 0 + wwm_packed_guid_size(object->guid) + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_START_FORWARD_Server_read(WowWorldReader* reader, vanilla_MSG_MOVE_START_FORWARD_Server* object) {
    READ_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_START_FORWARD_Server_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_START_FORWARD_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_START_FORWARD_Server_size(object) + 2); /* size */

    WRITE_U16(0x000000b5); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_START_FORWARD_Server_free(vanilla_MSG_MOVE_START_FORWARD_Server* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_START_BACKWARD_Client_size(const vanilla_MSG_MOVE_START_BACKWARD_Client* object) {
    return 0 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_START_BACKWARD_Client_read(WowWorldReader* reader, vanilla_MSG_MOVE_START_BACKWARD_Client* object) {
    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_START_BACKWARD_Client_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_START_BACKWARD_Client* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_START_BACKWARD_Client_size(object) + 4); /* size */

    WRITE_U32(0x000000b6); /* opcode */

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_START_BACKWARD_Client_free(vanilla_MSG_MOVE_START_BACKWARD_Client* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_START_BACKWARD_Server_size(const vanilla_MSG_MOVE_START_BACKWARD_Server* object) {
    return 0 + wwm_packed_guid_size(object->guid) + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_START_BACKWARD_Server_read(WowWorldReader* reader, vanilla_MSG_MOVE_START_BACKWARD_Server* object) {
    READ_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_START_BACKWARD_Server_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_START_BACKWARD_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_START_BACKWARD_Server_size(object) + 2); /* size */

    WRITE_U16(0x000000b6); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_START_BACKWARD_Server_free(vanilla_MSG_MOVE_START_BACKWARD_Server* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_STOP_Client_size(const vanilla_MSG_MOVE_STOP_Client* object) {
    return 0 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_STOP_Client_read(WowWorldReader* reader, vanilla_MSG_MOVE_STOP_Client* object) {
    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_STOP_Client_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_STOP_Client* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_STOP_Client_size(object) + 4); /* size */

    WRITE_U32(0x000000b7); /* opcode */

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_STOP_Client_free(vanilla_MSG_MOVE_STOP_Client* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_STOP_Server_size(const vanilla_MSG_MOVE_STOP_Server* object) {
    return 0 + wwm_packed_guid_size(object->guid) + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_STOP_Server_read(WowWorldReader* reader, vanilla_MSG_MOVE_STOP_Server* object) {
    READ_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_STOP_Server_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_STOP_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_STOP_Server_size(object) + 2); /* size */

    WRITE_U16(0x000000b7); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_STOP_Server_free(vanilla_MSG_MOVE_STOP_Server* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_START_STRAFE_LEFT_Client_size(const vanilla_MSG_MOVE_START_STRAFE_LEFT_Client* object) {
    return 0 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_START_STRAFE_LEFT_Client_read(WowWorldReader* reader, vanilla_MSG_MOVE_START_STRAFE_LEFT_Client* object) {
    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_START_STRAFE_LEFT_Client_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_START_STRAFE_LEFT_Client* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_START_STRAFE_LEFT_Client_size(object) + 4); /* size */

    WRITE_U32(0x000000b8); /* opcode */

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_START_STRAFE_LEFT_Client_free(vanilla_MSG_MOVE_START_STRAFE_LEFT_Client* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_START_STRAFE_LEFT_Server_size(const vanilla_MSG_MOVE_START_STRAFE_LEFT_Server* object) {
    return 0 + wwm_packed_guid_size(object->guid) + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_START_STRAFE_LEFT_Server_read(WowWorldReader* reader, vanilla_MSG_MOVE_START_STRAFE_LEFT_Server* object) {
    READ_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_START_STRAFE_LEFT_Server_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_START_STRAFE_LEFT_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_START_STRAFE_LEFT_Server_size(object) + 2); /* size */

    WRITE_U16(0x000000b8); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_START_STRAFE_LEFT_Server_free(vanilla_MSG_MOVE_START_STRAFE_LEFT_Server* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_START_STRAFE_RIGHT_Client_size(const vanilla_MSG_MOVE_START_STRAFE_RIGHT_Client* object) {
    return 0 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_START_STRAFE_RIGHT_Client_read(WowWorldReader* reader, vanilla_MSG_MOVE_START_STRAFE_RIGHT_Client* object) {
    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_START_STRAFE_RIGHT_Client_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_START_STRAFE_RIGHT_Client* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_START_STRAFE_RIGHT_Client_size(object) + 4); /* size */

    WRITE_U32(0x000000b9); /* opcode */

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_START_STRAFE_RIGHT_Client_free(vanilla_MSG_MOVE_START_STRAFE_RIGHT_Client* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_START_STRAFE_RIGHT_Server_size(const vanilla_MSG_MOVE_START_STRAFE_RIGHT_Server* object) {
    return 0 + wwm_packed_guid_size(object->guid) + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_START_STRAFE_RIGHT_Server_read(WowWorldReader* reader, vanilla_MSG_MOVE_START_STRAFE_RIGHT_Server* object) {
    READ_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_START_STRAFE_RIGHT_Server_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_START_STRAFE_RIGHT_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_START_STRAFE_RIGHT_Server_size(object) + 2); /* size */

    WRITE_U16(0x000000b9); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_START_STRAFE_RIGHT_Server_free(vanilla_MSG_MOVE_START_STRAFE_RIGHT_Server* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_STOP_STRAFE_Client_size(const vanilla_MSG_MOVE_STOP_STRAFE_Client* object) {
    return 0 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_STOP_STRAFE_Client_read(WowWorldReader* reader, vanilla_MSG_MOVE_STOP_STRAFE_Client* object) {
    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_STOP_STRAFE_Client_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_STOP_STRAFE_Client* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_STOP_STRAFE_Client_size(object) + 4); /* size */

    WRITE_U32(0x000000ba); /* opcode */

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_STOP_STRAFE_Client_free(vanilla_MSG_MOVE_STOP_STRAFE_Client* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_STOP_STRAFE_Server_size(const vanilla_MSG_MOVE_STOP_STRAFE_Server* object) {
    return 0 + wwm_packed_guid_size(object->guid) + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_STOP_STRAFE_Server_read(WowWorldReader* reader, vanilla_MSG_MOVE_STOP_STRAFE_Server* object) {
    READ_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_STOP_STRAFE_Server_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_STOP_STRAFE_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_STOP_STRAFE_Server_size(object) + 2); /* size */

    WRITE_U16(0x000000ba); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_STOP_STRAFE_Server_free(vanilla_MSG_MOVE_STOP_STRAFE_Server* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_JUMP_Client_size(const vanilla_MSG_MOVE_JUMP_Client* object) {
    return 0 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_JUMP_Client_read(WowWorldReader* reader, vanilla_MSG_MOVE_JUMP_Client* object) {
    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_JUMP_Client_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_JUMP_Client* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_JUMP_Client_size(object) + 4); /* size */

    WRITE_U32(0x000000bb); /* opcode */

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_JUMP_Client_free(vanilla_MSG_MOVE_JUMP_Client* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_JUMP_Server_size(const vanilla_MSG_MOVE_JUMP_Server* object) {
    return 0 + wwm_packed_guid_size(object->guid) + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_JUMP_Server_read(WowWorldReader* reader, vanilla_MSG_MOVE_JUMP_Server* object) {
    READ_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_JUMP_Server_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_JUMP_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_JUMP_Server_size(object) + 2); /* size */

    WRITE_U16(0x000000bb); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_JUMP_Server_free(vanilla_MSG_MOVE_JUMP_Server* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_START_TURN_LEFT_Client_size(const vanilla_MSG_MOVE_START_TURN_LEFT_Client* object) {
    return 0 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_START_TURN_LEFT_Client_read(WowWorldReader* reader, vanilla_MSG_MOVE_START_TURN_LEFT_Client* object) {
    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_START_TURN_LEFT_Client_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_START_TURN_LEFT_Client* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_START_TURN_LEFT_Client_size(object) + 4); /* size */

    WRITE_U32(0x000000bc); /* opcode */

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_START_TURN_LEFT_Client_free(vanilla_MSG_MOVE_START_TURN_LEFT_Client* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_START_TURN_LEFT_Server_size(const vanilla_MSG_MOVE_START_TURN_LEFT_Server* object) {
    return 0 + wwm_packed_guid_size(object->guid) + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_START_TURN_LEFT_Server_read(WowWorldReader* reader, vanilla_MSG_MOVE_START_TURN_LEFT_Server* object) {
    READ_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_START_TURN_LEFT_Server_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_START_TURN_LEFT_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_START_TURN_LEFT_Server_size(object) + 2); /* size */

    WRITE_U16(0x000000bc); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_START_TURN_LEFT_Server_free(vanilla_MSG_MOVE_START_TURN_LEFT_Server* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_START_TURN_RIGHT_Client_size(const vanilla_MSG_MOVE_START_TURN_RIGHT_Client* object) {
    return 0 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_START_TURN_RIGHT_Client_read(WowWorldReader* reader, vanilla_MSG_MOVE_START_TURN_RIGHT_Client* object) {
    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_START_TURN_RIGHT_Client_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_START_TURN_RIGHT_Client* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_START_TURN_RIGHT_Client_size(object) + 4); /* size */

    WRITE_U32(0x000000bd); /* opcode */

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_START_TURN_RIGHT_Client_free(vanilla_MSG_MOVE_START_TURN_RIGHT_Client* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_START_TURN_RIGHT_Server_size(const vanilla_MSG_MOVE_START_TURN_RIGHT_Server* object) {
    return 0 + wwm_packed_guid_size(object->guid) + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_START_TURN_RIGHT_Server_read(WowWorldReader* reader, vanilla_MSG_MOVE_START_TURN_RIGHT_Server* object) {
    READ_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_START_TURN_RIGHT_Server_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_START_TURN_RIGHT_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_START_TURN_RIGHT_Server_size(object) + 2); /* size */

    WRITE_U16(0x000000bd); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_START_TURN_RIGHT_Server_free(vanilla_MSG_MOVE_START_TURN_RIGHT_Server* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_STOP_TURN_Client_size(const vanilla_MSG_MOVE_STOP_TURN_Client* object) {
    return 0 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_STOP_TURN_Client_read(WowWorldReader* reader, vanilla_MSG_MOVE_STOP_TURN_Client* object) {
    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_STOP_TURN_Client_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_STOP_TURN_Client* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_STOP_TURN_Client_size(object) + 4); /* size */

    WRITE_U32(0x000000be); /* opcode */

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_STOP_TURN_Client_free(vanilla_MSG_MOVE_STOP_TURN_Client* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_STOP_TURN_Server_size(const vanilla_MSG_MOVE_STOP_TURN_Server* object) {
    return 0 + wwm_packed_guid_size(object->guid) + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_STOP_TURN_Server_read(WowWorldReader* reader, vanilla_MSG_MOVE_STOP_TURN_Server* object) {
    READ_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_STOP_TURN_Server_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_STOP_TURN_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_STOP_TURN_Server_size(object) + 2); /* size */

    WRITE_U16(0x000000be); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_STOP_TURN_Server_free(vanilla_MSG_MOVE_STOP_TURN_Server* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_START_PITCH_UP_Client_size(const vanilla_MSG_MOVE_START_PITCH_UP_Client* object) {
    return 0 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_START_PITCH_UP_Client_read(WowWorldReader* reader, vanilla_MSG_MOVE_START_PITCH_UP_Client* object) {
    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_START_PITCH_UP_Client_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_START_PITCH_UP_Client* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_START_PITCH_UP_Client_size(object) + 4); /* size */

    WRITE_U32(0x000000bf); /* opcode */

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_START_PITCH_UP_Client_free(vanilla_MSG_MOVE_START_PITCH_UP_Client* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_START_PITCH_UP_Server_size(const vanilla_MSG_MOVE_START_PITCH_UP_Server* object) {
    return 0 + wwm_packed_guid_size(object->guid) + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_START_PITCH_UP_Server_read(WowWorldReader* reader, vanilla_MSG_MOVE_START_PITCH_UP_Server* object) {
    READ_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_START_PITCH_UP_Server_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_START_PITCH_UP_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_START_PITCH_UP_Server_size(object) + 2); /* size */

    WRITE_U16(0x000000bf); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_START_PITCH_UP_Server_free(vanilla_MSG_MOVE_START_PITCH_UP_Server* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_START_PITCH_DOWN_Client_size(const vanilla_MSG_MOVE_START_PITCH_DOWN_Client* object) {
    return 0 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_START_PITCH_DOWN_Client_read(WowWorldReader* reader, vanilla_MSG_MOVE_START_PITCH_DOWN_Client* object) {
    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_START_PITCH_DOWN_Client_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_START_PITCH_DOWN_Client* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_START_PITCH_DOWN_Client_size(object) + 4); /* size */

    WRITE_U32(0x000000c0); /* opcode */

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_START_PITCH_DOWN_Client_free(vanilla_MSG_MOVE_START_PITCH_DOWN_Client* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_START_PITCH_DOWN_Server_size(const vanilla_MSG_MOVE_START_PITCH_DOWN_Server* object) {
    return 0 + wwm_packed_guid_size(object->guid) + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_START_PITCH_DOWN_Server_read(WowWorldReader* reader, vanilla_MSG_MOVE_START_PITCH_DOWN_Server* object) {
    READ_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_START_PITCH_DOWN_Server_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_START_PITCH_DOWN_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_START_PITCH_DOWN_Server_size(object) + 2); /* size */

    WRITE_U16(0x000000c0); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_START_PITCH_DOWN_Server_free(vanilla_MSG_MOVE_START_PITCH_DOWN_Server* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_STOP_PITCH_Client_size(const vanilla_MSG_MOVE_STOP_PITCH_Client* object) {
    return 0 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_STOP_PITCH_Client_read(WowWorldReader* reader, vanilla_MSG_MOVE_STOP_PITCH_Client* object) {
    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_STOP_PITCH_Client_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_STOP_PITCH_Client* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_STOP_PITCH_Client_size(object) + 4); /* size */

    WRITE_U32(0x000000c1); /* opcode */

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_STOP_PITCH_Client_free(vanilla_MSG_MOVE_STOP_PITCH_Client* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_STOP_PITCH_Server_size(const vanilla_MSG_MOVE_STOP_PITCH_Server* object) {
    return 0 + wwm_packed_guid_size(object->guid) + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_STOP_PITCH_Server_read(WowWorldReader* reader, vanilla_MSG_MOVE_STOP_PITCH_Server* object) {
    READ_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_STOP_PITCH_Server_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_STOP_PITCH_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_STOP_PITCH_Server_size(object) + 2); /* size */

    WRITE_U16(0x000000c1); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_STOP_PITCH_Server_free(vanilla_MSG_MOVE_STOP_PITCH_Server* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_SET_RUN_MODE_Client_size(const vanilla_MSG_MOVE_SET_RUN_MODE_Client* object) {
    return 0 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_SET_RUN_MODE_Client_read(WowWorldReader* reader, vanilla_MSG_MOVE_SET_RUN_MODE_Client* object) {
    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_SET_RUN_MODE_Client_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_SET_RUN_MODE_Client* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_SET_RUN_MODE_Client_size(object) + 4); /* size */

    WRITE_U32(0x000000c2); /* opcode */

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_SET_RUN_MODE_Client_free(vanilla_MSG_MOVE_SET_RUN_MODE_Client* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_SET_RUN_MODE_Server_size(const vanilla_MSG_MOVE_SET_RUN_MODE_Server* object) {
    return 0 + wwm_packed_guid_size(object->guid) + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_SET_RUN_MODE_Server_read(WowWorldReader* reader, vanilla_MSG_MOVE_SET_RUN_MODE_Server* object) {
    READ_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_SET_RUN_MODE_Server_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_SET_RUN_MODE_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_SET_RUN_MODE_Server_size(object) + 2); /* size */

    WRITE_U16(0x000000c2); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_SET_RUN_MODE_Server_free(vanilla_MSG_MOVE_SET_RUN_MODE_Server* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_SET_WALK_MODE_Client_size(const vanilla_MSG_MOVE_SET_WALK_MODE_Client* object) {
    return 0 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_SET_WALK_MODE_Client_read(WowWorldReader* reader, vanilla_MSG_MOVE_SET_WALK_MODE_Client* object) {
    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_SET_WALK_MODE_Client_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_SET_WALK_MODE_Client* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_SET_WALK_MODE_Client_size(object) + 4); /* size */

    WRITE_U32(0x000000c3); /* opcode */

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_SET_WALK_MODE_Client_free(vanilla_MSG_MOVE_SET_WALK_MODE_Client* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_SET_WALK_MODE_Server_size(const vanilla_MSG_MOVE_SET_WALK_MODE_Server* object) {
    return 0 + wwm_packed_guid_size(object->guid) + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_SET_WALK_MODE_Server_read(WowWorldReader* reader, vanilla_MSG_MOVE_SET_WALK_MODE_Server* object) {
    READ_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_SET_WALK_MODE_Server_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_SET_WALK_MODE_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_SET_WALK_MODE_Server_size(object) + 2); /* size */

    WRITE_U16(0x000000c3); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_SET_WALK_MODE_Server_free(vanilla_MSG_MOVE_SET_WALK_MODE_Server* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_TELEPORT_ACK_Client_size(const vanilla_MSG_MOVE_TELEPORT_ACK_Client* object) {
    return 8 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_MSG_MOVE_TELEPORT_ACK_Client_read(WowWorldReader* reader, vanilla_MSG_MOVE_TELEPORT_ACK_Client* object) {
    READ_PACKED_GUID(object->guid);

    READ_U32(object->movement_counter);

    READ_U32(object->time);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_TELEPORT_ACK_Client_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_TELEPORT_ACK_Client* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_TELEPORT_ACK_Client_size(object) + 4); /* size */

    WRITE_U32(0x000000c7); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WRITE_U32(object->movement_counter);

    WRITE_U32(object->time);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_MSG_MOVE_TELEPORT_ACK_Server_size(const vanilla_MSG_MOVE_TELEPORT_ACK_Server* object) {
    return 4 + wwm_packed_guid_size(object->guid) + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_TELEPORT_ACK_Server_read(WowWorldReader* reader, vanilla_MSG_MOVE_TELEPORT_ACK_Server* object) {
    READ_PACKED_GUID(object->guid);

    READ_U32(object->movement_counter);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_TELEPORT_ACK_Server_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_TELEPORT_ACK_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_TELEPORT_ACK_Server_size(object) + 2); /* size */

    WRITE_U16(0x000000c7); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WRITE_U32(object->movement_counter);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_TELEPORT_ACK_Server_free(vanilla_MSG_MOVE_TELEPORT_ACK_Server* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_FALL_LAND_Client_size(const vanilla_MSG_MOVE_FALL_LAND_Client* object) {
    return 0 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_FALL_LAND_Client_read(WowWorldReader* reader, vanilla_MSG_MOVE_FALL_LAND_Client* object) {
    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_FALL_LAND_Client_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_FALL_LAND_Client* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_FALL_LAND_Client_size(object) + 4); /* size */

    WRITE_U32(0x000000c9); /* opcode */

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_FALL_LAND_Client_free(vanilla_MSG_MOVE_FALL_LAND_Client* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_FALL_LAND_Server_size(const vanilla_MSG_MOVE_FALL_LAND_Server* object) {
    return 0 + wwm_packed_guid_size(object->guid) + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_FALL_LAND_Server_read(WowWorldReader* reader, vanilla_MSG_MOVE_FALL_LAND_Server* object) {
    READ_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_FALL_LAND_Server_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_FALL_LAND_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_FALL_LAND_Server_size(object) + 2); /* size */

    WRITE_U16(0x000000c9); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_FALL_LAND_Server_free(vanilla_MSG_MOVE_FALL_LAND_Server* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_START_SWIM_Client_size(const vanilla_MSG_MOVE_START_SWIM_Client* object) {
    return 0 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_START_SWIM_Client_read(WowWorldReader* reader, vanilla_MSG_MOVE_START_SWIM_Client* object) {
    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_START_SWIM_Client_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_START_SWIM_Client* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_START_SWIM_Client_size(object) + 4); /* size */

    WRITE_U32(0x000000ca); /* opcode */

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_START_SWIM_Client_free(vanilla_MSG_MOVE_START_SWIM_Client* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_START_SWIM_Server_size(const vanilla_MSG_MOVE_START_SWIM_Server* object) {
    return 0 + wwm_packed_guid_size(object->guid) + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_START_SWIM_Server_read(WowWorldReader* reader, vanilla_MSG_MOVE_START_SWIM_Server* object) {
    READ_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_START_SWIM_Server_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_START_SWIM_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_START_SWIM_Server_size(object) + 2); /* size */

    WRITE_U16(0x000000ca); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_START_SWIM_Server_free(vanilla_MSG_MOVE_START_SWIM_Server* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_STOP_SWIM_Client_size(const vanilla_MSG_MOVE_STOP_SWIM_Client* object) {
    return 0 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_STOP_SWIM_Client_read(WowWorldReader* reader, vanilla_MSG_MOVE_STOP_SWIM_Client* object) {
    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_STOP_SWIM_Client_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_STOP_SWIM_Client* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_STOP_SWIM_Client_size(object) + 4); /* size */

    WRITE_U32(0x000000cb); /* opcode */

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_STOP_SWIM_Client_free(vanilla_MSG_MOVE_STOP_SWIM_Client* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_STOP_SWIM_Server_size(const vanilla_MSG_MOVE_STOP_SWIM_Server* object) {
    return 0 + wwm_packed_guid_size(object->guid) + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_STOP_SWIM_Server_read(WowWorldReader* reader, vanilla_MSG_MOVE_STOP_SWIM_Server* object) {
    READ_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_STOP_SWIM_Server_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_STOP_SWIM_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_STOP_SWIM_Server_size(object) + 2); /* size */

    WRITE_U16(0x000000cb); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_STOP_SWIM_Server_free(vanilla_MSG_MOVE_STOP_SWIM_Server* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_SET_FACING_Client_size(const vanilla_MSG_MOVE_SET_FACING_Client* object) {
    return 0 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_SET_FACING_Client_read(WowWorldReader* reader, vanilla_MSG_MOVE_SET_FACING_Client* object) {
    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_SET_FACING_Client_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_SET_FACING_Client* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_SET_FACING_Client_size(object) + 4); /* size */

    WRITE_U32(0x000000da); /* opcode */

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_SET_FACING_Client_free(vanilla_MSG_MOVE_SET_FACING_Client* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_SET_FACING_Server_size(const vanilla_MSG_MOVE_SET_FACING_Server* object) {
    return 0 + wwm_packed_guid_size(object->guid) + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_SET_FACING_Server_read(WowWorldReader* reader, vanilla_MSG_MOVE_SET_FACING_Server* object) {
    READ_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_SET_FACING_Server_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_SET_FACING_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_SET_FACING_Server_size(object) + 2); /* size */

    WRITE_U16(0x000000da); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_SET_FACING_Server_free(vanilla_MSG_MOVE_SET_FACING_Server* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_SET_PITCH_Client_size(const vanilla_MSG_MOVE_SET_PITCH_Client* object) {
    return 0 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_SET_PITCH_Client_read(WowWorldReader* reader, vanilla_MSG_MOVE_SET_PITCH_Client* object) {
    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_SET_PITCH_Client_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_SET_PITCH_Client* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_SET_PITCH_Client_size(object) + 4); /* size */

    WRITE_U32(0x000000db); /* opcode */

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_SET_PITCH_Client_free(vanilla_MSG_MOVE_SET_PITCH_Client* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_SET_PITCH_Server_size(const vanilla_MSG_MOVE_SET_PITCH_Server* object) {
    return 0 + wwm_packed_guid_size(object->guid) + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_SET_PITCH_Server_read(WowWorldReader* reader, vanilla_MSG_MOVE_SET_PITCH_Server* object) {
    READ_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_SET_PITCH_Server_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_SET_PITCH_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_SET_PITCH_Server_size(object) + 2); /* size */

    WRITE_U16(0x000000db); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_SET_PITCH_Server_free(vanilla_MSG_MOVE_SET_PITCH_Server* object) {
    vanilla_MovementInfo_free(&object->info);
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_WORLDPORT_ACK_cmsg_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x000000dc); /* opcode */


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_WORLDPORT_ACK_smsg_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 2); /* size */

    WRITE_U16(0x000000dc); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_MONSTER_MOVE_size(const vanilla_SMSG_MONSTER_MOVE* object) {
    size_t _size = 25 + wwm_packed_guid_size(object->guid) + wwm_monster_move_spline_size(&object->splines);

    if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_TARGET) {
        _size += 8;
    }
    else if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_ANGLE) {
        _size += 4;
    }
    else if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_SPOT) {
        _size += 12;
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_MONSTER_MOVE_read(WowWorldReader* reader, vanilla_SMSG_MONSTER_MOVE* object) {
    READ_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(all_Vector3d_read(reader, &object->spline_point));

    READ_U32(object->spline_id);

    object->move_type = 0;
    READ_U8(object->move_type);

    if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_TARGET) {
        READ_U64(object->target);

    }
    else if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_ANGLE) {
        READ_FLOAT(object->angle);

    }
    else if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_SPOT) {
        WWM_CHECK_RETURN_CODE(all_Vector3d_read(reader, &object->position));

    }
    object->spline_flags = 0;
    READ_U32(object->spline_flags);

    READ_U32(object->duration);

    READ_MONSTER_MOVE_SPLINE(object->splines);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_MONSTER_MOVE_write(WowWorldWriter* writer, const vanilla_SMSG_MONSTER_MOVE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_MONSTER_MOVE_size(object) + 2); /* size */

    WRITE_U16(0x000000dd); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(all_Vector3d_write(writer, &object->spline_point));

    WRITE_U32(object->spline_id);

    WRITE_U8(object->move_type);

    if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_TARGET) {
        WRITE_U64(object->target);

    }
    else if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_ANGLE) {
        WRITE_FLOAT(object->angle);

    }
    else if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_SPOT) {
        WWM_CHECK_RETURN_CODE(all_Vector3d_write(writer, &object->position));

    }
    WRITE_U32(object->spline_flags);

    WRITE_U32(object->duration);

    WRITE_MONSTER_MOVE_SPLINE(object->splines);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_MONSTER_MOVE_free(vanilla_SMSG_MONSTER_MOVE* object) {
    if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_TARGET) {
    }
    else if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_ANGLE) {
    }
    else if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_SPOT) {
    }
    wwm_monster_move_spline_free(&object->splines);

}

static size_t vanilla_SMSG_MOVE_WATER_WALK_size(const vanilla_SMSG_MOVE_WATER_WALK* object) {
    return 4 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_MOVE_WATER_WALK_read(WowWorldReader* reader, vanilla_SMSG_MOVE_WATER_WALK* object) {
    READ_PACKED_GUID(object->guid);

    READ_U32(object->counter);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_MOVE_WATER_WALK_write(WowWorldWriter* writer, const vanilla_SMSG_MOVE_WATER_WALK* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_MOVE_WATER_WALK_size(object) + 2); /* size */

    WRITE_U16(0x000000de); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WRITE_U32(object->counter);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_MOVE_LAND_WALK_size(const vanilla_SMSG_MOVE_LAND_WALK* object) {
    return 4 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_MOVE_LAND_WALK_read(WowWorldReader* reader, vanilla_SMSG_MOVE_LAND_WALK* object) {
    READ_PACKED_GUID(object->guid);

    READ_U32(object->counter);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_MOVE_LAND_WALK_write(WowWorldWriter* writer, const vanilla_SMSG_MOVE_LAND_WALK* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_MOVE_LAND_WALK_size(object) + 2); /* size */

    WRITE_U16(0x000000df); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WRITE_U32(object->counter);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_MOVE_SET_RAW_POSITION_read(WowWorldReader* reader, vanilla_CMSG_MOVE_SET_RAW_POSITION* object) {
    WWM_CHECK_RETURN_CODE(all_Vector3d_read(reader, &object->position));

    READ_FLOAT(object->orientation);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_MOVE_SET_RAW_POSITION_write(WowWorldWriter* writer, const vanilla_CMSG_MOVE_SET_RAW_POSITION* object) {
    WRITE_U16_BE(0x0010 + 4); /* size */

    WRITE_U32(0x000000e1); /* opcode */

    WWM_CHECK_RETURN_CODE(all_Vector3d_write(writer, &object->position));

    WRITE_FLOAT(object->orientation);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_FORCE_RUN_SPEED_CHANGE_size(const vanilla_SMSG_FORCE_RUN_SPEED_CHANGE* object) {
    return 8 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_FORCE_RUN_SPEED_CHANGE_read(WowWorldReader* reader, vanilla_SMSG_FORCE_RUN_SPEED_CHANGE* object) {
    READ_PACKED_GUID(object->guid);

    READ_U32(object->move_event);

    READ_FLOAT(object->speed);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_FORCE_RUN_SPEED_CHANGE_write(WowWorldWriter* writer, const vanilla_SMSG_FORCE_RUN_SPEED_CHANGE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_FORCE_RUN_SPEED_CHANGE_size(object) + 2); /* size */

    WRITE_U16(0x000000e2); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WRITE_U32(object->move_event);

    WRITE_FLOAT(object->speed);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_FORCE_RUN_SPEED_CHANGE_ACK_size(const vanilla_CMSG_FORCE_RUN_SPEED_CHANGE_ACK* object) {
    return 16 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_CMSG_FORCE_RUN_SPEED_CHANGE_ACK_read(WowWorldReader* reader, vanilla_CMSG_FORCE_RUN_SPEED_CHANGE_ACK* object) {
    READ_U64(object->guid);

    READ_U32(object->counter);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    READ_FLOAT(object->new_speed);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_FORCE_RUN_SPEED_CHANGE_ACK_write(WowWorldWriter* writer, const vanilla_CMSG_FORCE_RUN_SPEED_CHANGE_ACK* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_FORCE_RUN_SPEED_CHANGE_ACK_size(object) + 4); /* size */

    WRITE_U32(0x000000e3); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->counter);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));

    WRITE_FLOAT(object->new_speed);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_FORCE_RUN_SPEED_CHANGE_ACK_free(vanilla_CMSG_FORCE_RUN_SPEED_CHANGE_ACK* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_SMSG_FORCE_RUN_BACK_SPEED_CHANGE_size(const vanilla_SMSG_FORCE_RUN_BACK_SPEED_CHANGE* object) {
    return 8 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_FORCE_RUN_BACK_SPEED_CHANGE_read(WowWorldReader* reader, vanilla_SMSG_FORCE_RUN_BACK_SPEED_CHANGE* object) {
    READ_PACKED_GUID(object->guid);

    READ_U32(object->move_event);

    READ_FLOAT(object->speed);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_FORCE_RUN_BACK_SPEED_CHANGE_write(WowWorldWriter* writer, const vanilla_SMSG_FORCE_RUN_BACK_SPEED_CHANGE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_FORCE_RUN_BACK_SPEED_CHANGE_size(object) + 2); /* size */

    WRITE_U16(0x000000e4); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WRITE_U32(object->move_event);

    WRITE_FLOAT(object->speed);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK_size(const vanilla_CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK* object) {
    return 16 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK_read(WowWorldReader* reader, vanilla_CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK* object) {
    READ_U64(object->guid);

    READ_U32(object->movement_counter);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    READ_FLOAT(object->new_speed);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK_write(WowWorldWriter* writer, const vanilla_CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK_size(object) + 4); /* size */

    WRITE_U32(0x000000e5); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->movement_counter);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));

    WRITE_FLOAT(object->new_speed);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK_free(vanilla_CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_SMSG_FORCE_SWIM_SPEED_CHANGE_size(const vanilla_SMSG_FORCE_SWIM_SPEED_CHANGE* object) {
    return 8 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_FORCE_SWIM_SPEED_CHANGE_read(WowWorldReader* reader, vanilla_SMSG_FORCE_SWIM_SPEED_CHANGE* object) {
    READ_PACKED_GUID(object->guid);

    READ_U32(object->move_event);

    READ_FLOAT(object->speed);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_FORCE_SWIM_SPEED_CHANGE_write(WowWorldWriter* writer, const vanilla_SMSG_FORCE_SWIM_SPEED_CHANGE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_FORCE_SWIM_SPEED_CHANGE_size(object) + 2); /* size */

    WRITE_U16(0x000000e6); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WRITE_U32(object->move_event);

    WRITE_FLOAT(object->speed);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_FORCE_SWIM_SPEED_CHANGE_ACK_size(const vanilla_CMSG_FORCE_SWIM_SPEED_CHANGE_ACK* object) {
    return 16 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_CMSG_FORCE_SWIM_SPEED_CHANGE_ACK_read(WowWorldReader* reader, vanilla_CMSG_FORCE_SWIM_SPEED_CHANGE_ACK* object) {
    READ_U64(object->guid);

    READ_U32(object->counter);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    READ_FLOAT(object->new_speed);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_FORCE_SWIM_SPEED_CHANGE_ACK_write(WowWorldWriter* writer, const vanilla_CMSG_FORCE_SWIM_SPEED_CHANGE_ACK* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_FORCE_SWIM_SPEED_CHANGE_ACK_size(object) + 4); /* size */

    WRITE_U32(0x000000e7); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->counter);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));

    WRITE_FLOAT(object->new_speed);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_FORCE_SWIM_SPEED_CHANGE_ACK_free(vanilla_CMSG_FORCE_SWIM_SPEED_CHANGE_ACK* object) {
    vanilla_MovementInfo_free(&object->info);
}

static WowWorldResult vanilla_SMSG_FORCE_MOVE_ROOT_read(WowWorldReader* reader, vanilla_SMSG_FORCE_MOVE_ROOT* object) {
    READ_U64(object->guid);

    READ_U32(object->counter);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_FORCE_MOVE_ROOT_write(WowWorldWriter* writer, const vanilla_SMSG_FORCE_MOVE_ROOT* object) {
    WRITE_U16_BE(0x000c + 2); /* size */

    WRITE_U16(0x000000e8); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->counter);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_FORCE_MOVE_ROOT_ACK_size(const vanilla_CMSG_FORCE_MOVE_ROOT_ACK* object) {
    return 12 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_CMSG_FORCE_MOVE_ROOT_ACK_read(WowWorldReader* reader, vanilla_CMSG_FORCE_MOVE_ROOT_ACK* object) {
    READ_U64(object->guid);

    READ_U32(object->movement_counter);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_FORCE_MOVE_ROOT_ACK_write(WowWorldWriter* writer, const vanilla_CMSG_FORCE_MOVE_ROOT_ACK* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_FORCE_MOVE_ROOT_ACK_size(object) + 4); /* size */

    WRITE_U32(0x000000e9); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->movement_counter);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_FORCE_MOVE_ROOT_ACK_free(vanilla_CMSG_FORCE_MOVE_ROOT_ACK* object) {
    vanilla_MovementInfo_free(&object->info);
}

static WowWorldResult vanilla_SMSG_FORCE_MOVE_UNROOT_read(WowWorldReader* reader, vanilla_SMSG_FORCE_MOVE_UNROOT* object) {
    READ_U64(object->guid);

    READ_U32(object->counter);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_FORCE_MOVE_UNROOT_write(WowWorldWriter* writer, const vanilla_SMSG_FORCE_MOVE_UNROOT* object) {
    WRITE_U16_BE(0x000c + 2); /* size */

    WRITE_U16(0x000000ea); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->counter);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_FORCE_MOVE_UNROOT_ACK_size(const vanilla_CMSG_FORCE_MOVE_UNROOT_ACK* object) {
    return 12 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_CMSG_FORCE_MOVE_UNROOT_ACK_read(WowWorldReader* reader, vanilla_CMSG_FORCE_MOVE_UNROOT_ACK* object) {
    READ_U64(object->guid);

    READ_U32(object->movement_counter);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_FORCE_MOVE_UNROOT_ACK_write(WowWorldWriter* writer, const vanilla_CMSG_FORCE_MOVE_UNROOT_ACK* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_FORCE_MOVE_UNROOT_ACK_size(object) + 4); /* size */

    WRITE_U32(0x000000eb); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->movement_counter);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_FORCE_MOVE_UNROOT_ACK_free(vanilla_CMSG_FORCE_MOVE_UNROOT_ACK* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_HEARTBEAT_Client_size(const vanilla_MSG_MOVE_HEARTBEAT_Client* object) {
    return 0 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_HEARTBEAT_Client_read(WowWorldReader* reader, vanilla_MSG_MOVE_HEARTBEAT_Client* object) {
    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_HEARTBEAT_Client_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_HEARTBEAT_Client* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_HEARTBEAT_Client_size(object) + 4); /* size */

    WRITE_U32(0x000000ee); /* opcode */

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_HEARTBEAT_Client_free(vanilla_MSG_MOVE_HEARTBEAT_Client* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_HEARTBEAT_Server_size(const vanilla_MSG_MOVE_HEARTBEAT_Server* object) {
    return 0 + wwm_packed_guid_size(object->guid) + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_HEARTBEAT_Server_read(WowWorldReader* reader, vanilla_MSG_MOVE_HEARTBEAT_Server* object) {
    READ_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_HEARTBEAT_Server_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_HEARTBEAT_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_HEARTBEAT_Server_size(object) + 2); /* size */

    WRITE_U16(0x000000ee); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_HEARTBEAT_Server_free(vanilla_MSG_MOVE_HEARTBEAT_Server* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_SMSG_MOVE_KNOCK_BACK_size(const vanilla_SMSG_MOVE_KNOCK_BACK* object) {
    return 20 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_MOVE_KNOCK_BACK_read(WowWorldReader* reader, vanilla_SMSG_MOVE_KNOCK_BACK* object) {
    READ_PACKED_GUID(object->guid);

    READ_U32(object->movement_counter);

    READ_FLOAT(object->v_cos);

    READ_FLOAT(object->v_sin);

    READ_FLOAT(object->horizontal_speed);

    READ_FLOAT(object->vertical_speed);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_MOVE_KNOCK_BACK_write(WowWorldWriter* writer, const vanilla_SMSG_MOVE_KNOCK_BACK* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_MOVE_KNOCK_BACK_size(object) + 2); /* size */

    WRITE_U16(0x000000ef); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WRITE_U32(object->movement_counter);

    WRITE_FLOAT(object->v_cos);

    WRITE_FLOAT(object->v_sin);

    WRITE_FLOAT(object->horizontal_speed);

    WRITE_FLOAT(object->vertical_speed);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_MOVE_KNOCK_BACK_ACK_size(const vanilla_CMSG_MOVE_KNOCK_BACK_ACK* object) {
    return 12 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_CMSG_MOVE_KNOCK_BACK_ACK_read(WowWorldReader* reader, vanilla_CMSG_MOVE_KNOCK_BACK_ACK* object) {
    READ_U64(object->guid);

    READ_U32(object->counter);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_MOVE_KNOCK_BACK_ACK_write(WowWorldWriter* writer, const vanilla_CMSG_MOVE_KNOCK_BACK_ACK* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_MOVE_KNOCK_BACK_ACK_size(object) + 4); /* size */

    WRITE_U32(0x000000f0); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->counter);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_MOVE_KNOCK_BACK_ACK_free(vanilla_CMSG_MOVE_KNOCK_BACK_ACK* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_SMSG_MOVE_FEATHER_FALL_size(const vanilla_SMSG_MOVE_FEATHER_FALL* object) {
    return 4 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_MOVE_FEATHER_FALL_read(WowWorldReader* reader, vanilla_SMSG_MOVE_FEATHER_FALL* object) {
    READ_PACKED_GUID(object->guid);

    READ_U32(object->counter);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_MOVE_FEATHER_FALL_write(WowWorldWriter* writer, const vanilla_SMSG_MOVE_FEATHER_FALL* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_MOVE_FEATHER_FALL_size(object) + 2); /* size */

    WRITE_U16(0x000000f2); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WRITE_U32(object->counter);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_MOVE_NORMAL_FALL_size(const vanilla_SMSG_MOVE_NORMAL_FALL* object) {
    return 4 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_MOVE_NORMAL_FALL_read(WowWorldReader* reader, vanilla_SMSG_MOVE_NORMAL_FALL* object) {
    READ_PACKED_GUID(object->guid);

    READ_U32(object->counter);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_MOVE_NORMAL_FALL_write(WowWorldWriter* writer, const vanilla_SMSG_MOVE_NORMAL_FALL* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_MOVE_NORMAL_FALL_size(object) + 2); /* size */

    WRITE_U16(0x000000f3); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WRITE_U32(object->counter);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_MOVE_SET_HOVER_size(const vanilla_SMSG_MOVE_SET_HOVER* object) {
    return 4 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_MOVE_SET_HOVER_read(WowWorldReader* reader, vanilla_SMSG_MOVE_SET_HOVER* object) {
    READ_PACKED_GUID(object->guid);

    READ_U32(object->counter);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_MOVE_SET_HOVER_write(WowWorldWriter* writer, const vanilla_SMSG_MOVE_SET_HOVER* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_MOVE_SET_HOVER_size(object) + 2); /* size */

    WRITE_U16(0x000000f4); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WRITE_U32(object->counter);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_MOVE_UNSET_HOVER_size(const vanilla_SMSG_MOVE_UNSET_HOVER* object) {
    return 4 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_MOVE_UNSET_HOVER_read(WowWorldReader* reader, vanilla_SMSG_MOVE_UNSET_HOVER* object) {
    READ_PACKED_GUID(object->guid);

    READ_U32(object->counter);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_MOVE_UNSET_HOVER_write(WowWorldWriter* writer, const vanilla_SMSG_MOVE_UNSET_HOVER* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_MOVE_UNSET_HOVER_size(object) + 2); /* size */

    WRITE_U16(0x000000f5); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WRITE_U32(object->counter);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_MOVE_HOVER_ACK_size(const vanilla_CMSG_MOVE_HOVER_ACK* object) {
    return 16 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_CMSG_MOVE_HOVER_ACK_read(WowWorldReader* reader, vanilla_CMSG_MOVE_HOVER_ACK* object) {
    READ_U64(object->guid);

    READ_U32(object->counter);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    READ_U32(object->is_applied);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_MOVE_HOVER_ACK_write(WowWorldWriter* writer, const vanilla_CMSG_MOVE_HOVER_ACK* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_MOVE_HOVER_ACK_size(object) + 4); /* size */

    WRITE_U32(0x000000f6); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->counter);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));

    WRITE_U32(object->is_applied);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_MOVE_HOVER_ACK_free(vanilla_CMSG_MOVE_HOVER_ACK* object) {
    vanilla_MovementInfo_free(&object->info);
}

static WowWorldResult vanilla_SMSG_TRIGGER_CINEMATIC_read(WowWorldReader* reader, vanilla_SMSG_TRIGGER_CINEMATIC* object) {
    object->cinematic_sequence_id = 0;
    READ_U32(object->cinematic_sequence_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_TRIGGER_CINEMATIC_write(WowWorldWriter* writer, const vanilla_SMSG_TRIGGER_CINEMATIC* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x000000fa); /* opcode */

    WRITE_U32(object->cinematic_sequence_id);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_NEXT_CINEMATIC_CAMERA_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x000000fb); /* opcode */


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_COMPLETE_CINEMATIC_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x000000fc); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_TUTORIAL_FLAGS_read(WowWorldReader* reader, vanilla_SMSG_TUTORIAL_FLAGS* object) {
    READ_ARRAY_ALLOCATE(object->tutorial_data, 8, sizeof(uint32_t));
    READ_ARRAY(object->tutorial_data, 8, READ_U32((*object->tutorial_data)[i]));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_TUTORIAL_FLAGS_write(WowWorldWriter* writer, const vanilla_SMSG_TUTORIAL_FLAGS* object) {
    WRITE_U16_BE(0x0020 + 2); /* size */

    WRITE_U16(0x000000fd); /* opcode */

    WRITE_ARRAY(object->tutorial_data, 8, WRITE_U32((*object->tutorial_data)[i]));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_TUTORIAL_FLAGS_free(vanilla_SMSG_TUTORIAL_FLAGS* object) {
    free(object->tutorial_data);
    object->tutorial_data = NULL;
}

static WowWorldResult vanilla_CMSG_TUTORIAL_FLAG_read(WowWorldReader* reader, vanilla_CMSG_TUTORIAL_FLAG* object) {
    READ_U32(object->tutorial_flag);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_TUTORIAL_FLAG_write(WowWorldWriter* writer, const vanilla_CMSG_TUTORIAL_FLAG* object) {
    WRITE_U16_BE(0x0004 + 4); /* size */

    WRITE_U32(0x000000fe); /* opcode */

    WRITE_U32(object->tutorial_flag);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_TUTORIAL_CLEAR_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x000000ff); /* opcode */


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_TUTORIAL_RESET_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x00000100); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_STANDSTATECHANGE_read(WowWorldReader* reader, vanilla_CMSG_STANDSTATECHANGE* object) {
    object->animation_state = 0;
    READ_U32(object->animation_state);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_STANDSTATECHANGE_write(WowWorldWriter* writer, const vanilla_CMSG_STANDSTATECHANGE* object) {
    WRITE_U16_BE(0x0004 + 4); /* size */

    WRITE_U32(0x00000101); /* opcode */

    WRITE_U32(object->animation_state);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_EMOTE_read(WowWorldReader* reader, vanilla_CMSG_EMOTE* object) {
    object->emote = 0;
    READ_U32(object->emote);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_EMOTE_write(WowWorldWriter* writer, const vanilla_CMSG_EMOTE* object) {
    WRITE_U16_BE(0x0004 + 4); /* size */

    WRITE_U32(0x00000102); /* opcode */

    WRITE_U32(object->emote);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_EMOTE_read(WowWorldReader* reader, vanilla_SMSG_EMOTE* object) {
    object->emote = 0;
    READ_U32(object->emote);

    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_EMOTE_write(WowWorldWriter* writer, const vanilla_SMSG_EMOTE* object) {
    WRITE_U16_BE(0x000c + 2); /* size */

    WRITE_U16(0x00000103); /* opcode */

    WRITE_U32(object->emote);

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_TEXT_EMOTE_read(WowWorldReader* reader, vanilla_CMSG_TEXT_EMOTE* object) {
    object->text_emote = 0;
    READ_U32(object->text_emote);

    READ_U32(object->emote);

    READ_U64(object->target);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_TEXT_EMOTE_write(WowWorldWriter* writer, const vanilla_CMSG_TEXT_EMOTE* object) {
    WRITE_U16_BE(0x0010 + 4); /* size */

    WRITE_U32(0x00000104); /* opcode */

    WRITE_U32(object->text_emote);

    WRITE_U32(object->emote);

    WRITE_U64(object->target);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_TEXT_EMOTE_size(const vanilla_SMSG_TEXT_EMOTE* object) {
    return 20 + STRING_SIZE(object->name);
}

static WowWorldResult vanilla_SMSG_TEXT_EMOTE_read(WowWorldReader* reader, vanilla_SMSG_TEXT_EMOTE* object) {
    READ_U64(object->guid);

    object->text_emote = 0;
    READ_U32(object->text_emote);

    READ_U32(object->emote);

    READ_SIZED_CSTRING(object->name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_TEXT_EMOTE_write(WowWorldWriter* writer, const vanilla_SMSG_TEXT_EMOTE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_TEXT_EMOTE_size(object) + 2); /* size */

    WRITE_U16(0x00000105); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->text_emote);

    WRITE_U32(object->emote);

    WRITE_SIZED_CSTRING(object->name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_TEXT_EMOTE_free(vanilla_SMSG_TEXT_EMOTE* object) {
    FREE_STRING(object->name);

}

static WowWorldResult vanilla_CMSG_AUTOSTORE_LOOT_ITEM_read(WowWorldReader* reader, vanilla_CMSG_AUTOSTORE_LOOT_ITEM* object) {
    READ_U8(object->item_slot);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_AUTOSTORE_LOOT_ITEM_write(WowWorldWriter* writer, const vanilla_CMSG_AUTOSTORE_LOOT_ITEM* object) {
    WRITE_U16_BE(0x0001 + 4); /* size */

    WRITE_U32(0x00000108); /* opcode */

    WRITE_U8(object->item_slot);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_AUTOEQUIP_ITEM_read(WowWorldReader* reader, vanilla_CMSG_AUTOEQUIP_ITEM* object) {
    READ_U8(object->source_bag);

    READ_U8(object->source_slot);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_AUTOEQUIP_ITEM_write(WowWorldWriter* writer, const vanilla_CMSG_AUTOEQUIP_ITEM* object) {
    WRITE_U16_BE(0x0002 + 4); /* size */

    WRITE_U32(0x0000010a); /* opcode */

    WRITE_U8(object->source_bag);

    WRITE_U8(object->source_slot);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_AUTOSTORE_BAG_ITEM_read(WowWorldReader* reader, vanilla_CMSG_AUTOSTORE_BAG_ITEM* object) {
    READ_U8(object->source_bag);

    READ_U8(object->source_slot);

    READ_U8(object->destination_bag);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_AUTOSTORE_BAG_ITEM_write(WowWorldWriter* writer, const vanilla_CMSG_AUTOSTORE_BAG_ITEM* object) {
    WRITE_U16_BE(0x0003 + 4); /* size */

    WRITE_U32(0x0000010b); /* opcode */

    WRITE_U8(object->source_bag);

    WRITE_U8(object->source_slot);

    WRITE_U8(object->destination_bag);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_SWAP_ITEM_read(WowWorldReader* reader, vanilla_CMSG_SWAP_ITEM* object) {
    READ_U8(object->destination_bag);

    READ_U8(object->destionation_slot);

    READ_U8(object->source_bag);

    READ_U8(object->source_slot);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_SWAP_ITEM_write(WowWorldWriter* writer, const vanilla_CMSG_SWAP_ITEM* object) {
    WRITE_U16_BE(0x0004 + 4); /* size */

    WRITE_U32(0x0000010c); /* opcode */

    WRITE_U8(object->destination_bag);

    WRITE_U8(object->destionation_slot);

    WRITE_U8(object->source_bag);

    WRITE_U8(object->source_slot);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_SWAP_INV_ITEM_read(WowWorldReader* reader, vanilla_CMSG_SWAP_INV_ITEM* object) {
    object->source_slot = 0;
    READ_U8(object->source_slot);

    object->destination_slot = 0;
    READ_U8(object->destination_slot);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_SWAP_INV_ITEM_write(WowWorldWriter* writer, const vanilla_CMSG_SWAP_INV_ITEM* object) {
    WRITE_U16_BE(0x0002 + 4); /* size */

    WRITE_U32(0x0000010d); /* opcode */

    WRITE_U8(object->source_slot);

    WRITE_U8(object->destination_slot);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_SPLIT_ITEM_read(WowWorldReader* reader, vanilla_CMSG_SPLIT_ITEM* object) {
    READ_U8(object->source_bag);

    READ_U8(object->source_slot);

    READ_U8(object->destination_bag);

    READ_U8(object->destination_slot);

    READ_U8(object->amount);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_SPLIT_ITEM_write(WowWorldWriter* writer, const vanilla_CMSG_SPLIT_ITEM* object) {
    WRITE_U16_BE(0x0005 + 4); /* size */

    WRITE_U32(0x0000010e); /* opcode */

    WRITE_U8(object->source_bag);

    WRITE_U8(object->source_slot);

    WRITE_U8(object->destination_bag);

    WRITE_U8(object->destination_slot);

    WRITE_U8(object->amount);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_AUTOEQUIP_ITEM_SLOT_read(WowWorldReader* reader, vanilla_CMSG_AUTOEQUIP_ITEM_SLOT* object) {
    READ_U64(object->guid);

    READ_U8(object->destination_slot);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_AUTOEQUIP_ITEM_SLOT_write(WowWorldWriter* writer, const vanilla_CMSG_AUTOEQUIP_ITEM_SLOT* object) {
    WRITE_U16_BE(0x0009 + 4); /* size */

    WRITE_U32(0x0000010f); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U8(object->destination_slot);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_DESTROYITEM_read(WowWorldReader* reader, vanilla_CMSG_DESTROYITEM* object) {
    READ_U8(object->bag);

    READ_U8(object->slot);

    READ_U8(object->amount);

    READ_U8(object->unknown1);

    READ_U8(object->unknown2);

    READ_U8(object->unknown3);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_DESTROYITEM_write(WowWorldWriter* writer, const vanilla_CMSG_DESTROYITEM* object) {
    WRITE_U16_BE(0x0006 + 4); /* size */

    WRITE_U32(0x00000111); /* opcode */

    WRITE_U8(object->bag);

    WRITE_U8(object->slot);

    WRITE_U8(object->amount);

    WRITE_U8(object->unknown1);

    WRITE_U8(object->unknown2);

    WRITE_U8(object->unknown3);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_INVENTORY_CHANGE_FAILURE_size(const vanilla_SMSG_INVENTORY_CHANGE_FAILURE* object) {
    size_t _size = 1;

    if (object->result == VANILLA_INVENTORY_RESULT_CANT_EQUIP_LEVEL_I) {
        _size += 4;
    }

    if (object->result == VANILLA_INVENTORY_RESULT_CANT_EQUIP_LEVEL_I|| object->result == VANILLA_INVENTORY_RESULT_CANT_EQUIP_SKILL|| object->result == VANILLA_INVENTORY_RESULT_ITEM_DOESNT_GO_TO_SLOT|| object->result == VANILLA_INVENTORY_RESULT_BAG_FULL|| object->result == VANILLA_INVENTORY_RESULT_NONEMPTY_BAG_OVER_OTHER_BAG|| object->result == VANILLA_INVENTORY_RESULT_CANT_TRADE_EQUIP_BAGS|| object->result == VANILLA_INVENTORY_RESULT_ONLY_AMMO_CAN_GO_HERE|| object->result == VANILLA_INVENTORY_RESULT_NO_REQUIRED_PROFICIENCY|| object->result == VANILLA_INVENTORY_RESULT_NO_EQUIPMENT_SLOT_AVAILABLE|| object->result == VANILLA_INVENTORY_RESULT_YOU_CAN_NEVER_USE_THAT_ITEM|| object->result == VANILLA_INVENTORY_RESULT_YOU_CAN_NEVER_USE_THAT_ITEM2|| object->result == VANILLA_INVENTORY_RESULT_NO_EQUIPMENT_SLOT_AVAILABLE2|| object->result == VANILLA_INVENTORY_RESULT_CANT_EQUIP_WITH_TWOHANDED|| object->result == VANILLA_INVENTORY_RESULT_CANT_DUAL_WIELD|| object->result == VANILLA_INVENTORY_RESULT_ITEM_DOESNT_GO_INTO_BAG|| object->result == VANILLA_INVENTORY_RESULT_ITEM_DOESNT_GO_INTO_BAG2|| object->result == VANILLA_INVENTORY_RESULT_CANT_CARRY_MORE_OF_THIS|| object->result == VANILLA_INVENTORY_RESULT_NO_EQUIPMENT_SLOT_AVAILABLE3|| object->result == VANILLA_INVENTORY_RESULT_ITEM_CANT_STACK|| object->result == VANILLA_INVENTORY_RESULT_ITEM_CANT_BE_EQUIPPED|| object->result == VANILLA_INVENTORY_RESULT_ITEMS_CANT_BE_SWAPPED|| object->result == VANILLA_INVENTORY_RESULT_SLOT_IS_EMPTY|| object->result == VANILLA_INVENTORY_RESULT_ITEM_NOT_FOUND|| object->result == VANILLA_INVENTORY_RESULT_CANT_DROP_SOULBOUND|| object->result == VANILLA_INVENTORY_RESULT_OUT_OF_RANGE|| object->result == VANILLA_INVENTORY_RESULT_TRIED_TO_SPLIT_MORE_THAN_COUNT|| object->result == VANILLA_INVENTORY_RESULT_COULDNT_SPLIT_ITEMS|| object->result == VANILLA_INVENTORY_RESULT_MISSING_REAGENT|| object->result == VANILLA_INVENTORY_RESULT_NOT_ENOUGH_MONEY|| object->result == VANILLA_INVENTORY_RESULT_NOT_A_BAG|| object->result == VANILLA_INVENTORY_RESULT_CAN_ONLY_DO_WITH_EMPTY_BAGS|| object->result == VANILLA_INVENTORY_RESULT_DONT_OWN_THAT_ITEM|| object->result == VANILLA_INVENTORY_RESULT_CAN_EQUIP_ONLY1_QUIVER|| object->result == VANILLA_INVENTORY_RESULT_MUST_PURCHASE_THAT_BAG_SLOT|| object->result == VANILLA_INVENTORY_RESULT_TOO_FAR_AWAY_FROM_BANK|| object->result == VANILLA_INVENTORY_RESULT_ITEM_LOCKED|| object->result == VANILLA_INVENTORY_RESULT_YOU_ARE_STUNNED|| object->result == VANILLA_INVENTORY_RESULT_YOU_ARE_DEAD|| object->result == VANILLA_INVENTORY_RESULT_CANT_DO_RIGHT_NOW|| object->result == VANILLA_INVENTORY_RESULT_INT_BAG_ERROR|| object->result == VANILLA_INVENTORY_RESULT_CAN_EQUIP_ONLY1_BOLT|| object->result == VANILLA_INVENTORY_RESULT_CAN_EQUIP_ONLY1_AMMOPOUCH|| object->result == VANILLA_INVENTORY_RESULT_STACKABLE_CANT_BE_WRAPPED|| object->result == VANILLA_INVENTORY_RESULT_EQUIPPED_CANT_BE_WRAPPED|| object->result == VANILLA_INVENTORY_RESULT_WRAPPED_CANT_BE_WRAPPED|| object->result == VANILLA_INVENTORY_RESULT_BOUND_CANT_BE_WRAPPED|| object->result == VANILLA_INVENTORY_RESULT_UNIQUE_CANT_BE_WRAPPED|| object->result == VANILLA_INVENTORY_RESULT_BAGS_CANT_BE_WRAPPED|| object->result == VANILLA_INVENTORY_RESULT_ALREADY_LOOTED|| object->result == VANILLA_INVENTORY_RESULT_INVENTORY_FULL|| object->result == VANILLA_INVENTORY_RESULT_BANK_FULL|| object->result == VANILLA_INVENTORY_RESULT_ITEM_IS_CURRENTLY_SOLD_OUT|| object->result == VANILLA_INVENTORY_RESULT_BAG_FULL3|| object->result == VANILLA_INVENTORY_RESULT_ITEM_NOT_FOUND2|| object->result == VANILLA_INVENTORY_RESULT_ITEM_CANT_STACK2|| object->result == VANILLA_INVENTORY_RESULT_BAG_FULL4|| object->result == VANILLA_INVENTORY_RESULT_ITEM_SOLD_OUT|| object->result == VANILLA_INVENTORY_RESULT_OBJECT_IS_BUSY|| object->result == VANILLA_INVENTORY_RESULT_NONE|| object->result == VANILLA_INVENTORY_RESULT_NOT_IN_COMBAT|| object->result == VANILLA_INVENTORY_RESULT_NOT_WHILE_DISARMED|| object->result == VANILLA_INVENTORY_RESULT_BAG_FULL6|| object->result == VANILLA_INVENTORY_RESULT_CANT_EQUIP_RANK|| object->result == VANILLA_INVENTORY_RESULT_CANT_EQUIP_REPUTATION|| object->result == VANILLA_INVENTORY_RESULT_TOO_MANY_SPECIAL_BAGS|| object->result == VANILLA_INVENTORY_RESULT_LOOT_CANT_LOOT_THAT_NOW) {
        _size += 17;
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_INVENTORY_CHANGE_FAILURE_read(WowWorldReader* reader, vanilla_SMSG_INVENTORY_CHANGE_FAILURE* object) {
    object->result = 0;
    READ_U8(object->result);

    if (object->result == VANILLA_INVENTORY_RESULT_CANT_EQUIP_LEVEL_I) {
        READ_U32(object->required_level);

    }
    if (object->result == VANILLA_INVENTORY_RESULT_CANT_EQUIP_LEVEL_I|| object->result == VANILLA_INVENTORY_RESULT_CANT_EQUIP_SKILL|| object->result == VANILLA_INVENTORY_RESULT_ITEM_DOESNT_GO_TO_SLOT|| object->result == VANILLA_INVENTORY_RESULT_BAG_FULL|| object->result == VANILLA_INVENTORY_RESULT_NONEMPTY_BAG_OVER_OTHER_BAG|| object->result == VANILLA_INVENTORY_RESULT_CANT_TRADE_EQUIP_BAGS|| object->result == VANILLA_INVENTORY_RESULT_ONLY_AMMO_CAN_GO_HERE|| object->result == VANILLA_INVENTORY_RESULT_NO_REQUIRED_PROFICIENCY|| object->result == VANILLA_INVENTORY_RESULT_NO_EQUIPMENT_SLOT_AVAILABLE|| object->result == VANILLA_INVENTORY_RESULT_YOU_CAN_NEVER_USE_THAT_ITEM|| object->result == VANILLA_INVENTORY_RESULT_YOU_CAN_NEVER_USE_THAT_ITEM2|| object->result == VANILLA_INVENTORY_RESULT_NO_EQUIPMENT_SLOT_AVAILABLE2|| object->result == VANILLA_INVENTORY_RESULT_CANT_EQUIP_WITH_TWOHANDED|| object->result == VANILLA_INVENTORY_RESULT_CANT_DUAL_WIELD|| object->result == VANILLA_INVENTORY_RESULT_ITEM_DOESNT_GO_INTO_BAG|| object->result == VANILLA_INVENTORY_RESULT_ITEM_DOESNT_GO_INTO_BAG2|| object->result == VANILLA_INVENTORY_RESULT_CANT_CARRY_MORE_OF_THIS|| object->result == VANILLA_INVENTORY_RESULT_NO_EQUIPMENT_SLOT_AVAILABLE3|| object->result == VANILLA_INVENTORY_RESULT_ITEM_CANT_STACK|| object->result == VANILLA_INVENTORY_RESULT_ITEM_CANT_BE_EQUIPPED|| object->result == VANILLA_INVENTORY_RESULT_ITEMS_CANT_BE_SWAPPED|| object->result == VANILLA_INVENTORY_RESULT_SLOT_IS_EMPTY|| object->result == VANILLA_INVENTORY_RESULT_ITEM_NOT_FOUND|| object->result == VANILLA_INVENTORY_RESULT_CANT_DROP_SOULBOUND|| object->result == VANILLA_INVENTORY_RESULT_OUT_OF_RANGE|| object->result == VANILLA_INVENTORY_RESULT_TRIED_TO_SPLIT_MORE_THAN_COUNT|| object->result == VANILLA_INVENTORY_RESULT_COULDNT_SPLIT_ITEMS|| object->result == VANILLA_INVENTORY_RESULT_MISSING_REAGENT|| object->result == VANILLA_INVENTORY_RESULT_NOT_ENOUGH_MONEY|| object->result == VANILLA_INVENTORY_RESULT_NOT_A_BAG|| object->result == VANILLA_INVENTORY_RESULT_CAN_ONLY_DO_WITH_EMPTY_BAGS|| object->result == VANILLA_INVENTORY_RESULT_DONT_OWN_THAT_ITEM|| object->result == VANILLA_INVENTORY_RESULT_CAN_EQUIP_ONLY1_QUIVER|| object->result == VANILLA_INVENTORY_RESULT_MUST_PURCHASE_THAT_BAG_SLOT|| object->result == VANILLA_INVENTORY_RESULT_TOO_FAR_AWAY_FROM_BANK|| object->result == VANILLA_INVENTORY_RESULT_ITEM_LOCKED|| object->result == VANILLA_INVENTORY_RESULT_YOU_ARE_STUNNED|| object->result == VANILLA_INVENTORY_RESULT_YOU_ARE_DEAD|| object->result == VANILLA_INVENTORY_RESULT_CANT_DO_RIGHT_NOW|| object->result == VANILLA_INVENTORY_RESULT_INT_BAG_ERROR|| object->result == VANILLA_INVENTORY_RESULT_CAN_EQUIP_ONLY1_BOLT|| object->result == VANILLA_INVENTORY_RESULT_CAN_EQUIP_ONLY1_AMMOPOUCH|| object->result == VANILLA_INVENTORY_RESULT_STACKABLE_CANT_BE_WRAPPED|| object->result == VANILLA_INVENTORY_RESULT_EQUIPPED_CANT_BE_WRAPPED|| object->result == VANILLA_INVENTORY_RESULT_WRAPPED_CANT_BE_WRAPPED|| object->result == VANILLA_INVENTORY_RESULT_BOUND_CANT_BE_WRAPPED|| object->result == VANILLA_INVENTORY_RESULT_UNIQUE_CANT_BE_WRAPPED|| object->result == VANILLA_INVENTORY_RESULT_BAGS_CANT_BE_WRAPPED|| object->result == VANILLA_INVENTORY_RESULT_ALREADY_LOOTED|| object->result == VANILLA_INVENTORY_RESULT_INVENTORY_FULL|| object->result == VANILLA_INVENTORY_RESULT_BANK_FULL|| object->result == VANILLA_INVENTORY_RESULT_ITEM_IS_CURRENTLY_SOLD_OUT|| object->result == VANILLA_INVENTORY_RESULT_BAG_FULL3|| object->result == VANILLA_INVENTORY_RESULT_ITEM_NOT_FOUND2|| object->result == VANILLA_INVENTORY_RESULT_ITEM_CANT_STACK2|| object->result == VANILLA_INVENTORY_RESULT_BAG_FULL4|| object->result == VANILLA_INVENTORY_RESULT_ITEM_SOLD_OUT|| object->result == VANILLA_INVENTORY_RESULT_OBJECT_IS_BUSY|| object->result == VANILLA_INVENTORY_RESULT_NONE|| object->result == VANILLA_INVENTORY_RESULT_NOT_IN_COMBAT|| object->result == VANILLA_INVENTORY_RESULT_NOT_WHILE_DISARMED|| object->result == VANILLA_INVENTORY_RESULT_BAG_FULL6|| object->result == VANILLA_INVENTORY_RESULT_CANT_EQUIP_RANK|| object->result == VANILLA_INVENTORY_RESULT_CANT_EQUIP_REPUTATION|| object->result == VANILLA_INVENTORY_RESULT_TOO_MANY_SPECIAL_BAGS|| object->result == VANILLA_INVENTORY_RESULT_LOOT_CANT_LOOT_THAT_NOW) {
        READ_U64(object->item1);

        READ_U64(object->item2);

        READ_U8(object->bag_type_subclass);

    }
    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_INVENTORY_CHANGE_FAILURE_write(WowWorldWriter* writer, const vanilla_SMSG_INVENTORY_CHANGE_FAILURE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_INVENTORY_CHANGE_FAILURE_size(object) + 2); /* size */

    WRITE_U16(0x00000112); /* opcode */

    WRITE_U8(object->result);

    if (object->result == VANILLA_INVENTORY_RESULT_CANT_EQUIP_LEVEL_I) {
        WRITE_U32(object->required_level);

    }
    if (object->result == VANILLA_INVENTORY_RESULT_CANT_EQUIP_LEVEL_I|| object->result == VANILLA_INVENTORY_RESULT_CANT_EQUIP_SKILL|| object->result == VANILLA_INVENTORY_RESULT_ITEM_DOESNT_GO_TO_SLOT|| object->result == VANILLA_INVENTORY_RESULT_BAG_FULL|| object->result == VANILLA_INVENTORY_RESULT_NONEMPTY_BAG_OVER_OTHER_BAG|| object->result == VANILLA_INVENTORY_RESULT_CANT_TRADE_EQUIP_BAGS|| object->result == VANILLA_INVENTORY_RESULT_ONLY_AMMO_CAN_GO_HERE|| object->result == VANILLA_INVENTORY_RESULT_NO_REQUIRED_PROFICIENCY|| object->result == VANILLA_INVENTORY_RESULT_NO_EQUIPMENT_SLOT_AVAILABLE|| object->result == VANILLA_INVENTORY_RESULT_YOU_CAN_NEVER_USE_THAT_ITEM|| object->result == VANILLA_INVENTORY_RESULT_YOU_CAN_NEVER_USE_THAT_ITEM2|| object->result == VANILLA_INVENTORY_RESULT_NO_EQUIPMENT_SLOT_AVAILABLE2|| object->result == VANILLA_INVENTORY_RESULT_CANT_EQUIP_WITH_TWOHANDED|| object->result == VANILLA_INVENTORY_RESULT_CANT_DUAL_WIELD|| object->result == VANILLA_INVENTORY_RESULT_ITEM_DOESNT_GO_INTO_BAG|| object->result == VANILLA_INVENTORY_RESULT_ITEM_DOESNT_GO_INTO_BAG2|| object->result == VANILLA_INVENTORY_RESULT_CANT_CARRY_MORE_OF_THIS|| object->result == VANILLA_INVENTORY_RESULT_NO_EQUIPMENT_SLOT_AVAILABLE3|| object->result == VANILLA_INVENTORY_RESULT_ITEM_CANT_STACK|| object->result == VANILLA_INVENTORY_RESULT_ITEM_CANT_BE_EQUIPPED|| object->result == VANILLA_INVENTORY_RESULT_ITEMS_CANT_BE_SWAPPED|| object->result == VANILLA_INVENTORY_RESULT_SLOT_IS_EMPTY|| object->result == VANILLA_INVENTORY_RESULT_ITEM_NOT_FOUND|| object->result == VANILLA_INVENTORY_RESULT_CANT_DROP_SOULBOUND|| object->result == VANILLA_INVENTORY_RESULT_OUT_OF_RANGE|| object->result == VANILLA_INVENTORY_RESULT_TRIED_TO_SPLIT_MORE_THAN_COUNT|| object->result == VANILLA_INVENTORY_RESULT_COULDNT_SPLIT_ITEMS|| object->result == VANILLA_INVENTORY_RESULT_MISSING_REAGENT|| object->result == VANILLA_INVENTORY_RESULT_NOT_ENOUGH_MONEY|| object->result == VANILLA_INVENTORY_RESULT_NOT_A_BAG|| object->result == VANILLA_INVENTORY_RESULT_CAN_ONLY_DO_WITH_EMPTY_BAGS|| object->result == VANILLA_INVENTORY_RESULT_DONT_OWN_THAT_ITEM|| object->result == VANILLA_INVENTORY_RESULT_CAN_EQUIP_ONLY1_QUIVER|| object->result == VANILLA_INVENTORY_RESULT_MUST_PURCHASE_THAT_BAG_SLOT|| object->result == VANILLA_INVENTORY_RESULT_TOO_FAR_AWAY_FROM_BANK|| object->result == VANILLA_INVENTORY_RESULT_ITEM_LOCKED|| object->result == VANILLA_INVENTORY_RESULT_YOU_ARE_STUNNED|| object->result == VANILLA_INVENTORY_RESULT_YOU_ARE_DEAD|| object->result == VANILLA_INVENTORY_RESULT_CANT_DO_RIGHT_NOW|| object->result == VANILLA_INVENTORY_RESULT_INT_BAG_ERROR|| object->result == VANILLA_INVENTORY_RESULT_CAN_EQUIP_ONLY1_BOLT|| object->result == VANILLA_INVENTORY_RESULT_CAN_EQUIP_ONLY1_AMMOPOUCH|| object->result == VANILLA_INVENTORY_RESULT_STACKABLE_CANT_BE_WRAPPED|| object->result == VANILLA_INVENTORY_RESULT_EQUIPPED_CANT_BE_WRAPPED|| object->result == VANILLA_INVENTORY_RESULT_WRAPPED_CANT_BE_WRAPPED|| object->result == VANILLA_INVENTORY_RESULT_BOUND_CANT_BE_WRAPPED|| object->result == VANILLA_INVENTORY_RESULT_UNIQUE_CANT_BE_WRAPPED|| object->result == VANILLA_INVENTORY_RESULT_BAGS_CANT_BE_WRAPPED|| object->result == VANILLA_INVENTORY_RESULT_ALREADY_LOOTED|| object->result == VANILLA_INVENTORY_RESULT_INVENTORY_FULL|| object->result == VANILLA_INVENTORY_RESULT_BANK_FULL|| object->result == VANILLA_INVENTORY_RESULT_ITEM_IS_CURRENTLY_SOLD_OUT|| object->result == VANILLA_INVENTORY_RESULT_BAG_FULL3|| object->result == VANILLA_INVENTORY_RESULT_ITEM_NOT_FOUND2|| object->result == VANILLA_INVENTORY_RESULT_ITEM_CANT_STACK2|| object->result == VANILLA_INVENTORY_RESULT_BAG_FULL4|| object->result == VANILLA_INVENTORY_RESULT_ITEM_SOLD_OUT|| object->result == VANILLA_INVENTORY_RESULT_OBJECT_IS_BUSY|| object->result == VANILLA_INVENTORY_RESULT_NONE|| object->result == VANILLA_INVENTORY_RESULT_NOT_IN_COMBAT|| object->result == VANILLA_INVENTORY_RESULT_NOT_WHILE_DISARMED|| object->result == VANILLA_INVENTORY_RESULT_BAG_FULL6|| object->result == VANILLA_INVENTORY_RESULT_CANT_EQUIP_RANK|| object->result == VANILLA_INVENTORY_RESULT_CANT_EQUIP_REPUTATION|| object->result == VANILLA_INVENTORY_RESULT_TOO_MANY_SPECIAL_BAGS|| object->result == VANILLA_INVENTORY_RESULT_LOOT_CANT_LOOT_THAT_NOW) {
        WRITE_U64(object->item1);

        WRITE_U64(object->item2);

        WRITE_U8(object->bag_type_subclass);

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_INVENTORY_CHANGE_FAILURE_free(vanilla_SMSG_INVENTORY_CHANGE_FAILURE* object) {
    if (object->result == VANILLA_INVENTORY_RESULT_CANT_EQUIP_LEVEL_I) {
    }
    if (object->result == VANILLA_INVENTORY_RESULT_CANT_EQUIP_LEVEL_I|| object->result == VANILLA_INVENTORY_RESULT_CANT_EQUIP_SKILL|| object->result == VANILLA_INVENTORY_RESULT_ITEM_DOESNT_GO_TO_SLOT|| object->result == VANILLA_INVENTORY_RESULT_BAG_FULL|| object->result == VANILLA_INVENTORY_RESULT_NONEMPTY_BAG_OVER_OTHER_BAG|| object->result == VANILLA_INVENTORY_RESULT_CANT_TRADE_EQUIP_BAGS|| object->result == VANILLA_INVENTORY_RESULT_ONLY_AMMO_CAN_GO_HERE|| object->result == VANILLA_INVENTORY_RESULT_NO_REQUIRED_PROFICIENCY|| object->result == VANILLA_INVENTORY_RESULT_NO_EQUIPMENT_SLOT_AVAILABLE|| object->result == VANILLA_INVENTORY_RESULT_YOU_CAN_NEVER_USE_THAT_ITEM|| object->result == VANILLA_INVENTORY_RESULT_YOU_CAN_NEVER_USE_THAT_ITEM2|| object->result == VANILLA_INVENTORY_RESULT_NO_EQUIPMENT_SLOT_AVAILABLE2|| object->result == VANILLA_INVENTORY_RESULT_CANT_EQUIP_WITH_TWOHANDED|| object->result == VANILLA_INVENTORY_RESULT_CANT_DUAL_WIELD|| object->result == VANILLA_INVENTORY_RESULT_ITEM_DOESNT_GO_INTO_BAG|| object->result == VANILLA_INVENTORY_RESULT_ITEM_DOESNT_GO_INTO_BAG2|| object->result == VANILLA_INVENTORY_RESULT_CANT_CARRY_MORE_OF_THIS|| object->result == VANILLA_INVENTORY_RESULT_NO_EQUIPMENT_SLOT_AVAILABLE3|| object->result == VANILLA_INVENTORY_RESULT_ITEM_CANT_STACK|| object->result == VANILLA_INVENTORY_RESULT_ITEM_CANT_BE_EQUIPPED|| object->result == VANILLA_INVENTORY_RESULT_ITEMS_CANT_BE_SWAPPED|| object->result == VANILLA_INVENTORY_RESULT_SLOT_IS_EMPTY|| object->result == VANILLA_INVENTORY_RESULT_ITEM_NOT_FOUND|| object->result == VANILLA_INVENTORY_RESULT_CANT_DROP_SOULBOUND|| object->result == VANILLA_INVENTORY_RESULT_OUT_OF_RANGE|| object->result == VANILLA_INVENTORY_RESULT_TRIED_TO_SPLIT_MORE_THAN_COUNT|| object->result == VANILLA_INVENTORY_RESULT_COULDNT_SPLIT_ITEMS|| object->result == VANILLA_INVENTORY_RESULT_MISSING_REAGENT|| object->result == VANILLA_INVENTORY_RESULT_NOT_ENOUGH_MONEY|| object->result == VANILLA_INVENTORY_RESULT_NOT_A_BAG|| object->result == VANILLA_INVENTORY_RESULT_CAN_ONLY_DO_WITH_EMPTY_BAGS|| object->result == VANILLA_INVENTORY_RESULT_DONT_OWN_THAT_ITEM|| object->result == VANILLA_INVENTORY_RESULT_CAN_EQUIP_ONLY1_QUIVER|| object->result == VANILLA_INVENTORY_RESULT_MUST_PURCHASE_THAT_BAG_SLOT|| object->result == VANILLA_INVENTORY_RESULT_TOO_FAR_AWAY_FROM_BANK|| object->result == VANILLA_INVENTORY_RESULT_ITEM_LOCKED|| object->result == VANILLA_INVENTORY_RESULT_YOU_ARE_STUNNED|| object->result == VANILLA_INVENTORY_RESULT_YOU_ARE_DEAD|| object->result == VANILLA_INVENTORY_RESULT_CANT_DO_RIGHT_NOW|| object->result == VANILLA_INVENTORY_RESULT_INT_BAG_ERROR|| object->result == VANILLA_INVENTORY_RESULT_CAN_EQUIP_ONLY1_BOLT|| object->result == VANILLA_INVENTORY_RESULT_CAN_EQUIP_ONLY1_AMMOPOUCH|| object->result == VANILLA_INVENTORY_RESULT_STACKABLE_CANT_BE_WRAPPED|| object->result == VANILLA_INVENTORY_RESULT_EQUIPPED_CANT_BE_WRAPPED|| object->result == VANILLA_INVENTORY_RESULT_WRAPPED_CANT_BE_WRAPPED|| object->result == VANILLA_INVENTORY_RESULT_BOUND_CANT_BE_WRAPPED|| object->result == VANILLA_INVENTORY_RESULT_UNIQUE_CANT_BE_WRAPPED|| object->result == VANILLA_INVENTORY_RESULT_BAGS_CANT_BE_WRAPPED|| object->result == VANILLA_INVENTORY_RESULT_ALREADY_LOOTED|| object->result == VANILLA_INVENTORY_RESULT_INVENTORY_FULL|| object->result == VANILLA_INVENTORY_RESULT_BANK_FULL|| object->result == VANILLA_INVENTORY_RESULT_ITEM_IS_CURRENTLY_SOLD_OUT|| object->result == VANILLA_INVENTORY_RESULT_BAG_FULL3|| object->result == VANILLA_INVENTORY_RESULT_ITEM_NOT_FOUND2|| object->result == VANILLA_INVENTORY_RESULT_ITEM_CANT_STACK2|| object->result == VANILLA_INVENTORY_RESULT_BAG_FULL4|| object->result == VANILLA_INVENTORY_RESULT_ITEM_SOLD_OUT|| object->result == VANILLA_INVENTORY_RESULT_OBJECT_IS_BUSY|| object->result == VANILLA_INVENTORY_RESULT_NONE|| object->result == VANILLA_INVENTORY_RESULT_NOT_IN_COMBAT|| object->result == VANILLA_INVENTORY_RESULT_NOT_WHILE_DISARMED|| object->result == VANILLA_INVENTORY_RESULT_BAG_FULL6|| object->result == VANILLA_INVENTORY_RESULT_CANT_EQUIP_RANK|| object->result == VANILLA_INVENTORY_RESULT_CANT_EQUIP_REPUTATION|| object->result == VANILLA_INVENTORY_RESULT_TOO_MANY_SPECIAL_BAGS|| object->result == VANILLA_INVENTORY_RESULT_LOOT_CANT_LOOT_THAT_NOW) {
    }
}

static WowWorldResult vanilla_SMSG_OPEN_CONTAINER_read(WowWorldReader* reader, vanilla_SMSG_OPEN_CONTAINER* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_OPEN_CONTAINER_write(WowWorldWriter* writer, const vanilla_SMSG_OPEN_CONTAINER* object) {
    WRITE_U16_BE(0x0008 + 2); /* size */

    WRITE_U16(0x00000113); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_INSPECT_read(WowWorldReader* reader, vanilla_CMSG_INSPECT* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_INSPECT_write(WowWorldWriter* writer, const vanilla_CMSG_INSPECT* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x00000114); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_INSPECT_read(WowWorldReader* reader, vanilla_SMSG_INSPECT* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_INSPECT_write(WowWorldWriter* writer, const vanilla_SMSG_INSPECT* object) {
    WRITE_U16_BE(0x0008 + 2); /* size */

    WRITE_U16(0x00000115); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_INITIATE_TRADE_read(WowWorldReader* reader, vanilla_CMSG_INITIATE_TRADE* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_INITIATE_TRADE_write(WowWorldWriter* writer, const vanilla_CMSG_INITIATE_TRADE* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x00000116); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_BEGIN_TRADE_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x00000117); /* opcode */


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_BUSY_TRADE_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x00000118); /* opcode */


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_IGNORE_TRADE_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x00000119); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_ACCEPT_TRADE_read(WowWorldReader* reader, vanilla_CMSG_ACCEPT_TRADE* object) {
    READ_U32(object->unknown1);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_ACCEPT_TRADE_write(WowWorldWriter* writer, const vanilla_CMSG_ACCEPT_TRADE* object) {
    WRITE_U16_BE(0x0004 + 4); /* size */

    WRITE_U32(0x0000011a); /* opcode */

    WRITE_U32(object->unknown1);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_UNACCEPT_TRADE_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x0000011b); /* opcode */


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_CANCEL_TRADE_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x0000011c); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_SET_TRADE_ITEM_read(WowWorldReader* reader, vanilla_CMSG_SET_TRADE_ITEM* object) {
    READ_U8(object->trade_slot);

    READ_U8(object->bag);

    READ_U8(object->slot);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_SET_TRADE_ITEM_write(WowWorldWriter* writer, const vanilla_CMSG_SET_TRADE_ITEM* object) {
    WRITE_U16_BE(0x0003 + 4); /* size */

    WRITE_U32(0x0000011d); /* opcode */

    WRITE_U8(object->trade_slot);

    WRITE_U8(object->bag);

    WRITE_U8(object->slot);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_CLEAR_TRADE_ITEM_read(WowWorldReader* reader, vanilla_CMSG_CLEAR_TRADE_ITEM* object) {
    READ_U8(object->trade_slot);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_CLEAR_TRADE_ITEM_write(WowWorldWriter* writer, const vanilla_CMSG_CLEAR_TRADE_ITEM* object) {
    WRITE_U16_BE(0x0001 + 4); /* size */

    WRITE_U32(0x0000011e); /* opcode */

    WRITE_U8(object->trade_slot);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_SET_TRADE_GOLD_read(WowWorldReader* reader, vanilla_CMSG_SET_TRADE_GOLD* object) {
    READ_U32(object->gold);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_SET_TRADE_GOLD_write(WowWorldWriter* writer, const vanilla_CMSG_SET_TRADE_GOLD* object) {
    WRITE_U16_BE(0x0004 + 4); /* size */

    WRITE_U32(0x0000011f); /* opcode */

    WRITE_U32(object->gold);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_TRADE_STATUS_size(const vanilla_SMSG_TRADE_STATUS* object) {
    size_t _size = 4;

    if (object->status == VANILLA_TRADE_STATUS_BEGIN_TRADE) {
        _size += 8;
    }
    else if (object->status == VANILLA_TRADE_STATUS_CLOSE_WINDOW) {
        _size += 9;
    }
    else if (object->status == VANILLA_TRADE_STATUS_ONLY_CONJURED|| object->status == VANILLA_TRADE_STATUS_NOT_ON_TAPLIST) {
        _size += 1;
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_TRADE_STATUS_read(WowWorldReader* reader, vanilla_SMSG_TRADE_STATUS* object) {
    object->status = 0;
    READ_U32(object->status);

    if (object->status == VANILLA_TRADE_STATUS_BEGIN_TRADE) {
        READ_U64(object->unknown1);

    }
    else if (object->status == VANILLA_TRADE_STATUS_CLOSE_WINDOW) {
        object->inventory_result = 0;
        READ_U32(object->inventory_result);

        READ_BOOL8(object->target_error);

        READ_U32(object->item_limit_category_id);

    }
    else if (object->status == VANILLA_TRADE_STATUS_ONLY_CONJURED|| object->status == VANILLA_TRADE_STATUS_NOT_ON_TAPLIST) {
        READ_U8(object->slot);

    }
    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_TRADE_STATUS_write(WowWorldWriter* writer, const vanilla_SMSG_TRADE_STATUS* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_TRADE_STATUS_size(object) + 2); /* size */

    WRITE_U16(0x00000120); /* opcode */

    WRITE_U32(object->status);

    if (object->status == VANILLA_TRADE_STATUS_BEGIN_TRADE) {
        WRITE_U64(object->unknown1);

    }
    else if (object->status == VANILLA_TRADE_STATUS_CLOSE_WINDOW) {
        WRITE_U32(object->inventory_result);

        WRITE_BOOL8(object->target_error);

        WRITE_U32(object->item_limit_category_id);

    }
    else if (object->status == VANILLA_TRADE_STATUS_ONLY_CONJURED|| object->status == VANILLA_TRADE_STATUS_NOT_ON_TAPLIST) {
        WRITE_U8(object->slot);

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_TRADE_STATUS_free(vanilla_SMSG_TRADE_STATUS* object) {
    if (object->status == VANILLA_TRADE_STATUS_BEGIN_TRADE) {
    }
    else if (object->status == VANILLA_TRADE_STATUS_CLOSE_WINDOW) {
    }
    else if (object->status == VANILLA_TRADE_STATUS_ONLY_CONJURED|| object->status == VANILLA_TRADE_STATUS_NOT_ON_TAPLIST) {
    }
}

static WowWorldResult vanilla_SMSG_TRADE_STATUS_EXTENDED_read(WowWorldReader* reader, vanilla_SMSG_TRADE_STATUS_EXTENDED* object) {
    READ_BOOL8(object->self_player);

    READ_U32(object->trade_slot_count1);

    READ_U32(object->trade_slot_count2);

    READ_U32(object->money_in_trade);

    READ_U32(object->spell_on_lowest_slot);

    READ_ARRAY_ALLOCATE(object->trade_slots, 7, sizeof(vanilla_TradeSlot));
    READ_ARRAY(object->trade_slots, 7, WWM_CHECK_RETURN_CODE(vanilla_TradeSlot_read(reader, &(*object->trade_slots)[i])));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_TRADE_STATUS_EXTENDED_write(WowWorldWriter* writer, const vanilla_SMSG_TRADE_STATUS_EXTENDED* object) {
    WRITE_U16_BE(0x01bc + 2); /* size */

    WRITE_U16(0x00000121); /* opcode */

    WRITE_BOOL8(object->self_player);

    WRITE_U32(object->trade_slot_count1);

    WRITE_U32(object->trade_slot_count2);

    WRITE_U32(object->money_in_trade);

    WRITE_U32(object->spell_on_lowest_slot);

    WRITE_ARRAY(object->trade_slots, 7, WWM_CHECK_RETURN_CODE(vanilla_TradeSlot_write(writer, &(*object->trade_slots)[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_TRADE_STATUS_EXTENDED_free(vanilla_SMSG_TRADE_STATUS_EXTENDED* object) {
    free(object->trade_slots);
    object->trade_slots = NULL;
}

static size_t vanilla_SMSG_INITIALIZE_FACTIONS_size(const vanilla_SMSG_INITIALIZE_FACTIONS* object) {
    return 4 + 5 * object->amount_of_factions;
}

static WowWorldResult vanilla_SMSG_INITIALIZE_FACTIONS_read(WowWorldReader* reader, vanilla_SMSG_INITIALIZE_FACTIONS* object) {
    READ_U32(object->amount_of_factions);

    READ_ARRAY_ALLOCATE(object->factions, object->amount_of_factions, sizeof(vanilla_FactionInitializer));
    READ_ARRAY(object->factions, object->amount_of_factions, WWM_CHECK_RETURN_CODE(vanilla_FactionInitializer_read(reader, &object->factions[i])));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_INITIALIZE_FACTIONS_write(WowWorldWriter* writer, const vanilla_SMSG_INITIALIZE_FACTIONS* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_INITIALIZE_FACTIONS_size(object) + 2); /* size */

    WRITE_U16(0x00000122); /* opcode */

    WRITE_U32(object->amount_of_factions);

    WRITE_ARRAY(object->factions, object->amount_of_factions, WWM_CHECK_RETURN_CODE(vanilla_FactionInitializer_write(writer, &object->factions[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_INITIALIZE_FACTIONS_free(vanilla_SMSG_INITIALIZE_FACTIONS* object) {
    free(object->factions);
    object->factions = NULL;
}

static WowWorldResult vanilla_SMSG_SET_FACTION_VISIBLE_read(WowWorldReader* reader, vanilla_SMSG_SET_FACTION_VISIBLE* object) {
    object->faction = 0;
    READ_U16(object->faction);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SET_FACTION_VISIBLE_write(WowWorldWriter* writer, const vanilla_SMSG_SET_FACTION_VISIBLE* object) {
    WRITE_U16_BE(0x0002 + 2); /* size */

    WRITE_U16(0x00000123); /* opcode */

    WRITE_U16(object->faction);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_SET_FACTION_STANDING_size(const vanilla_SMSG_SET_FACTION_STANDING* object) {
    return 4 + 6 * object->amount_of_faction_standings;
}

static WowWorldResult vanilla_SMSG_SET_FACTION_STANDING_read(WowWorldReader* reader, vanilla_SMSG_SET_FACTION_STANDING* object) {
    READ_U32(object->amount_of_faction_standings);

    READ_ARRAY_ALLOCATE(object->faction_standings, object->amount_of_faction_standings, sizeof(vanilla_FactionStanding));
    READ_ARRAY(object->faction_standings, object->amount_of_faction_standings, WWM_CHECK_RETURN_CODE(vanilla_FactionStanding_read(reader, &object->faction_standings[i])));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SET_FACTION_STANDING_write(WowWorldWriter* writer, const vanilla_SMSG_SET_FACTION_STANDING* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SET_FACTION_STANDING_size(object) + 2); /* size */

    WRITE_U16(0x00000124); /* opcode */

    WRITE_U32(object->amount_of_faction_standings);

    WRITE_ARRAY(object->faction_standings, object->amount_of_faction_standings, WWM_CHECK_RETURN_CODE(vanilla_FactionStanding_write(writer, &object->faction_standings[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_SET_FACTION_STANDING_free(vanilla_SMSG_SET_FACTION_STANDING* object) {
    free(object->faction_standings);
    object->faction_standings = NULL;
}

static WowWorldResult vanilla_CMSG_SET_FACTION_ATWAR_read(WowWorldReader* reader, vanilla_CMSG_SET_FACTION_ATWAR* object) {
    object->faction = 0;
    READ_U16(object->faction);

    object->flags = 0;
    READ_U8(object->flags);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_SET_FACTION_ATWAR_write(WowWorldWriter* writer, const vanilla_CMSG_SET_FACTION_ATWAR* object) {
    WRITE_U16_BE(0x0003 + 4); /* size */

    WRITE_U32(0x00000125); /* opcode */

    WRITE_U16(object->faction);

    WRITE_U8(object->flags);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_SET_PROFICIENCY_read(WowWorldReader* reader, vanilla_SMSG_SET_PROFICIENCY* object) {
    object->class_type = 0;
    READ_U8(object->class_type);

    READ_U32(object->item_sub_class_mask);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SET_PROFICIENCY_write(WowWorldWriter* writer, const vanilla_SMSG_SET_PROFICIENCY* object) {
    WRITE_U16_BE(0x0005 + 2); /* size */

    WRITE_U16(0x00000127); /* opcode */

    WRITE_U8(object->class_type);

    WRITE_U32(object->item_sub_class_mask);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_SET_ACTION_BUTTON_read(WowWorldReader* reader, vanilla_CMSG_SET_ACTION_BUTTON* object) {
    READ_U8(object->button);

    READ_U16(object->action);

    READ_U8(object->misc);

    READ_U8(object->action_type);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_SET_ACTION_BUTTON_write(WowWorldWriter* writer, const vanilla_CMSG_SET_ACTION_BUTTON* object) {
    WRITE_U16_BE(0x0005 + 4); /* size */

    WRITE_U32(0x00000128); /* opcode */

    WRITE_U8(object->button);

    WRITE_U16(object->action);

    WRITE_U8(object->misc);

    WRITE_U8(object->action_type);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_ACTION_BUTTONS_read(WowWorldReader* reader, vanilla_SMSG_ACTION_BUTTONS* object) {
    READ_ARRAY_ALLOCATE(object->data, 120, sizeof(uint32_t));
    READ_ARRAY(object->data, 120, READ_U32((*object->data)[i]));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_ACTION_BUTTONS_write(WowWorldWriter* writer, const vanilla_SMSG_ACTION_BUTTONS* object) {
    WRITE_U16_BE(0x01e0 + 2); /* size */

    WRITE_U16(0x00000129); /* opcode */

    WRITE_ARRAY(object->data, 120, WRITE_U32((*object->data)[i]));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_ACTION_BUTTONS_free(vanilla_SMSG_ACTION_BUTTONS* object) {
    free(object->data);
    object->data = NULL;
}

static size_t vanilla_SMSG_INITIAL_SPELLS_size(const vanilla_SMSG_INITIAL_SPELLS* object) {
    return 5 + 4 * object->spell_count + 14 * object->cooldown_count;
}

static WowWorldResult vanilla_SMSG_INITIAL_SPELLS_read(WowWorldReader* reader, vanilla_SMSG_INITIAL_SPELLS* object) {
    READ_U8(object->unknown1);

    READ_U16(object->spell_count);

    READ_ARRAY_ALLOCATE(object->initial_spells, object->spell_count, sizeof(vanilla_InitialSpell));
    READ_ARRAY(object->initial_spells, object->spell_count, WWM_CHECK_RETURN_CODE(vanilla_InitialSpell_read(reader, &object->initial_spells[i])));

    READ_U16(object->cooldown_count);

    READ_ARRAY_ALLOCATE(object->cooldowns, object->cooldown_count, sizeof(vanilla_CooldownSpell));
    READ_ARRAY(object->cooldowns, object->cooldown_count, WWM_CHECK_RETURN_CODE(vanilla_CooldownSpell_read(reader, &object->cooldowns[i])));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_INITIAL_SPELLS_write(WowWorldWriter* writer, const vanilla_SMSG_INITIAL_SPELLS* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_INITIAL_SPELLS_size(object) + 2); /* size */

    WRITE_U16(0x0000012a); /* opcode */

    WRITE_U8(object->unknown1);

    WRITE_U16(object->spell_count);

    WRITE_ARRAY(object->initial_spells, object->spell_count, WWM_CHECK_RETURN_CODE(vanilla_InitialSpell_write(writer, &object->initial_spells[i])));

    WRITE_U16(object->cooldown_count);

    WRITE_ARRAY(object->cooldowns, object->cooldown_count, WWM_CHECK_RETURN_CODE(vanilla_CooldownSpell_write(writer, &object->cooldowns[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_INITIAL_SPELLS_free(vanilla_SMSG_INITIAL_SPELLS* object) {
    free(object->initial_spells);
    object->initial_spells = NULL;
    free(object->cooldowns);
    object->cooldowns = NULL;
}

static WowWorldResult vanilla_SMSG_LEARNED_SPELL_read(WowWorldReader* reader, vanilla_SMSG_LEARNED_SPELL* object) {
    READ_U32(object->id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_LEARNED_SPELL_write(WowWorldWriter* writer, const vanilla_SMSG_LEARNED_SPELL* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x0000012b); /* opcode */

    WRITE_U32(object->id);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_SUPERCEDED_SPELL_read(WowWorldReader* reader, vanilla_SMSG_SUPERCEDED_SPELL* object) {
    READ_U16(object->new_spell_id);

    READ_U16(object->old_spell_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SUPERCEDED_SPELL_write(WowWorldWriter* writer, const vanilla_SMSG_SUPERCEDED_SPELL* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x0000012c); /* opcode */

    WRITE_U16(object->new_spell_id);

    WRITE_U16(object->old_spell_id);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_CAST_SPELL_size(const vanilla_CMSG_CAST_SPELL* object) {
    return 4 + vanilla_SpellCastTargets_size(&object->targets);
}

static WowWorldResult vanilla_CMSG_CAST_SPELL_read(WowWorldReader* reader, vanilla_CMSG_CAST_SPELL* object) {
    READ_U32(object->spell);

    WWM_CHECK_RETURN_CODE(vanilla_SpellCastTargets_read(reader, &object->targets));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_CAST_SPELL_write(WowWorldWriter* writer, const vanilla_CMSG_CAST_SPELL* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_CAST_SPELL_size(object) + 4); /* size */

    WRITE_U32(0x0000012e); /* opcode */

    WRITE_U32(object->spell);

    WWM_CHECK_RETURN_CODE(vanilla_SpellCastTargets_write(writer, &object->targets));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_CAST_SPELL_free(vanilla_CMSG_CAST_SPELL* object) {
    vanilla_SpellCastTargets_free(&object->targets);
}

static WowWorldResult vanilla_CMSG_CANCEL_CAST_read(WowWorldReader* reader, vanilla_CMSG_CANCEL_CAST* object) {
    READ_U32(object->id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_CANCEL_CAST_write(WowWorldWriter* writer, const vanilla_CMSG_CANCEL_CAST* object) {
    WRITE_U16_BE(0x0004 + 4); /* size */

    WRITE_U32(0x0000012f); /* opcode */

    WRITE_U32(object->id);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_CAST_RESULT_size(const vanilla_SMSG_CAST_RESULT* object) {
    size_t _size = 5;

    if (object->result == VANILLA_SIMPLE_SPELL_CAST_RESULT_SUCCESS) {
        _size += 1;

        if (object->reason == VANILLA_CAST_FAILURE_REASON_REQUIRES_SPELL_FOCUS) {
            _size += 4;
        }
        else if (object->reason == VANILLA_CAST_FAILURE_REASON_REQUIRES_AREA) {
            _size += 4;
        }
        else if (object->reason == VANILLA_CAST_FAILURE_REASON_EQUIPPED_ITEM_CLASS) {
            _size += 12;
        }

    }

    return _size;
}

static WowWorldResult vanilla_SMSG_CAST_RESULT_read(WowWorldReader* reader, vanilla_SMSG_CAST_RESULT* object) {
    READ_U32(object->spell);

    object->result = 0;
    READ_U8(object->result);

    if (object->result == VANILLA_SIMPLE_SPELL_CAST_RESULT_SUCCESS) {
        object->reason = 0;
        READ_U8(object->reason);

        if (object->reason == VANILLA_CAST_FAILURE_REASON_REQUIRES_SPELL_FOCUS) {
            READ_U32(object->required_spell_focus);

        }
        else if (object->reason == VANILLA_CAST_FAILURE_REASON_REQUIRES_AREA) {
            object->area = 0;
            READ_U32(object->area);

        }
        else if (object->reason == VANILLA_CAST_FAILURE_REASON_EQUIPPED_ITEM_CLASS) {
            READ_U32(object->equipped_item_class);

            READ_U32(object->equipped_item_subclass_mask);

            READ_U32(object->equipped_item_inventory_type_mask);

        }
    }
    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_CAST_RESULT_write(WowWorldWriter* writer, const vanilla_SMSG_CAST_RESULT* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_CAST_RESULT_size(object) + 2); /* size */

    WRITE_U16(0x00000130); /* opcode */

    WRITE_U32(object->spell);

    WRITE_U8(object->result);

    if (object->result == VANILLA_SIMPLE_SPELL_CAST_RESULT_SUCCESS) {
        WRITE_U8(object->reason);

        if (object->reason == VANILLA_CAST_FAILURE_REASON_REQUIRES_SPELL_FOCUS) {
            WRITE_U32(object->required_spell_focus);

        }
        else if (object->reason == VANILLA_CAST_FAILURE_REASON_REQUIRES_AREA) {
            WRITE_U32(object->area);

        }
        else if (object->reason == VANILLA_CAST_FAILURE_REASON_EQUIPPED_ITEM_CLASS) {
            WRITE_U32(object->equipped_item_class);

            WRITE_U32(object->equipped_item_subclass_mask);

            WRITE_U32(object->equipped_item_inventory_type_mask);

        }
    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_CAST_RESULT_free(vanilla_SMSG_CAST_RESULT* object) {
    if (object->result == VANILLA_SIMPLE_SPELL_CAST_RESULT_SUCCESS) {
        if (object->reason == VANILLA_CAST_FAILURE_REASON_REQUIRES_SPELL_FOCUS) {
        }
        else if (object->reason == VANILLA_CAST_FAILURE_REASON_REQUIRES_AREA) {
        }
        else if (object->reason == VANILLA_CAST_FAILURE_REASON_EQUIPPED_ITEM_CLASS) {
        }
    }
}

static size_t vanilla_SMSG_SPELL_START_size(const vanilla_SMSG_SPELL_START* object) {
    size_t _size = 10 + wwm_packed_guid_size(object->cast_item) + wwm_packed_guid_size(object->caster) + vanilla_SpellCastTargets_size(&object->targets);

    if ((object->flags & VANILLA_CAST_FLAGS_AMMO) != 0) {
        _size += 8;
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_SPELL_START_read(WowWorldReader* reader, vanilla_SMSG_SPELL_START* object) {
    READ_PACKED_GUID(object->cast_item);

    READ_PACKED_GUID(object->caster);

    READ_U32(object->spell);

    object->flags = 0;
    READ_U16(object->flags);

    READ_U32(object->timer);

    WWM_CHECK_RETURN_CODE(vanilla_SpellCastTargets_read(reader, &object->targets));

    if ((object->flags & VANILLA_CAST_FLAGS_AMMO) != 0) {
        READ_U32(object->ammo_display_id);

        READ_U32(object->ammo_inventory_type);

    }
    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPELL_START_write(WowWorldWriter* writer, const vanilla_SMSG_SPELL_START* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SPELL_START_size(object) + 2); /* size */

    WRITE_U16(0x00000131); /* opcode */

    WRITE_PACKED_GUID(object->cast_item);

    WRITE_PACKED_GUID(object->caster);

    WRITE_U32(object->spell);

    WRITE_U16(object->flags);

    WRITE_U32(object->timer);

    WWM_CHECK_RETURN_CODE(vanilla_SpellCastTargets_write(writer, &object->targets));

    if ((object->flags & VANILLA_CAST_FLAGS_AMMO) != 0) {
        WRITE_U32(object->ammo_display_id);

        WRITE_U32(object->ammo_inventory_type);

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_SPELL_START_free(vanilla_SMSG_SPELL_START* object) {
    vanilla_SpellCastTargets_free(&object->targets);
    if ((object->flags & VANILLA_CAST_FLAGS_AMMO) != 0) {
    }
}

static size_t vanilla_SMSG_SPELL_GO_size(const vanilla_SMSG_SPELL_GO* object) {
    size_t _size = 8 + wwm_packed_guid_size(object->cast_item) + wwm_packed_guid_size(object->caster) + 8 * object->amount_of_hits + 9 * object->amount_of_misses + vanilla_SpellCastTargets_size(&object->targets);

    if ((object->flags & VANILLA_CAST_FLAGS_AMMO) != 0) {
        _size += 8;
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_SPELL_GO_read(WowWorldReader* reader, vanilla_SMSG_SPELL_GO* object) {
    READ_PACKED_GUID(object->cast_item);

    READ_PACKED_GUID(object->caster);

    READ_U32(object->spell);

    object->flags = 0;
    READ_U16(object->flags);

    READ_U8(object->amount_of_hits);

    READ_ARRAY_ALLOCATE(object->hits, object->amount_of_hits, sizeof(uint64_t));
    READ_ARRAY(object->hits, object->amount_of_hits, READ_U64(object->hits[i]));

    READ_U8(object->amount_of_misses);

    READ_ARRAY_ALLOCATE(object->misses, object->amount_of_misses, sizeof(vanilla_SpellMiss));
    READ_ARRAY(object->misses, object->amount_of_misses, WWM_CHECK_RETURN_CODE(vanilla_SpellMiss_read(reader, &object->misses[i])));

    WWM_CHECK_RETURN_CODE(vanilla_SpellCastTargets_read(reader, &object->targets));

    if ((object->flags & VANILLA_CAST_FLAGS_AMMO) != 0) {
        READ_U32(object->ammo_display_id);

        READ_U32(object->ammo_inventory_type);

    }
    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPELL_GO_write(WowWorldWriter* writer, const vanilla_SMSG_SPELL_GO* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SPELL_GO_size(object) + 2); /* size */

    WRITE_U16(0x00000132); /* opcode */

    WRITE_PACKED_GUID(object->cast_item);

    WRITE_PACKED_GUID(object->caster);

    WRITE_U32(object->spell);

    WRITE_U16(object->flags);

    WRITE_U8(object->amount_of_hits);

    WRITE_ARRAY(object->hits, object->amount_of_hits, WRITE_U64(object->hits[i]));

    WRITE_U8(object->amount_of_misses);

    WRITE_ARRAY(object->misses, object->amount_of_misses, WWM_CHECK_RETURN_CODE(vanilla_SpellMiss_write(writer, &object->misses[i])));

    WWM_CHECK_RETURN_CODE(vanilla_SpellCastTargets_write(writer, &object->targets));

    if ((object->flags & VANILLA_CAST_FLAGS_AMMO) != 0) {
        WRITE_U32(object->ammo_display_id);

        WRITE_U32(object->ammo_inventory_type);

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_SPELL_GO_free(vanilla_SMSG_SPELL_GO* object) {
    free(object->hits);
    object->hits = NULL;
    free(object->misses);
    object->misses = NULL;
    vanilla_SpellCastTargets_free(&object->targets);
    if ((object->flags & VANILLA_CAST_FLAGS_AMMO) != 0) {
    }
}

static WowWorldResult vanilla_SMSG_SPELL_FAILURE_read(WowWorldReader* reader, vanilla_SMSG_SPELL_FAILURE* object) {
    READ_U64(object->guid);

    READ_U32(object->spell);

    object->result = 0;
    READ_U8(object->result);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPELL_FAILURE_write(WowWorldWriter* writer, const vanilla_SMSG_SPELL_FAILURE* object) {
    WRITE_U16_BE(0x000d + 2); /* size */

    WRITE_U16(0x00000133); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->spell);

    WRITE_U8(object->result);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_SPELL_COOLDOWN_size(const vanilla_SMSG_SPELL_COOLDOWN* object) {
    return 8 + 8 * object->amount_of_cooldowns;
}

static WowWorldResult vanilla_SMSG_SPELL_COOLDOWN_read(WowWorldReader* reader, vanilla_SMSG_SPELL_COOLDOWN* object, size_t body_size) {
    size_t _size = 0;

    READ_U64(object->guid);
    _size += 8;

    object->amount_of_cooldowns = 0;
    /* C89 scope to allow variable declarations */ {
        int i = 0;
        size_t _current_size = 8 * sizeof(*object->cooldowns);

        object->cooldowns = malloc(_current_size);
        while (_size < body_size) {
            WWM_CHECK_RETURN_CODE(vanilla_SpellCooldownStatus_read(reader, &object->cooldowns[i]));
            _size += 8;
            ++i;

            if (i * sizeof(*object->cooldowns) >= _current_size) {
                _current_size *= 2;
                object->cooldowns = realloc(object->cooldowns, _current_size);
            }
        }

        object->amount_of_cooldowns = i;
    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPELL_COOLDOWN_write(WowWorldWriter* writer, const vanilla_SMSG_SPELL_COOLDOWN* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SPELL_COOLDOWN_size(object) + 2); /* size */

    WRITE_U16(0x00000134); /* opcode */

    WRITE_U64(object->guid);

    WRITE_ARRAY(object->cooldowns, object->amount_of_cooldowns, WWM_CHECK_RETURN_CODE(vanilla_SpellCooldownStatus_write(writer, &object->cooldowns[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_SPELL_COOLDOWN_free(vanilla_SMSG_SPELL_COOLDOWN* object) {
    free(object->cooldowns);
    object->cooldowns = NULL;
}

static WowWorldResult vanilla_SMSG_COOLDOWN_EVENT_read(WowWorldReader* reader, vanilla_SMSG_COOLDOWN_EVENT* object) {
    READ_U32(object->id);

    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_COOLDOWN_EVENT_write(WowWorldWriter* writer, const vanilla_SMSG_COOLDOWN_EVENT* object) {
    WRITE_U16_BE(0x000c + 2); /* size */

    WRITE_U16(0x00000135); /* opcode */

    WRITE_U32(object->id);

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_CANCEL_AURA_read(WowWorldReader* reader, vanilla_CMSG_CANCEL_AURA* object) {
    READ_U32(object->id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_CANCEL_AURA_write(WowWorldWriter* writer, const vanilla_CMSG_CANCEL_AURA* object) {
    WRITE_U16_BE(0x0004 + 4); /* size */

    WRITE_U32(0x00000136); /* opcode */

    WRITE_U32(object->id);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_UPDATE_AURA_DURATION_read(WowWorldReader* reader, vanilla_SMSG_UPDATE_AURA_DURATION* object) {
    READ_U8(object->aura_slot);

    READ_U32(object->aura_duration);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_UPDATE_AURA_DURATION_write(WowWorldWriter* writer, const vanilla_SMSG_UPDATE_AURA_DURATION* object) {
    WRITE_U16_BE(0x0005 + 2); /* size */

    WRITE_U16(0x00000137); /* opcode */

    WRITE_U8(object->aura_slot);

    WRITE_U32(object->aura_duration);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_PET_CAST_FAILED_read(WowWorldReader* reader, vanilla_SMSG_PET_CAST_FAILED* object) {
    READ_U32(object->id);

    READ_U8(object->unknown1);

    object->result = 0;
    READ_U8(object->result);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PET_CAST_FAILED_write(WowWorldWriter* writer, const vanilla_SMSG_PET_CAST_FAILED* object) {
    WRITE_U16_BE(0x0006 + 2); /* size */

    WRITE_U16(0x00000138); /* opcode */

    WRITE_U32(object->id);

    WRITE_U8(object->unknown1);

    WRITE_U8(object->result);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_MSG_CHANNEL_START_Server_read(WowWorldReader* reader, vanilla_MSG_CHANNEL_START_Server* object) {
    READ_U32(object->spell);

    READ_U32(object->duration);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_CHANNEL_START_Server_write(WowWorldWriter* writer, const vanilla_MSG_CHANNEL_START_Server* object) {
    WRITE_U16_BE(0x0008 + 2); /* size */

    WRITE_U16(0x00000139); /* opcode */

    WRITE_U32(object->spell);

    WRITE_U32(object->duration);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_MSG_CHANNEL_UPDATE_Server_read(WowWorldReader* reader, vanilla_MSG_CHANNEL_UPDATE_Server* object) {
    READ_U32(object->time);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_CHANNEL_UPDATE_Server_write(WowWorldWriter* writer, const vanilla_MSG_CHANNEL_UPDATE_Server* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x0000013a); /* opcode */

    WRITE_U32(object->time);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_CANCEL_CHANNELLING_read(WowWorldReader* reader, vanilla_CMSG_CANCEL_CHANNELLING* object) {
    READ_U32(object->id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_CANCEL_CHANNELLING_write(WowWorldWriter* writer, const vanilla_CMSG_CANCEL_CHANNELLING* object) {
    WRITE_U16_BE(0x0004 + 4); /* size */

    WRITE_U32(0x0000013b); /* opcode */

    WRITE_U32(object->id);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_AI_REACTION_read(WowWorldReader* reader, vanilla_SMSG_AI_REACTION* object) {
    READ_U64(object->guid);

    object->reaction = 0;
    READ_U32(object->reaction);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_AI_REACTION_write(WowWorldWriter* writer, const vanilla_SMSG_AI_REACTION* object) {
    WRITE_U16_BE(0x000c + 2); /* size */

    WRITE_U16(0x0000013c); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->reaction);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_SET_SELECTION_read(WowWorldReader* reader, vanilla_CMSG_SET_SELECTION* object) {
    READ_U64(object->target);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_SET_SELECTION_write(WowWorldWriter* writer, const vanilla_CMSG_SET_SELECTION* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x0000013d); /* opcode */

    WRITE_U64(object->target);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_SET_TARGET_OBSOLETE_read(WowWorldReader* reader, vanilla_CMSG_SET_TARGET_OBSOLETE* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_SET_TARGET_OBSOLETE_write(WowWorldWriter* writer, const vanilla_CMSG_SET_TARGET_OBSOLETE* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x0000013e); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_ATTACKSWING_read(WowWorldReader* reader, vanilla_CMSG_ATTACKSWING* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_ATTACKSWING_write(WowWorldWriter* writer, const vanilla_CMSG_ATTACKSWING* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x00000141); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_ATTACKSTOP_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x00000142); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_ATTACKSTART_read(WowWorldReader* reader, vanilla_SMSG_ATTACKSTART* object) {
    READ_U64(object->attacker);

    READ_U64(object->victim);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_ATTACKSTART_write(WowWorldWriter* writer, const vanilla_SMSG_ATTACKSTART* object) {
    WRITE_U16_BE(0x0010 + 2); /* size */

    WRITE_U16(0x00000143); /* opcode */

    WRITE_U64(object->attacker);

    WRITE_U64(object->victim);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_ATTACKSTOP_size(const vanilla_SMSG_ATTACKSTOP* object) {
    return 4 + wwm_packed_guid_size(object->player) + wwm_packed_guid_size(object->enemy);
}

static WowWorldResult vanilla_SMSG_ATTACKSTOP_read(WowWorldReader* reader, vanilla_SMSG_ATTACKSTOP* object) {
    READ_PACKED_GUID(object->player);

    READ_PACKED_GUID(object->enemy);

    READ_U32(object->unknown1);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_ATTACKSTOP_write(WowWorldWriter* writer, const vanilla_SMSG_ATTACKSTOP* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_ATTACKSTOP_size(object) + 2); /* size */

    WRITE_U16(0x00000144); /* opcode */

    WRITE_PACKED_GUID(object->player);

    WRITE_PACKED_GUID(object->enemy);

    WRITE_U32(object->unknown1);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_ATTACKSWING_NOTINRANGE_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 2); /* size */

    WRITE_U16(0x00000145); /* opcode */


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_ATTACKSWING_BADFACING_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 2); /* size */

    WRITE_U16(0x00000146); /* opcode */


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_ATTACKSWING_NOTSTANDING_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 2); /* size */

    WRITE_U16(0x00000147); /* opcode */


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_ATTACKSWING_DEADTARGET_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 2); /* size */

    WRITE_U16(0x00000148); /* opcode */


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_ATTACKSWING_CANT_ATTACK_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 2); /* size */

    WRITE_U16(0x00000149); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_ATTACKERSTATEUPDATE_size(const vanilla_SMSG_ATTACKERSTATEUPDATE* object) {
    return 25 + wwm_packed_guid_size(object->attacker) + wwm_packed_guid_size(object->target) + 20 * object->amount_of_damages;
}

static WowWorldResult vanilla_SMSG_ATTACKERSTATEUPDATE_read(WowWorldReader* reader, vanilla_SMSG_ATTACKERSTATEUPDATE* object) {
    object->hit_info = 0;
    READ_U32(object->hit_info);

    READ_PACKED_GUID(object->attacker);

    READ_PACKED_GUID(object->target);

    READ_U32(object->total_damage);

    READ_U8(object->amount_of_damages);

    READ_ARRAY_ALLOCATE(object->damages, object->amount_of_damages, sizeof(vanilla_DamageInfo));
    READ_ARRAY(object->damages, object->amount_of_damages, WWM_CHECK_RETURN_CODE(vanilla_DamageInfo_read(reader, &object->damages[i])));

    READ_U32(object->damage_state);

    READ_U32(object->unknown1);

    READ_U32(object->spell_id);

    READ_U32(object->blocked_amount);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_ATTACKERSTATEUPDATE_write(WowWorldWriter* writer, const vanilla_SMSG_ATTACKERSTATEUPDATE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_ATTACKERSTATEUPDATE_size(object) + 2); /* size */

    WRITE_U16(0x0000014a); /* opcode */

    WRITE_U32(object->hit_info);

    WRITE_PACKED_GUID(object->attacker);

    WRITE_PACKED_GUID(object->target);

    WRITE_U32(object->total_damage);

    WRITE_U8(object->amount_of_damages);

    WRITE_ARRAY(object->damages, object->amount_of_damages, WWM_CHECK_RETURN_CODE(vanilla_DamageInfo_write(writer, &object->damages[i])));

    WRITE_U32(object->damage_state);

    WRITE_U32(object->unknown1);

    WRITE_U32(object->spell_id);

    WRITE_U32(object->blocked_amount);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_ATTACKERSTATEUPDATE_free(vanilla_SMSG_ATTACKERSTATEUPDATE* object) {
    free(object->damages);
    object->damages = NULL;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_CANCEL_COMBAT_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 2); /* size */

    WRITE_U16(0x0000014e); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_SPELLHEALLOG_size(const vanilla_SMSG_SPELLHEALLOG* object) {
    return 9 + wwm_packed_guid_size(object->victim) + wwm_packed_guid_size(object->caster);
}

static WowWorldResult vanilla_SMSG_SPELLHEALLOG_read(WowWorldReader* reader, vanilla_SMSG_SPELLHEALLOG* object) {
    READ_PACKED_GUID(object->victim);

    READ_PACKED_GUID(object->caster);

    READ_U32(object->id);

    READ_U32(object->damage);

    READ_BOOL8(object->critical);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPELLHEALLOG_write(WowWorldWriter* writer, const vanilla_SMSG_SPELLHEALLOG* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SPELLHEALLOG_size(object) + 2); /* size */

    WRITE_U16(0x00000150); /* opcode */

    WRITE_PACKED_GUID(object->victim);

    WRITE_PACKED_GUID(object->caster);

    WRITE_U32(object->id);

    WRITE_U32(object->damage);

    WRITE_BOOL8(object->critical);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_SPELLENERGIZELOG_size(const vanilla_SMSG_SPELLENERGIZELOG* object) {
    return 12 + wwm_packed_guid_size(object->victim) + wwm_packed_guid_size(object->caster);
}

static WowWorldResult vanilla_SMSG_SPELLENERGIZELOG_read(WowWorldReader* reader, vanilla_SMSG_SPELLENERGIZELOG* object) {
    READ_PACKED_GUID(object->victim);

    READ_PACKED_GUID(object->caster);

    READ_U32(object->spell);

    object->power = 0;
    READ_U32(object->power);

    READ_U32(object->damage);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPELLENERGIZELOG_write(WowWorldWriter* writer, const vanilla_SMSG_SPELLENERGIZELOG* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SPELLENERGIZELOG_size(object) + 2); /* size */

    WRITE_U16(0x00000151); /* opcode */

    WRITE_PACKED_GUID(object->victim);

    WRITE_PACKED_GUID(object->caster);

    WRITE_U32(object->spell);

    WRITE_U32(object->power);

    WRITE_U32(object->damage);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_BINDPOINTUPDATE_read(WowWorldReader* reader, vanilla_SMSG_BINDPOINTUPDATE* object) {
    WWM_CHECK_RETURN_CODE(all_Vector3d_read(reader, &object->position));

    object->map = 0;
    READ_U32(object->map);

    object->area = 0;
    READ_U32(object->area);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_BINDPOINTUPDATE_write(WowWorldWriter* writer, const vanilla_SMSG_BINDPOINTUPDATE* object) {
    WRITE_U16_BE(0x0014 + 2); /* size */

    WRITE_U16(0x00000155); /* opcode */

    WWM_CHECK_RETURN_CODE(all_Vector3d_write(writer, &object->position));

    WRITE_U32(object->map);

    WRITE_U32(object->area);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_PLAYERBOUND_read(WowWorldReader* reader, vanilla_SMSG_PLAYERBOUND* object) {
    READ_U64(object->guid);

    object->area = 0;
    READ_U32(object->area);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PLAYERBOUND_write(WowWorldWriter* writer, const vanilla_SMSG_PLAYERBOUND* object) {
    WRITE_U16_BE(0x000c + 2); /* size */

    WRITE_U16(0x00000158); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->area);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_CLIENT_CONTROL_UPDATE_size(const vanilla_SMSG_CLIENT_CONTROL_UPDATE* object) {
    return 1 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_CLIENT_CONTROL_UPDATE_read(WowWorldReader* reader, vanilla_SMSG_CLIENT_CONTROL_UPDATE* object) {
    READ_PACKED_GUID(object->guid);

    READ_BOOL8(object->allow_movement);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_CLIENT_CONTROL_UPDATE_write(WowWorldWriter* writer, const vanilla_SMSG_CLIENT_CONTROL_UPDATE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_CLIENT_CONTROL_UPDATE_size(object) + 2); /* size */

    WRITE_U16(0x00000159); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WRITE_BOOL8(object->allow_movement);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_REPOP_REQUEST_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x0000015a); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_RESURRECT_REQUEST_size(const vanilla_SMSG_RESURRECT_REQUEST* object) {
    return 13 + STRING_SIZE(object->name);
}

static WowWorldResult vanilla_SMSG_RESURRECT_REQUEST_read(WowWorldReader* reader, vanilla_SMSG_RESURRECT_REQUEST* object) {
    READ_U64(object->guid);

    READ_SIZED_CSTRING(object->name);

    READ_BOOL8(object->player);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_RESURRECT_REQUEST_write(WowWorldWriter* writer, const vanilla_SMSG_RESURRECT_REQUEST* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_RESURRECT_REQUEST_size(object) + 2); /* size */

    WRITE_U16(0x0000015b); /* opcode */

    WRITE_U64(object->guid);

    WRITE_SIZED_CSTRING(object->name);

    WRITE_BOOL8(object->player);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_RESURRECT_REQUEST_free(vanilla_SMSG_RESURRECT_REQUEST* object) {
    FREE_STRING(object->name);

}

static WowWorldResult vanilla_CMSG_RESURRECT_RESPONSE_read(WowWorldReader* reader, vanilla_CMSG_RESURRECT_RESPONSE* object) {
    READ_U64(object->guid);

    READ_U8(object->status);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_RESURRECT_RESPONSE_write(WowWorldWriter* writer, const vanilla_CMSG_RESURRECT_RESPONSE* object) {
    WRITE_U16_BE(0x0009 + 4); /* size */

    WRITE_U32(0x0000015c); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U8(object->status);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_LOOT_read(WowWorldReader* reader, vanilla_CMSG_LOOT* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_LOOT_write(WowWorldWriter* writer, const vanilla_CMSG_LOOT* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x0000015d); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_LOOT_MONEY_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x0000015e); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_LOOT_RELEASE_read(WowWorldReader* reader, vanilla_CMSG_LOOT_RELEASE* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_LOOT_RELEASE_write(WowWorldWriter* writer, const vanilla_CMSG_LOOT_RELEASE* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x0000015f); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_LOOT_RESPONSE_size(const vanilla_SMSG_LOOT_RESPONSE* object) {
    size_t _size = 14 + 6 * object->amount_of_items;

    if (object->loot_method == VANILLA_LOOT_METHOD_ERROR) {
        _size += 1;
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_LOOT_RESPONSE_read(WowWorldReader* reader, vanilla_SMSG_LOOT_RESPONSE* object) {
    READ_U64(object->guid);

    object->loot_method = 0;
    READ_U8(object->loot_method);

    if (object->loot_method == VANILLA_LOOT_METHOD_ERROR) {
        object->loot_error = 0;
        READ_U8(object->loot_error);

    }
    READ_U32(object->gold);

    READ_U8(object->amount_of_items);

    READ_ARRAY_ALLOCATE(object->items, object->amount_of_items, sizeof(vanilla_LootItem));
    READ_ARRAY(object->items, object->amount_of_items, WWM_CHECK_RETURN_CODE(vanilla_LootItem_read(reader, &object->items[i])));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_LOOT_RESPONSE_write(WowWorldWriter* writer, const vanilla_SMSG_LOOT_RESPONSE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_LOOT_RESPONSE_size(object) + 2); /* size */

    WRITE_U16(0x00000160); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U8(object->loot_method);

    if (object->loot_method == VANILLA_LOOT_METHOD_ERROR) {
        WRITE_U8(object->loot_error);

    }
    WRITE_U32(object->gold);

    WRITE_U8(object->amount_of_items);

    WRITE_ARRAY(object->items, object->amount_of_items, WWM_CHECK_RETURN_CODE(vanilla_LootItem_write(writer, &object->items[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_LOOT_RESPONSE_free(vanilla_SMSG_LOOT_RESPONSE* object) {
    if (object->loot_method == VANILLA_LOOT_METHOD_ERROR) {
    }
    free(object->items);
    object->items = NULL;
}

static WowWorldResult vanilla_SMSG_LOOT_RELEASE_RESPONSE_read(WowWorldReader* reader, vanilla_SMSG_LOOT_RELEASE_RESPONSE* object) {
    READ_U64(object->guid);

    READ_U8(object->unknown1);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_LOOT_RELEASE_RESPONSE_write(WowWorldWriter* writer, const vanilla_SMSG_LOOT_RELEASE_RESPONSE* object) {
    WRITE_U16_BE(0x0009 + 2); /* size */

    WRITE_U16(0x00000161); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U8(object->unknown1);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_LOOT_REMOVED_read(WowWorldReader* reader, vanilla_SMSG_LOOT_REMOVED* object) {
    READ_U8(object->slot);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_LOOT_REMOVED_write(WowWorldWriter* writer, const vanilla_SMSG_LOOT_REMOVED* object) {
    WRITE_U16_BE(0x0001 + 2); /* size */

    WRITE_U16(0x00000162); /* opcode */

    WRITE_U8(object->slot);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_LOOT_MONEY_NOTIFY_read(WowWorldReader* reader, vanilla_SMSG_LOOT_MONEY_NOTIFY* object) {
    READ_U32(object->amount);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_LOOT_MONEY_NOTIFY_write(WowWorldWriter* writer, const vanilla_SMSG_LOOT_MONEY_NOTIFY* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x00000163); /* opcode */

    WRITE_U32(object->amount);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_LOOT_CLEAR_MONEY_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 2); /* size */

    WRITE_U16(0x00000165); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_ITEM_PUSH_RESULT_read(WowWorldReader* reader, vanilla_SMSG_ITEM_PUSH_RESULT* object) {
    READ_U64(object->guid);

    object->source = 0;
    READ_U32(object->source);

    object->creation_type = 0;
    READ_U32(object->creation_type);

    object->alert_chat = 0;
    READ_U32(object->alert_chat);

    READ_U8(object->bag_slot);

    READ_U32(object->item_slot);

    READ_U32(object->item);

    READ_U32(object->item_suffix_factor);

    READ_U32(object->item_random_property_id);

    READ_U32(object->item_count);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_ITEM_PUSH_RESULT_write(WowWorldWriter* writer, const vanilla_SMSG_ITEM_PUSH_RESULT* object) {
    WRITE_U16_BE(0x0029 + 2); /* size */

    WRITE_U16(0x00000166); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->source);

    WRITE_U32(object->creation_type);

    WRITE_U32(object->alert_chat);

    WRITE_U8(object->bag_slot);

    WRITE_U32(object->item_slot);

    WRITE_U32(object->item);

    WRITE_U32(object->item_suffix_factor);

    WRITE_U32(object->item_random_property_id);

    WRITE_U32(object->item_count);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_DUEL_REQUESTED_read(WowWorldReader* reader, vanilla_SMSG_DUEL_REQUESTED* object) {
    READ_U64(object->initiator);

    READ_U64(object->target);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_DUEL_REQUESTED_write(WowWorldWriter* writer, const vanilla_SMSG_DUEL_REQUESTED* object) {
    WRITE_U16_BE(0x0010 + 2); /* size */

    WRITE_U16(0x00000167); /* opcode */

    WRITE_U64(object->initiator);

    WRITE_U64(object->target);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_DUEL_OUTOFBOUNDS_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 2); /* size */

    WRITE_U16(0x00000168); /* opcode */


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_DUEL_INBOUNDS_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 2); /* size */

    WRITE_U16(0x00000169); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_DUEL_COMPLETE_read(WowWorldReader* reader, vanilla_SMSG_DUEL_COMPLETE* object) {
    READ_BOOL8(object->ended_without_interruption);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_DUEL_COMPLETE_write(WowWorldWriter* writer, const vanilla_SMSG_DUEL_COMPLETE* object) {
    WRITE_U16_BE(0x0001 + 2); /* size */

    WRITE_U16(0x0000016a); /* opcode */

    WRITE_BOOL8(object->ended_without_interruption);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_DUEL_WINNER_size(const vanilla_SMSG_DUEL_WINNER* object) {
    return 3 + STRING_SIZE(object->opponent_name) + STRING_SIZE(object->initiator_name);
}

static WowWorldResult vanilla_SMSG_DUEL_WINNER_read(WowWorldReader* reader, vanilla_SMSG_DUEL_WINNER* object) {
    object->reason = 0;
    READ_U8(object->reason);

    READ_CSTRING(object->opponent_name);

    READ_CSTRING(object->initiator_name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_DUEL_WINNER_write(WowWorldWriter* writer, const vanilla_SMSG_DUEL_WINNER* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_DUEL_WINNER_size(object) + 2); /* size */

    WRITE_U16(0x0000016b); /* opcode */

    WRITE_U8(object->reason);

    WRITE_CSTRING(object->opponent_name);

    WRITE_CSTRING(object->initiator_name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_DUEL_WINNER_free(vanilla_SMSG_DUEL_WINNER* object) {
    FREE_STRING(object->opponent_name);

    FREE_STRING(object->initiator_name);

}

static WowWorldResult vanilla_CMSG_DUEL_ACCEPTED_read(WowWorldReader* reader, vanilla_CMSG_DUEL_ACCEPTED* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_DUEL_ACCEPTED_write(WowWorldWriter* writer, const vanilla_CMSG_DUEL_ACCEPTED* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x0000016c); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_DUEL_CANCELLED_read(WowWorldReader* reader, vanilla_CMSG_DUEL_CANCELLED* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_DUEL_CANCELLED_write(WowWorldWriter* writer, const vanilla_CMSG_DUEL_CANCELLED* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x0000016d); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_MOUNTRESULT_read(WowWorldReader* reader, vanilla_SMSG_MOUNTRESULT* object) {
    object->result = 0;
    READ_U32(object->result);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_MOUNTRESULT_write(WowWorldWriter* writer, const vanilla_SMSG_MOUNTRESULT* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x0000016e); /* opcode */

    WRITE_U32(object->result);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_DISMOUNTRESULT_read(WowWorldReader* reader, vanilla_SMSG_DISMOUNTRESULT* object) {
    object->result = 0;
    READ_U32(object->result);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_DISMOUNTRESULT_write(WowWorldWriter* writer, const vanilla_SMSG_DISMOUNTRESULT* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x0000016f); /* opcode */

    WRITE_U32(object->result);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_MOUNTSPECIAL_ANIM_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x00000171); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_MOUNTSPECIAL_ANIM_read(WowWorldReader* reader, vanilla_SMSG_MOUNTSPECIAL_ANIM* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_MOUNTSPECIAL_ANIM_write(WowWorldWriter* writer, const vanilla_SMSG_MOUNTSPECIAL_ANIM* object) {
    WRITE_U16_BE(0x0008 + 2); /* size */

    WRITE_U16(0x00000172); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_PET_TAME_FAILURE_read(WowWorldReader* reader, vanilla_SMSG_PET_TAME_FAILURE* object) {
    object->reason = 0;
    READ_U8(object->reason);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PET_TAME_FAILURE_write(WowWorldWriter* writer, const vanilla_SMSG_PET_TAME_FAILURE* object) {
    WRITE_U16_BE(0x0001 + 2); /* size */

    WRITE_U16(0x00000173); /* opcode */

    WRITE_U8(object->reason);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_PET_SET_ACTION_size(const vanilla_CMSG_PET_SET_ACTION* object) {
    size_t _size = 16;

    if(object->extra) {
        _size += 8;
    }

    return _size;
}

static WowWorldResult vanilla_CMSG_PET_SET_ACTION_read(WowWorldReader* reader, vanilla_CMSG_PET_SET_ACTION* object, size_t body_size) {
    size_t _size = 0;

    READ_U64(object->guid);
    _size += 8;

    READ_U32(object->position1);
    _size += 4;

    READ_U32(object->data1);
    _size += 4;

    object->extra = NULL;
    if (_size < body_size) {
        object->extra = malloc(sizeof(vanilla_CMSG_PET_SET_ACTION_extra));

        READ_U32(object->extra->position2);

        READ_U32(object->extra->data2);

    }
    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_PET_SET_ACTION_write(WowWorldWriter* writer, const vanilla_CMSG_PET_SET_ACTION* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_PET_SET_ACTION_size(object) + 4); /* size */

    WRITE_U32(0x00000174); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->position1);

    WRITE_U32(object->data1);

    if(object->extra) {
        WRITE_U32(object->extra->position2);

        WRITE_U32(object->extra->data2);

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_PET_SET_ACTION_free(vanilla_CMSG_PET_SET_ACTION* object) {
    free(object->extra);
}

static WowWorldResult vanilla_CMSG_PET_ACTION_read(WowWorldReader* reader, vanilla_CMSG_PET_ACTION* object) {
    READ_U64(object->pet);

    READ_U32(object->data);

    READ_U64(object->target);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_PET_ACTION_write(WowWorldWriter* writer, const vanilla_CMSG_PET_ACTION* object) {
    WRITE_U16_BE(0x0014 + 4); /* size */

    WRITE_U32(0x00000175); /* opcode */

    WRITE_U64(object->pet);

    WRITE_U32(object->data);

    WRITE_U64(object->target);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_PET_ABANDON_read(WowWorldReader* reader, vanilla_CMSG_PET_ABANDON* object) {
    READ_U64(object->pet);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_PET_ABANDON_write(WowWorldWriter* writer, const vanilla_CMSG_PET_ABANDON* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x00000176); /* opcode */

    WRITE_U64(object->pet);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_PET_RENAME_size(const vanilla_CMSG_PET_RENAME* object) {
    return 9 + STRING_SIZE(object->name);
}

static WowWorldResult vanilla_CMSG_PET_RENAME_read(WowWorldReader* reader, vanilla_CMSG_PET_RENAME* object) {
    READ_U64(object->pet);

    READ_CSTRING(object->name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_PET_RENAME_write(WowWorldWriter* writer, const vanilla_CMSG_PET_RENAME* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_PET_RENAME_size(object) + 4); /* size */

    WRITE_U32(0x00000177); /* opcode */

    WRITE_U64(object->pet);

    WRITE_CSTRING(object->name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_PET_RENAME_free(vanilla_CMSG_PET_RENAME* object) {
    FREE_STRING(object->name);

}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PET_NAME_INVALID_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 2); /* size */

    WRITE_U16(0x00000178); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_PET_SPELLS_size(const vanilla_SMSG_PET_SPELLS* object) {
    size_t _size = 8;

    if(object->action_bars) {
        _size += 50 + 4 * object->action_bars->amount_of_spells + 12 * object->action_bars->amount_of_cooldowns;
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_PET_SPELLS_read(WowWorldReader* reader, vanilla_SMSG_PET_SPELLS* object, size_t body_size) {
    size_t _size = 0;

    READ_U64(object->pet);
    _size += 8;

    object->action_bars = NULL;
    if (_size < body_size) {
        object->action_bars = malloc(sizeof(vanilla_SMSG_PET_SPELLS_action_bars));

        READ_U32(object->action_bars->duration);

        object->action_bars->react = 0;
        READ_U8(object->action_bars->react);

        object->action_bars->command = 0;
        READ_U8(object->action_bars->command);

        READ_U8(object->action_bars->unknown);

        object->action_bars->pet_enabled = 0;
        READ_U8(object->action_bars->pet_enabled);

        READ_ARRAY_ALLOCATE(object->action_bars->action_bars, 10, sizeof(uint32_t));
        READ_ARRAY(object->action_bars->action_bars, 10, READ_U32((*object->action_bars->action_bars)[i]));

        READ_U8(object->action_bars->amount_of_spells);

        READ_ARRAY_ALLOCATE(object->action_bars->spells, object->action_bars->amount_of_spells, sizeof(uint32_t));
        READ_ARRAY(object->action_bars->spells, object->action_bars->amount_of_spells, READ_U32(object->action_bars->spells[i]));

        READ_U8(object->action_bars->amount_of_cooldowns);

        READ_ARRAY_ALLOCATE(object->action_bars->cooldowns, object->action_bars->amount_of_cooldowns, sizeof(vanilla_PetSpellCooldown));
        READ_ARRAY(object->action_bars->cooldowns, object->action_bars->amount_of_cooldowns, WWM_CHECK_RETURN_CODE(vanilla_PetSpellCooldown_read(reader, &object->action_bars->cooldowns[i])));

    }
    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PET_SPELLS_write(WowWorldWriter* writer, const vanilla_SMSG_PET_SPELLS* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_PET_SPELLS_size(object) + 2); /* size */

    WRITE_U16(0x00000179); /* opcode */

    WRITE_U64(object->pet);

    if(object->action_bars) {
        WRITE_U32(object->action_bars->duration);

        WRITE_U8(object->action_bars->react);

        WRITE_U8(object->action_bars->command);

        WRITE_U8(object->action_bars->unknown);

        WRITE_U8(object->action_bars->pet_enabled);

        WRITE_ARRAY(object->action_bars->action_bars, 10, WRITE_U32((*object->action_bars->action_bars)[i]));

        WRITE_U8(object->action_bars->amount_of_spells);

        WRITE_ARRAY(object->action_bars->spells, object->action_bars->amount_of_spells, WRITE_U32(object->action_bars->spells[i]));

        WRITE_U8(object->action_bars->amount_of_cooldowns);

        WRITE_ARRAY(object->action_bars->cooldowns, object->action_bars->amount_of_cooldowns, WWM_CHECK_RETURN_CODE(vanilla_PetSpellCooldown_write(writer, &object->action_bars->cooldowns[i])));

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_PET_SPELLS_free(vanilla_SMSG_PET_SPELLS* object) {
    free(object->action_bars->action_bars);
    object->action_bars->action_bars = NULL;
    free(object->action_bars->spells);
    object->action_bars->spells = NULL;
    free(object->action_bars->cooldowns);
    object->action_bars->cooldowns = NULL;
    free(object->action_bars);
}

static WowWorldResult vanilla_SMSG_PET_MODE_read(WowWorldReader* reader, vanilla_SMSG_PET_MODE* object) {
    READ_U64(object->guid);

    object->react_state = 0;
    READ_U8(object->react_state);

    object->command_state = 0;
    READ_U8(object->command_state);

    READ_U8(object->unknown1);

    object->pet_enabled = 0;
    READ_U8(object->pet_enabled);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PET_MODE_write(WowWorldWriter* writer, const vanilla_SMSG_PET_MODE* object) {
    WRITE_U16_BE(0x000c + 2); /* size */

    WRITE_U16(0x0000017a); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U8(object->react_state);

    WRITE_U8(object->command_state);

    WRITE_U8(object->unknown1);

    WRITE_U8(object->pet_enabled);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_GOSSIP_HELLO_read(WowWorldReader* reader, vanilla_CMSG_GOSSIP_HELLO* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GOSSIP_HELLO_write(WowWorldWriter* writer, const vanilla_CMSG_GOSSIP_HELLO* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x0000017b); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_GOSSIP_SELECT_OPTION_size(const vanilla_CMSG_GOSSIP_SELECT_OPTION* object) {
    size_t _size = 12;

    if(object->unknown) {
        _size += 1 + STRING_SIZE(object->unknown->code);
    }

    return _size;
}

static WowWorldResult vanilla_CMSG_GOSSIP_SELECT_OPTION_read(WowWorldReader* reader, vanilla_CMSG_GOSSIP_SELECT_OPTION* object, size_t body_size) {
    size_t _size = 0;

    READ_U64(object->guid);
    _size += 8;

    READ_U32(object->gossip_list_id);
    _size += 4;

    object->unknown = NULL;
    if (_size < body_size) {
        object->unknown = malloc(sizeof(vanilla_CMSG_GOSSIP_SELECT_OPTION_unknown));

        READ_CSTRING(object->unknown->code);

    }
    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GOSSIP_SELECT_OPTION_write(WowWorldWriter* writer, const vanilla_CMSG_GOSSIP_SELECT_OPTION* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_GOSSIP_SELECT_OPTION_size(object) + 4); /* size */

    WRITE_U32(0x0000017c); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->gossip_list_id);

    if(object->unknown) {
        WRITE_CSTRING(object->unknown->code);

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_GOSSIP_SELECT_OPTION_free(vanilla_CMSG_GOSSIP_SELECT_OPTION* object) {
    FREE_STRING(object->unknown->code);

    free(object->unknown);
}

static size_t vanilla_SMSG_GOSSIP_MESSAGE_size(const vanilla_SMSG_GOSSIP_MESSAGE* object) {
    size_t _size = 20;

    /* C89 scope to allow variable declarations */ {
        int i;
        for(i = 0; i < (int)object->amount_of_gossip_items; ++i) {
            _size += vanilla_GossipItem_size(&object->gossips[i]);
        }
    }

    /* C89 scope to allow variable declarations */ {
        int i;
        for(i = 0; i < (int)object->amount_of_quests; ++i) {
            _size += vanilla_QuestItem_size(&object->quests[i]);
        }
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_GOSSIP_MESSAGE_read(WowWorldReader* reader, vanilla_SMSG_GOSSIP_MESSAGE* object) {
    READ_U64(object->guid);

    READ_U32(object->title_text_id);

    READ_U32(object->amount_of_gossip_items);

    READ_ARRAY_ALLOCATE(object->gossips, object->amount_of_gossip_items, sizeof(vanilla_GossipItem));
    READ_ARRAY(object->gossips, object->amount_of_gossip_items, WWM_CHECK_RETURN_CODE(vanilla_GossipItem_read(reader, &object->gossips[i])));

    READ_U32(object->amount_of_quests);

    READ_ARRAY_ALLOCATE(object->quests, object->amount_of_quests, sizeof(vanilla_QuestItem));
    READ_ARRAY(object->quests, object->amount_of_quests, WWM_CHECK_RETURN_CODE(vanilla_QuestItem_read(reader, &object->quests[i])));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_GOSSIP_MESSAGE_write(WowWorldWriter* writer, const vanilla_SMSG_GOSSIP_MESSAGE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_GOSSIP_MESSAGE_size(object) + 2); /* size */

    WRITE_U16(0x0000017d); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->title_text_id);

    WRITE_U32(object->amount_of_gossip_items);

    WRITE_ARRAY(object->gossips, object->amount_of_gossip_items, WWM_CHECK_RETURN_CODE(vanilla_GossipItem_write(writer, &object->gossips[i])));

    WRITE_U32(object->amount_of_quests);

    WRITE_ARRAY(object->quests, object->amount_of_quests, WWM_CHECK_RETURN_CODE(vanilla_QuestItem_write(writer, &object->quests[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_GOSSIP_MESSAGE_free(vanilla_SMSG_GOSSIP_MESSAGE* object) {
    size_t i;

    for (i = 0; i < object->amount_of_gossip_items; ++i) {
        vanilla_GossipItem_free(&((object->gossips)[i]));
    }
    free(object->gossips);
    object->gossips = NULL;
    for (i = 0; i < object->amount_of_quests; ++i) {
        vanilla_QuestItem_free(&((object->quests)[i]));
    }
    free(object->quests);
    object->quests = NULL;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_GOSSIP_COMPLETE_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 2); /* size */

    WRITE_U16(0x0000017e); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_NPC_TEXT_QUERY_read(WowWorldReader* reader, vanilla_CMSG_NPC_TEXT_QUERY* object) {
    READ_U32(object->text_id);

    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_NPC_TEXT_QUERY_write(WowWorldWriter* writer, const vanilla_CMSG_NPC_TEXT_QUERY* object) {
    WRITE_U16_BE(0x000c + 4); /* size */

    WRITE_U32(0x0000017f); /* opcode */

    WRITE_U32(object->text_id);

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_NPC_TEXT_UPDATE_size(const vanilla_SMSG_NPC_TEXT_UPDATE* object) {
    size_t _size = 4;

    /* C89 scope to allow variable declarations */ {
        int i;
        for(i = 0; i < 8; ++i) {
            _size += vanilla_NpcTextUpdate_size(&(*object->texts[i]));
        }
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_NPC_TEXT_UPDATE_read(WowWorldReader* reader, vanilla_SMSG_NPC_TEXT_UPDATE* object) {
    READ_U32(object->text_id);

    READ_ARRAY_ALLOCATE(object->texts, 8, sizeof(vanilla_NpcTextUpdate));
    READ_ARRAY(object->texts, 8, WWM_CHECK_RETURN_CODE(vanilla_NpcTextUpdate_read(reader, &(*object->texts)[i])));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_NPC_TEXT_UPDATE_write(WowWorldWriter* writer, const vanilla_SMSG_NPC_TEXT_UPDATE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_NPC_TEXT_UPDATE_size(object) + 2); /* size */

    WRITE_U16(0x00000180); /* opcode */

    WRITE_U32(object->text_id);

    WRITE_ARRAY(object->texts, 8, WWM_CHECK_RETURN_CODE(vanilla_NpcTextUpdate_write(writer, &(*object->texts)[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_NPC_TEXT_UPDATE_free(vanilla_SMSG_NPC_TEXT_UPDATE* object) {
    size_t i;

    for (i = 0; i < 8; ++i) {
        vanilla_NpcTextUpdate_free(&((*object->texts)[i]));
    }
    free(object->texts);
    object->texts = NULL;
}

static WowWorldResult vanilla_CMSG_QUESTGIVER_STATUS_QUERY_read(WowWorldReader* reader, vanilla_CMSG_QUESTGIVER_STATUS_QUERY* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_QUESTGIVER_STATUS_QUERY_write(WowWorldWriter* writer, const vanilla_CMSG_QUESTGIVER_STATUS_QUERY* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x00000182); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_QUESTGIVER_STATUS_read(WowWorldReader* reader, vanilla_SMSG_QUESTGIVER_STATUS* object) {
    READ_U64(object->guid);

    object->status = 0;
    READ_U32(object->status);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_QUESTGIVER_STATUS_write(WowWorldWriter* writer, const vanilla_SMSG_QUESTGIVER_STATUS* object) {
    WRITE_U16_BE(0x000c + 2); /* size */

    WRITE_U16(0x00000183); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->status);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_QUESTGIVER_HELLO_read(WowWorldReader* reader, vanilla_CMSG_QUESTGIVER_HELLO* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_QUESTGIVER_HELLO_write(WowWorldWriter* writer, const vanilla_CMSG_QUESTGIVER_HELLO* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x00000184); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_QUESTGIVER_QUEST_LIST_size(const vanilla_SMSG_QUESTGIVER_QUEST_LIST* object) {
    size_t _size = 18 + STRING_SIZE(object->title);

    /* C89 scope to allow variable declarations */ {
        int i;
        for(i = 0; i < (int)object->amount_of_entries; ++i) {
            _size += vanilla_QuestItem_size(&object->quest_items[i]);
        }
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_QUESTGIVER_QUEST_LIST_read(WowWorldReader* reader, vanilla_SMSG_QUESTGIVER_QUEST_LIST* object) {
    READ_U64(object->npc);

    READ_CSTRING(object->title);

    READ_U32(object->emote_delay);

    READ_U32(object->emote);

    READ_U8(object->amount_of_entries);

    READ_ARRAY_ALLOCATE(object->quest_items, object->amount_of_entries, sizeof(vanilla_QuestItem));
    READ_ARRAY(object->quest_items, object->amount_of_entries, WWM_CHECK_RETURN_CODE(vanilla_QuestItem_read(reader, &object->quest_items[i])));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_QUESTGIVER_QUEST_LIST_write(WowWorldWriter* writer, const vanilla_SMSG_QUESTGIVER_QUEST_LIST* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_QUESTGIVER_QUEST_LIST_size(object) + 2); /* size */

    WRITE_U16(0x00000185); /* opcode */

    WRITE_U64(object->npc);

    WRITE_CSTRING(object->title);

    WRITE_U32(object->emote_delay);

    WRITE_U32(object->emote);

    WRITE_U8(object->amount_of_entries);

    WRITE_ARRAY(object->quest_items, object->amount_of_entries, WWM_CHECK_RETURN_CODE(vanilla_QuestItem_write(writer, &object->quest_items[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_QUESTGIVER_QUEST_LIST_free(vanilla_SMSG_QUESTGIVER_QUEST_LIST* object) {
    size_t i;

    FREE_STRING(object->title);

    for (i = 0; i < object->amount_of_entries; ++i) {
        vanilla_QuestItem_free(&((object->quest_items)[i]));
    }
    free(object->quest_items);
    object->quest_items = NULL;
}

static WowWorldResult vanilla_CMSG_QUESTGIVER_QUERY_QUEST_read(WowWorldReader* reader, vanilla_CMSG_QUESTGIVER_QUERY_QUEST* object) {
    READ_U64(object->guid);

    READ_U32(object->quest_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_QUESTGIVER_QUERY_QUEST_write(WowWorldWriter* writer, const vanilla_CMSG_QUESTGIVER_QUERY_QUEST* object) {
    WRITE_U16_BE(0x000c + 4); /* size */

    WRITE_U32(0x00000186); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->quest_id);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_QUESTGIVER_QUEST_AUTOLAUNCH_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x00000187); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_QUESTGIVER_QUEST_DETAILS_size(const vanilla_SMSG_QUESTGIVER_QUEST_DETAILS* object) {
    return 39 + STRING_SIZE(object->title) + STRING_SIZE(object->details) + STRING_SIZE(object->objectives) + 8 * object->amount_of_choice_item_rewards + 8 * object->amount_of_item_rewards + 8 * object->amount_of_emotes;
}

static WowWorldResult vanilla_SMSG_QUESTGIVER_QUEST_DETAILS_read(WowWorldReader* reader, vanilla_SMSG_QUESTGIVER_QUEST_DETAILS* object) {
    READ_U64(object->guid);

    READ_U32(object->quest_id);

    READ_CSTRING(object->title);

    READ_CSTRING(object->details);

    READ_CSTRING(object->objectives);

    READ_BOOL32(object->auto_finish);

    READ_U32(object->amount_of_choice_item_rewards);

    READ_ARRAY_ALLOCATE(object->choice_item_rewards, object->amount_of_choice_item_rewards, sizeof(vanilla_QuestItemReward));
    READ_ARRAY(object->choice_item_rewards, object->amount_of_choice_item_rewards, WWM_CHECK_RETURN_CODE(vanilla_QuestItemReward_read(reader, &object->choice_item_rewards[i])));

    READ_U32(object->amount_of_item_rewards);

    READ_ARRAY_ALLOCATE(object->item_rewards, object->amount_of_item_rewards, sizeof(vanilla_QuestItemReward));
    READ_ARRAY(object->item_rewards, object->amount_of_item_rewards, WWM_CHECK_RETURN_CODE(vanilla_QuestItemReward_read(reader, &object->item_rewards[i])));

    READ_U32(object->money_reward);

    READ_U32(object->reward_spell);

    READ_U32(object->amount_of_emotes);

    READ_ARRAY_ALLOCATE(object->emotes, object->amount_of_emotes, sizeof(vanilla_QuestDetailsEmote));
    READ_ARRAY(object->emotes, object->amount_of_emotes, WWM_CHECK_RETURN_CODE(vanilla_QuestDetailsEmote_read(reader, &object->emotes[i])));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_QUESTGIVER_QUEST_DETAILS_write(WowWorldWriter* writer, const vanilla_SMSG_QUESTGIVER_QUEST_DETAILS* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_QUESTGIVER_QUEST_DETAILS_size(object) + 2); /* size */

    WRITE_U16(0x00000188); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->quest_id);

    WRITE_CSTRING(object->title);

    WRITE_CSTRING(object->details);

    WRITE_CSTRING(object->objectives);

    WRITE_BOOL32(object->auto_finish);

    WRITE_U32(object->amount_of_choice_item_rewards);

    WRITE_ARRAY(object->choice_item_rewards, object->amount_of_choice_item_rewards, WWM_CHECK_RETURN_CODE(vanilla_QuestItemReward_write(writer, &object->choice_item_rewards[i])));

    WRITE_U32(object->amount_of_item_rewards);

    WRITE_ARRAY(object->item_rewards, object->amount_of_item_rewards, WWM_CHECK_RETURN_CODE(vanilla_QuestItemReward_write(writer, &object->item_rewards[i])));

    WRITE_U32(object->money_reward);

    WRITE_U32(object->reward_spell);

    WRITE_U32(object->amount_of_emotes);

    WRITE_ARRAY(object->emotes, object->amount_of_emotes, WWM_CHECK_RETURN_CODE(vanilla_QuestDetailsEmote_write(writer, &object->emotes[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_QUESTGIVER_QUEST_DETAILS_free(vanilla_SMSG_QUESTGIVER_QUEST_DETAILS* object) {
    FREE_STRING(object->title);

    FREE_STRING(object->details);

    FREE_STRING(object->objectives);

    free(object->choice_item_rewards);
    object->choice_item_rewards = NULL;
    free(object->item_rewards);
    object->item_rewards = NULL;
    free(object->emotes);
    object->emotes = NULL;
}

static WowWorldResult vanilla_CMSG_QUESTGIVER_ACCEPT_QUEST_read(WowWorldReader* reader, vanilla_CMSG_QUESTGIVER_ACCEPT_QUEST* object) {
    READ_U64(object->guid);

    READ_U32(object->quest_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_QUESTGIVER_ACCEPT_QUEST_write(WowWorldWriter* writer, const vanilla_CMSG_QUESTGIVER_ACCEPT_QUEST* object) {
    WRITE_U16_BE(0x000c + 4); /* size */

    WRITE_U32(0x00000189); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->quest_id);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_QUESTGIVER_COMPLETE_QUEST_read(WowWorldReader* reader, vanilla_CMSG_QUESTGIVER_COMPLETE_QUEST* object) {
    READ_U64(object->guid);

    READ_U32(object->quest_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_QUESTGIVER_COMPLETE_QUEST_write(WowWorldWriter* writer, const vanilla_CMSG_QUESTGIVER_COMPLETE_QUEST* object) {
    WRITE_U16_BE(0x000c + 4); /* size */

    WRITE_U32(0x0000018a); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->quest_id);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_QUESTGIVER_REQUEST_ITEMS_size(const vanilla_SMSG_QUESTGIVER_REQUEST_ITEMS* object) {
    return 50 + STRING_SIZE(object->title) + STRING_SIZE(object->request_items_text) + 12 * object->amount_of_required_items;
}

static WowWorldResult vanilla_SMSG_QUESTGIVER_REQUEST_ITEMS_read(WowWorldReader* reader, vanilla_SMSG_QUESTGIVER_REQUEST_ITEMS* object) {
    READ_U64(object->npc);

    READ_U32(object->quest_id);

    READ_CSTRING(object->title);

    READ_CSTRING(object->request_items_text);

    READ_U32(object->emote_delay);

    READ_U32(object->emote);

    READ_BOOL32(object->auto_finish);

    READ_U32(object->required_money);

    READ_U32(object->amount_of_required_items);

    READ_ARRAY_ALLOCATE(object->required_items, object->amount_of_required_items, sizeof(vanilla_QuestItemRequirement));
    READ_ARRAY(object->required_items, object->amount_of_required_items, WWM_CHECK_RETURN_CODE(vanilla_QuestItemRequirement_read(reader, &object->required_items[i])));

    READ_U32(object->unknown1);

    object->completable = 0;
    READ_U32(object->completable);

    READ_U32(object->flags2);

    READ_U32(object->flags3);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_QUESTGIVER_REQUEST_ITEMS_write(WowWorldWriter* writer, const vanilla_SMSG_QUESTGIVER_REQUEST_ITEMS* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_QUESTGIVER_REQUEST_ITEMS_size(object) + 2); /* size */

    WRITE_U16(0x0000018b); /* opcode */

    WRITE_U64(object->npc);

    WRITE_U32(object->quest_id);

    WRITE_CSTRING(object->title);

    WRITE_CSTRING(object->request_items_text);

    WRITE_U32(object->emote_delay);

    WRITE_U32(object->emote);

    WRITE_BOOL32(object->auto_finish);

    WRITE_U32(object->required_money);

    WRITE_U32(object->amount_of_required_items);

    WRITE_ARRAY(object->required_items, object->amount_of_required_items, WWM_CHECK_RETURN_CODE(vanilla_QuestItemRequirement_write(writer, &object->required_items[i])));

    WRITE_U32(object->unknown1);

    WRITE_U32(object->completable);

    WRITE_U32(object->flags2);

    WRITE_U32(object->flags3);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_QUESTGIVER_REQUEST_ITEMS_free(vanilla_SMSG_QUESTGIVER_REQUEST_ITEMS* object) {
    FREE_STRING(object->title);

    FREE_STRING(object->request_items_text);

    free(object->required_items);
    object->required_items = NULL;
}

static WowWorldResult vanilla_CMSG_QUESTGIVER_REQUEST_REWARD_read(WowWorldReader* reader, vanilla_CMSG_QUESTGIVER_REQUEST_REWARD* object) {
    READ_U64(object->guid);

    READ_U32(object->quest_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_QUESTGIVER_REQUEST_REWARD_write(WowWorldWriter* writer, const vanilla_CMSG_QUESTGIVER_REQUEST_REWARD* object) {
    WRITE_U16_BE(0x000c + 4); /* size */

    WRITE_U32(0x0000018c); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->quest_id);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_QUESTGIVER_OFFER_REWARD_size(const vanilla_SMSG_QUESTGIVER_OFFER_REWARD* object) {
    return 42 + STRING_SIZE(object->title) + STRING_SIZE(object->offer_reward_text) + 8 * object->amount_of_emotes + 12 * object->amount_of_choice_item_rewards + 12 * object->amount_of_item_rewards;
}

static WowWorldResult vanilla_SMSG_QUESTGIVER_OFFER_REWARD_read(WowWorldReader* reader, vanilla_SMSG_QUESTGIVER_OFFER_REWARD* object) {
    READ_U64(object->npc);

    READ_U32(object->quest_id);

    READ_CSTRING(object->title);

    READ_CSTRING(object->offer_reward_text);

    READ_BOOL32(object->auto_finish);

    READ_U32(object->amount_of_emotes);

    READ_ARRAY_ALLOCATE(object->emotes, object->amount_of_emotes, sizeof(vanilla_NpcTextUpdateEmote));
    READ_ARRAY(object->emotes, object->amount_of_emotes, WWM_CHECK_RETURN_CODE(vanilla_NpcTextUpdateEmote_read(reader, &object->emotes[i])));

    READ_U32(object->amount_of_choice_item_rewards);

    READ_ARRAY_ALLOCATE(object->choice_item_rewards, object->amount_of_choice_item_rewards, sizeof(vanilla_QuestItemRequirement));
    READ_ARRAY(object->choice_item_rewards, object->amount_of_choice_item_rewards, WWM_CHECK_RETURN_CODE(vanilla_QuestItemRequirement_read(reader, &object->choice_item_rewards[i])));

    READ_U32(object->amount_of_item_rewards);

    READ_ARRAY_ALLOCATE(object->item_rewards, object->amount_of_item_rewards, sizeof(vanilla_QuestItemRequirement));
    READ_ARRAY(object->item_rewards, object->amount_of_item_rewards, WWM_CHECK_RETURN_CODE(vanilla_QuestItemRequirement_read(reader, &object->item_rewards[i])));

    READ_U32(object->money_reward);

    READ_U32(object->reward_spell);

    READ_U32(object->reward_spell_cast);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_QUESTGIVER_OFFER_REWARD_write(WowWorldWriter* writer, const vanilla_SMSG_QUESTGIVER_OFFER_REWARD* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_QUESTGIVER_OFFER_REWARD_size(object) + 2); /* size */

    WRITE_U16(0x0000018d); /* opcode */

    WRITE_U64(object->npc);

    WRITE_U32(object->quest_id);

    WRITE_CSTRING(object->title);

    WRITE_CSTRING(object->offer_reward_text);

    WRITE_BOOL32(object->auto_finish);

    WRITE_U32(object->amount_of_emotes);

    WRITE_ARRAY(object->emotes, object->amount_of_emotes, WWM_CHECK_RETURN_CODE(vanilla_NpcTextUpdateEmote_write(writer, &object->emotes[i])));

    WRITE_U32(object->amount_of_choice_item_rewards);

    WRITE_ARRAY(object->choice_item_rewards, object->amount_of_choice_item_rewards, WWM_CHECK_RETURN_CODE(vanilla_QuestItemRequirement_write(writer, &object->choice_item_rewards[i])));

    WRITE_U32(object->amount_of_item_rewards);

    WRITE_ARRAY(object->item_rewards, object->amount_of_item_rewards, WWM_CHECK_RETURN_CODE(vanilla_QuestItemRequirement_write(writer, &object->item_rewards[i])));

    WRITE_U32(object->money_reward);

    WRITE_U32(object->reward_spell);

    WRITE_U32(object->reward_spell_cast);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_QUESTGIVER_OFFER_REWARD_free(vanilla_SMSG_QUESTGIVER_OFFER_REWARD* object) {
    FREE_STRING(object->title);

    FREE_STRING(object->offer_reward_text);

    free(object->emotes);
    object->emotes = NULL;
    free(object->choice_item_rewards);
    object->choice_item_rewards = NULL;
    free(object->item_rewards);
    object->item_rewards = NULL;
}

static WowWorldResult vanilla_CMSG_QUESTGIVER_CHOOSE_REWARD_read(WowWorldReader* reader, vanilla_CMSG_QUESTGIVER_CHOOSE_REWARD* object) {
    READ_U64(object->guid);

    READ_U32(object->quest_id);

    READ_U32(object->reward);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_QUESTGIVER_CHOOSE_REWARD_write(WowWorldWriter* writer, const vanilla_CMSG_QUESTGIVER_CHOOSE_REWARD* object) {
    WRITE_U16_BE(0x0010 + 4); /* size */

    WRITE_U32(0x0000018e); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->quest_id);

    WRITE_U32(object->reward);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_QUESTGIVER_QUEST_INVALID_read(WowWorldReader* reader, vanilla_SMSG_QUESTGIVER_QUEST_INVALID* object) {
    object->msg = 0;
    READ_U32(object->msg);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_QUESTGIVER_QUEST_INVALID_write(WowWorldWriter* writer, const vanilla_SMSG_QUESTGIVER_QUEST_INVALID* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x0000018f); /* opcode */

    WRITE_U32(object->msg);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_QUESTGIVER_CANCEL_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x00000190); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_QUESTGIVER_QUEST_COMPLETE_size(const vanilla_SMSG_QUESTGIVER_QUEST_COMPLETE* object) {
    return 20 + 8 * object->amount_of_item_rewards;
}

static WowWorldResult vanilla_SMSG_QUESTGIVER_QUEST_COMPLETE_read(WowWorldReader* reader, vanilla_SMSG_QUESTGIVER_QUEST_COMPLETE* object) {
    READ_U32(object->quest_id);

    READ_U32(object->unknown);

    READ_U32(object->experience_reward);

    READ_U32(object->money_reward);

    READ_U32(object->amount_of_item_rewards);

    READ_ARRAY_ALLOCATE(object->item_rewards, object->amount_of_item_rewards, sizeof(vanilla_QuestItemReward));
    READ_ARRAY(object->item_rewards, object->amount_of_item_rewards, WWM_CHECK_RETURN_CODE(vanilla_QuestItemReward_read(reader, &object->item_rewards[i])));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_QUESTGIVER_QUEST_COMPLETE_write(WowWorldWriter* writer, const vanilla_SMSG_QUESTGIVER_QUEST_COMPLETE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_QUESTGIVER_QUEST_COMPLETE_size(object) + 2); /* size */

    WRITE_U16(0x00000191); /* opcode */

    WRITE_U32(object->quest_id);

    WRITE_U32(object->unknown);

    WRITE_U32(object->experience_reward);

    WRITE_U32(object->money_reward);

    WRITE_U32(object->amount_of_item_rewards);

    WRITE_ARRAY(object->item_rewards, object->amount_of_item_rewards, WWM_CHECK_RETURN_CODE(vanilla_QuestItemReward_write(writer, &object->item_rewards[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_QUESTGIVER_QUEST_COMPLETE_free(vanilla_SMSG_QUESTGIVER_QUEST_COMPLETE* object) {
    free(object->item_rewards);
    object->item_rewards = NULL;
}

static WowWorldResult vanilla_SMSG_QUESTGIVER_QUEST_FAILED_read(WowWorldReader* reader, vanilla_SMSG_QUESTGIVER_QUEST_FAILED* object) {
    READ_U32(object->quest_id);

    object->reason = 0;
    READ_U32(object->reason);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_QUESTGIVER_QUEST_FAILED_write(WowWorldWriter* writer, const vanilla_SMSG_QUESTGIVER_QUEST_FAILED* object) {
    WRITE_U16_BE(0x0008 + 2); /* size */

    WRITE_U16(0x00000192); /* opcode */

    WRITE_U32(object->quest_id);

    WRITE_U32(object->reason);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_QUESTLOG_SWAP_QUEST_read(WowWorldReader* reader, vanilla_CMSG_QUESTLOG_SWAP_QUEST* object) {
    READ_U8(object->slot1);

    READ_U8(object->slot2);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_QUESTLOG_SWAP_QUEST_write(WowWorldWriter* writer, const vanilla_CMSG_QUESTLOG_SWAP_QUEST* object) {
    WRITE_U16_BE(0x0002 + 4); /* size */

    WRITE_U32(0x00000193); /* opcode */

    WRITE_U8(object->slot1);

    WRITE_U8(object->slot2);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_QUESTLOG_REMOVE_QUEST_read(WowWorldReader* reader, vanilla_CMSG_QUESTLOG_REMOVE_QUEST* object) {
    READ_U8(object->slot);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_QUESTLOG_REMOVE_QUEST_write(WowWorldWriter* writer, const vanilla_CMSG_QUESTLOG_REMOVE_QUEST* object) {
    WRITE_U16_BE(0x0001 + 4); /* size */

    WRITE_U32(0x00000194); /* opcode */

    WRITE_U8(object->slot);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_QUESTLOG_FULL_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 2); /* size */

    WRITE_U16(0x00000195); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_QUESTUPDATE_FAILED_read(WowWorldReader* reader, vanilla_SMSG_QUESTUPDATE_FAILED* object) {
    READ_U32(object->quest_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_QUESTUPDATE_FAILED_write(WowWorldWriter* writer, const vanilla_SMSG_QUESTUPDATE_FAILED* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x00000196); /* opcode */

    WRITE_U32(object->quest_id);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_QUESTUPDATE_FAILEDTIMER_read(WowWorldReader* reader, vanilla_SMSG_QUESTUPDATE_FAILEDTIMER* object) {
    READ_U32(object->quest_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_QUESTUPDATE_FAILEDTIMER_write(WowWorldWriter* writer, const vanilla_SMSG_QUESTUPDATE_FAILEDTIMER* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x00000197); /* opcode */

    WRITE_U32(object->quest_id);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_QUESTUPDATE_COMPLETE_read(WowWorldReader* reader, vanilla_SMSG_QUESTUPDATE_COMPLETE* object) {
    READ_U32(object->quest_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_QUESTUPDATE_COMPLETE_write(WowWorldWriter* writer, const vanilla_SMSG_QUESTUPDATE_COMPLETE* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x00000198); /* opcode */

    WRITE_U32(object->quest_id);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_QUESTUPDATE_ADD_KILL_read(WowWorldReader* reader, vanilla_SMSG_QUESTUPDATE_ADD_KILL* object) {
    READ_U32(object->quest_id);

    READ_U32(object->creature_id);

    READ_U32(object->kill_count);

    READ_U32(object->required_kill_count);

    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_QUESTUPDATE_ADD_KILL_write(WowWorldWriter* writer, const vanilla_SMSG_QUESTUPDATE_ADD_KILL* object) {
    WRITE_U16_BE(0x0018 + 2); /* size */

    WRITE_U16(0x00000199); /* opcode */

    WRITE_U32(object->quest_id);

    WRITE_U32(object->creature_id);

    WRITE_U32(object->kill_count);

    WRITE_U32(object->required_kill_count);

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_QUESTUPDATE_ADD_ITEM_read(WowWorldReader* reader, vanilla_SMSG_QUESTUPDATE_ADD_ITEM* object) {
    READ_U32(object->required_item_id);

    READ_U32(object->items_required);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_QUESTUPDATE_ADD_ITEM_write(WowWorldWriter* writer, const vanilla_SMSG_QUESTUPDATE_ADD_ITEM* object) {
    WRITE_U16_BE(0x0008 + 2); /* size */

    WRITE_U16(0x0000019a); /* opcode */

    WRITE_U32(object->required_item_id);

    WRITE_U32(object->items_required);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_QUEST_CONFIRM_ACCEPT_read(WowWorldReader* reader, vanilla_CMSG_QUEST_CONFIRM_ACCEPT* object) {
    READ_U32(object->quest_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_QUEST_CONFIRM_ACCEPT_write(WowWorldWriter* writer, const vanilla_CMSG_QUEST_CONFIRM_ACCEPT* object) {
    WRITE_U16_BE(0x0004 + 4); /* size */

    WRITE_U32(0x0000019b); /* opcode */

    WRITE_U32(object->quest_id);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_QUEST_CONFIRM_ACCEPT_size(const vanilla_SMSG_QUEST_CONFIRM_ACCEPT* object) {
    return 13 + STRING_SIZE(object->quest_title);
}

static WowWorldResult vanilla_SMSG_QUEST_CONFIRM_ACCEPT_read(WowWorldReader* reader, vanilla_SMSG_QUEST_CONFIRM_ACCEPT* object) {
    READ_U32(object->quest_id);

    READ_CSTRING(object->quest_title);

    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_QUEST_CONFIRM_ACCEPT_write(WowWorldWriter* writer, const vanilla_SMSG_QUEST_CONFIRM_ACCEPT* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_QUEST_CONFIRM_ACCEPT_size(object) + 2); /* size */

    WRITE_U16(0x0000019c); /* opcode */

    WRITE_U32(object->quest_id);

    WRITE_CSTRING(object->quest_title);

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_QUEST_CONFIRM_ACCEPT_free(vanilla_SMSG_QUEST_CONFIRM_ACCEPT* object) {
    FREE_STRING(object->quest_title);

}

static WowWorldResult vanilla_CMSG_PUSHQUESTTOPARTY_read(WowWorldReader* reader, vanilla_CMSG_PUSHQUESTTOPARTY* object) {
    READ_U32(object->quest_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_PUSHQUESTTOPARTY_write(WowWorldWriter* writer, const vanilla_CMSG_PUSHQUESTTOPARTY* object) {
    WRITE_U16_BE(0x0004 + 4); /* size */

    WRITE_U32(0x0000019d); /* opcode */

    WRITE_U32(object->quest_id);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_LIST_INVENTORY_read(WowWorldReader* reader, vanilla_CMSG_LIST_INVENTORY* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_LIST_INVENTORY_write(WowWorldWriter* writer, const vanilla_CMSG_LIST_INVENTORY* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x0000019e); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_LIST_INVENTORY_size(const vanilla_SMSG_LIST_INVENTORY* object) {
    return 9 + 28 * object->amount_of_items;
}

static WowWorldResult vanilla_SMSG_LIST_INVENTORY_read(WowWorldReader* reader, vanilla_SMSG_LIST_INVENTORY* object) {
    READ_U64(object->vendor);

    READ_U8(object->amount_of_items);

    READ_ARRAY_ALLOCATE(object->items, object->amount_of_items, sizeof(vanilla_ListInventoryItem));
    READ_ARRAY(object->items, object->amount_of_items, WWM_CHECK_RETURN_CODE(vanilla_ListInventoryItem_read(reader, &object->items[i])));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_LIST_INVENTORY_write(WowWorldWriter* writer, const vanilla_SMSG_LIST_INVENTORY* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_LIST_INVENTORY_size(object) + 2); /* size */

    WRITE_U16(0x0000019f); /* opcode */

    WRITE_U64(object->vendor);

    WRITE_U8(object->amount_of_items);

    WRITE_ARRAY(object->items, object->amount_of_items, WWM_CHECK_RETURN_CODE(vanilla_ListInventoryItem_write(writer, &object->items[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_LIST_INVENTORY_free(vanilla_SMSG_LIST_INVENTORY* object) {
    free(object->items);
    object->items = NULL;
}

static WowWorldResult vanilla_CMSG_SELL_ITEM_read(WowWorldReader* reader, vanilla_CMSG_SELL_ITEM* object) {
    READ_U64(object->vendor);

    READ_U64(object->item);

    READ_U8(object->amount);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_SELL_ITEM_write(WowWorldWriter* writer, const vanilla_CMSG_SELL_ITEM* object) {
    WRITE_U16_BE(0x0011 + 4); /* size */

    WRITE_U32(0x000001a0); /* opcode */

    WRITE_U64(object->vendor);

    WRITE_U64(object->item);

    WRITE_U8(object->amount);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_SELL_ITEM_read(WowWorldReader* reader, vanilla_SMSG_SELL_ITEM* object) {
    READ_U64(object->guid);

    READ_U64(object->item);

    object->result = 0;
    READ_U8(object->result);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SELL_ITEM_write(WowWorldWriter* writer, const vanilla_SMSG_SELL_ITEM* object) {
    WRITE_U16_BE(0x0011 + 2); /* size */

    WRITE_U16(0x000001a1); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U64(object->item);

    WRITE_U8(object->result);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_BUY_ITEM_read(WowWorldReader* reader, vanilla_CMSG_BUY_ITEM* object) {
    READ_U64(object->vendor);

    READ_U32(object->item);

    READ_U8(object->amount);

    READ_U8(object->unknown1);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_BUY_ITEM_write(WowWorldWriter* writer, const vanilla_CMSG_BUY_ITEM* object) {
    WRITE_U16_BE(0x000e + 4); /* size */

    WRITE_U32(0x000001a2); /* opcode */

    WRITE_U64(object->vendor);

    WRITE_U32(object->item);

    WRITE_U8(object->amount);

    WRITE_U8(object->unknown1);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_BUY_ITEM_IN_SLOT_read(WowWorldReader* reader, vanilla_CMSG_BUY_ITEM_IN_SLOT* object) {
    READ_U64(object->vendor);

    READ_U32(object->item);

    READ_U64(object->bag);

    READ_U8(object->bag_slot);

    READ_U8(object->amount);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_BUY_ITEM_IN_SLOT_write(WowWorldWriter* writer, const vanilla_CMSG_BUY_ITEM_IN_SLOT* object) {
    WRITE_U16_BE(0x0016 + 4); /* size */

    WRITE_U32(0x000001a3); /* opcode */

    WRITE_U64(object->vendor);

    WRITE_U32(object->item);

    WRITE_U64(object->bag);

    WRITE_U8(object->bag_slot);

    WRITE_U8(object->amount);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_BUY_ITEM_read(WowWorldReader* reader, vanilla_SMSG_BUY_ITEM* object) {
    READ_U64(object->guid);

    READ_U32(object->vendor_slot);

    READ_U32(object->amount_for_sale);

    READ_U32(object->amount_bought);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_BUY_ITEM_write(WowWorldWriter* writer, const vanilla_SMSG_BUY_ITEM* object) {
    WRITE_U16_BE(0x0014 + 2); /* size */

    WRITE_U16(0x000001a4); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->vendor_slot);

    WRITE_U32(object->amount_for_sale);

    WRITE_U32(object->amount_bought);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_BUY_FAILED_read(WowWorldReader* reader, vanilla_SMSG_BUY_FAILED* object) {
    READ_U64(object->guid);

    READ_U32(object->item);

    object->result = 0;
    READ_U8(object->result);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_BUY_FAILED_write(WowWorldWriter* writer, const vanilla_SMSG_BUY_FAILED* object) {
    WRITE_U16_BE(0x000d + 2); /* size */

    WRITE_U16(0x000001a5); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->item);

    WRITE_U8(object->result);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_SHOWTAXINODES_size(const vanilla_SMSG_SHOWTAXINODES* object) {
    return 16 + 4 * object->amount_of_nodes;
}

static WowWorldResult vanilla_SMSG_SHOWTAXINODES_read(WowWorldReader* reader, vanilla_SMSG_SHOWTAXINODES* object, size_t body_size) {
    size_t _size = 0;

    READ_U32(object->unknown1);
    _size += 4;

    READ_U64(object->guid);
    _size += 8;

    READ_U32(object->nearest_node);
    _size += 4;

    object->amount_of_nodes = 0;
    /* C89 scope to allow variable declarations */ {
        int i = 0;
        size_t _current_size = 8 * sizeof(*object->nodes);

        object->nodes = malloc(_current_size);
        while (_size < body_size) {
            READ_U32(object->nodes[i]);
            _size += 4;
            ++i;

            if (i * sizeof(*object->nodes) >= _current_size) {
                _current_size *= 2;
                object->nodes = realloc(object->nodes, _current_size);
            }
        }

        object->amount_of_nodes = i;
    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SHOWTAXINODES_write(WowWorldWriter* writer, const vanilla_SMSG_SHOWTAXINODES* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SHOWTAXINODES_size(object) + 2); /* size */

    WRITE_U16(0x000001a9); /* opcode */

    WRITE_U32(object->unknown1);

    WRITE_U64(object->guid);

    WRITE_U32(object->nearest_node);

    WRITE_ARRAY(object->nodes, object->amount_of_nodes, WRITE_U32(object->nodes[i]));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_SHOWTAXINODES_free(vanilla_SMSG_SHOWTAXINODES* object) {
    free(object->nodes);
    object->nodes = NULL;
}

static WowWorldResult vanilla_CMSG_TAXINODE_STATUS_QUERY_read(WowWorldReader* reader, vanilla_CMSG_TAXINODE_STATUS_QUERY* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_TAXINODE_STATUS_QUERY_write(WowWorldWriter* writer, const vanilla_CMSG_TAXINODE_STATUS_QUERY* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x000001aa); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_TAXINODE_STATUS_read(WowWorldReader* reader, vanilla_SMSG_TAXINODE_STATUS* object) {
    READ_U64(object->guid);

    READ_BOOL8(object->taxi_mask_node_known);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_TAXINODE_STATUS_write(WowWorldWriter* writer, const vanilla_SMSG_TAXINODE_STATUS* object) {
    WRITE_U16_BE(0x0009 + 2); /* size */

    WRITE_U16(0x000001ab); /* opcode */

    WRITE_U64(object->guid);

    WRITE_BOOL8(object->taxi_mask_node_known);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_TAXIQUERYAVAILABLENODES_read(WowWorldReader* reader, vanilla_CMSG_TAXIQUERYAVAILABLENODES* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_TAXIQUERYAVAILABLENODES_write(WowWorldWriter* writer, const vanilla_CMSG_TAXIQUERYAVAILABLENODES* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x000001ac); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_ACTIVATETAXI_read(WowWorldReader* reader, vanilla_CMSG_ACTIVATETAXI* object) {
    READ_U64(object->guid);

    READ_U32(object->source_node);

    READ_U32(object->destination_node);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_ACTIVATETAXI_write(WowWorldWriter* writer, const vanilla_CMSG_ACTIVATETAXI* object) {
    WRITE_U16_BE(0x0010 + 4); /* size */

    WRITE_U32(0x000001ad); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->source_node);

    WRITE_U32(object->destination_node);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_ACTIVATETAXIREPLY_read(WowWorldReader* reader, vanilla_SMSG_ACTIVATETAXIREPLY* object) {
    object->reply = 0;
    READ_U32(object->reply);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_ACTIVATETAXIREPLY_write(WowWorldWriter* writer, const vanilla_SMSG_ACTIVATETAXIREPLY* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x000001ae); /* opcode */

    WRITE_U32(object->reply);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_NEW_TAXI_PATH_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 2); /* size */

    WRITE_U16(0x000001af); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_TRAINER_LIST_read(WowWorldReader* reader, vanilla_CMSG_TRAINER_LIST* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_TRAINER_LIST_write(WowWorldWriter* writer, const vanilla_CMSG_TRAINER_LIST* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x000001b0); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_TRAINER_LIST_size(const vanilla_SMSG_TRAINER_LIST* object) {
    return 17 + 38 * object->amount_of_spells + STRING_SIZE(object->greeting);
}

static WowWorldResult vanilla_SMSG_TRAINER_LIST_read(WowWorldReader* reader, vanilla_SMSG_TRAINER_LIST* object) {
    READ_U64(object->guid);

    READ_U32(object->trainer_type);

    READ_U32(object->amount_of_spells);

    READ_ARRAY_ALLOCATE(object->spells, object->amount_of_spells, sizeof(vanilla_TrainerSpell));
    READ_ARRAY(object->spells, object->amount_of_spells, WWM_CHECK_RETURN_CODE(vanilla_TrainerSpell_read(reader, &object->spells[i])));

    READ_CSTRING(object->greeting);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_TRAINER_LIST_write(WowWorldWriter* writer, const vanilla_SMSG_TRAINER_LIST* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_TRAINER_LIST_size(object) + 2); /* size */

    WRITE_U16(0x000001b1); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->trainer_type);

    WRITE_U32(object->amount_of_spells);

    WRITE_ARRAY(object->spells, object->amount_of_spells, WWM_CHECK_RETURN_CODE(vanilla_TrainerSpell_write(writer, &object->spells[i])));

    WRITE_CSTRING(object->greeting);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_TRAINER_LIST_free(vanilla_SMSG_TRAINER_LIST* object) {
    size_t i;

    for (i = 0; i < object->amount_of_spells; ++i) {
        vanilla_TrainerSpell_free(&((object->spells)[i]));
    }
    free(object->spells);
    object->spells = NULL;
    FREE_STRING(object->greeting);

}

static WowWorldResult vanilla_CMSG_TRAINER_BUY_SPELL_read(WowWorldReader* reader, vanilla_CMSG_TRAINER_BUY_SPELL* object) {
    READ_U64(object->guid);

    READ_U32(object->id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_TRAINER_BUY_SPELL_write(WowWorldWriter* writer, const vanilla_CMSG_TRAINER_BUY_SPELL* object) {
    WRITE_U16_BE(0x000c + 4); /* size */

    WRITE_U32(0x000001b2); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->id);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_TRAINER_BUY_SUCCEEDED_read(WowWorldReader* reader, vanilla_SMSG_TRAINER_BUY_SUCCEEDED* object) {
    READ_U64(object->guid);

    READ_U32(object->id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_TRAINER_BUY_SUCCEEDED_write(WowWorldWriter* writer, const vanilla_SMSG_TRAINER_BUY_SUCCEEDED* object) {
    WRITE_U16_BE(0x000c + 2); /* size */

    WRITE_U16(0x000001b3); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->id);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_TRAINER_BUY_FAILED_read(WowWorldReader* reader, vanilla_SMSG_TRAINER_BUY_FAILED* object) {
    READ_U64(object->guid);

    READ_U32(object->id);

    object->error = 0;
    READ_U32(object->error);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_TRAINER_BUY_FAILED_write(WowWorldWriter* writer, const vanilla_SMSG_TRAINER_BUY_FAILED* object) {
    WRITE_U16_BE(0x0010 + 2); /* size */

    WRITE_U16(0x000001b4); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->id);

    WRITE_U32(object->error);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_BINDER_ACTIVATE_read(WowWorldReader* reader, vanilla_CMSG_BINDER_ACTIVATE* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_BINDER_ACTIVATE_write(WowWorldWriter* writer, const vanilla_CMSG_BINDER_ACTIVATE* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x000001b5); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_BANKER_ACTIVATE_read(WowWorldReader* reader, vanilla_CMSG_BANKER_ACTIVATE* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_BANKER_ACTIVATE_write(WowWorldWriter* writer, const vanilla_CMSG_BANKER_ACTIVATE* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x000001b7); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_SHOW_BANK_read(WowWorldReader* reader, vanilla_SMSG_SHOW_BANK* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SHOW_BANK_write(WowWorldWriter* writer, const vanilla_SMSG_SHOW_BANK* object) {
    WRITE_U16_BE(0x0008 + 2); /* size */

    WRITE_U16(0x000001b8); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_BUY_BANK_SLOT_read(WowWorldReader* reader, vanilla_CMSG_BUY_BANK_SLOT* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_BUY_BANK_SLOT_write(WowWorldWriter* writer, const vanilla_CMSG_BUY_BANK_SLOT* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x000001b9); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_BUY_BANK_SLOT_RESULT_read(WowWorldReader* reader, vanilla_SMSG_BUY_BANK_SLOT_RESULT* object) {
    object->result = 0;
    READ_U32(object->result);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_BUY_BANK_SLOT_RESULT_write(WowWorldWriter* writer, const vanilla_SMSG_BUY_BANK_SLOT_RESULT* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x000001ba); /* opcode */

    WRITE_U32(object->result);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_PETITION_SHOWLIST_read(WowWorldReader* reader, vanilla_CMSG_PETITION_SHOWLIST* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_PETITION_SHOWLIST_write(WowWorldWriter* writer, const vanilla_CMSG_PETITION_SHOWLIST* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x000001bb); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_PETITION_SHOWLIST_size(const vanilla_SMSG_PETITION_SHOWLIST* object) {
    return 9 + 20 * object->amount_of_petitions;
}

static WowWorldResult vanilla_SMSG_PETITION_SHOWLIST_read(WowWorldReader* reader, vanilla_SMSG_PETITION_SHOWLIST* object) {
    READ_U64(object->npc);

    READ_U8(object->amount_of_petitions);

    READ_ARRAY_ALLOCATE(object->petitions, object->amount_of_petitions, sizeof(vanilla_PetitionShowlist));
    READ_ARRAY(object->petitions, object->amount_of_petitions, WWM_CHECK_RETURN_CODE(vanilla_PetitionShowlist_read(reader, &object->petitions[i])));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PETITION_SHOWLIST_write(WowWorldWriter* writer, const vanilla_SMSG_PETITION_SHOWLIST* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_PETITION_SHOWLIST_size(object) + 2); /* size */

    WRITE_U16(0x000001bc); /* opcode */

    WRITE_U64(object->npc);

    WRITE_U8(object->amount_of_petitions);

    WRITE_ARRAY(object->petitions, object->amount_of_petitions, WWM_CHECK_RETURN_CODE(vanilla_PetitionShowlist_write(writer, &object->petitions[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_PETITION_SHOWLIST_free(vanilla_SMSG_PETITION_SHOWLIST* object) {
    free(object->petitions);
    object->petitions = NULL;
}

static size_t vanilla_CMSG_PETITION_BUY_size(const vanilla_CMSG_PETITION_BUY* object) {
    return 72 + STRING_SIZE(object->name);
}

static WowWorldResult vanilla_CMSG_PETITION_BUY_read(WowWorldReader* reader, vanilla_CMSG_PETITION_BUY* object) {
    READ_U64(object->npc);

    READ_U32(object->unknown1);

    READ_U64(object->unknown2);

    READ_CSTRING(object->name);

    READ_U32(object->unknown3);

    READ_U32(object->unknown4);

    READ_U32(object->unknown5);

    READ_U32(object->unknown6);

    READ_U32(object->unknown7);

    READ_U32(object->unknown8);

    READ_U32(object->unknown9);

    READ_U32(object->unknown10);

    READ_U32(object->unknown11);

    READ_U32(object->unknown12);

    READ_U16(object->unknown13);

    READ_U8(object->unknown14);

    READ_U32(object->index);

    READ_U32(object->unknown15);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_PETITION_BUY_write(WowWorldWriter* writer, const vanilla_CMSG_PETITION_BUY* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_PETITION_BUY_size(object) + 4); /* size */

    WRITE_U32(0x000001bd); /* opcode */

    WRITE_U64(object->npc);

    WRITE_U32(object->unknown1);

    WRITE_U64(object->unknown2);

    WRITE_CSTRING(object->name);

    WRITE_U32(object->unknown3);

    WRITE_U32(object->unknown4);

    WRITE_U32(object->unknown5);

    WRITE_U32(object->unknown6);

    WRITE_U32(object->unknown7);

    WRITE_U32(object->unknown8);

    WRITE_U32(object->unknown9);

    WRITE_U32(object->unknown10);

    WRITE_U32(object->unknown11);

    WRITE_U32(object->unknown12);

    WRITE_U16(object->unknown13);

    WRITE_U8(object->unknown14);

    WRITE_U32(object->index);

    WRITE_U32(object->unknown15);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_PETITION_BUY_free(vanilla_CMSG_PETITION_BUY* object) {
    FREE_STRING(object->name);

}

static WowWorldResult vanilla_CMSG_PETITION_SHOW_SIGNATURES_read(WowWorldReader* reader, vanilla_CMSG_PETITION_SHOW_SIGNATURES* object) {
    READ_U64(object->item);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_PETITION_SHOW_SIGNATURES_write(WowWorldWriter* writer, const vanilla_CMSG_PETITION_SHOW_SIGNATURES* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x000001be); /* opcode */

    WRITE_U64(object->item);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_PETITION_SHOW_SIGNATURES_size(const vanilla_SMSG_PETITION_SHOW_SIGNATURES* object) {
    return 21 + 12 * object->amount_of_signatures;
}

static WowWorldResult vanilla_SMSG_PETITION_SHOW_SIGNATURES_read(WowWorldReader* reader, vanilla_SMSG_PETITION_SHOW_SIGNATURES* object) {
    READ_U64(object->item);

    READ_U64(object->owner);

    READ_U32(object->petition);

    READ_U8(object->amount_of_signatures);

    READ_ARRAY_ALLOCATE(object->signatures, object->amount_of_signatures, sizeof(vanilla_PetitionSignature));
    READ_ARRAY(object->signatures, object->amount_of_signatures, WWM_CHECK_RETURN_CODE(vanilla_PetitionSignature_read(reader, &object->signatures[i])));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PETITION_SHOW_SIGNATURES_write(WowWorldWriter* writer, const vanilla_SMSG_PETITION_SHOW_SIGNATURES* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_PETITION_SHOW_SIGNATURES_size(object) + 2); /* size */

    WRITE_U16(0x000001bf); /* opcode */

    WRITE_U64(object->item);

    WRITE_U64(object->owner);

    WRITE_U32(object->petition);

    WRITE_U8(object->amount_of_signatures);

    WRITE_ARRAY(object->signatures, object->amount_of_signatures, WWM_CHECK_RETURN_CODE(vanilla_PetitionSignature_write(writer, &object->signatures[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_PETITION_SHOW_SIGNATURES_free(vanilla_SMSG_PETITION_SHOW_SIGNATURES* object) {
    free(object->signatures);
    object->signatures = NULL;
}

static WowWorldResult vanilla_CMSG_PETITION_SIGN_read(WowWorldReader* reader, vanilla_CMSG_PETITION_SIGN* object) {
    READ_U64(object->petition);

    READ_U8(object->unknown1);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_PETITION_SIGN_write(WowWorldWriter* writer, const vanilla_CMSG_PETITION_SIGN* object) {
    WRITE_U16_BE(0x0009 + 4); /* size */

    WRITE_U32(0x000001c0); /* opcode */

    WRITE_U64(object->petition);

    WRITE_U8(object->unknown1);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_PETITION_SIGN_RESULTS_read(WowWorldReader* reader, vanilla_SMSG_PETITION_SIGN_RESULTS* object) {
    READ_U64(object->petition);

    READ_U64(object->owner);

    object->result = 0;
    READ_U32(object->result);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PETITION_SIGN_RESULTS_write(WowWorldWriter* writer, const vanilla_SMSG_PETITION_SIGN_RESULTS* object) {
    WRITE_U16_BE(0x0014 + 2); /* size */

    WRITE_U16(0x000001c1); /* opcode */

    WRITE_U64(object->petition);

    WRITE_U64(object->owner);

    WRITE_U32(object->result);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_MSG_PETITION_DECLINE_read(WowWorldReader* reader, vanilla_MSG_PETITION_DECLINE* object) {
    READ_U64(object->petition);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_PETITION_DECLINE_cmsg_write(WowWorldWriter* writer, const vanilla_MSG_PETITION_DECLINE* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x000001c2); /* opcode */

    WRITE_U64(object->petition);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_PETITION_DECLINE_smsg_write(WowWorldWriter* writer, const vanilla_MSG_PETITION_DECLINE* object) {
    WRITE_U16_BE(0x0008 + 2); /* size */

    WRITE_U16(0x000001c2); /* opcode */

    WRITE_U64(object->petition);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_OFFER_PETITION_read(WowWorldReader* reader, vanilla_CMSG_OFFER_PETITION* object) {
    READ_U64(object->petition);

    READ_U64(object->target);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_OFFER_PETITION_write(WowWorldWriter* writer, const vanilla_CMSG_OFFER_PETITION* object) {
    WRITE_U16_BE(0x0010 + 4); /* size */

    WRITE_U32(0x000001c3); /* opcode */

    WRITE_U64(object->petition);

    WRITE_U64(object->target);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_TURN_IN_PETITION_read(WowWorldReader* reader, vanilla_CMSG_TURN_IN_PETITION* object) {
    READ_U64(object->petition);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_TURN_IN_PETITION_write(WowWorldWriter* writer, const vanilla_CMSG_TURN_IN_PETITION* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x000001c4); /* opcode */

    WRITE_U64(object->petition);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_TURN_IN_PETITION_RESULTS_read(WowWorldReader* reader, vanilla_SMSG_TURN_IN_PETITION_RESULTS* object) {
    object->result = 0;
    READ_U32(object->result);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_TURN_IN_PETITION_RESULTS_write(WowWorldWriter* writer, const vanilla_SMSG_TURN_IN_PETITION_RESULTS* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x000001c5); /* opcode */

    WRITE_U32(object->result);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_PETITION_QUERY_read(WowWorldReader* reader, vanilla_CMSG_PETITION_QUERY* object) {
    READ_U32(object->guild_id);

    READ_U64(object->petition);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_PETITION_QUERY_write(WowWorldWriter* writer, const vanilla_CMSG_PETITION_QUERY* object) {
    WRITE_U16_BE(0x000c + 4); /* size */

    WRITE_U32(0x000001c6); /* opcode */

    WRITE_U32(object->guild_id);

    WRITE_U64(object->petition);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_PETITION_QUERY_RESPONSE_size(const vanilla_SMSG_PETITION_QUERY_RESPONSE* object) {
    return 64 + STRING_SIZE(object->guild_name) + STRING_SIZE(object->body_text);
}

static WowWorldResult vanilla_SMSG_PETITION_QUERY_RESPONSE_read(WowWorldReader* reader, vanilla_SMSG_PETITION_QUERY_RESPONSE* object) {
    READ_U32(object->petition_id);

    READ_U64(object->charter_owner);

    READ_CSTRING(object->guild_name);

    READ_CSTRING(object->body_text);

    READ_U32(object->unknown_flags);

    READ_U32(object->minimum_signatures);

    READ_U32(object->maximum_signatures);

    READ_U32(object->deadline);

    READ_U32(object->issue_date);

    READ_U32(object->allowed_guild_id);

    object->allowed_class = 0;
    READ_U32(object->allowed_class);

    object->allowed_race = 0;
    READ_U32(object->allowed_race);

    READ_U16(object->allowed_genders);

    READ_U32(object->allowed_minimum_level);

    READ_U32(object->allowed_maximum_level);

    READ_U32(object->todo_amount_of_signers);

    READ_U32(object->number_of_choices);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PETITION_QUERY_RESPONSE_write(WowWorldWriter* writer, const vanilla_SMSG_PETITION_QUERY_RESPONSE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_PETITION_QUERY_RESPONSE_size(object) + 2); /* size */

    WRITE_U16(0x000001c7); /* opcode */

    WRITE_U32(object->petition_id);

    WRITE_U64(object->charter_owner);

    WRITE_CSTRING(object->guild_name);

    WRITE_CSTRING(object->body_text);

    WRITE_U32(object->unknown_flags);

    WRITE_U32(object->minimum_signatures);

    WRITE_U32(object->maximum_signatures);

    WRITE_U32(object->deadline);

    WRITE_U32(object->issue_date);

    WRITE_U32(object->allowed_guild_id);

    WRITE_U32(object->allowed_class);

    WRITE_U32(object->allowed_race);

    WRITE_U16(object->allowed_genders);

    WRITE_U32(object->allowed_minimum_level);

    WRITE_U32(object->allowed_maximum_level);

    WRITE_U32(object->todo_amount_of_signers);

    WRITE_U32(object->number_of_choices);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_PETITION_QUERY_RESPONSE_free(vanilla_SMSG_PETITION_QUERY_RESPONSE* object) {
    FREE_STRING(object->guild_name);

    FREE_STRING(object->body_text);

}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_FISH_NOT_HOOKED_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 2); /* size */

    WRITE_U16(0x000001c8); /* opcode */


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_FISH_ESCAPED_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 2); /* size */

    WRITE_U16(0x000001c9); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_BUG_size(const vanilla_CMSG_BUG* object) {
    return 12 + STRING_SIZE(object->content) + STRING_SIZE(object->bug_type);
}

static WowWorldResult vanilla_CMSG_BUG_read(WowWorldReader* reader, vanilla_CMSG_BUG* object) {
    READ_U32(object->suggestion);

    READ_SIZED_CSTRING(object->content);

    READ_SIZED_CSTRING(object->bug_type);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_BUG_write(WowWorldWriter* writer, const vanilla_CMSG_BUG* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_BUG_size(object) + 4); /* size */

    WRITE_U32(0x000001ca); /* opcode */

    WRITE_U32(object->suggestion);

    WRITE_SIZED_CSTRING(object->content);

    WRITE_SIZED_CSTRING(object->bug_type);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_BUG_free(vanilla_CMSG_BUG* object) {
    FREE_STRING(object->content);

    FREE_STRING(object->bug_type);

}

static size_t vanilla_SMSG_NOTIFICATION_size(const vanilla_SMSG_NOTIFICATION* object) {
    return 1 + STRING_SIZE(object->notification);
}

static WowWorldResult vanilla_SMSG_NOTIFICATION_read(WowWorldReader* reader, vanilla_SMSG_NOTIFICATION* object) {
    READ_CSTRING(object->notification);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_NOTIFICATION_write(WowWorldWriter* writer, const vanilla_SMSG_NOTIFICATION* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_NOTIFICATION_size(object) + 2); /* size */

    WRITE_U16(0x000001cb); /* opcode */

    WRITE_CSTRING(object->notification);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_NOTIFICATION_free(vanilla_SMSG_NOTIFICATION* object) {
    FREE_STRING(object->notification);

}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_PLAYED_TIME_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x000001cc); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_PLAYED_TIME_read(WowWorldReader* reader, vanilla_SMSG_PLAYED_TIME* object) {
    READ_U32(object->total_played_time);

    READ_U32(object->level_played_time);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PLAYED_TIME_write(WowWorldWriter* writer, const vanilla_SMSG_PLAYED_TIME* object) {
    WRITE_U16_BE(0x0008 + 2); /* size */

    WRITE_U16(0x000001cd); /* opcode */

    WRITE_U32(object->total_played_time);

    WRITE_U32(object->level_played_time);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_QUERY_TIME_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x000001ce); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_QUERY_TIME_RESPONSE_read(WowWorldReader* reader, vanilla_SMSG_QUERY_TIME_RESPONSE* object) {
    READ_U32(object->time);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_QUERY_TIME_RESPONSE_write(WowWorldWriter* writer, const vanilla_SMSG_QUERY_TIME_RESPONSE* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x000001cf); /* opcode */

    WRITE_U32(object->time);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_LOG_XPGAIN_size(const vanilla_SMSG_LOG_XPGAIN* object) {
    size_t _size = 13;

    if (object->exp_type == VANILLA_EXPERIENCE_AWARD_TYPE_NON_KILL) {
        _size += 8;
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_LOG_XPGAIN_read(WowWorldReader* reader, vanilla_SMSG_LOG_XPGAIN* object) {
    READ_U64(object->target);

    READ_U32(object->total_exp);

    object->exp_type = 0;
    READ_U8(object->exp_type);

    if (object->exp_type == VANILLA_EXPERIENCE_AWARD_TYPE_NON_KILL) {
        READ_U32(object->experience_without_rested);

        READ_FLOAT(object->exp_group_bonus);

    }
    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_LOG_XPGAIN_write(WowWorldWriter* writer, const vanilla_SMSG_LOG_XPGAIN* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_LOG_XPGAIN_size(object) + 2); /* size */

    WRITE_U16(0x000001d0); /* opcode */

    WRITE_U64(object->target);

    WRITE_U32(object->total_exp);

    WRITE_U8(object->exp_type);

    if (object->exp_type == VANILLA_EXPERIENCE_AWARD_TYPE_NON_KILL) {
        WRITE_U32(object->experience_without_rested);

        WRITE_FLOAT(object->exp_group_bonus);

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_LOG_XPGAIN_free(vanilla_SMSG_LOG_XPGAIN* object) {
    if (object->exp_type == VANILLA_EXPERIENCE_AWARD_TYPE_NON_KILL) {
    }
}

static WowWorldResult vanilla_CMSG_RECLAIM_CORPSE_read(WowWorldReader* reader, vanilla_CMSG_RECLAIM_CORPSE* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_RECLAIM_CORPSE_write(WowWorldWriter* writer, const vanilla_CMSG_RECLAIM_CORPSE* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x000001d2); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_WRAP_ITEM_read(WowWorldReader* reader, vanilla_CMSG_WRAP_ITEM* object) {
    READ_U8(object->gift_bag_index);

    READ_U8(object->gift_slot);

    READ_U8(object->item_bag_index);

    READ_U8(object->item_slot);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_WRAP_ITEM_write(WowWorldWriter* writer, const vanilla_CMSG_WRAP_ITEM* object) {
    WRITE_U16_BE(0x0004 + 4); /* size */

    WRITE_U32(0x000001d3); /* opcode */

    WRITE_U8(object->gift_bag_index);

    WRITE_U8(object->gift_slot);

    WRITE_U8(object->item_bag_index);

    WRITE_U8(object->item_slot);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_LEVELUP_INFO_read(WowWorldReader* reader, vanilla_SMSG_LEVELUP_INFO* object) {
    READ_U32(object->new_level);

    READ_U32(object->health);

    READ_U32(object->mana);

    READ_U32(object->rage);

    READ_U32(object->focus);

    READ_U32(object->energy);

    READ_U32(object->happiness);

    READ_U32(object->strength);

    READ_U32(object->agility);

    READ_U32(object->stamina);

    READ_U32(object->intellect);

    READ_U32(object->spirit);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_LEVELUP_INFO_write(WowWorldWriter* writer, const vanilla_SMSG_LEVELUP_INFO* object) {
    WRITE_U16_BE(0x0030 + 2); /* size */

    WRITE_U16(0x000001d4); /* opcode */

    WRITE_U32(object->new_level);

    WRITE_U32(object->health);

    WRITE_U32(object->mana);

    WRITE_U32(object->rage);

    WRITE_U32(object->focus);

    WRITE_U32(object->energy);

    WRITE_U32(object->happiness);

    WRITE_U32(object->strength);

    WRITE_U32(object->agility);

    WRITE_U32(object->stamina);

    WRITE_U32(object->intellect);

    WRITE_U32(object->spirit);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_MSG_MINIMAP_PING_Client_read(WowWorldReader* reader, vanilla_MSG_MINIMAP_PING_Client* object) {
    READ_FLOAT(object->position_x);

    READ_FLOAT(object->position_y);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MINIMAP_PING_Client_write(WowWorldWriter* writer, const vanilla_MSG_MINIMAP_PING_Client* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x000001d5); /* opcode */

    WRITE_FLOAT(object->position_x);

    WRITE_FLOAT(object->position_y);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_MSG_MINIMAP_PING_Server_read(WowWorldReader* reader, vanilla_MSG_MINIMAP_PING_Server* object) {
    READ_U64(object->guid);

    READ_FLOAT(object->position_x);

    READ_FLOAT(object->position_y);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MINIMAP_PING_Server_write(WowWorldWriter* writer, const vanilla_MSG_MINIMAP_PING_Server* object) {
    WRITE_U16_BE(0x0010 + 2); /* size */

    WRITE_U16(0x000001d5); /* opcode */

    WRITE_U64(object->guid);

    WRITE_FLOAT(object->position_x);

    WRITE_FLOAT(object->position_y);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_RESISTLOG_read(WowWorldReader* reader, vanilla_SMSG_RESISTLOG* object) {
    READ_U64(object->guid1);

    READ_U64(object->guid2);

    READ_U32(object->unknown1);

    READ_FLOAT(object->unknown2);

    READ_FLOAT(object->unknown3);

    READ_U32(object->unknown4);

    READ_U32(object->unknown5);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_RESISTLOG_write(WowWorldWriter* writer, const vanilla_SMSG_RESISTLOG* object) {
    WRITE_U16_BE(0x0024 + 2); /* size */

    WRITE_U16(0x000001d6); /* opcode */

    WRITE_U64(object->guid1);

    WRITE_U64(object->guid2);

    WRITE_U32(object->unknown1);

    WRITE_FLOAT(object->unknown2);

    WRITE_FLOAT(object->unknown3);

    WRITE_U32(object->unknown4);

    WRITE_U32(object->unknown5);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_ENCHANTMENTLOG_read(WowWorldReader* reader, vanilla_SMSG_ENCHANTMENTLOG* object) {
    READ_U64(object->target);

    READ_U64(object->caster);

    READ_U32(object->item);

    READ_U32(object->spell);

    READ_BOOL8(object->show_affiliation);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_ENCHANTMENTLOG_write(WowWorldWriter* writer, const vanilla_SMSG_ENCHANTMENTLOG* object) {
    WRITE_U16_BE(0x0019 + 2); /* size */

    WRITE_U16(0x000001d7); /* opcode */

    WRITE_U64(object->target);

    WRITE_U64(object->caster);

    WRITE_U32(object->item);

    WRITE_U32(object->spell);

    WRITE_BOOL8(object->show_affiliation);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_START_MIRROR_TIMER_read(WowWorldReader* reader, vanilla_SMSG_START_MIRROR_TIMER* object) {
    object->timer = 0;
    READ_U32(object->timer);

    READ_U32(object->time_remaining);

    READ_U32(object->duration);

    READ_U32(object->scale);

    READ_BOOL8(object->is_frozen);

    READ_U32(object->id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_START_MIRROR_TIMER_write(WowWorldWriter* writer, const vanilla_SMSG_START_MIRROR_TIMER* object) {
    WRITE_U16_BE(0x0015 + 2); /* size */

    WRITE_U16(0x000001d9); /* opcode */

    WRITE_U32(object->timer);

    WRITE_U32(object->time_remaining);

    WRITE_U32(object->duration);

    WRITE_U32(object->scale);

    WRITE_BOOL8(object->is_frozen);

    WRITE_U32(object->id);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_PAUSE_MIRROR_TIMER_read(WowWorldReader* reader, vanilla_SMSG_PAUSE_MIRROR_TIMER* object) {
    object->timer = 0;
    READ_U32(object->timer);

    READ_BOOL8(object->is_frozen);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PAUSE_MIRROR_TIMER_write(WowWorldWriter* writer, const vanilla_SMSG_PAUSE_MIRROR_TIMER* object) {
    WRITE_U16_BE(0x0005 + 2); /* size */

    WRITE_U16(0x000001da); /* opcode */

    WRITE_U32(object->timer);

    WRITE_BOOL8(object->is_frozen);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_STOP_MIRROR_TIMER_read(WowWorldReader* reader, vanilla_SMSG_STOP_MIRROR_TIMER* object) {
    object->timer = 0;
    READ_U32(object->timer);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_STOP_MIRROR_TIMER_write(WowWorldWriter* writer, const vanilla_SMSG_STOP_MIRROR_TIMER* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x000001db); /* opcode */

    WRITE_U32(object->timer);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_PING_read(WowWorldReader* reader, vanilla_CMSG_PING* object) {
    READ_U32(object->sequence_id);

    READ_U32(object->round_time_in_ms);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_PING_write(WowWorldWriter* writer, const vanilla_CMSG_PING* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x000001dc); /* opcode */

    WRITE_U32(object->sequence_id);

    WRITE_U32(object->round_time_in_ms);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_PONG_read(WowWorldReader* reader, vanilla_SMSG_PONG* object) {
    READ_U32(object->sequence_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PONG_write(WowWorldWriter* writer, const vanilla_SMSG_PONG* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x000001dd); /* opcode */

    WRITE_U32(object->sequence_id);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_CLEAR_COOLDOWN_read(WowWorldReader* reader, vanilla_SMSG_CLEAR_COOLDOWN* object) {
    READ_U32(object->id);

    READ_U64(object->target);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_CLEAR_COOLDOWN_write(WowWorldWriter* writer, const vanilla_SMSG_CLEAR_COOLDOWN* object) {
    WRITE_U16_BE(0x000c + 2); /* size */

    WRITE_U16(0x000001de); /* opcode */

    WRITE_U32(object->id);

    WRITE_U64(object->target);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_GAMEOBJECT_PAGETEXT_read(WowWorldReader* reader, vanilla_SMSG_GAMEOBJECT_PAGETEXT* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_GAMEOBJECT_PAGETEXT_write(WowWorldWriter* writer, const vanilla_SMSG_GAMEOBJECT_PAGETEXT* object) {
    WRITE_U16_BE(0x0008 + 2); /* size */

    WRITE_U16(0x000001df); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_SETSHEATHED_read(WowWorldReader* reader, vanilla_CMSG_SETSHEATHED* object) {
    object->sheathed = 0;
    READ_U32(object->sheathed);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_SETSHEATHED_write(WowWorldWriter* writer, const vanilla_CMSG_SETSHEATHED* object) {
    WRITE_U16_BE(0x0004 + 4); /* size */

    WRITE_U32(0x000001e0); /* opcode */

    WRITE_U32(object->sheathed);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_SPELL_DELAYED_read(WowWorldReader* reader, vanilla_SMSG_SPELL_DELAYED* object) {
    READ_U64(object->guid);

    READ_U32(object->delay_time);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPELL_DELAYED_write(WowWorldWriter* writer, const vanilla_SMSG_SPELL_DELAYED* object) {
    WRITE_U16_BE(0x000c + 2); /* size */

    WRITE_U16(0x000001e2); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->delay_time);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_ITEM_TIME_UPDATE_read(WowWorldReader* reader, vanilla_SMSG_ITEM_TIME_UPDATE* object) {
    READ_U64(object->guid);

    READ_U32(object->duration);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_ITEM_TIME_UPDATE_write(WowWorldWriter* writer, const vanilla_SMSG_ITEM_TIME_UPDATE* object) {
    WRITE_U16_BE(0x000c + 2); /* size */

    WRITE_U16(0x000001ea); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->duration);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_ITEM_ENCHANT_TIME_UPDATE_read(WowWorldReader* reader, vanilla_SMSG_ITEM_ENCHANT_TIME_UPDATE* object) {
    READ_U64(object->item);

    READ_U32(object->slot);

    READ_U32(object->duration);

    READ_U64(object->player);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_ITEM_ENCHANT_TIME_UPDATE_write(WowWorldWriter* writer, const vanilla_SMSG_ITEM_ENCHANT_TIME_UPDATE* object) {
    WRITE_U16_BE(0x0018 + 2); /* size */

    WRITE_U16(0x000001eb); /* opcode */

    WRITE_U64(object->item);

    WRITE_U32(object->slot);

    WRITE_U32(object->duration);

    WRITE_U64(object->player);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_AUTH_CHALLENGE_read(WowWorldReader* reader, vanilla_SMSG_AUTH_CHALLENGE* object) {
    READ_U32(object->server_seed);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_AUTH_CHALLENGE_write(WowWorldWriter* writer, const vanilla_SMSG_AUTH_CHALLENGE* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x000001ec); /* opcode */

    WRITE_U32(object->server_seed);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_AUTH_SESSION_size(const vanilla_CMSG_AUTH_SESSION* object) {
    size_t _size = 33 + STRING_SIZE(object->username);

    /* C89 scope for compressed size */ {
        unsigned char* addon_info_uncompressed_data = NULL;
        unsigned char* addon_info_compressed_data = NULL;
        uint32_t compressed_i;
        size_t compressed_size = 0;

        WowWorldWriter stack_writer;
        WowWorldWriter* writer = &stack_writer;

        for(compressed_i = 0; compressed_i < object->amount_of_addon_info; ++compressed_i) {
            compressed_size += vanilla_AddonInfo_size(&object->addon_info[compressed_i]);
        }

        if (compressed_size) {
            addon_info_uncompressed_data = malloc(compressed_size);
            addon_info_compressed_data = malloc(compressed_size + WWM_COMPRESS_EXTRA_LENGTH);

            stack_writer = wwm_create_writer(addon_info_uncompressed_data, compressed_size);
            WRITE_ARRAY(object->addon_info, object->amount_of_addon_info, WWM_CHECK_RETURN_CODE(vanilla_AddonInfo_write(writer, &object->addon_info[i])));

            _size += wwm_compress_data(addon_info_uncompressed_data, compressed_size, addon_info_compressed_data, compressed_size + WWM_COMPRESS_EXTRA_LENGTH);

            free(addon_info_uncompressed_data);
            free(addon_info_compressed_data);
        }
    }

    return _size;
}

static WowWorldResult vanilla_CMSG_AUTH_SESSION_read(WowWorldReader* reader, vanilla_CMSG_AUTH_SESSION* object, size_t body_size) {
    size_t _size = 0;

    unsigned char* addon_info_decompressed_data = NULL;
    unsigned char* addon_info_compressed_data = NULL;
    uint32_t addon_info_decompressed_size;

    WowWorldReader new_reader;

    READ_U32(object->build);
    _size += 4;

    READ_U32(object->server_id);
    _size += 4;

    READ_CSTRING(object->username);
    _size += STRING_SIZE(object->username) + 1;

    READ_U32(object->client_seed);
    _size += 4;

    READ_ARRAY_ALLOCATE(object->client_proof, 20, sizeof(uint8_t));
    READ_ARRAY(object->client_proof, 20, READ_U8((*object->client_proof)[i]);_size += 1;);

    if((body_size - _size) > (reader->length - reader->index)) {
        return WWM_RESULT_NOT_ENOUGH_BYTES;
    }

    /* addon_info_decompressed_size: u32 */
    READ_U32(addon_info_decompressed_size);
    _size += 4;

    object->addon_info = NULL;
    if(addon_info_decompressed_size) {
        addon_info_decompressed_data = malloc(addon_info_decompressed_size);

        addon_info_compressed_data = malloc(body_size - _size);

        memcpy(addon_info_compressed_data, &reader->source[reader->index], reader->length - reader->index);

        wwm_decompress_data(addon_info_compressed_data, body_size - _size, addon_info_decompressed_data, addon_info_decompressed_size);
        new_reader = wwm_create_reader(addon_info_decompressed_data, addon_info_decompressed_size);
        reader = &new_reader;

        object->amount_of_addon_info = 0;
        /* C89 scope to allow variable declarations */ {
            int i = 0;
            size_t _current_size = 8 * sizeof(*object->addon_info);

            object->addon_info = malloc(_current_size);
            while ((reader->index + 1) < reader->length) {
                WWM_CHECK_RETURN_CODE(vanilla_AddonInfo_read(reader, &object->addon_info[i]));
                ++i;

                if (i * sizeof(*object->addon_info) >= _current_size) {
                    _current_size *= 2;
                    object->addon_info = realloc(object->addon_info, _current_size);
                }
            }

            object->amount_of_addon_info = i;
        }

        free(addon_info_decompressed_data);
        free(addon_info_compressed_data);
    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_AUTH_SESSION_write(WowWorldWriter* writer, const vanilla_CMSG_AUTH_SESSION* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_AUTH_SESSION_size(object) + 4); /* size */

    WRITE_U32(0x000001ed); /* opcode */

    WRITE_U32(object->build);

    WRITE_U32(object->server_id);

    WRITE_CSTRING(object->username);

    WRITE_U32(object->client_seed);

    WRITE_ARRAY(object->client_proof, 20, WRITE_U8((*object->client_proof)[i]));

    /* C89 Scope for compressed */ {
        unsigned char* addon_info_uncompressed_data = NULL;

        uint32_t compressed_i;
        size_t _size = 0;
        WowWorldWriter new_writer;
        WowWorldWriter* old_writer = writer;

        for(compressed_i = 0; compressed_i < object->amount_of_addon_info; ++compressed_i) {
            _size += vanilla_AddonInfo_size(&object->addon_info[compressed_i]);
        }
        if (_size) {
            WRITE_U32((uint32_t)_size);

            addon_info_uncompressed_data = malloc(_size);
            new_writer = wwm_create_writer(addon_info_uncompressed_data, _size);
            writer = &new_writer;
            WRITE_ARRAY(object->addon_info, object->amount_of_addon_info, WWM_CHECK_RETURN_CODE(vanilla_AddonInfo_write(writer, &object->addon_info[i])));

            wwm_compress_data(addon_info_uncompressed_data, _size, old_writer->destination, old_writer->length - old_writer->index);
            free(addon_info_uncompressed_data);
        }
    }


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_AUTH_SESSION_free(vanilla_CMSG_AUTH_SESSION* object) {
    size_t i;

    FREE_STRING(object->username);

    free(object->client_proof);
    object->client_proof = NULL;
    for (i = 0; i < object->amount_of_addon_info; ++i) {
        vanilla_AddonInfo_free(&((object->addon_info)[i]));
    }
    free(object->addon_info);
    object->addon_info = NULL;
}

static size_t vanilla_SMSG_AUTH_RESPONSE_size(const vanilla_SMSG_AUTH_RESPONSE* object) {
    size_t _size = 1;

    if (object->result == VANILLA_WORLD_RESULT_AUTH_OK) {
        _size += 9;
    }
    else if (object->result == VANILLA_WORLD_RESULT_AUTH_WAIT_QUEUE) {
        _size += 4;
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_AUTH_RESPONSE_read(WowWorldReader* reader, vanilla_SMSG_AUTH_RESPONSE* object) {
    object->result = 0;
    READ_U8(object->result);

    if (object->result == VANILLA_WORLD_RESULT_AUTH_OK) {
        READ_U32(object->billing_time);

        READ_U8(object->billing_flags);

        READ_U32(object->billing_rested);

    }
    else if (object->result == VANILLA_WORLD_RESULT_AUTH_WAIT_QUEUE) {
        READ_U32(object->queue_position);

    }
    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_AUTH_RESPONSE_write(WowWorldWriter* writer, const vanilla_SMSG_AUTH_RESPONSE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_AUTH_RESPONSE_size(object) + 2); /* size */

    WRITE_U16(0x000001ee); /* opcode */

    WRITE_U8(object->result);

    if (object->result == VANILLA_WORLD_RESULT_AUTH_OK) {
        WRITE_U32(object->billing_time);

        WRITE_U8(object->billing_flags);

        WRITE_U32(object->billing_rested);

    }
    else if (object->result == VANILLA_WORLD_RESULT_AUTH_WAIT_QUEUE) {
        WRITE_U32(object->queue_position);

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_AUTH_RESPONSE_free(vanilla_SMSG_AUTH_RESPONSE* object) {
    if (object->result == VANILLA_WORLD_RESULT_AUTH_OK) {
    }
    else if (object->result == VANILLA_WORLD_RESULT_AUTH_WAIT_QUEUE) {
    }
}

static size_t vanilla_CMSG_PET_CAST_SPELL_size(const vanilla_CMSG_PET_CAST_SPELL* object) {
    return 12 + vanilla_SpellCastTargets_size(&object->targets);
}

static WowWorldResult vanilla_CMSG_PET_CAST_SPELL_read(WowWorldReader* reader, vanilla_CMSG_PET_CAST_SPELL* object) {
    READ_U64(object->guid);

    READ_U32(object->id);

    WWM_CHECK_RETURN_CODE(vanilla_SpellCastTargets_read(reader, &object->targets));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_PET_CAST_SPELL_write(WowWorldWriter* writer, const vanilla_CMSG_PET_CAST_SPELL* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_PET_CAST_SPELL_size(object) + 4); /* size */

    WRITE_U32(0x000001f0); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->id);

    WWM_CHECK_RETURN_CODE(vanilla_SpellCastTargets_write(writer, &object->targets));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_PET_CAST_SPELL_free(vanilla_CMSG_PET_CAST_SPELL* object) {
    vanilla_SpellCastTargets_free(&object->targets);
}

static WowWorldResult vanilla_MSG_SAVE_GUILD_EMBLEM_Server_read(WowWorldReader* reader, vanilla_MSG_SAVE_GUILD_EMBLEM_Server* object) {
    object->result = 0;
    READ_U32(object->result);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_SAVE_GUILD_EMBLEM_Server_write(WowWorldWriter* writer, const vanilla_MSG_SAVE_GUILD_EMBLEM_Server* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x000001f1); /* opcode */

    WRITE_U32(object->result);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_MSG_SAVE_GUILD_EMBLEM_Client_read(WowWorldReader* reader, vanilla_MSG_SAVE_GUILD_EMBLEM_Client* object) {
    READ_U64(object->vendor);

    READ_U32(object->emblem_style);

    READ_U32(object->emblem_color);

    READ_U32(object->border_style);

    READ_U32(object->border_color);

    READ_U32(object->background_color);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_SAVE_GUILD_EMBLEM_Client_write(WowWorldWriter* writer, const vanilla_MSG_SAVE_GUILD_EMBLEM_Client* object) {
    WRITE_U16_BE(0x001c + 4); /* size */

    WRITE_U32(0x000001f1); /* opcode */

    WRITE_U64(object->vendor);

    WRITE_U32(object->emblem_style);

    WRITE_U32(object->emblem_color);

    WRITE_U32(object->border_style);

    WRITE_U32(object->border_color);

    WRITE_U32(object->background_color);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_MSG_TABARDVENDOR_ACTIVATE_read(WowWorldReader* reader, vanilla_MSG_TABARDVENDOR_ACTIVATE* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_TABARDVENDOR_ACTIVATE_cmsg_write(WowWorldWriter* writer, const vanilla_MSG_TABARDVENDOR_ACTIVATE* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x000001f2); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_TABARDVENDOR_ACTIVATE_smsg_write(WowWorldWriter* writer, const vanilla_MSG_TABARDVENDOR_ACTIVATE* object) {
    WRITE_U16_BE(0x0008 + 2); /* size */

    WRITE_U16(0x000001f2); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_PLAY_SPELL_VISUAL_read(WowWorldReader* reader, vanilla_SMSG_PLAY_SPELL_VISUAL* object) {
    READ_U64(object->guid);

    READ_U32(object->spell_art_kit);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PLAY_SPELL_VISUAL_write(WowWorldWriter* writer, const vanilla_SMSG_PLAY_SPELL_VISUAL* object) {
    WRITE_U16_BE(0x000c + 2); /* size */

    WRITE_U16(0x000001f3); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->spell_art_kit);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_ZONEUPDATE_read(WowWorldReader* reader, vanilla_CMSG_ZONEUPDATE* object) {
    object->area = 0;
    READ_U32(object->area);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_ZONEUPDATE_write(WowWorldWriter* writer, const vanilla_CMSG_ZONEUPDATE* object) {
    WRITE_U16_BE(0x0004 + 4); /* size */

    WRITE_U32(0x000001f4); /* opcode */

    WRITE_U32(object->area);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_PARTYKILLLOG_read(WowWorldReader* reader, vanilla_SMSG_PARTYKILLLOG* object) {
    READ_U64(object->player_with_killing_blow);

    READ_U64(object->victim);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PARTYKILLLOG_write(WowWorldWriter* writer, const vanilla_SMSG_PARTYKILLLOG* object) {
    WRITE_U16_BE(0x0010 + 2); /* size */

    WRITE_U16(0x000001f5); /* opcode */

    WRITE_U64(object->player_with_killing_blow);

    WRITE_U64(object->victim);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_COMPRESSED_UPDATE_OBJECT_size(const vanilla_SMSG_COMPRESSED_UPDATE_OBJECT* object) {
    size_t _size = 5;

    /* C89 scope to allow variable declarations */ {
        int i;
        for(i = 0; i < (int)object->amount_of_objects; ++i) {
            _size += vanilla_Object_size(&object->objects[i]);
        }
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_COMPRESSED_UPDATE_OBJECT_read(WowWorldReader* reader, vanilla_SMSG_COMPRESSED_UPDATE_OBJECT* object, size_t body_size) {
    size_t _size = 0;

    WowWorldReader stack_reader;
    unsigned char* _compressed_data = NULL;
    uint32_t _decompressed_size;
    READ_U32(_decompressed_size);
    _size += 4;

    _compressed_data = malloc(_decompressed_size);

    if (!wwm_decompress_data(&reader->source[reader->index], body_size - _size, _compressed_data, _decompressed_size)) {
        return WWM_RESULT_COMPRESSION_ERROR;
    }

    stack_reader = wwm_create_reader(_compressed_data, _decompressed_size);
    reader = &stack_reader;

    READ_U32(object->amount_of_objects);
    _size += 4;

    READ_U8(object->has_transport);
    _size += 1;

    READ_ARRAY_ALLOCATE(object->objects, object->amount_of_objects, sizeof(vanilla_Object));
    READ_ARRAY(object->objects, object->amount_of_objects, WWM_CHECK_RETURN_CODE(vanilla_Object_read(reader, &object->objects[i]));_size += vanilla_Object_size(&object->objects[i]););

    free(_compressed_data);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_COMPRESSED_UPDATE_OBJECT_write(WowWorldWriter* writer, const vanilla_SMSG_COMPRESSED_UPDATE_OBJECT* object) {
    WowWorldWriter* old_writer = writer;
    unsigned char* _decompressed_data;
    WowWorldWriter stack_writer;
    size_t _compressed_data_length;
    size_t saved_writer_index;
    const uint32_t _decompressed_data_length = (uint32_t)vanilla_SMSG_COMPRESSED_UPDATE_OBJECT_size(object);


    WRITE_U16_BE(0 /* place holder */ + 2); /* size */

    WRITE_U16(0x000001f6); /* opcode */

    WRITE_U32(_decompressed_data_length);
    writer = &stack_writer;

    if (_decompressed_data_length == 0) {
        return WWM_RESULT_SUCCESS;
    }

    _decompressed_data = malloc(_decompressed_data_length);
    stack_writer = wwm_create_writer(_decompressed_data, _decompressed_data_length);

    WRITE_U32(object->amount_of_objects);

    WRITE_U8(object->has_transport);

    WRITE_ARRAY(object->objects, object->amount_of_objects, WWM_CHECK_RETURN_CODE(vanilla_Object_write(writer, &object->objects[i])));

    writer = old_writer;
    _compressed_data_length = wwm_compress_data(stack_writer.destination, stack_writer.length, &writer->destination[writer->index], writer->length - writer->index);
    writer->index += _compressed_data_length;
    saved_writer_index = writer->index;
    writer->index = 0;

    WRITE_U16_BE((uint16_t)(_compressed_data_length + 4 + 4)); /* size */

    writer->index = saved_writer_index;
    free(_decompressed_data);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_COMPRESSED_UPDATE_OBJECT_free(vanilla_SMSG_COMPRESSED_UPDATE_OBJECT* object) {
    size_t i;

    for (i = 0; i < object->amount_of_objects; ++i) {
        vanilla_Object_free(&((object->objects)[i]));
    }
    free(object->objects);
    object->objects = NULL;
}

static WowWorldResult vanilla_SMSG_PLAY_SPELL_IMPACT_read(WowWorldReader* reader, vanilla_SMSG_PLAY_SPELL_IMPACT* object) {
    READ_U64(object->guid);

    READ_U32(object->spell_visual_kit);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PLAY_SPELL_IMPACT_write(WowWorldWriter* writer, const vanilla_SMSG_PLAY_SPELL_IMPACT* object) {
    WRITE_U16_BE(0x000c + 2); /* size */

    WRITE_U16(0x000001f7); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->spell_visual_kit);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_EXPLORATION_EXPERIENCE_read(WowWorldReader* reader, vanilla_SMSG_EXPLORATION_EXPERIENCE* object) {
    object->area = 0;
    READ_U32(object->area);

    READ_U32(object->experience);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_EXPLORATION_EXPERIENCE_write(WowWorldWriter* writer, const vanilla_SMSG_EXPLORATION_EXPERIENCE* object) {
    WRITE_U16_BE(0x0008 + 2); /* size */

    WRITE_U16(0x000001f8); /* opcode */

    WRITE_U32(object->area);

    WRITE_U32(object->experience);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_MSG_RANDOM_ROLL_Client_read(WowWorldReader* reader, vanilla_MSG_RANDOM_ROLL_Client* object) {
    READ_U32(object->minimum);

    READ_U32(object->maximum);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_RANDOM_ROLL_Client_write(WowWorldWriter* writer, const vanilla_MSG_RANDOM_ROLL_Client* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x000001fb); /* opcode */

    WRITE_U32(object->minimum);

    WRITE_U32(object->maximum);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_MSG_RANDOM_ROLL_Server_read(WowWorldReader* reader, vanilla_MSG_RANDOM_ROLL_Server* object) {
    READ_U32(object->minimum);

    READ_U32(object->maximum);

    READ_U32(object->actual_roll);

    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_RANDOM_ROLL_Server_write(WowWorldWriter* writer, const vanilla_MSG_RANDOM_ROLL_Server* object) {
    WRITE_U16_BE(0x0014 + 2); /* size */

    WRITE_U16(0x000001fb); /* opcode */

    WRITE_U32(object->minimum);

    WRITE_U32(object->maximum);

    WRITE_U32(object->actual_roll);

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_ENVIRONMENTAL_DAMAGE_LOG_read(WowWorldReader* reader, vanilla_SMSG_ENVIRONMENTAL_DAMAGE_LOG* object) {
    READ_U64(object->guid);

    object->damage_type = 0;
    READ_U8(object->damage_type);

    READ_U32(object->damage);

    READ_U32(object->absorb);

    READ_U32(object->resist);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_ENVIRONMENTAL_DAMAGE_LOG_write(WowWorldWriter* writer, const vanilla_SMSG_ENVIRONMENTAL_DAMAGE_LOG* object) {
    WRITE_U16_BE(0x0015 + 2); /* size */

    WRITE_U16(0x000001fc); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U8(object->damage_type);

    WRITE_U32(object->damage);

    WRITE_U32(object->absorb);

    WRITE_U32(object->resist);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_LOOKING_FOR_GROUP_Client_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x000001ff); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_MSG_LOOKING_FOR_GROUP_Server_read(WowWorldReader* reader, vanilla_MSG_LOOKING_FOR_GROUP_Server* object) {
    READ_U32(object->unknown1);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_LOOKING_FOR_GROUP_Server_write(WowWorldWriter* writer, const vanilla_MSG_LOOKING_FOR_GROUP_Server* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x000001ff); /* opcode */

    WRITE_U32(object->unknown1);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_UNLEARN_SKILL_read(WowWorldReader* reader, vanilla_CMSG_UNLEARN_SKILL* object) {
    object->skill = 0;
    READ_U32(object->skill);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_UNLEARN_SKILL_write(WowWorldWriter* writer, const vanilla_CMSG_UNLEARN_SKILL* object) {
    WRITE_U16_BE(0x0004 + 4); /* size */

    WRITE_U32(0x00000202); /* opcode */

    WRITE_U32(object->skill);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_REMOVED_SPELL_read(WowWorldReader* reader, vanilla_SMSG_REMOVED_SPELL* object) {
    READ_U16(object->spell);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_REMOVED_SPELL_write(WowWorldWriter* writer, const vanilla_SMSG_REMOVED_SPELL* object) {
    WRITE_U16_BE(0x0002 + 2); /* size */

    WRITE_U16(0x00000203); /* opcode */

    WRITE_U16(object->spell);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_GMTICKET_CREATE_size(const vanilla_CMSG_GMTICKET_CREATE* object) {
    size_t _size = 19 + STRING_SIZE(object->message) + STRING_SIZE(object->reserved_for_future_use);

    if (object->category == VANILLA_GM_TICKET_TYPE_BEHAVIOR_HARASSMENT) {
        _size += 4;

        /* C89 scope for compressed size */ {
            unsigned char* compressed_chat_data_uncompressed_data = NULL;
            unsigned char* compressed_chat_data_compressed_data = NULL;
            uint32_t compressed_i;
            size_t compressed_size = 0;

            WowWorldWriter stack_writer;
            WowWorldWriter* writer = &stack_writer;

            for(compressed_i = 0; compressed_i < object->amount_of_compressed_chat_data; ++compressed_i) {
                compressed_size += 1;
            }

            if (compressed_size) {
                compressed_chat_data_uncompressed_data = malloc(compressed_size);
                compressed_chat_data_compressed_data = malloc(compressed_size + WWM_COMPRESS_EXTRA_LENGTH);

                stack_writer = wwm_create_writer(compressed_chat_data_uncompressed_data, compressed_size);
                WRITE_ARRAY(object->compressed_chat_data, object->amount_of_compressed_chat_data, WRITE_U8(object->compressed_chat_data[i]));

                _size += wwm_compress_data(compressed_chat_data_uncompressed_data, compressed_size, compressed_chat_data_compressed_data, compressed_size + WWM_COMPRESS_EXTRA_LENGTH);

                free(compressed_chat_data_uncompressed_data);
                free(compressed_chat_data_compressed_data);
            }
        }

    }

    return _size;
}

static WowWorldResult vanilla_CMSG_GMTICKET_CREATE_read(WowWorldReader* reader, vanilla_CMSG_GMTICKET_CREATE* object, size_t body_size) {
    size_t _size = 0;

    unsigned char* compressed_chat_data_decompressed_data = NULL;
    unsigned char* compressed_chat_data_compressed_data = NULL;
    uint32_t compressed_chat_data_decompressed_size;

    WowWorldReader new_reader;

    object->category = 0;
    READ_U8(object->category);
    _size += 1;

    object->map = 0;
    READ_U32(object->map);
    _size += 4;

    WWM_CHECK_RETURN_CODE(all_Vector3d_read(reader, &object->position));
    _size += 12;

    READ_CSTRING(object->message);
    _size += STRING_SIZE(object->message) + 1;

    READ_CSTRING(object->reserved_for_future_use);
    _size += STRING_SIZE(object->reserved_for_future_use) + 1;

    if (object->category == VANILLA_GM_TICKET_TYPE_BEHAVIOR_HARASSMENT) {
        READ_U32(object->chat_data_line_count);
        _size += 4;

        if((body_size - _size) > (reader->length - reader->index)) {
            return WWM_RESULT_NOT_ENOUGH_BYTES;
        }

        /* compressed_chat_data_decompressed_size: u32 */
        READ_U32(compressed_chat_data_decompressed_size);
        _size += 4;

        object->compressed_chat_data = NULL;
        if(compressed_chat_data_decompressed_size) {
            compressed_chat_data_decompressed_data = malloc(compressed_chat_data_decompressed_size);

            compressed_chat_data_compressed_data = malloc(body_size - _size);

            memcpy(compressed_chat_data_compressed_data, &reader->source[reader->index], reader->length - reader->index);

            wwm_decompress_data(compressed_chat_data_compressed_data, body_size - _size, compressed_chat_data_decompressed_data, compressed_chat_data_decompressed_size);
            new_reader = wwm_create_reader(compressed_chat_data_decompressed_data, compressed_chat_data_decompressed_size);
            reader = &new_reader;

            object->amount_of_compressed_chat_data = 0;
            /* C89 scope to allow variable declarations */ {
                int i = 0;
                size_t _current_size = 8 * sizeof(*object->compressed_chat_data);

                object->compressed_chat_data = malloc(_current_size);
                while ((reader->index + 1) < reader->length) {
                    READ_U8(object->compressed_chat_data[i]);
                    ++i;

                    if (i * sizeof(*object->compressed_chat_data) >= _current_size) {
                        _current_size *= 2;
                        object->compressed_chat_data = realloc(object->compressed_chat_data, _current_size);
                    }
                }

                object->amount_of_compressed_chat_data = i;
            }

        }
        free(compressed_chat_data_decompressed_data);
        free(compressed_chat_data_compressed_data);
    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GMTICKET_CREATE_write(WowWorldWriter* writer, const vanilla_CMSG_GMTICKET_CREATE* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_GMTICKET_CREATE_size(object) + 4); /* size */

    WRITE_U32(0x00000205); /* opcode */

    WRITE_U8(object->category);

    WRITE_U32(object->map);

    WWM_CHECK_RETURN_CODE(all_Vector3d_write(writer, &object->position));

    WRITE_CSTRING(object->message);

    WRITE_CSTRING(object->reserved_for_future_use);

    if (object->category == VANILLA_GM_TICKET_TYPE_BEHAVIOR_HARASSMENT) {
        WRITE_U32(object->chat_data_line_count);

        /* C89 Scope for compressed */ {
            unsigned char* compressed_chat_data_uncompressed_data = NULL;

            uint32_t compressed_i;
            size_t _size = 0;
            WowWorldWriter new_writer;
            WowWorldWriter* old_writer = writer;

            for(compressed_i = 0; compressed_i < object->amount_of_compressed_chat_data; ++compressed_i) {
                _size += 1;
            }
            if (_size) {
                WRITE_U32((uint32_t)_size);

                compressed_chat_data_uncompressed_data = malloc(_size);
                new_writer = wwm_create_writer(compressed_chat_data_uncompressed_data, _size);
                writer = &new_writer;
                WRITE_ARRAY(object->compressed_chat_data, object->amount_of_compressed_chat_data, WRITE_U8(object->compressed_chat_data[i]));

                wwm_compress_data(compressed_chat_data_uncompressed_data, _size, old_writer->destination, old_writer->length - old_writer->index);
                free(compressed_chat_data_uncompressed_data);
            }
        }

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_GMTICKET_CREATE_free(vanilla_CMSG_GMTICKET_CREATE* object) {
    FREE_STRING(object->message);

    FREE_STRING(object->reserved_for_future_use);

    if (object->category == VANILLA_GM_TICKET_TYPE_BEHAVIOR_HARASSMENT) {
        free(object->compressed_chat_data);
        object->compressed_chat_data = NULL;
    }
}

static WowWorldResult vanilla_SMSG_GMTICKET_CREATE_read(WowWorldReader* reader, vanilla_SMSG_GMTICKET_CREATE* object) {
    object->response = 0;
    READ_U32(object->response);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_GMTICKET_CREATE_write(WowWorldWriter* writer, const vanilla_SMSG_GMTICKET_CREATE* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x00000206); /* opcode */

    WRITE_U32(object->response);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_GMTICKET_UPDATETEXT_size(const vanilla_CMSG_GMTICKET_UPDATETEXT* object) {
    return 2 + STRING_SIZE(object->message);
}

static WowWorldResult vanilla_CMSG_GMTICKET_UPDATETEXT_read(WowWorldReader* reader, vanilla_CMSG_GMTICKET_UPDATETEXT* object) {
    object->ticket_type = 0;
    READ_U8(object->ticket_type);

    READ_CSTRING(object->message);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GMTICKET_UPDATETEXT_write(WowWorldWriter* writer, const vanilla_CMSG_GMTICKET_UPDATETEXT* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_GMTICKET_UPDATETEXT_size(object) + 4); /* size */

    WRITE_U32(0x00000207); /* opcode */

    WRITE_U8(object->ticket_type);

    WRITE_CSTRING(object->message);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_GMTICKET_UPDATETEXT_free(vanilla_CMSG_GMTICKET_UPDATETEXT* object) {
    FREE_STRING(object->message);

}

static WowWorldResult vanilla_SMSG_GMTICKET_UPDATETEXT_read(WowWorldReader* reader, vanilla_SMSG_GMTICKET_UPDATETEXT* object) {
    object->response = 0;
    READ_U32(object->response);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_GMTICKET_UPDATETEXT_write(WowWorldWriter* writer, const vanilla_SMSG_GMTICKET_UPDATETEXT* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x00000208); /* opcode */

    WRITE_U32(object->response);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_ACCOUNT_DATA_TIMES_read(WowWorldReader* reader, vanilla_SMSG_ACCOUNT_DATA_TIMES* object) {
    READ_ARRAY_ALLOCATE(object->data, 32, sizeof(uint32_t));
    READ_ARRAY(object->data, 32, READ_U32((*object->data)[i]));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_ACCOUNT_DATA_TIMES_write(WowWorldWriter* writer, const vanilla_SMSG_ACCOUNT_DATA_TIMES* object) {
    WRITE_U16_BE(0x0080 + 2); /* size */

    WRITE_U16(0x00000209); /* opcode */

    WRITE_ARRAY(object->data, 32, WRITE_U32((*object->data)[i]));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_ACCOUNT_DATA_TIMES_free(vanilla_SMSG_ACCOUNT_DATA_TIMES* object) {
    free(object->data);
    object->data = NULL;
}

static WowWorldResult vanilla_CMSG_REQUEST_ACCOUNT_DATA_read(WowWorldReader* reader, vanilla_CMSG_REQUEST_ACCOUNT_DATA* object) {
    READ_U32(object->data_type);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_REQUEST_ACCOUNT_DATA_write(WowWorldWriter* writer, const vanilla_CMSG_REQUEST_ACCOUNT_DATA* object) {
    WRITE_U16_BE(0x0004 + 4); /* size */

    WRITE_U32(0x0000020a); /* opcode */

    WRITE_U32(object->data_type);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_UPDATE_ACCOUNT_DATA_size(const vanilla_CMSG_UPDATE_ACCOUNT_DATA* object) {
    size_t _size = 4;

    /* C89 scope for compressed size */ {
        unsigned char* compressed_data_uncompressed_data = NULL;
        unsigned char* compressed_data_compressed_data = NULL;
        uint32_t compressed_i;
        size_t compressed_size = 0;

        WowWorldWriter stack_writer;
        WowWorldWriter* writer = &stack_writer;

        for(compressed_i = 0; compressed_i < object->amount_of_compressed_data; ++compressed_i) {
            compressed_size += 1;
        }

        if (compressed_size) {
            compressed_data_uncompressed_data = malloc(compressed_size);
            compressed_data_compressed_data = malloc(compressed_size + WWM_COMPRESS_EXTRA_LENGTH);

            stack_writer = wwm_create_writer(compressed_data_uncompressed_data, compressed_size);
            WRITE_ARRAY(object->compressed_data, object->amount_of_compressed_data, WRITE_U8(object->compressed_data[i]));

            _size += wwm_compress_data(compressed_data_uncompressed_data, compressed_size, compressed_data_compressed_data, compressed_size + WWM_COMPRESS_EXTRA_LENGTH);

            free(compressed_data_uncompressed_data);
            free(compressed_data_compressed_data);
        }
    }

    return _size;
}

static WowWorldResult vanilla_CMSG_UPDATE_ACCOUNT_DATA_read(WowWorldReader* reader, vanilla_CMSG_UPDATE_ACCOUNT_DATA* object, size_t body_size) {
    size_t _size = 0;

    unsigned char* compressed_data_decompressed_data = NULL;
    unsigned char* compressed_data_compressed_data = NULL;
    uint32_t compressed_data_decompressed_size;

    WowWorldReader new_reader;

    object->data_type = 0;
    READ_U32(object->data_type);
    _size += 4;

    if((body_size - _size) > (reader->length - reader->index)) {
        return WWM_RESULT_NOT_ENOUGH_BYTES;
    }

    /* compressed_data_decompressed_size: u32 */
    READ_U32(compressed_data_decompressed_size);
    _size += 4;

    object->compressed_data = NULL;
    if(compressed_data_decompressed_size) {
        compressed_data_decompressed_data = malloc(compressed_data_decompressed_size);

        compressed_data_compressed_data = malloc(body_size - _size);

        memcpy(compressed_data_compressed_data, &reader->source[reader->index], reader->length - reader->index);

        wwm_decompress_data(compressed_data_compressed_data, body_size - _size, compressed_data_decompressed_data, compressed_data_decompressed_size);
        new_reader = wwm_create_reader(compressed_data_decompressed_data, compressed_data_decompressed_size);
        reader = &new_reader;

        object->amount_of_compressed_data = 0;
        /* C89 scope to allow variable declarations */ {
            int i = 0;
            size_t _current_size = 8 * sizeof(*object->compressed_data);

            object->compressed_data = malloc(_current_size);
            while ((reader->index + 1) < reader->length) {
                READ_U8(object->compressed_data[i]);
                ++i;

                if (i * sizeof(*object->compressed_data) >= _current_size) {
                    _current_size *= 2;
                    object->compressed_data = realloc(object->compressed_data, _current_size);
                }
            }

            object->amount_of_compressed_data = i;
        }

        free(compressed_data_decompressed_data);
        free(compressed_data_compressed_data);
    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_UPDATE_ACCOUNT_DATA_write(WowWorldWriter* writer, const vanilla_CMSG_UPDATE_ACCOUNT_DATA* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_UPDATE_ACCOUNT_DATA_size(object) + 4); /* size */

    WRITE_U32(0x0000020b); /* opcode */

    WRITE_U32(object->data_type);

    /* C89 Scope for compressed */ {
        unsigned char* compressed_data_uncompressed_data = NULL;

        uint32_t compressed_i;
        size_t _size = 0;
        WowWorldWriter new_writer;
        WowWorldWriter* old_writer = writer;

        for(compressed_i = 0; compressed_i < object->amount_of_compressed_data; ++compressed_i) {
            _size += 1;
        }
        if (_size) {
            WRITE_U32((uint32_t)_size);

            compressed_data_uncompressed_data = malloc(_size);
            new_writer = wwm_create_writer(compressed_data_uncompressed_data, _size);
            writer = &new_writer;
            WRITE_ARRAY(object->compressed_data, object->amount_of_compressed_data, WRITE_U8(object->compressed_data[i]));

            wwm_compress_data(compressed_data_uncompressed_data, _size, old_writer->destination, old_writer->length - old_writer->index);
            free(compressed_data_uncompressed_data);
        }
    }


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_UPDATE_ACCOUNT_DATA_free(vanilla_CMSG_UPDATE_ACCOUNT_DATA* object) {
    free(object->compressed_data);
    object->compressed_data = NULL;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GMTICKET_GETTICKET_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x00000211); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_GMTICKET_GETTICKET_size(const vanilla_SMSG_GMTICKET_GETTICKET* object) {
    size_t _size = 4;

    if (object->status == VANILLA_GM_TICKET_STATUS_HAS_TEXT) {
        _size += 16 + STRING_SIZE(object->text);
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_GMTICKET_GETTICKET_read(WowWorldReader* reader, vanilla_SMSG_GMTICKET_GETTICKET* object) {
    object->status = 0;
    READ_U32(object->status);

    if (object->status == VANILLA_GM_TICKET_STATUS_HAS_TEXT) {
        READ_CSTRING(object->text);

        object->ticket_type = 0;
        READ_U8(object->ticket_type);

        READ_FLOAT(object->days_since_ticket_creation);

        READ_FLOAT(object->days_since_oldest_ticket_creation);

        READ_FLOAT(object->days_since_last_updated);

        object->escalation_status = 0;
        READ_U8(object->escalation_status);

        READ_BOOL8(object->read_by_gm);

    }
    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_GMTICKET_GETTICKET_write(WowWorldWriter* writer, const vanilla_SMSG_GMTICKET_GETTICKET* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_GMTICKET_GETTICKET_size(object) + 2); /* size */

    WRITE_U16(0x00000212); /* opcode */

    WRITE_U32(object->status);

    if (object->status == VANILLA_GM_TICKET_STATUS_HAS_TEXT) {
        WRITE_CSTRING(object->text);

        WRITE_U8(object->ticket_type);

        WRITE_FLOAT(object->days_since_ticket_creation);

        WRITE_FLOAT(object->days_since_oldest_ticket_creation);

        WRITE_FLOAT(object->days_since_last_updated);

        WRITE_U8(object->escalation_status);

        WRITE_BOOL8(object->read_by_gm);

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_GMTICKET_GETTICKET_free(vanilla_SMSG_GMTICKET_GETTICKET* object) {
    if (object->status == VANILLA_GM_TICKET_STATUS_HAS_TEXT) {
        FREE_STRING(object->text);

    }
}

static WowWorldResult vanilla_SMSG_GAMEOBJECT_SPAWN_ANIM_read(WowWorldReader* reader, vanilla_SMSG_GAMEOBJECT_SPAWN_ANIM* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_GAMEOBJECT_SPAWN_ANIM_write(WowWorldWriter* writer, const vanilla_SMSG_GAMEOBJECT_SPAWN_ANIM* object) {
    WRITE_U16_BE(0x0008 + 2); /* size */

    WRITE_U16(0x00000214); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_GAMEOBJECT_DESPAWN_ANIM_read(WowWorldReader* reader, vanilla_SMSG_GAMEOBJECT_DESPAWN_ANIM* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_GAMEOBJECT_DESPAWN_ANIM_write(WowWorldWriter* writer, const vanilla_SMSG_GAMEOBJECT_DESPAWN_ANIM* object) {
    WRITE_U16_BE(0x0008 + 2); /* size */

    WRITE_U16(0x00000215); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_CORPSE_QUERY_Client_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x00000216); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_MSG_CORPSE_QUERY_Server_size(const vanilla_MSG_CORPSE_QUERY_Server* object) {
    size_t _size = 1;

    if (object->result == VANILLA_CORPSE_QUERY_RESULT_FOUND) {
        _size += 20;
    }

    return _size;
}

static WowWorldResult vanilla_MSG_CORPSE_QUERY_Server_read(WowWorldReader* reader, vanilla_MSG_CORPSE_QUERY_Server* object) {
    object->result = 0;
    READ_U8(object->result);

    if (object->result == VANILLA_CORPSE_QUERY_RESULT_FOUND) {
        object->map = 0;
        READ_U32(object->map);

        WWM_CHECK_RETURN_CODE(all_Vector3d_read(reader, &object->position));

        object->corpse_map = 0;
        READ_U32(object->corpse_map);

    }
    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_CORPSE_QUERY_Server_write(WowWorldWriter* writer, const vanilla_MSG_CORPSE_QUERY_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_CORPSE_QUERY_Server_size(object) + 2); /* size */

    WRITE_U16(0x00000216); /* opcode */

    WRITE_U8(object->result);

    if (object->result == VANILLA_CORPSE_QUERY_RESULT_FOUND) {
        WRITE_U32(object->map);

        WWM_CHECK_RETURN_CODE(all_Vector3d_write(writer, &object->position));

        WRITE_U32(object->corpse_map);

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_CORPSE_QUERY_Server_free(vanilla_MSG_CORPSE_QUERY_Server* object) {
    if (object->result == VANILLA_CORPSE_QUERY_RESULT_FOUND) {
    }
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GMTICKET_DELETETICKET_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x00000217); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_GMTICKET_DELETETICKET_read(WowWorldReader* reader, vanilla_SMSG_GMTICKET_DELETETICKET* object) {
    object->response = 0;
    READ_U32(object->response);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_GMTICKET_DELETETICKET_write(WowWorldWriter* writer, const vanilla_SMSG_GMTICKET_DELETETICKET* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x00000218); /* opcode */

    WRITE_U32(object->response);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_CHAT_WRONG_FACTION_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 2); /* size */

    WRITE_U16(0x00000219); /* opcode */


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GMTICKET_SYSTEMSTATUS_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x0000021a); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_GMTICKET_SYSTEMSTATUS_read(WowWorldReader* reader, vanilla_SMSG_GMTICKET_SYSTEMSTATUS* object) {
    object->will_accept_tickets = 0;
    READ_U32(object->will_accept_tickets);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_GMTICKET_SYSTEMSTATUS_write(WowWorldWriter* writer, const vanilla_SMSG_GMTICKET_SYSTEMSTATUS* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x0000021b); /* opcode */

    WRITE_U32(object->will_accept_tickets);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_SPIRIT_HEALER_ACTIVATE_read(WowWorldReader* reader, vanilla_CMSG_SPIRIT_HEALER_ACTIVATE* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_SPIRIT_HEALER_ACTIVATE_write(WowWorldWriter* writer, const vanilla_CMSG_SPIRIT_HEALER_ACTIVATE* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x0000021c); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_SET_REST_START_read(WowWorldReader* reader, vanilla_SMSG_SET_REST_START* object) {
    READ_U32(object->unknown1);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SET_REST_START_write(WowWorldWriter* writer, const vanilla_SMSG_SET_REST_START* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x0000021e); /* opcode */

    WRITE_U32(object->unknown1);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_SPIRIT_HEALER_CONFIRM_read(WowWorldReader* reader, vanilla_SMSG_SPIRIT_HEALER_CONFIRM* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPIRIT_HEALER_CONFIRM_write(WowWorldWriter* writer, const vanilla_SMSG_SPIRIT_HEALER_CONFIRM* object) {
    WRITE_U16_BE(0x0008 + 2); /* size */

    WRITE_U16(0x00000222); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_GOSSIP_POI_size(const vanilla_SMSG_GOSSIP_POI* object) {
    return 21 + STRING_SIZE(object->location_name);
}

static WowWorldResult vanilla_SMSG_GOSSIP_POI_read(WowWorldReader* reader, vanilla_SMSG_GOSSIP_POI* object) {
    READ_U32(object->flags);

    WWM_CHECK_RETURN_CODE(all_Vector2d_read(reader, &object->position));

    READ_U32(object->icon);

    READ_U32(object->data);

    READ_CSTRING(object->location_name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_GOSSIP_POI_write(WowWorldWriter* writer, const vanilla_SMSG_GOSSIP_POI* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_GOSSIP_POI_size(object) + 2); /* size */

    WRITE_U16(0x00000224); /* opcode */

    WRITE_U32(object->flags);

    WWM_CHECK_RETURN_CODE(all_Vector2d_write(writer, &object->position));

    WRITE_U32(object->icon);

    WRITE_U32(object->data);

    WRITE_CSTRING(object->location_name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_GOSSIP_POI_free(vanilla_SMSG_GOSSIP_POI* object) {
    FREE_STRING(object->location_name);

}

static WowWorldResult vanilla_CMSG_CHAT_IGNORED_read(WowWorldReader* reader, vanilla_CMSG_CHAT_IGNORED* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_CHAT_IGNORED_write(WowWorldWriter* writer, const vanilla_CMSG_CHAT_IGNORED* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x00000225); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_GUILD_RANK_size(const vanilla_CMSG_GUILD_RANK* object) {
    return 9 + STRING_SIZE(object->rank_name);
}

static WowWorldResult vanilla_CMSG_GUILD_RANK_read(WowWorldReader* reader, vanilla_CMSG_GUILD_RANK* object) {
    READ_U32(object->rank_id);

    READ_U32(object->rights);

    READ_CSTRING(object->rank_name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GUILD_RANK_write(WowWorldWriter* writer, const vanilla_CMSG_GUILD_RANK* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_GUILD_RANK_size(object) + 4); /* size */

    WRITE_U32(0x00000231); /* opcode */

    WRITE_U32(object->rank_id);

    WRITE_U32(object->rights);

    WRITE_CSTRING(object->rank_name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_GUILD_RANK_free(vanilla_CMSG_GUILD_RANK* object) {
    FREE_STRING(object->rank_name);

}

static size_t vanilla_CMSG_GUILD_ADD_RANK_size(const vanilla_CMSG_GUILD_ADD_RANK* object) {
    return 1 + STRING_SIZE(object->rank_name);
}

static WowWorldResult vanilla_CMSG_GUILD_ADD_RANK_read(WowWorldReader* reader, vanilla_CMSG_GUILD_ADD_RANK* object) {
    READ_CSTRING(object->rank_name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GUILD_ADD_RANK_write(WowWorldWriter* writer, const vanilla_CMSG_GUILD_ADD_RANK* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_GUILD_ADD_RANK_size(object) + 4); /* size */

    WRITE_U32(0x00000232); /* opcode */

    WRITE_CSTRING(object->rank_name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_GUILD_ADD_RANK_free(vanilla_CMSG_GUILD_ADD_RANK* object) {
    FREE_STRING(object->rank_name);

}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GUILD_DEL_RANK_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x00000233); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_GUILD_SET_PUBLIC_NOTE_size(const vanilla_CMSG_GUILD_SET_PUBLIC_NOTE* object) {
    return 2 + STRING_SIZE(object->player_name) + STRING_SIZE(object->note);
}

static WowWorldResult vanilla_CMSG_GUILD_SET_PUBLIC_NOTE_read(WowWorldReader* reader, vanilla_CMSG_GUILD_SET_PUBLIC_NOTE* object) {
    READ_CSTRING(object->player_name);

    READ_CSTRING(object->note);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GUILD_SET_PUBLIC_NOTE_write(WowWorldWriter* writer, const vanilla_CMSG_GUILD_SET_PUBLIC_NOTE* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_GUILD_SET_PUBLIC_NOTE_size(object) + 4); /* size */

    WRITE_U32(0x00000234); /* opcode */

    WRITE_CSTRING(object->player_name);

    WRITE_CSTRING(object->note);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_GUILD_SET_PUBLIC_NOTE_free(vanilla_CMSG_GUILD_SET_PUBLIC_NOTE* object) {
    FREE_STRING(object->player_name);

    FREE_STRING(object->note);

}

static size_t vanilla_CMSG_GUILD_SET_OFFICER_NOTE_size(const vanilla_CMSG_GUILD_SET_OFFICER_NOTE* object) {
    return 2 + STRING_SIZE(object->player_name) + STRING_SIZE(object->note);
}

static WowWorldResult vanilla_CMSG_GUILD_SET_OFFICER_NOTE_read(WowWorldReader* reader, vanilla_CMSG_GUILD_SET_OFFICER_NOTE* object) {
    READ_CSTRING(object->player_name);

    READ_CSTRING(object->note);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GUILD_SET_OFFICER_NOTE_write(WowWorldWriter* writer, const vanilla_CMSG_GUILD_SET_OFFICER_NOTE* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_GUILD_SET_OFFICER_NOTE_size(object) + 4); /* size */

    WRITE_U32(0x00000235); /* opcode */

    WRITE_CSTRING(object->player_name);

    WRITE_CSTRING(object->note);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_GUILD_SET_OFFICER_NOTE_free(vanilla_CMSG_GUILD_SET_OFFICER_NOTE* object) {
    FREE_STRING(object->player_name);

    FREE_STRING(object->note);

}

static WowWorldResult vanilla_SMSG_LOGIN_VERIFY_WORLD_read(WowWorldReader* reader, vanilla_SMSG_LOGIN_VERIFY_WORLD* object) {
    object->map = 0;
    READ_U32(object->map);

    WWM_CHECK_RETURN_CODE(all_Vector3d_read(reader, &object->position));

    READ_FLOAT(object->orientation);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_LOGIN_VERIFY_WORLD_write(WowWorldWriter* writer, const vanilla_SMSG_LOGIN_VERIFY_WORLD* object) {
    WRITE_U16_BE(0x0014 + 2); /* size */

    WRITE_U16(0x00000236); /* opcode */

    WRITE_U32(object->map);

    WWM_CHECK_RETURN_CODE(all_Vector3d_write(writer, &object->position));

    WRITE_FLOAT(object->orientation);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_SEND_MAIL_size(const vanilla_CMSG_SEND_MAIL* object) {
    return 43 + STRING_SIZE(object->receiver) + STRING_SIZE(object->subject) + STRING_SIZE(object->body);
}

static WowWorldResult vanilla_CMSG_SEND_MAIL_read(WowWorldReader* reader, vanilla_CMSG_SEND_MAIL* object) {
    READ_U64(object->mailbox);

    READ_CSTRING(object->receiver);

    READ_CSTRING(object->subject);

    READ_CSTRING(object->body);

    READ_U32(object->unknown1);

    READ_U32(object->unknown2);

    READ_U64(object->item);

    READ_U32(object->money);

    READ_U32(object->cash_on_delivery_amount);

    READ_U32(object->unknown3);

    READ_U32(object->unknown4);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_SEND_MAIL_write(WowWorldWriter* writer, const vanilla_CMSG_SEND_MAIL* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_SEND_MAIL_size(object) + 4); /* size */

    WRITE_U32(0x00000238); /* opcode */

    WRITE_U64(object->mailbox);

    WRITE_CSTRING(object->receiver);

    WRITE_CSTRING(object->subject);

    WRITE_CSTRING(object->body);

    WRITE_U32(object->unknown1);

    WRITE_U32(object->unknown2);

    WRITE_U64(object->item);

    WRITE_U32(object->money);

    WRITE_U32(object->cash_on_delivery_amount);

    WRITE_U32(object->unknown3);

    WRITE_U32(object->unknown4);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_SEND_MAIL_free(vanilla_CMSG_SEND_MAIL* object) {
    FREE_STRING(object->receiver);

    FREE_STRING(object->subject);

    FREE_STRING(object->body);

}

static size_t vanilla_SMSG_SEND_MAIL_RESULT_size(const vanilla_SMSG_SEND_MAIL_RESULT* object) {
    size_t _size = 8;

    if (object->action == VANILLA_MAIL_ACTION_ITEM_TAKEN) {
        _size += 4;

        if (object->result == VANILLA_MAIL_RESULT_ERR_EQUIP_ERROR) {
            _size += 4;
        }
        else if (object->result == VANILLA_MAIL_RESULT_OK|| object->result == VANILLA_MAIL_RESULT_ERR_CANNOT_SEND_TO_SELF|| object->result == VANILLA_MAIL_RESULT_ERR_NOT_ENOUGH_MONEY|| object->result == VANILLA_MAIL_RESULT_ERR_RECIPIENT_NOT_FOUND|| object->result == VANILLA_MAIL_RESULT_ERR_NOT_YOUR_TEAM|| object->result == VANILLA_MAIL_RESULT_ERR_INTERNAL_ERROR|| object->result == VANILLA_MAIL_RESULT_ERR_DISABLED_FOR_TRIAL_ACC|| object->result == VANILLA_MAIL_RESULT_ERR_RECIPIENT_CAP_REACHED|| object->result == VANILLA_MAIL_RESULT_ERR_CANT_SEND_WRAPPED_COD|| object->result == VANILLA_MAIL_RESULT_ERR_MAIL_AND_CHAT_SUSPENDED|| object->result == VANILLA_MAIL_RESULT_ERR_TOO_MANY_ATTACHMENTS|| object->result == VANILLA_MAIL_RESULT_ERR_MAIL_ATTACHMENT_INVALID) {
            _size += 8;
        }

    }
    else if (object->action == VANILLA_MAIL_ACTION_SEND|| object->action == VANILLA_MAIL_ACTION_MONEY_TAKEN|| object->action == VANILLA_MAIL_ACTION_RETURNED_TO_SENDER|| object->action == VANILLA_MAIL_ACTION_DELETED|| object->action == VANILLA_MAIL_ACTION_MADE_PERMANENT) {
        _size += 4;

        if (object->result2 == VANILLA_MAIL_RESULT_TWO_ERR_EQUIP_ERROR) {
            _size += 4;
        }

    }

    return _size;
}

static WowWorldResult vanilla_SMSG_SEND_MAIL_RESULT_read(WowWorldReader* reader, vanilla_SMSG_SEND_MAIL_RESULT* object) {
    READ_U32(object->mail_id);

    object->action = 0;
    READ_U32(object->action);

    if (object->action == VANILLA_MAIL_ACTION_ITEM_TAKEN) {
        object->result = 0;
        READ_U32(object->result);

        if (object->result == VANILLA_MAIL_RESULT_ERR_EQUIP_ERROR) {
            READ_U32(object->equip_error);

        }
        else if (object->result == VANILLA_MAIL_RESULT_OK|| object->result == VANILLA_MAIL_RESULT_ERR_CANNOT_SEND_TO_SELF|| object->result == VANILLA_MAIL_RESULT_ERR_NOT_ENOUGH_MONEY|| object->result == VANILLA_MAIL_RESULT_ERR_RECIPIENT_NOT_FOUND|| object->result == VANILLA_MAIL_RESULT_ERR_NOT_YOUR_TEAM|| object->result == VANILLA_MAIL_RESULT_ERR_INTERNAL_ERROR|| object->result == VANILLA_MAIL_RESULT_ERR_DISABLED_FOR_TRIAL_ACC|| object->result == VANILLA_MAIL_RESULT_ERR_RECIPIENT_CAP_REACHED|| object->result == VANILLA_MAIL_RESULT_ERR_CANT_SEND_WRAPPED_COD|| object->result == VANILLA_MAIL_RESULT_ERR_MAIL_AND_CHAT_SUSPENDED|| object->result == VANILLA_MAIL_RESULT_ERR_TOO_MANY_ATTACHMENTS|| object->result == VANILLA_MAIL_RESULT_ERR_MAIL_ATTACHMENT_INVALID) {
            READ_U32(object->item);

            READ_U32(object->item_count);

        }
    }
    else if (object->action == VANILLA_MAIL_ACTION_SEND|| object->action == VANILLA_MAIL_ACTION_MONEY_TAKEN|| object->action == VANILLA_MAIL_ACTION_RETURNED_TO_SENDER|| object->action == VANILLA_MAIL_ACTION_DELETED|| object->action == VANILLA_MAIL_ACTION_MADE_PERMANENT) {
        object->result2 = 0;
        READ_U32(object->result2);

        if (object->result2 == VANILLA_MAIL_RESULT_TWO_ERR_EQUIP_ERROR) {
            READ_U32(object->equip_error2);

        }
    }
    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SEND_MAIL_RESULT_write(WowWorldWriter* writer, const vanilla_SMSG_SEND_MAIL_RESULT* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SEND_MAIL_RESULT_size(object) + 2); /* size */

    WRITE_U16(0x00000239); /* opcode */

    WRITE_U32(object->mail_id);

    WRITE_U32(object->action);

    if (object->action == VANILLA_MAIL_ACTION_ITEM_TAKEN) {
        WRITE_U32(object->result);

        if (object->result == VANILLA_MAIL_RESULT_ERR_EQUIP_ERROR) {
            WRITE_U32(object->equip_error);

        }
        else if (object->result == VANILLA_MAIL_RESULT_OK|| object->result == VANILLA_MAIL_RESULT_ERR_CANNOT_SEND_TO_SELF|| object->result == VANILLA_MAIL_RESULT_ERR_NOT_ENOUGH_MONEY|| object->result == VANILLA_MAIL_RESULT_ERR_RECIPIENT_NOT_FOUND|| object->result == VANILLA_MAIL_RESULT_ERR_NOT_YOUR_TEAM|| object->result == VANILLA_MAIL_RESULT_ERR_INTERNAL_ERROR|| object->result == VANILLA_MAIL_RESULT_ERR_DISABLED_FOR_TRIAL_ACC|| object->result == VANILLA_MAIL_RESULT_ERR_RECIPIENT_CAP_REACHED|| object->result == VANILLA_MAIL_RESULT_ERR_CANT_SEND_WRAPPED_COD|| object->result == VANILLA_MAIL_RESULT_ERR_MAIL_AND_CHAT_SUSPENDED|| object->result == VANILLA_MAIL_RESULT_ERR_TOO_MANY_ATTACHMENTS|| object->result == VANILLA_MAIL_RESULT_ERR_MAIL_ATTACHMENT_INVALID) {
            WRITE_U32(object->item);

            WRITE_U32(object->item_count);

        }
    }
    else if (object->action == VANILLA_MAIL_ACTION_SEND|| object->action == VANILLA_MAIL_ACTION_MONEY_TAKEN|| object->action == VANILLA_MAIL_ACTION_RETURNED_TO_SENDER|| object->action == VANILLA_MAIL_ACTION_DELETED|| object->action == VANILLA_MAIL_ACTION_MADE_PERMANENT) {
        WRITE_U32(object->result2);

        if (object->result2 == VANILLA_MAIL_RESULT_TWO_ERR_EQUIP_ERROR) {
            WRITE_U32(object->equip_error2);

        }
    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_SEND_MAIL_RESULT_free(vanilla_SMSG_SEND_MAIL_RESULT* object) {
    if (object->action == VANILLA_MAIL_ACTION_ITEM_TAKEN) {
        if (object->result == VANILLA_MAIL_RESULT_ERR_EQUIP_ERROR) {
        }
        else if (object->result == VANILLA_MAIL_RESULT_OK|| object->result == VANILLA_MAIL_RESULT_ERR_CANNOT_SEND_TO_SELF|| object->result == VANILLA_MAIL_RESULT_ERR_NOT_ENOUGH_MONEY|| object->result == VANILLA_MAIL_RESULT_ERR_RECIPIENT_NOT_FOUND|| object->result == VANILLA_MAIL_RESULT_ERR_NOT_YOUR_TEAM|| object->result == VANILLA_MAIL_RESULT_ERR_INTERNAL_ERROR|| object->result == VANILLA_MAIL_RESULT_ERR_DISABLED_FOR_TRIAL_ACC|| object->result == VANILLA_MAIL_RESULT_ERR_RECIPIENT_CAP_REACHED|| object->result == VANILLA_MAIL_RESULT_ERR_CANT_SEND_WRAPPED_COD|| object->result == VANILLA_MAIL_RESULT_ERR_MAIL_AND_CHAT_SUSPENDED|| object->result == VANILLA_MAIL_RESULT_ERR_TOO_MANY_ATTACHMENTS|| object->result == VANILLA_MAIL_RESULT_ERR_MAIL_ATTACHMENT_INVALID) {
        }
    }
    else if (object->action == VANILLA_MAIL_ACTION_SEND|| object->action == VANILLA_MAIL_ACTION_MONEY_TAKEN|| object->action == VANILLA_MAIL_ACTION_RETURNED_TO_SENDER|| object->action == VANILLA_MAIL_ACTION_DELETED|| object->action == VANILLA_MAIL_ACTION_MADE_PERMANENT) {
        if (object->result2 == VANILLA_MAIL_RESULT_TWO_ERR_EQUIP_ERROR) {
        }
    }
}

static WowWorldResult vanilla_CMSG_GET_MAIL_LIST_read(WowWorldReader* reader, vanilla_CMSG_GET_MAIL_LIST* object) {
    READ_U64(object->mailbox);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GET_MAIL_LIST_write(WowWorldWriter* writer, const vanilla_CMSG_GET_MAIL_LIST* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x0000023a); /* opcode */

    WRITE_U64(object->mailbox);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_MAIL_LIST_RESULT_size(const vanilla_SMSG_MAIL_LIST_RESULT* object) {
    size_t _size = 1;

    /* C89 scope to allow variable declarations */ {
        int i;
        for(i = 0; i < (int)object->amount_of_mails; ++i) {
            _size += vanilla_Mail_size(&object->mails[i]);
        }
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_MAIL_LIST_RESULT_read(WowWorldReader* reader, vanilla_SMSG_MAIL_LIST_RESULT* object) {
    READ_U8(object->amount_of_mails);

    READ_ARRAY_ALLOCATE(object->mails, object->amount_of_mails, sizeof(vanilla_Mail));
    READ_ARRAY(object->mails, object->amount_of_mails, WWM_CHECK_RETURN_CODE(vanilla_Mail_read(reader, &object->mails[i])));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_MAIL_LIST_RESULT_write(WowWorldWriter* writer, const vanilla_SMSG_MAIL_LIST_RESULT* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_MAIL_LIST_RESULT_size(object) + 2); /* size */

    WRITE_U16(0x0000023b); /* opcode */

    WRITE_U8(object->amount_of_mails);

    WRITE_ARRAY(object->mails, object->amount_of_mails, WWM_CHECK_RETURN_CODE(vanilla_Mail_write(writer, &object->mails[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_MAIL_LIST_RESULT_free(vanilla_SMSG_MAIL_LIST_RESULT* object) {
    size_t i;

    for (i = 0; i < object->amount_of_mails; ++i) {
        vanilla_Mail_free(&((object->mails)[i]));
    }
    free(object->mails);
    object->mails = NULL;
}

static WowWorldResult vanilla_CMSG_BATTLEFIELD_LIST_read(WowWorldReader* reader, vanilla_CMSG_BATTLEFIELD_LIST* object) {
    object->map = 0;
    READ_U32(object->map);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_BATTLEFIELD_LIST_write(WowWorldWriter* writer, const vanilla_CMSG_BATTLEFIELD_LIST* object) {
    WRITE_U16_BE(0x0004 + 4); /* size */

    WRITE_U32(0x0000023c); /* opcode */

    WRITE_U32(object->map);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_BATTLEFIELD_LIST_size(const vanilla_SMSG_BATTLEFIELD_LIST* object) {
    return 17 + 4 * object->number_of_battlegrounds;
}

static WowWorldResult vanilla_SMSG_BATTLEFIELD_LIST_read(WowWorldReader* reader, vanilla_SMSG_BATTLEFIELD_LIST* object) {
    READ_U64(object->battlemaster);

    object->map = 0;
    READ_U32(object->map);

    object->bracket = 0;
    READ_U8(object->bracket);

    READ_U32(object->number_of_battlegrounds);

    READ_ARRAY_ALLOCATE(object->battlegrounds, object->number_of_battlegrounds, sizeof(uint32_t));
    READ_ARRAY(object->battlegrounds, object->number_of_battlegrounds, READ_U32(object->battlegrounds[i]));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_BATTLEFIELD_LIST_write(WowWorldWriter* writer, const vanilla_SMSG_BATTLEFIELD_LIST* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_BATTLEFIELD_LIST_size(object) + 2); /* size */

    WRITE_U16(0x0000023d); /* opcode */

    WRITE_U64(object->battlemaster);

    WRITE_U32(object->map);

    WRITE_U8(object->bracket);

    WRITE_U32(object->number_of_battlegrounds);

    WRITE_ARRAY(object->battlegrounds, object->number_of_battlegrounds, WRITE_U32(object->battlegrounds[i]));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_BATTLEFIELD_LIST_free(vanilla_SMSG_BATTLEFIELD_LIST* object) {
    free(object->battlegrounds);
    object->battlegrounds = NULL;
}

static WowWorldResult vanilla_CMSG_BATTLEFIELD_JOIN_read(WowWorldReader* reader, vanilla_CMSG_BATTLEFIELD_JOIN* object) {
    object->map = 0;
    READ_U32(object->map);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_BATTLEFIELD_JOIN_write(WowWorldWriter* writer, const vanilla_CMSG_BATTLEFIELD_JOIN* object) {
    WRITE_U16_BE(0x0004 + 4); /* size */

    WRITE_U32(0x0000023e); /* opcode */

    WRITE_U32(object->map);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_ITEM_TEXT_QUERY_read(WowWorldReader* reader, vanilla_CMSG_ITEM_TEXT_QUERY* object) {
    READ_U32(object->item_text_id);

    READ_U32(object->mail_id);

    READ_U32(object->unknown1);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_ITEM_TEXT_QUERY_write(WowWorldWriter* writer, const vanilla_CMSG_ITEM_TEXT_QUERY* object) {
    WRITE_U16_BE(0x000c + 4); /* size */

    WRITE_U32(0x00000243); /* opcode */

    WRITE_U32(object->item_text_id);

    WRITE_U32(object->mail_id);

    WRITE_U32(object->unknown1);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_ITEM_TEXT_QUERY_RESPONSE_size(const vanilla_SMSG_ITEM_TEXT_QUERY_RESPONSE* object) {
    return 5 + STRING_SIZE(object->text);
}

static WowWorldResult vanilla_SMSG_ITEM_TEXT_QUERY_RESPONSE_read(WowWorldReader* reader, vanilla_SMSG_ITEM_TEXT_QUERY_RESPONSE* object) {
    READ_U32(object->item_text_id);

    READ_CSTRING(object->text);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_ITEM_TEXT_QUERY_RESPONSE_write(WowWorldWriter* writer, const vanilla_SMSG_ITEM_TEXT_QUERY_RESPONSE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_ITEM_TEXT_QUERY_RESPONSE_size(object) + 2); /* size */

    WRITE_U16(0x00000244); /* opcode */

    WRITE_U32(object->item_text_id);

    WRITE_CSTRING(object->text);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_ITEM_TEXT_QUERY_RESPONSE_free(vanilla_SMSG_ITEM_TEXT_QUERY_RESPONSE* object) {
    FREE_STRING(object->text);

}

static WowWorldResult vanilla_CMSG_MAIL_TAKE_MONEY_read(WowWorldReader* reader, vanilla_CMSG_MAIL_TAKE_MONEY* object) {
    READ_U64(object->mailbox);

    READ_U32(object->mail_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_MAIL_TAKE_MONEY_write(WowWorldWriter* writer, const vanilla_CMSG_MAIL_TAKE_MONEY* object) {
    WRITE_U16_BE(0x000c + 4); /* size */

    WRITE_U32(0x00000245); /* opcode */

    WRITE_U64(object->mailbox);

    WRITE_U32(object->mail_id);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_MAIL_TAKE_ITEM_read(WowWorldReader* reader, vanilla_CMSG_MAIL_TAKE_ITEM* object) {
    READ_U64(object->mailbox);

    READ_U32(object->mail_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_MAIL_TAKE_ITEM_write(WowWorldWriter* writer, const vanilla_CMSG_MAIL_TAKE_ITEM* object) {
    WRITE_U16_BE(0x000c + 4); /* size */

    WRITE_U32(0x00000246); /* opcode */

    WRITE_U64(object->mailbox);

    WRITE_U32(object->mail_id);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_MAIL_MARK_AS_READ_read(WowWorldReader* reader, vanilla_CMSG_MAIL_MARK_AS_READ* object) {
    READ_U64(object->mailbox);

    READ_U32(object->mail_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_MAIL_MARK_AS_READ_write(WowWorldWriter* writer, const vanilla_CMSG_MAIL_MARK_AS_READ* object) {
    WRITE_U16_BE(0x000c + 4); /* size */

    WRITE_U32(0x00000247); /* opcode */

    WRITE_U64(object->mailbox);

    WRITE_U32(object->mail_id);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_MAIL_RETURN_TO_SENDER_read(WowWorldReader* reader, vanilla_CMSG_MAIL_RETURN_TO_SENDER* object) {
    READ_U64(object->mailbox_id);

    READ_U32(object->mail_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_MAIL_RETURN_TO_SENDER_write(WowWorldWriter* writer, const vanilla_CMSG_MAIL_RETURN_TO_SENDER* object) {
    WRITE_U16_BE(0x000c + 4); /* size */

    WRITE_U32(0x00000248); /* opcode */

    WRITE_U64(object->mailbox_id);

    WRITE_U32(object->mail_id);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_MAIL_DELETE_read(WowWorldReader* reader, vanilla_CMSG_MAIL_DELETE* object) {
    READ_U64(object->mailbox_id);

    READ_U32(object->mail_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_MAIL_DELETE_write(WowWorldWriter* writer, const vanilla_CMSG_MAIL_DELETE* object) {
    WRITE_U16_BE(0x000c + 4); /* size */

    WRITE_U32(0x00000249); /* opcode */

    WRITE_U64(object->mailbox_id);

    WRITE_U32(object->mail_id);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_MAIL_CREATE_TEXT_ITEM_read(WowWorldReader* reader, vanilla_CMSG_MAIL_CREATE_TEXT_ITEM* object) {
    READ_U64(object->mailbox);

    READ_U32(object->mail_id);

    READ_U32(object->mail_template_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_MAIL_CREATE_TEXT_ITEM_write(WowWorldWriter* writer, const vanilla_CMSG_MAIL_CREATE_TEXT_ITEM* object) {
    WRITE_U16_BE(0x0010 + 4); /* size */

    WRITE_U32(0x0000024a); /* opcode */

    WRITE_U64(object->mailbox);

    WRITE_U32(object->mail_id);

    WRITE_U32(object->mail_template_id);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_SPELLLOGMISS_size(const vanilla_SMSG_SPELLLOGMISS* object) {
    return 17 + 9 * object->amount_of_targets;
}

static WowWorldResult vanilla_SMSG_SPELLLOGMISS_read(WowWorldReader* reader, vanilla_SMSG_SPELLLOGMISS* object) {
    READ_U32(object->id);

    READ_U64(object->caster);

    READ_U8(object->unknown1);

    READ_U32(object->amount_of_targets);

    READ_ARRAY_ALLOCATE(object->targets, object->amount_of_targets, sizeof(vanilla_SpellLogMiss));
    READ_ARRAY(object->targets, object->amount_of_targets, WWM_CHECK_RETURN_CODE(vanilla_SpellLogMiss_read(reader, &object->targets[i])));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPELLLOGMISS_write(WowWorldWriter* writer, const vanilla_SMSG_SPELLLOGMISS* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SPELLLOGMISS_size(object) + 2); /* size */

    WRITE_U16(0x0000024b); /* opcode */

    WRITE_U32(object->id);

    WRITE_U64(object->caster);

    WRITE_U8(object->unknown1);

    WRITE_U32(object->amount_of_targets);

    WRITE_ARRAY(object->targets, object->amount_of_targets, WWM_CHECK_RETURN_CODE(vanilla_SpellLogMiss_write(writer, &object->targets[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_SPELLLOGMISS_free(vanilla_SMSG_SPELLLOGMISS* object) {
    free(object->targets);
    object->targets = NULL;
}

static size_t vanilla_SMSG_SPELLLOGEXECUTE_size(const vanilla_SMSG_SPELLLOGEXECUTE* object) {
    size_t _size = 8 + wwm_packed_guid_size(object->caster);

    /* C89 scope to allow variable declarations */ {
        int i;
        for(i = 0; i < (int)object->amount_of_effects; ++i) {
            _size += vanilla_SpellLog_size(&object->logs[i]);
        }
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_SPELLLOGEXECUTE_read(WowWorldReader* reader, vanilla_SMSG_SPELLLOGEXECUTE* object) {
    READ_PACKED_GUID(object->caster);

    READ_U32(object->spell);

    READ_U32(object->amount_of_effects);

    READ_ARRAY_ALLOCATE(object->logs, object->amount_of_effects, sizeof(vanilla_SpellLog));
    READ_ARRAY(object->logs, object->amount_of_effects, WWM_CHECK_RETURN_CODE(vanilla_SpellLog_read(reader, &object->logs[i])));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPELLLOGEXECUTE_write(WowWorldWriter* writer, const vanilla_SMSG_SPELLLOGEXECUTE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SPELLLOGEXECUTE_size(object) + 2); /* size */

    WRITE_U16(0x0000024c); /* opcode */

    WRITE_PACKED_GUID(object->caster);

    WRITE_U32(object->spell);

    WRITE_U32(object->amount_of_effects);

    WRITE_ARRAY(object->logs, object->amount_of_effects, WWM_CHECK_RETURN_CODE(vanilla_SpellLog_write(writer, &object->logs[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_SPELLLOGEXECUTE_free(vanilla_SMSG_SPELLLOGEXECUTE* object) {
    size_t i;

    for (i = 0; i < object->amount_of_effects; ++i) {
        vanilla_SpellLog_free(&((object->logs)[i]));
    }
    free(object->logs);
    object->logs = NULL;
}

static size_t vanilla_SMSG_PERIODICAURALOG_size(const vanilla_SMSG_PERIODICAURALOG* object) {
    size_t _size = 8 + wwm_packed_guid_size(object->target) + wwm_packed_guid_size(object->caster);

    /* C89 scope to allow variable declarations */ {
        int i;
        for(i = 0; i < (int)object->amount_of_auras; ++i) {
            _size += vanilla_AuraLog_size(&object->auras[i]);
        }
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_PERIODICAURALOG_read(WowWorldReader* reader, vanilla_SMSG_PERIODICAURALOG* object) {
    READ_PACKED_GUID(object->target);

    READ_PACKED_GUID(object->caster);

    READ_U32(object->spell);

    READ_U32(object->amount_of_auras);

    READ_ARRAY_ALLOCATE(object->auras, object->amount_of_auras, sizeof(vanilla_AuraLog));
    READ_ARRAY(object->auras, object->amount_of_auras, WWM_CHECK_RETURN_CODE(vanilla_AuraLog_read(reader, &object->auras[i])));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PERIODICAURALOG_write(WowWorldWriter* writer, const vanilla_SMSG_PERIODICAURALOG* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_PERIODICAURALOG_size(object) + 2); /* size */

    WRITE_U16(0x0000024e); /* opcode */

    WRITE_PACKED_GUID(object->target);

    WRITE_PACKED_GUID(object->caster);

    WRITE_U32(object->spell);

    WRITE_U32(object->amount_of_auras);

    WRITE_ARRAY(object->auras, object->amount_of_auras, WWM_CHECK_RETURN_CODE(vanilla_AuraLog_write(writer, &object->auras[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_PERIODICAURALOG_free(vanilla_SMSG_PERIODICAURALOG* object) {
    size_t i;

    for (i = 0; i < object->amount_of_auras; ++i) {
        vanilla_AuraLog_free(&((object->auras)[i]));
    }
    free(object->auras);
    object->auras = NULL;
}

static WowWorldResult vanilla_SMSG_SPELLDAMAGESHIELD_read(WowWorldReader* reader, vanilla_SMSG_SPELLDAMAGESHIELD* object) {
    READ_U64(object->victim);

    READ_U64(object->caster);

    READ_U32(object->damage);

    object->school = 0;
    READ_U32(object->school);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPELLDAMAGESHIELD_write(WowWorldWriter* writer, const vanilla_SMSG_SPELLDAMAGESHIELD* object) {
    WRITE_U16_BE(0x0018 + 2); /* size */

    WRITE_U16(0x0000024f); /* opcode */

    WRITE_U64(object->victim);

    WRITE_U64(object->caster);

    WRITE_U32(object->damage);

    WRITE_U32(object->school);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_SPELLNONMELEEDAMAGELOG_size(const vanilla_SMSG_SPELLNONMELEEDAMAGELOG* object) {
    return 28 + wwm_packed_guid_size(object->target) + wwm_packed_guid_size(object->attacker);
}

static WowWorldResult vanilla_SMSG_SPELLNONMELEEDAMAGELOG_read(WowWorldReader* reader, vanilla_SMSG_SPELLNONMELEEDAMAGELOG* object) {
    READ_PACKED_GUID(object->target);

    READ_PACKED_GUID(object->attacker);

    READ_U32(object->spell);

    READ_U32(object->damage);

    object->school = 0;
    READ_U8(object->school);

    READ_U32(object->absorbed_damage);

    READ_U32(object->resisted);

    READ_BOOL8(object->periodic_log);

    READ_U8(object->unused);

    READ_U32(object->blocked);

    object->hit_info = 0;
    READ_U32(object->hit_info);

    READ_U8(object->extend_flag);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPELLNONMELEEDAMAGELOG_write(WowWorldWriter* writer, const vanilla_SMSG_SPELLNONMELEEDAMAGELOG* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SPELLNONMELEEDAMAGELOG_size(object) + 2); /* size */

    WRITE_U16(0x00000250); /* opcode */

    WRITE_PACKED_GUID(object->target);

    WRITE_PACKED_GUID(object->attacker);

    WRITE_U32(object->spell);

    WRITE_U32(object->damage);

    WRITE_U8(object->school);

    WRITE_U32(object->absorbed_damage);

    WRITE_U32(object->resisted);

    WRITE_BOOL8(object->periodic_log);

    WRITE_U8(object->unused);

    WRITE_U32(object->blocked);

    WRITE_U32(object->hit_info);

    WRITE_U8(object->extend_flag);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_LEARN_TALENT_read(WowWorldReader* reader, vanilla_CMSG_LEARN_TALENT* object) {
    object->talent = 0;
    READ_U32(object->talent);

    READ_U32(object->requested_rank);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_LEARN_TALENT_write(WowWorldWriter* writer, const vanilla_CMSG_LEARN_TALENT* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x00000251); /* opcode */

    WRITE_U32(object->talent);

    WRITE_U32(object->requested_rank);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_TOGGLE_PVP_size(const vanilla_CMSG_TOGGLE_PVP* object) {
    size_t _size = 0;

    if(object->set) {
        _size += 1;
    }

    return _size;
}

static WowWorldResult vanilla_CMSG_TOGGLE_PVP_read(WowWorldReader* reader, vanilla_CMSG_TOGGLE_PVP* object, size_t body_size) {
    size_t _size = 0;

    object->set = NULL;
    if (_size < body_size) {
        object->set = malloc(sizeof(vanilla_CMSG_TOGGLE_PVP_set));

        READ_BOOL8(object->set->enable_pvp);

    }
    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_TOGGLE_PVP_write(WowWorldWriter* writer, const vanilla_CMSG_TOGGLE_PVP* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_TOGGLE_PVP_size(object) + 4); /* size */

    WRITE_U32(0x00000253); /* opcode */

    if(object->set) {
        WRITE_BOOL8(object->set->enable_pvp);

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_TOGGLE_PVP_free(vanilla_CMSG_TOGGLE_PVP* object) {
    free(object->set);
}

static WowWorldResult vanilla_SMSG_ZONE_UNDER_ATTACK_read(WowWorldReader* reader, vanilla_SMSG_ZONE_UNDER_ATTACK* object) {
    object->zone_id = 0;
    READ_U32(object->zone_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_ZONE_UNDER_ATTACK_write(WowWorldWriter* writer, const vanilla_SMSG_ZONE_UNDER_ATTACK* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x00000254); /* opcode */

    WRITE_U32(object->zone_id);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_MSG_AUCTION_HELLO_Client_read(WowWorldReader* reader, vanilla_MSG_AUCTION_HELLO_Client* object) {
    READ_U64(object->auctioneer);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_AUCTION_HELLO_Client_write(WowWorldWriter* writer, const vanilla_MSG_AUCTION_HELLO_Client* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x00000255); /* opcode */

    WRITE_U64(object->auctioneer);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_MSG_AUCTION_HELLO_Server_read(WowWorldReader* reader, vanilla_MSG_AUCTION_HELLO_Server* object) {
    READ_U64(object->auctioneer);

    object->auction_house = 0;
    READ_U32(object->auction_house);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_AUCTION_HELLO_Server_write(WowWorldWriter* writer, const vanilla_MSG_AUCTION_HELLO_Server* object) {
    WRITE_U16_BE(0x000c + 2); /* size */

    WRITE_U16(0x00000255); /* opcode */

    WRITE_U64(object->auctioneer);

    WRITE_U32(object->auction_house);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_AUCTION_SELL_ITEM_read(WowWorldReader* reader, vanilla_CMSG_AUCTION_SELL_ITEM* object) {
    READ_U64(object->auctioneer);

    READ_U64(object->item);

    READ_U32(object->starting_bid);

    READ_U32(object->buyout);

    READ_U32(object->auction_duration_in_minutes);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_AUCTION_SELL_ITEM_write(WowWorldWriter* writer, const vanilla_CMSG_AUCTION_SELL_ITEM* object) {
    WRITE_U16_BE(0x001c + 4); /* size */

    WRITE_U32(0x00000256); /* opcode */

    WRITE_U64(object->auctioneer);

    WRITE_U64(object->item);

    WRITE_U32(object->starting_bid);

    WRITE_U32(object->buyout);

    WRITE_U32(object->auction_duration_in_minutes);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_AUCTION_REMOVE_ITEM_read(WowWorldReader* reader, vanilla_CMSG_AUCTION_REMOVE_ITEM* object) {
    READ_U64(object->auctioneer);

    READ_U32(object->auction_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_AUCTION_REMOVE_ITEM_write(WowWorldWriter* writer, const vanilla_CMSG_AUCTION_REMOVE_ITEM* object) {
    WRITE_U16_BE(0x000c + 4); /* size */

    WRITE_U32(0x00000257); /* opcode */

    WRITE_U64(object->auctioneer);

    WRITE_U32(object->auction_id);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_AUCTION_LIST_ITEMS_size(const vanilla_CMSG_AUCTION_LIST_ITEMS* object) {
    return 32 + STRING_SIZE(object->searched_name);
}

static WowWorldResult vanilla_CMSG_AUCTION_LIST_ITEMS_read(WowWorldReader* reader, vanilla_CMSG_AUCTION_LIST_ITEMS* object) {
    READ_U64(object->auctioneer);

    READ_U32(object->list_start_item);

    READ_CSTRING(object->searched_name);

    READ_U8(object->minimum_level);

    READ_U8(object->maximum_level);

    READ_U32(object->auction_slot_id);

    READ_U32(object->auction_main_category);

    READ_U32(object->auction_sub_category);

    object->auction_quality = 0;
    READ_U32(object->auction_quality);

    READ_U8(object->usable);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_AUCTION_LIST_ITEMS_write(WowWorldWriter* writer, const vanilla_CMSG_AUCTION_LIST_ITEMS* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_AUCTION_LIST_ITEMS_size(object) + 4); /* size */

    WRITE_U32(0x00000258); /* opcode */

    WRITE_U64(object->auctioneer);

    WRITE_U32(object->list_start_item);

    WRITE_CSTRING(object->searched_name);

    WRITE_U8(object->minimum_level);

    WRITE_U8(object->maximum_level);

    WRITE_U32(object->auction_slot_id);

    WRITE_U32(object->auction_main_category);

    WRITE_U32(object->auction_sub_category);

    WRITE_U32(object->auction_quality);

    WRITE_U8(object->usable);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_AUCTION_LIST_ITEMS_free(vanilla_CMSG_AUCTION_LIST_ITEMS* object) {
    FREE_STRING(object->searched_name);

}

static WowWorldResult vanilla_CMSG_AUCTION_LIST_OWNER_ITEMS_read(WowWorldReader* reader, vanilla_CMSG_AUCTION_LIST_OWNER_ITEMS* object) {
    READ_U64(object->auctioneer);

    READ_U32(object->list_from);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_AUCTION_LIST_OWNER_ITEMS_write(WowWorldWriter* writer, const vanilla_CMSG_AUCTION_LIST_OWNER_ITEMS* object) {
    WRITE_U16_BE(0x000c + 4); /* size */

    WRITE_U32(0x00000259); /* opcode */

    WRITE_U64(object->auctioneer);

    WRITE_U32(object->list_from);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_AUCTION_PLACE_BID_read(WowWorldReader* reader, vanilla_CMSG_AUCTION_PLACE_BID* object) {
    READ_U64(object->auctioneer);

    READ_U32(object->auction_id);

    READ_U32(object->price);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_AUCTION_PLACE_BID_write(WowWorldWriter* writer, const vanilla_CMSG_AUCTION_PLACE_BID* object) {
    WRITE_U16_BE(0x0010 + 4); /* size */

    WRITE_U32(0x0000025a); /* opcode */

    WRITE_U64(object->auctioneer);

    WRITE_U32(object->auction_id);

    WRITE_U32(object->price);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_AUCTION_COMMAND_RESULT_size(const vanilla_SMSG_AUCTION_COMMAND_RESULT* object) {
    size_t _size = 8;

    if (object->action == VANILLA_AUCTION_COMMAND_ACTION_BID_PLACED) {
        _size += 4;

        if (object->result == VANILLA_AUCTION_COMMAND_RESULT_OK) {
            _size += 4;
        }
        else if (object->result == VANILLA_AUCTION_COMMAND_RESULT_ERR_INVENTORY) {
            _size += 1;
        }
        else if (object->result == VANILLA_AUCTION_COMMAND_RESULT_ERR_HIGHER_BID) {
            _size += 16;
        }

    }
    else if (object->action == VANILLA_AUCTION_COMMAND_ACTION_STARTED|| object->action == VANILLA_AUCTION_COMMAND_ACTION_REMOVED) {
        _size += 4;

        if (object->result2 == VANILLA_AUCTION_COMMAND_RESULT_TWO_ERR_INVENTORY) {
            _size += 1;
        }
        else if (object->result2 == VANILLA_AUCTION_COMMAND_RESULT_TWO_ERR_HIGHER_BID) {
            _size += 16;
        }

    }

    return _size;
}

static WowWorldResult vanilla_SMSG_AUCTION_COMMAND_RESULT_read(WowWorldReader* reader, vanilla_SMSG_AUCTION_COMMAND_RESULT* object) {
    READ_U32(object->auction_id);

    object->action = 0;
    READ_U32(object->action);

    if (object->action == VANILLA_AUCTION_COMMAND_ACTION_BID_PLACED) {
        object->result = 0;
        READ_U32(object->result);

        if (object->result == VANILLA_AUCTION_COMMAND_RESULT_OK) {
            READ_U32(object->auction_outbid1);

        }
        else if (object->result == VANILLA_AUCTION_COMMAND_RESULT_ERR_INVENTORY) {
            object->inventory_result = 0;
            READ_U8(object->inventory_result);

        }
        else if (object->result == VANILLA_AUCTION_COMMAND_RESULT_ERR_HIGHER_BID) {
            READ_U64(object->higher_bidder);

            READ_U32(object->new_bid);

            READ_U32(object->auction_outbid2);

        }
    }
    else if (object->action == VANILLA_AUCTION_COMMAND_ACTION_STARTED|| object->action == VANILLA_AUCTION_COMMAND_ACTION_REMOVED) {
        object->result2 = 0;
        READ_U32(object->result2);

        if (object->result2 == VANILLA_AUCTION_COMMAND_RESULT_TWO_ERR_INVENTORY) {
            object->inventory_result2 = 0;
            READ_U8(object->inventory_result2);

        }
        else if (object->result2 == VANILLA_AUCTION_COMMAND_RESULT_TWO_ERR_HIGHER_BID) {
            READ_U64(object->higher_bidder2);

            READ_U32(object->new_bid2);

            READ_U32(object->auction_outbid3);

        }
    }
    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_AUCTION_COMMAND_RESULT_write(WowWorldWriter* writer, const vanilla_SMSG_AUCTION_COMMAND_RESULT* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_AUCTION_COMMAND_RESULT_size(object) + 2); /* size */

    WRITE_U16(0x0000025b); /* opcode */

    WRITE_U32(object->auction_id);

    WRITE_U32(object->action);

    if (object->action == VANILLA_AUCTION_COMMAND_ACTION_BID_PLACED) {
        WRITE_U32(object->result);

        if (object->result == VANILLA_AUCTION_COMMAND_RESULT_OK) {
            WRITE_U32(object->auction_outbid1);

        }
        else if (object->result == VANILLA_AUCTION_COMMAND_RESULT_ERR_INVENTORY) {
            WRITE_U8(object->inventory_result);

        }
        else if (object->result == VANILLA_AUCTION_COMMAND_RESULT_ERR_HIGHER_BID) {
            WRITE_U64(object->higher_bidder);

            WRITE_U32(object->new_bid);

            WRITE_U32(object->auction_outbid2);

        }
    }
    else if (object->action == VANILLA_AUCTION_COMMAND_ACTION_STARTED|| object->action == VANILLA_AUCTION_COMMAND_ACTION_REMOVED) {
        WRITE_U32(object->result2);

        if (object->result2 == VANILLA_AUCTION_COMMAND_RESULT_TWO_ERR_INVENTORY) {
            WRITE_U8(object->inventory_result2);

        }
        else if (object->result2 == VANILLA_AUCTION_COMMAND_RESULT_TWO_ERR_HIGHER_BID) {
            WRITE_U64(object->higher_bidder2);

            WRITE_U32(object->new_bid2);

            WRITE_U32(object->auction_outbid3);

        }
    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_AUCTION_COMMAND_RESULT_free(vanilla_SMSG_AUCTION_COMMAND_RESULT* object) {
    if (object->action == VANILLA_AUCTION_COMMAND_ACTION_BID_PLACED) {
        if (object->result == VANILLA_AUCTION_COMMAND_RESULT_OK) {
        }
        else if (object->result == VANILLA_AUCTION_COMMAND_RESULT_ERR_INVENTORY) {
        }
        else if (object->result == VANILLA_AUCTION_COMMAND_RESULT_ERR_HIGHER_BID) {
        }
    }
    else if (object->action == VANILLA_AUCTION_COMMAND_ACTION_STARTED|| object->action == VANILLA_AUCTION_COMMAND_ACTION_REMOVED) {
        if (object->result2 == VANILLA_AUCTION_COMMAND_RESULT_TWO_ERR_INVENTORY) {
        }
        else if (object->result2 == VANILLA_AUCTION_COMMAND_RESULT_TWO_ERR_HIGHER_BID) {
        }
    }
}

static size_t vanilla_SMSG_AUCTION_LIST_RESULT_size(const vanilla_SMSG_AUCTION_LIST_RESULT* object) {
    return 8 + 64 * object->count;
}

static WowWorldResult vanilla_SMSG_AUCTION_LIST_RESULT_read(WowWorldReader* reader, vanilla_SMSG_AUCTION_LIST_RESULT* object) {
    READ_U32(object->count);

    READ_ARRAY_ALLOCATE(object->auctions, object->count, sizeof(vanilla_AuctionListItem));
    READ_ARRAY(object->auctions, object->count, WWM_CHECK_RETURN_CODE(vanilla_AuctionListItem_read(reader, &object->auctions[i])));

    READ_U32(object->total_amount_of_auctions);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_AUCTION_LIST_RESULT_write(WowWorldWriter* writer, const vanilla_SMSG_AUCTION_LIST_RESULT* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_AUCTION_LIST_RESULT_size(object) + 2); /* size */

    WRITE_U16(0x0000025c); /* opcode */

    WRITE_U32(object->count);

    WRITE_ARRAY(object->auctions, object->count, WWM_CHECK_RETURN_CODE(vanilla_AuctionListItem_write(writer, &object->auctions[i])));

    WRITE_U32(object->total_amount_of_auctions);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_AUCTION_LIST_RESULT_free(vanilla_SMSG_AUCTION_LIST_RESULT* object) {
    free(object->auctions);
    object->auctions = NULL;
}

static size_t vanilla_SMSG_AUCTION_OWNER_LIST_RESULT_size(const vanilla_SMSG_AUCTION_OWNER_LIST_RESULT* object) {
    return 8 + 64 * object->count;
}

static WowWorldResult vanilla_SMSG_AUCTION_OWNER_LIST_RESULT_read(WowWorldReader* reader, vanilla_SMSG_AUCTION_OWNER_LIST_RESULT* object) {
    READ_U32(object->count);

    READ_ARRAY_ALLOCATE(object->auctions, object->count, sizeof(vanilla_AuctionListItem));
    READ_ARRAY(object->auctions, object->count, WWM_CHECK_RETURN_CODE(vanilla_AuctionListItem_read(reader, &object->auctions[i])));

    READ_U32(object->total_amount_of_auctions);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_AUCTION_OWNER_LIST_RESULT_write(WowWorldWriter* writer, const vanilla_SMSG_AUCTION_OWNER_LIST_RESULT* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_AUCTION_OWNER_LIST_RESULT_size(object) + 2); /* size */

    WRITE_U16(0x0000025d); /* opcode */

    WRITE_U32(object->count);

    WRITE_ARRAY(object->auctions, object->count, WWM_CHECK_RETURN_CODE(vanilla_AuctionListItem_write(writer, &object->auctions[i])));

    WRITE_U32(object->total_amount_of_auctions);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_AUCTION_OWNER_LIST_RESULT_free(vanilla_SMSG_AUCTION_OWNER_LIST_RESULT* object) {
    free(object->auctions);
    object->auctions = NULL;
}

static WowWorldResult vanilla_SMSG_AUCTION_BIDDER_NOTIFICATION_read(WowWorldReader* reader, vanilla_SMSG_AUCTION_BIDDER_NOTIFICATION* object) {
    object->auction_house = 0;
    READ_U32(object->auction_house);

    READ_U32(object->auction_id);

    READ_U64(object->bidder);

    READ_U32(object->won);

    READ_U32(object->out_bid);

    READ_U32(object->item_template);

    READ_U32(object->item_random_property_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_AUCTION_BIDDER_NOTIFICATION_write(WowWorldWriter* writer, const vanilla_SMSG_AUCTION_BIDDER_NOTIFICATION* object) {
    WRITE_U16_BE(0x0020 + 2); /* size */

    WRITE_U16(0x0000025e); /* opcode */

    WRITE_U32(object->auction_house);

    WRITE_U32(object->auction_id);

    WRITE_U64(object->bidder);

    WRITE_U32(object->won);

    WRITE_U32(object->out_bid);

    WRITE_U32(object->item_template);

    WRITE_U32(object->item_random_property_id);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_AUCTION_OWNER_NOTIFICATION_read(WowWorldReader* reader, vanilla_SMSG_AUCTION_OWNER_NOTIFICATION* object) {
    READ_U32(object->auction_id);

    READ_U32(object->bid);

    READ_U32(object->auction_out_bid);

    READ_U64(object->bidder);

    READ_U32(object->item);

    READ_U32(object->item_random_property_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_AUCTION_OWNER_NOTIFICATION_write(WowWorldWriter* writer, const vanilla_SMSG_AUCTION_OWNER_NOTIFICATION* object) {
    WRITE_U16_BE(0x001c + 2); /* size */

    WRITE_U16(0x0000025f); /* opcode */

    WRITE_U32(object->auction_id);

    WRITE_U32(object->bid);

    WRITE_U32(object->auction_out_bid);

    WRITE_U64(object->bidder);

    WRITE_U32(object->item);

    WRITE_U32(object->item_random_property_id);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_PROCRESIST_read(WowWorldReader* reader, vanilla_SMSG_PROCRESIST* object) {
    READ_U64(object->caster);

    READ_U64(object->target);

    READ_U32(object->id);

    object->log_format = 0;
    READ_U8(object->log_format);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PROCRESIST_write(WowWorldWriter* writer, const vanilla_SMSG_PROCRESIST* object) {
    WRITE_U16_BE(0x0015 + 2); /* size */

    WRITE_U16(0x00000260); /* opcode */

    WRITE_U64(object->caster);

    WRITE_U64(object->target);

    WRITE_U32(object->id);

    WRITE_U8(object->log_format);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_DISPEL_FAILED_size(const vanilla_SMSG_DISPEL_FAILED* object) {
    return 16 + 4 * object->amount_of_spells;
}

static WowWorldResult vanilla_SMSG_DISPEL_FAILED_read(WowWorldReader* reader, vanilla_SMSG_DISPEL_FAILED* object, size_t body_size) {
    size_t _size = 0;

    READ_U64(object->caster);
    _size += 8;

    READ_U64(object->target);
    _size += 8;

    object->amount_of_spells = 0;
    /* C89 scope to allow variable declarations */ {
        int i = 0;
        size_t _current_size = 8 * sizeof(*object->spells);

        object->spells = malloc(_current_size);
        while (_size < body_size) {
            READ_U32(object->spells[i]);
            _size += 4;
            ++i;

            if (i * sizeof(*object->spells) >= _current_size) {
                _current_size *= 2;
                object->spells = realloc(object->spells, _current_size);
            }
        }

        object->amount_of_spells = i;
    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_DISPEL_FAILED_write(WowWorldWriter* writer, const vanilla_SMSG_DISPEL_FAILED* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_DISPEL_FAILED_size(object) + 2); /* size */

    WRITE_U16(0x00000262); /* opcode */

    WRITE_U64(object->caster);

    WRITE_U64(object->target);

    WRITE_ARRAY(object->spells, object->amount_of_spells, WRITE_U32(object->spells[i]));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_DISPEL_FAILED_free(vanilla_SMSG_DISPEL_FAILED* object) {
    free(object->spells);
    object->spells = NULL;
}

static WowWorldResult vanilla_SMSG_SPELLORDAMAGE_IMMUNE_read(WowWorldReader* reader, vanilla_SMSG_SPELLORDAMAGE_IMMUNE* object) {
    READ_U64(object->caster);

    READ_U64(object->target);

    READ_U32(object->id);

    READ_BOOL8(object->debug_log_format);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPELLORDAMAGE_IMMUNE_write(WowWorldWriter* writer, const vanilla_SMSG_SPELLORDAMAGE_IMMUNE* object) {
    WRITE_U16_BE(0x0015 + 2); /* size */

    WRITE_U16(0x00000263); /* opcode */

    WRITE_U64(object->caster);

    WRITE_U64(object->target);

    WRITE_U32(object->id);

    WRITE_BOOL8(object->debug_log_format);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_AUCTION_LIST_BIDDER_ITEMS_size(const vanilla_CMSG_AUCTION_LIST_BIDDER_ITEMS* object) {
    return 16 + 4 * object->amount_of_outbid_items;
}

static WowWorldResult vanilla_CMSG_AUCTION_LIST_BIDDER_ITEMS_read(WowWorldReader* reader, vanilla_CMSG_AUCTION_LIST_BIDDER_ITEMS* object) {
    READ_U64(object->auctioneer);

    READ_U32(object->start_from_page);

    READ_U32(object->amount_of_outbid_items);

    READ_ARRAY_ALLOCATE(object->outbid_item_ids, object->amount_of_outbid_items, sizeof(uint32_t));
    READ_ARRAY(object->outbid_item_ids, object->amount_of_outbid_items, READ_U32(object->outbid_item_ids[i]));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_AUCTION_LIST_BIDDER_ITEMS_write(WowWorldWriter* writer, const vanilla_CMSG_AUCTION_LIST_BIDDER_ITEMS* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_AUCTION_LIST_BIDDER_ITEMS_size(object) + 4); /* size */

    WRITE_U32(0x00000264); /* opcode */

    WRITE_U64(object->auctioneer);

    WRITE_U32(object->start_from_page);

    WRITE_U32(object->amount_of_outbid_items);

    WRITE_ARRAY(object->outbid_item_ids, object->amount_of_outbid_items, WRITE_U32(object->outbid_item_ids[i]));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_AUCTION_LIST_BIDDER_ITEMS_free(vanilla_CMSG_AUCTION_LIST_BIDDER_ITEMS* object) {
    free(object->outbid_item_ids);
    object->outbid_item_ids = NULL;
}

static size_t vanilla_SMSG_AUCTION_BIDDER_LIST_RESULT_size(const vanilla_SMSG_AUCTION_BIDDER_LIST_RESULT* object) {
    return 8 + 64 * object->count;
}

static WowWorldResult vanilla_SMSG_AUCTION_BIDDER_LIST_RESULT_read(WowWorldReader* reader, vanilla_SMSG_AUCTION_BIDDER_LIST_RESULT* object) {
    READ_U32(object->count);

    READ_ARRAY_ALLOCATE(object->auctions, object->count, sizeof(vanilla_AuctionListItem));
    READ_ARRAY(object->auctions, object->count, WWM_CHECK_RETURN_CODE(vanilla_AuctionListItem_read(reader, &object->auctions[i])));

    READ_U32(object->total_amount_of_auctions);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_AUCTION_BIDDER_LIST_RESULT_write(WowWorldWriter* writer, const vanilla_SMSG_AUCTION_BIDDER_LIST_RESULT* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_AUCTION_BIDDER_LIST_RESULT_size(object) + 2); /* size */

    WRITE_U16(0x00000265); /* opcode */

    WRITE_U32(object->count);

    WRITE_ARRAY(object->auctions, object->count, WWM_CHECK_RETURN_CODE(vanilla_AuctionListItem_write(writer, &object->auctions[i])));

    WRITE_U32(object->total_amount_of_auctions);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_AUCTION_BIDDER_LIST_RESULT_free(vanilla_SMSG_AUCTION_BIDDER_LIST_RESULT* object) {
    free(object->auctions);
    object->auctions = NULL;
}

static WowWorldResult vanilla_SMSG_SET_FLAT_SPELL_MODIFIER_read(WowWorldReader* reader, vanilla_SMSG_SET_FLAT_SPELL_MODIFIER* object) {
    READ_U8(object->eff);

    READ_U8(object->op);

    READ_U32(object->value);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SET_FLAT_SPELL_MODIFIER_write(WowWorldWriter* writer, const vanilla_SMSG_SET_FLAT_SPELL_MODIFIER* object) {
    WRITE_U16_BE(0x0006 + 2); /* size */

    WRITE_U16(0x00000266); /* opcode */

    WRITE_U8(object->eff);

    WRITE_U8(object->op);

    WRITE_U32(object->value);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_SET_PCT_SPELL_MODIFIER_read(WowWorldReader* reader, vanilla_SMSG_SET_PCT_SPELL_MODIFIER* object) {
    READ_U8(object->eff);

    READ_U8(object->op);

    READ_U32(object->value);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SET_PCT_SPELL_MODIFIER_write(WowWorldWriter* writer, const vanilla_SMSG_SET_PCT_SPELL_MODIFIER* object) {
    WRITE_U16_BE(0x0006 + 2); /* size */

    WRITE_U16(0x00000267); /* opcode */

    WRITE_U8(object->eff);

    WRITE_U8(object->op);

    WRITE_U32(object->value);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_SET_AMMO_read(WowWorldReader* reader, vanilla_CMSG_SET_AMMO* object) {
    READ_U32(object->item);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_SET_AMMO_write(WowWorldWriter* writer, const vanilla_CMSG_SET_AMMO* object) {
    WRITE_U16_BE(0x0004 + 4); /* size */

    WRITE_U32(0x00000268); /* opcode */

    WRITE_U32(object->item);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_CORPSE_RECLAIM_DELAY_read(WowWorldReader* reader, vanilla_SMSG_CORPSE_RECLAIM_DELAY* object) {
    READ_U32(object->delay);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_CORPSE_RECLAIM_DELAY_write(WowWorldWriter* writer, const vanilla_SMSG_CORPSE_RECLAIM_DELAY* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x00000269); /* opcode */

    WRITE_U32(object->delay);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_SET_ACTIVE_MOVER_read(WowWorldReader* reader, vanilla_CMSG_SET_ACTIVE_MOVER* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_SET_ACTIVE_MOVER_write(WowWorldWriter* writer, const vanilla_CMSG_SET_ACTIVE_MOVER* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x0000026a); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_PET_CANCEL_AURA_read(WowWorldReader* reader, vanilla_CMSG_PET_CANCEL_AURA* object) {
    READ_U64(object->guid);

    READ_U32(object->id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_PET_CANCEL_AURA_write(WowWorldWriter* writer, const vanilla_CMSG_PET_CANCEL_AURA* object) {
    WRITE_U16_BE(0x000c + 4); /* size */

    WRITE_U32(0x0000026b); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->id);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_CANCEL_AUTO_REPEAT_SPELL_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x0000026d); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_MSG_LIST_STABLED_PETS_Client_read(WowWorldReader* reader, vanilla_MSG_LIST_STABLED_PETS_Client* object) {
    READ_U64(object->npc);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_LIST_STABLED_PETS_Client_write(WowWorldWriter* writer, const vanilla_MSG_LIST_STABLED_PETS_Client* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x0000026f); /* opcode */

    WRITE_U64(object->npc);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_MSG_LIST_STABLED_PETS_Server_size(const vanilla_MSG_LIST_STABLED_PETS_Server* object) {
    size_t _size = 10;

    /* C89 scope to allow variable declarations */ {
        int i;
        for(i = 0; i < (int)object->amount_of_pets; ++i) {
            _size += vanilla_StabledPet_size(&object->pets[i]);
        }
    }

    return _size;
}

static WowWorldResult vanilla_MSG_LIST_STABLED_PETS_Server_read(WowWorldReader* reader, vanilla_MSG_LIST_STABLED_PETS_Server* object) {
    READ_U64(object->npc);

    READ_U8(object->amount_of_pets);

    READ_U8(object->stable_slots);

    READ_ARRAY_ALLOCATE(object->pets, object->amount_of_pets, sizeof(vanilla_StabledPet));
    READ_ARRAY(object->pets, object->amount_of_pets, WWM_CHECK_RETURN_CODE(vanilla_StabledPet_read(reader, &object->pets[i])));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_LIST_STABLED_PETS_Server_write(WowWorldWriter* writer, const vanilla_MSG_LIST_STABLED_PETS_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_LIST_STABLED_PETS_Server_size(object) + 2); /* size */

    WRITE_U16(0x0000026f); /* opcode */

    WRITE_U64(object->npc);

    WRITE_U8(object->amount_of_pets);

    WRITE_U8(object->stable_slots);

    WRITE_ARRAY(object->pets, object->amount_of_pets, WWM_CHECK_RETURN_CODE(vanilla_StabledPet_write(writer, &object->pets[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_LIST_STABLED_PETS_Server_free(vanilla_MSG_LIST_STABLED_PETS_Server* object) {
    size_t i;

    for (i = 0; i < object->amount_of_pets; ++i) {
        vanilla_StabledPet_free(&((object->pets)[i]));
    }
    free(object->pets);
    object->pets = NULL;
}

static WowWorldResult vanilla_CMSG_STABLE_PET_read(WowWorldReader* reader, vanilla_CMSG_STABLE_PET* object) {
    READ_U64(object->stable_master);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_STABLE_PET_write(WowWorldWriter* writer, const vanilla_CMSG_STABLE_PET* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x00000270); /* opcode */

    WRITE_U64(object->stable_master);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_UNSTABLE_PET_read(WowWorldReader* reader, vanilla_CMSG_UNSTABLE_PET* object) {
    READ_U64(object->stable_master);

    READ_U32(object->pet_number);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_UNSTABLE_PET_write(WowWorldWriter* writer, const vanilla_CMSG_UNSTABLE_PET* object) {
    WRITE_U16_BE(0x000c + 4); /* size */

    WRITE_U32(0x00000271); /* opcode */

    WRITE_U64(object->stable_master);

    WRITE_U32(object->pet_number);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_BUY_STABLE_SLOT_read(WowWorldReader* reader, vanilla_CMSG_BUY_STABLE_SLOT* object) {
    READ_U64(object->npc);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_BUY_STABLE_SLOT_write(WowWorldWriter* writer, const vanilla_CMSG_BUY_STABLE_SLOT* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x00000272); /* opcode */

    WRITE_U64(object->npc);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_STABLE_RESULT_read(WowWorldReader* reader, vanilla_SMSG_STABLE_RESULT* object) {
    object->result = 0;
    READ_U8(object->result);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_STABLE_RESULT_write(WowWorldWriter* writer, const vanilla_SMSG_STABLE_RESULT* object) {
    WRITE_U16_BE(0x0001 + 2); /* size */

    WRITE_U16(0x00000273); /* opcode */

    WRITE_U8(object->result);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_STABLE_SWAP_PET_read(WowWorldReader* reader, vanilla_CMSG_STABLE_SWAP_PET* object) {
    READ_U64(object->npc);

    READ_U32(object->pet_slot);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_STABLE_SWAP_PET_write(WowWorldWriter* writer, const vanilla_CMSG_STABLE_SWAP_PET* object) {
    WRITE_U16_BE(0x000c + 4); /* size */

    WRITE_U32(0x00000275); /* opcode */

    WRITE_U64(object->npc);

    WRITE_U32(object->pet_slot);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_MSG_QUEST_PUSH_RESULT_read(WowWorldReader* reader, vanilla_MSG_QUEST_PUSH_RESULT* object) {
    READ_U64(object->guid);

    object->message = 0;
    READ_U8(object->message);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_QUEST_PUSH_RESULT_cmsg_write(WowWorldWriter* writer, const vanilla_MSG_QUEST_PUSH_RESULT* object) {
    WRITE_U16_BE(0x0009 + 4); /* size */

    WRITE_U32(0x00000276); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U8(object->message);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_QUEST_PUSH_RESULT_smsg_write(WowWorldWriter* writer, const vanilla_MSG_QUEST_PUSH_RESULT* object) {
    WRITE_U16_BE(0x0009 + 2); /* size */

    WRITE_U16(0x00000276); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U8(object->message);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_PLAY_MUSIC_read(WowWorldReader* reader, vanilla_SMSG_PLAY_MUSIC* object) {
    READ_U32(object->sound_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PLAY_MUSIC_write(WowWorldWriter* writer, const vanilla_SMSG_PLAY_MUSIC* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x00000277); /* opcode */

    WRITE_U32(object->sound_id);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_PLAY_OBJECT_SOUND_read(WowWorldReader* reader, vanilla_SMSG_PLAY_OBJECT_SOUND* object) {
    READ_U32(object->sound_id);

    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PLAY_OBJECT_SOUND_write(WowWorldWriter* writer, const vanilla_SMSG_PLAY_OBJECT_SOUND* object) {
    WRITE_U16_BE(0x000c + 2); /* size */

    WRITE_U16(0x00000278); /* opcode */

    WRITE_U32(object->sound_id);

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_REQUEST_PET_INFO_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x00000279); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_FAR_SIGHT_read(WowWorldReader* reader, vanilla_CMSG_FAR_SIGHT* object) {
    object->operation = 0;
    READ_U8(object->operation);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_FAR_SIGHT_write(WowWorldWriter* writer, const vanilla_CMSG_FAR_SIGHT* object) {
    WRITE_U16_BE(0x0001 + 4); /* size */

    WRITE_U32(0x0000027a); /* opcode */

    WRITE_U8(object->operation);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_SPELLDISPELLOG_size(const vanilla_SMSG_SPELLDISPELLOG* object) {
    return 4 + wwm_packed_guid_size(object->victim) + wwm_packed_guid_size(object->caster) + 4 * object->amount_of_spells;
}

static WowWorldResult vanilla_SMSG_SPELLDISPELLOG_read(WowWorldReader* reader, vanilla_SMSG_SPELLDISPELLOG* object) {
    READ_PACKED_GUID(object->victim);

    READ_PACKED_GUID(object->caster);

    READ_U32(object->amount_of_spells);

    READ_ARRAY_ALLOCATE(object->spells, object->amount_of_spells, sizeof(uint32_t));
    READ_ARRAY(object->spells, object->amount_of_spells, READ_U32(object->spells[i]));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPELLDISPELLOG_write(WowWorldWriter* writer, const vanilla_SMSG_SPELLDISPELLOG* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SPELLDISPELLOG_size(object) + 2); /* size */

    WRITE_U16(0x0000027b); /* opcode */

    WRITE_PACKED_GUID(object->victim);

    WRITE_PACKED_GUID(object->caster);

    WRITE_U32(object->amount_of_spells);

    WRITE_ARRAY(object->spells, object->amount_of_spells, WRITE_U32(object->spells[i]));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_SPELLDISPELLOG_free(vanilla_SMSG_SPELLDISPELLOG* object) {
    free(object->spells);
    object->spells = NULL;
}

static size_t vanilla_CMSG_GROUP_CHANGE_SUB_GROUP_size(const vanilla_CMSG_GROUP_CHANGE_SUB_GROUP* object) {
    return 2 + STRING_SIZE(object->name);
}

static WowWorldResult vanilla_CMSG_GROUP_CHANGE_SUB_GROUP_read(WowWorldReader* reader, vanilla_CMSG_GROUP_CHANGE_SUB_GROUP* object) {
    READ_CSTRING(object->name);

    READ_U8(object->group_number);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GROUP_CHANGE_SUB_GROUP_write(WowWorldWriter* writer, const vanilla_CMSG_GROUP_CHANGE_SUB_GROUP* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_GROUP_CHANGE_SUB_GROUP_size(object) + 4); /* size */

    WRITE_U32(0x0000027e); /* opcode */

    WRITE_CSTRING(object->name);

    WRITE_U8(object->group_number);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_GROUP_CHANGE_SUB_GROUP_free(vanilla_CMSG_GROUP_CHANGE_SUB_GROUP* object) {
    FREE_STRING(object->name);

}

static WowWorldResult vanilla_CMSG_REQUEST_PARTY_MEMBER_STATS_read(WowWorldReader* reader, vanilla_CMSG_REQUEST_PARTY_MEMBER_STATS* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_REQUEST_PARTY_MEMBER_STATS_write(WowWorldWriter* writer, const vanilla_CMSG_REQUEST_PARTY_MEMBER_STATS* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x0000027f); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_GROUP_SWAP_SUB_GROUP_size(const vanilla_CMSG_GROUP_SWAP_SUB_GROUP* object) {
    return 2 + STRING_SIZE(object->name) + STRING_SIZE(object->swap_with_name);
}

static WowWorldResult vanilla_CMSG_GROUP_SWAP_SUB_GROUP_read(WowWorldReader* reader, vanilla_CMSG_GROUP_SWAP_SUB_GROUP* object) {
    READ_CSTRING(object->name);

    READ_CSTRING(object->swap_with_name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GROUP_SWAP_SUB_GROUP_write(WowWorldWriter* writer, const vanilla_CMSG_GROUP_SWAP_SUB_GROUP* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_GROUP_SWAP_SUB_GROUP_size(object) + 4); /* size */

    WRITE_U32(0x00000280); /* opcode */

    WRITE_CSTRING(object->name);

    WRITE_CSTRING(object->swap_with_name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_GROUP_SWAP_SUB_GROUP_free(vanilla_CMSG_GROUP_SWAP_SUB_GROUP* object) {
    FREE_STRING(object->name);

    FREE_STRING(object->swap_with_name);

}

static WowWorldResult vanilla_CMSG_AUTOSTORE_BANK_ITEM_read(WowWorldReader* reader, vanilla_CMSG_AUTOSTORE_BANK_ITEM* object) {
    READ_U8(object->bag_index);

    READ_U8(object->slot_index);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_AUTOSTORE_BANK_ITEM_write(WowWorldWriter* writer, const vanilla_CMSG_AUTOSTORE_BANK_ITEM* object) {
    WRITE_U16_BE(0x0002 + 4); /* size */

    WRITE_U32(0x00000282); /* opcode */

    WRITE_U8(object->bag_index);

    WRITE_U8(object->slot_index);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_AUTOBANK_ITEM_read(WowWorldReader* reader, vanilla_CMSG_AUTOBANK_ITEM* object) {
    READ_U8(object->bag_index);

    READ_U8(object->slot_index);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_AUTOBANK_ITEM_write(WowWorldWriter* writer, const vanilla_CMSG_AUTOBANK_ITEM* object) {
    WRITE_U16_BE(0x0002 + 4); /* size */

    WRITE_U32(0x00000283); /* opcode */

    WRITE_U8(object->bag_index);

    WRITE_U8(object->slot_index);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_MSG_QUERY_NEXT_MAIL_TIME_Server_read(WowWorldReader* reader, vanilla_MSG_QUERY_NEXT_MAIL_TIME_Server* object) {
    READ_FLOAT(object->unread_mails);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_QUERY_NEXT_MAIL_TIME_Server_write(WowWorldWriter* writer, const vanilla_MSG_QUERY_NEXT_MAIL_TIME_Server* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x00000284); /* opcode */

    WRITE_FLOAT(object->unread_mails);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_QUERY_NEXT_MAIL_TIME_Client_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x00000284); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_RECEIVED_MAIL_read(WowWorldReader* reader, vanilla_SMSG_RECEIVED_MAIL* object) {
    READ_U32(object->unknown1);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_RECEIVED_MAIL_write(WowWorldWriter* writer, const vanilla_SMSG_RECEIVED_MAIL* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x00000285); /* opcode */

    WRITE_U32(object->unknown1);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_RAID_GROUP_ONLY_read(WowWorldReader* reader, vanilla_SMSG_RAID_GROUP_ONLY* object) {
    READ_U32(object->homebind_timer);

    object->error = 0;
    READ_U32(object->error);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_RAID_GROUP_ONLY_write(WowWorldWriter* writer, const vanilla_SMSG_RAID_GROUP_ONLY* object) {
    WRITE_U16_BE(0x0008 + 2); /* size */

    WRITE_U16(0x00000286); /* opcode */

    WRITE_U32(object->homebind_timer);

    WRITE_U32(object->error);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_PVP_CREDIT_read(WowWorldReader* reader, vanilla_SMSG_PVP_CREDIT* object) {
    READ_U32(object->honor_points);

    READ_U64(object->victim);

    object->rank = 0;
    READ_U32(object->rank);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PVP_CREDIT_write(WowWorldWriter* writer, const vanilla_SMSG_PVP_CREDIT* object) {
    WRITE_U16_BE(0x0010 + 2); /* size */

    WRITE_U16(0x0000028c); /* opcode */

    WRITE_U32(object->honor_points);

    WRITE_U64(object->victim);

    WRITE_U32(object->rank);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_AUCTION_REMOVED_NOTIFICATION_read(WowWorldReader* reader, vanilla_SMSG_AUCTION_REMOVED_NOTIFICATION* object) {
    READ_U32(object->item);

    READ_U32(object->item_template);

    READ_U32(object->random_property_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_AUCTION_REMOVED_NOTIFICATION_write(WowWorldWriter* writer, const vanilla_SMSG_AUCTION_REMOVED_NOTIFICATION* object) {
    WRITE_U16_BE(0x000c + 2); /* size */

    WRITE_U16(0x0000028d); /* opcode */

    WRITE_U32(object->item);

    WRITE_U32(object->item_template);

    WRITE_U32(object->random_property_id);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GROUP_RAID_CONVERT_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x0000028e); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_GROUP_ASSISTANT_LEADER_read(WowWorldReader* reader, vanilla_CMSG_GROUP_ASSISTANT_LEADER* object) {
    READ_U64(object->guid);

    READ_BOOL8(object->set_assistant);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GROUP_ASSISTANT_LEADER_write(WowWorldWriter* writer, const vanilla_CMSG_GROUP_ASSISTANT_LEADER* object) {
    WRITE_U16_BE(0x0009 + 4); /* size */

    WRITE_U32(0x0000028f); /* opcode */

    WRITE_U64(object->guid);

    WRITE_BOOL8(object->set_assistant);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_BUYBACK_ITEM_read(WowWorldReader* reader, vanilla_CMSG_BUYBACK_ITEM* object) {
    READ_U64(object->guid);

    object->slot = 0;
    READ_U32(object->slot);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_BUYBACK_ITEM_write(WowWorldWriter* writer, const vanilla_CMSG_BUYBACK_ITEM* object) {
    WRITE_U16_BE(0x000c + 4); /* size */

    WRITE_U32(0x00000290); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->slot);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_SERVER_MESSAGE_size(const vanilla_SMSG_SERVER_MESSAGE* object) {
    return 5 + STRING_SIZE(object->message);
}

static WowWorldResult vanilla_SMSG_SERVER_MESSAGE_read(WowWorldReader* reader, vanilla_SMSG_SERVER_MESSAGE* object) {
    object->message_type = 0;
    READ_U32(object->message_type);

    READ_CSTRING(object->message);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SERVER_MESSAGE_write(WowWorldWriter* writer, const vanilla_SMSG_SERVER_MESSAGE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SERVER_MESSAGE_size(object) + 2); /* size */

    WRITE_U16(0x00000291); /* opcode */

    WRITE_U32(object->message_type);

    WRITE_CSTRING(object->message);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_SERVER_MESSAGE_free(vanilla_SMSG_SERVER_MESSAGE* object) {
    FREE_STRING(object->message);

}

static WowWorldResult vanilla_CMSG_MEETINGSTONE_JOIN_read(WowWorldReader* reader, vanilla_CMSG_MEETINGSTONE_JOIN* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_MEETINGSTONE_JOIN_write(WowWorldWriter* writer, const vanilla_CMSG_MEETINGSTONE_JOIN* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x00000292); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_MEETINGSTONE_LEAVE_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x00000293); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_MEETINGSTONE_SETQUEUE_read(WowWorldReader* reader, vanilla_SMSG_MEETINGSTONE_SETQUEUE* object) {
    object->area = 0;
    READ_U32(object->area);

    object->status = 0;
    READ_U8(object->status);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_MEETINGSTONE_SETQUEUE_write(WowWorldWriter* writer, const vanilla_SMSG_MEETINGSTONE_SETQUEUE* object) {
    WRITE_U16_BE(0x0005 + 2); /* size */

    WRITE_U16(0x00000295); /* opcode */

    WRITE_U32(object->area);

    WRITE_U8(object->status);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_MEETINGSTONE_INFO_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x00000296); /* opcode */


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_MEETINGSTONE_COMPLETE_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 2); /* size */

    WRITE_U16(0x00000297); /* opcode */


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_MEETINGSTONE_IN_PROGRESS_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 2); /* size */

    WRITE_U16(0x00000298); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_MEETINGSTONE_MEMBER_ADDED_read(WowWorldReader* reader, vanilla_SMSG_MEETINGSTONE_MEMBER_ADDED* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_MEETINGSTONE_MEMBER_ADDED_write(WowWorldWriter* writer, const vanilla_SMSG_MEETINGSTONE_MEMBER_ADDED* object) {
    WRITE_U16_BE(0x0008 + 2); /* size */

    WRITE_U16(0x00000299); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_CANCEL_GROWTH_AURA_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x0000029b); /* opcode */


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_CANCEL_AUTO_REPEAT_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 2); /* size */

    WRITE_U16(0x0000029c); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_STANDSTATE_UPDATE_read(WowWorldReader* reader, vanilla_SMSG_STANDSTATE_UPDATE* object) {
    object->state = 0;
    READ_U8(object->state);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_STANDSTATE_UPDATE_write(WowWorldWriter* writer, const vanilla_SMSG_STANDSTATE_UPDATE* object) {
    WRITE_U16_BE(0x0001 + 2); /* size */

    WRITE_U16(0x0000029d); /* opcode */

    WRITE_U8(object->state);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_LOOT_ALL_PASSED_read(WowWorldReader* reader, vanilla_SMSG_LOOT_ALL_PASSED* object) {
    READ_U64(object->looted_target);

    READ_U32(object->loot_slot);

    READ_U32(object->item);

    READ_U32(object->item_random_property_id);

    READ_U32(object->item_random_suffix_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_LOOT_ALL_PASSED_write(WowWorldWriter* writer, const vanilla_SMSG_LOOT_ALL_PASSED* object) {
    WRITE_U16_BE(0x0018 + 2); /* size */

    WRITE_U16(0x0000029e); /* opcode */

    WRITE_U64(object->looted_target);

    WRITE_U32(object->loot_slot);

    WRITE_U32(object->item);

    WRITE_U32(object->item_random_property_id);

    WRITE_U32(object->item_random_suffix_id);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_LOOT_ROLL_WON_read(WowWorldReader* reader, vanilla_SMSG_LOOT_ROLL_WON* object) {
    READ_U64(object->looted_target);

    READ_U32(object->loot_slot);

    READ_U32(object->item);

    READ_U32(object->item_random_suffix);

    READ_U32(object->item_random_property_id);

    READ_U64(object->winning_player);

    READ_U8(object->winning_roll);

    object->vote = 0;
    READ_U8(object->vote);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_LOOT_ROLL_WON_write(WowWorldWriter* writer, const vanilla_SMSG_LOOT_ROLL_WON* object) {
    WRITE_U16_BE(0x0022 + 2); /* size */

    WRITE_U16(0x0000029f); /* opcode */

    WRITE_U64(object->looted_target);

    WRITE_U32(object->loot_slot);

    WRITE_U32(object->item);

    WRITE_U32(object->item_random_suffix);

    WRITE_U32(object->item_random_property_id);

    WRITE_U64(object->winning_player);

    WRITE_U8(object->winning_roll);

    WRITE_U8(object->vote);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_LOOT_ROLL_read(WowWorldReader* reader, vanilla_CMSG_LOOT_ROLL* object) {
    READ_U64(object->item);

    READ_U32(object->item_slot);

    object->vote = 0;
    READ_U8(object->vote);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_LOOT_ROLL_write(WowWorldWriter* writer, const vanilla_CMSG_LOOT_ROLL* object) {
    WRITE_U16_BE(0x000d + 4); /* size */

    WRITE_U32(0x000002a0); /* opcode */

    WRITE_U64(object->item);

    WRITE_U32(object->item_slot);

    WRITE_U8(object->vote);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_LOOT_START_ROLL_read(WowWorldReader* reader, vanilla_SMSG_LOOT_START_ROLL* object) {
    READ_U64(object->creature);

    READ_U32(object->loot_slot);

    READ_U32(object->item);

    READ_U32(object->item_random_suffix);

    READ_U32(object->item_random_property_id);

    READ_U32(object->countdown_time);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_LOOT_START_ROLL_write(WowWorldWriter* writer, const vanilla_SMSG_LOOT_START_ROLL* object) {
    WRITE_U16_BE(0x001c + 2); /* size */

    WRITE_U16(0x000002a1); /* opcode */

    WRITE_U64(object->creature);

    WRITE_U32(object->loot_slot);

    WRITE_U32(object->item);

    WRITE_U32(object->item_random_suffix);

    WRITE_U32(object->item_random_property_id);

    WRITE_U32(object->countdown_time);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_LOOT_ROLL_read(WowWorldReader* reader, vanilla_SMSG_LOOT_ROLL* object) {
    READ_U64(object->creature);

    READ_U32(object->loot_slot);

    READ_U64(object->player);

    READ_U32(object->item);

    READ_U32(object->item_random_suffix);

    READ_U32(object->item_random_property_id);

    READ_U8(object->roll_number);

    object->vote = 0;
    READ_U8(object->vote);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_LOOT_ROLL_write(WowWorldWriter* writer, const vanilla_SMSG_LOOT_ROLL* object) {
    WRITE_U16_BE(0x0022 + 2); /* size */

    WRITE_U16(0x000002a2); /* opcode */

    WRITE_U64(object->creature);

    WRITE_U32(object->loot_slot);

    WRITE_U64(object->player);

    WRITE_U32(object->item);

    WRITE_U32(object->item_random_suffix);

    WRITE_U32(object->item_random_property_id);

    WRITE_U8(object->roll_number);

    WRITE_U8(object->vote);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_LOOT_MASTER_GIVE_read(WowWorldReader* reader, vanilla_CMSG_LOOT_MASTER_GIVE* object) {
    READ_U64(object->loot);

    READ_U8(object->slot_id);

    READ_U64(object->player);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_LOOT_MASTER_GIVE_write(WowWorldWriter* writer, const vanilla_CMSG_LOOT_MASTER_GIVE* object) {
    WRITE_U16_BE(0x0011 + 4); /* size */

    WRITE_U32(0x000002a3); /* opcode */

    WRITE_U64(object->loot);

    WRITE_U8(object->slot_id);

    WRITE_U64(object->player);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_LOOT_MASTER_LIST_size(const vanilla_SMSG_LOOT_MASTER_LIST* object) {
    return 1 + 8 * object->amount_of_players;
}

static WowWorldResult vanilla_SMSG_LOOT_MASTER_LIST_read(WowWorldReader* reader, vanilla_SMSG_LOOT_MASTER_LIST* object) {
    READ_U8(object->amount_of_players);

    READ_ARRAY_ALLOCATE(object->guids, object->amount_of_players, sizeof(uint64_t));
    READ_ARRAY(object->guids, object->amount_of_players, READ_U64(object->guids[i]));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_LOOT_MASTER_LIST_write(WowWorldWriter* writer, const vanilla_SMSG_LOOT_MASTER_LIST* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_LOOT_MASTER_LIST_size(object) + 2); /* size */

    WRITE_U16(0x000002a4); /* opcode */

    WRITE_U8(object->amount_of_players);

    WRITE_ARRAY(object->guids, object->amount_of_players, WRITE_U64(object->guids[i]));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_LOOT_MASTER_LIST_free(vanilla_SMSG_LOOT_MASTER_LIST* object) {
    free(object->guids);
    object->guids = NULL;
}

static size_t vanilla_SMSG_SET_FORCED_REACTIONS_size(const vanilla_SMSG_SET_FORCED_REACTIONS* object) {
    return 4 + 6 * object->amount_of_reactions;
}

static WowWorldResult vanilla_SMSG_SET_FORCED_REACTIONS_read(WowWorldReader* reader, vanilla_SMSG_SET_FORCED_REACTIONS* object) {
    READ_U32(object->amount_of_reactions);

    READ_ARRAY_ALLOCATE(object->reactions, object->amount_of_reactions, sizeof(vanilla_ForcedReaction));
    READ_ARRAY(object->reactions, object->amount_of_reactions, WWM_CHECK_RETURN_CODE(vanilla_ForcedReaction_read(reader, &object->reactions[i])));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SET_FORCED_REACTIONS_write(WowWorldWriter* writer, const vanilla_SMSG_SET_FORCED_REACTIONS* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SET_FORCED_REACTIONS_size(object) + 2); /* size */

    WRITE_U16(0x000002a5); /* opcode */

    WRITE_U32(object->amount_of_reactions);

    WRITE_ARRAY(object->reactions, object->amount_of_reactions, WWM_CHECK_RETURN_CODE(vanilla_ForcedReaction_write(writer, &object->reactions[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_SET_FORCED_REACTIONS_free(vanilla_SMSG_SET_FORCED_REACTIONS* object) {
    free(object->reactions);
    object->reactions = NULL;
}

static WowWorldResult vanilla_SMSG_SPELL_FAILED_OTHER_read(WowWorldReader* reader, vanilla_SMSG_SPELL_FAILED_OTHER* object) {
    READ_U64(object->caster);

    READ_U32(object->id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPELL_FAILED_OTHER_write(WowWorldWriter* writer, const vanilla_SMSG_SPELL_FAILED_OTHER* object) {
    WRITE_U16_BE(0x000c + 2); /* size */

    WRITE_U16(0x000002a6); /* opcode */

    WRITE_U64(object->caster);

    WRITE_U32(object->id);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_GAMEOBJECT_RESET_STATE_read(WowWorldReader* reader, vanilla_SMSG_GAMEOBJECT_RESET_STATE* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_GAMEOBJECT_RESET_STATE_write(WowWorldWriter* writer, const vanilla_SMSG_GAMEOBJECT_RESET_STATE* object) {
    WRITE_U16_BE(0x0008 + 2); /* size */

    WRITE_U16(0x000002a7); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_REPAIR_ITEM_read(WowWorldReader* reader, vanilla_CMSG_REPAIR_ITEM* object) {
    READ_U64(object->npc);

    READ_U64(object->item);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_REPAIR_ITEM_write(WowWorldWriter* writer, const vanilla_CMSG_REPAIR_ITEM* object) {
    WRITE_U16_BE(0x0010 + 4); /* size */

    WRITE_U32(0x000002a8); /* opcode */

    WRITE_U64(object->npc);

    WRITE_U64(object->item);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_CHAT_PLAYER_NOT_FOUND_size(const vanilla_SMSG_CHAT_PLAYER_NOT_FOUND* object) {
    return 1 + STRING_SIZE(object->name);
}

static WowWorldResult vanilla_SMSG_CHAT_PLAYER_NOT_FOUND_read(WowWorldReader* reader, vanilla_SMSG_CHAT_PLAYER_NOT_FOUND* object) {
    READ_CSTRING(object->name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_CHAT_PLAYER_NOT_FOUND_write(WowWorldWriter* writer, const vanilla_SMSG_CHAT_PLAYER_NOT_FOUND* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_CHAT_PLAYER_NOT_FOUND_size(object) + 2); /* size */

    WRITE_U16(0x000002a9); /* opcode */

    WRITE_CSTRING(object->name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_CHAT_PLAYER_NOT_FOUND_free(vanilla_SMSG_CHAT_PLAYER_NOT_FOUND* object) {
    FREE_STRING(object->name);

}

static WowWorldResult vanilla_MSG_TALENT_WIPE_CONFIRM_Client_read(WowWorldReader* reader, vanilla_MSG_TALENT_WIPE_CONFIRM_Client* object) {
    READ_U64(object->wiping_npc);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_TALENT_WIPE_CONFIRM_Client_write(WowWorldWriter* writer, const vanilla_MSG_TALENT_WIPE_CONFIRM_Client* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x000002aa); /* opcode */

    WRITE_U64(object->wiping_npc);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_MSG_TALENT_WIPE_CONFIRM_Server_read(WowWorldReader* reader, vanilla_MSG_TALENT_WIPE_CONFIRM_Server* object) {
    READ_U64(object->wiping_npc);

    READ_U32(object->cost_in_copper);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_TALENT_WIPE_CONFIRM_Server_write(WowWorldWriter* writer, const vanilla_MSG_TALENT_WIPE_CONFIRM_Server* object) {
    WRITE_U16_BE(0x000c + 2); /* size */

    WRITE_U16(0x000002aa); /* opcode */

    WRITE_U64(object->wiping_npc);

    WRITE_U32(object->cost_in_copper);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_SUMMON_REQUEST_read(WowWorldReader* reader, vanilla_SMSG_SUMMON_REQUEST* object) {
    READ_U64(object->summoner);

    object->area = 0;
    READ_U32(object->area);

    READ_U32(object->auto_decline_time);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SUMMON_REQUEST_write(WowWorldWriter* writer, const vanilla_SMSG_SUMMON_REQUEST* object) {
    WRITE_U16_BE(0x0010 + 2); /* size */

    WRITE_U16(0x000002ab); /* opcode */

    WRITE_U64(object->summoner);

    WRITE_U32(object->area);

    WRITE_U32(object->auto_decline_time);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_SUMMON_RESPONSE_read(WowWorldReader* reader, vanilla_CMSG_SUMMON_RESPONSE* object) {
    READ_U64(object->summoner);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_SUMMON_RESPONSE_write(WowWorldWriter* writer, const vanilla_CMSG_SUMMON_RESPONSE* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x000002ac); /* opcode */

    WRITE_U64(object->summoner);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_MONSTER_MOVE_TRANSPORT_size(const vanilla_SMSG_MONSTER_MOVE_TRANSPORT* object) {
    size_t _size = 25 + wwm_packed_guid_size(object->guid) + wwm_packed_guid_size(object->transport) + wwm_monster_move_spline_size(&object->splines);

    if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_TARGET) {
        _size += 8;
    }
    else if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_ANGLE) {
        _size += 4;
    }
    else if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_SPOT) {
        _size += 12;
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_MONSTER_MOVE_TRANSPORT_read(WowWorldReader* reader, vanilla_SMSG_MONSTER_MOVE_TRANSPORT* object) {
    READ_PACKED_GUID(object->guid);

    READ_PACKED_GUID(object->transport);

    WWM_CHECK_RETURN_CODE(all_Vector3d_read(reader, &object->spline_point));

    READ_U32(object->spline_id);

    object->move_type = 0;
    READ_U8(object->move_type);

    if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_TARGET) {
        READ_U64(object->target);

    }
    else if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_ANGLE) {
        READ_FLOAT(object->angle);

    }
    else if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_SPOT) {
        WWM_CHECK_RETURN_CODE(all_Vector3d_read(reader, &object->position));

    }
    object->spline_flags = 0;
    READ_U32(object->spline_flags);

    READ_U32(object->duration);

    READ_MONSTER_MOVE_SPLINE(object->splines);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_MONSTER_MOVE_TRANSPORT_write(WowWorldWriter* writer, const vanilla_SMSG_MONSTER_MOVE_TRANSPORT* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_MONSTER_MOVE_TRANSPORT_size(object) + 2); /* size */

    WRITE_U16(0x000002ae); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WRITE_PACKED_GUID(object->transport);

    WWM_CHECK_RETURN_CODE(all_Vector3d_write(writer, &object->spline_point));

    WRITE_U32(object->spline_id);

    WRITE_U8(object->move_type);

    if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_TARGET) {
        WRITE_U64(object->target);

    }
    else if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_ANGLE) {
        WRITE_FLOAT(object->angle);

    }
    else if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_SPOT) {
        WWM_CHECK_RETURN_CODE(all_Vector3d_write(writer, &object->position));

    }
    WRITE_U32(object->spline_flags);

    WRITE_U32(object->duration);

    WRITE_MONSTER_MOVE_SPLINE(object->splines);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_MONSTER_MOVE_TRANSPORT_free(vanilla_SMSG_MONSTER_MOVE_TRANSPORT* object) {
    if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_TARGET) {
    }
    else if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_ANGLE) {
    }
    else if (object->move_type == VANILLA_MONSTER_MOVE_TYPE_FACING_SPOT) {
    }
    wwm_monster_move_spline_free(&object->splines);

}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PET_BROKEN_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 2); /* size */

    WRITE_U16(0x000002af); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_MSG_MOVE_FEATHER_FALL_Server_size(const vanilla_MSG_MOVE_FEATHER_FALL_Server* object) {
    return 0 + wwm_packed_guid_size(object->player) + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_FEATHER_FALL_Server_read(WowWorldReader* reader, vanilla_MSG_MOVE_FEATHER_FALL_Server* object) {
    READ_PACKED_GUID(object->player);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_FEATHER_FALL_Server_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_FEATHER_FALL_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_FEATHER_FALL_Server_size(object) + 2); /* size */

    WRITE_U16(0x000002b0); /* opcode */

    WRITE_PACKED_GUID(object->player);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_FEATHER_FALL_Server_free(vanilla_MSG_MOVE_FEATHER_FALL_Server* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_MSG_MOVE_WATER_WALK_size(const vanilla_MSG_MOVE_WATER_WALK* object) {
    return 0 + wwm_packed_guid_size(object->player) + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_MSG_MOVE_WATER_WALK_read(WowWorldReader* reader, vanilla_MSG_MOVE_WATER_WALK* object) {
    READ_PACKED_GUID(object->player);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_WATER_WALK_cmsg_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_WATER_WALK* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_WATER_WALK_size(object) + 4); /* size */

    WRITE_U32(0x000002b1); /* opcode */

    WRITE_PACKED_GUID(object->player);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_WATER_WALK_smsg_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_WATER_WALK* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_WATER_WALK_size(object) + 2); /* size */

    WRITE_U16(0x000002b1); /* opcode */

    WRITE_PACKED_GUID(object->player);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_MOVE_WATER_WALK_free(vanilla_MSG_MOVE_WATER_WALK* object) {
    vanilla_MovementInfo_free(&object->info);
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_SELF_RES_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x000002b3); /* opcode */


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_FEIGN_DEATH_RESISTED_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 2); /* size */

    WRITE_U16(0x000002b4); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_DUEL_COUNTDOWN_read(WowWorldReader* reader, vanilla_SMSG_DUEL_COUNTDOWN* object) {
    READ_U32(object->time);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_DUEL_COUNTDOWN_write(WowWorldWriter* writer, const vanilla_SMSG_DUEL_COUNTDOWN* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x000002b7); /* opcode */

    WRITE_U32(object->time);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_AREA_TRIGGER_MESSAGE_size(const vanilla_SMSG_AREA_TRIGGER_MESSAGE* object) {
    return 4 + STRING_SIZE(object->message);
}

static WowWorldResult vanilla_SMSG_AREA_TRIGGER_MESSAGE_read(WowWorldReader* reader, vanilla_SMSG_AREA_TRIGGER_MESSAGE* object) {
    READ_SIZED_CSTRING(object->message);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_AREA_TRIGGER_MESSAGE_write(WowWorldWriter* writer, const vanilla_SMSG_AREA_TRIGGER_MESSAGE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_AREA_TRIGGER_MESSAGE_size(object) + 2); /* size */

    WRITE_U16(0x000002b8); /* opcode */

    WRITE_SIZED_CSTRING(object->message);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_AREA_TRIGGER_MESSAGE_free(vanilla_SMSG_AREA_TRIGGER_MESSAGE* object) {
    FREE_STRING(object->message);

}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_TOGGLE_HELM_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x000002b9); /* opcode */


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_TOGGLE_CLOAK_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x000002ba); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_MEETINGSTONE_JOINFAILED_read(WowWorldReader* reader, vanilla_SMSG_MEETINGSTONE_JOINFAILED* object) {
    object->reason = 0;
    READ_U8(object->reason);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_MEETINGSTONE_JOINFAILED_write(WowWorldWriter* writer, const vanilla_SMSG_MEETINGSTONE_JOINFAILED* object) {
    WRITE_U16_BE(0x0001 + 2); /* size */

    WRITE_U16(0x000002bb); /* opcode */

    WRITE_U8(object->reason);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_PLAYER_SKINNED_read(WowWorldReader* reader, vanilla_SMSG_PLAYER_SKINNED* object) {
    READ_BOOL8(object->spirit_released);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PLAYER_SKINNED_write(WowWorldWriter* writer, const vanilla_SMSG_PLAYER_SKINNED* object) {
    WRITE_U16_BE(0x0001 + 2); /* size */

    WRITE_U16(0x000002bc); /* opcode */

    WRITE_BOOL8(object->spirit_released);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_DURABILITY_DAMAGE_DEATH_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 2); /* size */

    WRITE_U16(0x000002bd); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_SET_ACTIONBAR_TOGGLES_read(WowWorldReader* reader, vanilla_CMSG_SET_ACTIONBAR_TOGGLES* object) {
    READ_U8(object->action_bar);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_SET_ACTIONBAR_TOGGLES_write(WowWorldWriter* writer, const vanilla_CMSG_SET_ACTIONBAR_TOGGLES* object) {
    WRITE_U16_BE(0x0001 + 4); /* size */

    WRITE_U32(0x000002bf); /* opcode */

    WRITE_U8(object->action_bar);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_MSG_PETITION_RENAME_size(const vanilla_MSG_PETITION_RENAME* object) {
    return 9 + STRING_SIZE(object->new_name);
}

static WowWorldResult vanilla_MSG_PETITION_RENAME_read(WowWorldReader* reader, vanilla_MSG_PETITION_RENAME* object) {
    READ_U64(object->petition);

    READ_CSTRING(object->new_name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_PETITION_RENAME_cmsg_write(WowWorldWriter* writer, const vanilla_MSG_PETITION_RENAME* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_PETITION_RENAME_size(object) + 4); /* size */

    WRITE_U32(0x000002c1); /* opcode */

    WRITE_U64(object->petition);

    WRITE_CSTRING(object->new_name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_PETITION_RENAME_smsg_write(WowWorldWriter* writer, const vanilla_MSG_PETITION_RENAME* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_PETITION_RENAME_size(object) + 2); /* size */

    WRITE_U16(0x000002c1); /* opcode */

    WRITE_U64(object->petition);

    WRITE_CSTRING(object->new_name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_PETITION_RENAME_free(vanilla_MSG_PETITION_RENAME* object) {
    FREE_STRING(object->new_name);

}

static size_t vanilla_SMSG_INIT_WORLD_STATES_size(const vanilla_SMSG_INIT_WORLD_STATES* object) {
    return 10 + 8 * object->amount_of_states;
}

static WowWorldResult vanilla_SMSG_INIT_WORLD_STATES_read(WowWorldReader* reader, vanilla_SMSG_INIT_WORLD_STATES* object) {
    object->map = 0;
    READ_U32(object->map);

    object->area = 0;
    READ_U32(object->area);

    READ_U16(object->amount_of_states);

    READ_ARRAY_ALLOCATE(object->states, object->amount_of_states, sizeof(vanilla_WorldState));
    READ_ARRAY(object->states, object->amount_of_states, WWM_CHECK_RETURN_CODE(vanilla_WorldState_read(reader, &object->states[i])));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_INIT_WORLD_STATES_write(WowWorldWriter* writer, const vanilla_SMSG_INIT_WORLD_STATES* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_INIT_WORLD_STATES_size(object) + 2); /* size */

    WRITE_U16(0x000002c2); /* opcode */

    WRITE_U32(object->map);

    WRITE_U32(object->area);

    WRITE_U16(object->amount_of_states);

    WRITE_ARRAY(object->states, object->amount_of_states, WWM_CHECK_RETURN_CODE(vanilla_WorldState_write(writer, &object->states[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_INIT_WORLD_STATES_free(vanilla_SMSG_INIT_WORLD_STATES* object) {
    free(object->states);
    object->states = NULL;
}

static WowWorldResult vanilla_SMSG_UPDATE_WORLD_STATE_read(WowWorldReader* reader, vanilla_SMSG_UPDATE_WORLD_STATE* object) {
    WWM_CHECK_RETURN_CODE(vanilla_WorldState_read(reader, &object->state));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_UPDATE_WORLD_STATE_write(WowWorldWriter* writer, const vanilla_SMSG_UPDATE_WORLD_STATE* object) {
    WRITE_U16_BE(0x0008 + 2); /* size */

    WRITE_U16(0x000002c3); /* opcode */

    WWM_CHECK_RETURN_CODE(vanilla_WorldState_write(writer, &object->state));


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_ITEM_NAME_QUERY_read(WowWorldReader* reader, vanilla_CMSG_ITEM_NAME_QUERY* object) {
    READ_U32(object->item);

    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_ITEM_NAME_QUERY_write(WowWorldWriter* writer, const vanilla_CMSG_ITEM_NAME_QUERY* object) {
    WRITE_U16_BE(0x000c + 4); /* size */

    WRITE_U32(0x000002c4); /* opcode */

    WRITE_U32(object->item);

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_ITEM_NAME_QUERY_RESPONSE_size(const vanilla_SMSG_ITEM_NAME_QUERY_RESPONSE* object) {
    return 5 + STRING_SIZE(object->item_name);
}

static WowWorldResult vanilla_SMSG_ITEM_NAME_QUERY_RESPONSE_read(WowWorldReader* reader, vanilla_SMSG_ITEM_NAME_QUERY_RESPONSE* object) {
    READ_U32(object->item);

    READ_CSTRING(object->item_name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_ITEM_NAME_QUERY_RESPONSE_write(WowWorldWriter* writer, const vanilla_SMSG_ITEM_NAME_QUERY_RESPONSE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_ITEM_NAME_QUERY_RESPONSE_size(object) + 2); /* size */

    WRITE_U16(0x000002c5); /* opcode */

    WRITE_U32(object->item);

    WRITE_CSTRING(object->item_name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_ITEM_NAME_QUERY_RESPONSE_free(vanilla_SMSG_ITEM_NAME_QUERY_RESPONSE* object) {
    FREE_STRING(object->item_name);

}

static WowWorldResult vanilla_SMSG_PET_ACTION_FEEDBACK_read(WowWorldReader* reader, vanilla_SMSG_PET_ACTION_FEEDBACK* object) {
    object->feedback = 0;
    READ_U8(object->feedback);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PET_ACTION_FEEDBACK_write(WowWorldWriter* writer, const vanilla_SMSG_PET_ACTION_FEEDBACK* object) {
    WRITE_U16_BE(0x0001 + 2); /* size */

    WRITE_U16(0x000002c6); /* opcode */

    WRITE_U8(object->feedback);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_CHAR_RENAME_size(const vanilla_CMSG_CHAR_RENAME* object) {
    return 9 + STRING_SIZE(object->new_name);
}

static WowWorldResult vanilla_CMSG_CHAR_RENAME_read(WowWorldReader* reader, vanilla_CMSG_CHAR_RENAME* object) {
    READ_U64(object->character);

    READ_CSTRING(object->new_name);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_CHAR_RENAME_write(WowWorldWriter* writer, const vanilla_CMSG_CHAR_RENAME* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_CHAR_RENAME_size(object) + 4); /* size */

    WRITE_U32(0x000002c7); /* opcode */

    WRITE_U64(object->character);

    WRITE_CSTRING(object->new_name);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_CHAR_RENAME_free(vanilla_CMSG_CHAR_RENAME* object) {
    FREE_STRING(object->new_name);

}

static size_t vanilla_SMSG_CHAR_RENAME_size(const vanilla_SMSG_CHAR_RENAME* object) {
    size_t _size = 1;

    if (object->result == VANILLA_WORLD_RESULT_RESPONSE_SUCCESS) {
        _size += 9 + STRING_SIZE(object->new_name);
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_CHAR_RENAME_read(WowWorldReader* reader, vanilla_SMSG_CHAR_RENAME* object) {
    object->result = 0;
    READ_U8(object->result);

    if (object->result == VANILLA_WORLD_RESULT_RESPONSE_SUCCESS) {
        READ_U64(object->character);

        READ_CSTRING(object->new_name);

    }
    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_CHAR_RENAME_write(WowWorldWriter* writer, const vanilla_SMSG_CHAR_RENAME* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_CHAR_RENAME_size(object) + 2); /* size */

    WRITE_U16(0x000002c8); /* opcode */

    WRITE_U8(object->result);

    if (object->result == VANILLA_WORLD_RESULT_RESPONSE_SUCCESS) {
        WRITE_U64(object->character);

        WRITE_CSTRING(object->new_name);

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_CHAR_RENAME_free(vanilla_SMSG_CHAR_RENAME* object) {
    if (object->result == VANILLA_WORLD_RESULT_RESPONSE_SUCCESS) {
        FREE_STRING(object->new_name);

    }
}

static size_t vanilla_CMSG_MOVE_SPLINE_DONE_size(const vanilla_CMSG_MOVE_SPLINE_DONE* object) {
    return 8 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_CMSG_MOVE_SPLINE_DONE_read(WowWorldReader* reader, vanilla_CMSG_MOVE_SPLINE_DONE* object) {
    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    READ_U32(object->movement_counter);

    READ_U32(object->unknown1);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_MOVE_SPLINE_DONE_write(WowWorldWriter* writer, const vanilla_CMSG_MOVE_SPLINE_DONE* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_MOVE_SPLINE_DONE_size(object) + 4); /* size */

    WRITE_U32(0x000002c9); /* opcode */

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));

    WRITE_U32(object->movement_counter);

    WRITE_U32(object->unknown1);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_MOVE_SPLINE_DONE_free(vanilla_CMSG_MOVE_SPLINE_DONE* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_CMSG_MOVE_FALL_RESET_size(const vanilla_CMSG_MOVE_FALL_RESET* object) {
    return 0 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_CMSG_MOVE_FALL_RESET_read(WowWorldReader* reader, vanilla_CMSG_MOVE_FALL_RESET* object) {
    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_MOVE_FALL_RESET_write(WowWorldWriter* writer, const vanilla_CMSG_MOVE_FALL_RESET* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_MOVE_FALL_RESET_size(object) + 4); /* size */

    WRITE_U32(0x000002ca); /* opcode */

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_MOVE_FALL_RESET_free(vanilla_CMSG_MOVE_FALL_RESET* object) {
    vanilla_MovementInfo_free(&object->info);
}

static WowWorldResult vanilla_SMSG_INSTANCE_SAVE_CREATED_read(WowWorldReader* reader, vanilla_SMSG_INSTANCE_SAVE_CREATED* object) {
    READ_U32(object->unknown);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_INSTANCE_SAVE_CREATED_write(WowWorldWriter* writer, const vanilla_SMSG_INSTANCE_SAVE_CREATED* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x000002cb); /* opcode */

    WRITE_U32(object->unknown);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_RAID_INSTANCE_INFO_size(const vanilla_SMSG_RAID_INSTANCE_INFO* object) {
    return 4 + 12 * object->amount_of_raid_infos;
}

static WowWorldResult vanilla_SMSG_RAID_INSTANCE_INFO_read(WowWorldReader* reader, vanilla_SMSG_RAID_INSTANCE_INFO* object) {
    READ_U32(object->amount_of_raid_infos);

    READ_ARRAY_ALLOCATE(object->raid_infos, object->amount_of_raid_infos, sizeof(vanilla_RaidInfo));
    READ_ARRAY(object->raid_infos, object->amount_of_raid_infos, WWM_CHECK_RETURN_CODE(vanilla_RaidInfo_read(reader, &object->raid_infos[i])));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_RAID_INSTANCE_INFO_write(WowWorldWriter* writer, const vanilla_SMSG_RAID_INSTANCE_INFO* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_RAID_INSTANCE_INFO_size(object) + 2); /* size */

    WRITE_U16(0x000002cc); /* opcode */

    WRITE_U32(object->amount_of_raid_infos);

    WRITE_ARRAY(object->raid_infos, object->amount_of_raid_infos, WWM_CHECK_RETURN_CODE(vanilla_RaidInfo_write(writer, &object->raid_infos[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_RAID_INSTANCE_INFO_free(vanilla_SMSG_RAID_INSTANCE_INFO* object) {
    free(object->raid_infos);
    object->raid_infos = NULL;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_REQUEST_RAID_INFO_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x000002cd); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_MOVE_TIME_SKIPPED_read(WowWorldReader* reader, vanilla_CMSG_MOVE_TIME_SKIPPED* object) {
    READ_U64(object->guid);

    READ_U32(object->lag);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_MOVE_TIME_SKIPPED_write(WowWorldWriter* writer, const vanilla_CMSG_MOVE_TIME_SKIPPED* object) {
    WRITE_U16_BE(0x000c + 4); /* size */

    WRITE_U32(0x000002ce); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->lag);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_MOVE_FEATHER_FALL_ACK_size(const vanilla_CMSG_MOVE_FEATHER_FALL_ACK* object) {
    return 16 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_CMSG_MOVE_FEATHER_FALL_ACK_read(WowWorldReader* reader, vanilla_CMSG_MOVE_FEATHER_FALL_ACK* object) {
    READ_U64(object->guid);

    READ_U32(object->movement_counter);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    READ_U32(object->apply);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_MOVE_FEATHER_FALL_ACK_write(WowWorldWriter* writer, const vanilla_CMSG_MOVE_FEATHER_FALL_ACK* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_MOVE_FEATHER_FALL_ACK_size(object) + 4); /* size */

    WRITE_U32(0x000002cf); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->movement_counter);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));

    WRITE_U32(object->apply);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_MOVE_FEATHER_FALL_ACK_free(vanilla_CMSG_MOVE_FEATHER_FALL_ACK* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_CMSG_MOVE_WATER_WALK_ACK_size(const vanilla_CMSG_MOVE_WATER_WALK_ACK* object) {
    return 16 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_CMSG_MOVE_WATER_WALK_ACK_read(WowWorldReader* reader, vanilla_CMSG_MOVE_WATER_WALK_ACK* object) {
    READ_U64(object->guid);

    READ_U32(object->movement_counter);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    READ_U32(object->apply);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_MOVE_WATER_WALK_ACK_write(WowWorldWriter* writer, const vanilla_CMSG_MOVE_WATER_WALK_ACK* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_MOVE_WATER_WALK_ACK_size(object) + 4); /* size */

    WRITE_U32(0x000002d0); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->movement_counter);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));

    WRITE_U32(object->apply);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_MOVE_WATER_WALK_ACK_free(vanilla_CMSG_MOVE_WATER_WALK_ACK* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_CMSG_MOVE_NOT_ACTIVE_MOVER_size(const vanilla_CMSG_MOVE_NOT_ACTIVE_MOVER* object) {
    return 8 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_CMSG_MOVE_NOT_ACTIVE_MOVER_read(WowWorldReader* reader, vanilla_CMSG_MOVE_NOT_ACTIVE_MOVER* object) {
    READ_U64(object->old_mover);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_MOVE_NOT_ACTIVE_MOVER_write(WowWorldWriter* writer, const vanilla_CMSG_MOVE_NOT_ACTIVE_MOVER* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_MOVE_NOT_ACTIVE_MOVER_size(object) + 4); /* size */

    WRITE_U32(0x000002d1); /* opcode */

    WRITE_U64(object->old_mover);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_MOVE_NOT_ACTIVE_MOVER_free(vanilla_CMSG_MOVE_NOT_ACTIVE_MOVER* object) {
    vanilla_MovementInfo_free(&object->info);
}

static WowWorldResult vanilla_SMSG_PLAY_SOUND_read(WowWorldReader* reader, vanilla_SMSG_PLAY_SOUND* object) {
    READ_U32(object->sound_id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PLAY_SOUND_write(WowWorldWriter* writer, const vanilla_SMSG_PLAY_SOUND* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x000002d2); /* opcode */

    WRITE_U32(object->sound_id);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_BATTLEFIELD_STATUS_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x000002d3); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_BATTLEFIELD_STATUS_size(const vanilla_SMSG_BATTLEFIELD_STATUS* object) {
    size_t _size = 8;

    if (object->map == VANILLA_MAP_KALIMDOR|| object->map == VANILLA_MAP_TESTING|| object->map == VANILLA_MAP_SCOTT_TEST|| object->map == VANILLA_MAP_CASH_TEST|| object->map == VANILLA_MAP_ALTERAC_VALLEY|| object->map == VANILLA_MAP_SHADOWFANG_KEEP|| object->map == VANILLA_MAP_STORMWIND_STOCKADE|| object->map == VANILLA_MAP_STORMWIND_PRISON|| object->map == VANILLA_MAP_DEADMINES|| object->map == VANILLA_MAP_AZSHARA_CRATER|| object->map == VANILLA_MAP_COLLINS_TEST|| object->map == VANILLA_MAP_WAILING_CAVERNS|| object->map == VANILLA_MAP_MONASTERY_UNUSED|| object->map == VANILLA_MAP_RAZORFEN_KRAUL|| object->map == VANILLA_MAP_BLACKFATHOM_DEEPS|| object->map == VANILLA_MAP_ULDAMAN|| object->map == VANILLA_MAP_GNOMEREGAN|| object->map == VANILLA_MAP_SUNKEN_TEMPLE|| object->map == VANILLA_MAP_RAZORFEN_DOWNS|| object->map == VANILLA_MAP_EMERALD_DREAM|| object->map == VANILLA_MAP_SCARLET_MONASTERY|| object->map == VANILLA_MAP_ZUL_FARRAK|| object->map == VANILLA_MAP_BLACKROCK_SPIRE|| object->map == VANILLA_MAP_BLACKROCK_DEPTHS|| object->map == VANILLA_MAP_ONYXIAS_LAIR|| object->map == VANILLA_MAP_OPENING_OF_THE_DARK_PORTAL|| object->map == VANILLA_MAP_SCHOLOMANCE|| object->map == VANILLA_MAP_ZUL_GURUB|| object->map == VANILLA_MAP_STRATHOLME|| object->map == VANILLA_MAP_MARAUDON|| object->map == VANILLA_MAP_DEEPRUN_TRAM|| object->map == VANILLA_MAP_RAGEFIRE_CHASM|| object->map == VANILLA_MAP_MOLTEN_CORE|| object->map == VANILLA_MAP_DIRE_MAUL|| object->map == VANILLA_MAP_ALLIANCE_PVP_BARRACKS|| object->map == VANILLA_MAP_HORDE_PVP_BARRACKS|| object->map == VANILLA_MAP_DEVELOPMENT_LAND|| object->map == VANILLA_MAP_BLACKWING_LAIR|| object->map == VANILLA_MAP_WARSONG_GULCH|| object->map == VANILLA_MAP_RUINS_OF_AHN_QIRAJ|| object->map == VANILLA_MAP_ARATHI_BASIN|| object->map == VANILLA_MAP_AHN_QIRAJ_TEMPLE|| object->map == VANILLA_MAP_NAXXRAMAS) {
        _size += 6;

        if (object->status_id == VANILLA_STATUS_ID_WAIT_QUEUE) {
            _size += 8;
        }
        else if (object->status_id == VANILLA_STATUS_ID_WAIT_JOIN) {
            _size += 4;
        }
        else if (object->status_id == VANILLA_STATUS_ID_IN_PROGRESS) {
            _size += 8;
        }

    }

    return _size;
}

static WowWorldResult vanilla_SMSG_BATTLEFIELD_STATUS_read(WowWorldReader* reader, vanilla_SMSG_BATTLEFIELD_STATUS* object) {
    READ_U32(object->queue_slot);

    object->map = 0;
    READ_U32(object->map);

    if (object->map == VANILLA_MAP_KALIMDOR|| object->map == VANILLA_MAP_TESTING|| object->map == VANILLA_MAP_SCOTT_TEST|| object->map == VANILLA_MAP_CASH_TEST|| object->map == VANILLA_MAP_ALTERAC_VALLEY|| object->map == VANILLA_MAP_SHADOWFANG_KEEP|| object->map == VANILLA_MAP_STORMWIND_STOCKADE|| object->map == VANILLA_MAP_STORMWIND_PRISON|| object->map == VANILLA_MAP_DEADMINES|| object->map == VANILLA_MAP_AZSHARA_CRATER|| object->map == VANILLA_MAP_COLLINS_TEST|| object->map == VANILLA_MAP_WAILING_CAVERNS|| object->map == VANILLA_MAP_MONASTERY_UNUSED|| object->map == VANILLA_MAP_RAZORFEN_KRAUL|| object->map == VANILLA_MAP_BLACKFATHOM_DEEPS|| object->map == VANILLA_MAP_ULDAMAN|| object->map == VANILLA_MAP_GNOMEREGAN|| object->map == VANILLA_MAP_SUNKEN_TEMPLE|| object->map == VANILLA_MAP_RAZORFEN_DOWNS|| object->map == VANILLA_MAP_EMERALD_DREAM|| object->map == VANILLA_MAP_SCARLET_MONASTERY|| object->map == VANILLA_MAP_ZUL_FARRAK|| object->map == VANILLA_MAP_BLACKROCK_SPIRE|| object->map == VANILLA_MAP_BLACKROCK_DEPTHS|| object->map == VANILLA_MAP_ONYXIAS_LAIR|| object->map == VANILLA_MAP_OPENING_OF_THE_DARK_PORTAL|| object->map == VANILLA_MAP_SCHOLOMANCE|| object->map == VANILLA_MAP_ZUL_GURUB|| object->map == VANILLA_MAP_STRATHOLME|| object->map == VANILLA_MAP_MARAUDON|| object->map == VANILLA_MAP_DEEPRUN_TRAM|| object->map == VANILLA_MAP_RAGEFIRE_CHASM|| object->map == VANILLA_MAP_MOLTEN_CORE|| object->map == VANILLA_MAP_DIRE_MAUL|| object->map == VANILLA_MAP_ALLIANCE_PVP_BARRACKS|| object->map == VANILLA_MAP_HORDE_PVP_BARRACKS|| object->map == VANILLA_MAP_DEVELOPMENT_LAND|| object->map == VANILLA_MAP_BLACKWING_LAIR|| object->map == VANILLA_MAP_WARSONG_GULCH|| object->map == VANILLA_MAP_RUINS_OF_AHN_QIRAJ|| object->map == VANILLA_MAP_ARATHI_BASIN|| object->map == VANILLA_MAP_AHN_QIRAJ_TEMPLE|| object->map == VANILLA_MAP_NAXXRAMAS) {
        object->bracket = 0;
        READ_U8(object->bracket);

        READ_U32(object->client_instance_id);

        object->status_id = 0;
        READ_U8(object->status_id);

        if (object->status_id == VANILLA_STATUS_ID_WAIT_QUEUE) {
            READ_U32(object->average_wait_time_in_ms);

            READ_U32(object->time_in_queue_in_ms);

        }
        else if (object->status_id == VANILLA_STATUS_ID_WAIT_JOIN) {
            READ_U32(object->time_to_remove_in_queue_in_ms);

        }
        else if (object->status_id == VANILLA_STATUS_ID_IN_PROGRESS) {
            READ_U32(object->time_to_bg_autoleave_in_ms);

            READ_U32(object->time_to_bg_start_in_ms);

        }
    }
    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_BATTLEFIELD_STATUS_write(WowWorldWriter* writer, const vanilla_SMSG_BATTLEFIELD_STATUS* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_BATTLEFIELD_STATUS_size(object) + 2); /* size */

    WRITE_U16(0x000002d4); /* opcode */

    WRITE_U32(object->queue_slot);

    WRITE_U32(object->map);

    if (object->map == VANILLA_MAP_KALIMDOR|| object->map == VANILLA_MAP_TESTING|| object->map == VANILLA_MAP_SCOTT_TEST|| object->map == VANILLA_MAP_CASH_TEST|| object->map == VANILLA_MAP_ALTERAC_VALLEY|| object->map == VANILLA_MAP_SHADOWFANG_KEEP|| object->map == VANILLA_MAP_STORMWIND_STOCKADE|| object->map == VANILLA_MAP_STORMWIND_PRISON|| object->map == VANILLA_MAP_DEADMINES|| object->map == VANILLA_MAP_AZSHARA_CRATER|| object->map == VANILLA_MAP_COLLINS_TEST|| object->map == VANILLA_MAP_WAILING_CAVERNS|| object->map == VANILLA_MAP_MONASTERY_UNUSED|| object->map == VANILLA_MAP_RAZORFEN_KRAUL|| object->map == VANILLA_MAP_BLACKFATHOM_DEEPS|| object->map == VANILLA_MAP_ULDAMAN|| object->map == VANILLA_MAP_GNOMEREGAN|| object->map == VANILLA_MAP_SUNKEN_TEMPLE|| object->map == VANILLA_MAP_RAZORFEN_DOWNS|| object->map == VANILLA_MAP_EMERALD_DREAM|| object->map == VANILLA_MAP_SCARLET_MONASTERY|| object->map == VANILLA_MAP_ZUL_FARRAK|| object->map == VANILLA_MAP_BLACKROCK_SPIRE|| object->map == VANILLA_MAP_BLACKROCK_DEPTHS|| object->map == VANILLA_MAP_ONYXIAS_LAIR|| object->map == VANILLA_MAP_OPENING_OF_THE_DARK_PORTAL|| object->map == VANILLA_MAP_SCHOLOMANCE|| object->map == VANILLA_MAP_ZUL_GURUB|| object->map == VANILLA_MAP_STRATHOLME|| object->map == VANILLA_MAP_MARAUDON|| object->map == VANILLA_MAP_DEEPRUN_TRAM|| object->map == VANILLA_MAP_RAGEFIRE_CHASM|| object->map == VANILLA_MAP_MOLTEN_CORE|| object->map == VANILLA_MAP_DIRE_MAUL|| object->map == VANILLA_MAP_ALLIANCE_PVP_BARRACKS|| object->map == VANILLA_MAP_HORDE_PVP_BARRACKS|| object->map == VANILLA_MAP_DEVELOPMENT_LAND|| object->map == VANILLA_MAP_BLACKWING_LAIR|| object->map == VANILLA_MAP_WARSONG_GULCH|| object->map == VANILLA_MAP_RUINS_OF_AHN_QIRAJ|| object->map == VANILLA_MAP_ARATHI_BASIN|| object->map == VANILLA_MAP_AHN_QIRAJ_TEMPLE|| object->map == VANILLA_MAP_NAXXRAMAS) {
        WRITE_U8(object->bracket);

        WRITE_U32(object->client_instance_id);

        WRITE_U8(object->status_id);

        if (object->status_id == VANILLA_STATUS_ID_WAIT_QUEUE) {
            WRITE_U32(object->average_wait_time_in_ms);

            WRITE_U32(object->time_in_queue_in_ms);

        }
        else if (object->status_id == VANILLA_STATUS_ID_WAIT_JOIN) {
            WRITE_U32(object->time_to_remove_in_queue_in_ms);

        }
        else if (object->status_id == VANILLA_STATUS_ID_IN_PROGRESS) {
            WRITE_U32(object->time_to_bg_autoleave_in_ms);

            WRITE_U32(object->time_to_bg_start_in_ms);

        }
    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_BATTLEFIELD_STATUS_free(vanilla_SMSG_BATTLEFIELD_STATUS* object) {
    if (object->map == VANILLA_MAP_KALIMDOR|| object->map == VANILLA_MAP_TESTING|| object->map == VANILLA_MAP_SCOTT_TEST|| object->map == VANILLA_MAP_CASH_TEST|| object->map == VANILLA_MAP_ALTERAC_VALLEY|| object->map == VANILLA_MAP_SHADOWFANG_KEEP|| object->map == VANILLA_MAP_STORMWIND_STOCKADE|| object->map == VANILLA_MAP_STORMWIND_PRISON|| object->map == VANILLA_MAP_DEADMINES|| object->map == VANILLA_MAP_AZSHARA_CRATER|| object->map == VANILLA_MAP_COLLINS_TEST|| object->map == VANILLA_MAP_WAILING_CAVERNS|| object->map == VANILLA_MAP_MONASTERY_UNUSED|| object->map == VANILLA_MAP_RAZORFEN_KRAUL|| object->map == VANILLA_MAP_BLACKFATHOM_DEEPS|| object->map == VANILLA_MAP_ULDAMAN|| object->map == VANILLA_MAP_GNOMEREGAN|| object->map == VANILLA_MAP_SUNKEN_TEMPLE|| object->map == VANILLA_MAP_RAZORFEN_DOWNS|| object->map == VANILLA_MAP_EMERALD_DREAM|| object->map == VANILLA_MAP_SCARLET_MONASTERY|| object->map == VANILLA_MAP_ZUL_FARRAK|| object->map == VANILLA_MAP_BLACKROCK_SPIRE|| object->map == VANILLA_MAP_BLACKROCK_DEPTHS|| object->map == VANILLA_MAP_ONYXIAS_LAIR|| object->map == VANILLA_MAP_OPENING_OF_THE_DARK_PORTAL|| object->map == VANILLA_MAP_SCHOLOMANCE|| object->map == VANILLA_MAP_ZUL_GURUB|| object->map == VANILLA_MAP_STRATHOLME|| object->map == VANILLA_MAP_MARAUDON|| object->map == VANILLA_MAP_DEEPRUN_TRAM|| object->map == VANILLA_MAP_RAGEFIRE_CHASM|| object->map == VANILLA_MAP_MOLTEN_CORE|| object->map == VANILLA_MAP_DIRE_MAUL|| object->map == VANILLA_MAP_ALLIANCE_PVP_BARRACKS|| object->map == VANILLA_MAP_HORDE_PVP_BARRACKS|| object->map == VANILLA_MAP_DEVELOPMENT_LAND|| object->map == VANILLA_MAP_BLACKWING_LAIR|| object->map == VANILLA_MAP_WARSONG_GULCH|| object->map == VANILLA_MAP_RUINS_OF_AHN_QIRAJ|| object->map == VANILLA_MAP_ARATHI_BASIN|| object->map == VANILLA_MAP_AHN_QIRAJ_TEMPLE|| object->map == VANILLA_MAP_NAXXRAMAS) {
        if (object->status_id == VANILLA_STATUS_ID_WAIT_QUEUE) {
        }
        else if (object->status_id == VANILLA_STATUS_ID_WAIT_JOIN) {
        }
        else if (object->status_id == VANILLA_STATUS_ID_IN_PROGRESS) {
        }
    }
}

static WowWorldResult vanilla_CMSG_BATTLEFIELD_PORT_read(WowWorldReader* reader, vanilla_CMSG_BATTLEFIELD_PORT* object) {
    object->map = 0;
    READ_U32(object->map);

    object->action = 0;
    READ_U8(object->action);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_BATTLEFIELD_PORT_write(WowWorldWriter* writer, const vanilla_CMSG_BATTLEFIELD_PORT* object) {
    WRITE_U16_BE(0x0005 + 4); /* size */

    WRITE_U32(0x000002d5); /* opcode */

    WRITE_U32(object->map);

    WRITE_U8(object->action);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_MSG_INSPECT_HONOR_STATS_Client_read(WowWorldReader* reader, vanilla_MSG_INSPECT_HONOR_STATS_Client* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_INSPECT_HONOR_STATS_Client_write(WowWorldWriter* writer, const vanilla_MSG_INSPECT_HONOR_STATS_Client* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x000002d6); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_MSG_INSPECT_HONOR_STATS_Server_read(WowWorldReader* reader, vanilla_MSG_INSPECT_HONOR_STATS_Server* object) {
    READ_U64(object->guid);

    object->highest_rank = 0;
    READ_U8(object->highest_rank);

    READ_U32(object->today_honorable_and_dishonorable);

    READ_U16(object->yesterday_honorable);

    READ_U16(object->unknown1);

    READ_U16(object->last_week_honorable);

    READ_U16(object->unknown2);

    READ_U16(object->this_week_honorable);

    READ_U16(object->unknown3);

    READ_U32(object->lifetime_honorable);

    READ_U32(object->lifetime_dishonorable);

    READ_U32(object->yesterday_honor);

    READ_U32(object->last_week_honor);

    READ_U32(object->this_week_honor);

    object->last_week_standing = 0;
    READ_U32(object->last_week_standing);

    READ_U8(object->rank_progress_bar);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_INSPECT_HONOR_STATS_Server_write(WowWorldWriter* writer, const vanilla_MSG_INSPECT_HONOR_STATS_Server* object) {
    WRITE_U16_BE(0x0032 + 2); /* size */

    WRITE_U16(0x000002d6); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U8(object->highest_rank);

    WRITE_U32(object->today_honorable_and_dishonorable);

    WRITE_U16(object->yesterday_honorable);

    WRITE_U16(object->unknown1);

    WRITE_U16(object->last_week_honorable);

    WRITE_U16(object->unknown2);

    WRITE_U16(object->this_week_honorable);

    WRITE_U16(object->unknown3);

    WRITE_U32(object->lifetime_honorable);

    WRITE_U32(object->lifetime_dishonorable);

    WRITE_U32(object->yesterday_honor);

    WRITE_U32(object->last_week_honor);

    WRITE_U32(object->this_week_honor);

    WRITE_U32(object->last_week_standing);

    WRITE_U8(object->rank_progress_bar);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_BATTLEMASTER_HELLO_read(WowWorldReader* reader, vanilla_CMSG_BATTLEMASTER_HELLO* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_BATTLEMASTER_HELLO_write(WowWorldWriter* writer, const vanilla_CMSG_BATTLEMASTER_HELLO* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x000002d7); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_FORCE_WALK_SPEED_CHANGE_size(const vanilla_SMSG_FORCE_WALK_SPEED_CHANGE* object) {
    return 8 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_FORCE_WALK_SPEED_CHANGE_read(WowWorldReader* reader, vanilla_SMSG_FORCE_WALK_SPEED_CHANGE* object) {
    READ_PACKED_GUID(object->guid);

    READ_U32(object->move_event);

    READ_FLOAT(object->speed);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_FORCE_WALK_SPEED_CHANGE_write(WowWorldWriter* writer, const vanilla_SMSG_FORCE_WALK_SPEED_CHANGE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_FORCE_WALK_SPEED_CHANGE_size(object) + 2); /* size */

    WRITE_U16(0x000002da); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WRITE_U32(object->move_event);

    WRITE_FLOAT(object->speed);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_FORCE_WALK_SPEED_CHANGE_ACK_size(const vanilla_CMSG_FORCE_WALK_SPEED_CHANGE_ACK* object) {
    return 16 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_CMSG_FORCE_WALK_SPEED_CHANGE_ACK_read(WowWorldReader* reader, vanilla_CMSG_FORCE_WALK_SPEED_CHANGE_ACK* object) {
    READ_U64(object->guid);

    READ_U32(object->counter);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    READ_FLOAT(object->new_speed);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_FORCE_WALK_SPEED_CHANGE_ACK_write(WowWorldWriter* writer, const vanilla_CMSG_FORCE_WALK_SPEED_CHANGE_ACK* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_FORCE_WALK_SPEED_CHANGE_ACK_size(object) + 4); /* size */

    WRITE_U32(0x000002db); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->counter);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));

    WRITE_FLOAT(object->new_speed);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_FORCE_WALK_SPEED_CHANGE_ACK_free(vanilla_CMSG_FORCE_WALK_SPEED_CHANGE_ACK* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_SMSG_FORCE_SWIM_BACK_SPEED_CHANGE_size(const vanilla_SMSG_FORCE_SWIM_BACK_SPEED_CHANGE* object) {
    return 8 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_FORCE_SWIM_BACK_SPEED_CHANGE_read(WowWorldReader* reader, vanilla_SMSG_FORCE_SWIM_BACK_SPEED_CHANGE* object) {
    READ_PACKED_GUID(object->guid);

    READ_U32(object->move_event);

    READ_FLOAT(object->speed);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_FORCE_SWIM_BACK_SPEED_CHANGE_write(WowWorldWriter* writer, const vanilla_SMSG_FORCE_SWIM_BACK_SPEED_CHANGE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_FORCE_SWIM_BACK_SPEED_CHANGE_size(object) + 2); /* size */

    WRITE_U16(0x000002dc); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WRITE_U32(object->move_event);

    WRITE_FLOAT(object->speed);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK_size(const vanilla_CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK* object) {
    return 16 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK_read(WowWorldReader* reader, vanilla_CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK* object) {
    READ_U64(object->guid);

    READ_U32(object->counter);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    READ_FLOAT(object->new_speed);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK_write(WowWorldWriter* writer, const vanilla_CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK_size(object) + 4); /* size */

    WRITE_U32(0x000002dd); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->counter);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));

    WRITE_FLOAT(object->new_speed);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK_free(vanilla_CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK* object) {
    vanilla_MovementInfo_free(&object->info);
}

static size_t vanilla_SMSG_FORCE_TURN_RATE_CHANGE_size(const vanilla_SMSG_FORCE_TURN_RATE_CHANGE* object) {
    return 8 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_FORCE_TURN_RATE_CHANGE_read(WowWorldReader* reader, vanilla_SMSG_FORCE_TURN_RATE_CHANGE* object) {
    READ_PACKED_GUID(object->guid);

    READ_U32(object->move_event);

    READ_FLOAT(object->speed);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_FORCE_TURN_RATE_CHANGE_write(WowWorldWriter* writer, const vanilla_SMSG_FORCE_TURN_RATE_CHANGE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_FORCE_TURN_RATE_CHANGE_size(object) + 2); /* size */

    WRITE_U16(0x000002de); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WRITE_U32(object->move_event);

    WRITE_FLOAT(object->speed);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_FORCE_TURN_RATE_CHANGE_ACK_size(const vanilla_CMSG_FORCE_TURN_RATE_CHANGE_ACK* object) {
    return 16 + vanilla_MovementInfo_size(&object->info);
}

static WowWorldResult vanilla_CMSG_FORCE_TURN_RATE_CHANGE_ACK_read(WowWorldReader* reader, vanilla_CMSG_FORCE_TURN_RATE_CHANGE_ACK* object) {
    READ_U64(object->guid);

    READ_U32(object->counter);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_read(reader, &object->info));

    READ_FLOAT(object->new_speed);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_FORCE_TURN_RATE_CHANGE_ACK_write(WowWorldWriter* writer, const vanilla_CMSG_FORCE_TURN_RATE_CHANGE_ACK* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_FORCE_TURN_RATE_CHANGE_ACK_size(object) + 4); /* size */

    WRITE_U32(0x000002df); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->counter);

    WWM_CHECK_RETURN_CODE(vanilla_MovementInfo_write(writer, &object->info));

    WRITE_FLOAT(object->new_speed);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_FORCE_TURN_RATE_CHANGE_ACK_free(vanilla_CMSG_FORCE_TURN_RATE_CHANGE_ACK* object) {
    vanilla_MovementInfo_free(&object->info);
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_PVP_LOG_DATA_Client_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x000002e0); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_MSG_PVP_LOG_DATA_Server_size(const vanilla_MSG_PVP_LOG_DATA_Server* object) {
    size_t _size = 5;

    if (object->status == VANILLA_BATTLEGROUND_END_STATUS_ENDED) {
        _size += 1;
    }

    /* C89 scope to allow variable declarations */ {
        int i;
        for(i = 0; i < (int)object->amount_of_players; ++i) {
            _size += vanilla_BattlegroundPlayer_size(&object->players[i]);
        }
    }

    return _size;
}

static WowWorldResult vanilla_MSG_PVP_LOG_DATA_Server_read(WowWorldReader* reader, vanilla_MSG_PVP_LOG_DATA_Server* object) {
    object->status = 0;
    READ_U8(object->status);

    if (object->status == VANILLA_BATTLEGROUND_END_STATUS_ENDED) {
        object->winner = 0;
        READ_U8(object->winner);

    }
    READ_U32(object->amount_of_players);

    READ_ARRAY_ALLOCATE(object->players, object->amount_of_players, sizeof(vanilla_BattlegroundPlayer));
    READ_ARRAY(object->players, object->amount_of_players, WWM_CHECK_RETURN_CODE(vanilla_BattlegroundPlayer_read(reader, &object->players[i])));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_PVP_LOG_DATA_Server_write(WowWorldWriter* writer, const vanilla_MSG_PVP_LOG_DATA_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_PVP_LOG_DATA_Server_size(object) + 2); /* size */

    WRITE_U16(0x000002e0); /* opcode */

    WRITE_U8(object->status);

    if (object->status == VANILLA_BATTLEGROUND_END_STATUS_ENDED) {
        WRITE_U8(object->winner);

    }
    WRITE_U32(object->amount_of_players);

    WRITE_ARRAY(object->players, object->amount_of_players, WWM_CHECK_RETURN_CODE(vanilla_BattlegroundPlayer_write(writer, &object->players[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_PVP_LOG_DATA_Server_free(vanilla_MSG_PVP_LOG_DATA_Server* object) {
    size_t i;

    if (object->status == VANILLA_BATTLEGROUND_END_STATUS_ENDED) {
    }
    for (i = 0; i < object->amount_of_players; ++i) {
        vanilla_BattlegroundPlayer_free(&((object->players)[i]));
    }
    free(object->players);
    object->players = NULL;
}

static WowWorldResult vanilla_CMSG_LEAVE_BATTLEFIELD_read(WowWorldReader* reader, vanilla_CMSG_LEAVE_BATTLEFIELD* object) {
    object->map = 0;
    READ_U32(object->map);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_LEAVE_BATTLEFIELD_write(WowWorldWriter* writer, const vanilla_CMSG_LEAVE_BATTLEFIELD* object) {
    WRITE_U16_BE(0x0004 + 4); /* size */

    WRITE_U32(0x000002e1); /* opcode */

    WRITE_U32(object->map);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_AREA_SPIRIT_HEALER_QUERY_read(WowWorldReader* reader, vanilla_CMSG_AREA_SPIRIT_HEALER_QUERY* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_AREA_SPIRIT_HEALER_QUERY_write(WowWorldWriter* writer, const vanilla_CMSG_AREA_SPIRIT_HEALER_QUERY* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x000002e2); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_AREA_SPIRIT_HEALER_QUEUE_read(WowWorldReader* reader, vanilla_CMSG_AREA_SPIRIT_HEALER_QUEUE* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_AREA_SPIRIT_HEALER_QUEUE_write(WowWorldWriter* writer, const vanilla_CMSG_AREA_SPIRIT_HEALER_QUEUE* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x000002e3); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_AREA_SPIRIT_HEALER_TIME_read(WowWorldReader* reader, vanilla_SMSG_AREA_SPIRIT_HEALER_TIME* object) {
    READ_U64(object->guid);

    READ_U32(object->next_resurrect_time);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_AREA_SPIRIT_HEALER_TIME_write(WowWorldWriter* writer, const vanilla_SMSG_AREA_SPIRIT_HEALER_TIME* object) {
    WRITE_U16_BE(0x000c + 2); /* size */

    WRITE_U16(0x000002e4); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->next_resurrect_time);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_WARDEN_DATA_size(const vanilla_SMSG_WARDEN_DATA* object) {
    return 0 + 1 * object->amount_of_encrypted_data;
}

static WowWorldResult vanilla_SMSG_WARDEN_DATA_read(WowWorldReader* reader, vanilla_SMSG_WARDEN_DATA* object, size_t body_size) {
    size_t _size = 0;

    object->amount_of_encrypted_data = 0;
    /* C89 scope to allow variable declarations */ {
        int i = 0;
        size_t _current_size = 8 * sizeof(*object->encrypted_data);

        object->encrypted_data = malloc(_current_size);
        while (_size < body_size) {
            READ_U8(object->encrypted_data[i]);
            _size += 1;
            ++i;

            if (i * sizeof(*object->encrypted_data) >= _current_size) {
                _current_size *= 2;
                object->encrypted_data = realloc(object->encrypted_data, _current_size);
            }
        }

        object->amount_of_encrypted_data = i;
    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_WARDEN_DATA_write(WowWorldWriter* writer, const vanilla_SMSG_WARDEN_DATA* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_WARDEN_DATA_size(object) + 2); /* size */

    WRITE_U16(0x000002e6); /* opcode */

    WRITE_ARRAY(object->encrypted_data, object->amount_of_encrypted_data, WRITE_U8(object->encrypted_data[i]));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_WARDEN_DATA_free(vanilla_SMSG_WARDEN_DATA* object) {
    free(object->encrypted_data);
    object->encrypted_data = NULL;
}

static size_t vanilla_CMSG_WARDEN_DATA_size(const vanilla_CMSG_WARDEN_DATA* object) {
    return 0 + 1 * object->amount_of_encrypted_data;
}

static WowWorldResult vanilla_CMSG_WARDEN_DATA_read(WowWorldReader* reader, vanilla_CMSG_WARDEN_DATA* object, size_t body_size) {
    size_t _size = 0;

    object->amount_of_encrypted_data = 0;
    /* C89 scope to allow variable declarations */ {
        int i = 0;
        size_t _current_size = 8 * sizeof(*object->encrypted_data);

        object->encrypted_data = malloc(_current_size);
        while (_size < body_size) {
            READ_U8(object->encrypted_data[i]);
            _size += 1;
            ++i;

            if (i * sizeof(*object->encrypted_data) >= _current_size) {
                _current_size *= 2;
                object->encrypted_data = realloc(object->encrypted_data, _current_size);
            }
        }

        object->amount_of_encrypted_data = i;
    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_WARDEN_DATA_write(WowWorldWriter* writer, const vanilla_CMSG_WARDEN_DATA* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_WARDEN_DATA_size(object) + 4); /* size */

    WRITE_U32(0x000002e7); /* opcode */

    WRITE_ARRAY(object->encrypted_data, object->amount_of_encrypted_data, WRITE_U8(object->encrypted_data[i]));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_WARDEN_DATA_free(vanilla_CMSG_WARDEN_DATA* object) {
    free(object->encrypted_data);
    object->encrypted_data = NULL;
}

static WowWorldResult vanilla_SMSG_GROUP_JOINED_BATTLEGROUND_read(WowWorldReader* reader, vanilla_SMSG_GROUP_JOINED_BATTLEGROUND* object) {
    object->id = 0;
    READ_U32(object->id);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_GROUP_JOINED_BATTLEGROUND_write(WowWorldWriter* writer, const vanilla_SMSG_GROUP_JOINED_BATTLEGROUND* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x000002e8); /* opcode */

    WRITE_U32(object->id);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_BATTLEGROUND_PLAYER_POSITIONS_Client_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x000002e9); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_MSG_BATTLEGROUND_PLAYER_POSITIONS_Server_size(const vanilla_MSG_BATTLEGROUND_PLAYER_POSITIONS_Server* object) {
    return 5 + 16 * object->amount_of_teammates + 16 * object->amount_of_carriers;
}

static WowWorldResult vanilla_MSG_BATTLEGROUND_PLAYER_POSITIONS_Server_read(WowWorldReader* reader, vanilla_MSG_BATTLEGROUND_PLAYER_POSITIONS_Server* object) {
    READ_U32(object->amount_of_teammates);

    READ_ARRAY_ALLOCATE(object->teammates, object->amount_of_teammates, sizeof(vanilla_BattlegroundPlayerPosition));
    READ_ARRAY(object->teammates, object->amount_of_teammates, WWM_CHECK_RETURN_CODE(vanilla_BattlegroundPlayerPosition_read(reader, &object->teammates[i])));

    READ_U8(object->amount_of_carriers);

    READ_ARRAY_ALLOCATE(object->carriers, object->amount_of_carriers, sizeof(vanilla_BattlegroundPlayerPosition));
    READ_ARRAY(object->carriers, object->amount_of_carriers, WWM_CHECK_RETURN_CODE(vanilla_BattlegroundPlayerPosition_read(reader, &object->carriers[i])));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_BATTLEGROUND_PLAYER_POSITIONS_Server_write(WowWorldWriter* writer, const vanilla_MSG_BATTLEGROUND_PLAYER_POSITIONS_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_BATTLEGROUND_PLAYER_POSITIONS_Server_size(object) + 2); /* size */

    WRITE_U16(0x000002e9); /* opcode */

    WRITE_U32(object->amount_of_teammates);

    WRITE_ARRAY(object->teammates, object->amount_of_teammates, WWM_CHECK_RETURN_CODE(vanilla_BattlegroundPlayerPosition_write(writer, &object->teammates[i])));

    WRITE_U8(object->amount_of_carriers);

    WRITE_ARRAY(object->carriers, object->amount_of_carriers, WWM_CHECK_RETURN_CODE(vanilla_BattlegroundPlayerPosition_write(writer, &object->carriers[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_BATTLEGROUND_PLAYER_POSITIONS_Server_free(vanilla_MSG_BATTLEGROUND_PLAYER_POSITIONS_Server* object) {
    free(object->teammates);
    object->teammates = NULL;
    free(object->carriers);
    object->carriers = NULL;
}

static WowWorldResult vanilla_CMSG_PET_STOP_ATTACK_read(WowWorldReader* reader, vanilla_CMSG_PET_STOP_ATTACK* object) {
    READ_U64(object->pet);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_PET_STOP_ATTACK_write(WowWorldWriter* writer, const vanilla_CMSG_PET_STOP_ATTACK* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x000002ea); /* opcode */

    WRITE_U64(object->pet);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_BINDER_CONFIRM_read(WowWorldReader* reader, vanilla_SMSG_BINDER_CONFIRM* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_BINDER_CONFIRM_write(WowWorldWriter* writer, const vanilla_SMSG_BINDER_CONFIRM* object) {
    WRITE_U16_BE(0x0008 + 2); /* size */

    WRITE_U16(0x000002eb); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_BATTLEGROUND_PLAYER_JOINED_read(WowWorldReader* reader, vanilla_SMSG_BATTLEGROUND_PLAYER_JOINED* object) {
    READ_U64(object->player);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_BATTLEGROUND_PLAYER_JOINED_write(WowWorldWriter* writer, const vanilla_SMSG_BATTLEGROUND_PLAYER_JOINED* object) {
    WRITE_U16_BE(0x0008 + 2); /* size */

    WRITE_U16(0x000002ec); /* opcode */

    WRITE_U64(object->player);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_BATTLEGROUND_PLAYER_LEFT_read(WowWorldReader* reader, vanilla_SMSG_BATTLEGROUND_PLAYER_LEFT* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_BATTLEGROUND_PLAYER_LEFT_write(WowWorldWriter* writer, const vanilla_SMSG_BATTLEGROUND_PLAYER_LEFT* object) {
    WRITE_U16_BE(0x0008 + 2); /* size */

    WRITE_U16(0x000002ed); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_BATTLEMASTER_JOIN_read(WowWorldReader* reader, vanilla_CMSG_BATTLEMASTER_JOIN* object) {
    READ_U64(object->guid);

    object->map = 0;
    READ_U32(object->map);

    READ_U32(object->instance_id);

    READ_BOOL8(object->join_as_group);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_BATTLEMASTER_JOIN_write(WowWorldWriter* writer, const vanilla_CMSG_BATTLEMASTER_JOIN* object) {
    WRITE_U16_BE(0x0011 + 4); /* size */

    WRITE_U32(0x000002ee); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->map);

    WRITE_U32(object->instance_id);

    WRITE_BOOL8(object->join_as_group);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_ADDON_INFO_size(const vanilla_SMSG_ADDON_INFO* object) {
    size_t _size = 0;

    /* C89 scope to allow variable declarations */ {
        int i;
        for(i = 0; i < (int)object->amount_of_addons; ++i) {
            _size += vanilla_Addon_size(&object->addons[i]);
        }
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_ADDON_INFO_read(WowWorldReader* reader, vanilla_SMSG_ADDON_INFO* object, size_t body_size) {
    size_t _size = 0;

    object->amount_of_addons = 0;
    /* C89 scope to allow variable declarations */ {
        int i = 0;
        size_t _current_size = 8 * sizeof(*object->addons);

        object->addons = malloc(_current_size);
        while (_size < body_size) {
            WWM_CHECK_RETURN_CODE(vanilla_Addon_read(reader, &object->addons[i]));
            _size += vanilla_Addon_size(&object->addons[i]);
            ++i;

            if (i * sizeof(*object->addons) >= _current_size) {
                _current_size *= 2;
                object->addons = realloc(object->addons, _current_size);
            }
        }

        object->amount_of_addons = i;
    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_ADDON_INFO_write(WowWorldWriter* writer, const vanilla_SMSG_ADDON_INFO* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_ADDON_INFO_size(object) + 2); /* size */

    WRITE_U16(0x000002ef); /* opcode */

    WRITE_ARRAY(object->addons, object->amount_of_addons, WWM_CHECK_RETURN_CODE(vanilla_Addon_write(writer, &object->addons[i])));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_ADDON_INFO_free(vanilla_SMSG_ADDON_INFO* object) {
    size_t i;

    for (i = 0; i < object->amount_of_addons; ++i) {
        vanilla_Addon_free(&((object->addons)[i]));
    }
    free(object->addons);
    object->addons = NULL;
}

static WowWorldResult vanilla_CMSG_PET_UNLEARN_read(WowWorldReader* reader, vanilla_CMSG_PET_UNLEARN* object) {
    READ_U64(object->pet);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_PET_UNLEARN_write(WowWorldWriter* writer, const vanilla_CMSG_PET_UNLEARN* object) {
    WRITE_U16_BE(0x0008 + 4); /* size */

    WRITE_U32(0x000002f0); /* opcode */

    WRITE_U64(object->pet);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_PET_UNLEARN_CONFIRM_read(WowWorldReader* reader, vanilla_SMSG_PET_UNLEARN_CONFIRM* object) {
    READ_U64(object->pet);

    READ_U32(object->talent_reset_cost);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PET_UNLEARN_CONFIRM_write(WowWorldWriter* writer, const vanilla_SMSG_PET_UNLEARN_CONFIRM* object) {
    WRITE_U16_BE(0x000c + 2); /* size */

    WRITE_U16(0x000002f1); /* opcode */

    WRITE_U64(object->pet);

    WRITE_U32(object->talent_reset_cost);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_PARTY_MEMBER_STATS_FULL_size(const vanilla_SMSG_PARTY_MEMBER_STATS_FULL* object) {
    size_t _size = 4 + wwm_packed_guid_size(object->player);

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_STATUS) != 0) {
        _size += 1;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_CUR_HP) != 0) {
        _size += 2;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_MAX_HP) != 0) {
        _size += 2;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_POWER_TYPE) != 0) {
        _size += 1;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_CUR_POWER) != 0) {
        _size += 2;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_MAX_POWER) != 0) {
        _size += 2;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_LEVEL) != 0) {
        _size += 2;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_ZONE) != 0) {
        _size += 4;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_POSITION) != 0) {
        _size += 4;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_AURAS) != 0) {
        _size += 0 + vanilla_aura_mask_size(&object->auras);
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_GUID) != 0) {
        _size += 8;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_NAME) != 0) {
        _size += 1 + STRING_SIZE(object->pet_name);
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_MODEL_ID) != 0) {
        _size += 2;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_CUR_HP) != 0) {
        _size += 2;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_MAX_HP) != 0) {
        _size += 2;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_POWER_TYPE) != 0) {
        _size += 1;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_CUR_POWER) != 0) {
        _size += 2;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_MAX_POWER) != 0) {
        _size += 2;
    }

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_AURAS) != 0) {
        _size += 0 + vanilla_aura_mask_size(&object->pet_auras);
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_PARTY_MEMBER_STATS_FULL_read(WowWorldReader* reader, vanilla_SMSG_PARTY_MEMBER_STATS_FULL* object) {
    READ_PACKED_GUID(object->player);

    object->mask = 0;
    READ_U32(object->mask);

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_STATUS) != 0) {
        object->status = 0;
        READ_U8(object->status);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_CUR_HP) != 0) {
        READ_U16(object->current_health);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_MAX_HP) != 0) {
        READ_U16(object->max_health);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_POWER_TYPE) != 0) {
        object->power = 0;
        READ_U8(object->power);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_CUR_POWER) != 0) {
        READ_U16(object->current_power);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_MAX_POWER) != 0) {
        READ_U16(object->max_power);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_LEVEL) != 0) {
        READ_U16(object->level);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_ZONE) != 0) {
        object->area = 0;
        READ_U32(object->area);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_POSITION) != 0) {
        READ_U16(object->position_x);

        READ_U16(object->position_y);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_AURAS) != 0) {
        WWM_CHECK_RETURN_CODE(vanilla_aura_mask_read(reader, &object->auras));

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_GUID) != 0) {
        READ_U64(object->pet);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_NAME) != 0) {
        READ_CSTRING(object->pet_name);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_MODEL_ID) != 0) {
        READ_U16(object->pet_display_id);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_CUR_HP) != 0) {
        READ_U16(object->pet_current_health);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_MAX_HP) != 0) {
        READ_U16(object->pet_max_health);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_POWER_TYPE) != 0) {
        object->pet_power_type = 0;
        READ_U8(object->pet_power_type);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_CUR_POWER) != 0) {
        READ_U16(object->pet_current_power);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_MAX_POWER) != 0) {
        READ_U16(object->pet_max_power);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_AURAS) != 0) {
        WWM_CHECK_RETURN_CODE(vanilla_aura_mask_read(reader, &object->pet_auras));

    }
    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PARTY_MEMBER_STATS_FULL_write(WowWorldWriter* writer, const vanilla_SMSG_PARTY_MEMBER_STATS_FULL* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_PARTY_MEMBER_STATS_FULL_size(object) + 2); /* size */

    WRITE_U16(0x000002f2); /* opcode */

    WRITE_PACKED_GUID(object->player);

    WRITE_U32(object->mask);

    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_STATUS) != 0) {
        WRITE_U8(object->status);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_CUR_HP) != 0) {
        WRITE_U16(object->current_health);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_MAX_HP) != 0) {
        WRITE_U16(object->max_health);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_POWER_TYPE) != 0) {
        WRITE_U8(object->power);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_CUR_POWER) != 0) {
        WRITE_U16(object->current_power);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_MAX_POWER) != 0) {
        WRITE_U16(object->max_power);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_LEVEL) != 0) {
        WRITE_U16(object->level);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_ZONE) != 0) {
        WRITE_U32(object->area);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_POSITION) != 0) {
        WRITE_U16(object->position_x);

        WRITE_U16(object->position_y);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_AURAS) != 0) {
        WWM_CHECK_RETURN_CODE(vanilla_aura_mask_write(writer, &object->auras));

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_GUID) != 0) {
        WRITE_U64(object->pet);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_NAME) != 0) {
        WRITE_CSTRING(object->pet_name);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_MODEL_ID) != 0) {
        WRITE_U16(object->pet_display_id);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_CUR_HP) != 0) {
        WRITE_U16(object->pet_current_health);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_MAX_HP) != 0) {
        WRITE_U16(object->pet_max_health);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_POWER_TYPE) != 0) {
        WRITE_U8(object->pet_power_type);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_CUR_POWER) != 0) {
        WRITE_U16(object->pet_current_power);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_MAX_POWER) != 0) {
        WRITE_U16(object->pet_max_power);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_AURAS) != 0) {
        WWM_CHECK_RETURN_CODE(vanilla_aura_mask_write(writer, &object->pet_auras));

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_PARTY_MEMBER_STATS_FULL_free(vanilla_SMSG_PARTY_MEMBER_STATS_FULL* object) {
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_STATUS) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_CUR_HP) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_MAX_HP) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_POWER_TYPE) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_CUR_POWER) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_MAX_POWER) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_LEVEL) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_ZONE) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_POSITION) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_AURAS) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_GUID) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_NAME) != 0) {
        FREE_STRING(object->pet_name);

    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_MODEL_ID) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_CUR_HP) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_MAX_HP) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_POWER_TYPE) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_CUR_POWER) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_MAX_POWER) != 0) {
    }
    if ((object->mask & VANILLA_GROUP_UPDATE_FLAGS_PET_AURAS) != 0) {
    }
}

static WowWorldResult vanilla_CMSG_PET_SPELL_AUTOCAST_read(WowWorldReader* reader, vanilla_CMSG_PET_SPELL_AUTOCAST* object) {
    READ_U64(object->guid);

    READ_U32(object->id);

    READ_BOOL8(object->autocast_enabled);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_PET_SPELL_AUTOCAST_write(WowWorldWriter* writer, const vanilla_CMSG_PET_SPELL_AUTOCAST* object) {
    WRITE_U16_BE(0x000d + 4); /* size */

    WRITE_U32(0x000002f3); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->id);

    WRITE_BOOL8(object->autocast_enabled);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_WEATHER_read(WowWorldReader* reader, vanilla_SMSG_WEATHER* object) {
    object->weather_type = 0;
    READ_U32(object->weather_type);

    READ_FLOAT(object->grade);

    READ_U32(object->sound_id);

    object->change = 0;
    READ_U8(object->change);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_WEATHER_write(WowWorldWriter* writer, const vanilla_SMSG_WEATHER* object) {
    WRITE_U16_BE(0x000d + 2); /* size */

    WRITE_U16(0x000002f4); /* opcode */

    WRITE_U32(object->weather_type);

    WRITE_FLOAT(object->grade);

    WRITE_U32(object->sound_id);

    WRITE_U8(object->change);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_RAID_INSTANCE_MESSAGE_read(WowWorldReader* reader, vanilla_SMSG_RAID_INSTANCE_MESSAGE* object) {
    object->message_type = 0;
    READ_U32(object->message_type);

    object->map = 0;
    READ_U32(object->map);

    READ_U32(object->time_left);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_RAID_INSTANCE_MESSAGE_write(WowWorldWriter* writer, const vanilla_SMSG_RAID_INSTANCE_MESSAGE* object) {
    WRITE_U16_BE(0x000c + 2); /* size */

    WRITE_U16(0x000002fa); /* opcode */

    WRITE_U32(object->message_type);

    WRITE_U32(object->map);

    WRITE_U32(object->time_left);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_COMPRESSED_MOVES_size(const vanilla_SMSG_COMPRESSED_MOVES* object) {
    size_t _size = 0;

    /* C89 scope to allow variable declarations */ {
        int i;
        for(i = 0; i < (int)object->amount_of_moves; ++i) {
            _size += vanilla_CompressedMove_size(&object->moves[i]);
        }
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_COMPRESSED_MOVES_read(WowWorldReader* reader, vanilla_SMSG_COMPRESSED_MOVES* object, size_t body_size) {
    size_t _size = 0;

    WowWorldReader stack_reader;
    unsigned char* _compressed_data = NULL;
    uint32_t _decompressed_size;
    READ_U32(_decompressed_size);
    _size += 4;

    _compressed_data = malloc(_decompressed_size);

    if (!wwm_decompress_data(&reader->source[reader->index], body_size - _size, _compressed_data, _decompressed_size)) {
        return WWM_RESULT_COMPRESSION_ERROR;
    }

    stack_reader = wwm_create_reader(_compressed_data, _decompressed_size);
    reader = &stack_reader;

    object->amount_of_moves = 0;
    /* C89 scope to allow variable declarations */ {
        int i = 0;
        size_t _current_size = 8 * sizeof(*object->moves);

        object->moves = malloc(_current_size);
        while ((reader->index + 1) < reader->length) {
            WWM_CHECK_RETURN_CODE(vanilla_CompressedMove_read(reader, &object->moves[i]));
            ++i;

            if (i * sizeof(*object->moves) >= _current_size) {
                _current_size *= 2;
                object->moves = realloc(object->moves, _current_size);
            }
        }

        object->amount_of_moves = i;
    }

    free(_compressed_data);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_COMPRESSED_MOVES_write(WowWorldWriter* writer, const vanilla_SMSG_COMPRESSED_MOVES* object) {
    WowWorldWriter* old_writer = writer;
    unsigned char* _decompressed_data;
    WowWorldWriter stack_writer;
    size_t _compressed_data_length;
    size_t saved_writer_index;
    const uint32_t _decompressed_data_length = (uint32_t)vanilla_SMSG_COMPRESSED_MOVES_size(object);


    WRITE_U16_BE(0 /* place holder */ + 2); /* size */

    WRITE_U16(0x000002fb); /* opcode */

    WRITE_U32(_decompressed_data_length);
    writer = &stack_writer;

    if (_decompressed_data_length == 0) {
        return WWM_RESULT_SUCCESS;
    }

    _decompressed_data = malloc(_decompressed_data_length);
    stack_writer = wwm_create_writer(_decompressed_data, _decompressed_data_length);

    WRITE_ARRAY(object->moves, object->amount_of_moves, WWM_CHECK_RETURN_CODE(vanilla_CompressedMove_write(writer, &object->moves[i])));

    writer = old_writer;
    _compressed_data_length = wwm_compress_data(stack_writer.destination, stack_writer.length, &writer->destination[writer->index], writer->length - writer->index);
    writer->index += _compressed_data_length;
    saved_writer_index = writer->index;
    writer->index = 0;

    WRITE_U16_BE((uint16_t)(_compressed_data_length + 4 + 4)); /* size */

    writer->index = saved_writer_index;
    free(_decompressed_data);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_COMPRESSED_MOVES_free(vanilla_SMSG_COMPRESSED_MOVES* object) {
    size_t i;

    for (i = 0; i < object->amount_of_moves; ++i) {
        vanilla_CompressedMove_free(&((object->moves)[i]));
    }
    free(object->moves);
    object->moves = NULL;
}

static size_t vanilla_CMSG_GUILD_INFO_TEXT_size(const vanilla_CMSG_GUILD_INFO_TEXT* object) {
    return 1 + STRING_SIZE(object->guild_info);
}

static WowWorldResult vanilla_CMSG_GUILD_INFO_TEXT_read(WowWorldReader* reader, vanilla_CMSG_GUILD_INFO_TEXT* object) {
    READ_CSTRING(object->guild_info);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GUILD_INFO_TEXT_write(WowWorldWriter* writer, const vanilla_CMSG_GUILD_INFO_TEXT* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_GUILD_INFO_TEXT_size(object) + 4); /* size */

    WRITE_U32(0x000002fc); /* opcode */

    WRITE_CSTRING(object->guild_info);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_GUILD_INFO_TEXT_free(vanilla_CMSG_GUILD_INFO_TEXT* object) {
    FREE_STRING(object->guild_info);

}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_CHAT_RESTRICTED_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 2); /* size */

    WRITE_U16(0x000002fd); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_SPLINE_SET_RUN_SPEED_size(const vanilla_SMSG_SPLINE_SET_RUN_SPEED* object) {
    return 4 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_SPLINE_SET_RUN_SPEED_read(WowWorldReader* reader, vanilla_SMSG_SPLINE_SET_RUN_SPEED* object) {
    READ_PACKED_GUID(object->guid);

    READ_FLOAT(object->speed);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPLINE_SET_RUN_SPEED_write(WowWorldWriter* writer, const vanilla_SMSG_SPLINE_SET_RUN_SPEED* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SPLINE_SET_RUN_SPEED_size(object) + 2); /* size */

    WRITE_U16(0x000002fe); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WRITE_FLOAT(object->speed);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_SPLINE_SET_RUN_BACK_SPEED_size(const vanilla_SMSG_SPLINE_SET_RUN_BACK_SPEED* object) {
    return 4 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_SPLINE_SET_RUN_BACK_SPEED_read(WowWorldReader* reader, vanilla_SMSG_SPLINE_SET_RUN_BACK_SPEED* object) {
    READ_PACKED_GUID(object->guid);

    READ_FLOAT(object->speed);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPLINE_SET_RUN_BACK_SPEED_write(WowWorldWriter* writer, const vanilla_SMSG_SPLINE_SET_RUN_BACK_SPEED* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SPLINE_SET_RUN_BACK_SPEED_size(object) + 2); /* size */

    WRITE_U16(0x000002ff); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WRITE_FLOAT(object->speed);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_SPLINE_SET_SWIM_SPEED_size(const vanilla_SMSG_SPLINE_SET_SWIM_SPEED* object) {
    return 4 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_SPLINE_SET_SWIM_SPEED_read(WowWorldReader* reader, vanilla_SMSG_SPLINE_SET_SWIM_SPEED* object) {
    READ_PACKED_GUID(object->guid);

    READ_FLOAT(object->speed);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPLINE_SET_SWIM_SPEED_write(WowWorldWriter* writer, const vanilla_SMSG_SPLINE_SET_SWIM_SPEED* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SPLINE_SET_SWIM_SPEED_size(object) + 2); /* size */

    WRITE_U16(0x00000300); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WRITE_FLOAT(object->speed);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_SPLINE_SET_WALK_SPEED_size(const vanilla_SMSG_SPLINE_SET_WALK_SPEED* object) {
    return 4 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_SPLINE_SET_WALK_SPEED_read(WowWorldReader* reader, vanilla_SMSG_SPLINE_SET_WALK_SPEED* object) {
    READ_PACKED_GUID(object->guid);

    READ_FLOAT(object->speed);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPLINE_SET_WALK_SPEED_write(WowWorldWriter* writer, const vanilla_SMSG_SPLINE_SET_WALK_SPEED* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SPLINE_SET_WALK_SPEED_size(object) + 2); /* size */

    WRITE_U16(0x00000301); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WRITE_FLOAT(object->speed);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_SPLINE_SET_SWIM_BACK_SPEED_size(const vanilla_SMSG_SPLINE_SET_SWIM_BACK_SPEED* object) {
    return 4 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_SPLINE_SET_SWIM_BACK_SPEED_read(WowWorldReader* reader, vanilla_SMSG_SPLINE_SET_SWIM_BACK_SPEED* object) {
    READ_PACKED_GUID(object->guid);

    READ_FLOAT(object->speed);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPLINE_SET_SWIM_BACK_SPEED_write(WowWorldWriter* writer, const vanilla_SMSG_SPLINE_SET_SWIM_BACK_SPEED* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SPLINE_SET_SWIM_BACK_SPEED_size(object) + 2); /* size */

    WRITE_U16(0x00000302); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WRITE_FLOAT(object->speed);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_SPLINE_SET_TURN_RATE_size(const vanilla_SMSG_SPLINE_SET_TURN_RATE* object) {
    return 4 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_SPLINE_SET_TURN_RATE_read(WowWorldReader* reader, vanilla_SMSG_SPLINE_SET_TURN_RATE* object) {
    READ_PACKED_GUID(object->guid);

    READ_FLOAT(object->speed);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPLINE_SET_TURN_RATE_write(WowWorldWriter* writer, const vanilla_SMSG_SPLINE_SET_TURN_RATE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SPLINE_SET_TURN_RATE_size(object) + 2); /* size */

    WRITE_U16(0x00000303); /* opcode */

    WRITE_PACKED_GUID(object->guid);

    WRITE_FLOAT(object->speed);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_SPLINE_MOVE_UNROOT_size(const vanilla_SMSG_SPLINE_MOVE_UNROOT* object) {
    return 0 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_SPLINE_MOVE_UNROOT_read(WowWorldReader* reader, vanilla_SMSG_SPLINE_MOVE_UNROOT* object) {
    READ_PACKED_GUID(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPLINE_MOVE_UNROOT_write(WowWorldWriter* writer, const vanilla_SMSG_SPLINE_MOVE_UNROOT* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SPLINE_MOVE_UNROOT_size(object) + 2); /* size */

    WRITE_U16(0x00000304); /* opcode */

    WRITE_PACKED_GUID(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_SPLINE_MOVE_FEATHER_FALL_size(const vanilla_SMSG_SPLINE_MOVE_FEATHER_FALL* object) {
    return 0 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_SPLINE_MOVE_FEATHER_FALL_read(WowWorldReader* reader, vanilla_SMSG_SPLINE_MOVE_FEATHER_FALL* object) {
    READ_PACKED_GUID(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPLINE_MOVE_FEATHER_FALL_write(WowWorldWriter* writer, const vanilla_SMSG_SPLINE_MOVE_FEATHER_FALL* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SPLINE_MOVE_FEATHER_FALL_size(object) + 2); /* size */

    WRITE_U16(0x00000305); /* opcode */

    WRITE_PACKED_GUID(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_SPLINE_MOVE_NORMAL_FALL_size(const vanilla_SMSG_SPLINE_MOVE_NORMAL_FALL* object) {
    return 0 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_SPLINE_MOVE_NORMAL_FALL_read(WowWorldReader* reader, vanilla_SMSG_SPLINE_MOVE_NORMAL_FALL* object) {
    READ_PACKED_GUID(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPLINE_MOVE_NORMAL_FALL_write(WowWorldWriter* writer, const vanilla_SMSG_SPLINE_MOVE_NORMAL_FALL* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SPLINE_MOVE_NORMAL_FALL_size(object) + 2); /* size */

    WRITE_U16(0x00000306); /* opcode */

    WRITE_PACKED_GUID(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_SPLINE_MOVE_SET_HOVER_size(const vanilla_SMSG_SPLINE_MOVE_SET_HOVER* object) {
    return 0 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_SPLINE_MOVE_SET_HOVER_read(WowWorldReader* reader, vanilla_SMSG_SPLINE_MOVE_SET_HOVER* object) {
    READ_PACKED_GUID(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPLINE_MOVE_SET_HOVER_write(WowWorldWriter* writer, const vanilla_SMSG_SPLINE_MOVE_SET_HOVER* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SPLINE_MOVE_SET_HOVER_size(object) + 2); /* size */

    WRITE_U16(0x00000307); /* opcode */

    WRITE_PACKED_GUID(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_SPLINE_MOVE_UNSET_HOVER_size(const vanilla_SMSG_SPLINE_MOVE_UNSET_HOVER* object) {
    return 0 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_SPLINE_MOVE_UNSET_HOVER_read(WowWorldReader* reader, vanilla_SMSG_SPLINE_MOVE_UNSET_HOVER* object) {
    READ_PACKED_GUID(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPLINE_MOVE_UNSET_HOVER_write(WowWorldWriter* writer, const vanilla_SMSG_SPLINE_MOVE_UNSET_HOVER* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SPLINE_MOVE_UNSET_HOVER_size(object) + 2); /* size */

    WRITE_U16(0x00000308); /* opcode */

    WRITE_PACKED_GUID(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_SPLINE_MOVE_WATER_WALK_size(const vanilla_SMSG_SPLINE_MOVE_WATER_WALK* object) {
    return 0 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_SPLINE_MOVE_WATER_WALK_read(WowWorldReader* reader, vanilla_SMSG_SPLINE_MOVE_WATER_WALK* object) {
    READ_PACKED_GUID(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPLINE_MOVE_WATER_WALK_write(WowWorldWriter* writer, const vanilla_SMSG_SPLINE_MOVE_WATER_WALK* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SPLINE_MOVE_WATER_WALK_size(object) + 2); /* size */

    WRITE_U16(0x00000309); /* opcode */

    WRITE_PACKED_GUID(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_SPLINE_MOVE_LAND_WALK_size(const vanilla_SMSG_SPLINE_MOVE_LAND_WALK* object) {
    return 0 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_SPLINE_MOVE_LAND_WALK_read(WowWorldReader* reader, vanilla_SMSG_SPLINE_MOVE_LAND_WALK* object) {
    READ_PACKED_GUID(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPLINE_MOVE_LAND_WALK_write(WowWorldWriter* writer, const vanilla_SMSG_SPLINE_MOVE_LAND_WALK* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SPLINE_MOVE_LAND_WALK_size(object) + 2); /* size */

    WRITE_U16(0x0000030a); /* opcode */

    WRITE_PACKED_GUID(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_SPLINE_MOVE_START_SWIM_size(const vanilla_SMSG_SPLINE_MOVE_START_SWIM* object) {
    return 0 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_SPLINE_MOVE_START_SWIM_read(WowWorldReader* reader, vanilla_SMSG_SPLINE_MOVE_START_SWIM* object) {
    READ_PACKED_GUID(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPLINE_MOVE_START_SWIM_write(WowWorldWriter* writer, const vanilla_SMSG_SPLINE_MOVE_START_SWIM* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SPLINE_MOVE_START_SWIM_size(object) + 2); /* size */

    WRITE_U16(0x0000030b); /* opcode */

    WRITE_PACKED_GUID(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_SPLINE_MOVE_STOP_SWIM_size(const vanilla_SMSG_SPLINE_MOVE_STOP_SWIM* object) {
    return 0 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_SPLINE_MOVE_STOP_SWIM_read(WowWorldReader* reader, vanilla_SMSG_SPLINE_MOVE_STOP_SWIM* object) {
    READ_PACKED_GUID(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPLINE_MOVE_STOP_SWIM_write(WowWorldWriter* writer, const vanilla_SMSG_SPLINE_MOVE_STOP_SWIM* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SPLINE_MOVE_STOP_SWIM_size(object) + 2); /* size */

    WRITE_U16(0x0000030c); /* opcode */

    WRITE_PACKED_GUID(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_SPLINE_MOVE_SET_RUN_MODE_size(const vanilla_SMSG_SPLINE_MOVE_SET_RUN_MODE* object) {
    return 0 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_SPLINE_MOVE_SET_RUN_MODE_read(WowWorldReader* reader, vanilla_SMSG_SPLINE_MOVE_SET_RUN_MODE* object) {
    READ_PACKED_GUID(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPLINE_MOVE_SET_RUN_MODE_write(WowWorldWriter* writer, const vanilla_SMSG_SPLINE_MOVE_SET_RUN_MODE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SPLINE_MOVE_SET_RUN_MODE_size(object) + 2); /* size */

    WRITE_U16(0x0000030d); /* opcode */

    WRITE_PACKED_GUID(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_SPLINE_MOVE_SET_WALK_MODE_size(const vanilla_SMSG_SPLINE_MOVE_SET_WALK_MODE* object) {
    return 0 + wwm_packed_guid_size(object->guid);
}

static WowWorldResult vanilla_SMSG_SPLINE_MOVE_SET_WALK_MODE_read(WowWorldReader* reader, vanilla_SMSG_SPLINE_MOVE_SET_WALK_MODE* object) {
    READ_PACKED_GUID(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPLINE_MOVE_SET_WALK_MODE_write(WowWorldWriter* writer, const vanilla_SMSG_SPLINE_MOVE_SET_WALK_MODE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SPLINE_MOVE_SET_WALK_MODE_size(object) + 2); /* size */

    WRITE_U16(0x0000030e); /* opcode */

    WRITE_PACKED_GUID(object->guid);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_ACTIVATETAXIEXPRESS_size(const vanilla_CMSG_ACTIVATETAXIEXPRESS* object) {
    return 16 + 4 * object->node_count;
}

static WowWorldResult vanilla_CMSG_ACTIVATETAXIEXPRESS_read(WowWorldReader* reader, vanilla_CMSG_ACTIVATETAXIEXPRESS* object) {
    READ_U64(object->guid);

    READ_U32(object->total_cost);

    READ_U32(object->node_count);

    READ_ARRAY_ALLOCATE(object->nodes, object->node_count, sizeof(uint32_t));
    READ_ARRAY(object->nodes, object->node_count, READ_U32(object->nodes[i]));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_ACTIVATETAXIEXPRESS_write(WowWorldWriter* writer, const vanilla_CMSG_ACTIVATETAXIEXPRESS* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_ACTIVATETAXIEXPRESS_size(object) + 4); /* size */

    WRITE_U32(0x00000312); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->total_cost);

    WRITE_U32(object->node_count);

    WRITE_ARRAY(object->nodes, object->node_count, WRITE_U32(object->nodes[i]));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_ACTIVATETAXIEXPRESS_free(vanilla_CMSG_ACTIVATETAXIEXPRESS* object) {
    free(object->nodes);
    object->nodes = NULL;
}

static WowWorldResult vanilla_CMSG_SET_FACTION_INACTIVE_read(WowWorldReader* reader, vanilla_CMSG_SET_FACTION_INACTIVE* object) {
    object->faction = 0;
    READ_U16(object->faction);

    READ_BOOL8(object->inactive);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_SET_FACTION_INACTIVE_write(WowWorldWriter* writer, const vanilla_CMSG_SET_FACTION_INACTIVE* object) {
    WRITE_U16_BE(0x0003 + 4); /* size */

    WRITE_U32(0x00000317); /* opcode */

    WRITE_U16(object->faction);

    WRITE_BOOL8(object->inactive);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_CMSG_SET_WATCHED_FACTION_read(WowWorldReader* reader, vanilla_CMSG_SET_WATCHED_FACTION* object) {
    object->faction = 0;
    READ_U16(object->faction);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_SET_WATCHED_FACTION_write(WowWorldWriter* writer, const vanilla_CMSG_SET_WATCHED_FACTION* object) {
    WRITE_U16_BE(0x0002 + 4); /* size */

    WRITE_U32(0x00000318); /* opcode */

    WRITE_U16(object->faction);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_MSG_MOVE_TIME_SKIPPED_Server_size(const vanilla_MSG_MOVE_TIME_SKIPPED_Server* object) {
    return 4 + wwm_packed_guid_size(object->player);
}

static WowWorldResult vanilla_MSG_MOVE_TIME_SKIPPED_Server_read(WowWorldReader* reader, vanilla_MSG_MOVE_TIME_SKIPPED_Server* object) {
    READ_PACKED_GUID(object->player);

    READ_U32(object->time_skipped);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_MOVE_TIME_SKIPPED_Server_write(WowWorldWriter* writer, const vanilla_MSG_MOVE_TIME_SKIPPED_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_MOVE_TIME_SKIPPED_Server_size(object) + 2); /* size */

    WRITE_U16(0x00000319); /* opcode */

    WRITE_PACKED_GUID(object->player);

    WRITE_U32(object->time_skipped);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_SPLINE_MOVE_ROOT_read(WowWorldReader* reader, vanilla_SMSG_SPLINE_MOVE_ROOT* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPLINE_MOVE_ROOT_write(WowWorldWriter* writer, const vanilla_SMSG_SPLINE_MOVE_ROOT* object) {
    WRITE_U16_BE(0x0008 + 2); /* size */

    WRITE_U16(0x0000031a); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_INVALIDATE_PLAYER_read(WowWorldReader* reader, vanilla_SMSG_INVALIDATE_PLAYER* object) {
    READ_U64(object->guid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_INVALIDATE_PLAYER_write(WowWorldWriter* writer, const vanilla_SMSG_INVALIDATE_PLAYER* object) {
    WRITE_U16_BE(0x0008 + 2); /* size */

    WRITE_U16(0x0000031c); /* opcode */

    WRITE_U64(object->guid);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_RESET_INSTANCES_write(WowWorldWriter* writer) {
    WRITE_U16_BE(0x0000 + 4); /* size */

    WRITE_U32(0x0000031d); /* opcode */


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_INSTANCE_RESET_read(WowWorldReader* reader, vanilla_SMSG_INSTANCE_RESET* object) {
    object->map = 0;
    READ_U32(object->map);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_INSTANCE_RESET_write(WowWorldWriter* writer, const vanilla_SMSG_INSTANCE_RESET* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x0000031e); /* opcode */

    WRITE_U32(object->map);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_INSTANCE_RESET_FAILED_read(WowWorldReader* reader, vanilla_SMSG_INSTANCE_RESET_FAILED* object) {
    object->reason = 0;
    READ_U32(object->reason);

    object->map = 0;
    READ_U32(object->map);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_INSTANCE_RESET_FAILED_write(WowWorldWriter* writer, const vanilla_SMSG_INSTANCE_RESET_FAILED* object) {
    WRITE_U16_BE(0x0008 + 2); /* size */

    WRITE_U16(0x0000031f); /* opcode */

    WRITE_U32(object->reason);

    WRITE_U32(object->map);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_UPDATE_LAST_INSTANCE_read(WowWorldReader* reader, vanilla_SMSG_UPDATE_LAST_INSTANCE* object) {
    object->map = 0;
    READ_U32(object->map);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_UPDATE_LAST_INSTANCE_write(WowWorldWriter* writer, const vanilla_SMSG_UPDATE_LAST_INSTANCE* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x00000320); /* opcode */

    WRITE_U32(object->map);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_MSG_RAID_TARGET_UPDATE_Client_size(const vanilla_MSG_RAID_TARGET_UPDATE_Client* object) {
    size_t _size = 1;

    if (object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN0|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN1|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN2|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN3|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN4|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN5|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN6|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN7|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN8) {
        _size += 8;
    }

    return _size;
}

static WowWorldResult vanilla_MSG_RAID_TARGET_UPDATE_Client_read(WowWorldReader* reader, vanilla_MSG_RAID_TARGET_UPDATE_Client* object) {
    object->target_index = 0;
    READ_U8(object->target_index);

    if (object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN0|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN1|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN2|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN3|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN4|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN5|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN6|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN7|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN8) {
        READ_U64(object->target);

    }
    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_RAID_TARGET_UPDATE_Client_write(WowWorldWriter* writer, const vanilla_MSG_RAID_TARGET_UPDATE_Client* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_RAID_TARGET_UPDATE_Client_size(object) + 4); /* size */

    WRITE_U32(0x00000321); /* opcode */

    WRITE_U8(object->target_index);

    if (object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN0|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN1|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN2|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN3|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN4|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN5|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN6|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN7|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN8) {
        WRITE_U64(object->target);

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_RAID_TARGET_UPDATE_Client_free(vanilla_MSG_RAID_TARGET_UPDATE_Client* object) {
    if (object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN0|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN1|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN2|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN3|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN4|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN5|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN6|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN7|| object->target_index == VANILLA_RAID_TARGET_INDEX_UNKNOWN8) {
    }
}

static size_t vanilla_MSG_RAID_TARGET_UPDATE_Server_size(const vanilla_MSG_RAID_TARGET_UPDATE_Server* object) {
    size_t _size = 1;

    if (object->update_type == VANILLA_RAID_TARGET_UPDATE_TYPE_FULL) {
        _size += 72;
    }
    else if (object->update_type == VANILLA_RAID_TARGET_UPDATE_TYPE_PARTIAL) {
        _size += 9;
    }

    return _size;
}

static WowWorldResult vanilla_MSG_RAID_TARGET_UPDATE_Server_read(WowWorldReader* reader, vanilla_MSG_RAID_TARGET_UPDATE_Server* object) {
    object->update_type = 0;
    READ_U8(object->update_type);

    if (object->update_type == VANILLA_RAID_TARGET_UPDATE_TYPE_FULL) {
        READ_ARRAY_ALLOCATE(object->raid_targets, 8, sizeof(vanilla_RaidTargetUpdate));
        READ_ARRAY(object->raid_targets, 8, WWM_CHECK_RETURN_CODE(vanilla_RaidTargetUpdate_read(reader, &(*object->raid_targets)[i])));

    }
    else if (object->update_type == VANILLA_RAID_TARGET_UPDATE_TYPE_PARTIAL) {
        WWM_CHECK_RETURN_CODE(vanilla_RaidTargetUpdate_read(reader, &object->raid_target));

    }
    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_RAID_TARGET_UPDATE_Server_write(WowWorldWriter* writer, const vanilla_MSG_RAID_TARGET_UPDATE_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_RAID_TARGET_UPDATE_Server_size(object) + 2); /* size */

    WRITE_U16(0x00000321); /* opcode */

    WRITE_U8(object->update_type);

    if (object->update_type == VANILLA_RAID_TARGET_UPDATE_TYPE_FULL) {
        WRITE_ARRAY(object->raid_targets, 8, WWM_CHECK_RETURN_CODE(vanilla_RaidTargetUpdate_write(writer, &(*object->raid_targets)[i])));

    }
    else if (object->update_type == VANILLA_RAID_TARGET_UPDATE_TYPE_PARTIAL) {
        WWM_CHECK_RETURN_CODE(vanilla_RaidTargetUpdate_write(writer, &object->raid_target));

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_RAID_TARGET_UPDATE_Server_free(vanilla_MSG_RAID_TARGET_UPDATE_Server* object) {
    if (object->update_type == VANILLA_RAID_TARGET_UPDATE_TYPE_FULL) {
        free(object->raid_targets);
        object->raid_targets = NULL;
    }
    else if (object->update_type == VANILLA_RAID_TARGET_UPDATE_TYPE_PARTIAL) {
    }
}

static size_t vanilla_MSG_RAID_READY_CHECK_Client_size(const vanilla_MSG_RAID_READY_CHECK_Client* object) {
    size_t _size = 0;

    if(object->answer) {
        _size += 1;
    }

    return _size;
}

static WowWorldResult vanilla_MSG_RAID_READY_CHECK_Client_read(WowWorldReader* reader, vanilla_MSG_RAID_READY_CHECK_Client* object, size_t body_size) {
    size_t _size = 0;

    object->answer = NULL;
    if (_size < body_size) {
        object->answer = malloc(sizeof(vanilla_MSG_RAID_READY_CHECK_Client_answer));

        READ_U8(object->answer->state);

    }
    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_RAID_READY_CHECK_Client_write(WowWorldWriter* writer, const vanilla_MSG_RAID_READY_CHECK_Client* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_RAID_READY_CHECK_Client_size(object) + 4); /* size */

    WRITE_U32(0x00000322); /* opcode */

    if(object->answer) {
        WRITE_U8(object->answer->state);

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_RAID_READY_CHECK_Client_free(vanilla_MSG_RAID_READY_CHECK_Client* object) {
    free(object->answer);
}

static size_t vanilla_MSG_RAID_READY_CHECK_Server_size(const vanilla_MSG_RAID_READY_CHECK_Server* object) {
    size_t _size = 0;

    if(object->state_check) {
        _size += 9;
    }

    return _size;
}

static WowWorldResult vanilla_MSG_RAID_READY_CHECK_Server_read(WowWorldReader* reader, vanilla_MSG_RAID_READY_CHECK_Server* object, size_t body_size) {
    size_t _size = 0;

    object->state_check = NULL;
    if (_size < body_size) {
        object->state_check = malloc(sizeof(vanilla_MSG_RAID_READY_CHECK_Server_state_check));

        READ_U64(object->state_check->guid);

        READ_U8(object->state_check->state);

    }
    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_MSG_RAID_READY_CHECK_Server_write(WowWorldWriter* writer, const vanilla_MSG_RAID_READY_CHECK_Server* object) {
    WRITE_U16_BE((uint16_t)vanilla_MSG_RAID_READY_CHECK_Server_size(object) + 2); /* size */

    WRITE_U16(0x00000322); /* opcode */

    if(object->state_check) {
        WRITE_U64(object->state_check->guid);

        WRITE_U8(object->state_check->state);

    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_MSG_RAID_READY_CHECK_Server_free(vanilla_MSG_RAID_READY_CHECK_Server* object) {
    free(object->state_check);
}

static WowWorldResult vanilla_SMSG_PET_ACTION_SOUND_read(WowWorldReader* reader, vanilla_SMSG_PET_ACTION_SOUND* object) {
    READ_U64(object->guid);

    object->reason = 0;
    READ_U32(object->reason);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PET_ACTION_SOUND_write(WowWorldWriter* writer, const vanilla_SMSG_PET_ACTION_SOUND* object) {
    WRITE_U16_BE(0x000c + 2); /* size */

    WRITE_U16(0x00000324); /* opcode */

    WRITE_U64(object->guid);

    WRITE_U32(object->reason);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_PET_DISMISS_SOUND_read(WowWorldReader* reader, vanilla_SMSG_PET_DISMISS_SOUND* object) {
    READ_U32(object->sound_id);

    WWM_CHECK_RETURN_CODE(all_Vector3d_read(reader, &object->position));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_PET_DISMISS_SOUND_write(WowWorldWriter* writer, const vanilla_SMSG_PET_DISMISS_SOUND* object) {
    WRITE_U16_BE(0x0010 + 2); /* size */

    WRITE_U16(0x00000325); /* opcode */

    WRITE_U32(object->sound_id);

    WWM_CHECK_RETURN_CODE(all_Vector3d_write(writer, &object->position));


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_GM_TICKET_STATUS_UPDATE_read(WowWorldReader* reader, vanilla_SMSG_GM_TICKET_STATUS_UPDATE* object) {
    object->response = 0;
    READ_U32(object->response);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_GM_TICKET_STATUS_UPDATE_write(WowWorldWriter* writer, const vanilla_SMSG_GM_TICKET_STATUS_UPDATE* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x00000328); /* opcode */

    WRITE_U32(object->response);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_CMSG_GMSURVEY_SUBMIT_size(const vanilla_CMSG_GMSURVEY_SUBMIT* object) {
    size_t _size = 5 + STRING_SIZE(object->answer_comment);

    /* C89 scope to allow variable declarations */ {
        int i;
        for(i = 0; i < 10; ++i) {
            _size += vanilla_GmSurveyQuestion_size(&(*object->questions[i]));
        }
    }

    return _size;
}

static WowWorldResult vanilla_CMSG_GMSURVEY_SUBMIT_read(WowWorldReader* reader, vanilla_CMSG_GMSURVEY_SUBMIT* object) {
    READ_U32(object->survey_id);

    READ_ARRAY_ALLOCATE(object->questions, 10, sizeof(vanilla_GmSurveyQuestion));
    READ_ARRAY(object->questions, 10, WWM_CHECK_RETURN_CODE(vanilla_GmSurveyQuestion_read(reader, &(*object->questions)[i])));

    READ_CSTRING(object->answer_comment);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_CMSG_GMSURVEY_SUBMIT_write(WowWorldWriter* writer, const vanilla_CMSG_GMSURVEY_SUBMIT* object) {
    WRITE_U16_BE((uint16_t)vanilla_CMSG_GMSURVEY_SUBMIT_size(object) + 4); /* size */

    WRITE_U32(0x0000032a); /* opcode */

    WRITE_U32(object->survey_id);

    WRITE_ARRAY(object->questions, 10, WWM_CHECK_RETURN_CODE(vanilla_GmSurveyQuestion_write(writer, &(*object->questions)[i])));

    WRITE_CSTRING(object->answer_comment);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_CMSG_GMSURVEY_SUBMIT_free(vanilla_CMSG_GMSURVEY_SUBMIT* object) {
    size_t i;

    for (i = 0; i < 10; ++i) {
        vanilla_GmSurveyQuestion_free(&((*object->questions)[i]));
    }
    free(object->questions);
    object->questions = NULL;
    FREE_STRING(object->answer_comment);

}

static WowWorldResult vanilla_SMSG_UPDATE_INSTANCE_OWNERSHIP_read(WowWorldReader* reader, vanilla_SMSG_UPDATE_INSTANCE_OWNERSHIP* object) {
    READ_BOOL32(object->player_is_saved_to_a_raid);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_UPDATE_INSTANCE_OWNERSHIP_write(WowWorldWriter* writer, const vanilla_SMSG_UPDATE_INSTANCE_OWNERSHIP* object) {
    WRITE_U16_BE(0x0004 + 2); /* size */

    WRITE_U16(0x0000032b); /* opcode */

    WRITE_BOOL32(object->player_is_saved_to_a_raid);


    return WWM_RESULT_SUCCESS;
}

static WowWorldResult vanilla_SMSG_SPELLINSTAKILLLOG_read(WowWorldReader* reader, vanilla_SMSG_SPELLINSTAKILLLOG* object) {
    READ_U64(object->target);

    READ_U32(object->spell);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPELLINSTAKILLLOG_write(WowWorldWriter* writer, const vanilla_SMSG_SPELLINSTAKILLLOG* object) {
    WRITE_U16_BE(0x000c + 2); /* size */

    WRITE_U16(0x0000032f); /* opcode */

    WRITE_U64(object->target);

    WRITE_U32(object->spell);


    return WWM_RESULT_SUCCESS;
}

static size_t vanilla_SMSG_SPELL_UPDATE_CHAIN_TARGETS_size(const vanilla_SMSG_SPELL_UPDATE_CHAIN_TARGETS* object) {
    return 16 + 8 * object->amount_of_targets;
}

static WowWorldResult vanilla_SMSG_SPELL_UPDATE_CHAIN_TARGETS_read(WowWorldReader* reader, vanilla_SMSG_SPELL_UPDATE_CHAIN_TARGETS* object) {
    READ_U64(object->caster);

    READ_U32(object->spell);

    READ_U32(object->amount_of_targets);

    READ_ARRAY_ALLOCATE(object->targets, object->amount_of_targets, sizeof(uint64_t));
    READ_ARRAY(object->targets, object->amount_of_targets, READ_U64(object->targets[i]));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_SPELL_UPDATE_CHAIN_TARGETS_write(WowWorldWriter* writer, const vanilla_SMSG_SPELL_UPDATE_CHAIN_TARGETS* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_SPELL_UPDATE_CHAIN_TARGETS_size(object) + 2); /* size */

    WRITE_U16(0x00000330); /* opcode */

    WRITE_U64(object->caster);

    WRITE_U32(object->spell);

    WRITE_U32(object->amount_of_targets);

    WRITE_ARRAY(object->targets, object->amount_of_targets, WRITE_U64(object->targets[i]));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_SPELL_UPDATE_CHAIN_TARGETS_free(vanilla_SMSG_SPELL_UPDATE_CHAIN_TARGETS* object) {
    free(object->targets);
    object->targets = NULL;
}

static size_t vanilla_SMSG_EXPECTED_SPAM_RECORDS_size(const vanilla_SMSG_EXPECTED_SPAM_RECORDS* object) {
    size_t _size = 4;

    /* C89 scope to allow variable declarations */ {
        int i;
        for(i = 0; i < (int)object->amount_of_records; ++i) {
            _size += STRING_SIZE(object->records[i]) + 1;
        }
    }

    return _size;
}

static WowWorldResult vanilla_SMSG_EXPECTED_SPAM_RECORDS_read(WowWorldReader* reader, vanilla_SMSG_EXPECTED_SPAM_RECORDS* object) {
    READ_U32(object->amount_of_records);

    READ_ARRAY_ALLOCATE(object->records, object->amount_of_records, sizeof(WowWorldString));
    READ_ARRAY(object->records, object->amount_of_records, READ_CSTRING(object->records[i]));

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_EXPECTED_SPAM_RECORDS_write(WowWorldWriter* writer, const vanilla_SMSG_EXPECTED_SPAM_RECORDS* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_EXPECTED_SPAM_RECORDS_size(object) + 2); /* size */

    WRITE_U16(0x00000332); /* opcode */

    WRITE_U32(object->amount_of_records);

    WRITE_ARRAY(object->records, object->amount_of_records, WRITE_CSTRING(object->records[i]));


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_EXPECTED_SPAM_RECORDS_free(vanilla_SMSG_EXPECTED_SPAM_RECORDS* object) {
    size_t i;

    for (i = 0; i < object->amount_of_records; ++i) {
        FREE_STRING(((object->records)[i]));
    }
    free(object->records);
    object->records = NULL;
}

static size_t vanilla_SMSG_DEFENSE_MESSAGE_size(const vanilla_SMSG_DEFENSE_MESSAGE* object) {
    return 8 + STRING_SIZE(object->message);
}

static WowWorldResult vanilla_SMSG_DEFENSE_MESSAGE_read(WowWorldReader* reader, vanilla_SMSG_DEFENSE_MESSAGE* object) {
    object->area = 0;
    READ_U32(object->area);

    READ_SIZED_CSTRING(object->message);

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_SMSG_DEFENSE_MESSAGE_write(WowWorldWriter* writer, const vanilla_SMSG_DEFENSE_MESSAGE* object) {
    WRITE_U16_BE((uint16_t)vanilla_SMSG_DEFENSE_MESSAGE_size(object) + 2); /* size */

    WRITE_U16(0x0000033b); /* opcode */

    WRITE_U32(object->area);

    WRITE_SIZED_CSTRING(object->message);


    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_SMSG_DEFENSE_MESSAGE_free(vanilla_SMSG_DEFENSE_MESSAGE* object) {
    FREE_STRING(object->message);

}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_client_write_opcode(WowWorldWriter* writer, const VanillaClientOpcodeContainer* opcodes) {
    switch (opcodes->opcode) {
        case CMSG_DBLOOKUP:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_DBLOOKUP_write(writer, &opcodes->body.CMSG_DBLOOKUP));
            break;
        case CMSG_WORLD_TELEPORT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_WORLD_TELEPORT_write(writer, &opcodes->body.CMSG_WORLD_TELEPORT));
            break;
        case CMSG_TELEPORT_TO_UNIT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_TELEPORT_TO_UNIT_write(writer, &opcodes->body.CMSG_TELEPORT_TO_UNIT));
            break;
        case CMSG_CHAR_CREATE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHAR_CREATE_write(writer, &opcodes->body.CMSG_CHAR_CREATE));
            break;
        case CMSG_CHAR_DELETE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHAR_DELETE_write(writer, &opcodes->body.CMSG_CHAR_DELETE));
            break;
        case CMSG_PLAYER_LOGIN:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PLAYER_LOGIN_write(writer, &opcodes->body.CMSG_PLAYER_LOGIN));
            break;
        case CMSG_NAME_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_NAME_QUERY_write(writer, &opcodes->body.CMSG_NAME_QUERY));
            break;
        case CMSG_PET_NAME_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PET_NAME_QUERY_write(writer, &opcodes->body.CMSG_PET_NAME_QUERY));
            break;
        case CMSG_GUILD_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GUILD_QUERY_write(writer, &opcodes->body.CMSG_GUILD_QUERY));
            break;
        case CMSG_ITEM_QUERY_SINGLE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_ITEM_QUERY_SINGLE_write(writer, &opcodes->body.CMSG_ITEM_QUERY_SINGLE));
            break;
        case CMSG_PAGE_TEXT_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PAGE_TEXT_QUERY_write(writer, &opcodes->body.CMSG_PAGE_TEXT_QUERY));
            break;
        case CMSG_QUEST_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_QUEST_QUERY_write(writer, &opcodes->body.CMSG_QUEST_QUERY));
            break;
        case CMSG_GAMEOBJECT_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GAMEOBJECT_QUERY_write(writer, &opcodes->body.CMSG_GAMEOBJECT_QUERY));
            break;
        case CMSG_CREATURE_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CREATURE_QUERY_write(writer, &opcodes->body.CMSG_CREATURE_QUERY));
            break;
        case CMSG_WHO:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_WHO_write(writer, &opcodes->body.CMSG_WHO));
            break;
        case CMSG_WHOIS:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_WHOIS_write(writer, &opcodes->body.CMSG_WHOIS));
            break;
        case CMSG_ADD_FRIEND:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_ADD_FRIEND_write(writer, &opcodes->body.CMSG_ADD_FRIEND));
            break;
        case CMSG_DEL_FRIEND:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_DEL_FRIEND_write(writer, &opcodes->body.CMSG_DEL_FRIEND));
            break;
        case CMSG_ADD_IGNORE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_ADD_IGNORE_write(writer, &opcodes->body.CMSG_ADD_IGNORE));
            break;
        case CMSG_DEL_IGNORE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_DEL_IGNORE_write(writer, &opcodes->body.CMSG_DEL_IGNORE));
            break;
        case CMSG_GROUP_INVITE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GROUP_INVITE_write(writer, &opcodes->body.CMSG_GROUP_INVITE));
            break;
        case CMSG_GROUP_UNINVITE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GROUP_UNINVITE_write(writer, &opcodes->body.CMSG_GROUP_UNINVITE));
            break;
        case CMSG_GROUP_UNINVITE_GUID:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GROUP_UNINVITE_GUID_write(writer, &opcodes->body.CMSG_GROUP_UNINVITE_GUID));
            break;
        case CMSG_GROUP_SET_LEADER:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GROUP_SET_LEADER_write(writer, &opcodes->body.CMSG_GROUP_SET_LEADER));
            break;
        case CMSG_LOOT_METHOD:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_LOOT_METHOD_write(writer, &opcodes->body.CMSG_LOOT_METHOD));
            break;
        case CMSG_GUILD_CREATE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GUILD_CREATE_write(writer, &opcodes->body.CMSG_GUILD_CREATE));
            break;
        case CMSG_GUILD_INVITE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GUILD_INVITE_write(writer, &opcodes->body.CMSG_GUILD_INVITE));
            break;
        case CMSG_GUILD_PROMOTE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GUILD_PROMOTE_write(writer, &opcodes->body.CMSG_GUILD_PROMOTE));
            break;
        case CMSG_GUILD_DEMOTE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GUILD_DEMOTE_write(writer, &opcodes->body.CMSG_GUILD_DEMOTE));
            break;
        case CMSG_GUILD_REMOVE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GUILD_REMOVE_write(writer, &opcodes->body.CMSG_GUILD_REMOVE));
            break;
        case CMSG_GUILD_LEADER:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GUILD_LEADER_write(writer, &opcodes->body.CMSG_GUILD_LEADER));
            break;
        case CMSG_GUILD_MOTD:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GUILD_MOTD_write(writer, &opcodes->body.CMSG_GUILD_MOTD));
            break;
        case CMSG_MESSAGECHAT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MESSAGECHAT_write(writer, &opcodes->body.CMSG_MESSAGECHAT));
            break;
        case CMSG_JOIN_CHANNEL:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_JOIN_CHANNEL_write(writer, &opcodes->body.CMSG_JOIN_CHANNEL));
            break;
        case CMSG_LEAVE_CHANNEL:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_LEAVE_CHANNEL_write(writer, &opcodes->body.CMSG_LEAVE_CHANNEL));
            break;
        case CMSG_CHANNEL_LIST:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHANNEL_LIST_write(writer, &opcodes->body.CMSG_CHANNEL_LIST));
            break;
        case CMSG_CHANNEL_PASSWORD:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHANNEL_PASSWORD_write(writer, &opcodes->body.CMSG_CHANNEL_PASSWORD));
            break;
        case CMSG_CHANNEL_SET_OWNER:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHANNEL_SET_OWNER_write(writer, &opcodes->body.CMSG_CHANNEL_SET_OWNER));
            break;
        case CMSG_CHANNEL_OWNER:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHANNEL_OWNER_write(writer, &opcodes->body.CMSG_CHANNEL_OWNER));
            break;
        case CMSG_CHANNEL_MODERATOR:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHANNEL_MODERATOR_write(writer, &opcodes->body.CMSG_CHANNEL_MODERATOR));
            break;
        case CMSG_CHANNEL_UNMODERATOR:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHANNEL_UNMODERATOR_write(writer, &opcodes->body.CMSG_CHANNEL_UNMODERATOR));
            break;
        case CMSG_CHANNEL_MUTE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHANNEL_MUTE_write(writer, &opcodes->body.CMSG_CHANNEL_MUTE));
            break;
        case CMSG_CHANNEL_UNMUTE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHANNEL_UNMUTE_write(writer, &opcodes->body.CMSG_CHANNEL_UNMUTE));
            break;
        case CMSG_CHANNEL_INVITE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHANNEL_INVITE_write(writer, &opcodes->body.CMSG_CHANNEL_INVITE));
            break;
        case CMSG_CHANNEL_KICK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHANNEL_KICK_write(writer, &opcodes->body.CMSG_CHANNEL_KICK));
            break;
        case CMSG_CHANNEL_BAN:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHANNEL_BAN_write(writer, &opcodes->body.CMSG_CHANNEL_BAN));
            break;
        case CMSG_CHANNEL_UNBAN:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHANNEL_UNBAN_write(writer, &opcodes->body.CMSG_CHANNEL_UNBAN));
            break;
        case CMSG_CHANNEL_ANNOUNCEMENTS:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHANNEL_ANNOUNCEMENTS_write(writer, &opcodes->body.CMSG_CHANNEL_ANNOUNCEMENTS));
            break;
        case CMSG_CHANNEL_MODERATE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHANNEL_MODERATE_write(writer, &opcodes->body.CMSG_CHANNEL_MODERATE));
            break;
        case CMSG_USE_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_USE_ITEM_write(writer, &opcodes->body.CMSG_USE_ITEM));
            break;
        case CMSG_OPEN_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_OPEN_ITEM_write(writer, &opcodes->body.CMSG_OPEN_ITEM));
            break;
        case CMSG_READ_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_READ_ITEM_write(writer, &opcodes->body.CMSG_READ_ITEM));
            break;
        case CMSG_GAMEOBJ_USE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GAMEOBJ_USE_write(writer, &opcodes->body.CMSG_GAMEOBJ_USE));
            break;
        case CMSG_AREATRIGGER:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AREATRIGGER_write(writer, &opcodes->body.CMSG_AREATRIGGER));
            break;
        case MSG_MOVE_START_FORWARD:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_FORWARD_Client_write(writer, &opcodes->body.MSG_MOVE_START_FORWARD_Client));
            break;
        case MSG_MOVE_START_BACKWARD:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_BACKWARD_Client_write(writer, &opcodes->body.MSG_MOVE_START_BACKWARD_Client));
            break;
        case MSG_MOVE_STOP:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_STOP_Client_write(writer, &opcodes->body.MSG_MOVE_STOP_Client));
            break;
        case MSG_MOVE_START_STRAFE_LEFT:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_STRAFE_LEFT_Client_write(writer, &opcodes->body.MSG_MOVE_START_STRAFE_LEFT_Client));
            break;
        case MSG_MOVE_START_STRAFE_RIGHT:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_STRAFE_RIGHT_Client_write(writer, &opcodes->body.MSG_MOVE_START_STRAFE_RIGHT_Client));
            break;
        case MSG_MOVE_STOP_STRAFE:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_STOP_STRAFE_Client_write(writer, &opcodes->body.MSG_MOVE_STOP_STRAFE_Client));
            break;
        case MSG_MOVE_JUMP:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_JUMP_Client_write(writer, &opcodes->body.MSG_MOVE_JUMP_Client));
            break;
        case MSG_MOVE_START_TURN_LEFT:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_TURN_LEFT_Client_write(writer, &opcodes->body.MSG_MOVE_START_TURN_LEFT_Client));
            break;
        case MSG_MOVE_START_TURN_RIGHT:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_TURN_RIGHT_Client_write(writer, &opcodes->body.MSG_MOVE_START_TURN_RIGHT_Client));
            break;
        case MSG_MOVE_STOP_TURN:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_STOP_TURN_Client_write(writer, &opcodes->body.MSG_MOVE_STOP_TURN_Client));
            break;
        case MSG_MOVE_START_PITCH_UP:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_PITCH_UP_Client_write(writer, &opcodes->body.MSG_MOVE_START_PITCH_UP_Client));
            break;
        case MSG_MOVE_START_PITCH_DOWN:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_PITCH_DOWN_Client_write(writer, &opcodes->body.MSG_MOVE_START_PITCH_DOWN_Client));
            break;
        case MSG_MOVE_STOP_PITCH:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_STOP_PITCH_Client_write(writer, &opcodes->body.MSG_MOVE_STOP_PITCH_Client));
            break;
        case MSG_MOVE_SET_RUN_MODE:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_SET_RUN_MODE_Client_write(writer, &opcodes->body.MSG_MOVE_SET_RUN_MODE_Client));
            break;
        case MSG_MOVE_SET_WALK_MODE:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_SET_WALK_MODE_Client_write(writer, &opcodes->body.MSG_MOVE_SET_WALK_MODE_Client));
            break;
        case MSG_MOVE_TELEPORT_ACK:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_TELEPORT_ACK_Client_write(writer, &opcodes->body.MSG_MOVE_TELEPORT_ACK_Client));
            break;
        case MSG_MOVE_FALL_LAND:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_FALL_LAND_Client_write(writer, &opcodes->body.MSG_MOVE_FALL_LAND_Client));
            break;
        case MSG_MOVE_START_SWIM:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_SWIM_Client_write(writer, &opcodes->body.MSG_MOVE_START_SWIM_Client));
            break;
        case MSG_MOVE_STOP_SWIM:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_STOP_SWIM_Client_write(writer, &opcodes->body.MSG_MOVE_STOP_SWIM_Client));
            break;
        case MSG_MOVE_SET_FACING:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_SET_FACING_Client_write(writer, &opcodes->body.MSG_MOVE_SET_FACING_Client));
            break;
        case MSG_MOVE_SET_PITCH:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_SET_PITCH_Client_write(writer, &opcodes->body.MSG_MOVE_SET_PITCH_Client));
            break;
        case CMSG_MOVE_SET_RAW_POSITION:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MOVE_SET_RAW_POSITION_write(writer, &opcodes->body.CMSG_MOVE_SET_RAW_POSITION));
            break;
        case CMSG_FORCE_RUN_SPEED_CHANGE_ACK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_FORCE_RUN_SPEED_CHANGE_ACK_write(writer, &opcodes->body.CMSG_FORCE_RUN_SPEED_CHANGE_ACK));
            break;
        case CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK_write(writer, &opcodes->body.CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK));
            break;
        case CMSG_FORCE_SWIM_SPEED_CHANGE_ACK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_FORCE_SWIM_SPEED_CHANGE_ACK_write(writer, &opcodes->body.CMSG_FORCE_SWIM_SPEED_CHANGE_ACK));
            break;
        case CMSG_FORCE_MOVE_ROOT_ACK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_FORCE_MOVE_ROOT_ACK_write(writer, &opcodes->body.CMSG_FORCE_MOVE_ROOT_ACK));
            break;
        case CMSG_FORCE_MOVE_UNROOT_ACK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_FORCE_MOVE_UNROOT_ACK_write(writer, &opcodes->body.CMSG_FORCE_MOVE_UNROOT_ACK));
            break;
        case MSG_MOVE_HEARTBEAT:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_HEARTBEAT_Client_write(writer, &opcodes->body.MSG_MOVE_HEARTBEAT_Client));
            break;
        case CMSG_MOVE_KNOCK_BACK_ACK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MOVE_KNOCK_BACK_ACK_write(writer, &opcodes->body.CMSG_MOVE_KNOCK_BACK_ACK));
            break;
        case CMSG_MOVE_HOVER_ACK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MOVE_HOVER_ACK_write(writer, &opcodes->body.CMSG_MOVE_HOVER_ACK));
            break;
        case CMSG_TUTORIAL_FLAG:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_TUTORIAL_FLAG_write(writer, &opcodes->body.CMSG_TUTORIAL_FLAG));
            break;
        case CMSG_STANDSTATECHANGE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_STANDSTATECHANGE_write(writer, &opcodes->body.CMSG_STANDSTATECHANGE));
            break;
        case CMSG_EMOTE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_EMOTE_write(writer, &opcodes->body.CMSG_EMOTE));
            break;
        case CMSG_TEXT_EMOTE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_TEXT_EMOTE_write(writer, &opcodes->body.CMSG_TEXT_EMOTE));
            break;
        case CMSG_AUTOSTORE_LOOT_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AUTOSTORE_LOOT_ITEM_write(writer, &opcodes->body.CMSG_AUTOSTORE_LOOT_ITEM));
            break;
        case CMSG_AUTOEQUIP_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AUTOEQUIP_ITEM_write(writer, &opcodes->body.CMSG_AUTOEQUIP_ITEM));
            break;
        case CMSG_AUTOSTORE_BAG_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AUTOSTORE_BAG_ITEM_write(writer, &opcodes->body.CMSG_AUTOSTORE_BAG_ITEM));
            break;
        case CMSG_SWAP_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SWAP_ITEM_write(writer, &opcodes->body.CMSG_SWAP_ITEM));
            break;
        case CMSG_SWAP_INV_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SWAP_INV_ITEM_write(writer, &opcodes->body.CMSG_SWAP_INV_ITEM));
            break;
        case CMSG_SPLIT_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SPLIT_ITEM_write(writer, &opcodes->body.CMSG_SPLIT_ITEM));
            break;
        case CMSG_AUTOEQUIP_ITEM_SLOT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AUTOEQUIP_ITEM_SLOT_write(writer, &opcodes->body.CMSG_AUTOEQUIP_ITEM_SLOT));
            break;
        case CMSG_DESTROYITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_DESTROYITEM_write(writer, &opcodes->body.CMSG_DESTROYITEM));
            break;
        case CMSG_INSPECT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_INSPECT_write(writer, &opcodes->body.CMSG_INSPECT));
            break;
        case CMSG_INITIATE_TRADE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_INITIATE_TRADE_write(writer, &opcodes->body.CMSG_INITIATE_TRADE));
            break;
        case CMSG_ACCEPT_TRADE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_ACCEPT_TRADE_write(writer, &opcodes->body.CMSG_ACCEPT_TRADE));
            break;
        case CMSG_SET_TRADE_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SET_TRADE_ITEM_write(writer, &opcodes->body.CMSG_SET_TRADE_ITEM));
            break;
        case CMSG_CLEAR_TRADE_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CLEAR_TRADE_ITEM_write(writer, &opcodes->body.CMSG_CLEAR_TRADE_ITEM));
            break;
        case CMSG_SET_TRADE_GOLD:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SET_TRADE_GOLD_write(writer, &opcodes->body.CMSG_SET_TRADE_GOLD));
            break;
        case CMSG_SET_FACTION_ATWAR:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SET_FACTION_ATWAR_write(writer, &opcodes->body.CMSG_SET_FACTION_ATWAR));
            break;
        case CMSG_SET_ACTION_BUTTON:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SET_ACTION_BUTTON_write(writer, &opcodes->body.CMSG_SET_ACTION_BUTTON));
            break;
        case CMSG_CAST_SPELL:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CAST_SPELL_write(writer, &opcodes->body.CMSG_CAST_SPELL));
            break;
        case CMSG_CANCEL_CAST:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CANCEL_CAST_write(writer, &opcodes->body.CMSG_CANCEL_CAST));
            break;
        case CMSG_CANCEL_AURA:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CANCEL_AURA_write(writer, &opcodes->body.CMSG_CANCEL_AURA));
            break;
        case CMSG_CANCEL_CHANNELLING:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CANCEL_CHANNELLING_write(writer, &opcodes->body.CMSG_CANCEL_CHANNELLING));
            break;
        case CMSG_SET_SELECTION:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SET_SELECTION_write(writer, &opcodes->body.CMSG_SET_SELECTION));
            break;
        case CMSG_SET_TARGET_OBSOLETE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SET_TARGET_OBSOLETE_write(writer, &opcodes->body.CMSG_SET_TARGET_OBSOLETE));
            break;
        case CMSG_ATTACKSWING:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_ATTACKSWING_write(writer, &opcodes->body.CMSG_ATTACKSWING));
            break;
        case CMSG_RESURRECT_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_RESURRECT_RESPONSE_write(writer, &opcodes->body.CMSG_RESURRECT_RESPONSE));
            break;
        case CMSG_LOOT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_LOOT_write(writer, &opcodes->body.CMSG_LOOT));
            break;
        case CMSG_LOOT_RELEASE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_LOOT_RELEASE_write(writer, &opcodes->body.CMSG_LOOT_RELEASE));
            break;
        case CMSG_DUEL_ACCEPTED:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_DUEL_ACCEPTED_write(writer, &opcodes->body.CMSG_DUEL_ACCEPTED));
            break;
        case CMSG_DUEL_CANCELLED:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_DUEL_CANCELLED_write(writer, &opcodes->body.CMSG_DUEL_CANCELLED));
            break;
        case CMSG_PET_SET_ACTION:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PET_SET_ACTION_write(writer, &opcodes->body.CMSG_PET_SET_ACTION));
            break;
        case CMSG_PET_ACTION:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PET_ACTION_write(writer, &opcodes->body.CMSG_PET_ACTION));
            break;
        case CMSG_PET_ABANDON:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PET_ABANDON_write(writer, &opcodes->body.CMSG_PET_ABANDON));
            break;
        case CMSG_PET_RENAME:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PET_RENAME_write(writer, &opcodes->body.CMSG_PET_RENAME));
            break;
        case CMSG_GOSSIP_HELLO:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GOSSIP_HELLO_write(writer, &opcodes->body.CMSG_GOSSIP_HELLO));
            break;
        case CMSG_GOSSIP_SELECT_OPTION:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GOSSIP_SELECT_OPTION_write(writer, &opcodes->body.CMSG_GOSSIP_SELECT_OPTION));
            break;
        case CMSG_NPC_TEXT_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_NPC_TEXT_QUERY_write(writer, &opcodes->body.CMSG_NPC_TEXT_QUERY));
            break;
        case CMSG_QUESTGIVER_STATUS_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_QUESTGIVER_STATUS_QUERY_write(writer, &opcodes->body.CMSG_QUESTGIVER_STATUS_QUERY));
            break;
        case CMSG_QUESTGIVER_HELLO:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_QUESTGIVER_HELLO_write(writer, &opcodes->body.CMSG_QUESTGIVER_HELLO));
            break;
        case CMSG_QUESTGIVER_QUERY_QUEST:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_QUESTGIVER_QUERY_QUEST_write(writer, &opcodes->body.CMSG_QUESTGIVER_QUERY_QUEST));
            break;
        case CMSG_QUESTGIVER_ACCEPT_QUEST:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_QUESTGIVER_ACCEPT_QUEST_write(writer, &opcodes->body.CMSG_QUESTGIVER_ACCEPT_QUEST));
            break;
        case CMSG_QUESTGIVER_COMPLETE_QUEST:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_QUESTGIVER_COMPLETE_QUEST_write(writer, &opcodes->body.CMSG_QUESTGIVER_COMPLETE_QUEST));
            break;
        case CMSG_QUESTGIVER_REQUEST_REWARD:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_QUESTGIVER_REQUEST_REWARD_write(writer, &opcodes->body.CMSG_QUESTGIVER_REQUEST_REWARD));
            break;
        case CMSG_QUESTGIVER_CHOOSE_REWARD:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_QUESTGIVER_CHOOSE_REWARD_write(writer, &opcodes->body.CMSG_QUESTGIVER_CHOOSE_REWARD));
            break;
        case CMSG_QUESTLOG_SWAP_QUEST:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_QUESTLOG_SWAP_QUEST_write(writer, &opcodes->body.CMSG_QUESTLOG_SWAP_QUEST));
            break;
        case CMSG_QUESTLOG_REMOVE_QUEST:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_QUESTLOG_REMOVE_QUEST_write(writer, &opcodes->body.CMSG_QUESTLOG_REMOVE_QUEST));
            break;
        case CMSG_QUEST_CONFIRM_ACCEPT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_QUEST_CONFIRM_ACCEPT_write(writer, &opcodes->body.CMSG_QUEST_CONFIRM_ACCEPT));
            break;
        case CMSG_PUSHQUESTTOPARTY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PUSHQUESTTOPARTY_write(writer, &opcodes->body.CMSG_PUSHQUESTTOPARTY));
            break;
        case CMSG_LIST_INVENTORY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_LIST_INVENTORY_write(writer, &opcodes->body.CMSG_LIST_INVENTORY));
            break;
        case CMSG_SELL_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SELL_ITEM_write(writer, &opcodes->body.CMSG_SELL_ITEM));
            break;
        case CMSG_BUY_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_BUY_ITEM_write(writer, &opcodes->body.CMSG_BUY_ITEM));
            break;
        case CMSG_BUY_ITEM_IN_SLOT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_BUY_ITEM_IN_SLOT_write(writer, &opcodes->body.CMSG_BUY_ITEM_IN_SLOT));
            break;
        case CMSG_TAXINODE_STATUS_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_TAXINODE_STATUS_QUERY_write(writer, &opcodes->body.CMSG_TAXINODE_STATUS_QUERY));
            break;
        case CMSG_TAXIQUERYAVAILABLENODES:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_TAXIQUERYAVAILABLENODES_write(writer, &opcodes->body.CMSG_TAXIQUERYAVAILABLENODES));
            break;
        case CMSG_ACTIVATETAXI:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_ACTIVATETAXI_write(writer, &opcodes->body.CMSG_ACTIVATETAXI));
            break;
        case CMSG_TRAINER_LIST:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_TRAINER_LIST_write(writer, &opcodes->body.CMSG_TRAINER_LIST));
            break;
        case CMSG_TRAINER_BUY_SPELL:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_TRAINER_BUY_SPELL_write(writer, &opcodes->body.CMSG_TRAINER_BUY_SPELL));
            break;
        case CMSG_BINDER_ACTIVATE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_BINDER_ACTIVATE_write(writer, &opcodes->body.CMSG_BINDER_ACTIVATE));
            break;
        case CMSG_BANKER_ACTIVATE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_BANKER_ACTIVATE_write(writer, &opcodes->body.CMSG_BANKER_ACTIVATE));
            break;
        case CMSG_BUY_BANK_SLOT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_BUY_BANK_SLOT_write(writer, &opcodes->body.CMSG_BUY_BANK_SLOT));
            break;
        case CMSG_PETITION_SHOWLIST:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PETITION_SHOWLIST_write(writer, &opcodes->body.CMSG_PETITION_SHOWLIST));
            break;
        case CMSG_PETITION_BUY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PETITION_BUY_write(writer, &opcodes->body.CMSG_PETITION_BUY));
            break;
        case CMSG_PETITION_SHOW_SIGNATURES:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PETITION_SHOW_SIGNATURES_write(writer, &opcodes->body.CMSG_PETITION_SHOW_SIGNATURES));
            break;
        case CMSG_PETITION_SIGN:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PETITION_SIGN_write(writer, &opcodes->body.CMSG_PETITION_SIGN));
            break;
        case MSG_PETITION_DECLINE:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_PETITION_DECLINE_cmsg_write(writer, &opcodes->body.MSG_PETITION_DECLINE));
            break;
        case CMSG_OFFER_PETITION:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_OFFER_PETITION_write(writer, &opcodes->body.CMSG_OFFER_PETITION));
            break;
        case CMSG_TURN_IN_PETITION:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_TURN_IN_PETITION_write(writer, &opcodes->body.CMSG_TURN_IN_PETITION));
            break;
        case CMSG_PETITION_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PETITION_QUERY_write(writer, &opcodes->body.CMSG_PETITION_QUERY));
            break;
        case CMSG_BUG:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_BUG_write(writer, &opcodes->body.CMSG_BUG));
            break;
        case CMSG_RECLAIM_CORPSE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_RECLAIM_CORPSE_write(writer, &opcodes->body.CMSG_RECLAIM_CORPSE));
            break;
        case CMSG_WRAP_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_WRAP_ITEM_write(writer, &opcodes->body.CMSG_WRAP_ITEM));
            break;
        case MSG_MINIMAP_PING:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MINIMAP_PING_Client_write(writer, &opcodes->body.MSG_MINIMAP_PING_Client));
            break;
        case CMSG_PING:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PING_write(writer, &opcodes->body.CMSG_PING));
            break;
        case CMSG_SETSHEATHED:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SETSHEATHED_write(writer, &opcodes->body.CMSG_SETSHEATHED));
            break;
        case CMSG_AUTH_SESSION:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AUTH_SESSION_write(writer, &opcodes->body.CMSG_AUTH_SESSION));
            break;
        case CMSG_PET_CAST_SPELL:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PET_CAST_SPELL_write(writer, &opcodes->body.CMSG_PET_CAST_SPELL));
            break;
        case MSG_SAVE_GUILD_EMBLEM:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_SAVE_GUILD_EMBLEM_Client_write(writer, &opcodes->body.MSG_SAVE_GUILD_EMBLEM_Client));
            break;
        case MSG_TABARDVENDOR_ACTIVATE:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_TABARDVENDOR_ACTIVATE_cmsg_write(writer, &opcodes->body.MSG_TABARDVENDOR_ACTIVATE));
            break;
        case CMSG_ZONEUPDATE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_ZONEUPDATE_write(writer, &opcodes->body.CMSG_ZONEUPDATE));
            break;
        case MSG_RANDOM_ROLL:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_RANDOM_ROLL_Client_write(writer, &opcodes->body.MSG_RANDOM_ROLL_Client));
            break;
        case CMSG_UNLEARN_SKILL:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_UNLEARN_SKILL_write(writer, &opcodes->body.CMSG_UNLEARN_SKILL));
            break;
        case CMSG_GMTICKET_CREATE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GMTICKET_CREATE_write(writer, &opcodes->body.CMSG_GMTICKET_CREATE));
            break;
        case CMSG_GMTICKET_UPDATETEXT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GMTICKET_UPDATETEXT_write(writer, &opcodes->body.CMSG_GMTICKET_UPDATETEXT));
            break;
        case CMSG_REQUEST_ACCOUNT_DATA:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_REQUEST_ACCOUNT_DATA_write(writer, &opcodes->body.CMSG_REQUEST_ACCOUNT_DATA));
            break;
        case CMSG_UPDATE_ACCOUNT_DATA:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_UPDATE_ACCOUNT_DATA_write(writer, &opcodes->body.CMSG_UPDATE_ACCOUNT_DATA));
            break;
        case CMSG_SPIRIT_HEALER_ACTIVATE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SPIRIT_HEALER_ACTIVATE_write(writer, &opcodes->body.CMSG_SPIRIT_HEALER_ACTIVATE));
            break;
        case CMSG_CHAT_IGNORED:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHAT_IGNORED_write(writer, &opcodes->body.CMSG_CHAT_IGNORED));
            break;
        case CMSG_GUILD_RANK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GUILD_RANK_write(writer, &opcodes->body.CMSG_GUILD_RANK));
            break;
        case CMSG_GUILD_ADD_RANK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GUILD_ADD_RANK_write(writer, &opcodes->body.CMSG_GUILD_ADD_RANK));
            break;
        case CMSG_GUILD_SET_PUBLIC_NOTE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GUILD_SET_PUBLIC_NOTE_write(writer, &opcodes->body.CMSG_GUILD_SET_PUBLIC_NOTE));
            break;
        case CMSG_GUILD_SET_OFFICER_NOTE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GUILD_SET_OFFICER_NOTE_write(writer, &opcodes->body.CMSG_GUILD_SET_OFFICER_NOTE));
            break;
        case CMSG_SEND_MAIL:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SEND_MAIL_write(writer, &opcodes->body.CMSG_SEND_MAIL));
            break;
        case CMSG_GET_MAIL_LIST:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GET_MAIL_LIST_write(writer, &opcodes->body.CMSG_GET_MAIL_LIST));
            break;
        case CMSG_BATTLEFIELD_LIST:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_BATTLEFIELD_LIST_write(writer, &opcodes->body.CMSG_BATTLEFIELD_LIST));
            break;
        case CMSG_BATTLEFIELD_JOIN:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_BATTLEFIELD_JOIN_write(writer, &opcodes->body.CMSG_BATTLEFIELD_JOIN));
            break;
        case CMSG_ITEM_TEXT_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_ITEM_TEXT_QUERY_write(writer, &opcodes->body.CMSG_ITEM_TEXT_QUERY));
            break;
        case CMSG_MAIL_TAKE_MONEY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MAIL_TAKE_MONEY_write(writer, &opcodes->body.CMSG_MAIL_TAKE_MONEY));
            break;
        case CMSG_MAIL_TAKE_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MAIL_TAKE_ITEM_write(writer, &opcodes->body.CMSG_MAIL_TAKE_ITEM));
            break;
        case CMSG_MAIL_MARK_AS_READ:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MAIL_MARK_AS_READ_write(writer, &opcodes->body.CMSG_MAIL_MARK_AS_READ));
            break;
        case CMSG_MAIL_RETURN_TO_SENDER:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MAIL_RETURN_TO_SENDER_write(writer, &opcodes->body.CMSG_MAIL_RETURN_TO_SENDER));
            break;
        case CMSG_MAIL_DELETE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MAIL_DELETE_write(writer, &opcodes->body.CMSG_MAIL_DELETE));
            break;
        case CMSG_MAIL_CREATE_TEXT_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MAIL_CREATE_TEXT_ITEM_write(writer, &opcodes->body.CMSG_MAIL_CREATE_TEXT_ITEM));
            break;
        case CMSG_LEARN_TALENT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_LEARN_TALENT_write(writer, &opcodes->body.CMSG_LEARN_TALENT));
            break;
        case CMSG_TOGGLE_PVP:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_TOGGLE_PVP_write(writer, &opcodes->body.CMSG_TOGGLE_PVP));
            break;
        case MSG_AUCTION_HELLO:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_AUCTION_HELLO_Client_write(writer, &opcodes->body.MSG_AUCTION_HELLO_Client));
            break;
        case CMSG_AUCTION_SELL_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AUCTION_SELL_ITEM_write(writer, &opcodes->body.CMSG_AUCTION_SELL_ITEM));
            break;
        case CMSG_AUCTION_REMOVE_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AUCTION_REMOVE_ITEM_write(writer, &opcodes->body.CMSG_AUCTION_REMOVE_ITEM));
            break;
        case CMSG_AUCTION_LIST_ITEMS:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AUCTION_LIST_ITEMS_write(writer, &opcodes->body.CMSG_AUCTION_LIST_ITEMS));
            break;
        case CMSG_AUCTION_LIST_OWNER_ITEMS:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AUCTION_LIST_OWNER_ITEMS_write(writer, &opcodes->body.CMSG_AUCTION_LIST_OWNER_ITEMS));
            break;
        case CMSG_AUCTION_PLACE_BID:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AUCTION_PLACE_BID_write(writer, &opcodes->body.CMSG_AUCTION_PLACE_BID));
            break;
        case CMSG_AUCTION_LIST_BIDDER_ITEMS:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AUCTION_LIST_BIDDER_ITEMS_write(writer, &opcodes->body.CMSG_AUCTION_LIST_BIDDER_ITEMS));
            break;
        case CMSG_SET_AMMO:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SET_AMMO_write(writer, &opcodes->body.CMSG_SET_AMMO));
            break;
        case CMSG_SET_ACTIVE_MOVER:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SET_ACTIVE_MOVER_write(writer, &opcodes->body.CMSG_SET_ACTIVE_MOVER));
            break;
        case CMSG_PET_CANCEL_AURA:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PET_CANCEL_AURA_write(writer, &opcodes->body.CMSG_PET_CANCEL_AURA));
            break;
        case MSG_LIST_STABLED_PETS:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_LIST_STABLED_PETS_Client_write(writer, &opcodes->body.MSG_LIST_STABLED_PETS_Client));
            break;
        case CMSG_STABLE_PET:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_STABLE_PET_write(writer, &opcodes->body.CMSG_STABLE_PET));
            break;
        case CMSG_UNSTABLE_PET:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_UNSTABLE_PET_write(writer, &opcodes->body.CMSG_UNSTABLE_PET));
            break;
        case CMSG_BUY_STABLE_SLOT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_BUY_STABLE_SLOT_write(writer, &opcodes->body.CMSG_BUY_STABLE_SLOT));
            break;
        case CMSG_STABLE_SWAP_PET:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_STABLE_SWAP_PET_write(writer, &opcodes->body.CMSG_STABLE_SWAP_PET));
            break;
        case MSG_QUEST_PUSH_RESULT:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_QUEST_PUSH_RESULT_cmsg_write(writer, &opcodes->body.MSG_QUEST_PUSH_RESULT));
            break;
        case CMSG_FAR_SIGHT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_FAR_SIGHT_write(writer, &opcodes->body.CMSG_FAR_SIGHT));
            break;
        case CMSG_GROUP_CHANGE_SUB_GROUP:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GROUP_CHANGE_SUB_GROUP_write(writer, &opcodes->body.CMSG_GROUP_CHANGE_SUB_GROUP));
            break;
        case CMSG_REQUEST_PARTY_MEMBER_STATS:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_REQUEST_PARTY_MEMBER_STATS_write(writer, &opcodes->body.CMSG_REQUEST_PARTY_MEMBER_STATS));
            break;
        case CMSG_GROUP_SWAP_SUB_GROUP:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GROUP_SWAP_SUB_GROUP_write(writer, &opcodes->body.CMSG_GROUP_SWAP_SUB_GROUP));
            break;
        case CMSG_AUTOSTORE_BANK_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AUTOSTORE_BANK_ITEM_write(writer, &opcodes->body.CMSG_AUTOSTORE_BANK_ITEM));
            break;
        case CMSG_AUTOBANK_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AUTOBANK_ITEM_write(writer, &opcodes->body.CMSG_AUTOBANK_ITEM));
            break;
        case CMSG_GROUP_ASSISTANT_LEADER:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GROUP_ASSISTANT_LEADER_write(writer, &opcodes->body.CMSG_GROUP_ASSISTANT_LEADER));
            break;
        case CMSG_BUYBACK_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_BUYBACK_ITEM_write(writer, &opcodes->body.CMSG_BUYBACK_ITEM));
            break;
        case CMSG_MEETINGSTONE_JOIN:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MEETINGSTONE_JOIN_write(writer, &opcodes->body.CMSG_MEETINGSTONE_JOIN));
            break;
        case CMSG_LOOT_ROLL:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_LOOT_ROLL_write(writer, &opcodes->body.CMSG_LOOT_ROLL));
            break;
        case CMSG_LOOT_MASTER_GIVE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_LOOT_MASTER_GIVE_write(writer, &opcodes->body.CMSG_LOOT_MASTER_GIVE));
            break;
        case CMSG_REPAIR_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_REPAIR_ITEM_write(writer, &opcodes->body.CMSG_REPAIR_ITEM));
            break;
        case MSG_TALENT_WIPE_CONFIRM:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_TALENT_WIPE_CONFIRM_Client_write(writer, &opcodes->body.MSG_TALENT_WIPE_CONFIRM_Client));
            break;
        case CMSG_SUMMON_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SUMMON_RESPONSE_write(writer, &opcodes->body.CMSG_SUMMON_RESPONSE));
            break;
        case MSG_MOVE_WATER_WALK:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_WATER_WALK_cmsg_write(writer, &opcodes->body.MSG_MOVE_WATER_WALK));
            break;
        case CMSG_SET_ACTIONBAR_TOGGLES:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SET_ACTIONBAR_TOGGLES_write(writer, &opcodes->body.CMSG_SET_ACTIONBAR_TOGGLES));
            break;
        case MSG_PETITION_RENAME:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_PETITION_RENAME_cmsg_write(writer, &opcodes->body.MSG_PETITION_RENAME));
            break;
        case CMSG_ITEM_NAME_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_ITEM_NAME_QUERY_write(writer, &opcodes->body.CMSG_ITEM_NAME_QUERY));
            break;
        case CMSG_CHAR_RENAME:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHAR_RENAME_write(writer, &opcodes->body.CMSG_CHAR_RENAME));
            break;
        case CMSG_MOVE_SPLINE_DONE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MOVE_SPLINE_DONE_write(writer, &opcodes->body.CMSG_MOVE_SPLINE_DONE));
            break;
        case CMSG_MOVE_FALL_RESET:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MOVE_FALL_RESET_write(writer, &opcodes->body.CMSG_MOVE_FALL_RESET));
            break;
        case CMSG_MOVE_TIME_SKIPPED:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MOVE_TIME_SKIPPED_write(writer, &opcodes->body.CMSG_MOVE_TIME_SKIPPED));
            break;
        case CMSG_MOVE_FEATHER_FALL_ACK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MOVE_FEATHER_FALL_ACK_write(writer, &opcodes->body.CMSG_MOVE_FEATHER_FALL_ACK));
            break;
        case CMSG_MOVE_WATER_WALK_ACK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MOVE_WATER_WALK_ACK_write(writer, &opcodes->body.CMSG_MOVE_WATER_WALK_ACK));
            break;
        case CMSG_MOVE_NOT_ACTIVE_MOVER:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MOVE_NOT_ACTIVE_MOVER_write(writer, &opcodes->body.CMSG_MOVE_NOT_ACTIVE_MOVER));
            break;
        case CMSG_BATTLEFIELD_PORT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_BATTLEFIELD_PORT_write(writer, &opcodes->body.CMSG_BATTLEFIELD_PORT));
            break;
        case MSG_INSPECT_HONOR_STATS:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_INSPECT_HONOR_STATS_Client_write(writer, &opcodes->body.MSG_INSPECT_HONOR_STATS_Client));
            break;
        case CMSG_BATTLEMASTER_HELLO:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_BATTLEMASTER_HELLO_write(writer, &opcodes->body.CMSG_BATTLEMASTER_HELLO));
            break;
        case CMSG_FORCE_WALK_SPEED_CHANGE_ACK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_FORCE_WALK_SPEED_CHANGE_ACK_write(writer, &opcodes->body.CMSG_FORCE_WALK_SPEED_CHANGE_ACK));
            break;
        case CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK_write(writer, &opcodes->body.CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK));
            break;
        case CMSG_FORCE_TURN_RATE_CHANGE_ACK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_FORCE_TURN_RATE_CHANGE_ACK_write(writer, &opcodes->body.CMSG_FORCE_TURN_RATE_CHANGE_ACK));
            break;
        case CMSG_LEAVE_BATTLEFIELD:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_LEAVE_BATTLEFIELD_write(writer, &opcodes->body.CMSG_LEAVE_BATTLEFIELD));
            break;
        case CMSG_AREA_SPIRIT_HEALER_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AREA_SPIRIT_HEALER_QUERY_write(writer, &opcodes->body.CMSG_AREA_SPIRIT_HEALER_QUERY));
            break;
        case CMSG_AREA_SPIRIT_HEALER_QUEUE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AREA_SPIRIT_HEALER_QUEUE_write(writer, &opcodes->body.CMSG_AREA_SPIRIT_HEALER_QUEUE));
            break;
        case CMSG_WARDEN_DATA:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_WARDEN_DATA_write(writer, &opcodes->body.CMSG_WARDEN_DATA));
            break;
        case CMSG_PET_STOP_ATTACK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PET_STOP_ATTACK_write(writer, &opcodes->body.CMSG_PET_STOP_ATTACK));
            break;
        case CMSG_BATTLEMASTER_JOIN:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_BATTLEMASTER_JOIN_write(writer, &opcodes->body.CMSG_BATTLEMASTER_JOIN));
            break;
        case CMSG_PET_UNLEARN:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PET_UNLEARN_write(writer, &opcodes->body.CMSG_PET_UNLEARN));
            break;
        case CMSG_PET_SPELL_AUTOCAST:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PET_SPELL_AUTOCAST_write(writer, &opcodes->body.CMSG_PET_SPELL_AUTOCAST));
            break;
        case CMSG_GUILD_INFO_TEXT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GUILD_INFO_TEXT_write(writer, &opcodes->body.CMSG_GUILD_INFO_TEXT));
            break;
        case CMSG_ACTIVATETAXIEXPRESS:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_ACTIVATETAXIEXPRESS_write(writer, &opcodes->body.CMSG_ACTIVATETAXIEXPRESS));
            break;
        case CMSG_SET_FACTION_INACTIVE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SET_FACTION_INACTIVE_write(writer, &opcodes->body.CMSG_SET_FACTION_INACTIVE));
            break;
        case CMSG_SET_WATCHED_FACTION:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SET_WATCHED_FACTION_write(writer, &opcodes->body.CMSG_SET_WATCHED_FACTION));
            break;
        case MSG_RAID_TARGET_UPDATE:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_RAID_TARGET_UPDATE_Client_write(writer, &opcodes->body.MSG_RAID_TARGET_UPDATE_Client));
            break;
        case MSG_RAID_READY_CHECK:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_RAID_READY_CHECK_Client_write(writer, &opcodes->body.MSG_RAID_READY_CHECK_Client));
            break;
        case CMSG_GMSURVEY_SUBMIT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GMSURVEY_SUBMIT_write(writer, &opcodes->body.CMSG_GMSURVEY_SUBMIT));
            break;
        default:
            break;
    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_client_opcode_read(WowWorldReader* reader, VanillaClientOpcodeContainer* opcodes) {
    uint16_t _size;
    READ_U16_BE(_size);

    opcodes->opcode = 0;
    READ_U32(opcodes->opcode);

    switch (opcodes->opcode) {
        case CMSG_DBLOOKUP:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_DBLOOKUP_read(reader, &opcodes->body.CMSG_DBLOOKUP));
            break;
        case CMSG_WORLD_TELEPORT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_WORLD_TELEPORT_read(reader, &opcodes->body.CMSG_WORLD_TELEPORT));
            break;
        case CMSG_TELEPORT_TO_UNIT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_TELEPORT_TO_UNIT_read(reader, &opcodes->body.CMSG_TELEPORT_TO_UNIT));
            break;
        case CMSG_CHAR_CREATE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHAR_CREATE_read(reader, &opcodes->body.CMSG_CHAR_CREATE));
            break;
        case CMSG_CHAR_DELETE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHAR_DELETE_read(reader, &opcodes->body.CMSG_CHAR_DELETE));
            break;
        case CMSG_PLAYER_LOGIN:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PLAYER_LOGIN_read(reader, &opcodes->body.CMSG_PLAYER_LOGIN));
            break;
        case CMSG_NAME_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_NAME_QUERY_read(reader, &opcodes->body.CMSG_NAME_QUERY));
            break;
        case CMSG_PET_NAME_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PET_NAME_QUERY_read(reader, &opcodes->body.CMSG_PET_NAME_QUERY));
            break;
        case CMSG_GUILD_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GUILD_QUERY_read(reader, &opcodes->body.CMSG_GUILD_QUERY));
            break;
        case CMSG_ITEM_QUERY_SINGLE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_ITEM_QUERY_SINGLE_read(reader, &opcodes->body.CMSG_ITEM_QUERY_SINGLE));
            break;
        case CMSG_PAGE_TEXT_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PAGE_TEXT_QUERY_read(reader, &opcodes->body.CMSG_PAGE_TEXT_QUERY));
            break;
        case CMSG_QUEST_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_QUEST_QUERY_read(reader, &opcodes->body.CMSG_QUEST_QUERY));
            break;
        case CMSG_GAMEOBJECT_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GAMEOBJECT_QUERY_read(reader, &opcodes->body.CMSG_GAMEOBJECT_QUERY));
            break;
        case CMSG_CREATURE_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CREATURE_QUERY_read(reader, &opcodes->body.CMSG_CREATURE_QUERY));
            break;
        case CMSG_WHO:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_WHO_read(reader, &opcodes->body.CMSG_WHO));
            break;
        case CMSG_WHOIS:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_WHOIS_read(reader, &opcodes->body.CMSG_WHOIS));
            break;
        case CMSG_ADD_FRIEND:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_ADD_FRIEND_read(reader, &opcodes->body.CMSG_ADD_FRIEND));
            break;
        case CMSG_DEL_FRIEND:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_DEL_FRIEND_read(reader, &opcodes->body.CMSG_DEL_FRIEND));
            break;
        case CMSG_ADD_IGNORE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_ADD_IGNORE_read(reader, &opcodes->body.CMSG_ADD_IGNORE));
            break;
        case CMSG_DEL_IGNORE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_DEL_IGNORE_read(reader, &opcodes->body.CMSG_DEL_IGNORE));
            break;
        case CMSG_GROUP_INVITE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GROUP_INVITE_read(reader, &opcodes->body.CMSG_GROUP_INVITE));
            break;
        case CMSG_GROUP_UNINVITE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GROUP_UNINVITE_read(reader, &opcodes->body.CMSG_GROUP_UNINVITE));
            break;
        case CMSG_GROUP_UNINVITE_GUID:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GROUP_UNINVITE_GUID_read(reader, &opcodes->body.CMSG_GROUP_UNINVITE_GUID));
            break;
        case CMSG_GROUP_SET_LEADER:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GROUP_SET_LEADER_read(reader, &opcodes->body.CMSG_GROUP_SET_LEADER));
            break;
        case CMSG_LOOT_METHOD:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_LOOT_METHOD_read(reader, &opcodes->body.CMSG_LOOT_METHOD));
            break;
        case CMSG_GUILD_CREATE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GUILD_CREATE_read(reader, &opcodes->body.CMSG_GUILD_CREATE));
            break;
        case CMSG_GUILD_INVITE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GUILD_INVITE_read(reader, &opcodes->body.CMSG_GUILD_INVITE));
            break;
        case CMSG_GUILD_PROMOTE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GUILD_PROMOTE_read(reader, &opcodes->body.CMSG_GUILD_PROMOTE));
            break;
        case CMSG_GUILD_DEMOTE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GUILD_DEMOTE_read(reader, &opcodes->body.CMSG_GUILD_DEMOTE));
            break;
        case CMSG_GUILD_REMOVE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GUILD_REMOVE_read(reader, &opcodes->body.CMSG_GUILD_REMOVE));
            break;
        case CMSG_GUILD_LEADER:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GUILD_LEADER_read(reader, &opcodes->body.CMSG_GUILD_LEADER));
            break;
        case CMSG_GUILD_MOTD:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GUILD_MOTD_read(reader, &opcodes->body.CMSG_GUILD_MOTD));
            break;
        case CMSG_MESSAGECHAT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MESSAGECHAT_read(reader, &opcodes->body.CMSG_MESSAGECHAT));
            break;
        case CMSG_JOIN_CHANNEL:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_JOIN_CHANNEL_read(reader, &opcodes->body.CMSG_JOIN_CHANNEL));
            break;
        case CMSG_LEAVE_CHANNEL:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_LEAVE_CHANNEL_read(reader, &opcodes->body.CMSG_LEAVE_CHANNEL));
            break;
        case CMSG_CHANNEL_LIST:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHANNEL_LIST_read(reader, &opcodes->body.CMSG_CHANNEL_LIST));
            break;
        case CMSG_CHANNEL_PASSWORD:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHANNEL_PASSWORD_read(reader, &opcodes->body.CMSG_CHANNEL_PASSWORD));
            break;
        case CMSG_CHANNEL_SET_OWNER:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHANNEL_SET_OWNER_read(reader, &opcodes->body.CMSG_CHANNEL_SET_OWNER));
            break;
        case CMSG_CHANNEL_OWNER:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHANNEL_OWNER_read(reader, &opcodes->body.CMSG_CHANNEL_OWNER));
            break;
        case CMSG_CHANNEL_MODERATOR:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHANNEL_MODERATOR_read(reader, &opcodes->body.CMSG_CHANNEL_MODERATOR));
            break;
        case CMSG_CHANNEL_UNMODERATOR:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHANNEL_UNMODERATOR_read(reader, &opcodes->body.CMSG_CHANNEL_UNMODERATOR));
            break;
        case CMSG_CHANNEL_MUTE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHANNEL_MUTE_read(reader, &opcodes->body.CMSG_CHANNEL_MUTE));
            break;
        case CMSG_CHANNEL_UNMUTE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHANNEL_UNMUTE_read(reader, &opcodes->body.CMSG_CHANNEL_UNMUTE));
            break;
        case CMSG_CHANNEL_INVITE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHANNEL_INVITE_read(reader, &opcodes->body.CMSG_CHANNEL_INVITE));
            break;
        case CMSG_CHANNEL_KICK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHANNEL_KICK_read(reader, &opcodes->body.CMSG_CHANNEL_KICK));
            break;
        case CMSG_CHANNEL_BAN:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHANNEL_BAN_read(reader, &opcodes->body.CMSG_CHANNEL_BAN));
            break;
        case CMSG_CHANNEL_UNBAN:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHANNEL_UNBAN_read(reader, &opcodes->body.CMSG_CHANNEL_UNBAN));
            break;
        case CMSG_CHANNEL_ANNOUNCEMENTS:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHANNEL_ANNOUNCEMENTS_read(reader, &opcodes->body.CMSG_CHANNEL_ANNOUNCEMENTS));
            break;
        case CMSG_CHANNEL_MODERATE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHANNEL_MODERATE_read(reader, &opcodes->body.CMSG_CHANNEL_MODERATE));
            break;
        case CMSG_USE_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_USE_ITEM_read(reader, &opcodes->body.CMSG_USE_ITEM));
            break;
        case CMSG_OPEN_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_OPEN_ITEM_read(reader, &opcodes->body.CMSG_OPEN_ITEM));
            break;
        case CMSG_READ_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_READ_ITEM_read(reader, &opcodes->body.CMSG_READ_ITEM));
            break;
        case CMSG_GAMEOBJ_USE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GAMEOBJ_USE_read(reader, &opcodes->body.CMSG_GAMEOBJ_USE));
            break;
        case CMSG_AREATRIGGER:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AREATRIGGER_read(reader, &opcodes->body.CMSG_AREATRIGGER));
            break;
        case MSG_MOVE_START_FORWARD:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_FORWARD_Client_read(reader, &opcodes->body.MSG_MOVE_START_FORWARD_Client));
            break;
        case MSG_MOVE_START_BACKWARD:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_BACKWARD_Client_read(reader, &opcodes->body.MSG_MOVE_START_BACKWARD_Client));
            break;
        case MSG_MOVE_STOP:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_STOP_Client_read(reader, &opcodes->body.MSG_MOVE_STOP_Client));
            break;
        case MSG_MOVE_START_STRAFE_LEFT:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_STRAFE_LEFT_Client_read(reader, &opcodes->body.MSG_MOVE_START_STRAFE_LEFT_Client));
            break;
        case MSG_MOVE_START_STRAFE_RIGHT:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_STRAFE_RIGHT_Client_read(reader, &opcodes->body.MSG_MOVE_START_STRAFE_RIGHT_Client));
            break;
        case MSG_MOVE_STOP_STRAFE:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_STOP_STRAFE_Client_read(reader, &opcodes->body.MSG_MOVE_STOP_STRAFE_Client));
            break;
        case MSG_MOVE_JUMP:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_JUMP_Client_read(reader, &opcodes->body.MSG_MOVE_JUMP_Client));
            break;
        case MSG_MOVE_START_TURN_LEFT:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_TURN_LEFT_Client_read(reader, &opcodes->body.MSG_MOVE_START_TURN_LEFT_Client));
            break;
        case MSG_MOVE_START_TURN_RIGHT:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_TURN_RIGHT_Client_read(reader, &opcodes->body.MSG_MOVE_START_TURN_RIGHT_Client));
            break;
        case MSG_MOVE_STOP_TURN:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_STOP_TURN_Client_read(reader, &opcodes->body.MSG_MOVE_STOP_TURN_Client));
            break;
        case MSG_MOVE_START_PITCH_UP:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_PITCH_UP_Client_read(reader, &opcodes->body.MSG_MOVE_START_PITCH_UP_Client));
            break;
        case MSG_MOVE_START_PITCH_DOWN:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_PITCH_DOWN_Client_read(reader, &opcodes->body.MSG_MOVE_START_PITCH_DOWN_Client));
            break;
        case MSG_MOVE_STOP_PITCH:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_STOP_PITCH_Client_read(reader, &opcodes->body.MSG_MOVE_STOP_PITCH_Client));
            break;
        case MSG_MOVE_SET_RUN_MODE:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_SET_RUN_MODE_Client_read(reader, &opcodes->body.MSG_MOVE_SET_RUN_MODE_Client));
            break;
        case MSG_MOVE_SET_WALK_MODE:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_SET_WALK_MODE_Client_read(reader, &opcodes->body.MSG_MOVE_SET_WALK_MODE_Client));
            break;
        case MSG_MOVE_TELEPORT_ACK:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_TELEPORT_ACK_Client_read(reader, &opcodes->body.MSG_MOVE_TELEPORT_ACK_Client));
            break;
        case MSG_MOVE_FALL_LAND:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_FALL_LAND_Client_read(reader, &opcodes->body.MSG_MOVE_FALL_LAND_Client));
            break;
        case MSG_MOVE_START_SWIM:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_SWIM_Client_read(reader, &opcodes->body.MSG_MOVE_START_SWIM_Client));
            break;
        case MSG_MOVE_STOP_SWIM:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_STOP_SWIM_Client_read(reader, &opcodes->body.MSG_MOVE_STOP_SWIM_Client));
            break;
        case MSG_MOVE_SET_FACING:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_SET_FACING_Client_read(reader, &opcodes->body.MSG_MOVE_SET_FACING_Client));
            break;
        case MSG_MOVE_SET_PITCH:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_SET_PITCH_Client_read(reader, &opcodes->body.MSG_MOVE_SET_PITCH_Client));
            break;
        case CMSG_MOVE_SET_RAW_POSITION:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MOVE_SET_RAW_POSITION_read(reader, &opcodes->body.CMSG_MOVE_SET_RAW_POSITION));
            break;
        case CMSG_FORCE_RUN_SPEED_CHANGE_ACK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_FORCE_RUN_SPEED_CHANGE_ACK_read(reader, &opcodes->body.CMSG_FORCE_RUN_SPEED_CHANGE_ACK));
            break;
        case CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK_read(reader, &opcodes->body.CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK));
            break;
        case CMSG_FORCE_SWIM_SPEED_CHANGE_ACK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_FORCE_SWIM_SPEED_CHANGE_ACK_read(reader, &opcodes->body.CMSG_FORCE_SWIM_SPEED_CHANGE_ACK));
            break;
        case CMSG_FORCE_MOVE_ROOT_ACK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_FORCE_MOVE_ROOT_ACK_read(reader, &opcodes->body.CMSG_FORCE_MOVE_ROOT_ACK));
            break;
        case CMSG_FORCE_MOVE_UNROOT_ACK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_FORCE_MOVE_UNROOT_ACK_read(reader, &opcodes->body.CMSG_FORCE_MOVE_UNROOT_ACK));
            break;
        case MSG_MOVE_HEARTBEAT:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_HEARTBEAT_Client_read(reader, &opcodes->body.MSG_MOVE_HEARTBEAT_Client));
            break;
        case CMSG_MOVE_KNOCK_BACK_ACK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MOVE_KNOCK_BACK_ACK_read(reader, &opcodes->body.CMSG_MOVE_KNOCK_BACK_ACK));
            break;
        case CMSG_MOVE_HOVER_ACK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MOVE_HOVER_ACK_read(reader, &opcodes->body.CMSG_MOVE_HOVER_ACK));
            break;
        case CMSG_TUTORIAL_FLAG:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_TUTORIAL_FLAG_read(reader, &opcodes->body.CMSG_TUTORIAL_FLAG));
            break;
        case CMSG_STANDSTATECHANGE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_STANDSTATECHANGE_read(reader, &opcodes->body.CMSG_STANDSTATECHANGE));
            break;
        case CMSG_EMOTE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_EMOTE_read(reader, &opcodes->body.CMSG_EMOTE));
            break;
        case CMSG_TEXT_EMOTE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_TEXT_EMOTE_read(reader, &opcodes->body.CMSG_TEXT_EMOTE));
            break;
        case CMSG_AUTOSTORE_LOOT_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AUTOSTORE_LOOT_ITEM_read(reader, &opcodes->body.CMSG_AUTOSTORE_LOOT_ITEM));
            break;
        case CMSG_AUTOEQUIP_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AUTOEQUIP_ITEM_read(reader, &opcodes->body.CMSG_AUTOEQUIP_ITEM));
            break;
        case CMSG_AUTOSTORE_BAG_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AUTOSTORE_BAG_ITEM_read(reader, &opcodes->body.CMSG_AUTOSTORE_BAG_ITEM));
            break;
        case CMSG_SWAP_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SWAP_ITEM_read(reader, &opcodes->body.CMSG_SWAP_ITEM));
            break;
        case CMSG_SWAP_INV_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SWAP_INV_ITEM_read(reader, &opcodes->body.CMSG_SWAP_INV_ITEM));
            break;
        case CMSG_SPLIT_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SPLIT_ITEM_read(reader, &opcodes->body.CMSG_SPLIT_ITEM));
            break;
        case CMSG_AUTOEQUIP_ITEM_SLOT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AUTOEQUIP_ITEM_SLOT_read(reader, &opcodes->body.CMSG_AUTOEQUIP_ITEM_SLOT));
            break;
        case CMSG_DESTROYITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_DESTROYITEM_read(reader, &opcodes->body.CMSG_DESTROYITEM));
            break;
        case CMSG_INSPECT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_INSPECT_read(reader, &opcodes->body.CMSG_INSPECT));
            break;
        case CMSG_INITIATE_TRADE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_INITIATE_TRADE_read(reader, &opcodes->body.CMSG_INITIATE_TRADE));
            break;
        case CMSG_ACCEPT_TRADE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_ACCEPT_TRADE_read(reader, &opcodes->body.CMSG_ACCEPT_TRADE));
            break;
        case CMSG_SET_TRADE_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SET_TRADE_ITEM_read(reader, &opcodes->body.CMSG_SET_TRADE_ITEM));
            break;
        case CMSG_CLEAR_TRADE_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CLEAR_TRADE_ITEM_read(reader, &opcodes->body.CMSG_CLEAR_TRADE_ITEM));
            break;
        case CMSG_SET_TRADE_GOLD:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SET_TRADE_GOLD_read(reader, &opcodes->body.CMSG_SET_TRADE_GOLD));
            break;
        case CMSG_SET_FACTION_ATWAR:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SET_FACTION_ATWAR_read(reader, &opcodes->body.CMSG_SET_FACTION_ATWAR));
            break;
        case CMSG_SET_ACTION_BUTTON:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SET_ACTION_BUTTON_read(reader, &opcodes->body.CMSG_SET_ACTION_BUTTON));
            break;
        case CMSG_CAST_SPELL:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CAST_SPELL_read(reader, &opcodes->body.CMSG_CAST_SPELL));
            break;
        case CMSG_CANCEL_CAST:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CANCEL_CAST_read(reader, &opcodes->body.CMSG_CANCEL_CAST));
            break;
        case CMSG_CANCEL_AURA:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CANCEL_AURA_read(reader, &opcodes->body.CMSG_CANCEL_AURA));
            break;
        case CMSG_CANCEL_CHANNELLING:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CANCEL_CHANNELLING_read(reader, &opcodes->body.CMSG_CANCEL_CHANNELLING));
            break;
        case CMSG_SET_SELECTION:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SET_SELECTION_read(reader, &opcodes->body.CMSG_SET_SELECTION));
            break;
        case CMSG_SET_TARGET_OBSOLETE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SET_TARGET_OBSOLETE_read(reader, &opcodes->body.CMSG_SET_TARGET_OBSOLETE));
            break;
        case CMSG_ATTACKSWING:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_ATTACKSWING_read(reader, &opcodes->body.CMSG_ATTACKSWING));
            break;
        case CMSG_RESURRECT_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_RESURRECT_RESPONSE_read(reader, &opcodes->body.CMSG_RESURRECT_RESPONSE));
            break;
        case CMSG_LOOT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_LOOT_read(reader, &opcodes->body.CMSG_LOOT));
            break;
        case CMSG_LOOT_RELEASE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_LOOT_RELEASE_read(reader, &opcodes->body.CMSG_LOOT_RELEASE));
            break;
        case CMSG_DUEL_ACCEPTED:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_DUEL_ACCEPTED_read(reader, &opcodes->body.CMSG_DUEL_ACCEPTED));
            break;
        case CMSG_DUEL_CANCELLED:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_DUEL_CANCELLED_read(reader, &opcodes->body.CMSG_DUEL_CANCELLED));
            break;
        case CMSG_PET_SET_ACTION:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PET_SET_ACTION_read(reader, &opcodes->body.CMSG_PET_SET_ACTION, _size - 4));
            break;
        case CMSG_PET_ACTION:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PET_ACTION_read(reader, &opcodes->body.CMSG_PET_ACTION));
            break;
        case CMSG_PET_ABANDON:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PET_ABANDON_read(reader, &opcodes->body.CMSG_PET_ABANDON));
            break;
        case CMSG_PET_RENAME:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PET_RENAME_read(reader, &opcodes->body.CMSG_PET_RENAME));
            break;
        case CMSG_GOSSIP_HELLO:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GOSSIP_HELLO_read(reader, &opcodes->body.CMSG_GOSSIP_HELLO));
            break;
        case CMSG_GOSSIP_SELECT_OPTION:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GOSSIP_SELECT_OPTION_read(reader, &opcodes->body.CMSG_GOSSIP_SELECT_OPTION, _size - 4));
            break;
        case CMSG_NPC_TEXT_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_NPC_TEXT_QUERY_read(reader, &opcodes->body.CMSG_NPC_TEXT_QUERY));
            break;
        case CMSG_QUESTGIVER_STATUS_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_QUESTGIVER_STATUS_QUERY_read(reader, &opcodes->body.CMSG_QUESTGIVER_STATUS_QUERY));
            break;
        case CMSG_QUESTGIVER_HELLO:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_QUESTGIVER_HELLO_read(reader, &opcodes->body.CMSG_QUESTGIVER_HELLO));
            break;
        case CMSG_QUESTGIVER_QUERY_QUEST:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_QUESTGIVER_QUERY_QUEST_read(reader, &opcodes->body.CMSG_QUESTGIVER_QUERY_QUEST));
            break;
        case CMSG_QUESTGIVER_ACCEPT_QUEST:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_QUESTGIVER_ACCEPT_QUEST_read(reader, &opcodes->body.CMSG_QUESTGIVER_ACCEPT_QUEST));
            break;
        case CMSG_QUESTGIVER_COMPLETE_QUEST:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_QUESTGIVER_COMPLETE_QUEST_read(reader, &opcodes->body.CMSG_QUESTGIVER_COMPLETE_QUEST));
            break;
        case CMSG_QUESTGIVER_REQUEST_REWARD:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_QUESTGIVER_REQUEST_REWARD_read(reader, &opcodes->body.CMSG_QUESTGIVER_REQUEST_REWARD));
            break;
        case CMSG_QUESTGIVER_CHOOSE_REWARD:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_QUESTGIVER_CHOOSE_REWARD_read(reader, &opcodes->body.CMSG_QUESTGIVER_CHOOSE_REWARD));
            break;
        case CMSG_QUESTLOG_SWAP_QUEST:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_QUESTLOG_SWAP_QUEST_read(reader, &opcodes->body.CMSG_QUESTLOG_SWAP_QUEST));
            break;
        case CMSG_QUESTLOG_REMOVE_QUEST:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_QUESTLOG_REMOVE_QUEST_read(reader, &opcodes->body.CMSG_QUESTLOG_REMOVE_QUEST));
            break;
        case CMSG_QUEST_CONFIRM_ACCEPT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_QUEST_CONFIRM_ACCEPT_read(reader, &opcodes->body.CMSG_QUEST_CONFIRM_ACCEPT));
            break;
        case CMSG_PUSHQUESTTOPARTY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PUSHQUESTTOPARTY_read(reader, &opcodes->body.CMSG_PUSHQUESTTOPARTY));
            break;
        case CMSG_LIST_INVENTORY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_LIST_INVENTORY_read(reader, &opcodes->body.CMSG_LIST_INVENTORY));
            break;
        case CMSG_SELL_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SELL_ITEM_read(reader, &opcodes->body.CMSG_SELL_ITEM));
            break;
        case CMSG_BUY_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_BUY_ITEM_read(reader, &opcodes->body.CMSG_BUY_ITEM));
            break;
        case CMSG_BUY_ITEM_IN_SLOT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_BUY_ITEM_IN_SLOT_read(reader, &opcodes->body.CMSG_BUY_ITEM_IN_SLOT));
            break;
        case CMSG_TAXINODE_STATUS_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_TAXINODE_STATUS_QUERY_read(reader, &opcodes->body.CMSG_TAXINODE_STATUS_QUERY));
            break;
        case CMSG_TAXIQUERYAVAILABLENODES:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_TAXIQUERYAVAILABLENODES_read(reader, &opcodes->body.CMSG_TAXIQUERYAVAILABLENODES));
            break;
        case CMSG_ACTIVATETAXI:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_ACTIVATETAXI_read(reader, &opcodes->body.CMSG_ACTIVATETAXI));
            break;
        case CMSG_TRAINER_LIST:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_TRAINER_LIST_read(reader, &opcodes->body.CMSG_TRAINER_LIST));
            break;
        case CMSG_TRAINER_BUY_SPELL:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_TRAINER_BUY_SPELL_read(reader, &opcodes->body.CMSG_TRAINER_BUY_SPELL));
            break;
        case CMSG_BINDER_ACTIVATE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_BINDER_ACTIVATE_read(reader, &opcodes->body.CMSG_BINDER_ACTIVATE));
            break;
        case CMSG_BANKER_ACTIVATE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_BANKER_ACTIVATE_read(reader, &opcodes->body.CMSG_BANKER_ACTIVATE));
            break;
        case CMSG_BUY_BANK_SLOT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_BUY_BANK_SLOT_read(reader, &opcodes->body.CMSG_BUY_BANK_SLOT));
            break;
        case CMSG_PETITION_SHOWLIST:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PETITION_SHOWLIST_read(reader, &opcodes->body.CMSG_PETITION_SHOWLIST));
            break;
        case CMSG_PETITION_BUY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PETITION_BUY_read(reader, &opcodes->body.CMSG_PETITION_BUY));
            break;
        case CMSG_PETITION_SHOW_SIGNATURES:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PETITION_SHOW_SIGNATURES_read(reader, &opcodes->body.CMSG_PETITION_SHOW_SIGNATURES));
            break;
        case CMSG_PETITION_SIGN:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PETITION_SIGN_read(reader, &opcodes->body.CMSG_PETITION_SIGN));
            break;
        case MSG_PETITION_DECLINE:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_PETITION_DECLINE_read(reader, &opcodes->body.MSG_PETITION_DECLINE));
            break;
        case CMSG_OFFER_PETITION:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_OFFER_PETITION_read(reader, &opcodes->body.CMSG_OFFER_PETITION));
            break;
        case CMSG_TURN_IN_PETITION:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_TURN_IN_PETITION_read(reader, &opcodes->body.CMSG_TURN_IN_PETITION));
            break;
        case CMSG_PETITION_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PETITION_QUERY_read(reader, &opcodes->body.CMSG_PETITION_QUERY));
            break;
        case CMSG_BUG:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_BUG_read(reader, &opcodes->body.CMSG_BUG));
            break;
        case CMSG_RECLAIM_CORPSE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_RECLAIM_CORPSE_read(reader, &opcodes->body.CMSG_RECLAIM_CORPSE));
            break;
        case CMSG_WRAP_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_WRAP_ITEM_read(reader, &opcodes->body.CMSG_WRAP_ITEM));
            break;
        case MSG_MINIMAP_PING:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MINIMAP_PING_Client_read(reader, &opcodes->body.MSG_MINIMAP_PING_Client));
            break;
        case CMSG_PING:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PING_read(reader, &opcodes->body.CMSG_PING));
            break;
        case CMSG_SETSHEATHED:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SETSHEATHED_read(reader, &opcodes->body.CMSG_SETSHEATHED));
            break;
        case CMSG_AUTH_SESSION:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AUTH_SESSION_read(reader, &opcodes->body.CMSG_AUTH_SESSION, _size - 4));
            break;
        case CMSG_PET_CAST_SPELL:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PET_CAST_SPELL_read(reader, &opcodes->body.CMSG_PET_CAST_SPELL));
            break;
        case MSG_SAVE_GUILD_EMBLEM:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_SAVE_GUILD_EMBLEM_Client_read(reader, &opcodes->body.MSG_SAVE_GUILD_EMBLEM_Client));
            break;
        case MSG_TABARDVENDOR_ACTIVATE:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_TABARDVENDOR_ACTIVATE_read(reader, &opcodes->body.MSG_TABARDVENDOR_ACTIVATE));
            break;
        case CMSG_ZONEUPDATE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_ZONEUPDATE_read(reader, &opcodes->body.CMSG_ZONEUPDATE));
            break;
        case MSG_RANDOM_ROLL:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_RANDOM_ROLL_Client_read(reader, &opcodes->body.MSG_RANDOM_ROLL_Client));
            break;
        case CMSG_UNLEARN_SKILL:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_UNLEARN_SKILL_read(reader, &opcodes->body.CMSG_UNLEARN_SKILL));
            break;
        case CMSG_GMTICKET_CREATE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GMTICKET_CREATE_read(reader, &opcodes->body.CMSG_GMTICKET_CREATE, _size - 4));
            break;
        case CMSG_GMTICKET_UPDATETEXT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GMTICKET_UPDATETEXT_read(reader, &opcodes->body.CMSG_GMTICKET_UPDATETEXT));
            break;
        case CMSG_REQUEST_ACCOUNT_DATA:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_REQUEST_ACCOUNT_DATA_read(reader, &opcodes->body.CMSG_REQUEST_ACCOUNT_DATA));
            break;
        case CMSG_UPDATE_ACCOUNT_DATA:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_UPDATE_ACCOUNT_DATA_read(reader, &opcodes->body.CMSG_UPDATE_ACCOUNT_DATA, _size - 4));
            break;
        case CMSG_SPIRIT_HEALER_ACTIVATE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SPIRIT_HEALER_ACTIVATE_read(reader, &opcodes->body.CMSG_SPIRIT_HEALER_ACTIVATE));
            break;
        case CMSG_CHAT_IGNORED:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHAT_IGNORED_read(reader, &opcodes->body.CMSG_CHAT_IGNORED));
            break;
        case CMSG_GUILD_RANK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GUILD_RANK_read(reader, &opcodes->body.CMSG_GUILD_RANK));
            break;
        case CMSG_GUILD_ADD_RANK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GUILD_ADD_RANK_read(reader, &opcodes->body.CMSG_GUILD_ADD_RANK));
            break;
        case CMSG_GUILD_SET_PUBLIC_NOTE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GUILD_SET_PUBLIC_NOTE_read(reader, &opcodes->body.CMSG_GUILD_SET_PUBLIC_NOTE));
            break;
        case CMSG_GUILD_SET_OFFICER_NOTE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GUILD_SET_OFFICER_NOTE_read(reader, &opcodes->body.CMSG_GUILD_SET_OFFICER_NOTE));
            break;
        case CMSG_SEND_MAIL:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SEND_MAIL_read(reader, &opcodes->body.CMSG_SEND_MAIL));
            break;
        case CMSG_GET_MAIL_LIST:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GET_MAIL_LIST_read(reader, &opcodes->body.CMSG_GET_MAIL_LIST));
            break;
        case CMSG_BATTLEFIELD_LIST:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_BATTLEFIELD_LIST_read(reader, &opcodes->body.CMSG_BATTLEFIELD_LIST));
            break;
        case CMSG_BATTLEFIELD_JOIN:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_BATTLEFIELD_JOIN_read(reader, &opcodes->body.CMSG_BATTLEFIELD_JOIN));
            break;
        case CMSG_ITEM_TEXT_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_ITEM_TEXT_QUERY_read(reader, &opcodes->body.CMSG_ITEM_TEXT_QUERY));
            break;
        case CMSG_MAIL_TAKE_MONEY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MAIL_TAKE_MONEY_read(reader, &opcodes->body.CMSG_MAIL_TAKE_MONEY));
            break;
        case CMSG_MAIL_TAKE_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MAIL_TAKE_ITEM_read(reader, &opcodes->body.CMSG_MAIL_TAKE_ITEM));
            break;
        case CMSG_MAIL_MARK_AS_READ:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MAIL_MARK_AS_READ_read(reader, &opcodes->body.CMSG_MAIL_MARK_AS_READ));
            break;
        case CMSG_MAIL_RETURN_TO_SENDER:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MAIL_RETURN_TO_SENDER_read(reader, &opcodes->body.CMSG_MAIL_RETURN_TO_SENDER));
            break;
        case CMSG_MAIL_DELETE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MAIL_DELETE_read(reader, &opcodes->body.CMSG_MAIL_DELETE));
            break;
        case CMSG_MAIL_CREATE_TEXT_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MAIL_CREATE_TEXT_ITEM_read(reader, &opcodes->body.CMSG_MAIL_CREATE_TEXT_ITEM));
            break;
        case CMSG_LEARN_TALENT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_LEARN_TALENT_read(reader, &opcodes->body.CMSG_LEARN_TALENT));
            break;
        case CMSG_TOGGLE_PVP:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_TOGGLE_PVP_read(reader, &opcodes->body.CMSG_TOGGLE_PVP, _size - 4));
            break;
        case MSG_AUCTION_HELLO:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_AUCTION_HELLO_Client_read(reader, &opcodes->body.MSG_AUCTION_HELLO_Client));
            break;
        case CMSG_AUCTION_SELL_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AUCTION_SELL_ITEM_read(reader, &opcodes->body.CMSG_AUCTION_SELL_ITEM));
            break;
        case CMSG_AUCTION_REMOVE_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AUCTION_REMOVE_ITEM_read(reader, &opcodes->body.CMSG_AUCTION_REMOVE_ITEM));
            break;
        case CMSG_AUCTION_LIST_ITEMS:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AUCTION_LIST_ITEMS_read(reader, &opcodes->body.CMSG_AUCTION_LIST_ITEMS));
            break;
        case CMSG_AUCTION_LIST_OWNER_ITEMS:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AUCTION_LIST_OWNER_ITEMS_read(reader, &opcodes->body.CMSG_AUCTION_LIST_OWNER_ITEMS));
            break;
        case CMSG_AUCTION_PLACE_BID:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AUCTION_PLACE_BID_read(reader, &opcodes->body.CMSG_AUCTION_PLACE_BID));
            break;
        case CMSG_AUCTION_LIST_BIDDER_ITEMS:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AUCTION_LIST_BIDDER_ITEMS_read(reader, &opcodes->body.CMSG_AUCTION_LIST_BIDDER_ITEMS));
            break;
        case CMSG_SET_AMMO:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SET_AMMO_read(reader, &opcodes->body.CMSG_SET_AMMO));
            break;
        case CMSG_SET_ACTIVE_MOVER:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SET_ACTIVE_MOVER_read(reader, &opcodes->body.CMSG_SET_ACTIVE_MOVER));
            break;
        case CMSG_PET_CANCEL_AURA:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PET_CANCEL_AURA_read(reader, &opcodes->body.CMSG_PET_CANCEL_AURA));
            break;
        case MSG_LIST_STABLED_PETS:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_LIST_STABLED_PETS_Client_read(reader, &opcodes->body.MSG_LIST_STABLED_PETS_Client));
            break;
        case CMSG_STABLE_PET:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_STABLE_PET_read(reader, &opcodes->body.CMSG_STABLE_PET));
            break;
        case CMSG_UNSTABLE_PET:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_UNSTABLE_PET_read(reader, &opcodes->body.CMSG_UNSTABLE_PET));
            break;
        case CMSG_BUY_STABLE_SLOT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_BUY_STABLE_SLOT_read(reader, &opcodes->body.CMSG_BUY_STABLE_SLOT));
            break;
        case CMSG_STABLE_SWAP_PET:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_STABLE_SWAP_PET_read(reader, &opcodes->body.CMSG_STABLE_SWAP_PET));
            break;
        case MSG_QUEST_PUSH_RESULT:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_QUEST_PUSH_RESULT_read(reader, &opcodes->body.MSG_QUEST_PUSH_RESULT));
            break;
        case CMSG_FAR_SIGHT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_FAR_SIGHT_read(reader, &opcodes->body.CMSG_FAR_SIGHT));
            break;
        case CMSG_GROUP_CHANGE_SUB_GROUP:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GROUP_CHANGE_SUB_GROUP_read(reader, &opcodes->body.CMSG_GROUP_CHANGE_SUB_GROUP));
            break;
        case CMSG_REQUEST_PARTY_MEMBER_STATS:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_REQUEST_PARTY_MEMBER_STATS_read(reader, &opcodes->body.CMSG_REQUEST_PARTY_MEMBER_STATS));
            break;
        case CMSG_GROUP_SWAP_SUB_GROUP:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GROUP_SWAP_SUB_GROUP_read(reader, &opcodes->body.CMSG_GROUP_SWAP_SUB_GROUP));
            break;
        case CMSG_AUTOSTORE_BANK_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AUTOSTORE_BANK_ITEM_read(reader, &opcodes->body.CMSG_AUTOSTORE_BANK_ITEM));
            break;
        case CMSG_AUTOBANK_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AUTOBANK_ITEM_read(reader, &opcodes->body.CMSG_AUTOBANK_ITEM));
            break;
        case CMSG_GROUP_ASSISTANT_LEADER:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GROUP_ASSISTANT_LEADER_read(reader, &opcodes->body.CMSG_GROUP_ASSISTANT_LEADER));
            break;
        case CMSG_BUYBACK_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_BUYBACK_ITEM_read(reader, &opcodes->body.CMSG_BUYBACK_ITEM));
            break;
        case CMSG_MEETINGSTONE_JOIN:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MEETINGSTONE_JOIN_read(reader, &opcodes->body.CMSG_MEETINGSTONE_JOIN));
            break;
        case CMSG_LOOT_ROLL:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_LOOT_ROLL_read(reader, &opcodes->body.CMSG_LOOT_ROLL));
            break;
        case CMSG_LOOT_MASTER_GIVE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_LOOT_MASTER_GIVE_read(reader, &opcodes->body.CMSG_LOOT_MASTER_GIVE));
            break;
        case CMSG_REPAIR_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_REPAIR_ITEM_read(reader, &opcodes->body.CMSG_REPAIR_ITEM));
            break;
        case MSG_TALENT_WIPE_CONFIRM:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_TALENT_WIPE_CONFIRM_Client_read(reader, &opcodes->body.MSG_TALENT_WIPE_CONFIRM_Client));
            break;
        case CMSG_SUMMON_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SUMMON_RESPONSE_read(reader, &opcodes->body.CMSG_SUMMON_RESPONSE));
            break;
        case MSG_MOVE_WATER_WALK:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_WATER_WALK_read(reader, &opcodes->body.MSG_MOVE_WATER_WALK));
            break;
        case CMSG_SET_ACTIONBAR_TOGGLES:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SET_ACTIONBAR_TOGGLES_read(reader, &opcodes->body.CMSG_SET_ACTIONBAR_TOGGLES));
            break;
        case MSG_PETITION_RENAME:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_PETITION_RENAME_read(reader, &opcodes->body.MSG_PETITION_RENAME));
            break;
        case CMSG_ITEM_NAME_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_ITEM_NAME_QUERY_read(reader, &opcodes->body.CMSG_ITEM_NAME_QUERY));
            break;
        case CMSG_CHAR_RENAME:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_CHAR_RENAME_read(reader, &opcodes->body.CMSG_CHAR_RENAME));
            break;
        case CMSG_MOVE_SPLINE_DONE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MOVE_SPLINE_DONE_read(reader, &opcodes->body.CMSG_MOVE_SPLINE_DONE));
            break;
        case CMSG_MOVE_FALL_RESET:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MOVE_FALL_RESET_read(reader, &opcodes->body.CMSG_MOVE_FALL_RESET));
            break;
        case CMSG_MOVE_TIME_SKIPPED:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MOVE_TIME_SKIPPED_read(reader, &opcodes->body.CMSG_MOVE_TIME_SKIPPED));
            break;
        case CMSG_MOVE_FEATHER_FALL_ACK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MOVE_FEATHER_FALL_ACK_read(reader, &opcodes->body.CMSG_MOVE_FEATHER_FALL_ACK));
            break;
        case CMSG_MOVE_WATER_WALK_ACK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MOVE_WATER_WALK_ACK_read(reader, &opcodes->body.CMSG_MOVE_WATER_WALK_ACK));
            break;
        case CMSG_MOVE_NOT_ACTIVE_MOVER:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_MOVE_NOT_ACTIVE_MOVER_read(reader, &opcodes->body.CMSG_MOVE_NOT_ACTIVE_MOVER));
            break;
        case CMSG_BATTLEFIELD_PORT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_BATTLEFIELD_PORT_read(reader, &opcodes->body.CMSG_BATTLEFIELD_PORT));
            break;
        case MSG_INSPECT_HONOR_STATS:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_INSPECT_HONOR_STATS_Client_read(reader, &opcodes->body.MSG_INSPECT_HONOR_STATS_Client));
            break;
        case CMSG_BATTLEMASTER_HELLO:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_BATTLEMASTER_HELLO_read(reader, &opcodes->body.CMSG_BATTLEMASTER_HELLO));
            break;
        case CMSG_FORCE_WALK_SPEED_CHANGE_ACK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_FORCE_WALK_SPEED_CHANGE_ACK_read(reader, &opcodes->body.CMSG_FORCE_WALK_SPEED_CHANGE_ACK));
            break;
        case CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK_read(reader, &opcodes->body.CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK));
            break;
        case CMSG_FORCE_TURN_RATE_CHANGE_ACK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_FORCE_TURN_RATE_CHANGE_ACK_read(reader, &opcodes->body.CMSG_FORCE_TURN_RATE_CHANGE_ACK));
            break;
        case CMSG_LEAVE_BATTLEFIELD:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_LEAVE_BATTLEFIELD_read(reader, &opcodes->body.CMSG_LEAVE_BATTLEFIELD));
            break;
        case CMSG_AREA_SPIRIT_HEALER_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AREA_SPIRIT_HEALER_QUERY_read(reader, &opcodes->body.CMSG_AREA_SPIRIT_HEALER_QUERY));
            break;
        case CMSG_AREA_SPIRIT_HEALER_QUEUE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_AREA_SPIRIT_HEALER_QUEUE_read(reader, &opcodes->body.CMSG_AREA_SPIRIT_HEALER_QUEUE));
            break;
        case CMSG_WARDEN_DATA:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_WARDEN_DATA_read(reader, &opcodes->body.CMSG_WARDEN_DATA, _size - 4));
            break;
        case CMSG_PET_STOP_ATTACK:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PET_STOP_ATTACK_read(reader, &opcodes->body.CMSG_PET_STOP_ATTACK));
            break;
        case CMSG_BATTLEMASTER_JOIN:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_BATTLEMASTER_JOIN_read(reader, &opcodes->body.CMSG_BATTLEMASTER_JOIN));
            break;
        case CMSG_PET_UNLEARN:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PET_UNLEARN_read(reader, &opcodes->body.CMSG_PET_UNLEARN));
            break;
        case CMSG_PET_SPELL_AUTOCAST:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_PET_SPELL_AUTOCAST_read(reader, &opcodes->body.CMSG_PET_SPELL_AUTOCAST));
            break;
        case CMSG_GUILD_INFO_TEXT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GUILD_INFO_TEXT_read(reader, &opcodes->body.CMSG_GUILD_INFO_TEXT));
            break;
        case CMSG_ACTIVATETAXIEXPRESS:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_ACTIVATETAXIEXPRESS_read(reader, &opcodes->body.CMSG_ACTIVATETAXIEXPRESS));
            break;
        case CMSG_SET_FACTION_INACTIVE:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SET_FACTION_INACTIVE_read(reader, &opcodes->body.CMSG_SET_FACTION_INACTIVE));
            break;
        case CMSG_SET_WATCHED_FACTION:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_SET_WATCHED_FACTION_read(reader, &opcodes->body.CMSG_SET_WATCHED_FACTION));
            break;
        case MSG_RAID_TARGET_UPDATE:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_RAID_TARGET_UPDATE_Client_read(reader, &opcodes->body.MSG_RAID_TARGET_UPDATE_Client));
            break;
        case MSG_RAID_READY_CHECK:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_RAID_READY_CHECK_Client_read(reader, &opcodes->body.MSG_RAID_READY_CHECK_Client, _size - 4));
            break;
        case CMSG_GMSURVEY_SUBMIT:
            WWM_CHECK_RETURN_CODE(vanilla_CMSG_GMSURVEY_SUBMIT_read(reader, &opcodes->body.CMSG_GMSURVEY_SUBMIT));
            break;
        default:
            break;
    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_client_opcode_free(VanillaClientOpcodeContainer* opcodes) {
    switch (opcodes->opcode) {
        case CMSG_DBLOOKUP:
            vanilla_CMSG_DBLOOKUP_free(&opcodes->body.CMSG_DBLOOKUP);
            break;
        case CMSG_TELEPORT_TO_UNIT:
            vanilla_CMSG_TELEPORT_TO_UNIT_free(&opcodes->body.CMSG_TELEPORT_TO_UNIT);
            break;
        case CMSG_CHAR_CREATE:
            vanilla_CMSG_CHAR_CREATE_free(&opcodes->body.CMSG_CHAR_CREATE);
            break;
        case CMSG_WHO:
            vanilla_CMSG_WHO_free(&opcodes->body.CMSG_WHO);
            break;
        case CMSG_WHOIS:
            vanilla_CMSG_WHOIS_free(&opcodes->body.CMSG_WHOIS);
            break;
        case CMSG_ADD_FRIEND:
            vanilla_CMSG_ADD_FRIEND_free(&opcodes->body.CMSG_ADD_FRIEND);
            break;
        case CMSG_ADD_IGNORE:
            vanilla_CMSG_ADD_IGNORE_free(&opcodes->body.CMSG_ADD_IGNORE);
            break;
        case CMSG_GROUP_INVITE:
            vanilla_CMSG_GROUP_INVITE_free(&opcodes->body.CMSG_GROUP_INVITE);
            break;
        case CMSG_GROUP_UNINVITE:
            vanilla_CMSG_GROUP_UNINVITE_free(&opcodes->body.CMSG_GROUP_UNINVITE);
            break;
        case CMSG_GUILD_CREATE:
            vanilla_CMSG_GUILD_CREATE_free(&opcodes->body.CMSG_GUILD_CREATE);
            break;
        case CMSG_GUILD_INVITE:
            vanilla_CMSG_GUILD_INVITE_free(&opcodes->body.CMSG_GUILD_INVITE);
            break;
        case CMSG_GUILD_PROMOTE:
            vanilla_CMSG_GUILD_PROMOTE_free(&opcodes->body.CMSG_GUILD_PROMOTE);
            break;
        case CMSG_GUILD_DEMOTE:
            vanilla_CMSG_GUILD_DEMOTE_free(&opcodes->body.CMSG_GUILD_DEMOTE);
            break;
        case CMSG_GUILD_REMOVE:
            vanilla_CMSG_GUILD_REMOVE_free(&opcodes->body.CMSG_GUILD_REMOVE);
            break;
        case CMSG_GUILD_LEADER:
            vanilla_CMSG_GUILD_LEADER_free(&opcodes->body.CMSG_GUILD_LEADER);
            break;
        case CMSG_GUILD_MOTD:
            vanilla_CMSG_GUILD_MOTD_free(&opcodes->body.CMSG_GUILD_MOTD);
            break;
        case CMSG_MESSAGECHAT:
            vanilla_CMSG_MESSAGECHAT_free(&opcodes->body.CMSG_MESSAGECHAT);
            break;
        case CMSG_JOIN_CHANNEL:
            vanilla_CMSG_JOIN_CHANNEL_free(&opcodes->body.CMSG_JOIN_CHANNEL);
            break;
        case CMSG_LEAVE_CHANNEL:
            vanilla_CMSG_LEAVE_CHANNEL_free(&opcodes->body.CMSG_LEAVE_CHANNEL);
            break;
        case CMSG_CHANNEL_LIST:
            vanilla_CMSG_CHANNEL_LIST_free(&opcodes->body.CMSG_CHANNEL_LIST);
            break;
        case CMSG_CHANNEL_PASSWORD:
            vanilla_CMSG_CHANNEL_PASSWORD_free(&opcodes->body.CMSG_CHANNEL_PASSWORD);
            break;
        case CMSG_CHANNEL_SET_OWNER:
            vanilla_CMSG_CHANNEL_SET_OWNER_free(&opcodes->body.CMSG_CHANNEL_SET_OWNER);
            break;
        case CMSG_CHANNEL_OWNER:
            vanilla_CMSG_CHANNEL_OWNER_free(&opcodes->body.CMSG_CHANNEL_OWNER);
            break;
        case CMSG_CHANNEL_MODERATOR:
            vanilla_CMSG_CHANNEL_MODERATOR_free(&opcodes->body.CMSG_CHANNEL_MODERATOR);
            break;
        case CMSG_CHANNEL_UNMODERATOR:
            vanilla_CMSG_CHANNEL_UNMODERATOR_free(&opcodes->body.CMSG_CHANNEL_UNMODERATOR);
            break;
        case CMSG_CHANNEL_MUTE:
            vanilla_CMSG_CHANNEL_MUTE_free(&opcodes->body.CMSG_CHANNEL_MUTE);
            break;
        case CMSG_CHANNEL_UNMUTE:
            vanilla_CMSG_CHANNEL_UNMUTE_free(&opcodes->body.CMSG_CHANNEL_UNMUTE);
            break;
        case CMSG_CHANNEL_INVITE:
            vanilla_CMSG_CHANNEL_INVITE_free(&opcodes->body.CMSG_CHANNEL_INVITE);
            break;
        case CMSG_CHANNEL_KICK:
            vanilla_CMSG_CHANNEL_KICK_free(&opcodes->body.CMSG_CHANNEL_KICK);
            break;
        case CMSG_CHANNEL_BAN:
            vanilla_CMSG_CHANNEL_BAN_free(&opcodes->body.CMSG_CHANNEL_BAN);
            break;
        case CMSG_CHANNEL_UNBAN:
            vanilla_CMSG_CHANNEL_UNBAN_free(&opcodes->body.CMSG_CHANNEL_UNBAN);
            break;
        case CMSG_CHANNEL_ANNOUNCEMENTS:
            vanilla_CMSG_CHANNEL_ANNOUNCEMENTS_free(&opcodes->body.CMSG_CHANNEL_ANNOUNCEMENTS);
            break;
        case CMSG_CHANNEL_MODERATE:
            vanilla_CMSG_CHANNEL_MODERATE_free(&opcodes->body.CMSG_CHANNEL_MODERATE);
            break;
        case CMSG_USE_ITEM:
            vanilla_CMSG_USE_ITEM_free(&opcodes->body.CMSG_USE_ITEM);
            break;
        case MSG_MOVE_START_FORWARD:
            vanilla_MSG_MOVE_START_FORWARD_Client_free(&opcodes->body.MSG_MOVE_START_FORWARD_Client);
            break;
        case MSG_MOVE_START_BACKWARD:
            vanilla_MSG_MOVE_START_BACKWARD_Client_free(&opcodes->body.MSG_MOVE_START_BACKWARD_Client);
            break;
        case MSG_MOVE_STOP:
            vanilla_MSG_MOVE_STOP_Client_free(&opcodes->body.MSG_MOVE_STOP_Client);
            break;
        case MSG_MOVE_START_STRAFE_LEFT:
            vanilla_MSG_MOVE_START_STRAFE_LEFT_Client_free(&opcodes->body.MSG_MOVE_START_STRAFE_LEFT_Client);
            break;
        case MSG_MOVE_START_STRAFE_RIGHT:
            vanilla_MSG_MOVE_START_STRAFE_RIGHT_Client_free(&opcodes->body.MSG_MOVE_START_STRAFE_RIGHT_Client);
            break;
        case MSG_MOVE_STOP_STRAFE:
            vanilla_MSG_MOVE_STOP_STRAFE_Client_free(&opcodes->body.MSG_MOVE_STOP_STRAFE_Client);
            break;
        case MSG_MOVE_JUMP:
            vanilla_MSG_MOVE_JUMP_Client_free(&opcodes->body.MSG_MOVE_JUMP_Client);
            break;
        case MSG_MOVE_START_TURN_LEFT:
            vanilla_MSG_MOVE_START_TURN_LEFT_Client_free(&opcodes->body.MSG_MOVE_START_TURN_LEFT_Client);
            break;
        case MSG_MOVE_START_TURN_RIGHT:
            vanilla_MSG_MOVE_START_TURN_RIGHT_Client_free(&opcodes->body.MSG_MOVE_START_TURN_RIGHT_Client);
            break;
        case MSG_MOVE_STOP_TURN:
            vanilla_MSG_MOVE_STOP_TURN_Client_free(&opcodes->body.MSG_MOVE_STOP_TURN_Client);
            break;
        case MSG_MOVE_START_PITCH_UP:
            vanilla_MSG_MOVE_START_PITCH_UP_Client_free(&opcodes->body.MSG_MOVE_START_PITCH_UP_Client);
            break;
        case MSG_MOVE_START_PITCH_DOWN:
            vanilla_MSG_MOVE_START_PITCH_DOWN_Client_free(&opcodes->body.MSG_MOVE_START_PITCH_DOWN_Client);
            break;
        case MSG_MOVE_STOP_PITCH:
            vanilla_MSG_MOVE_STOP_PITCH_Client_free(&opcodes->body.MSG_MOVE_STOP_PITCH_Client);
            break;
        case MSG_MOVE_SET_RUN_MODE:
            vanilla_MSG_MOVE_SET_RUN_MODE_Client_free(&opcodes->body.MSG_MOVE_SET_RUN_MODE_Client);
            break;
        case MSG_MOVE_SET_WALK_MODE:
            vanilla_MSG_MOVE_SET_WALK_MODE_Client_free(&opcodes->body.MSG_MOVE_SET_WALK_MODE_Client);
            break;
        case MSG_MOVE_FALL_LAND:
            vanilla_MSG_MOVE_FALL_LAND_Client_free(&opcodes->body.MSG_MOVE_FALL_LAND_Client);
            break;
        case MSG_MOVE_START_SWIM:
            vanilla_MSG_MOVE_START_SWIM_Client_free(&opcodes->body.MSG_MOVE_START_SWIM_Client);
            break;
        case MSG_MOVE_STOP_SWIM:
            vanilla_MSG_MOVE_STOP_SWIM_Client_free(&opcodes->body.MSG_MOVE_STOP_SWIM_Client);
            break;
        case MSG_MOVE_SET_FACING:
            vanilla_MSG_MOVE_SET_FACING_Client_free(&opcodes->body.MSG_MOVE_SET_FACING_Client);
            break;
        case MSG_MOVE_SET_PITCH:
            vanilla_MSG_MOVE_SET_PITCH_Client_free(&opcodes->body.MSG_MOVE_SET_PITCH_Client);
            break;
        case CMSG_FORCE_RUN_SPEED_CHANGE_ACK:
            vanilla_CMSG_FORCE_RUN_SPEED_CHANGE_ACK_free(&opcodes->body.CMSG_FORCE_RUN_SPEED_CHANGE_ACK);
            break;
        case CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK:
            vanilla_CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK_free(&opcodes->body.CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK);
            break;
        case CMSG_FORCE_SWIM_SPEED_CHANGE_ACK:
            vanilla_CMSG_FORCE_SWIM_SPEED_CHANGE_ACK_free(&opcodes->body.CMSG_FORCE_SWIM_SPEED_CHANGE_ACK);
            break;
        case CMSG_FORCE_MOVE_ROOT_ACK:
            vanilla_CMSG_FORCE_MOVE_ROOT_ACK_free(&opcodes->body.CMSG_FORCE_MOVE_ROOT_ACK);
            break;
        case CMSG_FORCE_MOVE_UNROOT_ACK:
            vanilla_CMSG_FORCE_MOVE_UNROOT_ACK_free(&opcodes->body.CMSG_FORCE_MOVE_UNROOT_ACK);
            break;
        case MSG_MOVE_HEARTBEAT:
            vanilla_MSG_MOVE_HEARTBEAT_Client_free(&opcodes->body.MSG_MOVE_HEARTBEAT_Client);
            break;
        case CMSG_MOVE_KNOCK_BACK_ACK:
            vanilla_CMSG_MOVE_KNOCK_BACK_ACK_free(&opcodes->body.CMSG_MOVE_KNOCK_BACK_ACK);
            break;
        case CMSG_MOVE_HOVER_ACK:
            vanilla_CMSG_MOVE_HOVER_ACK_free(&opcodes->body.CMSG_MOVE_HOVER_ACK);
            break;
        case CMSG_CAST_SPELL:
            vanilla_CMSG_CAST_SPELL_free(&opcodes->body.CMSG_CAST_SPELL);
            break;
        case CMSG_PET_SET_ACTION:
            vanilla_CMSG_PET_SET_ACTION_free(&opcodes->body.CMSG_PET_SET_ACTION);
            break;
        case CMSG_PET_RENAME:
            vanilla_CMSG_PET_RENAME_free(&opcodes->body.CMSG_PET_RENAME);
            break;
        case CMSG_GOSSIP_SELECT_OPTION:
            vanilla_CMSG_GOSSIP_SELECT_OPTION_free(&opcodes->body.CMSG_GOSSIP_SELECT_OPTION);
            break;
        case CMSG_PETITION_BUY:
            vanilla_CMSG_PETITION_BUY_free(&opcodes->body.CMSG_PETITION_BUY);
            break;
        case CMSG_BUG:
            vanilla_CMSG_BUG_free(&opcodes->body.CMSG_BUG);
            break;
        case CMSG_AUTH_SESSION:
            vanilla_CMSG_AUTH_SESSION_free(&opcodes->body.CMSG_AUTH_SESSION);
            break;
        case CMSG_PET_CAST_SPELL:
            vanilla_CMSG_PET_CAST_SPELL_free(&opcodes->body.CMSG_PET_CAST_SPELL);
            break;
        case CMSG_GMTICKET_CREATE:
            vanilla_CMSG_GMTICKET_CREATE_free(&opcodes->body.CMSG_GMTICKET_CREATE);
            break;
        case CMSG_GMTICKET_UPDATETEXT:
            vanilla_CMSG_GMTICKET_UPDATETEXT_free(&opcodes->body.CMSG_GMTICKET_UPDATETEXT);
            break;
        case CMSG_UPDATE_ACCOUNT_DATA:
            vanilla_CMSG_UPDATE_ACCOUNT_DATA_free(&opcodes->body.CMSG_UPDATE_ACCOUNT_DATA);
            break;
        case CMSG_GUILD_RANK:
            vanilla_CMSG_GUILD_RANK_free(&opcodes->body.CMSG_GUILD_RANK);
            break;
        case CMSG_GUILD_ADD_RANK:
            vanilla_CMSG_GUILD_ADD_RANK_free(&opcodes->body.CMSG_GUILD_ADD_RANK);
            break;
        case CMSG_GUILD_SET_PUBLIC_NOTE:
            vanilla_CMSG_GUILD_SET_PUBLIC_NOTE_free(&opcodes->body.CMSG_GUILD_SET_PUBLIC_NOTE);
            break;
        case CMSG_GUILD_SET_OFFICER_NOTE:
            vanilla_CMSG_GUILD_SET_OFFICER_NOTE_free(&opcodes->body.CMSG_GUILD_SET_OFFICER_NOTE);
            break;
        case CMSG_SEND_MAIL:
            vanilla_CMSG_SEND_MAIL_free(&opcodes->body.CMSG_SEND_MAIL);
            break;
        case CMSG_TOGGLE_PVP:
            vanilla_CMSG_TOGGLE_PVP_free(&opcodes->body.CMSG_TOGGLE_PVP);
            break;
        case CMSG_AUCTION_LIST_ITEMS:
            vanilla_CMSG_AUCTION_LIST_ITEMS_free(&opcodes->body.CMSG_AUCTION_LIST_ITEMS);
            break;
        case CMSG_AUCTION_LIST_BIDDER_ITEMS:
            vanilla_CMSG_AUCTION_LIST_BIDDER_ITEMS_free(&opcodes->body.CMSG_AUCTION_LIST_BIDDER_ITEMS);
            break;
        case CMSG_GROUP_CHANGE_SUB_GROUP:
            vanilla_CMSG_GROUP_CHANGE_SUB_GROUP_free(&opcodes->body.CMSG_GROUP_CHANGE_SUB_GROUP);
            break;
        case CMSG_GROUP_SWAP_SUB_GROUP:
            vanilla_CMSG_GROUP_SWAP_SUB_GROUP_free(&opcodes->body.CMSG_GROUP_SWAP_SUB_GROUP);
            break;
        case MSG_MOVE_WATER_WALK:
            vanilla_MSG_MOVE_WATER_WALK_free(&opcodes->body.MSG_MOVE_WATER_WALK);
            break;
        case MSG_PETITION_RENAME:
            vanilla_MSG_PETITION_RENAME_free(&opcodes->body.MSG_PETITION_RENAME);
            break;
        case CMSG_CHAR_RENAME:
            vanilla_CMSG_CHAR_RENAME_free(&opcodes->body.CMSG_CHAR_RENAME);
            break;
        case CMSG_MOVE_SPLINE_DONE:
            vanilla_CMSG_MOVE_SPLINE_DONE_free(&opcodes->body.CMSG_MOVE_SPLINE_DONE);
            break;
        case CMSG_MOVE_FALL_RESET:
            vanilla_CMSG_MOVE_FALL_RESET_free(&opcodes->body.CMSG_MOVE_FALL_RESET);
            break;
        case CMSG_MOVE_FEATHER_FALL_ACK:
            vanilla_CMSG_MOVE_FEATHER_FALL_ACK_free(&opcodes->body.CMSG_MOVE_FEATHER_FALL_ACK);
            break;
        case CMSG_MOVE_WATER_WALK_ACK:
            vanilla_CMSG_MOVE_WATER_WALK_ACK_free(&opcodes->body.CMSG_MOVE_WATER_WALK_ACK);
            break;
        case CMSG_MOVE_NOT_ACTIVE_MOVER:
            vanilla_CMSG_MOVE_NOT_ACTIVE_MOVER_free(&opcodes->body.CMSG_MOVE_NOT_ACTIVE_MOVER);
            break;
        case CMSG_FORCE_WALK_SPEED_CHANGE_ACK:
            vanilla_CMSG_FORCE_WALK_SPEED_CHANGE_ACK_free(&opcodes->body.CMSG_FORCE_WALK_SPEED_CHANGE_ACK);
            break;
        case CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK:
            vanilla_CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK_free(&opcodes->body.CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK);
            break;
        case CMSG_FORCE_TURN_RATE_CHANGE_ACK:
            vanilla_CMSG_FORCE_TURN_RATE_CHANGE_ACK_free(&opcodes->body.CMSG_FORCE_TURN_RATE_CHANGE_ACK);
            break;
        case CMSG_WARDEN_DATA:
            vanilla_CMSG_WARDEN_DATA_free(&opcodes->body.CMSG_WARDEN_DATA);
            break;
        case CMSG_GUILD_INFO_TEXT:
            vanilla_CMSG_GUILD_INFO_TEXT_free(&opcodes->body.CMSG_GUILD_INFO_TEXT);
            break;
        case CMSG_ACTIVATETAXIEXPRESS:
            vanilla_CMSG_ACTIVATETAXIEXPRESS_free(&opcodes->body.CMSG_ACTIVATETAXIEXPRESS);
            break;
        case MSG_RAID_TARGET_UPDATE:
            vanilla_MSG_RAID_TARGET_UPDATE_Client_free(&opcodes->body.MSG_RAID_TARGET_UPDATE_Client);
            break;
        case MSG_RAID_READY_CHECK:
            vanilla_MSG_RAID_READY_CHECK_Client_free(&opcodes->body.MSG_RAID_READY_CHECK_Client);
            break;
        case CMSG_GMSURVEY_SUBMIT:
            vanilla_CMSG_GMSURVEY_SUBMIT_free(&opcodes->body.CMSG_GMSURVEY_SUBMIT);
            break;
        default:
            break;
    }
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_server_write_opcode(WowWorldWriter* writer, const VanillaServerOpcodeContainer* opcodes) {
    switch (opcodes->opcode) {
        case SMSG_CHAR_CREATE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_CHAR_CREATE_write(writer, &opcodes->body.SMSG_CHAR_CREATE));
            break;
        case SMSG_CHAR_ENUM:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_CHAR_ENUM_write(writer, &opcodes->body.SMSG_CHAR_ENUM));
            break;
        case SMSG_CHAR_DELETE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_CHAR_DELETE_write(writer, &opcodes->body.SMSG_CHAR_DELETE));
            break;
        case SMSG_NEW_WORLD:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_NEW_WORLD_write(writer, &opcodes->body.SMSG_NEW_WORLD));
            break;
        case SMSG_TRANSFER_PENDING:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_TRANSFER_PENDING_write(writer, &opcodes->body.SMSG_TRANSFER_PENDING));
            break;
        case SMSG_TRANSFER_ABORTED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_TRANSFER_ABORTED_write(writer, &opcodes->body.SMSG_TRANSFER_ABORTED));
            break;
        case SMSG_CHARACTER_LOGIN_FAILED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_CHARACTER_LOGIN_FAILED_write(writer, &opcodes->body.SMSG_CHARACTER_LOGIN_FAILED));
            break;
        case SMSG_LOGIN_SETTIMESPEED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LOGIN_SETTIMESPEED_write(writer, &opcodes->body.SMSG_LOGIN_SETTIMESPEED));
            break;
        case SMSG_LOGOUT_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LOGOUT_RESPONSE_write(writer, &opcodes->body.SMSG_LOGOUT_RESPONSE));
            break;
        case SMSG_NAME_QUERY_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_NAME_QUERY_RESPONSE_write(writer, &opcodes->body.SMSG_NAME_QUERY_RESPONSE));
            break;
        case SMSG_PET_NAME_QUERY_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PET_NAME_QUERY_RESPONSE_write(writer, &opcodes->body.SMSG_PET_NAME_QUERY_RESPONSE));
            break;
        case SMSG_GUILD_QUERY_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GUILD_QUERY_RESPONSE_write(writer, &opcodes->body.SMSG_GUILD_QUERY_RESPONSE));
            break;
        case SMSG_ITEM_QUERY_SINGLE_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ITEM_QUERY_SINGLE_RESPONSE_write(writer, &opcodes->body.SMSG_ITEM_QUERY_SINGLE_RESPONSE));
            break;
        case SMSG_PAGE_TEXT_QUERY_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PAGE_TEXT_QUERY_RESPONSE_write(writer, &opcodes->body.SMSG_PAGE_TEXT_QUERY_RESPONSE));
            break;
        case SMSG_QUEST_QUERY_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUEST_QUERY_RESPONSE_write(writer, &opcodes->body.SMSG_QUEST_QUERY_RESPONSE));
            break;
        case SMSG_GAMEOBJECT_QUERY_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GAMEOBJECT_QUERY_RESPONSE_write(writer, &opcodes->body.SMSG_GAMEOBJECT_QUERY_RESPONSE));
            break;
        case SMSG_CREATURE_QUERY_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_CREATURE_QUERY_RESPONSE_write(writer, &opcodes->body.SMSG_CREATURE_QUERY_RESPONSE));
            break;
        case SMSG_WHO:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_WHO_write(writer, &opcodes->body.SMSG_WHO));
            break;
        case SMSG_WHOIS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_WHOIS_write(writer, &opcodes->body.SMSG_WHOIS));
            break;
        case SMSG_FRIEND_LIST:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_FRIEND_LIST_write(writer, &opcodes->body.SMSG_FRIEND_LIST));
            break;
        case SMSG_FRIEND_STATUS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_FRIEND_STATUS_write(writer, &opcodes->body.SMSG_FRIEND_STATUS));
            break;
        case SMSG_IGNORE_LIST:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_IGNORE_LIST_write(writer, &opcodes->body.SMSG_IGNORE_LIST));
            break;
        case SMSG_GROUP_INVITE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GROUP_INVITE_write(writer, &opcodes->body.SMSG_GROUP_INVITE));
            break;
        case SMSG_GROUP_DECLINE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GROUP_DECLINE_write(writer, &opcodes->body.SMSG_GROUP_DECLINE));
            break;
        case SMSG_GROUP_SET_LEADER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GROUP_SET_LEADER_write(writer, &opcodes->body.SMSG_GROUP_SET_LEADER));
            break;
        case SMSG_GROUP_LIST:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GROUP_LIST_write(writer, &opcodes->body.SMSG_GROUP_LIST));
            break;
        case SMSG_PARTY_MEMBER_STATS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PARTY_MEMBER_STATS_write(writer, &opcodes->body.SMSG_PARTY_MEMBER_STATS));
            break;
        case SMSG_PARTY_COMMAND_RESULT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PARTY_COMMAND_RESULT_write(writer, &opcodes->body.SMSG_PARTY_COMMAND_RESULT));
            break;
        case SMSG_GUILD_INVITE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GUILD_INVITE_write(writer, &opcodes->body.SMSG_GUILD_INVITE));
            break;
        case SMSG_GUILD_INFO:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GUILD_INFO_write(writer, &opcodes->body.SMSG_GUILD_INFO));
            break;
        case SMSG_GUILD_ROSTER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GUILD_ROSTER_write(writer, &opcodes->body.SMSG_GUILD_ROSTER));
            break;
        case SMSG_GUILD_EVENT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GUILD_EVENT_write(writer, &opcodes->body.SMSG_GUILD_EVENT));
            break;
        case SMSG_GUILD_COMMAND_RESULT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GUILD_COMMAND_RESULT_write(writer, &opcodes->body.SMSG_GUILD_COMMAND_RESULT));
            break;
        case SMSG_MESSAGECHAT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MESSAGECHAT_write(writer, &opcodes->body.SMSG_MESSAGECHAT));
            break;
        case SMSG_CHANNEL_NOTIFY:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_CHANNEL_NOTIFY_write(writer, &opcodes->body.SMSG_CHANNEL_NOTIFY));
            break;
        case SMSG_CHANNEL_LIST:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_CHANNEL_LIST_write(writer, &opcodes->body.SMSG_CHANNEL_LIST));
            break;
        case SMSG_UPDATE_OBJECT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_UPDATE_OBJECT_write(writer, &opcodes->body.SMSG_UPDATE_OBJECT));
            break;
        case SMSG_DESTROY_OBJECT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_DESTROY_OBJECT_write(writer, &opcodes->body.SMSG_DESTROY_OBJECT));
            break;
        case SMSG_READ_ITEM_OK:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_READ_ITEM_OK_write(writer, &opcodes->body.SMSG_READ_ITEM_OK));
            break;
        case SMSG_READ_ITEM_FAILED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_READ_ITEM_FAILED_write(writer, &opcodes->body.SMSG_READ_ITEM_FAILED));
            break;
        case SMSG_ITEM_COOLDOWN:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ITEM_COOLDOWN_write(writer, &opcodes->body.SMSG_ITEM_COOLDOWN));
            break;
        case SMSG_GAMEOBJECT_CUSTOM_ANIM:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GAMEOBJECT_CUSTOM_ANIM_write(writer, &opcodes->body.SMSG_GAMEOBJECT_CUSTOM_ANIM));
            break;
        case MSG_MOVE_START_FORWARD:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_FORWARD_Server_write(writer, &opcodes->body.MSG_MOVE_START_FORWARD_Server));
            break;
        case MSG_MOVE_START_BACKWARD:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_BACKWARD_Server_write(writer, &opcodes->body.MSG_MOVE_START_BACKWARD_Server));
            break;
        case MSG_MOVE_STOP:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_STOP_Server_write(writer, &opcodes->body.MSG_MOVE_STOP_Server));
            break;
        case MSG_MOVE_START_STRAFE_LEFT:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_STRAFE_LEFT_Server_write(writer, &opcodes->body.MSG_MOVE_START_STRAFE_LEFT_Server));
            break;
        case MSG_MOVE_START_STRAFE_RIGHT:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_STRAFE_RIGHT_Server_write(writer, &opcodes->body.MSG_MOVE_START_STRAFE_RIGHT_Server));
            break;
        case MSG_MOVE_STOP_STRAFE:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_STOP_STRAFE_Server_write(writer, &opcodes->body.MSG_MOVE_STOP_STRAFE_Server));
            break;
        case MSG_MOVE_JUMP:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_JUMP_Server_write(writer, &opcodes->body.MSG_MOVE_JUMP_Server));
            break;
        case MSG_MOVE_START_TURN_LEFT:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_TURN_LEFT_Server_write(writer, &opcodes->body.MSG_MOVE_START_TURN_LEFT_Server));
            break;
        case MSG_MOVE_START_TURN_RIGHT:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_TURN_RIGHT_Server_write(writer, &opcodes->body.MSG_MOVE_START_TURN_RIGHT_Server));
            break;
        case MSG_MOVE_STOP_TURN:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_STOP_TURN_Server_write(writer, &opcodes->body.MSG_MOVE_STOP_TURN_Server));
            break;
        case MSG_MOVE_START_PITCH_UP:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_PITCH_UP_Server_write(writer, &opcodes->body.MSG_MOVE_START_PITCH_UP_Server));
            break;
        case MSG_MOVE_START_PITCH_DOWN:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_PITCH_DOWN_Server_write(writer, &opcodes->body.MSG_MOVE_START_PITCH_DOWN_Server));
            break;
        case MSG_MOVE_STOP_PITCH:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_STOP_PITCH_Server_write(writer, &opcodes->body.MSG_MOVE_STOP_PITCH_Server));
            break;
        case MSG_MOVE_SET_RUN_MODE:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_SET_RUN_MODE_Server_write(writer, &opcodes->body.MSG_MOVE_SET_RUN_MODE_Server));
            break;
        case MSG_MOVE_SET_WALK_MODE:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_SET_WALK_MODE_Server_write(writer, &opcodes->body.MSG_MOVE_SET_WALK_MODE_Server));
            break;
        case MSG_MOVE_TELEPORT_ACK:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_TELEPORT_ACK_Server_write(writer, &opcodes->body.MSG_MOVE_TELEPORT_ACK_Server));
            break;
        case MSG_MOVE_FALL_LAND:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_FALL_LAND_Server_write(writer, &opcodes->body.MSG_MOVE_FALL_LAND_Server));
            break;
        case MSG_MOVE_START_SWIM:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_SWIM_Server_write(writer, &opcodes->body.MSG_MOVE_START_SWIM_Server));
            break;
        case MSG_MOVE_STOP_SWIM:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_STOP_SWIM_Server_write(writer, &opcodes->body.MSG_MOVE_STOP_SWIM_Server));
            break;
        case MSG_MOVE_SET_FACING:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_SET_FACING_Server_write(writer, &opcodes->body.MSG_MOVE_SET_FACING_Server));
            break;
        case MSG_MOVE_SET_PITCH:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_SET_PITCH_Server_write(writer, &opcodes->body.MSG_MOVE_SET_PITCH_Server));
            break;
        case SMSG_MONSTER_MOVE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MONSTER_MOVE_write(writer, &opcodes->body.SMSG_MONSTER_MOVE));
            break;
        case SMSG_MOVE_WATER_WALK:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MOVE_WATER_WALK_write(writer, &opcodes->body.SMSG_MOVE_WATER_WALK));
            break;
        case SMSG_MOVE_LAND_WALK:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MOVE_LAND_WALK_write(writer, &opcodes->body.SMSG_MOVE_LAND_WALK));
            break;
        case SMSG_FORCE_RUN_SPEED_CHANGE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_FORCE_RUN_SPEED_CHANGE_write(writer, &opcodes->body.SMSG_FORCE_RUN_SPEED_CHANGE));
            break;
        case SMSG_FORCE_RUN_BACK_SPEED_CHANGE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_FORCE_RUN_BACK_SPEED_CHANGE_write(writer, &opcodes->body.SMSG_FORCE_RUN_BACK_SPEED_CHANGE));
            break;
        case SMSG_FORCE_SWIM_SPEED_CHANGE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_FORCE_SWIM_SPEED_CHANGE_write(writer, &opcodes->body.SMSG_FORCE_SWIM_SPEED_CHANGE));
            break;
        case SMSG_FORCE_MOVE_ROOT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_FORCE_MOVE_ROOT_write(writer, &opcodes->body.SMSG_FORCE_MOVE_ROOT));
            break;
        case SMSG_FORCE_MOVE_UNROOT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_FORCE_MOVE_UNROOT_write(writer, &opcodes->body.SMSG_FORCE_MOVE_UNROOT));
            break;
        case MSG_MOVE_HEARTBEAT:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_HEARTBEAT_Server_write(writer, &opcodes->body.MSG_MOVE_HEARTBEAT_Server));
            break;
        case SMSG_MOVE_KNOCK_BACK:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MOVE_KNOCK_BACK_write(writer, &opcodes->body.SMSG_MOVE_KNOCK_BACK));
            break;
        case SMSG_MOVE_FEATHER_FALL:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MOVE_FEATHER_FALL_write(writer, &opcodes->body.SMSG_MOVE_FEATHER_FALL));
            break;
        case SMSG_MOVE_NORMAL_FALL:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MOVE_NORMAL_FALL_write(writer, &opcodes->body.SMSG_MOVE_NORMAL_FALL));
            break;
        case SMSG_MOVE_SET_HOVER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MOVE_SET_HOVER_write(writer, &opcodes->body.SMSG_MOVE_SET_HOVER));
            break;
        case SMSG_MOVE_UNSET_HOVER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MOVE_UNSET_HOVER_write(writer, &opcodes->body.SMSG_MOVE_UNSET_HOVER));
            break;
        case SMSG_TRIGGER_CINEMATIC:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_TRIGGER_CINEMATIC_write(writer, &opcodes->body.SMSG_TRIGGER_CINEMATIC));
            break;
        case SMSG_TUTORIAL_FLAGS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_TUTORIAL_FLAGS_write(writer, &opcodes->body.SMSG_TUTORIAL_FLAGS));
            break;
        case SMSG_EMOTE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_EMOTE_write(writer, &opcodes->body.SMSG_EMOTE));
            break;
        case SMSG_TEXT_EMOTE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_TEXT_EMOTE_write(writer, &opcodes->body.SMSG_TEXT_EMOTE));
            break;
        case SMSG_INVENTORY_CHANGE_FAILURE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_INVENTORY_CHANGE_FAILURE_write(writer, &opcodes->body.SMSG_INVENTORY_CHANGE_FAILURE));
            break;
        case SMSG_OPEN_CONTAINER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_OPEN_CONTAINER_write(writer, &opcodes->body.SMSG_OPEN_CONTAINER));
            break;
        case SMSG_INSPECT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_INSPECT_write(writer, &opcodes->body.SMSG_INSPECT));
            break;
        case SMSG_TRADE_STATUS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_TRADE_STATUS_write(writer, &opcodes->body.SMSG_TRADE_STATUS));
            break;
        case SMSG_TRADE_STATUS_EXTENDED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_TRADE_STATUS_EXTENDED_write(writer, &opcodes->body.SMSG_TRADE_STATUS_EXTENDED));
            break;
        case SMSG_INITIALIZE_FACTIONS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_INITIALIZE_FACTIONS_write(writer, &opcodes->body.SMSG_INITIALIZE_FACTIONS));
            break;
        case SMSG_SET_FACTION_VISIBLE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SET_FACTION_VISIBLE_write(writer, &opcodes->body.SMSG_SET_FACTION_VISIBLE));
            break;
        case SMSG_SET_FACTION_STANDING:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SET_FACTION_STANDING_write(writer, &opcodes->body.SMSG_SET_FACTION_STANDING));
            break;
        case SMSG_SET_PROFICIENCY:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SET_PROFICIENCY_write(writer, &opcodes->body.SMSG_SET_PROFICIENCY));
            break;
        case SMSG_ACTION_BUTTONS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ACTION_BUTTONS_write(writer, &opcodes->body.SMSG_ACTION_BUTTONS));
            break;
        case SMSG_INITIAL_SPELLS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_INITIAL_SPELLS_write(writer, &opcodes->body.SMSG_INITIAL_SPELLS));
            break;
        case SMSG_LEARNED_SPELL:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LEARNED_SPELL_write(writer, &opcodes->body.SMSG_LEARNED_SPELL));
            break;
        case SMSG_SUPERCEDED_SPELL:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SUPERCEDED_SPELL_write(writer, &opcodes->body.SMSG_SUPERCEDED_SPELL));
            break;
        case SMSG_CAST_RESULT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_CAST_RESULT_write(writer, &opcodes->body.SMSG_CAST_RESULT));
            break;
        case SMSG_SPELL_START:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELL_START_write(writer, &opcodes->body.SMSG_SPELL_START));
            break;
        case SMSG_SPELL_GO:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELL_GO_write(writer, &opcodes->body.SMSG_SPELL_GO));
            break;
        case SMSG_SPELL_FAILURE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELL_FAILURE_write(writer, &opcodes->body.SMSG_SPELL_FAILURE));
            break;
        case SMSG_SPELL_COOLDOWN:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELL_COOLDOWN_write(writer, &opcodes->body.SMSG_SPELL_COOLDOWN));
            break;
        case SMSG_COOLDOWN_EVENT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_COOLDOWN_EVENT_write(writer, &opcodes->body.SMSG_COOLDOWN_EVENT));
            break;
        case SMSG_UPDATE_AURA_DURATION:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_UPDATE_AURA_DURATION_write(writer, &opcodes->body.SMSG_UPDATE_AURA_DURATION));
            break;
        case SMSG_PET_CAST_FAILED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PET_CAST_FAILED_write(writer, &opcodes->body.SMSG_PET_CAST_FAILED));
            break;
        case MSG_CHANNEL_START:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_CHANNEL_START_Server_write(writer, &opcodes->body.MSG_CHANNEL_START_Server));
            break;
        case MSG_CHANNEL_UPDATE:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_CHANNEL_UPDATE_Server_write(writer, &opcodes->body.MSG_CHANNEL_UPDATE_Server));
            break;
        case SMSG_AI_REACTION:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_AI_REACTION_write(writer, &opcodes->body.SMSG_AI_REACTION));
            break;
        case SMSG_ATTACKSTART:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ATTACKSTART_write(writer, &opcodes->body.SMSG_ATTACKSTART));
            break;
        case SMSG_ATTACKSTOP:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ATTACKSTOP_write(writer, &opcodes->body.SMSG_ATTACKSTOP));
            break;
        case SMSG_ATTACKERSTATEUPDATE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ATTACKERSTATEUPDATE_write(writer, &opcodes->body.SMSG_ATTACKERSTATEUPDATE));
            break;
        case SMSG_SPELLHEALLOG:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELLHEALLOG_write(writer, &opcodes->body.SMSG_SPELLHEALLOG));
            break;
        case SMSG_SPELLENERGIZELOG:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELLENERGIZELOG_write(writer, &opcodes->body.SMSG_SPELLENERGIZELOG));
            break;
        case SMSG_BINDPOINTUPDATE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_BINDPOINTUPDATE_write(writer, &opcodes->body.SMSG_BINDPOINTUPDATE));
            break;
        case SMSG_PLAYERBOUND:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PLAYERBOUND_write(writer, &opcodes->body.SMSG_PLAYERBOUND));
            break;
        case SMSG_CLIENT_CONTROL_UPDATE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_CLIENT_CONTROL_UPDATE_write(writer, &opcodes->body.SMSG_CLIENT_CONTROL_UPDATE));
            break;
        case SMSG_RESURRECT_REQUEST:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_RESURRECT_REQUEST_write(writer, &opcodes->body.SMSG_RESURRECT_REQUEST));
            break;
        case SMSG_LOOT_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LOOT_RESPONSE_write(writer, &opcodes->body.SMSG_LOOT_RESPONSE));
            break;
        case SMSG_LOOT_RELEASE_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LOOT_RELEASE_RESPONSE_write(writer, &opcodes->body.SMSG_LOOT_RELEASE_RESPONSE));
            break;
        case SMSG_LOOT_REMOVED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LOOT_REMOVED_write(writer, &opcodes->body.SMSG_LOOT_REMOVED));
            break;
        case SMSG_LOOT_MONEY_NOTIFY:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LOOT_MONEY_NOTIFY_write(writer, &opcodes->body.SMSG_LOOT_MONEY_NOTIFY));
            break;
        case SMSG_ITEM_PUSH_RESULT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ITEM_PUSH_RESULT_write(writer, &opcodes->body.SMSG_ITEM_PUSH_RESULT));
            break;
        case SMSG_DUEL_REQUESTED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_DUEL_REQUESTED_write(writer, &opcodes->body.SMSG_DUEL_REQUESTED));
            break;
        case SMSG_DUEL_COMPLETE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_DUEL_COMPLETE_write(writer, &opcodes->body.SMSG_DUEL_COMPLETE));
            break;
        case SMSG_DUEL_WINNER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_DUEL_WINNER_write(writer, &opcodes->body.SMSG_DUEL_WINNER));
            break;
        case SMSG_MOUNTRESULT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MOUNTRESULT_write(writer, &opcodes->body.SMSG_MOUNTRESULT));
            break;
        case SMSG_DISMOUNTRESULT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_DISMOUNTRESULT_write(writer, &opcodes->body.SMSG_DISMOUNTRESULT));
            break;
        case SMSG_MOUNTSPECIAL_ANIM:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MOUNTSPECIAL_ANIM_write(writer, &opcodes->body.SMSG_MOUNTSPECIAL_ANIM));
            break;
        case SMSG_PET_TAME_FAILURE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PET_TAME_FAILURE_write(writer, &opcodes->body.SMSG_PET_TAME_FAILURE));
            break;
        case SMSG_PET_SPELLS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PET_SPELLS_write(writer, &opcodes->body.SMSG_PET_SPELLS));
            break;
        case SMSG_PET_MODE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PET_MODE_write(writer, &opcodes->body.SMSG_PET_MODE));
            break;
        case SMSG_GOSSIP_MESSAGE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GOSSIP_MESSAGE_write(writer, &opcodes->body.SMSG_GOSSIP_MESSAGE));
            break;
        case SMSG_NPC_TEXT_UPDATE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_NPC_TEXT_UPDATE_write(writer, &opcodes->body.SMSG_NPC_TEXT_UPDATE));
            break;
        case SMSG_QUESTGIVER_STATUS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUESTGIVER_STATUS_write(writer, &opcodes->body.SMSG_QUESTGIVER_STATUS));
            break;
        case SMSG_QUESTGIVER_QUEST_LIST:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUESTGIVER_QUEST_LIST_write(writer, &opcodes->body.SMSG_QUESTGIVER_QUEST_LIST));
            break;
        case SMSG_QUESTGIVER_QUEST_DETAILS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUESTGIVER_QUEST_DETAILS_write(writer, &opcodes->body.SMSG_QUESTGIVER_QUEST_DETAILS));
            break;
        case SMSG_QUESTGIVER_REQUEST_ITEMS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUESTGIVER_REQUEST_ITEMS_write(writer, &opcodes->body.SMSG_QUESTGIVER_REQUEST_ITEMS));
            break;
        case SMSG_QUESTGIVER_OFFER_REWARD:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUESTGIVER_OFFER_REWARD_write(writer, &opcodes->body.SMSG_QUESTGIVER_OFFER_REWARD));
            break;
        case SMSG_QUESTGIVER_QUEST_INVALID:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUESTGIVER_QUEST_INVALID_write(writer, &opcodes->body.SMSG_QUESTGIVER_QUEST_INVALID));
            break;
        case SMSG_QUESTGIVER_QUEST_COMPLETE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUESTGIVER_QUEST_COMPLETE_write(writer, &opcodes->body.SMSG_QUESTGIVER_QUEST_COMPLETE));
            break;
        case SMSG_QUESTGIVER_QUEST_FAILED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUESTGIVER_QUEST_FAILED_write(writer, &opcodes->body.SMSG_QUESTGIVER_QUEST_FAILED));
            break;
        case SMSG_QUESTUPDATE_FAILED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUESTUPDATE_FAILED_write(writer, &opcodes->body.SMSG_QUESTUPDATE_FAILED));
            break;
        case SMSG_QUESTUPDATE_FAILEDTIMER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUESTUPDATE_FAILEDTIMER_write(writer, &opcodes->body.SMSG_QUESTUPDATE_FAILEDTIMER));
            break;
        case SMSG_QUESTUPDATE_COMPLETE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUESTUPDATE_COMPLETE_write(writer, &opcodes->body.SMSG_QUESTUPDATE_COMPLETE));
            break;
        case SMSG_QUESTUPDATE_ADD_KILL:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUESTUPDATE_ADD_KILL_write(writer, &opcodes->body.SMSG_QUESTUPDATE_ADD_KILL));
            break;
        case SMSG_QUESTUPDATE_ADD_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUESTUPDATE_ADD_ITEM_write(writer, &opcodes->body.SMSG_QUESTUPDATE_ADD_ITEM));
            break;
        case SMSG_QUEST_CONFIRM_ACCEPT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUEST_CONFIRM_ACCEPT_write(writer, &opcodes->body.SMSG_QUEST_CONFIRM_ACCEPT));
            break;
        case SMSG_LIST_INVENTORY:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LIST_INVENTORY_write(writer, &opcodes->body.SMSG_LIST_INVENTORY));
            break;
        case SMSG_SELL_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SELL_ITEM_write(writer, &opcodes->body.SMSG_SELL_ITEM));
            break;
        case SMSG_BUY_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_BUY_ITEM_write(writer, &opcodes->body.SMSG_BUY_ITEM));
            break;
        case SMSG_BUY_FAILED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_BUY_FAILED_write(writer, &opcodes->body.SMSG_BUY_FAILED));
            break;
        case SMSG_SHOWTAXINODES:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SHOWTAXINODES_write(writer, &opcodes->body.SMSG_SHOWTAXINODES));
            break;
        case SMSG_TAXINODE_STATUS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_TAXINODE_STATUS_write(writer, &opcodes->body.SMSG_TAXINODE_STATUS));
            break;
        case SMSG_ACTIVATETAXIREPLY:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ACTIVATETAXIREPLY_write(writer, &opcodes->body.SMSG_ACTIVATETAXIREPLY));
            break;
        case SMSG_TRAINER_LIST:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_TRAINER_LIST_write(writer, &opcodes->body.SMSG_TRAINER_LIST));
            break;
        case SMSG_TRAINER_BUY_SUCCEEDED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_TRAINER_BUY_SUCCEEDED_write(writer, &opcodes->body.SMSG_TRAINER_BUY_SUCCEEDED));
            break;
        case SMSG_TRAINER_BUY_FAILED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_TRAINER_BUY_FAILED_write(writer, &opcodes->body.SMSG_TRAINER_BUY_FAILED));
            break;
        case SMSG_SHOW_BANK:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SHOW_BANK_write(writer, &opcodes->body.SMSG_SHOW_BANK));
            break;
        case SMSG_BUY_BANK_SLOT_RESULT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_BUY_BANK_SLOT_RESULT_write(writer, &opcodes->body.SMSG_BUY_BANK_SLOT_RESULT));
            break;
        case SMSG_PETITION_SHOWLIST:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PETITION_SHOWLIST_write(writer, &opcodes->body.SMSG_PETITION_SHOWLIST));
            break;
        case SMSG_PETITION_SHOW_SIGNATURES:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PETITION_SHOW_SIGNATURES_write(writer, &opcodes->body.SMSG_PETITION_SHOW_SIGNATURES));
            break;
        case SMSG_PETITION_SIGN_RESULTS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PETITION_SIGN_RESULTS_write(writer, &opcodes->body.SMSG_PETITION_SIGN_RESULTS));
            break;
        case MSG_PETITION_DECLINE:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_PETITION_DECLINE_smsg_write(writer, &opcodes->body.MSG_PETITION_DECLINE));
            break;
        case SMSG_TURN_IN_PETITION_RESULTS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_TURN_IN_PETITION_RESULTS_write(writer, &opcodes->body.SMSG_TURN_IN_PETITION_RESULTS));
            break;
        case SMSG_PETITION_QUERY_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PETITION_QUERY_RESPONSE_write(writer, &opcodes->body.SMSG_PETITION_QUERY_RESPONSE));
            break;
        case SMSG_NOTIFICATION:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_NOTIFICATION_write(writer, &opcodes->body.SMSG_NOTIFICATION));
            break;
        case SMSG_PLAYED_TIME:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PLAYED_TIME_write(writer, &opcodes->body.SMSG_PLAYED_TIME));
            break;
        case SMSG_QUERY_TIME_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUERY_TIME_RESPONSE_write(writer, &opcodes->body.SMSG_QUERY_TIME_RESPONSE));
            break;
        case SMSG_LOG_XPGAIN:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LOG_XPGAIN_write(writer, &opcodes->body.SMSG_LOG_XPGAIN));
            break;
        case SMSG_LEVELUP_INFO:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LEVELUP_INFO_write(writer, &opcodes->body.SMSG_LEVELUP_INFO));
            break;
        case MSG_MINIMAP_PING:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MINIMAP_PING_Server_write(writer, &opcodes->body.MSG_MINIMAP_PING_Server));
            break;
        case SMSG_RESISTLOG:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_RESISTLOG_write(writer, &opcodes->body.SMSG_RESISTLOG));
            break;
        case SMSG_ENCHANTMENTLOG:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ENCHANTMENTLOG_write(writer, &opcodes->body.SMSG_ENCHANTMENTLOG));
            break;
        case SMSG_START_MIRROR_TIMER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_START_MIRROR_TIMER_write(writer, &opcodes->body.SMSG_START_MIRROR_TIMER));
            break;
        case SMSG_PAUSE_MIRROR_TIMER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PAUSE_MIRROR_TIMER_write(writer, &opcodes->body.SMSG_PAUSE_MIRROR_TIMER));
            break;
        case SMSG_STOP_MIRROR_TIMER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_STOP_MIRROR_TIMER_write(writer, &opcodes->body.SMSG_STOP_MIRROR_TIMER));
            break;
        case SMSG_PONG:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PONG_write(writer, &opcodes->body.SMSG_PONG));
            break;
        case SMSG_CLEAR_COOLDOWN:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_CLEAR_COOLDOWN_write(writer, &opcodes->body.SMSG_CLEAR_COOLDOWN));
            break;
        case SMSG_GAMEOBJECT_PAGETEXT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GAMEOBJECT_PAGETEXT_write(writer, &opcodes->body.SMSG_GAMEOBJECT_PAGETEXT));
            break;
        case SMSG_SPELL_DELAYED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELL_DELAYED_write(writer, &opcodes->body.SMSG_SPELL_DELAYED));
            break;
        case SMSG_ITEM_TIME_UPDATE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ITEM_TIME_UPDATE_write(writer, &opcodes->body.SMSG_ITEM_TIME_UPDATE));
            break;
        case SMSG_ITEM_ENCHANT_TIME_UPDATE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ITEM_ENCHANT_TIME_UPDATE_write(writer, &opcodes->body.SMSG_ITEM_ENCHANT_TIME_UPDATE));
            break;
        case SMSG_AUTH_CHALLENGE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_AUTH_CHALLENGE_write(writer, &opcodes->body.SMSG_AUTH_CHALLENGE));
            break;
        case SMSG_AUTH_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_AUTH_RESPONSE_write(writer, &opcodes->body.SMSG_AUTH_RESPONSE));
            break;
        case MSG_SAVE_GUILD_EMBLEM:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_SAVE_GUILD_EMBLEM_Server_write(writer, &opcodes->body.MSG_SAVE_GUILD_EMBLEM_Server));
            break;
        case MSG_TABARDVENDOR_ACTIVATE:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_TABARDVENDOR_ACTIVATE_smsg_write(writer, &opcodes->body.MSG_TABARDVENDOR_ACTIVATE));
            break;
        case SMSG_PLAY_SPELL_VISUAL:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PLAY_SPELL_VISUAL_write(writer, &opcodes->body.SMSG_PLAY_SPELL_VISUAL));
            break;
        case SMSG_PARTYKILLLOG:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PARTYKILLLOG_write(writer, &opcodes->body.SMSG_PARTYKILLLOG));
            break;
        case SMSG_COMPRESSED_UPDATE_OBJECT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_COMPRESSED_UPDATE_OBJECT_write(writer, &opcodes->body.SMSG_COMPRESSED_UPDATE_OBJECT));
            break;
        case SMSG_PLAY_SPELL_IMPACT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PLAY_SPELL_IMPACT_write(writer, &opcodes->body.SMSG_PLAY_SPELL_IMPACT));
            break;
        case SMSG_EXPLORATION_EXPERIENCE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_EXPLORATION_EXPERIENCE_write(writer, &opcodes->body.SMSG_EXPLORATION_EXPERIENCE));
            break;
        case MSG_RANDOM_ROLL:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_RANDOM_ROLL_Server_write(writer, &opcodes->body.MSG_RANDOM_ROLL_Server));
            break;
        case SMSG_ENVIRONMENTAL_DAMAGE_LOG:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ENVIRONMENTAL_DAMAGE_LOG_write(writer, &opcodes->body.SMSG_ENVIRONMENTAL_DAMAGE_LOG));
            break;
        case MSG_LOOKING_FOR_GROUP:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_LOOKING_FOR_GROUP_Server_write(writer, &opcodes->body.MSG_LOOKING_FOR_GROUP_Server));
            break;
        case SMSG_REMOVED_SPELL:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_REMOVED_SPELL_write(writer, &opcodes->body.SMSG_REMOVED_SPELL));
            break;
        case SMSG_GMTICKET_CREATE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GMTICKET_CREATE_write(writer, &opcodes->body.SMSG_GMTICKET_CREATE));
            break;
        case SMSG_GMTICKET_UPDATETEXT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GMTICKET_UPDATETEXT_write(writer, &opcodes->body.SMSG_GMTICKET_UPDATETEXT));
            break;
        case SMSG_ACCOUNT_DATA_TIMES:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ACCOUNT_DATA_TIMES_write(writer, &opcodes->body.SMSG_ACCOUNT_DATA_TIMES));
            break;
        case SMSG_GMTICKET_GETTICKET:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GMTICKET_GETTICKET_write(writer, &opcodes->body.SMSG_GMTICKET_GETTICKET));
            break;
        case SMSG_GAMEOBJECT_SPAWN_ANIM:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GAMEOBJECT_SPAWN_ANIM_write(writer, &opcodes->body.SMSG_GAMEOBJECT_SPAWN_ANIM));
            break;
        case SMSG_GAMEOBJECT_DESPAWN_ANIM:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GAMEOBJECT_DESPAWN_ANIM_write(writer, &opcodes->body.SMSG_GAMEOBJECT_DESPAWN_ANIM));
            break;
        case MSG_CORPSE_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_CORPSE_QUERY_Server_write(writer, &opcodes->body.MSG_CORPSE_QUERY_Server));
            break;
        case SMSG_GMTICKET_DELETETICKET:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GMTICKET_DELETETICKET_write(writer, &opcodes->body.SMSG_GMTICKET_DELETETICKET));
            break;
        case SMSG_GMTICKET_SYSTEMSTATUS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GMTICKET_SYSTEMSTATUS_write(writer, &opcodes->body.SMSG_GMTICKET_SYSTEMSTATUS));
            break;
        case SMSG_SET_REST_START:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SET_REST_START_write(writer, &opcodes->body.SMSG_SET_REST_START));
            break;
        case SMSG_SPIRIT_HEALER_CONFIRM:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPIRIT_HEALER_CONFIRM_write(writer, &opcodes->body.SMSG_SPIRIT_HEALER_CONFIRM));
            break;
        case SMSG_GOSSIP_POI:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GOSSIP_POI_write(writer, &opcodes->body.SMSG_GOSSIP_POI));
            break;
        case SMSG_LOGIN_VERIFY_WORLD:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LOGIN_VERIFY_WORLD_write(writer, &opcodes->body.SMSG_LOGIN_VERIFY_WORLD));
            break;
        case SMSG_SEND_MAIL_RESULT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SEND_MAIL_RESULT_write(writer, &opcodes->body.SMSG_SEND_MAIL_RESULT));
            break;
        case SMSG_MAIL_LIST_RESULT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MAIL_LIST_RESULT_write(writer, &opcodes->body.SMSG_MAIL_LIST_RESULT));
            break;
        case SMSG_BATTLEFIELD_LIST:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_BATTLEFIELD_LIST_write(writer, &opcodes->body.SMSG_BATTLEFIELD_LIST));
            break;
        case SMSG_ITEM_TEXT_QUERY_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ITEM_TEXT_QUERY_RESPONSE_write(writer, &opcodes->body.SMSG_ITEM_TEXT_QUERY_RESPONSE));
            break;
        case SMSG_SPELLLOGMISS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELLLOGMISS_write(writer, &opcodes->body.SMSG_SPELLLOGMISS));
            break;
        case SMSG_SPELLLOGEXECUTE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELLLOGEXECUTE_write(writer, &opcodes->body.SMSG_SPELLLOGEXECUTE));
            break;
        case SMSG_PERIODICAURALOG:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PERIODICAURALOG_write(writer, &opcodes->body.SMSG_PERIODICAURALOG));
            break;
        case SMSG_SPELLDAMAGESHIELD:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELLDAMAGESHIELD_write(writer, &opcodes->body.SMSG_SPELLDAMAGESHIELD));
            break;
        case SMSG_SPELLNONMELEEDAMAGELOG:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELLNONMELEEDAMAGELOG_write(writer, &opcodes->body.SMSG_SPELLNONMELEEDAMAGELOG));
            break;
        case SMSG_ZONE_UNDER_ATTACK:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ZONE_UNDER_ATTACK_write(writer, &opcodes->body.SMSG_ZONE_UNDER_ATTACK));
            break;
        case MSG_AUCTION_HELLO:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_AUCTION_HELLO_Server_write(writer, &opcodes->body.MSG_AUCTION_HELLO_Server));
            break;
        case SMSG_AUCTION_COMMAND_RESULT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_AUCTION_COMMAND_RESULT_write(writer, &opcodes->body.SMSG_AUCTION_COMMAND_RESULT));
            break;
        case SMSG_AUCTION_LIST_RESULT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_AUCTION_LIST_RESULT_write(writer, &opcodes->body.SMSG_AUCTION_LIST_RESULT));
            break;
        case SMSG_AUCTION_OWNER_LIST_RESULT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_AUCTION_OWNER_LIST_RESULT_write(writer, &opcodes->body.SMSG_AUCTION_OWNER_LIST_RESULT));
            break;
        case SMSG_AUCTION_BIDDER_NOTIFICATION:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_AUCTION_BIDDER_NOTIFICATION_write(writer, &opcodes->body.SMSG_AUCTION_BIDDER_NOTIFICATION));
            break;
        case SMSG_AUCTION_OWNER_NOTIFICATION:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_AUCTION_OWNER_NOTIFICATION_write(writer, &opcodes->body.SMSG_AUCTION_OWNER_NOTIFICATION));
            break;
        case SMSG_PROCRESIST:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PROCRESIST_write(writer, &opcodes->body.SMSG_PROCRESIST));
            break;
        case SMSG_DISPEL_FAILED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_DISPEL_FAILED_write(writer, &opcodes->body.SMSG_DISPEL_FAILED));
            break;
        case SMSG_SPELLORDAMAGE_IMMUNE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELLORDAMAGE_IMMUNE_write(writer, &opcodes->body.SMSG_SPELLORDAMAGE_IMMUNE));
            break;
        case SMSG_AUCTION_BIDDER_LIST_RESULT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_AUCTION_BIDDER_LIST_RESULT_write(writer, &opcodes->body.SMSG_AUCTION_BIDDER_LIST_RESULT));
            break;
        case SMSG_SET_FLAT_SPELL_MODIFIER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SET_FLAT_SPELL_MODIFIER_write(writer, &opcodes->body.SMSG_SET_FLAT_SPELL_MODIFIER));
            break;
        case SMSG_SET_PCT_SPELL_MODIFIER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SET_PCT_SPELL_MODIFIER_write(writer, &opcodes->body.SMSG_SET_PCT_SPELL_MODIFIER));
            break;
        case SMSG_CORPSE_RECLAIM_DELAY:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_CORPSE_RECLAIM_DELAY_write(writer, &opcodes->body.SMSG_CORPSE_RECLAIM_DELAY));
            break;
        case MSG_LIST_STABLED_PETS:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_LIST_STABLED_PETS_Server_write(writer, &opcodes->body.MSG_LIST_STABLED_PETS_Server));
            break;
        case SMSG_STABLE_RESULT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_STABLE_RESULT_write(writer, &opcodes->body.SMSG_STABLE_RESULT));
            break;
        case MSG_QUEST_PUSH_RESULT:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_QUEST_PUSH_RESULT_smsg_write(writer, &opcodes->body.MSG_QUEST_PUSH_RESULT));
            break;
        case SMSG_PLAY_MUSIC:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PLAY_MUSIC_write(writer, &opcodes->body.SMSG_PLAY_MUSIC));
            break;
        case SMSG_PLAY_OBJECT_SOUND:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PLAY_OBJECT_SOUND_write(writer, &opcodes->body.SMSG_PLAY_OBJECT_SOUND));
            break;
        case SMSG_SPELLDISPELLOG:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELLDISPELLOG_write(writer, &opcodes->body.SMSG_SPELLDISPELLOG));
            break;
        case MSG_QUERY_NEXT_MAIL_TIME:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_QUERY_NEXT_MAIL_TIME_Server_write(writer, &opcodes->body.MSG_QUERY_NEXT_MAIL_TIME_Server));
            break;
        case SMSG_RECEIVED_MAIL:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_RECEIVED_MAIL_write(writer, &opcodes->body.SMSG_RECEIVED_MAIL));
            break;
        case SMSG_RAID_GROUP_ONLY:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_RAID_GROUP_ONLY_write(writer, &opcodes->body.SMSG_RAID_GROUP_ONLY));
            break;
        case SMSG_PVP_CREDIT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PVP_CREDIT_write(writer, &opcodes->body.SMSG_PVP_CREDIT));
            break;
        case SMSG_AUCTION_REMOVED_NOTIFICATION:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_AUCTION_REMOVED_NOTIFICATION_write(writer, &opcodes->body.SMSG_AUCTION_REMOVED_NOTIFICATION));
            break;
        case SMSG_SERVER_MESSAGE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SERVER_MESSAGE_write(writer, &opcodes->body.SMSG_SERVER_MESSAGE));
            break;
        case SMSG_MEETINGSTONE_SETQUEUE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MEETINGSTONE_SETQUEUE_write(writer, &opcodes->body.SMSG_MEETINGSTONE_SETQUEUE));
            break;
        case SMSG_MEETINGSTONE_MEMBER_ADDED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MEETINGSTONE_MEMBER_ADDED_write(writer, &opcodes->body.SMSG_MEETINGSTONE_MEMBER_ADDED));
            break;
        case SMSG_STANDSTATE_UPDATE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_STANDSTATE_UPDATE_write(writer, &opcodes->body.SMSG_STANDSTATE_UPDATE));
            break;
        case SMSG_LOOT_ALL_PASSED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LOOT_ALL_PASSED_write(writer, &opcodes->body.SMSG_LOOT_ALL_PASSED));
            break;
        case SMSG_LOOT_ROLL_WON:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LOOT_ROLL_WON_write(writer, &opcodes->body.SMSG_LOOT_ROLL_WON));
            break;
        case SMSG_LOOT_START_ROLL:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LOOT_START_ROLL_write(writer, &opcodes->body.SMSG_LOOT_START_ROLL));
            break;
        case SMSG_LOOT_ROLL:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LOOT_ROLL_write(writer, &opcodes->body.SMSG_LOOT_ROLL));
            break;
        case SMSG_LOOT_MASTER_LIST:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LOOT_MASTER_LIST_write(writer, &opcodes->body.SMSG_LOOT_MASTER_LIST));
            break;
        case SMSG_SET_FORCED_REACTIONS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SET_FORCED_REACTIONS_write(writer, &opcodes->body.SMSG_SET_FORCED_REACTIONS));
            break;
        case SMSG_SPELL_FAILED_OTHER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELL_FAILED_OTHER_write(writer, &opcodes->body.SMSG_SPELL_FAILED_OTHER));
            break;
        case SMSG_GAMEOBJECT_RESET_STATE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GAMEOBJECT_RESET_STATE_write(writer, &opcodes->body.SMSG_GAMEOBJECT_RESET_STATE));
            break;
        case SMSG_CHAT_PLAYER_NOT_FOUND:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_CHAT_PLAYER_NOT_FOUND_write(writer, &opcodes->body.SMSG_CHAT_PLAYER_NOT_FOUND));
            break;
        case MSG_TALENT_WIPE_CONFIRM:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_TALENT_WIPE_CONFIRM_Server_write(writer, &opcodes->body.MSG_TALENT_WIPE_CONFIRM_Server));
            break;
        case SMSG_SUMMON_REQUEST:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SUMMON_REQUEST_write(writer, &opcodes->body.SMSG_SUMMON_REQUEST));
            break;
        case SMSG_MONSTER_MOVE_TRANSPORT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MONSTER_MOVE_TRANSPORT_write(writer, &opcodes->body.SMSG_MONSTER_MOVE_TRANSPORT));
            break;
        case MSG_MOVE_FEATHER_FALL:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_FEATHER_FALL_Server_write(writer, &opcodes->body.MSG_MOVE_FEATHER_FALL_Server));
            break;
        case MSG_MOVE_WATER_WALK:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_WATER_WALK_smsg_write(writer, &opcodes->body.MSG_MOVE_WATER_WALK));
            break;
        case SMSG_DUEL_COUNTDOWN:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_DUEL_COUNTDOWN_write(writer, &opcodes->body.SMSG_DUEL_COUNTDOWN));
            break;
        case SMSG_AREA_TRIGGER_MESSAGE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_AREA_TRIGGER_MESSAGE_write(writer, &opcodes->body.SMSG_AREA_TRIGGER_MESSAGE));
            break;
        case SMSG_MEETINGSTONE_JOINFAILED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MEETINGSTONE_JOINFAILED_write(writer, &opcodes->body.SMSG_MEETINGSTONE_JOINFAILED));
            break;
        case SMSG_PLAYER_SKINNED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PLAYER_SKINNED_write(writer, &opcodes->body.SMSG_PLAYER_SKINNED));
            break;
        case MSG_PETITION_RENAME:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_PETITION_RENAME_smsg_write(writer, &opcodes->body.MSG_PETITION_RENAME));
            break;
        case SMSG_INIT_WORLD_STATES:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_INIT_WORLD_STATES_write(writer, &opcodes->body.SMSG_INIT_WORLD_STATES));
            break;
        case SMSG_UPDATE_WORLD_STATE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_UPDATE_WORLD_STATE_write(writer, &opcodes->body.SMSG_UPDATE_WORLD_STATE));
            break;
        case SMSG_ITEM_NAME_QUERY_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ITEM_NAME_QUERY_RESPONSE_write(writer, &opcodes->body.SMSG_ITEM_NAME_QUERY_RESPONSE));
            break;
        case SMSG_PET_ACTION_FEEDBACK:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PET_ACTION_FEEDBACK_write(writer, &opcodes->body.SMSG_PET_ACTION_FEEDBACK));
            break;
        case SMSG_CHAR_RENAME:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_CHAR_RENAME_write(writer, &opcodes->body.SMSG_CHAR_RENAME));
            break;
        case SMSG_INSTANCE_SAVE_CREATED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_INSTANCE_SAVE_CREATED_write(writer, &opcodes->body.SMSG_INSTANCE_SAVE_CREATED));
            break;
        case SMSG_RAID_INSTANCE_INFO:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_RAID_INSTANCE_INFO_write(writer, &opcodes->body.SMSG_RAID_INSTANCE_INFO));
            break;
        case SMSG_PLAY_SOUND:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PLAY_SOUND_write(writer, &opcodes->body.SMSG_PLAY_SOUND));
            break;
        case SMSG_BATTLEFIELD_STATUS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_BATTLEFIELD_STATUS_write(writer, &opcodes->body.SMSG_BATTLEFIELD_STATUS));
            break;
        case MSG_INSPECT_HONOR_STATS:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_INSPECT_HONOR_STATS_Server_write(writer, &opcodes->body.MSG_INSPECT_HONOR_STATS_Server));
            break;
        case SMSG_FORCE_WALK_SPEED_CHANGE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_FORCE_WALK_SPEED_CHANGE_write(writer, &opcodes->body.SMSG_FORCE_WALK_SPEED_CHANGE));
            break;
        case SMSG_FORCE_SWIM_BACK_SPEED_CHANGE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_FORCE_SWIM_BACK_SPEED_CHANGE_write(writer, &opcodes->body.SMSG_FORCE_SWIM_BACK_SPEED_CHANGE));
            break;
        case SMSG_FORCE_TURN_RATE_CHANGE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_FORCE_TURN_RATE_CHANGE_write(writer, &opcodes->body.SMSG_FORCE_TURN_RATE_CHANGE));
            break;
        case MSG_PVP_LOG_DATA:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_PVP_LOG_DATA_Server_write(writer, &opcodes->body.MSG_PVP_LOG_DATA_Server));
            break;
        case SMSG_AREA_SPIRIT_HEALER_TIME:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_AREA_SPIRIT_HEALER_TIME_write(writer, &opcodes->body.SMSG_AREA_SPIRIT_HEALER_TIME));
            break;
        case SMSG_WARDEN_DATA:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_WARDEN_DATA_write(writer, &opcodes->body.SMSG_WARDEN_DATA));
            break;
        case SMSG_GROUP_JOINED_BATTLEGROUND:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GROUP_JOINED_BATTLEGROUND_write(writer, &opcodes->body.SMSG_GROUP_JOINED_BATTLEGROUND));
            break;
        case MSG_BATTLEGROUND_PLAYER_POSITIONS:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_BATTLEGROUND_PLAYER_POSITIONS_Server_write(writer, &opcodes->body.MSG_BATTLEGROUND_PLAYER_POSITIONS_Server));
            break;
        case SMSG_BINDER_CONFIRM:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_BINDER_CONFIRM_write(writer, &opcodes->body.SMSG_BINDER_CONFIRM));
            break;
        case SMSG_BATTLEGROUND_PLAYER_JOINED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_BATTLEGROUND_PLAYER_JOINED_write(writer, &opcodes->body.SMSG_BATTLEGROUND_PLAYER_JOINED));
            break;
        case SMSG_BATTLEGROUND_PLAYER_LEFT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_BATTLEGROUND_PLAYER_LEFT_write(writer, &opcodes->body.SMSG_BATTLEGROUND_PLAYER_LEFT));
            break;
        case SMSG_ADDON_INFO:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ADDON_INFO_write(writer, &opcodes->body.SMSG_ADDON_INFO));
            break;
        case SMSG_PET_UNLEARN_CONFIRM:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PET_UNLEARN_CONFIRM_write(writer, &opcodes->body.SMSG_PET_UNLEARN_CONFIRM));
            break;
        case SMSG_PARTY_MEMBER_STATS_FULL:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PARTY_MEMBER_STATS_FULL_write(writer, &opcodes->body.SMSG_PARTY_MEMBER_STATS_FULL));
            break;
        case SMSG_WEATHER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_WEATHER_write(writer, &opcodes->body.SMSG_WEATHER));
            break;
        case SMSG_RAID_INSTANCE_MESSAGE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_RAID_INSTANCE_MESSAGE_write(writer, &opcodes->body.SMSG_RAID_INSTANCE_MESSAGE));
            break;
        case SMSG_COMPRESSED_MOVES:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_COMPRESSED_MOVES_write(writer, &opcodes->body.SMSG_COMPRESSED_MOVES));
            break;
        case SMSG_SPLINE_SET_RUN_SPEED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_SET_RUN_SPEED_write(writer, &opcodes->body.SMSG_SPLINE_SET_RUN_SPEED));
            break;
        case SMSG_SPLINE_SET_RUN_BACK_SPEED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_SET_RUN_BACK_SPEED_write(writer, &opcodes->body.SMSG_SPLINE_SET_RUN_BACK_SPEED));
            break;
        case SMSG_SPLINE_SET_SWIM_SPEED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_SET_SWIM_SPEED_write(writer, &opcodes->body.SMSG_SPLINE_SET_SWIM_SPEED));
            break;
        case SMSG_SPLINE_SET_WALK_SPEED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_SET_WALK_SPEED_write(writer, &opcodes->body.SMSG_SPLINE_SET_WALK_SPEED));
            break;
        case SMSG_SPLINE_SET_SWIM_BACK_SPEED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_SET_SWIM_BACK_SPEED_write(writer, &opcodes->body.SMSG_SPLINE_SET_SWIM_BACK_SPEED));
            break;
        case SMSG_SPLINE_SET_TURN_RATE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_SET_TURN_RATE_write(writer, &opcodes->body.SMSG_SPLINE_SET_TURN_RATE));
            break;
        case SMSG_SPLINE_MOVE_UNROOT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_MOVE_UNROOT_write(writer, &opcodes->body.SMSG_SPLINE_MOVE_UNROOT));
            break;
        case SMSG_SPLINE_MOVE_FEATHER_FALL:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_MOVE_FEATHER_FALL_write(writer, &opcodes->body.SMSG_SPLINE_MOVE_FEATHER_FALL));
            break;
        case SMSG_SPLINE_MOVE_NORMAL_FALL:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_MOVE_NORMAL_FALL_write(writer, &opcodes->body.SMSG_SPLINE_MOVE_NORMAL_FALL));
            break;
        case SMSG_SPLINE_MOVE_SET_HOVER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_MOVE_SET_HOVER_write(writer, &opcodes->body.SMSG_SPLINE_MOVE_SET_HOVER));
            break;
        case SMSG_SPLINE_MOVE_UNSET_HOVER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_MOVE_UNSET_HOVER_write(writer, &opcodes->body.SMSG_SPLINE_MOVE_UNSET_HOVER));
            break;
        case SMSG_SPLINE_MOVE_WATER_WALK:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_MOVE_WATER_WALK_write(writer, &opcodes->body.SMSG_SPLINE_MOVE_WATER_WALK));
            break;
        case SMSG_SPLINE_MOVE_LAND_WALK:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_MOVE_LAND_WALK_write(writer, &opcodes->body.SMSG_SPLINE_MOVE_LAND_WALK));
            break;
        case SMSG_SPLINE_MOVE_START_SWIM:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_MOVE_START_SWIM_write(writer, &opcodes->body.SMSG_SPLINE_MOVE_START_SWIM));
            break;
        case SMSG_SPLINE_MOVE_STOP_SWIM:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_MOVE_STOP_SWIM_write(writer, &opcodes->body.SMSG_SPLINE_MOVE_STOP_SWIM));
            break;
        case SMSG_SPLINE_MOVE_SET_RUN_MODE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_MOVE_SET_RUN_MODE_write(writer, &opcodes->body.SMSG_SPLINE_MOVE_SET_RUN_MODE));
            break;
        case SMSG_SPLINE_MOVE_SET_WALK_MODE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_MOVE_SET_WALK_MODE_write(writer, &opcodes->body.SMSG_SPLINE_MOVE_SET_WALK_MODE));
            break;
        case MSG_MOVE_TIME_SKIPPED:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_TIME_SKIPPED_Server_write(writer, &opcodes->body.MSG_MOVE_TIME_SKIPPED_Server));
            break;
        case SMSG_SPLINE_MOVE_ROOT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_MOVE_ROOT_write(writer, &opcodes->body.SMSG_SPLINE_MOVE_ROOT));
            break;
        case SMSG_INVALIDATE_PLAYER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_INVALIDATE_PLAYER_write(writer, &opcodes->body.SMSG_INVALIDATE_PLAYER));
            break;
        case SMSG_INSTANCE_RESET:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_INSTANCE_RESET_write(writer, &opcodes->body.SMSG_INSTANCE_RESET));
            break;
        case SMSG_INSTANCE_RESET_FAILED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_INSTANCE_RESET_FAILED_write(writer, &opcodes->body.SMSG_INSTANCE_RESET_FAILED));
            break;
        case SMSG_UPDATE_LAST_INSTANCE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_UPDATE_LAST_INSTANCE_write(writer, &opcodes->body.SMSG_UPDATE_LAST_INSTANCE));
            break;
        case MSG_RAID_TARGET_UPDATE:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_RAID_TARGET_UPDATE_Server_write(writer, &opcodes->body.MSG_RAID_TARGET_UPDATE_Server));
            break;
        case MSG_RAID_READY_CHECK:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_RAID_READY_CHECK_Server_write(writer, &opcodes->body.MSG_RAID_READY_CHECK_Server));
            break;
        case SMSG_PET_ACTION_SOUND:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PET_ACTION_SOUND_write(writer, &opcodes->body.SMSG_PET_ACTION_SOUND));
            break;
        case SMSG_PET_DISMISS_SOUND:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PET_DISMISS_SOUND_write(writer, &opcodes->body.SMSG_PET_DISMISS_SOUND));
            break;
        case SMSG_GM_TICKET_STATUS_UPDATE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GM_TICKET_STATUS_UPDATE_write(writer, &opcodes->body.SMSG_GM_TICKET_STATUS_UPDATE));
            break;
        case SMSG_UPDATE_INSTANCE_OWNERSHIP:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_UPDATE_INSTANCE_OWNERSHIP_write(writer, &opcodes->body.SMSG_UPDATE_INSTANCE_OWNERSHIP));
            break;
        case SMSG_SPELLINSTAKILLLOG:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELLINSTAKILLLOG_write(writer, &opcodes->body.SMSG_SPELLINSTAKILLLOG));
            break;
        case SMSG_SPELL_UPDATE_CHAIN_TARGETS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELL_UPDATE_CHAIN_TARGETS_write(writer, &opcodes->body.SMSG_SPELL_UPDATE_CHAIN_TARGETS));
            break;
        case SMSG_EXPECTED_SPAM_RECORDS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_EXPECTED_SPAM_RECORDS_write(writer, &opcodes->body.SMSG_EXPECTED_SPAM_RECORDS));
            break;
        case SMSG_DEFENSE_MESSAGE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_DEFENSE_MESSAGE_write(writer, &opcodes->body.SMSG_DEFENSE_MESSAGE));
            break;
        default:
            break;
    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT WowWorldResult vanilla_server_opcode_read(WowWorldReader* reader, VanillaServerOpcodeContainer* opcodes) {
    uint16_t _size;
    READ_U16_BE(_size);

    opcodes->opcode = 0;
    READ_U16(opcodes->opcode);

    switch (opcodes->opcode) {
        case SMSG_CHAR_CREATE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_CHAR_CREATE_read(reader, &opcodes->body.SMSG_CHAR_CREATE));
            break;
        case SMSG_CHAR_ENUM:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_CHAR_ENUM_read(reader, &opcodes->body.SMSG_CHAR_ENUM));
            break;
        case SMSG_CHAR_DELETE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_CHAR_DELETE_read(reader, &opcodes->body.SMSG_CHAR_DELETE));
            break;
        case SMSG_NEW_WORLD:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_NEW_WORLD_read(reader, &opcodes->body.SMSG_NEW_WORLD));
            break;
        case SMSG_TRANSFER_PENDING:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_TRANSFER_PENDING_read(reader, &opcodes->body.SMSG_TRANSFER_PENDING, _size - 2));
            break;
        case SMSG_TRANSFER_ABORTED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_TRANSFER_ABORTED_read(reader, &opcodes->body.SMSG_TRANSFER_ABORTED));
            break;
        case SMSG_CHARACTER_LOGIN_FAILED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_CHARACTER_LOGIN_FAILED_read(reader, &opcodes->body.SMSG_CHARACTER_LOGIN_FAILED));
            break;
        case SMSG_LOGIN_SETTIMESPEED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LOGIN_SETTIMESPEED_read(reader, &opcodes->body.SMSG_LOGIN_SETTIMESPEED));
            break;
        case SMSG_LOGOUT_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LOGOUT_RESPONSE_read(reader, &opcodes->body.SMSG_LOGOUT_RESPONSE));
            break;
        case SMSG_NAME_QUERY_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_NAME_QUERY_RESPONSE_read(reader, &opcodes->body.SMSG_NAME_QUERY_RESPONSE));
            break;
        case SMSG_PET_NAME_QUERY_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PET_NAME_QUERY_RESPONSE_read(reader, &opcodes->body.SMSG_PET_NAME_QUERY_RESPONSE));
            break;
        case SMSG_GUILD_QUERY_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GUILD_QUERY_RESPONSE_read(reader, &opcodes->body.SMSG_GUILD_QUERY_RESPONSE));
            break;
        case SMSG_ITEM_QUERY_SINGLE_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ITEM_QUERY_SINGLE_RESPONSE_read(reader, &opcodes->body.SMSG_ITEM_QUERY_SINGLE_RESPONSE, _size - 2));
            break;
        case SMSG_PAGE_TEXT_QUERY_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PAGE_TEXT_QUERY_RESPONSE_read(reader, &opcodes->body.SMSG_PAGE_TEXT_QUERY_RESPONSE));
            break;
        case SMSG_QUEST_QUERY_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUEST_QUERY_RESPONSE_read(reader, &opcodes->body.SMSG_QUEST_QUERY_RESPONSE));
            break;
        case SMSG_GAMEOBJECT_QUERY_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GAMEOBJECT_QUERY_RESPONSE_read(reader, &opcodes->body.SMSG_GAMEOBJECT_QUERY_RESPONSE, _size - 2));
            break;
        case SMSG_CREATURE_QUERY_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_CREATURE_QUERY_RESPONSE_read(reader, &opcodes->body.SMSG_CREATURE_QUERY_RESPONSE, _size - 2));
            break;
        case SMSG_WHO:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_WHO_read(reader, &opcodes->body.SMSG_WHO));
            break;
        case SMSG_WHOIS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_WHOIS_read(reader, &opcodes->body.SMSG_WHOIS));
            break;
        case SMSG_FRIEND_LIST:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_FRIEND_LIST_read(reader, &opcodes->body.SMSG_FRIEND_LIST));
            break;
        case SMSG_FRIEND_STATUS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_FRIEND_STATUS_read(reader, &opcodes->body.SMSG_FRIEND_STATUS));
            break;
        case SMSG_IGNORE_LIST:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_IGNORE_LIST_read(reader, &opcodes->body.SMSG_IGNORE_LIST));
            break;
        case SMSG_GROUP_INVITE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GROUP_INVITE_read(reader, &opcodes->body.SMSG_GROUP_INVITE));
            break;
        case SMSG_GROUP_DECLINE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GROUP_DECLINE_read(reader, &opcodes->body.SMSG_GROUP_DECLINE));
            break;
        case SMSG_GROUP_SET_LEADER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GROUP_SET_LEADER_read(reader, &opcodes->body.SMSG_GROUP_SET_LEADER));
            break;
        case SMSG_GROUP_LIST:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GROUP_LIST_read(reader, &opcodes->body.SMSG_GROUP_LIST, _size - 2));
            break;
        case SMSG_PARTY_MEMBER_STATS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PARTY_MEMBER_STATS_read(reader, &opcodes->body.SMSG_PARTY_MEMBER_STATS));
            break;
        case SMSG_PARTY_COMMAND_RESULT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PARTY_COMMAND_RESULT_read(reader, &opcodes->body.SMSG_PARTY_COMMAND_RESULT));
            break;
        case SMSG_GUILD_INVITE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GUILD_INVITE_read(reader, &opcodes->body.SMSG_GUILD_INVITE));
            break;
        case SMSG_GUILD_INFO:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GUILD_INFO_read(reader, &opcodes->body.SMSG_GUILD_INFO));
            break;
        case SMSG_GUILD_ROSTER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GUILD_ROSTER_read(reader, &opcodes->body.SMSG_GUILD_ROSTER));
            break;
        case SMSG_GUILD_EVENT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GUILD_EVENT_read(reader, &opcodes->body.SMSG_GUILD_EVENT));
            break;
        case SMSG_GUILD_COMMAND_RESULT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GUILD_COMMAND_RESULT_read(reader, &opcodes->body.SMSG_GUILD_COMMAND_RESULT));
            break;
        case SMSG_MESSAGECHAT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MESSAGECHAT_read(reader, &opcodes->body.SMSG_MESSAGECHAT));
            break;
        case SMSG_CHANNEL_NOTIFY:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_CHANNEL_NOTIFY_read(reader, &opcodes->body.SMSG_CHANNEL_NOTIFY));
            break;
        case SMSG_CHANNEL_LIST:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_CHANNEL_LIST_read(reader, &opcodes->body.SMSG_CHANNEL_LIST));
            break;
        case SMSG_UPDATE_OBJECT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_UPDATE_OBJECT_read(reader, &opcodes->body.SMSG_UPDATE_OBJECT));
            break;
        case SMSG_DESTROY_OBJECT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_DESTROY_OBJECT_read(reader, &opcodes->body.SMSG_DESTROY_OBJECT));
            break;
        case SMSG_READ_ITEM_OK:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_READ_ITEM_OK_read(reader, &opcodes->body.SMSG_READ_ITEM_OK));
            break;
        case SMSG_READ_ITEM_FAILED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_READ_ITEM_FAILED_read(reader, &opcodes->body.SMSG_READ_ITEM_FAILED));
            break;
        case SMSG_ITEM_COOLDOWN:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ITEM_COOLDOWN_read(reader, &opcodes->body.SMSG_ITEM_COOLDOWN));
            break;
        case SMSG_GAMEOBJECT_CUSTOM_ANIM:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GAMEOBJECT_CUSTOM_ANIM_read(reader, &opcodes->body.SMSG_GAMEOBJECT_CUSTOM_ANIM));
            break;
        case MSG_MOVE_START_FORWARD:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_FORWARD_Server_read(reader, &opcodes->body.MSG_MOVE_START_FORWARD_Server));
            break;
        case MSG_MOVE_START_BACKWARD:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_BACKWARD_Server_read(reader, &opcodes->body.MSG_MOVE_START_BACKWARD_Server));
            break;
        case MSG_MOVE_STOP:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_STOP_Server_read(reader, &opcodes->body.MSG_MOVE_STOP_Server));
            break;
        case MSG_MOVE_START_STRAFE_LEFT:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_STRAFE_LEFT_Server_read(reader, &opcodes->body.MSG_MOVE_START_STRAFE_LEFT_Server));
            break;
        case MSG_MOVE_START_STRAFE_RIGHT:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_STRAFE_RIGHT_Server_read(reader, &opcodes->body.MSG_MOVE_START_STRAFE_RIGHT_Server));
            break;
        case MSG_MOVE_STOP_STRAFE:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_STOP_STRAFE_Server_read(reader, &opcodes->body.MSG_MOVE_STOP_STRAFE_Server));
            break;
        case MSG_MOVE_JUMP:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_JUMP_Server_read(reader, &opcodes->body.MSG_MOVE_JUMP_Server));
            break;
        case MSG_MOVE_START_TURN_LEFT:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_TURN_LEFT_Server_read(reader, &opcodes->body.MSG_MOVE_START_TURN_LEFT_Server));
            break;
        case MSG_MOVE_START_TURN_RIGHT:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_TURN_RIGHT_Server_read(reader, &opcodes->body.MSG_MOVE_START_TURN_RIGHT_Server));
            break;
        case MSG_MOVE_STOP_TURN:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_STOP_TURN_Server_read(reader, &opcodes->body.MSG_MOVE_STOP_TURN_Server));
            break;
        case MSG_MOVE_START_PITCH_UP:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_PITCH_UP_Server_read(reader, &opcodes->body.MSG_MOVE_START_PITCH_UP_Server));
            break;
        case MSG_MOVE_START_PITCH_DOWN:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_PITCH_DOWN_Server_read(reader, &opcodes->body.MSG_MOVE_START_PITCH_DOWN_Server));
            break;
        case MSG_MOVE_STOP_PITCH:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_STOP_PITCH_Server_read(reader, &opcodes->body.MSG_MOVE_STOP_PITCH_Server));
            break;
        case MSG_MOVE_SET_RUN_MODE:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_SET_RUN_MODE_Server_read(reader, &opcodes->body.MSG_MOVE_SET_RUN_MODE_Server));
            break;
        case MSG_MOVE_SET_WALK_MODE:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_SET_WALK_MODE_Server_read(reader, &opcodes->body.MSG_MOVE_SET_WALK_MODE_Server));
            break;
        case MSG_MOVE_TELEPORT_ACK:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_TELEPORT_ACK_Server_read(reader, &opcodes->body.MSG_MOVE_TELEPORT_ACK_Server));
            break;
        case MSG_MOVE_FALL_LAND:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_FALL_LAND_Server_read(reader, &opcodes->body.MSG_MOVE_FALL_LAND_Server));
            break;
        case MSG_MOVE_START_SWIM:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_START_SWIM_Server_read(reader, &opcodes->body.MSG_MOVE_START_SWIM_Server));
            break;
        case MSG_MOVE_STOP_SWIM:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_STOP_SWIM_Server_read(reader, &opcodes->body.MSG_MOVE_STOP_SWIM_Server));
            break;
        case MSG_MOVE_SET_FACING:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_SET_FACING_Server_read(reader, &opcodes->body.MSG_MOVE_SET_FACING_Server));
            break;
        case MSG_MOVE_SET_PITCH:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_SET_PITCH_Server_read(reader, &opcodes->body.MSG_MOVE_SET_PITCH_Server));
            break;
        case SMSG_MONSTER_MOVE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MONSTER_MOVE_read(reader, &opcodes->body.SMSG_MONSTER_MOVE));
            break;
        case SMSG_MOVE_WATER_WALK:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MOVE_WATER_WALK_read(reader, &opcodes->body.SMSG_MOVE_WATER_WALK));
            break;
        case SMSG_MOVE_LAND_WALK:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MOVE_LAND_WALK_read(reader, &opcodes->body.SMSG_MOVE_LAND_WALK));
            break;
        case SMSG_FORCE_RUN_SPEED_CHANGE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_FORCE_RUN_SPEED_CHANGE_read(reader, &opcodes->body.SMSG_FORCE_RUN_SPEED_CHANGE));
            break;
        case SMSG_FORCE_RUN_BACK_SPEED_CHANGE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_FORCE_RUN_BACK_SPEED_CHANGE_read(reader, &opcodes->body.SMSG_FORCE_RUN_BACK_SPEED_CHANGE));
            break;
        case SMSG_FORCE_SWIM_SPEED_CHANGE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_FORCE_SWIM_SPEED_CHANGE_read(reader, &opcodes->body.SMSG_FORCE_SWIM_SPEED_CHANGE));
            break;
        case SMSG_FORCE_MOVE_ROOT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_FORCE_MOVE_ROOT_read(reader, &opcodes->body.SMSG_FORCE_MOVE_ROOT));
            break;
        case SMSG_FORCE_MOVE_UNROOT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_FORCE_MOVE_UNROOT_read(reader, &opcodes->body.SMSG_FORCE_MOVE_UNROOT));
            break;
        case MSG_MOVE_HEARTBEAT:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_HEARTBEAT_Server_read(reader, &opcodes->body.MSG_MOVE_HEARTBEAT_Server));
            break;
        case SMSG_MOVE_KNOCK_BACK:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MOVE_KNOCK_BACK_read(reader, &opcodes->body.SMSG_MOVE_KNOCK_BACK));
            break;
        case SMSG_MOVE_FEATHER_FALL:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MOVE_FEATHER_FALL_read(reader, &opcodes->body.SMSG_MOVE_FEATHER_FALL));
            break;
        case SMSG_MOVE_NORMAL_FALL:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MOVE_NORMAL_FALL_read(reader, &opcodes->body.SMSG_MOVE_NORMAL_FALL));
            break;
        case SMSG_MOVE_SET_HOVER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MOVE_SET_HOVER_read(reader, &opcodes->body.SMSG_MOVE_SET_HOVER));
            break;
        case SMSG_MOVE_UNSET_HOVER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MOVE_UNSET_HOVER_read(reader, &opcodes->body.SMSG_MOVE_UNSET_HOVER));
            break;
        case SMSG_TRIGGER_CINEMATIC:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_TRIGGER_CINEMATIC_read(reader, &opcodes->body.SMSG_TRIGGER_CINEMATIC));
            break;
        case SMSG_TUTORIAL_FLAGS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_TUTORIAL_FLAGS_read(reader, &opcodes->body.SMSG_TUTORIAL_FLAGS));
            break;
        case SMSG_EMOTE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_EMOTE_read(reader, &opcodes->body.SMSG_EMOTE));
            break;
        case SMSG_TEXT_EMOTE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_TEXT_EMOTE_read(reader, &opcodes->body.SMSG_TEXT_EMOTE));
            break;
        case SMSG_INVENTORY_CHANGE_FAILURE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_INVENTORY_CHANGE_FAILURE_read(reader, &opcodes->body.SMSG_INVENTORY_CHANGE_FAILURE));
            break;
        case SMSG_OPEN_CONTAINER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_OPEN_CONTAINER_read(reader, &opcodes->body.SMSG_OPEN_CONTAINER));
            break;
        case SMSG_INSPECT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_INSPECT_read(reader, &opcodes->body.SMSG_INSPECT));
            break;
        case SMSG_TRADE_STATUS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_TRADE_STATUS_read(reader, &opcodes->body.SMSG_TRADE_STATUS));
            break;
        case SMSG_TRADE_STATUS_EXTENDED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_TRADE_STATUS_EXTENDED_read(reader, &opcodes->body.SMSG_TRADE_STATUS_EXTENDED));
            break;
        case SMSG_INITIALIZE_FACTIONS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_INITIALIZE_FACTIONS_read(reader, &opcodes->body.SMSG_INITIALIZE_FACTIONS));
            break;
        case SMSG_SET_FACTION_VISIBLE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SET_FACTION_VISIBLE_read(reader, &opcodes->body.SMSG_SET_FACTION_VISIBLE));
            break;
        case SMSG_SET_FACTION_STANDING:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SET_FACTION_STANDING_read(reader, &opcodes->body.SMSG_SET_FACTION_STANDING));
            break;
        case SMSG_SET_PROFICIENCY:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SET_PROFICIENCY_read(reader, &opcodes->body.SMSG_SET_PROFICIENCY));
            break;
        case SMSG_ACTION_BUTTONS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ACTION_BUTTONS_read(reader, &opcodes->body.SMSG_ACTION_BUTTONS));
            break;
        case SMSG_INITIAL_SPELLS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_INITIAL_SPELLS_read(reader, &opcodes->body.SMSG_INITIAL_SPELLS));
            break;
        case SMSG_LEARNED_SPELL:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LEARNED_SPELL_read(reader, &opcodes->body.SMSG_LEARNED_SPELL));
            break;
        case SMSG_SUPERCEDED_SPELL:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SUPERCEDED_SPELL_read(reader, &opcodes->body.SMSG_SUPERCEDED_SPELL));
            break;
        case SMSG_CAST_RESULT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_CAST_RESULT_read(reader, &opcodes->body.SMSG_CAST_RESULT));
            break;
        case SMSG_SPELL_START:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELL_START_read(reader, &opcodes->body.SMSG_SPELL_START));
            break;
        case SMSG_SPELL_GO:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELL_GO_read(reader, &opcodes->body.SMSG_SPELL_GO));
            break;
        case SMSG_SPELL_FAILURE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELL_FAILURE_read(reader, &opcodes->body.SMSG_SPELL_FAILURE));
            break;
        case SMSG_SPELL_COOLDOWN:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELL_COOLDOWN_read(reader, &opcodes->body.SMSG_SPELL_COOLDOWN, _size - 2));
            break;
        case SMSG_COOLDOWN_EVENT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_COOLDOWN_EVENT_read(reader, &opcodes->body.SMSG_COOLDOWN_EVENT));
            break;
        case SMSG_UPDATE_AURA_DURATION:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_UPDATE_AURA_DURATION_read(reader, &opcodes->body.SMSG_UPDATE_AURA_DURATION));
            break;
        case SMSG_PET_CAST_FAILED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PET_CAST_FAILED_read(reader, &opcodes->body.SMSG_PET_CAST_FAILED));
            break;
        case MSG_CHANNEL_START:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_CHANNEL_START_Server_read(reader, &opcodes->body.MSG_CHANNEL_START_Server));
            break;
        case MSG_CHANNEL_UPDATE:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_CHANNEL_UPDATE_Server_read(reader, &opcodes->body.MSG_CHANNEL_UPDATE_Server));
            break;
        case SMSG_AI_REACTION:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_AI_REACTION_read(reader, &opcodes->body.SMSG_AI_REACTION));
            break;
        case SMSG_ATTACKSTART:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ATTACKSTART_read(reader, &opcodes->body.SMSG_ATTACKSTART));
            break;
        case SMSG_ATTACKSTOP:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ATTACKSTOP_read(reader, &opcodes->body.SMSG_ATTACKSTOP));
            break;
        case SMSG_ATTACKERSTATEUPDATE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ATTACKERSTATEUPDATE_read(reader, &opcodes->body.SMSG_ATTACKERSTATEUPDATE));
            break;
        case SMSG_SPELLHEALLOG:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELLHEALLOG_read(reader, &opcodes->body.SMSG_SPELLHEALLOG));
            break;
        case SMSG_SPELLENERGIZELOG:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELLENERGIZELOG_read(reader, &opcodes->body.SMSG_SPELLENERGIZELOG));
            break;
        case SMSG_BINDPOINTUPDATE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_BINDPOINTUPDATE_read(reader, &opcodes->body.SMSG_BINDPOINTUPDATE));
            break;
        case SMSG_PLAYERBOUND:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PLAYERBOUND_read(reader, &opcodes->body.SMSG_PLAYERBOUND));
            break;
        case SMSG_CLIENT_CONTROL_UPDATE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_CLIENT_CONTROL_UPDATE_read(reader, &opcodes->body.SMSG_CLIENT_CONTROL_UPDATE));
            break;
        case SMSG_RESURRECT_REQUEST:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_RESURRECT_REQUEST_read(reader, &opcodes->body.SMSG_RESURRECT_REQUEST));
            break;
        case SMSG_LOOT_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LOOT_RESPONSE_read(reader, &opcodes->body.SMSG_LOOT_RESPONSE));
            break;
        case SMSG_LOOT_RELEASE_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LOOT_RELEASE_RESPONSE_read(reader, &opcodes->body.SMSG_LOOT_RELEASE_RESPONSE));
            break;
        case SMSG_LOOT_REMOVED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LOOT_REMOVED_read(reader, &opcodes->body.SMSG_LOOT_REMOVED));
            break;
        case SMSG_LOOT_MONEY_NOTIFY:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LOOT_MONEY_NOTIFY_read(reader, &opcodes->body.SMSG_LOOT_MONEY_NOTIFY));
            break;
        case SMSG_ITEM_PUSH_RESULT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ITEM_PUSH_RESULT_read(reader, &opcodes->body.SMSG_ITEM_PUSH_RESULT));
            break;
        case SMSG_DUEL_REQUESTED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_DUEL_REQUESTED_read(reader, &opcodes->body.SMSG_DUEL_REQUESTED));
            break;
        case SMSG_DUEL_COMPLETE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_DUEL_COMPLETE_read(reader, &opcodes->body.SMSG_DUEL_COMPLETE));
            break;
        case SMSG_DUEL_WINNER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_DUEL_WINNER_read(reader, &opcodes->body.SMSG_DUEL_WINNER));
            break;
        case SMSG_MOUNTRESULT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MOUNTRESULT_read(reader, &opcodes->body.SMSG_MOUNTRESULT));
            break;
        case SMSG_DISMOUNTRESULT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_DISMOUNTRESULT_read(reader, &opcodes->body.SMSG_DISMOUNTRESULT));
            break;
        case SMSG_MOUNTSPECIAL_ANIM:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MOUNTSPECIAL_ANIM_read(reader, &opcodes->body.SMSG_MOUNTSPECIAL_ANIM));
            break;
        case SMSG_PET_TAME_FAILURE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PET_TAME_FAILURE_read(reader, &opcodes->body.SMSG_PET_TAME_FAILURE));
            break;
        case SMSG_PET_SPELLS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PET_SPELLS_read(reader, &opcodes->body.SMSG_PET_SPELLS, _size - 2));
            break;
        case SMSG_PET_MODE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PET_MODE_read(reader, &opcodes->body.SMSG_PET_MODE));
            break;
        case SMSG_GOSSIP_MESSAGE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GOSSIP_MESSAGE_read(reader, &opcodes->body.SMSG_GOSSIP_MESSAGE));
            break;
        case SMSG_NPC_TEXT_UPDATE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_NPC_TEXT_UPDATE_read(reader, &opcodes->body.SMSG_NPC_TEXT_UPDATE));
            break;
        case SMSG_QUESTGIVER_STATUS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUESTGIVER_STATUS_read(reader, &opcodes->body.SMSG_QUESTGIVER_STATUS));
            break;
        case SMSG_QUESTGIVER_QUEST_LIST:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUESTGIVER_QUEST_LIST_read(reader, &opcodes->body.SMSG_QUESTGIVER_QUEST_LIST));
            break;
        case SMSG_QUESTGIVER_QUEST_DETAILS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUESTGIVER_QUEST_DETAILS_read(reader, &opcodes->body.SMSG_QUESTGIVER_QUEST_DETAILS));
            break;
        case SMSG_QUESTGIVER_REQUEST_ITEMS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUESTGIVER_REQUEST_ITEMS_read(reader, &opcodes->body.SMSG_QUESTGIVER_REQUEST_ITEMS));
            break;
        case SMSG_QUESTGIVER_OFFER_REWARD:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUESTGIVER_OFFER_REWARD_read(reader, &opcodes->body.SMSG_QUESTGIVER_OFFER_REWARD));
            break;
        case SMSG_QUESTGIVER_QUEST_INVALID:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUESTGIVER_QUEST_INVALID_read(reader, &opcodes->body.SMSG_QUESTGIVER_QUEST_INVALID));
            break;
        case SMSG_QUESTGIVER_QUEST_COMPLETE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUESTGIVER_QUEST_COMPLETE_read(reader, &opcodes->body.SMSG_QUESTGIVER_QUEST_COMPLETE));
            break;
        case SMSG_QUESTGIVER_QUEST_FAILED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUESTGIVER_QUEST_FAILED_read(reader, &opcodes->body.SMSG_QUESTGIVER_QUEST_FAILED));
            break;
        case SMSG_QUESTUPDATE_FAILED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUESTUPDATE_FAILED_read(reader, &opcodes->body.SMSG_QUESTUPDATE_FAILED));
            break;
        case SMSG_QUESTUPDATE_FAILEDTIMER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUESTUPDATE_FAILEDTIMER_read(reader, &opcodes->body.SMSG_QUESTUPDATE_FAILEDTIMER));
            break;
        case SMSG_QUESTUPDATE_COMPLETE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUESTUPDATE_COMPLETE_read(reader, &opcodes->body.SMSG_QUESTUPDATE_COMPLETE));
            break;
        case SMSG_QUESTUPDATE_ADD_KILL:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUESTUPDATE_ADD_KILL_read(reader, &opcodes->body.SMSG_QUESTUPDATE_ADD_KILL));
            break;
        case SMSG_QUESTUPDATE_ADD_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUESTUPDATE_ADD_ITEM_read(reader, &opcodes->body.SMSG_QUESTUPDATE_ADD_ITEM));
            break;
        case SMSG_QUEST_CONFIRM_ACCEPT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUEST_CONFIRM_ACCEPT_read(reader, &opcodes->body.SMSG_QUEST_CONFIRM_ACCEPT));
            break;
        case SMSG_LIST_INVENTORY:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LIST_INVENTORY_read(reader, &opcodes->body.SMSG_LIST_INVENTORY));
            break;
        case SMSG_SELL_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SELL_ITEM_read(reader, &opcodes->body.SMSG_SELL_ITEM));
            break;
        case SMSG_BUY_ITEM:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_BUY_ITEM_read(reader, &opcodes->body.SMSG_BUY_ITEM));
            break;
        case SMSG_BUY_FAILED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_BUY_FAILED_read(reader, &opcodes->body.SMSG_BUY_FAILED));
            break;
        case SMSG_SHOWTAXINODES:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SHOWTAXINODES_read(reader, &opcodes->body.SMSG_SHOWTAXINODES, _size - 2));
            break;
        case SMSG_TAXINODE_STATUS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_TAXINODE_STATUS_read(reader, &opcodes->body.SMSG_TAXINODE_STATUS));
            break;
        case SMSG_ACTIVATETAXIREPLY:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ACTIVATETAXIREPLY_read(reader, &opcodes->body.SMSG_ACTIVATETAXIREPLY));
            break;
        case SMSG_TRAINER_LIST:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_TRAINER_LIST_read(reader, &opcodes->body.SMSG_TRAINER_LIST));
            break;
        case SMSG_TRAINER_BUY_SUCCEEDED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_TRAINER_BUY_SUCCEEDED_read(reader, &opcodes->body.SMSG_TRAINER_BUY_SUCCEEDED));
            break;
        case SMSG_TRAINER_BUY_FAILED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_TRAINER_BUY_FAILED_read(reader, &opcodes->body.SMSG_TRAINER_BUY_FAILED));
            break;
        case SMSG_SHOW_BANK:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SHOW_BANK_read(reader, &opcodes->body.SMSG_SHOW_BANK));
            break;
        case SMSG_BUY_BANK_SLOT_RESULT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_BUY_BANK_SLOT_RESULT_read(reader, &opcodes->body.SMSG_BUY_BANK_SLOT_RESULT));
            break;
        case SMSG_PETITION_SHOWLIST:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PETITION_SHOWLIST_read(reader, &opcodes->body.SMSG_PETITION_SHOWLIST));
            break;
        case SMSG_PETITION_SHOW_SIGNATURES:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PETITION_SHOW_SIGNATURES_read(reader, &opcodes->body.SMSG_PETITION_SHOW_SIGNATURES));
            break;
        case SMSG_PETITION_SIGN_RESULTS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PETITION_SIGN_RESULTS_read(reader, &opcodes->body.SMSG_PETITION_SIGN_RESULTS));
            break;
        case MSG_PETITION_DECLINE:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_PETITION_DECLINE_read(reader, &opcodes->body.MSG_PETITION_DECLINE));
            break;
        case SMSG_TURN_IN_PETITION_RESULTS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_TURN_IN_PETITION_RESULTS_read(reader, &opcodes->body.SMSG_TURN_IN_PETITION_RESULTS));
            break;
        case SMSG_PETITION_QUERY_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PETITION_QUERY_RESPONSE_read(reader, &opcodes->body.SMSG_PETITION_QUERY_RESPONSE));
            break;
        case SMSG_NOTIFICATION:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_NOTIFICATION_read(reader, &opcodes->body.SMSG_NOTIFICATION));
            break;
        case SMSG_PLAYED_TIME:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PLAYED_TIME_read(reader, &opcodes->body.SMSG_PLAYED_TIME));
            break;
        case SMSG_QUERY_TIME_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_QUERY_TIME_RESPONSE_read(reader, &opcodes->body.SMSG_QUERY_TIME_RESPONSE));
            break;
        case SMSG_LOG_XPGAIN:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LOG_XPGAIN_read(reader, &opcodes->body.SMSG_LOG_XPGAIN));
            break;
        case SMSG_LEVELUP_INFO:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LEVELUP_INFO_read(reader, &opcodes->body.SMSG_LEVELUP_INFO));
            break;
        case MSG_MINIMAP_PING:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MINIMAP_PING_Server_read(reader, &opcodes->body.MSG_MINIMAP_PING_Server));
            break;
        case SMSG_RESISTLOG:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_RESISTLOG_read(reader, &opcodes->body.SMSG_RESISTLOG));
            break;
        case SMSG_ENCHANTMENTLOG:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ENCHANTMENTLOG_read(reader, &opcodes->body.SMSG_ENCHANTMENTLOG));
            break;
        case SMSG_START_MIRROR_TIMER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_START_MIRROR_TIMER_read(reader, &opcodes->body.SMSG_START_MIRROR_TIMER));
            break;
        case SMSG_PAUSE_MIRROR_TIMER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PAUSE_MIRROR_TIMER_read(reader, &opcodes->body.SMSG_PAUSE_MIRROR_TIMER));
            break;
        case SMSG_STOP_MIRROR_TIMER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_STOP_MIRROR_TIMER_read(reader, &opcodes->body.SMSG_STOP_MIRROR_TIMER));
            break;
        case SMSG_PONG:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PONG_read(reader, &opcodes->body.SMSG_PONG));
            break;
        case SMSG_CLEAR_COOLDOWN:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_CLEAR_COOLDOWN_read(reader, &opcodes->body.SMSG_CLEAR_COOLDOWN));
            break;
        case SMSG_GAMEOBJECT_PAGETEXT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GAMEOBJECT_PAGETEXT_read(reader, &opcodes->body.SMSG_GAMEOBJECT_PAGETEXT));
            break;
        case SMSG_SPELL_DELAYED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELL_DELAYED_read(reader, &opcodes->body.SMSG_SPELL_DELAYED));
            break;
        case SMSG_ITEM_TIME_UPDATE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ITEM_TIME_UPDATE_read(reader, &opcodes->body.SMSG_ITEM_TIME_UPDATE));
            break;
        case SMSG_ITEM_ENCHANT_TIME_UPDATE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ITEM_ENCHANT_TIME_UPDATE_read(reader, &opcodes->body.SMSG_ITEM_ENCHANT_TIME_UPDATE));
            break;
        case SMSG_AUTH_CHALLENGE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_AUTH_CHALLENGE_read(reader, &opcodes->body.SMSG_AUTH_CHALLENGE));
            break;
        case SMSG_AUTH_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_AUTH_RESPONSE_read(reader, &opcodes->body.SMSG_AUTH_RESPONSE));
            break;
        case MSG_SAVE_GUILD_EMBLEM:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_SAVE_GUILD_EMBLEM_Server_read(reader, &opcodes->body.MSG_SAVE_GUILD_EMBLEM_Server));
            break;
        case MSG_TABARDVENDOR_ACTIVATE:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_TABARDVENDOR_ACTIVATE_read(reader, &opcodes->body.MSG_TABARDVENDOR_ACTIVATE));
            break;
        case SMSG_PLAY_SPELL_VISUAL:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PLAY_SPELL_VISUAL_read(reader, &opcodes->body.SMSG_PLAY_SPELL_VISUAL));
            break;
        case SMSG_PARTYKILLLOG:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PARTYKILLLOG_read(reader, &opcodes->body.SMSG_PARTYKILLLOG));
            break;
        case SMSG_COMPRESSED_UPDATE_OBJECT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_COMPRESSED_UPDATE_OBJECT_read(reader, &opcodes->body.SMSG_COMPRESSED_UPDATE_OBJECT, _size - 2));
            break;
        case SMSG_PLAY_SPELL_IMPACT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PLAY_SPELL_IMPACT_read(reader, &opcodes->body.SMSG_PLAY_SPELL_IMPACT));
            break;
        case SMSG_EXPLORATION_EXPERIENCE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_EXPLORATION_EXPERIENCE_read(reader, &opcodes->body.SMSG_EXPLORATION_EXPERIENCE));
            break;
        case MSG_RANDOM_ROLL:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_RANDOM_ROLL_Server_read(reader, &opcodes->body.MSG_RANDOM_ROLL_Server));
            break;
        case SMSG_ENVIRONMENTAL_DAMAGE_LOG:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ENVIRONMENTAL_DAMAGE_LOG_read(reader, &opcodes->body.SMSG_ENVIRONMENTAL_DAMAGE_LOG));
            break;
        case MSG_LOOKING_FOR_GROUP:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_LOOKING_FOR_GROUP_Server_read(reader, &opcodes->body.MSG_LOOKING_FOR_GROUP_Server));
            break;
        case SMSG_REMOVED_SPELL:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_REMOVED_SPELL_read(reader, &opcodes->body.SMSG_REMOVED_SPELL));
            break;
        case SMSG_GMTICKET_CREATE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GMTICKET_CREATE_read(reader, &opcodes->body.SMSG_GMTICKET_CREATE));
            break;
        case SMSG_GMTICKET_UPDATETEXT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GMTICKET_UPDATETEXT_read(reader, &opcodes->body.SMSG_GMTICKET_UPDATETEXT));
            break;
        case SMSG_ACCOUNT_DATA_TIMES:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ACCOUNT_DATA_TIMES_read(reader, &opcodes->body.SMSG_ACCOUNT_DATA_TIMES));
            break;
        case SMSG_GMTICKET_GETTICKET:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GMTICKET_GETTICKET_read(reader, &opcodes->body.SMSG_GMTICKET_GETTICKET));
            break;
        case SMSG_GAMEOBJECT_SPAWN_ANIM:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GAMEOBJECT_SPAWN_ANIM_read(reader, &opcodes->body.SMSG_GAMEOBJECT_SPAWN_ANIM));
            break;
        case SMSG_GAMEOBJECT_DESPAWN_ANIM:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GAMEOBJECT_DESPAWN_ANIM_read(reader, &opcodes->body.SMSG_GAMEOBJECT_DESPAWN_ANIM));
            break;
        case MSG_CORPSE_QUERY:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_CORPSE_QUERY_Server_read(reader, &opcodes->body.MSG_CORPSE_QUERY_Server));
            break;
        case SMSG_GMTICKET_DELETETICKET:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GMTICKET_DELETETICKET_read(reader, &opcodes->body.SMSG_GMTICKET_DELETETICKET));
            break;
        case SMSG_GMTICKET_SYSTEMSTATUS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GMTICKET_SYSTEMSTATUS_read(reader, &opcodes->body.SMSG_GMTICKET_SYSTEMSTATUS));
            break;
        case SMSG_SET_REST_START:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SET_REST_START_read(reader, &opcodes->body.SMSG_SET_REST_START));
            break;
        case SMSG_SPIRIT_HEALER_CONFIRM:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPIRIT_HEALER_CONFIRM_read(reader, &opcodes->body.SMSG_SPIRIT_HEALER_CONFIRM));
            break;
        case SMSG_GOSSIP_POI:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GOSSIP_POI_read(reader, &opcodes->body.SMSG_GOSSIP_POI));
            break;
        case SMSG_LOGIN_VERIFY_WORLD:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LOGIN_VERIFY_WORLD_read(reader, &opcodes->body.SMSG_LOGIN_VERIFY_WORLD));
            break;
        case SMSG_SEND_MAIL_RESULT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SEND_MAIL_RESULT_read(reader, &opcodes->body.SMSG_SEND_MAIL_RESULT));
            break;
        case SMSG_MAIL_LIST_RESULT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MAIL_LIST_RESULT_read(reader, &opcodes->body.SMSG_MAIL_LIST_RESULT));
            break;
        case SMSG_BATTLEFIELD_LIST:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_BATTLEFIELD_LIST_read(reader, &opcodes->body.SMSG_BATTLEFIELD_LIST));
            break;
        case SMSG_ITEM_TEXT_QUERY_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ITEM_TEXT_QUERY_RESPONSE_read(reader, &opcodes->body.SMSG_ITEM_TEXT_QUERY_RESPONSE));
            break;
        case SMSG_SPELLLOGMISS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELLLOGMISS_read(reader, &opcodes->body.SMSG_SPELLLOGMISS));
            break;
        case SMSG_SPELLLOGEXECUTE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELLLOGEXECUTE_read(reader, &opcodes->body.SMSG_SPELLLOGEXECUTE));
            break;
        case SMSG_PERIODICAURALOG:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PERIODICAURALOG_read(reader, &opcodes->body.SMSG_PERIODICAURALOG));
            break;
        case SMSG_SPELLDAMAGESHIELD:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELLDAMAGESHIELD_read(reader, &opcodes->body.SMSG_SPELLDAMAGESHIELD));
            break;
        case SMSG_SPELLNONMELEEDAMAGELOG:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELLNONMELEEDAMAGELOG_read(reader, &opcodes->body.SMSG_SPELLNONMELEEDAMAGELOG));
            break;
        case SMSG_ZONE_UNDER_ATTACK:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ZONE_UNDER_ATTACK_read(reader, &opcodes->body.SMSG_ZONE_UNDER_ATTACK));
            break;
        case MSG_AUCTION_HELLO:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_AUCTION_HELLO_Server_read(reader, &opcodes->body.MSG_AUCTION_HELLO_Server));
            break;
        case SMSG_AUCTION_COMMAND_RESULT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_AUCTION_COMMAND_RESULT_read(reader, &opcodes->body.SMSG_AUCTION_COMMAND_RESULT));
            break;
        case SMSG_AUCTION_LIST_RESULT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_AUCTION_LIST_RESULT_read(reader, &opcodes->body.SMSG_AUCTION_LIST_RESULT));
            break;
        case SMSG_AUCTION_OWNER_LIST_RESULT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_AUCTION_OWNER_LIST_RESULT_read(reader, &opcodes->body.SMSG_AUCTION_OWNER_LIST_RESULT));
            break;
        case SMSG_AUCTION_BIDDER_NOTIFICATION:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_AUCTION_BIDDER_NOTIFICATION_read(reader, &opcodes->body.SMSG_AUCTION_BIDDER_NOTIFICATION));
            break;
        case SMSG_AUCTION_OWNER_NOTIFICATION:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_AUCTION_OWNER_NOTIFICATION_read(reader, &opcodes->body.SMSG_AUCTION_OWNER_NOTIFICATION));
            break;
        case SMSG_PROCRESIST:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PROCRESIST_read(reader, &opcodes->body.SMSG_PROCRESIST));
            break;
        case SMSG_DISPEL_FAILED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_DISPEL_FAILED_read(reader, &opcodes->body.SMSG_DISPEL_FAILED, _size - 2));
            break;
        case SMSG_SPELLORDAMAGE_IMMUNE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELLORDAMAGE_IMMUNE_read(reader, &opcodes->body.SMSG_SPELLORDAMAGE_IMMUNE));
            break;
        case SMSG_AUCTION_BIDDER_LIST_RESULT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_AUCTION_BIDDER_LIST_RESULT_read(reader, &opcodes->body.SMSG_AUCTION_BIDDER_LIST_RESULT));
            break;
        case SMSG_SET_FLAT_SPELL_MODIFIER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SET_FLAT_SPELL_MODIFIER_read(reader, &opcodes->body.SMSG_SET_FLAT_SPELL_MODIFIER));
            break;
        case SMSG_SET_PCT_SPELL_MODIFIER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SET_PCT_SPELL_MODIFIER_read(reader, &opcodes->body.SMSG_SET_PCT_SPELL_MODIFIER));
            break;
        case SMSG_CORPSE_RECLAIM_DELAY:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_CORPSE_RECLAIM_DELAY_read(reader, &opcodes->body.SMSG_CORPSE_RECLAIM_DELAY));
            break;
        case MSG_LIST_STABLED_PETS:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_LIST_STABLED_PETS_Server_read(reader, &opcodes->body.MSG_LIST_STABLED_PETS_Server));
            break;
        case SMSG_STABLE_RESULT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_STABLE_RESULT_read(reader, &opcodes->body.SMSG_STABLE_RESULT));
            break;
        case MSG_QUEST_PUSH_RESULT:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_QUEST_PUSH_RESULT_read(reader, &opcodes->body.MSG_QUEST_PUSH_RESULT));
            break;
        case SMSG_PLAY_MUSIC:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PLAY_MUSIC_read(reader, &opcodes->body.SMSG_PLAY_MUSIC));
            break;
        case SMSG_PLAY_OBJECT_SOUND:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PLAY_OBJECT_SOUND_read(reader, &opcodes->body.SMSG_PLAY_OBJECT_SOUND));
            break;
        case SMSG_SPELLDISPELLOG:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELLDISPELLOG_read(reader, &opcodes->body.SMSG_SPELLDISPELLOG));
            break;
        case MSG_QUERY_NEXT_MAIL_TIME:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_QUERY_NEXT_MAIL_TIME_Server_read(reader, &opcodes->body.MSG_QUERY_NEXT_MAIL_TIME_Server));
            break;
        case SMSG_RECEIVED_MAIL:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_RECEIVED_MAIL_read(reader, &opcodes->body.SMSG_RECEIVED_MAIL));
            break;
        case SMSG_RAID_GROUP_ONLY:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_RAID_GROUP_ONLY_read(reader, &opcodes->body.SMSG_RAID_GROUP_ONLY));
            break;
        case SMSG_PVP_CREDIT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PVP_CREDIT_read(reader, &opcodes->body.SMSG_PVP_CREDIT));
            break;
        case SMSG_AUCTION_REMOVED_NOTIFICATION:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_AUCTION_REMOVED_NOTIFICATION_read(reader, &opcodes->body.SMSG_AUCTION_REMOVED_NOTIFICATION));
            break;
        case SMSG_SERVER_MESSAGE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SERVER_MESSAGE_read(reader, &opcodes->body.SMSG_SERVER_MESSAGE));
            break;
        case SMSG_MEETINGSTONE_SETQUEUE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MEETINGSTONE_SETQUEUE_read(reader, &opcodes->body.SMSG_MEETINGSTONE_SETQUEUE));
            break;
        case SMSG_MEETINGSTONE_MEMBER_ADDED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MEETINGSTONE_MEMBER_ADDED_read(reader, &opcodes->body.SMSG_MEETINGSTONE_MEMBER_ADDED));
            break;
        case SMSG_STANDSTATE_UPDATE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_STANDSTATE_UPDATE_read(reader, &opcodes->body.SMSG_STANDSTATE_UPDATE));
            break;
        case SMSG_LOOT_ALL_PASSED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LOOT_ALL_PASSED_read(reader, &opcodes->body.SMSG_LOOT_ALL_PASSED));
            break;
        case SMSG_LOOT_ROLL_WON:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LOOT_ROLL_WON_read(reader, &opcodes->body.SMSG_LOOT_ROLL_WON));
            break;
        case SMSG_LOOT_START_ROLL:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LOOT_START_ROLL_read(reader, &opcodes->body.SMSG_LOOT_START_ROLL));
            break;
        case SMSG_LOOT_ROLL:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LOOT_ROLL_read(reader, &opcodes->body.SMSG_LOOT_ROLL));
            break;
        case SMSG_LOOT_MASTER_LIST:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_LOOT_MASTER_LIST_read(reader, &opcodes->body.SMSG_LOOT_MASTER_LIST));
            break;
        case SMSG_SET_FORCED_REACTIONS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SET_FORCED_REACTIONS_read(reader, &opcodes->body.SMSG_SET_FORCED_REACTIONS));
            break;
        case SMSG_SPELL_FAILED_OTHER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELL_FAILED_OTHER_read(reader, &opcodes->body.SMSG_SPELL_FAILED_OTHER));
            break;
        case SMSG_GAMEOBJECT_RESET_STATE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GAMEOBJECT_RESET_STATE_read(reader, &opcodes->body.SMSG_GAMEOBJECT_RESET_STATE));
            break;
        case SMSG_CHAT_PLAYER_NOT_FOUND:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_CHAT_PLAYER_NOT_FOUND_read(reader, &opcodes->body.SMSG_CHAT_PLAYER_NOT_FOUND));
            break;
        case MSG_TALENT_WIPE_CONFIRM:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_TALENT_WIPE_CONFIRM_Server_read(reader, &opcodes->body.MSG_TALENT_WIPE_CONFIRM_Server));
            break;
        case SMSG_SUMMON_REQUEST:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SUMMON_REQUEST_read(reader, &opcodes->body.SMSG_SUMMON_REQUEST));
            break;
        case SMSG_MONSTER_MOVE_TRANSPORT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MONSTER_MOVE_TRANSPORT_read(reader, &opcodes->body.SMSG_MONSTER_MOVE_TRANSPORT));
            break;
        case MSG_MOVE_FEATHER_FALL:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_FEATHER_FALL_Server_read(reader, &opcodes->body.MSG_MOVE_FEATHER_FALL_Server));
            break;
        case MSG_MOVE_WATER_WALK:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_WATER_WALK_read(reader, &opcodes->body.MSG_MOVE_WATER_WALK));
            break;
        case SMSG_DUEL_COUNTDOWN:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_DUEL_COUNTDOWN_read(reader, &opcodes->body.SMSG_DUEL_COUNTDOWN));
            break;
        case SMSG_AREA_TRIGGER_MESSAGE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_AREA_TRIGGER_MESSAGE_read(reader, &opcodes->body.SMSG_AREA_TRIGGER_MESSAGE));
            break;
        case SMSG_MEETINGSTONE_JOINFAILED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_MEETINGSTONE_JOINFAILED_read(reader, &opcodes->body.SMSG_MEETINGSTONE_JOINFAILED));
            break;
        case SMSG_PLAYER_SKINNED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PLAYER_SKINNED_read(reader, &opcodes->body.SMSG_PLAYER_SKINNED));
            break;
        case MSG_PETITION_RENAME:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_PETITION_RENAME_read(reader, &opcodes->body.MSG_PETITION_RENAME));
            break;
        case SMSG_INIT_WORLD_STATES:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_INIT_WORLD_STATES_read(reader, &opcodes->body.SMSG_INIT_WORLD_STATES));
            break;
        case SMSG_UPDATE_WORLD_STATE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_UPDATE_WORLD_STATE_read(reader, &opcodes->body.SMSG_UPDATE_WORLD_STATE));
            break;
        case SMSG_ITEM_NAME_QUERY_RESPONSE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ITEM_NAME_QUERY_RESPONSE_read(reader, &opcodes->body.SMSG_ITEM_NAME_QUERY_RESPONSE));
            break;
        case SMSG_PET_ACTION_FEEDBACK:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PET_ACTION_FEEDBACK_read(reader, &opcodes->body.SMSG_PET_ACTION_FEEDBACK));
            break;
        case SMSG_CHAR_RENAME:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_CHAR_RENAME_read(reader, &opcodes->body.SMSG_CHAR_RENAME));
            break;
        case SMSG_INSTANCE_SAVE_CREATED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_INSTANCE_SAVE_CREATED_read(reader, &opcodes->body.SMSG_INSTANCE_SAVE_CREATED));
            break;
        case SMSG_RAID_INSTANCE_INFO:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_RAID_INSTANCE_INFO_read(reader, &opcodes->body.SMSG_RAID_INSTANCE_INFO));
            break;
        case SMSG_PLAY_SOUND:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PLAY_SOUND_read(reader, &opcodes->body.SMSG_PLAY_SOUND));
            break;
        case SMSG_BATTLEFIELD_STATUS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_BATTLEFIELD_STATUS_read(reader, &opcodes->body.SMSG_BATTLEFIELD_STATUS));
            break;
        case MSG_INSPECT_HONOR_STATS:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_INSPECT_HONOR_STATS_Server_read(reader, &opcodes->body.MSG_INSPECT_HONOR_STATS_Server));
            break;
        case SMSG_FORCE_WALK_SPEED_CHANGE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_FORCE_WALK_SPEED_CHANGE_read(reader, &opcodes->body.SMSG_FORCE_WALK_SPEED_CHANGE));
            break;
        case SMSG_FORCE_SWIM_BACK_SPEED_CHANGE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_FORCE_SWIM_BACK_SPEED_CHANGE_read(reader, &opcodes->body.SMSG_FORCE_SWIM_BACK_SPEED_CHANGE));
            break;
        case SMSG_FORCE_TURN_RATE_CHANGE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_FORCE_TURN_RATE_CHANGE_read(reader, &opcodes->body.SMSG_FORCE_TURN_RATE_CHANGE));
            break;
        case MSG_PVP_LOG_DATA:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_PVP_LOG_DATA_Server_read(reader, &opcodes->body.MSG_PVP_LOG_DATA_Server));
            break;
        case SMSG_AREA_SPIRIT_HEALER_TIME:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_AREA_SPIRIT_HEALER_TIME_read(reader, &opcodes->body.SMSG_AREA_SPIRIT_HEALER_TIME));
            break;
        case SMSG_WARDEN_DATA:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_WARDEN_DATA_read(reader, &opcodes->body.SMSG_WARDEN_DATA, _size - 2));
            break;
        case SMSG_GROUP_JOINED_BATTLEGROUND:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GROUP_JOINED_BATTLEGROUND_read(reader, &opcodes->body.SMSG_GROUP_JOINED_BATTLEGROUND));
            break;
        case MSG_BATTLEGROUND_PLAYER_POSITIONS:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_BATTLEGROUND_PLAYER_POSITIONS_Server_read(reader, &opcodes->body.MSG_BATTLEGROUND_PLAYER_POSITIONS_Server));
            break;
        case SMSG_BINDER_CONFIRM:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_BINDER_CONFIRM_read(reader, &opcodes->body.SMSG_BINDER_CONFIRM));
            break;
        case SMSG_BATTLEGROUND_PLAYER_JOINED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_BATTLEGROUND_PLAYER_JOINED_read(reader, &opcodes->body.SMSG_BATTLEGROUND_PLAYER_JOINED));
            break;
        case SMSG_BATTLEGROUND_PLAYER_LEFT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_BATTLEGROUND_PLAYER_LEFT_read(reader, &opcodes->body.SMSG_BATTLEGROUND_PLAYER_LEFT));
            break;
        case SMSG_ADDON_INFO:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_ADDON_INFO_read(reader, &opcodes->body.SMSG_ADDON_INFO, _size - 2));
            break;
        case SMSG_PET_UNLEARN_CONFIRM:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PET_UNLEARN_CONFIRM_read(reader, &opcodes->body.SMSG_PET_UNLEARN_CONFIRM));
            break;
        case SMSG_PARTY_MEMBER_STATS_FULL:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PARTY_MEMBER_STATS_FULL_read(reader, &opcodes->body.SMSG_PARTY_MEMBER_STATS_FULL));
            break;
        case SMSG_WEATHER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_WEATHER_read(reader, &opcodes->body.SMSG_WEATHER));
            break;
        case SMSG_RAID_INSTANCE_MESSAGE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_RAID_INSTANCE_MESSAGE_read(reader, &opcodes->body.SMSG_RAID_INSTANCE_MESSAGE));
            break;
        case SMSG_COMPRESSED_MOVES:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_COMPRESSED_MOVES_read(reader, &opcodes->body.SMSG_COMPRESSED_MOVES, _size - 2));
            break;
        case SMSG_SPLINE_SET_RUN_SPEED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_SET_RUN_SPEED_read(reader, &opcodes->body.SMSG_SPLINE_SET_RUN_SPEED));
            break;
        case SMSG_SPLINE_SET_RUN_BACK_SPEED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_SET_RUN_BACK_SPEED_read(reader, &opcodes->body.SMSG_SPLINE_SET_RUN_BACK_SPEED));
            break;
        case SMSG_SPLINE_SET_SWIM_SPEED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_SET_SWIM_SPEED_read(reader, &opcodes->body.SMSG_SPLINE_SET_SWIM_SPEED));
            break;
        case SMSG_SPLINE_SET_WALK_SPEED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_SET_WALK_SPEED_read(reader, &opcodes->body.SMSG_SPLINE_SET_WALK_SPEED));
            break;
        case SMSG_SPLINE_SET_SWIM_BACK_SPEED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_SET_SWIM_BACK_SPEED_read(reader, &opcodes->body.SMSG_SPLINE_SET_SWIM_BACK_SPEED));
            break;
        case SMSG_SPLINE_SET_TURN_RATE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_SET_TURN_RATE_read(reader, &opcodes->body.SMSG_SPLINE_SET_TURN_RATE));
            break;
        case SMSG_SPLINE_MOVE_UNROOT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_MOVE_UNROOT_read(reader, &opcodes->body.SMSG_SPLINE_MOVE_UNROOT));
            break;
        case SMSG_SPLINE_MOVE_FEATHER_FALL:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_MOVE_FEATHER_FALL_read(reader, &opcodes->body.SMSG_SPLINE_MOVE_FEATHER_FALL));
            break;
        case SMSG_SPLINE_MOVE_NORMAL_FALL:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_MOVE_NORMAL_FALL_read(reader, &opcodes->body.SMSG_SPLINE_MOVE_NORMAL_FALL));
            break;
        case SMSG_SPLINE_MOVE_SET_HOVER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_MOVE_SET_HOVER_read(reader, &opcodes->body.SMSG_SPLINE_MOVE_SET_HOVER));
            break;
        case SMSG_SPLINE_MOVE_UNSET_HOVER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_MOVE_UNSET_HOVER_read(reader, &opcodes->body.SMSG_SPLINE_MOVE_UNSET_HOVER));
            break;
        case SMSG_SPLINE_MOVE_WATER_WALK:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_MOVE_WATER_WALK_read(reader, &opcodes->body.SMSG_SPLINE_MOVE_WATER_WALK));
            break;
        case SMSG_SPLINE_MOVE_LAND_WALK:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_MOVE_LAND_WALK_read(reader, &opcodes->body.SMSG_SPLINE_MOVE_LAND_WALK));
            break;
        case SMSG_SPLINE_MOVE_START_SWIM:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_MOVE_START_SWIM_read(reader, &opcodes->body.SMSG_SPLINE_MOVE_START_SWIM));
            break;
        case SMSG_SPLINE_MOVE_STOP_SWIM:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_MOVE_STOP_SWIM_read(reader, &opcodes->body.SMSG_SPLINE_MOVE_STOP_SWIM));
            break;
        case SMSG_SPLINE_MOVE_SET_RUN_MODE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_MOVE_SET_RUN_MODE_read(reader, &opcodes->body.SMSG_SPLINE_MOVE_SET_RUN_MODE));
            break;
        case SMSG_SPLINE_MOVE_SET_WALK_MODE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_MOVE_SET_WALK_MODE_read(reader, &opcodes->body.SMSG_SPLINE_MOVE_SET_WALK_MODE));
            break;
        case MSG_MOVE_TIME_SKIPPED:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_MOVE_TIME_SKIPPED_Server_read(reader, &opcodes->body.MSG_MOVE_TIME_SKIPPED_Server));
            break;
        case SMSG_SPLINE_MOVE_ROOT:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPLINE_MOVE_ROOT_read(reader, &opcodes->body.SMSG_SPLINE_MOVE_ROOT));
            break;
        case SMSG_INVALIDATE_PLAYER:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_INVALIDATE_PLAYER_read(reader, &opcodes->body.SMSG_INVALIDATE_PLAYER));
            break;
        case SMSG_INSTANCE_RESET:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_INSTANCE_RESET_read(reader, &opcodes->body.SMSG_INSTANCE_RESET));
            break;
        case SMSG_INSTANCE_RESET_FAILED:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_INSTANCE_RESET_FAILED_read(reader, &opcodes->body.SMSG_INSTANCE_RESET_FAILED));
            break;
        case SMSG_UPDATE_LAST_INSTANCE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_UPDATE_LAST_INSTANCE_read(reader, &opcodes->body.SMSG_UPDATE_LAST_INSTANCE));
            break;
        case MSG_RAID_TARGET_UPDATE:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_RAID_TARGET_UPDATE_Server_read(reader, &opcodes->body.MSG_RAID_TARGET_UPDATE_Server));
            break;
        case MSG_RAID_READY_CHECK:
            WWM_CHECK_RETURN_CODE(vanilla_MSG_RAID_READY_CHECK_Server_read(reader, &opcodes->body.MSG_RAID_READY_CHECK_Server, _size - 2));
            break;
        case SMSG_PET_ACTION_SOUND:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PET_ACTION_SOUND_read(reader, &opcodes->body.SMSG_PET_ACTION_SOUND));
            break;
        case SMSG_PET_DISMISS_SOUND:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_PET_DISMISS_SOUND_read(reader, &opcodes->body.SMSG_PET_DISMISS_SOUND));
            break;
        case SMSG_GM_TICKET_STATUS_UPDATE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_GM_TICKET_STATUS_UPDATE_read(reader, &opcodes->body.SMSG_GM_TICKET_STATUS_UPDATE));
            break;
        case SMSG_UPDATE_INSTANCE_OWNERSHIP:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_UPDATE_INSTANCE_OWNERSHIP_read(reader, &opcodes->body.SMSG_UPDATE_INSTANCE_OWNERSHIP));
            break;
        case SMSG_SPELLINSTAKILLLOG:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELLINSTAKILLLOG_read(reader, &opcodes->body.SMSG_SPELLINSTAKILLLOG));
            break;
        case SMSG_SPELL_UPDATE_CHAIN_TARGETS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_SPELL_UPDATE_CHAIN_TARGETS_read(reader, &opcodes->body.SMSG_SPELL_UPDATE_CHAIN_TARGETS));
            break;
        case SMSG_EXPECTED_SPAM_RECORDS:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_EXPECTED_SPAM_RECORDS_read(reader, &opcodes->body.SMSG_EXPECTED_SPAM_RECORDS));
            break;
        case SMSG_DEFENSE_MESSAGE:
            WWM_CHECK_RETURN_CODE(vanilla_SMSG_DEFENSE_MESSAGE_read(reader, &opcodes->body.SMSG_DEFENSE_MESSAGE));
            break;
        default:
            break;
    }

    return WWM_RESULT_SUCCESS;
}

WOW_WORLD_MESSAGES_C_EXPORT void vanilla_server_opcode_free(VanillaServerOpcodeContainer* opcodes) {
    switch (opcodes->opcode) {
        case SMSG_CHAR_ENUM:
            vanilla_SMSG_CHAR_ENUM_free(&opcodes->body.SMSG_CHAR_ENUM);
            break;
        case SMSG_TRANSFER_PENDING:
            vanilla_SMSG_TRANSFER_PENDING_free(&opcodes->body.SMSG_TRANSFER_PENDING);
            break;
        case SMSG_NAME_QUERY_RESPONSE:
            vanilla_SMSG_NAME_QUERY_RESPONSE_free(&opcodes->body.SMSG_NAME_QUERY_RESPONSE);
            break;
        case SMSG_PET_NAME_QUERY_RESPONSE:
            vanilla_SMSG_PET_NAME_QUERY_RESPONSE_free(&opcodes->body.SMSG_PET_NAME_QUERY_RESPONSE);
            break;
        case SMSG_GUILD_QUERY_RESPONSE:
            vanilla_SMSG_GUILD_QUERY_RESPONSE_free(&opcodes->body.SMSG_GUILD_QUERY_RESPONSE);
            break;
        case SMSG_ITEM_QUERY_SINGLE_RESPONSE:
            vanilla_SMSG_ITEM_QUERY_SINGLE_RESPONSE_free(&opcodes->body.SMSG_ITEM_QUERY_SINGLE_RESPONSE);
            break;
        case SMSG_PAGE_TEXT_QUERY_RESPONSE:
            vanilla_SMSG_PAGE_TEXT_QUERY_RESPONSE_free(&opcodes->body.SMSG_PAGE_TEXT_QUERY_RESPONSE);
            break;
        case SMSG_QUEST_QUERY_RESPONSE:
            vanilla_SMSG_QUEST_QUERY_RESPONSE_free(&opcodes->body.SMSG_QUEST_QUERY_RESPONSE);
            break;
        case SMSG_GAMEOBJECT_QUERY_RESPONSE:
            vanilla_SMSG_GAMEOBJECT_QUERY_RESPONSE_free(&opcodes->body.SMSG_GAMEOBJECT_QUERY_RESPONSE);
            break;
        case SMSG_CREATURE_QUERY_RESPONSE:
            vanilla_SMSG_CREATURE_QUERY_RESPONSE_free(&opcodes->body.SMSG_CREATURE_QUERY_RESPONSE);
            break;
        case SMSG_WHO:
            vanilla_SMSG_WHO_free(&opcodes->body.SMSG_WHO);
            break;
        case SMSG_WHOIS:
            vanilla_SMSG_WHOIS_free(&opcodes->body.SMSG_WHOIS);
            break;
        case SMSG_FRIEND_LIST:
            vanilla_SMSG_FRIEND_LIST_free(&opcodes->body.SMSG_FRIEND_LIST);
            break;
        case SMSG_IGNORE_LIST:
            vanilla_SMSG_IGNORE_LIST_free(&opcodes->body.SMSG_IGNORE_LIST);
            break;
        case SMSG_GROUP_INVITE:
            vanilla_SMSG_GROUP_INVITE_free(&opcodes->body.SMSG_GROUP_INVITE);
            break;
        case SMSG_GROUP_DECLINE:
            vanilla_SMSG_GROUP_DECLINE_free(&opcodes->body.SMSG_GROUP_DECLINE);
            break;
        case SMSG_GROUP_SET_LEADER:
            vanilla_SMSG_GROUP_SET_LEADER_free(&opcodes->body.SMSG_GROUP_SET_LEADER);
            break;
        case SMSG_GROUP_LIST:
            vanilla_SMSG_GROUP_LIST_free(&opcodes->body.SMSG_GROUP_LIST);
            break;
        case SMSG_PARTY_MEMBER_STATS:
            vanilla_SMSG_PARTY_MEMBER_STATS_free(&opcodes->body.SMSG_PARTY_MEMBER_STATS);
            break;
        case SMSG_PARTY_COMMAND_RESULT:
            vanilla_SMSG_PARTY_COMMAND_RESULT_free(&opcodes->body.SMSG_PARTY_COMMAND_RESULT);
            break;
        case SMSG_GUILD_INVITE:
            vanilla_SMSG_GUILD_INVITE_free(&opcodes->body.SMSG_GUILD_INVITE);
            break;
        case SMSG_GUILD_INFO:
            vanilla_SMSG_GUILD_INFO_free(&opcodes->body.SMSG_GUILD_INFO);
            break;
        case SMSG_GUILD_ROSTER:
            vanilla_SMSG_GUILD_ROSTER_free(&opcodes->body.SMSG_GUILD_ROSTER);
            break;
        case SMSG_GUILD_EVENT:
            vanilla_SMSG_GUILD_EVENT_free(&opcodes->body.SMSG_GUILD_EVENT);
            break;
        case SMSG_GUILD_COMMAND_RESULT:
            vanilla_SMSG_GUILD_COMMAND_RESULT_free(&opcodes->body.SMSG_GUILD_COMMAND_RESULT);
            break;
        case SMSG_MESSAGECHAT:
            vanilla_SMSG_MESSAGECHAT_free(&opcodes->body.SMSG_MESSAGECHAT);
            break;
        case SMSG_CHANNEL_NOTIFY:
            vanilla_SMSG_CHANNEL_NOTIFY_free(&opcodes->body.SMSG_CHANNEL_NOTIFY);
            break;
        case SMSG_CHANNEL_LIST:
            vanilla_SMSG_CHANNEL_LIST_free(&opcodes->body.SMSG_CHANNEL_LIST);
            break;
        case SMSG_UPDATE_OBJECT:
            vanilla_SMSG_UPDATE_OBJECT_free(&opcodes->body.SMSG_UPDATE_OBJECT);
            break;
        case MSG_MOVE_START_FORWARD:
            vanilla_MSG_MOVE_START_FORWARD_Server_free(&opcodes->body.MSG_MOVE_START_FORWARD_Server);
            break;
        case MSG_MOVE_START_BACKWARD:
            vanilla_MSG_MOVE_START_BACKWARD_Server_free(&opcodes->body.MSG_MOVE_START_BACKWARD_Server);
            break;
        case MSG_MOVE_STOP:
            vanilla_MSG_MOVE_STOP_Server_free(&opcodes->body.MSG_MOVE_STOP_Server);
            break;
        case MSG_MOVE_START_STRAFE_LEFT:
            vanilla_MSG_MOVE_START_STRAFE_LEFT_Server_free(&opcodes->body.MSG_MOVE_START_STRAFE_LEFT_Server);
            break;
        case MSG_MOVE_START_STRAFE_RIGHT:
            vanilla_MSG_MOVE_START_STRAFE_RIGHT_Server_free(&opcodes->body.MSG_MOVE_START_STRAFE_RIGHT_Server);
            break;
        case MSG_MOVE_STOP_STRAFE:
            vanilla_MSG_MOVE_STOP_STRAFE_Server_free(&opcodes->body.MSG_MOVE_STOP_STRAFE_Server);
            break;
        case MSG_MOVE_JUMP:
            vanilla_MSG_MOVE_JUMP_Server_free(&opcodes->body.MSG_MOVE_JUMP_Server);
            break;
        case MSG_MOVE_START_TURN_LEFT:
            vanilla_MSG_MOVE_START_TURN_LEFT_Server_free(&opcodes->body.MSG_MOVE_START_TURN_LEFT_Server);
            break;
        case MSG_MOVE_START_TURN_RIGHT:
            vanilla_MSG_MOVE_START_TURN_RIGHT_Server_free(&opcodes->body.MSG_MOVE_START_TURN_RIGHT_Server);
            break;
        case MSG_MOVE_STOP_TURN:
            vanilla_MSG_MOVE_STOP_TURN_Server_free(&opcodes->body.MSG_MOVE_STOP_TURN_Server);
            break;
        case MSG_MOVE_START_PITCH_UP:
            vanilla_MSG_MOVE_START_PITCH_UP_Server_free(&opcodes->body.MSG_MOVE_START_PITCH_UP_Server);
            break;
        case MSG_MOVE_START_PITCH_DOWN:
            vanilla_MSG_MOVE_START_PITCH_DOWN_Server_free(&opcodes->body.MSG_MOVE_START_PITCH_DOWN_Server);
            break;
        case MSG_MOVE_STOP_PITCH:
            vanilla_MSG_MOVE_STOP_PITCH_Server_free(&opcodes->body.MSG_MOVE_STOP_PITCH_Server);
            break;
        case MSG_MOVE_SET_RUN_MODE:
            vanilla_MSG_MOVE_SET_RUN_MODE_Server_free(&opcodes->body.MSG_MOVE_SET_RUN_MODE_Server);
            break;
        case MSG_MOVE_SET_WALK_MODE:
            vanilla_MSG_MOVE_SET_WALK_MODE_Server_free(&opcodes->body.MSG_MOVE_SET_WALK_MODE_Server);
            break;
        case MSG_MOVE_TELEPORT_ACK:
            vanilla_MSG_MOVE_TELEPORT_ACK_Server_free(&opcodes->body.MSG_MOVE_TELEPORT_ACK_Server);
            break;
        case MSG_MOVE_FALL_LAND:
            vanilla_MSG_MOVE_FALL_LAND_Server_free(&opcodes->body.MSG_MOVE_FALL_LAND_Server);
            break;
        case MSG_MOVE_START_SWIM:
            vanilla_MSG_MOVE_START_SWIM_Server_free(&opcodes->body.MSG_MOVE_START_SWIM_Server);
            break;
        case MSG_MOVE_STOP_SWIM:
            vanilla_MSG_MOVE_STOP_SWIM_Server_free(&opcodes->body.MSG_MOVE_STOP_SWIM_Server);
            break;
        case MSG_MOVE_SET_FACING:
            vanilla_MSG_MOVE_SET_FACING_Server_free(&opcodes->body.MSG_MOVE_SET_FACING_Server);
            break;
        case MSG_MOVE_SET_PITCH:
            vanilla_MSG_MOVE_SET_PITCH_Server_free(&opcodes->body.MSG_MOVE_SET_PITCH_Server);
            break;
        case SMSG_MONSTER_MOVE:
            vanilla_SMSG_MONSTER_MOVE_free(&opcodes->body.SMSG_MONSTER_MOVE);
            break;
        case MSG_MOVE_HEARTBEAT:
            vanilla_MSG_MOVE_HEARTBEAT_Server_free(&opcodes->body.MSG_MOVE_HEARTBEAT_Server);
            break;
        case SMSG_TUTORIAL_FLAGS:
            vanilla_SMSG_TUTORIAL_FLAGS_free(&opcodes->body.SMSG_TUTORIAL_FLAGS);
            break;
        case SMSG_TEXT_EMOTE:
            vanilla_SMSG_TEXT_EMOTE_free(&opcodes->body.SMSG_TEXT_EMOTE);
            break;
        case SMSG_INVENTORY_CHANGE_FAILURE:
            vanilla_SMSG_INVENTORY_CHANGE_FAILURE_free(&opcodes->body.SMSG_INVENTORY_CHANGE_FAILURE);
            break;
        case SMSG_TRADE_STATUS:
            vanilla_SMSG_TRADE_STATUS_free(&opcodes->body.SMSG_TRADE_STATUS);
            break;
        case SMSG_TRADE_STATUS_EXTENDED:
            vanilla_SMSG_TRADE_STATUS_EXTENDED_free(&opcodes->body.SMSG_TRADE_STATUS_EXTENDED);
            break;
        case SMSG_INITIALIZE_FACTIONS:
            vanilla_SMSG_INITIALIZE_FACTIONS_free(&opcodes->body.SMSG_INITIALIZE_FACTIONS);
            break;
        case SMSG_SET_FACTION_STANDING:
            vanilla_SMSG_SET_FACTION_STANDING_free(&opcodes->body.SMSG_SET_FACTION_STANDING);
            break;
        case SMSG_ACTION_BUTTONS:
            vanilla_SMSG_ACTION_BUTTONS_free(&opcodes->body.SMSG_ACTION_BUTTONS);
            break;
        case SMSG_INITIAL_SPELLS:
            vanilla_SMSG_INITIAL_SPELLS_free(&opcodes->body.SMSG_INITIAL_SPELLS);
            break;
        case SMSG_CAST_RESULT:
            vanilla_SMSG_CAST_RESULT_free(&opcodes->body.SMSG_CAST_RESULT);
            break;
        case SMSG_SPELL_START:
            vanilla_SMSG_SPELL_START_free(&opcodes->body.SMSG_SPELL_START);
            break;
        case SMSG_SPELL_GO:
            vanilla_SMSG_SPELL_GO_free(&opcodes->body.SMSG_SPELL_GO);
            break;
        case SMSG_SPELL_COOLDOWN:
            vanilla_SMSG_SPELL_COOLDOWN_free(&opcodes->body.SMSG_SPELL_COOLDOWN);
            break;
        case SMSG_ATTACKERSTATEUPDATE:
            vanilla_SMSG_ATTACKERSTATEUPDATE_free(&opcodes->body.SMSG_ATTACKERSTATEUPDATE);
            break;
        case SMSG_RESURRECT_REQUEST:
            vanilla_SMSG_RESURRECT_REQUEST_free(&opcodes->body.SMSG_RESURRECT_REQUEST);
            break;
        case SMSG_LOOT_RESPONSE:
            vanilla_SMSG_LOOT_RESPONSE_free(&opcodes->body.SMSG_LOOT_RESPONSE);
            break;
        case SMSG_DUEL_WINNER:
            vanilla_SMSG_DUEL_WINNER_free(&opcodes->body.SMSG_DUEL_WINNER);
            break;
        case SMSG_PET_SPELLS:
            vanilla_SMSG_PET_SPELLS_free(&opcodes->body.SMSG_PET_SPELLS);
            break;
        case SMSG_GOSSIP_MESSAGE:
            vanilla_SMSG_GOSSIP_MESSAGE_free(&opcodes->body.SMSG_GOSSIP_MESSAGE);
            break;
        case SMSG_NPC_TEXT_UPDATE:
            vanilla_SMSG_NPC_TEXT_UPDATE_free(&opcodes->body.SMSG_NPC_TEXT_UPDATE);
            break;
        case SMSG_QUESTGIVER_QUEST_LIST:
            vanilla_SMSG_QUESTGIVER_QUEST_LIST_free(&opcodes->body.SMSG_QUESTGIVER_QUEST_LIST);
            break;
        case SMSG_QUESTGIVER_QUEST_DETAILS:
            vanilla_SMSG_QUESTGIVER_QUEST_DETAILS_free(&opcodes->body.SMSG_QUESTGIVER_QUEST_DETAILS);
            break;
        case SMSG_QUESTGIVER_REQUEST_ITEMS:
            vanilla_SMSG_QUESTGIVER_REQUEST_ITEMS_free(&opcodes->body.SMSG_QUESTGIVER_REQUEST_ITEMS);
            break;
        case SMSG_QUESTGIVER_OFFER_REWARD:
            vanilla_SMSG_QUESTGIVER_OFFER_REWARD_free(&opcodes->body.SMSG_QUESTGIVER_OFFER_REWARD);
            break;
        case SMSG_QUESTGIVER_QUEST_COMPLETE:
            vanilla_SMSG_QUESTGIVER_QUEST_COMPLETE_free(&opcodes->body.SMSG_QUESTGIVER_QUEST_COMPLETE);
            break;
        case SMSG_QUEST_CONFIRM_ACCEPT:
            vanilla_SMSG_QUEST_CONFIRM_ACCEPT_free(&opcodes->body.SMSG_QUEST_CONFIRM_ACCEPT);
            break;
        case SMSG_LIST_INVENTORY:
            vanilla_SMSG_LIST_INVENTORY_free(&opcodes->body.SMSG_LIST_INVENTORY);
            break;
        case SMSG_SHOWTAXINODES:
            vanilla_SMSG_SHOWTAXINODES_free(&opcodes->body.SMSG_SHOWTAXINODES);
            break;
        case SMSG_TRAINER_LIST:
            vanilla_SMSG_TRAINER_LIST_free(&opcodes->body.SMSG_TRAINER_LIST);
            break;
        case SMSG_PETITION_SHOWLIST:
            vanilla_SMSG_PETITION_SHOWLIST_free(&opcodes->body.SMSG_PETITION_SHOWLIST);
            break;
        case SMSG_PETITION_SHOW_SIGNATURES:
            vanilla_SMSG_PETITION_SHOW_SIGNATURES_free(&opcodes->body.SMSG_PETITION_SHOW_SIGNATURES);
            break;
        case SMSG_PETITION_QUERY_RESPONSE:
            vanilla_SMSG_PETITION_QUERY_RESPONSE_free(&opcodes->body.SMSG_PETITION_QUERY_RESPONSE);
            break;
        case SMSG_NOTIFICATION:
            vanilla_SMSG_NOTIFICATION_free(&opcodes->body.SMSG_NOTIFICATION);
            break;
        case SMSG_LOG_XPGAIN:
            vanilla_SMSG_LOG_XPGAIN_free(&opcodes->body.SMSG_LOG_XPGAIN);
            break;
        case SMSG_AUTH_RESPONSE:
            vanilla_SMSG_AUTH_RESPONSE_free(&opcodes->body.SMSG_AUTH_RESPONSE);
            break;
        case SMSG_COMPRESSED_UPDATE_OBJECT:
            vanilla_SMSG_COMPRESSED_UPDATE_OBJECT_free(&opcodes->body.SMSG_COMPRESSED_UPDATE_OBJECT);
            break;
        case SMSG_ACCOUNT_DATA_TIMES:
            vanilla_SMSG_ACCOUNT_DATA_TIMES_free(&opcodes->body.SMSG_ACCOUNT_DATA_TIMES);
            break;
        case SMSG_GMTICKET_GETTICKET:
            vanilla_SMSG_GMTICKET_GETTICKET_free(&opcodes->body.SMSG_GMTICKET_GETTICKET);
            break;
        case MSG_CORPSE_QUERY:
            vanilla_MSG_CORPSE_QUERY_Server_free(&opcodes->body.MSG_CORPSE_QUERY_Server);
            break;
        case SMSG_GOSSIP_POI:
            vanilla_SMSG_GOSSIP_POI_free(&opcodes->body.SMSG_GOSSIP_POI);
            break;
        case SMSG_SEND_MAIL_RESULT:
            vanilla_SMSG_SEND_MAIL_RESULT_free(&opcodes->body.SMSG_SEND_MAIL_RESULT);
            break;
        case SMSG_MAIL_LIST_RESULT:
            vanilla_SMSG_MAIL_LIST_RESULT_free(&opcodes->body.SMSG_MAIL_LIST_RESULT);
            break;
        case SMSG_BATTLEFIELD_LIST:
            vanilla_SMSG_BATTLEFIELD_LIST_free(&opcodes->body.SMSG_BATTLEFIELD_LIST);
            break;
        case SMSG_ITEM_TEXT_QUERY_RESPONSE:
            vanilla_SMSG_ITEM_TEXT_QUERY_RESPONSE_free(&opcodes->body.SMSG_ITEM_TEXT_QUERY_RESPONSE);
            break;
        case SMSG_SPELLLOGMISS:
            vanilla_SMSG_SPELLLOGMISS_free(&opcodes->body.SMSG_SPELLLOGMISS);
            break;
        case SMSG_SPELLLOGEXECUTE:
            vanilla_SMSG_SPELLLOGEXECUTE_free(&opcodes->body.SMSG_SPELLLOGEXECUTE);
            break;
        case SMSG_PERIODICAURALOG:
            vanilla_SMSG_PERIODICAURALOG_free(&opcodes->body.SMSG_PERIODICAURALOG);
            break;
        case SMSG_AUCTION_COMMAND_RESULT:
            vanilla_SMSG_AUCTION_COMMAND_RESULT_free(&opcodes->body.SMSG_AUCTION_COMMAND_RESULT);
            break;
        case SMSG_AUCTION_LIST_RESULT:
            vanilla_SMSG_AUCTION_LIST_RESULT_free(&opcodes->body.SMSG_AUCTION_LIST_RESULT);
            break;
        case SMSG_AUCTION_OWNER_LIST_RESULT:
            vanilla_SMSG_AUCTION_OWNER_LIST_RESULT_free(&opcodes->body.SMSG_AUCTION_OWNER_LIST_RESULT);
            break;
        case SMSG_DISPEL_FAILED:
            vanilla_SMSG_DISPEL_FAILED_free(&opcodes->body.SMSG_DISPEL_FAILED);
            break;
        case SMSG_AUCTION_BIDDER_LIST_RESULT:
            vanilla_SMSG_AUCTION_BIDDER_LIST_RESULT_free(&opcodes->body.SMSG_AUCTION_BIDDER_LIST_RESULT);
            break;
        case MSG_LIST_STABLED_PETS:
            vanilla_MSG_LIST_STABLED_PETS_Server_free(&opcodes->body.MSG_LIST_STABLED_PETS_Server);
            break;
        case SMSG_SPELLDISPELLOG:
            vanilla_SMSG_SPELLDISPELLOG_free(&opcodes->body.SMSG_SPELLDISPELLOG);
            break;
        case SMSG_SERVER_MESSAGE:
            vanilla_SMSG_SERVER_MESSAGE_free(&opcodes->body.SMSG_SERVER_MESSAGE);
            break;
        case SMSG_LOOT_MASTER_LIST:
            vanilla_SMSG_LOOT_MASTER_LIST_free(&opcodes->body.SMSG_LOOT_MASTER_LIST);
            break;
        case SMSG_SET_FORCED_REACTIONS:
            vanilla_SMSG_SET_FORCED_REACTIONS_free(&opcodes->body.SMSG_SET_FORCED_REACTIONS);
            break;
        case SMSG_CHAT_PLAYER_NOT_FOUND:
            vanilla_SMSG_CHAT_PLAYER_NOT_FOUND_free(&opcodes->body.SMSG_CHAT_PLAYER_NOT_FOUND);
            break;
        case SMSG_MONSTER_MOVE_TRANSPORT:
            vanilla_SMSG_MONSTER_MOVE_TRANSPORT_free(&opcodes->body.SMSG_MONSTER_MOVE_TRANSPORT);
            break;
        case MSG_MOVE_FEATHER_FALL:
            vanilla_MSG_MOVE_FEATHER_FALL_Server_free(&opcodes->body.MSG_MOVE_FEATHER_FALL_Server);
            break;
        case MSG_MOVE_WATER_WALK:
            vanilla_MSG_MOVE_WATER_WALK_free(&opcodes->body.MSG_MOVE_WATER_WALK);
            break;
        case SMSG_AREA_TRIGGER_MESSAGE:
            vanilla_SMSG_AREA_TRIGGER_MESSAGE_free(&opcodes->body.SMSG_AREA_TRIGGER_MESSAGE);
            break;
        case MSG_PETITION_RENAME:
            vanilla_MSG_PETITION_RENAME_free(&opcodes->body.MSG_PETITION_RENAME);
            break;
        case SMSG_INIT_WORLD_STATES:
            vanilla_SMSG_INIT_WORLD_STATES_free(&opcodes->body.SMSG_INIT_WORLD_STATES);
            break;
        case SMSG_ITEM_NAME_QUERY_RESPONSE:
            vanilla_SMSG_ITEM_NAME_QUERY_RESPONSE_free(&opcodes->body.SMSG_ITEM_NAME_QUERY_RESPONSE);
            break;
        case SMSG_CHAR_RENAME:
            vanilla_SMSG_CHAR_RENAME_free(&opcodes->body.SMSG_CHAR_RENAME);
            break;
        case SMSG_RAID_INSTANCE_INFO:
            vanilla_SMSG_RAID_INSTANCE_INFO_free(&opcodes->body.SMSG_RAID_INSTANCE_INFO);
            break;
        case SMSG_BATTLEFIELD_STATUS:
            vanilla_SMSG_BATTLEFIELD_STATUS_free(&opcodes->body.SMSG_BATTLEFIELD_STATUS);
            break;
        case MSG_PVP_LOG_DATA:
            vanilla_MSG_PVP_LOG_DATA_Server_free(&opcodes->body.MSG_PVP_LOG_DATA_Server);
            break;
        case SMSG_WARDEN_DATA:
            vanilla_SMSG_WARDEN_DATA_free(&opcodes->body.SMSG_WARDEN_DATA);
            break;
        case MSG_BATTLEGROUND_PLAYER_POSITIONS:
            vanilla_MSG_BATTLEGROUND_PLAYER_POSITIONS_Server_free(&opcodes->body.MSG_BATTLEGROUND_PLAYER_POSITIONS_Server);
            break;
        case SMSG_ADDON_INFO:
            vanilla_SMSG_ADDON_INFO_free(&opcodes->body.SMSG_ADDON_INFO);
            break;
        case SMSG_PARTY_MEMBER_STATS_FULL:
            vanilla_SMSG_PARTY_MEMBER_STATS_FULL_free(&opcodes->body.SMSG_PARTY_MEMBER_STATS_FULL);
            break;
        case SMSG_COMPRESSED_MOVES:
            vanilla_SMSG_COMPRESSED_MOVES_free(&opcodes->body.SMSG_COMPRESSED_MOVES);
            break;
        case MSG_RAID_TARGET_UPDATE:
            vanilla_MSG_RAID_TARGET_UPDATE_Server_free(&opcodes->body.MSG_RAID_TARGET_UPDATE_Server);
            break;
        case MSG_RAID_READY_CHECK:
            vanilla_MSG_RAID_READY_CHECK_Server_free(&opcodes->body.MSG_RAID_READY_CHECK_Server);
            break;
        case SMSG_SPELL_UPDATE_CHAIN_TARGETS:
            vanilla_SMSG_SPELL_UPDATE_CHAIN_TARGETS_free(&opcodes->body.SMSG_SPELL_UPDATE_CHAIN_TARGETS);
            break;
        case SMSG_EXPECTED_SPAM_RECORDS:
            vanilla_SMSG_EXPECTED_SPAM_RECORDS_free(&opcodes->body.SMSG_EXPECTED_SPAM_RECORDS);
            break;
        case SMSG_DEFENSE_MESSAGE:
            vanilla_SMSG_DEFENSE_MESSAGE_free(&opcodes->body.SMSG_DEFENSE_MESSAGE);
            break;
        default:
            break;
    }
}

