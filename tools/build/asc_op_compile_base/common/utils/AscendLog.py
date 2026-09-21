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
Function: Objects for Log
"""

import ctypes
import os
import threading


class LogMap:
    """
    Log Map
    """

    class LogMapError(TypeError):
        """
        error for Log Map
        """

    def __setattr__(self: any, name: str, value: any) -> None:
        if name in self.__dict__:
            raise self.LogMapError("Can't rebind (%s)" % name)
        self.__dict__[name] = value

    @staticmethod
    def class_name() -> str:
        """
        class name
        """
        return "LogMap"


class AscendLog:
    """
    Ascend Log Instance
    usage example:
    from te.utils.AscendLog import LOGGER
    LOGGER.set_level(LOGGER.module.cce, LOGGER.level.info,LOGGER.level.event_enable)
    LOGGER.info(LOGGER.module.cce,"this is test .")
    """

    _instance_lock = threading.Lock()

    def __new__(cls: any) -> any:
        if not hasattr(AscendLog, "_instance"):
            with AscendLog._instance_lock:
                if not hasattr(AscendLog, "_instance"):
                    AscendLog._instance = object.__new__(cls)
        return AscendLog._instance

    def __init__(self: any) -> None:
        """
        Initialize AscendLog
        """
        self.log = None
        self.module = LogMap()
        self.module.cce = 8
        self.module.app = 33
        self.module.aicpu = 36
        self.module.mind_data = 40
        self.module.mind_board = 41
        self.module.mind_engine = 42
        self.module.acl = 48
        self.module.tbe = 57
        self.module.fvr = 58
        self.module.tune = 60
        self.level = LogMap()
        self.level.debug = 0
        self.level.info = 1
        self.level.warning = 2
        self.level.error = 3
        self.level.null = 4
        self.level.event = 16
        self.level.event_enable = 1
        self.level.event_disable = 0
        try:
            self.log = ctypes.cdll.LoadLibrary("libunified_dlog.so")
        except OSError:
            ld_path = os.getenv("LD_LIBRARY_PATH")
            if ld_path is None:
                print("[Warning]Can not find libunified_dlog.so")
                return
            path_list = ld_path.split(":")
            for path in path_list:
                target_path = os.path.join(path, "libunified_dlog.so")
                if os.path.isfile(target_path):
                    self.log = ctypes.cdll.LoadLibrary(target_path)
                    break
            if self.log is None:
                print("[Warning]Can not find libunified_dlog.so")
            else:
                print("success to load log")
        finally:
            pass

    def debug(self: any, module: any, fmt: str) -> None:
        """
        print debug log
        :param module: module id, eg: CCE
        :param fmt:log content
        :return: None
        """
        if self.log is None:
            return
        self.log.DlogRecord(ctypes.c_int(module), self.level.debug, ctypes.c_char_p(fmt.encode("utf-8")))

    def info(self: any, module: any, fmt: str) -> None:
        """
        print info log
        :param module: module id, eg: CCE
        :param fmt:log content
        :return: None
        """
        if self.log is None:
            return
        self.log.DlogRecord(ctypes.c_int(module), self.level.info, ctypes.c_char_p(fmt.encode("utf-8")))

    def warn(self: any, module: any, fmt: str) -> None:
        """
        print warn log
        :param module: module id, eg: CCE
        :param fmt:log content
        :return: None
        """
        if self.log is None:
            return
        self.log.DlogRecord(ctypes.c_int(module), self.level.warning, ctypes.c_char_p(fmt.encode("utf-8")))

    def error(self: any, module: any, fmt: str) -> None:
        """
        print error log
        :param module: module id, eg: CCE
        :param fmt:log content
        :return: None
        """
        if self.log is None:
            return
        self.log.DlogRecord(ctypes.c_int(module), self.level.error, ctypes.c_char_p(fmt.encode("utf-8")))

    def event(self: any, module: any, fmt: str) -> None:
        """
        print event log
        :param module: module id, eg: CCE
        :param fmt:log content
        :return: None
        """
        if self.log is None:
            return
        self.log.DlogRecord(ctypes.c_int(module), self.level.event, ctypes.c_char_p(fmt.encode("utf-8")))

    def set_level(self: any, module: any, level: any, event: any) -> None:
        """
        set log level
        :param module: module id, eg: CCE
        :param level: level id, include: debug, info, warning, error, null
        :param event: event switch, enable: event_enable; disable: event_disable;
        :return: None
        """
        if self.log is None:
            return
        self.log.dlog_setlevel(ctypes.c_int(module), ctypes.c_int(level), ctypes.c_int(event))


LOGGER = AscendLog()
