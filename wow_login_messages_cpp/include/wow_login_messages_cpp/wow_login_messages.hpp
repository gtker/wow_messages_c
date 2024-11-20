#ifndef WOW_LOGIN_MESSAGES_H
#define WOW_LOGIN_MESSAGES_H

#include <stddef.h>
#include <stdint.h>
#include <string>
#include <array>
#include <vector>
#include <variant>
#include <optional>

#include "wow_login_messages_cpp_export.h"

namespace wow_login_messages {

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
        return read_u32();
    }

    virtual float read_float()
    {
        return read_u32();
    }

    virtual bool read_bool8()
    {
        return read_u8() == 1;
    }

    virtual std::string read_string()
    {
        const uint8_t length = read_u8();
        std::string str;

        for(uint8_t i = 0; i < length; ++i)
        {
            str.push_back(read_u8());
        }

        return str;
    }

    virtual std::string read_cstring()
    {
        std::string str;

        uint8_t val = read_u8();
        while(val != 0)
        {
            str.push_back(val);
            val = read_u8();
        }

        return str;
    }

    virtual ~Reader() = default;
};

} /* namespace wow_login_messages */
#endif /* WOW_LOGIN_MESSAGES_H */
