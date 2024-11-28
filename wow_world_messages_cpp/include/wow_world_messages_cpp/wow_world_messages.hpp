#ifndef WWM_WOW_WORLD_MESSAGES_CPP_H
#define WWM_WOW_WORLD_MESSAGES_CPP_H


#include <array>
#include <cstring>
#include <stddef.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <memory>

#include "wow_world_messages_cpp/all.hpp"

#include "wow_world_messages_cpp_export.h"

namespace wow_world_messages {

class Reader
{
public:
    virtual uint8_t read_u8() = 0;

    virtual uint16_t read_u16()
    {
        const uint8_t lower = read_u8();
        const uint8_t upper = read_u8();

        return static_cast<uint16_t>(lower) | static_cast<uint16_t>(upper) << 8;
    }

    virtual uint16_t read_u16_be()
    {
        const uint8_t upper = read_u8();
        const uint8_t lower = read_u8();

        return static_cast<uint16_t>(lower) | static_cast<uint16_t>(upper) << 8;
    }

    virtual uint32_t read_u32()
    {
        const uint16_t lower = read_u16();
        const uint16_t upper = read_u16();

        return static_cast<uint32_t>(lower) | static_cast<uint32_t>(upper) << 16;
    }

    virtual uint64_t read_u64()
    {
        const uint32_t lower = read_u32();
        const uint32_t upper = read_u32();

        return static_cast<uint64_t>(lower) | static_cast<uint64_t>(upper) << 32;
    }

    virtual int32_t read_i32()
    {
        auto v = read_u32();
        int32_t value;
        std::memcpy(&value, &v, 4);
        return value;
    }

    virtual float read_float()
    {
        auto v = read_u32();
        float value;
        std::memcpy(&value, &v, 4);

        return value;
    }

    virtual bool read_bool8() { return read_u8() == 1; }

    virtual bool read_bool32() { return read_u32() == 1; }

    virtual std::string read_string()
    {
        const uint8_t length = read_u8();
        std::string str;

        for (uint8_t i = 0; i < length; ++i)
        {
            str.push_back(static_cast<char>(read_u8()));
        }

        return str;
    }

    virtual std::string read_cstring()
    {
        std::string str;

        uint8_t val = read_u8();
        while (val != 0)
        {
            str.push_back(static_cast<char>(val));
            val = read_u8();
        }

        return str;
    }

    virtual std::string read_sized_cstring()
    {
        const uint32_t length = read_u32();
        (void)length; /* this could be used in a specialized method */
        return read_cstring();
    }

    virtual uint64_t read_packed_guid()
    {
        uint64_t value = 0;

        const auto header = read_u8();

        for (int i = 0; i < 8; ++i)
        {
            const bool byte_has_value = (header & (1 << i)) != 0;
            if (byte_has_value)
            {
                value |= read_u8() << i * 8;
            }
        }

        return value;
    }

    virtual ~Reader() = default;
};

struct bad_opcode_access final : std::exception {};

} /* namespace wow_world_messages */

#endif /* WWM_WOW_WORLD_MESSAGES_CPP_H */
