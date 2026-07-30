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
import tempfile
import unittest

SCRIPTS_DIR = pathlib.Path(__file__).resolve().parents[1]
sys.path.insert(0, str(SCRIPTS_DIR))

from mdparser.parser import parse_file, parse_string  # noqa: E402


class MarkdownLinkTests(unittest.TestCase):
    def test_parse_file_only_converts_documentation_markdown_links(self):
        with tempfile.TemporaryDirectory() as temp_dir:
            repo = pathlib.Path(temp_dir)
            source = repo / "docs" / "zh" / "guide" / "page.md"
            source.parent.mkdir(parents=True)
            source.write_text(
                "[internal](./other.md)\n"
                "[example](../../../examples/sample/README.md)\n"
                "[root](../../../CONTRIBUTING.md)\n",
                encoding="utf-8",
            )

            html = parse_file(str(source))

            self.assertIn('href="./other.html"', html)
            self.assertIn('href="../../../examples/sample/README.md"', html)
            self.assertIn('href="../../../CONTRIBUTING.md"', html)

    def test_english_document_preserves_repository_markdown_links(self):
        with tempfile.TemporaryDirectory() as temp_dir:
            repo = pathlib.Path(temp_dir)
            source = repo / "docs" / "en" / "page.md"
            source.parent.mkdir(parents=True)
            source.write_text(
                "[internal](./other.md)\n[root](../../CONTRIBUTING_en.md)\n",
                encoding="utf-8",
            )

            html = parse_file(str(source))

            self.assertIn('href="./other.html"', html)
            self.assertIn('href="../../CONTRIBUTING_en.md"', html)

    def test_parse_string_without_source_path_keeps_existing_behavior(self):
        html = parse_string("[document](../document.md)")
        self.assertIn('href="../document.html"', html)


if __name__ == "__main__":
    unittest.main()
