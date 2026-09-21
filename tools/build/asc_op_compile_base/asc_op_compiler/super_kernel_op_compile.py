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
super kernel compile
"""

import os
import json
import shutil
import subprocess
from dataclasses import dataclass
from asc_op_compile_base.common.context import get_context
from asc_op_compile_base.common.error_mgr import (
    raise_tbe_python_err,
    TBE_DEFAULT_PYTHON_ERROR_CODE,
)
from .global_storage import global_var_storage
from .get_op_tiling import OpInfo
from .ascendc_compile_dfx import DFXSectionGenerator
from .compile_op import (
    _compile_ascendc_cce_v220_with_kernel_type_for_static,
    _json_post_process,
)
from .ascendc_constants import CompileOptionTuple, KernelMetaType
from .ascendc_common_utility import CommonUtility, CompileInfo
from .ascendc_compile_base import fatbin_objs, link_relocatable, compile_pre_process
from asc_op_compile_base.common.utils.log_utils import CompileStage
from .get_op_tiling import TilingInfo
from .super_kernel_constants import SuperKernelLinkMode, CALL_INSTS, ERR_CODE
from .super_kernel_utility import run_local_cmd


@dataclass
class SuperSplitInfo:
    sk_new: list
    sk_funs: list
    sk_path: str


def gen_super_kernel_compile_info(kernel_info, compile_log_path):
    kernel_meta_dir = CommonUtility.get_kernel_meta_dir()
    compile_info = CompileInfo()
    compile_info.src_file = kernel_info["kernel_file"]
    compile_info.dst_file = os.path.join(
        kernel_meta_dir, kernel_info["kernel_name"] + ".o"
    )
    compile_info.kernel_name = kernel_info["kernel_name"]
    compile_info.origin_func_name = kernel_info["kernel_name"]
    compile_info.op_type = kernel_info["kernel_name"]
    compile_info.tiling_key_list = ["0"]
    compile_info.is_debug = False
    compile_info.compile_log_path = compile_log_path
    compile_info.tiling_key_kernel_type = {"0": kernel_info["kernel_type"]}
    compile_info.no_set_kernel_type = False
    compile_info.default_kernel_type = kernel_info["kernel_type"]
    compile_info.gen_kernel_func_file = kernel_info["kernel_file"]
    compile_info.enable_final_super_kernel_compile = True
    compile_info.super_kernel_info = kernel_info
    compile_info.is_super_kernel_compile = True

    tiling_info = TilingInfo()
    tiling_info.static_shape_flag = True
    tiling_info.tiling_key = 0
    tiling_info.tiling_data_file_path = ""
    tiling_info.block_num = kernel_info["block_num"]
    return compile_info, tiling_info


def gen_super_kernel_link_obj_sequence(
    compile_info: CompileInfo,
    sub_op_kernel_info,
    link_mode: SuperKernelLinkMode,
    split_mode,
    compile_log_path=None,
):
    super_kernl_files = (
        compile_info.super_kernel_objs
        if len(compile_info.super_kernel_objs) != 0
        else [compile_info.dst_file]
    )
    objs_vec = []
    objs_cube = []
    objs_dynamic = []
    objs = []
    for sub_op in sub_op_kernel_info:
        if "aiv_bin" in sub_op:
            objs_vec.append(sub_op["aiv_bin"])
        if "aic_bin" in sub_op:
            objs_cube.append(sub_op["aic_bin"])
        if "dynamic_bin" in sub_op:
            objs_dynamic.append(sub_op["dynamic_bin"])
    if link_mode.value == SuperKernelLinkMode.PerVecHerCube.value:
        objs += super_kernl_files
        objs += objs_vec
        objs += objs_cube
    elif link_mode.value == SuperKernelLinkMode.PerCubeHerVec.value:
        objs += super_kernl_files
        objs += objs_cube
        objs += objs_vec
    elif link_mode.value == SuperKernelLinkMode.PerCubeHerVecWithSuper.value:
        if len(super_kernl_files) == 1:
            objs += super_kernl_files
            objs += objs_cube
            objs += objs_vec
        else:
            sp_vec = ""
            sp_cube = ""
            for sp_file in super_kernl_files:
                if "mix_aic" in sp_file:
                    sp_cube = sp_file
                elif "mix_aiv" in sp_file:
                    sp_vec = sp_file
            objs.append(sp_cube)
            objs += objs_cube
            objs.append(sp_vec)
            objs += objs_vec
    else:
        CommonUtility().ascendc_raise_python_err(
            ERR_CODE, "[Super Kernel] Invalid link mode type"
        )
    # add dynamic bin at the end of objs
    objs += objs_dynamic
    unique_lst = list(dict.fromkeys(objs))
    return unique_lst


def gen_system_run_cfg(kernel_type):
    file_header = ""
    if (
        kernel_type == KernelMetaType.KERNEL_TYPE_AIV_ONLY
        or kernel_type == KernelMetaType.KERNEL_TYPE_MIX_AIV_1_0
    ):
        file_header += "#if (defined(__DAV_VEC__) && __NPU_ARCH__ == 2201)\n"
    else:
        file_header += "#if (defined(__DAV_CUBE__) && __NPU_ARCH__ == 2201)\n"

    file_header += f"    __gm__ struct OpSystemRunCfg g_opSystemRunCfg = {{{0}}};\n"
    file_header += "#else\n"
    file_header += "    extern __gm__ struct OpSystemRunCfg g_opSystemRunCfg;\n"
    file_header += "#endif\n\n"
    return file_header


def gen_file_header(kernel_type, split_mode):
    file_header = """
#if 1
#include "kernel_operator.h"
"""
    if split_mode <= 1:
        file_header += gen_system_run_cfg(kernel_type)
    return file_header


def super_kernel_gen_entry(entry, sk_fun, split_mode, kernel_type):
    pre_insts = [
        "0x02010880",
        "0x07020003",
        "0x00c0100a",
        "0x0a000000",
        "0x40200006",
        "0x0a000001",
        "0x40200005",
        "0x0a000002",
        "0x40200004",
    ]
    code = '#include "kernel_operator.h"\n'
    if split_mode > 1:
        code += gen_system_run_cfg(kernel_type)
    for i in range(0, split_mode):
        if i == 0:
            fun = sk_fun
        else:
            fun = f"{sk_fun}_{i}"
        code += f'extern "C" [aicore] void {fun}(void);\n'
    code += f'extern "C" [aicore] void {entry}(void) {{\n'
    code += '__asm__ volatile("\\n\\\n'
    for inst in pre_insts:
        code += f"                  .long {inst}\\n\\\n"
    for i in range(0, split_mode):
        code += f"                  {CALL_INSTS} %{i}\\n\\\n"
    code += '                  "\n'
    code += "                  :\n"
    args = []
    for i in range(0, split_mode):
        if i == 0:
            fun = sk_fun
        else:
            fun = f"{sk_fun}_{i}"
        args.append(f'"l"(((int64_t){fun})/4)')
    args_info = ", ".join(args)
    code += f"                  : {args_info}\n"
    code += '                  : "X0", "X1"\n'
    code += ");\n"
    code += "}\n"
    return code


def gen_spk_kernel_call(
    super_split_info: SuperSplitInfo, split_mode, kernel_type, compile_log_path=None
):
    sk_out = []
    for i, item in enumerate(super_split_info.sk_new):
        _sk_code = super_kernel_gen_entry(
            super_split_info.sk_funs[i], item, split_mode, kernel_type
        )
        _sk_e_file = super_split_info.sk_path + f"/sk_{i}.cpp"
        _sk_o_file = super_split_info.sk_path + f"/sk_{i}.o"
        CommonUtility().ascendc_write_file(_sk_e_file, _sk_code)
        chip_version = CommonUtility.get_chip_version()

        if super_split_info.sk_funs[i].find("mix_aiv") > 0:
            _sk_arch = f"dav-{chip_version}-vec"
        else:
            _sk_arch = f"dav-{chip_version}-cube"

        cmds = ["ccec", "-x", "cce"]
        ascend_home_path = os.environ.get("ASCEND_HOME_PATH")
        import platform

        archlinux = platform.machine()
        if ascend_home_path is None or ascend_home_path == "":
            asc_opc_path = shutil.which("asc_opc")
            if asc_opc_path is not None:
                asc_opc_path_link = os.path.dirname(asc_opc_path)
                asc_opc_real_path = os.path.realpath(asc_opc_path_link)
                ascend_home_path = os.path.realpath(
                    os.path.join(asc_opc_real_path, "..", "..")
                )
            else:
                ascend_home_path = "/usr/local/Ascend/cann"

        if "x86" in archlinux:
            asc_path = os.path.realpath(
                os.path.join(ascend_home_path, "x86_64-linux", "asc")
            )
        else:
            asc_path = os.path.realpath(
                os.path.join(ascend_home_path, "aarch64-linux", "asc")
            )
        if asc_path is None:
            asc_path = os.path.realpath(
                os.path.join(ascend_home_path, "compiler", "asc")
            )

        cmds.append("-I" + os.path.join(asc_path, "impl", "adv_api"))
        cmds.append("-I" + os.path.join(asc_path, "impl", "basic_api"))
        cmds.append("-I" + os.path.join(asc_path, "impl", "c_api"))
        cmds.append("-I" + os.path.join(asc_path, "impl", "basic_api", "reg_compute"))
        cmds.append("-I" + os.path.join(asc_path, "impl", "simt_api"))
        cmds.append("-I" + os.path.join(asc_path, "impl", "utils"))
        cmds.append("-I" + os.path.join(asc_path, "include"))
        cmds.append("-I" + os.path.join(asc_path, "include", "adv_api"))
        cmds.append("-I" + os.path.join(asc_path, "include", "basic_api"))
        cmds.append("-I" + os.path.join(asc_path, "include", "aicpu_api"))
        cmds.append("-I" + os.path.join(asc_path, "include", "c_api"))
        cmds.append(
            "-I" + os.path.join(asc_path, "include", "basic_api", "reg_compute")
        )
        cmds.append("-I" + os.path.join(asc_path, "include", "simt_api"))
        cmds.append("-I" + os.path.join(asc_path, "include", "utils"))
        cmds.append("-I" + os.path.join(asc_path, "..", "ascendc", "act"))
        cmds.append("-I" + os.path.join(asc_path, "impl"))
        cmds.append("-I" + asc_path)
        cmds.append("-I" + os.path.join(asc_path, "..", "tikcpp"))
        cmds.append("-I" + os.path.join(asc_path, "..", "..", "include"))
        cmds.append("-I" + os.path.join(asc_path, "..", "..", "include", "ascendc"))
        cmds.append("-I" + os.path.join(asc_path, "..", "tikcpp", "tikcfw"))
        cmds.append("-I" + os.path.join(asc_path, "..", "tikcpp", "tikcfw", "impl"))
        cmds.append(
            "-I" + os.path.join(asc_path, "..", "tikcpp", "tikcfw", "interface")
        )

        if CommonUtility.is_c310():
            cmds += ["-D__DAV_C310__"]
        cmds += [
            "-O3",
            f"--cce-aicore-arch={_sk_arch}",
            "--cce-aicore-only",
            "-mllvm",
            "-cce-aicore-addr-transform",
            "-mllvm",
            "-cce-aicore-dcci-insert-for-scalar=false",
            "-mllvm",
            "-cce-aicore-stack-size=0x8000",
            "-std=c++17",
            "-DTILING_KEY_VAR=0",
            "-o",
            f"{_sk_o_file}",
            "-c",
            f"{_sk_e_file}",
        ]
        CommonUtility.run_cmd_inner(cmds, CompileStage.COMPILE, compile_log_path)
        sk_out.append(_sk_o_file)
    return sk_out


def split_spk_kernel_objs(
    sub_objs: list, split_mode, kernel_type, compile_log_path=None
):
    _sk_objs = []
    _sk_funs = []
    _sk_new = []
    _sk_path = ""
    _sk_out = []
    if split_mode is None or split_mode <= 1:
        return sub_objs, _sk_new
    for _obj in sub_objs:
        if _obj.find("te_superkernel_") > 0:
            _o_name = _obj.split("/")[-1]
            _k_name = _o_name.split(".")[0]
            _no_mix = "_".join(_k_name.split("_")[:3])
            _n_name = _k_name.replace(_no_mix, "_sk")
            for i in range(1, split_mode):
                copy_cmd = ["cp", "{}".format(_obj), "{}.split{}.o".format(_obj, i)]
                run_local_cmd(copy_cmd, compile_log_path)
                redefine_sym_cmd = [
                    "llvm-objcopy",
                    "--redefine-sym",
                    "{}={}_{}".format(_k_name, _n_name, i),
                    "{}.split{}.o".format(_obj, i),
                ]
                run_local_cmd(redefine_sym_cmd, compile_log_path)
                strip_sym_cmd = [
                    "llvm-objcopy",
                    "--strip-symbol=g_opSystemRunCfg",
                    "{}.split{}.o".format(_obj, i),
                ]
                run_local_cmd(strip_sym_cmd, compile_log_path)
            redefine_sym_cmd = [
                "llvm-objcopy",
                "--redefine-sym",
                "{}={}".format(_k_name, _n_name),
                "{}".format(_obj),
            ]
            run_local_cmd(redefine_sym_cmd, compile_log_path)
            _sk_objs.append(_obj)
            _sk_funs.append(_k_name)
            _sk_new.append(_n_name)
            _sk_path = os.path.dirname(_obj)
    super_split_info = SuperSplitInfo(
        sk_new=_sk_new, sk_funs=_sk_funs, sk_path=_sk_path
    )
    _sk_out = gen_spk_kernel_call(
        super_split_info, split_mode, kernel_type, compile_log_path
    )
    _sk_all = ":".join(sub_objs)
    for i in range(0, len(_sk_new)):
        _sk_new_files = [
            _sk_out[i],
            _sk_objs[i],
            f"{_sk_objs[i]}.split1.o",
            f"{_sk_objs[i]}.split2.o",
            f"{_sk_objs[i]}.split3.o",
        ]
        _sk_nobj = ":".join(_sk_new_files)
        _sk_all = _sk_all.replace(_sk_objs[i], _sk_nobj)
    sub_objs = _sk_all.split(":")
    return sub_objs, _sk_new


def localization_sub_op_func_sym(dst_file: str, sub_op_kernel_info):
    symbols = []
    for sub_op in sub_op_kernel_info:
        sub_symbols = sub_op.get("sub_kernel_names", [])
        symbols += sub_symbols
    try:
        unique_symbols = list(dict.fromkeys(symbols))
        length_symbols = len(unique_symbols)
        for i in range(0, length_symbols, 256):
            end_idx = min(i + 256, length_symbols)
            localization_symbols_cmd = (
                ["llvm-objcopy"]
                + [
                    f"--localize-symbol={symbol}"
                    for symbol in unique_symbols[i:end_idx]
                ]
                + [dst_file]
            )
            subprocess.run(localization_symbols_cmd)
    except Exception as err:
        raise_tbe_python_err(
            TBE_DEFAULT_PYTHON_ERROR_CODE, ("localize sub op func sym failed", err)
        )


def localize_symbol_of_sk(split_mode, sks, spk_dst_file, compile_log_path):
    for i in range(0, split_mode):
        for _sk_n in sks:
            if i == 0:
                _sk_sym = _sk_n
            else:
                _sk_sym = f"{_sk_n}_{i}"
            local_synbol_cmds = [
                "llvm-objcopy",
                "--localize-symbol={}".format(_sk_sym),
                "{}".format(spk_dst_file),
            ]
            run_local_cmd(local_synbol_cmds, compile_log_path)


def compile_super_kernel(kernel_info, compile_log_path, enable_features: dict = None):
    global_var_storage.set_variable("super_kenel_save_sub_op_files", True)
    kernel_info["kernel_type"] = KernelMetaType(kernel_info["kernel_type"])
    op_info = OpInfo()
    compile_options = kernel_info["compile_option"]
    DFXSectionGenerator().is_support = False
    compile_options = compile_pre_process(op_info, compile_options)
    # if enable timestamp, need set workspace to generate workspace param
    if kernel_info["timestamp_option"]:
        get_context().add_workspace("total_workspace", size=1)
    json_str = json.dumps(kernel_info["op_list"], indent=4)
    CommonUtility.dump_compile_log([json_str], CompileStage.SPK_INPUT, compile_log_path)
    compile_info, tiling_info = gen_super_kernel_compile_info(
        kernel_info, compile_log_path
    )
    compile_option_tuple = CompileOptionTuple(
        [] if compile_options is None else compile_options, []
    )
    if kernel_info["split_mode"] is not None and kernel_info["split_mode"] > 1:
        compile_option_tuple.mllvm_options.append("-mllvm")
        compile_option_tuple.mllvm_options.append("-cce-aicore-jump-expand=true")
        compile_option_tuple.mllvm_options.append("--cce-long-call=true")
        compile_option_tuple.mllvm_options.append("-mllvm")
        compile_option_tuple.mllvm_options.append("-cce-aicore-long-call")
    if CommonUtility.is_c310() or CommonUtility.is_m510():
        compile_option_tuple.compile_options.append("--cce-no-dcache-flush")
    if kernel_info["timestamp_option"]:
        compile_options.append(
            "-DONE_CORE_DUMP_SIZE="
            + str(
                compile_info.super_kernel_info["debug_size"]
                / CommonUtility.get_dump_core_num()
            )
        )
    _compile_ascendc_cce_v220_with_kernel_type_for_static(
        compile_info, compile_option_tuple, tiling_info
    )
    sub_objs = gen_super_kernel_link_obj_sequence(
        compile_info,
        kernel_info["sub_operator"],
        kernel_info["link_mode"],
        kernel_info["split_mode"],
        compile_info.compile_log_path,
    )
    ## begin add superkernel split
    sub_objs, _sk_new = split_spk_kernel_objs(
        sub_objs,
        kernel_info["split_mode"],
        kernel_info["kernel_type"],
        compile_info.compile_log_path,
    )
    fatbin_objs(
        sub_objs,
        compile_info.dst_file,
        compile_info.is_debug,
        compile_info.compile_log_path,
    )
    op_info = OpInfo()
    link_relocatable(compile_info.dst_file, compile_info.compile_log_path)
    localization_sub_op_func_sym(compile_info.dst_file, kernel_info["sub_operator"])
    _json_post_process(
        compile_info, op_info, tiling_info, True, True, compile_info.compile_log_path
    )
    localize_symbol_of_sk(
        kernel_info["split_mode"],
        _sk_new,
        compile_info.dst_file,
        compile_info.compile_log_path,
    )


def super_kernel_compile(kernel_info, compile_log_path):
    kernel_info["kernel_type"] = kernel_info["kernel_type"].value
    compile_super_kernel(kernel_info, compile_log_path)
