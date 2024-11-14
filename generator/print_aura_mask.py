from util import world_version_is_vanilla, world_version_is_tbc, \
    world_version_is_wrath, world_version_is_all
from writer import Writer
import model


def print_aura_mask(s: Writer, h: Writer, v: model.WorldVersion):
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
    
    for(i = 0; i < 32; ++i) {
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
