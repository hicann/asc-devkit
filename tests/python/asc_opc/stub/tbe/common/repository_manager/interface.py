#! /usr/bin/env python3
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
"""
Copyright (C) 2022. Huawei Technologies Co., Ltd. All rights reserved.
External interfaces of the cann knowledege bank manager.
"""

__all__ = ["cann_kb_search"]


def cann_kb_search(info_dict: str, search_config: dict, option: dict = {}) -> list:
    """Cann Knowledge Search

    Parameters:
    info_dict(str): Operator Info To Search Knowledge
    search_config(dict): Additional Configuration Items for Knowledge Search
    option(dict): Reserved Configuration Item

    Return: Knowledge List
    """
    results_json = []
    knowledge_list = {"knowledge": {"dynamic_compile_static": "true", "op_impl_switch": "dsl"}}
    results_json.append(knowledge_list)
    return results_json
