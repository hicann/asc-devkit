#!/usr/bin/python
# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------
"""
ascendc compile backend kernel type
"""

import copy
import hashlib
import os

from asc_op_compile_base.common.platform.platform_info import set_soc_spec
from asc_op_compile_base.common.utils.log_utils import CompileStage
from asc_op_compile_base.asc_op_compiler.cce_runtime import tvm_callback_cce_postproc
from .get_op_tiling import TilingInfo
from .global_storage import global_var_storage
from .ascendc_constants import (
    KernelMetaType,
    CORE_TYPE_MIX,
    CORE_TYPE_CUBE,
    CORE_TYPE_VEC,
    TILING_KEY_MACRO,
)
from .ascendc_common_utility import (
    CommonUtility,
    CompileInfo,
    KernelCompileCommand,
    get_kernel_fun_name_with_tiling_key_and_kernel_type,
)
from .ascendc_compile_dfx import DFXSectionGenerator
from .ascendc_compile_base import (
    compile_multi_tilingkey,
    fatbin_objs,
    SingleTilingKeyCompileParams,
)
from .ascendc_compile_gen_code import _gen_compile_cmd
from .ascendc_compile_gen_json import (
    _dynamic_kernel_list_to_json,
    _generate_final_json,
    _gen_dynamic_json_for_v200,
    _gen_mix_json_from_seperate_json,
    _gen_mix_json_from_seperate_json_for_kernel_type,
    _gen_mix_sub_json,
    _gen_non_mix_sub_json,
    _gen_static_json_for_mix_v200,
    _gen_static_json_for_no_mix_v200,
)
from .ascendc_compile_v220 import (
    call_bisheng_v220,
    gen_compile_cmd_v220,
    get_compile_core_types,
    get_compile_target_name,
    get_compile_target_options,
    get_ktype_section_variable,
    set_dynamic_sub_func_names_of_super_kernel_with_kernel_type_group,
)
from .ascendc_compile_v200 import call_bisheng_v200_static, call_bisheng_v200_dynamic
from .super_kernel_sub_op_compile import gen_sub_kernel_name


def _compile_single_tiling(tiling_key, compile_info, tiling_info, compile_option_tuple):
    dst_file = compile_info.dst_file[:-2] + "_%s.o" % tiling_key
    compile_cmd = _gen_compile_cmd(
        compile_info.gen_kernel_func_file,
        dst_file,
        compile_option_tuple,
        tiling_info.tiling_data_file_path,
    )
    compile_cmd += [f"-D{TILING_KEY_MACRO}={tiling_key}UL"]
    if global_var_storage.get_variable("ascendc_enable_super_kernel") is True:
        tiling_data_hash_src = tiling_info.tiling_data
        if isinstance(tiling_data_hash_src, str):
            tiling_data_hash_src = tiling_data_hash_src.encode("utf-8")
        elif not tiling_data_hash_src:
            tiling_data_hash_src = tiling_info.file_content.encode("utf-8")
        tiling_data_hash = hashlib.sha256(tiling_data_hash_src).hexdigest()[:8]
        compile_cmd += [
            f"-D{compile_info.origin_func_name}="
            f"{compile_info.origin_func_name}_{tiling_data_hash}_{tiling_key}_tilingkey"
        ]
    else:
        compile_cmd += [
            f"-D{compile_info.origin_func_name}={compile_info.origin_func_name}_{tiling_key}_tilingkey"
        ]
    kernel_func_name = compile_info.kernel_name + "_%s" % tiling_key
    compile_cmd += [
        f"-Dauto_gen_{compile_info.origin_func_name}_kernel={kernel_func_name}"
    ]
    section_content = DFXSectionGenerator().generate_dfx_section(
        tiling_key, tiling_info, kernel_func_name, compile_info, True
    )
    return compile_cmd, section_content


def _compile_ascendc_cce(
    compile_info: CompileInfo, compile_option_tuple, tiling_info: TilingInfo
):
    """call cce-c to compile a AscendC.cce file, generate a binary file and a json file

    Args:
        compile_info (CompileInfo): compile info for generate .o and .json
        compile_options (list): compile options for bisheng
        tiling_info (TilingInfo): tiling info
    """
    sources = CommonUtility().ascendc_read_file(compile_info.gen_kernel_func_file)

    new_sources = sources[:-1]
    if tiling_info.static_shape_flag:
        compile_cmd = _gen_compile_cmd(
            compile_info.gen_kernel_func_file,
            compile_info.dst_file,
            compile_option_tuple,
            tiling_info.tiling_data_file_path,
        )
        # tbe-pass add "__kernel0" in tbe-codegen and json, we use -D to change function name
        compile_cmd += [
            f"-Dauto_gen_{compile_info.origin_func_name}_kernel={compile_info.get_kernel_func_name()}"
        ]
        if global_var_storage.get_variable("ascendc_enable_super_kernel") is True:
            tiling_data_hash_src = tiling_info.tiling_data
            if isinstance(tiling_data_hash_src, str):
                tiling_data_hash_src = tiling_data_hash_src.encode("utf-8")
            elif not tiling_data_hash_src:
                tiling_data_hash_src = tiling_info.file_content.encode("utf-8")
            tiling_data_hash = hashlib.sha256(tiling_data_hash_src).hexdigest()[:8]
            compile_cmd += [
                f"-D{compile_info.origin_func_name}="
                f"{compile_info.origin_func_name}_{tiling_data_hash}_{tiling_info.tiling_key}_tilingkey"
            ]
        compile_cmd += [f"-D{TILING_KEY_MACRO}={tiling_info.tiling_key}UL"]
        new_sources += DFXSectionGenerator().generate_dfx_section(
            str(tiling_info.tiling_key),
            tiling_info,
            compile_info.get_kernel_func_name(),
            compile_info,
            True,
        )
        new_sources += "#endif\n"
        # add dfx info section to sourse file
        CommonUtility().ascendc_write_file(
            compile_info.gen_kernel_func_file, new_sources
        )

        CommonUtility.run_cmd_inner(
            compile_cmd, CompileStage.COMPILE, compile_info.compile_log_path
        )
        target = "cce_core"
        tvm_callback_cce_postproc(
            target, compile_info.kernel_name, tiling_info.block_num
        )
    else:
        obj_files = []
        for tiling_key in compile_info.tiling_key_list:
            dst_file = compile_info.dst_file[:-2] + "_%s.o" % tiling_key
            obj_files.append(dst_file)
        cmds_list = []
        for tiling_key in compile_info.tiling_key_list:
            compile_cmd, section_content = _compile_single_tiling(
                tiling_key, compile_info, tiling_info, compile_option_tuple
            )
            cmds_list.append(compile_cmd)
            new_sources += section_content
        new_sources += "#endif\n"
        # add dfx info section to sourse file
        CommonUtility().ascendc_write_file(
            compile_info.gen_kernel_func_file, new_sources
        )
        # compile binary
        compile_multi_tilingkey(
            compile_info.tiling_key_list,
            cmds_list,
            os.path.basename(compile_info.dst_file)[:-2],
            compile_info.compile_log_path,
        )
        fatbin_objs(
            obj_files,
            compile_info.dst_file,
            compile_info.is_debug,
            compile_info.compile_log_path,
        )
        target = "cce_core"
        tvm_callback_cce_postproc(
            target, compile_info.kernel_name, tiling_info.block_num
        )
        _dynamic_kernel_list_to_json(
            compile_info.kernel_name,
            compile_info.tiling_key_list,
            compile_info.enable_deterministic,
            compile_info.tiling_key_deterministic,
        )


def _get_sub_kernel_name(compile_info: CompileInfo, core_type: int):
    core_type_marker = "_mix_aic" if core_type == CORE_TYPE_CUBE else "_mix_aiv"
    # i.e. change demo_kernel.o to demo_kernel_mix_aic.o
    sub_kernel_name = compile_info.kernel_name + core_type_marker
    return sub_kernel_name


def _generate_section_content(
    kernel_name: str,
    tiling_key: str,
    kernel_type: KernelMetaType,
    tiling_info: TilingInfo,
    compile_info: CompileInfo,
):
    if global_var_storage.get_variable("ascendc_enable_super_kernel") is True:
        return ""
    section_content = ""
    section_content += f"\n#if {TILING_KEY_MACRO} == {tiling_key}UL\n"
    section_content += get_ktype_section_variable(
        f"{kernel_name}_section", f"{kernel_name}", kernel_type
    )
    if compile_info.tiling_key_group_map is not None:
        if tiling_key in compile_info.tiling_key_group_map.keys():
            for tiling_key_slave in compile_info.tiling_key_group_map[tiling_key]:
                kernel_name_slave = get_kernel_fun_name_with_tiling_key_and_kernel_type(
                    compile_info, tiling_key_slave
                )
                if (
                    compile_info.tiling_key_kernel_type.get(tiling_key_slave)
                    is not None
                ):
                    kernel_type_slave = compile_info.tiling_key_kernel_type.get(
                        str(tiling_key_slave)
                    )
                else:
                    raise Exception(
                        f"the kernel type of tiling key {tiling_key_slave} is None"
                    )
                section_content += get_ktype_section_variable(
                    f"{kernel_name_slave}_section",
                    f"{kernel_name_slave}",
                    kernel_type_slave,
                )
    section_content += "#endif\n"
    section_content += DFXSectionGenerator().generate_dfx_section(
        tiling_key, tiling_info, kernel_name, compile_info
    )
    return section_content


def _get_compile_cmd_and_section_content(
    compile_info: CompileInfo,
    arch: str,
    compile_option_tuple,
    tiling_info: TilingInfo,
    tiling_key: str,
):
    compile_cmd = gen_compile_cmd_v220(
        compile_info.gen_kernel_func_file,
        compile_info.dst_file,
        compile_option_tuple,
        arch,
        tiling_info.tiling_data_file_path,
    )

    current_kernel_name = ""
    kernel_type = compile_info.tiling_key_kernel_type[str(tiling_key)]
    if tiling_info.static_shape_flag:
        if kernel_type.value >= 2:
            current_kernel_name = compile_info.kernel_name
            current_kernel_name = gen_sub_kernel_name(
                current_kernel_name, arch, kernel_type.name, compile_info.dst_file
            )
            compile_cmd += [
                f"-Dauto_gen_{compile_info.origin_func_name}_kernel={current_kernel_name}"
            ]
        else:
            current_kernel_name = compile_info.get_kernel_func_name()
            current_kernel_name = gen_sub_kernel_name(
                current_kernel_name, "AiCore", kernel_type.name, compile_info.dst_file
            )
            compile_cmd += [
                f"-Dauto_gen_{compile_info.origin_func_name}_kernel={current_kernel_name}"
            ]
    else:
        core_type = "cube" if arch.endswith("-cube") else "vec"
        current_kernel_name = get_compile_target_name(
            compile_info, tiling_key, core_type
        )
        compile_cmd += [
            f"-Dauto_gen_{compile_info.origin_func_name}_kernel={current_kernel_name}"
        ]
        if kernel_type.value >= 2:
            set_dynamic_sub_func_names_of_super_kernel_with_kernel_type_group(
                tiling_key, arch, kernel_type.name, current_kernel_name, compile_info
            )
        else:
            set_dynamic_sub_func_names_of_super_kernel_with_kernel_type_group(
                tiling_key,
                "AiCore",
                kernel_type.name,
                current_kernel_name,
                compile_info,
            )
    compile_cmd.extend(
        get_compile_target_options(compile_info, tiling_key, CommonUtility.is_c310())
    )
    compile_cmd += [f"-D{TILING_KEY_MACRO}={tiling_key}UL"]
    if global_var_storage.get_variable("ascendc_enable_super_kernel") is True:
        tiling_data_hash_src = tiling_info.tiling_data
        if isinstance(tiling_data_hash_src, str):
            tiling_data_hash_src = tiling_data_hash_src.encode("utf-8")
        elif not tiling_data_hash_src:
            tiling_data_hash_src = tiling_info.file_content.encode("utf-8")
        tiling_data_hash = hashlib.sha256(tiling_data_hash_src).hexdigest()[:8]
        compile_cmd += [
            f"-D{compile_info.origin_func_name}="
            f"{compile_info.origin_func_name}_{tiling_data_hash}_{tiling_key}_tilingkey"
        ]
    else:
        compile_cmd += [
            f"-D{compile_info.origin_func_name}={compile_info.origin_func_name}_{tiling_key}_tilingkey"
        ]
    section_content = _generate_section_content(
        current_kernel_name, tiling_key, kernel_type, tiling_info, compile_info
    )
    if global_var_storage.get_variable("ascendc_sk_double_compile") is True:
        compile_info.global_kernel_symbols.append(current_kernel_name)
    compile_info.last_compiled_symbol = current_kernel_name
    return compile_cmd, section_content


def _is_mix_aic_1x_kernel_type(kernel_type):
    return kernel_type in [
        KernelMetaType.KERNEL_TYPE_MIX_AIC_1_1,
        KernelMetaType.KERNEL_TYPE_MIX_AIC_1_2,
    ]


def _is_hard_sync_or_mix_1x_kernel_type(kernel_type):
    return kernel_type in [
        KernelMetaType.KERNEL_TYPE_MIX_AIV_HARD_SYNC,
        KernelMetaType.KERNEL_TYPE_MIX_AIC_HARD_SYNC,
        KernelMetaType.KERNEL_TYPE_MIX_AIV_1_0,
        KernelMetaType.KERNEL_TYPE_MIX_AIC_1_0,
    ]


def _is_single_core_kernel_type(kernel_type):
    return kernel_type in [
        KernelMetaType.KERNEL_TYPE_AIV_ONLY,
        KernelMetaType.KERNEL_TYPE_AIC_ONLY,
    ]


def _get_arch_and_code_type(kernel_type, chip_version):
    if kernel_type in [
        KernelMetaType.KERNEL_TYPE_MIX_AIC_HARD_SYNC,
        KernelMetaType.KERNEL_TYPE_MIX_AIC_1_0,
    ]:
        arch = f"dav-{chip_version}-cube"
        code_type = CORE_TYPE_CUBE
    else:
        arch = f"dav-{chip_version}-vec"
        code_type = CORE_TYPE_VEC
    return arch, code_type


def _compile_core(
    compile_info, arch, code_type, compile_option_tuple, tiling_info, tiling_key
):
    sub_compile_info = _get_sub_compile_info(compile_info, code_type)
    compile_cmd, section_content = _get_compile_cmd_and_section_content(
        sub_compile_info, arch, compile_option_tuple, tiling_info, tiling_key
    )
    if global_var_storage.get_variable("ascendc_sk_double_compile") is True:
        compile_info.global_kernel_symbols.extend(
            sub_compile_info.global_kernel_symbols
        )
    return sub_compile_info, compile_cmd, section_content


def _finalize_and_write_sources(new_sources):
    new_sources += global_var_storage.get_variable("ascendc_meta_info")
    new_sources += "#endif\n"
    return new_sources


def _handle_mix_objs(compile_info, mix_objs, dst_file):
    if compile_info.enable_final_super_kernel_compile is True:
        compile_info.super_kernel_objs = mix_objs
    else:
        fatbin_objs(
            mix_objs, dst_file, compile_info.is_debug, compile_info.compile_log_path
        )


def _compile_mix_aic_1x_kernel(
    compile_info, chip_version, compile_option_tuple, tiling_info, new_sources
):
    kernel_type = compile_info.tiling_key_kernel_type[str(tiling_info.tiling_key)]
    cmds_list = []
    dst_file = compile_info.dst_file

    cube_compile_info, cube_compile_cmd, cube_section = _compile_core(
        compile_info,
        f"dav-{chip_version}-cube",
        CORE_TYPE_CUBE,
        compile_option_tuple,
        tiling_info,
        tiling_info.tiling_key,
    )
    new_sources += cube_section
    cmds_list.append(cube_compile_cmd)

    vec_compile_info, vec_compile_cmd, vec_section = _compile_core(
        compile_info,
        f"dav-{chip_version}-vec",
        CORE_TYPE_VEC,
        compile_option_tuple,
        tiling_info,
        tiling_info.tiling_key,
    )
    new_sources += vec_section
    cmds_list.append(vec_compile_cmd)

    new_sources = _finalize_and_write_sources(new_sources)
    CommonUtility().ascendc_write_file(compile_info.gen_kernel_func_file, new_sources)

    for cmd in cmds_list:
        CommonUtility.run_cmd_inner(
            cmd, CompileStage.COMPILE, compile_info.compile_log_path
        )

    mix_objs = [cube_compile_info.dst_file, vec_compile_info.dst_file]
    _handle_mix_objs(compile_info, mix_objs, dst_file)

    _gen_mix_sub_json(compile_info, tiling_info, CORE_TYPE_CUBE)
    if kernel_type.value == 6:
        tiling_info.task_ration = 1
    task_ration_str = f"1:{tiling_info.task_ration}"
    _gen_mix_json_from_seperate_json_for_kernel_type(
        compile_info.kernel_name, task_ration_str, CORE_TYPE_CUBE, True
    )
    set_soc_spec("AiCore")


def _compile_hard_sync_or_mix_1x_kernel(
    compile_info, chip_version, compile_option_tuple, tiling_info, new_sources
):
    kernel_type = compile_info.tiling_key_kernel_type[str(tiling_info.tiling_key)]
    arch, code_type = _get_arch_and_code_type(kernel_type, chip_version)

    sub_compile_info, compile_cmd, section_content = _compile_core(
        compile_info,
        arch,
        code_type,
        compile_option_tuple,
        tiling_info,
        tiling_info.tiling_key,
    )
    new_sources += section_content
    new_sources = _finalize_and_write_sources(new_sources)
    CommonUtility().ascendc_write_file(compile_info.gen_kernel_func_file, new_sources)

    CommonUtility.run_cmd_inner(
        compile_cmd, CompileStage.COMPILE, compile_info.compile_log_path
    )

    _gen_mix_sub_json(sub_compile_info, tiling_info, code_type)
    mix_objs = [sub_compile_info.dst_file]
    _handle_mix_objs(compile_info, mix_objs, compile_info.dst_file)

    task_ration_str = "1:0" if code_type == CORE_TYPE_CUBE else "0:1"
    _gen_mix_json_from_seperate_json(
        compile_info.kernel_name, task_ration_str, code_type, True
    )
    set_soc_spec("AiCore")


def _compile_single_core_kernel(
    compile_info, chip_version, compile_option_tuple, tiling_info, new_sources
):
    kernel_type = compile_info.tiling_key_kernel_type[str(tiling_info.tiling_key)]
    arch = (
        f"dav-{chip_version}-cube"
        if kernel_type == KernelMetaType.KERNEL_TYPE_AIC_ONLY
        else f"dav-{chip_version}-vec"
    )
    sub_code_type = (
        "AIC" if kernel_type == KernelMetaType.KERNEL_TYPE_AIC_ONLY else "AIV"
    )
    optional_core = (
        "AiCore" if kernel_type == KernelMetaType.KERNEL_TYPE_AIC_ONLY else "VectorCore"
    )
    set_soc_spec(optional_core)

    compile_cmd, section_content = _get_compile_cmd_and_section_content(
        compile_info, arch, compile_option_tuple, tiling_info, tiling_info.tiling_key
    )
    new_sources += section_content
    new_sources = _finalize_and_write_sources(new_sources)
    CommonUtility().ascendc_write_file(compile_info.gen_kernel_func_file, new_sources)

    CommonUtility.run_cmd_inner(
        compile_cmd, CompileStage.COMPILE, compile_info.compile_log_path
    )
    _gen_non_mix_sub_json(compile_info, tiling_info, sub_code_type)


def _compile_ascendc_cce_v220_with_kernel_type_for_static(
    compile_info: CompileInfo, compile_option_tuple, tiling_info: TilingInfo
):
    """call cce-c to compile a AscendC.cce file, generate a binary file and a json file
       for staic shape
    Args:
        compile_info (CompileInfo): compile info for generate .o and .json
        compile_options (list): compile options for bisheng
        tiling_info (TilingInfo): tiling info
    """
    sources = CommonUtility().ascendc_read_file(compile_info.gen_kernel_func_file)
    chip_version = CommonUtility.get_chip_version()
    new_sources = sources[:-1]
    kernel_type = compile_info.tiling_key_kernel_type[str(tiling_info.tiling_key)]

    if _is_mix_aic_1x_kernel_type(kernel_type):
        _compile_mix_aic_1x_kernel(
            compile_info, chip_version, compile_option_tuple, tiling_info, new_sources
        )
    elif _is_hard_sync_or_mix_1x_kernel_type(kernel_type):
        _compile_hard_sync_or_mix_1x_kernel(
            compile_info, chip_version, compile_option_tuple, tiling_info, new_sources
        )
    elif _is_single_core_kernel_type(kernel_type):
        _compile_single_core_kernel(
            compile_info, chip_version, compile_option_tuple, tiling_info, new_sources
        )


def _compile_ascendc_cce_v220_with_kernel_type_for_dynamic(
    compile_info: CompileInfo, compile_option_tuple, tiling_info: TilingInfo
):
    """call cce-c to compile a AscendC.cce file, generate a binary file and a json file
       for dynamic shape
    Args:
        compile_info (CompileInfo): compile info for generate .o and .json
        compile_options (list): compile options for bisheng
        tiling_info (TilingInfo): tiling info
    """
    sources = CommonUtility().ascendc_read_file(compile_info.gen_kernel_func_file)
    chip_version = CommonUtility.get_chip_version()
    new_sources = sources[:-1]
    obj_files = []
    cmds_list_vec = []
    tiling_key_vec = []
    cmds_list_cube = []
    tiling_key_cube = []
    for tiling_key in compile_info.tiling_key_list:
        kernel_type = compile_info.tiling_key_kernel_type[tiling_key]
        for core_type in get_compile_core_types(compile_info, tiling_key):
            code_type = CORE_TYPE_CUBE if core_type == "cube" else CORE_TYPE_VEC
            sub_compile_info = (
                _get_sub_compile_info(compile_info, code_type)
                if kernel_type.value >= 2
                else copy.deepcopy(compile_info)
            )
            sub_compile_info.dst_file = (
                sub_compile_info.dst_file[:-2] + "_%s.o" % tiling_key
            )
            arch = f"dav-{chip_version}-{core_type}"
            compile_cmd, section_content = _get_compile_cmd_and_section_content(
                sub_compile_info, arch, compile_option_tuple, tiling_info, tiling_key
            )
            # Keep one command per physical core target for later Manifest replay.
            compile_info.compile_command_session.submit(
                KernelCompileCommand(
                    tiling_key=str(tiling_key),
                    compiled_symbol=sub_compile_info.last_compiled_symbol,
                    core_type=core_type,
                    source_path=sub_compile_info.gen_kernel_func_file,
                    argv=tuple(compile_cmd),
                    output_path=sub_compile_info.dst_file,
                )
            )
            if global_var_storage.get_variable("ascendc_sk_double_compile") is True:
                compile_info.global_kernel_symbols.extend(
                    sub_compile_info.global_kernel_symbols
                )
            new_sources += section_content
            obj_files.append(sub_compile_info.dst_file)
            if code_type == CORE_TYPE_CUBE:
                cmds_list_cube.append(compile_cmd)
                tiling_key_cube.append(tiling_key)
            else:
                cmds_list_vec.append(compile_cmd)
                tiling_key_vec.append(tiling_key)
    new_sources += global_var_storage.get_variable("ascendc_meta_info")
    new_sources += "#endif\n"
    # add dfx info section to sourse file
    CommonUtility().ascendc_write_file(compile_info.gen_kernel_func_file, new_sources)

    # The rewritten wrapper is retained as a resource; no object exists in replay.
    if not compile_info.compile_command_session.should_execute:
        return

    if len(cmds_list_vec) != 0:
        compile_multi_tilingkey(
            tiling_key_vec,
            cmds_list_vec,
            os.path.basename(compile_info.dst_file)[:-2] + "_tmp_aiv",
            compile_info.compile_log_path,
        )

    if len(cmds_list_cube) != 0:
        compile_multi_tilingkey(
            tiling_key_cube,
            cmds_list_cube,
            os.path.basename(compile_info.dst_file)[:-2] + "_tmp_aic",
            compile_info.compile_log_path,
        )
    fatbin_objs(
        obj_files,
        compile_info.dst_file,
        compile_info.is_debug,
        compile_info.compile_log_path,
    )
    _generate_final_json(compile_info, tiling_info)


def _compile_ascendc_cce_v220_with_kernel_type(
    compile_info: CompileInfo, compile_option_tuple, tiling_info: TilingInfo
):
    """call cce-c to compile a AscendC.cce file, generate a binary file and a json file with kernel type

    Args:
        compile_info (CompileInfo): compile info for generate .o and .json
        compile_options (list): compile options for bisheng
        tiling_info (TilingInfo): tiling info
    """
    if tiling_info.static_shape_flag:
        _compile_ascendc_cce_v220_with_kernel_type_for_static(
            compile_info, compile_option_tuple, tiling_info
        )
    else:
        _compile_ascendc_cce_v220_with_kernel_type_for_dynamic(
            compile_info, compile_option_tuple, tiling_info
        )


def _compile_ascendc_cce_v200_with_kernel_type_for_static(
    compile_info: CompileInfo, compile_option_tuple, tiling_info: TilingInfo
):
    """call cce-c to compile a AscendC.cce file, generate a binary file and a json file
       for staic shape
    Args:
        compile_info (CompileInfo): compile info for generate .o and .json
        compile_options (list): compile options for bisheng
        tiling_info (TilingInfo): tiling info
    """
    kernel_type = compile_info.tiling_key_kernel_type[str(tiling_info.tiling_key)]
    if kernel_type in [
        KernelMetaType.KERNEL_TYPE_MIX_AICORE,
        KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE,
    ]:
        # build Aicore
        set_soc_spec("AiCore")
        dst_file = compile_info.dst_file
        aicore_compile_info = _get_sub_compile_info(compile_info, CORE_TYPE_CUBE)
        arch = "dav-m200"
        call_bisheng_v200_static(
            aicore_compile_info, compile_option_tuple, tiling_info, arch, kernel_type
        )
        # build vector
        set_soc_spec("VectorCore")
        vec_compile_info = _get_sub_compile_info(compile_info, CORE_TYPE_VEC)
        arch = "dav-m200-vec"
        if kernel_type is KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE:
            compile_option_tuple.compile_options.append("-D__ENABLE_VECTOR_CORE__")
        call_bisheng_v200_static(
            vec_compile_info, compile_option_tuple, tiling_info, arch, kernel_type
        )
        # fatbin 2o->1o
        mix_objs = [aicore_compile_info.dst_file, vec_compile_info.dst_file]
        fatbin_objs(
            mix_objs, dst_file, compile_info.is_debug, compile_info.compile_log_path
        )
        # gen main json
        _gen_static_json_for_mix_v200(compile_info, tiling_info, kernel_type)
    elif kernel_type in [KernelMetaType.KERNEL_TYPE_AICORE]:
        arch = "dav-m200"
        set_soc_spec("AiCore")
        call_bisheng_v200_static(
            compile_info, compile_option_tuple, tiling_info, arch, kernel_type
        )
        # gen json for v200
        _gen_static_json_for_no_mix_v200(compile_info, tiling_info, kernel_type)
    else:
        raise Exception("current kernel core type is not support")
    return


def _compile_ascendc_cce_v200_with_kernel_type_for_dynamic(
    compile_info: CompileInfo,
    compile_option_tuple,
    tiling_info: TilingInfo,
    final_kernel_type,
):
    """call cce-c to compile a AscendC.cce file, generate a binary file and a json file
       for dynamic shape
    Args:
        compile_info (CompileInfo): compile info for generate .o and .json
        compile_options (list): compile options for bisheng
        tiling_info (TilingInfo): tiling info
    """
    obj_files = []
    cmds_list_vec = []
    tiling_key_vec = []
    cmds_list_aicore = []
    tiling_key_aicore = []
    sources = CommonUtility().ascendc_read_file(compile_info.gen_kernel_func_file)

    new_sources = sources[:-1]
    for tiling_key in compile_info.tiling_key_list:
        kernel_type = compile_info.tiling_key_kernel_type[tiling_key]
        if kernel_type in [
            KernelMetaType.KERNEL_TYPE_MIX_AICORE,
            KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE,
        ]:
            # build Aicore
            set_soc_spec("AiCore")
            dst_file = compile_info.dst_file
            aicore_compile_info = _get_sub_compile_info(compile_info, CORE_TYPE_CUBE)
            arch = "dav-m200"
            param = SingleTilingKeyCompileParams(
                tiling_key,
                aicore_compile_info,
                arch,
                tiling_info,
                compile_info.code_channel,
                compile_option_tuple,
            )
            dst_file, compile_cmd, section_content = call_bisheng_v200_dynamic(
                param, kernel_type
            )
            new_sources += section_content
            cmds_list_aicore.append(compile_cmd)
            obj_files.append(dst_file)
            tiling_key_aicore.append(tiling_key)
            # build vector
            set_soc_spec("VectorCore")
            vec_compile_info = _get_sub_compile_info(compile_info, CORE_TYPE_VEC)
            arch = "dav-m200-vec"
            if kernel_type is KernelMetaType.KERNEL_TYPE_MIX_VECTOR_CORE:
                compile_option_tuple.compile_options.append("-D__ENABLE_VECTOR_CORE__")
            param = SingleTilingKeyCompileParams(
                tiling_key,
                vec_compile_info,
                arch,
                tiling_info,
                compile_info.code_channel,
                compile_option_tuple,
            )
            dst_file, compile_cmd, section_content = call_bisheng_v200_dynamic(
                param, kernel_type
            )
            new_sources += section_content
            cmds_list_vec.append(compile_cmd)
            obj_files.append(dst_file)
            tiling_key_vec.append(tiling_key)
        elif kernel_type in [KernelMetaType.KERNEL_TYPE_AICORE]:
            arch = "dav-m200"
            set_soc_spec("AiCore")
            param = SingleTilingKeyCompileParams(
                tiling_key,
                compile_info,
                arch,
                tiling_info,
                compile_info.code_channel,
                compile_option_tuple,
            )
            dst_file, compile_cmd, section_content = call_bisheng_v200_dynamic(
                param, kernel_type
            )
            new_sources += section_content
            cmds_list_aicore.append(compile_cmd)
            obj_files.append(dst_file)
            tiling_key_aicore.append(tiling_key)
        else:
            raise Exception("current kernel core type is not support")
            # gen main json
    new_sources += "#endif\n"
    # add dfx info section to sourse file
    CommonUtility().ascendc_write_file(compile_info.gen_kernel_func_file, new_sources)

    if len(cmds_list_vec) != 0:
        compile_multi_tilingkey(
            tiling_key_vec,
            cmds_list_vec,
            os.path.basename(compile_info.dst_file)[:-2] + "_tmp_aiv",
            compile_info.compile_log_path,
        )

    if len(cmds_list_aicore) != 0:
        compile_multi_tilingkey(
            tiling_key_aicore,
            cmds_list_aicore,
            os.path.basename(compile_info.dst_file)[:-2] + "_tmp_aic",
            compile_info.compile_log_path,
        )

    fatbin_objs(
        obj_files,
        compile_info.dst_file,
        compile_info.is_debug,
        compile_info.compile_log_path,
    )
    _gen_dynamic_json_for_v200(compile_info, tiling_info, final_kernel_type)
    return


def _compile_ascendc_cce_v200_with_kernel_type(
    compile_info: CompileInfo, compile_option_tuple, tiling_info: TilingInfo
):
    """call cce-c to compile a AscendC.cce file, generate a binary file and a json file

    Args:
        compile_info (CompileInfo): compile info for generate .o and .json
        compile_options (list): compile options for bisheng
        tiling_info (TilingInfo): tiling info
    """
    from .ascendc_compile_v200 import judge_valid_for_v200

    final_kernel_type = judge_valid_for_v200(compile_info.tiling_key_kernel_type)
    if tiling_info.static_shape_flag:
        _compile_ascendc_cce_v200_with_kernel_type_for_static(
            compile_info, compile_option_tuple, tiling_info
        )
    else:
        _compile_ascendc_cce_v200_with_kernel_type_for_dynamic(
            compile_info, compile_option_tuple, tiling_info, final_kernel_type
        )


def _compile_ascendc_cce_v220(
    compile_info: CompileInfo, compile_option_tuple, tiling_info: TilingInfo
):
    """call cce-c to compile a AscendC.cce file, generate a binary file and a json file

    Args:
        compile_info (CompileInfo): compile info for generate .o and .json
        compile_options (list): compile options for bisheng
        tiling_info (TilingInfo): tiling info
    """
    chip_version = CommonUtility.get_chip_version()
    if compile_info.code_channel == CORE_TYPE_MIX:
        # build cube
        set_soc_spec("AiCore")
        dst_file = compile_info.dst_file
        cube_compile_info = _get_sub_compile_info(compile_info, CORE_TYPE_CUBE)
        arch = f"dav-{chip_version}-cube"
        tiling_key_list = call_bisheng_v220(
            cube_compile_info,
            compile_option_tuple,
            tiling_info,
            arch,
            compile_info.code_channel,
        )
        if global_var_storage.get_variable("ascendc_sk_double_compile") is True:
            compile_info.global_kernel_symbols.extend(
                cube_compile_info.global_kernel_symbols
            )
        compile_info.compile_command_session.extend(
            cube_compile_info.compile_command_session.records
        )
        if compile_info.compile_command_session.should_execute:
            _gen_mix_sub_json(cube_compile_info, tiling_info, CORE_TYPE_CUBE)
        # build vector
        set_soc_spec("VectorCore")
        vec_compile_info = _get_sub_compile_info(compile_info, CORE_TYPE_VEC)
        arch = f"dav-{chip_version}-vec"
        call_bisheng_v220(
            vec_compile_info,
            compile_option_tuple,
            tiling_info,
            arch,
            compile_info.code_channel,
        )
        if global_var_storage.get_variable("ascendc_sk_double_compile") is True:
            compile_info.global_kernel_symbols.extend(
                vec_compile_info.global_kernel_symbols
            )
        compile_info.compile_command_session.extend(
            vec_compile_info.compile_command_session.records
        )
        # Record-only MIX replay merges both command streams and restores AiCore.
        if not compile_info.compile_command_session.should_execute:
            set_soc_spec("AiCore")
            return
        # fatbin 2o->1o
        mix_objs = [cube_compile_info.dst_file, vec_compile_info.dst_file]
        fatbin_objs(
            mix_objs, dst_file, compile_info.is_debug, compile_info.compile_log_path
        )
        # gen main json
        task_ration_str = f"1:{tiling_info.task_ration}"
        _gen_mix_json_from_seperate_json(
            compile_info.kernel_name, task_ration_str, CORE_TYPE_CUBE, True
        )
        set_soc_spec("AiCore")
    elif compile_info.hard_sync and compile_info.code_channel in [
        CORE_TYPE_VEC,
        CORE_TYPE_CUBE,
    ]:
        dst_file = compile_info.dst_file
        single_side_compile_info = _get_sub_compile_info(
            compile_info, compile_info.code_channel
        )
        arch = (
            f"dav-{chip_version}-vec"
            if compile_info.code_channel == CORE_TYPE_VEC
            else f"dav-{chip_version}-cube"
        )
        tiling_key_list = call_bisheng_v220(
            single_side_compile_info,
            compile_option_tuple,
            tiling_info,
            arch,
            compile_info.code_channel,
        )
        if global_var_storage.get_variable("ascendc_sk_double_compile") is True:
            compile_info.global_kernel_symbols.extend(
                single_side_compile_info.global_kernel_symbols
            )
        compile_info.compile_command_session.extend(
            single_side_compile_info.compile_command_session.records
        )
        # No JSON or fatbin is produced when only the command plan is requested.
        if not compile_info.compile_command_session.should_execute:
            set_soc_spec("AiCore")
            return
        _gen_mix_sub_json(
            single_side_compile_info, tiling_info, compile_info.code_channel
        )
        mix_objs = [single_side_compile_info.dst_file]
        fatbin_objs(
            mix_objs, dst_file, compile_info.is_debug, compile_info.compile_log_path
        )
        # gen main json
        task_ration_str = (
            "1:0" if compile_info.code_channel == CORE_TYPE_CUBE else "0:1"
        )
        _gen_mix_json_from_seperate_json(
            compile_info.kernel_name, task_ration_str, compile_info.code_channel, True
        )
        set_soc_spec("AiCore")
    else:
        arch, sub_core_type, optional_core = get_core_info(compile_info)
        set_soc_spec(optional_core)
        tiling_key_list = call_bisheng_v220(
            compile_info,
            compile_option_tuple,
            tiling_info,
            arch,
            compile_info.code_channel,
        )
        # call_bisheng_v220 already recorded the command and generated wrapper.
        if not compile_info.compile_command_session.should_execute:
            set_soc_spec("AiCore")
            return
        _gen_non_mix_sub_json(compile_info, tiling_info, sub_core_type)
    if not tiling_info.static_shape_flag:
        _dynamic_kernel_list_to_json(
            compile_info.kernel_name,
            tiling_key_list,
            compile_info.enable_deterministic,
            compile_info.tiling_key_deterministic,
        )


def get_core_info(compile_info: CompileInfo):
    chip_version = CommonUtility.get_chip_version()
    if compile_info.code_channel == CORE_TYPE_CUBE:
        arch = f"dav-{chip_version}-cube"
        sub_core_type = "AIC"
        optional_core = "AiCore"
        return arch, sub_core_type, optional_core
    elif compile_info.code_channel == CORE_TYPE_VEC:
        arch = f"dav-{chip_version}-vec"
        sub_core_type = "AIV"
        optional_core = "VectorCore"  # do the same work with SetOptionalCoreType in cpp
        return arch, sub_core_type, optional_core
    else:
        raise Exception(f"invalid code_channel = {compile_info.code_channel}")


def _compile_ascendc_cce_m510(
    compile_info: CompileInfo, compile_option_tuple, tiling_info: TilingInfo
):
    """call cce-c to compile a AscendC.cce file, generate a binary file and a json file

    Args:
        compile_info (CompileInfo): compile info for generate .o and .json
        compile_options (list): compile options for bisheng
        tiling_info (TilingInfo): tiling info
    """
    sub_core_type = "AIC"
    optional_core = "AiCore"
    arch = None
    set_soc_spec(optional_core)
    tiling_key_list = call_bisheng_v220(
        compile_info, compile_option_tuple, tiling_info, arch, compile_info.code_channel
    )
    _gen_non_mix_sub_json(compile_info, tiling_info, sub_core_type)
    if not tiling_info.static_shape_flag:
        _dynamic_kernel_list_to_json(
            compile_info.kernel_name,
            tiling_key_list,
            compile_info.enable_deterministic,
            compile_info.tiling_key_deterministic,
        )


def _get_sub_compile_info(compile_info: CompileInfo, core_type: int):
    sub_compile_info = copy.deepcopy(compile_info)
    core_type_marker = "_mix_aic" if core_type == CORE_TYPE_CUBE else "_mix_aiv"
    # i.e. change demo_kernel.o to demo_kernel_mix_aic.o
    sub_compile_info.dst_file = (
        compile_info.dst_file[:-2] + core_type_marker + compile_info.dst_file[-2:]
    )
    sub_compile_info.kernel_name = compile_info.kernel_name + core_type_marker
    sub_compile_info.sub_core_type = core_type
    # Clear global_kernel_symbols to avoid accumulation from parent compile_info
    sub_compile_info.global_kernel_symbols = []
    # Child core compilation inherits the mode without sharing recorded commands.
    sub_compile_info.compile_command_session = (
        compile_info.compile_command_session.fork()
    )
    return sub_compile_info
