#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

import pathlib
import sys
import unittest

SCRIPTS_DIR = pathlib.Path(__file__).resolve().parents[1]
sys.path.insert(0, str(SCRIPTS_DIR))

from mdparser.parser import parse_string  # noqa: E402

OKF_HEADER = (
    "---\n"
    "okftype: reference.api\n"
    'description: "更新地址寄存器addr_reg以支持1至4维迭代寻址。"\n'
    "tag:\n"
    "  - roles: [data_movement_configuration]\n"
    'version: ">=9.2.0"\n'
    "---\n"
)


class FrontMatterTests(unittest.TestCase):
    def test_okf_front_matter_is_stripped(self):
        html = parse_string(OKF_HEADER + "# 基础定义\n")

        self.assertNotIn("okftype", html)
        self.assertNotIn("reference.api", html)
        self.assertNotIn("<hr", html)
        self.assertIn("<h1>基础定义</h1>", html)

    def test_crlf_front_matter_is_stripped(self):
        source = OKF_HEADER.replace("\n", "\r\n") + "# 标题\r\n"

        html = parse_string(source)

        self.assertNotIn("okftype", html)
        self.assertIn("<h1>标题</h1>", html)

    def test_bom_and_dots_terminator_are_supported(self):
        source = "\ufeff---\nokftype: reference.api\n...\n# 标题\n"

        html = parse_string(source)

        self.assertNotIn("okftype", html)
        self.assertIn("<h1>标题</h1>", html)

    def test_unclosed_front_matter_is_kept(self):
        source = "---\nokftype: reference.api\n# 标题\n"

        html = parse_string(source)

        self.assertIn("okftype", html)

    def test_leading_horizontal_rule_is_not_treated_as_front_matter(self):
        source = "---\n\n正文内容\n\n---\n"

        html = parse_string(source)

        self.assertIn("<hr", html)
        self.assertIn("正文内容", html)

    def test_document_without_front_matter_is_unchanged(self):
        html = parse_string("# 标题\n\n正文内容\n")

        self.assertIn("<h1>标题</h1>", html)
        self.assertIn("正文内容", html)


if __name__ == "__main__":
    unittest.main()
