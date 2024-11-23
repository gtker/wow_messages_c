import model
from util import is_cpp
from writer import Writer


def print_update_mask(s: Writer, h: Writer, update_mask: list[model.UpdateMask], module_name: str):
    if module_name == "all":
        return

    if is_cpp():
        print_update_mask_cpp(s, h, update_mask, module_name)
    else:
        print_update_mask_c(s, h, update_mask, module_name)


def print_update_mask_cpp(s: Writer, h: Writer, update_mask: list[model.UpdateMask], module_name: str):
    h.open_curly("enum class UpdateMaskValues")

    highest_offset = 0
    for index, value in enumerate(update_mask):
        highest_offset = max(value.offset, highest_offset)

        comma = "," if index < (len(update_mask) - 1) else ""
        name = f"{value.object_type.name}_{value.name}"
        match value.data_type:
            case model.UpdateMaskDataTypeGUIDArrayUsingEnum(content=content):
                for enumerator in content.definer.enumerators:
                    h.wln(f"{name}_{enumerator.name} = {value.offset + int(enumerator.value.value) * 2}{comma}")
            case model.UpdateMaskDataTypeArrayOfStruct(content=content):
                amount_of_items = value.size // content.size
                mask = content.update_mask_struct
                for i in range(0, amount_of_items):
                    for word_index, word in enumerate(mask.members):
                        for member_index, member in enumerate(word):
                            val = value.offset + i * content.size + word_index
                            extra = member.member.name.upper()

                            if member.size == 4:
                                h.wln(f"{name}_{i}_{extra} = {val}{comma}")
                            elif member.size == 8:
                                h.wln(f"{name}_{i}_{extra}_LOW = {val}{comma}")
                                h.wln(f"{name}_{i}_{extra}_HIGH = {val + 1}{comma}")
                            else:
                                h.wln(f"{name}_{i}_{extra}_{member_index} = {val}{comma}")

            case _:
                h.wln(f"{name} = {value.offset}{comma}")

    h.closing_curly(";")
    h.newline()

    max_headers = highest_offset // 32
    if max_headers % 32 != 0:
        max_headers += 1
    max_values = highest_offset
    h.write_block(f"""
constexpr auto UPDATE_MASK_HEADERS_LENGTH = {max_headers};
constexpr auto UPDATE_MASK_VALUES_LENGTH = {max_values};
    
struct UpdateMask {{
    uint32_t headers[UPDATE_MASK_HEADERS_LENGTH];
    uint32_t values[UPDATE_MASK_VALUES_LENGTH];
}};

""")

    set_declaration = f"WOW_WORLD_MESSAGES_CPP_EXPORT void update_mask_set(UpdateMask& mask, UpdateMaskValues offset, uint32_t value)"
    h.wln(f"{set_declaration};")
    h.newline()

    get_declaration = f"WOW_WORLD_MESSAGES_CPP_EXPORT uint32_t update_mask_get(const UpdateMask& mask, UpdateMaskValues offset)"
    h.wln(f"{get_declaration};")
    h.newline()

    s.write_block(f"""
{set_declaration} {{
    uint32_t block = static_cast<uint32_t>(offset) / 32;
    uint32_t bit = static_cast<uint32_t>(offset) % 32;

    mask.headers[block] |= 1 << bit;
    mask.values[static_cast<uint32_t>(offset)] = value;
}}

{get_declaration} {{
    uint32_t block = static_cast<uint32_t>(offset) / 32;
    uint32_t bit = static_cast<uint32_t>(offset) % 32;
    
    if((mask.headers[block] & 1 << bit) != 0) {{
        return mask.values[static_cast<uint32_t>(offset)];
    }}

    return 0;
}}

static void update_mask_write(Writer& writer, const UpdateMask& mask) {{
    uint8_t amount_of_headers = 0;

    for (int i = 0; i < UPDATE_MASK_HEADERS_LENGTH; ++i) {{
        uint32_t header = mask.headers[i];
        if (header != 0) {{
            amount_of_headers = i + 1;
        }}
    }}

    writer.write_u8(amount_of_headers);

    for (int i = 0; i < amount_of_headers; ++i) {{
        writer.write_u32(mask.headers[i]);
    }}
    
    for (int i = 0; i < amount_of_headers; ++i) {{
        uint32_t header = mask.headers[i];
        for (int j = 0; j < 32; ++j) {{
            if ((header & (1 << j)) != 0) {{
                writer.write_u32(mask.values[i * 32 + j]);
            }}
        }}
    }}
}}

static UpdateMask update_mask_read(Reader& reader) {{
    UpdateMask mask{{}};

    uint8_t amount_of_headers = reader.read_u8();

    for (int i = 0; i < amount_of_headers; ++i) {{
        mask.headers[i] = reader.read_u32();
    }}

    for (int i = 0; i < amount_of_headers; ++i) {{
        uint32_t header = mask.headers[i];
        for (int j = 0; j < 32; ++j) {{
            if ((header & (1 << j)) != 0) {{
                mask.values[i * 32 + j] = reader.read_u32();
            }}
        }}
    }}
    
    return mask;
}}

static size_t update_mask_size(const UpdateMask& mask) {{
    size_t max_header = 0;
    size_t amount_of_values = 0;
    
    size_t size = 1; /* initial u8 */
    
    for(int i = 0; i < UPDATE_MASK_HEADERS_LENGTH; ++i) {{
        uint32_t header = mask.headers[i];
        for(int j = 0; j < 32; ++j) {{
            if((header & (1 << j)) != 0) {{
                max_header = i + 1;
                amount_of_values += 4;
            }}
        }}
    }}
    
    return size + amount_of_values + (max_header * 4);
}}
""")


def print_update_mask_c(s: Writer, h: Writer, update_mask: list[model.UpdateMask], module_name: str):
    h.open_curly("typedef enum")

    highest_offset = 0
    for index, value in enumerate(update_mask):
        highest_offset = max(value.offset, highest_offset)

        comma = "," if index < (len(update_mask) - 1) else ""
        name = f"{module_name.upper()}_{value.object_type.name}_{value.name}"
        match value.data_type:
            case model.UpdateMaskDataTypeGUIDArrayUsingEnum(content=content):
                for enumerator in content.definer.enumerators:
                    h.wln(f"{name}_{enumerator.name} = {value.offset + int(enumerator.value.value) * 2}{comma}")
            case model.UpdateMaskDataTypeArrayOfStruct(content=content):
                amount_of_items = value.size // content.size
                mask = content.update_mask_struct
                for i in range(0, amount_of_items):
                    for word_index, word in enumerate(mask.members):
                        for member_index, member in enumerate(word):
                            val = value.offset + i * content.size + word_index
                            extra = member.member.name.upper()

                            if member.size == 4:
                                h.wln(f"{name}_{i}_{extra} = {val}{comma}")
                            elif member.size == 8:
                                h.wln(f"{name}_{i}_{extra}_LOW = {val}{comma}")
                                h.wln(f"{name}_{i}_{extra}_HIGH = {val + 1}{comma}")
                            else:
                                h.wln(f"{name}_{i}_{extra}_{member_index} = {val}{comma}")

            case _:
                h.wln(f"{name} = {value.offset}{comma}")

    h.closing_curly(f" {module_name}_UpdateMaskValues;")
    h.newline()

    max_headers = highest_offset // 32
    if max_headers % 32 != 0:
        max_headers += 1
    max_values = highest_offset
    h.write_block(f"""
#define {module_name.upper()}_HEADERS_LENGTH {max_headers}
#define {module_name.upper()}_VALUES_LENGTH {max_values}
    
typedef struct {{
    uint32_t headers[{module_name.upper()}_HEADERS_LENGTH];
    uint32_t values[{module_name.upper()}_VALUES_LENGTH];
}} {module_name}_UpdateMask;

""")

    set_declaration = f"WOW_WORLD_MESSAGES_C_EXPORT void {module_name}_update_mask_set({module_name}_UpdateMask* mask, {module_name}_UpdateMaskValues offset, uint32_t value)"
    h.wln(f"{set_declaration};")
    h.newline()

    get_declaration = f"WOW_WORLD_MESSAGES_C_EXPORT uint32_t {module_name}_update_mask_get({module_name}_UpdateMask* mask, {module_name}_UpdateMaskValues offset)"
    h.wln(f"{get_declaration};")
    h.newline()

    s.write_block(f"""
{set_declaration} {{
    uint32_t block = offset / 32;
    uint32_t bit = offset % 32;

    mask->headers[block] |= 1 << bit;
    mask->values[offset] = value;
}}

{get_declaration} {{
    uint32_t block = offset / 32;
    uint32_t bit = offset % 32;
    
    if((mask->headers[block] & 1 << bit) != 0) {{
        return mask->values[offset];
    }}

    return 0;
}}

static WowWorldResult {module_name}_update_mask_write(WowWorldWriter* stream, const {module_name}_UpdateMask* mask) {{
    uint8_t i;
    uint8_t j;
    uint8_t amount_of_headers = 0;

    for (i = 0; i < {module_name.upper()}_HEADERS_LENGTH; ++i) {{
        uint32_t header = mask->headers[i];
        if (header != 0) {{
            amount_of_headers = i + 1;
        }}
    }}

    WWM_CHECK_RETURN_CODE(wwm_write_uint8(stream, amount_of_headers));

    for (i = 0; i < amount_of_headers; ++i) {{
        WWM_CHECK_RETURN_CODE(wwm_write_uint32(stream, mask->headers[i]));
    }}
    
    for (i = 0; i < amount_of_headers; ++i) {{
        uint32_t header = mask->headers[i];
        for (j = 0; j < 32; ++j) {{
            if ((header & (1 << j)) != 0) {{
                WWM_CHECK_RETURN_CODE(wwm_write_uint32(stream, mask->values[i * 32 + j]));
            }}
        }}
    }}

    return WWM_RESULT_SUCCESS;
}}

static WowWorldResult {module_name}_update_mask_read(WowWorldReader* stream, {module_name}_UpdateMask* mask) {{
    uint8_t i;
    uint8_t j;
    
    uint8_t amount_of_headers;

    memset(mask->headers, 0, sizeof(mask->headers));
    memset(mask->values, 0, sizeof(mask->values));

    WWM_CHECK_RETURN_CODE(wwm_read_uint8(stream, &amount_of_headers));

    for (i = 0; i < amount_of_headers; ++i) {{
        WWM_CHECK_RETURN_CODE(wwm_read_uint32(stream, &mask->headers[i]));
    }}

    for (i = 0; i < amount_of_headers; ++i) {{
        uint32_t header = mask->headers[i];
        for (j = 0; j < 32; ++j) {{
            if ((header & (1 << j)) != 0) {{
                WWM_CHECK_RETURN_CODE(wwm_read_uint32(stream, &mask->values[i * 32 + j]));
            }}
        }}
    }}
    
    return WWM_RESULT_SUCCESS;
}}

static size_t {module_name}_update_mask_size(const {module_name}_UpdateMask* mask) {{
    size_t i;
    size_t j;
    size_t max_header = 0;
    size_t amount_of_values = 0;
    
    size_t size = 1; /* initial u8 */
    
    for(i = 0; i < {module_name.upper()}_HEADERS_LENGTH; ++i) {{
        uint32_t header = mask->headers[i];
        for(j = 0; j < 32; ++j) {{
            if((header & (1 << j)) != 0) {{
                max_header = i + 1;
                amount_of_values += 4;
            }}
        }}
    }}
    
    return size + amount_of_values + (max_header * 4);
}}
""")

    nothing = """
@dataclasses.dataclass
class UpdateMask:
    fields: dict[int, int]
    
    @staticmethod
    async def read(reader: asyncio.StreamReader):
        amount_of_blocks = await read_int(reader, 1)

        blocks = []
        for _ in range(0, amount_of_blocks):
            blocks.append(await read_int(reader, 4))

        fields = {}
        for block_index, block in enumerate(blocks):
            for bit in range(0, 32):
                if block & 1 << bit:
                    value = await read_int(reader, 4)
                    key = block_index * 32 + bit
                    fields[key] = value

        return UpdateMask(fields=fields)

    def write(self, fmt, data):
        highest_key = max(self.fields, default=0)
        amount_of_blocks = highest_key // 32
        if highest_key % 32 != 0:
            amount_of_blocks += 1

        fmt += 'B'
        data.append(amount_of_blocks)

        blocks = [0] * amount_of_blocks

        for key in self.fields:
            block = key // 32
            index = key % 32
            blocks[block] |= 1 << index

        fmt += f'{len(blocks)}I'
        data.extend(blocks)

        for key in sorted(self.fields):
            if isinstance(self.fields[key], float):
                fmt += 'f'
            else:
                fmt += 'I'
            data.append(self.fields[key])

        return fmt, data

    def size(self):
        highest_key = max(self.fields, default=0)
        amount_of_blocks = highest_key // 32

        extra = highest_key % 32
        if extra != 0:
            extra = 1
        else:
            extra = 0

        return 1 + (extra + amount_of_blocks + len(self.fields)) * 4
"""
