import typing

import model
from print_struct.struct_util import container_has_c_members, container_module_prefix
from util import first_version_as_module, \
    get_type_prefix, is_world, version_to_module_name, version_matches, get_export_define, container_needs_size_in_read, \
    is_cpp, library_type
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
    def matches(container: model.Container) -> bool:
        if not version_matches(container.tags, v) \
                or side_matches(container, side) == INVALID_OPCODE \
                or (first_version_as_module(container.tags) == "all" and version_to_module_name(v) != "all"):
            return False
        return True

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
        h.open_curly(f"struct {side.capitalize()}Opcode")
        h.open_curly("enum class Opcode")

        if is_world(e.tags):
            h.wln("NONE = 0xFFFF,")
        else:
            h.wln("NONE = 0xFF,")
        for e in filter(matches, messages):
            opcode = opcodes[e.name.replace("_Client", "").replace("_Server", "")]

            h.wln(f"{e.name.replace('_Client', '').replace('_Server', '')} = {opcode},")

        h.closing_curly(" opcode;")

        h.newline()

        h.open_curly("union")

        first_opcode: typing.Union[None, str] = None
        for e in filter(matches, messages):
            ty = e.name.replace("_Client", "").replace("_Server", "")
            if first_opcode is None:
                first_opcode = ty

            h.wln(f"{module_name}::{e.name} {ty};")

        h.closing_curly(";")  # union
        h.newline()

        h.open_curly("bool is_none() const noexcept")
        h.wln("return opcode == Opcode::NONE;")
        h.closing_curly()

        h.newline()
        h.wln(f"explicit {side.capitalize()}Opcode() : opcode(Opcode::NONE), {first_opcode}() {{}}")
        h.newline()

        h.open_curly(f"{side.capitalize()}Opcode({side.capitalize()}Opcode&& other) noexcept")
        h.wln("this->opcode = other.opcode;")
        h.wln("other.opcode = Opcode::NONE;")
        for e in filter(matches, messages):
            ty = e.name.replace("_Client", "").replace("_Server", "")

            h.open_curly(f"if (opcode == Opcode::{ty})")
            h.wln(f"this->{ty} = std::move(other.{ty});")
            h.closing_curly()  # if (opcode == Opcode
        h.closing_curly()
        h.newline()

        # TODO FIX CONSTRUCTORS
        # h.wln(f"{side.capitalize()}Opcode(const {side.capitalize()}Opcode&) {{/* TODO */}}")
        # h.newline()

        h.open_curly(f"~{side.capitalize()}Opcode()")
        for e in filter(matches, messages):
            ty = e.name.replace("_Client", "").replace("_Server", "")

            h.open_curly(f"if (opcode == Opcode::{ty})")
            h.wln(f"this->{ty}.~{e.name}();")
            h.closing_curly()  # if (opcode == Opcode

        h.closing_curly()  # destructor
        h.newline()

        for e in filter(matches, messages):
            ty = e.name.replace("_Client", "").replace("_Server", "")

            h.open_curly(f"explicit {side.capitalize()}Opcode({module_name}::{e.name}&& obj)")
            h.wln(f"opcode = Opcode::{ty};")
            h.wln(f"new (&this->{ty}) {module_name}::{e.name} (std::move(obj));")
            h.closing_curly()  # constructor
        h.newline()

        h.wln("template<typename T>")
        h.wln("// NOLINTNEXTLINE")
        h.wln("T& get(); // All possible types have been specialized")
        h.newline()

        h.wln("template<typename T>")
        h.wln("// NOLINTNEXTLINE")
        h.wln("T* get_if(); // All possible types have been specialized")

        h.closing_curly(";")  # struct sideOpcode
        h.newline()

        for e in filter(matches, messages):
            ty = e.name.replace("_Client", "").replace("_Server", "")

            h.wln("template<>")
            h.wln(f"{module_name}::{e.name}* {side.capitalize()}Opcode::get_if();")

            s.wln("template <>")
            s.open_curly(f"{module_name}::{e.name}* {side.capitalize()}Opcode::get_if<{e.name}>()")
            s.open_curly(f"if (opcode == Opcode::{ty})")
            s.wln(f"return &{ty};")
            s.closing_curly() # if opcode ==
            s.wln("return nullptr;")
            s.closing_curly() # get_if()

            h.wln("template<>")
            h.wln(f"{module_name}::{e.name}& {side.capitalize()}Opcode::get();")
            s.wln("template <>")
            s.open_curly(f"{module_name}::{e.name}& {side.capitalize()}Opcode::get<{e.name}>()")
            s.wln(f"auto p = {side.capitalize()}Opcode::get_if<{module_name}::{e.name}>();")
            s.open_curly("if (p)")
            s.wln("return *p;")
            s.closing_curly() # if p
            s.wln("throw bad_opcode_access{};")

            s.closing_curly() # get()

            s.newline()
        s.newline()
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
            version = container_module_prefix(e.tags, module_name)
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
        export = get_export_define(messages[0].tags)
        function_declaration = f"{export} std::vector<unsigned char> write_opcode(const {side.capitalize()}Opcode& opcode)"
        h.wln(f"{function_declaration};")
        h.newline()

        s.open_curly(function_declaration)

        for e in messages:
            if not version_matches(e.tags, v) \
                    or side_matches(e, side) == INVALID_OPCODE \
                    or (first_version_as_module(e.tags) == "all" and version_to_module_name(v) != "all"):
                continue
            name = e.name.replace('_Client', '').replace('_Server', '')
            s.open_curly(f"if (opcode.opcode == {side.capitalize()}Opcode::Opcode::{name})")

            function_suffix: str = ""
            if type(e.object_type) is model.ObjectTypeMsg:
                first_char = side[0].lower()
                function_suffix = f"_{first_char}msg"
            s.wln(f"return opcode.{name}.write{function_suffix}();;")
            s.closing_curly()  # if

        s.newline()
        s.wln("return {}; /* unreachable */")

        s.closing_curly()  # function decl
        s.newline()


def write_opcode_read(s: Writer, h: Writer, messages: list[model.Container], v: typing.Union[int | model.WorldVersion],
                      side: str, module_name: str,
                      module_name_pascal: str, side_pascal: str, opcodes: dict[str, int]):
    export = get_export_define(messages[0].tags)
    result_type = get_type_prefix(messages[0].tags)
    size_field_size = 2 if side == "server" else 4

    if is_cpp():
        function_declaration = f"{export} {side.capitalize()}Opcode read_{side}_opcode(Reader& reader)"
        h.wln(f"{function_declaration};")
        h.newline()

        s.open_curly(function_declaration)

        opcode_type = None
        if is_world(messages[0].tags):
            if side == "server":
                s.wln("const uint16_t _size = reader.read_u16_be();")
                s.wln("const uint16_t opcode = reader.read_u16();")
                opcode_type = "uint16_t"
            else:
                s.wln("const uint16_t _size = reader.read_u16_be();")
                s.wln("const uint32_t opcode = reader.read_u32();")
                opcode_type = "uint32_t"
        else:
            opcode_type = "uint8_t"
            s.wln("const uint8_t opcode = reader.read_u8();")

        s.newline()
        s.wln(f"{side.capitalize()}Opcode op;")
        s.newline()

        for e in messages:
            if not version_matches(e.tags, v) \
                    or side_matches(e, side) == INVALID_OPCODE \
                    or (first_version_as_module(e.tags) == "all" and version_to_module_name(v) != "all"):
                continue
            name = e.name.replace('_Client', '').replace('_Server', '')
            s.open_curly(f"if (opcode == static_cast<{opcode_type}>({side.capitalize()}Opcode::Opcode::{name}))")
            module = library_type(e.tags)
            if not container_has_c_members(e):
                s.wln(f"return {side.capitalize()}Opcode(::wow_{module}_messages::{module_name}::{e.name}{{}});")
            else:
                body_size = f", _size - {size_field_size}" if container_needs_size_in_read(e) else ""

                module_prefix = container_module_prefix(e.tags, module_name)
                s.wln(
                    f"return {side.capitalize()}Opcode(::wow_{module}_messages::{module_prefix}::{e.name}_read(reader{body_size}));")

            s.closing_curly()  # if opcode == opcode

        s.newline()
        s.wln("return op;")

        s.closing_curly()  # function decl
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

            body_size = f", _size - {size_field_size}" if container_needs_size_in_read(e) else ""

            version = container_module_prefix(e.tags, module_name)
            s.wln(
                f"{wlm_prefix}_CHECK_RETURN_CODE({version}_{e.name}_read(reader, &opcodes->body.{e.name}{body_size}));")

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

        version = container_module_prefix(e.tags, module_name)
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
