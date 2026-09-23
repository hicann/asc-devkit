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

from mdparser.parser import _extract_math_blocks, parse_string  # noqa: E402


class MarkdownMathTests(unittest.TestCase):
    def test_shell_variables_do_not_escape_html_anchor_tags(self):
        markdown = (
            '<p>具体定义请参考<span id="ph105"><a name="ph105"></a>${INSTALL_DIR}</span>'
            "/asc/include/basic_api/kernel_struct_proposal.h，"
            '<span id="ph143"><a name="ph143"></a>${INSTALL_DIR}</span>请替换。</p>'
        )

        html = parse_string(markdown)

        self.assertIn('<span id="ph105"><a name="ph105"></a>${INSTALL_DIR}</span>', html)
        self.assertIn('<span id="ph143"><a name="ph143"></a>${INSTALL_DIR}</span>', html)
        self.assertNotIn("&lt;span", html)
        self.assertNotIn("&lt;/span&gt;", html)

    def test_shell_variables_are_not_treated_as_inline_math(self):
        text, blocks = _extract_math_blocks("${A} / ${B}")

        self.assertEqual(text, "${A} / ${B}")
        self.assertEqual(blocks, [])

    def test_inline_math_is_still_extracted(self):
        text, blocks = _extract_math_blocks("$x+y$")

        self.assertEqual(text, "@@MATH0@@")
        self.assertEqual(blocks, ["$x+y$"])

    def test_block_math_is_still_extracted(self):
        text, blocks = _extract_math_blocks("$$x+y$$")

        self.assertEqual(text, "@@MATH0@@")
        self.assertEqual(blocks, ["$$x+y$$"])

    def test_escaped_dollars_and_code_are_unchanged(self):
        markdown = "\\$x+y\\$\n\n```text\n$x+y$ ${VAR}\n```\n\n`$z$`"
        text, blocks = _extract_math_blocks(markdown)

        self.assertEqual(blocks, [])
        self.assertIn("\\$x+y\\$", text)
        self.assertIn("$x+y$ ${VAR}", text)
        self.assertIn("`$z$`", text)


if __name__ == "__main__":
    unittest.main()
