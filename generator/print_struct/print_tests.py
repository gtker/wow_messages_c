import typing

from print_struct import container_has_c_members, container_has_free

import model
from print_struct.struct_util import container_should_have_size_function, all_members_from_container, \
    container_has_compressed_array, container_uses_compression
from util import first_login_version, VERSIONS, world_version_to_module_name, \
    should_print_container, container_is_unencrypted, login_version_matches, version_matches, version_to_module_name, \
    is_world, is_cpp

from writer import Writer


def print_login_tests(s: Writer, m: model.Objects, v: int):
    for i, e in enumerate(m.messages):
        if not login_version_matches(e.tags, v):
            continue
        if first_login_version(e.tags) != v:
            continue
        if len(e.tests) != 0:
            print_tests_for_message(s, e, v)


def print_tests_for_message(s: Writer, e: model.Container, v: typing.Union[int | model.WorldVersion]):
    library_type = "World" if is_world(e.tags) else "Login"
    library_prefix = "wwm" if is_world(e.tags) else "wlm"

    if not is_cpp():
        s.wln(f"/* {e.name} */")
    for i, test_case in enumerate(e.tests):
        module_name = version_to_module_name(v)
        module_name_pascal = module_name.capitalize()
        side = ""
        match e.object_type:
            case model.ObjectTypeClogin():
                side = "Client"
            case model.ObjectTypeSlogin():
                side = "Server"
            case model.ObjectTypeCmsg():
                side = "Client"
            case model.ObjectTypeSmsg():
                side = "Server"

        opcode_name = e.name.replace('_Client', '').replace('_Server', '')
        print_individual_test(s, e, test_case, i, opcode_name, f"{module_name_pascal}{side}OpcodeContainer",
                              module_name,
                              side.lower(), library_type, library_prefix)


def print_individual_test(s: Writer, e: model.Container, test_case: model.TestCase, i: int, opcode_id: str,
                          opcode_type: str,
                          function_version: str, function_side: str, library_type: str, library_prefix: str):
    if is_cpp():
        s.open_curly(f"/* {e.name} */")

        s.w("const std::vector<unsigned char> buffer = {")
        for b in test_case.raw_bytes:
            s.w_no_indent(f"{b}, ")
        s.wln_no_indent("};")

        s.wln("auto reader = ByteReader(buffer);")
        s.newline()

        s.wln(
            f"const auto opcode = ::wow_{library_type.lower()}_messages::{function_version}::read_{function_side}_opcode(reader);")

        s.open_curly("if (opcode.is_none())")
        s.wln(
            f'printf(__FILE__ ":" STRINGIFY(__LINE__) " {e.name} {i} read invalid opcode");')
        s.wln("abort();")
        s.closing_curly()  # if (opcode.is_none())
        s.newline()

        s.open_curly(
            f"if (opcode.opcode != ::wow_{library_type.lower()}_messages::{function_version}::{function_side.capitalize()}Opcode::Opcode::{opcode_id})")
        s.wln(
            f'printf(__FILE__ ":" STRINGIFY(__LINE__) " {e.name} {i} read invalid opcode");')
        s.wln("abort();")
        s.closing_curly()  # if (opcode.opcode)
        s.newline()

        s.wln(
            f"const std::vector<unsigned char> write_buffer = ::wow_{library_type.lower()}_messages::{function_version}::write_opcode(opcode);")
        s.newline()

        if container_uses_compression(e):
            s.wln("auto reader2 = ByteReader(write_buffer);")
            s.newline()

            s.wln(
                f"const auto opcode2 = ::wow_{library_type.lower()}_messages::{function_version}::read_{function_side}_opcode(reader2);")

            s.open_curly("if (opcode2.is_none())")
            s.wln(
                f'printf(__FILE__ ":" STRINGIFY(__LINE__) " {e.name} {i} read invalid second opcode");')
            s.wln("abort();")
            s.closing_curly()  # if (opcode2.is_none())
            s.newline()

            s.open_curly(
                f"if (opcode2.opcode != ::wow_{library_type.lower()}_messages::{function_version}::{function_side.capitalize()}Opcode::Opcode::{opcode_id})")
            s.wln(
                f'printf(__FILE__ ":" STRINGIFY(__LINE__) " {e.name} {i} read invalid second opcode");')
            s.wln("abort();")
            s.closing_curly()  # if (opcode.opcode)
            s.newline()

            s.wln(
                f"const std::vector<unsigned char> write_buffer2 = ::wow_{library_type.lower()}_messages::{function_version}::write_opcode(opcode2);")
            s.newline()
            s.wln(
                f'world_test_compare_buffers(write_buffer.data(), write_buffer2.data(), write_buffer2.size(),  __FILE__ ":" STRINGIFY(__LINE__) " {e.name} {i}", TEST_UTILS_SIDE_{function_side.upper()});')
        elif is_world(e.tags):
            s.wln(
                f'world_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " {e.name} {i}", TEST_UTILS_SIDE_{function_side.upper()});')
        else:
            s.wln(
                f'wlm_test_compare_buffers(buffer.data(), write_buffer.data(), write_buffer.size(),  __FILE__ ":" STRINGIFY(__LINE__) " {e.name} {i}");')
        s.closing_curly()  # /* name */
    else:
        s.open_curly("do")
        s.w("unsigned char buffer[] = {")
        for b in test_case.raw_bytes:
            s.w_no_indent(f"{b}, ")
        s.wln_no_indent("};")
        s.newline()

        s.wln(f"Wow{library_type}Reader reader = {library_prefix}_create_reader(buffer, sizeof(buffer));")
        s.newline()

        s.wln(f"{opcode_type} opcode;")
        s.wln(f"Wow{library_type}Result result = {function_version}_{function_side}_opcode_read(&reader, &opcode);")
        s.newline()

        s.open_curly(f"if (result != {library_prefix.upper()}_RESULT_SUCCESS)")
        s.wln(
            f'printf(__FILE__ ":" STRINGIFY(__LINE__) " {e.name} {i} failed to read: \'%s\'\\n", {library_prefix}_error_code_to_string(result));')
        s.wln("abort();")
        s.newline()
        s.closing_curly()  # if (result != 0)

        s.open_curly(f"if (opcode.opcode != {opcode_id})")
        s.wln(
            f'printf(__FILE__ ":" STRINGIFY(__LINE__) " {e.name} {i} read wrong opcode: \'0x%x\' instead of \'0x%x\'\\n", opcode.opcode, {opcode_id});')
        s.wln("abort();")
        s.newline()
        s.closing_curly()  # if (opcode.opcode != opcode)

        s.open_curly("/* C89 scope to allow variable declarations */")
        s.wln(f"Wow{library_type}Writer writer = {library_prefix}_create_writer(write_buffer, sizeof(write_buffer));")
        extra_argument = f", &opcode.body.{e.name}" if container_has_c_members(e) else ""
        s.wln(f"result = {function_version}_{e.name}_write(&writer{extra_argument});")
        s.newline()

        s.open_curly(f"if (result != {library_prefix.upper()}_RESULT_SUCCESS)")
        s.wln(
            f'printf(__FILE__ ":" STRINGIFY(__LINE__) " {e.name} {i} failed to write: \'%s\'\\n", {library_prefix}_error_code_to_string(result));')
        s.wln("abort();")
        s.newline()
        s.closing_curly()  # if (result != 0)

        if container_uses_compression(e):
            s.wln("/* TODO compressed array compare do what? */")
        else:
            s.wln(
                f'wlm_test_compare_buffers(buffer, write_buffer, sizeof(buffer), __FILE__ ":" STRINGIFY(__LINE__) " {e.name} {i}");')

        s.closing_curly()  # C89 scope

        if container_has_free(e, function_version):
            s.wln(f"{function_version}_{function_side}_opcode_free(&opcode);")

        s.closing_curly("while (0);")  # do
        s.newline()


def print_login_test_prefix(tests: Writer, m: model.IntermediateRepresentationSchema):
    if is_cpp():
        tests.wln("#include <utility>")
        tests.newline()

    pp = "pp" if is_cpp() else ""
    cpp = "_cpp" if is_cpp() else ""

    tests.wln(f"#include \"wow_login_messages{cpp}/all.h{pp}\"")
    for v in m.distinct_login_versions_other_than_all:
        module_name = version_to_module_name(v)
        tests.wln(f"#include \"wow_login_messages{cpp}/{module_name}.h{pp}\"")

    tests.wln("#include \"test_utils.h\"")
    tests.newline()

    if not is_cpp():
        tests.wln("#include <stdlib.h> /* abort() */")
        tests.newline()

    if is_cpp():
        tests.write_block("""
class ByteReader final : public wow_login_messages::Reader
{
public:
    explicit ByteReader(std::vector<unsigned char> buf) : m_buf(std::move(buf)) { }

    uint8_t read_u8() override
    {
        const uint8_t value = m_buf[m_index];
        m_index += 1;

        return value;
    }

    std::vector<unsigned char> m_buf;
    size_t m_index = 0;
};
""")

    if is_cpp():
        tests.open_curly("int main()")
    else:
        tests.wln("unsigned char write_buffer[1 << 16] = {0}; /* uint16_t max */")
        tests.open_curly("int main(void)")


def print_login_test_suffix(tests: Writer):
    tests.wln("return 0;")
    tests.closing_curly()


def print_world_tests(s: Writer, messages: typing.List[model.Container], v: model.WorldVersion):
    module_name = world_version_to_module_name(v)
    pp = "pp" if is_cpp() else ""
    cpp = "_cpp" if is_cpp() else ""
    s.wln(f"#include \"wow_world_messages{cpp}/{module_name}.h{pp}\"")
    s.wln("#include \"test_utils.h\"")
    s.newline()

    if not is_cpp():
        s.wln("#include <stdlib.h> /* abort() */")
        s.newline()

    if is_cpp():
        s.write_block("""
class ByteReader final : public wow_world_messages::Reader
{
public:
    explicit ByteReader(std::vector<unsigned char> buf) : m_buf(std::move(buf)) { }

    uint8_t read_u8() override
    {
        const uint8_t value = m_buf[m_index];
        m_index += 1;

        return value;
    }

    std::vector<unsigned char> m_buf;
    size_t m_index = 0;
};
    """)

    if not is_cpp():
        s.wln("unsigned char write_buffer[1 << 16] = {0}; /* uint16_t max */")

    extra_void: str = "" if is_cpp() else "void"

    s.open_curly(f"int main({extra_void})")

    for e in messages:
        if len(e.tests) != 0:
            print_tests_for_message(s, e, v)

    s.wln("return 0;")
    s.closing_curly()  # int main(void)
