from print_struct.struct_util import print_if_statement_header, container_has_c_members, all_members_from_container, \
    container_module_prefix
from util import get_export_define, is_cpp, snake_case_to_pascal_case

import model
from print_struct.print_members import print_members_definitions, print_member_definition
from print_struct.print_read import print_read
from print_struct.print_size import print_size
from print_struct.print_write import print_write
from writer import Writer


def print_struct(s: Writer, h: Writer, container: model.Container, module_name: str):
    if container_has_c_members(container) or is_cpp():
        if is_cpp():
            h.open_curly(f"struct {container.name}")
            if container.optional is not None:
                h.open_curly(f"struct {snake_case_to_pascal_case(container.optional.name)}")
                for member in container.optional.members:
                    print_member_definition(h, member, module_name)
                h.closing_curly(f";")
                h.newline()

            for member in container.members:
                print_member_definition(h, member, module_name)

            if container.optional is not None:
                h.wln(f"std::unique_ptr<{snake_case_to_pascal_case(container.optional.name)}> {container.optional.name};")

            if type(container.object_type) is not model.ObjectTypeStruct:
                h.newline()
                export = get_export_define(container.tags)
                if type(container.object_type) is model.ObjectTypeMsg:
                    h.wln(f"{export} std::vector<unsigned char> write_smsg() const;")
                    h.wln(f"{export} std::vector<unsigned char> write_cmsg() const;")
                else:
                    h.wln(f"{export} std::vector<unsigned char> write() const;")

            h.closing_curly(";") # struct
        else:
            if container.optional is not None:
                h.open_curly("typedef struct")
                for member in container.optional.members:
                    print_member_definition(h, member, module_name)
                h.closing_curly(f" {module_name}_{container.name}_{container.optional.name};")
                h.newline()

            h.open_curly(f"typedef struct")
            print_members_definitions(h, container, module_name)
            if container.optional is not None:
                h.wln(f"{module_name}_{container.name}_{container.optional.name}* {container.optional.name};")
            h.closing_curly(f" {module_name}_{container.name};")

    print_size(s, container, module_name)

    print_read(s, container, module_name)

    match container.object_type:
        case model.ObjectTypeMsg(opcode=opcode):
            print_write(s, h, container, model.ObjectTypeCmsg("CMsg", opcode), module_name, "")
            print_write(s, h, container, model.ObjectTypeSmsg("SMsg", opcode), module_name, "")
        case _:
            print_write(s, h, container, container.object_type, module_name, "")

    if not is_cpp():
        print_free(s, h, container, module_name)

    h.newline()


def container_has_free(e: model.Container, module_name: str) -> bool:
    writer = Writer()

    if e.optional is not None:
        return True

    print_free_function_body(writer, e, module_name)

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

    needs_loop = False
    for d in all_members_from_container(e):
        match d.data_type:
            case model.DataTypeArray(inner_type=inner_type):
                match inner_type:
                    case model.ArrayTypeStruct(struct_data=struct_data):
                        if container_has_free(struct_data, module_name):
                            needs_loop = True
                    case model.ArrayTypeCstring():
                        needs_loop = True

    if needs_loop:
        s.wln("size_t i;")
        s.newline()

    print_free_function_body(s, e, module_name)

    s.closing_curly()  # function_declaration
    s.newline()


def print_free_function_body(s: Writer, e: model.Container, module_name: str):
    for m in e.members:
        print_free_member(s, m, module_name, "")
    if e.optional is not None:
        for m in e.optional.members:
            print_free_member(s, m, module_name, f"{e.optional.name}->")

        s.wln(f"free(object->{e.optional.name});")


def print_free_member(s: Writer, m: model.StructMember, module_name: str, extra_indirection: str):
    match m:
        case model.StructMemberDefinition(_tag, definition):
            print_free_struct_member(s, definition, module_name, extra_indirection)

        case model.StructMemberIfStatement(_tag, statement):
            print_free_if_statement(s, statement, False, module_name, extra_indirection)

        case _:
            raise Exception("invalid struct member")


def print_free_if_statement(s: Writer, statement: model.IfStatement, is_else_if: bool, module_name: str, extra_indirection: str):
    extra_elseif = ""
    if is_else_if:
        extra_elseif = "else "

    print_if_statement_header(s, statement, extra_elseif, extra_indirection, module_name)

    for m in statement.members:
        print_free_member(s, m, module_name, extra_indirection)

    s.closing_curly()

    for elseif in statement.else_if_statements:
        print_free_if_statement(s, elseif, True, module_name, extra_indirection)


IS_INSIDE_FIXED_ARRAY = False

def print_free_struct_member(s: Writer, d: model.Definition, module_name: str, extra_indirection: str):
    variable_name = f"object->{extra_indirection}{d.name}"
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
                version = container_module_prefix(e.tags, module_name)

                s.wln(f"{version}_{e.name}_free(&{variable_name});")

        case model.DataTypeUpdateMask():
            pass

        case model.DataTypeAuraMask():
            pass

        case model.DataTypeVariableItemRandomProperty():
            pass

        case model.DataTypeMonsterMoveSpline():
            s.wln(f"wwm_monster_move_spline_free(&{variable_name});")
            s.newline()

        case model.DataTypeNamedGUID():
            s.wln(f"wwm_named_guid_free(&{variable_name});")
            s.newline()

        case model.DataTypeEnchantMask():
            pass

        case model.DataTypeInspectTalentGearMask():
            pass

        case model.DataTypeCacheMask():
            pass

        case model.DataTypeAddonArray():
            s.wln(f"free({variable_name}.addons);")

        case model.DataTypeAchievementDoneArray():
            s.wln(f"{module_name}_achievement_done_array_free(&{variable_name});")

        case model.DataTypeAchievementInProgressArray():
            s.wln(f"{module_name}_achievement_in_progress_array_free(&{variable_name});")

        case model.DataTypeArray(inner_type=inner_type, size=size):
            extra = ""
            match size:
                case model.ArraySizeFixed(size=size):
                    loop_variable = size
                    extra = "*"
                case model.ArraySizeVariable(size=size):
                    loop_variable = f"object->{size}"
                case model.ArraySizeEndless():
                    loop_variable = f"object->amount_of_{d.name}"
                case _:
                    raise Exception("invalid size")

            match inner_type:
                case model.ArrayTypeStruct(struct_data=struct_data):
                    if container_has_free(struct_data, module_name):
                        s.open_curly(f"for (i = 0; i < {loop_variable}; ++i)")
                        s.wln(f"{container_module_prefix(struct_data.tags, module_name)}_{struct_data.name}_free(&(({extra}{variable_name})[i]));")
                        s.closing_curly() # for int i
                case model.ArrayTypeCstring():
                    s.open_curly(f"for (i = 0; i < {loop_variable}; ++i)")
                    s.wln(f"FREE_STRING((({extra}{variable_name})[i]));")
                    s.closing_curly() # for int i

            s.wln(f"free({variable_name});")
            s.wln(f"{variable_name} = NULL;")
