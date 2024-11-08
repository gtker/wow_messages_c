import typing

import model
from util import pascal_case_to_snake_case, first_version_as_module, world_version_to_module_name, \
    version_to_module_name
from writer import Writer


def print_skip(container: model.Container, reason: str):
    print(f"Skipping {container.name} because it {reason}")


def container_should_print(container: model.Container) -> bool:
    match container.object_type:
        case model.ObjectTypeMsg():
            print_skip(container, "is MSG")
            return False

    if container.optional is not None:
        print_skip(container, "has optional")
        return False

    if container.tags.compressed is not None:
        return False

    for member in all_members_from_container(container):
        match member.data_type:
            case model.DataTypeArray(compressed=compressed, inner_type=inner_type, size=size):
                if compressed:
                    print_skip(container, "has compressed array")
                    return False

                match size:
                    case model.ArraySizeEndless():
                        print_skip(container, "has endless array")
                        return False

                match inner_type:
                    case model.ArrayTypeStruct(struct_data=e):
                        if not container_should_print(e):
                            print_skip(container, f"has struct {e.name}")
                            return False

            # Wrath/TBC below
            case model.DataTypeEnchantMask():
                print_skip(container, f"has enchant mask")
                return False

            case model.DataTypeNamedGUID():
                print_skip(container, f"has named guid")
                return False

            case model.DataTypeInspectTalentGearMask():
                print_skip(container, f"has inspect talent gear mask")
                return False

            case model.DataTypeVariableItemRandomProperty():
                print_skip(container, f"has variable item random property")
                return False

            case model.DataTypeCacheMask():
                print_skip(container, f"has cache mask")
                return False

            case model.DataTypeAddonArray():
                print_skip(container, f"has addon array")
                return False

            case model.DataTypeAchievementDoneArray():
                print_skip(container, f"has achievement done array")
                return False

            case model.DataTypeAchievementInProgressArray():
                print_skip(container, f"has achievement in progress array")
                return False

    return True


def container_should_have_size_function(container: model.Container):
    match container.object_type:
        case model.ObjectTypeCmsg() | model.ObjectTypeSmsg() | model.ObjectTypeMsg() | model.ObjectTypeStruct():
            if container.sizes.constant_sized:
                return False
        case _:
            if container.manual_size_subtraction is None:
                return False

    return True


def integer_type_to_short(ty: model.IntegerType) -> str:
    prefix = "i" if integer_type_is_signed(ty) else "u"
    return f"{prefix}{integer_type_to_size(ty) * 8}"


def integer_type_is_signed(ty: model.IntegerType) -> bool:
    match ty:
        case model.IntegerType.I16:
            return True
        case model.IntegerType.I32:
            return True
        case model.IntegerType.I64:
            return True
        case model.IntegerType.I8:
            return True
        case model.IntegerType.U16:
            return False
        case model.IntegerType.U32:
            return False
        case model.IntegerType.U48:
            return False
        case model.IntegerType.U64:
            return False
        case model.IntegerType.U8:
            return False


def integer_type_to_size(ty: model.IntegerType) -> int:
    match ty:
        case model.IntegerType.I16:
            return 2
        case model.IntegerType.I32:
            return 4
        case model.IntegerType.I64:
            return 8
        case model.IntegerType.I8:
            return 1
        case model.IntegerType.U16:
            return 2
        case model.IntegerType.U32:
            return 4
        case model.IntegerType.U48:
            return 6
        case model.IntegerType.U64:
            return 8
        case model.IntegerType.U8:
            return 1


def container_has_c_members(e: model.Container) -> bool:
    for m in all_members_from_container(e):
        if m.constant_value is None and m.used_as_size_in is None:
            return True

    return False


def integer_type_to_c_str(integer_type: model.IntegerType) -> str:
    prefix = "u"
    if integer_type_is_signed(integer_type):
        prefix = ""
    size = integer_type_to_size(integer_type) * 8
    return f"{prefix}int{size}_t"


def type_to_c_str(ty: model.DataType, module_name: str) -> str:
    match ty:
        case model.DataTypeInteger(integer_type=integer_type):
            return integer_type_to_c_str(integer_type)
        case model.DataTypeBool():
            return "bool"

        case model.DataTypeString() | model.DataTypeCstring() | model.DataTypeSizedCstring():
            if module_name in {"vanilla", "tbc", "wrath"}:
                return "WowWorldString"

            return "WowLoginString"
        case model.DataTypeFloatingPoint():
            return "float"
        case model.DataTypeStruct(struct_data=e):
            version = first_version_as_module(e.tags)
            return f"{version}_{e.name}"
        case model.DataTypeEnum(type_name=type_name):
            return f"{module_name}_{type_name}"
        case model.DataTypeFlag(type_name=type_name):
            return f"{module_name}_{type_name}"

        case model.DataTypeArray(inner_type=inner_type):
            return f"{array_type_to_c_str(inner_type)}*"

        case model.DataTypeLevel():
            return "uint8_t"

        case model.DataTypeLevel16():
            return "uint16_t"
        case model.DataTypeSpell16():
            return "uint16_t"

        case model.DataTypeGold() | model.DataTypeIPAddress() | model.DataTypeLevel32() \
             | model.DataTypeSpell() | model.DataTypeItem() | model.DataTypeMilliseconds() \
             | model.DataTypeSeconds() | model.DataTypeDateTime():
            return "uint32_t"
        case model.DataTypeGUID() | model.DataTypePackedGUID():
            return "uint64_t"

        case model.DataTypePopulation():
            return "float"

        case model.DataTypeUpdateMask():
            return f"{module_name}_UpdateMask"

        case model.DataTypeAchievementDoneArray():
            return "AchievementDoneArray"
        case model.DataTypeAchievementInProgressArray():
            return "AchievementInProgressArray"
        case model.DataTypeAddonArray():
            return "AddonArray"
        case model.DataTypeAuraMask():
            return f"{module_name}_AuraMask"

        case model.DataTypeEnchantMask():
            return "EnchantMask"
        case model.DataTypeInspectTalentGearMask():
            return "InspectTalentGearMask"
        case model.DataTypeMonsterMoveSpline():
            return "MonsterMoveSpline"
        case model.DataTypeNamedGUID():
            return "NamedGUID"
        case model.DataTypeVariableItemRandomProperty():
            return "VariableItemRandomProperty"
        case model.DataTypeCacheMask():
            return "CacheMask"
        case v:
            raise Exception(f"{v}")


def array_type_to_c_str(ty: model.ArrayType):
    match ty:
        case model.ArrayTypeCstring():
            return "WowWorldString"
        case model.ArrayTypeGUID():
            return "uint64_t"
        case model.ArrayTypeInteger(integer_type=integer_type):
            return integer_type_to_c_str(integer_type)
        case model.ArrayTypeSpell():
            return "uint32_t"
        case model.ArrayTypePackedGUID():
            return "uint64_t"
        case model.ArrayTypeStruct(struct_data=struct_data):
            version = first_version_as_module(struct_data.tags)
            return f"{version}_{struct_data.name}"
        case v:
            raise Exception(f"{v}")


def all_members_from_container(
        container: model.Container,
) -> typing.List[model.Definition]:
    out_members: typing.List[model.Definition] = []

    def inner(m: model.StructMember, out_members: typing.List[model.Definition]):
        def inner_if(
                statement: model.IfStatement, out_members: typing.List[model.Definition]
        ):
            for member in statement.members:
                inner(member, out_members)

            for elseif in statement.else_if_statements:
                inner_if(elseif, out_members)

        match m:
            case model.StructMemberDefinition(_tag, definition):
                out_members.append(definition)

            case model.StructMemberIfStatement(_tag, struct_member_content=statement):
                inner_if(statement, out_members)

            case v:
                raise Exception(f"invalid struct member {v}")

    for m in container.members:
        inner(m, out_members)

    if container.optional is not None:
        for m in container.optional.members:
            inner(m, out_members)

    return out_members


def print_optional_statement_header(s: Writer, optional: model.OptionalMembers):
    s.wln(f"# {optional.name}: optional")
    s.w("if")
    i = 0

    extra_self = "self."

    for m in optional.members:
        match m:
            case model.StructMemberDefinition(struct_member_content=d):
                if d.constant_value is not None \
                        or d.size_of_fields_before_size is not None:
                    continue

                if i != 0:
                    s.w_no_indent(" and")

                s.w_no_indent(f" {extra_self}{d.name} is not None")

        i += 1

    s.wln_no_indent(":")
    s.inc_indent()


def print_if_statement_header(
        s: Writer,
        statement: model.IfStatement,
        extra_elseif: str,
        extra_self: str,
        module_name: str
):
    original_type = type_to_c_str(statement.original_type, module_name).replace(f"{module_name}_", '')
    original_type_pascal = pascal_case_to_snake_case(original_type).upper()
    var_name = statement.variable_name

    first_version = first_version_as_module(statement.original_type.tags).upper()

    match statement.definer_type:
        case model.DefinerType.ENUM:
            if len(statement.values) == 1:
                s.open_curly(
                    f"{extra_elseif}if (object->{var_name} == {first_version}_{original_type_pascal}_{statement.values[0]})"
                )
            else:
                s.w(f"{extra_elseif}if (")
                for i, val in enumerate(statement.values):
                    if i != 0:
                        s.w_no_indent("|| ")
                    s.w_no_indent(
                        f"object->{var_name} == {first_version}_{original_type_pascal}_{statement.values[i]}")
                s.wln_no_indent(") {")
                s.inc_indent()

        case model.DefinerType.FLAG:
            s.w(f"{extra_elseif}if (")
            for i, val in enumerate(statement.values):
                if i != 0:
                    s.w_no_indent("|| ")
                s.w_no_indent(
                    f"(object->{var_name} & {first_version}_{original_type_pascal}_{statement.values[i]}) != 0")
            s.wln_no_indent(") {")
            s.inc_indent()
        case _:
            raise Exception()
