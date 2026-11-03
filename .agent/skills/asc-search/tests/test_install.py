# -----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# -----------------------------------------------------------------------------------------------------------

"""Installation behavior with isolated destinations, never real user directories."""

from __future__ import annotations

import contextlib
import io
import os
import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "scripts"))
import install


class InstallTests(unittest.TestCase):
    def setUp(self) -> None:
        self.tmp = tempfile.TemporaryDirectory(prefix="asc-search install ")
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)
        self.source = self.root / "devkit/.agent/skills/asc-search"
        self.source.mkdir(parents=True)
        (self.source / "SKILL.md").write_text("---\nname: asc-search\n---\n")
        self.targets = {agent: self.root / agent / "skills" for agent in install.AGENTS}
        self.stdout = contextlib.redirect_stdout(io.StringIO())
        self.stdout.__enter__()
        self.addCleanup(self.stdout.__exit__, None, None, None)

    def test_links_keep_source_and_repeat_without_changes(self) -> None:
        install.install(self.source, self.targets)
        links = [directory / "asc-search" for directory in self.targets.values()]
        for link in links:
            self.assertTrue(link.is_symlink())
            self.assertEqual(link.resolve(), self.source.resolve())
            self.assertTrue((link / "SKILL.md").is_file())
        stats = [link.lstat().st_mtime_ns for link in links]
        install.install(self.source, self.targets)
        self.assertEqual(stats, [link.lstat().st_mtime_ns for link in links])

    def test_conflict_prevents_partial_installation(self) -> None:
        existing = self.targets["claude"] / "asc-search"
        existing.mkdir(parents=True)
        (existing / "SKILL.md").write_text("keep this skill")
        with self.assertRaises(ValueError):
            install.install(self.source, self.targets)
        self.assertFalse(self.targets["codex"].exists())
        self.assertEqual((existing / "SKILL.md").read_text(), "keep this skill")

    def test_dangling_link_is_preserved(self) -> None:
        existing = self.targets["claude"] / "asc-search"
        existing.parent.mkdir(parents=True)
        existing.symlink_to(self.root / "missing")
        with self.assertRaises(ValueError):
            install.install(self.source, self.targets)
        self.assertEqual(existing.readlink(), self.root / "missing")
        self.assertFalse(self.targets["codex"].exists())

    def test_project_directory_link_does_not_create_self_link(self) -> None:
        directory = self.root / "repo/.claude/skills"
        directory.parent.mkdir(parents=True)
        directory.symlink_to(self.source.parent, target_is_directory=True)
        install.install(self.source, {"claude": directory})
        self.assertFalse((self.source / "asc-search").exists())
        self.assertFalse(self.source.is_symlink())

    def test_dry_run_does_not_create_directories(self) -> None:
        install.install(self.source, self.targets, dry_run=True)
        self.assertTrue(all(not directory.exists() for directory in self.targets.values()))

    def test_discovery_paths(self) -> None:
        user_home = self.root / "user"
        config_home = self.root / "config"
        with mock.patch.object(Path, "home", return_value=user_home), mock.patch.dict(
            os.environ, {"XDG_CONFIG_HOME": str(config_home)}
        ):
            self.assertEqual(
                install.target_directories(self.root, "user"),
                {
                    "codex": user_home / ".agents/skills",
                    "claude": user_home / ".claude/skills",
                    "opencode": config_home / "opencode/skills",
                },
            )
        self.assertEqual(install.target_directories(self.root, "project")["codex"], self.root / ".agents/skills")

    def test_cli_from_outside_checkout_and_installed_link(self) -> None:
        repo = self.source.parents[2]
        (repo / "docs/zh/api/SIMD-API/c_api").mkdir(parents=True)
        (repo / "include/c_api").mkdir(parents=True)
        script = self.source / "scripts/install.py"
        script.parent.mkdir()
        shutil.copyfile(install.__file__, script)
        command = [sys.executable, str(script), "--scope", "project"]
        first = subprocess.run(command, cwd=self.root, capture_output=True, text=True)
        self.assertEqual(first.returncode, 0, first.stderr)
        for directory in install.target_directories(repo, "project").values():
            self.assertEqual((directory / "asc-search").resolve(), self.source.resolve())
        linked_script = repo / ".agents/skills/asc-search/scripts/install.py"
        second = subprocess.run(
            [sys.executable, str(linked_script), "--scope", "project"], cwd=self.root, capture_output=True, text=True
        )
        self.assertEqual(second.returncode, 0, second.stderr)
        self.assertIn(f"Devkit checkout: {repo.resolve()}", second.stdout)
        self.assertEqual(second.stdout.count("already installed"), 3)

    def test_user_install_default_and_single_agent(self) -> None:
        repo = self.source.parents[2]
        (repo / "docs/zh/api/SIMD-API/c_api").mkdir(parents=True)
        (repo / "include/c_api").mkdir(parents=True)
        user_home = self.root / "user"
        config_home = self.root / "config"
        human = self.source.parent / "asc-devkit-search"
        human.mkdir()
        (human / "SKILL.md").write_text("local human workflow")
        with mock.patch.object(install, "SKILL_ROOT", self.source), mock.patch.object(
            Path, "home", return_value=user_home
        ), mock.patch.dict(os.environ, {"XDG_CONFIG_HOME": str(config_home)}):
            self.assertEqual(install.main(["--agent", "codex"]), 0)
            targets = install.target_directories(repo, "user")
            self.assertEqual((targets["codex"] / "asc-search").resolve(), self.source.resolve())
            self.assertFalse(targets["claude"].exists())
            self.assertFalse(targets["opencode"].exists())
            self.assertEqual(install.main([]), 0)
            for directory in targets.values():
                self.assertEqual((directory / "asc-search").resolve(), self.source.resolve())
                self.assertFalse((directory / "asc-devkit-search").exists())


if __name__ == "__main__":
    unittest.main()
