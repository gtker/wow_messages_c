from util import world_version_is_vanilla, world_version_is_tbc, \
    world_version_is_wrath, world_version_is_all, is_cpp
from writer import Writer
import model


def print_aura_mask(s: Writer, h: Writer, v: model.WorldVersion):
    if is_cpp():
        print_aura_mask_cpp(s, h, v)
    else:
        print_aura_mask_c(s, h, v)


def print_aura_mask_cpp(s: Writer, h: Writer, v: model.WorldVersion):
    header = ""
    source = ""
    if world_version_is_vanilla(v):
        header = """
constexpr int AURA_MASK_SIZE = 32;
struct AuraMask {
    uint16_t auras[AURA_MASK_SIZE];
};
"""
        source = """
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
"""
    elif world_version_is_tbc(v):
        pass
    elif world_version_is_wrath(v):
        pass
    elif world_version_is_all(v):
        pass
    else:
        raise Exception("invalid version")

    s.write_block(source)
    s.newline()

    h.write_block(header)
    h.newline()


def print_aura_mask_c(s: Writer, h: Writer, v: model.WorldVersion):
    header = ""
    source = ""
    if world_version_is_vanilla(v):
        header = """
#define VANILLA_AURA_MASK_SIZE 32
typedef struct {
    uint16_t auras[VANILLA_AURA_MASK_SIZE];
} vanilla_AuraMask;
"""
        source = """
static size_t vanilla_aura_mask_size(const vanilla_AuraMask* object) {
    size_t size = 4; /* uint32_t header */
    int i;
    
    for(i = 0; i < VANILLA_AURA_MASK_SIZE; ++i) {
        if(object->auras[i] != 0) {
            size += 2; /* uint16_t members */
        }
    }
    
    return size;
}

static WowWorldResult vanilla_aura_mask_read(WowWorldReader* reader, vanilla_AuraMask* mask) {
    uint32_t header;
    int i;
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
    int i;
    
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
"""
    elif world_version_is_tbc(v):
        pass
    elif world_version_is_wrath(v):
        pass
    elif world_version_is_all(v):
        pass
    else:
        raise Exception("invalid version")

    s.write_block(source)
    s.newline()

    h.write_block(header)
    h.newline()
