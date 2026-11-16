#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2025 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

import os
import glob
import configparser
import shutil
import argparse
import json
from pathlib import Path
import subprocess
import const_var
import ascendc_impl_build
import ascendc_bin_param_build
import ascendc_op_info
from ascendc_build_kernel_spec import task_directory
from opdesc_parser import _trans_soc_ver_to_short


def _write_kernel_spec_result(directory, source, status, reason=""):
    directory.mkdir(parents=True, exist_ok=True)
    result = directory / ".producer-result.json"
    temporary = result.with_suffix(".tmp")
    temporary.write_text(json.dumps({"status": status, "source": source, "reason": reason}), encoding="utf-8")
    os.replace(temporary, result)


class CompileKernel:
    def __init__(self: any, args: any):
        self.op_type = args.op_type
        self.kernel_spec_mode = getattr(args, "kernel_spec_mode", "None")
        self.kernel_spec_dir = getattr(args, "kernel_spec_dir", None)
        self.op_cpp_file = os.path.realpath(args.src_file)
        self.impl_path = os.path.dirname(self.op_cpp_file)
        self.op_soc_ver = args.compute_unit
        self.compile_options = args.compile_options
        self.op_debug_config = args.debug_config
        self.op_cfg_ini = os.path.realpath(args.config_ini)
        self.op_tiling = os.path.realpath(args.tiling_lib)
        self.op_output = os.path.realpath(args.output_path)
        self.op_impl_py = None
        self.compile_sh = []
        self.target_name = args.target_name
        self.auto_gen_path = args.auto_gen_path
        self.source_ini_file = os.path.join(self.auto_gen_path, self.target_name + "_custom_source_files.ini")

        if os.path.exists(self.source_ini_file):
            self.get_op_cpp_file()

        self.working_dir = os.path.join(os.getcwd(), self.op_type + "_" + self.op_soc_ver)
        self.build_opp_path = os.path.join(self.working_dir, "customize")
        os.makedirs(self.working_dir, exist_ok=True)
        os.makedirs(self.op_output, exist_ok=True)
        if args.dynamic_dir is not None and args.dynamic_dir != "":
            self.dynamic_dir = os.path.realpath(args.dynamic_dir)
        else:
            self.dynamic_dir = None
        if args.json_file is not None and args.json_file != "":
            self.json_file = args.json_file
        else:
            self.json_file = None

    def get_op_cpp_file(self):
        src_ini_config = configparser.ConfigParser()
        src_ini_config.read(self.source_ini_file)
        find_type = False
        find_cpp_file = False
        src_compute_units_low = []
        for sub_sections in src_ini_config.sections():
            if sub_sections[65:] == self.op_type:
                find_type = True

                if src_ini_config.has_option(sub_sections, "compute_unit"):
                    src_compute_units = src_ini_config.get(sub_sections, "compute_unit").split(",")
                    for sub_unit in src_compute_units:
                        src_compute_units_low.append(_trans_soc_ver_to_short(sub_unit))

                if len(src_compute_units_low) == 0 or self.op_soc_ver in src_compute_units_low:
                    if src_ini_config.has_option(sub_sections, "kernel_dir"):
                        self.op_cpp_file = os.path.realpath(
                            os.path.join(
                                self.impl_path,
                                src_ini_config.get(sub_sections, "kernel_dir"),
                                src_ini_config.get(sub_sections, "kernel_file"),
                            )
                        )
                    else:
                        self.op_cpp_file = os.path.realpath(
                            os.path.join(self.impl_path, src_ini_config.get(sub_sections, "kernel_file"))
                        )
                    find_cpp_file = True
                    break
        if find_cpp_file != find_type:
            raise ValueError(
                f"Miss kernel implementation for op '{self.op_type}' on target '{self.op_soc_ver}'. "
                f"\nHost claims support, but kernel only define {src_compute_units_low}. "
                f"Please check ini file :'{self.source_ini_file}'. "
            )

    def clean(self: any):
        if "dump_cce" not in self.op_debug_config:
            shutil.rmtree(self.working_dir)
        return

    def ascendc_gen_impl(self: any):
        rep_cfg = {}
        rep_cfg[const_var.REPLAY_BATCH] = ""
        rep_cfg[const_var.REPLAY_ITERATE] = ""
        cfg_dir = {}
        cfg_dir[const_var.CFG_IMPL_DIR] = self.impl_path
        cfg_dir[const_var.CFG_OUT_DIR] = os.path.join(self.working_dir, "dynamic")
        os.makedirs(os.path.join(self.working_dir, "dynamic"), exist_ok=True)
        cfg_dir[const_var.AUTO_GEN_DIR] = os.path.dirname(self.op_cfg_ini)
        ascendc_impl_build.write_scripts(
            self.op_cfg_ini, rep_cfg, cfg_dir, [self.op_type], self.compile_options, self.source_ini_file
        )
        py_files = glob.glob(os.path.join(self.working_dir, "dynamic", "*.py"))
        if py_files is None or len(py_files) != 1:
            self.clean()
            raise RuntimeError("compile py file {} generated error!".format(py_files))
        self.op_impl_py = os.path.join(self.working_dir, "dynamic", self.op_type + ".py")
        if self.dynamic_dir is not None:
            shutil.copy(py_files[0], self.dynamic_dir)
        os.rename(py_files[0], self.op_impl_py)
        if not os.path.exists(self.op_impl_py):
            self.clean()
            raise RuntimeError("compile py file {} not generated!".format(self.op_impl_py))

    def ascendc_gen_param(self: any):
        bin_param_path = os.path.join(self.working_dir, "bin_param")
        os.makedirs(bin_param_path)
        base_dir = os.path.dirname(self.op_cfg_ini)
        opc_config_file = os.path.join(base_dir, "custom_opc_options.ini")
        task_options = {}
        if self.kernel_spec_mode != "None":
            task_options = {"kernel_spec_mode": self.kernel_spec_mode, "kernel_spec_dir": self.kernel_spec_dir}
        ascendc_bin_param_build.gen_bin_param_file(
            self.op_cfg_ini, bin_param_path, self.op_soc_ver, opc_config_file, [self.op_type], **task_options
        )
        (tiling_key_info, op_debug_config, kernel_json_file, _, kernel_template_input_info, _) = (
            ascendc_bin_param_build.parse_op_debug_config(opc_config_file, self.op_soc_ver)
        )
        if self.op_type in op_debug_config:
            self.op_debug_config = op_debug_config[self.op_type]
        if "ALL" in op_debug_config:
            self.op_debug_config = op_debug_config["ALL"]
        bin_param_files = glob.glob(os.path.join(bin_param_path, "*.json"))
        if bin_param_files is None or len(bin_param_files) <= 0:
            self.clean()
            raise RuntimeError("compile binary param json file not generated!")
        self.compile_sh = glob.glob(os.path.join(bin_param_path, "*.sh"))
        if self.compile_sh is None or len(self.compile_sh) != len(bin_param_files):
            self.clean()
            raise RuntimeError("compile binary shell file not generated!")

    def ascendc_put_tiling(self: any):
        tiling_path = os.path.join(self.build_opp_path, "op_impl", "ai_core", "tbe", "op_tiling")
        os.makedirs(tiling_path)
        tiling_so = os.path.join(tiling_path, "liboptiling.so")
        os.symlink(self.op_tiling, tiling_so)
        if not os.path.exists(tiling_so):
            self.clean()
            raise RuntimeError("prepare tiling lib {} link failed!".format(tiling_so))

    def ascendc_put_json(self: any):
        if self.json_file is not None:
            json_file_dir = os.path.join(self.build_opp_path, "op_impl", "ai_core", "tbe", "config", self.op_soc_ver)
            os.makedirs(json_file_dir)
            shutil.copy(self.json_file, json_file_dir)
            build_json_file = os.path.join(json_file_dir, "aic-{}-ops-info.json".format(self.op_soc_ver))
            if not os.path.exists(build_json_file):
                self.clean()
                raise RuntimeError("prepare json file aic-{}-ops-info.json failed!".format(self.op_soc_ver))

    def _write_makefile(self, op_file):
        all_tar = []
        sub_cmd = []
        index = 0
        for sh in self.compile_sh:
            tar = op_file + str(index)
            build_path = os.path.join(self.working_dir, "kernel_" + str(index))
            os.makedirs(build_path)
            all_tar.append(tar)
            sub_cmd.append(tar + ":")
            sub_cmd.append("\tcd {} && bash {} --kernel-src=$(CPP) $(PY) $(OUT) $(MAKE)".format(build_path, sh))
            index += 1
        mkfile = os.path.join(self.working_dir, op_file + ".make")
        with os.fdopen(os.open(mkfile, const_var.WFLAGS, const_var.WMODES), "w") as fd:
            sub_cmd.insert(0, "all: " + " ".join(all_tar))
            fd.write("\n".join(sub_cmd))

        return mkfile

    def _run_make(self, mkfile, op_bin_dir):
        if os.getenv("TILINGKEY_PAR_COMPILE") is None:
            cmd_str = (
                "export HI_PYTHON=python3 && export ASCEND_CUSTOM_OPP_PATH={} && export TILINGKEY_PAR_COMPILE=1"
                "&& make -f {} PY={} OUT={} CPP={}"
            )
        else:
            cmd_str = "export HI_PYTHON=python3 && export ASCEND_CUSTOM_OPP_PATH={} && make -f {} PY={} OUT={} CPP={}"
        command = cmd_str.format(self.build_opp_path, mkfile, self.op_impl_py, op_bin_dir, self.op_cpp_file)
        if self.kernel_spec_mode == "None":
            returncode = os.system(command)
        else:
            result = subprocess.run(
                command, shell=True, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, close_fds=False
            )
            print(result.stdout, end="", flush=True)
            returncode = result.returncode
            if returncode == 0:
                expected = sum("--kernel-spec=" in Path(script).read_text() for script in self.compile_sh)
                outcomes = [
                    line[len(const_var.KERNEL_SPEC_RESULT_PREFIX) :]
                    for line in result.stdout.splitlines()
                    if line.startswith(const_var.KERNEL_SPEC_RESULT_PREFIX)
                ]
                if (
                    not expected
                    or len(outcomes) != expected
                    or any(value not in {"completed", "filtered"} for value in outcomes)
                ):
                    raise RuntimeError("Kernel Spec compile variants did not report complete results")
                self.kernel_spec_status = "completed" if "completed" in outcomes else "filtered"
        if returncode != 0:
            raise RuntimeError(
                "Kernel Compilation Error: OpType {} Kernel File {}!".format(self.op_type, self.op_cpp_file)
            )

    def ascendc_build(self: any):
        op_info = ascendc_op_info.OpInfo(self.op_type, self.op_cfg_ini)
        op_file = op_info.get_op_file()
        op_bin_dir = os.path.join(self.op_output, self.op_soc_ver, op_file)
        os.makedirs(op_bin_dir, exist_ok=True)
        self._run_make(self._write_makefile(op_file), op_bin_dir)

    def record_kernel_spec_result(self, status, reason=""):
        """Publish this producer's result and source selection in one record."""
        if self.kernel_spec_mode == "None":
            return
        directory = task_directory(self.kernel_spec_dir, self.op_type, self.op_soc_ver)
        _write_kernel_spec_result(directory, self.op_cpp_file, status, reason)


def args_parse():
    parser = argparse.ArgumentParser()
    parser.add_argument("-n", "--op-type", nargs="?", help="Op type(Camel string) to compile.")
    parser.add_argument("-s", "--src-file", nargs="?", help="Op kernel source file.")

    parser.add_argument("-u", "--compute-unit", nargs="?", help="Compute unit.")
    parser.add_argument("-c", "--compile-options", nargs="?", help="Compile options of compiler.")
    parser.add_argument("-d", "--debug-config", nargs="?", help="Debug config of op, ref opc op-debug-config.")
    parser.add_argument("-i", "--config-ini", nargs="?", help="Op config ini file.")
    parser.add_argument("-t", "--tiling-lib", nargs="?", help="Tiling shared library file.")

    parser.add_argument("-o", "--output-path", nargs="?", help="Output path of compile result.")
    parser.add_argument("-dy", "--dynamic-dir", nargs="?", default=None, help="dynamic path of source compile.")
    parser.add_argument("-eb", "--enable-binary", nargs="?", default=None, help="whether binary compile is enabled.")
    parser.add_argument(
        "-j", "--json-file", nargs="?", default=None, help="aic-<compute-unit>-ops-info.json file path."
    )
    # $(MAKE) is necessary for parallel compiling
    parser.add_argument("-b", "--build-tool", nargs="?", default=None, help="build tool must be make.")
    parser.add_argument("-tn", "--target-name", nargs="?", default=None, help="kernel target name.")
    parser.add_argument("-a", "--auto-gen-path", nargs="?", default=None, help="auto gen file path.")
    parser.add_argument("--kernel-spec-mode", choices=("None", "Normal", "SK"), default="None")
    parser.add_argument("--kernel-spec-dir", default=None)
    return parser.parse_args()


def main():
    args = args_parse()
    kernel_builder = None
    try:
        kernel_builder = CompileKernel(args)
        kernel_builder.clean()
        kernel_builder.ascendc_gen_impl()
        if args.enable_binary != "False":
            kernel_builder.ascendc_gen_param()
            kernel_builder.ascendc_put_json()
            kernel_builder.ascendc_put_tiling()
            kernel_builder.ascendc_build()
            status = getattr(kernel_builder, "kernel_spec_status", "completed")
            kernel_builder.record_kernel_spec_result(
                status, "no variants produced Kernel Spec resources" if status == "filtered" else ""
            )
        kernel_builder.clean()
    except Exception as error:
        if kernel_builder is not None:
            kernel_builder.record_kernel_spec_result("failed", str(error))
        elif args.kernel_spec_mode != "None":
            _write_kernel_spec_result(
                task_directory(args.kernel_spec_dir, args.op_type, args.compute_unit),
                args.src_file,
                "failed",
                str(error),
            )
        raise


if __name__ == "__main__":
    main()
