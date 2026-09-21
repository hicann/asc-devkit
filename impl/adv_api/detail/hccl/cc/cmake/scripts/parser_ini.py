#!/usr/bin/env python3
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
import argparse
import configparser
import json
import os
import stat
from collections import defaultdict
from distutils import util


COLOR_BOLD = "\033[1m"
COLOR_CYAN = "\033[96m"
COLOR_END = "\033[0m"
COLOR_GREEN = "\033[91m"
COLOR_RED = "\033[91m"
CUSTOM_PREF = "custom"


class IniParser(object):
    """
    initial parser class
    """

    required_op_info_keys = ["opKernelLib"]
    required_custom_op_info_keys = ["kernelSo", "functionName"]
    input_output_info_keys = {"format", "type", "name"}  # set for difference

    def __init__(self):
        self.aicpu_ops_info = None
        self.custom_ops_info = None
        self.custom_flag = False
        self.warn_print = False
        self.warning_ops = defaultdict(list)

    def load_ini_info(self, ini_files):
        """
        Load config info from ini files, store in class struct: self.aicpu_ops_info
        """
        for ini_file in ini_files:
            self.parse_ini_to_obj(ini_file)

    def parse_ini_to_obj(self, ini_file):
        """
        Parse specific ini file
        """
        cfg = configparser.ConfigParser()
        cfg.optionxform = str
        with open(ini_file, "r") as inif:
            cfg.read_file(inif)

        # config file section is op name, eg. "Add", "Cast"
        for op in cfg.sections():
            self.aicpu_ops_info[op] = {}
            # option in op is configuration for op, eg. opInfo.engine=DNN_VM_AICPU
            for opt in cfg.options(op):
                if len(opt.split(".")) != 2:
                    print('## Parse op [%s] setting: "%s", not recognized!' % (op, opt))
                    continue
                # one opt_sec will include serval info, eg. opInfo: {"engine": xxx, "flagAsync": xxx, ...}
                opt_sec, opt_subsec = opt.split(".")
                if opt_sec not in self.aicpu_ops_info.get(op):
                    self.aicpu_ops_info.get(op)[opt_sec] = {opt_subsec: cfg[op][opt]}
                else:
                    self.aicpu_ops_info.get(op)[opt_sec].update({opt_subsec: cfg[op][opt]})

    def check_custom_op_info(self, op_name, op_info):
        """
        Check aicpu_cust_kernel.ini op definition
        """
        missing_keys = [k for k in self.required_custom_op_info_keys if k not in op_info]
        if len(missing_keys) > 0:
            print("op: " + op_name + " opInfo missing: " + ",".join(missing_keys))
            raise KeyError("bad key value")

    def check_op_info(self, op_name, op_info):
        """
        Check op info:
        1. if all required section is defined
        2. if defined CUSTAICPUKernel: will do specific check with check_custom_op_info
        3. if defined custom(来自众智）, will copy op define into self.custom_ops_info
        """
        missing_keys = [k for k in self.required_op_info_keys if k not in op_info]
        if len(missing_keys) > 0:
            print("op: " + op_name + " opInfo missing: " + ",".join(missing_keys))
            raise KeyError("bad key value")

        if op_info["opKernelLib"] == "CUSTAICPUKernel":
            self.check_custom_op_info(op_name, op_info)

        if CUSTOM_PREF in op_info:
            custom_set = op_info.get(CUSTOM_PREF)
            # NOTE: do not use bool(xxx) here, bool('False') returns True
            if bool(util.strtobool(custom_set)):
                self.custom_ops_info[op_name] = self.aicpu_ops_info.get(op_name)

    def check_op_input_output(self, io_sec_info):
        """
        Check input/output section for op, if defined other than ('format', 'type', 'name') maybe
        """
        subset = set(io_sec_info.keys()).difference(self.input_output_info_keys)
        return not subset

    def check_op_info_setting(self):
        """
        Check ini op info setting correct or enough
        If custom op found and self.custom_flag not set, will remove these op out from aicpu_ops_info
        """
        print("\n==============check valid for aicpu ops info start==============")
        for op_name, op_info in self.aicpu_ops_info.items():
            op_info_flag = False
            op_io_flag = False
            for op_sec, sec_info in op_info.items():
                if op_sec == "opInfo":
                    self.check_op_info(op_name, sec_info)
                    op_info_flag = True

                elif (
                    (op_sec[:5] == "input" and op_sec[5:].isdigit())
                    or (op_sec[:6] == "output" and op_sec[6:].isdigit())
                    or (op_sec[:13] == "dynamic_input" and op_sec[13:].isdigit())
                    or (op_sec[:14] == "dynamic_output" and op_sec[14:].isdigit())
                ):
                    ret = self.check_op_input_output(sec_info)
                    if not ret:
                        print(
                            "## %s: %s should has format type or name as the key, but getting %s"
                            % (op_name, op_sec, sec_info)
                        )
                        raise KeyError("bad op_sets key")
                    op_io_flag = False

                else:
                    print(
                        "Only opInfo, input[0-9], output[0-9] can be used as a key, "
                        "but op %s has the key %s" % (op_name, op_sec)
                    )
                    raise KeyError("bad key value")
            if not op_info_flag:
                if self.warn_print:
                    print("%s\t## OP %s: defined missing opInfo section %s" % (COLOR_RED, op_name, COLOR_END))
                self.warning_ops["opInfo"].append(op_name)
            if not op_io_flag:
                if self.warn_print:
                    print("%s\t## OP %s: defined missing input/output section %s" % (COLOR_CYAN, op_name, COLOR_END))
                self.warning_ops["io"].append(op_name)
        # if custom flag is set, we will push all custom op in the aicpu_op_info
        # else we will remove them, and push into individual custom json
        if not self.custom_flag:
            for op_name in self.custom_ops_info:
                del self.aicpu_ops_info[op_name]
        print("==============check valid for aicpu ops info end================\n")

    def write(self, json_file_path):
        """
        Write all the data into op json
        """

        def _write(info, file):
            with open(file, "w") as f:
                # Only the owner and group have rights
                os.chmod(file, stat.S_IWGRP + stat.S_IWUSR + stat.S_IRGRP + stat.S_IRUSR)
                json.dump(info, f, sort_keys=True, indent=4, separators=(",", ":"))

        json_file_real_path = os.path.realpath(json_file_path)
        _write(self.aicpu_ops_info, json_file_real_path)
        print(">>>> Found %s AICPU ops, write into: %s" % (len(self.aicpu_ops_info), json_file_real_path))

        if not self.custom_flag:
            file_path, file_name = os.path.split(json_file_real_path)
            custom_file_path = os.path.join(file_path, "%s_custom%s" % os.path.splitext(file_name))
            _write(self.custom_ops_info, custom_file_path)
            print(">>>> Found %s custom AICPU ops, write into: %s" % (len(self.custom_ops_info), custom_file_path))
        else:
            print("### Custom flag is set, all custom ops have been integrated into: %s" % json_file_real_path)

    def parse(self, ini_paths: list, out_file_path, custom=False):
        """
        Total parse function: get info from ini files, write into out_file(in json)
        :param ini_paths: op configuration files
        :param out_file_path: output write path, using json format
        :param custom: if custom True, will merge custom ops into the same json file,
                       if custom False, will split custom ops into the corresponding json
        """
        self.aicpu_ops_info = {}
        self.custom_ops_info = {}
        self.custom_flag = custom
        self.load_ini_info(ini_paths)
        try:
            self.check_op_info_setting()
        except KeyError as e:
            print("bad format key value, failed to generate json file, detail info: \n%s" % e)
        finally:
            self.write(out_file_path)
            if self.warning_ops and self.warn_print:
                print(COLOR_BOLD + "=" * 80 + COLOR_END)
                for warn_type, warn_ops in self.warning_ops.items():
                    print('\tNo "%s" ops set: %s' % (warn_type, warn_ops))
            print("parse try except normal")


def main():
    """A Parser function for ini file."""
    parser = argparse.ArgumentParser(
        prog="Parser_ini.py",
        usage="python3 PATH-TO/parser_ini.py [INI FILES] [OUTPUT_FILE.json] OPERATION",
        description="Parser ini info and check tool",
        add_help=True,
    )
    parser.add_argument("-c", "--custom", action="store_true", help="Custom op compiled in")
    parser.add_argument("FILES", nargs="*", help=argparse.SUPPRESS)
    args = parser.parse_args()
    outfile_path = "tf_kernel.json"
    ini_file_paths = []

    for arg in args.FILES:
        if arg.endswith("ini"):
            ini_file_paths.append(arg)
        elif arg.endswith("json"):
            outfile_path = arg

    if len(ini_file_paths) == 0:
        ini_file_paths.append("tf_kernel.ini")

    ini_parser = IniParser()
    ini_parser.parse(ini_file_paths, outfile_path, custom=args.custom)


if __name__ == "__main__":
    main()
