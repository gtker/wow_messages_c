import typing

from print_struct import container_has_c_members

import model
from model import Container
from print_struct.struct_util import print_if_statement_header, \
    integer_type_to_size, integer_type_to_short, integer_type_to_c_str, all_members_from_container
from util import container_is_unencrypted, first_version_as_module, login_version_matches, is_world, get_type_prefix, \
    get_export_define, is_cpp
from writer import Writer


def print_write_struct_member(s: Writer, d: model.Definition, module_name: str, container_name: str,
                              extra_indirection: str):
    variable_name = f"object->{extra_indirection}{d.name}"
    if is_cpp():
        variable_name = f"obj.{extra_indirection}{d.name}"

    match d.data_type:
        case model.DataTypeInteger(integer_type=integer_type):
            if d.constant_value is not None:
                variable_name = d.constant_value.value
            if d.size_of_fields_before_size is not None:
                c_type = integer_type_to_c_str(integer_type)
                variable_name = f"({c_type}){module_name}_{container_name}_size(object)"
                if is_cpp():
                    variable_name = f"static_cast<{c_type}>({container_name}_size(obj))"

            short = integer_type_to_short(integer_type)

            if is_cpp():
                s.wln(f"writer.write_{short}({variable_name});")
            else:
                s.wln(f"WRITE_{short.upper()}({variable_name});")

        case model.DataTypeBool(integer_type=integer_type):
            size = integer_type_to_size(integer_type)
            if is_cpp():
                s.wln(f"writer.write_bool{size * 8}({variable_name});")
            else:
                s.wln(f"WRITE_BOOL{size * 8}({variable_name});")

        case model.DataTypeFlag(
            type_name=type_name, integer_type=integer_type
        ) | model.DataTypeEnum(integer_type=integer_type, type_name=type_name):
            short = integer_type_to_short(integer_type)
            c_type = integer_type_to_c_str(integer_type)
            if is_cpp():
                s.wln(f"writer.write_{short}(static_cast<{c_type}>({variable_name}));")
            else:
                s.wln(f"WRITE_{short.upper()}({variable_name});")

        case model.DataTypeString():
            if is_cpp():
                s.wln(f"writer.write_string({variable_name});")
            else:
                s.wln(f"WRITE_STRING({variable_name});")

        case model.DataTypeCstring():
            if is_cpp():
                s.wln(f"writer.write_cstring({variable_name});")
            else:
                s.wln(f"WRITE_CSTRING({variable_name});")

        case model.DataTypeSizedCstring():
            if is_cpp():
                s.wln(f"writer.write_sized_cstring({variable_name});")
            else:
                s.wln(f"WRITE_SIZED_CSTRING({variable_name});")

        case model.DataTypeLevel32() | model.DataTypeSpell() | model.DataTypeItem() \
             | model.DataTypeDateTime() | model.DataTypeGold() | model.DataTypeSeconds() \
             | model.DataTypeMilliseconds() | model.DataTypeIPAddress():
            if is_cpp():
                s.wln(f"writer.write_u32({variable_name});")
            else:
                s.wln(f"WRITE_U32({variable_name});")

        case model.DataTypeGUID():
            if is_cpp():
                s.wln(f"writer.write_u64({variable_name});")
            else:
                s.wln(f"WRITE_U64({variable_name});")

        case model.DataTypeLevel():
            if is_cpp():
                s.wln(f"writer.write_u8({variable_name});")
            else:
                s.wln(f"WRITE_U8({variable_name});")

        case model.DataTypeLevel16() | model.DataTypeSpell16():
            if is_cpp():
                s.wln(f"writer.write_u16({variable_name});")
            else:
                s.wln(f"WRITE_U16({variable_name});")

        case model.DataTypePackedGUID():
            if is_cpp():
                s.wln(f"writer.write_packed_guid({variable_name});")
            else:
                s.wln(f"WRITE_PACKED_GUID({variable_name});")

        case model.DataTypeFloatingPoint() | model.DataTypePopulation():
            if is_cpp():
                s.wln(f"writer.write_float({variable_name});")
            else:
                s.wln(f"WRITE_FLOAT({variable_name});")

        case model.DataTypeStruct(struct_data=e):
            if is_cpp():
                s.wln(f"{e.name}_write(writer, {variable_name});")
            else:
                version = first_version_as_module(e.tags)
                wlm_prefix = "WWM" if is_world(e.tags) else "WLM"

                s.wln(f"{wlm_prefix}_CHECK_RETURN_CODE({version}_{e.name}_write(writer, &object->{d.name}));")

        case model.DataTypeUpdateMask():
            if is_cpp():
                s.wln(f"{module_name}::update_mask_write(writer, {variable_name});")
            else:
                s.wln(f"{module_name}_update_mask_write(writer, &{variable_name});")

        case model.DataTypeAuraMask():
            if is_cpp():
                s.wln(f"aura_mask_write(writer, {variable_name});")
            else:
                s.wln(f"WWM_CHECK_RETURN_CODE({module_name}_aura_mask_write(writer, &{variable_name}));")

        case model.DataTypeMonsterMoveSpline():
            if is_cpp():
                s.wln(f"::wow_world_messages::util::wwm_write_monster_move_spline(writer, {variable_name});")
            else:
                s.wln(f"WRITE_MONSTER_MOVE_SPLINE({variable_name});")

        case model.DataTypeEnchantMask():
            s.wln(f"WRITE_ENCHANT_MASK_{module_name}({variable_name});")

        case model.DataTypeNamedGUID():
            s.wln(f"WRITE_NAMED_GUID({variable_name});")

        case model.DataTypeInspectTalentGearMask():
            s.wln(f"WRITE_INSPECT_TALENT_GEAR_MASK({variable_name});")

        case model.DataTypeVariableItemRandomProperty():
            s.wln(f"WRITE_VARIABLE_ITEM_RANDOM_PROPERTY({variable_name});")

        case model.DataTypeCacheMask():
            s.wln(f"WRITE_CACHE_MASK_{module_name}({variable_name});")

        case model.DataTypeAddonArray():
            s.wln(f"WRITE_ADDON_ARRAY({variable_name});")

        case model.DataTypeAchievementDoneArray():
            s.wln(f"WRITE_ACHIEVEMENT_DONE_ARRAY({variable_name});")

        case model.DataTypeAchievementInProgressArray():
            s.wln(f"WRITE_ACHIEVEMENT_IN_PROGRESS_ARRAY({variable_name});")

        case model.DataTypeArray(compressed=compressed, size=size, inner_type=inner_type):
            if is_cpp():
                s.open_curly(f"for (const auto& v : {variable_name})")
                match inner_type:
                    case model.ArrayTypeInteger(integer_type=integer_type):
                        short = integer_type_to_short(integer_type)
                        s.wln(f"writer.write_{short}(v);")

                    case model.ArrayTypeSpell():
                        s.wln(f"writer.write_u32(v);")

                    case model.ArrayTypeGUID():
                        s.wln(f"writer.write_u64(v);")

                    case model.ArrayTypeStruct(struct_data=e):
                        s.wln(f"{e.name}_write(writer, v);")

                    case model.ArrayTypeCstring():
                        s.wln(f"writer.write_cstring(v);")

                    case model.ArrayTypePackedGUID():
                        s.wln(f"writer.write_packed_guid(v);")

                    case v2:
                        raise Exception(f"{v2}")

                s.closing_curly()  # for (const auto& v

            else:
                if compressed:
                    s.open_curly("/* C89 Scope for compressed */")
                    s.wln(f"unsigned char* {d.name}_uncompressed_data = NULL;")
                    s.newline()
                    s.wln("uint32_t compressed_i;")
                    s.wln("size_t _size = 0;")
                    s.wln("WowWorldWriter new_writer;")
                    s.wln("WowWorldWriter* old_writer = writer;")
                    s.newline()


                    s.open_curly(f"for(compressed_i = 0; compressed_i < object->amount_of_{extra_indirection}{d.name}; ++compressed_i)")
                    print_size_for_array_inner(s, inner_type, f"object->{extra_indirection}{d.name}", "_size")
                    s.closing_curly() # for

                    s.open_curly("if (_size)")

                    s.wln(f"{d.name}_uncompressed_data = malloc(_size);")
                    s.wln(f"new_writer.destination = {d.name}_uncompressed_data;")
                    s.wln(f"new_writer.length = _size;")
                    s.wln(f"new_writer.index = 0;")
                    s.wln("writer = &new_writer;")


                fixed_prefix = "(*" if type(size) is model.ArraySizeFixed else ""
                fixed_suffix = ")" if type(size) is model.ArraySizeFixed else ""

                print_array_inner_write(d, extra_indirection, fixed_prefix, fixed_suffix, inner_type, s, size,
                                        variable_name)

                if compressed:
                    s.newline()
                    s.wln(f"wwm_compress_data({d.name}_uncompressed_data, _size, old_writer->destination, old_writer->length - old_writer->index);")
                    s.newline()
                    s.wln(f"free({d.name}_uncompressed_data);")

                    s.closing_curly() # if (_size)
                    s.closing_curly() # C89 scope for compressed

    s.newline()


def print_size_for_array_inner(s: Writer, inner_type: model.ArrayType, variable_name: str, size_variable_name: str):
    inner_size = ""
    match inner_type:
        case model.ArrayTypeInteger(integer_type=integer_type):
            inner_size = integer_type_to_size(integer_type)
        case model.ArrayTypeSpell():
            inner_size = 4
        case model.ArrayTypeGUID():
            inner_size = 8
        case model.ArrayTypeStruct(struct_data=e):
            version = first_version_as_module(e.tags)
            inner_size = f"{version}_{e.name}_size(&{variable_name}[compressed_i])" if not e.sizes.constant_sized else str(
                e.sizes.maximum_size)
        case model.ArrayTypeCstring():
            inner_size = f"STRING_SIZE({variable_name}[compressed_i])"
        case model.ArrayTypePackedGUID():
            inner_size = f"wwm_packed_guid_size({variable_name}[compressed_i]);"

        case v2:
            raise Exception(f"{v2}")
    s.wln(f"{size_variable_name} += {str(inner_size)};")


def print_array_inner_write(d: model.Definition, extra_indirection: str, fixed_prefix: str, fixed_suffix: str, inner_type: model.ArrayType, s: Writer, size: model.ArraySize, variable_name: str):
    inner = ""
    match inner_type:
        case model.ArrayTypeInteger(integer_type=integer_type):
            short = integer_type_to_short(integer_type).upper()
            inner = f"WRITE_{short}({fixed_prefix}{variable_name}{fixed_suffix}[i])"

        case model.ArrayTypeSpell():
            inner = f"WRITE_U32({fixed_prefix}{variable_name}{fixed_suffix}[i])"

        case model.ArrayTypeGUID():
            inner = f"WRITE_U64({fixed_prefix}{variable_name}{fixed_suffix}[i])"

        case model.ArrayTypeStruct(struct_data=e):
            version = first_version_as_module(e.tags)
            wlm_prefix = "WWM" if is_world(e.tags) else "WLM"
            inner = f"{wlm_prefix}_CHECK_RETURN_CODE({version}_{e.name}_write(writer, &{fixed_prefix}{variable_name}{fixed_suffix}[i]))"

        case model.ArrayTypeCstring():
            inner = f"WRITE_STRING({fixed_prefix}{variable_name}{fixed_suffix}[i])"

        case model.ArrayTypePackedGUID():
            inner = f"WWM_CHECK_RETURN_CODE(wwm_write_packed_guid(writer, {fixed_prefix}{variable_name}{fixed_suffix}[i]))"

        case v2:
            raise Exception(f"{v2}")
    match size:
        case model.ArraySizeFixed(size=size):
            s.wln(f"WRITE_ARRAY({variable_name}, {size}, {inner});")
        case model.ArraySizeVariable(size=size):
            s.wln(f"WRITE_ARRAY({variable_name}, object->{extra_indirection}{size}, {inner});")
        case model.ArraySizeEndless():
            s.wln(
                f"WRITE_ARRAY({variable_name}, object->{extra_indirection}amount_of_{d.name}, {inner});")
        case v:
            raise Exception(f"{v}")


def print_write(s: Writer, h: Writer, container: Container, object_type: model.ObjectType, module_name: str,
                extra_indirection: str):
    result_type = get_type_prefix(container.tags)
    export = "static " if type(object_type) is model.ObjectTypeStruct else f"{get_export_define(container.tags)} "
    if is_world(container.tags) and module_name == "all":
        export = ""
    function_suffix = f"_{object_type.container_type_tag.lower()}" if object_type != container.object_type else ""
    function_declaration = f"{export}{result_type}Result {module_name}_{container.name}{function_suffix}_write({result_type}Writer* writer, const {first_version_as_module(container.tags)}_{container.name}* object)"
    if not container_has_c_members(container):
        function_declaration = f"{export}{result_type}Result {module_name}_{container.name}{function_suffix}_write({result_type}Writer* writer)"
    if is_cpp():
        if type(object_type) is model.ObjectTypeStruct:
            function_declaration = f"{export}void {container.name}_write{function_suffix}(Writer& writer, const {container.name}& obj)"
        else:
            function_declaration = f"{export}std::vector<unsigned char> {container.name}::write{function_suffix}() const"

    s.open_curly(function_declaration)
    if type(object_type) is not model.ObjectTypeStruct:
        if not is_cpp():
            h.wln(f"{function_declaration};")

    prefix = "_"
    if container.tags.compressed:
        prefix = "_compressed_"

    if is_cpp():
        size = f"{container.name}_size(obj)"

        if container.sizes.constant_sized:
            size = f"0x{container.sizes.maximum_size:04x}"

        if type(object_type) is not model.ObjectTypeStruct:
            if container_has_c_members(container):
                s.wln("const auto& obj = *this;")
            s.wln(f"auto writer = Writer({size});")
            s.newline()

        match object_type:
            case model.ObjectTypeClogin(opcode=opcode):
                s.wln(f"writer.write_u8(0x{opcode:02x}); /* opcode */")
                s.newline()
            case model.ObjectTypeSlogin(opcode=opcode):
                s.wln(f"writer.write_u8(0x{opcode:02x}); /* opcode */")
                s.newline()
            case model.ObjectTypeSmsg(opcode=opcode):
                s.wln(f"writer.write_u16_be(static_cast<uint16_t>({size} + 2)); /* size */")
                s.newline()
                s.wln(f"writer.write_u16(0x{opcode:08x}); /* opcode */")
                s.newline()
            case model.ObjectTypeCmsg(opcode=opcode):
                size = f"(uint16_t){container.name}_size(obj)"

                if container.sizes.constant_sized:
                    size = f"0x{container.sizes.maximum_size:04x}"

                s.wln(f"writer.write_u16_be(static_cast<uint16_t>({size} + 4)); /* size */")
                s.newline()
                s.wln(f"writer.write_u32(0x{opcode:08x}); /* opcode */")
                s.newline()

    else:
        match object_type:
            case model.ObjectTypeClogin(opcode=opcode):
                s.wln(f"WRITE_U8(0x{opcode:02x}); /* opcode */")
                s.newline()
            case model.ObjectTypeSlogin(opcode=opcode):
                s.wln(f"WRITE_U8(0x{opcode:02x}); /* opcode */")
                s.newline()
            case model.ObjectTypeSmsg(opcode=opcode):
                size = f"(uint16_t){module_name}_{container.name}_size(object)"

                if container.sizes.constant_sized:
                    size = f"0x{container.sizes.maximum_size:04x}"

                s.wln(f"WRITE_U16_BE({size} + 2); /* size */")
                s.newline()
                s.wln(f"WRITE_U16(0x{opcode:08x}); /* opcode */")
                s.newline()
            case model.ObjectTypeCmsg(opcode=opcode):
                size = f"(uint16_t){module_name}_{container.name}_size(object)"

                if container.sizes.constant_sized:
                    size = f"0x{container.sizes.maximum_size:04x}"

                s.wln(f"WRITE_U16_BE({size} + 4); /* size */")
                s.newline()
                s.wln(f"WRITE_U32(0x{opcode:08x}); /* opcode */")
                s.newline()

    for m in container.members:
        print_write_member(s, m, prefix, module_name, container.name, extra_indirection)

    if container.optional is not None:
        deref = "obj." if is_cpp() else "object->"
        s.open_curly(f"if({deref}{extra_indirection}{container.optional.name})")

        for m in container.optional.members:
            print_write_member(s, m, prefix, module_name, container.name, f"{container.optional.name}->")

        s.closing_curly()  # optional_statement_header

    if is_cpp():
        if type(object_type) is not model.ObjectTypeStruct:
            s.wln("return writer.m_buf;")
    else:
        s.newline()
        if is_world(container.tags):
            s.wln("return WWM_RESULT_SUCCESS;")
        else:
            s.wln("return WLM_RESULT_SUCCESS;")

    s.closing_curly()
    s.newline()


def print_write_member(s: Writer, m: model.StructMember, prefix: str, module_name: str, container_name: str,
                       extra_indirection: str):
    match m:
        case model.StructMemberDefinition(_tag, definition):
            print_write_struct_member(s, definition, module_name, container_name, extra_indirection)

        case model.StructMemberIfStatement(_tag, statement):
            print_write_if_statement(s, statement, False, prefix, module_name, container_name, extra_indirection)

        case _:
            raise Exception("invalid struct member")


def print_write_if_statement(s: Writer,
                             statement: model.IfStatement,
                             is_else_if: bool,
                             prefix: str, module_name: str, container_name: str, extra_indirection: str):
    extra_elseif = ""
    if is_else_if:
        extra_elseif = "else "

    print_if_statement_header(s, statement, extra_elseif, extra_indirection, module_name)

    for m in statement.members:
        print_write_member(s, m, prefix, module_name, container_name, extra_indirection)

    s.closing_curly()  # if

    for elseif in statement.else_if_statements:
        print_write_if_statement(s, elseif, True, prefix, module_name, container_name, extra_indirection)
