import typing

import model
from print_struct.struct_util import container_has_c_members
from util import login_version_matches, first_version_as_module, first_login_version, \
    get_type_prefix, is_world, version_to_module_name, version_matches, get_export_define, container_needs_size_in_read, \
    is_cpp
from writer import Writer
import print_struct

INVALID_OPCODE = 0xFFFF


def side_matches(e: model.Container, side: str) -> int:
    match e.object_type:
        case model.ObjectTypeClogin(opcode=opcode) | model.ObjectTypeCmsg(opcode=opcode):
            if side == "client":
                return opcode
        case model.ObjectTypeSlogin(opcode=opcode) | model.ObjectTypeSmsg(opcode=opcode):
            if side == "server":
                return opcode
        case model.ObjectTypeMsg(opcode=opcode):
            return opcode

    return INVALID_OPCODE


def print_login_utils_side(s: Writer, h: Writer, messages: list[model.Container],
                           v: typing.Union[int | model.WorldVersion], side: str, opcodes: dict[str, int]):
    has_messages = False
    for e in messages:
        if not version_matches(e.tags, v) or side_matches(e, side) == INVALID_OPCODE:
            continue
        has_messages = True

    if not has_messages:
        return

    side_pascal = side.capitalize()

    module_name = version_to_module_name(v)
    module_name_pascal = module_name.capitalize()

    if is_cpp():
        h.wln(f"using {side.capitalize()}Opcode = std::variant<")
        h.inc_indent()

        first = False
        for e in messages:
            if not version_matches(e.tags, v) \
                    or side_matches(e, side) == INVALID_OPCODE \
                    or (first_version_as_module(e.tags) == "all" and version_to_module_name(v) != "all"):
                continue
            if not first:
                first = True
            else:
                h.wln_no_indent(",")
            h.w(f"{e.name}")

        h.newline()
        h.dec_indent()
        h.wln(">;")
        h.newline()

    else:
        h.open_curly("typedef struct")
        if type(v) is int:
            h.wln(f"WowLoginOpcode opcode;")
        else:
            h.wln(f"Wow{module_name_pascal}WorldOpcode opcode;")
        h.newline()

        h.open_curly("union")

        for e in messages:
            if not version_matches(e.tags, v) \
                    or side_matches(e, side) == INVALID_OPCODE \
                    or (first_version_as_module(e.tags) == "all" and version_to_module_name(v) != "all"):
                continue
            if not container_has_c_members(e):
                continue
            version = first_version_as_module(e.tags)
            h.wln(f"{version}_{e.name} {e.name};")

        h.closing_curly(" body;")  # union

        h.closing_curly(f" {module_name_pascal}{side_pascal}OpcodeContainer;")  # struct
        h.newline()

    write_opcode_write(s, h, messages, v, side, module_name, module_name_pascal, side_pascal)

    write_opcode_read(s, h, messages, v, side, module_name, module_name_pascal, side_pascal, opcodes)

    if not is_cpp():
        write_opcode_free(s, h, messages, v, side, module_name, module_name_pascal, side_pascal)

def write_opcode_write(s: Writer, h: Writer, messages: list[model.Container], v: typing.Union[int | model.WorldVersion],
                       side: str, module_name: str,
                       module_name_pascal: str, side_pascal: str):
    if is_cpp():
        function_declaration = f"std::vector<unsigned char> write_opcode(const {side.capitalize()}Opcode& opcode)"
        h.wln(f"{function_declaration};")
        h.newline()

        s.open_curly(function_declaration)

        for e in messages:
            if not version_matches(e.tags, v) \
                    or side_matches(e, side) == INVALID_OPCODE \
                    or (first_version_as_module(e.tags) == "all" and version_to_module_name(v) != "all"):
                continue
            if not container_has_c_members(e):
                continue
            version = first_version_as_module(e.tags)
            s.open_curly(f"if (const auto p = std::get_if<{e.name}>(&opcode))")
            s.wln("return p->write();")
            s.closing_curly() # if const auto p

        s.newline()
        s.wln("return {}; /* unreachable */")

        s.closing_curly() # function decl
        s.newline()


def write_opcode_read(s: Writer, h: Writer, messages: list[model.Container], v: typing.Union[int | model.WorldVersion],
                      side: str, module_name: str,
                      module_name_pascal: str, side_pascal: str, opcodes: dict[str, int]):
    export = get_export_define(messages[0].tags)
    result_type = get_type_prefix(messages[0].tags)
    if is_cpp():
        function_declaration = f"std::optional<{side.capitalize()}Opcode> read_{side}_opcode(Reader& reader)"
        h.wln(f"{function_declaration};")
        h.newline()

        s.open_curly(function_declaration)

        s.open_curly("enum Opcode")
        for name, opcode in opcodes.items():
            s.wln(f"{name} = 0x{opcode:04x},")
        s.closing_curly(";")
        s.newline()

        if is_world(messages[0].tags):
            if side == "server":
                s.wln("uint16_t _size;")
                s.wln("READ_U16_BE(_size);")
                s.newline()

                s.wln("opcodes->opcode = 0;")
                s.wln("READ_U16(opcodes->opcode);")
            else:
                s.wln("uint16_t _size;")
                s.wln("READ_U16_BE(_size);")
                s.newline()

                s.wln("opcodes->opcode = 0;")
                s.wln("READ_U32(opcodes->opcode);")
        else:
            s.wln("uint8_t opcode = reader.read_u8();")

        s.newline()

        for e in messages:
            if not version_matches(e.tags, v) \
                    or side_matches(e, side) == INVALID_OPCODE \
                    or (first_version_as_module(e.tags) == "all" and version_to_module_name(v) != "all"):
                continue
            s.open_curly(f"if (opcode == Opcode::{e.name.replace('_Client', '').replace('_Server', '')})")
            if not container_has_c_members(e):
                s.wln(f"return wow_login_messages::{module_name}::{e.name}{{}};")
            else:
                s.wln(f"return {e.name}_read(reader);")

            s.closing_curly() # if opcode == opcode

        s.newline()
        s.wln("return {};")

        s.closing_curly() # function decl
    else:
        function_declaration = f"{export} {result_type}Result {module_name}_{side}_opcode_read({result_type}Reader* reader, {module_name_pascal}{side_pascal}OpcodeContainer* opcodes)"

        h.wln(f"{function_declaration};")
        h.newline()

        s.open_curly(function_declaration)

        if is_world(messages[0].tags):
            if side == "server":
                s.wln("uint16_t _size;")
                s.wln("READ_U16_BE(_size);")
                s.newline()

                s.wln("opcodes->opcode = 0;")
                s.wln("READ_U16(opcodes->opcode);")
            else:
                s.wln("uint16_t _size;")
                s.wln("READ_U16_BE(_size);")
                s.newline()

                s.wln("opcodes->opcode = 0;")
                s.wln("READ_U32(opcodes->opcode);")
        else:
            s.wln("opcodes->opcode = 0;")
            s.wln("READ_U8(opcodes->opcode);")

        s.newline()

        s.open_curly("switch (opcodes->opcode)")

        wlm_prefix = "WWM" if is_world(messages[0].tags) else "WLM"

        for e in messages:
            if not version_matches(e.tags, v) \
                    or side_matches(e, side) == INVALID_OPCODE \
                    or (first_version_as_module(e.tags) == "all" and version_to_module_name(v) != "all"):
                continue
            if not container_has_c_members(e):
                continue

            s.wln(f"case {e.name.replace('_Client', '').replace('_Server', '')}:")
            s.inc_indent()

            size_field_size = 2 if side == "server" else 4
            body_size = f", _size - {size_field_size}" if container_needs_size_in_read(e) else ""

            version = first_version_as_module(e.tags)
            s.wln(f"{wlm_prefix}_CHECK_RETURN_CODE({version}_{e.name}_read(reader, &opcodes->body.{e.name}{body_size}));")

            s.wln("break;")
            s.dec_indent()

        s.wln("default:")
        s.inc_indent()
        s.wln("break;")
        s.dec_indent()

        s.closing_curly()  # switch
        s.newline()

        if is_world(messages[0].tags):
            s.wln("return WWM_RESULT_SUCCESS;")
        else:
            s.wln("return WLM_RESULT_SUCCESS;")

        s.closing_curly()  # function_declaration
        s.newline()


def write_opcode_free(s: Writer, h: Writer, messages: list[model.Container], v: typing.Union[int | model.WorldVersion],
                      side: str, module_name: str,
                      module_name_pascal: str, side_pascal: str):
    export = get_export_define(messages[0].tags)
    function_declaration = f"{export} void {module_name}_{side}_opcode_free({module_name_pascal}{side_pascal}OpcodeContainer* opcodes)"
    h.wln(f"{function_declaration};")
    h.newline()

    s.open_curly(function_declaration)

    s.open_curly("switch (opcodes->opcode)")
    for e in messages:
        if not version_matches(e.tags, v) \
                or side_matches(e, side) == INVALID_OPCODE \
                or (first_version_as_module(e.tags) == "all" and version_to_module_name(v) != "all"):
            continue

        if not print_struct.container_has_free(e, module_name):
            continue

        s.wln(f"case {e.name.replace('_Client', '').replace('_Server', '')}:")
        s.inc_indent()

        version = first_version_as_module(e.tags)
        s.wln(f"{version}_{e.name}_free(&opcodes->body.{e.name});")

        s.wln("break;")
        s.dec_indent()

    s.wln("default:")
    s.inc_indent()
    s.wln("break;")
    s.dec_indent()

    s.closing_curly()  # switch

    s.closing_curly()  # function_declaration
    s.newline()


def print_login_utils(s: Writer, h: Writer, messages: list[model.Container], v: typing.Union[int | model.WorldVersion]):
    module_name = version_to_module_name(v)
    module_name_pascal = module_name.capitalize()

    opcodes: dict[str, int] = {}
    for e in messages:
        if not version_matches(e.tags, v):
            continue
        name = e.name.replace('_Client', '').replace('_Server', '')
        op = 0xFFFF
        match e.object_type:
            case model.ObjectTypeClogin(opcode=opcode):
                op = opcode
            case model.ObjectTypeSlogin(opcode=opcode):
                op = opcode
            case model.ObjectTypeSmsg(opcode=opcode):
                op = opcode
            case model.ObjectTypeCmsg(opcode=opcode):
                op = opcode
            case model.ObjectTypeMsg(opcode=opcode):
                op = opcode
            case _:
                continue

        opcodes[name] = op

    if not is_cpp() and type(v) is model.WorldVersion and module_name != "all":
        h.open_curly("typedef enum")

        for i, (name, opcode) in enumerate(opcodes.items()):
            comma = "," if i < len(opcodes.items()) - 1 else ""
            h.wln(f"{name} = {opcode}{comma}")

        h.closing_curly(f" Wow{module_name_pascal}WorldOpcode;")
        h.newline()

    print_login_utils_side(s, h, messages, v, "client", opcodes)
    print_login_utils_side(s, h, messages, v, "server", opcodes)
