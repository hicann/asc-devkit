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


class MarkdownMermaidTests(unittest.TestCase):
    def test_mermaid_fence_becomes_diagram_container(self):
        markdown = '```mermaid\nflowchart LR\n    A["<start>"] --> B & C\n```'

        html = parse_string(markdown)

        self.assertIn('<div class="mermaid-diagram" data-mermaid-diagram>', html)
        self.assertIn('<pre class="mermaid">flowchart LR', html)
        self.assertIn("A[&quot;&lt;start&gt;&quot;] --&gt; B &amp; C", html)
        self.assertNotIn('<div class="code-block">', html)

    def test_mermaid_language_matching_is_case_insensitive(self):
        html = parse_string("~~~Mermaid\ngraph TD\n    A --> B\n~~~")

        self.assertIn("data-mermaid-diagram", html)
        self.assertIn('<pre class="mermaid">graph TD', html)

    def test_non_mermaid_fence_keeps_existing_highlighting(self):
        html = parse_string("```cpp\nint main() { return 0; }\n```")

        self.assertIn('<div class="code-block">', html)
        self.assertIn('<div class="code-header">', html)
        self.assertNotIn("data-mermaid-diagram", html)


if __name__ == "__main__":
    unittest.main()
