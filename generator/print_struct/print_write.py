import typing

from print_struct import container_has_c_members

import model
from model import Container
from print_struct.struct_util import print_if_statement_header, print_optional_statement_header, \
    integer_type_to_size, integer_type_to_short, integer_type_to_c_str
from util import container_is_unencrypted, first_version_as_module, login_version_matches, is_world, get_type_prefix, \
    get_export_define
from writer import Writer


def print_write_struct_member(s: Writer, d: model.Definition, module_name: str, container_name: str):
    variable_name = f"object->{d.name}"
    match d.data_type:
        case model.DataTypeInteger(integer_type=integer_type):
            if d.constant_value is not None:
                variable_name = d.constant_value.value
            if d.size_of_fields_before_size is not None:
                c_type = integer_type_to_c_str(integer_type)
                variable_name = f"({c_type}){module_name}_{container_name}_size(object)"

            short = integer_type_to_short(integer_type).upper()

            s.wln(f"WRITE_{short}({variable_name});")

        case model.DataTypeBool(integer_type=integer_type):
            size = integer_type_to_size(integer_type)
            s.wln(f"WRITE_BOOL{size * 8}({variable_name});")

        case model.DataTypeFlag(
            type_name=type_name, integer_type=integer_type
        ):
            short = integer_type_to_short(integer_type).upper()
            s.wln(f"WRITE_{short}({variable_name});")

        case model.DataTypeEnum(integer_type=integer_type, type_name=type_name):
            short = integer_type_to_short(integer_type).upper()
            s.wln(f"WRITE_{short}({variable_name});")

        case model.DataTypeString():
            s.wln(f"WRITE_STRING({variable_name});")

        case model.DataTypeCstring():
            s.wln(f"WRITE_CSTRING({variable_name});")

        case model.DataTypeSizedCstring():
            s.wln(f"WRITE_SIZED_CSTRING({variable_name});")

        case model.DataTypeLevel32() | model.DataTypeSpell() | model.DataTypeItem() \
             | model.DataTypeDateTime() | model.DataTypeGold() | model.DataTypeSeconds() \
             | model.DataTypeMilliseconds() | model.DataTypeIPAddress():
            s.wln(f"WRITE_U32({variable_name});")

        case model.DataTypeGUID():
            s.wln(f"WRITE_U64({variable_name});")

        case model.DataTypeLevel():
            s.wln(f"WRITE_U8({variable_name});")

        case model.DataTypeLevel16() | model.DataTypeSpell16():
            s.wln(f"WRITE_U16({variable_name});")

        case model.DataTypePackedGUID():
            s.wln(f"WRITE_PACKED_GUID({variable_name});")

        case model.DataTypeFloatingPoint() | model.DataTypePopulation():
            s.wln(f"WRITE_FLOAT({variable_name});")

        case model.DataTypeStruct(struct_data=e):
            version = first_version_as_module(e.tags)
            wlm_prefix = "WWM" if is_world(e.tags) else "WLM"

            s.wln(f"{wlm_prefix}_CHECK_RETURN_CODE({version}_{e.name}_write(writer, &object->{d.name}));")

        case model.DataTypeUpdateMask():
            s.wln(f"{module_name}_update_mask_write(writer, &{variable_name});")

        case model.DataTypeAuraMask():
            s.wln(f"WWM_CHECK_RETURN_CODE({module_name}_aura_mask_write(writer, &{variable_name}));")

        case model.DataTypeMonsterMoveSpline():
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
            reader = "reader"
            if compressed:
                s.wln("# {d.name}_decompressed_size: u32")
                s.wln("_size += 4  # decompressed_size")
                s.newline()

                s.wln(f"{d.name}_decompressed_size = await read_int(reader, 4)")

                s.wln(f"{d.name}_bytes = await reader.readexactly(body_size - _size)")
                s.newline()

                s.wln(f"{d.name}_reader = reader")
                s.open(f"if len({d.name}_bytes) != 0:")
                s.wln(f"{d.name}_bytes = zlib.decompress({d.name}_bytes, bufsize={d.name}_decompressed_size)")
                s.wln(f"{d.name}_reader = asyncio.StreamReader()")
                s.wln(f"{d.name}_reader.feed_data({d.name}_bytes)")
                s.wln(f"{d.name}_reader.feed_eof()")
                s.close()

                s.newline()
                reader = f"{d.name}_reader"

            match size:
                case model.ArraySizeFixed(size=size):
                    s.w(f"WRITE_ARRAY({variable_name}, {size}, ")
                case model.ArraySizeVariable(size=size):
                    s.w(f"WRITE_ARRAY({variable_name}, object->{size}, ")
                case model.ArraySizeEndless():
                    if container_is_compressed:
                        s.open(f"while not reader.at_eof():")
                    elif not compressed:
                        s.open("while _size < body_size:")
                    else:
                        s.open(f"while not {d.name}_reader.at_eof():")
                case v:
                    raise Exception(f"{v}")

            match inner_type:
                case model.ArrayTypeInteger(integer_type=integer_type):
                    short = integer_type_to_short(integer_type).upper()
                    s.w_no_indent(f"WRITE_{short}({variable_name}[i])")

                case model.ArrayTypeSpell():
                    s.w_no_indent(f"WRITE_U32({variable_name}[i])")

                case model.ArrayTypeGUID():
                    s.w_no_indent(f"WRITE_U64({variable_name}[i])")

                case model.ArrayTypeStruct(struct_data=e):
                    version = first_version_as_module(e.tags)
                    wlm_prefix = "WWM" if is_world(e.tags) else "WLM"
                    s.w_no_indent(
                        f"{wlm_prefix}_CHECK_RETURN_CODE({version}_{e.name}_write(writer, &{variable_name}[i]))")

                case model.ArrayTypeCstring():
                    s.w_no_indent(f"WRITE_STRING({variable_name}[i])")

                case model.ArrayTypePackedGUID():
                    s.w_no_indent(
                        f"WWM_CHECK_RETURN_CODE(wwm_write_packed_guid(writer, {variable_name}[i]))"
                    )

                case v2:
                    raise Exception(f"{v2}")

            s.wln_no_indent(");")

    s.newline()


def print_write(s: Writer, h: Writer, container: Container, object_type: model.ObjectType, module_name: str):
    result_type = get_type_prefix(container.tags)
    export = "static " if type(object_type) is model.ObjectTypeStruct else f"{get_export_define(container.tags)} "
    if is_world(container.tags) and module_name == "all":
        export = ""
    function_declaration = f"{export}{result_type}Result {module_name}_{container.name}_write({result_type}Writer* writer, const {first_version_as_module(container.tags)}_{container.name}* object)"
    if not container_has_c_members(container):
        function_declaration = f"{export}{result_type}Result {module_name}_{container.name}_write({result_type}Writer* writer)"

    s.open_curly(function_declaration)
    match object_type:
        case model.ObjectTypeStruct():
            pass
        case _:
            h.wln(f"{function_declaration};")

    prefix = "_"
    if container.tags.compressed:
        prefix = "_compressed_"

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
        print_write_member(s, m, prefix, module_name, container.name)

    if container.optional is not None:
        print_optional_statement_header(s, container.optional)

        for m in container.optional.members:
            print_write_member(s, m, prefix, module_name, container.name)

        s.close()

    s.newline()
    if is_world(container.tags):
        s.wln("return WWM_RESULT_SUCCESS;")
    else:
        s.wln("return WLM_RESULT_SUCCESS;")

    s.closing_curly()
    s.newline()


def print_write_member(s: Writer, m: model.StructMember, prefix: str, module_name: str, container_name: str):
    match m:
        case model.StructMemberDefinition(_tag, definition):
            print_write_struct_member(s, definition, module_name, container_name)

        case model.StructMemberIfStatement(_tag, statement):
            print_write_if_statement(s, statement, False, prefix, module_name, container_name)

        case _:
            raise Exception("invalid struct member")


def print_write_if_statement(s: Writer,
                             statement: model.IfStatement,
                             is_else_if: bool,
                             prefix: str, module_name: str, container_name: str):
    extra_elseif = ""
    if is_else_if:
        extra_elseif = "else "

    print_if_statement_header(s, statement, extra_elseif, "self.", module_name)

    for m in statement.members:
        print_write_member(s, m, prefix, module_name, container_name)

    s.closing_curly()  # if

    for elseif in statement.else_if_statements:
        print_write_if_statement(s, elseif, True, prefix, module_name, container_name)
