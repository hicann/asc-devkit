# -----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# -----------------------------------------------------------------------------------------------------------

"""First support, scoped overload lifecycles and deprecation are independent."""

import json
import itertools
from pathlib import Path
import sys
import tempfile
import unittest
from unittest import mock

import yaml

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "scripts"))
import build_index
import query
from derive import gate
from derive.versioning import deprecation


class LifecycleTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)
        self.rel = "docs/zh/api/SIMD-API/c_api/sync/asc_sync_vec.md"
        path = self.root / self.rel
        path.parent.mkdir(parents=True)
        self.path = path
        header = self.root / "include/c_api/sync/sync.h"
        header.parent.mkdir(parents=True)
        header.write_text(
            "/**\n * @override_id asc_sync_vec__z\n * @archs 2201 3510\n */\n__aicore__ inline void asc_sync_vec();\n"
            "/**\n * @override_id asc_sync_vec__i\n * @archs 2201 3510\n * @param [in] id Event ID.\n */\n__aicore__ inline void asc_sync_vec(int id);\n"
        )
        self.events = [
            dict(since="9.2.0", kind="overload_added", scope="overload:asc_sync_vec__i", scope_basis="resolved"),
            dict(since="9.2.0", kind="deprecated", scope="overload:asc_sync_vec__z", scope_basis="resolved"),
        ]

    def build(self, target, version=">=8.5.0", events=None, arch="3510"):
        fm = {
            "okftype": "reference.api",
            "description": "无参形式等待全部流水，有参形式同步指定流水。",
            "tag": [{"roles": ["control"]}, {"operations": ["synchronization"]}],
            "dependencies": [],
            "version": version,
            "version_changes": self.events if events is None else events,
        }
        self.path.write_text("---\n" + yaml.safe_dump(fm, allow_unicode=True) + "---\n# asc_sync_vec\n")
        out = self.root / "index"
        build_index.build(self.root, build_index.SCOPES["c_api"], out, target, arch)
        return json.loads((out / "index.json").read_text())

    def test_first_support_not_deprecation_controls_page_presence(self):
        before = self.build("8.4.0")
        self.assertNotIn(self.rel, before["documents"])
        old = self.build("9.1.0")["documents"][self.rel]
        self.assertEqual(["asc_sync_vec__z"], [s["override_id"] for s in old["signatures"]])
        self.assertFalse(old["deprecated"])
        current = self.build("9.2.0")
        doc = current["documents"][self.rel]
        self.assertEqual(2, len(doc["signatures"]))
        self.assertTrue(doc["deprecated"])
        self.assertIn("id", current["views"]["by_param"])
        older = self.build("9.1.0")
        self.assertNotIn("id", older["views"]["by_param"])
        rows = query.run_query(
            query.parser().parse_args(["--api", "asc_sync_vec"]), older, dict(build_index.EMPTY_GAPS)
        )
        self.assertEqual("9.1.0", rows[0]["version_context"]["cann_version"])
        self.assertIn("需核对", query.render(older, rows))

    def test_removed_is_an_event_and_prerelease_compares_before_release(self):
        removed = dict(since="9.3.0", kind="removed", scope="page", scope_basis="kind_default")
        self.assertIn(self.rel, self.build("9.3.0-beta.1", events=[removed])["documents"])
        self.assertNotIn(self.rel, self.build("9.3.0", events=[removed])["documents"])
        removed["scope_basis"] = "fallback"
        self.assertIn(self.rel, self.build("9.3.0", events=[removed])["documents"])

    def test_arch_specific_removal_does_not_remove_other_architecture(self):
        removed = dict(since="9.2.0", kind="removed", scope="overload:asc_sync_vec__i@3510", scope_basis="resolved")
        events = self.events + [removed]
        a = self.build("9.2.0", events=events, arch="3510")["documents"][self.rel]
        b = self.build("9.2.0", events=events, arch="2201")["documents"][self.rel]
        self.assertEqual(1, len(a["signatures"]))
        self.assertEqual(2, len(b["signatures"]))

    def test_legacy_upper_bound_is_reported_and_not_used_as_removal(self):
        index = self.build("9.3.0", version=">=9.0.0,<=9.1.0", events=[])
        self.assertEqual(">=9.0.0", index["documents"][self.rel]["version"])
        unresolved = json.loads((self.root / "index/unresolved.json").read_text())
        self.assertTrue(any("legacy upper bound ignored" in r["issue"] for r in unresolved["records"]))

    def test_invalid_version_rejects_build_instead_of_silently_filtering(self):
        with self.assertRaises(build_index.BuildError):
            self.build("9.2.0", version="unknown")
        self.assertTrue(gate.version_covers(">=9.2", "9.2.0"))
        self.assertTrue(gate.version_covers(">=9.3.0-beta.1", "9.3.0"))

    def test_index_accepts_structured_event_evidence_without_copying_it(self):
        event = dict(
            since="9.2.0",
            kind="return_type_changed",
            scope="overload:asc_sync_vec__z",
            scope_basis="resolved",
            evidence=[{"source_path": "history.h", "revision": "9.2.0", "excerpt": "void asc_sync_vec();"}],
        )
        events = self.build("9.2.0", events=[event])["documents"][self.rel]["version_changes"]
        self.assertNotIn("evidence", events[0])

    def test_detect_cann_version_preserves_prerelease(self):
        with mock.patch.dict("os.environ", {"ASCEND_HOME_PATH": "/opt/cann-9.3.0-beta.1/toolkit"}):
            self.assertEqual("9.3.0-beta.1", gate.detect_cann_version(None, self.root))
        with self.assertRaises(ValueError):
            gate.detect_cann_version("not-a-version", self.root)

    def test_prerelease_with_hyphens_is_preserved_from_path_and_cmake(self):
        for version in ("9.3.0-beta-1", "9.3.0-beta.1", "9.3.0"):
            with self.subTest(version=version):
                with mock.patch.dict("os.environ", {"ASCEND_HOME_PATH": f"/opt/cann-{version}/toolkit"}):
                    self.assertEqual(version, gate.detect_cann_version(None, self.root))
                (self.root / "version.cmake").write_text(f'set(VERSION "{version}")\n')
                with mock.patch.dict("os.environ", {"ASCEND_HOME_PATH": ""}):
                    self.assertEqual(version, gate.detect_cann_version(None, self.root))

    def test_event_deprecation_keeps_declared_replacement(self):
        """声明废弃版本晚于目标时，事件给出版本，但声明自身的替代目标要保留。"""
        signature = {
            "override_id": "asc_sync_vec__z",
            "deprecated": {"version": "9.5.0", "replacement": "asc_other", "date": "2026-01-01"},
        }
        events = [dict(since="9.2.0", kind="deprecated", scope="overload:asc_sync_vec__z")]
        self.assertEqual(
            {"version": "9.2.0", "replacement": "asc_other"}, deprecation(signature, events, "9.3.0", "3510")
        )

    def test_malformed_declared_deprecation_version_falls_back_to_events(self):
        """畸形 ASC_DEPRECATED 版本不中断构建：视为声明证据不可用，改由事件推导。"""
        signature = {"override_id": "asc_sync_vec__z", "deprecated": {"version": "9..2", "replacement": "asc_other"}}
        events = [dict(since="9.3.0", kind="deprecated", scope="overload:asc_sync_vec__z")]
        self.assertEqual(
            {"version": "9.3.0", "replacement": "asc_other"}, deprecation(signature, events, "9.4.0", "3510")
        )
        self.assertIsNone(deprecation(signature, [], "9.4.0", "3510"))

    def test_code_level_deprecated_macro_binds_following_declaration(self):
        """代码级 ASC_DEPRECATED(v,date,replacement) 绑定到后续声明并保留替代目标。"""
        (self.root / "include/c_api/sync/sync.h").write_text(
            'ASC_DEPRECATED(9.2.0, "2027/09/07", asc_other)\n'
            "/**\n * @override_id asc_sync_vec__z\n * @archs 2201 3510\n */\n"
            "__aicore__ inline void asc_sync_vec();\n"
        )
        index = self.build("9.2.0", events=[])
        signature = index["documents"][self.rel]["signatures"][0]
        self.assertEqual({"version": "9.2.0", "replacement": "asc_other"}, signature["deprecated"])

    def test_malformed_deprecated_macro_does_not_abort_build(self):
        """畸形版本号的 ASC_DEPRECATED 不中断构建（该声明证据不可用）。"""
        (self.root / "include/c_api/sync/sync.h").write_text(
            'ASC_DEPRECATED(9..2, "2027/09/07", asc_other)\n'
            "/**\n * @override_id asc_sync_vec__z\n * @archs 2201 3510\n */\n"
            "__aicore__ inline void asc_sync_vec();\n"
        )
        index = self.build("9.2.0", events=[])
        self.assertIn(self.rel, index["documents"])
        self.assertFalse(index["documents"][self.rel]["deprecated"])

    def test_deprecation_is_order_independent_and_uses_precise_earliest_evidence(self):
        signature = {"override_id": "asc_sync_vec__z", "deprecated": None}
        events = [
            dict(since="9.0.0", kind="deprecated", scope="page", scope_basis="resolved"),
            dict(since="9.5.0", kind="deprecated", scope="overload:asc_sync_vec__z"),
            dict(since="9.4.0", kind="deprecated", scope="overload:asc_sync_vec__z"),
            dict(since="9.6.0", kind="deprecated", scope="overload:asc_sync_vec__z@3510"),
        ]
        for order in itertools.permutations(events):
            self.assertEqual(deprecation(signature, order, "9.7.0", "3510")["version"], "9.6.0")
            self.assertEqual(deprecation(signature, order, "9.7.0", "2201")["version"], "9.4.0")
            self.assertEqual(deprecation(signature, order, "9.3.0", "3510")["version"], "9.0.0")
        signature["deprecated"] = {"version": "9.2.0", "replacement": "asc_other", "date": "2026-01-01"}
        self.assertEqual(deprecation(signature, events, "9.7.0", "3510"), signature["deprecated"])
        self.assertEqual(deprecation(signature, events, "9.1.0", "3510")["version"], "9.0.0")
        self.assertIsNone(
            deprecation(
                signature,
                [dict(since="9.0.0", kind="deprecated", scope="page", scope_basis="fallback")],
                "9.1.0",
                "3510",
            )
        )

    def test_four_segment_versions_are_accepted(self):
        """version_key 接受三段以上数字段；四段版本在探测与构建中都不报错。"""
        self.assertEqual(gate.version_key("9.2"), gate.version_key("9.2.0"))
        self.assertGreater(gate.version_key("9.2.0.1"), gate.version_key("9.2.0"))
        self.assertEqual("9.2.0.1", gate.detect_cann_version("9.2.0.1", self.root))
        with mock.patch.dict("os.environ", {"ASCEND_HOME_PATH": "/opt/cann-9.2.0.1/toolkit"}):
            self.assertEqual("9.2.0.1", gate.detect_cann_version(None, self.root))
        (self.root / "version.cmake").write_text('set(VERSION "9.2.0.1")\n')
        with mock.patch.dict("os.environ", {"ASCEND_HOME_PATH": ""}):
            self.assertEqual("9.2.0.1", gate.detect_cann_version(None, self.root))
        self.assertIn(self.rel, self.build("9.2.0.1", version=">=9.2.0", events=[])["documents"])

    def test_version_query_tracks_prerelease_and_lifecycle(self):
        removed = dict(since="9.3.0", kind="removed", scope="page", scope_basis="kind_default")
        for target, present in (("9.2.0-beta.1", False), ("9.2.0", True), ("9.3.0-beta.1", True), ("9.3.0", False)):
            with self.subTest(target=target):
                index = self.build(target, version=">=9.2.0", events=self.events + [removed])
                args = query.parser().parse_args(["--version", target])
                rows = query.run_query(args, index, dict(build_index.EMPTY_GAPS))
                self.assertEqual([self.rel] if present else [], [row["path"] for row in rows])
                if present:
                    self.assertTrue(index["documents"][self.rel]["deprecated"])

    def test_version_query_requires_an_available_overload(self):
        events = [
            dict(since="9.2.0", kind="removed", scope=f"overload:asc_sync_vec__{suffix}", scope_basis="resolved")
            for suffix in ("z", "i")
        ]
        index = self.build("9.2.0", events=events)
        args = query.parser().parse_args(["--version", "9.2.0"])
        self.assertEqual([], query.run_query(args, index, dict(build_index.EMPTY_GAPS)))


if __name__ == "__main__":
    unittest.main()
