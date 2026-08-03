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


class MarkdownTildeTests(unittest.TestCase):
    def test_plain_text_lone_tildes_are_not_strikethrough(self):
        html = parse_string("1~2 and 3~4")

        self.assertIn("1~2 and 3~4", html)
        self.assertNotIn("<del>", html)
        self.assertNotIn(r"\~", html)

    def test_standard_strikethrough_is_preserved(self):
        html = parse_string("~~old~~")

        self.assertIn("<del>old</del>", html)

    def test_markdown_table_keeps_literal_tilde(self):
        html = parse_string("| range |\n| --- |\n| 0~12 |")

        self.assertIn("<td>0~12</td>", html)
        self.assertNotIn(r"\~", html)

    def test_indented_html_table_keeps_literal_tildes(self):
        markdown = (
            "- item\n\n"
            '    <a name="table45836437171"></a>\n'
            "    <table><tbody><tr><td><ul>"
            "<li>1025~2048 threads</li>"
            "<li>513~1024 threads</li>"
            "</ul></td></tr></tbody></table>\n\n"
            "after 1~2 and 3~4\n"
        )

        html = parse_string(markdown)

        self.assertIn("1025~2048 threads", html)
        self.assertIn("513~1024 threads", html)
        self.assertNotIn(r"1025\~2048", html)
        self.assertNotIn(r"513\~1024", html)
        self.assertIn("after 1~2 and 3~4", html)
        self.assertNotIn("<del>2 and 3</del>", html)

    def test_html_attribute_tilde_is_unchanged(self):
        html = parse_string('<div data-range="1~2">3~4</div>\n')

        self.assertIn('data-range="1~2"', html)
        self.assertIn(">3~4</div>", html)
        self.assertNotIn(r"\~", html)

    def test_inline_html_text_still_gets_lone_tilde_protection(self):
        html = parse_string("<span>1~2 and 3~4</span>")

        self.assertIn("<span>1~2 and 3~4</span>", html)
        self.assertNotIn("<del>", html)

    def test_inline_html_strikethrough_is_preserved(self):
        html = parse_string("<span>~~old~~</span>")

        self.assertIn("<span><del>old</del></span>", html)

    def test_html_block_does_not_enable_markdown_strikethrough(self):
        html = parse_string("<div>\n~~old~~ and 1~2\n</div>\n")

        self.assertIn("~~old~~ and 1~2", html)
        self.assertNotIn("<del>", html)
        self.assertNotIn(r"\~", html)

    def test_fenced_and_inline_code_tildes_are_unchanged(self):
        html = parse_string("```text\n1~2\n```\n\n`3~4`")

        self.assertIn("1~2", html)
        self.assertIn("3~4", html)
        self.assertNotIn(r"\~", html)

    def test_special_html_blocks_keep_literal_tildes(self):
        cases = (
            "<!-- 1~2 -->\n",
            "<?instruction 1~2?>\n",
            "<!DOCTYPE 1~2>\n",
            "<![CDATA[1~2]]>\n",
            "<style>\n.range-1~2 {}\n</style>\n",
            "<pre>\n1~2\n</pre>\n",
        )

        for markdown in cases:
            with self.subTest(markdown=markdown):
                html = parse_string(markdown)
                self.assertIn("1~2", html)
                self.assertNotIn(r"\~", html)


if __name__ == "__main__":
    unittest.main()
