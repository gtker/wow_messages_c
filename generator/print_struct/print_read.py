from print_struct import container_has_c_members

import model
from model import Container
from print_struct.struct_util import (
    integer_type_to_size,
    print_if_statement_header,
)
from print_struct.struct_util import integer_type_to_short, integer_type_to_c_str
from util import container_needs_size_in_read, first_version_as_module, get_type_prefix, is_world, is_cpp, library_type
from print_struct.struct_util import array_type_to_c_str
from writer import Writer


def print_read_struct_member(s: Writer, d: model.Definition, needs_size: bool, container_is_compressed: bool,
                             module_name: str, extra_indirection: str):
    variable_name = f"object->{extra_indirection}{d.name}"
    if is_cpp():
        variable_name = f"obj.{extra_indirection}{d.name}"

        if d.constant_value is not None or d.size_of_fields_before_size is not None:
            s.w("(void)")
        else:
            if type(d.data_type) is not model.DataTypeArray:
                s.w(f"{variable_name} = ")

    util_namespace = f"::wow_world_messages::util::" if is_cpp() else ""

    match d.data_type:
        case model.DataTypeInteger(integer_type=integer_type):
            size = integer_type_to_size(integer_type)
            short = integer_type_to_short(integer_type)
            if is_cpp():
                s.wln_no_indent(f"reader.read_{short}();")
            elif d.constant_value is not None or d.size_of_fields_before_size is not None:
                s.wln(f"SKIP_FORWARD_BYTES({size});")
            else:
                s.wln(f"READ_{short.upper()}({variable_name});")

            if needs_size:
                s.wln(f"_size += {size};")
        case model.DataTypeBool(integer_type=integer_type):
            size = integer_type_to_size(integer_type)
            if is_cpp():
                s.wln_no_indent(f"reader.read_bool{size * 8}();")
            else:
                s.wln(f"READ_BOOL{size * 8}({variable_name});")

            if needs_size:
                s.wln(f"_size += {size};")

        case model.DataTypeFlag(
            type_name=type_name, integer_type=integer_type
        ) | model.DataTypeEnum(integer_type=integer_type, type_name=type_name):
            short = integer_type_to_short(integer_type)
            if is_cpp():
                s.wln_no_indent(f"static_cast<{type_name}>(reader.read_{short}());")
            else:
                # enums can only be int in C, so we need to set upper bytes to 0
                s.wln(f"{variable_name} = 0;")
                s.wln(f"READ_{short.upper()}({variable_name});")

            size = integer_type_to_size(integer_type)
            if needs_size:
                s.wln(f"_size += {size};")

        case model.DataTypeString():
            if is_cpp():
                s.wln_no_indent("reader.read_string();")
            else:
                s.wln(f"READ_STRING({variable_name});")

            if needs_size:
                s.wln(f"_size += STRING_SIZE({variable_name}) + 1;")
        case model.DataTypeCstring():
            if is_cpp():
                s.wln_no_indent("reader.read_cstring();")
            else:
                s.wln(f"READ_CSTRING({variable_name});")

            if needs_size:
                s.wln(f"_size += STRING_SIZE({variable_name}) + 1;")

        case model.DataTypeSizedCstring():
            if is_cpp():
                s.wln_no_indent("reader.read_sized_cstring();")
            else:
                s.wln(f"READ_SIZED_CSTRING({variable_name});")

            if needs_size:
                s.wln(f"_size += STRING_SIZE({variable_name}) + 4;")

        case model.DataTypeLevel32() | model.DataTypeSpell() | model.DataTypeItem() \
             | model.DataTypeDateTime() | model.DataTypeGold() | model.DataTypeSeconds() \
             | model.DataTypeMilliseconds() | model.DataTypeIPAddress():
            if is_cpp():
                s.wln_no_indent("reader.read_u32();")
            else:
                s.wln(f"READ_U32({variable_name});")

            if needs_size:
                s.wln(f"_size += 4;")

        case model.DataTypeGUID():
            if is_cpp():
                s.wln_no_indent("reader.read_u64();")
            else:
                s.wln(f"READ_U64({variable_name});")

            if needs_size:
                s.wln(f"_size += 8;")

        case model.DataTypeLevel():
            if is_cpp():
                s.wln_no_indent("reader.read_u8();")
            else:
                s.wln(f"READ_U8({variable_name});")

            if needs_size:
                s.wln(f"_size += 1;")

        case model.DataTypeLevel16() | model.DataTypeSpell16():
            if is_cpp():
                s.wln_no_indent("reader.read_u16();")
            else:
                s.wln(f"READ_U16({variable_name});")

            if needs_size:
                s.wln(f"_size += 2;")

        case model.DataTypePackedGUID():
            if is_cpp():
                s.wln_no_indent(f"reader.read_packed_guid();")
            else:
                s.wln(f"READ_PACKED_GUID({variable_name});")

            if needs_size:
                s.wln(f"_size += packed_guid_size({d.name});")

        case model.DataTypeFloatingPoint() | model.DataTypePopulation():
            if is_cpp():
                s.wln_no_indent("reader.read_float();")
            else:
                s.wln(f"READ_FLOAT({variable_name});")

            if needs_size:
                s.wln(f"_size += 4;")

        case model.DataTypeStruct(struct_data=e):
            version = first_version_as_module(e.tags)
            if is_cpp():
                s.wln_no_indent(f"::wow_{library_type(e.tags)}_messages::{version}::{e.name}_read(reader);")
            else:
                wlm_prefix = "WWM" if is_world(e.tags) else "WLM"

                s.wln(f"{wlm_prefix}_CHECK_RETURN_CODE({version}_{e.name}_read(reader, &{variable_name}));")

            if needs_size:
                if e.sizes.constant_sized:
                    s.wln(f"_size += {e.sizes.maximum_size};")
                else:
                    s.wln(f"_size += {d.name}.size();")

        case model.DataTypeUpdateMask():
            if is_cpp():
                s.wln_no_indent(f"{module_name}::update_mask_read(reader);")
            else:
                s.wln(f"{module_name}_update_mask_read(reader, &{variable_name});")

            if needs_size:
                s.wln(f"_size += {d.name}.size();")

        case model.DataTypeAuraMask():
            if is_cpp():
                this_namespace = f"::wow_world_messages::{module_name}::"
                s.wln_no_indent(f"{this_namespace}aura_mask_read(reader);")
            else:
                s.wln(f"WWM_CHECK_RETURN_CODE({module_name}_aura_mask_read(reader, &{variable_name}));")

            if needs_size:
                prefix = "" if is_cpp() else f"{module_name}_"
                s.wln(f"_size += {prefix}aura_mask_size({variable_name});")

        case model.DataTypeMonsterMoveSpline():
            if is_cpp():
                s.wln_no_indent(f"{util_namespace}wwm_read_monster_move_spline(reader);")
            else:
                s.wln(f"READ_MONSTER_MOVE_SPLINE({variable_name});")

            if needs_size:
                s.wln(f"_size += {util_namespace}wwm_monster_move_spline_size({variable_name})")

        case model.DataTypeEnchantMask():
            s.wln(f"READ_ENCHANT_MASK_{module_name}({variable_name});")

            if needs_size:
                s.wln(f"_size += {d.name}.size();")

        case model.DataTypeNamedGUID():
            s.wln(f"READ_NAMED_GUID({variable_name});")

            if needs_size:
                s.wln(f"_size += {d.name}.size();")

        case model.DataTypeInspectTalentGearMask():
            s.wln(f"READ_INSPECT_TALENT_GEAR_MASK({variable_name});")

            if needs_size:
                s.wln(f"_size += {d.name}.size();")

        case model.DataTypeVariableItemRandomProperty():
            s.wln(f"READ_VARIABLE_ITEM_RANDOM_PROPERTY({variable_name});")

            if needs_size:
                s.wln(f"_size += {d.name}.size();")

        case model.DataTypeCacheMask():
            s.wln(f"READ_CACHE_MASK_{module_name}({variable_name});")

            if needs_size:
                s.wln(f"_size += {d.name}.size();")

        case model.DataTypeAddonArray():
            s.wln(f"READ_ADDON_ARRAY({variable_name});")

            if needs_size:
                s.wln(f"_size += {d.name}.size();")

        case model.DataTypeAchievementDoneArray():
            s.wln(f"READ_ACHIEVEMENT_DONE_ARRAY({variable_name});")

            if needs_size:
                s.wln(f"_size += {d.name}.size();")

        case model.DataTypeAchievementInProgressArray():
            s.wln(f"READ_ACHIEVEMENT_IN_PROGRESS_ARRAY({variable_name});")

            if needs_size:
                s.wln(f"_size += {d.name}.size();")

        case model.DataTypeArray(compressed=compressed, size=size, inner_type=inner_type):
            if is_cpp():
                size_var: str = ""
                match size:
                    case model.ArraySizeFixed(size=size_fixed_size):
                        s.open_curly(f"for (auto i = 0; i < {size_fixed_size}; ++i)")
                    case model.ArraySizeVariable(size=size_variable_size):
                        size_var = f"obj.{extra_indirection}{size_variable_size}"
                        s.open_curly(f"for (auto i = 0; i < {size_var}; ++i)")
                    case model.ArraySizeEndless():
                        s.open_curly("while (_size < body_size)")
                    case _:
                        raise Exception("unknown array size")

                inner: str = ""
                match inner_type:
                    case model.ArrayTypeInteger(integer_type=integer_type):
                        short = integer_type_to_short(integer_type)
                        inner = f"reader.read_{short}()"

                    case model.ArrayTypeSpell():
                        inner = f"reader.read_u32()"

                    case model.ArrayTypeGUID():
                        inner = f"reader.read_u64()"

                    case model.ArrayTypeCstring():
                        inner = f"reader.read_cstring()"

                    case model.ArrayTypePackedGUID():
                        inner = f"reader.read_packed_guid()"

                    case model.ArrayTypeStruct(struct_data=e):
                        inner = f"::wow_{library_type(e.tags)}_messages::{first_version_as_module(e.tags)}::{e.name}_read(reader)"

                    case v2:
                        raise Exception(f"{v2}")

                match size:
                    case model.ArraySizeFixed():
                        s.wln(f"{variable_name}[i] = {inner};")
                    case model.ArraySizeVariable() | model.ArraySizeEndless():
                        s.wln(f"{variable_name}.push_back({inner});")
                    case _:
                        raise Exception("unknown array size")

                if needs_size or isinstance(size, model.ArraySizeEndless):
                    s.w("_size += ")
                    match inner_type:
                        case model.ArrayTypeInteger(integer_type=integer_type):
                            size = integer_type_to_size(integer_type)
                            s.w_no_indent(str(size))

                        case model.ArrayTypeGUID():
                            s.w_no_indent(str(8))

                        case model.ArrayTypeSpell():
                            s.w_no_indent(str(4))

                        case model.ArrayTypeStruct(struct_data=e):
                            if e.sizes.constant_sized:
                                s.w_no_indent(str(e.sizes.maximum_size))
                            else:
                                s.w_no_indent(f"{module_name}::{e.name}_size({variable_name}.back())")

                        case model.ArrayTypeCstring():
                            s.w_no_indent(f"{variable_name}.back().size() + 1")

                        case model.ArrayTypePackedGUID():
                            s.w_no_indent(f"{module_name}::wwm_packed_guid_size({variable_name}.back())")

                        case v3:
                            raise Exception(f"{v3}")
                    s.wln_no_indent(";")
                s.closing_curly()  # for (auto i


            else:
                if compressed:
                    s.wln("/* {d.name}_decompressed_size: u32 */")
                    s.wln("_size += 4;")
                    s.newline()

                    s.wln(f"uint32_t {d.name}_decompressed_size;")
                    s.wln(f"READ_U32({d.name}_decompressed_size);")

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

                fixed_prefix = "(*" if type(size) is model.ArraySizeFixed else ""
                fixed_suffix = ")" if type(size) is model.ArraySizeFixed else ""

                inner = ""
                match inner_type:
                    case model.ArrayTypeInteger(integer_type=integer_type):
                        short = integer_type_to_short(integer_type).upper()
                        inner = f"READ_{short}({fixed_prefix}{variable_name}{fixed_suffix}[i])"

                    case model.ArrayTypeSpell():
                        inner = f"READ_U32({fixed_prefix}{variable_name}{fixed_suffix}[i])"

                    case model.ArrayTypeGUID():
                        inner = f"READ_U64({fixed_prefix}{variable_name}{fixed_suffix}[i])"

                    case model.ArrayTypeStruct(struct_data=e):
                        version = first_version_as_module(e.tags)
                        wlm_prefix = "WWM" if is_world(e.tags) else "WLM"
                        inner = f"{wlm_prefix}_CHECK_RETURN_CODE({version}_{e.name}_read(reader, &{fixed_prefix}{variable_name}{fixed_suffix}[i]))"

                    case model.ArrayTypeCstring():
                        inner = f"READ_CSTRING({fixed_prefix}{variable_name}{fixed_suffix}[i])"

                    case model.ArrayTypePackedGUID():
                        if is_cpp():
                            inner = f"reader.read_packed_guid()"
                        else:
                            inner = f"READ_PACKED_GUID({fixed_prefix}{variable_name}{fixed_suffix}[i])"

                    case v2:
                        raise Exception(f"{v2}")

                match size:
                    case model.ArraySizeFixed(size=size):
                        s.wln(
                            f"READ_ARRAY_ALLOCATE({variable_name}, {size}, sizeof({array_type_to_c_str(inner_type)}));")
                        s.w(f"READ_ARRAY({variable_name}, {size}, {inner});")
                    case model.ArraySizeVariable(size=size):
                        s.wln(
                            f"READ_ARRAY_ALLOCATE({variable_name}, object->{extra_indirection}{size}, sizeof({array_type_to_c_str(inner_type)}));")
                        s.w(f"READ_ARRAY({variable_name}, object->{extra_indirection}{size}, {inner});")
                    case model.ArraySizeEndless():
                        if container_is_compressed:
                            s.open(f"while not reader.at_eof():")
                        elif not compressed:
                            s.wln(f"object->amount_of_{d.name} = 0;")
                            s.open_curly("/* C89 scope to allow variable declarations */")
                            s.wln("int i = 0;")
                            s.wln(f"{variable_name} = malloc(body_size - _size);")

                            s.open_curly("while (_size < body_size)")
                            s.wln(f"{inner};")

                            if needs_size or isinstance(size, model.ArraySizeEndless):
                                s.w("_size += ")
                                match inner_type:
                                    case model.ArrayTypeInteger(integer_type=integer_type):
                                        size = integer_type_to_size(integer_type)
                                        s.w_no_indent(str(size))

                                    case model.ArrayTypeGUID():
                                        s.w_no_indent(str(8))

                                    case model.ArrayTypeSpell():
                                        s.w_no_indent(str(4))

                                    case model.ArrayTypeStruct(struct_data=e):
                                        if e.sizes.constant_sized:
                                            s.w_no_indent(str(e.sizes.maximum_size))
                                        else:
                                            s.w_no_indent(f"{module_name}_{e.name}_size(&{variable_name}[i])")

                                    case model.ArrayTypeCstring():
                                        s.w_no_indent(f"STRING_SIZE({variable_name}[i])")

                                    case model.ArrayTypePackedGUID():
                                        s.w_no_indent(f"wwm_packed_guid_size({variable_name}[i])")

                                    case v3:
                                        raise Exception(f"{v3}")
                                s.wln_no_indent(";")

                            s.wln("++i;")
                            s.closing_curly()
                            s.closing_curly()  # C89 scope

                        else:
                            s.open(f"while not {d.name}_reader.at_eof():")
                    case v:
                        raise Exception(f"{v}")

        case v:
            raise Exception(f"{v}")

    s.newline()


def print_read_member(s: Writer, m: model.StructMember, container: model.Container, needs_size: bool, module_name: str,
                      extra_indirection: str):
    match m:
        case model.StructMemberDefinition(_tag, definition):
            compressed = container.tags.compressed is not None and container.tags.compressed
            print_read_struct_member(s, definition, needs_size, compressed, module_name, extra_indirection)

        case model.StructMemberIfStatement(_tag, statement):
            print_read_if_statement(s, statement, container, False, needs_size, module_name, extra_indirection)

        case _:
            raise Exception("invalid struct member")


def print_read_if_statement(
        s: Writer,
        statement: model.IfStatement,
        container: model.Container,
        is_else_if: bool,
        needs_size: bool,
        module_name: str,
        extra_indirection: str,
):
    extra_elseif = ""
    if is_else_if:
        extra_elseif = "else "

    print_if_statement_header(s, statement, extra_elseif, "", module_name)

    for member in statement.members:
        print_read_member(s, member, container, needs_size, module_name, extra_indirection)

    s.closing_curly()  # if

    for elseif in statement.else_if_statements:
        print_read_if_statement(s, elseif, container, True, needs_size, module_name, extra_indirection)


def print_read(s: Writer, container: Container, module_name: str):
    if not container_has_c_members(container):
        return

    needs_size = container_needs_size_in_read(container)

    body_size = ", size_t body_size" if container_needs_size_in_read(container) else ""
    if is_cpp():
        s.open_curly(f"{container.name} {container.name}_read(Reader& reader{body_size})")
        s.wln(f"{container.name} obj;")
        if needs_size:
            s.wln("size_t _size = 0;")

        s.newline()

        for m in container.members:
            print_read_member(s, m, container, needs_size, module_name, "")

        s.wln("return obj;")

        s.closing_curly()  # * *_read(Reader&)
        s.newline()
    else:
        export = "" if is_world(container.tags) and module_name == "all" else "static "

        result_type = get_type_prefix(container.tags)
        s.open_curly(
            f"{export}{result_type}Result {module_name}_{container.name}_read({result_type}Reader* reader, {first_version_as_module(container.tags)}_{container.name}* object{body_size})")

        if container.tags.compressed:
            s.write_block("""
    decompressed_size = await read_int(reader, 4)
    compressed_bytes = await reader.readexactly(body_size - 4)
    decompressed_bytes = zlib.decompress(compressed_bytes, bufsize=decompressed_size)
    reader = asyncio.StreamReader()
    reader.feed_data(decompressed_bytes)
    reader.feed_eof()
            """)
            s.newline()

        if needs_size:
            s.wln("size_t _size = 0;")
            s.newline()

        for m in container.members:
            print_read_member(s, m, container, needs_size, module_name, "")

        if container.optional is not None:
            s.wln(f"object->{container.optional.name} = NULL;")
            s.open_curly("if (_size < body_size)")
            s.wln(
                f"object->{container.optional.name} = malloc(sizeof({module_name}_{container.name}_{container.optional.name}));")
            s.newline()

            for member in container.optional.members:
                print_read_member(s, member, container, True, module_name, f"{container.optional.name}->")

            s.closing_curly()

        if is_world(container.tags):
            s.wln("return WWM_RESULT_SUCCESS;")
        else:
            s.wln("return WLM_RESULT_SUCCESS;")

        s.closing_curly()  # enum Result
        s.newline()
