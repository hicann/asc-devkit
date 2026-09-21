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
This module provides the functions about build_config.
"""

import warnings
import contextvars

from .buildcfg_mapping import (
    dynamic_shape,
    disable_vectorize,
    instrument_bound_checkers,
    ub_fusion_tensors,
    ub_fusion_buffers,
)
from .default_buildcfg import cce_default_static_build_config
from .global_info import GlobalInfoContainer

_build_cfg = contextvars.ContextVar("_build_cfg", default=cce_default_static_build_config.copy())


def _check_kwargs(kwargs):
    if "tbe_debug_level" in kwargs.keys():
        if kwargs["tbe_debug_level"] not in (0, 1, 2):
            raise (
                Exception(
                    "Unsupported tbe_debug_level: %s, it must be \
                               one of (0, 1, 2) and the data type \
                               must be int "
                    % kwargs["tbe_debug_level"]
                )
            )

    if "vector_fp_ceiling" in kwargs.keys():
        if kwargs["vector_fp_ceiling"] not in (0, 1, 2):
            raise (
                Exception(
                    "Unsupported vector_fp_ceiling. it must be \
                               one of (0, 1, 2) and the data type \
                               must be int."
                )
            )

    if "compatible" in kwargs:
        if not isinstance(kwargs["compatible"], bool):
            raise ("'%s' should be a boolean variable" % (kwargs["compatible"]))


def _update_config_dict(kwargs, config_dict):
    """Use kwargs to update current build config dict."""
    for key in kwargs.keys():
        if key == "dynamic_shape":
            new_key = dynamic_shape
        elif "tir." in key:
            new_key = key
        else:
            new_key = "tir." + key
        if new_key in config_dict:
            config_dict[new_key] = kwargs[key]
            if key in GlobalInfoContainer.global_info:
                GlobalInfoContainer.global_info[key] = kwargs[key]
        elif key != "compatible":
            warnings.warn("build_config cannot set attribute '%s'" % key)

    """initialize config which do not expose to user"""
    config_dict[disable_vectorize] = False
    config_dict[instrument_bound_checkers] = False
    config_dict[ub_fusion_tensors] = []
    config_dict[ub_fusion_buffers] = []


class BuildConfigWrapper(object):
    def __init__(self, config_dict):
        self.config_dict = config_dict

    def __enter__(self):
        self.token = _build_cfg.set(self.config_dict)
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        _build_cfg.reset(self.token)

    def current(self):
        return _build_cfg.get()


def build_config(**kwargs):
    """Configure the build behavior by setting config variables.

    Parameters
    ----------
    compatible: bool, default=False
        If compatible = True, update build_config in cce_build.py

    Returns
    -------
    pass context: AscendPassContext
        The build configuration
    """
    _check_kwargs(kwargs)
    config_dict = _build_cfg.get().copy()
    _update_config_dict(kwargs, config_dict)
    return BuildConfigWrapper(config_dict)


def _get_config_dict_without_tir(config_dict):
    res = {}
    for key in config_dict:
        value = config_dict.get(key)

        if key.startswith("tir."):
            res[key[len("tir.") :]] = value

    return res


def get_config_value(key, config_dict):
    if key == "dynamic_shape":
        return config_dict.get(dynamic_shape)
    elif key in config_dict:
        return config_dict.get(key)
    elif ("tir." + key) in config_dict:
        return config_dict.get("tir." + key)
    elif key == "all":
        return _get_config_dict_without_tir(config_dict)
    return None


def get_current_build_config(key="all"):
    """Get the current build configuration."""
    cur_conf = _build_cfg.get()
    return get_config_value(key, cur_conf)


def get_default_build_config(key="all"):
    """Get the default build configuration."""
    config_dict = cce_default_static_build_config
    return get_config_value(key, config_dict)


def set_current_build_config(key, value):
    """set cce build config just for compatible with old ops."""
    if key == "dynamic_shape":
        _build_cfg.get()[dynamic_shape] = value
    elif key in _build_cfg.get():
        _build_cfg.get()[key] = value
    elif ("tir." + key) in _build_cfg.get():
        _build_cfg.get()["tir." + key] = value

    if key in GlobalInfoContainer.global_info:
        GlobalInfoContainer.global_info[key] = value
