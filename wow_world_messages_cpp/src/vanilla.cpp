#include "util.hpp"

#include "wow_world_messages_cpp/vanilla.hpp"

#include <string.h> /* memset */

namespace wow_world_messages {
    namespace all {
        /* forward declare all struct read/write */
        ::wow_world_messages::all::Vector3d Vector3d_read(Reader& reader);
        void Vector3d_write(Writer& writer, const ::wow_world_messages::all::Vector3d& obj);

        /* forward declare all struct read/write */
        ::wow_world_messages::all::Vector2d Vector2d_read(Reader& reader);
        void Vector2d_write(Writer& writer, const ::wow_world_messages::all::Vector2d& obj);

    } // namespace all
} // namespace wow_world_messages

namespace wow_world_messages {
namespace vanilla {
static size_t aura_mask_size(const AuraMask& mask) {
    size_t size = 4; /* uint32_t header */

    for(const auto v : mask.auras) {
        if(v != 0) {
            size += 2; /* uint16_t members */
        }
    }

    return size;
}

static AuraMask aura_mask_read(Reader& reader) {
    uint32_t header = reader.read_u32();

    AuraMask mask;

    for(int i = 0; i < AURA_MASK_SIZE; ++i) {
        mask.auras[i] = 0; /* initialize to 0 */

        if ((header & (1 << i)) != 0) {
            mask.auras[i] = reader.read_u16();
        }
    }

    return mask;
}

static void aura_mask_write(Writer& writer, const AuraMask& mask) {
    uint32_t header = 0;

    for(int i = 0; i < AURA_MASK_SIZE; ++i) {
        if (mask.auras[i] != 0) {
            header |= 1 << i;
        }
    }

    writer.write_u32(header);

    for(int i = 0; i < AURA_MASK_SIZE; ++i) {
        if (mask.auras[i] != 0) {
            writer.write_u16(mask.auras[i]);
        }
    }
}

WOW_WORLD_MESSAGES_CPP_EXPORT void update_mask_set(UpdateMask& mask, UpdateMaskValues offset, uint32_t value) {
    uint32_t block = static_cast<uint32_t>(offset) / 32;
    uint32_t bit = static_cast<uint32_t>(offset) % 32;

    mask.headers[block] |= 1 << bit;
    mask.values[static_cast<uint32_t>(offset)] = value;
}

WOW_WORLD_MESSAGES_CPP_EXPORT uint32_t update_mask_get(const UpdateMask& mask, UpdateMaskValues offset) {
    uint32_t block = static_cast<uint32_t>(offset) / 32;
    uint32_t bit = static_cast<uint32_t>(offset) % 32;

    if((mask.headers[block] & 1 << bit) != 0) {
        return mask.values[static_cast<uint32_t>(offset)];
    }

    return 0;
}

static void update_mask_write(Writer& writer, const UpdateMask& mask) {
    uint8_t amount_of_headers = 0;

    for (int i = 0; i < UPDATE_MASK_HEADERS_LENGTH; ++i) {
        uint32_t header = mask.headers[i];
        if (header != 0) {
            amount_of_headers = i + 1;
        }
    }

    writer.write_u8(amount_of_headers);

    for (int i = 0; i < amount_of_headers; ++i) {
        writer.write_u32(mask.headers[i]);
    }

    for (int i = 0; i < amount_of_headers; ++i) {
        uint32_t header = mask.headers[i];
        for (int j = 0; j < 32; ++j) {
            if ((header & (1 << j)) != 0) {
                writer.write_u32(mask.values[i * 32 + j]);
            }
        }
    }
}

static UpdateMask update_mask_read(Reader& reader) {
    UpdateMask mask{};

    uint8_t amount_of_headers = reader.read_u8();

    for (int i = 0; i < amount_of_headers; ++i) {
        mask.headers[i] = reader.read_u32();
    }

    for (int i = 0; i < amount_of_headers; ++i) {
        uint32_t header = mask.headers[i];
        for (int j = 0; j < 32; ++j) {
            if ((header & (1 << j)) != 0) {
                mask.values[i * 32 + j] = reader.read_u32();
            }
        }
    }

    return mask;
}

static size_t update_mask_size(const UpdateMask& mask) {
    size_t max_header = 0;
    size_t amount_of_values = 0;

    size_t size = 1; /* initial u8 */

    for(int i = 0; i < UPDATE_MASK_HEADERS_LENGTH; ++i) {
        uint32_t header = mask.headers[i];
        for(int j = 0; j < 32; ++j) {
            if((header & (1 << j)) != 0) {
                max_header = i + 1;
                amount_of_values += 4;
            }
        }
    }

    return size + amount_of_values + (max_header * 4);
}
static size_t Addon_size(const Addon& obj) {
    size_t _size = 3;

    if (obj.info_block == InfoBlock::AVAILABLE) {
        _size += 5;

        if (obj.key_version == KeyVersion::ONE|| obj.key_version == KeyVersion::TWO|| obj.key_version == KeyVersion::THREE|| obj.key_version == KeyVersion::FOUR|| obj.key_version == KeyVersion::FIVE|| obj.key_version == KeyVersion::SIX|| obj.key_version == KeyVersion::SEVEN|| obj.key_version == KeyVersion::EIGHT|| obj.key_version == KeyVersion::NINE) {
            _size += 256;
        }

    }

    if (obj.url_info == UrlInfo::AVAILABLE) {
        _size += 1 + obj.url.size();
    }

    return _size;
}

Addon Addon_read(Reader& reader) {
    Addon obj;

    obj.addon_type = static_cast<AddonType>(reader.read_u8());

    obj.info_block = static_cast<InfoBlock>(reader.read_u8());

    if (obj.info_block == InfoBlock::AVAILABLE) {
        obj.key_version = static_cast<KeyVersion>(reader.read_u8());

        if (obj.key_version == KeyVersion::ONE|| obj.key_version == KeyVersion::TWO|| obj.key_version == KeyVersion::THREE|| obj.key_version == KeyVersion::FOUR|| obj.key_version == KeyVersion::FIVE|| obj.key_version == KeyVersion::SIX|| obj.key_version == KeyVersion::SEVEN|| obj.key_version == KeyVersion::EIGHT|| obj.key_version == KeyVersion::NINE) {
            for (auto i = 0; i < 256; ++i) {
                obj.public_key[i] = reader.read_u8();
            }

        }
        obj.update_available_flag = reader.read_u32();

    }
    obj.url_info = static_cast<UrlInfo>(reader.read_u8());

    if (obj.url_info == UrlInfo::AVAILABLE) {
        obj.url = reader.read_cstring();

    }
    return obj;
}

void Addon_write(Writer& writer, const Addon& obj) {
    writer.write_u8(static_cast<uint8_t>(obj.addon_type));

    writer.write_u8(static_cast<uint8_t>(obj.info_block));

    if (obj.info_block == InfoBlock::AVAILABLE) {
        writer.write_u8(static_cast<uint8_t>(obj.key_version));

        if (obj.key_version == KeyVersion::ONE|| obj.key_version == KeyVersion::TWO|| obj.key_version == KeyVersion::THREE|| obj.key_version == KeyVersion::FOUR|| obj.key_version == KeyVersion::FIVE|| obj.key_version == KeyVersion::SIX|| obj.key_version == KeyVersion::SEVEN|| obj.key_version == KeyVersion::EIGHT|| obj.key_version == KeyVersion::NINE) {
            for (const auto& v : obj.public_key) {
                writer.write_u8(v);
            }

        }
        writer.write_u32(obj.update_available_flag);

    }
    writer.write_u8(static_cast<uint8_t>(obj.url_info));

    if (obj.url_info == UrlInfo::AVAILABLE) {
        writer.write_cstring(obj.url);

    }
}

static size_t AddonInfo_size(const AddonInfo& obj) {
    return 10 + obj.addon_name.size();
}

AddonInfo AddonInfo_read(Reader& reader) {
    AddonInfo obj;

    obj.addon_name = reader.read_cstring();

    obj.addon_has_signature = reader.read_u8();

    obj.addon_crc = reader.read_u32();

    obj.addon_extra_crc = reader.read_u32();

    return obj;
}

void AddonInfo_write(Writer& writer, const AddonInfo& obj) {
    writer.write_cstring(obj.addon_name);

    writer.write_u8(obj.addon_has_signature);

    writer.write_u32(obj.addon_crc);

    writer.write_u32(obj.addon_extra_crc);

}

AuctionListItem AuctionListItem_read(Reader& reader) {
    AuctionListItem obj;

    obj.id = reader.read_u32();

    obj.item = reader.read_u32();

    obj.item_enchantment = reader.read_u32();

    obj.item_random_property_id = reader.read_u32();

    obj.item_suffix_factor = reader.read_u32();

    obj.item_count = reader.read_u32();

    obj.item_charges = reader.read_u32();

    obj.item_owner = reader.read_u64();

    obj.start_bid = reader.read_u32();

    obj.minimum_bid = reader.read_u32();

    obj.buyout_amount = reader.read_u32();

    obj.time_left = reader.read_u32();

    obj.highest_bidder = reader.read_u64();

    obj.highest_bid = reader.read_u32();

    return obj;
}

void AuctionListItem_write(Writer& writer, const AuctionListItem& obj) {
    writer.write_u32(obj.id);

    writer.write_u32(obj.item);

    writer.write_u32(obj.item_enchantment);

    writer.write_u32(obj.item_random_property_id);

    writer.write_u32(obj.item_suffix_factor);

    writer.write_u32(obj.item_count);

    writer.write_u32(obj.item_charges);

    writer.write_u64(obj.item_owner);

    writer.write_u32(obj.start_bid);

    writer.write_u32(obj.minimum_bid);

    writer.write_u32(obj.buyout_amount);

    writer.write_u32(obj.time_left);

    writer.write_u64(obj.highest_bidder);

    writer.write_u32(obj.highest_bid);

}

static size_t AuraLog_size(const AuraLog& obj) {
    size_t _size = 4;

    if (obj.aura_type == AuraType::PERIODIC_DAMAGE|| obj.aura_type == AuraType::PERIODIC_DAMAGE_PERCENT) {
        _size += 13;
    }
    else if (obj.aura_type == AuraType::PERIODIC_HEAL|| obj.aura_type == AuraType::OBS_MOD_HEALTH) {
        _size += 4;
    }
    else if (obj.aura_type == AuraType::OBS_MOD_MANA|| obj.aura_type == AuraType::PERIODIC_ENERGIZE) {
        _size += 8;
    }
    else if (obj.aura_type == AuraType::PERIODIC_MANA_LEECH) {
        _size += 12;
    }

    return _size;
}

AuraLog AuraLog_read(Reader& reader) {
    AuraLog obj;

    obj.aura_type = static_cast<AuraType>(reader.read_u32());

    if (obj.aura_type == AuraType::PERIODIC_DAMAGE|| obj.aura_type == AuraType::PERIODIC_DAMAGE_PERCENT) {
        obj.damage1 = reader.read_u32();

        obj.school = static_cast<SpellSchool>(reader.read_u8());

        obj.absorbed = reader.read_u32();

        obj.resisted = reader.read_u32();

    }
    else if (obj.aura_type == AuraType::PERIODIC_HEAL|| obj.aura_type == AuraType::OBS_MOD_HEALTH) {
        obj.damage2 = reader.read_u32();

    }
    else if (obj.aura_type == AuraType::OBS_MOD_MANA|| obj.aura_type == AuraType::PERIODIC_ENERGIZE) {
        obj.misc_value1 = reader.read_u32();

        obj.damage3 = reader.read_u32();

    }
    else if (obj.aura_type == AuraType::PERIODIC_MANA_LEECH) {
        obj.misc_value2 = reader.read_u32();

        obj.damage = reader.read_u32();

        obj.gain_multiplier = reader.read_float();

    }
    return obj;
}

void AuraLog_write(Writer& writer, const AuraLog& obj) {
    writer.write_u32(static_cast<uint32_t>(obj.aura_type));

    if (obj.aura_type == AuraType::PERIODIC_DAMAGE|| obj.aura_type == AuraType::PERIODIC_DAMAGE_PERCENT) {
        writer.write_u32(obj.damage1);

        writer.write_u8(static_cast<uint8_t>(obj.school));

        writer.write_u32(obj.absorbed);

        writer.write_u32(obj.resisted);

    }
    else if (obj.aura_type == AuraType::PERIODIC_HEAL|| obj.aura_type == AuraType::OBS_MOD_HEALTH) {
        writer.write_u32(obj.damage2);

    }
    else if (obj.aura_type == AuraType::OBS_MOD_MANA|| obj.aura_type == AuraType::PERIODIC_ENERGIZE) {
        writer.write_u32(obj.misc_value1);

        writer.write_u32(obj.damage3);

    }
    else if (obj.aura_type == AuraType::PERIODIC_MANA_LEECH) {
        writer.write_u32(obj.misc_value2);

        writer.write_u32(obj.damage);

        writer.write_float(obj.gain_multiplier);

    }
}

static size_t BattlegroundPlayer_size(const BattlegroundPlayer& obj) {
    return 32 + 4 * obj.fields.size();
}

BattlegroundPlayer BattlegroundPlayer_read(Reader& reader) {
    BattlegroundPlayer obj;

    obj.player = reader.read_u64();

    obj.rank = static_cast<PvpRank>(reader.read_u32());

    obj.killing_blows = reader.read_u32();

    obj.honorable_kills = reader.read_u32();

    obj.deaths = reader.read_u32();

    obj.bonus_honor = reader.read_u32();

    obj.amount_of_extra_fields = reader.read_u32();

    for (auto i = 0; i < obj.amount_of_extra_fields; ++i) {
        obj.fields.push_back(reader.read_u32());
    }

    return obj;
}

void BattlegroundPlayer_write(Writer& writer, const BattlegroundPlayer& obj) {
    writer.write_u64(obj.player);

    writer.write_u32(static_cast<uint32_t>(obj.rank));

    writer.write_u32(obj.killing_blows);

    writer.write_u32(obj.honorable_kills);

    writer.write_u32(obj.deaths);

    writer.write_u32(obj.bonus_honor);

    writer.write_u32(obj.amount_of_extra_fields);

    for (const auto& v : obj.fields) {
        writer.write_u32(v);
    }

}

BattlegroundPlayerPosition BattlegroundPlayerPosition_read(Reader& reader) {
    BattlegroundPlayerPosition obj;

    obj.player = reader.read_u64();

    obj.position_x = reader.read_float();

    obj.position_y = reader.read_float();

    return obj;
}

void BattlegroundPlayerPosition_write(Writer& writer, const BattlegroundPlayerPosition& obj) {
    writer.write_u64(obj.player);

    writer.write_float(obj.position_x);

    writer.write_float(obj.position_y);

}

ChannelMember ChannelMember_read(Reader& reader) {
    ChannelMember obj;

    obj.guid = reader.read_u64();

    obj.member_flags = static_cast<ChannelMemberFlags>(reader.read_u8());

    return obj;
}

void ChannelMember_write(Writer& writer, const ChannelMember& obj) {
    writer.write_u64(obj.guid);

    writer.write_u8(static_cast<uint8_t>(obj.member_flags));

}

CharacterGear CharacterGear_read(Reader& reader) {
    CharacterGear obj;

    obj.equipment_display_id = reader.read_u32();

    obj.inventory_type = static_cast<InventoryType>(reader.read_u8());

    return obj;
}

void CharacterGear_write(Writer& writer, const CharacterGear& obj) {
    writer.write_u32(obj.equipment_display_id);

    writer.write_u8(static_cast<uint8_t>(obj.inventory_type));

}

static size_t Character_size(const Character& obj) {
    return 159 + obj.name.size();
}

Character Character_read(Reader& reader) {
    Character obj;

    obj.guid = reader.read_u64();

    obj.name = reader.read_cstring();

    obj.race = static_cast<Race>(reader.read_u8());

    obj.class_type = static_cast<Class>(reader.read_u8());

    obj.gender = static_cast<Gender>(reader.read_u8());

    obj.skin = reader.read_u8();

    obj.face = reader.read_u8();

    obj.hair_style = reader.read_u8();

    obj.hair_color = reader.read_u8();

    obj.facial_hair = reader.read_u8();

    obj.level = reader.read_u8();

    obj.area = static_cast<Area>(reader.read_u32());

    obj.map = static_cast<Map>(reader.read_u32());

    obj.position = ::wow_world_messages::all::Vector3d_read(reader);

    obj.guild_id = reader.read_u32();

    obj.flags = static_cast<CharacterFlags>(reader.read_u32());

    obj.first_login = reader.read_bool8();

    obj.pet_display_id = reader.read_u32();

    obj.pet_level = reader.read_u32();

    obj.pet_family = static_cast<CreatureFamily>(reader.read_u32());

    for (auto i = 0; i < 19; ++i) {
        obj.equipment[i] = ::wow_world_messages::vanilla::CharacterGear_read(reader);
    }

    (void)reader.read_u32();

    (void)reader.read_u8();

    return obj;
}

void Character_write(Writer& writer, const Character& obj) {
    writer.write_u64(obj.guid);

    writer.write_cstring(obj.name);

    writer.write_u8(static_cast<uint8_t>(obj.race));

    writer.write_u8(static_cast<uint8_t>(obj.class_type));

    writer.write_u8(static_cast<uint8_t>(obj.gender));

    writer.write_u8(obj.skin);

    writer.write_u8(obj.face);

    writer.write_u8(obj.hair_style);

    writer.write_u8(obj.hair_color);

    writer.write_u8(obj.facial_hair);

    writer.write_u8(obj.level);

    writer.write_u32(static_cast<uint32_t>(obj.area));

    writer.write_u32(static_cast<uint32_t>(obj.map));

    Vector3d_write(writer, obj.position);

    writer.write_u32(obj.guild_id);

    writer.write_u32(static_cast<uint32_t>(obj.flags));

    writer.write_bool8(obj.first_login);

    writer.write_u32(obj.pet_display_id);

    writer.write_u32(obj.pet_level);

    writer.write_u32(static_cast<uint32_t>(obj.pet_family));

    for (const auto& v : obj.equipment) {
        CharacterGear_write(writer, v);
    }

    writer.write_u32(0);

    writer.write_u8(0);

}

static size_t MonsterMove_size(const MonsterMove& obj) {
    size_t _size = 17;

    if (obj.move_type == MonsterMoveType::FACING_TARGET) {
        _size += 8;
    }
    else if (obj.move_type == MonsterMoveType::FACING_ANGLE) {
        _size += 4;
    }
    else if (obj.move_type == MonsterMoveType::FACING_SPOT) {
        _size += 12;
    }

    if (obj.move_type == MonsterMoveType::NORMAL|| obj.move_type == MonsterMoveType::FACING_SPOT|| obj.move_type == MonsterMoveType::FACING_TARGET|| obj.move_type == MonsterMoveType::FACING_ANGLE) {
        _size += 8 + ::wow_world_messages::util::wwm_monster_move_spline_size(obj.splines);
    }

    return _size;
}

MonsterMove MonsterMove_read(Reader& reader) {
    MonsterMove obj;

    obj.spline_point = ::wow_world_messages::all::Vector3d_read(reader);

    obj.spline_id = reader.read_u32();

    obj.move_type = static_cast<MonsterMoveType>(reader.read_u8());

    if (obj.move_type == MonsterMoveType::FACING_TARGET) {
        obj.target = reader.read_u64();

    }
    else if (obj.move_type == MonsterMoveType::FACING_ANGLE) {
        obj.angle = reader.read_float();

    }
    else if (obj.move_type == MonsterMoveType::FACING_SPOT) {
        obj.position = ::wow_world_messages::all::Vector3d_read(reader);

    }
    if (obj.move_type == MonsterMoveType::NORMAL|| obj.move_type == MonsterMoveType::FACING_SPOT|| obj.move_type == MonsterMoveType::FACING_TARGET|| obj.move_type == MonsterMoveType::FACING_ANGLE) {
        obj.spline_flags = static_cast<SplineFlag>(reader.read_u32());

        obj.duration = reader.read_u32();

        obj.splines = ::wow_world_messages::util::wwm_read_monster_move_spline(reader);

    }
    return obj;
}

void MonsterMove_write(Writer& writer, const MonsterMove& obj) {
    Vector3d_write(writer, obj.spline_point);

    writer.write_u32(obj.spline_id);

    writer.write_u8(static_cast<uint8_t>(obj.move_type));

    if (obj.move_type == MonsterMoveType::FACING_TARGET) {
        writer.write_u64(obj.target);

    }
    else if (obj.move_type == MonsterMoveType::FACING_ANGLE) {
        writer.write_float(obj.angle);

    }
    else if (obj.move_type == MonsterMoveType::FACING_SPOT) {
        Vector3d_write(writer, obj.position);

    }
    if (obj.move_type == MonsterMoveType::NORMAL|| obj.move_type == MonsterMoveType::FACING_SPOT|| obj.move_type == MonsterMoveType::FACING_TARGET|| obj.move_type == MonsterMoveType::FACING_ANGLE) {
        writer.write_u32(static_cast<uint32_t>(obj.spline_flags));

        writer.write_u32(obj.duration);

        ::wow_world_messages::util::wwm_write_monster_move_spline(writer, obj.splines);

    }
}

static size_t CompressedMove_size(const CompressedMove& obj) {
    size_t _size = 3 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);

    if (obj.opcode == CompressedMoveOpcode::SMSG_SPLINE_SET_RUN_SPEED) {
        _size += 4;
    }
    else if (obj.opcode == CompressedMoveOpcode::SMSG_MONSTER_MOVE) {
        _size += 0 + MonsterMove_size(obj.monster_move);
    }
    else if (obj.opcode == CompressedMoveOpcode::SMSG_MONSTER_MOVE_TRANSPORT) {
        _size += 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.transport) + MonsterMove_size(obj.monster_move_transport);
    }

    return _size;
}

CompressedMove CompressedMove_read(Reader& reader) {
    CompressedMove obj;

    (void)reader.read_u8();

    obj.opcode = static_cast<CompressedMoveOpcode>(reader.read_u16());

    obj.guid = reader.read_packed_guid();

    if (obj.opcode == CompressedMoveOpcode::SMSG_SPLINE_SET_RUN_SPEED) {
        obj.speed = reader.read_float();

    }
    else if (obj.opcode == CompressedMoveOpcode::SMSG_MONSTER_MOVE) {
        obj.monster_move = ::wow_world_messages::vanilla::MonsterMove_read(reader);

    }
    else if (obj.opcode == CompressedMoveOpcode::SMSG_MONSTER_MOVE_TRANSPORT) {
        obj.transport = reader.read_packed_guid();

        obj.monster_move_transport = ::wow_world_messages::vanilla::MonsterMove_read(reader);

    }
    return obj;
}

void CompressedMove_write(Writer& writer, const CompressedMove& obj) {
    writer.write_u8(static_cast<uint8_t>(CompressedMove_size(obj)));

    writer.write_u16(static_cast<uint16_t>(obj.opcode));

    writer.write_packed_guid(obj.guid);

    if (obj.opcode == CompressedMoveOpcode::SMSG_SPLINE_SET_RUN_SPEED) {
        writer.write_float(obj.speed);

    }
    else if (obj.opcode == CompressedMoveOpcode::SMSG_MONSTER_MOVE) {
        MonsterMove_write(writer, obj.monster_move);

    }
    else if (obj.opcode == CompressedMoveOpcode::SMSG_MONSTER_MOVE_TRANSPORT) {
        writer.write_packed_guid(obj.transport);

        MonsterMove_write(writer, obj.monster_move_transport);

    }
}

CooldownSpell CooldownSpell_read(Reader& reader) {
    CooldownSpell obj;

    obj.spell_id = reader.read_u16();

    obj.item_id = reader.read_u16();

    obj.spell_category = reader.read_u16();

    obj.cooldown = reader.read_u32();

    obj.category_cooldown = reader.read_u32();

    return obj;
}

void CooldownSpell_write(Writer& writer, const CooldownSpell& obj) {
    writer.write_u16(obj.spell_id);

    writer.write_u16(obj.item_id);

    writer.write_u16(obj.spell_category);

    writer.write_u32(obj.cooldown);

    writer.write_u32(obj.category_cooldown);

}

DamageInfo DamageInfo_read(Reader& reader) {
    DamageInfo obj;

    obj.spell_school_mask = reader.read_u32();

    obj.damage_float = reader.read_float();

    obj.damage_uint = reader.read_u32();

    obj.absorb = reader.read_u32();

    obj.resist = reader.read_u32();

    return obj;
}

void DamageInfo_write(Writer& writer, const DamageInfo& obj) {
    writer.write_u32(obj.spell_school_mask);

    writer.write_float(obj.damage_float);

    writer.write_u32(obj.damage_uint);

    writer.write_u32(obj.absorb);

    writer.write_u32(obj.resist);

}

FactionInitializer FactionInitializer_read(Reader& reader) {
    FactionInitializer obj;

    obj.flag = static_cast<FactionFlag>(reader.read_u8());

    obj.standing = reader.read_u32();

    return obj;
}

void FactionInitializer_write(Writer& writer, const FactionInitializer& obj) {
    writer.write_u8(static_cast<uint8_t>(obj.flag));

    writer.write_u32(obj.standing);

}

FactionStanding FactionStanding_read(Reader& reader) {
    FactionStanding obj;

    obj.faction = static_cast<Faction>(reader.read_u16());

    obj.standing = reader.read_u32();

    return obj;
}

void FactionStanding_write(Writer& writer, const FactionStanding& obj) {
    writer.write_u16(static_cast<uint16_t>(obj.faction));

    writer.write_u32(obj.standing);

}

ForcedReaction ForcedReaction_read(Reader& reader) {
    ForcedReaction obj;

    obj.faction = static_cast<Faction>(reader.read_u16());

    obj.reputation_rank = reader.read_u32();

    return obj;
}

void ForcedReaction_write(Writer& writer, const ForcedReaction& obj) {
    writer.write_u16(static_cast<uint16_t>(obj.faction));

    writer.write_u32(obj.reputation_rank);

}

static size_t Friend_size(const Friend& obj) {
    size_t _size = 9;

    if (obj.status == FriendStatus::ONLINE|| obj.status == FriendStatus::AFK|| obj.status == FriendStatus::UNKNOWN3|| obj.status == FriendStatus::DND) {
        _size += 12;
    }

    return _size;
}

Friend Friend_read(Reader& reader) {
    Friend obj;

    obj.guid = reader.read_u64();

    obj.status = static_cast<FriendStatus>(reader.read_u8());

    if (obj.status == FriendStatus::ONLINE|| obj.status == FriendStatus::AFK|| obj.status == FriendStatus::UNKNOWN3|| obj.status == FriendStatus::DND) {
        obj.area = static_cast<Area>(reader.read_u32());

        obj.level = reader.read_u32();

        obj.class_type = static_cast<Class>(reader.read_u32());

    }
    return obj;
}

void Friend_write(Writer& writer, const Friend& obj) {
    writer.write_u64(obj.guid);

    writer.write_u8(static_cast<uint8_t>(obj.status));

    if (obj.status == FriendStatus::ONLINE|| obj.status == FriendStatus::AFK|| obj.status == FriendStatus::UNKNOWN3|| obj.status == FriendStatus::DND) {
        writer.write_u32(static_cast<uint32_t>(obj.area));

        writer.write_u32(obj.level);

        writer.write_u32(static_cast<uint32_t>(obj.class_type));

    }
}

static size_t GmSurveyQuestion_size(const GmSurveyQuestion& obj) {
    return 6 + obj.comment.size();
}

GmSurveyQuestion GmSurveyQuestion_read(Reader& reader) {
    GmSurveyQuestion obj;

    obj.question_id = reader.read_u32();

    obj.answer = reader.read_u8();

    obj.comment = reader.read_cstring();

    return obj;
}

void GmSurveyQuestion_write(Writer& writer, const GmSurveyQuestion& obj) {
    writer.write_u32(obj.question_id);

    writer.write_u8(obj.answer);

    writer.write_cstring(obj.comment);

}

static size_t GossipItem_size(const GossipItem& obj) {
    return 7 + obj.message.size();
}

GossipItem GossipItem_read(Reader& reader) {
    GossipItem obj;

    obj.id = reader.read_u32();

    obj.item_icon = reader.read_u8();

    obj.coded = reader.read_bool8();

    obj.message = reader.read_cstring();

    return obj;
}

void GossipItem_write(Writer& writer, const GossipItem& obj) {
    writer.write_u32(obj.id);

    writer.write_u8(obj.item_icon);

    writer.write_bool8(obj.coded);

    writer.write_cstring(obj.message);

}

static size_t GroupListMember_size(const GroupListMember& obj) {
    return 11 + obj.name.size();
}

GroupListMember GroupListMember_read(Reader& reader) {
    GroupListMember obj;

    obj.name = reader.read_cstring();

    obj.guid = reader.read_u64();

    obj.is_online = reader.read_bool8();

    obj.flags = reader.read_u8();

    return obj;
}

void GroupListMember_write(Writer& writer, const GroupListMember& obj) {
    writer.write_cstring(obj.name);

    writer.write_u64(obj.guid);

    writer.write_bool8(obj.is_online);

    writer.write_u8(obj.flags);

}

static size_t GuildMember_size(const GuildMember& obj) {
    size_t _size = 22 + obj.name.size() + obj.public_note.size() + obj.officer_note.size();

    if (obj.status == GuildMemberStatus::OFFLINE) {
        _size += 4;
    }

    return _size;
}

GuildMember GuildMember_read(Reader& reader) {
    GuildMember obj;

    obj.guid = reader.read_u64();

    obj.status = static_cast<GuildMemberStatus>(reader.read_u8());

    obj.name = reader.read_cstring();

    obj.rank = reader.read_u32();

    obj.level = reader.read_u8();

    obj.class_type = static_cast<Class>(reader.read_u8());

    obj.area = static_cast<Area>(reader.read_u32());

    if (obj.status == GuildMemberStatus::OFFLINE) {
        obj.time_offline = reader.read_float();

    }
    obj.public_note = reader.read_cstring();

    obj.officer_note = reader.read_cstring();

    return obj;
}

void GuildMember_write(Writer& writer, const GuildMember& obj) {
    writer.write_u64(obj.guid);

    writer.write_u8(static_cast<uint8_t>(obj.status));

    writer.write_cstring(obj.name);

    writer.write_u32(obj.rank);

    writer.write_u8(obj.level);

    writer.write_u8(static_cast<uint8_t>(obj.class_type));

    writer.write_u32(static_cast<uint32_t>(obj.area));

    if (obj.status == GuildMemberStatus::OFFLINE) {
        writer.write_float(obj.time_offline);

    }
    writer.write_cstring(obj.public_note);

    writer.write_cstring(obj.officer_note);

}

InitialSpell InitialSpell_read(Reader& reader) {
    InitialSpell obj;

    obj.spell_id = reader.read_u16();

    obj.unknown1 = reader.read_u16();

    return obj;
}

void InitialSpell_write(Writer& writer, const InitialSpell& obj) {
    writer.write_u16(obj.spell_id);

    writer.write_u16(obj.unknown1);

}

ItemDamageType ItemDamageType_read(Reader& reader) {
    ItemDamageType obj;

    obj.damage_minimum = reader.read_float();

    obj.damage_maximum = reader.read_float();

    obj.school = static_cast<SpellSchool>(reader.read_u32());

    return obj;
}

void ItemDamageType_write(Writer& writer, const ItemDamageType& obj) {
    writer.write_float(obj.damage_minimum);

    writer.write_float(obj.damage_maximum);

    writer.write_u32(static_cast<uint32_t>(obj.school));

}

ItemSpells ItemSpells_read(Reader& reader) {
    ItemSpells obj;

    obj.spell = reader.read_u32();

    obj.spell_trigger = static_cast<SpellTriggerType>(reader.read_u32());

    obj.spell_charges = reader.read_i32();

    obj.spell_cooldown = reader.read_i32();

    obj.spell_category = reader.read_u32();

    obj.spell_category_cooldown = reader.read_i32();

    return obj;
}

void ItemSpells_write(Writer& writer, const ItemSpells& obj) {
    writer.write_u32(obj.spell);

    writer.write_u32(static_cast<uint32_t>(obj.spell_trigger));

    writer.write_i32(obj.spell_charges);

    writer.write_i32(obj.spell_cooldown);

    writer.write_u32(obj.spell_category);

    writer.write_i32(obj.spell_category_cooldown);

}

ItemStat ItemStat_read(Reader& reader) {
    ItemStat obj;

    obj.stat_type = static_cast<ItemStatType>(reader.read_u32());

    obj.value = reader.read_i32();

    return obj;
}

void ItemStat_write(Writer& writer, const ItemStat& obj) {
    writer.write_u32(static_cast<uint32_t>(obj.stat_type));

    writer.write_i32(obj.value);

}

ListInventoryItem ListInventoryItem_read(Reader& reader) {
    ListInventoryItem obj;

    obj.item_stack_count = reader.read_u32();

    obj.item = reader.read_u32();

    obj.item_display_id = reader.read_u32();

    obj.max_items = reader.read_u32();

    obj.price = reader.read_u32();

    obj.max_durability = reader.read_u32();

    obj.durability = reader.read_u32();

    return obj;
}

void ListInventoryItem_write(Writer& writer, const ListInventoryItem& obj) {
    writer.write_u32(obj.item_stack_count);

    writer.write_u32(obj.item);

    writer.write_u32(obj.item_display_id);

    writer.write_u32(obj.max_items);

    writer.write_u32(obj.price);

    writer.write_u32(obj.max_durability);

    writer.write_u32(obj.durability);

}

LootItem LootItem_read(Reader& reader) {
    LootItem obj;

    obj.index = reader.read_u8();

    obj.item = reader.read_u32();

    obj.ty = static_cast<LootSlotType>(reader.read_u8());

    return obj;
}

void LootItem_write(Writer& writer, const LootItem& obj) {
    writer.write_u8(obj.index);

    writer.write_u32(obj.item);

    writer.write_u8(static_cast<uint8_t>(obj.ty));

}

static size_t Mail_size(const Mail& obj) {
    size_t _size = 67 + obj.subject.size();

    if (obj.message_type == MailType::NORMAL) {
        _size += 8;
    }
    else if (obj.message_type == MailType::CREATURE|| obj.message_type == MailType::GAMEOBJECT) {
        _size += 4;
    }
    else if (obj.message_type == MailType::AUCTION) {
        _size += 4;
    }

    return _size;
}

Mail Mail_read(Reader& reader) {
    Mail obj;

    obj.message_id = reader.read_u32();

    obj.message_type = static_cast<MailType>(reader.read_u8());

    if (obj.message_type == MailType::NORMAL) {
        obj.sender = reader.read_u64();

    }
    else if (obj.message_type == MailType::CREATURE|| obj.message_type == MailType::GAMEOBJECT) {
        obj.sender_id = reader.read_u32();

    }
    else if (obj.message_type == MailType::AUCTION) {
        obj.auction_id = reader.read_u32();

    }
    obj.subject = reader.read_cstring();

    obj.item_text_id = reader.read_u32();

    obj.unknown1 = reader.read_u32();

    obj.stationery = reader.read_u32();

    obj.item = reader.read_u32();

    obj.item_enchant_id = reader.read_u32();

    obj.item_random_property_id = reader.read_u32();

    obj.item_suffix_factor = reader.read_u32();

    obj.item_stack_size = reader.read_u8();

    obj.item_spell_charges = reader.read_u32();

    obj.max_durability = reader.read_u32();

    obj.durability = reader.read_u32();

    obj.money = reader.read_u32();

    obj.cash_on_delivery_amount = reader.read_u32();

    obj.checked_timestamp = reader.read_u32();

    obj.expiration_time = reader.read_float();

    obj.mail_template_id = reader.read_u32();

    return obj;
}

void Mail_write(Writer& writer, const Mail& obj) {
    writer.write_u32(obj.message_id);

    writer.write_u8(static_cast<uint8_t>(obj.message_type));

    if (obj.message_type == MailType::NORMAL) {
        writer.write_u64(obj.sender);

    }
    else if (obj.message_type == MailType::CREATURE|| obj.message_type == MailType::GAMEOBJECT) {
        writer.write_u32(obj.sender_id);

    }
    else if (obj.message_type == MailType::AUCTION) {
        writer.write_u32(obj.auction_id);

    }
    writer.write_cstring(obj.subject);

    writer.write_u32(obj.item_text_id);

    writer.write_u32(obj.unknown1);

    writer.write_u32(obj.stationery);

    writer.write_u32(obj.item);

    writer.write_u32(obj.item_enchant_id);

    writer.write_u32(obj.item_random_property_id);

    writer.write_u32(obj.item_suffix_factor);

    writer.write_u8(obj.item_stack_size);

    writer.write_u32(obj.item_spell_charges);

    writer.write_u32(obj.max_durability);

    writer.write_u32(obj.durability);

    writer.write_u32(obj.money);

    writer.write_u32(obj.cash_on_delivery_amount);

    writer.write_u32(obj.checked_timestamp);

    writer.write_float(obj.expiration_time);

    writer.write_u32(obj.mail_template_id);

}

static size_t MovementBlock_size(const MovementBlock& obj) {
    size_t _size = 1;

    if ((obj.update_flag & UPDATE_FLAG_LIVING) != 0) {
        _size += 52;

        if ((obj.flags & MOVEMENT_FLAGS_ON_TRANSPORT) != 0) {
            _size += 16 + ::wow_world_messages::util::wwm_packed_guid_size(obj.transport_guid);
        }

        if ((obj.flags & MOVEMENT_FLAGS_SWIMMING) != 0) {
            _size += 4;
        }

        if ((obj.flags & MOVEMENT_FLAGS_JUMPING) != 0) {
            _size += 16;
        }

        if ((obj.flags & MOVEMENT_FLAGS_SPLINE_ELEVATION) != 0) {
            _size += 4;
        }

        if ((obj.flags & MOVEMENT_FLAGS_SPLINE_ENABLED) != 0) {
            _size += 32 + 12 * obj.nodes.size();

            if ((obj.spline_flags & SPLINE_FLAG_FINAL_ANGLE) != 0) {
                _size += 4;
            }
            else if ((obj.spline_flags & SPLINE_FLAG_FINAL_TARGET) != 0) {
                _size += 8;
            }
            else if ((obj.spline_flags & SPLINE_FLAG_FINAL_POINT) != 0) {
                _size += 12;
            }

        }

    }
    else if ((obj.update_flag & UPDATE_FLAG_HAS_POSITION) != 0) {
        _size += 16;
    }

    if ((obj.update_flag & UPDATE_FLAG_HIGH_GUID) != 0) {
        _size += 4;
    }

    if ((obj.update_flag & UPDATE_FLAG_ALL) != 0) {
        _size += 4;
    }

    if ((obj.update_flag & UPDATE_FLAG_MELEE_ATTACKING) != 0) {
        _size += 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
    }

    if ((obj.update_flag & UPDATE_FLAG_TRANSPORT) != 0) {
        _size += 4;
    }

    return _size;
}

MovementBlock MovementBlock_read(Reader& reader) {
    MovementBlock obj;

    obj.update_flag = static_cast<UpdateFlag>(reader.read_u8());

    if ((obj.update_flag & UPDATE_FLAG_LIVING) != 0) {
        obj.flags = static_cast<MovementFlags>(reader.read_u32());

        obj.timestamp = reader.read_u32();

        obj.living_position = ::wow_world_messages::all::Vector3d_read(reader);

        obj.living_orientation = reader.read_float();

        if ((obj.flags & MOVEMENT_FLAGS_ON_TRANSPORT) != 0) {
            obj.transport_guid = reader.read_packed_guid();

            obj.transport_position = ::wow_world_messages::all::Vector3d_read(reader);

            obj.transport_orientation = reader.read_float();

        }
        if ((obj.flags & MOVEMENT_FLAGS_SWIMMING) != 0) {
            obj.pitch = reader.read_float();

        }
        obj.fall_time = reader.read_float();

        if ((obj.flags & MOVEMENT_FLAGS_JUMPING) != 0) {
            obj.z_speed = reader.read_float();

            obj.cos_angle = reader.read_float();

            obj.sin_angle = reader.read_float();

            obj.xy_speed = reader.read_float();

        }
        if ((obj.flags & MOVEMENT_FLAGS_SPLINE_ELEVATION) != 0) {
            obj.spline_elevation = reader.read_float();

        }
        obj.walking_speed = reader.read_float();

        obj.running_speed = reader.read_float();

        obj.backwards_running_speed = reader.read_float();

        obj.swimming_speed = reader.read_float();

        obj.backwards_swimming_speed = reader.read_float();

        obj.turn_rate = reader.read_float();

        if ((obj.flags & MOVEMENT_FLAGS_SPLINE_ENABLED) != 0) {
            obj.spline_flags = static_cast<SplineFlag>(reader.read_u32());

            if ((obj.spline_flags & SPLINE_FLAG_FINAL_ANGLE) != 0) {
                obj.angle = reader.read_float();

            }
            else if ((obj.spline_flags & SPLINE_FLAG_FINAL_TARGET) != 0) {
                obj.target = reader.read_u64();

            }
            else if ((obj.spline_flags & SPLINE_FLAG_FINAL_POINT) != 0) {
                obj.spline_final_point = ::wow_world_messages::all::Vector3d_read(reader);

            }
            obj.time_passed = reader.read_u32();

            obj.duration = reader.read_u32();

            obj.id = reader.read_u32();

            obj.amount_of_nodes = reader.read_u32();

            for (auto i = 0; i < obj.amount_of_nodes; ++i) {
                obj.nodes.push_back(::wow_world_messages::all::Vector3d_read(reader));
            }

            obj.final_node = ::wow_world_messages::all::Vector3d_read(reader);

        }
    }
    else if ((obj.update_flag & UPDATE_FLAG_HAS_POSITION) != 0) {
        obj.position = ::wow_world_messages::all::Vector3d_read(reader);

        obj.orientation = reader.read_float();

    }
    if ((obj.update_flag & UPDATE_FLAG_HIGH_GUID) != 0) {
        obj.unknown0 = reader.read_u32();

    }
    if ((obj.update_flag & UPDATE_FLAG_ALL) != 0) {
        obj.unknown1 = reader.read_u32();

    }
    if ((obj.update_flag & UPDATE_FLAG_MELEE_ATTACKING) != 0) {
        obj.guid = reader.read_packed_guid();

    }
    if ((obj.update_flag & UPDATE_FLAG_TRANSPORT) != 0) {
        obj.transport_progress_in_ms = reader.read_u32();

    }
    return obj;
}

void MovementBlock_write(Writer& writer, const MovementBlock& obj) {
    writer.write_u8(static_cast<uint8_t>(obj.update_flag));

    if ((obj.update_flag & UPDATE_FLAG_LIVING) != 0) {
        writer.write_u32(static_cast<uint32_t>(obj.flags));

        writer.write_u32(obj.timestamp);

        Vector3d_write(writer, obj.living_position);

        writer.write_float(obj.living_orientation);

        if ((obj.flags & MOVEMENT_FLAGS_ON_TRANSPORT) != 0) {
            writer.write_packed_guid(obj.transport_guid);

            Vector3d_write(writer, obj.transport_position);

            writer.write_float(obj.transport_orientation);

        }
        if ((obj.flags & MOVEMENT_FLAGS_SWIMMING) != 0) {
            writer.write_float(obj.pitch);

        }
        writer.write_float(obj.fall_time);

        if ((obj.flags & MOVEMENT_FLAGS_JUMPING) != 0) {
            writer.write_float(obj.z_speed);

            writer.write_float(obj.cos_angle);

            writer.write_float(obj.sin_angle);

            writer.write_float(obj.xy_speed);

        }
        if ((obj.flags & MOVEMENT_FLAGS_SPLINE_ELEVATION) != 0) {
            writer.write_float(obj.spline_elevation);

        }
        writer.write_float(obj.walking_speed);

        writer.write_float(obj.running_speed);

        writer.write_float(obj.backwards_running_speed);

        writer.write_float(obj.swimming_speed);

        writer.write_float(obj.backwards_swimming_speed);

        writer.write_float(obj.turn_rate);

        if ((obj.flags & MOVEMENT_FLAGS_SPLINE_ENABLED) != 0) {
            writer.write_u32(static_cast<uint32_t>(obj.spline_flags));

            if ((obj.spline_flags & SPLINE_FLAG_FINAL_ANGLE) != 0) {
                writer.write_float(obj.angle);

            }
            else if ((obj.spline_flags & SPLINE_FLAG_FINAL_TARGET) != 0) {
                writer.write_u64(obj.target);

            }
            else if ((obj.spline_flags & SPLINE_FLAG_FINAL_POINT) != 0) {
                Vector3d_write(writer, obj.spline_final_point);

            }
            writer.write_u32(obj.time_passed);

            writer.write_u32(obj.duration);

            writer.write_u32(obj.id);

            writer.write_u32(obj.amount_of_nodes);

            for (const auto& v : obj.nodes) {
                Vector3d_write(writer, v);
            }

            Vector3d_write(writer, obj.final_node);

        }
    }
    else if ((obj.update_flag & UPDATE_FLAG_HAS_POSITION) != 0) {
        Vector3d_write(writer, obj.position);

        writer.write_float(obj.orientation);

    }
    if ((obj.update_flag & UPDATE_FLAG_HIGH_GUID) != 0) {
        writer.write_u32(obj.unknown0);

    }
    if ((obj.update_flag & UPDATE_FLAG_ALL) != 0) {
        writer.write_u32(obj.unknown1);

    }
    if ((obj.update_flag & UPDATE_FLAG_MELEE_ATTACKING) != 0) {
        writer.write_packed_guid(obj.guid);

    }
    if ((obj.update_flag & UPDATE_FLAG_TRANSPORT) != 0) {
        writer.write_u32(obj.transport_progress_in_ms);

    }
}

static size_t TransportInfo_size(const TransportInfo& obj) {
    return 20 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

TransportInfo TransportInfo_read(Reader& reader) {
    TransportInfo obj;

    obj.guid = reader.read_packed_guid();

    obj.position = ::wow_world_messages::all::Vector3d_read(reader);

    obj.orientation = reader.read_float();

    obj.timestamp = reader.read_u32();

    return obj;
}

void TransportInfo_write(Writer& writer, const TransportInfo& obj) {
    writer.write_packed_guid(obj.guid);

    Vector3d_write(writer, obj.position);

    writer.write_float(obj.orientation);

    writer.write_u32(obj.timestamp);

}

static size_t MovementInfo_size(const MovementInfo& obj) {
    size_t _size = 28;

    if ((obj.flags & MOVEMENT_FLAGS_ON_TRANSPORT) != 0) {
        _size += 0 + TransportInfo_size(obj.transport);
    }

    if ((obj.flags & MOVEMENT_FLAGS_SWIMMING) != 0) {
        _size += 4;
    }

    if ((obj.flags & MOVEMENT_FLAGS_JUMPING) != 0) {
        _size += 16;
    }

    if ((obj.flags & MOVEMENT_FLAGS_SPLINE_ELEVATION) != 0) {
        _size += 4;
    }

    return _size;
}

MovementInfo MovementInfo_read(Reader& reader) {
    MovementInfo obj;

    obj.flags = static_cast<MovementFlags>(reader.read_u32());

    obj.timestamp = reader.read_u32();

    obj.position = ::wow_world_messages::all::Vector3d_read(reader);

    obj.orientation = reader.read_float();

    if ((obj.flags & MOVEMENT_FLAGS_ON_TRANSPORT) != 0) {
        obj.transport = ::wow_world_messages::vanilla::TransportInfo_read(reader);

    }
    if ((obj.flags & MOVEMENT_FLAGS_SWIMMING) != 0) {
        obj.pitch = reader.read_float();

    }
    obj.fall_time = reader.read_float();

    if ((obj.flags & MOVEMENT_FLAGS_JUMPING) != 0) {
        obj.z_speed = reader.read_float();

        obj.cos_angle = reader.read_float();

        obj.sin_angle = reader.read_float();

        obj.xy_speed = reader.read_float();

    }
    if ((obj.flags & MOVEMENT_FLAGS_SPLINE_ELEVATION) != 0) {
        obj.spline_elevation = reader.read_float();

    }
    return obj;
}

void MovementInfo_write(Writer& writer, const MovementInfo& obj) {
    writer.write_u32(static_cast<uint32_t>(obj.flags));

    writer.write_u32(obj.timestamp);

    Vector3d_write(writer, obj.position);

    writer.write_float(obj.orientation);

    if ((obj.flags & MOVEMENT_FLAGS_ON_TRANSPORT) != 0) {
        TransportInfo_write(writer, obj.transport);

    }
    if ((obj.flags & MOVEMENT_FLAGS_SWIMMING) != 0) {
        writer.write_float(obj.pitch);

    }
    writer.write_float(obj.fall_time);

    if ((obj.flags & MOVEMENT_FLAGS_JUMPING) != 0) {
        writer.write_float(obj.z_speed);

        writer.write_float(obj.cos_angle);

        writer.write_float(obj.sin_angle);

        writer.write_float(obj.xy_speed);

    }
    if ((obj.flags & MOVEMENT_FLAGS_SPLINE_ELEVATION) != 0) {
        writer.write_float(obj.spline_elevation);

    }
}

NpcTextUpdateEmote NpcTextUpdateEmote_read(Reader& reader) {
    NpcTextUpdateEmote obj;

    obj.delay = reader.read_u32();

    obj.emote = reader.read_u32();

    return obj;
}

void NpcTextUpdateEmote_write(Writer& writer, const NpcTextUpdateEmote& obj) {
    writer.write_u32(obj.delay);

    writer.write_u32(obj.emote);

}

static size_t NpcTextUpdate_size(const NpcTextUpdate& obj) {
    size_t _size = 32;

    for(const auto& v : obj.texts) {
        _size += v.size() + 1;
    }

    return _size;
}

NpcTextUpdate NpcTextUpdate_read(Reader& reader) {
    NpcTextUpdate obj;

    obj.probability = reader.read_float();

    for (auto i = 0; i < 2; ++i) {
        obj.texts[i] = reader.read_cstring();
    }

    obj.language = static_cast<Language>(reader.read_u32());

    for (auto i = 0; i < 3; ++i) {
        obj.emotes[i] = ::wow_world_messages::vanilla::NpcTextUpdateEmote_read(reader);
    }

    return obj;
}

void NpcTextUpdate_write(Writer& writer, const NpcTextUpdate& obj) {
    writer.write_float(obj.probability);

    for (const auto& v : obj.texts) {
        writer.write_cstring(v);
    }

    writer.write_u32(static_cast<uint32_t>(obj.language));

    for (const auto& v : obj.emotes) {
        NpcTextUpdateEmote_write(writer, v);
    }

}

static size_t Object_size(const Object& obj) {
    size_t _size = 1;

    if (obj.update_type == UpdateType::VALUES) {
        _size += 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid1) + vanilla::update_mask_size(obj.mask1);
    }
    else if (obj.update_type == UpdateType::MOVEMENT) {
        _size += 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid2) + MovementBlock_size(obj.movement1);
    }
    else if (obj.update_type == UpdateType::CREATE_OBJECT|| obj.update_type == UpdateType::CREATE_OBJECT2) {
        _size += 1 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid3) + MovementBlock_size(obj.movement2) + vanilla::update_mask_size(obj.mask2);
    }
    else if (obj.update_type == UpdateType::OUT_OF_RANGE_OBJECTS|| obj.update_type == UpdateType::NEAR_OBJECTS) {
        _size += 4;

        for(const auto& v : obj.guids) {
            _size += wow_world_messages::util::wwm_packed_guid_size(v);
        }

    }

    return _size;
}

Object Object_read(Reader& reader) {
    Object obj;

    obj.update_type = static_cast<UpdateType>(reader.read_u8());

    if (obj.update_type == UpdateType::VALUES) {
        obj.guid1 = reader.read_packed_guid();

        obj.mask1 = vanilla::update_mask_read(reader);

    }
    else if (obj.update_type == UpdateType::MOVEMENT) {
        obj.guid2 = reader.read_packed_guid();

        obj.movement1 = ::wow_world_messages::vanilla::MovementBlock_read(reader);

    }
    else if (obj.update_type == UpdateType::CREATE_OBJECT|| obj.update_type == UpdateType::CREATE_OBJECT2) {
        obj.guid3 = reader.read_packed_guid();

        obj.object_type = static_cast<ObjectType>(reader.read_u8());

        obj.movement2 = ::wow_world_messages::vanilla::MovementBlock_read(reader);

        obj.mask2 = vanilla::update_mask_read(reader);

    }
    else if (obj.update_type == UpdateType::OUT_OF_RANGE_OBJECTS|| obj.update_type == UpdateType::NEAR_OBJECTS) {
        obj.count = reader.read_u32();

        for (auto i = 0; i < obj.count; ++i) {
            obj.guids.push_back(reader.read_packed_guid());
        }

    }
    return obj;
}

void Object_write(Writer& writer, const Object& obj) {
    writer.write_u8(static_cast<uint8_t>(obj.update_type));

    if (obj.update_type == UpdateType::VALUES) {
        writer.write_packed_guid(obj.guid1);

        vanilla::update_mask_write(writer, obj.mask1);

    }
    else if (obj.update_type == UpdateType::MOVEMENT) {
        writer.write_packed_guid(obj.guid2);

        MovementBlock_write(writer, obj.movement1);

    }
    else if (obj.update_type == UpdateType::CREATE_OBJECT|| obj.update_type == UpdateType::CREATE_OBJECT2) {
        writer.write_packed_guid(obj.guid3);

        writer.write_u8(static_cast<uint8_t>(obj.object_type));

        MovementBlock_write(writer, obj.movement2);

        vanilla::update_mask_write(writer, obj.mask2);

    }
    else if (obj.update_type == UpdateType::OUT_OF_RANGE_OBJECTS|| obj.update_type == UpdateType::NEAR_OBJECTS) {
        writer.write_u32(obj.count);

        for (const auto& v : obj.guids) {
            writer.write_packed_guid(v);
        }

    }
}

PetSpellCooldown PetSpellCooldown_read(Reader& reader) {
    PetSpellCooldown obj;

    obj.spell = reader.read_u16();

    obj.spell_category = reader.read_u16();

    obj.cooldown = reader.read_u32();

    obj.category_cooldown = reader.read_u32();

    return obj;
}

void PetSpellCooldown_write(Writer& writer, const PetSpellCooldown& obj) {
    writer.write_u16(obj.spell);

    writer.write_u16(obj.spell_category);

    writer.write_u32(obj.cooldown);

    writer.write_u32(obj.category_cooldown);

}

PetitionShowlist PetitionShowlist_read(Reader& reader) {
    PetitionShowlist obj;

    obj.index = reader.read_u32();

    obj.charter_entry = reader.read_u32();

    obj.charter_display_id = reader.read_u32();

    obj.guild_charter_cost = reader.read_u32();

    obj.unknown1 = reader.read_u32();

    return obj;
}

void PetitionShowlist_write(Writer& writer, const PetitionShowlist& obj) {
    writer.write_u32(obj.index);

    writer.write_u32(obj.charter_entry);

    writer.write_u32(obj.charter_display_id);

    writer.write_u32(obj.guild_charter_cost);

    writer.write_u32(obj.unknown1);

}

PetitionSignature PetitionSignature_read(Reader& reader) {
    PetitionSignature obj;

    obj.signer = reader.read_u64();

    obj.unknown1 = reader.read_u32();

    return obj;
}

void PetitionSignature_write(Writer& writer, const PetitionSignature& obj) {
    writer.write_u64(obj.signer);

    writer.write_u32(obj.unknown1);

}

QuestDetailsEmote QuestDetailsEmote_read(Reader& reader) {
    QuestDetailsEmote obj;

    obj.emote = reader.read_u32();

    obj.emote_delay = reader.read_u32();

    return obj;
}

void QuestDetailsEmote_write(Writer& writer, const QuestDetailsEmote& obj) {
    writer.write_u32(obj.emote);

    writer.write_u32(obj.emote_delay);

}

static size_t QuestItem_size(const QuestItem& obj) {
    return 13 + obj.title.size();
}

QuestItem QuestItem_read(Reader& reader) {
    QuestItem obj;

    obj.quest_id = reader.read_u32();

    obj.quest_icon = reader.read_u32();

    obj.level = reader.read_u32();

    obj.title = reader.read_cstring();

    return obj;
}

void QuestItem_write(Writer& writer, const QuestItem& obj) {
    writer.write_u32(obj.quest_id);

    writer.write_u32(obj.quest_icon);

    writer.write_u32(obj.level);

    writer.write_cstring(obj.title);

}

QuestItemRequirement QuestItemRequirement_read(Reader& reader) {
    QuestItemRequirement obj;

    obj.item = reader.read_u32();

    obj.item_count = reader.read_u32();

    obj.item_display_id = reader.read_u32();

    return obj;
}

void QuestItemRequirement_write(Writer& writer, const QuestItemRequirement& obj) {
    writer.write_u32(obj.item);

    writer.write_u32(obj.item_count);

    writer.write_u32(obj.item_display_id);

}

QuestItemReward QuestItemReward_read(Reader& reader) {
    QuestItemReward obj;

    obj.item = reader.read_u32();

    obj.item_count = reader.read_u32();

    return obj;
}

void QuestItemReward_write(Writer& writer, const QuestItemReward& obj) {
    writer.write_u32(obj.item);

    writer.write_u32(obj.item_count);

}

QuestObjective QuestObjective_read(Reader& reader) {
    QuestObjective obj;

    obj.creature_id = reader.read_u32();

    obj.kill_count = reader.read_u32();

    obj.required_item_id = reader.read_u32();

    obj.required_item_count = reader.read_u32();

    return obj;
}

void QuestObjective_write(Writer& writer, const QuestObjective& obj) {
    writer.write_u32(obj.creature_id);

    writer.write_u32(obj.kill_count);

    writer.write_u32(obj.required_item_id);

    writer.write_u32(obj.required_item_count);

}

RaidInfo RaidInfo_read(Reader& reader) {
    RaidInfo obj;

    obj.map = static_cast<Map>(reader.read_u32());

    obj.reset_time = reader.read_u32();

    obj.instance_id = reader.read_u32();

    return obj;
}

void RaidInfo_write(Writer& writer, const RaidInfo& obj) {
    writer.write_u32(static_cast<uint32_t>(obj.map));

    writer.write_u32(obj.reset_time);

    writer.write_u32(obj.instance_id);

}

RaidTargetUpdate RaidTargetUpdate_read(Reader& reader) {
    RaidTargetUpdate obj;

    obj.index = static_cast<RaidTargetIndex>(reader.read_u8());

    obj.guid = reader.read_u64();

    return obj;
}

void RaidTargetUpdate_write(Writer& writer, const RaidTargetUpdate& obj) {
    writer.write_u8(static_cast<uint8_t>(obj.index));

    writer.write_u64(obj.guid);

}

static size_t SpellCastTargets_size(const SpellCastTargets& obj) {
    size_t _size = 2;

    if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_UNIT) != 0) {
        _size += 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.unit_target);
    }

    if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_GAMEOBJECT) != 0) {
        _size += 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.gameobject);
    }
    else if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_OBJECT_UNK) != 0) {
        _size += 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.object_unk);
    }

    if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_ITEM) != 0) {
        _size += 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.item);
    }
    else if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_TRADE_ITEM) != 0) {
        _size += 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.trade_item);
    }

    if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_SOURCE_LOCATION) != 0) {
        _size += 12;
    }

    if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_DEST_LOCATION) != 0) {
        _size += 12;
    }

    if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_STRING) != 0) {
        _size += 1 + obj.target_string.size();
    }

    if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_CORPSE) != 0) {
        _size += 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.corpse);
    }
    else if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_PVP_CORPSE) != 0) {
        _size += 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.pvp_corpse);
    }

    return _size;
}

SpellCastTargets SpellCastTargets_read(Reader& reader) {
    SpellCastTargets obj;

    obj.target_flags = static_cast<SpellCastTargetFlags>(reader.read_u16());

    if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_UNIT) != 0) {
        obj.unit_target = reader.read_packed_guid();

    }
    if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_GAMEOBJECT) != 0) {
        obj.gameobject = reader.read_packed_guid();

    }
    else if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_OBJECT_UNK) != 0) {
        obj.object_unk = reader.read_packed_guid();

    }
    if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_ITEM) != 0) {
        obj.item = reader.read_packed_guid();

    }
    else if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_TRADE_ITEM) != 0) {
        obj.trade_item = reader.read_packed_guid();

    }
    if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_SOURCE_LOCATION) != 0) {
        obj.source = ::wow_world_messages::all::Vector3d_read(reader);

    }
    if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_DEST_LOCATION) != 0) {
        obj.destination = ::wow_world_messages::all::Vector3d_read(reader);

    }
    if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_STRING) != 0) {
        obj.target_string = reader.read_cstring();

    }
    if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_CORPSE) != 0) {
        obj.corpse = reader.read_packed_guid();

    }
    else if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_PVP_CORPSE) != 0) {
        obj.pvp_corpse = reader.read_packed_guid();

    }
    return obj;
}

void SpellCastTargets_write(Writer& writer, const SpellCastTargets& obj) {
    writer.write_u16(static_cast<uint16_t>(obj.target_flags));

    if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_UNIT) != 0) {
        writer.write_packed_guid(obj.unit_target);

    }
    if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_GAMEOBJECT) != 0) {
        writer.write_packed_guid(obj.gameobject);

    }
    else if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_OBJECT_UNK) != 0) {
        writer.write_packed_guid(obj.object_unk);

    }
    if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_ITEM) != 0) {
        writer.write_packed_guid(obj.item);

    }
    else if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_TRADE_ITEM) != 0) {
        writer.write_packed_guid(obj.trade_item);

    }
    if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_SOURCE_LOCATION) != 0) {
        Vector3d_write(writer, obj.source);

    }
    if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_DEST_LOCATION) != 0) {
        Vector3d_write(writer, obj.destination);

    }
    if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_STRING) != 0) {
        writer.write_cstring(obj.target_string);

    }
    if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_CORPSE) != 0) {
        writer.write_packed_guid(obj.corpse);

    }
    else if ((obj.target_flags & SPELL_CAST_TARGET_FLAGS_PVP_CORPSE) != 0) {
        writer.write_packed_guid(obj.pvp_corpse);

    }
}

SpellCooldownStatus SpellCooldownStatus_read(Reader& reader) {
    SpellCooldownStatus obj;

    obj.id = reader.read_u32();

    obj.cooldown_time = reader.read_u32();

    return obj;
}

void SpellCooldownStatus_write(Writer& writer, const SpellCooldownStatus& obj) {
    writer.write_u32(obj.id);

    writer.write_u32(obj.cooldown_time);

}

static size_t SpellLog_size(const SpellLog& obj) {
    size_t _size = 8;

    if (obj.effect == SpellEffect::POWER_DRAIN) {
        _size += 20;
    }
    else if (obj.effect == SpellEffect::HEAL|| obj.effect == SpellEffect::HEAL_MAX_HEALTH) {
        _size += 16;
    }
    else if (obj.effect == SpellEffect::ENERGIZE) {
        _size += 16;
    }
    else if (obj.effect == SpellEffect::ADD_EXTRA_ATTACKS) {
        _size += 12;
    }
    else if (obj.effect == SpellEffect::CREATE_ITEM) {
        _size += 4;
    }
    else if (obj.effect == SpellEffect::INTERRUPT_CAST) {
        _size += 12;
    }
    else if (obj.effect == SpellEffect::DURABILITY_DAMAGE) {
        _size += 16;
    }
    else if (obj.effect == SpellEffect::FEED_PET) {
        _size += 4;
    }
    else if (obj.effect == SpellEffect::INSTAKILL|| obj.effect == SpellEffect::RESURRECT|| obj.effect == SpellEffect::DISPEL|| obj.effect == SpellEffect::THREAT|| obj.effect == SpellEffect::DISTRACT|| obj.effect == SpellEffect::SANCTUARY|| obj.effect == SpellEffect::THREAT_ALL|| obj.effect == SpellEffect::DISPEL_MECHANIC|| obj.effect == SpellEffect::RESURRECT_NEW|| obj.effect == SpellEffect::ATTACK_ME|| obj.effect == SpellEffect::SKIN_PLAYER_CORPSE|| obj.effect == SpellEffect::MODIFY_THREAT_PERCENT|| obj.effect == SpellEffect::UNKNOWN126|| obj.effect == SpellEffect::OPEN_LOCK|| obj.effect == SpellEffect::OPEN_LOCK_ITEM|| obj.effect == SpellEffect::DISMISS_PET|| obj.effect == SpellEffect::TRANS_DOOR|| obj.effect == SpellEffect::SUMMON|| obj.effect == SpellEffect::SUMMON_PET|| obj.effect == SpellEffect::SUMMON_WILD|| obj.effect == SpellEffect::SUMMON_GUARDIAN|| obj.effect == SpellEffect::SUMMON_TOTEM_SLOT1|| obj.effect == SpellEffect::SUMMON_TOTEM_SLOT2|| obj.effect == SpellEffect::SUMMON_TOTEM_SLOT3|| obj.effect == SpellEffect::SUMMON_TOTEM_SLOT4|| obj.effect == SpellEffect::SUMMON_POSSESSED|| obj.effect == SpellEffect::SUMMON_TOTEM|| obj.effect == SpellEffect::SUMMON_CRITTER|| obj.effect == SpellEffect::SUMMON_OBJECT_WILD|| obj.effect == SpellEffect::SUMMON_OBJECT_SLOT1|| obj.effect == SpellEffect::SUMMON_OBJECT_SLOT2|| obj.effect == SpellEffect::SUMMON_OBJECT_SLOT3|| obj.effect == SpellEffect::SUMMON_OBJECT_SLOT4|| obj.effect == SpellEffect::SUMMON_DEMON) {
        _size += 8;
    }

    return _size;
}

SpellLog SpellLog_read(Reader& reader) {
    SpellLog obj;

    obj.effect = static_cast<SpellEffect>(reader.read_u32());

    (void)reader.read_u32();

    if (obj.effect == SpellEffect::POWER_DRAIN) {
        obj.target1 = reader.read_u64();

        obj.amount = reader.read_u32();

        obj.power = static_cast<Power>(reader.read_u32());

        obj.multiplier = reader.read_float();

    }
    else if (obj.effect == SpellEffect::HEAL|| obj.effect == SpellEffect::HEAL_MAX_HEALTH) {
        obj.target2 = reader.read_u64();

        obj.heal_amount = reader.read_u32();

        obj.heal_critical = reader.read_u32();

    }
    else if (obj.effect == SpellEffect::ENERGIZE) {
        obj.target3 = reader.read_u64();

        obj.energize_amount = reader.read_u32();

        obj.energize_power = reader.read_u32();

    }
    else if (obj.effect == SpellEffect::ADD_EXTRA_ATTACKS) {
        obj.target4 = reader.read_u64();

        obj.extra_attacks = reader.read_u32();

    }
    else if (obj.effect == SpellEffect::CREATE_ITEM) {
        obj.item = reader.read_u32();

    }
    else if (obj.effect == SpellEffect::INTERRUPT_CAST) {
        obj.target5 = reader.read_u64();

        obj.interrupted_spell = reader.read_u32();

    }
    else if (obj.effect == SpellEffect::DURABILITY_DAMAGE) {
        obj.target6 = reader.read_u64();

        obj.item_to_damage = reader.read_u32();

        obj.unknown5 = reader.read_u32();

    }
    else if (obj.effect == SpellEffect::FEED_PET) {
        obj.feed_pet_item = reader.read_u32();

    }
    else if (obj.effect == SpellEffect::INSTAKILL|| obj.effect == SpellEffect::RESURRECT|| obj.effect == SpellEffect::DISPEL|| obj.effect == SpellEffect::THREAT|| obj.effect == SpellEffect::DISTRACT|| obj.effect == SpellEffect::SANCTUARY|| obj.effect == SpellEffect::THREAT_ALL|| obj.effect == SpellEffect::DISPEL_MECHANIC|| obj.effect == SpellEffect::RESURRECT_NEW|| obj.effect == SpellEffect::ATTACK_ME|| obj.effect == SpellEffect::SKIN_PLAYER_CORPSE|| obj.effect == SpellEffect::MODIFY_THREAT_PERCENT|| obj.effect == SpellEffect::UNKNOWN126|| obj.effect == SpellEffect::OPEN_LOCK|| obj.effect == SpellEffect::OPEN_LOCK_ITEM|| obj.effect == SpellEffect::DISMISS_PET|| obj.effect == SpellEffect::TRANS_DOOR|| obj.effect == SpellEffect::SUMMON|| obj.effect == SpellEffect::SUMMON_PET|| obj.effect == SpellEffect::SUMMON_WILD|| obj.effect == SpellEffect::SUMMON_GUARDIAN|| obj.effect == SpellEffect::SUMMON_TOTEM_SLOT1|| obj.effect == SpellEffect::SUMMON_TOTEM_SLOT2|| obj.effect == SpellEffect::SUMMON_TOTEM_SLOT3|| obj.effect == SpellEffect::SUMMON_TOTEM_SLOT4|| obj.effect == SpellEffect::SUMMON_POSSESSED|| obj.effect == SpellEffect::SUMMON_TOTEM|| obj.effect == SpellEffect::SUMMON_CRITTER|| obj.effect == SpellEffect::SUMMON_OBJECT_WILD|| obj.effect == SpellEffect::SUMMON_OBJECT_SLOT1|| obj.effect == SpellEffect::SUMMON_OBJECT_SLOT2|| obj.effect == SpellEffect::SUMMON_OBJECT_SLOT3|| obj.effect == SpellEffect::SUMMON_OBJECT_SLOT4|| obj.effect == SpellEffect::SUMMON_DEMON) {
        obj.target7 = reader.read_u64();

    }
    return obj;
}

void SpellLog_write(Writer& writer, const SpellLog& obj) {
    writer.write_u32(static_cast<uint32_t>(obj.effect));

    writer.write_u32(1);

    if (obj.effect == SpellEffect::POWER_DRAIN) {
        writer.write_u64(obj.target1);

        writer.write_u32(obj.amount);

        writer.write_u32(static_cast<uint32_t>(obj.power));

        writer.write_float(obj.multiplier);

    }
    else if (obj.effect == SpellEffect::HEAL|| obj.effect == SpellEffect::HEAL_MAX_HEALTH) {
        writer.write_u64(obj.target2);

        writer.write_u32(obj.heal_amount);

        writer.write_u32(obj.heal_critical);

    }
    else if (obj.effect == SpellEffect::ENERGIZE) {
        writer.write_u64(obj.target3);

        writer.write_u32(obj.energize_amount);

        writer.write_u32(obj.energize_power);

    }
    else if (obj.effect == SpellEffect::ADD_EXTRA_ATTACKS) {
        writer.write_u64(obj.target4);

        writer.write_u32(obj.extra_attacks);

    }
    else if (obj.effect == SpellEffect::CREATE_ITEM) {
        writer.write_u32(obj.item);

    }
    else if (obj.effect == SpellEffect::INTERRUPT_CAST) {
        writer.write_u64(obj.target5);

        writer.write_u32(obj.interrupted_spell);

    }
    else if (obj.effect == SpellEffect::DURABILITY_DAMAGE) {
        writer.write_u64(obj.target6);

        writer.write_u32(obj.item_to_damage);

        writer.write_u32(obj.unknown5);

    }
    else if (obj.effect == SpellEffect::FEED_PET) {
        writer.write_u32(obj.feed_pet_item);

    }
    else if (obj.effect == SpellEffect::INSTAKILL|| obj.effect == SpellEffect::RESURRECT|| obj.effect == SpellEffect::DISPEL|| obj.effect == SpellEffect::THREAT|| obj.effect == SpellEffect::DISTRACT|| obj.effect == SpellEffect::SANCTUARY|| obj.effect == SpellEffect::THREAT_ALL|| obj.effect == SpellEffect::DISPEL_MECHANIC|| obj.effect == SpellEffect::RESURRECT_NEW|| obj.effect == SpellEffect::ATTACK_ME|| obj.effect == SpellEffect::SKIN_PLAYER_CORPSE|| obj.effect == SpellEffect::MODIFY_THREAT_PERCENT|| obj.effect == SpellEffect::UNKNOWN126|| obj.effect == SpellEffect::OPEN_LOCK|| obj.effect == SpellEffect::OPEN_LOCK_ITEM|| obj.effect == SpellEffect::DISMISS_PET|| obj.effect == SpellEffect::TRANS_DOOR|| obj.effect == SpellEffect::SUMMON|| obj.effect == SpellEffect::SUMMON_PET|| obj.effect == SpellEffect::SUMMON_WILD|| obj.effect == SpellEffect::SUMMON_GUARDIAN|| obj.effect == SpellEffect::SUMMON_TOTEM_SLOT1|| obj.effect == SpellEffect::SUMMON_TOTEM_SLOT2|| obj.effect == SpellEffect::SUMMON_TOTEM_SLOT3|| obj.effect == SpellEffect::SUMMON_TOTEM_SLOT4|| obj.effect == SpellEffect::SUMMON_POSSESSED|| obj.effect == SpellEffect::SUMMON_TOTEM|| obj.effect == SpellEffect::SUMMON_CRITTER|| obj.effect == SpellEffect::SUMMON_OBJECT_WILD|| obj.effect == SpellEffect::SUMMON_OBJECT_SLOT1|| obj.effect == SpellEffect::SUMMON_OBJECT_SLOT2|| obj.effect == SpellEffect::SUMMON_OBJECT_SLOT3|| obj.effect == SpellEffect::SUMMON_OBJECT_SLOT4|| obj.effect == SpellEffect::SUMMON_DEMON) {
        writer.write_u64(obj.target7);

    }
}

SpellLogMiss SpellLogMiss_read(Reader& reader) {
    SpellLogMiss obj;

    obj.target = reader.read_u64();

    obj.miss_info = static_cast<SpellMissInfo>(reader.read_u8());

    return obj;
}

void SpellLogMiss_write(Writer& writer, const SpellLogMiss& obj) {
    writer.write_u64(obj.target);

    writer.write_u8(static_cast<uint8_t>(obj.miss_info));

}

SpellMiss SpellMiss_read(Reader& reader) {
    SpellMiss obj;

    obj.target = reader.read_u64();

    obj.miss_info = static_cast<SpellMissInfo>(reader.read_u8());

    return obj;
}

void SpellMiss_write(Writer& writer, const SpellMiss& obj) {
    writer.write_u64(obj.target);

    writer.write_u8(static_cast<uint8_t>(obj.miss_info));

}

static size_t StabledPet_size(const StabledPet& obj) {
    return 18 + obj.name.size();
}

StabledPet StabledPet_read(Reader& reader) {
    StabledPet obj;

    obj.pet_number = reader.read_u32();

    obj.entry = reader.read_u32();

    obj.level = reader.read_u32();

    obj.name = reader.read_cstring();

    obj.loyalty = reader.read_u32();

    obj.slot = reader.read_u8();

    return obj;
}

void StabledPet_write(Writer& writer, const StabledPet& obj) {
    writer.write_u32(obj.pet_number);

    writer.write_u32(obj.entry);

    writer.write_u32(obj.level);

    writer.write_cstring(obj.name);

    writer.write_u32(obj.loyalty);

    writer.write_u8(obj.slot);

}

TradeSlot TradeSlot_read(Reader& reader) {
    TradeSlot obj;

    obj.trade_slot_number = reader.read_u8();

    obj.item = reader.read_u32();

    obj.display_id = reader.read_u32();

    obj.stack_count = reader.read_u32();

    obj.wrapped = reader.read_bool32();

    obj.gift_wrapper = reader.read_u64();

    obj.enchantment = reader.read_u32();

    obj.item_creator = reader.read_u64();

    obj.spell_charges = reader.read_u32();

    obj.item_suffix_factor = reader.read_u32();

    obj.item_random_properties_id = reader.read_u32();

    obj.lock_id = reader.read_u32();

    obj.max_durability = reader.read_u32();

    obj.durability = reader.read_u32();

    return obj;
}

void TradeSlot_write(Writer& writer, const TradeSlot& obj) {
    writer.write_u8(obj.trade_slot_number);

    writer.write_u32(obj.item);

    writer.write_u32(obj.display_id);

    writer.write_u32(obj.stack_count);

    writer.write_bool32(obj.wrapped);

    writer.write_u64(obj.gift_wrapper);

    writer.write_u32(obj.enchantment);

    writer.write_u64(obj.item_creator);

    writer.write_u32(obj.spell_charges);

    writer.write_u32(obj.item_suffix_factor);

    writer.write_u32(obj.item_random_properties_id);

    writer.write_u32(obj.lock_id);

    writer.write_u32(obj.max_durability);

    writer.write_u32(obj.durability);

}

TrainerSpell TrainerSpell_read(Reader& reader) {
    TrainerSpell obj;

    obj.spell = reader.read_u32();

    obj.state = static_cast<TrainerSpellState>(reader.read_u8());

    obj.spell_cost = reader.read_u32();

    obj.talent_point_cost = reader.read_u32();

    obj.first_rank = reader.read_u32();

    obj.required_level = reader.read_u8();

    obj.required_skill = static_cast<Skill>(reader.read_u32());

    obj.required_skill_value = reader.read_u32();

    for (auto i = 0; i < 3; ++i) {
        obj.required_spells[i] = reader.read_u32();
    }

    return obj;
}

void TrainerSpell_write(Writer& writer, const TrainerSpell& obj) {
    writer.write_u32(obj.spell);

    writer.write_u8(static_cast<uint8_t>(obj.state));

    writer.write_u32(obj.spell_cost);

    writer.write_u32(obj.talent_point_cost);

    writer.write_u32(obj.first_rank);

    writer.write_u8(obj.required_level);

    writer.write_u32(static_cast<uint32_t>(obj.required_skill));

    writer.write_u32(obj.required_skill_value);

    for (const auto& v : obj.required_spells) {
        writer.write_u32(v);
    }

}

static size_t WhoPlayer_size(const WhoPlayer& obj) {
    return 16 + obj.name.size() + obj.guild.size();
}

WhoPlayer WhoPlayer_read(Reader& reader) {
    WhoPlayer obj;

    obj.name = reader.read_cstring();

    obj.guild = reader.read_cstring();

    obj.level = reader.read_u32();

    obj.class_type = static_cast<Class>(reader.read_u8());

    obj.race = static_cast<Race>(reader.read_u8());

    obj.area = static_cast<Area>(reader.read_u32());

    obj.party_status = reader.read_u32();

    return obj;
}

void WhoPlayer_write(Writer& writer, const WhoPlayer& obj) {
    writer.write_cstring(obj.name);

    writer.write_cstring(obj.guild);

    writer.write_u32(obj.level);

    writer.write_u8(static_cast<uint8_t>(obj.class_type));

    writer.write_u8(static_cast<uint8_t>(obj.race));

    writer.write_u32(static_cast<uint32_t>(obj.area));

    writer.write_u32(obj.party_status);

}

WorldState WorldState_read(Reader& reader) {
    WorldState obj;

    obj.state = reader.read_u32();

    obj.value = reader.read_u32();

    return obj;
}

void WorldState_write(Writer& writer, const WorldState& obj) {
    writer.write_u32(obj.state);

    writer.write_u32(obj.value);

}

std::vector<unsigned char> CMSG_BOOTME::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x00000001); /* opcode */

    return writer.m_buf;
}

static size_t CMSG_DBLOOKUP_size(const CMSG_DBLOOKUP& obj) {
    return 1 + obj.query.size();
}

CMSG_DBLOOKUP CMSG_DBLOOKUP_read(Reader& reader) {
    CMSG_DBLOOKUP obj;

    obj.query = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_DBLOOKUP::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_DBLOOKUP_size(obj));

    writer.write_u16_be((uint16_t)CMSG_DBLOOKUP_size(obj) + 4); /* size */

    writer.write_u32(0x00000002); /* opcode */

    writer.write_cstring(obj.query);

    return writer.m_buf;
}

CMSG_WORLD_TELEPORT CMSG_WORLD_TELEPORT_read(Reader& reader) {
    CMSG_WORLD_TELEPORT obj;

    obj.time = reader.read_u32();

    obj.map = static_cast<Map>(reader.read_u32());

    obj.position = ::wow_world_messages::all::Vector3d_read(reader);

    obj.orientation = reader.read_float();

    return obj;
}

std::vector<unsigned char> CMSG_WORLD_TELEPORT::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0018);

    writer.write_u16_be(0x0018 + 4); /* size */

    writer.write_u32(0x00000008); /* opcode */

    writer.write_u32(obj.time);

    writer.write_u32(static_cast<uint32_t>(obj.map));

    Vector3d_write(writer, obj.position);

    writer.write_float(obj.orientation);

    return writer.m_buf;
}

static size_t CMSG_TELEPORT_TO_UNIT_size(const CMSG_TELEPORT_TO_UNIT& obj) {
    return 1 + obj.name.size();
}

CMSG_TELEPORT_TO_UNIT CMSG_TELEPORT_TO_UNIT_read(Reader& reader) {
    CMSG_TELEPORT_TO_UNIT obj;

    obj.name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_TELEPORT_TO_UNIT::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_TELEPORT_TO_UNIT_size(obj));

    writer.write_u16_be((uint16_t)CMSG_TELEPORT_TO_UNIT_size(obj) + 4); /* size */

    writer.write_u32(0x00000009); /* opcode */

    writer.write_cstring(obj.name);

    return writer.m_buf;
}

static size_t CMSG_CHAR_CREATE_size(const CMSG_CHAR_CREATE& obj) {
    return 10 + obj.name.size();
}

CMSG_CHAR_CREATE CMSG_CHAR_CREATE_read(Reader& reader) {
    CMSG_CHAR_CREATE obj;

    obj.name = reader.read_cstring();

    obj.race = static_cast<Race>(reader.read_u8());

    obj.class_type = static_cast<Class>(reader.read_u8());

    obj.gender = static_cast<Gender>(reader.read_u8());

    obj.skin_color = reader.read_u8();

    obj.face = reader.read_u8();

    obj.hair_style = reader.read_u8();

    obj.hair_color = reader.read_u8();

    obj.facial_hair = reader.read_u8();

    (void)reader.read_u8();

    return obj;
}

std::vector<unsigned char> CMSG_CHAR_CREATE::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_CHAR_CREATE_size(obj));

    writer.write_u16_be((uint16_t)CMSG_CHAR_CREATE_size(obj) + 4); /* size */

    writer.write_u32(0x00000036); /* opcode */

    writer.write_cstring(obj.name);

    writer.write_u8(static_cast<uint8_t>(obj.race));

    writer.write_u8(static_cast<uint8_t>(obj.class_type));

    writer.write_u8(static_cast<uint8_t>(obj.gender));

    writer.write_u8(obj.skin_color);

    writer.write_u8(obj.face);

    writer.write_u8(obj.hair_style);

    writer.write_u8(obj.hair_color);

    writer.write_u8(obj.facial_hair);

    writer.write_u8(0);

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_CHAR_ENUM::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x00000037); /* opcode */

    return writer.m_buf;
}

CMSG_CHAR_DELETE CMSG_CHAR_DELETE_read(Reader& reader) {
    CMSG_CHAR_DELETE obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_CHAR_DELETE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x00000038); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

SMSG_CHAR_CREATE SMSG_CHAR_CREATE_read(Reader& reader) {
    SMSG_CHAR_CREATE obj;

    obj.result = static_cast<WorldResult>(reader.read_u8());

    return obj;
}

std::vector<unsigned char> SMSG_CHAR_CREATE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0001);

    writer.write_u16_be(0x0001 + 2); /* size */

    writer.write_u16(0x0000003a); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.result));

    return writer.m_buf;
}

static size_t SMSG_CHAR_ENUM_size(const SMSG_CHAR_ENUM& obj) {
    size_t _size = 1;

    for(const auto& v : obj.characters) {
        _size += Character_size(v);
    }

    return _size;
}

SMSG_CHAR_ENUM SMSG_CHAR_ENUM_read(Reader& reader) {
    SMSG_CHAR_ENUM obj;

    obj.amount_of_characters = reader.read_u8();

    for (auto i = 0; i < obj.amount_of_characters; ++i) {
        obj.characters.push_back(::wow_world_messages::vanilla::Character_read(reader));
    }

    return obj;
}

std::vector<unsigned char> SMSG_CHAR_ENUM::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_CHAR_ENUM_size(obj));

    writer.write_u16_be(SMSG_CHAR_ENUM_size(obj) + 2); /* size */

    writer.write_u16(0x0000003b); /* opcode */

    writer.write_u8(obj.amount_of_characters);

    for (const auto& v : obj.characters) {
        Character_write(writer, v);
    }

    return writer.m_buf;
}

SMSG_CHAR_DELETE SMSG_CHAR_DELETE_read(Reader& reader) {
    SMSG_CHAR_DELETE obj;

    obj.result = static_cast<WorldResult>(reader.read_u8());

    return obj;
}

std::vector<unsigned char> SMSG_CHAR_DELETE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0001);

    writer.write_u16_be(0x0001 + 2); /* size */

    writer.write_u16(0x0000003c); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.result));

    return writer.m_buf;
}

CMSG_PLAYER_LOGIN CMSG_PLAYER_LOGIN_read(Reader& reader) {
    CMSG_PLAYER_LOGIN obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_PLAYER_LOGIN::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x0000003d); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

SMSG_NEW_WORLD SMSG_NEW_WORLD_read(Reader& reader) {
    SMSG_NEW_WORLD obj;

    obj.map = static_cast<Map>(reader.read_u32());

    obj.position = ::wow_world_messages::all::Vector3d_read(reader);

    obj.orientation = reader.read_float();

    return obj;
}

std::vector<unsigned char> SMSG_NEW_WORLD::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0014);

    writer.write_u16_be(0x0014 + 2); /* size */

    writer.write_u16(0x0000003e); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.map));

    Vector3d_write(writer, obj.position);

    writer.write_float(obj.orientation);

    return writer.m_buf;
}

SMSG_TRANSFER_ABORTED SMSG_TRANSFER_ABORTED_read(Reader& reader) {
    SMSG_TRANSFER_ABORTED obj;

    obj.map = static_cast<Map>(reader.read_u32());

    obj.reason = static_cast<TransferAbortReason>(reader.read_u8());

    obj.argument = reader.read_u8();

    return obj;
}

std::vector<unsigned char> SMSG_TRANSFER_ABORTED::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0006);

    writer.write_u16_be(0x0006 + 2); /* size */

    writer.write_u16(0x00000040); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.map));

    writer.write_u8(static_cast<uint8_t>(obj.reason));

    writer.write_u8(obj.argument);

    return writer.m_buf;
}

SMSG_CHARACTER_LOGIN_FAILED SMSG_CHARACTER_LOGIN_FAILED_read(Reader& reader) {
    SMSG_CHARACTER_LOGIN_FAILED obj;

    obj.result = static_cast<WorldResult>(reader.read_u8());

    return obj;
}

std::vector<unsigned char> SMSG_CHARACTER_LOGIN_FAILED::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0001);

    writer.write_u16_be(0x0001 + 2); /* size */

    writer.write_u16(0x00000041); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.result));

    return writer.m_buf;
}

SMSG_LOGIN_SETTIMESPEED SMSG_LOGIN_SETTIMESPEED_read(Reader& reader) {
    SMSG_LOGIN_SETTIMESPEED obj;

    obj.datetime = reader.read_u32();

    obj.timescale = reader.read_float();

    return obj;
}

std::vector<unsigned char> SMSG_LOGIN_SETTIMESPEED::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 2); /* size */

    writer.write_u16(0x00000042); /* opcode */

    writer.write_u32(obj.datetime);

    writer.write_float(obj.timescale);

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_PLAYER_LOGOUT::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x0000004a); /* opcode */

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_LOGOUT_REQUEST::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x0000004b); /* opcode */

    return writer.m_buf;
}

SMSG_LOGOUT_RESPONSE SMSG_LOGOUT_RESPONSE_read(Reader& reader) {
    SMSG_LOGOUT_RESPONSE obj;

    obj.result = static_cast<LogoutResult>(reader.read_u32());

    obj.speed = static_cast<LogoutSpeed>(reader.read_u8());

    return obj;
}

std::vector<unsigned char> SMSG_LOGOUT_RESPONSE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0005);

    writer.write_u16_be(0x0005 + 2); /* size */

    writer.write_u16(0x0000004c); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.result));

    writer.write_u8(static_cast<uint8_t>(obj.speed));

    return writer.m_buf;
}

std::vector<unsigned char> SMSG_LOGOUT_COMPLETE::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 2); /* size */

    writer.write_u16(0x0000004d); /* opcode */

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_LOGOUT_CANCEL::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x0000004e); /* opcode */

    return writer.m_buf;
}

std::vector<unsigned char> SMSG_LOGOUT_CANCEL_ACK::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 2); /* size */

    writer.write_u16(0x0000004f); /* opcode */

    return writer.m_buf;
}

CMSG_NAME_QUERY CMSG_NAME_QUERY_read(Reader& reader) {
    CMSG_NAME_QUERY obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_NAME_QUERY::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x00000050); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

static size_t SMSG_NAME_QUERY_RESPONSE_size(const SMSG_NAME_QUERY_RESPONSE& obj) {
    return 22 + obj.character_name.size() + obj.realm_name.size();
}

SMSG_NAME_QUERY_RESPONSE SMSG_NAME_QUERY_RESPONSE_read(Reader& reader) {
    SMSG_NAME_QUERY_RESPONSE obj;

    obj.guid = reader.read_u64();

    obj.character_name = reader.read_cstring();

    obj.realm_name = reader.read_cstring();

    obj.race = static_cast<Race>(reader.read_u32());

    obj.gender = static_cast<Gender>(reader.read_u32());

    obj.class_type = static_cast<Class>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_NAME_QUERY_RESPONSE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_NAME_QUERY_RESPONSE_size(obj));

    writer.write_u16_be(SMSG_NAME_QUERY_RESPONSE_size(obj) + 2); /* size */

    writer.write_u16(0x00000051); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_cstring(obj.character_name);

    writer.write_cstring(obj.realm_name);

    writer.write_u32(static_cast<uint32_t>(obj.race));

    writer.write_u32(static_cast<uint32_t>(obj.gender));

    writer.write_u32(static_cast<uint32_t>(obj.class_type));

    return writer.m_buf;
}

CMSG_PET_NAME_QUERY CMSG_PET_NAME_QUERY_read(Reader& reader) {
    CMSG_PET_NAME_QUERY obj;

    obj.pet_number = reader.read_u32();

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_PET_NAME_QUERY::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 4); /* size */

    writer.write_u32(0x00000052); /* opcode */

    writer.write_u32(obj.pet_number);

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

static size_t SMSG_PET_NAME_QUERY_RESPONSE_size(const SMSG_PET_NAME_QUERY_RESPONSE& obj) {
    return 9 + obj.name.size();
}

SMSG_PET_NAME_QUERY_RESPONSE SMSG_PET_NAME_QUERY_RESPONSE_read(Reader& reader) {
    SMSG_PET_NAME_QUERY_RESPONSE obj;

    obj.pet_number = reader.read_u32();

    obj.name = reader.read_cstring();

    obj.pet_name_timestamp = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_PET_NAME_QUERY_RESPONSE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_PET_NAME_QUERY_RESPONSE_size(obj));

    writer.write_u16_be(SMSG_PET_NAME_QUERY_RESPONSE_size(obj) + 2); /* size */

    writer.write_u16(0x00000053); /* opcode */

    writer.write_u32(obj.pet_number);

    writer.write_cstring(obj.name);

    writer.write_u32(obj.pet_name_timestamp);

    return writer.m_buf;
}

CMSG_GUILD_QUERY CMSG_GUILD_QUERY_read(Reader& reader) {
    CMSG_GUILD_QUERY obj;

    obj.guild_id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_GUILD_QUERY::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 4); /* size */

    writer.write_u32(0x00000054); /* opcode */

    writer.write_u32(obj.guild_id);

    return writer.m_buf;
}

static size_t SMSG_GUILD_QUERY_RESPONSE_size(const SMSG_GUILD_QUERY_RESPONSE& obj) {
    size_t _size = 25 + obj.name.size();

    for(const auto& v : obj.rank_names) {
        _size += v.size() + 1;
    }

    return _size;
}

SMSG_GUILD_QUERY_RESPONSE SMSG_GUILD_QUERY_RESPONSE_read(Reader& reader) {
    SMSG_GUILD_QUERY_RESPONSE obj;

    obj.id = reader.read_u32();

    obj.name = reader.read_cstring();

    for (auto i = 0; i < 10; ++i) {
        obj.rank_names[i] = reader.read_cstring();
    }

    obj.emblem_style = reader.read_u32();

    obj.emblem_color = reader.read_u32();

    obj.border_style = reader.read_u32();

    obj.border_color = reader.read_u32();

    obj.background_color = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_GUILD_QUERY_RESPONSE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_GUILD_QUERY_RESPONSE_size(obj));

    writer.write_u16_be(SMSG_GUILD_QUERY_RESPONSE_size(obj) + 2); /* size */

    writer.write_u16(0x00000055); /* opcode */

    writer.write_u32(obj.id);

    writer.write_cstring(obj.name);

    for (const auto& v : obj.rank_names) {
        writer.write_cstring(v);
    }

    writer.write_u32(obj.emblem_style);

    writer.write_u32(obj.emblem_color);

    writer.write_u32(obj.border_style);

    writer.write_u32(obj.border_color);

    writer.write_u32(obj.background_color);

    return writer.m_buf;
}

CMSG_ITEM_QUERY_SINGLE CMSG_ITEM_QUERY_SINGLE_read(Reader& reader) {
    CMSG_ITEM_QUERY_SINGLE obj;

    obj.item = reader.read_u32();

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_ITEM_QUERY_SINGLE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 4); /* size */

    writer.write_u32(0x00000056); /* opcode */

    writer.write_u32(obj.item);

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

CMSG_PAGE_TEXT_QUERY CMSG_PAGE_TEXT_QUERY_read(Reader& reader) {
    CMSG_PAGE_TEXT_QUERY obj;

    obj.page_id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_PAGE_TEXT_QUERY::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 4); /* size */

    writer.write_u32(0x0000005a); /* opcode */

    writer.write_u32(obj.page_id);

    return writer.m_buf;
}

static size_t SMSG_PAGE_TEXT_QUERY_RESPONSE_size(const SMSG_PAGE_TEXT_QUERY_RESPONSE& obj) {
    return 9 + obj.text.size();
}

SMSG_PAGE_TEXT_QUERY_RESPONSE SMSG_PAGE_TEXT_QUERY_RESPONSE_read(Reader& reader) {
    SMSG_PAGE_TEXT_QUERY_RESPONSE obj;

    obj.page_id = reader.read_u32();

    obj.text = reader.read_cstring();

    obj.next_page_id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_PAGE_TEXT_QUERY_RESPONSE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_PAGE_TEXT_QUERY_RESPONSE_size(obj));

    writer.write_u16_be(SMSG_PAGE_TEXT_QUERY_RESPONSE_size(obj) + 2); /* size */

    writer.write_u16(0x0000005b); /* opcode */

    writer.write_u32(obj.page_id);

    writer.write_cstring(obj.text);

    writer.write_u32(obj.next_page_id);

    return writer.m_buf;
}

CMSG_QUEST_QUERY CMSG_QUEST_QUERY_read(Reader& reader) {
    CMSG_QUEST_QUERY obj;

    obj.quest_id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_QUEST_QUERY::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 4); /* size */

    writer.write_u32(0x0000005c); /* opcode */

    writer.write_u32(obj.quest_id);

    return writer.m_buf;
}

static size_t SMSG_QUEST_QUERY_RESPONSE_size(const SMSG_QUEST_QUERY_RESPONSE& obj) {
    size_t _size = 220 + obj.title.size() + obj.objective_text.size() + obj.details.size() + obj.end_text.size();

    for(const auto& v : obj.objective_texts) {
        _size += v.size() + 1;
    }

    return _size;
}

SMSG_QUEST_QUERY_RESPONSE SMSG_QUEST_QUERY_RESPONSE_read(Reader& reader) {
    SMSG_QUEST_QUERY_RESPONSE obj;

    obj.quest_id = reader.read_u32();

    obj.quest_method = reader.read_u32();

    obj.quest_level = reader.read_u32();

    obj.zone_or_sort = reader.read_u32();

    obj.quest_type = reader.read_u32();

    obj.reputation_objective_faction = static_cast<Faction>(reader.read_u16());

    obj.reputation_objective_value = reader.read_u32();

    obj.required_opposite_faction = static_cast<Faction>(reader.read_u16());

    obj.required_opposite_reputation_value = reader.read_u32();

    obj.next_quest_in_chain = reader.read_u32();

    obj.money_reward = reader.read_u32();

    obj.max_level_money_reward = reader.read_u32();

    obj.reward_spell = reader.read_u32();

    obj.source_item_id = reader.read_u32();

    obj.quest_flags = reader.read_u32();

    for (auto i = 0; i < 4; ++i) {
        obj.rewards[i] = ::wow_world_messages::vanilla::QuestItemReward_read(reader);
    }

    for (auto i = 0; i < 6; ++i) {
        obj.choice_rewards[i] = ::wow_world_messages::vanilla::QuestItemReward_read(reader);
    }

    obj.point_map_id = reader.read_u32();

    obj.position = ::wow_world_messages::all::Vector2d_read(reader);

    obj.point_opt = reader.read_u32();

    obj.title = reader.read_cstring();

    obj.objective_text = reader.read_cstring();

    obj.details = reader.read_cstring();

    obj.end_text = reader.read_cstring();

    for (auto i = 0; i < 4; ++i) {
        obj.objectives[i] = ::wow_world_messages::vanilla::QuestObjective_read(reader);
    }

    for (auto i = 0; i < 4; ++i) {
        obj.objective_texts[i] = reader.read_cstring();
    }

    return obj;
}

std::vector<unsigned char> SMSG_QUEST_QUERY_RESPONSE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_QUEST_QUERY_RESPONSE_size(obj));

    writer.write_u16_be(SMSG_QUEST_QUERY_RESPONSE_size(obj) + 2); /* size */

    writer.write_u16(0x0000005d); /* opcode */

    writer.write_u32(obj.quest_id);

    writer.write_u32(obj.quest_method);

    writer.write_u32(obj.quest_level);

    writer.write_u32(obj.zone_or_sort);

    writer.write_u32(obj.quest_type);

    writer.write_u16(static_cast<uint16_t>(obj.reputation_objective_faction));

    writer.write_u32(obj.reputation_objective_value);

    writer.write_u16(static_cast<uint16_t>(obj.required_opposite_faction));

    writer.write_u32(obj.required_opposite_reputation_value);

    writer.write_u32(obj.next_quest_in_chain);

    writer.write_u32(obj.money_reward);

    writer.write_u32(obj.max_level_money_reward);

    writer.write_u32(obj.reward_spell);

    writer.write_u32(obj.source_item_id);

    writer.write_u32(obj.quest_flags);

    for (const auto& v : obj.rewards) {
        QuestItemReward_write(writer, v);
    }

    for (const auto& v : obj.choice_rewards) {
        QuestItemReward_write(writer, v);
    }

    writer.write_u32(obj.point_map_id);

    Vector2d_write(writer, obj.position);

    writer.write_u32(obj.point_opt);

    writer.write_cstring(obj.title);

    writer.write_cstring(obj.objective_text);

    writer.write_cstring(obj.details);

    writer.write_cstring(obj.end_text);

    for (const auto& v : obj.objectives) {
        QuestObjective_write(writer, v);
    }

    for (const auto& v : obj.objective_texts) {
        writer.write_cstring(v);
    }

    return writer.m_buf;
}

CMSG_GAMEOBJECT_QUERY CMSG_GAMEOBJECT_QUERY_read(Reader& reader) {
    CMSG_GAMEOBJECT_QUERY obj;

    obj.entry_id = reader.read_u32();

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_GAMEOBJECT_QUERY::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 4); /* size */

    writer.write_u32(0x0000005e); /* opcode */

    writer.write_u32(obj.entry_id);

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

CMSG_CREATURE_QUERY CMSG_CREATURE_QUERY_read(Reader& reader) {
    CMSG_CREATURE_QUERY obj;

    obj.creature = reader.read_u32();

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_CREATURE_QUERY::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 4); /* size */

    writer.write_u32(0x00000060); /* opcode */

    writer.write_u32(obj.creature);

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

static size_t CMSG_WHO_size(const CMSG_WHO& obj) {
    size_t _size = 26 + obj.player_name.size() + obj.guild_name.size() + 4 * obj.zones.size();

    for(const auto& v : obj.search_strings) {
        _size += v.size() + 1;
    }

    return _size;
}

CMSG_WHO CMSG_WHO_read(Reader& reader) {
    CMSG_WHO obj;

    obj.minimum_level = reader.read_u32();

    obj.maximum_level = reader.read_u32();

    obj.player_name = reader.read_cstring();

    obj.guild_name = reader.read_cstring();

    obj.race_mask = reader.read_u32();

    obj.class_mask = reader.read_u32();

    obj.amount_of_zones = reader.read_u32();

    for (auto i = 0; i < obj.amount_of_zones; ++i) {
        obj.zones.push_back(reader.read_u32());
    }

    obj.amount_of_strings = reader.read_u32();

    for (auto i = 0; i < obj.amount_of_strings; ++i) {
        obj.search_strings.push_back(reader.read_cstring());
    }

    return obj;
}

std::vector<unsigned char> CMSG_WHO::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_WHO_size(obj));

    writer.write_u16_be((uint16_t)CMSG_WHO_size(obj) + 4); /* size */

    writer.write_u32(0x00000062); /* opcode */

    writer.write_u32(obj.minimum_level);

    writer.write_u32(obj.maximum_level);

    writer.write_cstring(obj.player_name);

    writer.write_cstring(obj.guild_name);

    writer.write_u32(obj.race_mask);

    writer.write_u32(obj.class_mask);

    writer.write_u32(obj.amount_of_zones);

    for (const auto& v : obj.zones) {
        writer.write_u32(v);
    }

    writer.write_u32(obj.amount_of_strings);

    for (const auto& v : obj.search_strings) {
        writer.write_cstring(v);
    }

    return writer.m_buf;
}

static size_t SMSG_WHO_size(const SMSG_WHO& obj) {
    size_t _size = 8;

    for(const auto& v : obj.players) {
        _size += WhoPlayer_size(v);
    }

    return _size;
}

SMSG_WHO SMSG_WHO_read(Reader& reader) {
    SMSG_WHO obj;

    obj.listed_players = reader.read_u32();

    obj.online_players = reader.read_u32();

    for (auto i = 0; i < obj.listed_players; ++i) {
        obj.players.push_back(::wow_world_messages::vanilla::WhoPlayer_read(reader));
    }

    return obj;
}

std::vector<unsigned char> SMSG_WHO::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_WHO_size(obj));

    writer.write_u16_be(SMSG_WHO_size(obj) + 2); /* size */

    writer.write_u16(0x00000063); /* opcode */

    writer.write_u32(obj.listed_players);

    writer.write_u32(obj.online_players);

    for (const auto& v : obj.players) {
        WhoPlayer_write(writer, v);
    }

    return writer.m_buf;
}

static size_t CMSG_WHOIS_size(const CMSG_WHOIS& obj) {
    return 1 + obj.character.size();
}

CMSG_WHOIS CMSG_WHOIS_read(Reader& reader) {
    CMSG_WHOIS obj;

    obj.character = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_WHOIS::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_WHOIS_size(obj));

    writer.write_u16_be((uint16_t)CMSG_WHOIS_size(obj) + 4); /* size */

    writer.write_u32(0x00000064); /* opcode */

    writer.write_cstring(obj.character);

    return writer.m_buf;
}

static size_t SMSG_WHOIS_size(const SMSG_WHOIS& obj) {
    return 1 + obj.message.size();
}

SMSG_WHOIS SMSG_WHOIS_read(Reader& reader) {
    SMSG_WHOIS obj;

    obj.message = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> SMSG_WHOIS::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_WHOIS_size(obj));

    writer.write_u16_be(SMSG_WHOIS_size(obj) + 2); /* size */

    writer.write_u16(0x00000065); /* opcode */

    writer.write_cstring(obj.message);

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_FRIEND_LIST::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x00000066); /* opcode */

    return writer.m_buf;
}

static size_t SMSG_FRIEND_LIST_size(const SMSG_FRIEND_LIST& obj) {
    size_t _size = 1;

    for(const auto& v : obj.friends) {
        _size += Friend_size(v);
    }

    return _size;
}

SMSG_FRIEND_LIST SMSG_FRIEND_LIST_read(Reader& reader) {
    SMSG_FRIEND_LIST obj;

    obj.amount_of_friends = reader.read_u8();

    for (auto i = 0; i < obj.amount_of_friends; ++i) {
        obj.friends.push_back(::wow_world_messages::vanilla::Friend_read(reader));
    }

    return obj;
}

std::vector<unsigned char> SMSG_FRIEND_LIST::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_FRIEND_LIST_size(obj));

    writer.write_u16_be(SMSG_FRIEND_LIST_size(obj) + 2); /* size */

    writer.write_u16(0x00000067); /* opcode */

    writer.write_u8(obj.amount_of_friends);

    for (const auto& v : obj.friends) {
        Friend_write(writer, v);
    }

    return writer.m_buf;
}

SMSG_FRIEND_STATUS SMSG_FRIEND_STATUS_read(Reader& reader) {
    SMSG_FRIEND_STATUS obj;

    obj.result = static_cast<FriendResult>(reader.read_u8());

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> SMSG_FRIEND_STATUS::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0009);

    writer.write_u16_be(0x0009 + 2); /* size */

    writer.write_u16(0x00000068); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.result));

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

static size_t CMSG_ADD_FRIEND_size(const CMSG_ADD_FRIEND& obj) {
    return 1 + obj.name.size();
}

CMSG_ADD_FRIEND CMSG_ADD_FRIEND_read(Reader& reader) {
    CMSG_ADD_FRIEND obj;

    obj.name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_ADD_FRIEND::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_ADD_FRIEND_size(obj));

    writer.write_u16_be((uint16_t)CMSG_ADD_FRIEND_size(obj) + 4); /* size */

    writer.write_u32(0x00000069); /* opcode */

    writer.write_cstring(obj.name);

    return writer.m_buf;
}

CMSG_DEL_FRIEND CMSG_DEL_FRIEND_read(Reader& reader) {
    CMSG_DEL_FRIEND obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_DEL_FRIEND::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x0000006a); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

static size_t SMSG_IGNORE_LIST_size(const SMSG_IGNORE_LIST& obj) {
    return 1 + 8 * obj.ignored.size();
}

SMSG_IGNORE_LIST SMSG_IGNORE_LIST_read(Reader& reader) {
    SMSG_IGNORE_LIST obj;

    obj.amount_of_ignored = reader.read_u8();

    for (auto i = 0; i < obj.amount_of_ignored; ++i) {
        obj.ignored.push_back(reader.read_u64());
    }

    return obj;
}

std::vector<unsigned char> SMSG_IGNORE_LIST::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_IGNORE_LIST_size(obj));

    writer.write_u16_be(SMSG_IGNORE_LIST_size(obj) + 2); /* size */

    writer.write_u16(0x0000006b); /* opcode */

    writer.write_u8(obj.amount_of_ignored);

    for (const auto& v : obj.ignored) {
        writer.write_u64(v);
    }

    return writer.m_buf;
}

static size_t CMSG_ADD_IGNORE_size(const CMSG_ADD_IGNORE& obj) {
    return 1 + obj.name.size();
}

CMSG_ADD_IGNORE CMSG_ADD_IGNORE_read(Reader& reader) {
    CMSG_ADD_IGNORE obj;

    obj.name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_ADD_IGNORE::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_ADD_IGNORE_size(obj));

    writer.write_u16_be((uint16_t)CMSG_ADD_IGNORE_size(obj) + 4); /* size */

    writer.write_u32(0x0000006c); /* opcode */

    writer.write_cstring(obj.name);

    return writer.m_buf;
}

CMSG_DEL_IGNORE CMSG_DEL_IGNORE_read(Reader& reader) {
    CMSG_DEL_IGNORE obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_DEL_IGNORE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x0000006d); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

static size_t CMSG_GROUP_INVITE_size(const CMSG_GROUP_INVITE& obj) {
    return 1 + obj.name.size();
}

CMSG_GROUP_INVITE CMSG_GROUP_INVITE_read(Reader& reader) {
    CMSG_GROUP_INVITE obj;

    obj.name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_GROUP_INVITE::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_GROUP_INVITE_size(obj));

    writer.write_u16_be((uint16_t)CMSG_GROUP_INVITE_size(obj) + 4); /* size */

    writer.write_u32(0x0000006e); /* opcode */

    writer.write_cstring(obj.name);

    return writer.m_buf;
}

static size_t SMSG_GROUP_INVITE_size(const SMSG_GROUP_INVITE& obj) {
    return 1 + obj.name.size();
}

SMSG_GROUP_INVITE SMSG_GROUP_INVITE_read(Reader& reader) {
    SMSG_GROUP_INVITE obj;

    obj.name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> SMSG_GROUP_INVITE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_GROUP_INVITE_size(obj));

    writer.write_u16_be(SMSG_GROUP_INVITE_size(obj) + 2); /* size */

    writer.write_u16(0x0000006f); /* opcode */

    writer.write_cstring(obj.name);

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_GROUP_ACCEPT::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x00000072); /* opcode */

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_GROUP_DECLINE::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x00000073); /* opcode */

    return writer.m_buf;
}

static size_t SMSG_GROUP_DECLINE_size(const SMSG_GROUP_DECLINE& obj) {
    return 1 + obj.name.size();
}

SMSG_GROUP_DECLINE SMSG_GROUP_DECLINE_read(Reader& reader) {
    SMSG_GROUP_DECLINE obj;

    obj.name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> SMSG_GROUP_DECLINE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_GROUP_DECLINE_size(obj));

    writer.write_u16_be(SMSG_GROUP_DECLINE_size(obj) + 2); /* size */

    writer.write_u16(0x00000074); /* opcode */

    writer.write_cstring(obj.name);

    return writer.m_buf;
}

static size_t CMSG_GROUP_UNINVITE_size(const CMSG_GROUP_UNINVITE& obj) {
    return 1 + obj.name.size();
}

CMSG_GROUP_UNINVITE CMSG_GROUP_UNINVITE_read(Reader& reader) {
    CMSG_GROUP_UNINVITE obj;

    obj.name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_GROUP_UNINVITE::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_GROUP_UNINVITE_size(obj));

    writer.write_u16_be((uint16_t)CMSG_GROUP_UNINVITE_size(obj) + 4); /* size */

    writer.write_u32(0x00000075); /* opcode */

    writer.write_cstring(obj.name);

    return writer.m_buf;
}

CMSG_GROUP_UNINVITE_GUID CMSG_GROUP_UNINVITE_GUID_read(Reader& reader) {
    CMSG_GROUP_UNINVITE_GUID obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_GROUP_UNINVITE_GUID::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x00000076); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

std::vector<unsigned char> SMSG_GROUP_UNINVITE::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 2); /* size */

    writer.write_u16(0x00000077); /* opcode */

    return writer.m_buf;
}

CMSG_GROUP_SET_LEADER CMSG_GROUP_SET_LEADER_read(Reader& reader) {
    CMSG_GROUP_SET_LEADER obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_GROUP_SET_LEADER::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x00000078); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

static size_t SMSG_GROUP_SET_LEADER_size(const SMSG_GROUP_SET_LEADER& obj) {
    return 1 + obj.name.size();
}

SMSG_GROUP_SET_LEADER SMSG_GROUP_SET_LEADER_read(Reader& reader) {
    SMSG_GROUP_SET_LEADER obj;

    obj.name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> SMSG_GROUP_SET_LEADER::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_GROUP_SET_LEADER_size(obj));

    writer.write_u16_be(SMSG_GROUP_SET_LEADER_size(obj) + 2); /* size */

    writer.write_u16(0x00000079); /* opcode */

    writer.write_cstring(obj.name);

    return writer.m_buf;
}

CMSG_LOOT_METHOD CMSG_LOOT_METHOD_read(Reader& reader) {
    CMSG_LOOT_METHOD obj;

    obj.loot_setting = static_cast<GroupLootSetting>(reader.read_u32());

    obj.loot_master = reader.read_u64();

    obj.loot_threshold = static_cast<ItemQuality>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> CMSG_LOOT_METHOD::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0010);

    writer.write_u16_be(0x0010 + 4); /* size */

    writer.write_u32(0x0000007a); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.loot_setting));

    writer.write_u64(obj.loot_master);

    writer.write_u32(static_cast<uint32_t>(obj.loot_threshold));

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_GROUP_DISBAND::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x0000007b); /* opcode */

    return writer.m_buf;
}

std::vector<unsigned char> SMSG_GROUP_DESTROYED::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 2); /* size */

    writer.write_u16(0x0000007c); /* opcode */

    return writer.m_buf;
}

static size_t SMSG_PARTY_MEMBER_STATS_size(const SMSG_PARTY_MEMBER_STATS& obj) {
    size_t _size = 4 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);

    if ((obj.mask & GROUP_UPDATE_FLAGS_STATUS) != 0) {
        _size += 1;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_CUR_HP) != 0) {
        _size += 2;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_MAX_HP) != 0) {
        _size += 2;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_POWER_TYPE) != 0) {
        _size += 1;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_CUR_POWER) != 0) {
        _size += 2;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_MAX_POWER) != 0) {
        _size += 2;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_LEVEL) != 0) {
        _size += 2;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_ZONE) != 0) {
        _size += 4;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_POSITION) != 0) {
        _size += 4;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_AURAS) != 0) {
        _size += 0 + aura_mask_size(obj.auras);
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_AURAS_2) != 0) {
        _size += 0 + aura_mask_size(obj.negative_auras);
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_GUID) != 0) {
        _size += 8;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_NAME) != 0) {
        _size += 1 + obj.pet_name.size();
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_MODEL_ID) != 0) {
        _size += 2;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_CUR_HP) != 0) {
        _size += 2;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_MAX_HP) != 0) {
        _size += 2;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_POWER_TYPE) != 0) {
        _size += 1;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_CUR_POWER) != 0) {
        _size += 2;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_MAX_POWER) != 0) {
        _size += 2;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_AURAS) != 0) {
        _size += 0 + aura_mask_size(obj.pet_auras);
    }

    return _size;
}

SMSG_PARTY_MEMBER_STATS SMSG_PARTY_MEMBER_STATS_read(Reader& reader) {
    SMSG_PARTY_MEMBER_STATS obj;

    obj.guid = reader.read_packed_guid();

    obj.mask = static_cast<GroupUpdateFlags>(reader.read_u32());

    if ((obj.mask & GROUP_UPDATE_FLAGS_STATUS) != 0) {
        obj.status = static_cast<GroupMemberOnlineStatus>(reader.read_u8());

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_CUR_HP) != 0) {
        obj.current_health = reader.read_u16();

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_MAX_HP) != 0) {
        obj.max_health = reader.read_u16();

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_POWER_TYPE) != 0) {
        obj.power = static_cast<Power>(reader.read_u8());

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_CUR_POWER) != 0) {
        obj.current_power = reader.read_u16();

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_MAX_POWER) != 0) {
        obj.max_power = reader.read_u16();

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_LEVEL) != 0) {
        obj.level = reader.read_u16();

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_ZONE) != 0) {
        obj.area = static_cast<Area>(reader.read_u32());

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_POSITION) != 0) {
        obj.position_x = reader.read_u16();

        obj.position_y = reader.read_u16();

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_AURAS) != 0) {
        obj.auras = ::wow_world_messages::vanilla::aura_mask_read(reader);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_AURAS_2) != 0) {
        obj.negative_auras = ::wow_world_messages::vanilla::aura_mask_read(reader);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_GUID) != 0) {
        obj.pet = reader.read_u64();

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_NAME) != 0) {
        obj.pet_name = reader.read_cstring();

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_MODEL_ID) != 0) {
        obj.pet_display_id = reader.read_u16();

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_CUR_HP) != 0) {
        obj.pet_current_health = reader.read_u16();

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_MAX_HP) != 0) {
        obj.pet_max_health = reader.read_u16();

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_POWER_TYPE) != 0) {
        obj.pet_power_type = static_cast<Power>(reader.read_u8());

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_CUR_POWER) != 0) {
        obj.pet_current_power = reader.read_u16();

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_MAX_POWER) != 0) {
        obj.pet_max_power = reader.read_u16();

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_AURAS) != 0) {
        obj.pet_auras = ::wow_world_messages::vanilla::aura_mask_read(reader);

    }
    return obj;
}

std::vector<unsigned char> SMSG_PARTY_MEMBER_STATS::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_PARTY_MEMBER_STATS_size(obj));

    writer.write_u16_be(SMSG_PARTY_MEMBER_STATS_size(obj) + 2); /* size */

    writer.write_u16(0x0000007e); /* opcode */

    writer.write_packed_guid(obj.guid);

    writer.write_u32(static_cast<uint32_t>(obj.mask));

    if ((obj.mask & GROUP_UPDATE_FLAGS_STATUS) != 0) {
        writer.write_u8(static_cast<uint8_t>(obj.status));

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_CUR_HP) != 0) {
        writer.write_u16(obj.current_health);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_MAX_HP) != 0) {
        writer.write_u16(obj.max_health);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_POWER_TYPE) != 0) {
        writer.write_u8(static_cast<uint8_t>(obj.power));

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_CUR_POWER) != 0) {
        writer.write_u16(obj.current_power);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_MAX_POWER) != 0) {
        writer.write_u16(obj.max_power);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_LEVEL) != 0) {
        writer.write_u16(obj.level);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_ZONE) != 0) {
        writer.write_u32(static_cast<uint32_t>(obj.area));

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_POSITION) != 0) {
        writer.write_u16(obj.position_x);

        writer.write_u16(obj.position_y);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_AURAS) != 0) {
        aura_mask_write(writer, obj.auras);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_AURAS_2) != 0) {
        aura_mask_write(writer, obj.negative_auras);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_GUID) != 0) {
        writer.write_u64(obj.pet);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_NAME) != 0) {
        writer.write_cstring(obj.pet_name);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_MODEL_ID) != 0) {
        writer.write_u16(obj.pet_display_id);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_CUR_HP) != 0) {
        writer.write_u16(obj.pet_current_health);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_MAX_HP) != 0) {
        writer.write_u16(obj.pet_max_health);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_POWER_TYPE) != 0) {
        writer.write_u8(static_cast<uint8_t>(obj.pet_power_type));

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_CUR_POWER) != 0) {
        writer.write_u16(obj.pet_current_power);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_MAX_POWER) != 0) {
        writer.write_u16(obj.pet_max_power);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_AURAS) != 0) {
        aura_mask_write(writer, obj.pet_auras);

    }
    return writer.m_buf;
}

static size_t SMSG_PARTY_COMMAND_RESULT_size(const SMSG_PARTY_COMMAND_RESULT& obj) {
    return 9 + obj.member.size();
}

SMSG_PARTY_COMMAND_RESULT SMSG_PARTY_COMMAND_RESULT_read(Reader& reader) {
    SMSG_PARTY_COMMAND_RESULT obj;

    obj.operation = static_cast<PartyOperation>(reader.read_u32());

    obj.member = reader.read_cstring();

    obj.result = static_cast<PartyResult>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_PARTY_COMMAND_RESULT::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_PARTY_COMMAND_RESULT_size(obj));

    writer.write_u16_be(SMSG_PARTY_COMMAND_RESULT_size(obj) + 2); /* size */

    writer.write_u16(0x0000007f); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.operation));

    writer.write_cstring(obj.member);

    writer.write_u32(static_cast<uint32_t>(obj.result));

    return writer.m_buf;
}

static size_t CMSG_GUILD_CREATE_size(const CMSG_GUILD_CREATE& obj) {
    return 1 + obj.guild_name.size();
}

CMSG_GUILD_CREATE CMSG_GUILD_CREATE_read(Reader& reader) {
    CMSG_GUILD_CREATE obj;

    obj.guild_name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_GUILD_CREATE::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_GUILD_CREATE_size(obj));

    writer.write_u16_be((uint16_t)CMSG_GUILD_CREATE_size(obj) + 4); /* size */

    writer.write_u32(0x00000081); /* opcode */

    writer.write_cstring(obj.guild_name);

    return writer.m_buf;
}

static size_t CMSG_GUILD_INVITE_size(const CMSG_GUILD_INVITE& obj) {
    return 1 + obj.invited_player.size();
}

CMSG_GUILD_INVITE CMSG_GUILD_INVITE_read(Reader& reader) {
    CMSG_GUILD_INVITE obj;

    obj.invited_player = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_GUILD_INVITE::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_GUILD_INVITE_size(obj));

    writer.write_u16_be((uint16_t)CMSG_GUILD_INVITE_size(obj) + 4); /* size */

    writer.write_u32(0x00000082); /* opcode */

    writer.write_cstring(obj.invited_player);

    return writer.m_buf;
}

static size_t SMSG_GUILD_INVITE_size(const SMSG_GUILD_INVITE& obj) {
    return 2 + obj.player_name.size() + obj.guild_name.size();
}

SMSG_GUILD_INVITE SMSG_GUILD_INVITE_read(Reader& reader) {
    SMSG_GUILD_INVITE obj;

    obj.player_name = reader.read_cstring();

    obj.guild_name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> SMSG_GUILD_INVITE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_GUILD_INVITE_size(obj));

    writer.write_u16_be(SMSG_GUILD_INVITE_size(obj) + 2); /* size */

    writer.write_u16(0x00000083); /* opcode */

    writer.write_cstring(obj.player_name);

    writer.write_cstring(obj.guild_name);

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_GUILD_ACCEPT::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x00000084); /* opcode */

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_GUILD_DECLINE::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x00000085); /* opcode */

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_GUILD_INFO::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x00000087); /* opcode */

    return writer.m_buf;
}

static size_t SMSG_GUILD_INFO_size(const SMSG_GUILD_INFO& obj) {
    return 21 + obj.guild_name.size();
}

SMSG_GUILD_INFO SMSG_GUILD_INFO_read(Reader& reader) {
    SMSG_GUILD_INFO obj;

    obj.guild_name = reader.read_cstring();

    obj.created_day = reader.read_u32();

    obj.created_month = reader.read_u32();

    obj.created_year = reader.read_u32();

    obj.amount_of_characters_in_guild = reader.read_u32();

    obj.amount_of_accounts_in_guild = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_GUILD_INFO::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_GUILD_INFO_size(obj));

    writer.write_u16_be(SMSG_GUILD_INFO_size(obj) + 2); /* size */

    writer.write_u16(0x00000088); /* opcode */

    writer.write_cstring(obj.guild_name);

    writer.write_u32(obj.created_day);

    writer.write_u32(obj.created_month);

    writer.write_u32(obj.created_year);

    writer.write_u32(obj.amount_of_characters_in_guild);

    writer.write_u32(obj.amount_of_accounts_in_guild);

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_GUILD_ROSTER::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x00000089); /* opcode */

    return writer.m_buf;
}

static size_t SMSG_GUILD_ROSTER_size(const SMSG_GUILD_ROSTER& obj) {
    size_t _size = 10 + obj.motd.size() + obj.guild_info.size() + 4 * obj.rights.size();

    for(const auto& v : obj.members) {
        _size += GuildMember_size(v);
    }

    return _size;
}

SMSG_GUILD_ROSTER SMSG_GUILD_ROSTER_read(Reader& reader) {
    SMSG_GUILD_ROSTER obj;

    obj.amount_of_members = reader.read_u32();

    obj.motd = reader.read_cstring();

    obj.guild_info = reader.read_cstring();

    obj.amount_of_rights = reader.read_u32();

    for (auto i = 0; i < obj.amount_of_rights; ++i) {
        obj.rights.push_back(reader.read_u32());
    }

    for (auto i = 0; i < obj.amount_of_members; ++i) {
        obj.members.push_back(::wow_world_messages::vanilla::GuildMember_read(reader));
    }

    return obj;
}

std::vector<unsigned char> SMSG_GUILD_ROSTER::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_GUILD_ROSTER_size(obj));

    writer.write_u16_be(SMSG_GUILD_ROSTER_size(obj) + 2); /* size */

    writer.write_u16(0x0000008a); /* opcode */

    writer.write_u32(obj.amount_of_members);

    writer.write_cstring(obj.motd);

    writer.write_cstring(obj.guild_info);

    writer.write_u32(obj.amount_of_rights);

    for (const auto& v : obj.rights) {
        writer.write_u32(v);
    }

    for (const auto& v : obj.members) {
        GuildMember_write(writer, v);
    }

    return writer.m_buf;
}

static size_t CMSG_GUILD_PROMOTE_size(const CMSG_GUILD_PROMOTE& obj) {
    return 1 + obj.player_name.size();
}

CMSG_GUILD_PROMOTE CMSG_GUILD_PROMOTE_read(Reader& reader) {
    CMSG_GUILD_PROMOTE obj;

    obj.player_name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_GUILD_PROMOTE::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_GUILD_PROMOTE_size(obj));

    writer.write_u16_be((uint16_t)CMSG_GUILD_PROMOTE_size(obj) + 4); /* size */

    writer.write_u32(0x0000008b); /* opcode */

    writer.write_cstring(obj.player_name);

    return writer.m_buf;
}

static size_t CMSG_GUILD_DEMOTE_size(const CMSG_GUILD_DEMOTE& obj) {
    return 1 + obj.player_name.size();
}

CMSG_GUILD_DEMOTE CMSG_GUILD_DEMOTE_read(Reader& reader) {
    CMSG_GUILD_DEMOTE obj;

    obj.player_name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_GUILD_DEMOTE::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_GUILD_DEMOTE_size(obj));

    writer.write_u16_be((uint16_t)CMSG_GUILD_DEMOTE_size(obj) + 4); /* size */

    writer.write_u32(0x0000008c); /* opcode */

    writer.write_cstring(obj.player_name);

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_GUILD_LEAVE::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x0000008d); /* opcode */

    return writer.m_buf;
}

static size_t CMSG_GUILD_REMOVE_size(const CMSG_GUILD_REMOVE& obj) {
    return 1 + obj.player_name.size();
}

CMSG_GUILD_REMOVE CMSG_GUILD_REMOVE_read(Reader& reader) {
    CMSG_GUILD_REMOVE obj;

    obj.player_name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_GUILD_REMOVE::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_GUILD_REMOVE_size(obj));

    writer.write_u16_be((uint16_t)CMSG_GUILD_REMOVE_size(obj) + 4); /* size */

    writer.write_u32(0x0000008e); /* opcode */

    writer.write_cstring(obj.player_name);

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_GUILD_DISBAND::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x0000008f); /* opcode */

    return writer.m_buf;
}

static size_t CMSG_GUILD_LEADER_size(const CMSG_GUILD_LEADER& obj) {
    return 1 + obj.new_guild_leader_name.size();
}

CMSG_GUILD_LEADER CMSG_GUILD_LEADER_read(Reader& reader) {
    CMSG_GUILD_LEADER obj;

    obj.new_guild_leader_name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_GUILD_LEADER::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_GUILD_LEADER_size(obj));

    writer.write_u16_be((uint16_t)CMSG_GUILD_LEADER_size(obj) + 4); /* size */

    writer.write_u32(0x00000090); /* opcode */

    writer.write_cstring(obj.new_guild_leader_name);

    return writer.m_buf;
}

static size_t CMSG_GUILD_MOTD_size(const CMSG_GUILD_MOTD& obj) {
    return 1 + obj.message_of_the_day.size();
}

CMSG_GUILD_MOTD CMSG_GUILD_MOTD_read(Reader& reader) {
    CMSG_GUILD_MOTD obj;

    obj.message_of_the_day = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_GUILD_MOTD::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_GUILD_MOTD_size(obj));

    writer.write_u16_be((uint16_t)CMSG_GUILD_MOTD_size(obj) + 4); /* size */

    writer.write_u32(0x00000091); /* opcode */

    writer.write_cstring(obj.message_of_the_day);

    return writer.m_buf;
}

static size_t SMSG_GUILD_EVENT_size(const SMSG_GUILD_EVENT& obj) {
    size_t _size = 2;

    for(const auto& v : obj.event_descriptions) {
        _size += v.size() + 1;
    }

    return _size;
}

SMSG_GUILD_EVENT SMSG_GUILD_EVENT_read(Reader& reader) {
    SMSG_GUILD_EVENT obj;

    obj.event = static_cast<GuildEvent>(reader.read_u8());

    obj.amount_of_events = reader.read_u8();

    for (auto i = 0; i < obj.amount_of_events; ++i) {
        obj.event_descriptions.push_back(reader.read_cstring());
    }

    return obj;
}

std::vector<unsigned char> SMSG_GUILD_EVENT::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_GUILD_EVENT_size(obj));

    writer.write_u16_be(SMSG_GUILD_EVENT_size(obj) + 2); /* size */

    writer.write_u16(0x00000092); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.event));

    writer.write_u8(obj.amount_of_events);

    for (const auto& v : obj.event_descriptions) {
        writer.write_cstring(v);
    }

    return writer.m_buf;
}

static size_t SMSG_GUILD_COMMAND_RESULT_size(const SMSG_GUILD_COMMAND_RESULT& obj) {
    return 9 + obj.string.size();
}

SMSG_GUILD_COMMAND_RESULT SMSG_GUILD_COMMAND_RESULT_read(Reader& reader) {
    SMSG_GUILD_COMMAND_RESULT obj;

    obj.command = static_cast<GuildCommand>(reader.read_u32());

    obj.string = reader.read_cstring();

    obj.result = static_cast<GuildCommandResult>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_GUILD_COMMAND_RESULT::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_GUILD_COMMAND_RESULT_size(obj));

    writer.write_u16_be(SMSG_GUILD_COMMAND_RESULT_size(obj) + 2); /* size */

    writer.write_u16(0x00000093); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.command));

    writer.write_cstring(obj.string);

    writer.write_u32(static_cast<uint32_t>(obj.result));

    return writer.m_buf;
}

static size_t CMSG_MESSAGECHAT_size(const CMSG_MESSAGECHAT& obj) {
    size_t _size = 9 + obj.message.size();

    if (obj.chat_type == ChatType::WHISPER) {
        _size += 1 + obj.target_player.size();
    }
    else if (obj.chat_type == ChatType::CHANNEL) {
        _size += 1 + obj.channel.size();
    }

    return _size;
}

CMSG_MESSAGECHAT CMSG_MESSAGECHAT_read(Reader& reader) {
    CMSG_MESSAGECHAT obj;

    obj.chat_type = static_cast<ChatType>(reader.read_u32());

    obj.language = static_cast<Language>(reader.read_u32());

    if (obj.chat_type == ChatType::WHISPER) {
        obj.target_player = reader.read_cstring();

    }
    else if (obj.chat_type == ChatType::CHANNEL) {
        obj.channel = reader.read_cstring();

    }
    obj.message = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_MESSAGECHAT::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_MESSAGECHAT_size(obj));

    writer.write_u16_be((uint16_t)CMSG_MESSAGECHAT_size(obj) + 4); /* size */

    writer.write_u32(0x00000095); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.chat_type));

    writer.write_u32(static_cast<uint32_t>(obj.language));

    if (obj.chat_type == ChatType::WHISPER) {
        writer.write_cstring(obj.target_player);

    }
    else if (obj.chat_type == ChatType::CHANNEL) {
        writer.write_cstring(obj.channel);

    }
    writer.write_cstring(obj.message);

    return writer.m_buf;
}

static size_t SMSG_MESSAGECHAT_size(const SMSG_MESSAGECHAT& obj) {
    size_t _size = 11 + obj.message.size();

    if (obj.chat_type == ChatType::MONSTER_WHISPER|| obj.chat_type == ChatType::RAID_BOSS_EMOTE|| obj.chat_type == ChatType::MONSTER_EMOTE) {
        _size += 13 + obj.monster_name.size();
    }
    else if (obj.chat_type == ChatType::SAY|| obj.chat_type == ChatType::PARTY|| obj.chat_type == ChatType::YELL) {
        _size += 16;
    }
    else if (obj.chat_type == ChatType::MONSTER_SAY|| obj.chat_type == ChatType::MONSTER_YELL) {
        _size += 21 + obj.sender_name.size();
    }
    else if (obj.chat_type == ChatType::CHANNEL) {
        _size += 13 + obj.channel_name.size();
    }
    else if (obj.chat_type == ChatType::RAID|| obj.chat_type == ChatType::GUILD|| obj.chat_type == ChatType::OFFICER|| obj.chat_type == ChatType::WHISPER|| obj.chat_type == ChatType::WHISPER_INFORM|| obj.chat_type == ChatType::EMOTE|| obj.chat_type == ChatType::TEXT_EMOTE|| obj.chat_type == ChatType::SYSTEM|| obj.chat_type == ChatType::CHANNEL_JOIN|| obj.chat_type == ChatType::CHANNEL_LEAVE|| obj.chat_type == ChatType::CHANNEL_LIST|| obj.chat_type == ChatType::CHANNEL_NOTICE|| obj.chat_type == ChatType::CHANNEL_NOTICE_USER|| obj.chat_type == ChatType::AFK|| obj.chat_type == ChatType::DND|| obj.chat_type == ChatType::IGNORED|| obj.chat_type == ChatType::SKILL|| obj.chat_type == ChatType::LOOT|| obj.chat_type == ChatType::BG_SYSTEM_NEUTRAL|| obj.chat_type == ChatType::BG_SYSTEM_ALLIANCE|| obj.chat_type == ChatType::BG_SYSTEM_HORDE|| obj.chat_type == ChatType::RAID_LEADER|| obj.chat_type == ChatType::RAID_WARNING|| obj.chat_type == ChatType::RAID_BOSS_WHISPER|| obj.chat_type == ChatType::BATTLEGROUND|| obj.chat_type == ChatType::BATTLEGROUND_LEADER) {
        _size += 8;
    }

    return _size;
}

SMSG_MESSAGECHAT SMSG_MESSAGECHAT_read(Reader& reader) {
    SMSG_MESSAGECHAT obj;

    obj.chat_type = static_cast<ChatType>(reader.read_u8());

    obj.language = static_cast<Language>(reader.read_u32());

    if (obj.chat_type == ChatType::MONSTER_WHISPER|| obj.chat_type == ChatType::RAID_BOSS_EMOTE|| obj.chat_type == ChatType::MONSTER_EMOTE) {
        obj.monster_name = reader.read_sized_cstring();

        obj.monster = reader.read_u64();

    }
    else if (obj.chat_type == ChatType::SAY|| obj.chat_type == ChatType::PARTY|| obj.chat_type == ChatType::YELL) {
        obj.speech_bubble_credit = reader.read_u64();

        obj.chat_credit = reader.read_u64();

    }
    else if (obj.chat_type == ChatType::MONSTER_SAY|| obj.chat_type == ChatType::MONSTER_YELL) {
        obj.sender1 = reader.read_u64();

        obj.sender_name = reader.read_sized_cstring();

        obj.target = reader.read_u64();

    }
    else if (obj.chat_type == ChatType::CHANNEL) {
        obj.channel_name = reader.read_cstring();

        obj.player_rank = reader.read_u32();

        obj.player = reader.read_u64();

    }
    else if (obj.chat_type == ChatType::RAID|| obj.chat_type == ChatType::GUILD|| obj.chat_type == ChatType::OFFICER|| obj.chat_type == ChatType::WHISPER|| obj.chat_type == ChatType::WHISPER_INFORM|| obj.chat_type == ChatType::EMOTE|| obj.chat_type == ChatType::TEXT_EMOTE|| obj.chat_type == ChatType::SYSTEM|| obj.chat_type == ChatType::CHANNEL_JOIN|| obj.chat_type == ChatType::CHANNEL_LEAVE|| obj.chat_type == ChatType::CHANNEL_LIST|| obj.chat_type == ChatType::CHANNEL_NOTICE|| obj.chat_type == ChatType::CHANNEL_NOTICE_USER|| obj.chat_type == ChatType::AFK|| obj.chat_type == ChatType::DND|| obj.chat_type == ChatType::IGNORED|| obj.chat_type == ChatType::SKILL|| obj.chat_type == ChatType::LOOT|| obj.chat_type == ChatType::BG_SYSTEM_NEUTRAL|| obj.chat_type == ChatType::BG_SYSTEM_ALLIANCE|| obj.chat_type == ChatType::BG_SYSTEM_HORDE|| obj.chat_type == ChatType::RAID_LEADER|| obj.chat_type == ChatType::RAID_WARNING|| obj.chat_type == ChatType::RAID_BOSS_WHISPER|| obj.chat_type == ChatType::BATTLEGROUND|| obj.chat_type == ChatType::BATTLEGROUND_LEADER) {
        obj.sender2 = reader.read_u64();

    }
    obj.message = reader.read_sized_cstring();

    obj.tag = static_cast<PlayerChatTag>(reader.read_u8());

    return obj;
}

std::vector<unsigned char> SMSG_MESSAGECHAT::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_MESSAGECHAT_size(obj));

    writer.write_u16_be(SMSG_MESSAGECHAT_size(obj) + 2); /* size */

    writer.write_u16(0x00000096); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.chat_type));

    writer.write_u32(static_cast<uint32_t>(obj.language));

    if (obj.chat_type == ChatType::MONSTER_WHISPER|| obj.chat_type == ChatType::RAID_BOSS_EMOTE|| obj.chat_type == ChatType::MONSTER_EMOTE) {
        writer.write_sized_cstring(obj.monster_name);

        writer.write_u64(obj.monster);

    }
    else if (obj.chat_type == ChatType::SAY|| obj.chat_type == ChatType::PARTY|| obj.chat_type == ChatType::YELL) {
        writer.write_u64(obj.speech_bubble_credit);

        writer.write_u64(obj.chat_credit);

    }
    else if (obj.chat_type == ChatType::MONSTER_SAY|| obj.chat_type == ChatType::MONSTER_YELL) {
        writer.write_u64(obj.sender1);

        writer.write_sized_cstring(obj.sender_name);

        writer.write_u64(obj.target);

    }
    else if (obj.chat_type == ChatType::CHANNEL) {
        writer.write_cstring(obj.channel_name);

        writer.write_u32(obj.player_rank);

        writer.write_u64(obj.player);

    }
    else if (obj.chat_type == ChatType::RAID|| obj.chat_type == ChatType::GUILD|| obj.chat_type == ChatType::OFFICER|| obj.chat_type == ChatType::WHISPER|| obj.chat_type == ChatType::WHISPER_INFORM|| obj.chat_type == ChatType::EMOTE|| obj.chat_type == ChatType::TEXT_EMOTE|| obj.chat_type == ChatType::SYSTEM|| obj.chat_type == ChatType::CHANNEL_JOIN|| obj.chat_type == ChatType::CHANNEL_LEAVE|| obj.chat_type == ChatType::CHANNEL_LIST|| obj.chat_type == ChatType::CHANNEL_NOTICE|| obj.chat_type == ChatType::CHANNEL_NOTICE_USER|| obj.chat_type == ChatType::AFK|| obj.chat_type == ChatType::DND|| obj.chat_type == ChatType::IGNORED|| obj.chat_type == ChatType::SKILL|| obj.chat_type == ChatType::LOOT|| obj.chat_type == ChatType::BG_SYSTEM_NEUTRAL|| obj.chat_type == ChatType::BG_SYSTEM_ALLIANCE|| obj.chat_type == ChatType::BG_SYSTEM_HORDE|| obj.chat_type == ChatType::RAID_LEADER|| obj.chat_type == ChatType::RAID_WARNING|| obj.chat_type == ChatType::RAID_BOSS_WHISPER|| obj.chat_type == ChatType::BATTLEGROUND|| obj.chat_type == ChatType::BATTLEGROUND_LEADER) {
        writer.write_u64(obj.sender2);

    }
    writer.write_sized_cstring(obj.message);

    writer.write_u8(static_cast<uint8_t>(obj.tag));

    return writer.m_buf;
}

static size_t CMSG_JOIN_CHANNEL_size(const CMSG_JOIN_CHANNEL& obj) {
    return 2 + obj.channel_name.size() + obj.channel_password.size();
}

CMSG_JOIN_CHANNEL CMSG_JOIN_CHANNEL_read(Reader& reader) {
    CMSG_JOIN_CHANNEL obj;

    obj.channel_name = reader.read_cstring();

    obj.channel_password = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_JOIN_CHANNEL::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_JOIN_CHANNEL_size(obj));

    writer.write_u16_be((uint16_t)CMSG_JOIN_CHANNEL_size(obj) + 4); /* size */

    writer.write_u32(0x00000097); /* opcode */

    writer.write_cstring(obj.channel_name);

    writer.write_cstring(obj.channel_password);

    return writer.m_buf;
}

static size_t CMSG_LEAVE_CHANNEL_size(const CMSG_LEAVE_CHANNEL& obj) {
    return 1 + obj.channel_name.size();
}

CMSG_LEAVE_CHANNEL CMSG_LEAVE_CHANNEL_read(Reader& reader) {
    CMSG_LEAVE_CHANNEL obj;

    obj.channel_name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_LEAVE_CHANNEL::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_LEAVE_CHANNEL_size(obj));

    writer.write_u16_be((uint16_t)CMSG_LEAVE_CHANNEL_size(obj) + 4); /* size */

    writer.write_u32(0x00000098); /* opcode */

    writer.write_cstring(obj.channel_name);

    return writer.m_buf;
}

static size_t SMSG_CHANNEL_NOTIFY_size(const SMSG_CHANNEL_NOTIFY& obj) {
    return 2 + obj.channel_name.size();
}

SMSG_CHANNEL_NOTIFY SMSG_CHANNEL_NOTIFY_read(Reader& reader) {
    SMSG_CHANNEL_NOTIFY obj;

    obj.notify_type = static_cast<ChatNotify>(reader.read_u8());

    obj.channel_name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> SMSG_CHANNEL_NOTIFY::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_CHANNEL_NOTIFY_size(obj));

    writer.write_u16_be(SMSG_CHANNEL_NOTIFY_size(obj) + 2); /* size */

    writer.write_u16(0x00000099); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.notify_type));

    writer.write_cstring(obj.channel_name);

    return writer.m_buf;
}

static size_t CMSG_CHANNEL_LIST_size(const CMSG_CHANNEL_LIST& obj) {
    return 1 + obj.channel_name.size();
}

CMSG_CHANNEL_LIST CMSG_CHANNEL_LIST_read(Reader& reader) {
    CMSG_CHANNEL_LIST obj;

    obj.channel_name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_CHANNEL_LIST::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_CHANNEL_LIST_size(obj));

    writer.write_u16_be((uint16_t)CMSG_CHANNEL_LIST_size(obj) + 4); /* size */

    writer.write_u32(0x0000009a); /* opcode */

    writer.write_cstring(obj.channel_name);

    return writer.m_buf;
}

static size_t SMSG_CHANNEL_LIST_size(const SMSG_CHANNEL_LIST& obj) {
    return 6 + obj.channel_name.size() + 9 * obj.members.size();
}

SMSG_CHANNEL_LIST SMSG_CHANNEL_LIST_read(Reader& reader) {
    SMSG_CHANNEL_LIST obj;

    obj.channel_name = reader.read_cstring();

    obj.channel_flags = static_cast<ChannelFlags>(reader.read_u8());

    obj.amount_of_members = reader.read_u32();

    for (auto i = 0; i < obj.amount_of_members; ++i) {
        obj.members.push_back(::wow_world_messages::vanilla::ChannelMember_read(reader));
    }

    return obj;
}

std::vector<unsigned char> SMSG_CHANNEL_LIST::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_CHANNEL_LIST_size(obj));

    writer.write_u16_be(SMSG_CHANNEL_LIST_size(obj) + 2); /* size */

    writer.write_u16(0x0000009b); /* opcode */

    writer.write_cstring(obj.channel_name);

    writer.write_u8(static_cast<uint8_t>(obj.channel_flags));

    writer.write_u32(obj.amount_of_members);

    for (const auto& v : obj.members) {
        ChannelMember_write(writer, v);
    }

    return writer.m_buf;
}

static size_t CMSG_CHANNEL_PASSWORD_size(const CMSG_CHANNEL_PASSWORD& obj) {
    return 2 + obj.channel_name.size() + obj.channel_password.size();
}

CMSG_CHANNEL_PASSWORD CMSG_CHANNEL_PASSWORD_read(Reader& reader) {
    CMSG_CHANNEL_PASSWORD obj;

    obj.channel_name = reader.read_cstring();

    obj.channel_password = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_CHANNEL_PASSWORD::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_CHANNEL_PASSWORD_size(obj));

    writer.write_u16_be((uint16_t)CMSG_CHANNEL_PASSWORD_size(obj) + 4); /* size */

    writer.write_u32(0x0000009c); /* opcode */

    writer.write_cstring(obj.channel_name);

    writer.write_cstring(obj.channel_password);

    return writer.m_buf;
}

static size_t CMSG_CHANNEL_SET_OWNER_size(const CMSG_CHANNEL_SET_OWNER& obj) {
    return 2 + obj.channel_name.size() + obj.new_owner.size();
}

CMSG_CHANNEL_SET_OWNER CMSG_CHANNEL_SET_OWNER_read(Reader& reader) {
    CMSG_CHANNEL_SET_OWNER obj;

    obj.channel_name = reader.read_cstring();

    obj.new_owner = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_CHANNEL_SET_OWNER::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_CHANNEL_SET_OWNER_size(obj));

    writer.write_u16_be((uint16_t)CMSG_CHANNEL_SET_OWNER_size(obj) + 4); /* size */

    writer.write_u32(0x0000009d); /* opcode */

    writer.write_cstring(obj.channel_name);

    writer.write_cstring(obj.new_owner);

    return writer.m_buf;
}

static size_t CMSG_CHANNEL_OWNER_size(const CMSG_CHANNEL_OWNER& obj) {
    return 1 + obj.channel_name.size();
}

CMSG_CHANNEL_OWNER CMSG_CHANNEL_OWNER_read(Reader& reader) {
    CMSG_CHANNEL_OWNER obj;

    obj.channel_name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_CHANNEL_OWNER::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_CHANNEL_OWNER_size(obj));

    writer.write_u16_be((uint16_t)CMSG_CHANNEL_OWNER_size(obj) + 4); /* size */

    writer.write_u32(0x0000009e); /* opcode */

    writer.write_cstring(obj.channel_name);

    return writer.m_buf;
}

static size_t CMSG_CHANNEL_MODERATOR_size(const CMSG_CHANNEL_MODERATOR& obj) {
    return 2 + obj.channel_name.size() + obj.player_name.size();
}

CMSG_CHANNEL_MODERATOR CMSG_CHANNEL_MODERATOR_read(Reader& reader) {
    CMSG_CHANNEL_MODERATOR obj;

    obj.channel_name = reader.read_cstring();

    obj.player_name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_CHANNEL_MODERATOR::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_CHANNEL_MODERATOR_size(obj));

    writer.write_u16_be((uint16_t)CMSG_CHANNEL_MODERATOR_size(obj) + 4); /* size */

    writer.write_u32(0x0000009f); /* opcode */

    writer.write_cstring(obj.channel_name);

    writer.write_cstring(obj.player_name);

    return writer.m_buf;
}

static size_t CMSG_CHANNEL_UNMODERATOR_size(const CMSG_CHANNEL_UNMODERATOR& obj) {
    return 2 + obj.channel_name.size() + obj.player_name.size();
}

CMSG_CHANNEL_UNMODERATOR CMSG_CHANNEL_UNMODERATOR_read(Reader& reader) {
    CMSG_CHANNEL_UNMODERATOR obj;

    obj.channel_name = reader.read_cstring();

    obj.player_name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_CHANNEL_UNMODERATOR::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_CHANNEL_UNMODERATOR_size(obj));

    writer.write_u16_be((uint16_t)CMSG_CHANNEL_UNMODERATOR_size(obj) + 4); /* size */

    writer.write_u32(0x000000a0); /* opcode */

    writer.write_cstring(obj.channel_name);

    writer.write_cstring(obj.player_name);

    return writer.m_buf;
}

static size_t CMSG_CHANNEL_MUTE_size(const CMSG_CHANNEL_MUTE& obj) {
    return 2 + obj.channel_name.size() + obj.player_name.size();
}

CMSG_CHANNEL_MUTE CMSG_CHANNEL_MUTE_read(Reader& reader) {
    CMSG_CHANNEL_MUTE obj;

    obj.channel_name = reader.read_cstring();

    obj.player_name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_CHANNEL_MUTE::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_CHANNEL_MUTE_size(obj));

    writer.write_u16_be((uint16_t)CMSG_CHANNEL_MUTE_size(obj) + 4); /* size */

    writer.write_u32(0x000000a1); /* opcode */

    writer.write_cstring(obj.channel_name);

    writer.write_cstring(obj.player_name);

    return writer.m_buf;
}

static size_t CMSG_CHANNEL_UNMUTE_size(const CMSG_CHANNEL_UNMUTE& obj) {
    return 2 + obj.channel_name.size() + obj.player_name.size();
}

CMSG_CHANNEL_UNMUTE CMSG_CHANNEL_UNMUTE_read(Reader& reader) {
    CMSG_CHANNEL_UNMUTE obj;

    obj.channel_name = reader.read_cstring();

    obj.player_name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_CHANNEL_UNMUTE::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_CHANNEL_UNMUTE_size(obj));

    writer.write_u16_be((uint16_t)CMSG_CHANNEL_UNMUTE_size(obj) + 4); /* size */

    writer.write_u32(0x000000a2); /* opcode */

    writer.write_cstring(obj.channel_name);

    writer.write_cstring(obj.player_name);

    return writer.m_buf;
}

static size_t CMSG_CHANNEL_INVITE_size(const CMSG_CHANNEL_INVITE& obj) {
    return 2 + obj.channel_name.size() + obj.player_name.size();
}

CMSG_CHANNEL_INVITE CMSG_CHANNEL_INVITE_read(Reader& reader) {
    CMSG_CHANNEL_INVITE obj;

    obj.channel_name = reader.read_cstring();

    obj.player_name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_CHANNEL_INVITE::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_CHANNEL_INVITE_size(obj));

    writer.write_u16_be((uint16_t)CMSG_CHANNEL_INVITE_size(obj) + 4); /* size */

    writer.write_u32(0x000000a3); /* opcode */

    writer.write_cstring(obj.channel_name);

    writer.write_cstring(obj.player_name);

    return writer.m_buf;
}

static size_t CMSG_CHANNEL_KICK_size(const CMSG_CHANNEL_KICK& obj) {
    return 2 + obj.channel_name.size() + obj.player_name.size();
}

CMSG_CHANNEL_KICK CMSG_CHANNEL_KICK_read(Reader& reader) {
    CMSG_CHANNEL_KICK obj;

    obj.channel_name = reader.read_cstring();

    obj.player_name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_CHANNEL_KICK::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_CHANNEL_KICK_size(obj));

    writer.write_u16_be((uint16_t)CMSG_CHANNEL_KICK_size(obj) + 4); /* size */

    writer.write_u32(0x000000a4); /* opcode */

    writer.write_cstring(obj.channel_name);

    writer.write_cstring(obj.player_name);

    return writer.m_buf;
}

static size_t CMSG_CHANNEL_BAN_size(const CMSG_CHANNEL_BAN& obj) {
    return 2 + obj.channel_name.size() + obj.player_name.size();
}

CMSG_CHANNEL_BAN CMSG_CHANNEL_BAN_read(Reader& reader) {
    CMSG_CHANNEL_BAN obj;

    obj.channel_name = reader.read_cstring();

    obj.player_name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_CHANNEL_BAN::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_CHANNEL_BAN_size(obj));

    writer.write_u16_be((uint16_t)CMSG_CHANNEL_BAN_size(obj) + 4); /* size */

    writer.write_u32(0x000000a5); /* opcode */

    writer.write_cstring(obj.channel_name);

    writer.write_cstring(obj.player_name);

    return writer.m_buf;
}

static size_t CMSG_CHANNEL_UNBAN_size(const CMSG_CHANNEL_UNBAN& obj) {
    return 2 + obj.channel_name.size() + obj.player_name.size();
}

CMSG_CHANNEL_UNBAN CMSG_CHANNEL_UNBAN_read(Reader& reader) {
    CMSG_CHANNEL_UNBAN obj;

    obj.channel_name = reader.read_cstring();

    obj.player_name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_CHANNEL_UNBAN::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_CHANNEL_UNBAN_size(obj));

    writer.write_u16_be((uint16_t)CMSG_CHANNEL_UNBAN_size(obj) + 4); /* size */

    writer.write_u32(0x000000a6); /* opcode */

    writer.write_cstring(obj.channel_name);

    writer.write_cstring(obj.player_name);

    return writer.m_buf;
}

static size_t CMSG_CHANNEL_ANNOUNCEMENTS_size(const CMSG_CHANNEL_ANNOUNCEMENTS& obj) {
    return 1 + obj.channel_name.size();
}

CMSG_CHANNEL_ANNOUNCEMENTS CMSG_CHANNEL_ANNOUNCEMENTS_read(Reader& reader) {
    CMSG_CHANNEL_ANNOUNCEMENTS obj;

    obj.channel_name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_CHANNEL_ANNOUNCEMENTS::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_CHANNEL_ANNOUNCEMENTS_size(obj));

    writer.write_u16_be((uint16_t)CMSG_CHANNEL_ANNOUNCEMENTS_size(obj) + 4); /* size */

    writer.write_u32(0x000000a7); /* opcode */

    writer.write_cstring(obj.channel_name);

    return writer.m_buf;
}

static size_t CMSG_CHANNEL_MODERATE_size(const CMSG_CHANNEL_MODERATE& obj) {
    return 1 + obj.channel_name.size();
}

CMSG_CHANNEL_MODERATE CMSG_CHANNEL_MODERATE_read(Reader& reader) {
    CMSG_CHANNEL_MODERATE obj;

    obj.channel_name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_CHANNEL_MODERATE::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_CHANNEL_MODERATE_size(obj));

    writer.write_u16_be((uint16_t)CMSG_CHANNEL_MODERATE_size(obj) + 4); /* size */

    writer.write_u32(0x000000a8); /* opcode */

    writer.write_cstring(obj.channel_name);

    return writer.m_buf;
}

static size_t SMSG_UPDATE_OBJECT_size(const SMSG_UPDATE_OBJECT& obj) {
    size_t _size = 5;

    for(const auto& v : obj.objects) {
        _size += Object_size(v);
    }

    return _size;
}

SMSG_UPDATE_OBJECT SMSG_UPDATE_OBJECT_read(Reader& reader) {
    SMSG_UPDATE_OBJECT obj;

    obj.amount_of_objects = reader.read_u32();

    obj.has_transport = reader.read_u8();

    for (auto i = 0; i < obj.amount_of_objects; ++i) {
        obj.objects.push_back(::wow_world_messages::vanilla::Object_read(reader));
    }

    return obj;
}

std::vector<unsigned char> SMSG_UPDATE_OBJECT::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_UPDATE_OBJECT_size(obj));

    writer.write_u16_be(SMSG_UPDATE_OBJECT_size(obj) + 2); /* size */

    writer.write_u16(0x000000a9); /* opcode */

    writer.write_u32(obj.amount_of_objects);

    writer.write_u8(obj.has_transport);

    for (const auto& v : obj.objects) {
        Object_write(writer, v);
    }

    return writer.m_buf;
}

SMSG_DESTROY_OBJECT SMSG_DESTROY_OBJECT_read(Reader& reader) {
    SMSG_DESTROY_OBJECT obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> SMSG_DESTROY_OBJECT::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 2); /* size */

    writer.write_u16(0x000000aa); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

static size_t CMSG_USE_ITEM_size(const CMSG_USE_ITEM& obj) {
    return 3 + SpellCastTargets_size(obj.targets);
}

CMSG_USE_ITEM CMSG_USE_ITEM_read(Reader& reader) {
    CMSG_USE_ITEM obj;

    obj.bag_index = reader.read_u8();

    obj.bag_slot = reader.read_u8();

    obj.spell_index = reader.read_u8();

    obj.targets = ::wow_world_messages::vanilla::SpellCastTargets_read(reader);

    return obj;
}

std::vector<unsigned char> CMSG_USE_ITEM::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_USE_ITEM_size(obj));

    writer.write_u16_be((uint16_t)CMSG_USE_ITEM_size(obj) + 4); /* size */

    writer.write_u32(0x000000ab); /* opcode */

    writer.write_u8(obj.bag_index);

    writer.write_u8(obj.bag_slot);

    writer.write_u8(obj.spell_index);

    SpellCastTargets_write(writer, obj.targets);

    return writer.m_buf;
}

CMSG_OPEN_ITEM CMSG_OPEN_ITEM_read(Reader& reader) {
    CMSG_OPEN_ITEM obj;

    obj.bag_index = reader.read_u8();

    obj.slot = reader.read_u8();

    return obj;
}

std::vector<unsigned char> CMSG_OPEN_ITEM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0002);

    writer.write_u16_be(0x0002 + 4); /* size */

    writer.write_u32(0x000000ac); /* opcode */

    writer.write_u8(obj.bag_index);

    writer.write_u8(obj.slot);

    return writer.m_buf;
}

CMSG_READ_ITEM CMSG_READ_ITEM_read(Reader& reader) {
    CMSG_READ_ITEM obj;

    obj.bag_index = reader.read_u8();

    obj.slot = reader.read_u8();

    return obj;
}

std::vector<unsigned char> CMSG_READ_ITEM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0002);

    writer.write_u16_be(0x0002 + 4); /* size */

    writer.write_u32(0x000000ad); /* opcode */

    writer.write_u8(obj.bag_index);

    writer.write_u8(obj.slot);

    return writer.m_buf;
}

SMSG_READ_ITEM_OK SMSG_READ_ITEM_OK_read(Reader& reader) {
    SMSG_READ_ITEM_OK obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> SMSG_READ_ITEM_OK::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 2); /* size */

    writer.write_u16(0x000000ae); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

SMSG_READ_ITEM_FAILED SMSG_READ_ITEM_FAILED_read(Reader& reader) {
    SMSG_READ_ITEM_FAILED obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> SMSG_READ_ITEM_FAILED::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 2); /* size */

    writer.write_u16(0x000000af); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

SMSG_ITEM_COOLDOWN SMSG_ITEM_COOLDOWN_read(Reader& reader) {
    SMSG_ITEM_COOLDOWN obj;

    obj.guid = reader.read_u64();

    obj.id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_ITEM_COOLDOWN::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 2); /* size */

    writer.write_u16(0x000000b0); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.id);

    return writer.m_buf;
}

CMSG_GAMEOBJ_USE CMSG_GAMEOBJ_USE_read(Reader& reader) {
    CMSG_GAMEOBJ_USE obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_GAMEOBJ_USE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x000000b1); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

SMSG_GAMEOBJECT_CUSTOM_ANIM SMSG_GAMEOBJECT_CUSTOM_ANIM_read(Reader& reader) {
    SMSG_GAMEOBJECT_CUSTOM_ANIM obj;

    obj.guid = reader.read_u64();

    obj.animation_id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_GAMEOBJECT_CUSTOM_ANIM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 2); /* size */

    writer.write_u16(0x000000b3); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.animation_id);

    return writer.m_buf;
}

CMSG_AREATRIGGER CMSG_AREATRIGGER_read(Reader& reader) {
    CMSG_AREATRIGGER obj;

    obj.trigger_id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_AREATRIGGER::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 4); /* size */

    writer.write_u32(0x000000b4); /* opcode */

    writer.write_u32(obj.trigger_id);

    return writer.m_buf;
}

static size_t MSG_MOVE_START_FORWARD_Client_size(const MSG_MOVE_START_FORWARD_Client& obj) {
    return 0 + MovementInfo_size(obj.info);
}

MSG_MOVE_START_FORWARD_Client MSG_MOVE_START_FORWARD_Client_read(Reader& reader) {
    MSG_MOVE_START_FORWARD_Client obj;

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_START_FORWARD_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_START_FORWARD_Client_size(obj));

    writer.write_u16_be((uint16_t)MSG_MOVE_START_FORWARD_Client_size(obj) + 4); /* size */

    writer.write_u32(0x000000b5); /* opcode */

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_START_FORWARD_Server_size(const MSG_MOVE_START_FORWARD_Server& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid) + MovementInfo_size(obj.info);
}

MSG_MOVE_START_FORWARD_Server MSG_MOVE_START_FORWARD_Server_read(Reader& reader) {
    MSG_MOVE_START_FORWARD_Server obj;

    obj.guid = reader.read_packed_guid();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_START_FORWARD_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_START_FORWARD_Server_size(obj));

    writer.write_u16_be(MSG_MOVE_START_FORWARD_Server_size(obj) + 2); /* size */

    writer.write_u16(0x000000b5); /* opcode */

    writer.write_packed_guid(obj.guid);

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_START_BACKWARD_Client_size(const MSG_MOVE_START_BACKWARD_Client& obj) {
    return 0 + MovementInfo_size(obj.info);
}

MSG_MOVE_START_BACKWARD_Client MSG_MOVE_START_BACKWARD_Client_read(Reader& reader) {
    MSG_MOVE_START_BACKWARD_Client obj;

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_START_BACKWARD_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_START_BACKWARD_Client_size(obj));

    writer.write_u16_be((uint16_t)MSG_MOVE_START_BACKWARD_Client_size(obj) + 4); /* size */

    writer.write_u32(0x000000b6); /* opcode */

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_START_BACKWARD_Server_size(const MSG_MOVE_START_BACKWARD_Server& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid) + MovementInfo_size(obj.info);
}

MSG_MOVE_START_BACKWARD_Server MSG_MOVE_START_BACKWARD_Server_read(Reader& reader) {
    MSG_MOVE_START_BACKWARD_Server obj;

    obj.guid = reader.read_packed_guid();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_START_BACKWARD_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_START_BACKWARD_Server_size(obj));

    writer.write_u16_be(MSG_MOVE_START_BACKWARD_Server_size(obj) + 2); /* size */

    writer.write_u16(0x000000b6); /* opcode */

    writer.write_packed_guid(obj.guid);

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_STOP_Client_size(const MSG_MOVE_STOP_Client& obj) {
    return 0 + MovementInfo_size(obj.info);
}

MSG_MOVE_STOP_Client MSG_MOVE_STOP_Client_read(Reader& reader) {
    MSG_MOVE_STOP_Client obj;

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_STOP_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_STOP_Client_size(obj));

    writer.write_u16_be((uint16_t)MSG_MOVE_STOP_Client_size(obj) + 4); /* size */

    writer.write_u32(0x000000b7); /* opcode */

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_STOP_Server_size(const MSG_MOVE_STOP_Server& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid) + MovementInfo_size(obj.info);
}

MSG_MOVE_STOP_Server MSG_MOVE_STOP_Server_read(Reader& reader) {
    MSG_MOVE_STOP_Server obj;

    obj.guid = reader.read_packed_guid();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_STOP_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_STOP_Server_size(obj));

    writer.write_u16_be(MSG_MOVE_STOP_Server_size(obj) + 2); /* size */

    writer.write_u16(0x000000b7); /* opcode */

    writer.write_packed_guid(obj.guid);

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_START_STRAFE_LEFT_Client_size(const MSG_MOVE_START_STRAFE_LEFT_Client& obj) {
    return 0 + MovementInfo_size(obj.info);
}

MSG_MOVE_START_STRAFE_LEFT_Client MSG_MOVE_START_STRAFE_LEFT_Client_read(Reader& reader) {
    MSG_MOVE_START_STRAFE_LEFT_Client obj;

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_START_STRAFE_LEFT_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_START_STRAFE_LEFT_Client_size(obj));

    writer.write_u16_be((uint16_t)MSG_MOVE_START_STRAFE_LEFT_Client_size(obj) + 4); /* size */

    writer.write_u32(0x000000b8); /* opcode */

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_START_STRAFE_LEFT_Server_size(const MSG_MOVE_START_STRAFE_LEFT_Server& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid) + MovementInfo_size(obj.info);
}

MSG_MOVE_START_STRAFE_LEFT_Server MSG_MOVE_START_STRAFE_LEFT_Server_read(Reader& reader) {
    MSG_MOVE_START_STRAFE_LEFT_Server obj;

    obj.guid = reader.read_packed_guid();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_START_STRAFE_LEFT_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_START_STRAFE_LEFT_Server_size(obj));

    writer.write_u16_be(MSG_MOVE_START_STRAFE_LEFT_Server_size(obj) + 2); /* size */

    writer.write_u16(0x000000b8); /* opcode */

    writer.write_packed_guid(obj.guid);

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_START_STRAFE_RIGHT_Client_size(const MSG_MOVE_START_STRAFE_RIGHT_Client& obj) {
    return 0 + MovementInfo_size(obj.info);
}

MSG_MOVE_START_STRAFE_RIGHT_Client MSG_MOVE_START_STRAFE_RIGHT_Client_read(Reader& reader) {
    MSG_MOVE_START_STRAFE_RIGHT_Client obj;

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_START_STRAFE_RIGHT_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_START_STRAFE_RIGHT_Client_size(obj));

    writer.write_u16_be((uint16_t)MSG_MOVE_START_STRAFE_RIGHT_Client_size(obj) + 4); /* size */

    writer.write_u32(0x000000b9); /* opcode */

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_START_STRAFE_RIGHT_Server_size(const MSG_MOVE_START_STRAFE_RIGHT_Server& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid) + MovementInfo_size(obj.info);
}

MSG_MOVE_START_STRAFE_RIGHT_Server MSG_MOVE_START_STRAFE_RIGHT_Server_read(Reader& reader) {
    MSG_MOVE_START_STRAFE_RIGHT_Server obj;

    obj.guid = reader.read_packed_guid();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_START_STRAFE_RIGHT_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_START_STRAFE_RIGHT_Server_size(obj));

    writer.write_u16_be(MSG_MOVE_START_STRAFE_RIGHT_Server_size(obj) + 2); /* size */

    writer.write_u16(0x000000b9); /* opcode */

    writer.write_packed_guid(obj.guid);

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_STOP_STRAFE_Client_size(const MSG_MOVE_STOP_STRAFE_Client& obj) {
    return 0 + MovementInfo_size(obj.info);
}

MSG_MOVE_STOP_STRAFE_Client MSG_MOVE_STOP_STRAFE_Client_read(Reader& reader) {
    MSG_MOVE_STOP_STRAFE_Client obj;

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_STOP_STRAFE_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_STOP_STRAFE_Client_size(obj));

    writer.write_u16_be((uint16_t)MSG_MOVE_STOP_STRAFE_Client_size(obj) + 4); /* size */

    writer.write_u32(0x000000ba); /* opcode */

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_STOP_STRAFE_Server_size(const MSG_MOVE_STOP_STRAFE_Server& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid) + MovementInfo_size(obj.info);
}

MSG_MOVE_STOP_STRAFE_Server MSG_MOVE_STOP_STRAFE_Server_read(Reader& reader) {
    MSG_MOVE_STOP_STRAFE_Server obj;

    obj.guid = reader.read_packed_guid();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_STOP_STRAFE_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_STOP_STRAFE_Server_size(obj));

    writer.write_u16_be(MSG_MOVE_STOP_STRAFE_Server_size(obj) + 2); /* size */

    writer.write_u16(0x000000ba); /* opcode */

    writer.write_packed_guid(obj.guid);

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_JUMP_Client_size(const MSG_MOVE_JUMP_Client& obj) {
    return 0 + MovementInfo_size(obj.info);
}

MSG_MOVE_JUMP_Client MSG_MOVE_JUMP_Client_read(Reader& reader) {
    MSG_MOVE_JUMP_Client obj;

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_JUMP_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_JUMP_Client_size(obj));

    writer.write_u16_be((uint16_t)MSG_MOVE_JUMP_Client_size(obj) + 4); /* size */

    writer.write_u32(0x000000bb); /* opcode */

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_JUMP_Server_size(const MSG_MOVE_JUMP_Server& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid) + MovementInfo_size(obj.info);
}

MSG_MOVE_JUMP_Server MSG_MOVE_JUMP_Server_read(Reader& reader) {
    MSG_MOVE_JUMP_Server obj;

    obj.guid = reader.read_packed_guid();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_JUMP_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_JUMP_Server_size(obj));

    writer.write_u16_be(MSG_MOVE_JUMP_Server_size(obj) + 2); /* size */

    writer.write_u16(0x000000bb); /* opcode */

    writer.write_packed_guid(obj.guid);

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_START_TURN_LEFT_Client_size(const MSG_MOVE_START_TURN_LEFT_Client& obj) {
    return 0 + MovementInfo_size(obj.info);
}

MSG_MOVE_START_TURN_LEFT_Client MSG_MOVE_START_TURN_LEFT_Client_read(Reader& reader) {
    MSG_MOVE_START_TURN_LEFT_Client obj;

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_START_TURN_LEFT_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_START_TURN_LEFT_Client_size(obj));

    writer.write_u16_be((uint16_t)MSG_MOVE_START_TURN_LEFT_Client_size(obj) + 4); /* size */

    writer.write_u32(0x000000bc); /* opcode */

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_START_TURN_LEFT_Server_size(const MSG_MOVE_START_TURN_LEFT_Server& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid) + MovementInfo_size(obj.info);
}

MSG_MOVE_START_TURN_LEFT_Server MSG_MOVE_START_TURN_LEFT_Server_read(Reader& reader) {
    MSG_MOVE_START_TURN_LEFT_Server obj;

    obj.guid = reader.read_packed_guid();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_START_TURN_LEFT_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_START_TURN_LEFT_Server_size(obj));

    writer.write_u16_be(MSG_MOVE_START_TURN_LEFT_Server_size(obj) + 2); /* size */

    writer.write_u16(0x000000bc); /* opcode */

    writer.write_packed_guid(obj.guid);

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_START_TURN_RIGHT_Client_size(const MSG_MOVE_START_TURN_RIGHT_Client& obj) {
    return 0 + MovementInfo_size(obj.info);
}

MSG_MOVE_START_TURN_RIGHT_Client MSG_MOVE_START_TURN_RIGHT_Client_read(Reader& reader) {
    MSG_MOVE_START_TURN_RIGHT_Client obj;

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_START_TURN_RIGHT_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_START_TURN_RIGHT_Client_size(obj));

    writer.write_u16_be((uint16_t)MSG_MOVE_START_TURN_RIGHT_Client_size(obj) + 4); /* size */

    writer.write_u32(0x000000bd); /* opcode */

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_START_TURN_RIGHT_Server_size(const MSG_MOVE_START_TURN_RIGHT_Server& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid) + MovementInfo_size(obj.info);
}

MSG_MOVE_START_TURN_RIGHT_Server MSG_MOVE_START_TURN_RIGHT_Server_read(Reader& reader) {
    MSG_MOVE_START_TURN_RIGHT_Server obj;

    obj.guid = reader.read_packed_guid();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_START_TURN_RIGHT_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_START_TURN_RIGHT_Server_size(obj));

    writer.write_u16_be(MSG_MOVE_START_TURN_RIGHT_Server_size(obj) + 2); /* size */

    writer.write_u16(0x000000bd); /* opcode */

    writer.write_packed_guid(obj.guid);

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_STOP_TURN_Client_size(const MSG_MOVE_STOP_TURN_Client& obj) {
    return 0 + MovementInfo_size(obj.info);
}

MSG_MOVE_STOP_TURN_Client MSG_MOVE_STOP_TURN_Client_read(Reader& reader) {
    MSG_MOVE_STOP_TURN_Client obj;

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_STOP_TURN_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_STOP_TURN_Client_size(obj));

    writer.write_u16_be((uint16_t)MSG_MOVE_STOP_TURN_Client_size(obj) + 4); /* size */

    writer.write_u32(0x000000be); /* opcode */

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_STOP_TURN_Server_size(const MSG_MOVE_STOP_TURN_Server& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid) + MovementInfo_size(obj.info);
}

MSG_MOVE_STOP_TURN_Server MSG_MOVE_STOP_TURN_Server_read(Reader& reader) {
    MSG_MOVE_STOP_TURN_Server obj;

    obj.guid = reader.read_packed_guid();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_STOP_TURN_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_STOP_TURN_Server_size(obj));

    writer.write_u16_be(MSG_MOVE_STOP_TURN_Server_size(obj) + 2); /* size */

    writer.write_u16(0x000000be); /* opcode */

    writer.write_packed_guid(obj.guid);

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_START_PITCH_UP_Client_size(const MSG_MOVE_START_PITCH_UP_Client& obj) {
    return 0 + MovementInfo_size(obj.info);
}

MSG_MOVE_START_PITCH_UP_Client MSG_MOVE_START_PITCH_UP_Client_read(Reader& reader) {
    MSG_MOVE_START_PITCH_UP_Client obj;

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_START_PITCH_UP_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_START_PITCH_UP_Client_size(obj));

    writer.write_u16_be((uint16_t)MSG_MOVE_START_PITCH_UP_Client_size(obj) + 4); /* size */

    writer.write_u32(0x000000bf); /* opcode */

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_START_PITCH_UP_Server_size(const MSG_MOVE_START_PITCH_UP_Server& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid) + MovementInfo_size(obj.info);
}

MSG_MOVE_START_PITCH_UP_Server MSG_MOVE_START_PITCH_UP_Server_read(Reader& reader) {
    MSG_MOVE_START_PITCH_UP_Server obj;

    obj.guid = reader.read_packed_guid();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_START_PITCH_UP_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_START_PITCH_UP_Server_size(obj));

    writer.write_u16_be(MSG_MOVE_START_PITCH_UP_Server_size(obj) + 2); /* size */

    writer.write_u16(0x000000bf); /* opcode */

    writer.write_packed_guid(obj.guid);

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_START_PITCH_DOWN_Client_size(const MSG_MOVE_START_PITCH_DOWN_Client& obj) {
    return 0 + MovementInfo_size(obj.info);
}

MSG_MOVE_START_PITCH_DOWN_Client MSG_MOVE_START_PITCH_DOWN_Client_read(Reader& reader) {
    MSG_MOVE_START_PITCH_DOWN_Client obj;

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_START_PITCH_DOWN_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_START_PITCH_DOWN_Client_size(obj));

    writer.write_u16_be((uint16_t)MSG_MOVE_START_PITCH_DOWN_Client_size(obj) + 4); /* size */

    writer.write_u32(0x000000c0); /* opcode */

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_START_PITCH_DOWN_Server_size(const MSG_MOVE_START_PITCH_DOWN_Server& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid) + MovementInfo_size(obj.info);
}

MSG_MOVE_START_PITCH_DOWN_Server MSG_MOVE_START_PITCH_DOWN_Server_read(Reader& reader) {
    MSG_MOVE_START_PITCH_DOWN_Server obj;

    obj.guid = reader.read_packed_guid();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_START_PITCH_DOWN_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_START_PITCH_DOWN_Server_size(obj));

    writer.write_u16_be(MSG_MOVE_START_PITCH_DOWN_Server_size(obj) + 2); /* size */

    writer.write_u16(0x000000c0); /* opcode */

    writer.write_packed_guid(obj.guid);

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_STOP_PITCH_Client_size(const MSG_MOVE_STOP_PITCH_Client& obj) {
    return 0 + MovementInfo_size(obj.info);
}

MSG_MOVE_STOP_PITCH_Client MSG_MOVE_STOP_PITCH_Client_read(Reader& reader) {
    MSG_MOVE_STOP_PITCH_Client obj;

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_STOP_PITCH_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_STOP_PITCH_Client_size(obj));

    writer.write_u16_be((uint16_t)MSG_MOVE_STOP_PITCH_Client_size(obj) + 4); /* size */

    writer.write_u32(0x000000c1); /* opcode */

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_STOP_PITCH_Server_size(const MSG_MOVE_STOP_PITCH_Server& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid) + MovementInfo_size(obj.info);
}

MSG_MOVE_STOP_PITCH_Server MSG_MOVE_STOP_PITCH_Server_read(Reader& reader) {
    MSG_MOVE_STOP_PITCH_Server obj;

    obj.guid = reader.read_packed_guid();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_STOP_PITCH_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_STOP_PITCH_Server_size(obj));

    writer.write_u16_be(MSG_MOVE_STOP_PITCH_Server_size(obj) + 2); /* size */

    writer.write_u16(0x000000c1); /* opcode */

    writer.write_packed_guid(obj.guid);

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_SET_RUN_MODE_Client_size(const MSG_MOVE_SET_RUN_MODE_Client& obj) {
    return 0 + MovementInfo_size(obj.info);
}

MSG_MOVE_SET_RUN_MODE_Client MSG_MOVE_SET_RUN_MODE_Client_read(Reader& reader) {
    MSG_MOVE_SET_RUN_MODE_Client obj;

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_SET_RUN_MODE_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_SET_RUN_MODE_Client_size(obj));

    writer.write_u16_be((uint16_t)MSG_MOVE_SET_RUN_MODE_Client_size(obj) + 4); /* size */

    writer.write_u32(0x000000c2); /* opcode */

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_SET_RUN_MODE_Server_size(const MSG_MOVE_SET_RUN_MODE_Server& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid) + MovementInfo_size(obj.info);
}

MSG_MOVE_SET_RUN_MODE_Server MSG_MOVE_SET_RUN_MODE_Server_read(Reader& reader) {
    MSG_MOVE_SET_RUN_MODE_Server obj;

    obj.guid = reader.read_packed_guid();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_SET_RUN_MODE_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_SET_RUN_MODE_Server_size(obj));

    writer.write_u16_be(MSG_MOVE_SET_RUN_MODE_Server_size(obj) + 2); /* size */

    writer.write_u16(0x000000c2); /* opcode */

    writer.write_packed_guid(obj.guid);

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_SET_WALK_MODE_Client_size(const MSG_MOVE_SET_WALK_MODE_Client& obj) {
    return 0 + MovementInfo_size(obj.info);
}

MSG_MOVE_SET_WALK_MODE_Client MSG_MOVE_SET_WALK_MODE_Client_read(Reader& reader) {
    MSG_MOVE_SET_WALK_MODE_Client obj;

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_SET_WALK_MODE_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_SET_WALK_MODE_Client_size(obj));

    writer.write_u16_be((uint16_t)MSG_MOVE_SET_WALK_MODE_Client_size(obj) + 4); /* size */

    writer.write_u32(0x000000c3); /* opcode */

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_SET_WALK_MODE_Server_size(const MSG_MOVE_SET_WALK_MODE_Server& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid) + MovementInfo_size(obj.info);
}

MSG_MOVE_SET_WALK_MODE_Server MSG_MOVE_SET_WALK_MODE_Server_read(Reader& reader) {
    MSG_MOVE_SET_WALK_MODE_Server obj;

    obj.guid = reader.read_packed_guid();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_SET_WALK_MODE_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_SET_WALK_MODE_Server_size(obj));

    writer.write_u16_be(MSG_MOVE_SET_WALK_MODE_Server_size(obj) + 2); /* size */

    writer.write_u16(0x000000c3); /* opcode */

    writer.write_packed_guid(obj.guid);

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_TELEPORT_ACK_Client_size(const MSG_MOVE_TELEPORT_ACK_Client& obj) {
    return 8 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

MSG_MOVE_TELEPORT_ACK_Client MSG_MOVE_TELEPORT_ACK_Client_read(Reader& reader) {
    MSG_MOVE_TELEPORT_ACK_Client obj;

    obj.guid = reader.read_packed_guid();

    obj.movement_counter = reader.read_u32();

    obj.time = reader.read_u32();

    return obj;
}

std::vector<unsigned char> MSG_MOVE_TELEPORT_ACK_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_TELEPORT_ACK_Client_size(obj));

    writer.write_u16_be((uint16_t)MSG_MOVE_TELEPORT_ACK_Client_size(obj) + 4); /* size */

    writer.write_u32(0x000000c7); /* opcode */

    writer.write_packed_guid(obj.guid);

    writer.write_u32(obj.movement_counter);

    writer.write_u32(obj.time);

    return writer.m_buf;
}

static size_t MSG_MOVE_TELEPORT_ACK_Server_size(const MSG_MOVE_TELEPORT_ACK_Server& obj) {
    return 4 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid) + MovementInfo_size(obj.info);
}

MSG_MOVE_TELEPORT_ACK_Server MSG_MOVE_TELEPORT_ACK_Server_read(Reader& reader) {
    MSG_MOVE_TELEPORT_ACK_Server obj;

    obj.guid = reader.read_packed_guid();

    obj.movement_counter = reader.read_u32();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_TELEPORT_ACK_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_TELEPORT_ACK_Server_size(obj));

    writer.write_u16_be(MSG_MOVE_TELEPORT_ACK_Server_size(obj) + 2); /* size */

    writer.write_u16(0x000000c7); /* opcode */

    writer.write_packed_guid(obj.guid);

    writer.write_u32(obj.movement_counter);

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_FALL_LAND_Client_size(const MSG_MOVE_FALL_LAND_Client& obj) {
    return 0 + MovementInfo_size(obj.info);
}

MSG_MOVE_FALL_LAND_Client MSG_MOVE_FALL_LAND_Client_read(Reader& reader) {
    MSG_MOVE_FALL_LAND_Client obj;

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_FALL_LAND_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_FALL_LAND_Client_size(obj));

    writer.write_u16_be((uint16_t)MSG_MOVE_FALL_LAND_Client_size(obj) + 4); /* size */

    writer.write_u32(0x000000c9); /* opcode */

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_FALL_LAND_Server_size(const MSG_MOVE_FALL_LAND_Server& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid) + MovementInfo_size(obj.info);
}

MSG_MOVE_FALL_LAND_Server MSG_MOVE_FALL_LAND_Server_read(Reader& reader) {
    MSG_MOVE_FALL_LAND_Server obj;

    obj.guid = reader.read_packed_guid();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_FALL_LAND_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_FALL_LAND_Server_size(obj));

    writer.write_u16_be(MSG_MOVE_FALL_LAND_Server_size(obj) + 2); /* size */

    writer.write_u16(0x000000c9); /* opcode */

    writer.write_packed_guid(obj.guid);

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_START_SWIM_Client_size(const MSG_MOVE_START_SWIM_Client& obj) {
    return 0 + MovementInfo_size(obj.info);
}

MSG_MOVE_START_SWIM_Client MSG_MOVE_START_SWIM_Client_read(Reader& reader) {
    MSG_MOVE_START_SWIM_Client obj;

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_START_SWIM_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_START_SWIM_Client_size(obj));

    writer.write_u16_be((uint16_t)MSG_MOVE_START_SWIM_Client_size(obj) + 4); /* size */

    writer.write_u32(0x000000ca); /* opcode */

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_START_SWIM_Server_size(const MSG_MOVE_START_SWIM_Server& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid) + MovementInfo_size(obj.info);
}

MSG_MOVE_START_SWIM_Server MSG_MOVE_START_SWIM_Server_read(Reader& reader) {
    MSG_MOVE_START_SWIM_Server obj;

    obj.guid = reader.read_packed_guid();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_START_SWIM_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_START_SWIM_Server_size(obj));

    writer.write_u16_be(MSG_MOVE_START_SWIM_Server_size(obj) + 2); /* size */

    writer.write_u16(0x000000ca); /* opcode */

    writer.write_packed_guid(obj.guid);

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_STOP_SWIM_Client_size(const MSG_MOVE_STOP_SWIM_Client& obj) {
    return 0 + MovementInfo_size(obj.info);
}

MSG_MOVE_STOP_SWIM_Client MSG_MOVE_STOP_SWIM_Client_read(Reader& reader) {
    MSG_MOVE_STOP_SWIM_Client obj;

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_STOP_SWIM_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_STOP_SWIM_Client_size(obj));

    writer.write_u16_be((uint16_t)MSG_MOVE_STOP_SWIM_Client_size(obj) + 4); /* size */

    writer.write_u32(0x000000cb); /* opcode */

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_STOP_SWIM_Server_size(const MSG_MOVE_STOP_SWIM_Server& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid) + MovementInfo_size(obj.info);
}

MSG_MOVE_STOP_SWIM_Server MSG_MOVE_STOP_SWIM_Server_read(Reader& reader) {
    MSG_MOVE_STOP_SWIM_Server obj;

    obj.guid = reader.read_packed_guid();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_STOP_SWIM_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_STOP_SWIM_Server_size(obj));

    writer.write_u16_be(MSG_MOVE_STOP_SWIM_Server_size(obj) + 2); /* size */

    writer.write_u16(0x000000cb); /* opcode */

    writer.write_packed_guid(obj.guid);

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_SET_FACING_Client_size(const MSG_MOVE_SET_FACING_Client& obj) {
    return 0 + MovementInfo_size(obj.info);
}

MSG_MOVE_SET_FACING_Client MSG_MOVE_SET_FACING_Client_read(Reader& reader) {
    MSG_MOVE_SET_FACING_Client obj;

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_SET_FACING_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_SET_FACING_Client_size(obj));

    writer.write_u16_be((uint16_t)MSG_MOVE_SET_FACING_Client_size(obj) + 4); /* size */

    writer.write_u32(0x000000da); /* opcode */

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_SET_FACING_Server_size(const MSG_MOVE_SET_FACING_Server& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid) + MovementInfo_size(obj.info);
}

MSG_MOVE_SET_FACING_Server MSG_MOVE_SET_FACING_Server_read(Reader& reader) {
    MSG_MOVE_SET_FACING_Server obj;

    obj.guid = reader.read_packed_guid();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_SET_FACING_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_SET_FACING_Server_size(obj));

    writer.write_u16_be(MSG_MOVE_SET_FACING_Server_size(obj) + 2); /* size */

    writer.write_u16(0x000000da); /* opcode */

    writer.write_packed_guid(obj.guid);

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_SET_PITCH_Client_size(const MSG_MOVE_SET_PITCH_Client& obj) {
    return 0 + MovementInfo_size(obj.info);
}

MSG_MOVE_SET_PITCH_Client MSG_MOVE_SET_PITCH_Client_read(Reader& reader) {
    MSG_MOVE_SET_PITCH_Client obj;

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_SET_PITCH_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_SET_PITCH_Client_size(obj));

    writer.write_u16_be((uint16_t)MSG_MOVE_SET_PITCH_Client_size(obj) + 4); /* size */

    writer.write_u32(0x000000db); /* opcode */

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_SET_PITCH_Server_size(const MSG_MOVE_SET_PITCH_Server& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid) + MovementInfo_size(obj.info);
}

MSG_MOVE_SET_PITCH_Server MSG_MOVE_SET_PITCH_Server_read(Reader& reader) {
    MSG_MOVE_SET_PITCH_Server obj;

    obj.guid = reader.read_packed_guid();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_SET_PITCH_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_SET_PITCH_Server_size(obj));

    writer.write_u16_be(MSG_MOVE_SET_PITCH_Server_size(obj) + 2); /* size */

    writer.write_u16(0x000000db); /* opcode */

    writer.write_packed_guid(obj.guid);

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t SMSG_MONSTER_MOVE_size(const SMSG_MONSTER_MOVE& obj) {
    size_t _size = 25 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid) + ::wow_world_messages::util::wwm_monster_move_spline_size(obj.splines);

    if (obj.move_type == MonsterMoveType::FACING_TARGET) {
        _size += 8;
    }
    else if (obj.move_type == MonsterMoveType::FACING_ANGLE) {
        _size += 4;
    }
    else if (obj.move_type == MonsterMoveType::FACING_SPOT) {
        _size += 12;
    }

    return _size;
}

SMSG_MONSTER_MOVE SMSG_MONSTER_MOVE_read(Reader& reader) {
    SMSG_MONSTER_MOVE obj;

    obj.guid = reader.read_packed_guid();

    obj.spline_point = ::wow_world_messages::all::Vector3d_read(reader);

    obj.spline_id = reader.read_u32();

    obj.move_type = static_cast<MonsterMoveType>(reader.read_u8());

    if (obj.move_type == MonsterMoveType::FACING_TARGET) {
        obj.target = reader.read_u64();

    }
    else if (obj.move_type == MonsterMoveType::FACING_ANGLE) {
        obj.angle = reader.read_float();

    }
    else if (obj.move_type == MonsterMoveType::FACING_SPOT) {
        obj.position = ::wow_world_messages::all::Vector3d_read(reader);

    }
    obj.spline_flags = static_cast<SplineFlag>(reader.read_u32());

    obj.duration = reader.read_u32();

    obj.splines = ::wow_world_messages::util::wwm_read_monster_move_spline(reader);

    return obj;
}

std::vector<unsigned char> SMSG_MONSTER_MOVE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_MONSTER_MOVE_size(obj));

    writer.write_u16_be(SMSG_MONSTER_MOVE_size(obj) + 2); /* size */

    writer.write_u16(0x000000dd); /* opcode */

    writer.write_packed_guid(obj.guid);

    Vector3d_write(writer, obj.spline_point);

    writer.write_u32(obj.spline_id);

    writer.write_u8(static_cast<uint8_t>(obj.move_type));

    if (obj.move_type == MonsterMoveType::FACING_TARGET) {
        writer.write_u64(obj.target);

    }
    else if (obj.move_type == MonsterMoveType::FACING_ANGLE) {
        writer.write_float(obj.angle);

    }
    else if (obj.move_type == MonsterMoveType::FACING_SPOT) {
        Vector3d_write(writer, obj.position);

    }
    writer.write_u32(static_cast<uint32_t>(obj.spline_flags));

    writer.write_u32(obj.duration);

    ::wow_world_messages::util::wwm_write_monster_move_spline(writer, obj.splines);

    return writer.m_buf;
}

static size_t SMSG_MOVE_WATER_WALK_size(const SMSG_MOVE_WATER_WALK& obj) {
    return 4 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_MOVE_WATER_WALK SMSG_MOVE_WATER_WALK_read(Reader& reader) {
    SMSG_MOVE_WATER_WALK obj;

    obj.guid = reader.read_packed_guid();

    obj.counter = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_MOVE_WATER_WALK::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_MOVE_WATER_WALK_size(obj));

    writer.write_u16_be(SMSG_MOVE_WATER_WALK_size(obj) + 2); /* size */

    writer.write_u16(0x000000de); /* opcode */

    writer.write_packed_guid(obj.guid);

    writer.write_u32(obj.counter);

    return writer.m_buf;
}

static size_t SMSG_MOVE_LAND_WALK_size(const SMSG_MOVE_LAND_WALK& obj) {
    return 4 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_MOVE_LAND_WALK SMSG_MOVE_LAND_WALK_read(Reader& reader) {
    SMSG_MOVE_LAND_WALK obj;

    obj.guid = reader.read_packed_guid();

    obj.counter = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_MOVE_LAND_WALK::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_MOVE_LAND_WALK_size(obj));

    writer.write_u16_be(SMSG_MOVE_LAND_WALK_size(obj) + 2); /* size */

    writer.write_u16(0x000000df); /* opcode */

    writer.write_packed_guid(obj.guid);

    writer.write_u32(obj.counter);

    return writer.m_buf;
}

CMSG_MOVE_SET_RAW_POSITION CMSG_MOVE_SET_RAW_POSITION_read(Reader& reader) {
    CMSG_MOVE_SET_RAW_POSITION obj;

    obj.position = ::wow_world_messages::all::Vector3d_read(reader);

    obj.orientation = reader.read_float();

    return obj;
}

std::vector<unsigned char> CMSG_MOVE_SET_RAW_POSITION::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0010);

    writer.write_u16_be(0x0010 + 4); /* size */

    writer.write_u32(0x000000e1); /* opcode */

    Vector3d_write(writer, obj.position);

    writer.write_float(obj.orientation);

    return writer.m_buf;
}

static size_t SMSG_FORCE_RUN_SPEED_CHANGE_size(const SMSG_FORCE_RUN_SPEED_CHANGE& obj) {
    return 8 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_FORCE_RUN_SPEED_CHANGE SMSG_FORCE_RUN_SPEED_CHANGE_read(Reader& reader) {
    SMSG_FORCE_RUN_SPEED_CHANGE obj;

    obj.guid = reader.read_packed_guid();

    obj.move_event = reader.read_u32();

    obj.speed = reader.read_float();

    return obj;
}

std::vector<unsigned char> SMSG_FORCE_RUN_SPEED_CHANGE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_FORCE_RUN_SPEED_CHANGE_size(obj));

    writer.write_u16_be(SMSG_FORCE_RUN_SPEED_CHANGE_size(obj) + 2); /* size */

    writer.write_u16(0x000000e2); /* opcode */

    writer.write_packed_guid(obj.guid);

    writer.write_u32(obj.move_event);

    writer.write_float(obj.speed);

    return writer.m_buf;
}

static size_t CMSG_FORCE_RUN_SPEED_CHANGE_ACK_size(const CMSG_FORCE_RUN_SPEED_CHANGE_ACK& obj) {
    return 16 + MovementInfo_size(obj.info);
}

CMSG_FORCE_RUN_SPEED_CHANGE_ACK CMSG_FORCE_RUN_SPEED_CHANGE_ACK_read(Reader& reader) {
    CMSG_FORCE_RUN_SPEED_CHANGE_ACK obj;

    obj.guid = reader.read_u64();

    obj.counter = reader.read_u32();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    obj.new_speed = reader.read_float();

    return obj;
}

std::vector<unsigned char> CMSG_FORCE_RUN_SPEED_CHANGE_ACK::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_FORCE_RUN_SPEED_CHANGE_ACK_size(obj));

    writer.write_u16_be((uint16_t)CMSG_FORCE_RUN_SPEED_CHANGE_ACK_size(obj) + 4); /* size */

    writer.write_u32(0x000000e3); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.counter);

    MovementInfo_write(writer, obj.info);

    writer.write_float(obj.new_speed);

    return writer.m_buf;
}

static size_t SMSG_FORCE_RUN_BACK_SPEED_CHANGE_size(const SMSG_FORCE_RUN_BACK_SPEED_CHANGE& obj) {
    return 8 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_FORCE_RUN_BACK_SPEED_CHANGE SMSG_FORCE_RUN_BACK_SPEED_CHANGE_read(Reader& reader) {
    SMSG_FORCE_RUN_BACK_SPEED_CHANGE obj;

    obj.guid = reader.read_packed_guid();

    obj.move_event = reader.read_u32();

    obj.speed = reader.read_float();

    return obj;
}

std::vector<unsigned char> SMSG_FORCE_RUN_BACK_SPEED_CHANGE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_FORCE_RUN_BACK_SPEED_CHANGE_size(obj));

    writer.write_u16_be(SMSG_FORCE_RUN_BACK_SPEED_CHANGE_size(obj) + 2); /* size */

    writer.write_u16(0x000000e4); /* opcode */

    writer.write_packed_guid(obj.guid);

    writer.write_u32(obj.move_event);

    writer.write_float(obj.speed);

    return writer.m_buf;
}

static size_t CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK_size(const CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK& obj) {
    return 16 + MovementInfo_size(obj.info);
}

CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK_read(Reader& reader) {
    CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK obj;

    obj.guid = reader.read_u64();

    obj.movement_counter = reader.read_u32();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    obj.new_speed = reader.read_float();

    return obj;
}

std::vector<unsigned char> CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK_size(obj));

    writer.write_u16_be((uint16_t)CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK_size(obj) + 4); /* size */

    writer.write_u32(0x000000e5); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.movement_counter);

    MovementInfo_write(writer, obj.info);

    writer.write_float(obj.new_speed);

    return writer.m_buf;
}

static size_t SMSG_FORCE_SWIM_SPEED_CHANGE_size(const SMSG_FORCE_SWIM_SPEED_CHANGE& obj) {
    return 8 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_FORCE_SWIM_SPEED_CHANGE SMSG_FORCE_SWIM_SPEED_CHANGE_read(Reader& reader) {
    SMSG_FORCE_SWIM_SPEED_CHANGE obj;

    obj.guid = reader.read_packed_guid();

    obj.move_event = reader.read_u32();

    obj.speed = reader.read_float();

    return obj;
}

std::vector<unsigned char> SMSG_FORCE_SWIM_SPEED_CHANGE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_FORCE_SWIM_SPEED_CHANGE_size(obj));

    writer.write_u16_be(SMSG_FORCE_SWIM_SPEED_CHANGE_size(obj) + 2); /* size */

    writer.write_u16(0x000000e6); /* opcode */

    writer.write_packed_guid(obj.guid);

    writer.write_u32(obj.move_event);

    writer.write_float(obj.speed);

    return writer.m_buf;
}

static size_t CMSG_FORCE_SWIM_SPEED_CHANGE_ACK_size(const CMSG_FORCE_SWIM_SPEED_CHANGE_ACK& obj) {
    return 16 + MovementInfo_size(obj.info);
}

CMSG_FORCE_SWIM_SPEED_CHANGE_ACK CMSG_FORCE_SWIM_SPEED_CHANGE_ACK_read(Reader& reader) {
    CMSG_FORCE_SWIM_SPEED_CHANGE_ACK obj;

    obj.guid = reader.read_u64();

    obj.counter = reader.read_u32();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    obj.new_speed = reader.read_float();

    return obj;
}

std::vector<unsigned char> CMSG_FORCE_SWIM_SPEED_CHANGE_ACK::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_FORCE_SWIM_SPEED_CHANGE_ACK_size(obj));

    writer.write_u16_be((uint16_t)CMSG_FORCE_SWIM_SPEED_CHANGE_ACK_size(obj) + 4); /* size */

    writer.write_u32(0x000000e7); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.counter);

    MovementInfo_write(writer, obj.info);

    writer.write_float(obj.new_speed);

    return writer.m_buf;
}

SMSG_FORCE_MOVE_ROOT SMSG_FORCE_MOVE_ROOT_read(Reader& reader) {
    SMSG_FORCE_MOVE_ROOT obj;

    obj.guid = reader.read_u64();

    obj.counter = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_FORCE_MOVE_ROOT::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 2); /* size */

    writer.write_u16(0x000000e8); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.counter);

    return writer.m_buf;
}

static size_t CMSG_FORCE_MOVE_ROOT_ACK_size(const CMSG_FORCE_MOVE_ROOT_ACK& obj) {
    return 12 + MovementInfo_size(obj.info);
}

CMSG_FORCE_MOVE_ROOT_ACK CMSG_FORCE_MOVE_ROOT_ACK_read(Reader& reader) {
    CMSG_FORCE_MOVE_ROOT_ACK obj;

    obj.guid = reader.read_u64();

    obj.movement_counter = reader.read_u32();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> CMSG_FORCE_MOVE_ROOT_ACK::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_FORCE_MOVE_ROOT_ACK_size(obj));

    writer.write_u16_be((uint16_t)CMSG_FORCE_MOVE_ROOT_ACK_size(obj) + 4); /* size */

    writer.write_u32(0x000000e9); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.movement_counter);

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

SMSG_FORCE_MOVE_UNROOT SMSG_FORCE_MOVE_UNROOT_read(Reader& reader) {
    SMSG_FORCE_MOVE_UNROOT obj;

    obj.guid = reader.read_u64();

    obj.counter = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_FORCE_MOVE_UNROOT::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 2); /* size */

    writer.write_u16(0x000000ea); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.counter);

    return writer.m_buf;
}

static size_t CMSG_FORCE_MOVE_UNROOT_ACK_size(const CMSG_FORCE_MOVE_UNROOT_ACK& obj) {
    return 12 + MovementInfo_size(obj.info);
}

CMSG_FORCE_MOVE_UNROOT_ACK CMSG_FORCE_MOVE_UNROOT_ACK_read(Reader& reader) {
    CMSG_FORCE_MOVE_UNROOT_ACK obj;

    obj.guid = reader.read_u64();

    obj.movement_counter = reader.read_u32();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> CMSG_FORCE_MOVE_UNROOT_ACK::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_FORCE_MOVE_UNROOT_ACK_size(obj));

    writer.write_u16_be((uint16_t)CMSG_FORCE_MOVE_UNROOT_ACK_size(obj) + 4); /* size */

    writer.write_u32(0x000000eb); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.movement_counter);

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_HEARTBEAT_Client_size(const MSG_MOVE_HEARTBEAT_Client& obj) {
    return 0 + MovementInfo_size(obj.info);
}

MSG_MOVE_HEARTBEAT_Client MSG_MOVE_HEARTBEAT_Client_read(Reader& reader) {
    MSG_MOVE_HEARTBEAT_Client obj;

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_HEARTBEAT_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_HEARTBEAT_Client_size(obj));

    writer.write_u16_be((uint16_t)MSG_MOVE_HEARTBEAT_Client_size(obj) + 4); /* size */

    writer.write_u32(0x000000ee); /* opcode */

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t MSG_MOVE_HEARTBEAT_Server_size(const MSG_MOVE_HEARTBEAT_Server& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid) + MovementInfo_size(obj.info);
}

MSG_MOVE_HEARTBEAT_Server MSG_MOVE_HEARTBEAT_Server_read(Reader& reader) {
    MSG_MOVE_HEARTBEAT_Server obj;

    obj.guid = reader.read_packed_guid();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_HEARTBEAT_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_HEARTBEAT_Server_size(obj));

    writer.write_u16_be(MSG_MOVE_HEARTBEAT_Server_size(obj) + 2); /* size */

    writer.write_u16(0x000000ee); /* opcode */

    writer.write_packed_guid(obj.guid);

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t SMSG_MOVE_KNOCK_BACK_size(const SMSG_MOVE_KNOCK_BACK& obj) {
    return 20 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_MOVE_KNOCK_BACK SMSG_MOVE_KNOCK_BACK_read(Reader& reader) {
    SMSG_MOVE_KNOCK_BACK obj;

    obj.guid = reader.read_packed_guid();

    obj.movement_counter = reader.read_u32();

    obj.v_cos = reader.read_float();

    obj.v_sin = reader.read_float();

    obj.horizontal_speed = reader.read_float();

    obj.vertical_speed = reader.read_float();

    return obj;
}

std::vector<unsigned char> SMSG_MOVE_KNOCK_BACK::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_MOVE_KNOCK_BACK_size(obj));

    writer.write_u16_be(SMSG_MOVE_KNOCK_BACK_size(obj) + 2); /* size */

    writer.write_u16(0x000000ef); /* opcode */

    writer.write_packed_guid(obj.guid);

    writer.write_u32(obj.movement_counter);

    writer.write_float(obj.v_cos);

    writer.write_float(obj.v_sin);

    writer.write_float(obj.horizontal_speed);

    writer.write_float(obj.vertical_speed);

    return writer.m_buf;
}

static size_t CMSG_MOVE_KNOCK_BACK_ACK_size(const CMSG_MOVE_KNOCK_BACK_ACK& obj) {
    return 12 + MovementInfo_size(obj.info);
}

CMSG_MOVE_KNOCK_BACK_ACK CMSG_MOVE_KNOCK_BACK_ACK_read(Reader& reader) {
    CMSG_MOVE_KNOCK_BACK_ACK obj;

    obj.guid = reader.read_u64();

    obj.counter = reader.read_u32();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> CMSG_MOVE_KNOCK_BACK_ACK::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_MOVE_KNOCK_BACK_ACK_size(obj));

    writer.write_u16_be((uint16_t)CMSG_MOVE_KNOCK_BACK_ACK_size(obj) + 4); /* size */

    writer.write_u32(0x000000f0); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.counter);

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

static size_t SMSG_MOVE_FEATHER_FALL_size(const SMSG_MOVE_FEATHER_FALL& obj) {
    return 4 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_MOVE_FEATHER_FALL SMSG_MOVE_FEATHER_FALL_read(Reader& reader) {
    SMSG_MOVE_FEATHER_FALL obj;

    obj.guid = reader.read_packed_guid();

    obj.counter = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_MOVE_FEATHER_FALL::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_MOVE_FEATHER_FALL_size(obj));

    writer.write_u16_be(SMSG_MOVE_FEATHER_FALL_size(obj) + 2); /* size */

    writer.write_u16(0x000000f2); /* opcode */

    writer.write_packed_guid(obj.guid);

    writer.write_u32(obj.counter);

    return writer.m_buf;
}

static size_t SMSG_MOVE_NORMAL_FALL_size(const SMSG_MOVE_NORMAL_FALL& obj) {
    return 4 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_MOVE_NORMAL_FALL SMSG_MOVE_NORMAL_FALL_read(Reader& reader) {
    SMSG_MOVE_NORMAL_FALL obj;

    obj.guid = reader.read_packed_guid();

    obj.counter = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_MOVE_NORMAL_FALL::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_MOVE_NORMAL_FALL_size(obj));

    writer.write_u16_be(SMSG_MOVE_NORMAL_FALL_size(obj) + 2); /* size */

    writer.write_u16(0x000000f3); /* opcode */

    writer.write_packed_guid(obj.guid);

    writer.write_u32(obj.counter);

    return writer.m_buf;
}

static size_t SMSG_MOVE_SET_HOVER_size(const SMSG_MOVE_SET_HOVER& obj) {
    return 4 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_MOVE_SET_HOVER SMSG_MOVE_SET_HOVER_read(Reader& reader) {
    SMSG_MOVE_SET_HOVER obj;

    obj.guid = reader.read_packed_guid();

    obj.counter = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_MOVE_SET_HOVER::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_MOVE_SET_HOVER_size(obj));

    writer.write_u16_be(SMSG_MOVE_SET_HOVER_size(obj) + 2); /* size */

    writer.write_u16(0x000000f4); /* opcode */

    writer.write_packed_guid(obj.guid);

    writer.write_u32(obj.counter);

    return writer.m_buf;
}

static size_t SMSG_MOVE_UNSET_HOVER_size(const SMSG_MOVE_UNSET_HOVER& obj) {
    return 4 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_MOVE_UNSET_HOVER SMSG_MOVE_UNSET_HOVER_read(Reader& reader) {
    SMSG_MOVE_UNSET_HOVER obj;

    obj.guid = reader.read_packed_guid();

    obj.counter = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_MOVE_UNSET_HOVER::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_MOVE_UNSET_HOVER_size(obj));

    writer.write_u16_be(SMSG_MOVE_UNSET_HOVER_size(obj) + 2); /* size */

    writer.write_u16(0x000000f5); /* opcode */

    writer.write_packed_guid(obj.guid);

    writer.write_u32(obj.counter);

    return writer.m_buf;
}

static size_t CMSG_MOVE_HOVER_ACK_size(const CMSG_MOVE_HOVER_ACK& obj) {
    return 16 + MovementInfo_size(obj.info);
}

CMSG_MOVE_HOVER_ACK CMSG_MOVE_HOVER_ACK_read(Reader& reader) {
    CMSG_MOVE_HOVER_ACK obj;

    obj.guid = reader.read_u64();

    obj.counter = reader.read_u32();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    obj.is_applied = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_MOVE_HOVER_ACK::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_MOVE_HOVER_ACK_size(obj));

    writer.write_u16_be((uint16_t)CMSG_MOVE_HOVER_ACK_size(obj) + 4); /* size */

    writer.write_u32(0x000000f6); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.counter);

    MovementInfo_write(writer, obj.info);

    writer.write_u32(obj.is_applied);

    return writer.m_buf;
}

SMSG_TRIGGER_CINEMATIC SMSG_TRIGGER_CINEMATIC_read(Reader& reader) {
    SMSG_TRIGGER_CINEMATIC obj;

    obj.cinematic_sequence_id = static_cast<CinematicSequenceId>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_TRIGGER_CINEMATIC::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x000000fa); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.cinematic_sequence_id));

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_NEXT_CINEMATIC_CAMERA::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x000000fb); /* opcode */

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_COMPLETE_CINEMATIC::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x000000fc); /* opcode */

    return writer.m_buf;
}

SMSG_TUTORIAL_FLAGS SMSG_TUTORIAL_FLAGS_read(Reader& reader) {
    SMSG_TUTORIAL_FLAGS obj;

    for (auto i = 0; i < 8; ++i) {
        obj.tutorial_data[i] = reader.read_u32();
    }

    return obj;
}

std::vector<unsigned char> SMSG_TUTORIAL_FLAGS::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0020);

    writer.write_u16_be(0x0020 + 2); /* size */

    writer.write_u16(0x000000fd); /* opcode */

    for (const auto& v : obj.tutorial_data) {
        writer.write_u32(v);
    }

    return writer.m_buf;
}

CMSG_TUTORIAL_FLAG CMSG_TUTORIAL_FLAG_read(Reader& reader) {
    CMSG_TUTORIAL_FLAG obj;

    obj.tutorial_flag = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_TUTORIAL_FLAG::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 4); /* size */

    writer.write_u32(0x000000fe); /* opcode */

    writer.write_u32(obj.tutorial_flag);

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_TUTORIAL_CLEAR::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x000000ff); /* opcode */

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_TUTORIAL_RESET::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x00000100); /* opcode */

    return writer.m_buf;
}

CMSG_STANDSTATECHANGE CMSG_STANDSTATECHANGE_read(Reader& reader) {
    CMSG_STANDSTATECHANGE obj;

    obj.animation_state = static_cast<UnitStandState>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> CMSG_STANDSTATECHANGE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 4); /* size */

    writer.write_u32(0x00000101); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.animation_state));

    return writer.m_buf;
}

CMSG_EMOTE CMSG_EMOTE_read(Reader& reader) {
    CMSG_EMOTE obj;

    obj.emote = static_cast<Emote>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> CMSG_EMOTE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 4); /* size */

    writer.write_u32(0x00000102); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.emote));

    return writer.m_buf;
}

SMSG_EMOTE SMSG_EMOTE_read(Reader& reader) {
    SMSG_EMOTE obj;

    obj.emote = static_cast<Emote>(reader.read_u32());

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> SMSG_EMOTE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 2); /* size */

    writer.write_u16(0x00000103); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.emote));

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

CMSG_TEXT_EMOTE CMSG_TEXT_EMOTE_read(Reader& reader) {
    CMSG_TEXT_EMOTE obj;

    obj.text_emote = static_cast<TextEmote>(reader.read_u32());

    obj.emote = reader.read_u32();

    obj.target = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_TEXT_EMOTE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0010);

    writer.write_u16_be(0x0010 + 4); /* size */

    writer.write_u32(0x00000104); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.text_emote));

    writer.write_u32(obj.emote);

    writer.write_u64(obj.target);

    return writer.m_buf;
}

static size_t SMSG_TEXT_EMOTE_size(const SMSG_TEXT_EMOTE& obj) {
    return 21 + obj.name.size();
}

SMSG_TEXT_EMOTE SMSG_TEXT_EMOTE_read(Reader& reader) {
    SMSG_TEXT_EMOTE obj;

    obj.guid = reader.read_u64();

    obj.text_emote = static_cast<TextEmote>(reader.read_u32());

    obj.emote = reader.read_u32();

    obj.name = reader.read_sized_cstring();

    return obj;
}

std::vector<unsigned char> SMSG_TEXT_EMOTE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_TEXT_EMOTE_size(obj));

    writer.write_u16_be(SMSG_TEXT_EMOTE_size(obj) + 2); /* size */

    writer.write_u16(0x00000105); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(static_cast<uint32_t>(obj.text_emote));

    writer.write_u32(obj.emote);

    writer.write_sized_cstring(obj.name);

    return writer.m_buf;
}

CMSG_AUTOSTORE_LOOT_ITEM CMSG_AUTOSTORE_LOOT_ITEM_read(Reader& reader) {
    CMSG_AUTOSTORE_LOOT_ITEM obj;

    obj.item_slot = reader.read_u8();

    return obj;
}

std::vector<unsigned char> CMSG_AUTOSTORE_LOOT_ITEM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0001);

    writer.write_u16_be(0x0001 + 4); /* size */

    writer.write_u32(0x00000108); /* opcode */

    writer.write_u8(obj.item_slot);

    return writer.m_buf;
}

CMSG_AUTOEQUIP_ITEM CMSG_AUTOEQUIP_ITEM_read(Reader& reader) {
    CMSG_AUTOEQUIP_ITEM obj;

    obj.source_bag = reader.read_u8();

    obj.source_slot = reader.read_u8();

    return obj;
}

std::vector<unsigned char> CMSG_AUTOEQUIP_ITEM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0002);

    writer.write_u16_be(0x0002 + 4); /* size */

    writer.write_u32(0x0000010a); /* opcode */

    writer.write_u8(obj.source_bag);

    writer.write_u8(obj.source_slot);

    return writer.m_buf;
}

CMSG_AUTOSTORE_BAG_ITEM CMSG_AUTOSTORE_BAG_ITEM_read(Reader& reader) {
    CMSG_AUTOSTORE_BAG_ITEM obj;

    obj.source_bag = reader.read_u8();

    obj.source_slot = reader.read_u8();

    obj.destination_bag = reader.read_u8();

    return obj;
}

std::vector<unsigned char> CMSG_AUTOSTORE_BAG_ITEM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0003);

    writer.write_u16_be(0x0003 + 4); /* size */

    writer.write_u32(0x0000010b); /* opcode */

    writer.write_u8(obj.source_bag);

    writer.write_u8(obj.source_slot);

    writer.write_u8(obj.destination_bag);

    return writer.m_buf;
}

CMSG_SWAP_ITEM CMSG_SWAP_ITEM_read(Reader& reader) {
    CMSG_SWAP_ITEM obj;

    obj.destination_bag = reader.read_u8();

    obj.destionation_slot = reader.read_u8();

    obj.source_bag = reader.read_u8();

    obj.source_slot = reader.read_u8();

    return obj;
}

std::vector<unsigned char> CMSG_SWAP_ITEM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 4); /* size */

    writer.write_u32(0x0000010c); /* opcode */

    writer.write_u8(obj.destination_bag);

    writer.write_u8(obj.destionation_slot);

    writer.write_u8(obj.source_bag);

    writer.write_u8(obj.source_slot);

    return writer.m_buf;
}

CMSG_SWAP_INV_ITEM CMSG_SWAP_INV_ITEM_read(Reader& reader) {
    CMSG_SWAP_INV_ITEM obj;

    obj.source_slot = static_cast<ItemSlot>(reader.read_u8());

    obj.destination_slot = static_cast<ItemSlot>(reader.read_u8());

    return obj;
}

std::vector<unsigned char> CMSG_SWAP_INV_ITEM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0002);

    writer.write_u16_be(0x0002 + 4); /* size */

    writer.write_u32(0x0000010d); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.source_slot));

    writer.write_u8(static_cast<uint8_t>(obj.destination_slot));

    return writer.m_buf;
}

CMSG_SPLIT_ITEM CMSG_SPLIT_ITEM_read(Reader& reader) {
    CMSG_SPLIT_ITEM obj;

    obj.source_bag = reader.read_u8();

    obj.source_slot = reader.read_u8();

    obj.destination_bag = reader.read_u8();

    obj.destination_slot = reader.read_u8();

    obj.amount = reader.read_u8();

    return obj;
}

std::vector<unsigned char> CMSG_SPLIT_ITEM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0005);

    writer.write_u16_be(0x0005 + 4); /* size */

    writer.write_u32(0x0000010e); /* opcode */

    writer.write_u8(obj.source_bag);

    writer.write_u8(obj.source_slot);

    writer.write_u8(obj.destination_bag);

    writer.write_u8(obj.destination_slot);

    writer.write_u8(obj.amount);

    return writer.m_buf;
}

CMSG_AUTOEQUIP_ITEM_SLOT CMSG_AUTOEQUIP_ITEM_SLOT_read(Reader& reader) {
    CMSG_AUTOEQUIP_ITEM_SLOT obj;

    obj.guid = reader.read_u64();

    obj.destination_slot = reader.read_u8();

    return obj;
}

std::vector<unsigned char> CMSG_AUTOEQUIP_ITEM_SLOT::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0009);

    writer.write_u16_be(0x0009 + 4); /* size */

    writer.write_u32(0x0000010f); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u8(obj.destination_slot);

    return writer.m_buf;
}

CMSG_DESTROYITEM CMSG_DESTROYITEM_read(Reader& reader) {
    CMSG_DESTROYITEM obj;

    obj.bag = reader.read_u8();

    obj.slot = reader.read_u8();

    obj.amount = reader.read_u8();

    obj.unknown1 = reader.read_u8();

    obj.unknown2 = reader.read_u8();

    obj.unknown3 = reader.read_u8();

    return obj;
}

std::vector<unsigned char> CMSG_DESTROYITEM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0006);

    writer.write_u16_be(0x0006 + 4); /* size */

    writer.write_u32(0x00000111); /* opcode */

    writer.write_u8(obj.bag);

    writer.write_u8(obj.slot);

    writer.write_u8(obj.amount);

    writer.write_u8(obj.unknown1);

    writer.write_u8(obj.unknown2);

    writer.write_u8(obj.unknown3);

    return writer.m_buf;
}

static size_t SMSG_INVENTORY_CHANGE_FAILURE_size(const SMSG_INVENTORY_CHANGE_FAILURE& obj) {
    size_t _size = 1;

    if (obj.result == InventoryResult::CANT_EQUIP_LEVEL_I) {
        _size += 4;
    }

    if (obj.result == InventoryResult::CANT_EQUIP_LEVEL_I|| obj.result == InventoryResult::CANT_EQUIP_SKILL|| obj.result == InventoryResult::ITEM_DOESNT_GO_TO_SLOT|| obj.result == InventoryResult::BAG_FULL|| obj.result == InventoryResult::NONEMPTY_BAG_OVER_OTHER_BAG|| obj.result == InventoryResult::CANT_TRADE_EQUIP_BAGS|| obj.result == InventoryResult::ONLY_AMMO_CAN_GO_HERE|| obj.result == InventoryResult::NO_REQUIRED_PROFICIENCY|| obj.result == InventoryResult::NO_EQUIPMENT_SLOT_AVAILABLE|| obj.result == InventoryResult::YOU_CAN_NEVER_USE_THAT_ITEM|| obj.result == InventoryResult::YOU_CAN_NEVER_USE_THAT_ITEM2|| obj.result == InventoryResult::NO_EQUIPMENT_SLOT_AVAILABLE2|| obj.result == InventoryResult::CANT_EQUIP_WITH_TWOHANDED|| obj.result == InventoryResult::CANT_DUAL_WIELD|| obj.result == InventoryResult::ITEM_DOESNT_GO_INTO_BAG|| obj.result == InventoryResult::ITEM_DOESNT_GO_INTO_BAG2|| obj.result == InventoryResult::CANT_CARRY_MORE_OF_THIS|| obj.result == InventoryResult::NO_EQUIPMENT_SLOT_AVAILABLE3|| obj.result == InventoryResult::ITEM_CANT_STACK|| obj.result == InventoryResult::ITEM_CANT_BE_EQUIPPED|| obj.result == InventoryResult::ITEMS_CANT_BE_SWAPPED|| obj.result == InventoryResult::SLOT_IS_EMPTY|| obj.result == InventoryResult::ITEM_NOT_FOUND|| obj.result == InventoryResult::CANT_DROP_SOULBOUND|| obj.result == InventoryResult::OUT_OF_RANGE|| obj.result == InventoryResult::TRIED_TO_SPLIT_MORE_THAN_COUNT|| obj.result == InventoryResult::COULDNT_SPLIT_ITEMS|| obj.result == InventoryResult::MISSING_REAGENT|| obj.result == InventoryResult::NOT_ENOUGH_MONEY|| obj.result == InventoryResult::NOT_A_BAG|| obj.result == InventoryResult::CAN_ONLY_DO_WITH_EMPTY_BAGS|| obj.result == InventoryResult::DONT_OWN_THAT_ITEM|| obj.result == InventoryResult::CAN_EQUIP_ONLY1_QUIVER|| obj.result == InventoryResult::MUST_PURCHASE_THAT_BAG_SLOT|| obj.result == InventoryResult::TOO_FAR_AWAY_FROM_BANK|| obj.result == InventoryResult::ITEM_LOCKED|| obj.result == InventoryResult::YOU_ARE_STUNNED|| obj.result == InventoryResult::YOU_ARE_DEAD|| obj.result == InventoryResult::CANT_DO_RIGHT_NOW|| obj.result == InventoryResult::INT_BAG_ERROR|| obj.result == InventoryResult::CAN_EQUIP_ONLY1_BOLT|| obj.result == InventoryResult::CAN_EQUIP_ONLY1_AMMOPOUCH|| obj.result == InventoryResult::STACKABLE_CANT_BE_WRAPPED|| obj.result == InventoryResult::EQUIPPED_CANT_BE_WRAPPED|| obj.result == InventoryResult::WRAPPED_CANT_BE_WRAPPED|| obj.result == InventoryResult::BOUND_CANT_BE_WRAPPED|| obj.result == InventoryResult::UNIQUE_CANT_BE_WRAPPED|| obj.result == InventoryResult::BAGS_CANT_BE_WRAPPED|| obj.result == InventoryResult::ALREADY_LOOTED|| obj.result == InventoryResult::INVENTORY_FULL|| obj.result == InventoryResult::BANK_FULL|| obj.result == InventoryResult::ITEM_IS_CURRENTLY_SOLD_OUT|| obj.result == InventoryResult::BAG_FULL3|| obj.result == InventoryResult::ITEM_NOT_FOUND2|| obj.result == InventoryResult::ITEM_CANT_STACK2|| obj.result == InventoryResult::BAG_FULL4|| obj.result == InventoryResult::ITEM_SOLD_OUT|| obj.result == InventoryResult::OBJECT_IS_BUSY|| obj.result == InventoryResult::NONE|| obj.result == InventoryResult::NOT_IN_COMBAT|| obj.result == InventoryResult::NOT_WHILE_DISARMED|| obj.result == InventoryResult::BAG_FULL6|| obj.result == InventoryResult::CANT_EQUIP_RANK|| obj.result == InventoryResult::CANT_EQUIP_REPUTATION|| obj.result == InventoryResult::TOO_MANY_SPECIAL_BAGS|| obj.result == InventoryResult::LOOT_CANT_LOOT_THAT_NOW) {
        _size += 17;
    }

    return _size;
}

SMSG_INVENTORY_CHANGE_FAILURE SMSG_INVENTORY_CHANGE_FAILURE_read(Reader& reader) {
    SMSG_INVENTORY_CHANGE_FAILURE obj;

    obj.result = static_cast<InventoryResult>(reader.read_u8());

    if (obj.result == InventoryResult::CANT_EQUIP_LEVEL_I) {
        obj.required_level = reader.read_u32();

    }
    if (obj.result == InventoryResult::CANT_EQUIP_LEVEL_I|| obj.result == InventoryResult::CANT_EQUIP_SKILL|| obj.result == InventoryResult::ITEM_DOESNT_GO_TO_SLOT|| obj.result == InventoryResult::BAG_FULL|| obj.result == InventoryResult::NONEMPTY_BAG_OVER_OTHER_BAG|| obj.result == InventoryResult::CANT_TRADE_EQUIP_BAGS|| obj.result == InventoryResult::ONLY_AMMO_CAN_GO_HERE|| obj.result == InventoryResult::NO_REQUIRED_PROFICIENCY|| obj.result == InventoryResult::NO_EQUIPMENT_SLOT_AVAILABLE|| obj.result == InventoryResult::YOU_CAN_NEVER_USE_THAT_ITEM|| obj.result == InventoryResult::YOU_CAN_NEVER_USE_THAT_ITEM2|| obj.result == InventoryResult::NO_EQUIPMENT_SLOT_AVAILABLE2|| obj.result == InventoryResult::CANT_EQUIP_WITH_TWOHANDED|| obj.result == InventoryResult::CANT_DUAL_WIELD|| obj.result == InventoryResult::ITEM_DOESNT_GO_INTO_BAG|| obj.result == InventoryResult::ITEM_DOESNT_GO_INTO_BAG2|| obj.result == InventoryResult::CANT_CARRY_MORE_OF_THIS|| obj.result == InventoryResult::NO_EQUIPMENT_SLOT_AVAILABLE3|| obj.result == InventoryResult::ITEM_CANT_STACK|| obj.result == InventoryResult::ITEM_CANT_BE_EQUIPPED|| obj.result == InventoryResult::ITEMS_CANT_BE_SWAPPED|| obj.result == InventoryResult::SLOT_IS_EMPTY|| obj.result == InventoryResult::ITEM_NOT_FOUND|| obj.result == InventoryResult::CANT_DROP_SOULBOUND|| obj.result == InventoryResult::OUT_OF_RANGE|| obj.result == InventoryResult::TRIED_TO_SPLIT_MORE_THAN_COUNT|| obj.result == InventoryResult::COULDNT_SPLIT_ITEMS|| obj.result == InventoryResult::MISSING_REAGENT|| obj.result == InventoryResult::NOT_ENOUGH_MONEY|| obj.result == InventoryResult::NOT_A_BAG|| obj.result == InventoryResult::CAN_ONLY_DO_WITH_EMPTY_BAGS|| obj.result == InventoryResult::DONT_OWN_THAT_ITEM|| obj.result == InventoryResult::CAN_EQUIP_ONLY1_QUIVER|| obj.result == InventoryResult::MUST_PURCHASE_THAT_BAG_SLOT|| obj.result == InventoryResult::TOO_FAR_AWAY_FROM_BANK|| obj.result == InventoryResult::ITEM_LOCKED|| obj.result == InventoryResult::YOU_ARE_STUNNED|| obj.result == InventoryResult::YOU_ARE_DEAD|| obj.result == InventoryResult::CANT_DO_RIGHT_NOW|| obj.result == InventoryResult::INT_BAG_ERROR|| obj.result == InventoryResult::CAN_EQUIP_ONLY1_BOLT|| obj.result == InventoryResult::CAN_EQUIP_ONLY1_AMMOPOUCH|| obj.result == InventoryResult::STACKABLE_CANT_BE_WRAPPED|| obj.result == InventoryResult::EQUIPPED_CANT_BE_WRAPPED|| obj.result == InventoryResult::WRAPPED_CANT_BE_WRAPPED|| obj.result == InventoryResult::BOUND_CANT_BE_WRAPPED|| obj.result == InventoryResult::UNIQUE_CANT_BE_WRAPPED|| obj.result == InventoryResult::BAGS_CANT_BE_WRAPPED|| obj.result == InventoryResult::ALREADY_LOOTED|| obj.result == InventoryResult::INVENTORY_FULL|| obj.result == InventoryResult::BANK_FULL|| obj.result == InventoryResult::ITEM_IS_CURRENTLY_SOLD_OUT|| obj.result == InventoryResult::BAG_FULL3|| obj.result == InventoryResult::ITEM_NOT_FOUND2|| obj.result == InventoryResult::ITEM_CANT_STACK2|| obj.result == InventoryResult::BAG_FULL4|| obj.result == InventoryResult::ITEM_SOLD_OUT|| obj.result == InventoryResult::OBJECT_IS_BUSY|| obj.result == InventoryResult::NONE|| obj.result == InventoryResult::NOT_IN_COMBAT|| obj.result == InventoryResult::NOT_WHILE_DISARMED|| obj.result == InventoryResult::BAG_FULL6|| obj.result == InventoryResult::CANT_EQUIP_RANK|| obj.result == InventoryResult::CANT_EQUIP_REPUTATION|| obj.result == InventoryResult::TOO_MANY_SPECIAL_BAGS|| obj.result == InventoryResult::LOOT_CANT_LOOT_THAT_NOW) {
        obj.item1 = reader.read_u64();

        obj.item2 = reader.read_u64();

        obj.bag_type_subclass = reader.read_u8();

    }
    return obj;
}

std::vector<unsigned char> SMSG_INVENTORY_CHANGE_FAILURE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_INVENTORY_CHANGE_FAILURE_size(obj));

    writer.write_u16_be(SMSG_INVENTORY_CHANGE_FAILURE_size(obj) + 2); /* size */

    writer.write_u16(0x00000112); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.result));

    if (obj.result == InventoryResult::CANT_EQUIP_LEVEL_I) {
        writer.write_u32(obj.required_level);

    }
    if (obj.result == InventoryResult::CANT_EQUIP_LEVEL_I|| obj.result == InventoryResult::CANT_EQUIP_SKILL|| obj.result == InventoryResult::ITEM_DOESNT_GO_TO_SLOT|| obj.result == InventoryResult::BAG_FULL|| obj.result == InventoryResult::NONEMPTY_BAG_OVER_OTHER_BAG|| obj.result == InventoryResult::CANT_TRADE_EQUIP_BAGS|| obj.result == InventoryResult::ONLY_AMMO_CAN_GO_HERE|| obj.result == InventoryResult::NO_REQUIRED_PROFICIENCY|| obj.result == InventoryResult::NO_EQUIPMENT_SLOT_AVAILABLE|| obj.result == InventoryResult::YOU_CAN_NEVER_USE_THAT_ITEM|| obj.result == InventoryResult::YOU_CAN_NEVER_USE_THAT_ITEM2|| obj.result == InventoryResult::NO_EQUIPMENT_SLOT_AVAILABLE2|| obj.result == InventoryResult::CANT_EQUIP_WITH_TWOHANDED|| obj.result == InventoryResult::CANT_DUAL_WIELD|| obj.result == InventoryResult::ITEM_DOESNT_GO_INTO_BAG|| obj.result == InventoryResult::ITEM_DOESNT_GO_INTO_BAG2|| obj.result == InventoryResult::CANT_CARRY_MORE_OF_THIS|| obj.result == InventoryResult::NO_EQUIPMENT_SLOT_AVAILABLE3|| obj.result == InventoryResult::ITEM_CANT_STACK|| obj.result == InventoryResult::ITEM_CANT_BE_EQUIPPED|| obj.result == InventoryResult::ITEMS_CANT_BE_SWAPPED|| obj.result == InventoryResult::SLOT_IS_EMPTY|| obj.result == InventoryResult::ITEM_NOT_FOUND|| obj.result == InventoryResult::CANT_DROP_SOULBOUND|| obj.result == InventoryResult::OUT_OF_RANGE|| obj.result == InventoryResult::TRIED_TO_SPLIT_MORE_THAN_COUNT|| obj.result == InventoryResult::COULDNT_SPLIT_ITEMS|| obj.result == InventoryResult::MISSING_REAGENT|| obj.result == InventoryResult::NOT_ENOUGH_MONEY|| obj.result == InventoryResult::NOT_A_BAG|| obj.result == InventoryResult::CAN_ONLY_DO_WITH_EMPTY_BAGS|| obj.result == InventoryResult::DONT_OWN_THAT_ITEM|| obj.result == InventoryResult::CAN_EQUIP_ONLY1_QUIVER|| obj.result == InventoryResult::MUST_PURCHASE_THAT_BAG_SLOT|| obj.result == InventoryResult::TOO_FAR_AWAY_FROM_BANK|| obj.result == InventoryResult::ITEM_LOCKED|| obj.result == InventoryResult::YOU_ARE_STUNNED|| obj.result == InventoryResult::YOU_ARE_DEAD|| obj.result == InventoryResult::CANT_DO_RIGHT_NOW|| obj.result == InventoryResult::INT_BAG_ERROR|| obj.result == InventoryResult::CAN_EQUIP_ONLY1_BOLT|| obj.result == InventoryResult::CAN_EQUIP_ONLY1_AMMOPOUCH|| obj.result == InventoryResult::STACKABLE_CANT_BE_WRAPPED|| obj.result == InventoryResult::EQUIPPED_CANT_BE_WRAPPED|| obj.result == InventoryResult::WRAPPED_CANT_BE_WRAPPED|| obj.result == InventoryResult::BOUND_CANT_BE_WRAPPED|| obj.result == InventoryResult::UNIQUE_CANT_BE_WRAPPED|| obj.result == InventoryResult::BAGS_CANT_BE_WRAPPED|| obj.result == InventoryResult::ALREADY_LOOTED|| obj.result == InventoryResult::INVENTORY_FULL|| obj.result == InventoryResult::BANK_FULL|| obj.result == InventoryResult::ITEM_IS_CURRENTLY_SOLD_OUT|| obj.result == InventoryResult::BAG_FULL3|| obj.result == InventoryResult::ITEM_NOT_FOUND2|| obj.result == InventoryResult::ITEM_CANT_STACK2|| obj.result == InventoryResult::BAG_FULL4|| obj.result == InventoryResult::ITEM_SOLD_OUT|| obj.result == InventoryResult::OBJECT_IS_BUSY|| obj.result == InventoryResult::NONE|| obj.result == InventoryResult::NOT_IN_COMBAT|| obj.result == InventoryResult::NOT_WHILE_DISARMED|| obj.result == InventoryResult::BAG_FULL6|| obj.result == InventoryResult::CANT_EQUIP_RANK|| obj.result == InventoryResult::CANT_EQUIP_REPUTATION|| obj.result == InventoryResult::TOO_MANY_SPECIAL_BAGS|| obj.result == InventoryResult::LOOT_CANT_LOOT_THAT_NOW) {
        writer.write_u64(obj.item1);

        writer.write_u64(obj.item2);

        writer.write_u8(obj.bag_type_subclass);

    }
    return writer.m_buf;
}

SMSG_OPEN_CONTAINER SMSG_OPEN_CONTAINER_read(Reader& reader) {
    SMSG_OPEN_CONTAINER obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> SMSG_OPEN_CONTAINER::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 2); /* size */

    writer.write_u16(0x00000113); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

CMSG_INSPECT CMSG_INSPECT_read(Reader& reader) {
    CMSG_INSPECT obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_INSPECT::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x00000114); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

SMSG_INSPECT SMSG_INSPECT_read(Reader& reader) {
    SMSG_INSPECT obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> SMSG_INSPECT::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 2); /* size */

    writer.write_u16(0x00000115); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

CMSG_INITIATE_TRADE CMSG_INITIATE_TRADE_read(Reader& reader) {
    CMSG_INITIATE_TRADE obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_INITIATE_TRADE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x00000116); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_BEGIN_TRADE::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x00000117); /* opcode */

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_BUSY_TRADE::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x00000118); /* opcode */

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_IGNORE_TRADE::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x00000119); /* opcode */

    return writer.m_buf;
}

CMSG_ACCEPT_TRADE CMSG_ACCEPT_TRADE_read(Reader& reader) {
    CMSG_ACCEPT_TRADE obj;

    obj.unknown1 = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_ACCEPT_TRADE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 4); /* size */

    writer.write_u32(0x0000011a); /* opcode */

    writer.write_u32(obj.unknown1);

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_UNACCEPT_TRADE::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x0000011b); /* opcode */

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_CANCEL_TRADE::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x0000011c); /* opcode */

    return writer.m_buf;
}

CMSG_SET_TRADE_ITEM CMSG_SET_TRADE_ITEM_read(Reader& reader) {
    CMSG_SET_TRADE_ITEM obj;

    obj.trade_slot = reader.read_u8();

    obj.bag = reader.read_u8();

    obj.slot = reader.read_u8();

    return obj;
}

std::vector<unsigned char> CMSG_SET_TRADE_ITEM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0003);

    writer.write_u16_be(0x0003 + 4); /* size */

    writer.write_u32(0x0000011d); /* opcode */

    writer.write_u8(obj.trade_slot);

    writer.write_u8(obj.bag);

    writer.write_u8(obj.slot);

    return writer.m_buf;
}

CMSG_CLEAR_TRADE_ITEM CMSG_CLEAR_TRADE_ITEM_read(Reader& reader) {
    CMSG_CLEAR_TRADE_ITEM obj;

    obj.trade_slot = reader.read_u8();

    return obj;
}

std::vector<unsigned char> CMSG_CLEAR_TRADE_ITEM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0001);

    writer.write_u16_be(0x0001 + 4); /* size */

    writer.write_u32(0x0000011e); /* opcode */

    writer.write_u8(obj.trade_slot);

    return writer.m_buf;
}

CMSG_SET_TRADE_GOLD CMSG_SET_TRADE_GOLD_read(Reader& reader) {
    CMSG_SET_TRADE_GOLD obj;

    obj.gold = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_SET_TRADE_GOLD::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 4); /* size */

    writer.write_u32(0x0000011f); /* opcode */

    writer.write_u32(obj.gold);

    return writer.m_buf;
}

static size_t SMSG_TRADE_STATUS_size(const SMSG_TRADE_STATUS& obj) {
    size_t _size = 4;

    if (obj.status == TradeStatus::BEGIN_TRADE) {
        _size += 8;
    }
    else if (obj.status == TradeStatus::CLOSE_WINDOW) {
        _size += 9;
    }
    else if (obj.status == TradeStatus::ONLY_CONJURED|| obj.status == TradeStatus::NOT_ON_TAPLIST) {
        _size += 1;
    }

    return _size;
}

SMSG_TRADE_STATUS SMSG_TRADE_STATUS_read(Reader& reader) {
    SMSG_TRADE_STATUS obj;

    obj.status = static_cast<TradeStatus>(reader.read_u32());

    if (obj.status == TradeStatus::BEGIN_TRADE) {
        obj.unknown1 = reader.read_u64();

    }
    else if (obj.status == TradeStatus::CLOSE_WINDOW) {
        obj.inventory_result = static_cast<InventoryResult>(reader.read_u32());

        obj.target_error = reader.read_bool8();

        obj.item_limit_category_id = reader.read_u32();

    }
    else if (obj.status == TradeStatus::ONLY_CONJURED|| obj.status == TradeStatus::NOT_ON_TAPLIST) {
        obj.slot = reader.read_u8();

    }
    return obj;
}

std::vector<unsigned char> SMSG_TRADE_STATUS::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_TRADE_STATUS_size(obj));

    writer.write_u16_be(SMSG_TRADE_STATUS_size(obj) + 2); /* size */

    writer.write_u16(0x00000120); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.status));

    if (obj.status == TradeStatus::BEGIN_TRADE) {
        writer.write_u64(obj.unknown1);

    }
    else if (obj.status == TradeStatus::CLOSE_WINDOW) {
        writer.write_u32(static_cast<uint32_t>(obj.inventory_result));

        writer.write_bool8(obj.target_error);

        writer.write_u32(obj.item_limit_category_id);

    }
    else if (obj.status == TradeStatus::ONLY_CONJURED|| obj.status == TradeStatus::NOT_ON_TAPLIST) {
        writer.write_u8(obj.slot);

    }
    return writer.m_buf;
}

SMSG_TRADE_STATUS_EXTENDED SMSG_TRADE_STATUS_EXTENDED_read(Reader& reader) {
    SMSG_TRADE_STATUS_EXTENDED obj;

    obj.self_player = reader.read_bool8();

    obj.trade_slot_count1 = reader.read_u32();

    obj.trade_slot_count2 = reader.read_u32();

    obj.money_in_trade = reader.read_u32();

    obj.spell_on_lowest_slot = reader.read_u32();

    for (auto i = 0; i < 7; ++i) {
        obj.trade_slots[i] = ::wow_world_messages::vanilla::TradeSlot_read(reader);
    }

    return obj;
}

std::vector<unsigned char> SMSG_TRADE_STATUS_EXTENDED::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x01bc);

    writer.write_u16_be(0x01bc + 2); /* size */

    writer.write_u16(0x00000121); /* opcode */

    writer.write_bool8(obj.self_player);

    writer.write_u32(obj.trade_slot_count1);

    writer.write_u32(obj.trade_slot_count2);

    writer.write_u32(obj.money_in_trade);

    writer.write_u32(obj.spell_on_lowest_slot);

    for (const auto& v : obj.trade_slots) {
        TradeSlot_write(writer, v);
    }

    return writer.m_buf;
}

static size_t SMSG_INITIALIZE_FACTIONS_size(const SMSG_INITIALIZE_FACTIONS& obj) {
    return 4 + 5 * obj.factions.size();
}

SMSG_INITIALIZE_FACTIONS SMSG_INITIALIZE_FACTIONS_read(Reader& reader) {
    SMSG_INITIALIZE_FACTIONS obj;

    obj.amount_of_factions = reader.read_u32();

    for (auto i = 0; i < obj.amount_of_factions; ++i) {
        obj.factions.push_back(::wow_world_messages::vanilla::FactionInitializer_read(reader));
    }

    return obj;
}

std::vector<unsigned char> SMSG_INITIALIZE_FACTIONS::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_INITIALIZE_FACTIONS_size(obj));

    writer.write_u16_be(SMSG_INITIALIZE_FACTIONS_size(obj) + 2); /* size */

    writer.write_u16(0x00000122); /* opcode */

    writer.write_u32(obj.amount_of_factions);

    for (const auto& v : obj.factions) {
        FactionInitializer_write(writer, v);
    }

    return writer.m_buf;
}

SMSG_SET_FACTION_VISIBLE SMSG_SET_FACTION_VISIBLE_read(Reader& reader) {
    SMSG_SET_FACTION_VISIBLE obj;

    obj.faction = static_cast<Faction>(reader.read_u16());

    return obj;
}

std::vector<unsigned char> SMSG_SET_FACTION_VISIBLE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0002);

    writer.write_u16_be(0x0002 + 2); /* size */

    writer.write_u16(0x00000123); /* opcode */

    writer.write_u16(static_cast<uint16_t>(obj.faction));

    return writer.m_buf;
}

static size_t SMSG_SET_FACTION_STANDING_size(const SMSG_SET_FACTION_STANDING& obj) {
    return 4 + 6 * obj.faction_standings.size();
}

SMSG_SET_FACTION_STANDING SMSG_SET_FACTION_STANDING_read(Reader& reader) {
    SMSG_SET_FACTION_STANDING obj;

    obj.amount_of_faction_standings = reader.read_u32();

    for (auto i = 0; i < obj.amount_of_faction_standings; ++i) {
        obj.faction_standings.push_back(::wow_world_messages::vanilla::FactionStanding_read(reader));
    }

    return obj;
}

std::vector<unsigned char> SMSG_SET_FACTION_STANDING::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SET_FACTION_STANDING_size(obj));

    writer.write_u16_be(SMSG_SET_FACTION_STANDING_size(obj) + 2); /* size */

    writer.write_u16(0x00000124); /* opcode */

    writer.write_u32(obj.amount_of_faction_standings);

    for (const auto& v : obj.faction_standings) {
        FactionStanding_write(writer, v);
    }

    return writer.m_buf;
}

CMSG_SET_FACTION_ATWAR CMSG_SET_FACTION_ATWAR_read(Reader& reader) {
    CMSG_SET_FACTION_ATWAR obj;

    obj.faction = static_cast<Faction>(reader.read_u16());

    obj.flags = static_cast<FactionFlag>(reader.read_u8());

    return obj;
}

std::vector<unsigned char> CMSG_SET_FACTION_ATWAR::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0003);

    writer.write_u16_be(0x0003 + 4); /* size */

    writer.write_u32(0x00000125); /* opcode */

    writer.write_u16(static_cast<uint16_t>(obj.faction));

    writer.write_u8(static_cast<uint8_t>(obj.flags));

    return writer.m_buf;
}

SMSG_SET_PROFICIENCY SMSG_SET_PROFICIENCY_read(Reader& reader) {
    SMSG_SET_PROFICIENCY obj;

    obj.class_type = static_cast<ItemClass>(reader.read_u8());

    obj.item_sub_class_mask = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_SET_PROFICIENCY::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0005);

    writer.write_u16_be(0x0005 + 2); /* size */

    writer.write_u16(0x00000127); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.class_type));

    writer.write_u32(obj.item_sub_class_mask);

    return writer.m_buf;
}

CMSG_SET_ACTION_BUTTON CMSG_SET_ACTION_BUTTON_read(Reader& reader) {
    CMSG_SET_ACTION_BUTTON obj;

    obj.button = reader.read_u8();

    obj.action = reader.read_u16();

    obj.misc = reader.read_u8();

    obj.action_type = reader.read_u8();

    return obj;
}

std::vector<unsigned char> CMSG_SET_ACTION_BUTTON::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0005);

    writer.write_u16_be(0x0005 + 4); /* size */

    writer.write_u32(0x00000128); /* opcode */

    writer.write_u8(obj.button);

    writer.write_u16(obj.action);

    writer.write_u8(obj.misc);

    writer.write_u8(obj.action_type);

    return writer.m_buf;
}

SMSG_ACTION_BUTTONS SMSG_ACTION_BUTTONS_read(Reader& reader) {
    SMSG_ACTION_BUTTONS obj;

    for (auto i = 0; i < 120; ++i) {
        obj.data[i] = reader.read_u32();
    }

    return obj;
}

std::vector<unsigned char> SMSG_ACTION_BUTTONS::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x01e0);

    writer.write_u16_be(0x01e0 + 2); /* size */

    writer.write_u16(0x00000129); /* opcode */

    for (const auto& v : obj.data) {
        writer.write_u32(v);
    }

    return writer.m_buf;
}

static size_t SMSG_INITIAL_SPELLS_size(const SMSG_INITIAL_SPELLS& obj) {
    return 5 + 4 * obj.initial_spells.size() + 14 * obj.cooldowns.size();
}

SMSG_INITIAL_SPELLS SMSG_INITIAL_SPELLS_read(Reader& reader) {
    SMSG_INITIAL_SPELLS obj;

    obj.unknown1 = reader.read_u8();

    obj.spell_count = reader.read_u16();

    for (auto i = 0; i < obj.spell_count; ++i) {
        obj.initial_spells.push_back(::wow_world_messages::vanilla::InitialSpell_read(reader));
    }

    obj.cooldown_count = reader.read_u16();

    for (auto i = 0; i < obj.cooldown_count; ++i) {
        obj.cooldowns.push_back(::wow_world_messages::vanilla::CooldownSpell_read(reader));
    }

    return obj;
}

std::vector<unsigned char> SMSG_INITIAL_SPELLS::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_INITIAL_SPELLS_size(obj));

    writer.write_u16_be(SMSG_INITIAL_SPELLS_size(obj) + 2); /* size */

    writer.write_u16(0x0000012a); /* opcode */

    writer.write_u8(obj.unknown1);

    writer.write_u16(obj.spell_count);

    for (const auto& v : obj.initial_spells) {
        InitialSpell_write(writer, v);
    }

    writer.write_u16(obj.cooldown_count);

    for (const auto& v : obj.cooldowns) {
        CooldownSpell_write(writer, v);
    }

    return writer.m_buf;
}

SMSG_LEARNED_SPELL SMSG_LEARNED_SPELL_read(Reader& reader) {
    SMSG_LEARNED_SPELL obj;

    obj.id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_LEARNED_SPELL::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x0000012b); /* opcode */

    writer.write_u32(obj.id);

    return writer.m_buf;
}

SMSG_SUPERCEDED_SPELL SMSG_SUPERCEDED_SPELL_read(Reader& reader) {
    SMSG_SUPERCEDED_SPELL obj;

    obj.new_spell_id = reader.read_u16();

    obj.old_spell_id = reader.read_u16();

    return obj;
}

std::vector<unsigned char> SMSG_SUPERCEDED_SPELL::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x0000012c); /* opcode */

    writer.write_u16(obj.new_spell_id);

    writer.write_u16(obj.old_spell_id);

    return writer.m_buf;
}

static size_t CMSG_CAST_SPELL_size(const CMSG_CAST_SPELL& obj) {
    return 4 + SpellCastTargets_size(obj.targets);
}

CMSG_CAST_SPELL CMSG_CAST_SPELL_read(Reader& reader) {
    CMSG_CAST_SPELL obj;

    obj.spell = reader.read_u32();

    obj.targets = ::wow_world_messages::vanilla::SpellCastTargets_read(reader);

    return obj;
}

std::vector<unsigned char> CMSG_CAST_SPELL::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_CAST_SPELL_size(obj));

    writer.write_u16_be((uint16_t)CMSG_CAST_SPELL_size(obj) + 4); /* size */

    writer.write_u32(0x0000012e); /* opcode */

    writer.write_u32(obj.spell);

    SpellCastTargets_write(writer, obj.targets);

    return writer.m_buf;
}

CMSG_CANCEL_CAST CMSG_CANCEL_CAST_read(Reader& reader) {
    CMSG_CANCEL_CAST obj;

    obj.id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_CANCEL_CAST::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 4); /* size */

    writer.write_u32(0x0000012f); /* opcode */

    writer.write_u32(obj.id);

    return writer.m_buf;
}

static size_t SMSG_CAST_RESULT_size(const SMSG_CAST_RESULT& obj) {
    size_t _size = 5;

    if (obj.result == SimpleSpellCastResult::SUCCESS) {
        _size += 1;

        if (obj.reason == CastFailureReason::REQUIRES_SPELL_FOCUS) {
            _size += 4;
        }
        else if (obj.reason == CastFailureReason::REQUIRES_AREA) {
            _size += 4;
        }
        else if (obj.reason == CastFailureReason::EQUIPPED_ITEM_CLASS) {
            _size += 12;
        }

    }

    return _size;
}

SMSG_CAST_RESULT SMSG_CAST_RESULT_read(Reader& reader) {
    SMSG_CAST_RESULT obj;

    obj.spell = reader.read_u32();

    obj.result = static_cast<SimpleSpellCastResult>(reader.read_u8());

    if (obj.result == SimpleSpellCastResult::SUCCESS) {
        obj.reason = static_cast<CastFailureReason>(reader.read_u8());

        if (obj.reason == CastFailureReason::REQUIRES_SPELL_FOCUS) {
            obj.required_spell_focus = reader.read_u32();

        }
        else if (obj.reason == CastFailureReason::REQUIRES_AREA) {
            obj.area = static_cast<Area>(reader.read_u32());

        }
        else if (obj.reason == CastFailureReason::EQUIPPED_ITEM_CLASS) {
            obj.equipped_item_class = reader.read_u32();

            obj.equipped_item_subclass_mask = reader.read_u32();

            obj.equipped_item_inventory_type_mask = reader.read_u32();

        }
    }
    return obj;
}

std::vector<unsigned char> SMSG_CAST_RESULT::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_CAST_RESULT_size(obj));

    writer.write_u16_be(SMSG_CAST_RESULT_size(obj) + 2); /* size */

    writer.write_u16(0x00000130); /* opcode */

    writer.write_u32(obj.spell);

    writer.write_u8(static_cast<uint8_t>(obj.result));

    if (obj.result == SimpleSpellCastResult::SUCCESS) {
        writer.write_u8(static_cast<uint8_t>(obj.reason));

        if (obj.reason == CastFailureReason::REQUIRES_SPELL_FOCUS) {
            writer.write_u32(obj.required_spell_focus);

        }
        else if (obj.reason == CastFailureReason::REQUIRES_AREA) {
            writer.write_u32(static_cast<uint32_t>(obj.area));

        }
        else if (obj.reason == CastFailureReason::EQUIPPED_ITEM_CLASS) {
            writer.write_u32(obj.equipped_item_class);

            writer.write_u32(obj.equipped_item_subclass_mask);

            writer.write_u32(obj.equipped_item_inventory_type_mask);

        }
    }
    return writer.m_buf;
}

static size_t SMSG_SPELL_START_size(const SMSG_SPELL_START& obj) {
    size_t _size = 10 + ::wow_world_messages::util::wwm_packed_guid_size(obj.cast_item) + ::wow_world_messages::util::wwm_packed_guid_size(obj.caster) + SpellCastTargets_size(obj.targets);

    if ((obj.flags & CAST_FLAGS_AMMO) != 0) {
        _size += 8;
    }

    return _size;
}

SMSG_SPELL_START SMSG_SPELL_START_read(Reader& reader) {
    SMSG_SPELL_START obj;

    obj.cast_item = reader.read_packed_guid();

    obj.caster = reader.read_packed_guid();

    obj.spell = reader.read_u32();

    obj.flags = static_cast<CastFlags>(reader.read_u16());

    obj.timer = reader.read_u32();

    obj.targets = ::wow_world_messages::vanilla::SpellCastTargets_read(reader);

    if ((obj.flags & CAST_FLAGS_AMMO) != 0) {
        obj.ammo_display_id = reader.read_u32();

        obj.ammo_inventory_type = reader.read_u32();

    }
    return obj;
}

std::vector<unsigned char> SMSG_SPELL_START::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SPELL_START_size(obj));

    writer.write_u16_be(SMSG_SPELL_START_size(obj) + 2); /* size */

    writer.write_u16(0x00000131); /* opcode */

    writer.write_packed_guid(obj.cast_item);

    writer.write_packed_guid(obj.caster);

    writer.write_u32(obj.spell);

    writer.write_u16(static_cast<uint16_t>(obj.flags));

    writer.write_u32(obj.timer);

    SpellCastTargets_write(writer, obj.targets);

    if ((obj.flags & CAST_FLAGS_AMMO) != 0) {
        writer.write_u32(obj.ammo_display_id);

        writer.write_u32(obj.ammo_inventory_type);

    }
    return writer.m_buf;
}

static size_t SMSG_SPELL_GO_size(const SMSG_SPELL_GO& obj) {
    size_t _size = 8 + ::wow_world_messages::util::wwm_packed_guid_size(obj.cast_item) + ::wow_world_messages::util::wwm_packed_guid_size(obj.caster) + 8 * obj.hits.size() + 9 * obj.misses.size() + SpellCastTargets_size(obj.targets);

    if ((obj.flags & CAST_FLAGS_AMMO) != 0) {
        _size += 8;
    }

    return _size;
}

SMSG_SPELL_GO SMSG_SPELL_GO_read(Reader& reader) {
    SMSG_SPELL_GO obj;

    obj.cast_item = reader.read_packed_guid();

    obj.caster = reader.read_packed_guid();

    obj.spell = reader.read_u32();

    obj.flags = static_cast<CastFlags>(reader.read_u16());

    obj.amount_of_hits = reader.read_u8();

    for (auto i = 0; i < obj.amount_of_hits; ++i) {
        obj.hits.push_back(reader.read_u64());
    }

    obj.amount_of_misses = reader.read_u8();

    for (auto i = 0; i < obj.amount_of_misses; ++i) {
        obj.misses.push_back(::wow_world_messages::vanilla::SpellMiss_read(reader));
    }

    obj.targets = ::wow_world_messages::vanilla::SpellCastTargets_read(reader);

    if ((obj.flags & CAST_FLAGS_AMMO) != 0) {
        obj.ammo_display_id = reader.read_u32();

        obj.ammo_inventory_type = reader.read_u32();

    }
    return obj;
}

std::vector<unsigned char> SMSG_SPELL_GO::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SPELL_GO_size(obj));

    writer.write_u16_be(SMSG_SPELL_GO_size(obj) + 2); /* size */

    writer.write_u16(0x00000132); /* opcode */

    writer.write_packed_guid(obj.cast_item);

    writer.write_packed_guid(obj.caster);

    writer.write_u32(obj.spell);

    writer.write_u16(static_cast<uint16_t>(obj.flags));

    writer.write_u8(obj.amount_of_hits);

    for (const auto& v : obj.hits) {
        writer.write_u64(v);
    }

    writer.write_u8(obj.amount_of_misses);

    for (const auto& v : obj.misses) {
        SpellMiss_write(writer, v);
    }

    SpellCastTargets_write(writer, obj.targets);

    if ((obj.flags & CAST_FLAGS_AMMO) != 0) {
        writer.write_u32(obj.ammo_display_id);

        writer.write_u32(obj.ammo_inventory_type);

    }
    return writer.m_buf;
}

SMSG_SPELL_FAILURE SMSG_SPELL_FAILURE_read(Reader& reader) {
    SMSG_SPELL_FAILURE obj;

    obj.guid = reader.read_u64();

    obj.spell = reader.read_u32();

    obj.result = static_cast<SpellCastResult>(reader.read_u8());

    return obj;
}

std::vector<unsigned char> SMSG_SPELL_FAILURE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000d);

    writer.write_u16_be(0x000d + 2); /* size */

    writer.write_u16(0x00000133); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.spell);

    writer.write_u8(static_cast<uint8_t>(obj.result));

    return writer.m_buf;
}

static size_t SMSG_SPELL_COOLDOWN_size(const SMSG_SPELL_COOLDOWN& obj) {
    size_t _size = 8;

    for(const auto& v : obj.cooldowns) {
        _size += 8;
    }

    return _size;
}

SMSG_SPELL_COOLDOWN SMSG_SPELL_COOLDOWN_read(Reader& reader, size_t body_size) {
    SMSG_SPELL_COOLDOWN obj;
    size_t _size = 0;

    obj.guid = reader.read_u64();
    _size += 8;

    while (_size < body_size) {
        obj.cooldowns.push_back(::wow_world_messages::vanilla::SpellCooldownStatus_read(reader));
        _size += 8;
    }

    return obj;
}

std::vector<unsigned char> SMSG_SPELL_COOLDOWN::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SPELL_COOLDOWN_size(obj));

    writer.write_u16_be(SMSG_SPELL_COOLDOWN_size(obj) + 2); /* size */

    writer.write_u16(0x00000134); /* opcode */

    writer.write_u64(obj.guid);

    for (const auto& v : obj.cooldowns) {
        SpellCooldownStatus_write(writer, v);
    }

    return writer.m_buf;
}

SMSG_COOLDOWN_EVENT SMSG_COOLDOWN_EVENT_read(Reader& reader) {
    SMSG_COOLDOWN_EVENT obj;

    obj.id = reader.read_u32();

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> SMSG_COOLDOWN_EVENT::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 2); /* size */

    writer.write_u16(0x00000135); /* opcode */

    writer.write_u32(obj.id);

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

CMSG_CANCEL_AURA CMSG_CANCEL_AURA_read(Reader& reader) {
    CMSG_CANCEL_AURA obj;

    obj.id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_CANCEL_AURA::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 4); /* size */

    writer.write_u32(0x00000136); /* opcode */

    writer.write_u32(obj.id);

    return writer.m_buf;
}

SMSG_UPDATE_AURA_DURATION SMSG_UPDATE_AURA_DURATION_read(Reader& reader) {
    SMSG_UPDATE_AURA_DURATION obj;

    obj.aura_slot = reader.read_u8();

    obj.aura_duration = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_UPDATE_AURA_DURATION::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0005);

    writer.write_u16_be(0x0005 + 2); /* size */

    writer.write_u16(0x00000137); /* opcode */

    writer.write_u8(obj.aura_slot);

    writer.write_u32(obj.aura_duration);

    return writer.m_buf;
}

SMSG_PET_CAST_FAILED SMSG_PET_CAST_FAILED_read(Reader& reader) {
    SMSG_PET_CAST_FAILED obj;

    obj.id = reader.read_u32();

    obj.unknown1 = reader.read_u8();

    obj.result = static_cast<SpellCastResult>(reader.read_u8());

    return obj;
}

std::vector<unsigned char> SMSG_PET_CAST_FAILED::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0006);

    writer.write_u16_be(0x0006 + 2); /* size */

    writer.write_u16(0x00000138); /* opcode */

    writer.write_u32(obj.id);

    writer.write_u8(obj.unknown1);

    writer.write_u8(static_cast<uint8_t>(obj.result));

    return writer.m_buf;
}

MSG_CHANNEL_START_Server MSG_CHANNEL_START_Server_read(Reader& reader) {
    MSG_CHANNEL_START_Server obj;

    obj.spell = reader.read_u32();

    obj.duration = reader.read_u32();

    return obj;
}

std::vector<unsigned char> MSG_CHANNEL_START_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 2); /* size */

    writer.write_u16(0x00000139); /* opcode */

    writer.write_u32(obj.spell);

    writer.write_u32(obj.duration);

    return writer.m_buf;
}

MSG_CHANNEL_UPDATE_Server MSG_CHANNEL_UPDATE_Server_read(Reader& reader) {
    MSG_CHANNEL_UPDATE_Server obj;

    obj.time = reader.read_u32();

    return obj;
}

std::vector<unsigned char> MSG_CHANNEL_UPDATE_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x0000013a); /* opcode */

    writer.write_u32(obj.time);

    return writer.m_buf;
}

CMSG_CANCEL_CHANNELLING CMSG_CANCEL_CHANNELLING_read(Reader& reader) {
    CMSG_CANCEL_CHANNELLING obj;

    obj.id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_CANCEL_CHANNELLING::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 4); /* size */

    writer.write_u32(0x0000013b); /* opcode */

    writer.write_u32(obj.id);

    return writer.m_buf;
}

SMSG_AI_REACTION SMSG_AI_REACTION_read(Reader& reader) {
    SMSG_AI_REACTION obj;

    obj.guid = reader.read_u64();

    obj.reaction = static_cast<AiReaction>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_AI_REACTION::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 2); /* size */

    writer.write_u16(0x0000013c); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(static_cast<uint32_t>(obj.reaction));

    return writer.m_buf;
}

CMSG_SET_SELECTION CMSG_SET_SELECTION_read(Reader& reader) {
    CMSG_SET_SELECTION obj;

    obj.target = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_SET_SELECTION::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x0000013d); /* opcode */

    writer.write_u64(obj.target);

    return writer.m_buf;
}

CMSG_SET_TARGET_OBSOLETE CMSG_SET_TARGET_OBSOLETE_read(Reader& reader) {
    CMSG_SET_TARGET_OBSOLETE obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_SET_TARGET_OBSOLETE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x0000013e); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

CMSG_ATTACKSWING CMSG_ATTACKSWING_read(Reader& reader) {
    CMSG_ATTACKSWING obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_ATTACKSWING::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x00000141); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_ATTACKSTOP::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x00000142); /* opcode */

    return writer.m_buf;
}

SMSG_ATTACKSTART SMSG_ATTACKSTART_read(Reader& reader) {
    SMSG_ATTACKSTART obj;

    obj.attacker = reader.read_u64();

    obj.victim = reader.read_u64();

    return obj;
}

std::vector<unsigned char> SMSG_ATTACKSTART::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0010);

    writer.write_u16_be(0x0010 + 2); /* size */

    writer.write_u16(0x00000143); /* opcode */

    writer.write_u64(obj.attacker);

    writer.write_u64(obj.victim);

    return writer.m_buf;
}

static size_t SMSG_ATTACKSTOP_size(const SMSG_ATTACKSTOP& obj) {
    return 4 + ::wow_world_messages::util::wwm_packed_guid_size(obj.player) + ::wow_world_messages::util::wwm_packed_guid_size(obj.enemy);
}

SMSG_ATTACKSTOP SMSG_ATTACKSTOP_read(Reader& reader) {
    SMSG_ATTACKSTOP obj;

    obj.player = reader.read_packed_guid();

    obj.enemy = reader.read_packed_guid();

    obj.unknown1 = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_ATTACKSTOP::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_ATTACKSTOP_size(obj));

    writer.write_u16_be(SMSG_ATTACKSTOP_size(obj) + 2); /* size */

    writer.write_u16(0x00000144); /* opcode */

    writer.write_packed_guid(obj.player);

    writer.write_packed_guid(obj.enemy);

    writer.write_u32(obj.unknown1);

    return writer.m_buf;
}

std::vector<unsigned char> SMSG_ATTACKSWING_NOTINRANGE::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 2); /* size */

    writer.write_u16(0x00000145); /* opcode */

    return writer.m_buf;
}

std::vector<unsigned char> SMSG_ATTACKSWING_BADFACING::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 2); /* size */

    writer.write_u16(0x00000146); /* opcode */

    return writer.m_buf;
}

std::vector<unsigned char> SMSG_ATTACKSWING_NOTSTANDING::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 2); /* size */

    writer.write_u16(0x00000147); /* opcode */

    return writer.m_buf;
}

std::vector<unsigned char> SMSG_ATTACKSWING_DEADTARGET::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 2); /* size */

    writer.write_u16(0x00000148); /* opcode */

    return writer.m_buf;
}

std::vector<unsigned char> SMSG_ATTACKSWING_CANT_ATTACK::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 2); /* size */

    writer.write_u16(0x00000149); /* opcode */

    return writer.m_buf;
}

static size_t SMSG_ATTACKERSTATEUPDATE_size(const SMSG_ATTACKERSTATEUPDATE& obj) {
    return 25 + ::wow_world_messages::util::wwm_packed_guid_size(obj.attacker) + ::wow_world_messages::util::wwm_packed_guid_size(obj.target) + 20 * obj.damages.size();
}

SMSG_ATTACKERSTATEUPDATE SMSG_ATTACKERSTATEUPDATE_read(Reader& reader) {
    SMSG_ATTACKERSTATEUPDATE obj;

    obj.hit_info = static_cast<HitInfo>(reader.read_u32());

    obj.attacker = reader.read_packed_guid();

    obj.target = reader.read_packed_guid();

    obj.total_damage = reader.read_u32();

    obj.amount_of_damages = reader.read_u8();

    for (auto i = 0; i < obj.amount_of_damages; ++i) {
        obj.damages.push_back(::wow_world_messages::vanilla::DamageInfo_read(reader));
    }

    obj.damage_state = reader.read_u32();

    obj.unknown1 = reader.read_u32();

    obj.spell_id = reader.read_u32();

    obj.blocked_amount = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_ATTACKERSTATEUPDATE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_ATTACKERSTATEUPDATE_size(obj));

    writer.write_u16_be(SMSG_ATTACKERSTATEUPDATE_size(obj) + 2); /* size */

    writer.write_u16(0x0000014a); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.hit_info));

    writer.write_packed_guid(obj.attacker);

    writer.write_packed_guid(obj.target);

    writer.write_u32(obj.total_damage);

    writer.write_u8(obj.amount_of_damages);

    for (const auto& v : obj.damages) {
        DamageInfo_write(writer, v);
    }

    writer.write_u32(obj.damage_state);

    writer.write_u32(obj.unknown1);

    writer.write_u32(obj.spell_id);

    writer.write_u32(obj.blocked_amount);

    return writer.m_buf;
}

std::vector<unsigned char> SMSG_CANCEL_COMBAT::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 2); /* size */

    writer.write_u16(0x0000014e); /* opcode */

    return writer.m_buf;
}

static size_t SMSG_SPELLHEALLOG_size(const SMSG_SPELLHEALLOG& obj) {
    return 9 + ::wow_world_messages::util::wwm_packed_guid_size(obj.victim) + ::wow_world_messages::util::wwm_packed_guid_size(obj.caster);
}

SMSG_SPELLHEALLOG SMSG_SPELLHEALLOG_read(Reader& reader) {
    SMSG_SPELLHEALLOG obj;

    obj.victim = reader.read_packed_guid();

    obj.caster = reader.read_packed_guid();

    obj.id = reader.read_u32();

    obj.damage = reader.read_u32();

    obj.critical = reader.read_bool8();

    return obj;
}

std::vector<unsigned char> SMSG_SPELLHEALLOG::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SPELLHEALLOG_size(obj));

    writer.write_u16_be(SMSG_SPELLHEALLOG_size(obj) + 2); /* size */

    writer.write_u16(0x00000150); /* opcode */

    writer.write_packed_guid(obj.victim);

    writer.write_packed_guid(obj.caster);

    writer.write_u32(obj.id);

    writer.write_u32(obj.damage);

    writer.write_bool8(obj.critical);

    return writer.m_buf;
}

static size_t SMSG_SPELLENERGIZELOG_size(const SMSG_SPELLENERGIZELOG& obj) {
    return 12 + ::wow_world_messages::util::wwm_packed_guid_size(obj.victim) + ::wow_world_messages::util::wwm_packed_guid_size(obj.caster);
}

SMSG_SPELLENERGIZELOG SMSG_SPELLENERGIZELOG_read(Reader& reader) {
    SMSG_SPELLENERGIZELOG obj;

    obj.victim = reader.read_packed_guid();

    obj.caster = reader.read_packed_guid();

    obj.spell = reader.read_u32();

    obj.power = static_cast<Power>(reader.read_u32());

    obj.damage = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_SPELLENERGIZELOG::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SPELLENERGIZELOG_size(obj));

    writer.write_u16_be(SMSG_SPELLENERGIZELOG_size(obj) + 2); /* size */

    writer.write_u16(0x00000151); /* opcode */

    writer.write_packed_guid(obj.victim);

    writer.write_packed_guid(obj.caster);

    writer.write_u32(obj.spell);

    writer.write_u32(static_cast<uint32_t>(obj.power));

    writer.write_u32(obj.damage);

    return writer.m_buf;
}

SMSG_BINDPOINTUPDATE SMSG_BINDPOINTUPDATE_read(Reader& reader) {
    SMSG_BINDPOINTUPDATE obj;

    obj.position = ::wow_world_messages::all::Vector3d_read(reader);

    obj.map = static_cast<Map>(reader.read_u32());

    obj.area = static_cast<Area>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_BINDPOINTUPDATE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0014);

    writer.write_u16_be(0x0014 + 2); /* size */

    writer.write_u16(0x00000155); /* opcode */

    Vector3d_write(writer, obj.position);

    writer.write_u32(static_cast<uint32_t>(obj.map));

    writer.write_u32(static_cast<uint32_t>(obj.area));

    return writer.m_buf;
}

SMSG_PLAYERBOUND SMSG_PLAYERBOUND_read(Reader& reader) {
    SMSG_PLAYERBOUND obj;

    obj.guid = reader.read_u64();

    obj.area = static_cast<Area>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_PLAYERBOUND::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 2); /* size */

    writer.write_u16(0x00000158); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(static_cast<uint32_t>(obj.area));

    return writer.m_buf;
}

static size_t SMSG_CLIENT_CONTROL_UPDATE_size(const SMSG_CLIENT_CONTROL_UPDATE& obj) {
    return 1 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_CLIENT_CONTROL_UPDATE SMSG_CLIENT_CONTROL_UPDATE_read(Reader& reader) {
    SMSG_CLIENT_CONTROL_UPDATE obj;

    obj.guid = reader.read_packed_guid();

    obj.allow_movement = reader.read_bool8();

    return obj;
}

std::vector<unsigned char> SMSG_CLIENT_CONTROL_UPDATE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_CLIENT_CONTROL_UPDATE_size(obj));

    writer.write_u16_be(SMSG_CLIENT_CONTROL_UPDATE_size(obj) + 2); /* size */

    writer.write_u16(0x00000159); /* opcode */

    writer.write_packed_guid(obj.guid);

    writer.write_bool8(obj.allow_movement);

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_REPOP_REQUEST::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x0000015a); /* opcode */

    return writer.m_buf;
}

static size_t SMSG_RESURRECT_REQUEST_size(const SMSG_RESURRECT_REQUEST& obj) {
    return 14 + obj.name.size();
}

SMSG_RESURRECT_REQUEST SMSG_RESURRECT_REQUEST_read(Reader& reader) {
    SMSG_RESURRECT_REQUEST obj;

    obj.guid = reader.read_u64();

    obj.name = reader.read_sized_cstring();

    obj.player = reader.read_bool8();

    return obj;
}

std::vector<unsigned char> SMSG_RESURRECT_REQUEST::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_RESURRECT_REQUEST_size(obj));

    writer.write_u16_be(SMSG_RESURRECT_REQUEST_size(obj) + 2); /* size */

    writer.write_u16(0x0000015b); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_sized_cstring(obj.name);

    writer.write_bool8(obj.player);

    return writer.m_buf;
}

CMSG_RESURRECT_RESPONSE CMSG_RESURRECT_RESPONSE_read(Reader& reader) {
    CMSG_RESURRECT_RESPONSE obj;

    obj.guid = reader.read_u64();

    obj.status = reader.read_u8();

    return obj;
}

std::vector<unsigned char> CMSG_RESURRECT_RESPONSE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0009);

    writer.write_u16_be(0x0009 + 4); /* size */

    writer.write_u32(0x0000015c); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u8(obj.status);

    return writer.m_buf;
}

CMSG_LOOT CMSG_LOOT_read(Reader& reader) {
    CMSG_LOOT obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_LOOT::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x0000015d); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_LOOT_MONEY::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x0000015e); /* opcode */

    return writer.m_buf;
}

CMSG_LOOT_RELEASE CMSG_LOOT_RELEASE_read(Reader& reader) {
    CMSG_LOOT_RELEASE obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_LOOT_RELEASE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x0000015f); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

static size_t SMSG_LOOT_RESPONSE_size(const SMSG_LOOT_RESPONSE& obj) {
    size_t _size = 14 + 6 * obj.items.size();

    if (obj.loot_method == LootMethod::ERROR) {
        _size += 1;
    }

    return _size;
}

SMSG_LOOT_RESPONSE SMSG_LOOT_RESPONSE_read(Reader& reader) {
    SMSG_LOOT_RESPONSE obj;

    obj.guid = reader.read_u64();

    obj.loot_method = static_cast<LootMethod>(reader.read_u8());

    if (obj.loot_method == LootMethod::ERROR) {
        obj.loot_error = static_cast<LootMethodError>(reader.read_u8());

    }
    obj.gold = reader.read_u32();

    obj.amount_of_items = reader.read_u8();

    for (auto i = 0; i < obj.amount_of_items; ++i) {
        obj.items.push_back(::wow_world_messages::vanilla::LootItem_read(reader));
    }

    return obj;
}

std::vector<unsigned char> SMSG_LOOT_RESPONSE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_LOOT_RESPONSE_size(obj));

    writer.write_u16_be(SMSG_LOOT_RESPONSE_size(obj) + 2); /* size */

    writer.write_u16(0x00000160); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u8(static_cast<uint8_t>(obj.loot_method));

    if (obj.loot_method == LootMethod::ERROR) {
        writer.write_u8(static_cast<uint8_t>(obj.loot_error));

    }
    writer.write_u32(obj.gold);

    writer.write_u8(obj.amount_of_items);

    for (const auto& v : obj.items) {
        LootItem_write(writer, v);
    }

    return writer.m_buf;
}

SMSG_LOOT_RELEASE_RESPONSE SMSG_LOOT_RELEASE_RESPONSE_read(Reader& reader) {
    SMSG_LOOT_RELEASE_RESPONSE obj;

    obj.guid = reader.read_u64();

    obj.unknown1 = reader.read_u8();

    return obj;
}

std::vector<unsigned char> SMSG_LOOT_RELEASE_RESPONSE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0009);

    writer.write_u16_be(0x0009 + 2); /* size */

    writer.write_u16(0x00000161); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u8(obj.unknown1);

    return writer.m_buf;
}

SMSG_LOOT_REMOVED SMSG_LOOT_REMOVED_read(Reader& reader) {
    SMSG_LOOT_REMOVED obj;

    obj.slot = reader.read_u8();

    return obj;
}

std::vector<unsigned char> SMSG_LOOT_REMOVED::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0001);

    writer.write_u16_be(0x0001 + 2); /* size */

    writer.write_u16(0x00000162); /* opcode */

    writer.write_u8(obj.slot);

    return writer.m_buf;
}

SMSG_LOOT_MONEY_NOTIFY SMSG_LOOT_MONEY_NOTIFY_read(Reader& reader) {
    SMSG_LOOT_MONEY_NOTIFY obj;

    obj.amount = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_LOOT_MONEY_NOTIFY::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x00000163); /* opcode */

    writer.write_u32(obj.amount);

    return writer.m_buf;
}

std::vector<unsigned char> SMSG_LOOT_CLEAR_MONEY::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 2); /* size */

    writer.write_u16(0x00000165); /* opcode */

    return writer.m_buf;
}

SMSG_ITEM_PUSH_RESULT SMSG_ITEM_PUSH_RESULT_read(Reader& reader) {
    SMSG_ITEM_PUSH_RESULT obj;

    obj.guid = reader.read_u64();

    obj.source = static_cast<NewItemSource>(reader.read_u32());

    obj.creation_type = static_cast<NewItemCreationType>(reader.read_u32());

    obj.alert_chat = static_cast<NewItemChatAlert>(reader.read_u32());

    obj.bag_slot = reader.read_u8();

    obj.item_slot = reader.read_u32();

    obj.item = reader.read_u32();

    obj.item_suffix_factor = reader.read_u32();

    obj.item_random_property_id = reader.read_u32();

    obj.item_count = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_ITEM_PUSH_RESULT::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0029);

    writer.write_u16_be(0x0029 + 2); /* size */

    writer.write_u16(0x00000166); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(static_cast<uint32_t>(obj.source));

    writer.write_u32(static_cast<uint32_t>(obj.creation_type));

    writer.write_u32(static_cast<uint32_t>(obj.alert_chat));

    writer.write_u8(obj.bag_slot);

    writer.write_u32(obj.item_slot);

    writer.write_u32(obj.item);

    writer.write_u32(obj.item_suffix_factor);

    writer.write_u32(obj.item_random_property_id);

    writer.write_u32(obj.item_count);

    return writer.m_buf;
}

SMSG_DUEL_REQUESTED SMSG_DUEL_REQUESTED_read(Reader& reader) {
    SMSG_DUEL_REQUESTED obj;

    obj.initiator = reader.read_u64();

    obj.target = reader.read_u64();

    return obj;
}

std::vector<unsigned char> SMSG_DUEL_REQUESTED::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0010);

    writer.write_u16_be(0x0010 + 2); /* size */

    writer.write_u16(0x00000167); /* opcode */

    writer.write_u64(obj.initiator);

    writer.write_u64(obj.target);

    return writer.m_buf;
}

std::vector<unsigned char> SMSG_DUEL_OUTOFBOUNDS::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 2); /* size */

    writer.write_u16(0x00000168); /* opcode */

    return writer.m_buf;
}

std::vector<unsigned char> SMSG_DUEL_INBOUNDS::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 2); /* size */

    writer.write_u16(0x00000169); /* opcode */

    return writer.m_buf;
}

SMSG_DUEL_COMPLETE SMSG_DUEL_COMPLETE_read(Reader& reader) {
    SMSG_DUEL_COMPLETE obj;

    obj.ended_without_interruption = reader.read_bool8();

    return obj;
}

std::vector<unsigned char> SMSG_DUEL_COMPLETE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0001);

    writer.write_u16_be(0x0001 + 2); /* size */

    writer.write_u16(0x0000016a); /* opcode */

    writer.write_bool8(obj.ended_without_interruption);

    return writer.m_buf;
}

static size_t SMSG_DUEL_WINNER_size(const SMSG_DUEL_WINNER& obj) {
    return 3 + obj.opponent_name.size() + obj.initiator_name.size();
}

SMSG_DUEL_WINNER SMSG_DUEL_WINNER_read(Reader& reader) {
    SMSG_DUEL_WINNER obj;

    obj.reason = static_cast<DuelWinnerReason>(reader.read_u8());

    obj.opponent_name = reader.read_cstring();

    obj.initiator_name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> SMSG_DUEL_WINNER::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_DUEL_WINNER_size(obj));

    writer.write_u16_be(SMSG_DUEL_WINNER_size(obj) + 2); /* size */

    writer.write_u16(0x0000016b); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.reason));

    writer.write_cstring(obj.opponent_name);

    writer.write_cstring(obj.initiator_name);

    return writer.m_buf;
}

CMSG_DUEL_ACCEPTED CMSG_DUEL_ACCEPTED_read(Reader& reader) {
    CMSG_DUEL_ACCEPTED obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_DUEL_ACCEPTED::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x0000016c); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

CMSG_DUEL_CANCELLED CMSG_DUEL_CANCELLED_read(Reader& reader) {
    CMSG_DUEL_CANCELLED obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_DUEL_CANCELLED::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x0000016d); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

SMSG_MOUNTRESULT SMSG_MOUNTRESULT_read(Reader& reader) {
    SMSG_MOUNTRESULT obj;

    obj.result = static_cast<MountResult>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_MOUNTRESULT::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x0000016e); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.result));

    return writer.m_buf;
}

SMSG_DISMOUNTRESULT SMSG_DISMOUNTRESULT_read(Reader& reader) {
    SMSG_DISMOUNTRESULT obj;

    obj.result = static_cast<DismountResult>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_DISMOUNTRESULT::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x0000016f); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.result));

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_MOUNTSPECIAL_ANIM::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x00000171); /* opcode */

    return writer.m_buf;
}

SMSG_MOUNTSPECIAL_ANIM SMSG_MOUNTSPECIAL_ANIM_read(Reader& reader) {
    SMSG_MOUNTSPECIAL_ANIM obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> SMSG_MOUNTSPECIAL_ANIM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 2); /* size */

    writer.write_u16(0x00000172); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

SMSG_PET_TAME_FAILURE SMSG_PET_TAME_FAILURE_read(Reader& reader) {
    SMSG_PET_TAME_FAILURE obj;

    obj.reason = static_cast<PetTameFailureReason>(reader.read_u8());

    return obj;
}

std::vector<unsigned char> SMSG_PET_TAME_FAILURE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0001);

    writer.write_u16_be(0x0001 + 2); /* size */

    writer.write_u16(0x00000173); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.reason));

    return writer.m_buf;
}

CMSG_PET_ACTION CMSG_PET_ACTION_read(Reader& reader) {
    CMSG_PET_ACTION obj;

    obj.pet = reader.read_u64();

    obj.data = reader.read_u32();

    obj.target = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_PET_ACTION::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0014);

    writer.write_u16_be(0x0014 + 4); /* size */

    writer.write_u32(0x00000175); /* opcode */

    writer.write_u64(obj.pet);

    writer.write_u32(obj.data);

    writer.write_u64(obj.target);

    return writer.m_buf;
}

CMSG_PET_ABANDON CMSG_PET_ABANDON_read(Reader& reader) {
    CMSG_PET_ABANDON obj;

    obj.pet = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_PET_ABANDON::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x00000176); /* opcode */

    writer.write_u64(obj.pet);

    return writer.m_buf;
}

static size_t CMSG_PET_RENAME_size(const CMSG_PET_RENAME& obj) {
    return 9 + obj.name.size();
}

CMSG_PET_RENAME CMSG_PET_RENAME_read(Reader& reader) {
    CMSG_PET_RENAME obj;

    obj.pet = reader.read_u64();

    obj.name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_PET_RENAME::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_PET_RENAME_size(obj));

    writer.write_u16_be((uint16_t)CMSG_PET_RENAME_size(obj) + 4); /* size */

    writer.write_u32(0x00000177); /* opcode */

    writer.write_u64(obj.pet);

    writer.write_cstring(obj.name);

    return writer.m_buf;
}

std::vector<unsigned char> SMSG_PET_NAME_INVALID::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 2); /* size */

    writer.write_u16(0x00000178); /* opcode */

    return writer.m_buf;
}

SMSG_PET_MODE SMSG_PET_MODE_read(Reader& reader) {
    SMSG_PET_MODE obj;

    obj.guid = reader.read_u64();

    obj.react_state = static_cast<PetReactState>(reader.read_u8());

    obj.command_state = static_cast<PetCommandState>(reader.read_u8());

    obj.unknown1 = reader.read_u8();

    obj.pet_enabled = static_cast<PetEnabled>(reader.read_u8());

    return obj;
}

std::vector<unsigned char> SMSG_PET_MODE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 2); /* size */

    writer.write_u16(0x0000017a); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u8(static_cast<uint8_t>(obj.react_state));

    writer.write_u8(static_cast<uint8_t>(obj.command_state));

    writer.write_u8(obj.unknown1);

    writer.write_u8(static_cast<uint8_t>(obj.pet_enabled));

    return writer.m_buf;
}

CMSG_GOSSIP_HELLO CMSG_GOSSIP_HELLO_read(Reader& reader) {
    CMSG_GOSSIP_HELLO obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_GOSSIP_HELLO::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x0000017b); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

static size_t SMSG_GOSSIP_MESSAGE_size(const SMSG_GOSSIP_MESSAGE& obj) {
    size_t _size = 20;

    for(const auto& v : obj.gossips) {
        _size += GossipItem_size(v);
    }

    for(const auto& v : obj.quests) {
        _size += QuestItem_size(v);
    }

    return _size;
}

SMSG_GOSSIP_MESSAGE SMSG_GOSSIP_MESSAGE_read(Reader& reader) {
    SMSG_GOSSIP_MESSAGE obj;

    obj.guid = reader.read_u64();

    obj.title_text_id = reader.read_u32();

    obj.amount_of_gossip_items = reader.read_u32();

    for (auto i = 0; i < obj.amount_of_gossip_items; ++i) {
        obj.gossips.push_back(::wow_world_messages::vanilla::GossipItem_read(reader));
    }

    obj.amount_of_quests = reader.read_u32();

    for (auto i = 0; i < obj.amount_of_quests; ++i) {
        obj.quests.push_back(::wow_world_messages::vanilla::QuestItem_read(reader));
    }

    return obj;
}

std::vector<unsigned char> SMSG_GOSSIP_MESSAGE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_GOSSIP_MESSAGE_size(obj));

    writer.write_u16_be(SMSG_GOSSIP_MESSAGE_size(obj) + 2); /* size */

    writer.write_u16(0x0000017d); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.title_text_id);

    writer.write_u32(obj.amount_of_gossip_items);

    for (const auto& v : obj.gossips) {
        GossipItem_write(writer, v);
    }

    writer.write_u32(obj.amount_of_quests);

    for (const auto& v : obj.quests) {
        QuestItem_write(writer, v);
    }

    return writer.m_buf;
}

std::vector<unsigned char> SMSG_GOSSIP_COMPLETE::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 2); /* size */

    writer.write_u16(0x0000017e); /* opcode */

    return writer.m_buf;
}

CMSG_NPC_TEXT_QUERY CMSG_NPC_TEXT_QUERY_read(Reader& reader) {
    CMSG_NPC_TEXT_QUERY obj;

    obj.text_id = reader.read_u32();

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_NPC_TEXT_QUERY::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 4); /* size */

    writer.write_u32(0x0000017f); /* opcode */

    writer.write_u32(obj.text_id);

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

static size_t SMSG_NPC_TEXT_UPDATE_size(const SMSG_NPC_TEXT_UPDATE& obj) {
    size_t _size = 4;

    for(const auto& v : obj.texts) {
        _size += NpcTextUpdate_size(v);
    }

    return _size;
}

SMSG_NPC_TEXT_UPDATE SMSG_NPC_TEXT_UPDATE_read(Reader& reader) {
    SMSG_NPC_TEXT_UPDATE obj;

    obj.text_id = reader.read_u32();

    for (auto i = 0; i < 8; ++i) {
        obj.texts[i] = ::wow_world_messages::vanilla::NpcTextUpdate_read(reader);
    }

    return obj;
}

std::vector<unsigned char> SMSG_NPC_TEXT_UPDATE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_NPC_TEXT_UPDATE_size(obj));

    writer.write_u16_be(SMSG_NPC_TEXT_UPDATE_size(obj) + 2); /* size */

    writer.write_u16(0x00000180); /* opcode */

    writer.write_u32(obj.text_id);

    for (const auto& v : obj.texts) {
        NpcTextUpdate_write(writer, v);
    }

    return writer.m_buf;
}

CMSG_QUESTGIVER_STATUS_QUERY CMSG_QUESTGIVER_STATUS_QUERY_read(Reader& reader) {
    CMSG_QUESTGIVER_STATUS_QUERY obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_QUESTGIVER_STATUS_QUERY::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x00000182); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

SMSG_QUESTGIVER_STATUS SMSG_QUESTGIVER_STATUS_read(Reader& reader) {
    SMSG_QUESTGIVER_STATUS obj;

    obj.guid = reader.read_u64();

    obj.status = static_cast<QuestGiverStatus>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_QUESTGIVER_STATUS::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 2); /* size */

    writer.write_u16(0x00000183); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(static_cast<uint32_t>(obj.status));

    return writer.m_buf;
}

CMSG_QUESTGIVER_HELLO CMSG_QUESTGIVER_HELLO_read(Reader& reader) {
    CMSG_QUESTGIVER_HELLO obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_QUESTGIVER_HELLO::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x00000184); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

static size_t SMSG_QUESTGIVER_QUEST_LIST_size(const SMSG_QUESTGIVER_QUEST_LIST& obj) {
    size_t _size = 18 + obj.title.size();

    for(const auto& v : obj.quest_items) {
        _size += QuestItem_size(v);
    }

    return _size;
}

SMSG_QUESTGIVER_QUEST_LIST SMSG_QUESTGIVER_QUEST_LIST_read(Reader& reader) {
    SMSG_QUESTGIVER_QUEST_LIST obj;

    obj.npc = reader.read_u64();

    obj.title = reader.read_cstring();

    obj.emote_delay = reader.read_u32();

    obj.emote = reader.read_u32();

    obj.amount_of_entries = reader.read_u8();

    for (auto i = 0; i < obj.amount_of_entries; ++i) {
        obj.quest_items.push_back(::wow_world_messages::vanilla::QuestItem_read(reader));
    }

    return obj;
}

std::vector<unsigned char> SMSG_QUESTGIVER_QUEST_LIST::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_QUESTGIVER_QUEST_LIST_size(obj));

    writer.write_u16_be(SMSG_QUESTGIVER_QUEST_LIST_size(obj) + 2); /* size */

    writer.write_u16(0x00000185); /* opcode */

    writer.write_u64(obj.npc);

    writer.write_cstring(obj.title);

    writer.write_u32(obj.emote_delay);

    writer.write_u32(obj.emote);

    writer.write_u8(obj.amount_of_entries);

    for (const auto& v : obj.quest_items) {
        QuestItem_write(writer, v);
    }

    return writer.m_buf;
}

CMSG_QUESTGIVER_QUERY_QUEST CMSG_QUESTGIVER_QUERY_QUEST_read(Reader& reader) {
    CMSG_QUESTGIVER_QUERY_QUEST obj;

    obj.guid = reader.read_u64();

    obj.quest_id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_QUESTGIVER_QUERY_QUEST::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 4); /* size */

    writer.write_u32(0x00000186); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.quest_id);

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_QUESTGIVER_QUEST_AUTOLAUNCH::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x00000187); /* opcode */

    return writer.m_buf;
}

static size_t SMSG_QUESTGIVER_QUEST_DETAILS_size(const SMSG_QUESTGIVER_QUEST_DETAILS& obj) {
    return 39 + obj.title.size() + obj.details.size() + obj.objectives.size() + 8 * obj.choice_item_rewards.size() + 8 * obj.item_rewards.size() + 8 * obj.emotes.size();
}

SMSG_QUESTGIVER_QUEST_DETAILS SMSG_QUESTGIVER_QUEST_DETAILS_read(Reader& reader) {
    SMSG_QUESTGIVER_QUEST_DETAILS obj;

    obj.guid = reader.read_u64();

    obj.quest_id = reader.read_u32();

    obj.title = reader.read_cstring();

    obj.details = reader.read_cstring();

    obj.objectives = reader.read_cstring();

    obj.auto_finish = reader.read_bool32();

    obj.amount_of_choice_item_rewards = reader.read_u32();

    for (auto i = 0; i < obj.amount_of_choice_item_rewards; ++i) {
        obj.choice_item_rewards.push_back(::wow_world_messages::vanilla::QuestItemReward_read(reader));
    }

    obj.amount_of_item_rewards = reader.read_u32();

    for (auto i = 0; i < obj.amount_of_item_rewards; ++i) {
        obj.item_rewards.push_back(::wow_world_messages::vanilla::QuestItemReward_read(reader));
    }

    obj.money_reward = reader.read_u32();

    obj.reward_spell = reader.read_u32();

    obj.amount_of_emotes = reader.read_u32();

    for (auto i = 0; i < obj.amount_of_emotes; ++i) {
        obj.emotes.push_back(::wow_world_messages::vanilla::QuestDetailsEmote_read(reader));
    }

    return obj;
}

std::vector<unsigned char> SMSG_QUESTGIVER_QUEST_DETAILS::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_QUESTGIVER_QUEST_DETAILS_size(obj));

    writer.write_u16_be(SMSG_QUESTGIVER_QUEST_DETAILS_size(obj) + 2); /* size */

    writer.write_u16(0x00000188); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.quest_id);

    writer.write_cstring(obj.title);

    writer.write_cstring(obj.details);

    writer.write_cstring(obj.objectives);

    writer.write_bool32(obj.auto_finish);

    writer.write_u32(obj.amount_of_choice_item_rewards);

    for (const auto& v : obj.choice_item_rewards) {
        QuestItemReward_write(writer, v);
    }

    writer.write_u32(obj.amount_of_item_rewards);

    for (const auto& v : obj.item_rewards) {
        QuestItemReward_write(writer, v);
    }

    writer.write_u32(obj.money_reward);

    writer.write_u32(obj.reward_spell);

    writer.write_u32(obj.amount_of_emotes);

    for (const auto& v : obj.emotes) {
        QuestDetailsEmote_write(writer, v);
    }

    return writer.m_buf;
}

CMSG_QUESTGIVER_ACCEPT_QUEST CMSG_QUESTGIVER_ACCEPT_QUEST_read(Reader& reader) {
    CMSG_QUESTGIVER_ACCEPT_QUEST obj;

    obj.guid = reader.read_u64();

    obj.quest_id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_QUESTGIVER_ACCEPT_QUEST::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 4); /* size */

    writer.write_u32(0x00000189); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.quest_id);

    return writer.m_buf;
}

CMSG_QUESTGIVER_COMPLETE_QUEST CMSG_QUESTGIVER_COMPLETE_QUEST_read(Reader& reader) {
    CMSG_QUESTGIVER_COMPLETE_QUEST obj;

    obj.guid = reader.read_u64();

    obj.quest_id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_QUESTGIVER_COMPLETE_QUEST::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 4); /* size */

    writer.write_u32(0x0000018a); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.quest_id);

    return writer.m_buf;
}

static size_t SMSG_QUESTGIVER_REQUEST_ITEMS_size(const SMSG_QUESTGIVER_REQUEST_ITEMS& obj) {
    return 50 + obj.title.size() + obj.request_items_text.size() + 12 * obj.required_items.size();
}

SMSG_QUESTGIVER_REQUEST_ITEMS SMSG_QUESTGIVER_REQUEST_ITEMS_read(Reader& reader) {
    SMSG_QUESTGIVER_REQUEST_ITEMS obj;

    obj.npc = reader.read_u64();

    obj.quest_id = reader.read_u32();

    obj.title = reader.read_cstring();

    obj.request_items_text = reader.read_cstring();

    obj.emote_delay = reader.read_u32();

    obj.emote = reader.read_u32();

    obj.auto_finish = reader.read_bool32();

    obj.required_money = reader.read_u32();

    obj.amount_of_required_items = reader.read_u32();

    for (auto i = 0; i < obj.amount_of_required_items; ++i) {
        obj.required_items.push_back(::wow_world_messages::vanilla::QuestItemRequirement_read(reader));
    }

    obj.unknown1 = reader.read_u32();

    obj.completable = static_cast<QuestCompletable>(reader.read_u32());

    obj.flags2 = reader.read_u32();

    obj.flags3 = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_QUESTGIVER_REQUEST_ITEMS::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_QUESTGIVER_REQUEST_ITEMS_size(obj));

    writer.write_u16_be(SMSG_QUESTGIVER_REQUEST_ITEMS_size(obj) + 2); /* size */

    writer.write_u16(0x0000018b); /* opcode */

    writer.write_u64(obj.npc);

    writer.write_u32(obj.quest_id);

    writer.write_cstring(obj.title);

    writer.write_cstring(obj.request_items_text);

    writer.write_u32(obj.emote_delay);

    writer.write_u32(obj.emote);

    writer.write_bool32(obj.auto_finish);

    writer.write_u32(obj.required_money);

    writer.write_u32(obj.amount_of_required_items);

    for (const auto& v : obj.required_items) {
        QuestItemRequirement_write(writer, v);
    }

    writer.write_u32(obj.unknown1);

    writer.write_u32(static_cast<uint32_t>(obj.completable));

    writer.write_u32(obj.flags2);

    writer.write_u32(obj.flags3);

    return writer.m_buf;
}

CMSG_QUESTGIVER_REQUEST_REWARD CMSG_QUESTGIVER_REQUEST_REWARD_read(Reader& reader) {
    CMSG_QUESTGIVER_REQUEST_REWARD obj;

    obj.guid = reader.read_u64();

    obj.quest_id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_QUESTGIVER_REQUEST_REWARD::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 4); /* size */

    writer.write_u32(0x0000018c); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.quest_id);

    return writer.m_buf;
}

static size_t SMSG_QUESTGIVER_OFFER_REWARD_size(const SMSG_QUESTGIVER_OFFER_REWARD& obj) {
    return 42 + obj.title.size() + obj.offer_reward_text.size() + 8 * obj.emotes.size() + 12 * obj.choice_item_rewards.size() + 12 * obj.item_rewards.size();
}

SMSG_QUESTGIVER_OFFER_REWARD SMSG_QUESTGIVER_OFFER_REWARD_read(Reader& reader) {
    SMSG_QUESTGIVER_OFFER_REWARD obj;

    obj.npc = reader.read_u64();

    obj.quest_id = reader.read_u32();

    obj.title = reader.read_cstring();

    obj.offer_reward_text = reader.read_cstring();

    obj.auto_finish = reader.read_bool32();

    obj.amount_of_emotes = reader.read_u32();

    for (auto i = 0; i < obj.amount_of_emotes; ++i) {
        obj.emotes.push_back(::wow_world_messages::vanilla::NpcTextUpdateEmote_read(reader));
    }

    obj.amount_of_choice_item_rewards = reader.read_u32();

    for (auto i = 0; i < obj.amount_of_choice_item_rewards; ++i) {
        obj.choice_item_rewards.push_back(::wow_world_messages::vanilla::QuestItemRequirement_read(reader));
    }

    obj.amount_of_item_rewards = reader.read_u32();

    for (auto i = 0; i < obj.amount_of_item_rewards; ++i) {
        obj.item_rewards.push_back(::wow_world_messages::vanilla::QuestItemRequirement_read(reader));
    }

    obj.money_reward = reader.read_u32();

    obj.reward_spell = reader.read_u32();

    obj.reward_spell_cast = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_QUESTGIVER_OFFER_REWARD::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_QUESTGIVER_OFFER_REWARD_size(obj));

    writer.write_u16_be(SMSG_QUESTGIVER_OFFER_REWARD_size(obj) + 2); /* size */

    writer.write_u16(0x0000018d); /* opcode */

    writer.write_u64(obj.npc);

    writer.write_u32(obj.quest_id);

    writer.write_cstring(obj.title);

    writer.write_cstring(obj.offer_reward_text);

    writer.write_bool32(obj.auto_finish);

    writer.write_u32(obj.amount_of_emotes);

    for (const auto& v : obj.emotes) {
        NpcTextUpdateEmote_write(writer, v);
    }

    writer.write_u32(obj.amount_of_choice_item_rewards);

    for (const auto& v : obj.choice_item_rewards) {
        QuestItemRequirement_write(writer, v);
    }

    writer.write_u32(obj.amount_of_item_rewards);

    for (const auto& v : obj.item_rewards) {
        QuestItemRequirement_write(writer, v);
    }

    writer.write_u32(obj.money_reward);

    writer.write_u32(obj.reward_spell);

    writer.write_u32(obj.reward_spell_cast);

    return writer.m_buf;
}

CMSG_QUESTGIVER_CHOOSE_REWARD CMSG_QUESTGIVER_CHOOSE_REWARD_read(Reader& reader) {
    CMSG_QUESTGIVER_CHOOSE_REWARD obj;

    obj.guid = reader.read_u64();

    obj.quest_id = reader.read_u32();

    obj.reward = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_QUESTGIVER_CHOOSE_REWARD::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0010);

    writer.write_u16_be(0x0010 + 4); /* size */

    writer.write_u32(0x0000018e); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.quest_id);

    writer.write_u32(obj.reward);

    return writer.m_buf;
}

SMSG_QUESTGIVER_QUEST_INVALID SMSG_QUESTGIVER_QUEST_INVALID_read(Reader& reader) {
    SMSG_QUESTGIVER_QUEST_INVALID obj;

    obj.msg = static_cast<QuestFailedReason>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_QUESTGIVER_QUEST_INVALID::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x0000018f); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.msg));

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_QUESTGIVER_CANCEL::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x00000190); /* opcode */

    return writer.m_buf;
}

static size_t SMSG_QUESTGIVER_QUEST_COMPLETE_size(const SMSG_QUESTGIVER_QUEST_COMPLETE& obj) {
    return 20 + 8 * obj.item_rewards.size();
}

SMSG_QUESTGIVER_QUEST_COMPLETE SMSG_QUESTGIVER_QUEST_COMPLETE_read(Reader& reader) {
    SMSG_QUESTGIVER_QUEST_COMPLETE obj;

    obj.quest_id = reader.read_u32();

    obj.unknown = reader.read_u32();

    obj.experience_reward = reader.read_u32();

    obj.money_reward = reader.read_u32();

    obj.amount_of_item_rewards = reader.read_u32();

    for (auto i = 0; i < obj.amount_of_item_rewards; ++i) {
        obj.item_rewards.push_back(::wow_world_messages::vanilla::QuestItemReward_read(reader));
    }

    return obj;
}

std::vector<unsigned char> SMSG_QUESTGIVER_QUEST_COMPLETE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_QUESTGIVER_QUEST_COMPLETE_size(obj));

    writer.write_u16_be(SMSG_QUESTGIVER_QUEST_COMPLETE_size(obj) + 2); /* size */

    writer.write_u16(0x00000191); /* opcode */

    writer.write_u32(obj.quest_id);

    writer.write_u32(obj.unknown);

    writer.write_u32(obj.experience_reward);

    writer.write_u32(obj.money_reward);

    writer.write_u32(obj.amount_of_item_rewards);

    for (const auto& v : obj.item_rewards) {
        QuestItemReward_write(writer, v);
    }

    return writer.m_buf;
}

SMSG_QUESTGIVER_QUEST_FAILED SMSG_QUESTGIVER_QUEST_FAILED_read(Reader& reader) {
    SMSG_QUESTGIVER_QUEST_FAILED obj;

    obj.quest_id = reader.read_u32();

    obj.reason = static_cast<QuestFailedReason>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_QUESTGIVER_QUEST_FAILED::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 2); /* size */

    writer.write_u16(0x00000192); /* opcode */

    writer.write_u32(obj.quest_id);

    writer.write_u32(static_cast<uint32_t>(obj.reason));

    return writer.m_buf;
}

CMSG_QUESTLOG_SWAP_QUEST CMSG_QUESTLOG_SWAP_QUEST_read(Reader& reader) {
    CMSG_QUESTLOG_SWAP_QUEST obj;

    obj.slot1 = reader.read_u8();

    obj.slot2 = reader.read_u8();

    return obj;
}

std::vector<unsigned char> CMSG_QUESTLOG_SWAP_QUEST::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0002);

    writer.write_u16_be(0x0002 + 4); /* size */

    writer.write_u32(0x00000193); /* opcode */

    writer.write_u8(obj.slot1);

    writer.write_u8(obj.slot2);

    return writer.m_buf;
}

CMSG_QUESTLOG_REMOVE_QUEST CMSG_QUESTLOG_REMOVE_QUEST_read(Reader& reader) {
    CMSG_QUESTLOG_REMOVE_QUEST obj;

    obj.slot = reader.read_u8();

    return obj;
}

std::vector<unsigned char> CMSG_QUESTLOG_REMOVE_QUEST::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0001);

    writer.write_u16_be(0x0001 + 4); /* size */

    writer.write_u32(0x00000194); /* opcode */

    writer.write_u8(obj.slot);

    return writer.m_buf;
}

std::vector<unsigned char> SMSG_QUESTLOG_FULL::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 2); /* size */

    writer.write_u16(0x00000195); /* opcode */

    return writer.m_buf;
}

SMSG_QUESTUPDATE_FAILED SMSG_QUESTUPDATE_FAILED_read(Reader& reader) {
    SMSG_QUESTUPDATE_FAILED obj;

    obj.quest_id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_QUESTUPDATE_FAILED::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x00000196); /* opcode */

    writer.write_u32(obj.quest_id);

    return writer.m_buf;
}

SMSG_QUESTUPDATE_FAILEDTIMER SMSG_QUESTUPDATE_FAILEDTIMER_read(Reader& reader) {
    SMSG_QUESTUPDATE_FAILEDTIMER obj;

    obj.quest_id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_QUESTUPDATE_FAILEDTIMER::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x00000197); /* opcode */

    writer.write_u32(obj.quest_id);

    return writer.m_buf;
}

SMSG_QUESTUPDATE_COMPLETE SMSG_QUESTUPDATE_COMPLETE_read(Reader& reader) {
    SMSG_QUESTUPDATE_COMPLETE obj;

    obj.quest_id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_QUESTUPDATE_COMPLETE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x00000198); /* opcode */

    writer.write_u32(obj.quest_id);

    return writer.m_buf;
}

SMSG_QUESTUPDATE_ADD_KILL SMSG_QUESTUPDATE_ADD_KILL_read(Reader& reader) {
    SMSG_QUESTUPDATE_ADD_KILL obj;

    obj.quest_id = reader.read_u32();

    obj.creature_id = reader.read_u32();

    obj.kill_count = reader.read_u32();

    obj.required_kill_count = reader.read_u32();

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> SMSG_QUESTUPDATE_ADD_KILL::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0018);

    writer.write_u16_be(0x0018 + 2); /* size */

    writer.write_u16(0x00000199); /* opcode */

    writer.write_u32(obj.quest_id);

    writer.write_u32(obj.creature_id);

    writer.write_u32(obj.kill_count);

    writer.write_u32(obj.required_kill_count);

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

SMSG_QUESTUPDATE_ADD_ITEM SMSG_QUESTUPDATE_ADD_ITEM_read(Reader& reader) {
    SMSG_QUESTUPDATE_ADD_ITEM obj;

    obj.required_item_id = reader.read_u32();

    obj.items_required = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_QUESTUPDATE_ADD_ITEM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 2); /* size */

    writer.write_u16(0x0000019a); /* opcode */

    writer.write_u32(obj.required_item_id);

    writer.write_u32(obj.items_required);

    return writer.m_buf;
}

CMSG_QUEST_CONFIRM_ACCEPT CMSG_QUEST_CONFIRM_ACCEPT_read(Reader& reader) {
    CMSG_QUEST_CONFIRM_ACCEPT obj;

    obj.quest_id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_QUEST_CONFIRM_ACCEPT::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 4); /* size */

    writer.write_u32(0x0000019b); /* opcode */

    writer.write_u32(obj.quest_id);

    return writer.m_buf;
}

static size_t SMSG_QUEST_CONFIRM_ACCEPT_size(const SMSG_QUEST_CONFIRM_ACCEPT& obj) {
    return 13 + obj.quest_title.size();
}

SMSG_QUEST_CONFIRM_ACCEPT SMSG_QUEST_CONFIRM_ACCEPT_read(Reader& reader) {
    SMSG_QUEST_CONFIRM_ACCEPT obj;

    obj.quest_id = reader.read_u32();

    obj.quest_title = reader.read_cstring();

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> SMSG_QUEST_CONFIRM_ACCEPT::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_QUEST_CONFIRM_ACCEPT_size(obj));

    writer.write_u16_be(SMSG_QUEST_CONFIRM_ACCEPT_size(obj) + 2); /* size */

    writer.write_u16(0x0000019c); /* opcode */

    writer.write_u32(obj.quest_id);

    writer.write_cstring(obj.quest_title);

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

CMSG_PUSHQUESTTOPARTY CMSG_PUSHQUESTTOPARTY_read(Reader& reader) {
    CMSG_PUSHQUESTTOPARTY obj;

    obj.quest_id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_PUSHQUESTTOPARTY::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 4); /* size */

    writer.write_u32(0x0000019d); /* opcode */

    writer.write_u32(obj.quest_id);

    return writer.m_buf;
}

CMSG_LIST_INVENTORY CMSG_LIST_INVENTORY_read(Reader& reader) {
    CMSG_LIST_INVENTORY obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_LIST_INVENTORY::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x0000019e); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

static size_t SMSG_LIST_INVENTORY_size(const SMSG_LIST_INVENTORY& obj) {
    return 9 + 28 * obj.items.size();
}

SMSG_LIST_INVENTORY SMSG_LIST_INVENTORY_read(Reader& reader) {
    SMSG_LIST_INVENTORY obj;

    obj.vendor = reader.read_u64();

    obj.amount_of_items = reader.read_u8();

    for (auto i = 0; i < obj.amount_of_items; ++i) {
        obj.items.push_back(::wow_world_messages::vanilla::ListInventoryItem_read(reader));
    }

    return obj;
}

std::vector<unsigned char> SMSG_LIST_INVENTORY::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_LIST_INVENTORY_size(obj));

    writer.write_u16_be(SMSG_LIST_INVENTORY_size(obj) + 2); /* size */

    writer.write_u16(0x0000019f); /* opcode */

    writer.write_u64(obj.vendor);

    writer.write_u8(obj.amount_of_items);

    for (const auto& v : obj.items) {
        ListInventoryItem_write(writer, v);
    }

    return writer.m_buf;
}

CMSG_SELL_ITEM CMSG_SELL_ITEM_read(Reader& reader) {
    CMSG_SELL_ITEM obj;

    obj.vendor = reader.read_u64();

    obj.item = reader.read_u64();

    obj.amount = reader.read_u8();

    return obj;
}

std::vector<unsigned char> CMSG_SELL_ITEM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0011);

    writer.write_u16_be(0x0011 + 4); /* size */

    writer.write_u32(0x000001a0); /* opcode */

    writer.write_u64(obj.vendor);

    writer.write_u64(obj.item);

    writer.write_u8(obj.amount);

    return writer.m_buf;
}

SMSG_SELL_ITEM SMSG_SELL_ITEM_read(Reader& reader) {
    SMSG_SELL_ITEM obj;

    obj.guid = reader.read_u64();

    obj.item = reader.read_u64();

    obj.result = static_cast<SellItemResult>(reader.read_u8());

    return obj;
}

std::vector<unsigned char> SMSG_SELL_ITEM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0011);

    writer.write_u16_be(0x0011 + 2); /* size */

    writer.write_u16(0x000001a1); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u64(obj.item);

    writer.write_u8(static_cast<uint8_t>(obj.result));

    return writer.m_buf;
}

CMSG_BUY_ITEM CMSG_BUY_ITEM_read(Reader& reader) {
    CMSG_BUY_ITEM obj;

    obj.vendor = reader.read_u64();

    obj.item = reader.read_u32();

    obj.amount = reader.read_u8();

    obj.unknown1 = reader.read_u8();

    return obj;
}

std::vector<unsigned char> CMSG_BUY_ITEM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000e);

    writer.write_u16_be(0x000e + 4); /* size */

    writer.write_u32(0x000001a2); /* opcode */

    writer.write_u64(obj.vendor);

    writer.write_u32(obj.item);

    writer.write_u8(obj.amount);

    writer.write_u8(obj.unknown1);

    return writer.m_buf;
}

CMSG_BUY_ITEM_IN_SLOT CMSG_BUY_ITEM_IN_SLOT_read(Reader& reader) {
    CMSG_BUY_ITEM_IN_SLOT obj;

    obj.vendor = reader.read_u64();

    obj.item = reader.read_u32();

    obj.bag = reader.read_u64();

    obj.bag_slot = reader.read_u8();

    obj.amount = reader.read_u8();

    return obj;
}

std::vector<unsigned char> CMSG_BUY_ITEM_IN_SLOT::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0016);

    writer.write_u16_be(0x0016 + 4); /* size */

    writer.write_u32(0x000001a3); /* opcode */

    writer.write_u64(obj.vendor);

    writer.write_u32(obj.item);

    writer.write_u64(obj.bag);

    writer.write_u8(obj.bag_slot);

    writer.write_u8(obj.amount);

    return writer.m_buf;
}

SMSG_BUY_ITEM SMSG_BUY_ITEM_read(Reader& reader) {
    SMSG_BUY_ITEM obj;

    obj.guid = reader.read_u64();

    obj.vendor_slot = reader.read_u32();

    obj.amount_for_sale = reader.read_u32();

    obj.amount_bought = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_BUY_ITEM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0014);

    writer.write_u16_be(0x0014 + 2); /* size */

    writer.write_u16(0x000001a4); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.vendor_slot);

    writer.write_u32(obj.amount_for_sale);

    writer.write_u32(obj.amount_bought);

    return writer.m_buf;
}

SMSG_BUY_FAILED SMSG_BUY_FAILED_read(Reader& reader) {
    SMSG_BUY_FAILED obj;

    obj.guid = reader.read_u64();

    obj.item = reader.read_u32();

    obj.result = static_cast<BuyResult>(reader.read_u8());

    return obj;
}

std::vector<unsigned char> SMSG_BUY_FAILED::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000d);

    writer.write_u16_be(0x000d + 2); /* size */

    writer.write_u16(0x000001a5); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.item);

    writer.write_u8(static_cast<uint8_t>(obj.result));

    return writer.m_buf;
}

static size_t SMSG_SHOWTAXINODES_size(const SMSG_SHOWTAXINODES& obj) {
    size_t _size = 16;

    for(const auto& v : obj.nodes) {
        _size += 4;
    }

    return _size;
}

SMSG_SHOWTAXINODES SMSG_SHOWTAXINODES_read(Reader& reader, size_t body_size) {
    SMSG_SHOWTAXINODES obj;
    size_t _size = 0;

    obj.unknown1 = reader.read_u32();
    _size += 4;

    obj.guid = reader.read_u64();
    _size += 8;

    obj.nearest_node = reader.read_u32();
    _size += 4;

    while (_size < body_size) {
        obj.nodes.push_back(reader.read_u32());
        _size += 4;
    }

    return obj;
}

std::vector<unsigned char> SMSG_SHOWTAXINODES::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SHOWTAXINODES_size(obj));

    writer.write_u16_be(SMSG_SHOWTAXINODES_size(obj) + 2); /* size */

    writer.write_u16(0x000001a9); /* opcode */

    writer.write_u32(obj.unknown1);

    writer.write_u64(obj.guid);

    writer.write_u32(obj.nearest_node);

    for (const auto& v : obj.nodes) {
        writer.write_u32(v);
    }

    return writer.m_buf;
}

CMSG_TAXINODE_STATUS_QUERY CMSG_TAXINODE_STATUS_QUERY_read(Reader& reader) {
    CMSG_TAXINODE_STATUS_QUERY obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_TAXINODE_STATUS_QUERY::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x000001aa); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

SMSG_TAXINODE_STATUS SMSG_TAXINODE_STATUS_read(Reader& reader) {
    SMSG_TAXINODE_STATUS obj;

    obj.guid = reader.read_u64();

    obj.taxi_mask_node_known = reader.read_bool8();

    return obj;
}

std::vector<unsigned char> SMSG_TAXINODE_STATUS::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0009);

    writer.write_u16_be(0x0009 + 2); /* size */

    writer.write_u16(0x000001ab); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_bool8(obj.taxi_mask_node_known);

    return writer.m_buf;
}

CMSG_TAXIQUERYAVAILABLENODES CMSG_TAXIQUERYAVAILABLENODES_read(Reader& reader) {
    CMSG_TAXIQUERYAVAILABLENODES obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_TAXIQUERYAVAILABLENODES::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x000001ac); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

CMSG_ACTIVATETAXI CMSG_ACTIVATETAXI_read(Reader& reader) {
    CMSG_ACTIVATETAXI obj;

    obj.guid = reader.read_u64();

    obj.source_node = reader.read_u32();

    obj.destination_node = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_ACTIVATETAXI::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0010);

    writer.write_u16_be(0x0010 + 4); /* size */

    writer.write_u32(0x000001ad); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.source_node);

    writer.write_u32(obj.destination_node);

    return writer.m_buf;
}

SMSG_ACTIVATETAXIREPLY SMSG_ACTIVATETAXIREPLY_read(Reader& reader) {
    SMSG_ACTIVATETAXIREPLY obj;

    obj.reply = static_cast<ActivateTaxiReply>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_ACTIVATETAXIREPLY::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x000001ae); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.reply));

    return writer.m_buf;
}

std::vector<unsigned char> SMSG_NEW_TAXI_PATH::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 2); /* size */

    writer.write_u16(0x000001af); /* opcode */

    return writer.m_buf;
}

CMSG_TRAINER_LIST CMSG_TRAINER_LIST_read(Reader& reader) {
    CMSG_TRAINER_LIST obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_TRAINER_LIST::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x000001b0); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

static size_t SMSG_TRAINER_LIST_size(const SMSG_TRAINER_LIST& obj) {
    return 17 + 38 * obj.spells.size() + obj.greeting.size();
}

SMSG_TRAINER_LIST SMSG_TRAINER_LIST_read(Reader& reader) {
    SMSG_TRAINER_LIST obj;

    obj.guid = reader.read_u64();

    obj.trainer_type = reader.read_u32();

    obj.amount_of_spells = reader.read_u32();

    for (auto i = 0; i < obj.amount_of_spells; ++i) {
        obj.spells.push_back(::wow_world_messages::vanilla::TrainerSpell_read(reader));
    }

    obj.greeting = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> SMSG_TRAINER_LIST::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_TRAINER_LIST_size(obj));

    writer.write_u16_be(SMSG_TRAINER_LIST_size(obj) + 2); /* size */

    writer.write_u16(0x000001b1); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.trainer_type);

    writer.write_u32(obj.amount_of_spells);

    for (const auto& v : obj.spells) {
        TrainerSpell_write(writer, v);
    }

    writer.write_cstring(obj.greeting);

    return writer.m_buf;
}

CMSG_TRAINER_BUY_SPELL CMSG_TRAINER_BUY_SPELL_read(Reader& reader) {
    CMSG_TRAINER_BUY_SPELL obj;

    obj.guid = reader.read_u64();

    obj.id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_TRAINER_BUY_SPELL::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 4); /* size */

    writer.write_u32(0x000001b2); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.id);

    return writer.m_buf;
}

SMSG_TRAINER_BUY_SUCCEEDED SMSG_TRAINER_BUY_SUCCEEDED_read(Reader& reader) {
    SMSG_TRAINER_BUY_SUCCEEDED obj;

    obj.guid = reader.read_u64();

    obj.id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_TRAINER_BUY_SUCCEEDED::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 2); /* size */

    writer.write_u16(0x000001b3); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.id);

    return writer.m_buf;
}

SMSG_TRAINER_BUY_FAILED SMSG_TRAINER_BUY_FAILED_read(Reader& reader) {
    SMSG_TRAINER_BUY_FAILED obj;

    obj.guid = reader.read_u64();

    obj.id = reader.read_u32();

    obj.error = static_cast<TrainingFailureReason>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_TRAINER_BUY_FAILED::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0010);

    writer.write_u16_be(0x0010 + 2); /* size */

    writer.write_u16(0x000001b4); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.id);

    writer.write_u32(static_cast<uint32_t>(obj.error));

    return writer.m_buf;
}

CMSG_BINDER_ACTIVATE CMSG_BINDER_ACTIVATE_read(Reader& reader) {
    CMSG_BINDER_ACTIVATE obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_BINDER_ACTIVATE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x000001b5); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

CMSG_BANKER_ACTIVATE CMSG_BANKER_ACTIVATE_read(Reader& reader) {
    CMSG_BANKER_ACTIVATE obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_BANKER_ACTIVATE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x000001b7); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

SMSG_SHOW_BANK SMSG_SHOW_BANK_read(Reader& reader) {
    SMSG_SHOW_BANK obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> SMSG_SHOW_BANK::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 2); /* size */

    writer.write_u16(0x000001b8); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

CMSG_BUY_BANK_SLOT CMSG_BUY_BANK_SLOT_read(Reader& reader) {
    CMSG_BUY_BANK_SLOT obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_BUY_BANK_SLOT::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x000001b9); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

SMSG_BUY_BANK_SLOT_RESULT SMSG_BUY_BANK_SLOT_RESULT_read(Reader& reader) {
    SMSG_BUY_BANK_SLOT_RESULT obj;

    obj.result = static_cast<BuyBankSlotResult>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_BUY_BANK_SLOT_RESULT::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x000001ba); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.result));

    return writer.m_buf;
}

CMSG_PETITION_SHOWLIST CMSG_PETITION_SHOWLIST_read(Reader& reader) {
    CMSG_PETITION_SHOWLIST obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_PETITION_SHOWLIST::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x000001bb); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

static size_t SMSG_PETITION_SHOWLIST_size(const SMSG_PETITION_SHOWLIST& obj) {
    return 9 + 20 * obj.petitions.size();
}

SMSG_PETITION_SHOWLIST SMSG_PETITION_SHOWLIST_read(Reader& reader) {
    SMSG_PETITION_SHOWLIST obj;

    obj.npc = reader.read_u64();

    obj.amount_of_petitions = reader.read_u8();

    for (auto i = 0; i < obj.amount_of_petitions; ++i) {
        obj.petitions.push_back(::wow_world_messages::vanilla::PetitionShowlist_read(reader));
    }

    return obj;
}

std::vector<unsigned char> SMSG_PETITION_SHOWLIST::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_PETITION_SHOWLIST_size(obj));

    writer.write_u16_be(SMSG_PETITION_SHOWLIST_size(obj) + 2); /* size */

    writer.write_u16(0x000001bc); /* opcode */

    writer.write_u64(obj.npc);

    writer.write_u8(obj.amount_of_petitions);

    for (const auto& v : obj.petitions) {
        PetitionShowlist_write(writer, v);
    }

    return writer.m_buf;
}

static size_t CMSG_PETITION_BUY_size(const CMSG_PETITION_BUY& obj) {
    return 72 + obj.name.size();
}

CMSG_PETITION_BUY CMSG_PETITION_BUY_read(Reader& reader) {
    CMSG_PETITION_BUY obj;

    obj.npc = reader.read_u64();

    obj.unknown1 = reader.read_u32();

    obj.unknown2 = reader.read_u64();

    obj.name = reader.read_cstring();

    obj.unknown3 = reader.read_u32();

    obj.unknown4 = reader.read_u32();

    obj.unknown5 = reader.read_u32();

    obj.unknown6 = reader.read_u32();

    obj.unknown7 = reader.read_u32();

    obj.unknown8 = reader.read_u32();

    obj.unknown9 = reader.read_u32();

    obj.unknown10 = reader.read_u32();

    obj.unknown11 = reader.read_u32();

    obj.unknown12 = reader.read_u32();

    obj.unknown13 = reader.read_u16();

    obj.unknown14 = reader.read_u8();

    obj.index = reader.read_u32();

    obj.unknown15 = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_PETITION_BUY::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_PETITION_BUY_size(obj));

    writer.write_u16_be((uint16_t)CMSG_PETITION_BUY_size(obj) + 4); /* size */

    writer.write_u32(0x000001bd); /* opcode */

    writer.write_u64(obj.npc);

    writer.write_u32(obj.unknown1);

    writer.write_u64(obj.unknown2);

    writer.write_cstring(obj.name);

    writer.write_u32(obj.unknown3);

    writer.write_u32(obj.unknown4);

    writer.write_u32(obj.unknown5);

    writer.write_u32(obj.unknown6);

    writer.write_u32(obj.unknown7);

    writer.write_u32(obj.unknown8);

    writer.write_u32(obj.unknown9);

    writer.write_u32(obj.unknown10);

    writer.write_u32(obj.unknown11);

    writer.write_u32(obj.unknown12);

    writer.write_u16(obj.unknown13);

    writer.write_u8(obj.unknown14);

    writer.write_u32(obj.index);

    writer.write_u32(obj.unknown15);

    return writer.m_buf;
}

CMSG_PETITION_SHOW_SIGNATURES CMSG_PETITION_SHOW_SIGNATURES_read(Reader& reader) {
    CMSG_PETITION_SHOW_SIGNATURES obj;

    obj.item = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_PETITION_SHOW_SIGNATURES::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x000001be); /* opcode */

    writer.write_u64(obj.item);

    return writer.m_buf;
}

static size_t SMSG_PETITION_SHOW_SIGNATURES_size(const SMSG_PETITION_SHOW_SIGNATURES& obj) {
    return 21 + 12 * obj.signatures.size();
}

SMSG_PETITION_SHOW_SIGNATURES SMSG_PETITION_SHOW_SIGNATURES_read(Reader& reader) {
    SMSG_PETITION_SHOW_SIGNATURES obj;

    obj.item = reader.read_u64();

    obj.owner = reader.read_u64();

    obj.petition = reader.read_u32();

    obj.amount_of_signatures = reader.read_u8();

    for (auto i = 0; i < obj.amount_of_signatures; ++i) {
        obj.signatures.push_back(::wow_world_messages::vanilla::PetitionSignature_read(reader));
    }

    return obj;
}

std::vector<unsigned char> SMSG_PETITION_SHOW_SIGNATURES::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_PETITION_SHOW_SIGNATURES_size(obj));

    writer.write_u16_be(SMSG_PETITION_SHOW_SIGNATURES_size(obj) + 2); /* size */

    writer.write_u16(0x000001bf); /* opcode */

    writer.write_u64(obj.item);

    writer.write_u64(obj.owner);

    writer.write_u32(obj.petition);

    writer.write_u8(obj.amount_of_signatures);

    for (const auto& v : obj.signatures) {
        PetitionSignature_write(writer, v);
    }

    return writer.m_buf;
}

CMSG_PETITION_SIGN CMSG_PETITION_SIGN_read(Reader& reader) {
    CMSG_PETITION_SIGN obj;

    obj.petition = reader.read_u64();

    obj.unknown1 = reader.read_u8();

    return obj;
}

std::vector<unsigned char> CMSG_PETITION_SIGN::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0009);

    writer.write_u16_be(0x0009 + 4); /* size */

    writer.write_u32(0x000001c0); /* opcode */

    writer.write_u64(obj.petition);

    writer.write_u8(obj.unknown1);

    return writer.m_buf;
}

SMSG_PETITION_SIGN_RESULTS SMSG_PETITION_SIGN_RESULTS_read(Reader& reader) {
    SMSG_PETITION_SIGN_RESULTS obj;

    obj.petition = reader.read_u64();

    obj.owner = reader.read_u64();

    obj.result = static_cast<PetitionResult>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_PETITION_SIGN_RESULTS::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0014);

    writer.write_u16_be(0x0014 + 2); /* size */

    writer.write_u16(0x000001c1); /* opcode */

    writer.write_u64(obj.petition);

    writer.write_u64(obj.owner);

    writer.write_u32(static_cast<uint32_t>(obj.result));

    return writer.m_buf;
}

CMSG_OFFER_PETITION CMSG_OFFER_PETITION_read(Reader& reader) {
    CMSG_OFFER_PETITION obj;

    obj.petition = reader.read_u64();

    obj.target = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_OFFER_PETITION::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0010);

    writer.write_u16_be(0x0010 + 4); /* size */

    writer.write_u32(0x000001c3); /* opcode */

    writer.write_u64(obj.petition);

    writer.write_u64(obj.target);

    return writer.m_buf;
}

CMSG_TURN_IN_PETITION CMSG_TURN_IN_PETITION_read(Reader& reader) {
    CMSG_TURN_IN_PETITION obj;

    obj.petition = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_TURN_IN_PETITION::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x000001c4); /* opcode */

    writer.write_u64(obj.petition);

    return writer.m_buf;
}

SMSG_TURN_IN_PETITION_RESULTS SMSG_TURN_IN_PETITION_RESULTS_read(Reader& reader) {
    SMSG_TURN_IN_PETITION_RESULTS obj;

    obj.result = static_cast<PetitionResult>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_TURN_IN_PETITION_RESULTS::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x000001c5); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.result));

    return writer.m_buf;
}

CMSG_PETITION_QUERY CMSG_PETITION_QUERY_read(Reader& reader) {
    CMSG_PETITION_QUERY obj;

    obj.guild_id = reader.read_u32();

    obj.petition = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_PETITION_QUERY::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 4); /* size */

    writer.write_u32(0x000001c6); /* opcode */

    writer.write_u32(obj.guild_id);

    writer.write_u64(obj.petition);

    return writer.m_buf;
}

static size_t SMSG_PETITION_QUERY_RESPONSE_size(const SMSG_PETITION_QUERY_RESPONSE& obj) {
    return 64 + obj.guild_name.size() + obj.body_text.size();
}

SMSG_PETITION_QUERY_RESPONSE SMSG_PETITION_QUERY_RESPONSE_read(Reader& reader) {
    SMSG_PETITION_QUERY_RESPONSE obj;

    obj.petition_id = reader.read_u32();

    obj.charter_owner = reader.read_u64();

    obj.guild_name = reader.read_cstring();

    obj.body_text = reader.read_cstring();

    obj.unknown_flags = reader.read_u32();

    obj.minimum_signatures = reader.read_u32();

    obj.maximum_signatures = reader.read_u32();

    obj.deadline = reader.read_u32();

    obj.issue_date = reader.read_u32();

    obj.allowed_guild_id = reader.read_u32();

    obj.allowed_class = static_cast<AllowedClass>(reader.read_u32());

    obj.allowed_race = static_cast<AllowedRace>(reader.read_u32());

    obj.allowed_genders = reader.read_u16();

    obj.allowed_minimum_level = reader.read_u32();

    obj.allowed_maximum_level = reader.read_u32();

    obj.todo_amount_of_signers = reader.read_u32();

    obj.number_of_choices = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_PETITION_QUERY_RESPONSE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_PETITION_QUERY_RESPONSE_size(obj));

    writer.write_u16_be(SMSG_PETITION_QUERY_RESPONSE_size(obj) + 2); /* size */

    writer.write_u16(0x000001c7); /* opcode */

    writer.write_u32(obj.petition_id);

    writer.write_u64(obj.charter_owner);

    writer.write_cstring(obj.guild_name);

    writer.write_cstring(obj.body_text);

    writer.write_u32(obj.unknown_flags);

    writer.write_u32(obj.minimum_signatures);

    writer.write_u32(obj.maximum_signatures);

    writer.write_u32(obj.deadline);

    writer.write_u32(obj.issue_date);

    writer.write_u32(obj.allowed_guild_id);

    writer.write_u32(static_cast<uint32_t>(obj.allowed_class));

    writer.write_u32(static_cast<uint32_t>(obj.allowed_race));

    writer.write_u16(obj.allowed_genders);

    writer.write_u32(obj.allowed_minimum_level);

    writer.write_u32(obj.allowed_maximum_level);

    writer.write_u32(obj.todo_amount_of_signers);

    writer.write_u32(obj.number_of_choices);

    return writer.m_buf;
}

std::vector<unsigned char> SMSG_FISH_NOT_HOOKED::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 2); /* size */

    writer.write_u16(0x000001c8); /* opcode */

    return writer.m_buf;
}

std::vector<unsigned char> SMSG_FISH_ESCAPED::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 2); /* size */

    writer.write_u16(0x000001c9); /* opcode */

    return writer.m_buf;
}

static size_t CMSG_BUG_size(const CMSG_BUG& obj) {
    return 14 + obj.content.size() + obj.bug_type.size();
}

CMSG_BUG CMSG_BUG_read(Reader& reader) {
    CMSG_BUG obj;

    obj.suggestion = reader.read_u32();

    obj.content = reader.read_sized_cstring();

    obj.bug_type = reader.read_sized_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_BUG::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_BUG_size(obj));

    writer.write_u16_be((uint16_t)CMSG_BUG_size(obj) + 4); /* size */

    writer.write_u32(0x000001ca); /* opcode */

    writer.write_u32(obj.suggestion);

    writer.write_sized_cstring(obj.content);

    writer.write_sized_cstring(obj.bug_type);

    return writer.m_buf;
}

static size_t SMSG_NOTIFICATION_size(const SMSG_NOTIFICATION& obj) {
    return 1 + obj.notification.size();
}

SMSG_NOTIFICATION SMSG_NOTIFICATION_read(Reader& reader) {
    SMSG_NOTIFICATION obj;

    obj.notification = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> SMSG_NOTIFICATION::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_NOTIFICATION_size(obj));

    writer.write_u16_be(SMSG_NOTIFICATION_size(obj) + 2); /* size */

    writer.write_u16(0x000001cb); /* opcode */

    writer.write_cstring(obj.notification);

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_PLAYED_TIME::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x000001cc); /* opcode */

    return writer.m_buf;
}

SMSG_PLAYED_TIME SMSG_PLAYED_TIME_read(Reader& reader) {
    SMSG_PLAYED_TIME obj;

    obj.total_played_time = reader.read_u32();

    obj.level_played_time = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_PLAYED_TIME::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 2); /* size */

    writer.write_u16(0x000001cd); /* opcode */

    writer.write_u32(obj.total_played_time);

    writer.write_u32(obj.level_played_time);

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_QUERY_TIME::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x000001ce); /* opcode */

    return writer.m_buf;
}

SMSG_QUERY_TIME_RESPONSE SMSG_QUERY_TIME_RESPONSE_read(Reader& reader) {
    SMSG_QUERY_TIME_RESPONSE obj;

    obj.time = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_QUERY_TIME_RESPONSE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x000001cf); /* opcode */

    writer.write_u32(obj.time);

    return writer.m_buf;
}

static size_t SMSG_LOG_XPGAIN_size(const SMSG_LOG_XPGAIN& obj) {
    size_t _size = 13;

    if (obj.exp_type == ExperienceAwardType::NON_KILL) {
        _size += 8;
    }

    return _size;
}

SMSG_LOG_XPGAIN SMSG_LOG_XPGAIN_read(Reader& reader) {
    SMSG_LOG_XPGAIN obj;

    obj.target = reader.read_u64();

    obj.total_exp = reader.read_u32();

    obj.exp_type = static_cast<ExperienceAwardType>(reader.read_u8());

    if (obj.exp_type == ExperienceAwardType::NON_KILL) {
        obj.experience_without_rested = reader.read_u32();

        obj.exp_group_bonus = reader.read_float();

    }
    return obj;
}

std::vector<unsigned char> SMSG_LOG_XPGAIN::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_LOG_XPGAIN_size(obj));

    writer.write_u16_be(SMSG_LOG_XPGAIN_size(obj) + 2); /* size */

    writer.write_u16(0x000001d0); /* opcode */

    writer.write_u64(obj.target);

    writer.write_u32(obj.total_exp);

    writer.write_u8(static_cast<uint8_t>(obj.exp_type));

    if (obj.exp_type == ExperienceAwardType::NON_KILL) {
        writer.write_u32(obj.experience_without_rested);

        writer.write_float(obj.exp_group_bonus);

    }
    return writer.m_buf;
}

CMSG_RECLAIM_CORPSE CMSG_RECLAIM_CORPSE_read(Reader& reader) {
    CMSG_RECLAIM_CORPSE obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_RECLAIM_CORPSE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x000001d2); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

CMSG_WRAP_ITEM CMSG_WRAP_ITEM_read(Reader& reader) {
    CMSG_WRAP_ITEM obj;

    obj.gift_bag_index = reader.read_u8();

    obj.gift_slot = reader.read_u8();

    obj.item_bag_index = reader.read_u8();

    obj.item_slot = reader.read_u8();

    return obj;
}

std::vector<unsigned char> CMSG_WRAP_ITEM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 4); /* size */

    writer.write_u32(0x000001d3); /* opcode */

    writer.write_u8(obj.gift_bag_index);

    writer.write_u8(obj.gift_slot);

    writer.write_u8(obj.item_bag_index);

    writer.write_u8(obj.item_slot);

    return writer.m_buf;
}

SMSG_LEVELUP_INFO SMSG_LEVELUP_INFO_read(Reader& reader) {
    SMSG_LEVELUP_INFO obj;

    obj.new_level = reader.read_u32();

    obj.health = reader.read_u32();

    obj.mana = reader.read_u32();

    obj.rage = reader.read_u32();

    obj.focus = reader.read_u32();

    obj.energy = reader.read_u32();

    obj.happiness = reader.read_u32();

    obj.strength = reader.read_u32();

    obj.agility = reader.read_u32();

    obj.stamina = reader.read_u32();

    obj.intellect = reader.read_u32();

    obj.spirit = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_LEVELUP_INFO::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0030);

    writer.write_u16_be(0x0030 + 2); /* size */

    writer.write_u16(0x000001d4); /* opcode */

    writer.write_u32(obj.new_level);

    writer.write_u32(obj.health);

    writer.write_u32(obj.mana);

    writer.write_u32(obj.rage);

    writer.write_u32(obj.focus);

    writer.write_u32(obj.energy);

    writer.write_u32(obj.happiness);

    writer.write_u32(obj.strength);

    writer.write_u32(obj.agility);

    writer.write_u32(obj.stamina);

    writer.write_u32(obj.intellect);

    writer.write_u32(obj.spirit);

    return writer.m_buf;
}

MSG_MINIMAP_PING_Client MSG_MINIMAP_PING_Client_read(Reader& reader) {
    MSG_MINIMAP_PING_Client obj;

    obj.position_x = reader.read_float();

    obj.position_y = reader.read_float();

    return obj;
}

std::vector<unsigned char> MSG_MINIMAP_PING_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x000001d5); /* opcode */

    writer.write_float(obj.position_x);

    writer.write_float(obj.position_y);

    return writer.m_buf;
}

MSG_MINIMAP_PING_Server MSG_MINIMAP_PING_Server_read(Reader& reader) {
    MSG_MINIMAP_PING_Server obj;

    obj.guid = reader.read_u64();

    obj.position_x = reader.read_float();

    obj.position_y = reader.read_float();

    return obj;
}

std::vector<unsigned char> MSG_MINIMAP_PING_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0010);

    writer.write_u16_be(0x0010 + 2); /* size */

    writer.write_u16(0x000001d5); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_float(obj.position_x);

    writer.write_float(obj.position_y);

    return writer.m_buf;
}

SMSG_RESISTLOG SMSG_RESISTLOG_read(Reader& reader) {
    SMSG_RESISTLOG obj;

    obj.guid1 = reader.read_u64();

    obj.guid2 = reader.read_u64();

    obj.unknown1 = reader.read_u32();

    obj.unknown2 = reader.read_float();

    obj.unknown3 = reader.read_float();

    obj.unknown4 = reader.read_u32();

    obj.unknown5 = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_RESISTLOG::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0024);

    writer.write_u16_be(0x0024 + 2); /* size */

    writer.write_u16(0x000001d6); /* opcode */

    writer.write_u64(obj.guid1);

    writer.write_u64(obj.guid2);

    writer.write_u32(obj.unknown1);

    writer.write_float(obj.unknown2);

    writer.write_float(obj.unknown3);

    writer.write_u32(obj.unknown4);

    writer.write_u32(obj.unknown5);

    return writer.m_buf;
}

SMSG_ENCHANTMENTLOG SMSG_ENCHANTMENTLOG_read(Reader& reader) {
    SMSG_ENCHANTMENTLOG obj;

    obj.target = reader.read_u64();

    obj.caster = reader.read_u64();

    obj.item = reader.read_u32();

    obj.spell = reader.read_u32();

    obj.show_affiliation = reader.read_bool8();

    return obj;
}

std::vector<unsigned char> SMSG_ENCHANTMENTLOG::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0019);

    writer.write_u16_be(0x0019 + 2); /* size */

    writer.write_u16(0x000001d7); /* opcode */

    writer.write_u64(obj.target);

    writer.write_u64(obj.caster);

    writer.write_u32(obj.item);

    writer.write_u32(obj.spell);

    writer.write_bool8(obj.show_affiliation);

    return writer.m_buf;
}

SMSG_START_MIRROR_TIMER SMSG_START_MIRROR_TIMER_read(Reader& reader) {
    SMSG_START_MIRROR_TIMER obj;

    obj.timer = static_cast<TimerType>(reader.read_u32());

    obj.time_remaining = reader.read_u32();

    obj.duration = reader.read_u32();

    obj.scale = reader.read_u32();

    obj.is_frozen = reader.read_bool8();

    obj.id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_START_MIRROR_TIMER::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0015);

    writer.write_u16_be(0x0015 + 2); /* size */

    writer.write_u16(0x000001d9); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.timer));

    writer.write_u32(obj.time_remaining);

    writer.write_u32(obj.duration);

    writer.write_u32(obj.scale);

    writer.write_bool8(obj.is_frozen);

    writer.write_u32(obj.id);

    return writer.m_buf;
}

SMSG_PAUSE_MIRROR_TIMER SMSG_PAUSE_MIRROR_TIMER_read(Reader& reader) {
    SMSG_PAUSE_MIRROR_TIMER obj;

    obj.timer = static_cast<TimerType>(reader.read_u32());

    obj.is_frozen = reader.read_bool8();

    return obj;
}

std::vector<unsigned char> SMSG_PAUSE_MIRROR_TIMER::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0005);

    writer.write_u16_be(0x0005 + 2); /* size */

    writer.write_u16(0x000001da); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.timer));

    writer.write_bool8(obj.is_frozen);

    return writer.m_buf;
}

SMSG_STOP_MIRROR_TIMER SMSG_STOP_MIRROR_TIMER_read(Reader& reader) {
    SMSG_STOP_MIRROR_TIMER obj;

    obj.timer = static_cast<TimerType>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_STOP_MIRROR_TIMER::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x000001db); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.timer));

    return writer.m_buf;
}

CMSG_PING CMSG_PING_read(Reader& reader) {
    CMSG_PING obj;

    obj.sequence_id = reader.read_u32();

    obj.round_time_in_ms = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_PING::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x000001dc); /* opcode */

    writer.write_u32(obj.sequence_id);

    writer.write_u32(obj.round_time_in_ms);

    return writer.m_buf;
}

SMSG_PONG SMSG_PONG_read(Reader& reader) {
    SMSG_PONG obj;

    obj.sequence_id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_PONG::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x000001dd); /* opcode */

    writer.write_u32(obj.sequence_id);

    return writer.m_buf;
}

SMSG_CLEAR_COOLDOWN SMSG_CLEAR_COOLDOWN_read(Reader& reader) {
    SMSG_CLEAR_COOLDOWN obj;

    obj.id = reader.read_u32();

    obj.target = reader.read_u64();

    return obj;
}

std::vector<unsigned char> SMSG_CLEAR_COOLDOWN::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 2); /* size */

    writer.write_u16(0x000001de); /* opcode */

    writer.write_u32(obj.id);

    writer.write_u64(obj.target);

    return writer.m_buf;
}

SMSG_GAMEOBJECT_PAGETEXT SMSG_GAMEOBJECT_PAGETEXT_read(Reader& reader) {
    SMSG_GAMEOBJECT_PAGETEXT obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> SMSG_GAMEOBJECT_PAGETEXT::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 2); /* size */

    writer.write_u16(0x000001df); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

CMSG_SETSHEATHED CMSG_SETSHEATHED_read(Reader& reader) {
    CMSG_SETSHEATHED obj;

    obj.sheathed = static_cast<SheathState>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> CMSG_SETSHEATHED::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 4); /* size */

    writer.write_u32(0x000001e0); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.sheathed));

    return writer.m_buf;
}

SMSG_SPELL_DELAYED SMSG_SPELL_DELAYED_read(Reader& reader) {
    SMSG_SPELL_DELAYED obj;

    obj.guid = reader.read_u64();

    obj.delay_time = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_SPELL_DELAYED::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 2); /* size */

    writer.write_u16(0x000001e2); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.delay_time);

    return writer.m_buf;
}

SMSG_ITEM_TIME_UPDATE SMSG_ITEM_TIME_UPDATE_read(Reader& reader) {
    SMSG_ITEM_TIME_UPDATE obj;

    obj.guid = reader.read_u64();

    obj.duration = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_ITEM_TIME_UPDATE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 2); /* size */

    writer.write_u16(0x000001ea); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.duration);

    return writer.m_buf;
}

SMSG_ITEM_ENCHANT_TIME_UPDATE SMSG_ITEM_ENCHANT_TIME_UPDATE_read(Reader& reader) {
    SMSG_ITEM_ENCHANT_TIME_UPDATE obj;

    obj.item = reader.read_u64();

    obj.slot = reader.read_u32();

    obj.duration = reader.read_u32();

    obj.player = reader.read_u64();

    return obj;
}

std::vector<unsigned char> SMSG_ITEM_ENCHANT_TIME_UPDATE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0018);

    writer.write_u16_be(0x0018 + 2); /* size */

    writer.write_u16(0x000001eb); /* opcode */

    writer.write_u64(obj.item);

    writer.write_u32(obj.slot);

    writer.write_u32(obj.duration);

    writer.write_u64(obj.player);

    return writer.m_buf;
}

SMSG_AUTH_CHALLENGE SMSG_AUTH_CHALLENGE_read(Reader& reader) {
    SMSG_AUTH_CHALLENGE obj;

    obj.server_seed = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_AUTH_CHALLENGE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x000001ec); /* opcode */

    writer.write_u32(obj.server_seed);

    return writer.m_buf;
}

static size_t SMSG_AUTH_RESPONSE_size(const SMSG_AUTH_RESPONSE& obj) {
    size_t _size = 1;

    if (obj.result == WorldResult::AUTH_OK) {
        _size += 9;
    }
    else if (obj.result == WorldResult::AUTH_WAIT_QUEUE) {
        _size += 4;
    }

    return _size;
}

SMSG_AUTH_RESPONSE SMSG_AUTH_RESPONSE_read(Reader& reader) {
    SMSG_AUTH_RESPONSE obj;

    obj.result = static_cast<WorldResult>(reader.read_u8());

    if (obj.result == WorldResult::AUTH_OK) {
        obj.billing_time = reader.read_u32();

        obj.billing_flags = reader.read_u8();

        obj.billing_rested = reader.read_u32();

    }
    else if (obj.result == WorldResult::AUTH_WAIT_QUEUE) {
        obj.queue_position = reader.read_u32();

    }
    return obj;
}

std::vector<unsigned char> SMSG_AUTH_RESPONSE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_AUTH_RESPONSE_size(obj));

    writer.write_u16_be(SMSG_AUTH_RESPONSE_size(obj) + 2); /* size */

    writer.write_u16(0x000001ee); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.result));

    if (obj.result == WorldResult::AUTH_OK) {
        writer.write_u32(obj.billing_time);

        writer.write_u8(obj.billing_flags);

        writer.write_u32(obj.billing_rested);

    }
    else if (obj.result == WorldResult::AUTH_WAIT_QUEUE) {
        writer.write_u32(obj.queue_position);

    }
    return writer.m_buf;
}

static size_t CMSG_PET_CAST_SPELL_size(const CMSG_PET_CAST_SPELL& obj) {
    return 12 + SpellCastTargets_size(obj.targets);
}

CMSG_PET_CAST_SPELL CMSG_PET_CAST_SPELL_read(Reader& reader) {
    CMSG_PET_CAST_SPELL obj;

    obj.guid = reader.read_u64();

    obj.id = reader.read_u32();

    obj.targets = ::wow_world_messages::vanilla::SpellCastTargets_read(reader);

    return obj;
}

std::vector<unsigned char> CMSG_PET_CAST_SPELL::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_PET_CAST_SPELL_size(obj));

    writer.write_u16_be((uint16_t)CMSG_PET_CAST_SPELL_size(obj) + 4); /* size */

    writer.write_u32(0x000001f0); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.id);

    SpellCastTargets_write(writer, obj.targets);

    return writer.m_buf;
}

MSG_SAVE_GUILD_EMBLEM_Server MSG_SAVE_GUILD_EMBLEM_Server_read(Reader& reader) {
    MSG_SAVE_GUILD_EMBLEM_Server obj;

    obj.result = static_cast<GuildEmblemResult>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> MSG_SAVE_GUILD_EMBLEM_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x000001f1); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.result));

    return writer.m_buf;
}

MSG_SAVE_GUILD_EMBLEM_Client MSG_SAVE_GUILD_EMBLEM_Client_read(Reader& reader) {
    MSG_SAVE_GUILD_EMBLEM_Client obj;

    obj.vendor = reader.read_u64();

    obj.emblem_style = reader.read_u32();

    obj.emblem_color = reader.read_u32();

    obj.border_style = reader.read_u32();

    obj.border_color = reader.read_u32();

    obj.background_color = reader.read_u32();

    return obj;
}

std::vector<unsigned char> MSG_SAVE_GUILD_EMBLEM_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x001c);

    writer.write_u16_be(0x001c + 4); /* size */

    writer.write_u32(0x000001f1); /* opcode */

    writer.write_u64(obj.vendor);

    writer.write_u32(obj.emblem_style);

    writer.write_u32(obj.emblem_color);

    writer.write_u32(obj.border_style);

    writer.write_u32(obj.border_color);

    writer.write_u32(obj.background_color);

    return writer.m_buf;
}

SMSG_PLAY_SPELL_VISUAL SMSG_PLAY_SPELL_VISUAL_read(Reader& reader) {
    SMSG_PLAY_SPELL_VISUAL obj;

    obj.guid = reader.read_u64();

    obj.spell_art_kit = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_PLAY_SPELL_VISUAL::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 2); /* size */

    writer.write_u16(0x000001f3); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.spell_art_kit);

    return writer.m_buf;
}

CMSG_ZONEUPDATE CMSG_ZONEUPDATE_read(Reader& reader) {
    CMSG_ZONEUPDATE obj;

    obj.area = static_cast<Area>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> CMSG_ZONEUPDATE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 4); /* size */

    writer.write_u32(0x000001f4); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.area));

    return writer.m_buf;
}

SMSG_PARTYKILLLOG SMSG_PARTYKILLLOG_read(Reader& reader) {
    SMSG_PARTYKILLLOG obj;

    obj.player_with_killing_blow = reader.read_u64();

    obj.victim = reader.read_u64();

    return obj;
}

std::vector<unsigned char> SMSG_PARTYKILLLOG::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0010);

    writer.write_u16_be(0x0010 + 2); /* size */

    writer.write_u16(0x000001f5); /* opcode */

    writer.write_u64(obj.player_with_killing_blow);

    writer.write_u64(obj.victim);

    return writer.m_buf;
}

SMSG_PLAY_SPELL_IMPACT SMSG_PLAY_SPELL_IMPACT_read(Reader& reader) {
    SMSG_PLAY_SPELL_IMPACT obj;

    obj.guid = reader.read_u64();

    obj.spell_visual_kit = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_PLAY_SPELL_IMPACT::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 2); /* size */

    writer.write_u16(0x000001f7); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.spell_visual_kit);

    return writer.m_buf;
}

SMSG_EXPLORATION_EXPERIENCE SMSG_EXPLORATION_EXPERIENCE_read(Reader& reader) {
    SMSG_EXPLORATION_EXPERIENCE obj;

    obj.area = static_cast<Area>(reader.read_u32());

    obj.experience = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_EXPLORATION_EXPERIENCE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 2); /* size */

    writer.write_u16(0x000001f8); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.area));

    writer.write_u32(obj.experience);

    return writer.m_buf;
}

MSG_RANDOM_ROLL_Client MSG_RANDOM_ROLL_Client_read(Reader& reader) {
    MSG_RANDOM_ROLL_Client obj;

    obj.minimum = reader.read_u32();

    obj.maximum = reader.read_u32();

    return obj;
}

std::vector<unsigned char> MSG_RANDOM_ROLL_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x000001fb); /* opcode */

    writer.write_u32(obj.minimum);

    writer.write_u32(obj.maximum);

    return writer.m_buf;
}

MSG_RANDOM_ROLL_Server MSG_RANDOM_ROLL_Server_read(Reader& reader) {
    MSG_RANDOM_ROLL_Server obj;

    obj.minimum = reader.read_u32();

    obj.maximum = reader.read_u32();

    obj.actual_roll = reader.read_u32();

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> MSG_RANDOM_ROLL_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0014);

    writer.write_u16_be(0x0014 + 2); /* size */

    writer.write_u16(0x000001fb); /* opcode */

    writer.write_u32(obj.minimum);

    writer.write_u32(obj.maximum);

    writer.write_u32(obj.actual_roll);

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

SMSG_ENVIRONMENTAL_DAMAGE_LOG SMSG_ENVIRONMENTAL_DAMAGE_LOG_read(Reader& reader) {
    SMSG_ENVIRONMENTAL_DAMAGE_LOG obj;

    obj.guid = reader.read_u64();

    obj.damage_type = static_cast<EnvironmentalDamageType>(reader.read_u8());

    obj.damage = reader.read_u32();

    obj.absorb = reader.read_u32();

    obj.resist = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_ENVIRONMENTAL_DAMAGE_LOG::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0015);

    writer.write_u16_be(0x0015 + 2); /* size */

    writer.write_u16(0x000001fc); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u8(static_cast<uint8_t>(obj.damage_type));

    writer.write_u32(obj.damage);

    writer.write_u32(obj.absorb);

    writer.write_u32(obj.resist);

    return writer.m_buf;
}

std::vector<unsigned char> MSG_LOOKING_FOR_GROUP_Client::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x000001ff); /* opcode */

    return writer.m_buf;
}

MSG_LOOKING_FOR_GROUP_Server MSG_LOOKING_FOR_GROUP_Server_read(Reader& reader) {
    MSG_LOOKING_FOR_GROUP_Server obj;

    obj.unknown1 = reader.read_u32();

    return obj;
}

std::vector<unsigned char> MSG_LOOKING_FOR_GROUP_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x000001ff); /* opcode */

    writer.write_u32(obj.unknown1);

    return writer.m_buf;
}

CMSG_UNLEARN_SKILL CMSG_UNLEARN_SKILL_read(Reader& reader) {
    CMSG_UNLEARN_SKILL obj;

    obj.skill = static_cast<Skill>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> CMSG_UNLEARN_SKILL::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 4); /* size */

    writer.write_u32(0x00000202); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.skill));

    return writer.m_buf;
}

SMSG_REMOVED_SPELL SMSG_REMOVED_SPELL_read(Reader& reader) {
    SMSG_REMOVED_SPELL obj;

    obj.spell = reader.read_u16();

    return obj;
}

std::vector<unsigned char> SMSG_REMOVED_SPELL::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0002);

    writer.write_u16_be(0x0002 + 2); /* size */

    writer.write_u16(0x00000203); /* opcode */

    writer.write_u16(obj.spell);

    return writer.m_buf;
}

SMSG_GMTICKET_CREATE SMSG_GMTICKET_CREATE_read(Reader& reader) {
    SMSG_GMTICKET_CREATE obj;

    obj.response = static_cast<GmTicketResponse>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_GMTICKET_CREATE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x00000206); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.response));

    return writer.m_buf;
}

static size_t CMSG_GMTICKET_UPDATETEXT_size(const CMSG_GMTICKET_UPDATETEXT& obj) {
    return 2 + obj.message.size();
}

CMSG_GMTICKET_UPDATETEXT CMSG_GMTICKET_UPDATETEXT_read(Reader& reader) {
    CMSG_GMTICKET_UPDATETEXT obj;

    obj.ticket_type = static_cast<GmTicketType>(reader.read_u8());

    obj.message = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_GMTICKET_UPDATETEXT::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_GMTICKET_UPDATETEXT_size(obj));

    writer.write_u16_be((uint16_t)CMSG_GMTICKET_UPDATETEXT_size(obj) + 4); /* size */

    writer.write_u32(0x00000207); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.ticket_type));

    writer.write_cstring(obj.message);

    return writer.m_buf;
}

SMSG_GMTICKET_UPDATETEXT SMSG_GMTICKET_UPDATETEXT_read(Reader& reader) {
    SMSG_GMTICKET_UPDATETEXT obj;

    obj.response = static_cast<GmTicketResponse>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_GMTICKET_UPDATETEXT::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x00000208); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.response));

    return writer.m_buf;
}

SMSG_ACCOUNT_DATA_TIMES SMSG_ACCOUNT_DATA_TIMES_read(Reader& reader) {
    SMSG_ACCOUNT_DATA_TIMES obj;

    for (auto i = 0; i < 32; ++i) {
        obj.data[i] = reader.read_u32();
    }

    return obj;
}

std::vector<unsigned char> SMSG_ACCOUNT_DATA_TIMES::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0080);

    writer.write_u16_be(0x0080 + 2); /* size */

    writer.write_u16(0x00000209); /* opcode */

    for (const auto& v : obj.data) {
        writer.write_u32(v);
    }

    return writer.m_buf;
}

CMSG_REQUEST_ACCOUNT_DATA CMSG_REQUEST_ACCOUNT_DATA_read(Reader& reader) {
    CMSG_REQUEST_ACCOUNT_DATA obj;

    obj.data_type = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_REQUEST_ACCOUNT_DATA::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 4); /* size */

    writer.write_u32(0x0000020a); /* opcode */

    writer.write_u32(obj.data_type);

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_GMTICKET_GETTICKET::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x00000211); /* opcode */

    return writer.m_buf;
}

static size_t SMSG_GMTICKET_GETTICKET_size(const SMSG_GMTICKET_GETTICKET& obj) {
    size_t _size = 4;

    if (obj.status == GmTicketStatus::HAS_TEXT) {
        _size += 16 + obj.text.size();
    }

    return _size;
}

SMSG_GMTICKET_GETTICKET SMSG_GMTICKET_GETTICKET_read(Reader& reader) {
    SMSG_GMTICKET_GETTICKET obj;

    obj.status = static_cast<GmTicketStatus>(reader.read_u32());

    if (obj.status == GmTicketStatus::HAS_TEXT) {
        obj.text = reader.read_cstring();

        obj.ticket_type = static_cast<GmTicketType>(reader.read_u8());

        obj.days_since_ticket_creation = reader.read_float();

        obj.days_since_oldest_ticket_creation = reader.read_float();

        obj.days_since_last_updated = reader.read_float();

        obj.escalation_status = static_cast<GmTicketEscalationStatus>(reader.read_u8());

        obj.read_by_gm = reader.read_bool8();

    }
    return obj;
}

std::vector<unsigned char> SMSG_GMTICKET_GETTICKET::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_GMTICKET_GETTICKET_size(obj));

    writer.write_u16_be(SMSG_GMTICKET_GETTICKET_size(obj) + 2); /* size */

    writer.write_u16(0x00000212); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.status));

    if (obj.status == GmTicketStatus::HAS_TEXT) {
        writer.write_cstring(obj.text);

        writer.write_u8(static_cast<uint8_t>(obj.ticket_type));

        writer.write_float(obj.days_since_ticket_creation);

        writer.write_float(obj.days_since_oldest_ticket_creation);

        writer.write_float(obj.days_since_last_updated);

        writer.write_u8(static_cast<uint8_t>(obj.escalation_status));

        writer.write_bool8(obj.read_by_gm);

    }
    return writer.m_buf;
}

SMSG_GAMEOBJECT_SPAWN_ANIM SMSG_GAMEOBJECT_SPAWN_ANIM_read(Reader& reader) {
    SMSG_GAMEOBJECT_SPAWN_ANIM obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> SMSG_GAMEOBJECT_SPAWN_ANIM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 2); /* size */

    writer.write_u16(0x00000214); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

SMSG_GAMEOBJECT_DESPAWN_ANIM SMSG_GAMEOBJECT_DESPAWN_ANIM_read(Reader& reader) {
    SMSG_GAMEOBJECT_DESPAWN_ANIM obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> SMSG_GAMEOBJECT_DESPAWN_ANIM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 2); /* size */

    writer.write_u16(0x00000215); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

std::vector<unsigned char> MSG_CORPSE_QUERY_Client::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x00000216); /* opcode */

    return writer.m_buf;
}

static size_t MSG_CORPSE_QUERY_Server_size(const MSG_CORPSE_QUERY_Server& obj) {
    size_t _size = 1;

    if (obj.result == CorpseQueryResult::FOUND) {
        _size += 20;
    }

    return _size;
}

MSG_CORPSE_QUERY_Server MSG_CORPSE_QUERY_Server_read(Reader& reader) {
    MSG_CORPSE_QUERY_Server obj;

    obj.result = static_cast<CorpseQueryResult>(reader.read_u8());

    if (obj.result == CorpseQueryResult::FOUND) {
        obj.map = static_cast<Map>(reader.read_u32());

        obj.position = ::wow_world_messages::all::Vector3d_read(reader);

        obj.corpse_map = static_cast<Map>(reader.read_u32());

    }
    return obj;
}

std::vector<unsigned char> MSG_CORPSE_QUERY_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_CORPSE_QUERY_Server_size(obj));

    writer.write_u16_be(MSG_CORPSE_QUERY_Server_size(obj) + 2); /* size */

    writer.write_u16(0x00000216); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.result));

    if (obj.result == CorpseQueryResult::FOUND) {
        writer.write_u32(static_cast<uint32_t>(obj.map));

        Vector3d_write(writer, obj.position);

        writer.write_u32(static_cast<uint32_t>(obj.corpse_map));

    }
    return writer.m_buf;
}

std::vector<unsigned char> CMSG_GMTICKET_DELETETICKET::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x00000217); /* opcode */

    return writer.m_buf;
}

SMSG_GMTICKET_DELETETICKET SMSG_GMTICKET_DELETETICKET_read(Reader& reader) {
    SMSG_GMTICKET_DELETETICKET obj;

    obj.response = static_cast<GmTicketResponse>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_GMTICKET_DELETETICKET::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x00000218); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.response));

    return writer.m_buf;
}

std::vector<unsigned char> SMSG_CHAT_WRONG_FACTION::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 2); /* size */

    writer.write_u16(0x00000219); /* opcode */

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_GMTICKET_SYSTEMSTATUS::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x0000021a); /* opcode */

    return writer.m_buf;
}

SMSG_GMTICKET_SYSTEMSTATUS SMSG_GMTICKET_SYSTEMSTATUS_read(Reader& reader) {
    SMSG_GMTICKET_SYSTEMSTATUS obj;

    obj.will_accept_tickets = static_cast<GmTicketQueueStatus>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_GMTICKET_SYSTEMSTATUS::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x0000021b); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.will_accept_tickets));

    return writer.m_buf;
}

CMSG_SPIRIT_HEALER_ACTIVATE CMSG_SPIRIT_HEALER_ACTIVATE_read(Reader& reader) {
    CMSG_SPIRIT_HEALER_ACTIVATE obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_SPIRIT_HEALER_ACTIVATE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x0000021c); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

SMSG_SET_REST_START SMSG_SET_REST_START_read(Reader& reader) {
    SMSG_SET_REST_START obj;

    obj.unknown1 = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_SET_REST_START::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x0000021e); /* opcode */

    writer.write_u32(obj.unknown1);

    return writer.m_buf;
}

SMSG_SPIRIT_HEALER_CONFIRM SMSG_SPIRIT_HEALER_CONFIRM_read(Reader& reader) {
    SMSG_SPIRIT_HEALER_CONFIRM obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> SMSG_SPIRIT_HEALER_CONFIRM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 2); /* size */

    writer.write_u16(0x00000222); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

static size_t SMSG_GOSSIP_POI_size(const SMSG_GOSSIP_POI& obj) {
    return 21 + obj.location_name.size();
}

SMSG_GOSSIP_POI SMSG_GOSSIP_POI_read(Reader& reader) {
    SMSG_GOSSIP_POI obj;

    obj.flags = reader.read_u32();

    obj.position = ::wow_world_messages::all::Vector2d_read(reader);

    obj.icon = reader.read_u32();

    obj.data = reader.read_u32();

    obj.location_name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> SMSG_GOSSIP_POI::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_GOSSIP_POI_size(obj));

    writer.write_u16_be(SMSG_GOSSIP_POI_size(obj) + 2); /* size */

    writer.write_u16(0x00000224); /* opcode */

    writer.write_u32(obj.flags);

    Vector2d_write(writer, obj.position);

    writer.write_u32(obj.icon);

    writer.write_u32(obj.data);

    writer.write_cstring(obj.location_name);

    return writer.m_buf;
}

CMSG_CHAT_IGNORED CMSG_CHAT_IGNORED_read(Reader& reader) {
    CMSG_CHAT_IGNORED obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_CHAT_IGNORED::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x00000225); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

static size_t CMSG_GUILD_RANK_size(const CMSG_GUILD_RANK& obj) {
    return 9 + obj.rank_name.size();
}

CMSG_GUILD_RANK CMSG_GUILD_RANK_read(Reader& reader) {
    CMSG_GUILD_RANK obj;

    obj.rank_id = reader.read_u32();

    obj.rights = reader.read_u32();

    obj.rank_name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_GUILD_RANK::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_GUILD_RANK_size(obj));

    writer.write_u16_be((uint16_t)CMSG_GUILD_RANK_size(obj) + 4); /* size */

    writer.write_u32(0x00000231); /* opcode */

    writer.write_u32(obj.rank_id);

    writer.write_u32(obj.rights);

    writer.write_cstring(obj.rank_name);

    return writer.m_buf;
}

static size_t CMSG_GUILD_ADD_RANK_size(const CMSG_GUILD_ADD_RANK& obj) {
    return 1 + obj.rank_name.size();
}

CMSG_GUILD_ADD_RANK CMSG_GUILD_ADD_RANK_read(Reader& reader) {
    CMSG_GUILD_ADD_RANK obj;

    obj.rank_name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_GUILD_ADD_RANK::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_GUILD_ADD_RANK_size(obj));

    writer.write_u16_be((uint16_t)CMSG_GUILD_ADD_RANK_size(obj) + 4); /* size */

    writer.write_u32(0x00000232); /* opcode */

    writer.write_cstring(obj.rank_name);

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_GUILD_DEL_RANK::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x00000233); /* opcode */

    return writer.m_buf;
}

static size_t CMSG_GUILD_SET_PUBLIC_NOTE_size(const CMSG_GUILD_SET_PUBLIC_NOTE& obj) {
    return 2 + obj.player_name.size() + obj.note.size();
}

CMSG_GUILD_SET_PUBLIC_NOTE CMSG_GUILD_SET_PUBLIC_NOTE_read(Reader& reader) {
    CMSG_GUILD_SET_PUBLIC_NOTE obj;

    obj.player_name = reader.read_cstring();

    obj.note = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_GUILD_SET_PUBLIC_NOTE::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_GUILD_SET_PUBLIC_NOTE_size(obj));

    writer.write_u16_be((uint16_t)CMSG_GUILD_SET_PUBLIC_NOTE_size(obj) + 4); /* size */

    writer.write_u32(0x00000234); /* opcode */

    writer.write_cstring(obj.player_name);

    writer.write_cstring(obj.note);

    return writer.m_buf;
}

static size_t CMSG_GUILD_SET_OFFICER_NOTE_size(const CMSG_GUILD_SET_OFFICER_NOTE& obj) {
    return 2 + obj.player_name.size() + obj.note.size();
}

CMSG_GUILD_SET_OFFICER_NOTE CMSG_GUILD_SET_OFFICER_NOTE_read(Reader& reader) {
    CMSG_GUILD_SET_OFFICER_NOTE obj;

    obj.player_name = reader.read_cstring();

    obj.note = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_GUILD_SET_OFFICER_NOTE::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_GUILD_SET_OFFICER_NOTE_size(obj));

    writer.write_u16_be((uint16_t)CMSG_GUILD_SET_OFFICER_NOTE_size(obj) + 4); /* size */

    writer.write_u32(0x00000235); /* opcode */

    writer.write_cstring(obj.player_name);

    writer.write_cstring(obj.note);

    return writer.m_buf;
}

SMSG_LOGIN_VERIFY_WORLD SMSG_LOGIN_VERIFY_WORLD_read(Reader& reader) {
    SMSG_LOGIN_VERIFY_WORLD obj;

    obj.map = static_cast<Map>(reader.read_u32());

    obj.position = ::wow_world_messages::all::Vector3d_read(reader);

    obj.orientation = reader.read_float();

    return obj;
}

std::vector<unsigned char> SMSG_LOGIN_VERIFY_WORLD::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0014);

    writer.write_u16_be(0x0014 + 2); /* size */

    writer.write_u16(0x00000236); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.map));

    Vector3d_write(writer, obj.position);

    writer.write_float(obj.orientation);

    return writer.m_buf;
}

static size_t CMSG_SEND_MAIL_size(const CMSG_SEND_MAIL& obj) {
    return 43 + obj.receiver.size() + obj.subject.size() + obj.body.size();
}

CMSG_SEND_MAIL CMSG_SEND_MAIL_read(Reader& reader) {
    CMSG_SEND_MAIL obj;

    obj.mailbox = reader.read_u64();

    obj.receiver = reader.read_cstring();

    obj.subject = reader.read_cstring();

    obj.body = reader.read_cstring();

    obj.unknown1 = reader.read_u32();

    obj.unknown2 = reader.read_u32();

    obj.item = reader.read_u64();

    obj.money = reader.read_u32();

    obj.cash_on_delivery_amount = reader.read_u32();

    obj.unknown3 = reader.read_u32();

    obj.unknown4 = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_SEND_MAIL::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_SEND_MAIL_size(obj));

    writer.write_u16_be((uint16_t)CMSG_SEND_MAIL_size(obj) + 4); /* size */

    writer.write_u32(0x00000238); /* opcode */

    writer.write_u64(obj.mailbox);

    writer.write_cstring(obj.receiver);

    writer.write_cstring(obj.subject);

    writer.write_cstring(obj.body);

    writer.write_u32(obj.unknown1);

    writer.write_u32(obj.unknown2);

    writer.write_u64(obj.item);

    writer.write_u32(obj.money);

    writer.write_u32(obj.cash_on_delivery_amount);

    writer.write_u32(obj.unknown3);

    writer.write_u32(obj.unknown4);

    return writer.m_buf;
}

static size_t SMSG_SEND_MAIL_RESULT_size(const SMSG_SEND_MAIL_RESULT& obj) {
    size_t _size = 8;

    if (obj.action == MailAction::ITEM_TAKEN) {
        _size += 4;

        if (obj.result == MailResult::ERR_EQUIP_ERROR) {
            _size += 4;
        }
        else if (obj.result == MailResult::OK|| obj.result == MailResult::ERR_CANNOT_SEND_TO_SELF|| obj.result == MailResult::ERR_NOT_ENOUGH_MONEY|| obj.result == MailResult::ERR_RECIPIENT_NOT_FOUND|| obj.result == MailResult::ERR_NOT_YOUR_TEAM|| obj.result == MailResult::ERR_INTERNAL_ERROR|| obj.result == MailResult::ERR_DISABLED_FOR_TRIAL_ACC|| obj.result == MailResult::ERR_RECIPIENT_CAP_REACHED|| obj.result == MailResult::ERR_CANT_SEND_WRAPPED_COD|| obj.result == MailResult::ERR_MAIL_AND_CHAT_SUSPENDED|| obj.result == MailResult::ERR_TOO_MANY_ATTACHMENTS|| obj.result == MailResult::ERR_MAIL_ATTACHMENT_INVALID) {
            _size += 8;
        }

    }
    else if (obj.action == MailAction::SEND|| obj.action == MailAction::MONEY_TAKEN|| obj.action == MailAction::RETURNED_TO_SENDER|| obj.action == MailAction::DELETED|| obj.action == MailAction::MADE_PERMANENT) {
        _size += 4;

        if (obj.result2 == MailResultTwo::ERR_EQUIP_ERROR) {
            _size += 4;
        }

    }

    return _size;
}

SMSG_SEND_MAIL_RESULT SMSG_SEND_MAIL_RESULT_read(Reader& reader) {
    SMSG_SEND_MAIL_RESULT obj;

    obj.mail_id = reader.read_u32();

    obj.action = static_cast<MailAction>(reader.read_u32());

    if (obj.action == MailAction::ITEM_TAKEN) {
        obj.result = static_cast<MailResult>(reader.read_u32());

        if (obj.result == MailResult::ERR_EQUIP_ERROR) {
            obj.equip_error = reader.read_u32();

        }
        else if (obj.result == MailResult::OK|| obj.result == MailResult::ERR_CANNOT_SEND_TO_SELF|| obj.result == MailResult::ERR_NOT_ENOUGH_MONEY|| obj.result == MailResult::ERR_RECIPIENT_NOT_FOUND|| obj.result == MailResult::ERR_NOT_YOUR_TEAM|| obj.result == MailResult::ERR_INTERNAL_ERROR|| obj.result == MailResult::ERR_DISABLED_FOR_TRIAL_ACC|| obj.result == MailResult::ERR_RECIPIENT_CAP_REACHED|| obj.result == MailResult::ERR_CANT_SEND_WRAPPED_COD|| obj.result == MailResult::ERR_MAIL_AND_CHAT_SUSPENDED|| obj.result == MailResult::ERR_TOO_MANY_ATTACHMENTS|| obj.result == MailResult::ERR_MAIL_ATTACHMENT_INVALID) {
            obj.item = reader.read_u32();

            obj.item_count = reader.read_u32();

        }
    }
    else if (obj.action == MailAction::SEND|| obj.action == MailAction::MONEY_TAKEN|| obj.action == MailAction::RETURNED_TO_SENDER|| obj.action == MailAction::DELETED|| obj.action == MailAction::MADE_PERMANENT) {
        obj.result2 = static_cast<MailResultTwo>(reader.read_u32());

        if (obj.result2 == MailResultTwo::ERR_EQUIP_ERROR) {
            obj.equip_error2 = reader.read_u32();

        }
    }
    return obj;
}

std::vector<unsigned char> SMSG_SEND_MAIL_RESULT::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SEND_MAIL_RESULT_size(obj));

    writer.write_u16_be(SMSG_SEND_MAIL_RESULT_size(obj) + 2); /* size */

    writer.write_u16(0x00000239); /* opcode */

    writer.write_u32(obj.mail_id);

    writer.write_u32(static_cast<uint32_t>(obj.action));

    if (obj.action == MailAction::ITEM_TAKEN) {
        writer.write_u32(static_cast<uint32_t>(obj.result));

        if (obj.result == MailResult::ERR_EQUIP_ERROR) {
            writer.write_u32(obj.equip_error);

        }
        else if (obj.result == MailResult::OK|| obj.result == MailResult::ERR_CANNOT_SEND_TO_SELF|| obj.result == MailResult::ERR_NOT_ENOUGH_MONEY|| obj.result == MailResult::ERR_RECIPIENT_NOT_FOUND|| obj.result == MailResult::ERR_NOT_YOUR_TEAM|| obj.result == MailResult::ERR_INTERNAL_ERROR|| obj.result == MailResult::ERR_DISABLED_FOR_TRIAL_ACC|| obj.result == MailResult::ERR_RECIPIENT_CAP_REACHED|| obj.result == MailResult::ERR_CANT_SEND_WRAPPED_COD|| obj.result == MailResult::ERR_MAIL_AND_CHAT_SUSPENDED|| obj.result == MailResult::ERR_TOO_MANY_ATTACHMENTS|| obj.result == MailResult::ERR_MAIL_ATTACHMENT_INVALID) {
            writer.write_u32(obj.item);

            writer.write_u32(obj.item_count);

        }
    }
    else if (obj.action == MailAction::SEND|| obj.action == MailAction::MONEY_TAKEN|| obj.action == MailAction::RETURNED_TO_SENDER|| obj.action == MailAction::DELETED|| obj.action == MailAction::MADE_PERMANENT) {
        writer.write_u32(static_cast<uint32_t>(obj.result2));

        if (obj.result2 == MailResultTwo::ERR_EQUIP_ERROR) {
            writer.write_u32(obj.equip_error2);

        }
    }
    return writer.m_buf;
}

CMSG_GET_MAIL_LIST CMSG_GET_MAIL_LIST_read(Reader& reader) {
    CMSG_GET_MAIL_LIST obj;

    obj.mailbox = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_GET_MAIL_LIST::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x0000023a); /* opcode */

    writer.write_u64(obj.mailbox);

    return writer.m_buf;
}

static size_t SMSG_MAIL_LIST_RESULT_size(const SMSG_MAIL_LIST_RESULT& obj) {
    size_t _size = 1;

    for(const auto& v : obj.mails) {
        _size += Mail_size(v);
    }

    return _size;
}

SMSG_MAIL_LIST_RESULT SMSG_MAIL_LIST_RESULT_read(Reader& reader) {
    SMSG_MAIL_LIST_RESULT obj;

    obj.amount_of_mails = reader.read_u8();

    for (auto i = 0; i < obj.amount_of_mails; ++i) {
        obj.mails.push_back(::wow_world_messages::vanilla::Mail_read(reader));
    }

    return obj;
}

std::vector<unsigned char> SMSG_MAIL_LIST_RESULT::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_MAIL_LIST_RESULT_size(obj));

    writer.write_u16_be(SMSG_MAIL_LIST_RESULT_size(obj) + 2); /* size */

    writer.write_u16(0x0000023b); /* opcode */

    writer.write_u8(obj.amount_of_mails);

    for (const auto& v : obj.mails) {
        Mail_write(writer, v);
    }

    return writer.m_buf;
}

CMSG_BATTLEFIELD_LIST CMSG_BATTLEFIELD_LIST_read(Reader& reader) {
    CMSG_BATTLEFIELD_LIST obj;

    obj.map = static_cast<Map>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> CMSG_BATTLEFIELD_LIST::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 4); /* size */

    writer.write_u32(0x0000023c); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.map));

    return writer.m_buf;
}

static size_t SMSG_BATTLEFIELD_LIST_size(const SMSG_BATTLEFIELD_LIST& obj) {
    return 17 + 4 * obj.battlegrounds.size();
}

SMSG_BATTLEFIELD_LIST SMSG_BATTLEFIELD_LIST_read(Reader& reader) {
    SMSG_BATTLEFIELD_LIST obj;

    obj.battlemaster = reader.read_u64();

    obj.map = static_cast<Map>(reader.read_u32());

    obj.bracket = static_cast<BattlegroundBracket>(reader.read_u8());

    obj.number_of_battlegrounds = reader.read_u32();

    for (auto i = 0; i < obj.number_of_battlegrounds; ++i) {
        obj.battlegrounds.push_back(reader.read_u32());
    }

    return obj;
}

std::vector<unsigned char> SMSG_BATTLEFIELD_LIST::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_BATTLEFIELD_LIST_size(obj));

    writer.write_u16_be(SMSG_BATTLEFIELD_LIST_size(obj) + 2); /* size */

    writer.write_u16(0x0000023d); /* opcode */

    writer.write_u64(obj.battlemaster);

    writer.write_u32(static_cast<uint32_t>(obj.map));

    writer.write_u8(static_cast<uint8_t>(obj.bracket));

    writer.write_u32(obj.number_of_battlegrounds);

    for (const auto& v : obj.battlegrounds) {
        writer.write_u32(v);
    }

    return writer.m_buf;
}

CMSG_BATTLEFIELD_JOIN CMSG_BATTLEFIELD_JOIN_read(Reader& reader) {
    CMSG_BATTLEFIELD_JOIN obj;

    obj.map = static_cast<Map>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> CMSG_BATTLEFIELD_JOIN::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 4); /* size */

    writer.write_u32(0x0000023e); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.map));

    return writer.m_buf;
}

CMSG_ITEM_TEXT_QUERY CMSG_ITEM_TEXT_QUERY_read(Reader& reader) {
    CMSG_ITEM_TEXT_QUERY obj;

    obj.item_text_id = reader.read_u32();

    obj.mail_id = reader.read_u32();

    obj.unknown1 = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_ITEM_TEXT_QUERY::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 4); /* size */

    writer.write_u32(0x00000243); /* opcode */

    writer.write_u32(obj.item_text_id);

    writer.write_u32(obj.mail_id);

    writer.write_u32(obj.unknown1);

    return writer.m_buf;
}

static size_t SMSG_ITEM_TEXT_QUERY_RESPONSE_size(const SMSG_ITEM_TEXT_QUERY_RESPONSE& obj) {
    return 5 + obj.text.size();
}

SMSG_ITEM_TEXT_QUERY_RESPONSE SMSG_ITEM_TEXT_QUERY_RESPONSE_read(Reader& reader) {
    SMSG_ITEM_TEXT_QUERY_RESPONSE obj;

    obj.item_text_id = reader.read_u32();

    obj.text = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> SMSG_ITEM_TEXT_QUERY_RESPONSE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_ITEM_TEXT_QUERY_RESPONSE_size(obj));

    writer.write_u16_be(SMSG_ITEM_TEXT_QUERY_RESPONSE_size(obj) + 2); /* size */

    writer.write_u16(0x00000244); /* opcode */

    writer.write_u32(obj.item_text_id);

    writer.write_cstring(obj.text);

    return writer.m_buf;
}

CMSG_MAIL_TAKE_MONEY CMSG_MAIL_TAKE_MONEY_read(Reader& reader) {
    CMSG_MAIL_TAKE_MONEY obj;

    obj.mailbox = reader.read_u64();

    obj.mail_id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_MAIL_TAKE_MONEY::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 4); /* size */

    writer.write_u32(0x00000245); /* opcode */

    writer.write_u64(obj.mailbox);

    writer.write_u32(obj.mail_id);

    return writer.m_buf;
}

CMSG_MAIL_TAKE_ITEM CMSG_MAIL_TAKE_ITEM_read(Reader& reader) {
    CMSG_MAIL_TAKE_ITEM obj;

    obj.mailbox = reader.read_u64();

    obj.mail_id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_MAIL_TAKE_ITEM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 4); /* size */

    writer.write_u32(0x00000246); /* opcode */

    writer.write_u64(obj.mailbox);

    writer.write_u32(obj.mail_id);

    return writer.m_buf;
}

CMSG_MAIL_MARK_AS_READ CMSG_MAIL_MARK_AS_READ_read(Reader& reader) {
    CMSG_MAIL_MARK_AS_READ obj;

    obj.mailbox = reader.read_u64();

    obj.mail_id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_MAIL_MARK_AS_READ::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 4); /* size */

    writer.write_u32(0x00000247); /* opcode */

    writer.write_u64(obj.mailbox);

    writer.write_u32(obj.mail_id);

    return writer.m_buf;
}

CMSG_MAIL_RETURN_TO_SENDER CMSG_MAIL_RETURN_TO_SENDER_read(Reader& reader) {
    CMSG_MAIL_RETURN_TO_SENDER obj;

    obj.mailbox_id = reader.read_u64();

    obj.mail_id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_MAIL_RETURN_TO_SENDER::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 4); /* size */

    writer.write_u32(0x00000248); /* opcode */

    writer.write_u64(obj.mailbox_id);

    writer.write_u32(obj.mail_id);

    return writer.m_buf;
}

CMSG_MAIL_DELETE CMSG_MAIL_DELETE_read(Reader& reader) {
    CMSG_MAIL_DELETE obj;

    obj.mailbox_id = reader.read_u64();

    obj.mail_id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_MAIL_DELETE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 4); /* size */

    writer.write_u32(0x00000249); /* opcode */

    writer.write_u64(obj.mailbox_id);

    writer.write_u32(obj.mail_id);

    return writer.m_buf;
}

CMSG_MAIL_CREATE_TEXT_ITEM CMSG_MAIL_CREATE_TEXT_ITEM_read(Reader& reader) {
    CMSG_MAIL_CREATE_TEXT_ITEM obj;

    obj.mailbox = reader.read_u64();

    obj.mail_id = reader.read_u32();

    obj.mail_template_id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_MAIL_CREATE_TEXT_ITEM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0010);

    writer.write_u16_be(0x0010 + 4); /* size */

    writer.write_u32(0x0000024a); /* opcode */

    writer.write_u64(obj.mailbox);

    writer.write_u32(obj.mail_id);

    writer.write_u32(obj.mail_template_id);

    return writer.m_buf;
}

static size_t SMSG_SPELLLOGMISS_size(const SMSG_SPELLLOGMISS& obj) {
    return 17 + 9 * obj.targets.size();
}

SMSG_SPELLLOGMISS SMSG_SPELLLOGMISS_read(Reader& reader) {
    SMSG_SPELLLOGMISS obj;

    obj.id = reader.read_u32();

    obj.caster = reader.read_u64();

    obj.unknown1 = reader.read_u8();

    obj.amount_of_targets = reader.read_u32();

    for (auto i = 0; i < obj.amount_of_targets; ++i) {
        obj.targets.push_back(::wow_world_messages::vanilla::SpellLogMiss_read(reader));
    }

    return obj;
}

std::vector<unsigned char> SMSG_SPELLLOGMISS::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SPELLLOGMISS_size(obj));

    writer.write_u16_be(SMSG_SPELLLOGMISS_size(obj) + 2); /* size */

    writer.write_u16(0x0000024b); /* opcode */

    writer.write_u32(obj.id);

    writer.write_u64(obj.caster);

    writer.write_u8(obj.unknown1);

    writer.write_u32(obj.amount_of_targets);

    for (const auto& v : obj.targets) {
        SpellLogMiss_write(writer, v);
    }

    return writer.m_buf;
}

static size_t SMSG_SPELLLOGEXECUTE_size(const SMSG_SPELLLOGEXECUTE& obj) {
    size_t _size = 8 + ::wow_world_messages::util::wwm_packed_guid_size(obj.caster);

    for(const auto& v : obj.logs) {
        _size += SpellLog_size(v);
    }

    return _size;
}

SMSG_SPELLLOGEXECUTE SMSG_SPELLLOGEXECUTE_read(Reader& reader) {
    SMSG_SPELLLOGEXECUTE obj;

    obj.caster = reader.read_packed_guid();

    obj.spell = reader.read_u32();

    obj.amount_of_effects = reader.read_u32();

    for (auto i = 0; i < obj.amount_of_effects; ++i) {
        obj.logs.push_back(::wow_world_messages::vanilla::SpellLog_read(reader));
    }

    return obj;
}

std::vector<unsigned char> SMSG_SPELLLOGEXECUTE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SPELLLOGEXECUTE_size(obj));

    writer.write_u16_be(SMSG_SPELLLOGEXECUTE_size(obj) + 2); /* size */

    writer.write_u16(0x0000024c); /* opcode */

    writer.write_packed_guid(obj.caster);

    writer.write_u32(obj.spell);

    writer.write_u32(obj.amount_of_effects);

    for (const auto& v : obj.logs) {
        SpellLog_write(writer, v);
    }

    return writer.m_buf;
}

static size_t SMSG_PERIODICAURALOG_size(const SMSG_PERIODICAURALOG& obj) {
    size_t _size = 8 + ::wow_world_messages::util::wwm_packed_guid_size(obj.target) + ::wow_world_messages::util::wwm_packed_guid_size(obj.caster);

    for(const auto& v : obj.auras) {
        _size += AuraLog_size(v);
    }

    return _size;
}

SMSG_PERIODICAURALOG SMSG_PERIODICAURALOG_read(Reader& reader) {
    SMSG_PERIODICAURALOG obj;

    obj.target = reader.read_packed_guid();

    obj.caster = reader.read_packed_guid();

    obj.spell = reader.read_u32();

    obj.amount_of_auras = reader.read_u32();

    for (auto i = 0; i < obj.amount_of_auras; ++i) {
        obj.auras.push_back(::wow_world_messages::vanilla::AuraLog_read(reader));
    }

    return obj;
}

std::vector<unsigned char> SMSG_PERIODICAURALOG::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_PERIODICAURALOG_size(obj));

    writer.write_u16_be(SMSG_PERIODICAURALOG_size(obj) + 2); /* size */

    writer.write_u16(0x0000024e); /* opcode */

    writer.write_packed_guid(obj.target);

    writer.write_packed_guid(obj.caster);

    writer.write_u32(obj.spell);

    writer.write_u32(obj.amount_of_auras);

    for (const auto& v : obj.auras) {
        AuraLog_write(writer, v);
    }

    return writer.m_buf;
}

SMSG_SPELLDAMAGESHIELD SMSG_SPELLDAMAGESHIELD_read(Reader& reader) {
    SMSG_SPELLDAMAGESHIELD obj;

    obj.victim = reader.read_u64();

    obj.caster = reader.read_u64();

    obj.damage = reader.read_u32();

    obj.school = static_cast<SpellSchool>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_SPELLDAMAGESHIELD::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0018);

    writer.write_u16_be(0x0018 + 2); /* size */

    writer.write_u16(0x0000024f); /* opcode */

    writer.write_u64(obj.victim);

    writer.write_u64(obj.caster);

    writer.write_u32(obj.damage);

    writer.write_u32(static_cast<uint32_t>(obj.school));

    return writer.m_buf;
}

static size_t SMSG_SPELLNONMELEEDAMAGELOG_size(const SMSG_SPELLNONMELEEDAMAGELOG& obj) {
    return 28 + ::wow_world_messages::util::wwm_packed_guid_size(obj.target) + ::wow_world_messages::util::wwm_packed_guid_size(obj.attacker);
}

SMSG_SPELLNONMELEEDAMAGELOG SMSG_SPELLNONMELEEDAMAGELOG_read(Reader& reader) {
    SMSG_SPELLNONMELEEDAMAGELOG obj;

    obj.target = reader.read_packed_guid();

    obj.attacker = reader.read_packed_guid();

    obj.spell = reader.read_u32();

    obj.damage = reader.read_u32();

    obj.school = static_cast<SpellSchool>(reader.read_u8());

    obj.absorbed_damage = reader.read_u32();

    obj.resisted = reader.read_u32();

    obj.periodic_log = reader.read_bool8();

    obj.unused = reader.read_u8();

    obj.blocked = reader.read_u32();

    obj.hit_info = static_cast<HitInfo>(reader.read_u32());

    obj.extend_flag = reader.read_u8();

    return obj;
}

std::vector<unsigned char> SMSG_SPELLNONMELEEDAMAGELOG::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SPELLNONMELEEDAMAGELOG_size(obj));

    writer.write_u16_be(SMSG_SPELLNONMELEEDAMAGELOG_size(obj) + 2); /* size */

    writer.write_u16(0x00000250); /* opcode */

    writer.write_packed_guid(obj.target);

    writer.write_packed_guid(obj.attacker);

    writer.write_u32(obj.spell);

    writer.write_u32(obj.damage);

    writer.write_u8(static_cast<uint8_t>(obj.school));

    writer.write_u32(obj.absorbed_damage);

    writer.write_u32(obj.resisted);

    writer.write_bool8(obj.periodic_log);

    writer.write_u8(obj.unused);

    writer.write_u32(obj.blocked);

    writer.write_u32(static_cast<uint32_t>(obj.hit_info));

    writer.write_u8(obj.extend_flag);

    return writer.m_buf;
}

CMSG_LEARN_TALENT CMSG_LEARN_TALENT_read(Reader& reader) {
    CMSG_LEARN_TALENT obj;

    obj.talent = static_cast<Talent>(reader.read_u32());

    obj.requested_rank = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_LEARN_TALENT::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x00000251); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.talent));

    writer.write_u32(obj.requested_rank);

    return writer.m_buf;
}

SMSG_ZONE_UNDER_ATTACK SMSG_ZONE_UNDER_ATTACK_read(Reader& reader) {
    SMSG_ZONE_UNDER_ATTACK obj;

    obj.zone_id = static_cast<Area>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_ZONE_UNDER_ATTACK::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x00000254); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.zone_id));

    return writer.m_buf;
}

MSG_AUCTION_HELLO_Client MSG_AUCTION_HELLO_Client_read(Reader& reader) {
    MSG_AUCTION_HELLO_Client obj;

    obj.auctioneer = reader.read_u64();

    return obj;
}

std::vector<unsigned char> MSG_AUCTION_HELLO_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x00000255); /* opcode */

    writer.write_u64(obj.auctioneer);

    return writer.m_buf;
}

MSG_AUCTION_HELLO_Server MSG_AUCTION_HELLO_Server_read(Reader& reader) {
    MSG_AUCTION_HELLO_Server obj;

    obj.auctioneer = reader.read_u64();

    obj.auction_house = static_cast<AuctionHouse>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> MSG_AUCTION_HELLO_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 2); /* size */

    writer.write_u16(0x00000255); /* opcode */

    writer.write_u64(obj.auctioneer);

    writer.write_u32(static_cast<uint32_t>(obj.auction_house));

    return writer.m_buf;
}

CMSG_AUCTION_SELL_ITEM CMSG_AUCTION_SELL_ITEM_read(Reader& reader) {
    CMSG_AUCTION_SELL_ITEM obj;

    obj.auctioneer = reader.read_u64();

    obj.item = reader.read_u64();

    obj.starting_bid = reader.read_u32();

    obj.buyout = reader.read_u32();

    obj.auction_duration_in_minutes = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_AUCTION_SELL_ITEM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x001c);

    writer.write_u16_be(0x001c + 4); /* size */

    writer.write_u32(0x00000256); /* opcode */

    writer.write_u64(obj.auctioneer);

    writer.write_u64(obj.item);

    writer.write_u32(obj.starting_bid);

    writer.write_u32(obj.buyout);

    writer.write_u32(obj.auction_duration_in_minutes);

    return writer.m_buf;
}

CMSG_AUCTION_REMOVE_ITEM CMSG_AUCTION_REMOVE_ITEM_read(Reader& reader) {
    CMSG_AUCTION_REMOVE_ITEM obj;

    obj.auctioneer = reader.read_u64();

    obj.auction_id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_AUCTION_REMOVE_ITEM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 4); /* size */

    writer.write_u32(0x00000257); /* opcode */

    writer.write_u64(obj.auctioneer);

    writer.write_u32(obj.auction_id);

    return writer.m_buf;
}

static size_t CMSG_AUCTION_LIST_ITEMS_size(const CMSG_AUCTION_LIST_ITEMS& obj) {
    return 32 + obj.searched_name.size();
}

CMSG_AUCTION_LIST_ITEMS CMSG_AUCTION_LIST_ITEMS_read(Reader& reader) {
    CMSG_AUCTION_LIST_ITEMS obj;

    obj.auctioneer = reader.read_u64();

    obj.list_start_item = reader.read_u32();

    obj.searched_name = reader.read_cstring();

    obj.minimum_level = reader.read_u8();

    obj.maximum_level = reader.read_u8();

    obj.auction_slot_id = reader.read_u32();

    obj.auction_main_category = reader.read_u32();

    obj.auction_sub_category = reader.read_u32();

    obj.auction_quality = static_cast<ItemQuality>(reader.read_u32());

    obj.usable = reader.read_u8();

    return obj;
}

std::vector<unsigned char> CMSG_AUCTION_LIST_ITEMS::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_AUCTION_LIST_ITEMS_size(obj));

    writer.write_u16_be((uint16_t)CMSG_AUCTION_LIST_ITEMS_size(obj) + 4); /* size */

    writer.write_u32(0x00000258); /* opcode */

    writer.write_u64(obj.auctioneer);

    writer.write_u32(obj.list_start_item);

    writer.write_cstring(obj.searched_name);

    writer.write_u8(obj.minimum_level);

    writer.write_u8(obj.maximum_level);

    writer.write_u32(obj.auction_slot_id);

    writer.write_u32(obj.auction_main_category);

    writer.write_u32(obj.auction_sub_category);

    writer.write_u32(static_cast<uint32_t>(obj.auction_quality));

    writer.write_u8(obj.usable);

    return writer.m_buf;
}

CMSG_AUCTION_LIST_OWNER_ITEMS CMSG_AUCTION_LIST_OWNER_ITEMS_read(Reader& reader) {
    CMSG_AUCTION_LIST_OWNER_ITEMS obj;

    obj.auctioneer = reader.read_u64();

    obj.list_from = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_AUCTION_LIST_OWNER_ITEMS::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 4); /* size */

    writer.write_u32(0x00000259); /* opcode */

    writer.write_u64(obj.auctioneer);

    writer.write_u32(obj.list_from);

    return writer.m_buf;
}

CMSG_AUCTION_PLACE_BID CMSG_AUCTION_PLACE_BID_read(Reader& reader) {
    CMSG_AUCTION_PLACE_BID obj;

    obj.auctioneer = reader.read_u64();

    obj.auction_id = reader.read_u32();

    obj.price = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_AUCTION_PLACE_BID::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0010);

    writer.write_u16_be(0x0010 + 4); /* size */

    writer.write_u32(0x0000025a); /* opcode */

    writer.write_u64(obj.auctioneer);

    writer.write_u32(obj.auction_id);

    writer.write_u32(obj.price);

    return writer.m_buf;
}

static size_t SMSG_AUCTION_COMMAND_RESULT_size(const SMSG_AUCTION_COMMAND_RESULT& obj) {
    size_t _size = 8;

    if (obj.action == AuctionCommandAction::BID_PLACED) {
        _size += 4;

        if (obj.result == AuctionCommandResult::OK) {
            _size += 4;
        }
        else if (obj.result == AuctionCommandResult::ERR_INVENTORY) {
            _size += 1;
        }
        else if (obj.result == AuctionCommandResult::ERR_HIGHER_BID) {
            _size += 16;
        }

    }
    else if (obj.action == AuctionCommandAction::STARTED|| obj.action == AuctionCommandAction::REMOVED) {
        _size += 4;

        if (obj.result2 == AuctionCommandResultTwo::ERR_INVENTORY) {
            _size += 1;
        }
        else if (obj.result2 == AuctionCommandResultTwo::ERR_HIGHER_BID) {
            _size += 16;
        }

    }

    return _size;
}

SMSG_AUCTION_COMMAND_RESULT SMSG_AUCTION_COMMAND_RESULT_read(Reader& reader) {
    SMSG_AUCTION_COMMAND_RESULT obj;

    obj.auction_id = reader.read_u32();

    obj.action = static_cast<AuctionCommandAction>(reader.read_u32());

    if (obj.action == AuctionCommandAction::BID_PLACED) {
        obj.result = static_cast<AuctionCommandResult>(reader.read_u32());

        if (obj.result == AuctionCommandResult::OK) {
            obj.auction_outbid1 = reader.read_u32();

        }
        else if (obj.result == AuctionCommandResult::ERR_INVENTORY) {
            obj.inventory_result = static_cast<InventoryResult>(reader.read_u8());

        }
        else if (obj.result == AuctionCommandResult::ERR_HIGHER_BID) {
            obj.higher_bidder = reader.read_u64();

            obj.new_bid = reader.read_u32();

            obj.auction_outbid2 = reader.read_u32();

        }
    }
    else if (obj.action == AuctionCommandAction::STARTED|| obj.action == AuctionCommandAction::REMOVED) {
        obj.result2 = static_cast<AuctionCommandResultTwo>(reader.read_u32());

        if (obj.result2 == AuctionCommandResultTwo::ERR_INVENTORY) {
            obj.inventory_result2 = static_cast<InventoryResult>(reader.read_u8());

        }
        else if (obj.result2 == AuctionCommandResultTwo::ERR_HIGHER_BID) {
            obj.higher_bidder2 = reader.read_u64();

            obj.new_bid2 = reader.read_u32();

            obj.auction_outbid3 = reader.read_u32();

        }
    }
    return obj;
}

std::vector<unsigned char> SMSG_AUCTION_COMMAND_RESULT::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_AUCTION_COMMAND_RESULT_size(obj));

    writer.write_u16_be(SMSG_AUCTION_COMMAND_RESULT_size(obj) + 2); /* size */

    writer.write_u16(0x0000025b); /* opcode */

    writer.write_u32(obj.auction_id);

    writer.write_u32(static_cast<uint32_t>(obj.action));

    if (obj.action == AuctionCommandAction::BID_PLACED) {
        writer.write_u32(static_cast<uint32_t>(obj.result));

        if (obj.result == AuctionCommandResult::OK) {
            writer.write_u32(obj.auction_outbid1);

        }
        else if (obj.result == AuctionCommandResult::ERR_INVENTORY) {
            writer.write_u8(static_cast<uint8_t>(obj.inventory_result));

        }
        else if (obj.result == AuctionCommandResult::ERR_HIGHER_BID) {
            writer.write_u64(obj.higher_bidder);

            writer.write_u32(obj.new_bid);

            writer.write_u32(obj.auction_outbid2);

        }
    }
    else if (obj.action == AuctionCommandAction::STARTED|| obj.action == AuctionCommandAction::REMOVED) {
        writer.write_u32(static_cast<uint32_t>(obj.result2));

        if (obj.result2 == AuctionCommandResultTwo::ERR_INVENTORY) {
            writer.write_u8(static_cast<uint8_t>(obj.inventory_result2));

        }
        else if (obj.result2 == AuctionCommandResultTwo::ERR_HIGHER_BID) {
            writer.write_u64(obj.higher_bidder2);

            writer.write_u32(obj.new_bid2);

            writer.write_u32(obj.auction_outbid3);

        }
    }
    return writer.m_buf;
}

static size_t SMSG_AUCTION_LIST_RESULT_size(const SMSG_AUCTION_LIST_RESULT& obj) {
    return 8 + 64 * obj.auctions.size();
}

SMSG_AUCTION_LIST_RESULT SMSG_AUCTION_LIST_RESULT_read(Reader& reader) {
    SMSG_AUCTION_LIST_RESULT obj;

    obj.count = reader.read_u32();

    for (auto i = 0; i < obj.count; ++i) {
        obj.auctions.push_back(::wow_world_messages::vanilla::AuctionListItem_read(reader));
    }

    obj.total_amount_of_auctions = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_AUCTION_LIST_RESULT::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_AUCTION_LIST_RESULT_size(obj));

    writer.write_u16_be(SMSG_AUCTION_LIST_RESULT_size(obj) + 2); /* size */

    writer.write_u16(0x0000025c); /* opcode */

    writer.write_u32(obj.count);

    for (const auto& v : obj.auctions) {
        AuctionListItem_write(writer, v);
    }

    writer.write_u32(obj.total_amount_of_auctions);

    return writer.m_buf;
}

static size_t SMSG_AUCTION_OWNER_LIST_RESULT_size(const SMSG_AUCTION_OWNER_LIST_RESULT& obj) {
    return 8 + 64 * obj.auctions.size();
}

SMSG_AUCTION_OWNER_LIST_RESULT SMSG_AUCTION_OWNER_LIST_RESULT_read(Reader& reader) {
    SMSG_AUCTION_OWNER_LIST_RESULT obj;

    obj.count = reader.read_u32();

    for (auto i = 0; i < obj.count; ++i) {
        obj.auctions.push_back(::wow_world_messages::vanilla::AuctionListItem_read(reader));
    }

    obj.total_amount_of_auctions = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_AUCTION_OWNER_LIST_RESULT::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_AUCTION_OWNER_LIST_RESULT_size(obj));

    writer.write_u16_be(SMSG_AUCTION_OWNER_LIST_RESULT_size(obj) + 2); /* size */

    writer.write_u16(0x0000025d); /* opcode */

    writer.write_u32(obj.count);

    for (const auto& v : obj.auctions) {
        AuctionListItem_write(writer, v);
    }

    writer.write_u32(obj.total_amount_of_auctions);

    return writer.m_buf;
}

SMSG_AUCTION_BIDDER_NOTIFICATION SMSG_AUCTION_BIDDER_NOTIFICATION_read(Reader& reader) {
    SMSG_AUCTION_BIDDER_NOTIFICATION obj;

    obj.auction_house = static_cast<AuctionHouse>(reader.read_u32());

    obj.auction_id = reader.read_u32();

    obj.bidder = reader.read_u64();

    obj.won = reader.read_u32();

    obj.out_bid = reader.read_u32();

    obj.item_template = reader.read_u32();

    obj.item_random_property_id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_AUCTION_BIDDER_NOTIFICATION::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0020);

    writer.write_u16_be(0x0020 + 2); /* size */

    writer.write_u16(0x0000025e); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.auction_house));

    writer.write_u32(obj.auction_id);

    writer.write_u64(obj.bidder);

    writer.write_u32(obj.won);

    writer.write_u32(obj.out_bid);

    writer.write_u32(obj.item_template);

    writer.write_u32(obj.item_random_property_id);

    return writer.m_buf;
}

SMSG_AUCTION_OWNER_NOTIFICATION SMSG_AUCTION_OWNER_NOTIFICATION_read(Reader& reader) {
    SMSG_AUCTION_OWNER_NOTIFICATION obj;

    obj.auction_id = reader.read_u32();

    obj.bid = reader.read_u32();

    obj.auction_out_bid = reader.read_u32();

    obj.bidder = reader.read_u64();

    obj.item = reader.read_u32();

    obj.item_random_property_id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_AUCTION_OWNER_NOTIFICATION::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x001c);

    writer.write_u16_be(0x001c + 2); /* size */

    writer.write_u16(0x0000025f); /* opcode */

    writer.write_u32(obj.auction_id);

    writer.write_u32(obj.bid);

    writer.write_u32(obj.auction_out_bid);

    writer.write_u64(obj.bidder);

    writer.write_u32(obj.item);

    writer.write_u32(obj.item_random_property_id);

    return writer.m_buf;
}

SMSG_PROCRESIST SMSG_PROCRESIST_read(Reader& reader) {
    SMSG_PROCRESIST obj;

    obj.caster = reader.read_u64();

    obj.target = reader.read_u64();

    obj.id = reader.read_u32();

    obj.log_format = static_cast<LogFormat>(reader.read_u8());

    return obj;
}

std::vector<unsigned char> SMSG_PROCRESIST::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0015);

    writer.write_u16_be(0x0015 + 2); /* size */

    writer.write_u16(0x00000260); /* opcode */

    writer.write_u64(obj.caster);

    writer.write_u64(obj.target);

    writer.write_u32(obj.id);

    writer.write_u8(static_cast<uint8_t>(obj.log_format));

    return writer.m_buf;
}

static size_t SMSG_DISPEL_FAILED_size(const SMSG_DISPEL_FAILED& obj) {
    size_t _size = 16;

    for(const auto& v : obj.spells) {
        _size += 4;
    }

    return _size;
}

SMSG_DISPEL_FAILED SMSG_DISPEL_FAILED_read(Reader& reader, size_t body_size) {
    SMSG_DISPEL_FAILED obj;
    size_t _size = 0;

    obj.caster = reader.read_u64();
    _size += 8;

    obj.target = reader.read_u64();
    _size += 8;

    while (_size < body_size) {
        obj.spells.push_back(reader.read_u32());
        _size += 4;
    }

    return obj;
}

std::vector<unsigned char> SMSG_DISPEL_FAILED::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_DISPEL_FAILED_size(obj));

    writer.write_u16_be(SMSG_DISPEL_FAILED_size(obj) + 2); /* size */

    writer.write_u16(0x00000262); /* opcode */

    writer.write_u64(obj.caster);

    writer.write_u64(obj.target);

    for (const auto& v : obj.spells) {
        writer.write_u32(v);
    }

    return writer.m_buf;
}

SMSG_SPELLORDAMAGE_IMMUNE SMSG_SPELLORDAMAGE_IMMUNE_read(Reader& reader) {
    SMSG_SPELLORDAMAGE_IMMUNE obj;

    obj.caster = reader.read_u64();

    obj.target = reader.read_u64();

    obj.id = reader.read_u32();

    obj.debug_log_format = reader.read_bool8();

    return obj;
}

std::vector<unsigned char> SMSG_SPELLORDAMAGE_IMMUNE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0015);

    writer.write_u16_be(0x0015 + 2); /* size */

    writer.write_u16(0x00000263); /* opcode */

    writer.write_u64(obj.caster);

    writer.write_u64(obj.target);

    writer.write_u32(obj.id);

    writer.write_bool8(obj.debug_log_format);

    return writer.m_buf;
}

static size_t CMSG_AUCTION_LIST_BIDDER_ITEMS_size(const CMSG_AUCTION_LIST_BIDDER_ITEMS& obj) {
    return 16 + 4 * obj.outbid_item_ids.size();
}

CMSG_AUCTION_LIST_BIDDER_ITEMS CMSG_AUCTION_LIST_BIDDER_ITEMS_read(Reader& reader) {
    CMSG_AUCTION_LIST_BIDDER_ITEMS obj;

    obj.auctioneer = reader.read_u64();

    obj.start_from_page = reader.read_u32();

    obj.amount_of_outbid_items = reader.read_u32();

    for (auto i = 0; i < obj.amount_of_outbid_items; ++i) {
        obj.outbid_item_ids.push_back(reader.read_u32());
    }

    return obj;
}

std::vector<unsigned char> CMSG_AUCTION_LIST_BIDDER_ITEMS::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_AUCTION_LIST_BIDDER_ITEMS_size(obj));

    writer.write_u16_be((uint16_t)CMSG_AUCTION_LIST_BIDDER_ITEMS_size(obj) + 4); /* size */

    writer.write_u32(0x00000264); /* opcode */

    writer.write_u64(obj.auctioneer);

    writer.write_u32(obj.start_from_page);

    writer.write_u32(obj.amount_of_outbid_items);

    for (const auto& v : obj.outbid_item_ids) {
        writer.write_u32(v);
    }

    return writer.m_buf;
}

static size_t SMSG_AUCTION_BIDDER_LIST_RESULT_size(const SMSG_AUCTION_BIDDER_LIST_RESULT& obj) {
    return 8 + 64 * obj.auctions.size();
}

SMSG_AUCTION_BIDDER_LIST_RESULT SMSG_AUCTION_BIDDER_LIST_RESULT_read(Reader& reader) {
    SMSG_AUCTION_BIDDER_LIST_RESULT obj;

    obj.count = reader.read_u32();

    for (auto i = 0; i < obj.count; ++i) {
        obj.auctions.push_back(::wow_world_messages::vanilla::AuctionListItem_read(reader));
    }

    obj.total_amount_of_auctions = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_AUCTION_BIDDER_LIST_RESULT::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_AUCTION_BIDDER_LIST_RESULT_size(obj));

    writer.write_u16_be(SMSG_AUCTION_BIDDER_LIST_RESULT_size(obj) + 2); /* size */

    writer.write_u16(0x00000265); /* opcode */

    writer.write_u32(obj.count);

    for (const auto& v : obj.auctions) {
        AuctionListItem_write(writer, v);
    }

    writer.write_u32(obj.total_amount_of_auctions);

    return writer.m_buf;
}

SMSG_SET_FLAT_SPELL_MODIFIER SMSG_SET_FLAT_SPELL_MODIFIER_read(Reader& reader) {
    SMSG_SET_FLAT_SPELL_MODIFIER obj;

    obj.eff = reader.read_u8();

    obj.op = reader.read_u8();

    obj.value = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_SET_FLAT_SPELL_MODIFIER::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0006);

    writer.write_u16_be(0x0006 + 2); /* size */

    writer.write_u16(0x00000266); /* opcode */

    writer.write_u8(obj.eff);

    writer.write_u8(obj.op);

    writer.write_u32(obj.value);

    return writer.m_buf;
}

SMSG_SET_PCT_SPELL_MODIFIER SMSG_SET_PCT_SPELL_MODIFIER_read(Reader& reader) {
    SMSG_SET_PCT_SPELL_MODIFIER obj;

    obj.eff = reader.read_u8();

    obj.op = reader.read_u8();

    obj.value = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_SET_PCT_SPELL_MODIFIER::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0006);

    writer.write_u16_be(0x0006 + 2); /* size */

    writer.write_u16(0x00000267); /* opcode */

    writer.write_u8(obj.eff);

    writer.write_u8(obj.op);

    writer.write_u32(obj.value);

    return writer.m_buf;
}

CMSG_SET_AMMO CMSG_SET_AMMO_read(Reader& reader) {
    CMSG_SET_AMMO obj;

    obj.item = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_SET_AMMO::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 4); /* size */

    writer.write_u32(0x00000268); /* opcode */

    writer.write_u32(obj.item);

    return writer.m_buf;
}

SMSG_CORPSE_RECLAIM_DELAY SMSG_CORPSE_RECLAIM_DELAY_read(Reader& reader) {
    SMSG_CORPSE_RECLAIM_DELAY obj;

    obj.delay = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_CORPSE_RECLAIM_DELAY::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x00000269); /* opcode */

    writer.write_u32(obj.delay);

    return writer.m_buf;
}

CMSG_SET_ACTIVE_MOVER CMSG_SET_ACTIVE_MOVER_read(Reader& reader) {
    CMSG_SET_ACTIVE_MOVER obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_SET_ACTIVE_MOVER::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x0000026a); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

CMSG_PET_CANCEL_AURA CMSG_PET_CANCEL_AURA_read(Reader& reader) {
    CMSG_PET_CANCEL_AURA obj;

    obj.guid = reader.read_u64();

    obj.id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_PET_CANCEL_AURA::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 4); /* size */

    writer.write_u32(0x0000026b); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.id);

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_CANCEL_AUTO_REPEAT_SPELL::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x0000026d); /* opcode */

    return writer.m_buf;
}

MSG_LIST_STABLED_PETS_Client MSG_LIST_STABLED_PETS_Client_read(Reader& reader) {
    MSG_LIST_STABLED_PETS_Client obj;

    obj.npc = reader.read_u64();

    return obj;
}

std::vector<unsigned char> MSG_LIST_STABLED_PETS_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x0000026f); /* opcode */

    writer.write_u64(obj.npc);

    return writer.m_buf;
}

static size_t MSG_LIST_STABLED_PETS_Server_size(const MSG_LIST_STABLED_PETS_Server& obj) {
    size_t _size = 10;

    for(const auto& v : obj.pets) {
        _size += StabledPet_size(v);
    }

    return _size;
}

MSG_LIST_STABLED_PETS_Server MSG_LIST_STABLED_PETS_Server_read(Reader& reader) {
    MSG_LIST_STABLED_PETS_Server obj;

    obj.npc = reader.read_u64();

    obj.amount_of_pets = reader.read_u8();

    obj.stable_slots = reader.read_u8();

    for (auto i = 0; i < obj.amount_of_pets; ++i) {
        obj.pets.push_back(::wow_world_messages::vanilla::StabledPet_read(reader));
    }

    return obj;
}

std::vector<unsigned char> MSG_LIST_STABLED_PETS_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_LIST_STABLED_PETS_Server_size(obj));

    writer.write_u16_be(MSG_LIST_STABLED_PETS_Server_size(obj) + 2); /* size */

    writer.write_u16(0x0000026f); /* opcode */

    writer.write_u64(obj.npc);

    writer.write_u8(obj.amount_of_pets);

    writer.write_u8(obj.stable_slots);

    for (const auto& v : obj.pets) {
        StabledPet_write(writer, v);
    }

    return writer.m_buf;
}

CMSG_STABLE_PET CMSG_STABLE_PET_read(Reader& reader) {
    CMSG_STABLE_PET obj;

    obj.stable_master = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_STABLE_PET::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x00000270); /* opcode */

    writer.write_u64(obj.stable_master);

    return writer.m_buf;
}

CMSG_UNSTABLE_PET CMSG_UNSTABLE_PET_read(Reader& reader) {
    CMSG_UNSTABLE_PET obj;

    obj.stable_master = reader.read_u64();

    obj.pet_number = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_UNSTABLE_PET::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 4); /* size */

    writer.write_u32(0x00000271); /* opcode */

    writer.write_u64(obj.stable_master);

    writer.write_u32(obj.pet_number);

    return writer.m_buf;
}

CMSG_BUY_STABLE_SLOT CMSG_BUY_STABLE_SLOT_read(Reader& reader) {
    CMSG_BUY_STABLE_SLOT obj;

    obj.npc = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_BUY_STABLE_SLOT::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x00000272); /* opcode */

    writer.write_u64(obj.npc);

    return writer.m_buf;
}

SMSG_STABLE_RESULT SMSG_STABLE_RESULT_read(Reader& reader) {
    SMSG_STABLE_RESULT obj;

    obj.result = static_cast<StableResult>(reader.read_u8());

    return obj;
}

std::vector<unsigned char> SMSG_STABLE_RESULT::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0001);

    writer.write_u16_be(0x0001 + 2); /* size */

    writer.write_u16(0x00000273); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.result));

    return writer.m_buf;
}

CMSG_STABLE_SWAP_PET CMSG_STABLE_SWAP_PET_read(Reader& reader) {
    CMSG_STABLE_SWAP_PET obj;

    obj.npc = reader.read_u64();

    obj.pet_slot = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_STABLE_SWAP_PET::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 4); /* size */

    writer.write_u32(0x00000275); /* opcode */

    writer.write_u64(obj.npc);

    writer.write_u32(obj.pet_slot);

    return writer.m_buf;
}

SMSG_PLAY_MUSIC SMSG_PLAY_MUSIC_read(Reader& reader) {
    SMSG_PLAY_MUSIC obj;

    obj.sound_id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_PLAY_MUSIC::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x00000277); /* opcode */

    writer.write_u32(obj.sound_id);

    return writer.m_buf;
}

SMSG_PLAY_OBJECT_SOUND SMSG_PLAY_OBJECT_SOUND_read(Reader& reader) {
    SMSG_PLAY_OBJECT_SOUND obj;

    obj.sound_id = reader.read_u32();

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> SMSG_PLAY_OBJECT_SOUND::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 2); /* size */

    writer.write_u16(0x00000278); /* opcode */

    writer.write_u32(obj.sound_id);

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_REQUEST_PET_INFO::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x00000279); /* opcode */

    return writer.m_buf;
}

CMSG_FAR_SIGHT CMSG_FAR_SIGHT_read(Reader& reader) {
    CMSG_FAR_SIGHT obj;

    obj.operation = static_cast<FarSightOperation>(reader.read_u8());

    return obj;
}

std::vector<unsigned char> CMSG_FAR_SIGHT::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0001);

    writer.write_u16_be(0x0001 + 4); /* size */

    writer.write_u32(0x0000027a); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.operation));

    return writer.m_buf;
}

static size_t SMSG_SPELLDISPELLOG_size(const SMSG_SPELLDISPELLOG& obj) {
    return 4 + ::wow_world_messages::util::wwm_packed_guid_size(obj.victim) + ::wow_world_messages::util::wwm_packed_guid_size(obj.caster) + 4 * obj.spells.size();
}

SMSG_SPELLDISPELLOG SMSG_SPELLDISPELLOG_read(Reader& reader) {
    SMSG_SPELLDISPELLOG obj;

    obj.victim = reader.read_packed_guid();

    obj.caster = reader.read_packed_guid();

    obj.amount_of_spells = reader.read_u32();

    for (auto i = 0; i < obj.amount_of_spells; ++i) {
        obj.spells.push_back(reader.read_u32());
    }

    return obj;
}

std::vector<unsigned char> SMSG_SPELLDISPELLOG::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SPELLDISPELLOG_size(obj));

    writer.write_u16_be(SMSG_SPELLDISPELLOG_size(obj) + 2); /* size */

    writer.write_u16(0x0000027b); /* opcode */

    writer.write_packed_guid(obj.victim);

    writer.write_packed_guid(obj.caster);

    writer.write_u32(obj.amount_of_spells);

    for (const auto& v : obj.spells) {
        writer.write_u32(v);
    }

    return writer.m_buf;
}

static size_t CMSG_GROUP_CHANGE_SUB_GROUP_size(const CMSG_GROUP_CHANGE_SUB_GROUP& obj) {
    return 2 + obj.name.size();
}

CMSG_GROUP_CHANGE_SUB_GROUP CMSG_GROUP_CHANGE_SUB_GROUP_read(Reader& reader) {
    CMSG_GROUP_CHANGE_SUB_GROUP obj;

    obj.name = reader.read_cstring();

    obj.group_number = reader.read_u8();

    return obj;
}

std::vector<unsigned char> CMSG_GROUP_CHANGE_SUB_GROUP::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_GROUP_CHANGE_SUB_GROUP_size(obj));

    writer.write_u16_be((uint16_t)CMSG_GROUP_CHANGE_SUB_GROUP_size(obj) + 4); /* size */

    writer.write_u32(0x0000027e); /* opcode */

    writer.write_cstring(obj.name);

    writer.write_u8(obj.group_number);

    return writer.m_buf;
}

CMSG_REQUEST_PARTY_MEMBER_STATS CMSG_REQUEST_PARTY_MEMBER_STATS_read(Reader& reader) {
    CMSG_REQUEST_PARTY_MEMBER_STATS obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_REQUEST_PARTY_MEMBER_STATS::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x0000027f); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

static size_t CMSG_GROUP_SWAP_SUB_GROUP_size(const CMSG_GROUP_SWAP_SUB_GROUP& obj) {
    return 2 + obj.name.size() + obj.swap_with_name.size();
}

CMSG_GROUP_SWAP_SUB_GROUP CMSG_GROUP_SWAP_SUB_GROUP_read(Reader& reader) {
    CMSG_GROUP_SWAP_SUB_GROUP obj;

    obj.name = reader.read_cstring();

    obj.swap_with_name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_GROUP_SWAP_SUB_GROUP::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_GROUP_SWAP_SUB_GROUP_size(obj));

    writer.write_u16_be((uint16_t)CMSG_GROUP_SWAP_SUB_GROUP_size(obj) + 4); /* size */

    writer.write_u32(0x00000280); /* opcode */

    writer.write_cstring(obj.name);

    writer.write_cstring(obj.swap_with_name);

    return writer.m_buf;
}

CMSG_AUTOSTORE_BANK_ITEM CMSG_AUTOSTORE_BANK_ITEM_read(Reader& reader) {
    CMSG_AUTOSTORE_BANK_ITEM obj;

    obj.bag_index = reader.read_u8();

    obj.slot_index = reader.read_u8();

    return obj;
}

std::vector<unsigned char> CMSG_AUTOSTORE_BANK_ITEM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0002);

    writer.write_u16_be(0x0002 + 4); /* size */

    writer.write_u32(0x00000282); /* opcode */

    writer.write_u8(obj.bag_index);

    writer.write_u8(obj.slot_index);

    return writer.m_buf;
}

CMSG_AUTOBANK_ITEM CMSG_AUTOBANK_ITEM_read(Reader& reader) {
    CMSG_AUTOBANK_ITEM obj;

    obj.bag_index = reader.read_u8();

    obj.slot_index = reader.read_u8();

    return obj;
}

std::vector<unsigned char> CMSG_AUTOBANK_ITEM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0002);

    writer.write_u16_be(0x0002 + 4); /* size */

    writer.write_u32(0x00000283); /* opcode */

    writer.write_u8(obj.bag_index);

    writer.write_u8(obj.slot_index);

    return writer.m_buf;
}

MSG_QUERY_NEXT_MAIL_TIME_Server MSG_QUERY_NEXT_MAIL_TIME_Server_read(Reader& reader) {
    MSG_QUERY_NEXT_MAIL_TIME_Server obj;

    obj.unread_mails = reader.read_float();

    return obj;
}

std::vector<unsigned char> MSG_QUERY_NEXT_MAIL_TIME_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x00000284); /* opcode */

    writer.write_float(obj.unread_mails);

    return writer.m_buf;
}

std::vector<unsigned char> MSG_QUERY_NEXT_MAIL_TIME_Client::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x00000284); /* opcode */

    return writer.m_buf;
}

SMSG_RECEIVED_MAIL SMSG_RECEIVED_MAIL_read(Reader& reader) {
    SMSG_RECEIVED_MAIL obj;

    obj.unknown1 = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_RECEIVED_MAIL::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x00000285); /* opcode */

    writer.write_u32(obj.unknown1);

    return writer.m_buf;
}

SMSG_RAID_GROUP_ONLY SMSG_RAID_GROUP_ONLY_read(Reader& reader) {
    SMSG_RAID_GROUP_ONLY obj;

    obj.homebind_timer = reader.read_u32();

    obj.error = static_cast<RaidGroupError>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_RAID_GROUP_ONLY::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 2); /* size */

    writer.write_u16(0x00000286); /* opcode */

    writer.write_u32(obj.homebind_timer);

    writer.write_u32(static_cast<uint32_t>(obj.error));

    return writer.m_buf;
}

SMSG_PVP_CREDIT SMSG_PVP_CREDIT_read(Reader& reader) {
    SMSG_PVP_CREDIT obj;

    obj.honor_points = reader.read_u32();

    obj.victim = reader.read_u64();

    obj.rank = static_cast<PvpRank>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_PVP_CREDIT::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0010);

    writer.write_u16_be(0x0010 + 2); /* size */

    writer.write_u16(0x0000028c); /* opcode */

    writer.write_u32(obj.honor_points);

    writer.write_u64(obj.victim);

    writer.write_u32(static_cast<uint32_t>(obj.rank));

    return writer.m_buf;
}

SMSG_AUCTION_REMOVED_NOTIFICATION SMSG_AUCTION_REMOVED_NOTIFICATION_read(Reader& reader) {
    SMSG_AUCTION_REMOVED_NOTIFICATION obj;

    obj.item = reader.read_u32();

    obj.item_template = reader.read_u32();

    obj.random_property_id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_AUCTION_REMOVED_NOTIFICATION::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 2); /* size */

    writer.write_u16(0x0000028d); /* opcode */

    writer.write_u32(obj.item);

    writer.write_u32(obj.item_template);

    writer.write_u32(obj.random_property_id);

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_GROUP_RAID_CONVERT::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x0000028e); /* opcode */

    return writer.m_buf;
}

CMSG_GROUP_ASSISTANT_LEADER CMSG_GROUP_ASSISTANT_LEADER_read(Reader& reader) {
    CMSG_GROUP_ASSISTANT_LEADER obj;

    obj.guid = reader.read_u64();

    obj.set_assistant = reader.read_bool8();

    return obj;
}

std::vector<unsigned char> CMSG_GROUP_ASSISTANT_LEADER::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0009);

    writer.write_u16_be(0x0009 + 4); /* size */

    writer.write_u32(0x0000028f); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_bool8(obj.set_assistant);

    return writer.m_buf;
}

CMSG_BUYBACK_ITEM CMSG_BUYBACK_ITEM_read(Reader& reader) {
    CMSG_BUYBACK_ITEM obj;

    obj.guid = reader.read_u64();

    obj.slot = static_cast<BuybackSlot>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> CMSG_BUYBACK_ITEM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 4); /* size */

    writer.write_u32(0x00000290); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(static_cast<uint32_t>(obj.slot));

    return writer.m_buf;
}

static size_t SMSG_SERVER_MESSAGE_size(const SMSG_SERVER_MESSAGE& obj) {
    return 5 + obj.message.size();
}

SMSG_SERVER_MESSAGE SMSG_SERVER_MESSAGE_read(Reader& reader) {
    SMSG_SERVER_MESSAGE obj;

    obj.message_type = static_cast<ServerMessageType>(reader.read_u32());

    obj.message = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> SMSG_SERVER_MESSAGE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SERVER_MESSAGE_size(obj));

    writer.write_u16_be(SMSG_SERVER_MESSAGE_size(obj) + 2); /* size */

    writer.write_u16(0x00000291); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.message_type));

    writer.write_cstring(obj.message);

    return writer.m_buf;
}

CMSG_MEETINGSTONE_JOIN CMSG_MEETINGSTONE_JOIN_read(Reader& reader) {
    CMSG_MEETINGSTONE_JOIN obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_MEETINGSTONE_JOIN::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x00000292); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_MEETINGSTONE_LEAVE::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x00000293); /* opcode */

    return writer.m_buf;
}

SMSG_MEETINGSTONE_SETQUEUE SMSG_MEETINGSTONE_SETQUEUE_read(Reader& reader) {
    SMSG_MEETINGSTONE_SETQUEUE obj;

    obj.area = static_cast<Area>(reader.read_u32());

    obj.status = static_cast<MeetingStoneStatus>(reader.read_u8());

    return obj;
}

std::vector<unsigned char> SMSG_MEETINGSTONE_SETQUEUE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0005);

    writer.write_u16_be(0x0005 + 2); /* size */

    writer.write_u16(0x00000295); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.area));

    writer.write_u8(static_cast<uint8_t>(obj.status));

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_MEETINGSTONE_INFO::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x00000296); /* opcode */

    return writer.m_buf;
}

std::vector<unsigned char> SMSG_MEETINGSTONE_COMPLETE::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 2); /* size */

    writer.write_u16(0x00000297); /* opcode */

    return writer.m_buf;
}

std::vector<unsigned char> SMSG_MEETINGSTONE_IN_PROGRESS::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 2); /* size */

    writer.write_u16(0x00000298); /* opcode */

    return writer.m_buf;
}

SMSG_MEETINGSTONE_MEMBER_ADDED SMSG_MEETINGSTONE_MEMBER_ADDED_read(Reader& reader) {
    SMSG_MEETINGSTONE_MEMBER_ADDED obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> SMSG_MEETINGSTONE_MEMBER_ADDED::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 2); /* size */

    writer.write_u16(0x00000299); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_CANCEL_GROWTH_AURA::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x0000029b); /* opcode */

    return writer.m_buf;
}

std::vector<unsigned char> SMSG_CANCEL_AUTO_REPEAT::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 2); /* size */

    writer.write_u16(0x0000029c); /* opcode */

    return writer.m_buf;
}

SMSG_STANDSTATE_UPDATE SMSG_STANDSTATE_UPDATE_read(Reader& reader) {
    SMSG_STANDSTATE_UPDATE obj;

    obj.state = static_cast<UnitStandState>(reader.read_u8());

    return obj;
}

std::vector<unsigned char> SMSG_STANDSTATE_UPDATE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0001);

    writer.write_u16_be(0x0001 + 2); /* size */

    writer.write_u16(0x0000029d); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.state));

    return writer.m_buf;
}

SMSG_LOOT_ALL_PASSED SMSG_LOOT_ALL_PASSED_read(Reader& reader) {
    SMSG_LOOT_ALL_PASSED obj;

    obj.looted_target = reader.read_u64();

    obj.loot_slot = reader.read_u32();

    obj.item = reader.read_u32();

    obj.item_random_property_id = reader.read_u32();

    obj.item_random_suffix_id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_LOOT_ALL_PASSED::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0018);

    writer.write_u16_be(0x0018 + 2); /* size */

    writer.write_u16(0x0000029e); /* opcode */

    writer.write_u64(obj.looted_target);

    writer.write_u32(obj.loot_slot);

    writer.write_u32(obj.item);

    writer.write_u32(obj.item_random_property_id);

    writer.write_u32(obj.item_random_suffix_id);

    return writer.m_buf;
}

SMSG_LOOT_ROLL_WON SMSG_LOOT_ROLL_WON_read(Reader& reader) {
    SMSG_LOOT_ROLL_WON obj;

    obj.looted_target = reader.read_u64();

    obj.loot_slot = reader.read_u32();

    obj.item = reader.read_u32();

    obj.item_random_suffix = reader.read_u32();

    obj.item_random_property_id = reader.read_u32();

    obj.winning_player = reader.read_u64();

    obj.winning_roll = reader.read_u8();

    obj.vote = static_cast<RollVote>(reader.read_u8());

    return obj;
}

std::vector<unsigned char> SMSG_LOOT_ROLL_WON::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0022);

    writer.write_u16_be(0x0022 + 2); /* size */

    writer.write_u16(0x0000029f); /* opcode */

    writer.write_u64(obj.looted_target);

    writer.write_u32(obj.loot_slot);

    writer.write_u32(obj.item);

    writer.write_u32(obj.item_random_suffix);

    writer.write_u32(obj.item_random_property_id);

    writer.write_u64(obj.winning_player);

    writer.write_u8(obj.winning_roll);

    writer.write_u8(static_cast<uint8_t>(obj.vote));

    return writer.m_buf;
}

CMSG_LOOT_ROLL CMSG_LOOT_ROLL_read(Reader& reader) {
    CMSG_LOOT_ROLL obj;

    obj.item = reader.read_u64();

    obj.item_slot = reader.read_u32();

    obj.vote = static_cast<RollVote>(reader.read_u8());

    return obj;
}

std::vector<unsigned char> CMSG_LOOT_ROLL::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000d);

    writer.write_u16_be(0x000d + 4); /* size */

    writer.write_u32(0x000002a0); /* opcode */

    writer.write_u64(obj.item);

    writer.write_u32(obj.item_slot);

    writer.write_u8(static_cast<uint8_t>(obj.vote));

    return writer.m_buf;
}

SMSG_LOOT_START_ROLL SMSG_LOOT_START_ROLL_read(Reader& reader) {
    SMSG_LOOT_START_ROLL obj;

    obj.creature = reader.read_u64();

    obj.loot_slot = reader.read_u32();

    obj.item = reader.read_u32();

    obj.item_random_suffix = reader.read_u32();

    obj.item_random_property_id = reader.read_u32();

    obj.countdown_time = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_LOOT_START_ROLL::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x001c);

    writer.write_u16_be(0x001c + 2); /* size */

    writer.write_u16(0x000002a1); /* opcode */

    writer.write_u64(obj.creature);

    writer.write_u32(obj.loot_slot);

    writer.write_u32(obj.item);

    writer.write_u32(obj.item_random_suffix);

    writer.write_u32(obj.item_random_property_id);

    writer.write_u32(obj.countdown_time);

    return writer.m_buf;
}

SMSG_LOOT_ROLL SMSG_LOOT_ROLL_read(Reader& reader) {
    SMSG_LOOT_ROLL obj;

    obj.creature = reader.read_u64();

    obj.loot_slot = reader.read_u32();

    obj.player = reader.read_u64();

    obj.item = reader.read_u32();

    obj.item_random_suffix = reader.read_u32();

    obj.item_random_property_id = reader.read_u32();

    obj.roll_number = reader.read_u8();

    obj.vote = static_cast<RollVote>(reader.read_u8());

    return obj;
}

std::vector<unsigned char> SMSG_LOOT_ROLL::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0022);

    writer.write_u16_be(0x0022 + 2); /* size */

    writer.write_u16(0x000002a2); /* opcode */

    writer.write_u64(obj.creature);

    writer.write_u32(obj.loot_slot);

    writer.write_u64(obj.player);

    writer.write_u32(obj.item);

    writer.write_u32(obj.item_random_suffix);

    writer.write_u32(obj.item_random_property_id);

    writer.write_u8(obj.roll_number);

    writer.write_u8(static_cast<uint8_t>(obj.vote));

    return writer.m_buf;
}

CMSG_LOOT_MASTER_GIVE CMSG_LOOT_MASTER_GIVE_read(Reader& reader) {
    CMSG_LOOT_MASTER_GIVE obj;

    obj.loot = reader.read_u64();

    obj.slot_id = reader.read_u8();

    obj.player = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_LOOT_MASTER_GIVE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0011);

    writer.write_u16_be(0x0011 + 4); /* size */

    writer.write_u32(0x000002a3); /* opcode */

    writer.write_u64(obj.loot);

    writer.write_u8(obj.slot_id);

    writer.write_u64(obj.player);

    return writer.m_buf;
}

static size_t SMSG_LOOT_MASTER_LIST_size(const SMSG_LOOT_MASTER_LIST& obj) {
    return 1 + 8 * obj.guids.size();
}

SMSG_LOOT_MASTER_LIST SMSG_LOOT_MASTER_LIST_read(Reader& reader) {
    SMSG_LOOT_MASTER_LIST obj;

    obj.amount_of_players = reader.read_u8();

    for (auto i = 0; i < obj.amount_of_players; ++i) {
        obj.guids.push_back(reader.read_u64());
    }

    return obj;
}

std::vector<unsigned char> SMSG_LOOT_MASTER_LIST::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_LOOT_MASTER_LIST_size(obj));

    writer.write_u16_be(SMSG_LOOT_MASTER_LIST_size(obj) + 2); /* size */

    writer.write_u16(0x000002a4); /* opcode */

    writer.write_u8(obj.amount_of_players);

    for (const auto& v : obj.guids) {
        writer.write_u64(v);
    }

    return writer.m_buf;
}

static size_t SMSG_SET_FORCED_REACTIONS_size(const SMSG_SET_FORCED_REACTIONS& obj) {
    return 4 + 6 * obj.reactions.size();
}

SMSG_SET_FORCED_REACTIONS SMSG_SET_FORCED_REACTIONS_read(Reader& reader) {
    SMSG_SET_FORCED_REACTIONS obj;

    obj.amount_of_reactions = reader.read_u32();

    for (auto i = 0; i < obj.amount_of_reactions; ++i) {
        obj.reactions.push_back(::wow_world_messages::vanilla::ForcedReaction_read(reader));
    }

    return obj;
}

std::vector<unsigned char> SMSG_SET_FORCED_REACTIONS::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SET_FORCED_REACTIONS_size(obj));

    writer.write_u16_be(SMSG_SET_FORCED_REACTIONS_size(obj) + 2); /* size */

    writer.write_u16(0x000002a5); /* opcode */

    writer.write_u32(obj.amount_of_reactions);

    for (const auto& v : obj.reactions) {
        ForcedReaction_write(writer, v);
    }

    return writer.m_buf;
}

SMSG_SPELL_FAILED_OTHER SMSG_SPELL_FAILED_OTHER_read(Reader& reader) {
    SMSG_SPELL_FAILED_OTHER obj;

    obj.caster = reader.read_u64();

    obj.id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_SPELL_FAILED_OTHER::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 2); /* size */

    writer.write_u16(0x000002a6); /* opcode */

    writer.write_u64(obj.caster);

    writer.write_u32(obj.id);

    return writer.m_buf;
}

SMSG_GAMEOBJECT_RESET_STATE SMSG_GAMEOBJECT_RESET_STATE_read(Reader& reader) {
    SMSG_GAMEOBJECT_RESET_STATE obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> SMSG_GAMEOBJECT_RESET_STATE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 2); /* size */

    writer.write_u16(0x000002a7); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

CMSG_REPAIR_ITEM CMSG_REPAIR_ITEM_read(Reader& reader) {
    CMSG_REPAIR_ITEM obj;

    obj.npc = reader.read_u64();

    obj.item = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_REPAIR_ITEM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0010);

    writer.write_u16_be(0x0010 + 4); /* size */

    writer.write_u32(0x000002a8); /* opcode */

    writer.write_u64(obj.npc);

    writer.write_u64(obj.item);

    return writer.m_buf;
}

static size_t SMSG_CHAT_PLAYER_NOT_FOUND_size(const SMSG_CHAT_PLAYER_NOT_FOUND& obj) {
    return 1 + obj.name.size();
}

SMSG_CHAT_PLAYER_NOT_FOUND SMSG_CHAT_PLAYER_NOT_FOUND_read(Reader& reader) {
    SMSG_CHAT_PLAYER_NOT_FOUND obj;

    obj.name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> SMSG_CHAT_PLAYER_NOT_FOUND::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_CHAT_PLAYER_NOT_FOUND_size(obj));

    writer.write_u16_be(SMSG_CHAT_PLAYER_NOT_FOUND_size(obj) + 2); /* size */

    writer.write_u16(0x000002a9); /* opcode */

    writer.write_cstring(obj.name);

    return writer.m_buf;
}

MSG_TALENT_WIPE_CONFIRM_Client MSG_TALENT_WIPE_CONFIRM_Client_read(Reader& reader) {
    MSG_TALENT_WIPE_CONFIRM_Client obj;

    obj.wiping_npc = reader.read_u64();

    return obj;
}

std::vector<unsigned char> MSG_TALENT_WIPE_CONFIRM_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x000002aa); /* opcode */

    writer.write_u64(obj.wiping_npc);

    return writer.m_buf;
}

MSG_TALENT_WIPE_CONFIRM_Server MSG_TALENT_WIPE_CONFIRM_Server_read(Reader& reader) {
    MSG_TALENT_WIPE_CONFIRM_Server obj;

    obj.wiping_npc = reader.read_u64();

    obj.cost_in_copper = reader.read_u32();

    return obj;
}

std::vector<unsigned char> MSG_TALENT_WIPE_CONFIRM_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 2); /* size */

    writer.write_u16(0x000002aa); /* opcode */

    writer.write_u64(obj.wiping_npc);

    writer.write_u32(obj.cost_in_copper);

    return writer.m_buf;
}

SMSG_SUMMON_REQUEST SMSG_SUMMON_REQUEST_read(Reader& reader) {
    SMSG_SUMMON_REQUEST obj;

    obj.summoner = reader.read_u64();

    obj.area = static_cast<Area>(reader.read_u32());

    obj.auto_decline_time = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_SUMMON_REQUEST::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0010);

    writer.write_u16_be(0x0010 + 2); /* size */

    writer.write_u16(0x000002ab); /* opcode */

    writer.write_u64(obj.summoner);

    writer.write_u32(static_cast<uint32_t>(obj.area));

    writer.write_u32(obj.auto_decline_time);

    return writer.m_buf;
}

CMSG_SUMMON_RESPONSE CMSG_SUMMON_RESPONSE_read(Reader& reader) {
    CMSG_SUMMON_RESPONSE obj;

    obj.summoner = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_SUMMON_RESPONSE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x000002ac); /* opcode */

    writer.write_u64(obj.summoner);

    return writer.m_buf;
}

static size_t SMSG_MONSTER_MOVE_TRANSPORT_size(const SMSG_MONSTER_MOVE_TRANSPORT& obj) {
    size_t _size = 25 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid) + ::wow_world_messages::util::wwm_packed_guid_size(obj.transport) + ::wow_world_messages::util::wwm_monster_move_spline_size(obj.splines);

    if (obj.move_type == MonsterMoveType::FACING_TARGET) {
        _size += 8;
    }
    else if (obj.move_type == MonsterMoveType::FACING_ANGLE) {
        _size += 4;
    }
    else if (obj.move_type == MonsterMoveType::FACING_SPOT) {
        _size += 12;
    }

    return _size;
}

SMSG_MONSTER_MOVE_TRANSPORT SMSG_MONSTER_MOVE_TRANSPORT_read(Reader& reader) {
    SMSG_MONSTER_MOVE_TRANSPORT obj;

    obj.guid = reader.read_packed_guid();

    obj.transport = reader.read_packed_guid();

    obj.spline_point = ::wow_world_messages::all::Vector3d_read(reader);

    obj.spline_id = reader.read_u32();

    obj.move_type = static_cast<MonsterMoveType>(reader.read_u8());

    if (obj.move_type == MonsterMoveType::FACING_TARGET) {
        obj.target = reader.read_u64();

    }
    else if (obj.move_type == MonsterMoveType::FACING_ANGLE) {
        obj.angle = reader.read_float();

    }
    else if (obj.move_type == MonsterMoveType::FACING_SPOT) {
        obj.position = ::wow_world_messages::all::Vector3d_read(reader);

    }
    obj.spline_flags = static_cast<SplineFlag>(reader.read_u32());

    obj.duration = reader.read_u32();

    obj.splines = ::wow_world_messages::util::wwm_read_monster_move_spline(reader);

    return obj;
}

std::vector<unsigned char> SMSG_MONSTER_MOVE_TRANSPORT::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_MONSTER_MOVE_TRANSPORT_size(obj));

    writer.write_u16_be(SMSG_MONSTER_MOVE_TRANSPORT_size(obj) + 2); /* size */

    writer.write_u16(0x000002ae); /* opcode */

    writer.write_packed_guid(obj.guid);

    writer.write_packed_guid(obj.transport);

    Vector3d_write(writer, obj.spline_point);

    writer.write_u32(obj.spline_id);

    writer.write_u8(static_cast<uint8_t>(obj.move_type));

    if (obj.move_type == MonsterMoveType::FACING_TARGET) {
        writer.write_u64(obj.target);

    }
    else if (obj.move_type == MonsterMoveType::FACING_ANGLE) {
        writer.write_float(obj.angle);

    }
    else if (obj.move_type == MonsterMoveType::FACING_SPOT) {
        Vector3d_write(writer, obj.position);

    }
    writer.write_u32(static_cast<uint32_t>(obj.spline_flags));

    writer.write_u32(obj.duration);

    ::wow_world_messages::util::wwm_write_monster_move_spline(writer, obj.splines);

    return writer.m_buf;
}

std::vector<unsigned char> SMSG_PET_BROKEN::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 2); /* size */

    writer.write_u16(0x000002af); /* opcode */

    return writer.m_buf;
}

static size_t MSG_MOVE_FEATHER_FALL_Server_size(const MSG_MOVE_FEATHER_FALL_Server& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.player) + MovementInfo_size(obj.info);
}

MSG_MOVE_FEATHER_FALL_Server MSG_MOVE_FEATHER_FALL_Server_read(Reader& reader) {
    MSG_MOVE_FEATHER_FALL_Server obj;

    obj.player = reader.read_packed_guid();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> MSG_MOVE_FEATHER_FALL_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_FEATHER_FALL_Server_size(obj));

    writer.write_u16_be(MSG_MOVE_FEATHER_FALL_Server_size(obj) + 2); /* size */

    writer.write_u16(0x000002b0); /* opcode */

    writer.write_packed_guid(obj.player);

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_SELF_RES::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x000002b3); /* opcode */

    return writer.m_buf;
}

std::vector<unsigned char> SMSG_FEIGN_DEATH_RESISTED::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 2); /* size */

    writer.write_u16(0x000002b4); /* opcode */

    return writer.m_buf;
}

SMSG_DUEL_COUNTDOWN SMSG_DUEL_COUNTDOWN_read(Reader& reader) {
    SMSG_DUEL_COUNTDOWN obj;

    obj.time = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_DUEL_COUNTDOWN::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x000002b7); /* opcode */

    writer.write_u32(obj.time);

    return writer.m_buf;
}

static size_t SMSG_AREA_TRIGGER_MESSAGE_size(const SMSG_AREA_TRIGGER_MESSAGE& obj) {
    return 5 + obj.message.size();
}

SMSG_AREA_TRIGGER_MESSAGE SMSG_AREA_TRIGGER_MESSAGE_read(Reader& reader) {
    SMSG_AREA_TRIGGER_MESSAGE obj;

    obj.message = reader.read_sized_cstring();

    return obj;
}

std::vector<unsigned char> SMSG_AREA_TRIGGER_MESSAGE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_AREA_TRIGGER_MESSAGE_size(obj));

    writer.write_u16_be(SMSG_AREA_TRIGGER_MESSAGE_size(obj) + 2); /* size */

    writer.write_u16(0x000002b8); /* opcode */

    writer.write_sized_cstring(obj.message);

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_TOGGLE_HELM::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x000002b9); /* opcode */

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_TOGGLE_CLOAK::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x000002ba); /* opcode */

    return writer.m_buf;
}

SMSG_MEETINGSTONE_JOINFAILED SMSG_MEETINGSTONE_JOINFAILED_read(Reader& reader) {
    SMSG_MEETINGSTONE_JOINFAILED obj;

    obj.reason = static_cast<MeetingStoneFailure>(reader.read_u8());

    return obj;
}

std::vector<unsigned char> SMSG_MEETINGSTONE_JOINFAILED::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0001);

    writer.write_u16_be(0x0001 + 2); /* size */

    writer.write_u16(0x000002bb); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.reason));

    return writer.m_buf;
}

SMSG_PLAYER_SKINNED SMSG_PLAYER_SKINNED_read(Reader& reader) {
    SMSG_PLAYER_SKINNED obj;

    obj.spirit_released = reader.read_bool8();

    return obj;
}

std::vector<unsigned char> SMSG_PLAYER_SKINNED::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0001);

    writer.write_u16_be(0x0001 + 2); /* size */

    writer.write_u16(0x000002bc); /* opcode */

    writer.write_bool8(obj.spirit_released);

    return writer.m_buf;
}

std::vector<unsigned char> SMSG_DURABILITY_DAMAGE_DEATH::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 2); /* size */

    writer.write_u16(0x000002bd); /* opcode */

    return writer.m_buf;
}

CMSG_SET_ACTIONBAR_TOGGLES CMSG_SET_ACTIONBAR_TOGGLES_read(Reader& reader) {
    CMSG_SET_ACTIONBAR_TOGGLES obj;

    obj.action_bar = reader.read_u8();

    return obj;
}

std::vector<unsigned char> CMSG_SET_ACTIONBAR_TOGGLES::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0001);

    writer.write_u16_be(0x0001 + 4); /* size */

    writer.write_u32(0x000002bf); /* opcode */

    writer.write_u8(obj.action_bar);

    return writer.m_buf;
}

static size_t SMSG_INIT_WORLD_STATES_size(const SMSG_INIT_WORLD_STATES& obj) {
    return 10 + 8 * obj.states.size();
}

SMSG_INIT_WORLD_STATES SMSG_INIT_WORLD_STATES_read(Reader& reader) {
    SMSG_INIT_WORLD_STATES obj;

    obj.map = static_cast<Map>(reader.read_u32());

    obj.area = static_cast<Area>(reader.read_u32());

    obj.amount_of_states = reader.read_u16();

    for (auto i = 0; i < obj.amount_of_states; ++i) {
        obj.states.push_back(::wow_world_messages::vanilla::WorldState_read(reader));
    }

    return obj;
}

std::vector<unsigned char> SMSG_INIT_WORLD_STATES::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_INIT_WORLD_STATES_size(obj));

    writer.write_u16_be(SMSG_INIT_WORLD_STATES_size(obj) + 2); /* size */

    writer.write_u16(0x000002c2); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.map));

    writer.write_u32(static_cast<uint32_t>(obj.area));

    writer.write_u16(obj.amount_of_states);

    for (const auto& v : obj.states) {
        WorldState_write(writer, v);
    }

    return writer.m_buf;
}

SMSG_UPDATE_WORLD_STATE SMSG_UPDATE_WORLD_STATE_read(Reader& reader) {
    SMSG_UPDATE_WORLD_STATE obj;

    obj.state = ::wow_world_messages::vanilla::WorldState_read(reader);

    return obj;
}

std::vector<unsigned char> SMSG_UPDATE_WORLD_STATE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 2); /* size */

    writer.write_u16(0x000002c3); /* opcode */

    WorldState_write(writer, obj.state);

    return writer.m_buf;
}

CMSG_ITEM_NAME_QUERY CMSG_ITEM_NAME_QUERY_read(Reader& reader) {
    CMSG_ITEM_NAME_QUERY obj;

    obj.item = reader.read_u32();

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_ITEM_NAME_QUERY::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 4); /* size */

    writer.write_u32(0x000002c4); /* opcode */

    writer.write_u32(obj.item);

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

static size_t SMSG_ITEM_NAME_QUERY_RESPONSE_size(const SMSG_ITEM_NAME_QUERY_RESPONSE& obj) {
    return 5 + obj.item_name.size();
}

SMSG_ITEM_NAME_QUERY_RESPONSE SMSG_ITEM_NAME_QUERY_RESPONSE_read(Reader& reader) {
    SMSG_ITEM_NAME_QUERY_RESPONSE obj;

    obj.item = reader.read_u32();

    obj.item_name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> SMSG_ITEM_NAME_QUERY_RESPONSE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_ITEM_NAME_QUERY_RESPONSE_size(obj));

    writer.write_u16_be(SMSG_ITEM_NAME_QUERY_RESPONSE_size(obj) + 2); /* size */

    writer.write_u16(0x000002c5); /* opcode */

    writer.write_u32(obj.item);

    writer.write_cstring(obj.item_name);

    return writer.m_buf;
}

SMSG_PET_ACTION_FEEDBACK SMSG_PET_ACTION_FEEDBACK_read(Reader& reader) {
    SMSG_PET_ACTION_FEEDBACK obj;

    obj.feedback = static_cast<PetFeedback>(reader.read_u8());

    return obj;
}

std::vector<unsigned char> SMSG_PET_ACTION_FEEDBACK::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0001);

    writer.write_u16_be(0x0001 + 2); /* size */

    writer.write_u16(0x000002c6); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.feedback));

    return writer.m_buf;
}

static size_t CMSG_CHAR_RENAME_size(const CMSG_CHAR_RENAME& obj) {
    return 9 + obj.new_name.size();
}

CMSG_CHAR_RENAME CMSG_CHAR_RENAME_read(Reader& reader) {
    CMSG_CHAR_RENAME obj;

    obj.character = reader.read_u64();

    obj.new_name = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_CHAR_RENAME::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_CHAR_RENAME_size(obj));

    writer.write_u16_be((uint16_t)CMSG_CHAR_RENAME_size(obj) + 4); /* size */

    writer.write_u32(0x000002c7); /* opcode */

    writer.write_u64(obj.character);

    writer.write_cstring(obj.new_name);

    return writer.m_buf;
}

static size_t SMSG_CHAR_RENAME_size(const SMSG_CHAR_RENAME& obj) {
    size_t _size = 1;

    if (obj.result == WorldResult::RESPONSE_SUCCESS) {
        _size += 9 + obj.new_name.size();
    }

    return _size;
}

SMSG_CHAR_RENAME SMSG_CHAR_RENAME_read(Reader& reader) {
    SMSG_CHAR_RENAME obj;

    obj.result = static_cast<WorldResult>(reader.read_u8());

    if (obj.result == WorldResult::RESPONSE_SUCCESS) {
        obj.character = reader.read_u64();

        obj.new_name = reader.read_cstring();

    }
    return obj;
}

std::vector<unsigned char> SMSG_CHAR_RENAME::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_CHAR_RENAME_size(obj));

    writer.write_u16_be(SMSG_CHAR_RENAME_size(obj) + 2); /* size */

    writer.write_u16(0x000002c8); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.result));

    if (obj.result == WorldResult::RESPONSE_SUCCESS) {
        writer.write_u64(obj.character);

        writer.write_cstring(obj.new_name);

    }
    return writer.m_buf;
}

static size_t CMSG_MOVE_SPLINE_DONE_size(const CMSG_MOVE_SPLINE_DONE& obj) {
    return 8 + MovementInfo_size(obj.info);
}

CMSG_MOVE_SPLINE_DONE CMSG_MOVE_SPLINE_DONE_read(Reader& reader) {
    CMSG_MOVE_SPLINE_DONE obj;

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    obj.movement_counter = reader.read_u32();

    obj.unknown1 = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_MOVE_SPLINE_DONE::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_MOVE_SPLINE_DONE_size(obj));

    writer.write_u16_be((uint16_t)CMSG_MOVE_SPLINE_DONE_size(obj) + 4); /* size */

    writer.write_u32(0x000002c9); /* opcode */

    MovementInfo_write(writer, obj.info);

    writer.write_u32(obj.movement_counter);

    writer.write_u32(obj.unknown1);

    return writer.m_buf;
}

static size_t CMSG_MOVE_FALL_RESET_size(const CMSG_MOVE_FALL_RESET& obj) {
    return 0 + MovementInfo_size(obj.info);
}

CMSG_MOVE_FALL_RESET CMSG_MOVE_FALL_RESET_read(Reader& reader) {
    CMSG_MOVE_FALL_RESET obj;

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> CMSG_MOVE_FALL_RESET::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_MOVE_FALL_RESET_size(obj));

    writer.write_u16_be((uint16_t)CMSG_MOVE_FALL_RESET_size(obj) + 4); /* size */

    writer.write_u32(0x000002ca); /* opcode */

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

SMSG_INSTANCE_SAVE_CREATED SMSG_INSTANCE_SAVE_CREATED_read(Reader& reader) {
    SMSG_INSTANCE_SAVE_CREATED obj;

    obj.unknown = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_INSTANCE_SAVE_CREATED::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x000002cb); /* opcode */

    writer.write_u32(obj.unknown);

    return writer.m_buf;
}

static size_t SMSG_RAID_INSTANCE_INFO_size(const SMSG_RAID_INSTANCE_INFO& obj) {
    return 4 + 12 * obj.raid_infos.size();
}

SMSG_RAID_INSTANCE_INFO SMSG_RAID_INSTANCE_INFO_read(Reader& reader) {
    SMSG_RAID_INSTANCE_INFO obj;

    obj.amount_of_raid_infos = reader.read_u32();

    for (auto i = 0; i < obj.amount_of_raid_infos; ++i) {
        obj.raid_infos.push_back(::wow_world_messages::vanilla::RaidInfo_read(reader));
    }

    return obj;
}

std::vector<unsigned char> SMSG_RAID_INSTANCE_INFO::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_RAID_INSTANCE_INFO_size(obj));

    writer.write_u16_be(SMSG_RAID_INSTANCE_INFO_size(obj) + 2); /* size */

    writer.write_u16(0x000002cc); /* opcode */

    writer.write_u32(obj.amount_of_raid_infos);

    for (const auto& v : obj.raid_infos) {
        RaidInfo_write(writer, v);
    }

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_REQUEST_RAID_INFO::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x000002cd); /* opcode */

    return writer.m_buf;
}

CMSG_MOVE_TIME_SKIPPED CMSG_MOVE_TIME_SKIPPED_read(Reader& reader) {
    CMSG_MOVE_TIME_SKIPPED obj;

    obj.guid = reader.read_u64();

    obj.lag = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_MOVE_TIME_SKIPPED::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 4); /* size */

    writer.write_u32(0x000002ce); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.lag);

    return writer.m_buf;
}

static size_t CMSG_MOVE_FEATHER_FALL_ACK_size(const CMSG_MOVE_FEATHER_FALL_ACK& obj) {
    return 16 + MovementInfo_size(obj.info);
}

CMSG_MOVE_FEATHER_FALL_ACK CMSG_MOVE_FEATHER_FALL_ACK_read(Reader& reader) {
    CMSG_MOVE_FEATHER_FALL_ACK obj;

    obj.guid = reader.read_u64();

    obj.movement_counter = reader.read_u32();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    obj.apply = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_MOVE_FEATHER_FALL_ACK::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_MOVE_FEATHER_FALL_ACK_size(obj));

    writer.write_u16_be((uint16_t)CMSG_MOVE_FEATHER_FALL_ACK_size(obj) + 4); /* size */

    writer.write_u32(0x000002cf); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.movement_counter);

    MovementInfo_write(writer, obj.info);

    writer.write_u32(obj.apply);

    return writer.m_buf;
}

static size_t CMSG_MOVE_WATER_WALK_ACK_size(const CMSG_MOVE_WATER_WALK_ACK& obj) {
    return 16 + MovementInfo_size(obj.info);
}

CMSG_MOVE_WATER_WALK_ACK CMSG_MOVE_WATER_WALK_ACK_read(Reader& reader) {
    CMSG_MOVE_WATER_WALK_ACK obj;

    obj.guid = reader.read_u64();

    obj.movement_counter = reader.read_u32();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    obj.apply = reader.read_u32();

    return obj;
}

std::vector<unsigned char> CMSG_MOVE_WATER_WALK_ACK::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_MOVE_WATER_WALK_ACK_size(obj));

    writer.write_u16_be((uint16_t)CMSG_MOVE_WATER_WALK_ACK_size(obj) + 4); /* size */

    writer.write_u32(0x000002d0); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.movement_counter);

    MovementInfo_write(writer, obj.info);

    writer.write_u32(obj.apply);

    return writer.m_buf;
}

static size_t CMSG_MOVE_NOT_ACTIVE_MOVER_size(const CMSG_MOVE_NOT_ACTIVE_MOVER& obj) {
    return 8 + MovementInfo_size(obj.info);
}

CMSG_MOVE_NOT_ACTIVE_MOVER CMSG_MOVE_NOT_ACTIVE_MOVER_read(Reader& reader) {
    CMSG_MOVE_NOT_ACTIVE_MOVER obj;

    obj.old_mover = reader.read_u64();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    return obj;
}

std::vector<unsigned char> CMSG_MOVE_NOT_ACTIVE_MOVER::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_MOVE_NOT_ACTIVE_MOVER_size(obj));

    writer.write_u16_be((uint16_t)CMSG_MOVE_NOT_ACTIVE_MOVER_size(obj) + 4); /* size */

    writer.write_u32(0x000002d1); /* opcode */

    writer.write_u64(obj.old_mover);

    MovementInfo_write(writer, obj.info);

    return writer.m_buf;
}

SMSG_PLAY_SOUND SMSG_PLAY_SOUND_read(Reader& reader) {
    SMSG_PLAY_SOUND obj;

    obj.sound_id = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_PLAY_SOUND::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x000002d2); /* opcode */

    writer.write_u32(obj.sound_id);

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_BATTLEFIELD_STATUS::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x000002d3); /* opcode */

    return writer.m_buf;
}

static size_t SMSG_BATTLEFIELD_STATUS_size(const SMSG_BATTLEFIELD_STATUS& obj) {
    size_t _size = 8;

    if (obj.map == Map::KALIMDOR|| obj.map == Map::TESTING|| obj.map == Map::SCOTT_TEST|| obj.map == Map::CASH_TEST|| obj.map == Map::ALTERAC_VALLEY|| obj.map == Map::SHADOWFANG_KEEP|| obj.map == Map::STORMWIND_STOCKADE|| obj.map == Map::STORMWIND_PRISON|| obj.map == Map::DEADMINES|| obj.map == Map::AZSHARA_CRATER|| obj.map == Map::COLLINS_TEST|| obj.map == Map::WAILING_CAVERNS|| obj.map == Map::MONASTERY_UNUSED|| obj.map == Map::RAZORFEN_KRAUL|| obj.map == Map::BLACKFATHOM_DEEPS|| obj.map == Map::ULDAMAN|| obj.map == Map::GNOMEREGAN|| obj.map == Map::SUNKEN_TEMPLE|| obj.map == Map::RAZORFEN_DOWNS|| obj.map == Map::EMERALD_DREAM|| obj.map == Map::SCARLET_MONASTERY|| obj.map == Map::ZUL_FARRAK|| obj.map == Map::BLACKROCK_SPIRE|| obj.map == Map::BLACKROCK_DEPTHS|| obj.map == Map::ONYXIAS_LAIR|| obj.map == Map::OPENING_OF_THE_DARK_PORTAL|| obj.map == Map::SCHOLOMANCE|| obj.map == Map::ZUL_GURUB|| obj.map == Map::STRATHOLME|| obj.map == Map::MARAUDON|| obj.map == Map::DEEPRUN_TRAM|| obj.map == Map::RAGEFIRE_CHASM|| obj.map == Map::MOLTEN_CORE|| obj.map == Map::DIRE_MAUL|| obj.map == Map::ALLIANCE_PVP_BARRACKS|| obj.map == Map::HORDE_PVP_BARRACKS|| obj.map == Map::DEVELOPMENT_LAND|| obj.map == Map::BLACKWING_LAIR|| obj.map == Map::WARSONG_GULCH|| obj.map == Map::RUINS_OF_AHN_QIRAJ|| obj.map == Map::ARATHI_BASIN|| obj.map == Map::AHN_QIRAJ_TEMPLE|| obj.map == Map::NAXXRAMAS) {
        _size += 6;

        if (obj.status_id == StatusId::WAIT_QUEUE) {
            _size += 8;
        }
        else if (obj.status_id == StatusId::WAIT_JOIN) {
            _size += 4;
        }
        else if (obj.status_id == StatusId::IN_PROGRESS) {
            _size += 8;
        }

    }

    return _size;
}

SMSG_BATTLEFIELD_STATUS SMSG_BATTLEFIELD_STATUS_read(Reader& reader) {
    SMSG_BATTLEFIELD_STATUS obj;

    obj.queue_slot = reader.read_u32();

    obj.map = static_cast<Map>(reader.read_u32());

    if (obj.map == Map::KALIMDOR|| obj.map == Map::TESTING|| obj.map == Map::SCOTT_TEST|| obj.map == Map::CASH_TEST|| obj.map == Map::ALTERAC_VALLEY|| obj.map == Map::SHADOWFANG_KEEP|| obj.map == Map::STORMWIND_STOCKADE|| obj.map == Map::STORMWIND_PRISON|| obj.map == Map::DEADMINES|| obj.map == Map::AZSHARA_CRATER|| obj.map == Map::COLLINS_TEST|| obj.map == Map::WAILING_CAVERNS|| obj.map == Map::MONASTERY_UNUSED|| obj.map == Map::RAZORFEN_KRAUL|| obj.map == Map::BLACKFATHOM_DEEPS|| obj.map == Map::ULDAMAN|| obj.map == Map::GNOMEREGAN|| obj.map == Map::SUNKEN_TEMPLE|| obj.map == Map::RAZORFEN_DOWNS|| obj.map == Map::EMERALD_DREAM|| obj.map == Map::SCARLET_MONASTERY|| obj.map == Map::ZUL_FARRAK|| obj.map == Map::BLACKROCK_SPIRE|| obj.map == Map::BLACKROCK_DEPTHS|| obj.map == Map::ONYXIAS_LAIR|| obj.map == Map::OPENING_OF_THE_DARK_PORTAL|| obj.map == Map::SCHOLOMANCE|| obj.map == Map::ZUL_GURUB|| obj.map == Map::STRATHOLME|| obj.map == Map::MARAUDON|| obj.map == Map::DEEPRUN_TRAM|| obj.map == Map::RAGEFIRE_CHASM|| obj.map == Map::MOLTEN_CORE|| obj.map == Map::DIRE_MAUL|| obj.map == Map::ALLIANCE_PVP_BARRACKS|| obj.map == Map::HORDE_PVP_BARRACKS|| obj.map == Map::DEVELOPMENT_LAND|| obj.map == Map::BLACKWING_LAIR|| obj.map == Map::WARSONG_GULCH|| obj.map == Map::RUINS_OF_AHN_QIRAJ|| obj.map == Map::ARATHI_BASIN|| obj.map == Map::AHN_QIRAJ_TEMPLE|| obj.map == Map::NAXXRAMAS) {
        obj.bracket = static_cast<BattlegroundBracket>(reader.read_u8());

        obj.client_instance_id = reader.read_u32();

        obj.status_id = static_cast<StatusId>(reader.read_u8());

        if (obj.status_id == StatusId::WAIT_QUEUE) {
            obj.average_wait_time_in_ms = reader.read_u32();

            obj.time_in_queue_in_ms = reader.read_u32();

        }
        else if (obj.status_id == StatusId::WAIT_JOIN) {
            obj.time_to_remove_in_queue_in_ms = reader.read_u32();

        }
        else if (obj.status_id == StatusId::IN_PROGRESS) {
            obj.time_to_bg_autoleave_in_ms = reader.read_u32();

            obj.time_to_bg_start_in_ms = reader.read_u32();

        }
    }
    return obj;
}

std::vector<unsigned char> SMSG_BATTLEFIELD_STATUS::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_BATTLEFIELD_STATUS_size(obj));

    writer.write_u16_be(SMSG_BATTLEFIELD_STATUS_size(obj) + 2); /* size */

    writer.write_u16(0x000002d4); /* opcode */

    writer.write_u32(obj.queue_slot);

    writer.write_u32(static_cast<uint32_t>(obj.map));

    if (obj.map == Map::KALIMDOR|| obj.map == Map::TESTING|| obj.map == Map::SCOTT_TEST|| obj.map == Map::CASH_TEST|| obj.map == Map::ALTERAC_VALLEY|| obj.map == Map::SHADOWFANG_KEEP|| obj.map == Map::STORMWIND_STOCKADE|| obj.map == Map::STORMWIND_PRISON|| obj.map == Map::DEADMINES|| obj.map == Map::AZSHARA_CRATER|| obj.map == Map::COLLINS_TEST|| obj.map == Map::WAILING_CAVERNS|| obj.map == Map::MONASTERY_UNUSED|| obj.map == Map::RAZORFEN_KRAUL|| obj.map == Map::BLACKFATHOM_DEEPS|| obj.map == Map::ULDAMAN|| obj.map == Map::GNOMEREGAN|| obj.map == Map::SUNKEN_TEMPLE|| obj.map == Map::RAZORFEN_DOWNS|| obj.map == Map::EMERALD_DREAM|| obj.map == Map::SCARLET_MONASTERY|| obj.map == Map::ZUL_FARRAK|| obj.map == Map::BLACKROCK_SPIRE|| obj.map == Map::BLACKROCK_DEPTHS|| obj.map == Map::ONYXIAS_LAIR|| obj.map == Map::OPENING_OF_THE_DARK_PORTAL|| obj.map == Map::SCHOLOMANCE|| obj.map == Map::ZUL_GURUB|| obj.map == Map::STRATHOLME|| obj.map == Map::MARAUDON|| obj.map == Map::DEEPRUN_TRAM|| obj.map == Map::RAGEFIRE_CHASM|| obj.map == Map::MOLTEN_CORE|| obj.map == Map::DIRE_MAUL|| obj.map == Map::ALLIANCE_PVP_BARRACKS|| obj.map == Map::HORDE_PVP_BARRACKS|| obj.map == Map::DEVELOPMENT_LAND|| obj.map == Map::BLACKWING_LAIR|| obj.map == Map::WARSONG_GULCH|| obj.map == Map::RUINS_OF_AHN_QIRAJ|| obj.map == Map::ARATHI_BASIN|| obj.map == Map::AHN_QIRAJ_TEMPLE|| obj.map == Map::NAXXRAMAS) {
        writer.write_u8(static_cast<uint8_t>(obj.bracket));

        writer.write_u32(obj.client_instance_id);

        writer.write_u8(static_cast<uint8_t>(obj.status_id));

        if (obj.status_id == StatusId::WAIT_QUEUE) {
            writer.write_u32(obj.average_wait_time_in_ms);

            writer.write_u32(obj.time_in_queue_in_ms);

        }
        else if (obj.status_id == StatusId::WAIT_JOIN) {
            writer.write_u32(obj.time_to_remove_in_queue_in_ms);

        }
        else if (obj.status_id == StatusId::IN_PROGRESS) {
            writer.write_u32(obj.time_to_bg_autoleave_in_ms);

            writer.write_u32(obj.time_to_bg_start_in_ms);

        }
    }
    return writer.m_buf;
}

CMSG_BATTLEFIELD_PORT CMSG_BATTLEFIELD_PORT_read(Reader& reader) {
    CMSG_BATTLEFIELD_PORT obj;

    obj.map = static_cast<Map>(reader.read_u32());

    obj.action = static_cast<BattlefieldPortAction>(reader.read_u8());

    return obj;
}

std::vector<unsigned char> CMSG_BATTLEFIELD_PORT::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0005);

    writer.write_u16_be(0x0005 + 4); /* size */

    writer.write_u32(0x000002d5); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.map));

    writer.write_u8(static_cast<uint8_t>(obj.action));

    return writer.m_buf;
}

MSG_INSPECT_HONOR_STATS_Client MSG_INSPECT_HONOR_STATS_Client_read(Reader& reader) {
    MSG_INSPECT_HONOR_STATS_Client obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> MSG_INSPECT_HONOR_STATS_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x000002d6); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

MSG_INSPECT_HONOR_STATS_Server MSG_INSPECT_HONOR_STATS_Server_read(Reader& reader) {
    MSG_INSPECT_HONOR_STATS_Server obj;

    obj.guid = reader.read_u64();

    obj.highest_rank = static_cast<PvpRank>(reader.read_u8());

    obj.today_honorable_and_dishonorable = reader.read_u32();

    obj.yesterday_honorable = reader.read_u16();

    obj.unknown1 = reader.read_u16();

    obj.last_week_honorable = reader.read_u16();

    obj.unknown2 = reader.read_u16();

    obj.this_week_honorable = reader.read_u16();

    obj.unknown3 = reader.read_u16();

    obj.lifetime_honorable = reader.read_u32();

    obj.lifetime_dishonorable = reader.read_u32();

    obj.yesterday_honor = reader.read_u32();

    obj.last_week_honor = reader.read_u32();

    obj.this_week_honor = reader.read_u32();

    obj.last_week_standing = static_cast<PvpRank>(reader.read_u32());

    obj.rank_progress_bar = reader.read_u8();

    return obj;
}

std::vector<unsigned char> MSG_INSPECT_HONOR_STATS_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0032);

    writer.write_u16_be(0x0032 + 2); /* size */

    writer.write_u16(0x000002d6); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u8(static_cast<uint8_t>(obj.highest_rank));

    writer.write_u32(obj.today_honorable_and_dishonorable);

    writer.write_u16(obj.yesterday_honorable);

    writer.write_u16(obj.unknown1);

    writer.write_u16(obj.last_week_honorable);

    writer.write_u16(obj.unknown2);

    writer.write_u16(obj.this_week_honorable);

    writer.write_u16(obj.unknown3);

    writer.write_u32(obj.lifetime_honorable);

    writer.write_u32(obj.lifetime_dishonorable);

    writer.write_u32(obj.yesterday_honor);

    writer.write_u32(obj.last_week_honor);

    writer.write_u32(obj.this_week_honor);

    writer.write_u32(static_cast<uint32_t>(obj.last_week_standing));

    writer.write_u8(obj.rank_progress_bar);

    return writer.m_buf;
}

CMSG_BATTLEMASTER_HELLO CMSG_BATTLEMASTER_HELLO_read(Reader& reader) {
    CMSG_BATTLEMASTER_HELLO obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_BATTLEMASTER_HELLO::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x000002d7); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

static size_t SMSG_FORCE_WALK_SPEED_CHANGE_size(const SMSG_FORCE_WALK_SPEED_CHANGE& obj) {
    return 8 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_FORCE_WALK_SPEED_CHANGE SMSG_FORCE_WALK_SPEED_CHANGE_read(Reader& reader) {
    SMSG_FORCE_WALK_SPEED_CHANGE obj;

    obj.guid = reader.read_packed_guid();

    obj.move_event = reader.read_u32();

    obj.speed = reader.read_float();

    return obj;
}

std::vector<unsigned char> SMSG_FORCE_WALK_SPEED_CHANGE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_FORCE_WALK_SPEED_CHANGE_size(obj));

    writer.write_u16_be(SMSG_FORCE_WALK_SPEED_CHANGE_size(obj) + 2); /* size */

    writer.write_u16(0x000002da); /* opcode */

    writer.write_packed_guid(obj.guid);

    writer.write_u32(obj.move_event);

    writer.write_float(obj.speed);

    return writer.m_buf;
}

static size_t CMSG_FORCE_WALK_SPEED_CHANGE_ACK_size(const CMSG_FORCE_WALK_SPEED_CHANGE_ACK& obj) {
    return 16 + MovementInfo_size(obj.info);
}

CMSG_FORCE_WALK_SPEED_CHANGE_ACK CMSG_FORCE_WALK_SPEED_CHANGE_ACK_read(Reader& reader) {
    CMSG_FORCE_WALK_SPEED_CHANGE_ACK obj;

    obj.guid = reader.read_u64();

    obj.counter = reader.read_u32();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    obj.new_speed = reader.read_float();

    return obj;
}

std::vector<unsigned char> CMSG_FORCE_WALK_SPEED_CHANGE_ACK::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_FORCE_WALK_SPEED_CHANGE_ACK_size(obj));

    writer.write_u16_be((uint16_t)CMSG_FORCE_WALK_SPEED_CHANGE_ACK_size(obj) + 4); /* size */

    writer.write_u32(0x000002db); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.counter);

    MovementInfo_write(writer, obj.info);

    writer.write_float(obj.new_speed);

    return writer.m_buf;
}

static size_t SMSG_FORCE_SWIM_BACK_SPEED_CHANGE_size(const SMSG_FORCE_SWIM_BACK_SPEED_CHANGE& obj) {
    return 8 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_FORCE_SWIM_BACK_SPEED_CHANGE SMSG_FORCE_SWIM_BACK_SPEED_CHANGE_read(Reader& reader) {
    SMSG_FORCE_SWIM_BACK_SPEED_CHANGE obj;

    obj.guid = reader.read_packed_guid();

    obj.move_event = reader.read_u32();

    obj.speed = reader.read_float();

    return obj;
}

std::vector<unsigned char> SMSG_FORCE_SWIM_BACK_SPEED_CHANGE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_FORCE_SWIM_BACK_SPEED_CHANGE_size(obj));

    writer.write_u16_be(SMSG_FORCE_SWIM_BACK_SPEED_CHANGE_size(obj) + 2); /* size */

    writer.write_u16(0x000002dc); /* opcode */

    writer.write_packed_guid(obj.guid);

    writer.write_u32(obj.move_event);

    writer.write_float(obj.speed);

    return writer.m_buf;
}

static size_t CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK_size(const CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK& obj) {
    return 16 + MovementInfo_size(obj.info);
}

CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK_read(Reader& reader) {
    CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK obj;

    obj.guid = reader.read_u64();

    obj.counter = reader.read_u32();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    obj.new_speed = reader.read_float();

    return obj;
}

std::vector<unsigned char> CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK_size(obj));

    writer.write_u16_be((uint16_t)CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK_size(obj) + 4); /* size */

    writer.write_u32(0x000002dd); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.counter);

    MovementInfo_write(writer, obj.info);

    writer.write_float(obj.new_speed);

    return writer.m_buf;
}

static size_t SMSG_FORCE_TURN_RATE_CHANGE_size(const SMSG_FORCE_TURN_RATE_CHANGE& obj) {
    return 8 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_FORCE_TURN_RATE_CHANGE SMSG_FORCE_TURN_RATE_CHANGE_read(Reader& reader) {
    SMSG_FORCE_TURN_RATE_CHANGE obj;

    obj.guid = reader.read_packed_guid();

    obj.move_event = reader.read_u32();

    obj.speed = reader.read_float();

    return obj;
}

std::vector<unsigned char> SMSG_FORCE_TURN_RATE_CHANGE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_FORCE_TURN_RATE_CHANGE_size(obj));

    writer.write_u16_be(SMSG_FORCE_TURN_RATE_CHANGE_size(obj) + 2); /* size */

    writer.write_u16(0x000002de); /* opcode */

    writer.write_packed_guid(obj.guid);

    writer.write_u32(obj.move_event);

    writer.write_float(obj.speed);

    return writer.m_buf;
}

static size_t CMSG_FORCE_TURN_RATE_CHANGE_ACK_size(const CMSG_FORCE_TURN_RATE_CHANGE_ACK& obj) {
    return 16 + MovementInfo_size(obj.info);
}

CMSG_FORCE_TURN_RATE_CHANGE_ACK CMSG_FORCE_TURN_RATE_CHANGE_ACK_read(Reader& reader) {
    CMSG_FORCE_TURN_RATE_CHANGE_ACK obj;

    obj.guid = reader.read_u64();

    obj.counter = reader.read_u32();

    obj.info = ::wow_world_messages::vanilla::MovementInfo_read(reader);

    obj.new_speed = reader.read_float();

    return obj;
}

std::vector<unsigned char> CMSG_FORCE_TURN_RATE_CHANGE_ACK::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_FORCE_TURN_RATE_CHANGE_ACK_size(obj));

    writer.write_u16_be((uint16_t)CMSG_FORCE_TURN_RATE_CHANGE_ACK_size(obj) + 4); /* size */

    writer.write_u32(0x000002df); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.counter);

    MovementInfo_write(writer, obj.info);

    writer.write_float(obj.new_speed);

    return writer.m_buf;
}

std::vector<unsigned char> MSG_PVP_LOG_DATA_Client::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x000002e0); /* opcode */

    return writer.m_buf;
}

static size_t MSG_PVP_LOG_DATA_Server_size(const MSG_PVP_LOG_DATA_Server& obj) {
    size_t _size = 5;

    if (obj.status == BattlegroundEndStatus::ENDED) {
        _size += 1;
    }

    for(const auto& v : obj.players) {
        _size += BattlegroundPlayer_size(v);
    }

    return _size;
}

MSG_PVP_LOG_DATA_Server MSG_PVP_LOG_DATA_Server_read(Reader& reader) {
    MSG_PVP_LOG_DATA_Server obj;

    obj.status = static_cast<BattlegroundEndStatus>(reader.read_u8());

    if (obj.status == BattlegroundEndStatus::ENDED) {
        obj.winner = static_cast<BattlegroundWinner>(reader.read_u8());

    }
    obj.amount_of_players = reader.read_u32();

    for (auto i = 0; i < obj.amount_of_players; ++i) {
        obj.players.push_back(::wow_world_messages::vanilla::BattlegroundPlayer_read(reader));
    }

    return obj;
}

std::vector<unsigned char> MSG_PVP_LOG_DATA_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_PVP_LOG_DATA_Server_size(obj));

    writer.write_u16_be(MSG_PVP_LOG_DATA_Server_size(obj) + 2); /* size */

    writer.write_u16(0x000002e0); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.status));

    if (obj.status == BattlegroundEndStatus::ENDED) {
        writer.write_u8(static_cast<uint8_t>(obj.winner));

    }
    writer.write_u32(obj.amount_of_players);

    for (const auto& v : obj.players) {
        BattlegroundPlayer_write(writer, v);
    }

    return writer.m_buf;
}

CMSG_LEAVE_BATTLEFIELD CMSG_LEAVE_BATTLEFIELD_read(Reader& reader) {
    CMSG_LEAVE_BATTLEFIELD obj;

    obj.map = static_cast<Map>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> CMSG_LEAVE_BATTLEFIELD::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 4); /* size */

    writer.write_u32(0x000002e1); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.map));

    return writer.m_buf;
}

CMSG_AREA_SPIRIT_HEALER_QUERY CMSG_AREA_SPIRIT_HEALER_QUERY_read(Reader& reader) {
    CMSG_AREA_SPIRIT_HEALER_QUERY obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_AREA_SPIRIT_HEALER_QUERY::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x000002e2); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

CMSG_AREA_SPIRIT_HEALER_QUEUE CMSG_AREA_SPIRIT_HEALER_QUEUE_read(Reader& reader) {
    CMSG_AREA_SPIRIT_HEALER_QUEUE obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_AREA_SPIRIT_HEALER_QUEUE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x000002e3); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

SMSG_AREA_SPIRIT_HEALER_TIME SMSG_AREA_SPIRIT_HEALER_TIME_read(Reader& reader) {
    SMSG_AREA_SPIRIT_HEALER_TIME obj;

    obj.guid = reader.read_u64();

    obj.next_resurrect_time = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_AREA_SPIRIT_HEALER_TIME::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 2); /* size */

    writer.write_u16(0x000002e4); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.next_resurrect_time);

    return writer.m_buf;
}

static size_t SMSG_WARDEN_DATA_size(const SMSG_WARDEN_DATA& obj) {
    size_t _size = 0;

    for(const auto& v : obj.encrypted_data) {
        _size += 1;
    }

    return _size;
}

SMSG_WARDEN_DATA SMSG_WARDEN_DATA_read(Reader& reader, size_t body_size) {
    SMSG_WARDEN_DATA obj;
    size_t _size = 0;

    while (_size < body_size) {
        obj.encrypted_data.push_back(reader.read_u8());
        _size += 1;
    }

    return obj;
}

std::vector<unsigned char> SMSG_WARDEN_DATA::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_WARDEN_DATA_size(obj));

    writer.write_u16_be(SMSG_WARDEN_DATA_size(obj) + 2); /* size */

    writer.write_u16(0x000002e6); /* opcode */

    for (const auto& v : obj.encrypted_data) {
        writer.write_u8(v);
    }

    return writer.m_buf;
}

static size_t CMSG_WARDEN_DATA_size(const CMSG_WARDEN_DATA& obj) {
    size_t _size = 0;

    for(const auto& v : obj.encrypted_data) {
        _size += 1;
    }

    return _size;
}

CMSG_WARDEN_DATA CMSG_WARDEN_DATA_read(Reader& reader, size_t body_size) {
    CMSG_WARDEN_DATA obj;
    size_t _size = 0;

    while (_size < body_size) {
        obj.encrypted_data.push_back(reader.read_u8());
        _size += 1;
    }

    return obj;
}

std::vector<unsigned char> CMSG_WARDEN_DATA::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_WARDEN_DATA_size(obj));

    writer.write_u16_be((uint16_t)CMSG_WARDEN_DATA_size(obj) + 4); /* size */

    writer.write_u32(0x000002e7); /* opcode */

    for (const auto& v : obj.encrypted_data) {
        writer.write_u8(v);
    }

    return writer.m_buf;
}

SMSG_GROUP_JOINED_BATTLEGROUND SMSG_GROUP_JOINED_BATTLEGROUND_read(Reader& reader) {
    SMSG_GROUP_JOINED_BATTLEGROUND obj;

    obj.id = static_cast<BgTypeId>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_GROUP_JOINED_BATTLEGROUND::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x000002e8); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.id));

    return writer.m_buf;
}

std::vector<unsigned char> MSG_BATTLEGROUND_PLAYER_POSITIONS_Client::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x000002e9); /* opcode */

    return writer.m_buf;
}

static size_t MSG_BATTLEGROUND_PLAYER_POSITIONS_Server_size(const MSG_BATTLEGROUND_PLAYER_POSITIONS_Server& obj) {
    return 5 + 16 * obj.teammates.size() + 16 * obj.carriers.size();
}

MSG_BATTLEGROUND_PLAYER_POSITIONS_Server MSG_BATTLEGROUND_PLAYER_POSITIONS_Server_read(Reader& reader) {
    MSG_BATTLEGROUND_PLAYER_POSITIONS_Server obj;

    obj.amount_of_teammates = reader.read_u32();

    for (auto i = 0; i < obj.amount_of_teammates; ++i) {
        obj.teammates.push_back(::wow_world_messages::vanilla::BattlegroundPlayerPosition_read(reader));
    }

    obj.amount_of_carriers = reader.read_u8();

    for (auto i = 0; i < obj.amount_of_carriers; ++i) {
        obj.carriers.push_back(::wow_world_messages::vanilla::BattlegroundPlayerPosition_read(reader));
    }

    return obj;
}

std::vector<unsigned char> MSG_BATTLEGROUND_PLAYER_POSITIONS_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_BATTLEGROUND_PLAYER_POSITIONS_Server_size(obj));

    writer.write_u16_be(MSG_BATTLEGROUND_PLAYER_POSITIONS_Server_size(obj) + 2); /* size */

    writer.write_u16(0x000002e9); /* opcode */

    writer.write_u32(obj.amount_of_teammates);

    for (const auto& v : obj.teammates) {
        BattlegroundPlayerPosition_write(writer, v);
    }

    writer.write_u8(obj.amount_of_carriers);

    for (const auto& v : obj.carriers) {
        BattlegroundPlayerPosition_write(writer, v);
    }

    return writer.m_buf;
}

CMSG_PET_STOP_ATTACK CMSG_PET_STOP_ATTACK_read(Reader& reader) {
    CMSG_PET_STOP_ATTACK obj;

    obj.pet = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_PET_STOP_ATTACK::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x000002ea); /* opcode */

    writer.write_u64(obj.pet);

    return writer.m_buf;
}

SMSG_BINDER_CONFIRM SMSG_BINDER_CONFIRM_read(Reader& reader) {
    SMSG_BINDER_CONFIRM obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> SMSG_BINDER_CONFIRM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 2); /* size */

    writer.write_u16(0x000002eb); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

SMSG_BATTLEGROUND_PLAYER_JOINED SMSG_BATTLEGROUND_PLAYER_JOINED_read(Reader& reader) {
    SMSG_BATTLEGROUND_PLAYER_JOINED obj;

    obj.player = reader.read_u64();

    return obj;
}

std::vector<unsigned char> SMSG_BATTLEGROUND_PLAYER_JOINED::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 2); /* size */

    writer.write_u16(0x000002ec); /* opcode */

    writer.write_u64(obj.player);

    return writer.m_buf;
}

SMSG_BATTLEGROUND_PLAYER_LEFT SMSG_BATTLEGROUND_PLAYER_LEFT_read(Reader& reader) {
    SMSG_BATTLEGROUND_PLAYER_LEFT obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> SMSG_BATTLEGROUND_PLAYER_LEFT::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 2); /* size */

    writer.write_u16(0x000002ed); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

CMSG_BATTLEMASTER_JOIN CMSG_BATTLEMASTER_JOIN_read(Reader& reader) {
    CMSG_BATTLEMASTER_JOIN obj;

    obj.guid = reader.read_u64();

    obj.map = static_cast<Map>(reader.read_u32());

    obj.instance_id = reader.read_u32();

    obj.join_as_group = reader.read_bool8();

    return obj;
}

std::vector<unsigned char> CMSG_BATTLEMASTER_JOIN::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0011);

    writer.write_u16_be(0x0011 + 4); /* size */

    writer.write_u32(0x000002ee); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(static_cast<uint32_t>(obj.map));

    writer.write_u32(obj.instance_id);

    writer.write_bool8(obj.join_as_group);

    return writer.m_buf;
}

static size_t SMSG_ADDON_INFO_size(const SMSG_ADDON_INFO& obj) {
    size_t _size = 0;

    for(const auto& v : obj.addons) {
        _size += Addon_size(v);
    }

    return _size;
}

SMSG_ADDON_INFO SMSG_ADDON_INFO_read(Reader& reader, size_t body_size) {
    SMSG_ADDON_INFO obj;
    size_t _size = 0;

    while (_size < body_size) {
        obj.addons.push_back(::wow_world_messages::vanilla::Addon_read(reader));
        _size += vanilla::Addon_size(obj.addons.back());
    }

    return obj;
}

std::vector<unsigned char> SMSG_ADDON_INFO::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_ADDON_INFO_size(obj));

    writer.write_u16_be(SMSG_ADDON_INFO_size(obj) + 2); /* size */

    writer.write_u16(0x000002ef); /* opcode */

    for (const auto& v : obj.addons) {
        Addon_write(writer, v);
    }

    return writer.m_buf;
}

CMSG_PET_UNLEARN CMSG_PET_UNLEARN_read(Reader& reader) {
    CMSG_PET_UNLEARN obj;

    obj.pet = reader.read_u64();

    return obj;
}

std::vector<unsigned char> CMSG_PET_UNLEARN::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 4); /* size */

    writer.write_u32(0x000002f0); /* opcode */

    writer.write_u64(obj.pet);

    return writer.m_buf;
}

SMSG_PET_UNLEARN_CONFIRM SMSG_PET_UNLEARN_CONFIRM_read(Reader& reader) {
    SMSG_PET_UNLEARN_CONFIRM obj;

    obj.pet = reader.read_u64();

    obj.talent_reset_cost = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_PET_UNLEARN_CONFIRM::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 2); /* size */

    writer.write_u16(0x000002f1); /* opcode */

    writer.write_u64(obj.pet);

    writer.write_u32(obj.talent_reset_cost);

    return writer.m_buf;
}

static size_t SMSG_PARTY_MEMBER_STATS_FULL_size(const SMSG_PARTY_MEMBER_STATS_FULL& obj) {
    size_t _size = 4 + ::wow_world_messages::util::wwm_packed_guid_size(obj.player);

    if ((obj.mask & GROUP_UPDATE_FLAGS_STATUS) != 0) {
        _size += 1;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_CUR_HP) != 0) {
        _size += 2;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_MAX_HP) != 0) {
        _size += 2;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_POWER_TYPE) != 0) {
        _size += 1;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_CUR_POWER) != 0) {
        _size += 2;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_MAX_POWER) != 0) {
        _size += 2;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_LEVEL) != 0) {
        _size += 2;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_ZONE) != 0) {
        _size += 4;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_POSITION) != 0) {
        _size += 4;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_AURAS) != 0) {
        _size += 0 + aura_mask_size(obj.auras);
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_GUID) != 0) {
        _size += 8;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_NAME) != 0) {
        _size += 1 + obj.pet_name.size();
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_MODEL_ID) != 0) {
        _size += 2;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_CUR_HP) != 0) {
        _size += 2;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_MAX_HP) != 0) {
        _size += 2;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_POWER_TYPE) != 0) {
        _size += 1;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_CUR_POWER) != 0) {
        _size += 2;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_MAX_POWER) != 0) {
        _size += 2;
    }

    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_AURAS) != 0) {
        _size += 0 + aura_mask_size(obj.pet_auras);
    }

    return _size;
}

SMSG_PARTY_MEMBER_STATS_FULL SMSG_PARTY_MEMBER_STATS_FULL_read(Reader& reader) {
    SMSG_PARTY_MEMBER_STATS_FULL obj;

    obj.player = reader.read_packed_guid();

    obj.mask = static_cast<GroupUpdateFlags>(reader.read_u32());

    if ((obj.mask & GROUP_UPDATE_FLAGS_STATUS) != 0) {
        obj.status = static_cast<GroupMemberOnlineStatus>(reader.read_u8());

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_CUR_HP) != 0) {
        obj.current_health = reader.read_u16();

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_MAX_HP) != 0) {
        obj.max_health = reader.read_u16();

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_POWER_TYPE) != 0) {
        obj.power = static_cast<Power>(reader.read_u8());

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_CUR_POWER) != 0) {
        obj.current_power = reader.read_u16();

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_MAX_POWER) != 0) {
        obj.max_power = reader.read_u16();

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_LEVEL) != 0) {
        obj.level = reader.read_u16();

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_ZONE) != 0) {
        obj.area = static_cast<Area>(reader.read_u32());

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_POSITION) != 0) {
        obj.position_x = reader.read_u16();

        obj.position_y = reader.read_u16();

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_AURAS) != 0) {
        obj.auras = ::wow_world_messages::vanilla::aura_mask_read(reader);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_GUID) != 0) {
        obj.pet = reader.read_u64();

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_NAME) != 0) {
        obj.pet_name = reader.read_cstring();

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_MODEL_ID) != 0) {
        obj.pet_display_id = reader.read_u16();

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_CUR_HP) != 0) {
        obj.pet_current_health = reader.read_u16();

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_MAX_HP) != 0) {
        obj.pet_max_health = reader.read_u16();

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_POWER_TYPE) != 0) {
        obj.pet_power_type = static_cast<Power>(reader.read_u8());

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_CUR_POWER) != 0) {
        obj.pet_current_power = reader.read_u16();

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_MAX_POWER) != 0) {
        obj.pet_max_power = reader.read_u16();

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_AURAS) != 0) {
        obj.pet_auras = ::wow_world_messages::vanilla::aura_mask_read(reader);

    }
    return obj;
}

std::vector<unsigned char> SMSG_PARTY_MEMBER_STATS_FULL::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_PARTY_MEMBER_STATS_FULL_size(obj));

    writer.write_u16_be(SMSG_PARTY_MEMBER_STATS_FULL_size(obj) + 2); /* size */

    writer.write_u16(0x000002f2); /* opcode */

    writer.write_packed_guid(obj.player);

    writer.write_u32(static_cast<uint32_t>(obj.mask));

    if ((obj.mask & GROUP_UPDATE_FLAGS_STATUS) != 0) {
        writer.write_u8(static_cast<uint8_t>(obj.status));

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_CUR_HP) != 0) {
        writer.write_u16(obj.current_health);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_MAX_HP) != 0) {
        writer.write_u16(obj.max_health);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_POWER_TYPE) != 0) {
        writer.write_u8(static_cast<uint8_t>(obj.power));

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_CUR_POWER) != 0) {
        writer.write_u16(obj.current_power);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_MAX_POWER) != 0) {
        writer.write_u16(obj.max_power);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_LEVEL) != 0) {
        writer.write_u16(obj.level);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_ZONE) != 0) {
        writer.write_u32(static_cast<uint32_t>(obj.area));

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_POSITION) != 0) {
        writer.write_u16(obj.position_x);

        writer.write_u16(obj.position_y);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_AURAS) != 0) {
        aura_mask_write(writer, obj.auras);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_GUID) != 0) {
        writer.write_u64(obj.pet);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_NAME) != 0) {
        writer.write_cstring(obj.pet_name);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_MODEL_ID) != 0) {
        writer.write_u16(obj.pet_display_id);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_CUR_HP) != 0) {
        writer.write_u16(obj.pet_current_health);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_MAX_HP) != 0) {
        writer.write_u16(obj.pet_max_health);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_POWER_TYPE) != 0) {
        writer.write_u8(static_cast<uint8_t>(obj.pet_power_type));

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_CUR_POWER) != 0) {
        writer.write_u16(obj.pet_current_power);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_MAX_POWER) != 0) {
        writer.write_u16(obj.pet_max_power);

    }
    if ((obj.mask & GROUP_UPDATE_FLAGS_PET_AURAS) != 0) {
        aura_mask_write(writer, obj.pet_auras);

    }
    return writer.m_buf;
}

CMSG_PET_SPELL_AUTOCAST CMSG_PET_SPELL_AUTOCAST_read(Reader& reader) {
    CMSG_PET_SPELL_AUTOCAST obj;

    obj.guid = reader.read_u64();

    obj.id = reader.read_u32();

    obj.autocast_enabled = reader.read_bool8();

    return obj;
}

std::vector<unsigned char> CMSG_PET_SPELL_AUTOCAST::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000d);

    writer.write_u16_be(0x000d + 4); /* size */

    writer.write_u32(0x000002f3); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.id);

    writer.write_bool8(obj.autocast_enabled);

    return writer.m_buf;
}

SMSG_WEATHER SMSG_WEATHER_read(Reader& reader) {
    SMSG_WEATHER obj;

    obj.weather_type = static_cast<WeatherType>(reader.read_u32());

    obj.grade = reader.read_float();

    obj.sound_id = reader.read_u32();

    obj.change = static_cast<WeatherChangeType>(reader.read_u8());

    return obj;
}

std::vector<unsigned char> SMSG_WEATHER::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000d);

    writer.write_u16_be(0x000d + 2); /* size */

    writer.write_u16(0x000002f4); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.weather_type));

    writer.write_float(obj.grade);

    writer.write_u32(obj.sound_id);

    writer.write_u8(static_cast<uint8_t>(obj.change));

    return writer.m_buf;
}

SMSG_RAID_INSTANCE_MESSAGE SMSG_RAID_INSTANCE_MESSAGE_read(Reader& reader) {
    SMSG_RAID_INSTANCE_MESSAGE obj;

    obj.message_type = static_cast<RaidInstanceMessage>(reader.read_u32());

    obj.map = static_cast<Map>(reader.read_u32());

    obj.time_left = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_RAID_INSTANCE_MESSAGE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 2); /* size */

    writer.write_u16(0x000002fa); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.message_type));

    writer.write_u32(static_cast<uint32_t>(obj.map));

    writer.write_u32(obj.time_left);

    return writer.m_buf;
}

static size_t CMSG_GUILD_INFO_TEXT_size(const CMSG_GUILD_INFO_TEXT& obj) {
    return 1 + obj.guild_info.size();
}

CMSG_GUILD_INFO_TEXT CMSG_GUILD_INFO_TEXT_read(Reader& reader) {
    CMSG_GUILD_INFO_TEXT obj;

    obj.guild_info = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_GUILD_INFO_TEXT::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_GUILD_INFO_TEXT_size(obj));

    writer.write_u16_be((uint16_t)CMSG_GUILD_INFO_TEXT_size(obj) + 4); /* size */

    writer.write_u32(0x000002fc); /* opcode */

    writer.write_cstring(obj.guild_info);

    return writer.m_buf;
}

std::vector<unsigned char> SMSG_CHAT_RESTRICTED::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 2); /* size */

    writer.write_u16(0x000002fd); /* opcode */

    return writer.m_buf;
}

static size_t SMSG_SPLINE_SET_RUN_SPEED_size(const SMSG_SPLINE_SET_RUN_SPEED& obj) {
    return 4 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_SPLINE_SET_RUN_SPEED SMSG_SPLINE_SET_RUN_SPEED_read(Reader& reader) {
    SMSG_SPLINE_SET_RUN_SPEED obj;

    obj.guid = reader.read_packed_guid();

    obj.speed = reader.read_float();

    return obj;
}

std::vector<unsigned char> SMSG_SPLINE_SET_RUN_SPEED::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SPLINE_SET_RUN_SPEED_size(obj));

    writer.write_u16_be(SMSG_SPLINE_SET_RUN_SPEED_size(obj) + 2); /* size */

    writer.write_u16(0x000002fe); /* opcode */

    writer.write_packed_guid(obj.guid);

    writer.write_float(obj.speed);

    return writer.m_buf;
}

static size_t SMSG_SPLINE_SET_RUN_BACK_SPEED_size(const SMSG_SPLINE_SET_RUN_BACK_SPEED& obj) {
    return 4 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_SPLINE_SET_RUN_BACK_SPEED SMSG_SPLINE_SET_RUN_BACK_SPEED_read(Reader& reader) {
    SMSG_SPLINE_SET_RUN_BACK_SPEED obj;

    obj.guid = reader.read_packed_guid();

    obj.speed = reader.read_float();

    return obj;
}

std::vector<unsigned char> SMSG_SPLINE_SET_RUN_BACK_SPEED::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SPLINE_SET_RUN_BACK_SPEED_size(obj));

    writer.write_u16_be(SMSG_SPLINE_SET_RUN_BACK_SPEED_size(obj) + 2); /* size */

    writer.write_u16(0x000002ff); /* opcode */

    writer.write_packed_guid(obj.guid);

    writer.write_float(obj.speed);

    return writer.m_buf;
}

static size_t SMSG_SPLINE_SET_SWIM_SPEED_size(const SMSG_SPLINE_SET_SWIM_SPEED& obj) {
    return 4 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_SPLINE_SET_SWIM_SPEED SMSG_SPLINE_SET_SWIM_SPEED_read(Reader& reader) {
    SMSG_SPLINE_SET_SWIM_SPEED obj;

    obj.guid = reader.read_packed_guid();

    obj.speed = reader.read_float();

    return obj;
}

std::vector<unsigned char> SMSG_SPLINE_SET_SWIM_SPEED::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SPLINE_SET_SWIM_SPEED_size(obj));

    writer.write_u16_be(SMSG_SPLINE_SET_SWIM_SPEED_size(obj) + 2); /* size */

    writer.write_u16(0x00000300); /* opcode */

    writer.write_packed_guid(obj.guid);

    writer.write_float(obj.speed);

    return writer.m_buf;
}

static size_t SMSG_SPLINE_SET_WALK_SPEED_size(const SMSG_SPLINE_SET_WALK_SPEED& obj) {
    return 4 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_SPLINE_SET_WALK_SPEED SMSG_SPLINE_SET_WALK_SPEED_read(Reader& reader) {
    SMSG_SPLINE_SET_WALK_SPEED obj;

    obj.guid = reader.read_packed_guid();

    obj.speed = reader.read_float();

    return obj;
}

std::vector<unsigned char> SMSG_SPLINE_SET_WALK_SPEED::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SPLINE_SET_WALK_SPEED_size(obj));

    writer.write_u16_be(SMSG_SPLINE_SET_WALK_SPEED_size(obj) + 2); /* size */

    writer.write_u16(0x00000301); /* opcode */

    writer.write_packed_guid(obj.guid);

    writer.write_float(obj.speed);

    return writer.m_buf;
}

static size_t SMSG_SPLINE_SET_SWIM_BACK_SPEED_size(const SMSG_SPLINE_SET_SWIM_BACK_SPEED& obj) {
    return 4 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_SPLINE_SET_SWIM_BACK_SPEED SMSG_SPLINE_SET_SWIM_BACK_SPEED_read(Reader& reader) {
    SMSG_SPLINE_SET_SWIM_BACK_SPEED obj;

    obj.guid = reader.read_packed_guid();

    obj.speed = reader.read_float();

    return obj;
}

std::vector<unsigned char> SMSG_SPLINE_SET_SWIM_BACK_SPEED::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SPLINE_SET_SWIM_BACK_SPEED_size(obj));

    writer.write_u16_be(SMSG_SPLINE_SET_SWIM_BACK_SPEED_size(obj) + 2); /* size */

    writer.write_u16(0x00000302); /* opcode */

    writer.write_packed_guid(obj.guid);

    writer.write_float(obj.speed);

    return writer.m_buf;
}

static size_t SMSG_SPLINE_SET_TURN_RATE_size(const SMSG_SPLINE_SET_TURN_RATE& obj) {
    return 4 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_SPLINE_SET_TURN_RATE SMSG_SPLINE_SET_TURN_RATE_read(Reader& reader) {
    SMSG_SPLINE_SET_TURN_RATE obj;

    obj.guid = reader.read_packed_guid();

    obj.speed = reader.read_float();

    return obj;
}

std::vector<unsigned char> SMSG_SPLINE_SET_TURN_RATE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SPLINE_SET_TURN_RATE_size(obj));

    writer.write_u16_be(SMSG_SPLINE_SET_TURN_RATE_size(obj) + 2); /* size */

    writer.write_u16(0x00000303); /* opcode */

    writer.write_packed_guid(obj.guid);

    writer.write_float(obj.speed);

    return writer.m_buf;
}

static size_t SMSG_SPLINE_MOVE_UNROOT_size(const SMSG_SPLINE_MOVE_UNROOT& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_SPLINE_MOVE_UNROOT SMSG_SPLINE_MOVE_UNROOT_read(Reader& reader) {
    SMSG_SPLINE_MOVE_UNROOT obj;

    obj.guid = reader.read_packed_guid();

    return obj;
}

std::vector<unsigned char> SMSG_SPLINE_MOVE_UNROOT::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SPLINE_MOVE_UNROOT_size(obj));

    writer.write_u16_be(SMSG_SPLINE_MOVE_UNROOT_size(obj) + 2); /* size */

    writer.write_u16(0x00000304); /* opcode */

    writer.write_packed_guid(obj.guid);

    return writer.m_buf;
}

static size_t SMSG_SPLINE_MOVE_FEATHER_FALL_size(const SMSG_SPLINE_MOVE_FEATHER_FALL& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_SPLINE_MOVE_FEATHER_FALL SMSG_SPLINE_MOVE_FEATHER_FALL_read(Reader& reader) {
    SMSG_SPLINE_MOVE_FEATHER_FALL obj;

    obj.guid = reader.read_packed_guid();

    return obj;
}

std::vector<unsigned char> SMSG_SPLINE_MOVE_FEATHER_FALL::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SPLINE_MOVE_FEATHER_FALL_size(obj));

    writer.write_u16_be(SMSG_SPLINE_MOVE_FEATHER_FALL_size(obj) + 2); /* size */

    writer.write_u16(0x00000305); /* opcode */

    writer.write_packed_guid(obj.guid);

    return writer.m_buf;
}

static size_t SMSG_SPLINE_MOVE_NORMAL_FALL_size(const SMSG_SPLINE_MOVE_NORMAL_FALL& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_SPLINE_MOVE_NORMAL_FALL SMSG_SPLINE_MOVE_NORMAL_FALL_read(Reader& reader) {
    SMSG_SPLINE_MOVE_NORMAL_FALL obj;

    obj.guid = reader.read_packed_guid();

    return obj;
}

std::vector<unsigned char> SMSG_SPLINE_MOVE_NORMAL_FALL::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SPLINE_MOVE_NORMAL_FALL_size(obj));

    writer.write_u16_be(SMSG_SPLINE_MOVE_NORMAL_FALL_size(obj) + 2); /* size */

    writer.write_u16(0x00000306); /* opcode */

    writer.write_packed_guid(obj.guid);

    return writer.m_buf;
}

static size_t SMSG_SPLINE_MOVE_SET_HOVER_size(const SMSG_SPLINE_MOVE_SET_HOVER& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_SPLINE_MOVE_SET_HOVER SMSG_SPLINE_MOVE_SET_HOVER_read(Reader& reader) {
    SMSG_SPLINE_MOVE_SET_HOVER obj;

    obj.guid = reader.read_packed_guid();

    return obj;
}

std::vector<unsigned char> SMSG_SPLINE_MOVE_SET_HOVER::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SPLINE_MOVE_SET_HOVER_size(obj));

    writer.write_u16_be(SMSG_SPLINE_MOVE_SET_HOVER_size(obj) + 2); /* size */

    writer.write_u16(0x00000307); /* opcode */

    writer.write_packed_guid(obj.guid);

    return writer.m_buf;
}

static size_t SMSG_SPLINE_MOVE_UNSET_HOVER_size(const SMSG_SPLINE_MOVE_UNSET_HOVER& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_SPLINE_MOVE_UNSET_HOVER SMSG_SPLINE_MOVE_UNSET_HOVER_read(Reader& reader) {
    SMSG_SPLINE_MOVE_UNSET_HOVER obj;

    obj.guid = reader.read_packed_guid();

    return obj;
}

std::vector<unsigned char> SMSG_SPLINE_MOVE_UNSET_HOVER::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SPLINE_MOVE_UNSET_HOVER_size(obj));

    writer.write_u16_be(SMSG_SPLINE_MOVE_UNSET_HOVER_size(obj) + 2); /* size */

    writer.write_u16(0x00000308); /* opcode */

    writer.write_packed_guid(obj.guid);

    return writer.m_buf;
}

static size_t SMSG_SPLINE_MOVE_WATER_WALK_size(const SMSG_SPLINE_MOVE_WATER_WALK& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_SPLINE_MOVE_WATER_WALK SMSG_SPLINE_MOVE_WATER_WALK_read(Reader& reader) {
    SMSG_SPLINE_MOVE_WATER_WALK obj;

    obj.guid = reader.read_packed_guid();

    return obj;
}

std::vector<unsigned char> SMSG_SPLINE_MOVE_WATER_WALK::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SPLINE_MOVE_WATER_WALK_size(obj));

    writer.write_u16_be(SMSG_SPLINE_MOVE_WATER_WALK_size(obj) + 2); /* size */

    writer.write_u16(0x00000309); /* opcode */

    writer.write_packed_guid(obj.guid);

    return writer.m_buf;
}

static size_t SMSG_SPLINE_MOVE_LAND_WALK_size(const SMSG_SPLINE_MOVE_LAND_WALK& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_SPLINE_MOVE_LAND_WALK SMSG_SPLINE_MOVE_LAND_WALK_read(Reader& reader) {
    SMSG_SPLINE_MOVE_LAND_WALK obj;

    obj.guid = reader.read_packed_guid();

    return obj;
}

std::vector<unsigned char> SMSG_SPLINE_MOVE_LAND_WALK::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SPLINE_MOVE_LAND_WALK_size(obj));

    writer.write_u16_be(SMSG_SPLINE_MOVE_LAND_WALK_size(obj) + 2); /* size */

    writer.write_u16(0x0000030a); /* opcode */

    writer.write_packed_guid(obj.guid);

    return writer.m_buf;
}

static size_t SMSG_SPLINE_MOVE_START_SWIM_size(const SMSG_SPLINE_MOVE_START_SWIM& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_SPLINE_MOVE_START_SWIM SMSG_SPLINE_MOVE_START_SWIM_read(Reader& reader) {
    SMSG_SPLINE_MOVE_START_SWIM obj;

    obj.guid = reader.read_packed_guid();

    return obj;
}

std::vector<unsigned char> SMSG_SPLINE_MOVE_START_SWIM::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SPLINE_MOVE_START_SWIM_size(obj));

    writer.write_u16_be(SMSG_SPLINE_MOVE_START_SWIM_size(obj) + 2); /* size */

    writer.write_u16(0x0000030b); /* opcode */

    writer.write_packed_guid(obj.guid);

    return writer.m_buf;
}

static size_t SMSG_SPLINE_MOVE_STOP_SWIM_size(const SMSG_SPLINE_MOVE_STOP_SWIM& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_SPLINE_MOVE_STOP_SWIM SMSG_SPLINE_MOVE_STOP_SWIM_read(Reader& reader) {
    SMSG_SPLINE_MOVE_STOP_SWIM obj;

    obj.guid = reader.read_packed_guid();

    return obj;
}

std::vector<unsigned char> SMSG_SPLINE_MOVE_STOP_SWIM::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SPLINE_MOVE_STOP_SWIM_size(obj));

    writer.write_u16_be(SMSG_SPLINE_MOVE_STOP_SWIM_size(obj) + 2); /* size */

    writer.write_u16(0x0000030c); /* opcode */

    writer.write_packed_guid(obj.guid);

    return writer.m_buf;
}

static size_t SMSG_SPLINE_MOVE_SET_RUN_MODE_size(const SMSG_SPLINE_MOVE_SET_RUN_MODE& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_SPLINE_MOVE_SET_RUN_MODE SMSG_SPLINE_MOVE_SET_RUN_MODE_read(Reader& reader) {
    SMSG_SPLINE_MOVE_SET_RUN_MODE obj;

    obj.guid = reader.read_packed_guid();

    return obj;
}

std::vector<unsigned char> SMSG_SPLINE_MOVE_SET_RUN_MODE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SPLINE_MOVE_SET_RUN_MODE_size(obj));

    writer.write_u16_be(SMSG_SPLINE_MOVE_SET_RUN_MODE_size(obj) + 2); /* size */

    writer.write_u16(0x0000030d); /* opcode */

    writer.write_packed_guid(obj.guid);

    return writer.m_buf;
}

static size_t SMSG_SPLINE_MOVE_SET_WALK_MODE_size(const SMSG_SPLINE_MOVE_SET_WALK_MODE& obj) {
    return 0 + ::wow_world_messages::util::wwm_packed_guid_size(obj.guid);
}

SMSG_SPLINE_MOVE_SET_WALK_MODE SMSG_SPLINE_MOVE_SET_WALK_MODE_read(Reader& reader) {
    SMSG_SPLINE_MOVE_SET_WALK_MODE obj;

    obj.guid = reader.read_packed_guid();

    return obj;
}

std::vector<unsigned char> SMSG_SPLINE_MOVE_SET_WALK_MODE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SPLINE_MOVE_SET_WALK_MODE_size(obj));

    writer.write_u16_be(SMSG_SPLINE_MOVE_SET_WALK_MODE_size(obj) + 2); /* size */

    writer.write_u16(0x0000030e); /* opcode */

    writer.write_packed_guid(obj.guid);

    return writer.m_buf;
}

static size_t CMSG_ACTIVATETAXIEXPRESS_size(const CMSG_ACTIVATETAXIEXPRESS& obj) {
    return 16 + 4 * obj.nodes.size();
}

CMSG_ACTIVATETAXIEXPRESS CMSG_ACTIVATETAXIEXPRESS_read(Reader& reader) {
    CMSG_ACTIVATETAXIEXPRESS obj;

    obj.guid = reader.read_u64();

    obj.total_cost = reader.read_u32();

    obj.node_count = reader.read_u32();

    for (auto i = 0; i < obj.node_count; ++i) {
        obj.nodes.push_back(reader.read_u32());
    }

    return obj;
}

std::vector<unsigned char> CMSG_ACTIVATETAXIEXPRESS::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_ACTIVATETAXIEXPRESS_size(obj));

    writer.write_u16_be((uint16_t)CMSG_ACTIVATETAXIEXPRESS_size(obj) + 4); /* size */

    writer.write_u32(0x00000312); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(obj.total_cost);

    writer.write_u32(obj.node_count);

    for (const auto& v : obj.nodes) {
        writer.write_u32(v);
    }

    return writer.m_buf;
}

CMSG_SET_FACTION_INACTIVE CMSG_SET_FACTION_INACTIVE_read(Reader& reader) {
    CMSG_SET_FACTION_INACTIVE obj;

    obj.faction = static_cast<Faction>(reader.read_u16());

    obj.inactive = reader.read_bool8();

    return obj;
}

std::vector<unsigned char> CMSG_SET_FACTION_INACTIVE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0003);

    writer.write_u16_be(0x0003 + 4); /* size */

    writer.write_u32(0x00000317); /* opcode */

    writer.write_u16(static_cast<uint16_t>(obj.faction));

    writer.write_bool8(obj.inactive);

    return writer.m_buf;
}

CMSG_SET_WATCHED_FACTION CMSG_SET_WATCHED_FACTION_read(Reader& reader) {
    CMSG_SET_WATCHED_FACTION obj;

    obj.faction = static_cast<Faction>(reader.read_u16());

    return obj;
}

std::vector<unsigned char> CMSG_SET_WATCHED_FACTION::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0002);

    writer.write_u16_be(0x0002 + 4); /* size */

    writer.write_u32(0x00000318); /* opcode */

    writer.write_u16(static_cast<uint16_t>(obj.faction));

    return writer.m_buf;
}

static size_t MSG_MOVE_TIME_SKIPPED_Server_size(const MSG_MOVE_TIME_SKIPPED_Server& obj) {
    return 4 + ::wow_world_messages::util::wwm_packed_guid_size(obj.player);
}

MSG_MOVE_TIME_SKIPPED_Server MSG_MOVE_TIME_SKIPPED_Server_read(Reader& reader) {
    MSG_MOVE_TIME_SKIPPED_Server obj;

    obj.player = reader.read_packed_guid();

    obj.time_skipped = reader.read_u32();

    return obj;
}

std::vector<unsigned char> MSG_MOVE_TIME_SKIPPED_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_MOVE_TIME_SKIPPED_Server_size(obj));

    writer.write_u16_be(MSG_MOVE_TIME_SKIPPED_Server_size(obj) + 2); /* size */

    writer.write_u16(0x00000319); /* opcode */

    writer.write_packed_guid(obj.player);

    writer.write_u32(obj.time_skipped);

    return writer.m_buf;
}

SMSG_SPLINE_MOVE_ROOT SMSG_SPLINE_MOVE_ROOT_read(Reader& reader) {
    SMSG_SPLINE_MOVE_ROOT obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> SMSG_SPLINE_MOVE_ROOT::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 2); /* size */

    writer.write_u16(0x0000031a); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

SMSG_INVALIDATE_PLAYER SMSG_INVALIDATE_PLAYER_read(Reader& reader) {
    SMSG_INVALIDATE_PLAYER obj;

    obj.guid = reader.read_u64();

    return obj;
}

std::vector<unsigned char> SMSG_INVALIDATE_PLAYER::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 2); /* size */

    writer.write_u16(0x0000031c); /* opcode */

    writer.write_u64(obj.guid);

    return writer.m_buf;
}

std::vector<unsigned char> CMSG_RESET_INSTANCES::write() const {
    auto writer = Writer(0x0000);

    writer.write_u16_be(0x0000 + 4); /* size */

    writer.write_u32(0x0000031d); /* opcode */

    return writer.m_buf;
}

SMSG_INSTANCE_RESET SMSG_INSTANCE_RESET_read(Reader& reader) {
    SMSG_INSTANCE_RESET obj;

    obj.map = static_cast<Map>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_INSTANCE_RESET::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x0000031e); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.map));

    return writer.m_buf;
}

SMSG_INSTANCE_RESET_FAILED SMSG_INSTANCE_RESET_FAILED_read(Reader& reader) {
    SMSG_INSTANCE_RESET_FAILED obj;

    obj.reason = static_cast<InstanceResetFailedReason>(reader.read_u32());

    obj.map = static_cast<Map>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_INSTANCE_RESET_FAILED::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0008);

    writer.write_u16_be(0x0008 + 2); /* size */

    writer.write_u16(0x0000031f); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.reason));

    writer.write_u32(static_cast<uint32_t>(obj.map));

    return writer.m_buf;
}

SMSG_UPDATE_LAST_INSTANCE SMSG_UPDATE_LAST_INSTANCE_read(Reader& reader) {
    SMSG_UPDATE_LAST_INSTANCE obj;

    obj.map = static_cast<Map>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_UPDATE_LAST_INSTANCE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x00000320); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.map));

    return writer.m_buf;
}

static size_t MSG_RAID_TARGET_UPDATE_Client_size(const MSG_RAID_TARGET_UPDATE_Client& obj) {
    size_t _size = 1;

    if (obj.target_index == RaidTargetIndex::UNKNOWN0|| obj.target_index == RaidTargetIndex::UNKNOWN1|| obj.target_index == RaidTargetIndex::UNKNOWN2|| obj.target_index == RaidTargetIndex::UNKNOWN3|| obj.target_index == RaidTargetIndex::UNKNOWN4|| obj.target_index == RaidTargetIndex::UNKNOWN5|| obj.target_index == RaidTargetIndex::UNKNOWN6|| obj.target_index == RaidTargetIndex::UNKNOWN7|| obj.target_index == RaidTargetIndex::UNKNOWN8) {
        _size += 8;
    }

    return _size;
}

MSG_RAID_TARGET_UPDATE_Client MSG_RAID_TARGET_UPDATE_Client_read(Reader& reader) {
    MSG_RAID_TARGET_UPDATE_Client obj;

    obj.target_index = static_cast<RaidTargetIndex>(reader.read_u8());

    if (obj.target_index == RaidTargetIndex::UNKNOWN0|| obj.target_index == RaidTargetIndex::UNKNOWN1|| obj.target_index == RaidTargetIndex::UNKNOWN2|| obj.target_index == RaidTargetIndex::UNKNOWN3|| obj.target_index == RaidTargetIndex::UNKNOWN4|| obj.target_index == RaidTargetIndex::UNKNOWN5|| obj.target_index == RaidTargetIndex::UNKNOWN6|| obj.target_index == RaidTargetIndex::UNKNOWN7|| obj.target_index == RaidTargetIndex::UNKNOWN8) {
        obj.target = reader.read_u64();

    }
    return obj;
}

std::vector<unsigned char> MSG_RAID_TARGET_UPDATE_Client::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_RAID_TARGET_UPDATE_Client_size(obj));

    writer.write_u16_be((uint16_t)MSG_RAID_TARGET_UPDATE_Client_size(obj) + 4); /* size */

    writer.write_u32(0x00000321); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.target_index));

    if (obj.target_index == RaidTargetIndex::UNKNOWN0|| obj.target_index == RaidTargetIndex::UNKNOWN1|| obj.target_index == RaidTargetIndex::UNKNOWN2|| obj.target_index == RaidTargetIndex::UNKNOWN3|| obj.target_index == RaidTargetIndex::UNKNOWN4|| obj.target_index == RaidTargetIndex::UNKNOWN5|| obj.target_index == RaidTargetIndex::UNKNOWN6|| obj.target_index == RaidTargetIndex::UNKNOWN7|| obj.target_index == RaidTargetIndex::UNKNOWN8) {
        writer.write_u64(obj.target);

    }
    return writer.m_buf;
}

static size_t MSG_RAID_TARGET_UPDATE_Server_size(const MSG_RAID_TARGET_UPDATE_Server& obj) {
    size_t _size = 1;

    if (obj.update_type == RaidTargetUpdateType::FULL) {
        _size += 72;
    }
    else if (obj.update_type == RaidTargetUpdateType::PARTIAL) {
        _size += 9;
    }

    return _size;
}

MSG_RAID_TARGET_UPDATE_Server MSG_RAID_TARGET_UPDATE_Server_read(Reader& reader) {
    MSG_RAID_TARGET_UPDATE_Server obj;

    obj.update_type = static_cast<RaidTargetUpdateType>(reader.read_u8());

    if (obj.update_type == RaidTargetUpdateType::FULL) {
        for (auto i = 0; i < 8; ++i) {
            obj.raid_targets[i] = ::wow_world_messages::vanilla::RaidTargetUpdate_read(reader);
        }

    }
    else if (obj.update_type == RaidTargetUpdateType::PARTIAL) {
        obj.raid_target = ::wow_world_messages::vanilla::RaidTargetUpdate_read(reader);

    }
    return obj;
}

std::vector<unsigned char> MSG_RAID_TARGET_UPDATE_Server::write() const {
    const auto& obj = *this;
    auto writer = Writer(MSG_RAID_TARGET_UPDATE_Server_size(obj));

    writer.write_u16_be(MSG_RAID_TARGET_UPDATE_Server_size(obj) + 2); /* size */

    writer.write_u16(0x00000321); /* opcode */

    writer.write_u8(static_cast<uint8_t>(obj.update_type));

    if (obj.update_type == RaidTargetUpdateType::FULL) {
        for (const auto& v : obj.raid_targets) {
            RaidTargetUpdate_write(writer, v);
        }

    }
    else if (obj.update_type == RaidTargetUpdateType::PARTIAL) {
        RaidTargetUpdate_write(writer, obj.raid_target);

    }
    return writer.m_buf;
}

SMSG_PET_ACTION_SOUND SMSG_PET_ACTION_SOUND_read(Reader& reader) {
    SMSG_PET_ACTION_SOUND obj;

    obj.guid = reader.read_u64();

    obj.reason = static_cast<PetTalkReason>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_PET_ACTION_SOUND::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 2); /* size */

    writer.write_u16(0x00000324); /* opcode */

    writer.write_u64(obj.guid);

    writer.write_u32(static_cast<uint32_t>(obj.reason));

    return writer.m_buf;
}

SMSG_PET_DISMISS_SOUND SMSG_PET_DISMISS_SOUND_read(Reader& reader) {
    SMSG_PET_DISMISS_SOUND obj;

    obj.sound_id = reader.read_u32();

    obj.position = ::wow_world_messages::all::Vector3d_read(reader);

    return obj;
}

std::vector<unsigned char> SMSG_PET_DISMISS_SOUND::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0010);

    writer.write_u16_be(0x0010 + 2); /* size */

    writer.write_u16(0x00000325); /* opcode */

    writer.write_u32(obj.sound_id);

    Vector3d_write(writer, obj.position);

    return writer.m_buf;
}

SMSG_GM_TICKET_STATUS_UPDATE SMSG_GM_TICKET_STATUS_UPDATE_read(Reader& reader) {
    SMSG_GM_TICKET_STATUS_UPDATE obj;

    obj.response = static_cast<GmTicketStatusResponse>(reader.read_u32());

    return obj;
}

std::vector<unsigned char> SMSG_GM_TICKET_STATUS_UPDATE::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x00000328); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.response));

    return writer.m_buf;
}

static size_t CMSG_GMSURVEY_SUBMIT_size(const CMSG_GMSURVEY_SUBMIT& obj) {
    size_t _size = 5 + obj.answer_comment.size();

    for(const auto& v : obj.questions) {
        _size += GmSurveyQuestion_size(v);
    }

    return _size;
}

CMSG_GMSURVEY_SUBMIT CMSG_GMSURVEY_SUBMIT_read(Reader& reader) {
    CMSG_GMSURVEY_SUBMIT obj;

    obj.survey_id = reader.read_u32();

    for (auto i = 0; i < 10; ++i) {
        obj.questions[i] = ::wow_world_messages::vanilla::GmSurveyQuestion_read(reader);
    }

    obj.answer_comment = reader.read_cstring();

    return obj;
}

std::vector<unsigned char> CMSG_GMSURVEY_SUBMIT::write() const {
    const auto& obj = *this;
    auto writer = Writer(CMSG_GMSURVEY_SUBMIT_size(obj));

    writer.write_u16_be((uint16_t)CMSG_GMSURVEY_SUBMIT_size(obj) + 4); /* size */

    writer.write_u32(0x0000032a); /* opcode */

    writer.write_u32(obj.survey_id);

    for (const auto& v : obj.questions) {
        GmSurveyQuestion_write(writer, v);
    }

    writer.write_cstring(obj.answer_comment);

    return writer.m_buf;
}

SMSG_UPDATE_INSTANCE_OWNERSHIP SMSG_UPDATE_INSTANCE_OWNERSHIP_read(Reader& reader) {
    SMSG_UPDATE_INSTANCE_OWNERSHIP obj;

    obj.player_is_saved_to_a_raid = reader.read_bool32();

    return obj;
}

std::vector<unsigned char> SMSG_UPDATE_INSTANCE_OWNERSHIP::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x0004);

    writer.write_u16_be(0x0004 + 2); /* size */

    writer.write_u16(0x0000032b); /* opcode */

    writer.write_bool32(obj.player_is_saved_to_a_raid);

    return writer.m_buf;
}

SMSG_SPELLINSTAKILLLOG SMSG_SPELLINSTAKILLLOG_read(Reader& reader) {
    SMSG_SPELLINSTAKILLLOG obj;

    obj.target = reader.read_u64();

    obj.spell = reader.read_u32();

    return obj;
}

std::vector<unsigned char> SMSG_SPELLINSTAKILLLOG::write() const {
    const auto& obj = *this;
    auto writer = Writer(0x000c);

    writer.write_u16_be(0x000c + 2); /* size */

    writer.write_u16(0x0000032f); /* opcode */

    writer.write_u64(obj.target);

    writer.write_u32(obj.spell);

    return writer.m_buf;
}

static size_t SMSG_SPELL_UPDATE_CHAIN_TARGETS_size(const SMSG_SPELL_UPDATE_CHAIN_TARGETS& obj) {
    return 16 + 8 * obj.targets.size();
}

SMSG_SPELL_UPDATE_CHAIN_TARGETS SMSG_SPELL_UPDATE_CHAIN_TARGETS_read(Reader& reader) {
    SMSG_SPELL_UPDATE_CHAIN_TARGETS obj;

    obj.caster = reader.read_u64();

    obj.spell = reader.read_u32();

    obj.amount_of_targets = reader.read_u32();

    for (auto i = 0; i < obj.amount_of_targets; ++i) {
        obj.targets.push_back(reader.read_u64());
    }

    return obj;
}

std::vector<unsigned char> SMSG_SPELL_UPDATE_CHAIN_TARGETS::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_SPELL_UPDATE_CHAIN_TARGETS_size(obj));

    writer.write_u16_be(SMSG_SPELL_UPDATE_CHAIN_TARGETS_size(obj) + 2); /* size */

    writer.write_u16(0x00000330); /* opcode */

    writer.write_u64(obj.caster);

    writer.write_u32(obj.spell);

    writer.write_u32(obj.amount_of_targets);

    for (const auto& v : obj.targets) {
        writer.write_u64(v);
    }

    return writer.m_buf;
}

static size_t SMSG_EXPECTED_SPAM_RECORDS_size(const SMSG_EXPECTED_SPAM_RECORDS& obj) {
    size_t _size = 4;

    for(const auto& v : obj.records) {
        _size += v.size() + 1;
    }

    return _size;
}

SMSG_EXPECTED_SPAM_RECORDS SMSG_EXPECTED_SPAM_RECORDS_read(Reader& reader) {
    SMSG_EXPECTED_SPAM_RECORDS obj;

    obj.amount_of_records = reader.read_u32();

    for (auto i = 0; i < obj.amount_of_records; ++i) {
        obj.records.push_back(reader.read_cstring());
    }

    return obj;
}

std::vector<unsigned char> SMSG_EXPECTED_SPAM_RECORDS::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_EXPECTED_SPAM_RECORDS_size(obj));

    writer.write_u16_be(SMSG_EXPECTED_SPAM_RECORDS_size(obj) + 2); /* size */

    writer.write_u16(0x00000332); /* opcode */

    writer.write_u32(obj.amount_of_records);

    for (const auto& v : obj.records) {
        writer.write_cstring(v);
    }

    return writer.m_buf;
}

static size_t SMSG_DEFENSE_MESSAGE_size(const SMSG_DEFENSE_MESSAGE& obj) {
    return 9 + obj.message.size();
}

SMSG_DEFENSE_MESSAGE SMSG_DEFENSE_MESSAGE_read(Reader& reader) {
    SMSG_DEFENSE_MESSAGE obj;

    obj.area = static_cast<Area>(reader.read_u32());

    obj.message = reader.read_sized_cstring();

    return obj;
}

std::vector<unsigned char> SMSG_DEFENSE_MESSAGE::write() const {
    const auto& obj = *this;
    auto writer = Writer(SMSG_DEFENSE_MESSAGE_size(obj));

    writer.write_u16_be(SMSG_DEFENSE_MESSAGE_size(obj) + 2); /* size */

    writer.write_u16(0x0000033b); /* opcode */

    writer.write_u32(static_cast<uint32_t>(obj.area));

    writer.write_sized_cstring(obj.message);

    return writer.m_buf;
}

std::vector<unsigned char> write_opcode(const ClientOpcode& opcode) {
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_BOOTME) {
        return opcode.CMSG_BOOTME.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_DBLOOKUP) {
        return opcode.CMSG_DBLOOKUP.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_WORLD_TELEPORT) {
        return opcode.CMSG_WORLD_TELEPORT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_TELEPORT_TO_UNIT) {
        return opcode.CMSG_TELEPORT_TO_UNIT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_CHAR_CREATE) {
        return opcode.CMSG_CHAR_CREATE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_CHAR_ENUM) {
        return opcode.CMSG_CHAR_ENUM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_CHAR_DELETE) {
        return opcode.CMSG_CHAR_DELETE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_PLAYER_LOGIN) {
        return opcode.CMSG_PLAYER_LOGIN.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_PLAYER_LOGOUT) {
        return opcode.CMSG_PLAYER_LOGOUT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_LOGOUT_REQUEST) {
        return opcode.CMSG_LOGOUT_REQUEST.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_LOGOUT_CANCEL) {
        return opcode.CMSG_LOGOUT_CANCEL.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_NAME_QUERY) {
        return opcode.CMSG_NAME_QUERY.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_PET_NAME_QUERY) {
        return opcode.CMSG_PET_NAME_QUERY.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GUILD_QUERY) {
        return opcode.CMSG_GUILD_QUERY.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_ITEM_QUERY_SINGLE) {
        return opcode.CMSG_ITEM_QUERY_SINGLE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_PAGE_TEXT_QUERY) {
        return opcode.CMSG_PAGE_TEXT_QUERY.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_QUEST_QUERY) {
        return opcode.CMSG_QUEST_QUERY.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GAMEOBJECT_QUERY) {
        return opcode.CMSG_GAMEOBJECT_QUERY.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_CREATURE_QUERY) {
        return opcode.CMSG_CREATURE_QUERY.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_WHO) {
        return opcode.CMSG_WHO.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_WHOIS) {
        return opcode.CMSG_WHOIS.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_FRIEND_LIST) {
        return opcode.CMSG_FRIEND_LIST.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_ADD_FRIEND) {
        return opcode.CMSG_ADD_FRIEND.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_DEL_FRIEND) {
        return opcode.CMSG_DEL_FRIEND.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_ADD_IGNORE) {
        return opcode.CMSG_ADD_IGNORE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_DEL_IGNORE) {
        return opcode.CMSG_DEL_IGNORE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GROUP_INVITE) {
        return opcode.CMSG_GROUP_INVITE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GROUP_ACCEPT) {
        return opcode.CMSG_GROUP_ACCEPT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GROUP_DECLINE) {
        return opcode.CMSG_GROUP_DECLINE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GROUP_UNINVITE) {
        return opcode.CMSG_GROUP_UNINVITE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GROUP_UNINVITE_GUID) {
        return opcode.CMSG_GROUP_UNINVITE_GUID.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GROUP_SET_LEADER) {
        return opcode.CMSG_GROUP_SET_LEADER.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_LOOT_METHOD) {
        return opcode.CMSG_LOOT_METHOD.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GROUP_DISBAND) {
        return opcode.CMSG_GROUP_DISBAND.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GUILD_CREATE) {
        return opcode.CMSG_GUILD_CREATE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GUILD_INVITE) {
        return opcode.CMSG_GUILD_INVITE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GUILD_ACCEPT) {
        return opcode.CMSG_GUILD_ACCEPT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GUILD_DECLINE) {
        return opcode.CMSG_GUILD_DECLINE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GUILD_INFO) {
        return opcode.CMSG_GUILD_INFO.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GUILD_ROSTER) {
        return opcode.CMSG_GUILD_ROSTER.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GUILD_PROMOTE) {
        return opcode.CMSG_GUILD_PROMOTE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GUILD_DEMOTE) {
        return opcode.CMSG_GUILD_DEMOTE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GUILD_LEAVE) {
        return opcode.CMSG_GUILD_LEAVE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GUILD_REMOVE) {
        return opcode.CMSG_GUILD_REMOVE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GUILD_DISBAND) {
        return opcode.CMSG_GUILD_DISBAND.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GUILD_LEADER) {
        return opcode.CMSG_GUILD_LEADER.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GUILD_MOTD) {
        return opcode.CMSG_GUILD_MOTD.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_MESSAGECHAT) {
        return opcode.CMSG_MESSAGECHAT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_JOIN_CHANNEL) {
        return opcode.CMSG_JOIN_CHANNEL.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_LEAVE_CHANNEL) {
        return opcode.CMSG_LEAVE_CHANNEL.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_CHANNEL_LIST) {
        return opcode.CMSG_CHANNEL_LIST.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_CHANNEL_PASSWORD) {
        return opcode.CMSG_CHANNEL_PASSWORD.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_CHANNEL_SET_OWNER) {
        return opcode.CMSG_CHANNEL_SET_OWNER.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_CHANNEL_OWNER) {
        return opcode.CMSG_CHANNEL_OWNER.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_CHANNEL_MODERATOR) {
        return opcode.CMSG_CHANNEL_MODERATOR.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_CHANNEL_UNMODERATOR) {
        return opcode.CMSG_CHANNEL_UNMODERATOR.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_CHANNEL_MUTE) {
        return opcode.CMSG_CHANNEL_MUTE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_CHANNEL_UNMUTE) {
        return opcode.CMSG_CHANNEL_UNMUTE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_CHANNEL_INVITE) {
        return opcode.CMSG_CHANNEL_INVITE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_CHANNEL_KICK) {
        return opcode.CMSG_CHANNEL_KICK.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_CHANNEL_BAN) {
        return opcode.CMSG_CHANNEL_BAN.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_CHANNEL_UNBAN) {
        return opcode.CMSG_CHANNEL_UNBAN.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_CHANNEL_ANNOUNCEMENTS) {
        return opcode.CMSG_CHANNEL_ANNOUNCEMENTS.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_CHANNEL_MODERATE) {
        return opcode.CMSG_CHANNEL_MODERATE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_USE_ITEM) {
        return opcode.CMSG_USE_ITEM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_OPEN_ITEM) {
        return opcode.CMSG_OPEN_ITEM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_READ_ITEM) {
        return opcode.CMSG_READ_ITEM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GAMEOBJ_USE) {
        return opcode.CMSG_GAMEOBJ_USE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_AREATRIGGER) {
        return opcode.CMSG_AREATRIGGER.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_MOVE_START_FORWARD) {
        return opcode.MSG_MOVE_START_FORWARD.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_MOVE_START_BACKWARD) {
        return opcode.MSG_MOVE_START_BACKWARD.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_MOVE_STOP) {
        return opcode.MSG_MOVE_STOP.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_MOVE_START_STRAFE_LEFT) {
        return opcode.MSG_MOVE_START_STRAFE_LEFT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_MOVE_START_STRAFE_RIGHT) {
        return opcode.MSG_MOVE_START_STRAFE_RIGHT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_MOVE_STOP_STRAFE) {
        return opcode.MSG_MOVE_STOP_STRAFE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_MOVE_JUMP) {
        return opcode.MSG_MOVE_JUMP.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_MOVE_START_TURN_LEFT) {
        return opcode.MSG_MOVE_START_TURN_LEFT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_MOVE_START_TURN_RIGHT) {
        return opcode.MSG_MOVE_START_TURN_RIGHT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_MOVE_STOP_TURN) {
        return opcode.MSG_MOVE_STOP_TURN.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_MOVE_START_PITCH_UP) {
        return opcode.MSG_MOVE_START_PITCH_UP.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_MOVE_START_PITCH_DOWN) {
        return opcode.MSG_MOVE_START_PITCH_DOWN.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_MOVE_STOP_PITCH) {
        return opcode.MSG_MOVE_STOP_PITCH.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_MOVE_SET_RUN_MODE) {
        return opcode.MSG_MOVE_SET_RUN_MODE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_MOVE_SET_WALK_MODE) {
        return opcode.MSG_MOVE_SET_WALK_MODE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_MOVE_TELEPORT_ACK) {
        return opcode.MSG_MOVE_TELEPORT_ACK.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_MOVE_FALL_LAND) {
        return opcode.MSG_MOVE_FALL_LAND.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_MOVE_START_SWIM) {
        return opcode.MSG_MOVE_START_SWIM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_MOVE_STOP_SWIM) {
        return opcode.MSG_MOVE_STOP_SWIM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_MOVE_SET_FACING) {
        return opcode.MSG_MOVE_SET_FACING.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_MOVE_SET_PITCH) {
        return opcode.MSG_MOVE_SET_PITCH.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_MOVE_SET_RAW_POSITION) {
        return opcode.CMSG_MOVE_SET_RAW_POSITION.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_FORCE_RUN_SPEED_CHANGE_ACK) {
        return opcode.CMSG_FORCE_RUN_SPEED_CHANGE_ACK.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK) {
        return opcode.CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_FORCE_SWIM_SPEED_CHANGE_ACK) {
        return opcode.CMSG_FORCE_SWIM_SPEED_CHANGE_ACK.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_FORCE_MOVE_ROOT_ACK) {
        return opcode.CMSG_FORCE_MOVE_ROOT_ACK.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_FORCE_MOVE_UNROOT_ACK) {
        return opcode.CMSG_FORCE_MOVE_UNROOT_ACK.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_MOVE_HEARTBEAT) {
        return opcode.MSG_MOVE_HEARTBEAT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_MOVE_KNOCK_BACK_ACK) {
        return opcode.CMSG_MOVE_KNOCK_BACK_ACK.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_MOVE_HOVER_ACK) {
        return opcode.CMSG_MOVE_HOVER_ACK.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_NEXT_CINEMATIC_CAMERA) {
        return opcode.CMSG_NEXT_CINEMATIC_CAMERA.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_COMPLETE_CINEMATIC) {
        return opcode.CMSG_COMPLETE_CINEMATIC.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_TUTORIAL_FLAG) {
        return opcode.CMSG_TUTORIAL_FLAG.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_TUTORIAL_CLEAR) {
        return opcode.CMSG_TUTORIAL_CLEAR.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_TUTORIAL_RESET) {
        return opcode.CMSG_TUTORIAL_RESET.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_STANDSTATECHANGE) {
        return opcode.CMSG_STANDSTATECHANGE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_EMOTE) {
        return opcode.CMSG_EMOTE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_TEXT_EMOTE) {
        return opcode.CMSG_TEXT_EMOTE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_AUTOSTORE_LOOT_ITEM) {
        return opcode.CMSG_AUTOSTORE_LOOT_ITEM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_AUTOEQUIP_ITEM) {
        return opcode.CMSG_AUTOEQUIP_ITEM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_AUTOSTORE_BAG_ITEM) {
        return opcode.CMSG_AUTOSTORE_BAG_ITEM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_SWAP_ITEM) {
        return opcode.CMSG_SWAP_ITEM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_SWAP_INV_ITEM) {
        return opcode.CMSG_SWAP_INV_ITEM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_SPLIT_ITEM) {
        return opcode.CMSG_SPLIT_ITEM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_AUTOEQUIP_ITEM_SLOT) {
        return opcode.CMSG_AUTOEQUIP_ITEM_SLOT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_DESTROYITEM) {
        return opcode.CMSG_DESTROYITEM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_INSPECT) {
        return opcode.CMSG_INSPECT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_INITIATE_TRADE) {
        return opcode.CMSG_INITIATE_TRADE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_BEGIN_TRADE) {
        return opcode.CMSG_BEGIN_TRADE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_BUSY_TRADE) {
        return opcode.CMSG_BUSY_TRADE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_IGNORE_TRADE) {
        return opcode.CMSG_IGNORE_TRADE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_ACCEPT_TRADE) {
        return opcode.CMSG_ACCEPT_TRADE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_UNACCEPT_TRADE) {
        return opcode.CMSG_UNACCEPT_TRADE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_CANCEL_TRADE) {
        return opcode.CMSG_CANCEL_TRADE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_SET_TRADE_ITEM) {
        return opcode.CMSG_SET_TRADE_ITEM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_CLEAR_TRADE_ITEM) {
        return opcode.CMSG_CLEAR_TRADE_ITEM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_SET_TRADE_GOLD) {
        return opcode.CMSG_SET_TRADE_GOLD.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_SET_FACTION_ATWAR) {
        return opcode.CMSG_SET_FACTION_ATWAR.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_SET_ACTION_BUTTON) {
        return opcode.CMSG_SET_ACTION_BUTTON.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_CAST_SPELL) {
        return opcode.CMSG_CAST_SPELL.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_CANCEL_CAST) {
        return opcode.CMSG_CANCEL_CAST.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_CANCEL_AURA) {
        return opcode.CMSG_CANCEL_AURA.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_CANCEL_CHANNELLING) {
        return opcode.CMSG_CANCEL_CHANNELLING.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_SET_SELECTION) {
        return opcode.CMSG_SET_SELECTION.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_SET_TARGET_OBSOLETE) {
        return opcode.CMSG_SET_TARGET_OBSOLETE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_ATTACKSWING) {
        return opcode.CMSG_ATTACKSWING.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_ATTACKSTOP) {
        return opcode.CMSG_ATTACKSTOP.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_REPOP_REQUEST) {
        return opcode.CMSG_REPOP_REQUEST.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_RESURRECT_RESPONSE) {
        return opcode.CMSG_RESURRECT_RESPONSE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_LOOT) {
        return opcode.CMSG_LOOT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_LOOT_MONEY) {
        return opcode.CMSG_LOOT_MONEY.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_LOOT_RELEASE) {
        return opcode.CMSG_LOOT_RELEASE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_DUEL_ACCEPTED) {
        return opcode.CMSG_DUEL_ACCEPTED.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_DUEL_CANCELLED) {
        return opcode.CMSG_DUEL_CANCELLED.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_MOUNTSPECIAL_ANIM) {
        return opcode.CMSG_MOUNTSPECIAL_ANIM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_PET_ACTION) {
        return opcode.CMSG_PET_ACTION.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_PET_ABANDON) {
        return opcode.CMSG_PET_ABANDON.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_PET_RENAME) {
        return opcode.CMSG_PET_RENAME.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GOSSIP_HELLO) {
        return opcode.CMSG_GOSSIP_HELLO.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_NPC_TEXT_QUERY) {
        return opcode.CMSG_NPC_TEXT_QUERY.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_QUESTGIVER_STATUS_QUERY) {
        return opcode.CMSG_QUESTGIVER_STATUS_QUERY.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_QUESTGIVER_HELLO) {
        return opcode.CMSG_QUESTGIVER_HELLO.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_QUESTGIVER_QUERY_QUEST) {
        return opcode.CMSG_QUESTGIVER_QUERY_QUEST.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_QUESTGIVER_QUEST_AUTOLAUNCH) {
        return opcode.CMSG_QUESTGIVER_QUEST_AUTOLAUNCH.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_QUESTGIVER_ACCEPT_QUEST) {
        return opcode.CMSG_QUESTGIVER_ACCEPT_QUEST.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_QUESTGIVER_COMPLETE_QUEST) {
        return opcode.CMSG_QUESTGIVER_COMPLETE_QUEST.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_QUESTGIVER_REQUEST_REWARD) {
        return opcode.CMSG_QUESTGIVER_REQUEST_REWARD.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_QUESTGIVER_CHOOSE_REWARD) {
        return opcode.CMSG_QUESTGIVER_CHOOSE_REWARD.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_QUESTGIVER_CANCEL) {
        return opcode.CMSG_QUESTGIVER_CANCEL.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_QUESTLOG_SWAP_QUEST) {
        return opcode.CMSG_QUESTLOG_SWAP_QUEST.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_QUESTLOG_REMOVE_QUEST) {
        return opcode.CMSG_QUESTLOG_REMOVE_QUEST.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_QUEST_CONFIRM_ACCEPT) {
        return opcode.CMSG_QUEST_CONFIRM_ACCEPT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_PUSHQUESTTOPARTY) {
        return opcode.CMSG_PUSHQUESTTOPARTY.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_LIST_INVENTORY) {
        return opcode.CMSG_LIST_INVENTORY.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_SELL_ITEM) {
        return opcode.CMSG_SELL_ITEM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_BUY_ITEM) {
        return opcode.CMSG_BUY_ITEM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_BUY_ITEM_IN_SLOT) {
        return opcode.CMSG_BUY_ITEM_IN_SLOT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_TAXINODE_STATUS_QUERY) {
        return opcode.CMSG_TAXINODE_STATUS_QUERY.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_TAXIQUERYAVAILABLENODES) {
        return opcode.CMSG_TAXIQUERYAVAILABLENODES.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_ACTIVATETAXI) {
        return opcode.CMSG_ACTIVATETAXI.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_TRAINER_LIST) {
        return opcode.CMSG_TRAINER_LIST.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_TRAINER_BUY_SPELL) {
        return opcode.CMSG_TRAINER_BUY_SPELL.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_BINDER_ACTIVATE) {
        return opcode.CMSG_BINDER_ACTIVATE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_BANKER_ACTIVATE) {
        return opcode.CMSG_BANKER_ACTIVATE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_BUY_BANK_SLOT) {
        return opcode.CMSG_BUY_BANK_SLOT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_PETITION_SHOWLIST) {
        return opcode.CMSG_PETITION_SHOWLIST.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_PETITION_BUY) {
        return opcode.CMSG_PETITION_BUY.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_PETITION_SHOW_SIGNATURES) {
        return opcode.CMSG_PETITION_SHOW_SIGNATURES.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_PETITION_SIGN) {
        return opcode.CMSG_PETITION_SIGN.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_OFFER_PETITION) {
        return opcode.CMSG_OFFER_PETITION.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_TURN_IN_PETITION) {
        return opcode.CMSG_TURN_IN_PETITION.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_PETITION_QUERY) {
        return opcode.CMSG_PETITION_QUERY.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_BUG) {
        return opcode.CMSG_BUG.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_PLAYED_TIME) {
        return opcode.CMSG_PLAYED_TIME.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_QUERY_TIME) {
        return opcode.CMSG_QUERY_TIME.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_RECLAIM_CORPSE) {
        return opcode.CMSG_RECLAIM_CORPSE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_WRAP_ITEM) {
        return opcode.CMSG_WRAP_ITEM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_MINIMAP_PING) {
        return opcode.MSG_MINIMAP_PING.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_PING) {
        return opcode.CMSG_PING.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_SETSHEATHED) {
        return opcode.CMSG_SETSHEATHED.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_PET_CAST_SPELL) {
        return opcode.CMSG_PET_CAST_SPELL.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_SAVE_GUILD_EMBLEM) {
        return opcode.MSG_SAVE_GUILD_EMBLEM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_ZONEUPDATE) {
        return opcode.CMSG_ZONEUPDATE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_RANDOM_ROLL) {
        return opcode.MSG_RANDOM_ROLL.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_LOOKING_FOR_GROUP) {
        return opcode.MSG_LOOKING_FOR_GROUP.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_UNLEARN_SKILL) {
        return opcode.CMSG_UNLEARN_SKILL.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GMTICKET_UPDATETEXT) {
        return opcode.CMSG_GMTICKET_UPDATETEXT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_REQUEST_ACCOUNT_DATA) {
        return opcode.CMSG_REQUEST_ACCOUNT_DATA.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GMTICKET_GETTICKET) {
        return opcode.CMSG_GMTICKET_GETTICKET.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_CORPSE_QUERY) {
        return opcode.MSG_CORPSE_QUERY.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GMTICKET_DELETETICKET) {
        return opcode.CMSG_GMTICKET_DELETETICKET.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GMTICKET_SYSTEMSTATUS) {
        return opcode.CMSG_GMTICKET_SYSTEMSTATUS.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_SPIRIT_HEALER_ACTIVATE) {
        return opcode.CMSG_SPIRIT_HEALER_ACTIVATE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_CHAT_IGNORED) {
        return opcode.CMSG_CHAT_IGNORED.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GUILD_RANK) {
        return opcode.CMSG_GUILD_RANK.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GUILD_ADD_RANK) {
        return opcode.CMSG_GUILD_ADD_RANK.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GUILD_DEL_RANK) {
        return opcode.CMSG_GUILD_DEL_RANK.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GUILD_SET_PUBLIC_NOTE) {
        return opcode.CMSG_GUILD_SET_PUBLIC_NOTE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GUILD_SET_OFFICER_NOTE) {
        return opcode.CMSG_GUILD_SET_OFFICER_NOTE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_SEND_MAIL) {
        return opcode.CMSG_SEND_MAIL.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GET_MAIL_LIST) {
        return opcode.CMSG_GET_MAIL_LIST.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_BATTLEFIELD_LIST) {
        return opcode.CMSG_BATTLEFIELD_LIST.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_BATTLEFIELD_JOIN) {
        return opcode.CMSG_BATTLEFIELD_JOIN.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_ITEM_TEXT_QUERY) {
        return opcode.CMSG_ITEM_TEXT_QUERY.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_MAIL_TAKE_MONEY) {
        return opcode.CMSG_MAIL_TAKE_MONEY.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_MAIL_TAKE_ITEM) {
        return opcode.CMSG_MAIL_TAKE_ITEM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_MAIL_MARK_AS_READ) {
        return opcode.CMSG_MAIL_MARK_AS_READ.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_MAIL_RETURN_TO_SENDER) {
        return opcode.CMSG_MAIL_RETURN_TO_SENDER.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_MAIL_DELETE) {
        return opcode.CMSG_MAIL_DELETE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_MAIL_CREATE_TEXT_ITEM) {
        return opcode.CMSG_MAIL_CREATE_TEXT_ITEM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_LEARN_TALENT) {
        return opcode.CMSG_LEARN_TALENT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_AUCTION_HELLO) {
        return opcode.MSG_AUCTION_HELLO.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_AUCTION_SELL_ITEM) {
        return opcode.CMSG_AUCTION_SELL_ITEM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_AUCTION_REMOVE_ITEM) {
        return opcode.CMSG_AUCTION_REMOVE_ITEM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_AUCTION_LIST_ITEMS) {
        return opcode.CMSG_AUCTION_LIST_ITEMS.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_AUCTION_LIST_OWNER_ITEMS) {
        return opcode.CMSG_AUCTION_LIST_OWNER_ITEMS.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_AUCTION_PLACE_BID) {
        return opcode.CMSG_AUCTION_PLACE_BID.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_AUCTION_LIST_BIDDER_ITEMS) {
        return opcode.CMSG_AUCTION_LIST_BIDDER_ITEMS.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_SET_AMMO) {
        return opcode.CMSG_SET_AMMO.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_SET_ACTIVE_MOVER) {
        return opcode.CMSG_SET_ACTIVE_MOVER.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_PET_CANCEL_AURA) {
        return opcode.CMSG_PET_CANCEL_AURA.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_CANCEL_AUTO_REPEAT_SPELL) {
        return opcode.CMSG_CANCEL_AUTO_REPEAT_SPELL.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_LIST_STABLED_PETS) {
        return opcode.MSG_LIST_STABLED_PETS.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_STABLE_PET) {
        return opcode.CMSG_STABLE_PET.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_UNSTABLE_PET) {
        return opcode.CMSG_UNSTABLE_PET.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_BUY_STABLE_SLOT) {
        return opcode.CMSG_BUY_STABLE_SLOT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_STABLE_SWAP_PET) {
        return opcode.CMSG_STABLE_SWAP_PET.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_REQUEST_PET_INFO) {
        return opcode.CMSG_REQUEST_PET_INFO.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_FAR_SIGHT) {
        return opcode.CMSG_FAR_SIGHT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GROUP_CHANGE_SUB_GROUP) {
        return opcode.CMSG_GROUP_CHANGE_SUB_GROUP.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_REQUEST_PARTY_MEMBER_STATS) {
        return opcode.CMSG_REQUEST_PARTY_MEMBER_STATS.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GROUP_SWAP_SUB_GROUP) {
        return opcode.CMSG_GROUP_SWAP_SUB_GROUP.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_AUTOSTORE_BANK_ITEM) {
        return opcode.CMSG_AUTOSTORE_BANK_ITEM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_AUTOBANK_ITEM) {
        return opcode.CMSG_AUTOBANK_ITEM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_QUERY_NEXT_MAIL_TIME) {
        return opcode.MSG_QUERY_NEXT_MAIL_TIME.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GROUP_RAID_CONVERT) {
        return opcode.CMSG_GROUP_RAID_CONVERT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GROUP_ASSISTANT_LEADER) {
        return opcode.CMSG_GROUP_ASSISTANT_LEADER.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_BUYBACK_ITEM) {
        return opcode.CMSG_BUYBACK_ITEM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_MEETINGSTONE_JOIN) {
        return opcode.CMSG_MEETINGSTONE_JOIN.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_MEETINGSTONE_LEAVE) {
        return opcode.CMSG_MEETINGSTONE_LEAVE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_MEETINGSTONE_INFO) {
        return opcode.CMSG_MEETINGSTONE_INFO.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_CANCEL_GROWTH_AURA) {
        return opcode.CMSG_CANCEL_GROWTH_AURA.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_LOOT_ROLL) {
        return opcode.CMSG_LOOT_ROLL.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_LOOT_MASTER_GIVE) {
        return opcode.CMSG_LOOT_MASTER_GIVE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_REPAIR_ITEM) {
        return opcode.CMSG_REPAIR_ITEM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_TALENT_WIPE_CONFIRM) {
        return opcode.MSG_TALENT_WIPE_CONFIRM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_SUMMON_RESPONSE) {
        return opcode.CMSG_SUMMON_RESPONSE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_SELF_RES) {
        return opcode.CMSG_SELF_RES.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_TOGGLE_HELM) {
        return opcode.CMSG_TOGGLE_HELM.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_TOGGLE_CLOAK) {
        return opcode.CMSG_TOGGLE_CLOAK.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_SET_ACTIONBAR_TOGGLES) {
        return opcode.CMSG_SET_ACTIONBAR_TOGGLES.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_ITEM_NAME_QUERY) {
        return opcode.CMSG_ITEM_NAME_QUERY.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_CHAR_RENAME) {
        return opcode.CMSG_CHAR_RENAME.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_MOVE_SPLINE_DONE) {
        return opcode.CMSG_MOVE_SPLINE_DONE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_MOVE_FALL_RESET) {
        return opcode.CMSG_MOVE_FALL_RESET.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_REQUEST_RAID_INFO) {
        return opcode.CMSG_REQUEST_RAID_INFO.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_MOVE_TIME_SKIPPED) {
        return opcode.CMSG_MOVE_TIME_SKIPPED.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_MOVE_FEATHER_FALL_ACK) {
        return opcode.CMSG_MOVE_FEATHER_FALL_ACK.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_MOVE_WATER_WALK_ACK) {
        return opcode.CMSG_MOVE_WATER_WALK_ACK.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_MOVE_NOT_ACTIVE_MOVER) {
        return opcode.CMSG_MOVE_NOT_ACTIVE_MOVER.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_BATTLEFIELD_STATUS) {
        return opcode.CMSG_BATTLEFIELD_STATUS.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_BATTLEFIELD_PORT) {
        return opcode.CMSG_BATTLEFIELD_PORT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_INSPECT_HONOR_STATS) {
        return opcode.MSG_INSPECT_HONOR_STATS.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_BATTLEMASTER_HELLO) {
        return opcode.CMSG_BATTLEMASTER_HELLO.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_FORCE_WALK_SPEED_CHANGE_ACK) {
        return opcode.CMSG_FORCE_WALK_SPEED_CHANGE_ACK.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK) {
        return opcode.CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_FORCE_TURN_RATE_CHANGE_ACK) {
        return opcode.CMSG_FORCE_TURN_RATE_CHANGE_ACK.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_PVP_LOG_DATA) {
        return opcode.MSG_PVP_LOG_DATA.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_LEAVE_BATTLEFIELD) {
        return opcode.CMSG_LEAVE_BATTLEFIELD.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_AREA_SPIRIT_HEALER_QUERY) {
        return opcode.CMSG_AREA_SPIRIT_HEALER_QUERY.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_AREA_SPIRIT_HEALER_QUEUE) {
        return opcode.CMSG_AREA_SPIRIT_HEALER_QUEUE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_WARDEN_DATA) {
        return opcode.CMSG_WARDEN_DATA.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_BATTLEGROUND_PLAYER_POSITIONS) {
        return opcode.MSG_BATTLEGROUND_PLAYER_POSITIONS.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_PET_STOP_ATTACK) {
        return opcode.CMSG_PET_STOP_ATTACK.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_BATTLEMASTER_JOIN) {
        return opcode.CMSG_BATTLEMASTER_JOIN.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_PET_UNLEARN) {
        return opcode.CMSG_PET_UNLEARN.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_PET_SPELL_AUTOCAST) {
        return opcode.CMSG_PET_SPELL_AUTOCAST.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GUILD_INFO_TEXT) {
        return opcode.CMSG_GUILD_INFO_TEXT.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_ACTIVATETAXIEXPRESS) {
        return opcode.CMSG_ACTIVATETAXIEXPRESS.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_SET_FACTION_INACTIVE) {
        return opcode.CMSG_SET_FACTION_INACTIVE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_SET_WATCHED_FACTION) {
        return opcode.CMSG_SET_WATCHED_FACTION.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_RESET_INSTANCES) {
        return opcode.CMSG_RESET_INSTANCES.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::MSG_RAID_TARGET_UPDATE) {
        return opcode.MSG_RAID_TARGET_UPDATE.write();;
    }
    if (opcode.opcode == ClientOpcode::Opcode::CMSG_GMSURVEY_SUBMIT) {
        return opcode.CMSG_GMSURVEY_SUBMIT.write();;
    }

    return {}; /* unreachable */
}

ClientOpcode read_client_opcode(Reader& reader) {
    const uint16_t _size = reader.read_u16();
    const uint32_t opcode = reader.read_u32();

    ClientOpcode op;

    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_BOOTME)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_BOOTME{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_DBLOOKUP)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_DBLOOKUP_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_WORLD_TELEPORT)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_WORLD_TELEPORT_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_TELEPORT_TO_UNIT)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_TELEPORT_TO_UNIT_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_CHAR_CREATE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_CHAR_CREATE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_CHAR_ENUM)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_CHAR_ENUM{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_CHAR_DELETE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_CHAR_DELETE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_PLAYER_LOGIN)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_PLAYER_LOGIN_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_PLAYER_LOGOUT)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_PLAYER_LOGOUT{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_LOGOUT_REQUEST)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_LOGOUT_REQUEST{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_LOGOUT_CANCEL)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_LOGOUT_CANCEL{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_NAME_QUERY)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_NAME_QUERY_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_PET_NAME_QUERY)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_PET_NAME_QUERY_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GUILD_QUERY)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GUILD_QUERY_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_ITEM_QUERY_SINGLE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_ITEM_QUERY_SINGLE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_PAGE_TEXT_QUERY)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_PAGE_TEXT_QUERY_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_QUEST_QUERY)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_QUEST_QUERY_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GAMEOBJECT_QUERY)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GAMEOBJECT_QUERY_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_CREATURE_QUERY)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_CREATURE_QUERY_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_WHO)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_WHO_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_WHOIS)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_WHOIS_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_FRIEND_LIST)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_FRIEND_LIST{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_ADD_FRIEND)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_ADD_FRIEND_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_DEL_FRIEND)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_DEL_FRIEND_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_ADD_IGNORE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_ADD_IGNORE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_DEL_IGNORE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_DEL_IGNORE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GROUP_INVITE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GROUP_INVITE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GROUP_ACCEPT)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GROUP_ACCEPT{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GROUP_DECLINE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GROUP_DECLINE{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GROUP_UNINVITE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GROUP_UNINVITE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GROUP_UNINVITE_GUID)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GROUP_UNINVITE_GUID_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GROUP_SET_LEADER)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GROUP_SET_LEADER_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_LOOT_METHOD)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_LOOT_METHOD_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GROUP_DISBAND)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GROUP_DISBAND{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GUILD_CREATE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GUILD_CREATE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GUILD_INVITE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GUILD_INVITE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GUILD_ACCEPT)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GUILD_ACCEPT{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GUILD_DECLINE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GUILD_DECLINE{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GUILD_INFO)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GUILD_INFO{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GUILD_ROSTER)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GUILD_ROSTER{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GUILD_PROMOTE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GUILD_PROMOTE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GUILD_DEMOTE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GUILD_DEMOTE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GUILD_LEAVE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GUILD_LEAVE{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GUILD_REMOVE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GUILD_REMOVE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GUILD_DISBAND)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GUILD_DISBAND{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GUILD_LEADER)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GUILD_LEADER_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GUILD_MOTD)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GUILD_MOTD_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_MESSAGECHAT)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_MESSAGECHAT_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_JOIN_CHANNEL)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_JOIN_CHANNEL_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_LEAVE_CHANNEL)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_LEAVE_CHANNEL_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_CHANNEL_LIST)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_CHANNEL_LIST_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_CHANNEL_PASSWORD)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_CHANNEL_PASSWORD_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_CHANNEL_SET_OWNER)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_CHANNEL_SET_OWNER_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_CHANNEL_OWNER)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_CHANNEL_OWNER_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_CHANNEL_MODERATOR)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_CHANNEL_MODERATOR_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_CHANNEL_UNMODERATOR)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_CHANNEL_UNMODERATOR_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_CHANNEL_MUTE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_CHANNEL_MUTE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_CHANNEL_UNMUTE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_CHANNEL_UNMUTE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_CHANNEL_INVITE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_CHANNEL_INVITE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_CHANNEL_KICK)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_CHANNEL_KICK_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_CHANNEL_BAN)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_CHANNEL_BAN_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_CHANNEL_UNBAN)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_CHANNEL_UNBAN_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_CHANNEL_ANNOUNCEMENTS)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_CHANNEL_ANNOUNCEMENTS_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_CHANNEL_MODERATE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_CHANNEL_MODERATE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_USE_ITEM)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_USE_ITEM_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_OPEN_ITEM)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_OPEN_ITEM_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_READ_ITEM)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_READ_ITEM_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GAMEOBJ_USE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GAMEOBJ_USE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_AREATRIGGER)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_AREATRIGGER_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_MOVE_START_FORWARD)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_MOVE_START_FORWARD_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_MOVE_START_BACKWARD)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_MOVE_START_BACKWARD_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_MOVE_STOP)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_MOVE_STOP_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_MOVE_START_STRAFE_LEFT)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_MOVE_START_STRAFE_LEFT_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_MOVE_START_STRAFE_RIGHT)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_MOVE_START_STRAFE_RIGHT_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_MOVE_STOP_STRAFE)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_MOVE_STOP_STRAFE_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_MOVE_JUMP)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_MOVE_JUMP_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_MOVE_START_TURN_LEFT)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_MOVE_START_TURN_LEFT_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_MOVE_START_TURN_RIGHT)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_MOVE_START_TURN_RIGHT_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_MOVE_STOP_TURN)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_MOVE_STOP_TURN_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_MOVE_START_PITCH_UP)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_MOVE_START_PITCH_UP_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_MOVE_START_PITCH_DOWN)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_MOVE_START_PITCH_DOWN_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_MOVE_STOP_PITCH)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_MOVE_STOP_PITCH_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_MOVE_SET_RUN_MODE)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_MOVE_SET_RUN_MODE_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_MOVE_SET_WALK_MODE)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_MOVE_SET_WALK_MODE_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_MOVE_TELEPORT_ACK)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_MOVE_TELEPORT_ACK_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_MOVE_FALL_LAND)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_MOVE_FALL_LAND_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_MOVE_START_SWIM)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_MOVE_START_SWIM_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_MOVE_STOP_SWIM)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_MOVE_STOP_SWIM_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_MOVE_SET_FACING)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_MOVE_SET_FACING_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_MOVE_SET_PITCH)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_MOVE_SET_PITCH_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_MOVE_SET_RAW_POSITION)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_MOVE_SET_RAW_POSITION_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_FORCE_RUN_SPEED_CHANGE_ACK)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_FORCE_RUN_SPEED_CHANGE_ACK_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_FORCE_SWIM_SPEED_CHANGE_ACK)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_FORCE_SWIM_SPEED_CHANGE_ACK_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_FORCE_MOVE_ROOT_ACK)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_FORCE_MOVE_ROOT_ACK_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_FORCE_MOVE_UNROOT_ACK)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_FORCE_MOVE_UNROOT_ACK_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_MOVE_HEARTBEAT)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_MOVE_HEARTBEAT_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_MOVE_KNOCK_BACK_ACK)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_MOVE_KNOCK_BACK_ACK_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_MOVE_HOVER_ACK)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_MOVE_HOVER_ACK_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_NEXT_CINEMATIC_CAMERA)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_NEXT_CINEMATIC_CAMERA{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_COMPLETE_CINEMATIC)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_COMPLETE_CINEMATIC{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_TUTORIAL_FLAG)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_TUTORIAL_FLAG_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_TUTORIAL_CLEAR)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_TUTORIAL_CLEAR{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_TUTORIAL_RESET)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_TUTORIAL_RESET{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_STANDSTATECHANGE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_STANDSTATECHANGE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_EMOTE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_EMOTE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_TEXT_EMOTE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_TEXT_EMOTE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_AUTOSTORE_LOOT_ITEM)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_AUTOSTORE_LOOT_ITEM_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_AUTOEQUIP_ITEM)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_AUTOEQUIP_ITEM_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_AUTOSTORE_BAG_ITEM)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_AUTOSTORE_BAG_ITEM_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_SWAP_ITEM)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_SWAP_ITEM_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_SWAP_INV_ITEM)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_SWAP_INV_ITEM_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_SPLIT_ITEM)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_SPLIT_ITEM_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_AUTOEQUIP_ITEM_SLOT)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_AUTOEQUIP_ITEM_SLOT_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_DESTROYITEM)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_DESTROYITEM_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_INSPECT)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_INSPECT_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_INITIATE_TRADE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_INITIATE_TRADE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_BEGIN_TRADE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_BEGIN_TRADE{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_BUSY_TRADE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_BUSY_TRADE{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_IGNORE_TRADE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_IGNORE_TRADE{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_ACCEPT_TRADE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_ACCEPT_TRADE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_UNACCEPT_TRADE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_UNACCEPT_TRADE{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_CANCEL_TRADE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_CANCEL_TRADE{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_SET_TRADE_ITEM)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_SET_TRADE_ITEM_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_CLEAR_TRADE_ITEM)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_CLEAR_TRADE_ITEM_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_SET_TRADE_GOLD)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_SET_TRADE_GOLD_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_SET_FACTION_ATWAR)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_SET_FACTION_ATWAR_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_SET_ACTION_BUTTON)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_SET_ACTION_BUTTON_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_CAST_SPELL)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_CAST_SPELL_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_CANCEL_CAST)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_CANCEL_CAST_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_CANCEL_AURA)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_CANCEL_AURA_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_CANCEL_CHANNELLING)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_CANCEL_CHANNELLING_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_SET_SELECTION)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_SET_SELECTION_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_SET_TARGET_OBSOLETE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_SET_TARGET_OBSOLETE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_ATTACKSWING)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_ATTACKSWING_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_ATTACKSTOP)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_ATTACKSTOP{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_REPOP_REQUEST)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_REPOP_REQUEST{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_RESURRECT_RESPONSE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_RESURRECT_RESPONSE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_LOOT)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_LOOT_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_LOOT_MONEY)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_LOOT_MONEY{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_LOOT_RELEASE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_LOOT_RELEASE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_DUEL_ACCEPTED)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_DUEL_ACCEPTED_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_DUEL_CANCELLED)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_DUEL_CANCELLED_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_MOUNTSPECIAL_ANIM)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_MOUNTSPECIAL_ANIM{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_PET_ACTION)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_PET_ACTION_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_PET_ABANDON)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_PET_ABANDON_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_PET_RENAME)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_PET_RENAME_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GOSSIP_HELLO)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GOSSIP_HELLO_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_NPC_TEXT_QUERY)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_NPC_TEXT_QUERY_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_QUESTGIVER_STATUS_QUERY)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_QUESTGIVER_STATUS_QUERY_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_QUESTGIVER_HELLO)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_QUESTGIVER_HELLO_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_QUESTGIVER_QUERY_QUEST)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_QUESTGIVER_QUERY_QUEST_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_QUESTGIVER_QUEST_AUTOLAUNCH)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_QUESTGIVER_QUEST_AUTOLAUNCH{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_QUESTGIVER_ACCEPT_QUEST)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_QUESTGIVER_ACCEPT_QUEST_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_QUESTGIVER_COMPLETE_QUEST)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_QUESTGIVER_COMPLETE_QUEST_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_QUESTGIVER_REQUEST_REWARD)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_QUESTGIVER_REQUEST_REWARD_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_QUESTGIVER_CHOOSE_REWARD)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_QUESTGIVER_CHOOSE_REWARD_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_QUESTGIVER_CANCEL)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_QUESTGIVER_CANCEL{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_QUESTLOG_SWAP_QUEST)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_QUESTLOG_SWAP_QUEST_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_QUESTLOG_REMOVE_QUEST)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_QUESTLOG_REMOVE_QUEST_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_QUEST_CONFIRM_ACCEPT)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_QUEST_CONFIRM_ACCEPT_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_PUSHQUESTTOPARTY)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_PUSHQUESTTOPARTY_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_LIST_INVENTORY)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_LIST_INVENTORY_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_SELL_ITEM)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_SELL_ITEM_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_BUY_ITEM)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_BUY_ITEM_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_BUY_ITEM_IN_SLOT)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_BUY_ITEM_IN_SLOT_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_TAXINODE_STATUS_QUERY)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_TAXINODE_STATUS_QUERY_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_TAXIQUERYAVAILABLENODES)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_TAXIQUERYAVAILABLENODES_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_ACTIVATETAXI)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_ACTIVATETAXI_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_TRAINER_LIST)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_TRAINER_LIST_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_TRAINER_BUY_SPELL)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_TRAINER_BUY_SPELL_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_BINDER_ACTIVATE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_BINDER_ACTIVATE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_BANKER_ACTIVATE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_BANKER_ACTIVATE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_BUY_BANK_SLOT)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_BUY_BANK_SLOT_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_PETITION_SHOWLIST)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_PETITION_SHOWLIST_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_PETITION_BUY)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_PETITION_BUY_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_PETITION_SHOW_SIGNATURES)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_PETITION_SHOW_SIGNATURES_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_PETITION_SIGN)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_PETITION_SIGN_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_OFFER_PETITION)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_OFFER_PETITION_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_TURN_IN_PETITION)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_TURN_IN_PETITION_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_PETITION_QUERY)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_PETITION_QUERY_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_BUG)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_BUG_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_PLAYED_TIME)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_PLAYED_TIME{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_QUERY_TIME)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_QUERY_TIME{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_RECLAIM_CORPSE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_RECLAIM_CORPSE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_WRAP_ITEM)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_WRAP_ITEM_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_MINIMAP_PING)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_MINIMAP_PING_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_PING)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_PING_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_SETSHEATHED)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_SETSHEATHED_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_PET_CAST_SPELL)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_PET_CAST_SPELL_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_SAVE_GUILD_EMBLEM)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_SAVE_GUILD_EMBLEM_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_ZONEUPDATE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_ZONEUPDATE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_RANDOM_ROLL)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_RANDOM_ROLL_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_LOOKING_FOR_GROUP)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_LOOKING_FOR_GROUP_Client{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_UNLEARN_SKILL)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_UNLEARN_SKILL_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GMTICKET_UPDATETEXT)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GMTICKET_UPDATETEXT_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_REQUEST_ACCOUNT_DATA)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_REQUEST_ACCOUNT_DATA_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GMTICKET_GETTICKET)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GMTICKET_GETTICKET{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_CORPSE_QUERY)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_CORPSE_QUERY_Client{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GMTICKET_DELETETICKET)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GMTICKET_DELETETICKET{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GMTICKET_SYSTEMSTATUS)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GMTICKET_SYSTEMSTATUS{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_SPIRIT_HEALER_ACTIVATE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_SPIRIT_HEALER_ACTIVATE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_CHAT_IGNORED)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_CHAT_IGNORED_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GUILD_RANK)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GUILD_RANK_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GUILD_ADD_RANK)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GUILD_ADD_RANK_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GUILD_DEL_RANK)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GUILD_DEL_RANK{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GUILD_SET_PUBLIC_NOTE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GUILD_SET_PUBLIC_NOTE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GUILD_SET_OFFICER_NOTE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GUILD_SET_OFFICER_NOTE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_SEND_MAIL)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_SEND_MAIL_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GET_MAIL_LIST)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GET_MAIL_LIST_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_BATTLEFIELD_LIST)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_BATTLEFIELD_LIST_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_BATTLEFIELD_JOIN)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_BATTLEFIELD_JOIN_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_ITEM_TEXT_QUERY)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_ITEM_TEXT_QUERY_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_MAIL_TAKE_MONEY)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_MAIL_TAKE_MONEY_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_MAIL_TAKE_ITEM)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_MAIL_TAKE_ITEM_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_MAIL_MARK_AS_READ)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_MAIL_MARK_AS_READ_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_MAIL_RETURN_TO_SENDER)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_MAIL_RETURN_TO_SENDER_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_MAIL_DELETE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_MAIL_DELETE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_MAIL_CREATE_TEXT_ITEM)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_MAIL_CREATE_TEXT_ITEM_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_LEARN_TALENT)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_LEARN_TALENT_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_AUCTION_HELLO)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_AUCTION_HELLO_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_AUCTION_SELL_ITEM)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_AUCTION_SELL_ITEM_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_AUCTION_REMOVE_ITEM)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_AUCTION_REMOVE_ITEM_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_AUCTION_LIST_ITEMS)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_AUCTION_LIST_ITEMS_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_AUCTION_LIST_OWNER_ITEMS)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_AUCTION_LIST_OWNER_ITEMS_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_AUCTION_PLACE_BID)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_AUCTION_PLACE_BID_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_AUCTION_LIST_BIDDER_ITEMS)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_AUCTION_LIST_BIDDER_ITEMS_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_SET_AMMO)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_SET_AMMO_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_SET_ACTIVE_MOVER)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_SET_ACTIVE_MOVER_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_PET_CANCEL_AURA)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_PET_CANCEL_AURA_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_CANCEL_AUTO_REPEAT_SPELL)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_CANCEL_AUTO_REPEAT_SPELL{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_LIST_STABLED_PETS)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_LIST_STABLED_PETS_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_STABLE_PET)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_STABLE_PET_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_UNSTABLE_PET)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_UNSTABLE_PET_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_BUY_STABLE_SLOT)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_BUY_STABLE_SLOT_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_STABLE_SWAP_PET)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_STABLE_SWAP_PET_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_REQUEST_PET_INFO)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_REQUEST_PET_INFO{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_FAR_SIGHT)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_FAR_SIGHT_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GROUP_CHANGE_SUB_GROUP)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GROUP_CHANGE_SUB_GROUP_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_REQUEST_PARTY_MEMBER_STATS)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_REQUEST_PARTY_MEMBER_STATS_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GROUP_SWAP_SUB_GROUP)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GROUP_SWAP_SUB_GROUP_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_AUTOSTORE_BANK_ITEM)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_AUTOSTORE_BANK_ITEM_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_AUTOBANK_ITEM)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_AUTOBANK_ITEM_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_QUERY_NEXT_MAIL_TIME)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_QUERY_NEXT_MAIL_TIME_Client{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GROUP_RAID_CONVERT)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GROUP_RAID_CONVERT{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GROUP_ASSISTANT_LEADER)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GROUP_ASSISTANT_LEADER_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_BUYBACK_ITEM)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_BUYBACK_ITEM_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_MEETINGSTONE_JOIN)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_MEETINGSTONE_JOIN_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_MEETINGSTONE_LEAVE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_MEETINGSTONE_LEAVE{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_MEETINGSTONE_INFO)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_MEETINGSTONE_INFO{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_CANCEL_GROWTH_AURA)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_CANCEL_GROWTH_AURA{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_LOOT_ROLL)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_LOOT_ROLL_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_LOOT_MASTER_GIVE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_LOOT_MASTER_GIVE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_REPAIR_ITEM)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_REPAIR_ITEM_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_TALENT_WIPE_CONFIRM)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_TALENT_WIPE_CONFIRM_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_SUMMON_RESPONSE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_SUMMON_RESPONSE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_SELF_RES)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_SELF_RES{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_TOGGLE_HELM)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_TOGGLE_HELM{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_TOGGLE_CLOAK)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_TOGGLE_CLOAK{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_SET_ACTIONBAR_TOGGLES)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_SET_ACTIONBAR_TOGGLES_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_ITEM_NAME_QUERY)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_ITEM_NAME_QUERY_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_CHAR_RENAME)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_CHAR_RENAME_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_MOVE_SPLINE_DONE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_MOVE_SPLINE_DONE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_MOVE_FALL_RESET)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_MOVE_FALL_RESET_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_REQUEST_RAID_INFO)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_REQUEST_RAID_INFO{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_MOVE_TIME_SKIPPED)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_MOVE_TIME_SKIPPED_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_MOVE_FEATHER_FALL_ACK)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_MOVE_FEATHER_FALL_ACK_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_MOVE_WATER_WALK_ACK)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_MOVE_WATER_WALK_ACK_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_MOVE_NOT_ACTIVE_MOVER)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_MOVE_NOT_ACTIVE_MOVER_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_BATTLEFIELD_STATUS)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_BATTLEFIELD_STATUS{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_BATTLEFIELD_PORT)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_BATTLEFIELD_PORT_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_INSPECT_HONOR_STATS)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_INSPECT_HONOR_STATS_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_BATTLEMASTER_HELLO)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_BATTLEMASTER_HELLO_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_FORCE_WALK_SPEED_CHANGE_ACK)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_FORCE_WALK_SPEED_CHANGE_ACK_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_FORCE_TURN_RATE_CHANGE_ACK)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_FORCE_TURN_RATE_CHANGE_ACK_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_PVP_LOG_DATA)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_PVP_LOG_DATA_Client{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_LEAVE_BATTLEFIELD)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_LEAVE_BATTLEFIELD_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_AREA_SPIRIT_HEALER_QUERY)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_AREA_SPIRIT_HEALER_QUERY_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_AREA_SPIRIT_HEALER_QUEUE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_AREA_SPIRIT_HEALER_QUEUE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_WARDEN_DATA)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_WARDEN_DATA_read(reader, _size - 4));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_BATTLEGROUND_PLAYER_POSITIONS)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_BATTLEGROUND_PLAYER_POSITIONS_Client{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_PET_STOP_ATTACK)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_PET_STOP_ATTACK_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_BATTLEMASTER_JOIN)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_BATTLEMASTER_JOIN_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_PET_UNLEARN)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_PET_UNLEARN_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_PET_SPELL_AUTOCAST)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_PET_SPELL_AUTOCAST_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GUILD_INFO_TEXT)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GUILD_INFO_TEXT_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_ACTIVATETAXIEXPRESS)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_ACTIVATETAXIEXPRESS_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_SET_FACTION_INACTIVE)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_SET_FACTION_INACTIVE_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_SET_WATCHED_FACTION)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_SET_WATCHED_FACTION_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_RESET_INSTANCES)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_RESET_INSTANCES{});
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::MSG_RAID_TARGET_UPDATE)) {
        return ClientOpcode(::wow_world_messages::vanilla::MSG_RAID_TARGET_UPDATE_Client_read(reader));
    }
    if (opcode == static_cast<uint32_t>(ClientOpcode::Opcode::CMSG_GMSURVEY_SUBMIT)) {
        return ClientOpcode(::wow_world_messages::vanilla::CMSG_GMSURVEY_SUBMIT_read(reader));
    }

    return op;
}
std::vector<unsigned char> write_opcode(const ServerOpcode& opcode) {
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_CHAR_CREATE) {
        return opcode.SMSG_CHAR_CREATE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_CHAR_ENUM) {
        return opcode.SMSG_CHAR_ENUM.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_CHAR_DELETE) {
        return opcode.SMSG_CHAR_DELETE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_NEW_WORLD) {
        return opcode.SMSG_NEW_WORLD.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_TRANSFER_ABORTED) {
        return opcode.SMSG_TRANSFER_ABORTED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_CHARACTER_LOGIN_FAILED) {
        return opcode.SMSG_CHARACTER_LOGIN_FAILED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_LOGIN_SETTIMESPEED) {
        return opcode.SMSG_LOGIN_SETTIMESPEED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_LOGOUT_RESPONSE) {
        return opcode.SMSG_LOGOUT_RESPONSE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_LOGOUT_COMPLETE) {
        return opcode.SMSG_LOGOUT_COMPLETE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_LOGOUT_CANCEL_ACK) {
        return opcode.SMSG_LOGOUT_CANCEL_ACK.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_NAME_QUERY_RESPONSE) {
        return opcode.SMSG_NAME_QUERY_RESPONSE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PET_NAME_QUERY_RESPONSE) {
        return opcode.SMSG_PET_NAME_QUERY_RESPONSE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_GUILD_QUERY_RESPONSE) {
        return opcode.SMSG_GUILD_QUERY_RESPONSE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PAGE_TEXT_QUERY_RESPONSE) {
        return opcode.SMSG_PAGE_TEXT_QUERY_RESPONSE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_QUEST_QUERY_RESPONSE) {
        return opcode.SMSG_QUEST_QUERY_RESPONSE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_WHO) {
        return opcode.SMSG_WHO.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_WHOIS) {
        return opcode.SMSG_WHOIS.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_FRIEND_LIST) {
        return opcode.SMSG_FRIEND_LIST.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_FRIEND_STATUS) {
        return opcode.SMSG_FRIEND_STATUS.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_IGNORE_LIST) {
        return opcode.SMSG_IGNORE_LIST.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_GROUP_INVITE) {
        return opcode.SMSG_GROUP_INVITE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_GROUP_DECLINE) {
        return opcode.SMSG_GROUP_DECLINE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_GROUP_UNINVITE) {
        return opcode.SMSG_GROUP_UNINVITE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_GROUP_SET_LEADER) {
        return opcode.SMSG_GROUP_SET_LEADER.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_GROUP_DESTROYED) {
        return opcode.SMSG_GROUP_DESTROYED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PARTY_MEMBER_STATS) {
        return opcode.SMSG_PARTY_MEMBER_STATS.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PARTY_COMMAND_RESULT) {
        return opcode.SMSG_PARTY_COMMAND_RESULT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_GUILD_INVITE) {
        return opcode.SMSG_GUILD_INVITE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_GUILD_INFO) {
        return opcode.SMSG_GUILD_INFO.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_GUILD_ROSTER) {
        return opcode.SMSG_GUILD_ROSTER.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_GUILD_EVENT) {
        return opcode.SMSG_GUILD_EVENT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_GUILD_COMMAND_RESULT) {
        return opcode.SMSG_GUILD_COMMAND_RESULT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_MESSAGECHAT) {
        return opcode.SMSG_MESSAGECHAT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_CHANNEL_NOTIFY) {
        return opcode.SMSG_CHANNEL_NOTIFY.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_CHANNEL_LIST) {
        return opcode.SMSG_CHANNEL_LIST.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_UPDATE_OBJECT) {
        return opcode.SMSG_UPDATE_OBJECT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_DESTROY_OBJECT) {
        return opcode.SMSG_DESTROY_OBJECT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_READ_ITEM_OK) {
        return opcode.SMSG_READ_ITEM_OK.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_READ_ITEM_FAILED) {
        return opcode.SMSG_READ_ITEM_FAILED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_ITEM_COOLDOWN) {
        return opcode.SMSG_ITEM_COOLDOWN.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_GAMEOBJECT_CUSTOM_ANIM) {
        return opcode.SMSG_GAMEOBJECT_CUSTOM_ANIM.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_MOVE_START_FORWARD) {
        return opcode.MSG_MOVE_START_FORWARD.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_MOVE_START_BACKWARD) {
        return opcode.MSG_MOVE_START_BACKWARD.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_MOVE_STOP) {
        return opcode.MSG_MOVE_STOP.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_MOVE_START_STRAFE_LEFT) {
        return opcode.MSG_MOVE_START_STRAFE_LEFT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_MOVE_START_STRAFE_RIGHT) {
        return opcode.MSG_MOVE_START_STRAFE_RIGHT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_MOVE_STOP_STRAFE) {
        return opcode.MSG_MOVE_STOP_STRAFE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_MOVE_JUMP) {
        return opcode.MSG_MOVE_JUMP.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_MOVE_START_TURN_LEFT) {
        return opcode.MSG_MOVE_START_TURN_LEFT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_MOVE_START_TURN_RIGHT) {
        return opcode.MSG_MOVE_START_TURN_RIGHT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_MOVE_STOP_TURN) {
        return opcode.MSG_MOVE_STOP_TURN.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_MOVE_START_PITCH_UP) {
        return opcode.MSG_MOVE_START_PITCH_UP.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_MOVE_START_PITCH_DOWN) {
        return opcode.MSG_MOVE_START_PITCH_DOWN.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_MOVE_STOP_PITCH) {
        return opcode.MSG_MOVE_STOP_PITCH.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_MOVE_SET_RUN_MODE) {
        return opcode.MSG_MOVE_SET_RUN_MODE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_MOVE_SET_WALK_MODE) {
        return opcode.MSG_MOVE_SET_WALK_MODE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_MOVE_TELEPORT_ACK) {
        return opcode.MSG_MOVE_TELEPORT_ACK.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_MOVE_FALL_LAND) {
        return opcode.MSG_MOVE_FALL_LAND.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_MOVE_START_SWIM) {
        return opcode.MSG_MOVE_START_SWIM.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_MOVE_STOP_SWIM) {
        return opcode.MSG_MOVE_STOP_SWIM.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_MOVE_SET_FACING) {
        return opcode.MSG_MOVE_SET_FACING.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_MOVE_SET_PITCH) {
        return opcode.MSG_MOVE_SET_PITCH.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_MONSTER_MOVE) {
        return opcode.SMSG_MONSTER_MOVE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_MOVE_WATER_WALK) {
        return opcode.SMSG_MOVE_WATER_WALK.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_MOVE_LAND_WALK) {
        return opcode.SMSG_MOVE_LAND_WALK.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_FORCE_RUN_SPEED_CHANGE) {
        return opcode.SMSG_FORCE_RUN_SPEED_CHANGE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_FORCE_RUN_BACK_SPEED_CHANGE) {
        return opcode.SMSG_FORCE_RUN_BACK_SPEED_CHANGE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_FORCE_SWIM_SPEED_CHANGE) {
        return opcode.SMSG_FORCE_SWIM_SPEED_CHANGE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_FORCE_MOVE_ROOT) {
        return opcode.SMSG_FORCE_MOVE_ROOT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_FORCE_MOVE_UNROOT) {
        return opcode.SMSG_FORCE_MOVE_UNROOT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_MOVE_HEARTBEAT) {
        return opcode.MSG_MOVE_HEARTBEAT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_MOVE_KNOCK_BACK) {
        return opcode.SMSG_MOVE_KNOCK_BACK.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_MOVE_FEATHER_FALL) {
        return opcode.SMSG_MOVE_FEATHER_FALL.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_MOVE_NORMAL_FALL) {
        return opcode.SMSG_MOVE_NORMAL_FALL.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_MOVE_SET_HOVER) {
        return opcode.SMSG_MOVE_SET_HOVER.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_MOVE_UNSET_HOVER) {
        return opcode.SMSG_MOVE_UNSET_HOVER.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_TRIGGER_CINEMATIC) {
        return opcode.SMSG_TRIGGER_CINEMATIC.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_TUTORIAL_FLAGS) {
        return opcode.SMSG_TUTORIAL_FLAGS.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_EMOTE) {
        return opcode.SMSG_EMOTE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_TEXT_EMOTE) {
        return opcode.SMSG_TEXT_EMOTE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_INVENTORY_CHANGE_FAILURE) {
        return opcode.SMSG_INVENTORY_CHANGE_FAILURE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_OPEN_CONTAINER) {
        return opcode.SMSG_OPEN_CONTAINER.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_INSPECT) {
        return opcode.SMSG_INSPECT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_TRADE_STATUS) {
        return opcode.SMSG_TRADE_STATUS.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_TRADE_STATUS_EXTENDED) {
        return opcode.SMSG_TRADE_STATUS_EXTENDED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_INITIALIZE_FACTIONS) {
        return opcode.SMSG_INITIALIZE_FACTIONS.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SET_FACTION_VISIBLE) {
        return opcode.SMSG_SET_FACTION_VISIBLE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SET_FACTION_STANDING) {
        return opcode.SMSG_SET_FACTION_STANDING.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SET_PROFICIENCY) {
        return opcode.SMSG_SET_PROFICIENCY.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_ACTION_BUTTONS) {
        return opcode.SMSG_ACTION_BUTTONS.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_INITIAL_SPELLS) {
        return opcode.SMSG_INITIAL_SPELLS.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_LEARNED_SPELL) {
        return opcode.SMSG_LEARNED_SPELL.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SUPERCEDED_SPELL) {
        return opcode.SMSG_SUPERCEDED_SPELL.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_CAST_RESULT) {
        return opcode.SMSG_CAST_RESULT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPELL_START) {
        return opcode.SMSG_SPELL_START.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPELL_GO) {
        return opcode.SMSG_SPELL_GO.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPELL_FAILURE) {
        return opcode.SMSG_SPELL_FAILURE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPELL_COOLDOWN) {
        return opcode.SMSG_SPELL_COOLDOWN.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_COOLDOWN_EVENT) {
        return opcode.SMSG_COOLDOWN_EVENT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_UPDATE_AURA_DURATION) {
        return opcode.SMSG_UPDATE_AURA_DURATION.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PET_CAST_FAILED) {
        return opcode.SMSG_PET_CAST_FAILED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_CHANNEL_START) {
        return opcode.MSG_CHANNEL_START.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_CHANNEL_UPDATE) {
        return opcode.MSG_CHANNEL_UPDATE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_AI_REACTION) {
        return opcode.SMSG_AI_REACTION.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_ATTACKSTART) {
        return opcode.SMSG_ATTACKSTART.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_ATTACKSTOP) {
        return opcode.SMSG_ATTACKSTOP.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_ATTACKSWING_NOTINRANGE) {
        return opcode.SMSG_ATTACKSWING_NOTINRANGE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_ATTACKSWING_BADFACING) {
        return opcode.SMSG_ATTACKSWING_BADFACING.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_ATTACKSWING_NOTSTANDING) {
        return opcode.SMSG_ATTACKSWING_NOTSTANDING.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_ATTACKSWING_DEADTARGET) {
        return opcode.SMSG_ATTACKSWING_DEADTARGET.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_ATTACKSWING_CANT_ATTACK) {
        return opcode.SMSG_ATTACKSWING_CANT_ATTACK.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_ATTACKERSTATEUPDATE) {
        return opcode.SMSG_ATTACKERSTATEUPDATE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_CANCEL_COMBAT) {
        return opcode.SMSG_CANCEL_COMBAT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPELLHEALLOG) {
        return opcode.SMSG_SPELLHEALLOG.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPELLENERGIZELOG) {
        return opcode.SMSG_SPELLENERGIZELOG.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_BINDPOINTUPDATE) {
        return opcode.SMSG_BINDPOINTUPDATE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PLAYERBOUND) {
        return opcode.SMSG_PLAYERBOUND.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_CLIENT_CONTROL_UPDATE) {
        return opcode.SMSG_CLIENT_CONTROL_UPDATE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_RESURRECT_REQUEST) {
        return opcode.SMSG_RESURRECT_REQUEST.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_LOOT_RESPONSE) {
        return opcode.SMSG_LOOT_RESPONSE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_LOOT_RELEASE_RESPONSE) {
        return opcode.SMSG_LOOT_RELEASE_RESPONSE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_LOOT_REMOVED) {
        return opcode.SMSG_LOOT_REMOVED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_LOOT_MONEY_NOTIFY) {
        return opcode.SMSG_LOOT_MONEY_NOTIFY.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_LOOT_CLEAR_MONEY) {
        return opcode.SMSG_LOOT_CLEAR_MONEY.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_ITEM_PUSH_RESULT) {
        return opcode.SMSG_ITEM_PUSH_RESULT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_DUEL_REQUESTED) {
        return opcode.SMSG_DUEL_REQUESTED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_DUEL_OUTOFBOUNDS) {
        return opcode.SMSG_DUEL_OUTOFBOUNDS.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_DUEL_INBOUNDS) {
        return opcode.SMSG_DUEL_INBOUNDS.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_DUEL_COMPLETE) {
        return opcode.SMSG_DUEL_COMPLETE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_DUEL_WINNER) {
        return opcode.SMSG_DUEL_WINNER.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_MOUNTRESULT) {
        return opcode.SMSG_MOUNTRESULT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_DISMOUNTRESULT) {
        return opcode.SMSG_DISMOUNTRESULT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_MOUNTSPECIAL_ANIM) {
        return opcode.SMSG_MOUNTSPECIAL_ANIM.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PET_TAME_FAILURE) {
        return opcode.SMSG_PET_TAME_FAILURE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PET_NAME_INVALID) {
        return opcode.SMSG_PET_NAME_INVALID.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PET_MODE) {
        return opcode.SMSG_PET_MODE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_GOSSIP_MESSAGE) {
        return opcode.SMSG_GOSSIP_MESSAGE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_GOSSIP_COMPLETE) {
        return opcode.SMSG_GOSSIP_COMPLETE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_NPC_TEXT_UPDATE) {
        return opcode.SMSG_NPC_TEXT_UPDATE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_QUESTGIVER_STATUS) {
        return opcode.SMSG_QUESTGIVER_STATUS.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_QUESTGIVER_QUEST_LIST) {
        return opcode.SMSG_QUESTGIVER_QUEST_LIST.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_QUESTGIVER_QUEST_DETAILS) {
        return opcode.SMSG_QUESTGIVER_QUEST_DETAILS.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_QUESTGIVER_REQUEST_ITEMS) {
        return opcode.SMSG_QUESTGIVER_REQUEST_ITEMS.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_QUESTGIVER_OFFER_REWARD) {
        return opcode.SMSG_QUESTGIVER_OFFER_REWARD.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_QUESTGIVER_QUEST_INVALID) {
        return opcode.SMSG_QUESTGIVER_QUEST_INVALID.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_QUESTGIVER_QUEST_COMPLETE) {
        return opcode.SMSG_QUESTGIVER_QUEST_COMPLETE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_QUESTGIVER_QUEST_FAILED) {
        return opcode.SMSG_QUESTGIVER_QUEST_FAILED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_QUESTLOG_FULL) {
        return opcode.SMSG_QUESTLOG_FULL.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_QUESTUPDATE_FAILED) {
        return opcode.SMSG_QUESTUPDATE_FAILED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_QUESTUPDATE_FAILEDTIMER) {
        return opcode.SMSG_QUESTUPDATE_FAILEDTIMER.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_QUESTUPDATE_COMPLETE) {
        return opcode.SMSG_QUESTUPDATE_COMPLETE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_QUESTUPDATE_ADD_KILL) {
        return opcode.SMSG_QUESTUPDATE_ADD_KILL.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_QUESTUPDATE_ADD_ITEM) {
        return opcode.SMSG_QUESTUPDATE_ADD_ITEM.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_QUEST_CONFIRM_ACCEPT) {
        return opcode.SMSG_QUEST_CONFIRM_ACCEPT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_LIST_INVENTORY) {
        return opcode.SMSG_LIST_INVENTORY.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SELL_ITEM) {
        return opcode.SMSG_SELL_ITEM.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_BUY_ITEM) {
        return opcode.SMSG_BUY_ITEM.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_BUY_FAILED) {
        return opcode.SMSG_BUY_FAILED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SHOWTAXINODES) {
        return opcode.SMSG_SHOWTAXINODES.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_TAXINODE_STATUS) {
        return opcode.SMSG_TAXINODE_STATUS.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_ACTIVATETAXIREPLY) {
        return opcode.SMSG_ACTIVATETAXIREPLY.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_NEW_TAXI_PATH) {
        return opcode.SMSG_NEW_TAXI_PATH.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_TRAINER_LIST) {
        return opcode.SMSG_TRAINER_LIST.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_TRAINER_BUY_SUCCEEDED) {
        return opcode.SMSG_TRAINER_BUY_SUCCEEDED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_TRAINER_BUY_FAILED) {
        return opcode.SMSG_TRAINER_BUY_FAILED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SHOW_BANK) {
        return opcode.SMSG_SHOW_BANK.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_BUY_BANK_SLOT_RESULT) {
        return opcode.SMSG_BUY_BANK_SLOT_RESULT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PETITION_SHOWLIST) {
        return opcode.SMSG_PETITION_SHOWLIST.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PETITION_SHOW_SIGNATURES) {
        return opcode.SMSG_PETITION_SHOW_SIGNATURES.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PETITION_SIGN_RESULTS) {
        return opcode.SMSG_PETITION_SIGN_RESULTS.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_TURN_IN_PETITION_RESULTS) {
        return opcode.SMSG_TURN_IN_PETITION_RESULTS.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PETITION_QUERY_RESPONSE) {
        return opcode.SMSG_PETITION_QUERY_RESPONSE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_FISH_NOT_HOOKED) {
        return opcode.SMSG_FISH_NOT_HOOKED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_FISH_ESCAPED) {
        return opcode.SMSG_FISH_ESCAPED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_NOTIFICATION) {
        return opcode.SMSG_NOTIFICATION.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PLAYED_TIME) {
        return opcode.SMSG_PLAYED_TIME.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_QUERY_TIME_RESPONSE) {
        return opcode.SMSG_QUERY_TIME_RESPONSE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_LOG_XPGAIN) {
        return opcode.SMSG_LOG_XPGAIN.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_LEVELUP_INFO) {
        return opcode.SMSG_LEVELUP_INFO.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_MINIMAP_PING) {
        return opcode.MSG_MINIMAP_PING.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_RESISTLOG) {
        return opcode.SMSG_RESISTLOG.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_ENCHANTMENTLOG) {
        return opcode.SMSG_ENCHANTMENTLOG.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_START_MIRROR_TIMER) {
        return opcode.SMSG_START_MIRROR_TIMER.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PAUSE_MIRROR_TIMER) {
        return opcode.SMSG_PAUSE_MIRROR_TIMER.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_STOP_MIRROR_TIMER) {
        return opcode.SMSG_STOP_MIRROR_TIMER.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PONG) {
        return opcode.SMSG_PONG.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_CLEAR_COOLDOWN) {
        return opcode.SMSG_CLEAR_COOLDOWN.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_GAMEOBJECT_PAGETEXT) {
        return opcode.SMSG_GAMEOBJECT_PAGETEXT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPELL_DELAYED) {
        return opcode.SMSG_SPELL_DELAYED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_ITEM_TIME_UPDATE) {
        return opcode.SMSG_ITEM_TIME_UPDATE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_ITEM_ENCHANT_TIME_UPDATE) {
        return opcode.SMSG_ITEM_ENCHANT_TIME_UPDATE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_AUTH_CHALLENGE) {
        return opcode.SMSG_AUTH_CHALLENGE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_AUTH_RESPONSE) {
        return opcode.SMSG_AUTH_RESPONSE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_SAVE_GUILD_EMBLEM) {
        return opcode.MSG_SAVE_GUILD_EMBLEM.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PLAY_SPELL_VISUAL) {
        return opcode.SMSG_PLAY_SPELL_VISUAL.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PARTYKILLLOG) {
        return opcode.SMSG_PARTYKILLLOG.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PLAY_SPELL_IMPACT) {
        return opcode.SMSG_PLAY_SPELL_IMPACT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_EXPLORATION_EXPERIENCE) {
        return opcode.SMSG_EXPLORATION_EXPERIENCE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_RANDOM_ROLL) {
        return opcode.MSG_RANDOM_ROLL.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_ENVIRONMENTAL_DAMAGE_LOG) {
        return opcode.SMSG_ENVIRONMENTAL_DAMAGE_LOG.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_LOOKING_FOR_GROUP) {
        return opcode.MSG_LOOKING_FOR_GROUP.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_REMOVED_SPELL) {
        return opcode.SMSG_REMOVED_SPELL.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_GMTICKET_CREATE) {
        return opcode.SMSG_GMTICKET_CREATE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_GMTICKET_UPDATETEXT) {
        return opcode.SMSG_GMTICKET_UPDATETEXT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_ACCOUNT_DATA_TIMES) {
        return opcode.SMSG_ACCOUNT_DATA_TIMES.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_GMTICKET_GETTICKET) {
        return opcode.SMSG_GMTICKET_GETTICKET.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_GAMEOBJECT_SPAWN_ANIM) {
        return opcode.SMSG_GAMEOBJECT_SPAWN_ANIM.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_GAMEOBJECT_DESPAWN_ANIM) {
        return opcode.SMSG_GAMEOBJECT_DESPAWN_ANIM.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_CORPSE_QUERY) {
        return opcode.MSG_CORPSE_QUERY.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_GMTICKET_DELETETICKET) {
        return opcode.SMSG_GMTICKET_DELETETICKET.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_CHAT_WRONG_FACTION) {
        return opcode.SMSG_CHAT_WRONG_FACTION.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_GMTICKET_SYSTEMSTATUS) {
        return opcode.SMSG_GMTICKET_SYSTEMSTATUS.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SET_REST_START) {
        return opcode.SMSG_SET_REST_START.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPIRIT_HEALER_CONFIRM) {
        return opcode.SMSG_SPIRIT_HEALER_CONFIRM.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_GOSSIP_POI) {
        return opcode.SMSG_GOSSIP_POI.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_LOGIN_VERIFY_WORLD) {
        return opcode.SMSG_LOGIN_VERIFY_WORLD.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SEND_MAIL_RESULT) {
        return opcode.SMSG_SEND_MAIL_RESULT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_MAIL_LIST_RESULT) {
        return opcode.SMSG_MAIL_LIST_RESULT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_BATTLEFIELD_LIST) {
        return opcode.SMSG_BATTLEFIELD_LIST.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_ITEM_TEXT_QUERY_RESPONSE) {
        return opcode.SMSG_ITEM_TEXT_QUERY_RESPONSE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPELLLOGMISS) {
        return opcode.SMSG_SPELLLOGMISS.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPELLLOGEXECUTE) {
        return opcode.SMSG_SPELLLOGEXECUTE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PERIODICAURALOG) {
        return opcode.SMSG_PERIODICAURALOG.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPELLDAMAGESHIELD) {
        return opcode.SMSG_SPELLDAMAGESHIELD.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPELLNONMELEEDAMAGELOG) {
        return opcode.SMSG_SPELLNONMELEEDAMAGELOG.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_ZONE_UNDER_ATTACK) {
        return opcode.SMSG_ZONE_UNDER_ATTACK.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_AUCTION_HELLO) {
        return opcode.MSG_AUCTION_HELLO.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_AUCTION_COMMAND_RESULT) {
        return opcode.SMSG_AUCTION_COMMAND_RESULT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_AUCTION_LIST_RESULT) {
        return opcode.SMSG_AUCTION_LIST_RESULT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_AUCTION_OWNER_LIST_RESULT) {
        return opcode.SMSG_AUCTION_OWNER_LIST_RESULT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_AUCTION_BIDDER_NOTIFICATION) {
        return opcode.SMSG_AUCTION_BIDDER_NOTIFICATION.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_AUCTION_OWNER_NOTIFICATION) {
        return opcode.SMSG_AUCTION_OWNER_NOTIFICATION.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PROCRESIST) {
        return opcode.SMSG_PROCRESIST.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_DISPEL_FAILED) {
        return opcode.SMSG_DISPEL_FAILED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPELLORDAMAGE_IMMUNE) {
        return opcode.SMSG_SPELLORDAMAGE_IMMUNE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_AUCTION_BIDDER_LIST_RESULT) {
        return opcode.SMSG_AUCTION_BIDDER_LIST_RESULT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SET_FLAT_SPELL_MODIFIER) {
        return opcode.SMSG_SET_FLAT_SPELL_MODIFIER.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SET_PCT_SPELL_MODIFIER) {
        return opcode.SMSG_SET_PCT_SPELL_MODIFIER.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_CORPSE_RECLAIM_DELAY) {
        return opcode.SMSG_CORPSE_RECLAIM_DELAY.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_LIST_STABLED_PETS) {
        return opcode.MSG_LIST_STABLED_PETS.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_STABLE_RESULT) {
        return opcode.SMSG_STABLE_RESULT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PLAY_MUSIC) {
        return opcode.SMSG_PLAY_MUSIC.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PLAY_OBJECT_SOUND) {
        return opcode.SMSG_PLAY_OBJECT_SOUND.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPELLDISPELLOG) {
        return opcode.SMSG_SPELLDISPELLOG.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_QUERY_NEXT_MAIL_TIME) {
        return opcode.MSG_QUERY_NEXT_MAIL_TIME.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_RECEIVED_MAIL) {
        return opcode.SMSG_RECEIVED_MAIL.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_RAID_GROUP_ONLY) {
        return opcode.SMSG_RAID_GROUP_ONLY.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PVP_CREDIT) {
        return opcode.SMSG_PVP_CREDIT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_AUCTION_REMOVED_NOTIFICATION) {
        return opcode.SMSG_AUCTION_REMOVED_NOTIFICATION.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SERVER_MESSAGE) {
        return opcode.SMSG_SERVER_MESSAGE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_MEETINGSTONE_SETQUEUE) {
        return opcode.SMSG_MEETINGSTONE_SETQUEUE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_MEETINGSTONE_COMPLETE) {
        return opcode.SMSG_MEETINGSTONE_COMPLETE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_MEETINGSTONE_IN_PROGRESS) {
        return opcode.SMSG_MEETINGSTONE_IN_PROGRESS.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_MEETINGSTONE_MEMBER_ADDED) {
        return opcode.SMSG_MEETINGSTONE_MEMBER_ADDED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_CANCEL_AUTO_REPEAT) {
        return opcode.SMSG_CANCEL_AUTO_REPEAT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_STANDSTATE_UPDATE) {
        return opcode.SMSG_STANDSTATE_UPDATE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_LOOT_ALL_PASSED) {
        return opcode.SMSG_LOOT_ALL_PASSED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_LOOT_ROLL_WON) {
        return opcode.SMSG_LOOT_ROLL_WON.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_LOOT_START_ROLL) {
        return opcode.SMSG_LOOT_START_ROLL.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_LOOT_ROLL) {
        return opcode.SMSG_LOOT_ROLL.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_LOOT_MASTER_LIST) {
        return opcode.SMSG_LOOT_MASTER_LIST.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SET_FORCED_REACTIONS) {
        return opcode.SMSG_SET_FORCED_REACTIONS.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPELL_FAILED_OTHER) {
        return opcode.SMSG_SPELL_FAILED_OTHER.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_GAMEOBJECT_RESET_STATE) {
        return opcode.SMSG_GAMEOBJECT_RESET_STATE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_CHAT_PLAYER_NOT_FOUND) {
        return opcode.SMSG_CHAT_PLAYER_NOT_FOUND.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_TALENT_WIPE_CONFIRM) {
        return opcode.MSG_TALENT_WIPE_CONFIRM.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SUMMON_REQUEST) {
        return opcode.SMSG_SUMMON_REQUEST.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_MONSTER_MOVE_TRANSPORT) {
        return opcode.SMSG_MONSTER_MOVE_TRANSPORT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PET_BROKEN) {
        return opcode.SMSG_PET_BROKEN.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_MOVE_FEATHER_FALL) {
        return opcode.MSG_MOVE_FEATHER_FALL.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_FEIGN_DEATH_RESISTED) {
        return opcode.SMSG_FEIGN_DEATH_RESISTED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_DUEL_COUNTDOWN) {
        return opcode.SMSG_DUEL_COUNTDOWN.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_AREA_TRIGGER_MESSAGE) {
        return opcode.SMSG_AREA_TRIGGER_MESSAGE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_MEETINGSTONE_JOINFAILED) {
        return opcode.SMSG_MEETINGSTONE_JOINFAILED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PLAYER_SKINNED) {
        return opcode.SMSG_PLAYER_SKINNED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_DURABILITY_DAMAGE_DEATH) {
        return opcode.SMSG_DURABILITY_DAMAGE_DEATH.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_INIT_WORLD_STATES) {
        return opcode.SMSG_INIT_WORLD_STATES.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_UPDATE_WORLD_STATE) {
        return opcode.SMSG_UPDATE_WORLD_STATE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_ITEM_NAME_QUERY_RESPONSE) {
        return opcode.SMSG_ITEM_NAME_QUERY_RESPONSE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PET_ACTION_FEEDBACK) {
        return opcode.SMSG_PET_ACTION_FEEDBACK.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_CHAR_RENAME) {
        return opcode.SMSG_CHAR_RENAME.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_INSTANCE_SAVE_CREATED) {
        return opcode.SMSG_INSTANCE_SAVE_CREATED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_RAID_INSTANCE_INFO) {
        return opcode.SMSG_RAID_INSTANCE_INFO.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PLAY_SOUND) {
        return opcode.SMSG_PLAY_SOUND.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_BATTLEFIELD_STATUS) {
        return opcode.SMSG_BATTLEFIELD_STATUS.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_INSPECT_HONOR_STATS) {
        return opcode.MSG_INSPECT_HONOR_STATS.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_FORCE_WALK_SPEED_CHANGE) {
        return opcode.SMSG_FORCE_WALK_SPEED_CHANGE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_FORCE_SWIM_BACK_SPEED_CHANGE) {
        return opcode.SMSG_FORCE_SWIM_BACK_SPEED_CHANGE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_FORCE_TURN_RATE_CHANGE) {
        return opcode.SMSG_FORCE_TURN_RATE_CHANGE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_PVP_LOG_DATA) {
        return opcode.MSG_PVP_LOG_DATA.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_AREA_SPIRIT_HEALER_TIME) {
        return opcode.SMSG_AREA_SPIRIT_HEALER_TIME.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_WARDEN_DATA) {
        return opcode.SMSG_WARDEN_DATA.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_GROUP_JOINED_BATTLEGROUND) {
        return opcode.SMSG_GROUP_JOINED_BATTLEGROUND.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_BATTLEGROUND_PLAYER_POSITIONS) {
        return opcode.MSG_BATTLEGROUND_PLAYER_POSITIONS.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_BINDER_CONFIRM) {
        return opcode.SMSG_BINDER_CONFIRM.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_BATTLEGROUND_PLAYER_JOINED) {
        return opcode.SMSG_BATTLEGROUND_PLAYER_JOINED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_BATTLEGROUND_PLAYER_LEFT) {
        return opcode.SMSG_BATTLEGROUND_PLAYER_LEFT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_ADDON_INFO) {
        return opcode.SMSG_ADDON_INFO.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PET_UNLEARN_CONFIRM) {
        return opcode.SMSG_PET_UNLEARN_CONFIRM.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PARTY_MEMBER_STATS_FULL) {
        return opcode.SMSG_PARTY_MEMBER_STATS_FULL.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_WEATHER) {
        return opcode.SMSG_WEATHER.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_RAID_INSTANCE_MESSAGE) {
        return opcode.SMSG_RAID_INSTANCE_MESSAGE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_CHAT_RESTRICTED) {
        return opcode.SMSG_CHAT_RESTRICTED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPLINE_SET_RUN_SPEED) {
        return opcode.SMSG_SPLINE_SET_RUN_SPEED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPLINE_SET_RUN_BACK_SPEED) {
        return opcode.SMSG_SPLINE_SET_RUN_BACK_SPEED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPLINE_SET_SWIM_SPEED) {
        return opcode.SMSG_SPLINE_SET_SWIM_SPEED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPLINE_SET_WALK_SPEED) {
        return opcode.SMSG_SPLINE_SET_WALK_SPEED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPLINE_SET_SWIM_BACK_SPEED) {
        return opcode.SMSG_SPLINE_SET_SWIM_BACK_SPEED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPLINE_SET_TURN_RATE) {
        return opcode.SMSG_SPLINE_SET_TURN_RATE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPLINE_MOVE_UNROOT) {
        return opcode.SMSG_SPLINE_MOVE_UNROOT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPLINE_MOVE_FEATHER_FALL) {
        return opcode.SMSG_SPLINE_MOVE_FEATHER_FALL.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPLINE_MOVE_NORMAL_FALL) {
        return opcode.SMSG_SPLINE_MOVE_NORMAL_FALL.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPLINE_MOVE_SET_HOVER) {
        return opcode.SMSG_SPLINE_MOVE_SET_HOVER.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPLINE_MOVE_UNSET_HOVER) {
        return opcode.SMSG_SPLINE_MOVE_UNSET_HOVER.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPLINE_MOVE_WATER_WALK) {
        return opcode.SMSG_SPLINE_MOVE_WATER_WALK.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPLINE_MOVE_LAND_WALK) {
        return opcode.SMSG_SPLINE_MOVE_LAND_WALK.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPLINE_MOVE_START_SWIM) {
        return opcode.SMSG_SPLINE_MOVE_START_SWIM.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPLINE_MOVE_STOP_SWIM) {
        return opcode.SMSG_SPLINE_MOVE_STOP_SWIM.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPLINE_MOVE_SET_RUN_MODE) {
        return opcode.SMSG_SPLINE_MOVE_SET_RUN_MODE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPLINE_MOVE_SET_WALK_MODE) {
        return opcode.SMSG_SPLINE_MOVE_SET_WALK_MODE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_MOVE_TIME_SKIPPED) {
        return opcode.MSG_MOVE_TIME_SKIPPED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPLINE_MOVE_ROOT) {
        return opcode.SMSG_SPLINE_MOVE_ROOT.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_INVALIDATE_PLAYER) {
        return opcode.SMSG_INVALIDATE_PLAYER.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_INSTANCE_RESET) {
        return opcode.SMSG_INSTANCE_RESET.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_INSTANCE_RESET_FAILED) {
        return opcode.SMSG_INSTANCE_RESET_FAILED.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_UPDATE_LAST_INSTANCE) {
        return opcode.SMSG_UPDATE_LAST_INSTANCE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::MSG_RAID_TARGET_UPDATE) {
        return opcode.MSG_RAID_TARGET_UPDATE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PET_ACTION_SOUND) {
        return opcode.SMSG_PET_ACTION_SOUND.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_PET_DISMISS_SOUND) {
        return opcode.SMSG_PET_DISMISS_SOUND.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_GM_TICKET_STATUS_UPDATE) {
        return opcode.SMSG_GM_TICKET_STATUS_UPDATE.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_UPDATE_INSTANCE_OWNERSHIP) {
        return opcode.SMSG_UPDATE_INSTANCE_OWNERSHIP.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPELLINSTAKILLLOG) {
        return opcode.SMSG_SPELLINSTAKILLLOG.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_SPELL_UPDATE_CHAIN_TARGETS) {
        return opcode.SMSG_SPELL_UPDATE_CHAIN_TARGETS.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_EXPECTED_SPAM_RECORDS) {
        return opcode.SMSG_EXPECTED_SPAM_RECORDS.write();;
    }
    if (opcode.opcode == ServerOpcode::Opcode::SMSG_DEFENSE_MESSAGE) {
        return opcode.SMSG_DEFENSE_MESSAGE.write();;
    }

    return {}; /* unreachable */
}

ServerOpcode read_server_opcode(Reader& reader) {
    const uint16_t _size = reader.read_u16();
    const uint16_t opcode = reader.read_u16();

    ServerOpcode op;

    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_CHAR_CREATE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_CHAR_CREATE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_CHAR_ENUM)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_CHAR_ENUM_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_CHAR_DELETE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_CHAR_DELETE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_NEW_WORLD)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_NEW_WORLD_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_TRANSFER_ABORTED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_TRANSFER_ABORTED_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_CHARACTER_LOGIN_FAILED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_CHARACTER_LOGIN_FAILED_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_LOGIN_SETTIMESPEED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_LOGIN_SETTIMESPEED_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_LOGOUT_RESPONSE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_LOGOUT_RESPONSE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_LOGOUT_COMPLETE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_LOGOUT_COMPLETE{});
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_LOGOUT_CANCEL_ACK)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_LOGOUT_CANCEL_ACK{});
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_NAME_QUERY_RESPONSE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_NAME_QUERY_RESPONSE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PET_NAME_QUERY_RESPONSE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PET_NAME_QUERY_RESPONSE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_GUILD_QUERY_RESPONSE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_GUILD_QUERY_RESPONSE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PAGE_TEXT_QUERY_RESPONSE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PAGE_TEXT_QUERY_RESPONSE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_QUEST_QUERY_RESPONSE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_QUEST_QUERY_RESPONSE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_WHO)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_WHO_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_WHOIS)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_WHOIS_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_FRIEND_LIST)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_FRIEND_LIST_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_FRIEND_STATUS)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_FRIEND_STATUS_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_IGNORE_LIST)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_IGNORE_LIST_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_GROUP_INVITE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_GROUP_INVITE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_GROUP_DECLINE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_GROUP_DECLINE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_GROUP_UNINVITE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_GROUP_UNINVITE{});
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_GROUP_SET_LEADER)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_GROUP_SET_LEADER_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_GROUP_DESTROYED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_GROUP_DESTROYED{});
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PARTY_MEMBER_STATS)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PARTY_MEMBER_STATS_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PARTY_COMMAND_RESULT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PARTY_COMMAND_RESULT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_GUILD_INVITE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_GUILD_INVITE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_GUILD_INFO)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_GUILD_INFO_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_GUILD_ROSTER)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_GUILD_ROSTER_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_GUILD_EVENT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_GUILD_EVENT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_GUILD_COMMAND_RESULT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_GUILD_COMMAND_RESULT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_MESSAGECHAT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_MESSAGECHAT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_CHANNEL_NOTIFY)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_CHANNEL_NOTIFY_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_CHANNEL_LIST)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_CHANNEL_LIST_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_UPDATE_OBJECT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_UPDATE_OBJECT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_DESTROY_OBJECT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_DESTROY_OBJECT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_READ_ITEM_OK)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_READ_ITEM_OK_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_READ_ITEM_FAILED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_READ_ITEM_FAILED_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_ITEM_COOLDOWN)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_ITEM_COOLDOWN_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_GAMEOBJECT_CUSTOM_ANIM)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_GAMEOBJECT_CUSTOM_ANIM_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_MOVE_START_FORWARD)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_MOVE_START_FORWARD_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_MOVE_START_BACKWARD)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_MOVE_START_BACKWARD_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_MOVE_STOP)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_MOVE_STOP_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_MOVE_START_STRAFE_LEFT)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_MOVE_START_STRAFE_LEFT_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_MOVE_START_STRAFE_RIGHT)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_MOVE_START_STRAFE_RIGHT_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_MOVE_STOP_STRAFE)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_MOVE_STOP_STRAFE_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_MOVE_JUMP)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_MOVE_JUMP_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_MOVE_START_TURN_LEFT)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_MOVE_START_TURN_LEFT_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_MOVE_START_TURN_RIGHT)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_MOVE_START_TURN_RIGHT_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_MOVE_STOP_TURN)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_MOVE_STOP_TURN_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_MOVE_START_PITCH_UP)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_MOVE_START_PITCH_UP_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_MOVE_START_PITCH_DOWN)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_MOVE_START_PITCH_DOWN_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_MOVE_STOP_PITCH)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_MOVE_STOP_PITCH_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_MOVE_SET_RUN_MODE)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_MOVE_SET_RUN_MODE_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_MOVE_SET_WALK_MODE)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_MOVE_SET_WALK_MODE_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_MOVE_TELEPORT_ACK)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_MOVE_TELEPORT_ACK_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_MOVE_FALL_LAND)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_MOVE_FALL_LAND_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_MOVE_START_SWIM)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_MOVE_START_SWIM_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_MOVE_STOP_SWIM)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_MOVE_STOP_SWIM_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_MOVE_SET_FACING)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_MOVE_SET_FACING_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_MOVE_SET_PITCH)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_MOVE_SET_PITCH_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_MONSTER_MOVE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_MONSTER_MOVE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_MOVE_WATER_WALK)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_MOVE_WATER_WALK_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_MOVE_LAND_WALK)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_MOVE_LAND_WALK_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_FORCE_RUN_SPEED_CHANGE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_FORCE_RUN_SPEED_CHANGE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_FORCE_RUN_BACK_SPEED_CHANGE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_FORCE_RUN_BACK_SPEED_CHANGE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_FORCE_SWIM_SPEED_CHANGE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_FORCE_SWIM_SPEED_CHANGE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_FORCE_MOVE_ROOT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_FORCE_MOVE_ROOT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_FORCE_MOVE_UNROOT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_FORCE_MOVE_UNROOT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_MOVE_HEARTBEAT)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_MOVE_HEARTBEAT_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_MOVE_KNOCK_BACK)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_MOVE_KNOCK_BACK_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_MOVE_FEATHER_FALL)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_MOVE_FEATHER_FALL_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_MOVE_NORMAL_FALL)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_MOVE_NORMAL_FALL_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_MOVE_SET_HOVER)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_MOVE_SET_HOVER_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_MOVE_UNSET_HOVER)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_MOVE_UNSET_HOVER_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_TRIGGER_CINEMATIC)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_TRIGGER_CINEMATIC_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_TUTORIAL_FLAGS)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_TUTORIAL_FLAGS_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_EMOTE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_EMOTE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_TEXT_EMOTE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_TEXT_EMOTE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_INVENTORY_CHANGE_FAILURE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_INVENTORY_CHANGE_FAILURE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_OPEN_CONTAINER)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_OPEN_CONTAINER_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_INSPECT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_INSPECT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_TRADE_STATUS)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_TRADE_STATUS_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_TRADE_STATUS_EXTENDED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_TRADE_STATUS_EXTENDED_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_INITIALIZE_FACTIONS)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_INITIALIZE_FACTIONS_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SET_FACTION_VISIBLE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SET_FACTION_VISIBLE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SET_FACTION_STANDING)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SET_FACTION_STANDING_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SET_PROFICIENCY)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SET_PROFICIENCY_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_ACTION_BUTTONS)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_ACTION_BUTTONS_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_INITIAL_SPELLS)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_INITIAL_SPELLS_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_LEARNED_SPELL)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_LEARNED_SPELL_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SUPERCEDED_SPELL)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SUPERCEDED_SPELL_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_CAST_RESULT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_CAST_RESULT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPELL_START)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPELL_START_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPELL_GO)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPELL_GO_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPELL_FAILURE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPELL_FAILURE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPELL_COOLDOWN)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPELL_COOLDOWN_read(reader, _size - 2));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_COOLDOWN_EVENT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_COOLDOWN_EVENT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_UPDATE_AURA_DURATION)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_UPDATE_AURA_DURATION_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PET_CAST_FAILED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PET_CAST_FAILED_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_CHANNEL_START)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_CHANNEL_START_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_CHANNEL_UPDATE)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_CHANNEL_UPDATE_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_AI_REACTION)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_AI_REACTION_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_ATTACKSTART)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_ATTACKSTART_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_ATTACKSTOP)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_ATTACKSTOP_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_ATTACKSWING_NOTINRANGE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_ATTACKSWING_NOTINRANGE{});
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_ATTACKSWING_BADFACING)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_ATTACKSWING_BADFACING{});
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_ATTACKSWING_NOTSTANDING)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_ATTACKSWING_NOTSTANDING{});
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_ATTACKSWING_DEADTARGET)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_ATTACKSWING_DEADTARGET{});
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_ATTACKSWING_CANT_ATTACK)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_ATTACKSWING_CANT_ATTACK{});
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_ATTACKERSTATEUPDATE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_ATTACKERSTATEUPDATE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_CANCEL_COMBAT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_CANCEL_COMBAT{});
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPELLHEALLOG)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPELLHEALLOG_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPELLENERGIZELOG)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPELLENERGIZELOG_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_BINDPOINTUPDATE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_BINDPOINTUPDATE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PLAYERBOUND)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PLAYERBOUND_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_CLIENT_CONTROL_UPDATE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_CLIENT_CONTROL_UPDATE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_RESURRECT_REQUEST)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_RESURRECT_REQUEST_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_LOOT_RESPONSE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_LOOT_RESPONSE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_LOOT_RELEASE_RESPONSE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_LOOT_RELEASE_RESPONSE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_LOOT_REMOVED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_LOOT_REMOVED_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_LOOT_MONEY_NOTIFY)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_LOOT_MONEY_NOTIFY_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_LOOT_CLEAR_MONEY)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_LOOT_CLEAR_MONEY{});
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_ITEM_PUSH_RESULT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_ITEM_PUSH_RESULT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_DUEL_REQUESTED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_DUEL_REQUESTED_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_DUEL_OUTOFBOUNDS)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_DUEL_OUTOFBOUNDS{});
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_DUEL_INBOUNDS)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_DUEL_INBOUNDS{});
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_DUEL_COMPLETE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_DUEL_COMPLETE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_DUEL_WINNER)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_DUEL_WINNER_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_MOUNTRESULT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_MOUNTRESULT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_DISMOUNTRESULT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_DISMOUNTRESULT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_MOUNTSPECIAL_ANIM)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_MOUNTSPECIAL_ANIM_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PET_TAME_FAILURE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PET_TAME_FAILURE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PET_NAME_INVALID)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PET_NAME_INVALID{});
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PET_MODE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PET_MODE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_GOSSIP_MESSAGE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_GOSSIP_MESSAGE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_GOSSIP_COMPLETE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_GOSSIP_COMPLETE{});
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_NPC_TEXT_UPDATE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_NPC_TEXT_UPDATE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_QUESTGIVER_STATUS)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_QUESTGIVER_STATUS_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_QUESTGIVER_QUEST_LIST)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_QUESTGIVER_QUEST_LIST_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_QUESTGIVER_QUEST_DETAILS)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_QUESTGIVER_QUEST_DETAILS_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_QUESTGIVER_REQUEST_ITEMS)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_QUESTGIVER_REQUEST_ITEMS_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_QUESTGIVER_OFFER_REWARD)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_QUESTGIVER_OFFER_REWARD_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_QUESTGIVER_QUEST_INVALID)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_QUESTGIVER_QUEST_INVALID_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_QUESTGIVER_QUEST_COMPLETE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_QUESTGIVER_QUEST_COMPLETE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_QUESTGIVER_QUEST_FAILED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_QUESTGIVER_QUEST_FAILED_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_QUESTLOG_FULL)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_QUESTLOG_FULL{});
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_QUESTUPDATE_FAILED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_QUESTUPDATE_FAILED_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_QUESTUPDATE_FAILEDTIMER)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_QUESTUPDATE_FAILEDTIMER_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_QUESTUPDATE_COMPLETE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_QUESTUPDATE_COMPLETE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_QUESTUPDATE_ADD_KILL)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_QUESTUPDATE_ADD_KILL_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_QUESTUPDATE_ADD_ITEM)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_QUESTUPDATE_ADD_ITEM_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_QUEST_CONFIRM_ACCEPT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_QUEST_CONFIRM_ACCEPT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_LIST_INVENTORY)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_LIST_INVENTORY_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SELL_ITEM)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SELL_ITEM_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_BUY_ITEM)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_BUY_ITEM_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_BUY_FAILED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_BUY_FAILED_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SHOWTAXINODES)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SHOWTAXINODES_read(reader, _size - 2));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_TAXINODE_STATUS)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_TAXINODE_STATUS_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_ACTIVATETAXIREPLY)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_ACTIVATETAXIREPLY_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_NEW_TAXI_PATH)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_NEW_TAXI_PATH{});
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_TRAINER_LIST)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_TRAINER_LIST_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_TRAINER_BUY_SUCCEEDED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_TRAINER_BUY_SUCCEEDED_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_TRAINER_BUY_FAILED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_TRAINER_BUY_FAILED_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SHOW_BANK)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SHOW_BANK_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_BUY_BANK_SLOT_RESULT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_BUY_BANK_SLOT_RESULT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PETITION_SHOWLIST)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PETITION_SHOWLIST_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PETITION_SHOW_SIGNATURES)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PETITION_SHOW_SIGNATURES_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PETITION_SIGN_RESULTS)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PETITION_SIGN_RESULTS_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_TURN_IN_PETITION_RESULTS)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_TURN_IN_PETITION_RESULTS_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PETITION_QUERY_RESPONSE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PETITION_QUERY_RESPONSE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_FISH_NOT_HOOKED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_FISH_NOT_HOOKED{});
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_FISH_ESCAPED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_FISH_ESCAPED{});
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_NOTIFICATION)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_NOTIFICATION_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PLAYED_TIME)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PLAYED_TIME_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_QUERY_TIME_RESPONSE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_QUERY_TIME_RESPONSE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_LOG_XPGAIN)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_LOG_XPGAIN_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_LEVELUP_INFO)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_LEVELUP_INFO_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_MINIMAP_PING)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_MINIMAP_PING_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_RESISTLOG)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_RESISTLOG_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_ENCHANTMENTLOG)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_ENCHANTMENTLOG_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_START_MIRROR_TIMER)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_START_MIRROR_TIMER_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PAUSE_MIRROR_TIMER)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PAUSE_MIRROR_TIMER_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_STOP_MIRROR_TIMER)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_STOP_MIRROR_TIMER_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PONG)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PONG_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_CLEAR_COOLDOWN)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_CLEAR_COOLDOWN_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_GAMEOBJECT_PAGETEXT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_GAMEOBJECT_PAGETEXT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPELL_DELAYED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPELL_DELAYED_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_ITEM_TIME_UPDATE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_ITEM_TIME_UPDATE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_ITEM_ENCHANT_TIME_UPDATE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_ITEM_ENCHANT_TIME_UPDATE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_AUTH_CHALLENGE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_AUTH_CHALLENGE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_AUTH_RESPONSE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_AUTH_RESPONSE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_SAVE_GUILD_EMBLEM)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_SAVE_GUILD_EMBLEM_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PLAY_SPELL_VISUAL)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PLAY_SPELL_VISUAL_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PARTYKILLLOG)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PARTYKILLLOG_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PLAY_SPELL_IMPACT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PLAY_SPELL_IMPACT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_EXPLORATION_EXPERIENCE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_EXPLORATION_EXPERIENCE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_RANDOM_ROLL)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_RANDOM_ROLL_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_ENVIRONMENTAL_DAMAGE_LOG)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_ENVIRONMENTAL_DAMAGE_LOG_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_LOOKING_FOR_GROUP)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_LOOKING_FOR_GROUP_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_REMOVED_SPELL)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_REMOVED_SPELL_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_GMTICKET_CREATE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_GMTICKET_CREATE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_GMTICKET_UPDATETEXT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_GMTICKET_UPDATETEXT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_ACCOUNT_DATA_TIMES)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_ACCOUNT_DATA_TIMES_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_GMTICKET_GETTICKET)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_GMTICKET_GETTICKET_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_GAMEOBJECT_SPAWN_ANIM)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_GAMEOBJECT_SPAWN_ANIM_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_GAMEOBJECT_DESPAWN_ANIM)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_GAMEOBJECT_DESPAWN_ANIM_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_CORPSE_QUERY)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_CORPSE_QUERY_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_GMTICKET_DELETETICKET)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_GMTICKET_DELETETICKET_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_CHAT_WRONG_FACTION)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_CHAT_WRONG_FACTION{});
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_GMTICKET_SYSTEMSTATUS)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_GMTICKET_SYSTEMSTATUS_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SET_REST_START)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SET_REST_START_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPIRIT_HEALER_CONFIRM)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPIRIT_HEALER_CONFIRM_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_GOSSIP_POI)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_GOSSIP_POI_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_LOGIN_VERIFY_WORLD)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_LOGIN_VERIFY_WORLD_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SEND_MAIL_RESULT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SEND_MAIL_RESULT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_MAIL_LIST_RESULT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_MAIL_LIST_RESULT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_BATTLEFIELD_LIST)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_BATTLEFIELD_LIST_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_ITEM_TEXT_QUERY_RESPONSE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_ITEM_TEXT_QUERY_RESPONSE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPELLLOGMISS)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPELLLOGMISS_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPELLLOGEXECUTE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPELLLOGEXECUTE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PERIODICAURALOG)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PERIODICAURALOG_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPELLDAMAGESHIELD)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPELLDAMAGESHIELD_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPELLNONMELEEDAMAGELOG)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPELLNONMELEEDAMAGELOG_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_ZONE_UNDER_ATTACK)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_ZONE_UNDER_ATTACK_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_AUCTION_HELLO)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_AUCTION_HELLO_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_AUCTION_COMMAND_RESULT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_AUCTION_COMMAND_RESULT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_AUCTION_LIST_RESULT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_AUCTION_LIST_RESULT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_AUCTION_OWNER_LIST_RESULT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_AUCTION_OWNER_LIST_RESULT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_AUCTION_BIDDER_NOTIFICATION)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_AUCTION_BIDDER_NOTIFICATION_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_AUCTION_OWNER_NOTIFICATION)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_AUCTION_OWNER_NOTIFICATION_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PROCRESIST)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PROCRESIST_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_DISPEL_FAILED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_DISPEL_FAILED_read(reader, _size - 2));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPELLORDAMAGE_IMMUNE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPELLORDAMAGE_IMMUNE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_AUCTION_BIDDER_LIST_RESULT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_AUCTION_BIDDER_LIST_RESULT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SET_FLAT_SPELL_MODIFIER)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SET_FLAT_SPELL_MODIFIER_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SET_PCT_SPELL_MODIFIER)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SET_PCT_SPELL_MODIFIER_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_CORPSE_RECLAIM_DELAY)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_CORPSE_RECLAIM_DELAY_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_LIST_STABLED_PETS)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_LIST_STABLED_PETS_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_STABLE_RESULT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_STABLE_RESULT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PLAY_MUSIC)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PLAY_MUSIC_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PLAY_OBJECT_SOUND)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PLAY_OBJECT_SOUND_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPELLDISPELLOG)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPELLDISPELLOG_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_QUERY_NEXT_MAIL_TIME)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_QUERY_NEXT_MAIL_TIME_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_RECEIVED_MAIL)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_RECEIVED_MAIL_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_RAID_GROUP_ONLY)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_RAID_GROUP_ONLY_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PVP_CREDIT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PVP_CREDIT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_AUCTION_REMOVED_NOTIFICATION)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_AUCTION_REMOVED_NOTIFICATION_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SERVER_MESSAGE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SERVER_MESSAGE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_MEETINGSTONE_SETQUEUE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_MEETINGSTONE_SETQUEUE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_MEETINGSTONE_COMPLETE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_MEETINGSTONE_COMPLETE{});
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_MEETINGSTONE_IN_PROGRESS)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_MEETINGSTONE_IN_PROGRESS{});
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_MEETINGSTONE_MEMBER_ADDED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_MEETINGSTONE_MEMBER_ADDED_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_CANCEL_AUTO_REPEAT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_CANCEL_AUTO_REPEAT{});
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_STANDSTATE_UPDATE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_STANDSTATE_UPDATE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_LOOT_ALL_PASSED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_LOOT_ALL_PASSED_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_LOOT_ROLL_WON)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_LOOT_ROLL_WON_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_LOOT_START_ROLL)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_LOOT_START_ROLL_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_LOOT_ROLL)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_LOOT_ROLL_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_LOOT_MASTER_LIST)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_LOOT_MASTER_LIST_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SET_FORCED_REACTIONS)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SET_FORCED_REACTIONS_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPELL_FAILED_OTHER)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPELL_FAILED_OTHER_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_GAMEOBJECT_RESET_STATE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_GAMEOBJECT_RESET_STATE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_CHAT_PLAYER_NOT_FOUND)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_CHAT_PLAYER_NOT_FOUND_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_TALENT_WIPE_CONFIRM)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_TALENT_WIPE_CONFIRM_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SUMMON_REQUEST)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SUMMON_REQUEST_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_MONSTER_MOVE_TRANSPORT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_MONSTER_MOVE_TRANSPORT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PET_BROKEN)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PET_BROKEN{});
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_MOVE_FEATHER_FALL)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_MOVE_FEATHER_FALL_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_FEIGN_DEATH_RESISTED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_FEIGN_DEATH_RESISTED{});
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_DUEL_COUNTDOWN)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_DUEL_COUNTDOWN_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_AREA_TRIGGER_MESSAGE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_AREA_TRIGGER_MESSAGE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_MEETINGSTONE_JOINFAILED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_MEETINGSTONE_JOINFAILED_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PLAYER_SKINNED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PLAYER_SKINNED_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_DURABILITY_DAMAGE_DEATH)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_DURABILITY_DAMAGE_DEATH{});
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_INIT_WORLD_STATES)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_INIT_WORLD_STATES_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_UPDATE_WORLD_STATE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_UPDATE_WORLD_STATE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_ITEM_NAME_QUERY_RESPONSE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_ITEM_NAME_QUERY_RESPONSE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PET_ACTION_FEEDBACK)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PET_ACTION_FEEDBACK_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_CHAR_RENAME)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_CHAR_RENAME_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_INSTANCE_SAVE_CREATED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_INSTANCE_SAVE_CREATED_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_RAID_INSTANCE_INFO)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_RAID_INSTANCE_INFO_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PLAY_SOUND)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PLAY_SOUND_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_BATTLEFIELD_STATUS)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_BATTLEFIELD_STATUS_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_INSPECT_HONOR_STATS)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_INSPECT_HONOR_STATS_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_FORCE_WALK_SPEED_CHANGE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_FORCE_WALK_SPEED_CHANGE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_FORCE_SWIM_BACK_SPEED_CHANGE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_FORCE_SWIM_BACK_SPEED_CHANGE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_FORCE_TURN_RATE_CHANGE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_FORCE_TURN_RATE_CHANGE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_PVP_LOG_DATA)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_PVP_LOG_DATA_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_AREA_SPIRIT_HEALER_TIME)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_AREA_SPIRIT_HEALER_TIME_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_WARDEN_DATA)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_WARDEN_DATA_read(reader, _size - 2));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_GROUP_JOINED_BATTLEGROUND)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_GROUP_JOINED_BATTLEGROUND_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_BATTLEGROUND_PLAYER_POSITIONS)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_BATTLEGROUND_PLAYER_POSITIONS_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_BINDER_CONFIRM)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_BINDER_CONFIRM_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_BATTLEGROUND_PLAYER_JOINED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_BATTLEGROUND_PLAYER_JOINED_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_BATTLEGROUND_PLAYER_LEFT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_BATTLEGROUND_PLAYER_LEFT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_ADDON_INFO)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_ADDON_INFO_read(reader, _size - 2));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PET_UNLEARN_CONFIRM)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PET_UNLEARN_CONFIRM_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PARTY_MEMBER_STATS_FULL)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PARTY_MEMBER_STATS_FULL_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_WEATHER)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_WEATHER_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_RAID_INSTANCE_MESSAGE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_RAID_INSTANCE_MESSAGE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_CHAT_RESTRICTED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_CHAT_RESTRICTED{});
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPLINE_SET_RUN_SPEED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPLINE_SET_RUN_SPEED_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPLINE_SET_RUN_BACK_SPEED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPLINE_SET_RUN_BACK_SPEED_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPLINE_SET_SWIM_SPEED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPLINE_SET_SWIM_SPEED_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPLINE_SET_WALK_SPEED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPLINE_SET_WALK_SPEED_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPLINE_SET_SWIM_BACK_SPEED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPLINE_SET_SWIM_BACK_SPEED_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPLINE_SET_TURN_RATE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPLINE_SET_TURN_RATE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPLINE_MOVE_UNROOT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPLINE_MOVE_UNROOT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPLINE_MOVE_FEATHER_FALL)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPLINE_MOVE_FEATHER_FALL_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPLINE_MOVE_NORMAL_FALL)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPLINE_MOVE_NORMAL_FALL_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPLINE_MOVE_SET_HOVER)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPLINE_MOVE_SET_HOVER_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPLINE_MOVE_UNSET_HOVER)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPLINE_MOVE_UNSET_HOVER_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPLINE_MOVE_WATER_WALK)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPLINE_MOVE_WATER_WALK_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPLINE_MOVE_LAND_WALK)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPLINE_MOVE_LAND_WALK_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPLINE_MOVE_START_SWIM)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPLINE_MOVE_START_SWIM_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPLINE_MOVE_STOP_SWIM)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPLINE_MOVE_STOP_SWIM_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPLINE_MOVE_SET_RUN_MODE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPLINE_MOVE_SET_RUN_MODE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPLINE_MOVE_SET_WALK_MODE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPLINE_MOVE_SET_WALK_MODE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_MOVE_TIME_SKIPPED)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_MOVE_TIME_SKIPPED_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPLINE_MOVE_ROOT)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPLINE_MOVE_ROOT_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_INVALIDATE_PLAYER)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_INVALIDATE_PLAYER_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_INSTANCE_RESET)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_INSTANCE_RESET_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_INSTANCE_RESET_FAILED)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_INSTANCE_RESET_FAILED_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_UPDATE_LAST_INSTANCE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_UPDATE_LAST_INSTANCE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::MSG_RAID_TARGET_UPDATE)) {
        return ServerOpcode(::wow_world_messages::vanilla::MSG_RAID_TARGET_UPDATE_Server_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PET_ACTION_SOUND)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PET_ACTION_SOUND_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_PET_DISMISS_SOUND)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_PET_DISMISS_SOUND_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_GM_TICKET_STATUS_UPDATE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_GM_TICKET_STATUS_UPDATE_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_UPDATE_INSTANCE_OWNERSHIP)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_UPDATE_INSTANCE_OWNERSHIP_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPELLINSTAKILLLOG)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPELLINSTAKILLLOG_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_SPELL_UPDATE_CHAIN_TARGETS)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_SPELL_UPDATE_CHAIN_TARGETS_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_EXPECTED_SPAM_RECORDS)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_EXPECTED_SPAM_RECORDS_read(reader));
    }
    if (opcode == static_cast<uint16_t>(ServerOpcode::Opcode::SMSG_DEFENSE_MESSAGE)) {
        return ServerOpcode(::wow_world_messages::vanilla::SMSG_DEFENSE_MESSAGE_read(reader));
    }

    return op;
}
} // namespace vanilla
} // namespace wow_world_messages
