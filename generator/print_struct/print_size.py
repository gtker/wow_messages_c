import typing

import model
from print_struct.print_write import print_write_member
from print_struct.struct_util import (
    integer_type_to_size,
    print_if_statement_header, container_should_have_size_function, container_contains_array, )

from util import first_version_as_module, is_cpp
from writer import Writer


def print_size(s: Writer, container: model.Container, module_name: str):
    if not container_should_have_size_function(container):
        return

    if is_cpp():
        s.open_curly(
            f"static size_t {container.name}_size(const {container.name}& obj)")
    else:
        s.open_curly(
            f"static size_t {module_name}_{container.name}_size(const {first_version_as_module(container.tags)}_{container.name}* object)")

    if container.tags.compressed:
        print_size_for_compressed_container(s, container)
    else:
        print_size_until_inner_members(s, container.members, container.manual_size_subtraction,
                                       True, container.optional is not None, module_name, "")
        if container.optional is not None:
            s.open_curly(f"if(object->{container.optional.name} != NULL)")
            print_size_until_inner_members(s, container.optional.members, container.manual_size_subtraction, False,
                                           False, module_name, f"{container.optional.name}->")
            s.closing_curly()  # optional_statement_header
            s.newline()

            s.wln("return _size;")

    s.closing_curly()
    s.newline()


def print_size_until_inner_members(s: Writer, members: list[model.StructMember],
                                   manual_size_subtraction: typing.Optional[int], return_early: bool,
                                   has_optional: bool, module_name: str, extra_indirection: str):
    count, strings, uncounted_members = get_size_and_remaining_members(members, module_name, extra_indirection)

    if manual_size_subtraction is not None:
        count -= manual_size_subtraction

    size = str(count)
    if len(strings) != 0:
        size += f" + {' + '.join(strings)}"

    if len(uncounted_members) == 0 and return_early and not has_optional:
        s.wln(f"return {size};")
    else:
        if not return_early:
            s.wln(f"_size += {size};")
        else:
            s.wln(f"size_t _size = {size};")

        if len(uncounted_members) != 0 or has_optional:
            s.newline()

        for m in uncounted_members:
            print_size_inner(s, m, module_name, extra_indirection)

        if return_early and not has_optional:
            s.wln(f"return _size;")


def print_size_for_compressed_container(s, container):
    s.wln("_fmt = ''")
    s.wln("_data = []")
    s.newline()
    for m in container.members:
        print_write_member(s, m, "_")
    s.wln("_uncompressed_data = struct.pack(_fmt, *_data)")
    s.wln("_compressed_data = zlib.compress(_uncompressed_data)")
    s.wln("return len(_compressed_data) + 4")


def array_size_inner_values(
        array: model.DataTypeArray, name: str, extra_indirection: str,
) -> typing.Union[str, int, None]:
    size = 0
    match array.inner_type:
        case model.ArrayTypeGUID():
            size = 8
        case model.ArrayTypeStruct(struct_data=e):
            if e.sizes.constant_sized:
                size = e.sizes.maximum_size
            else:
                return None
        case model.ArrayTypePackedGUID():
            return None
        case model.ArrayTypeCstring():
            return None
        case model.ArrayTypeInteger(integer_type=integer_type):
            size = integer_type_to_size(integer_type)
        case model.ArrayTypeSpell():
            size = 4
        case v:
            raise Exception(f"{v}")

    match array.size:
        case model.ArraySizeFixed(size=array_size):
            return size * int(array_size)
        case model.ArraySizeVariable(size=array_size):
            return f"{size} * object->{extra_indirection}{array_size}"
        case model.ArraySizeEndless():
            return None

        case v2:
            raise Exception(f"{v2}")


def addable_size_value(
        data_type: model.DataType, extra_self: str, name: str, module_name: str, extra_indirection: str
) -> typing.Optional[typing.Tuple[int, typing.Optional[str]]]:
    variable_name = f"object->{extra_indirection}{name}"
    if is_cpp():
        variable_name = f"obj.{extra_indirection}{name}"

    match data_type:
        case model.DataTypeStruct(struct_data=e):
            if not e.sizes.constant_sized:
                return 0, f"{e.name}_size({variable_name})" if is_cpp() else f"{module_name}_{e.name}_size(&{variable_name})"
            else:
                return e.sizes.maximum_size, None
        case model.DataTypeString() | model.DataTypeCstring():
            return 1, f"{variable_name}.size()" if is_cpp() else f"STRING_SIZE({variable_name})"
        case model.DataTypeSizedCstring():
            return 4, f"STRING_SIZE({variable_name})"
        case model.DataTypePackedGUID():
            return 0, f"wwm_packed_guid_size({variable_name})"
        case model.DataTypeAchievementDoneArray() | model.DataTypeAchievementInProgressArray() \
             | model.DataTypeAddonArray() | model.DataTypeCacheMask() \
             | model.DataTypeVariableItemRandomProperty() \
             | model.DataTypeInspectTalentGearMask() \
             | model.DataTypeNamedGUID() \
             | model.DataTypeEnchantMask():
            return 0, f"{extra_indirection}{name}.size()"
        case model.DataTypeUpdateMask():
            return 0, f"{module_name}_update_mask_size(&{variable_name})"
        case model.DataTypeMonsterMoveSpline():
            return 0, f"wwm_monster_move_spline_size(&{variable_name})"
        case model.DataTypeAuraMask():
            return 0, f"{module_name}_aura_mask_size(&{variable_name})"
        case model.DataTypeArray(compressed=compressed):
            if compressed:
                return None
            size = array_size_inner_values(data_type, name, extra_indirection)
            if isinstance(size, str):
                return 0, size
            elif isinstance(size, int):
                return size, None
        case model.DataTypeInteger(integer_type=integer_type):
            return integer_type_to_size(integer_type), None
        case model.DataTypeBool(integer_type=integer_type):
            return integer_type_to_size(integer_type), None
        case model.DataTypeEnum(integer_type=integer_type):
            return integer_type_to_size(integer_type), None
        case model.DataTypeFlag(integer_type=integer_type):
            return integer_type_to_size(integer_type), None

        case model.DataTypeSpell() | model.DataTypeItem() | model.DataTypeDateTime() \
             | model.DataTypeGold() \
             | model.DataTypeSeconds() \
             | model.DataTypeMilliseconds() \
             | model.DataTypeIPAddress():
            return 4, None
        case model.DataTypePopulation():
            return 4, None
        case model.DataTypeGUID():
            return 8, None
        case model.DataTypeLevel():
            return 1, None
        case model.DataTypeLevel16() | model.DataTypeSpell16():
            return 2, None
        case model.DataTypeLevel32():
            return 4, None
        case model.DataTypeFloatingPoint():
            return 4, None

        case v:
            raise Exception(f"unhandled ty {v}")

    return None


def get_size_and_remaining_members(members: list[model.StructMember], module_name: str, extra_indirection: str) -> \
        typing.Tuple[
            int, list[str], list[model.StructMember]]:
    count = 0
    strings = []
    uncounted_members: typing.List[model.StructMember] = []

    for m in members:
        match m:
            case model.StructMemberDefinition(struct_member_content=d):
                addable = addable_size_value(d.data_type, "self.", d.name, module_name, extra_indirection)
                if addable is not None:
                    addable_count, addable_string = addable
                    count += addable_count
                    if addable_string is not None:
                        strings.append(addable_string)
                else:
                    uncounted_members.append(m)
            case model.StructMemberIfStatement():
                uncounted_members.append(m)
            case v:
                raise Exception(f"{v}")

    return count, strings, uncounted_members


def print_size_inner(s: Writer, m: model.StructMember, module_name: str, extra_indirection: str):
    extra_self = "self."

    match m:
        case model.StructMemberDefinition(struct_member_content=d):
            match d.data_type:
                case model.DataTypeArray(compressed=compressed, inner_type=inner_type, size=size):
                    if compressed:
                        s.wln(f"_{d.name}_fmt = ''")
                        s.wln(f"_{d.name}_data = []")
                        s.newline()

                        s.open(f"if len({extra_self}{d.name}) != 0:")

                        print_array_write_inner(s, d, inner_type, f"_{d.name}_", extra_self)

                        s.wln(f"_{d.name}_bytes = struct.pack(_{d.name}_fmt, *_{d.name}_data)")
                        s.wln(f"_size += len(_{d.name}_bytes) + 4")
                        s.close()

                        s.open("else:")
                        s.wln("_size += 4")
                        s.close()  # else:

                    else:
                        loop_max = ""
                        variable_name = f"obj{extra_indirection}.{d.name}" if is_cpp() else f"object{extra_indirection}->{d.name}"

                        fixed_prefix = "(*" if type(size) is model.ArraySizeFixed else ""
                        fixed_suffix = ")" if type(size) is model.ArraySizeFixed else ""

                        match size:
                            case model.ArraySizeFixed(size=size):
                                loop_max = size
                            case model.ArraySizeVariable(size=size):
                                loop_max = f"(int)object->{extra_indirection}{size}"
                            case model.ArraySizeEndless():
                                loop_max = f"(int)object->amount_of_{d.name}"
                        if is_cpp():
                            s.open_curly(f"for(const auto& v : {variable_name})")
                        else:
                            s.open_curly("/* C89 scope to allow variable declarations */")
                            s.wln("int i;")
                            s.open_curly(f"for(i = 0; i < {loop_max}; ++i)")

                        match inner_type:
                            case model.ArrayTypeInteger(integer_type=integer_type):
                                s.wln(f"_size += {integer_type_to_size(integer_type)};")

                            case model.ArrayTypeSpell():
                                s.wln(f"_size += 4;")

                            case model.ArrayTypeStruct(struct_data=e):
                                version = first_version_as_module(e.tags)
                                size = f"{version}_{e.name}_size(&{fixed_prefix}{variable_name}[i]{fixed_suffix})" if not e.sizes.constant_sized else str(
                                    e.sizes.maximum_size)
                                size  = f"{e.name}_size(v)" if is_cpp() else size
                                s.wln(
                                    f"_size += {size};")

                            case model.ArrayTypeCstring():
                                s.wln(f"_size += STRING_SIZE({fixed_prefix}{variable_name}[i]{fixed_suffix});")

                            case model.ArrayTypePackedGUID():
                                s.wln(
                                    f"_size += wwm_packed_guid_size({fixed_prefix}{variable_name}[i]{fixed_suffix});")

                            case _:
                                raise Exception(f"array size unknown type {inner_type}")

                        if not is_cpp():
                            s.closing_curly() # C89 scope

                        s.closing_curly()  # array scope
                case v:
                    raise Exception(f"{v}")

        case model.StructMemberIfStatement(struct_member_content=statement):
            print_size_if_statement(s, statement, False, module_name, extra_indirection)

        case v:
            raise Exception(f"{v}")

    s.newline()


def print_size_if_statement(s: Writer, statement: model.IfStatement, is_else_if: bool, module_name: str,
                            extra_indirection: str):
    extra_elseif = ""
    if is_else_if:
        extra_elseif = "else "

    extra_self = "self."

    print_if_statement_header(s, statement, extra_elseif, extra_self, module_name)

    print_size_until_inner_members(s, statement.members, None, False, False, module_name, extra_indirection)

    s.closing_curly()  # if

    for elseif in statement.else_if_statements:
        print_size_if_statement(s, elseif, True, module_name, extra_indirection)
