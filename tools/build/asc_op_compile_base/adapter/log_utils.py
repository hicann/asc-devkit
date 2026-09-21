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
AscendC compile log
"""

import re
import os
import stat
import threading
import time
import sys
import inspect
from enum import Enum
from types import MappingProxyType
from datetime import datetime
from .global_storage import global_var_storage
from tbe.common.platform.platform_info import set_current_compile_soc_info
from tbe.tvm.error_mgr import raise_tbe_python_err, TBE_DEFAULT_PYTHON_ERROR_CODE
from tbe.common.repository_manager.utils.repository_manager_log import LOG_INSTANCE


COMPILE_STAGE_MSG_INFO = {
    "compile_op_start": "compile op start",
    "preprocess_start": "preprocess start",
    "preprocess_end": "preprocess end",
    "generate_tiling_start": "generate tiling start",
    "generate_tiling_end": "generate tiling end",
}
COMPILE_STAGE_MSG_INFO = MappingProxyType(COMPILE_STAGE_MSG_INFO)


CompileStage = Enum(
    "CompileStage",
    (
        "PRECOMPILE",
        "INFERCHANNEL",
        "DEBUG_PRECOMPILE",
        "DEBUG_ASSEMBLE",
        "COMPILE",
        "FATBIN",
        "LINKRELOCATE",
        "SPLIT_SUB_OBJS",
        "SPK_INPUT",
        "PACK",
        "UNPACK",
    ),
)


class AscendCLogLevel(Enum):
    LOG_DEBUG = 0
    LOG_INFO = 1
    LOG_WARNING = 2
    LOG_ERROR = 3


LOG_LEVEL_TO_STR = {
    AscendCLogLevel.LOG_DEBUG: "DEBUG",
    AscendCLogLevel.LOG_INFO: "INFO",
    AscendCLogLevel.LOG_WARNING: "WARNING",
    AscendCLogLevel.LOG_ERROR: "ERROR",
}


class LogUtil:
    """
    This class defines some common tool function methods.
    """

    class Option(Enum):
        DEFAULT = 0
        NON_SOC = 1

    def __init__(self):
        pass

    # write the cmpile_cmd to log
    @staticmethod
    def dump_compile_log(compile_cmd, stage: CompileStage, log_file=None):
        if log_file is None or compile_cmd is None:
            return
        flags = os.O_RDWR | os.O_CREAT
        modes = stat.S_IWUSR | stat.S_IRUSR
        try:
            with os.fdopen(os.open(log_file, flags, modes), "a") as f:
                f.write(f"// Stage: {stage}\n")
                f.write(" ".join(str(cmd) for cmd in compile_cmd))
                f.write("\n\n")
        except Exception as err:
            raise_tbe_python_err(TBE_DEFAULT_PYTHON_ERROR_CODE, ("write log failed, reason:", err))

    @staticmethod
    def set_soc_version(soc_version):
        set_current_compile_soc_info(soc_version)

    @staticmethod
    def dump_log(log_str, log_file=None, level="[INFO] : "):
        if log_file is None or log_str is None:
            return
        flags = os.O_RDWR | os.O_CREAT
        modes = stat.S_IWUSR | stat.S_IRUSR
        try:
            with os.fdopen(os.open(log_file, flags, modes), "a") as f:
                f.write("// : log:\n")
                f.write(f"{level} {log_str}")
                f.write("\n\n")
        except Exception as err:
            raise_tbe_python_err(TBE_DEFAULT_PYTHON_ERROR_CODE, ("write log failed, reason:", err))

    # print log with level judge
    @staticmethod
    def print_compile_log(kernel_name: str, msg_info: str, log_level: AscendCLogLevel, option: Option = Option.DEFAULT):
        default_log_level = AscendCLogLevel.LOG_WARNING.value
        plog_switch = os.environ.get("ASCEND_SLOG_PRINT_TO_STDOUT")
        plog_level = os.environ.get("ASCEND_GLOBAL_LOG_LEVEL")
        if plog_level is None and log_level.value < default_log_level:
            return
        if plog_level is not None and log_level.value < int(plog_level):
            return
        LogUtil.plog_print(kernel_name, msg_info, log_level, option)
        if plog_switch is None and log_level.value < default_log_level:
            return
        if plog_switch is not None and int(plog_switch) == 0 and log_level.value < default_log_level:
            return
        LogUtil.log_print(kernel_name, msg_info, log_level, option)
        return

    # print log without level judge
    @staticmethod
    def log_print(kernel_name: str, msg_info: str, log_level: AscendCLogLevel, option: Option = Option.DEFAULT):
        short_soc_version = global_var_storage.get_variable("ascendc_short_soc_version")
        tim_head = datetime.now().strftime("[%Y-%m-%d %H:%M:%S]")
        level_info = " [{}]".format(LOG_LEVEL_TO_STR[log_level])
        log_msg = tim_head + level_info
        if option is not LogUtil.Option.NON_SOC:
            log_msg += " [{}]".format(short_soc_version.lower())
        if kernel_name != "":
            log_msg += " {}".format(kernel_name)
        log_msg += " {}".format(msg_info)
        print(log_msg, flush=True)

    @staticmethod
    def detail_log_print(kernel_name: str, msg_info: str, log_level: AscendCLogLevel, option: Option = Option.DEFAULT):
        plog_switch = os.environ.get("ASCEND_GLOBAL_EVENT_ENABLE")
        if plog_switch is not None and int(plog_switch) == 1:
            logpid = os.getpid()
            python_exe = os.path.basename(sys.executable)
            thread_id = threading.currentThread().ident
            current_time = time.strftime("%Y-%m-%d-%H:%M:%S")
            microsecond = int(time.time_ns() % 1_000_000_000)
            milliseconds = microsecond // 1_000_000
            microseconds = (microsecond % 1_000_000) // 1000
            tim_head = f" {current_time}.{milliseconds:03d}.{microseconds:03d}"
            nanoseconds = time.monotonic_ns()
            level_info = "[INFO] "
            log_msg = f"{level_info}ASC({logpid}, {python_exe}):{tim_head}"
            frame = inspect.currentframe()
            caller_frame = frame.f_back
            if caller_frame:
                filename = os.path.basename(caller_frame.f_code.co_filename)
                line_number = caller_frame.f_lineno
                file_line = f"[{filename}:{line_number}]"
            else:
                file_line = "[unknown:0]"
            del frame

            log_msg += " {} [tid: {}] {}".format(file_line, thread_id, msg_info)
            log_msg += " , timestamp: {}ns".format(nanoseconds)
            print(log_msg, flush=True)

    @staticmethod
    def plog_print(kernel_name: str, msg_info: str, log_level: AscendCLogLevel, option: Option = Option.DEFAULT):
        # plog print
        short_soc_version = global_var_storage.get_variable("ascendc_short_soc_version")
        plog_log_msg = "[AscendCCompiler] "
        if option is not LogUtil.Option.NON_SOC:
            plog_log_msg = "[{}] ".format(short_soc_version.lower())
        if kernel_name != "":
            plog_log_msg += " {} ".format(kernel_name)
        plog_log_msg += msg_info
        plog_log_msg = LogUtil.fix_string_escapes(plog_log_msg)
        if log_level == AscendCLogLevel.LOG_DEBUG:
            LOG_INSTANCE.debug(plog_log_msg)
        elif log_level == AscendCLogLevel.LOG_INFO:
            LOG_INSTANCE.info(plog_log_msg)
        elif log_level == AscendCLogLevel.LOG_WARNING:
            LOG_INSTANCE.warn(plog_log_msg)
        elif log_level == AscendCLogLevel.LOG_ERROR:
            LOG_INSTANCE.error(plog_log_msg)

    @staticmethod
    def fix_string_escapes(log_message: str) -> str:
        common_escapes = {0: r"\0", 7: r"\a", 8: r"\b", 9: r"\t", 10: r"\n", 11: r"\v", 12: r"\f", 13: r"\r", 27: r"\e"}

        # process control symbols
        def escape_control_symbols(match):
            chara = match.group(0)
            code = ord(chara)
            reserve_codes = {9, 10, 13}  # \t, \n, \r
            if code in reserve_codes:
                return chara
            elif code in common_escapes:
                return common_escapes[code]
            elif 0 <= code <= 31 or code == 127:
                return f"\\x{code:02x}"
            return chara

        control_pattern = re.compile(r"[\x00-\x1f\x7f]")
        result = control_pattern.sub(escape_control_symbols, log_message)

        # process backslashes except for control symbols
        def escape_backslash(match):
            return "\\\\"

        backslash_pattern = re.compile(r"(?<!\\)\\(?![ntr0abefv]|x[0-9a-fA-F]|\\)")
        result = backslash_pattern.sub(escape_backslash, result)

        # process format symbols
        result = result.replace("%", "%%")
        return result
