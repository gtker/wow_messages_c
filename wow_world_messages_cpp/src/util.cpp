#include "util.hpp"

namespace wow_world_messages {
namespace all {
::wow_world_messages::all::Vector3d Vector3d_read(Reader& reader);
void Vector3d_write(Writer& writer, const ::wow_world_messages::all::Vector3d& obj);
}  // namespace all
}  // namespace wow_world_messages

namespace wow_world_messages {
namespace util {

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

size_t wwm_monster_move_spline_size(const std::vector<all::Vector3d>& splines)
{
    if (splines.empty())
    {
        return 4; /* header uint32_t */
    }

    return 4 + 12 + (4 * (splines.size() - 1));
}

static uint32_t to_packed_vector3d(const all::Vector3d& v)
{
    uint32_t packed = 0;
    packed |= ((uint32_t)(v.x / 0.25f) & 0x7FF);
    packed |= ((uint32_t)(v.y / 0.25f) & 0x7FF) << 11;
    packed |= ((uint32_t)(v.z / 0.25f) & 0x3FF) << 22;
    return packed;
}

static all::Vector3d from_packed(uint32_t p)
{
    return {
        (float)((p & 0x7FF) / 4),
        (float)(((p >> 11) & 0x7FF) / 4),
        (float)(((p >> 22) & 0x3FF) / 4),
    };
}

void wwm_write_monster_move_spline(Writer& writer, const std::vector<all::Vector3d>& splines)
{
    writer.write_u32(static_cast<uint32_t>(splines.size()));

    if (splines.empty())
    {
        return;
    }

    ::wow_world_messages::all::Vector3d_write(writer, splines[0]);

    for (size_t i = 1; i < splines.size(); ++i)
    {
        const auto packed = to_packed_vector3d(splines[i]);
        writer.write_u32(packed);
    }
}

std::vector<all::Vector3d> wwm_read_monster_move_spline(Reader& reader)
{
    const auto amount_of_splines = reader.read_u32();

    if (amount_of_splines == 0)
    {
        return {};
    }

    auto splines = std::vector<all::Vector3d>{};
    splines.reserve(amount_of_splines);

    splines.push_back(all::Vector3d_read(reader));

    for (size_t i = 1; i < amount_of_splines; ++i)
    {
        uint32_t packed = reader.read_u32();
        splines.push_back(from_packed(packed));
    }

    return splines;
}

}  // namespace util
}  // namespace wow_world_messages