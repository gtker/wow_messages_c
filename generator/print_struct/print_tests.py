import typing

from print_struct import container_has_c_members

import model
from print_struct.struct_util import container_should_have_size_function, all_members_from_container
from util import first_login_version, VERSIONS, world_version_to_module_name, \
    should_print_container, container_is_unencrypted, login_version_matches, version_matches, version_to_module_name

from writer import Writer


def print_login_tests(s: Writer, m: model.Objects, v: int):
    module_name = version_to_module_name(v)
    s.wln(f"#include \"wow_login_messages/{module_name}.h\"")
    s.wln("#include \"test_utils.h\"")
    s.newline()

    s.open_curly("int main(void)")

    for i, e in enumerate(m.messages):
        if not login_version_matches(e.tags, v):
            continue
        if first_login_version(e.tags) != v:
            continue
        if len(e.tests) != 0:
            print_tests_for_login_message(s, e, v)

    s.wln("return 0;")
    s.closing_curly()


def print_tests_for_login_message(s: Writer, e: model.Container, v: int):
    s.wln(f"/* {e.name} */")
    for i, test_case in enumerate(e.tests):
        s.w(f"#define {e.name}{i} ")
        for b in test_case.raw_bytes:
            s.w_no_indent(f"{b}, ")

        s.newline()

        module_name = version_to_module_name(v)
        module_name_pascal = module_name.capitalize()
        side = ""
        match e.object_type:
            case model.ObjectTypeClogin():
                side = "Client"
            case model.ObjectTypeSlogin():
                side = "Server"

        opcode_name = e.name.replace('_Client', '').replace('_Server', '')

        macro_name = "ASSERT_SUCCESS" if container_has_c_members(e) else "ASSERT_SUCCESS_EMPTY"

        s.wln(
            f"{macro_name}({e.name}, {i}, {opcode_name}, {module_name_pascal}{side}OpcodeContainer, {module_name}, {side.lower()}, Login, wlm);")

        s.newline()


def print_world_tests(s: Writer, messages: typing.Iterator[model.Container], v: model.WorldVersion):
    module_name = world_version_to_module_name(v)
    s.wln(f"#include \"wow_world_messages/{module_name}.h\"")
    s.wln("#include \"test_utils.h\"")
    s.newline()

    s.open_curly("int main(void)")

    for e in messages:
        if len(e.tests) != 0:
            print_tests_for_world_message(s, e, v)

    s.wln("return 0;")
    s.closing_curly()  # int main(void)


def print_tests_for_world_message(s: Writer, e: model.Container, v: model.WorldVersion):
    s.wln(f"/* {e.name} */")
    for i, test_case in enumerate(e.tests):
        s.w(f"#define {e.name}{i} ")
        for b in test_case.raw_bytes:
            s.w_no_indent(f"{b}, ")

        s.newline()

        module_name = version_to_module_name(v)
        module_name_pascal = module_name.capitalize()
        side = ""
        match e.object_type:
            case model.ObjectTypeCmsg():
                side = "Client"
            case model.ObjectTypeSmsg():
                side = "Server"

        opcode_name = e.name.replace('_Client', '').replace('_Server', '')

        macro_name = "ASSERT_SUCCESS" if container_has_c_members(e) else "ASSERT_SUCCESS_EMPTY"

        s.wln(
            f"{macro_name}({e.name}, {i}, {opcode_name}, {module_name_pascal}{side}OpcodeContainer, {module_name}, {side.lower()}, World, wwm);")

        s.newline()
