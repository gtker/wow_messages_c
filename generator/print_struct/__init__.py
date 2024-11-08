from print_struct.struct_util import print_if_statement_header, container_has_c_members
from util import first_version_as_module, get_export_define

import model
from print_struct.print_members import print_members_definitions
from print_struct.print_read import print_read
from print_struct.print_size import print_size
from print_struct.print_write import print_write
from writer import Writer


def print_struct(s: Writer, h: Writer, container: model.Container, module_name: str):
    first_module = first_version_as_module(container.tags)
    if container_has_c_members(container):
        h.open_curly(f"typedef struct")
        print_members_definitions(h, container, module_name)
        h.closing_curly(f" {first_module}_{container.name};")

    print_size(s, container, module_name)

    print_read(s, container, module_name)

    match container.object_type:
        case model.ObjectTypeMsg(opcode=opcode):
            print_write(s, h, container, model.ObjectTypeCmsg("CMsg", opcode), module_name)
            print_write(s, h, container, model.ObjectTypeSmsg("SMsg", opcode), module_name)
        case _:
            print_write(s, h, container, container.object_type, module_name)

    print_free(s, h, container, module_name)
    h.newline()


def container_has_free(e: model.Container, module_name: str) -> bool:
    writer = Writer()
    for m in e.members:
        print_free_member(writer, m, module_name)

    if writer.inner() == "":
        return False

    return True


def print_free(s: Writer, h: Writer, e: model.Container, module_name):
    if not container_has_free(e, module_name):
        return

    export = get_export_define(e.tags)
    function_declaration = f"{export} void {module_name}_{e.name}_free({module_name}_{e.name}* object)"

    match e.object_type:
        case model.ObjectTypeStruct():
            pass
        case _:
            h.wln(f"{function_declaration};")

    s.open_curly(function_declaration)

    for m in e.members:
        print_free_member(s, m, module_name)

    s.closing_curly()  # function_declaration
    s.newline()


def print_free_member(s: Writer, m: model.StructMember, module_name: str):
    match m:
        case model.StructMemberDefinition(_tag, definition):
            print_free_struct_member(s, definition, module_name)

        case model.StructMemberIfStatement(_tag, statement):
            print_free_if_statement(s, statement, False, module_name)

        case _:
            raise Exception("invalid struct member")


def print_free_if_statement(s: Writer, statement: model.IfStatement, is_else_if: bool, module_name: str):
    extra_elseif = ""
    if is_else_if:
        extra_elseif = "else "

    print_if_statement_header(s, statement, extra_elseif, "self.", module_name)

    for m in statement.members:
        print_free_member(s, m, module_name)

    s.closing_curly()

    for elseif in statement.else_if_statements:
        print_free_if_statement(s, elseif, True, module_name)


def print_free_struct_member(s: Writer, d: model.Definition, module_name: str):
    variable_name = f"object->{d.name}"
    match d.data_type:
        case model.DataTypeInteger(integer_type=integer_type):
            pass

        case model.DataTypeBool(integer_type=integer_type):
            pass

        case model.DataTypeFlag(
            type_name=type_name, integer_type=integer_type
        ):
            pass

        case model.DataTypeEnum(integer_type=integer_type, type_name=type_name):
            pass

        case model.DataTypeString():
            s.wln(f"FREE_STRING({variable_name});")
            s.newline()

        case model.DataTypeCstring():
            s.wln(f"FREE_STRING({variable_name});")
            s.newline()

        case model.DataTypeSizedCstring():
            s.wln(f"FREE_STRING({variable_name});")
            s.newline()

        case model.DataTypeLevel32() | model.DataTypeSpell() | model.DataTypeItem() \
             | model.DataTypeDateTime() | model.DataTypeGold() | model.DataTypeSeconds() \
             | model.DataTypeMilliseconds() | model.DataTypeIPAddress() | model.DataTypePopulation():
            pass

        case model.DataTypeGUID():
            pass

        case model.DataTypeLevel():
            pass

        case model.DataTypeLevel16() | model.DataTypeSpell16():
            pass

        case model.DataTypePackedGUID():
            pass

        case model.DataTypeFloatingPoint():
            pass

        case model.DataTypeStruct(struct_data=e):
            if container_has_free(e, module_name):
                version = first_version_as_module(e.tags)

                s.wln(f"{version}_{e.name}_free(&object->{d.name});")

        case model.DataTypeUpdateMask():
            pass

        case model.DataTypeAuraMask():
            pass

        case model.DataTypeMonsterMoveSpline():
            s.wln(f"wwm_monster_move_spline_free(&{variable_name});")
            s.newline()

        case model.DataTypeEnchantMask():
            s.wln(f"{d.name} = await EnchantMask.read(reader)")

        case model.DataTypeNamedGUID():
            s.wln(f"{d.name} = await NamedGuid.read(reader)")

        case model.DataTypeInspectTalentGearMask():
            s.wln(f"{d.name} = await InspectTalentGearMask.read(reader)")

        case model.DataTypeVariableItemRandomProperty():
            s.wln(f"{d.name} = await VariableItemRandomProperty.read(reader)")

        case model.DataTypeCacheMask():
            s.wln(f"{d.name} = await CacheMask.read(reader)")

        case model.DataTypeAddonArray():
            s.wln(f"{d.name} = await CacheMask.read(reader)")

        case model.DataTypeAchievementDoneArray():
            s.wln(f"{d.name} = await CacheMask.read(reader)")

        case model.DataTypeAchievementInProgressArray():
            s.wln(f"{d.name} = await CacheMask.read(reader)")

        case model.DataTypeArray():
            s.wln(f"free({variable_name});")
            s.wln(f"{variable_name} = NULL;")
