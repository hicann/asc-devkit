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
super kernel option_parse
"""

from abc import ABC, abstractmethod
from tbe.common.context import get_context
from .ascendc_compile_base import CommonUtility, AscendCLogLevel
from .super_kernel_constants import (
    SuperKernelPreLoadMode,
    SuperKernelEarlyStartMode,
    SuperKernelDebugDcciAllMode,
    SuperKernelDebugSyncAllMode,
    SuperKernelStreamFusionMode,
    SuperKernelFeedSyncAllMode,
    SuperKernelProfilingMode,
    ERR_CODE,
)


class OptionParser(ABC):
    """base class of option parse_option"""

    @abstractmethod
    def parse_option(self, value: str) -> bool:
        pass


class ParserFactory:
    """factory class of option parse_option"""

    def __init__(self):
        self._parsers = {}

    def register(self, parser: OptionParser):
        self._parsers[parser.key] = parser

    def get_parses(self) -> dict:
        return self._parsers

    def get_parse_func(self, key: str) -> OptionParser:
        return self._parsers.get(key)


class CodeTextAlignParser(OptionParser):
    def __init__(self, key_value: str):
        self.key = key_value

    def parse_option(self, value: str) -> bool:
        if not value.isdigit():
            CommonUtility().ascendc_raise_python_err(
                ERR_CODE,
                f"[Super Kernel] Invalid compile option: {self.key} option should be a digit, {value} is invalid.",
            )
        number = int(value)
        if number < 0 or number == 1 or (number & (number - 1)) != 0:
            CommonUtility().ascendc_raise_python_err(
                ERR_CODE,
                f"[Super Kernel] Invalid compile option: \
{self.key} option should be [0, 2, 4, 8, ...], {number} is invalid.",
            )
        return number


class EnumParser(OptionParser):
    def __init__(self, key_value: str, allowed_values: dict):
        self.key = key_value
        self.allowed = allowed_values

    def parse_option(self, value: str):
        if value not in self.allowed:
            CommonUtility().ascendc_raise_python_err(
                ERR_CODE,
                f"[Super Kernel] Invalid compile option: {self.key} option should be one of {self.allowed.keys()}, {value} is invalid.",
            )
        return self.allowed[value]


class BinaryParser(OptionParser):
    def __init__(self, key_value: str):
        self.key = key_value

    def parse_option(self, value: str):
        if value not in {"0", "1"}:
            CommonUtility().ascendc_raise_python_err(
                ERR_CODE,
                f"[Super Kernel] Invalid compile option: {self.key} option should be 0 or 1, {value} is invalid.",
            )
        return value


class NumberParser(OptionParser):
    def __init__(self, key_value: str):
        self.key = key_value

    def parse_option(self, value: str):
        if not value.isdigit():
            CommonUtility().ascendc_raise_python_err(
                ERR_CODE,
                f"[Super Kernel] Invalid compile option: {self.key} option should be a digit, {value} is invalid.",
            )
        number = int(value)
        if number <= 0 or number > 64:
            CommonUtility().ascendc_raise_python_err(
                ERR_CODE,
                f"[Super Kernel] Invalid compile option: {self.key} option should between (0, 64], {number} is invalid.",
            )
        return number


class NonEmptyParser(OptionParser):
    def __init__(self, key_value: str):
        self.key = key_value

    def parse_option(self, value: str):
        if len(value.strip()) <= 0:
            CommonUtility().ascendc_raise_python_err(
                ERR_CODE,
                f"[Super Kernel] Invalid compile option: {self.key} option should not be empty.",
            )
        return value


class BlockNumParser(OptionParser):
    def __init__(self, key_value: str):
        self.key = key_value

    def parse_option(self, value: str):
        if not value.isdigit():
            CommonUtility().ascendc_raise_python_err(
                ERR_CODE,
                f"[Super Kernel] Invalid compile option: {self.key} option should be a digit, {value} is invalid.",
            )
        number = int(value)
        if number < 0:
            CommonUtility().ascendc_raise_python_err(
                ERR_CODE,
                f"[Super Kernel] Invalid compile option: {self.key} option should be non-negative integer, {number} is invalid.",
            )
        return number


def setup_super_kernel_option_parsers_ge() -> ParserFactory:
    """init super kernel_validate for ge backend"""
    factory = ParserFactory()

    # register validation
    factory.register(CodeTextAlignParser("func-align"))
    factory.register(
        EnumParser(
            "preload-code",
            {
                "max": SuperKernelPreLoadMode.PreLoadByWhole,
                "none": SuperKernelPreLoadMode.PreloadNA,
                "per-func": SuperKernelPreLoadMode.PreloadByAdanvanceStep,
            },
        )
    )
    factory.register(
        EnumParser(
            "early-start",
            {
                "0": SuperKernelEarlyStartMode.EarlyStartDisable,
                "1": SuperKernelEarlyStartMode.EarlyStartEnableV2,
                "2": SuperKernelEarlyStartMode.EarlyStartV2DisableSubKernel,
            },
        )
    )
    factory.register(
        EnumParser(
            "stream-fusion",
            {
                "0": SuperKernelStreamFusionMode.StreamFusionDisable,
                "1": SuperKernelStreamFusionMode.StreamFusionEnable,
            },
        )
    )
    factory.register(
        EnumParser(
            "debug-dcci-all",
            {
                "0": SuperKernelDebugDcciAllMode.DebugDcciAllDisable,
                "1": SuperKernelDebugDcciAllMode.DebugDcciAllEnable,
            },
        )
    )
    factory.register(
        EnumParser(
            "debug-sync-all",
            {
                "0": SuperKernelDebugSyncAllMode.DebugSyncAllDisable,
                "1": SuperKernelDebugSyncAllMode.DebugSyncAllEnable,
            },
        )
    )

    factory.register(
        EnumParser(
            "feed-sync-all",
            {
                "0": SuperKernelFeedSyncAllMode.FeedSyncAllDisable,
                "1": SuperKernelFeedSyncAllMode.FeedSyncAllEnable,
            },
        )
    )
    factory.register(
        EnumParser(
            "profiling",
            {
                "0": SuperKernelProfilingMode.ProfilingDisable,
                "1": SuperKernelProfilingMode.ProfilingEnable,
            },
        )
    )
    factory.register(NonEmptyParser("compile-options"))
    factory.register(NonEmptyParser("strict-scope-check"))
    factory.register(NonEmptyParser("dcci-before-kernel-start"))
    factory.register(NonEmptyParser("dcci-after-kernel-end"))
    factory.register(NonEmptyParser("dcci-disable-on-kernel"))
    factory.register(NumberParser("split-mode"))
    factory.register(BlockNumParser("debug-aic-num"))
    factory.register(BlockNumParser("debug-aiv-num"))

    return factory


def setup_super_kernel_option_parsers_aclgraph() -> ParserFactory:
    """init super kernel_validate for aclgraph backend"""
    factory = ParserFactory()

    # register validation
    factory.register(BinaryParser("early-start"))
    factory.register(BinaryParser("debug-sync-all"))
    factory.register(BinaryParser("debug-per-op-max-core-num"))
    factory.register(NonEmptyParser("dcci-before-kernel-start"))
    factory.register(NonEmptyParser("dcci-after-kernel-end"))
    factory.register(NonEmptyParser("dcci-disable-on-kernel"))

    return factory


def parse_super_kernel_options(option_string: str) -> bool:
    context = get_context()
    is_aclgraph = (
        context.get_addition("super_kernel_sub_combine") is True if context else False
    )
    if is_aclgraph:
        factory = setup_super_kernel_option_parsers_aclgraph()
    else:
        factory = setup_super_kernel_option_parsers_ge()
    if not option_string or not option_string.strip():
        return {}
    # Strip leading and trailing quotes, which may be introduced by json.dumps
    option_string = option_string.strip('"')
    pairs = [
        part_option.strip()
        for part_option in option_string.split(":")
        if part_option.strip()
    ]
    result_options = {}
    for pair in pairs:
        if "=" not in pair:
            CommonUtility.print_compile_log(
                "",
                f"Invalid compile option: {pair} do not match option={{value}}.",
                AscendCLogLevel.LOG_WARNING,
            )
            continue
        key, value = map(str.strip, pair.split("=", 1))
        key = key.replace("_", "-") if is_aclgraph else key

        if is_aclgraph and key not in factory.get_parses():
            CommonUtility.print_compile_log(
                "",
                f"Unsupported compile option for aclgraph backend: {key} in sub op compile.",
                AscendCLogLevel.LOG_INFO,
            )
            continue

        if not key or not value:
            CommonUtility().ascendc_raise_python_err(
                ERR_CODE,
                f"[Super Kernel] Invalid compile option: The key-value pair is missing for the option {pair}.",
            )
        if key in result_options:
            CommonUtility().ascendc_raise_python_err(
                ERR_CODE,
                f"[Super Kernel] Invalid compile option: {key} option has been set.",
            )
        parser = factory.get_parse_func(key)
        if not parser:
            CommonUtility().ascendc_raise_python_err(
                ERR_CODE,
                f"[Super Kernel] Invalid compile option: {key} option is not supported.",
            )
        result_options[key] = parser.parse_option(value)
    return result_options
