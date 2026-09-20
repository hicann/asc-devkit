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
from __future__ import absolute_import as _abs

import os
import stat
import subprocess
import sys

from .error_mgr import raise_tbe_python_err, TBE_DEFAULT_PYTHON_ERROR_CODE


class CCECInfo:
    """CCEC Compiler infos."""

    @classmethod
    def _get_exe_path(cls, exe):
        cmd = ["which", exe]
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        _, _ = proc.communicate()
        if proc.returncode != 0:
            # using default path
            arch = os.popen("arch").read()
            if "x86" in arch:
                exe_path = (
                    "/usr/local/Ascend/cann/x86_64-linux/ccec_compiler/bin/%s" % exe
                )
            else:
                exe_path = (
                    "/usr/local/Ascend/cann/aarch64-linux/ccec_compiler/bin/%s" % exe
                )
        else:
            # exe is under current env
            exe_path = exe
        return exe_path

    @classmethod
    def get_exe(cls, item):
        if item not in cls._info:
            raise_tbe_python_err(
                TBE_DEFAULT_PYTHON_ERROR_CODE, "No item found for cls %s" % type(cls)
            )
        if cls._info.get(item) is None:
            cls._info[item] = cls._get_exe_path(item)
        return cls._info.get(item)

    # ccec compiler, name and path
    _info = {
        "ccec": None,
        "ld.lld": None,
    }


def current_build_config():
    """Get the current build configuration."""
    # 'pylint: disable=protected-access
    from .buildcfg.buildcfg import _build_cfg

    return _build_cfg.get()


def switching_compilation_mode():
    """Sitching the current compilatio mode."""
    # 'pylint: disable=protected-access
    print("O0 mode fails to be compiled, the O2 mode is used")


def _set_vector_fp_ceiling(cmd):
    """Set cce-aicore-fp-ceiling for Hi3519AV200 and ASCEND_310P

    Parameters
    ----------
    cmd : list
        The compile command.

    Return
    ------
    cmd : list
        The compile command.

    """
    from .buildcfg.buildcfg_mapping import vector_fp_ceiling

    if current_build_config().get(vector_fp_ceiling) == 1:
        cmd += ["-mllvm", "-cce-aicore-fp-ceiling=1"]
    elif current_build_config().get(vector_fp_ceiling) == 2:
        cmd += ["-mllvm", "-cce-aicore-fp-ceiling=2"]
    else:
        cmd += ["-mllvm", "-cce-aicore-fp-ceiling=0"]
    return cmd


def _set_cce_overflow(cmd):
    cmd += ["-mllvm", "-cce-aicore-record-overflow=false"]
    return cmd


def check_is_regbase_v2():
    from .platform.platform_info import get_soc_spec
    from .platform.platform_info import ASCEND_310B
    from .platform.platform_info import AS31XM1
    from .platform.platform_info import ASCEND_031
    from .platform.platform_info import ASCEND_610LITE
    from .platform.platform_info import BS9SX2A
    from .platform.platform_info import MC61AM21A
    from .platform.platform_info import ASCEND_950
    from .platform.platform_info import KIRIN_X90
    from .platform.platform_info import KIRIN_9030

    if get_soc_spec("SHORT_SOC_VERSION") in [
        ASCEND_031,
        ASCEND_310B,
        ASCEND_610LITE,
        BS9SX2A,
        MC61AM21A,
        AS31XM1,
        ASCEND_950,
        KIRIN_X90,
        KIRIN_9030,
    ]:
        return True
    return False


def enable_sanitizer():
    from .platform.platform_info import get_soc_spec
    from .buildcfg.buildcfg_mapping import op_debug_config
    from .platform.platform_info import ASCEND_910B
    from .platform.platform_info import ASCEND_910_93
    from .platform.platform_info import ASCEND_310P

    support_sanitizer = get_soc_spec("SHORT_SOC_VERSION") in [
        ASCEND_910B,
        ASCEND_910_93,
        ASCEND_310P,
    ]
    if "sanitizer" in current_build_config().get(op_debug_config) and support_sanitizer:
        return True
    else:
        return False


# 'pylint: disable=too-many-branches, too-many-locals, too-many-statements
def _build_aicore_compile_cmd(
    src_file, dst_file, name="", is_ffts_needed=False, is_mix=False
):
    """Build the compile command for aicore op.

    Parameters
    ----------
    cce_product_params : TempDirectory
        Instance of class CceProductParams.

    src_file : str
        The file of source code used in compile.

    dst_file : str
        The object file.

    Return
    ------
    cmd : list
        The compile command.

    """
    from .platform.platform_info import get_soc_spec
    from .platform.platform_info import VEC_BS9SX1A
    from .platform.platform_info import AIC_BS9SX1A
    from .platform.platform_info import AIC_610B
    from .platform.platform_info import VEC_610B
    from .platform.platform_info import ASCEND_910
    from .platform.platform_info import ASCEND_910B
    from .platform.platform_info import ASCEND_910_93
    from .platform.platform_info import HI3796CV300ES
    from .platform.platform_info import HI3796CV300CS
    from .platform.platform_info import SD3403
    from .platform.platform_info import ASCEND_610
    from .platform.platform_info import ASCEND_310P
    from .platform.platform_info import ASCEND_610LITE
    from .platform.platform_info import BS9SX2A
    from .platform.platform_info import MC61AM21A
    from .platform.platform_info import ASCEND_950
    from .platform.platform_info import COMPILER_ARCH
    from .buildcfg.buildcfg_mapping import tbe_debug_level, op_debug_config
    from .buildcfg.buildcfg_mapping import enable_cce_licm_safe_hoist
    from .buildcfg.buildcfg_mapping import enable_machine_outliner
    from .buildcfg.buildcfg_mapping import enable_vector_core
    from .buildcfg.buildcfg_mapping import enable_cce_remat_higher_weight
    from .buildcfg.buildcfg_mapping import deterministic_level
    from . import cce_params

    cce_arch = get_soc_spec(COMPILER_ARCH)
    suffix_list = [cce_params.MIX_AIC_SUFFIX, cce_params.MIX_AIV_SUFFIX]
    ori_name = name
    suffix = ""
    for s in suffix_list:
        if name.count(s) > 0:
            ori_name = name.replace(s, "")
            suffix = s
    from asc_op_compile_base.common.buildcfg.buildcfg_mapping import build_fatbin

    if not current_build_config().get(build_fatbin):
        ori_name = ori_name + "__kernel0"
    is_c220 = get_soc_spec("SHORT_SOC_VERSION") in [ASCEND_910B, ASCEND_910_93]
    is_enable_vector_core = current_build_config().get(enable_vector_core)

    def get_init_cmd():
        optimization_level = "-O2"
        if cce_arch == "dav-l300" or cce_arch == "dav-l311":
            optimization_level = "-O3"
        # 2 enable tbe debug : ccec compiler with "O0 - g"
        if current_build_config().get(
            tbe_debug_level
        ) == 2 or "ccec_O0" in current_build_config().get(op_debug_config):
            optimization_level = "-O0"
        arch = "cce-aicore-only"
        cce_arch_prefix = "cce-aicore-arch"
        cmd = [
            CCECInfo.get_exe("ccec"),
            "-c",
            optimization_level,
            src_file,
            "--%s=%s" % (cce_arch_prefix, cce_arch),
            "--%s" % arch,
            "-o",
            dst_file,
        ]
        if (
            is_c220
            or is_enable_vector_core
            or get_soc_spec("SHORT_SOC_VERSION") == ASCEND_950
        ):
            aicore_type = get_soc_spec("AICORE_TYPE")
            from asc_op_compile_base.common.buildcfg.buildcfg_mapping import (
                enforce_mix_mode,
            )

            is_need_modify = (is_ffts_needed or is_mix) or current_build_config().get(
                enforce_mix_mode
            )
            if is_need_modify and aicore_type == "AiCore":
                cmd += [
                    "-D",
                    "%s=%s" % (ori_name, ori_name + cce_params.MIX_AIC_SUFFIX),
                ]
            elif is_need_modify and aicore_type == "VectorCore":
                cmd += [
                    "-D",
                    "%s=%s" % (ori_name, ori_name + cce_params.MIX_AIV_SUFFIX),
                ]
        # 2 enable tbe debug : ccec compiler with "O0 - g"
        if current_build_config().get(
            tbe_debug_level
        ) == 2 or "ccec_g" in current_build_config().get(op_debug_config):
            cmd.append("-g")
        return cmd

    cmd = get_init_cmd()
    if (
        check_is_regbase_v2()
        and current_build_config().get(tbe_debug_level) == 2
        and "-O0" in cmd
    ):
        cmd += ["--cce-ignore-always-inline=false"]
    is_vec_610B = get_soc_spec("SHORT_SOC_VERSION") + get_soc_spec("AICORE_TYPE") in [
        VEC_BS9SX1A,
        VEC_610B,
    ]
    is_aic_610B = get_soc_spec("SHORT_SOC_VERSION") + get_soc_spec("AICORE_TYPE") in [
        AIC_BS9SX1A,
        AIC_610B,
    ]
    if get_soc_spec("SHORT_SOC_VERSION") in [ASCEND_910_93, ASCEND_910B]:
        cmd += ["-mllvm", "-cce-aicore-stack-size=32768"]
        cmd += ["-mllvm", "-cce-aicore-function-stack-size=32768"]
        cmd = _set_cce_overflow(cmd)
        cmd += ["-mllvm", "-cce-aicore-addr-transform"]
        if current_build_config().get(enable_cce_remat_higher_weight):
            cmd += ["-mllvm", "-cce-aicore-weight-for-reg-operand-remat=higher"]
    elif get_soc_spec("SHORT_SOC_VERSION") == ASCEND_910:
        cmd += ["-mllvm", "-cce-aicore-function-stack-size=16000"]
        cmd = _set_cce_overflow(cmd)
    elif get_soc_spec("SOC_VERSION") in [HI3796CV300ES, HI3796CV300CS, SD3403]:
        cmd += ["-mllvm", "-cce-aicore-sk-transform"]
        if get_soc_spec("SOC_VERSION") == "Hi3519AV200":
            cmd = _set_vector_fp_ceiling(cmd)
    elif get_soc_spec("SHORT_SOC_VERSION") == ASCEND_310P:
        cmd = _set_vector_fp_ceiling(cmd)
        cmd = _set_cce_overflow(cmd)
    elif get_soc_spec("SHORT_SOC_VERSION") == ASCEND_610:
        cmd = _set_vector_fp_ceiling(cmd)
    elif is_vec_610B:
        cmd += ["-mllvm", "-cce-aicore-auto-nop-insert=true"]
        cmd += ["-cce-v210-no-uninitialized"]
        cmd += ["-mllvm", "-cce-aicore-addr-transform"]
    elif is_aic_610B:
        cmd += ["-mllvm", "-cce-aicore-addr-transform"]
        cmd += ["-mllvm", "-cce-aicore-function-stack-size=16000"]
        cmd += ["-mllvm", "-cce-aicore-dcci-insert-for-scalar"]
    elif check_is_regbase_v2():
        cmd += ["-mllvm", "-cce-aicore-function-stack-size=16000"]
        cmd += ["-mllvm", "-cce-aicore-addr-transform"]
        cmd += ["-mllvm", "--cce-aicore-or-combine=false"]
        cmd += ["-mllvm", "-instcombine-code-sinking=false"]
        from asc_op_compile_base.common.platform.platform_info import (
            VECTOR_INST_BLOCK_WIDTH,
        )

        vec_len = get_soc_spec("VECTOR_REG_WIDTH")
        if vec_len != VECTOR_INST_BLOCK_WIDTH and vec_len != "0":
            cmd += ["-Xclang", "-fcce-vf-vl=" + str(vec_len)]
    cmd = modify_cmd_by_enable_cce_debug_mode(cmd)
    skt_env = os.getenv("SKT_ENABLE")
    if skt_env == "1":
        from asc_op_compile_base.asc_op_compiler import cce_runtime

        if cce_runtime.CceFlag.BatchBindOnly is True:
            cmd += ["-mllvm", "-cce-aicore-sk-transform"]
            cce_runtime.CceFlag.BatchBindOnly = False
    if (
        get_soc_spec("SHORT_SOC_VERSION") != ASCEND_610LITE
        and get_soc_spec("SHORT_SOC_VERSION") != BS9SX2A
    ):
        if (
            get_soc_spec("SHORT_SOC_VERSION") != MC61AM21A
            and get_soc_spec("SHORT_SOC_VERSION") != ASCEND_950
        ):
            cmd += ["--cce-auto-sync=off"]
    if get_soc_spec("SHORT_SOC_VERSION") == ASCEND_950:
        cmd += ["--cce-long-scbz=true"]
        cmd += ["--cce-simd-vf-fusion=false"]
    if current_build_config().get(enable_cce_licm_safe_hoist):
        cmd += ["-mllvm", "-licm-safe-hoist=true"]
    cmd += ["-mllvm", "-cce-aicore-jump-expand=false"]
    cmd += ["-mllvm", "-cce-aicore-mask-opt=false"]
    if current_build_config().get(enable_machine_outliner):
        cmd += ["-mllvm", "-enable-machine-outliner"]
    if enable_sanitizer():
        cmd[-3] = "-cce-aicore-jump-expand=true"
        cmd += ["--cce-enable-sanitizer", "-g", "-mllvm", "-cce-aicore-long-call"]
    return cmd


def modify_cmd_by_enable_cce_debug_mode(cmd):
    """Add --cce-debug-mode to the compile command if trace_store is used on ASCEND_310 or
    ASCEND_910.
    :param cmd: list
        The compile command.
    :return: cmd: list
        The compile command.
    """
    from .platform.platform_info import get_soc_spec
    from .platform.platform_info import ASCEND_910
    from .platform.platform_info import ASCEND_310
    from .buildcfg.buildcfg_mapping import enable_cce_debug_mode

    if current_build_config().get(enable_cce_debug_mode):
        if not (
            ASCEND_910 in get_soc_spec("SOC_VERSION")
            or ASCEND_310 in get_soc_spec("SOC_VERSION")
        ):
            raise RuntimeError(
                "%s doesn't support trace_store\n" % get_soc_spec("SOC_VERSION")
            )
        cmd += ["--cce-debug-mode"]
    return cmd


# default delimiter for env path
PATH_DELIMITER = ":"
if sys.platform.startswith("linux"):
    PATH_DELIMITER = ":"
elif sys.platform.startswith("win32"):
    PATH_DELIMITER = ";"
else:
    raise_tbe_python_err(
        TBE_DEFAULT_PYTHON_ERROR_CODE, ("Platform % is not support now" % sys.platform)
    )
