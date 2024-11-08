import sys

from struct_util import integer_type_to_size

import model
from generator.util import pascal_case_to_snake_case
from model import Definer
from util import first_version_as_module
from writer import Writer


def value_to_integer(val: str, integer_type: model.IntegerType) -> str:
    value = int(val)
    if value > 2147483647:
        value = value.to_bytes(integer_type_to_size(integer_type), byteorder=sys.byteorder, signed=False)
        return str(int.from_bytes(value, byteorder=sys.byteorder, signed=True))
    else:
        return val


def print_enum(h: Writer, enum: Definer):
    h.open_curly("typedef enum")

    enum_name_upper = pascal_case_to_snake_case(enum.name).upper()

    first_version = first_version_as_module(enum.tags).upper()

    for enumerator in enum.enumerators:
        value = value_to_integer(enumerator.value.value, enum.integer_type)
        h.wln(f"{first_version}_{enum_name_upper}_{enumerator.name.upper()} = {value},")

    first_module = first_version_as_module(enum.tags)
    h.closing_curly(f" {first_module}_{enum.name};")
    h.newline()
