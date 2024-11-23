import cProfile
import json
import os.path
import pstats
import typing

from util import LOGIN_VERSION_ALL, WORLD_VERSION_ALL, is_cpp, set_cpp
from print_struct.struct_util import container_should_have_size_function, integer_type_to_size, container_should_print

import model
from login_utils import print_login_utils
from print_achievement_done_array import print_achievement_done_array
from print_achievement_in_progress_array import print_achievement_in_progress_array
from print_addon_array import print_addon_array
from print_aura_mask import print_aura_mask
from print_cache_mask import print_cache_mask
from print_enum import print_enum
from print_named_guid import print_named_guid
from print_struct import print_struct, container_has_c_members, struct_util
from print_struct.print_tests import print_login_tests, print_world_tests, print_login_test_prefix, \
    print_login_test_suffix
from print_struct.struct_util import all_members_from_container
from print_update_mask import print_update_mask
from print_variable_item_random_property import print_variable_item_random_property
from util import (
    world_version_matches,
    should_print_container,
    world_version_to_module_name,
    login_version_to_module_name,
    first_login_version,
    VERSIONS, VANILLA, WRATH, TBC, world_version_is_wrath,
    world_version_is_tbc, world_version_is_vanilla, write_file_if_not_same, version_matches, world_version_is_all,
    version_to_module_name, is_world, first_version_as_module,
)
from writer import Writer

THIS_FILE_PATH = os.path.dirname(os.path.realpath(__file__))
LOGIN_C_PROJECT_DIR = f"{THIS_FILE_PATH}/../wow_login_messages_c"
LOGIN_C_SOURCE_DIR = f"{LOGIN_C_PROJECT_DIR}/src"
LOGIN_C_HEADER_DIR = f"{LOGIN_C_PROJECT_DIR}/include/wow_login_messages"

LOGIN_CPP_PROJECT_DIR = f"{THIS_FILE_PATH}/../wow_login_messages_cpp"
LOGIN_CPP_SOURCE_DIR = f"{LOGIN_CPP_PROJECT_DIR}/src"
LOGIN_CPP_HEADER_DIR = f"{LOGIN_CPP_PROJECT_DIR}/include/wow_login_messages_cpp"

WORLD_C_PROJECT_DIR = f"{THIS_FILE_PATH}/../wow_world_messages_c"
WORLD_C_SOURCE_DIR = f"{WORLD_C_PROJECT_DIR}/src"
WORLD_C_HEADER_DIR = f"{WORLD_C_PROJECT_DIR}/include/wow_world_messages"

WORLD_CPP_PROJECT_DIR = f"{THIS_FILE_PATH}/../wow_world_messages_cpp"
WORLD_CPP_SOURCE_DIR = f"{WORLD_CPP_PROJECT_DIR}/src"
WORLD_CPP_HEADER_DIR = f"{WORLD_CPP_PROJECT_DIR}/include/wow_world_messages_cpp"

IR_FILE_PATH = f"{THIS_FILE_PATH}/wow_messages/intermediate_representation.json"


def main():
    print("Generating python files")
    f = open(IR_FILE_PATH)
    data = json.load(f)
    m = model.IntermediateRepresentationSchema.from_json_data(data)
    m = sanitize_model(m)

    for cpp in [True, False]:
        set_cpp(cpp)

        tests = Writer()
        print_login_test_prefix(tests, m)

        s = Writer()
        print_login(m.login, s, tests, LOGIN_VERSION_ALL)
        for i, v in enumerate(m.distinct_login_versions_other_than_all):
            print_login(m.login, s, tests, v)

        source_dir = LOGIN_CPP_SOURCE_DIR if is_cpp() else LOGIN_C_SOURCE_DIR
        pp = "pp" if is_cpp() else ""

        file_path = f"{source_dir}/login.c{pp}"
        write_file_if_not_same(s, file_path)

        print_login_test_suffix(tests)

        file_path = f"{source_dir}/all.test.c{pp}"
        write_file_if_not_same(tests, file_path)

        print_world(m.world, m.vanilla_update_mask, WORLD_VERSION_ALL)
        for v in VERSIONS:
            update_mask = m.vanilla_update_mask if v == VANILLA else m.tbc_update_mask if v == TBC else m.wrath_update_mask if v == WRATH else None
            print_world(m.world, update_mask, v)
            break

    print("Finished generating files")


def sanitize_model(
        m: model.IntermediateRepresentationSchema,
) -> model.IntermediateRepresentationSchema:
    def containers(container: model.Container) -> model.Container:
        for d in all_members_from_container(e):
            if d.name == "class":
                d.name = "class_type"

        return container

    for e in m.world.structs:
        e = containers(e)

    for e in m.world.messages:
        e = containers(e)

    return m


def print_includes(s: Writer, h: Writer, world: typing.Optional[model.WorldVersion], version_name: str,
                   v: typing.Union[int | model.WorldVersion], m: model.Objects):
    include_dir = "wow_login_messages"
    if world is not None:
        include_dir = "wow_world_messages"

    include_file = include_dir
    if is_cpp():
        include_dir += "_cpp"

    pp = "pp" if is_cpp() else ""

    include_guard = f"{include_dir.upper()}_{version_name.upper()}_H{pp.upper()}"
    h.wln(f"#ifndef {include_guard}")
    h.wln(f"#define {include_guard}")
    h.newline()

    h.wln(f"#include \"{include_dir}/{include_file}.h{pp}\"")
    if world is not None and version_name != "all":
        h.wln(f"#include \"{include_dir}/all.h{pp}\"")
    h.newline()

    if not is_cpp():
        h.wln("#ifdef __cplusplus")
        h.wln('extern "C" {')
        h.wln("#endif /* __cplusplus */")

    if world is not None or (version_name == "all" and world is None):
        s.wln(f"#include \"util.h{pp}\"")
        s.newline()

    s.wln(f"#include \"{include_dir}/{version_name}.h{pp}\"")
    s.newline()

    if world is not None and version_name != "all":
        s.wln("#include <string.h> /* memset */")
        s.newline()

    if world is not None and not world_version_is_all(v):
        if is_cpp():
            s.open_curly("namespace wow_world_messages")
            s.open_curly("namespace all")
        for e in m.structs:
            if is_world(e.tags) and first_version_as_module(e.tags) == "all":
                s.wln("/* forward declare all struct read/write */")
                if is_cpp():
                    s.wln(f"::wow_world_messages::all::{e.name} {e.name}_read(Reader& reader);")
                    s.wln(f"void {e.name}_write(Writer& writer, const ::wow_world_messages::all::{e.name}& obj);")
                else:
                    s.wln(f"WowWorldResult all_{e.name}_read(WowWorldReader* reader, all_{e.name}* object);")
                    s.wln(f"WowWorldResult all_{e.name}_write(WowWorldWriter* writer, const all_{e.name}* object);")
                s.newline()
        if is_cpp():
            s.closing_curly(" // namespace all")
            s.closing_curly(" // namespace wow_world_messages")
        s.newline()

    if is_cpp():
        h.wln(f"namespace {include_file} {{")
        h.wln(f"namespace {version_name} {{")
        s.wln(f"namespace {include_file} {{")
        s.wln(f"namespace {version_name} {{")


def print_footer(s: Writer, h: Writer, world: typing.Optional[model.WorldVersion], version_name: str):
    include_dir = "wow_login_messages"
    if world is not None:
        include_dir = "wow_world_messages"

    pp = "pp" if is_cpp() else ""
    cpp = "_CPP" if is_cpp() else ""

    include_guard = f"{include_dir.upper()}{cpp}_{version_name.upper()}_H{pp.upper()}"

    if not is_cpp():
        h.wln("#ifdef __cplusplus")
        h.wln("}")
        h.wln("#endif /* __cplusplus */")

    if is_cpp():
        h.wln(f"}} // namespace {version_name}")
        h.wln(f"}} // namespace {include_dir}")
        s.wln(f"}} // namespace {version_name}")
        s.wln(f"}} // namespace {include_dir}")

    h.wln(f"#endif /* {include_guard} */")


def print_world(m: model.Objects, update_mask: list[model.UpdateMask], v: model.WorldVersion):
    def should_print(container_or_definer: typing.Union[model.Definer | model.Container]):
        if type(container_or_definer) is model.Definer:
            return version_matches(container_or_definer.tags, v)

        match container_or_definer.tags.version:
            case model.ObjectVersionsWorld(version_type=version_type):
                match version_type:
                    case model.WorldVersionsAll():
                        if not world_version_is_all(v):
                            return False

        return version_matches(container_or_definer.tags, v) and container_should_print(container_or_definer)

    s = Writer()
    h = Writer()

    module_name = world_version_to_module_name(v)

    print_includes(s, h, v, module_name, v, m)

    for d in filter(should_print, m.enums):
        print_enum(h, d)

    for d in filter(should_print, m.flags):
        print_enum(h, d)

    print_aura_mask(s, h, v)
    print_update_mask(s, h, update_mask, module_name)

    for e in filter(should_print, m.structs):
        print_struct(s, h, e, module_name)

    for e in filter(should_print, m.messages):
        print_struct(s, h, e, module_name)

    print_login_utils(s, h, list(filter(should_print, m.messages)), v)

    print_footer(s, h, v, module_name)

    header_dir = WORLD_CPP_HEADER_DIR if is_cpp() else WORLD_C_HEADER_DIR
    pp = "pp" if is_cpp() else ""
    file_path = f"{header_dir}/{module_name}.h{pp}"
    write_file_if_not_same(h, file_path)

    source_dir = WORLD_CPP_SOURCE_DIR if is_cpp() else WORLD_C_SOURCE_DIR
    file_path = f"{source_dir}/{module_name}.c{pp}"
    write_file_if_not_same(s, file_path)

    if module_name != "all":
        tests = Writer()
        print_world_tests(tests, filter(should_print, m.messages), v)

        file_path = f"{source_dir}/{module_name}.test.c{pp}"
        write_file_if_not_same(tests, file_path)

    print()
    for reason, amount in struct_util.SKIPS.items():
        print(f"{reason}: {amount}")


def print_login(m: model.Objects, s: Writer, tests: Writer, v: int):
    def should_print(container_or_definer: typing.Union[model.Definer | model.Container]):
        return version_matches(container_or_definer.tags, v)

    h = Writer()
    module_name = login_version_to_module_name(v)

    h_includes = Writer()
    print_includes(s, h_includes, None, module_name, v, m)

    def typedef_existing(s: Writer, name: str, old_version: str, new_version: str):
        if is_cpp():
            s.wln(
                f"typedef {old_version}::{name} {name};")
        else:
            s.wln(
                f"typedef {old_version}_{name} {new_version}_{name};")
        s.newline()

    type_includes = {}
    for d in filter(should_print, m.enums):
        version = first_login_version(d.tags)
        if version != v:
            type_includes[version] = {}
            typedef_existing(h, d.name, login_version_to_module_name(version), module_name)
            continue

        print_enum(h, d)

    for d in filter(should_print, m.flags):
        if not version_matches(d.tags, v):
            continue

        version = first_login_version(d.tags)
        if version != v:
            type_includes[version] = {}
            typedef_existing(h, d.name, login_version_to_module_name(version), module_name)
            continue

        print_enum(h, d)

    for e in filter(should_print, m.structs):
        version = first_login_version(e.tags)
        if version != v:
            type_includes[version] = {}
            if container_has_c_members(e):
                typedef_existing(h, e.name, login_version_to_module_name(version), module_name)
            continue

        print_struct(s, h, e, module_name)
        pass

    for e in filter(should_print, m.messages):
        version = first_login_version(e.tags)
        if version != v:
            type_includes[version] = {}
            if container_has_c_members(e):
                typedef_existing(h, e.name, login_version_to_module_name(version), module_name)
                continue

        print_struct(s, h, e, module_name)

    if not is_cpp():
        for inc in type_includes:
            name = login_version_to_module_name(inc)
            h_includes.wln(f"#include \"wow_login_messages/{name}.h\" /* type include */")

    h_includes.newline()
    h.prepend(h_includes)

    print_login_utils(s, h, m.messages, v)

    print_footer(s, h, world=None, version_name=module_name)

    header_dir = LOGIN_CPP_HEADER_DIR if is_cpp() else LOGIN_C_HEADER_DIR
    pp = "pp" if is_cpp() else ""
    file_path = f"{header_dir}/{module_name}.h{pp}"
    write_file_if_not_same(h, file_path)

    print_login_tests(tests, m, v)


if __name__ == "__main__":
    # cProfile.run("main()", sort=pstats.SortKey.CUMULATIVE)
    main()
