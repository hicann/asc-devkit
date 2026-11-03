# -----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# -----------------------------------------------------------------------------------------------------------

"""Mirror checks reject missing checkouts and accidental self-comparison."""

import contextlib
import io
import json
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "scripts"))
import check_mirrors


class MirrorTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.source = self.root / "devkit"
        self.target = self.root / "knowledge"
        for checkout, prefix in ((self.source, ".agent"), (self.target, ".agents")):
            skill = checkout / prefix / "skills/asc-search"
            skill.mkdir(parents=True)
            (skill / "SKILL.md").write_text("---\nname: asc-search\n---\n")
            (skill / "script.py").write_text("content")

    def check(self):
        with mock.patch.object(
            sys, "argv", ["check_mirrors", "--source", str(self.source), "--target", str(self.target)]
        ), contextlib.redirect_stdout(io.StringIO()) as out:
            code = check_mirrors.main()
        return code, json.loads(out.getvalue())

    def test_identical_and_different_files(self):
        self.assertEqual(self.check()[0], 0)
        path = self.target / ".agents/skills/asc-search/script.py"
        path.write_text("different")
        code, payload = self.check()
        self.assertEqual(code, 1)
        self.assertEqual(payload["differences"], {"asc-search": ["script.py"]})

    def test_missing_checkouts_or_manifest_are_errors(self):
        for root in (self.source, self.target):
            with self.subTest(root=root):
                prefix = ".agent" if root == self.source else ".agents"
                manifest = root / prefix / "skills/asc-search/SKILL.md"
                data = manifest.read_bytes()
                manifest.unlink()
                self.assertEqual(self.check()[0], 2)
                manifest.write_bytes(data)
        shutil.rmtree(self.source)
        shutil.rmtree(self.target)
        self.assertEqual(self.check()[0], 2)

    def test_same_resolved_skill_directory_is_rejected(self):
        shutil.rmtree(self.target)
        self.target.mkdir()
        (self.target / ".agents").symlink_to(self.source / ".agent", target_is_directory=True)
        code, payload = self.check()
        self.assertEqual(code, 2)
        self.assertIn("different skill directories", payload["error"])

    def test_knowledge_copy_requires_an_explicit_devkit_source(self):
        script = self.target / ".agents/skills/asc-search/scripts/check_mirrors.py"
        script.parent.mkdir()
        shutil.copyfile(check_mirrors.__file__, script)
        command = [sys.executable, str(script), "--target", str(self.target)]
        result = subprocess.run(command, capture_output=True, text=True)
        self.assertEqual(result.returncode, 2)
        result = subprocess.run(command + ["--source", str(self.source)], capture_output=True, text=True)
        self.assertEqual(result.returncode, 1)  # The fixture target contains this extra script.
