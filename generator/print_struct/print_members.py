import model
from model import Container
from print_struct.struct_util import type_to_c_str
from writer import Writer


def print_members_definitions(s: Writer, container: Container, module_name: str):
    for member in container.members:
        print_member_definition(s, member, module_name)

    s.newline()


def print_member_definition(non_optional: Writer, member: model.StructMember, module_name: str):
    match member:
        case model.StructMemberDefinition(
            _tag,
            model.Definition(
                data_type=data_type,
                name=name,
                constant_value=constant_value,
                size_of_fields_before_size=size_of_fields_before_size,
                tags=tags,
            ),
        ):
            if (
                    size_of_fields_before_size is not None
                    or constant_value is not None
            ):
                return

            match data_type:
                case model.DataTypeArray(size=model.ArraySizeEndless()):
                    non_optional.wln(f"uint32_t amount_of_{name};")

            match data_type:
                case model.DataTypeArray(size=model.ArraySizeFixed(size=size)):
                    non_optional.wln(
                        f"{type_to_c_str(data_type, module_name)} (*{name})[{size}];")
                case _:
                    non_optional.wln(f"{type_to_c_str(data_type, module_name)} {name};")

        case model.StructMemberIfStatement(_tag, struct_member_content=statement):
            print_member_if_statement(non_optional, statement, module_name)

        case _:
            raise Exception("invalid struct member")


def print_member_if_statement(non_optional: Writer, statement: model.IfStatement, module_name: str):
    for member in statement.members:
        print_member_definition(non_optional, member, module_name)

    for elseif in statement.else_if_statements:
        print_member_if_statement(non_optional, elseif, module_name)
