import sys

from print_struct.struct_util import integer_type_to_size, integer_type_to_c_str

import model
from util import pascal_case_to_snake_case, is_cpp
from model import Definer
from util import first_version_as_module
from writer import Writer


def value_to_integer(val: str, integer_type: model.IntegerType) -> str:
    value = int(val)
    if integer_type_to_size(integer_type) <= 4 and value > 2147483647:
        value = value.to_bytes(integer_type_to_size(integer_type), byteorder=sys.byteorder, signed=False)
        value = str(int.from_bytes(value, byteorder=sys.byteorder, signed=True))
        if value == "-2147483648":
            return "-2147483647 - 1"  # Work around MSVC parsing it as unary negate operator, then as unsigned integer
        else:
            return value

    else:
        return val


def print_enum(h: Writer, enum: Definer):
    first_version = first_version_as_module(enum.tags).upper()
    enum_name_upper = pascal_case_to_snake_case(enum.name).upper()
    first_module = first_version_as_module(enum.tags)

    if is_cpp():
        h.open_curly(f"enum class {enum.name} : {integer_type_to_c_str(enum.integer_type)}")

        for i, enumerator in enumerate(enum.enumerators):
            h.wln(f"{enumerator.name.upper()} = {enumerator.value.value},")


        h.closing_curly(";") # enum class
    else:
        if integer_type_to_size(enum.integer_type) > 4:
            h.wln("/* standard C only allows enums to be ints. We need larger than that */")
            h.wln(f"typedef uint64_t {first_module}_{enum.name};")

            for enumerator in enum.enumerators:
                h.wln(f"#define {first_version}_{enum_name_upper}_{enumerator.name.upper()} {enumerator.value.value}")

        else:
            h.open_curly("typedef enum")

            for i, enumerator in enumerate(enum.enumerators):
                value = value_to_integer(enumerator.value.value, enum.integer_type)
                comma = "," if i < (len(enum.enumerators) - 1) else ""
                h.wln(f"{first_version}_{enum_name_upper}_{enumerator.name.upper()} = {value}{comma}")

            h.closing_curly(f" {first_module}_{enum.name};")

    h.newline()
