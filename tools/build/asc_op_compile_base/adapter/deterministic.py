#!/usr/bin/python
# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

"""Deterministic metadata shared by offline template compilation."""


def parse_levels(value):
    """None denotes the historical boolean-only selection path."""
    if value is None or value == "" or value == -1 or value == "-1":
        return None
    values = value if isinstance(value, (list, tuple)) else str(value).split(",")
    if not values or any(str(item).strip() not in {"0", "1", "2", "3"} for item in values):
        raise ValueError("deterministic levels must contain only 0, 1, 2, 3")
    levels = tuple(int(item) for item in values)
    if len(set(levels)) != len(levels):
        raise ValueError("duplicate deterministic levels")
    if 0 in levels and len(levels) != 1:
        raise ValueError("level 0 must be compiled separately from deterministic levels")
    return levels


def normalize_deterministic(deterministic=None, levels=None):
    """Complete false/0 and true/1 defaults, rejecting contradictory metadata."""
    levels = parse_levels(levels)
    if deterministic is not None and str(deterministic).lower() not in {"true", "false", "0", "1"}:
        raise ValueError("deterministic must be true or false")
    flag = None if deterministic is None else str(deterministic).lower() in {"true", "1"}
    if levels is None:
        return bool(flag), (1,) if flag else (0,)
    inferred_flag = levels != (0,)
    if flag is not None and flag != inferred_flag:
        raise ValueError("deterministic and deterministic levels conflict")
    return inferred_flag, levels


def level_simplified_keys(keys, levels):
    """One binary may advertise multiple dt keys; d remains the boolean field."""
    levels = parse_levels(levels)
    if levels is None:
        return keys
    result = []
    for key in keys:
        op_type, arguments, tensors = key.split("/", 2)
        boolean_fields = [item for item in arguments.split(",") if item.startswith("d=")]
        expected = "d=0" if levels == (0,) else "d=1"
        if boolean_fields != [expected]:
            raise ValueError("simplified key boolean deterministic field conflicts with levels")
        if any(item.startswith("dt=") for item in arguments.split(",")):
            raise ValueError("simplified key already contains deterministic level")
        for level in levels:
            fields = [f"d={int(level != 0)}" if item.startswith("d=") else item for item in arguments.split(",")]
            result.append(f"{op_type}/{','.join(fields)},dt={level}/{tensors}")
    return result


def offline_selection(legacy_context, legacy_flag):
    """Modern asc_opc can call an adapter wrapper; prefer its explicit request.

    Legacy TBE invocations retain their own context and build configuration.
    An implicit modern runtime level (normally zero) never enables this bridge.
    """
    selected = parse_levels(legacy_context.get_addition("deterministic_level_filter") if legacy_context else None)
    if selected is not None:
        return selected, bool(legacy_flag)
    try:
        from asc_op_compile_base.common.context import get_context
        from asc_op_compile_base.common.buildcfg import get_current_build_config
    except ImportError:
        return None, bool(legacy_flag)
    context = get_context()
    selected = parse_levels(context.get_addition("deterministic_level_filter") if context else None)
    if selected is not None:
        return selected, bool(get_current_build_config("enable_deterministic_mode"))
    return None, bool(legacy_flag)


def get_level_discovery_context(context=None):
    """Resolve discovery through the same adapter/modern context as task control."""
    context = deterministic_request_context(context)
    return context if context is not None and context.get_addition("deterministic_discovery_only") else None


def deterministic_request_context(context=None):
    """Prefer an explicit adapter request, then modern asc_opc's active context."""
    if context is None:
        from tbe.common.context import get_context

        context = get_context()
    if context is not None and (
        context.get_addition("deterministic_declared_legacy")
        or context.get_addition("deterministic_discovery_only")
        or parse_levels(context.get_addition("deterministic_level_filter")) is not None
    ):
        return context
    try:
        from asc_op_compile_base.common.context import get_context
    except ImportError:
        return context
    modern = get_context()
    if modern is not None and (
        modern.get_addition("deterministic_declared_legacy")
        or modern.get_addition("deterministic_discovery_only")
        or parse_levels(modern.get_addition("deterministic_level_filter")) is not None
    ):
        return modern
    return context


def deterministic_level_request(context=None):
    """Return context, whether probing is needed, and an explicit grade filter."""
    context = deterministic_request_context(context)
    addition = context.get_addition if context is not None else lambda _: None
    levels = parse_levels(addition("deterministic_level_filter"))
    requested = (
        addition("deterministic_declared_legacy") or addition("deterministic_discovery_only") or levels is not None
    )
    return context, bool(requested), levels


def deterministic_compile_options(compile_options):
    """Remove task switches before probing; never advertise unverified SDK support.

    ENABLE is added only by KernelInfoInfer after real selected-header
    preprocessing. Legacy/boolean calls therefore cannot inherit a grade task's
    ENABLE from a previous task.
    """
    managed_macro = "ASCENDC_TPL_ENABLE_DETERMINISTIC_LEVEL"
    options = list(compile_options)
    result = []
    index = 0
    while index < len(options):
        option = options[index]
        if option in {"-D", "-U"} and index + 1 < len(options):
            name = options[index + 1].split("=", 1)[0].strip()
            if name == managed_macro:
                index += 2
                continue
        elif option.startswith(("-D", "-U")):
            name = option[2:].split("=", 1)[0].strip()
            if name == managed_macro:
                index += 1
                continue
        result.append(option)
        index += 1
    return result
