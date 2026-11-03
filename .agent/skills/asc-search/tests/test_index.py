# -----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# -----------------------------------------------------------------------------------------------------------

"""asc-search 推导引擎、筛选条件与查询测试（不写入已提交索引，用临时目录构建）。"""

from __future__ import annotations

import io
import json
import os
import shutil
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

SCRIPTS = Path(__file__).resolve().parents[1] / "scripts"
REPO_ROOT = Path(os.environ.get("ASC_SEARCH_TEST_ROOT", SCRIPTS.parents[3])).resolve()
_DATA_PROBE = REPO_ROOT / "docs/zh/api/SIMD-API/c_api/vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub_arch_3510.md"
HAS_FULL_DATA = _DATA_PROBE.is_file() and _DATA_PROBE.read_text(encoding="utf-8").startswith("---\n")
sys.path.insert(0, str(SCRIPTS))

import build_index  # noqa: E402
from derive import gate  # noqa: E402
from derive.docparse import doc_product_support  # noqa: E402
from derive.examples_db import build_examples_db  # noqa: E402
from derive.include_db import build_include_db, declarations_for  # noqa: E402


class GateTests(unittest.TestCase):
    def test_version_covers(self) -> None:
        self.assertTrue(gate.version_covers(">=9.0.0", "9.2.0"))
        self.assertTrue(gate.version_covers(">=9.1.0", "9.2.0"))
        self.assertFalse(gate.version_covers(">=9.0.0,<=9.2.0", "9.2.0"))
        self.assertFalse(gate.version_covers(">=9.1.0", "9.0.0"))
        self.assertFalse(gate.version_covers(">=9.0.0,<=9.1.0", "9.2.0"))

    def test_arch_label_mapping(self) -> None:
        pm = gate.load_product_map(build_index.PRODUCT_MAP_PATH)
        self.assertEqual(gate.arch_label_to_number("3510", pm), "3510")
        self.assertEqual(gate.arch_label_to_number("2201", pm), "2201")
        self.assertEqual(gate.arch_label_to_number("A3", pm), "2201")
        self.assertEqual(gate.npu_to_arch("950", pm), "3510")
        self.assertEqual(gate.npu_to_arch("910b", pm), "2201")
        with self.assertRaises(ValueError):
            gate.arch_label_to_number("A9", pm)

    def test_cann_detection_chain(self) -> None:
        self.assertEqual(gate.detect_cann_version("8.0.0", REPO_ROOT), "8.0.0")
        with tempfile.TemporaryDirectory() as temporary, mock.patch.dict("os.environ", {"ASCEND_HOME_PATH": ""}):
            Path(temporary, "version.cmake").write_text('set(VERSION "9.2.0")\n')
            detected = gate.detect_cann_version(None, Path(temporary))
        self.assertRegex(detected, r"^\d+\.\d+\.\d+$")


@unittest.skipUnless(HAS_FULL_DATA, "annotated devkit data is not installed; set ASC_SEARCH_TEST_ROOT")
class DeriveTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.include_db = build_include_db(REPO_ROOT / "include" / "c_api")

    def test_declaration_derivation(self) -> None:
        decls, mode = declarations_for("asc_copy_gm2ub_align", self.include_db, "vector_datamove")
        self.assertEqual(mode, "exact")
        annotated = [d for d in decls if d.override_id]
        self.assertTrue(any("2201" in d.archs for d in annotated))
        self.assertTrue(any("3510" in d.archs for d in annotated))
        strided = next(d for d in annotated if "u16" in d.override_id and "3510" in d.archs)
        self.assertEqual(strided.locations(), ["global_memory", "unified_buffer"])

    def test_enum_catalog(self) -> None:
        enum = self.include_db["enums"]["asc_cast_round_mode"]
        self.assertIn("RN", enum["values"])
        self.assertTrue(
            any(c["name"] == "ASC_CAST_RN" for c in self.include_db["constants"]), "ASC_CAST_RN 常量应入 defs 目录"
        )

    def test_mask_derivation(self) -> None:
        decls, _ = declarations_for("asc_add", self.include_db, "vector_compute")
        self.assertTrue(all(d.masked() for d in decls if d.params and d.params[0].is_vector))

    def test_param_direction(self) -> None:
        from derive.include_db import _normalize_direction, _parse_doxygen

        self.assertEqual(_normalize_direction("in,out"), "in,out")
        self.assertEqual(_normalize_direction("out, in "), "in,out")
        self.assertEqual(_normalize_direction("in"), "in")
        parsed = _parse_doxygen("/**\n * @param [in,out] address 地址。\n * @param [in] val 值。\n */")
        self.assertEqual(parsed[0]["params"], {"address": "in,out", "val": "in"})

    def test_examples_db(self) -> None:
        db = build_examples_db(REPO_ROOT / "examples/02_simd_c_api/03_c_api")
        self.assertGreater(len(db), 50)
        marker_id = next(iter(db))
        self.assertIn("file", db[marker_id][0])

    def test_shared_example_range_and_header_source(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / "legacy.asc").write_text("//! [asc_old__z]\nasc_old();\n//! [asc_old__z]\n")
            (root / "shared.h").write_text(
                "//! [asc_add__i32, asc_copy__z]\nvoid first() { asc_add(1); }\n"
                "void second() { asc_copy(); }\n//! [asc_copy__z, asc_add__i32]\n"
            )
            db = build_examples_db(root)
            self.assertEqual({"asc_old__z", "asc_add__i32", "asc_copy__z"}, set(db))
            self.assertEqual("shared.h", db["asc_add__i32"][0]["file"])
            self.assertEqual((2, 3), (db["asc_add__i32"][0]["start_line"], db["asc_add__i32"][0]["end_line"]))
            self.assertEqual(db["asc_add__i32"], db["asc_copy__z"])
            (root / "broken.asc").write_text("//! [asc_bad__z]\nasc_bad();\n")
            with self.assertRaisesRegex(ValueError, "unclosed"):
                build_examples_db(root)

    def test_product_support_parse(self) -> None:
        body = (
            REPO_ROOT / "docs/zh/api/SIMD-API/c_api/vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub_arch_3510.md"
        ).read_text(encoding="utf-8")
        support = doc_product_support(body.split("---\n", 2)[-1])
        self.assertEqual(support["950"]["support"], "支持")
        self.assertEqual(support["A3"]["support"], "不支持")


@unittest.skipUnless(HAS_FULL_DATA, "annotated devkit data is not installed; set ASC_SEARCH_TEST_ROOT")
class BuildTests(unittest.TestCase):
    """真实构建（临时目录）：筛选条件、推导与确定性。"""

    @classmethod
    def setUpClass(cls) -> None:
        cls.tmp = tempfile.TemporaryDirectory()
        cls.out = Path(cls.tmp.name)
        scope = build_index.SCOPES["c_api"]
        build_index.build(REPO_ROOT, scope, cls.out / "arch3510", "9.2.0", "3510")
        build_index.build(REPO_ROOT, scope, cls.out / "a3", "9.2.0", "A3")
        cls.arch3510 = json.loads((cls.out / "arch3510" / "index.json").read_text(encoding="utf-8"))
        cls.a3 = json.loads((cls.out / "a3" / "index.json").read_text(encoding="utf-8"))

    @classmethod
    def tearDownClass(cls) -> None:
        cls.tmp.cleanup()

    def test_gate_arch(self) -> None:
        self.assertEqual(self.arch3510["meta"]["arch"], "3510")
        self.assertEqual(self.a3["meta"]["arch"], "2201")
        for path in self.arch3510["documents"]:
            if "_arch_2201" in path:
                self.fail(f"2201 变体页进入 3510 索引: {path}")
        for path in self.a3["documents"]:
            if "_arch_3510" in path:
                self.fail(f"3510 变体页进入 A3 索引: {path}")
        # 全量轮：3510 文档多于 A3（新架构 API 更多 + 废弃双架构文档保留）
        self.assertGreater(len(self.arch3510["documents"]), len(self.a3["documents"]))

    def test_gate_version(self) -> None:
        # 存量废弃头的旧上限不再作为移除证据，9.2.0 仍可索引。
        self.assertEqual(self.arch3510["meta"]["filtered_by_version"], 0)

    def test_arch_page_signatures(self) -> None:
        doc = self.arch3510["documents"][
            "docs/zh/api/SIMD-API/c_api/vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub_arch_3510.md"
        ]
        for signature in doc["signatures"]:
            self.assertIn("3510", signature["archs"])

    def test_views(self) -> None:
        for view in ("by_stage", "by_role", "by_operation", "by_dtype", "by_param", "by_enum", "by_feature"):
            self.assertIn(view, self.arch3510["views"])
        self.assertGreater(len(self.arch3510["views"]["by_param"]), 10)
        self.assertIn("asc_load_l2_cache_mode", self.arch3510["views"]["by_enum"])
        self.assertGreater(len(self.a3["views"]["by_dtype_pair"]), 0)
        self.assertIn("low_precision", self.a3["views"]["by_feature"])

    def test_enum_reverse_lookup(self) -> None:
        users = self.arch3510["views"]["by_enum"]["asc_load_l2_cache_mode"]
        self.assertTrue(users)
        enum = self.arch3510["defs"]["enums"]["asc_load_l2_cache_mode"]
        self.assertIn("NORMAL_PERSISTENT", enum["values"])

    def test_deterministic_rebuild(self) -> None:
        scope = build_index.SCOPES["c_api"]
        build_index.build(REPO_ROOT, scope, self.out / "arch3510-again", "9.2.0", "3510")
        first = build_index.strip_informational(self.arch3510)
        second = build_index.strip_informational(
            json.loads((self.out / "arch3510-again" / "index.json").read_text(encoding="utf-8"))
        )
        self.assertEqual(first, second)

    def test_gaps_optional(self) -> None:
        """gaps.json 缺失视为空台账：构建照常出索引并落盘空快照。"""
        with tempfile.TemporaryDirectory() as tmp:
            with mock.patch.object(build_index, "INDEX_DIR", Path(tmp)):
                out = self.out / "no-gaps"
                build_index.build(REPO_ROOT, build_index.SCOPES["c_api"], out, "9.2.0", "A3")
        gaps = json.loads((out / "gaps.json").read_text(encoding="utf-8"))
        self.assertEqual(gaps, {"scope_out_gaps": [], "relation_gaps": []})


FIXTURE_REL = "docs/zh/api/SIMD-API/c_api/vector_compute/asc_fixture.md"
FIXTURE_REL_OLD = "docs/zh/api/SIMD-API/c_api/vector_compute/asc_fixture_old.md"
FIXTURE_REL_PLAIN = "docs/zh/api/SIMD-API/c_api/vector_compute/asc_fixture_plain.md"
FIXTURE_EVENTS = (
    "version_changes:\n"
    '  - since: "9.2.0"\n'
    "    kind: overload_added\n"
    '    scope: "overload:asc_fixture__a"\n'
    '    note: "新增显式重载"\n'
    '    evidence: "asc_mmad新增显式传入Bias起始地址的重载"\n'
    '  - since: "9.1.0"\n'
    "    kind: introduced\n"
    "    scope: page\n"
    '    evidence: "新增矢量计算接口"\n'
)


def fixture_document(version_changes: str = "", version: str = ">=9.0.0") -> str:
    """最小合法 slim 头夹具（含可选 version_changes 块）。"""
    return (
        "---\n"
        "okftype: reference.api\n"
        "description: 版本事件夹具文档\n"
        "tag:\n"
        "  - roles:\n"
        "      - compute\n"
        "dependencies: []\n"
        f'version: "{version}"\n'
        f"{version_changes}"
        "---\n"
        "# asc_fixture\n"
        "\n正文。\n"
    )


class VersionChangesTests(unittest.TestCase):
    """version_changes：构建期结构校验 + 透传落盘 + by_change_version 视图与查询维度。"""

    def _build(self, documents: dict[str, str]) -> tuple[dict, Path]:
        tmp = tempfile.TemporaryDirectory()
        self.addCleanup(tmp.cleanup)
        root = Path(tmp.name) / "repo"
        for rel, text in documents.items():
            path = root / rel
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(text, encoding="utf-8")
        out = Path(tmp.name) / "out"
        build_index.build(root, build_index.SCOPES["c_api"], out, "9.2.0", "3510")
        return json.loads((out / "index.json").read_text(encoding="utf-8")), out

    def _query(self, index: dict, **overrides) -> list[dict]:
        import query as query_mod

        defaults = {
            "stage": None,
            "operation": None,
            "category": None,
            "api_class": None,
            "dtype": None,
            "kw": None,
            "convert": None,
            "feature": None,
            "version": None,
            "changed_in": None,
            "product": None,
            "api": None,
            "deps_of": None,
            "kind": None,
            "reverse": False,
            "param": None,
            "enum": None,
            "gap": None,
        }
        namespace = type("Args", (), {**defaults, **overrides, "json": True, "surface": "c_api", "index_dir": None})()
        return query_mod.run_query(namespace, index, dict(build_index.EMPTY_GAPS))

    def test_version_changes_round_trip(self) -> None:
        """含 version_changes 的文档进索引，字段出现在 index.json（落盘 payload）。

        索引内是**降维副本**：保留 since/kind/scope/note，`evidence` 不落盘
        （事实源在文档头）。
        """
        index, out = self._build({FIXTURE_REL: fixture_document(FIXTURE_EVENTS)})
        events = index["documents"][FIXTURE_REL]["version_changes"]
        self.assertEqual(["9.2.0", "9.1.0"], [event["since"] for event in events])
        self.assertEqual(["overload_added", "introduced"], [event["kind"] for event in events])
        self.assertEqual("overload:asc_fixture__a", events[0]["scope"])
        self.assertEqual("新增显式重载", events[0]["note"])
        self.assertNotIn("note", events[1])  # note 可缺省
        for event in events:
            self.assertNotIn("evidence", event, "evidence 不应进索引")
        self.assertNotIn("asc_mmad", (out / "index.json").read_text(encoding="utf-8"))

    def test_scope_basis_survives_projection(self) -> None:
        """T12 的 scope_basis 是查询要用的封闭集标记，降维时必须保留。"""
        extra = (
            "version_changes:\n"
            '  - since: "9.2.0"\n'
            "    kind: overload_added\n"
            "    scope: page\n"
            '    scope_basis: "fallback"\n'
        )
        index, _ = self._build({FIXTURE_REL: fixture_document(extra)})
        event = index["documents"][FIXTURE_REL]["version_changes"][0]
        self.assertEqual("fallback", event["scope_basis"])
        self.assertNotIn("evidence", event)

    def test_independent_query_intersects_with_filter_dims(self) -> None:
        """A11：独立查询 + 过滤维度必须求交，不得静默忽略。"""
        index, _ = self._build({FIXTURE_REL: fixture_document(FIXTURE_EVENTS)})
        api = FIXTURE_REL.split("/")[-1][:-3]
        self.assertTrue(self._query(index, api=api), "单独使用 --api 应命中")
        self.assertEqual([], self._query(index, api=api, changed_in="9.9.9"))
        self.assertTrue(self._query(index, api=api, changed_in="9.2.0"))

    def test_gap_with_filter_dimension_is_rejected(self) -> None:
        """无法表达的组合必须显式报错（缺口是范围外登记，无法与文档维度求交）。"""
        index, _ = self._build({FIXTURE_REL: fixture_document(FIXTURE_EVENTS)})
        for overrides in ({"gap": "x", "stage": "out"}, {"gap": "x", "changed_in": "9.2.0"}):
            with self.subTest(overrides=overrides):
                with self.assertRaisesRegex(ValueError, "不能与过滤维度组合"):
                    self._query(index, **overrides)

    def test_version_changes_default_empty(self) -> None:
        """无该字段（存量 5 字段文档）落盘为空数组，不报错。"""
        index, _ = self._build({FIXTURE_REL: fixture_document()})
        self.assertEqual([], index["documents"][FIXTURE_REL]["version_changes"])

    def test_unknown_kind_is_accepted(self) -> None:
        """宽进：未知 kind 不报错（建设侧新增 kind 不得让检索侧 BuildError）。"""
        extra = (
            "version_changes:\n"
            '  - since: "9.3.0"\n'
            "    kind: future_kind_2030\n"
            "    scope: page\n"
            '    evidence: "未冻结词表的新事件"\n'
        )
        index, _ = self._build({FIXTURE_REL: fixture_document(extra)})
        self.assertEqual("future_kind_2030", index["documents"][FIXTURE_REL]["version_changes"][0]["kind"])

    def test_null_optional_note_is_treated_as_absent(self) -> None:
        """宽进：可选键给 null（YAML `note:`）不报错，落盘时按缺省处理。"""
        extra = (
            "version_changes:\n"
            '  - since: "9.2.0"\n'
            "    kind: overload_added\n"
            "    scope: page\n"
            "    note:\n"
            '    evidence: "x"\n'
        )
        index, _ = self._build({FIXTURE_REL: fixture_document(extra)})
        self.assertNotIn("note", index["documents"][FIXTURE_REL]["version_changes"][0])

    def test_invalid_structure_is_rejected(self) -> None:
        """严出：容器/映射/必填键/类型不合法 → errors 报错（构建拒绝出索引）。"""
        invalid = {
            "not_a_list": "version_changes: {since: '9.2.0'}\n",
            "item_not_mapping": "version_changes:\n  - just-a-string\n",
            "missing_scope": 'version_changes:\n  - since: "9.2.0"\n    kind: overload_added\n    evidence: "x"\n',
            "since_not_str": (
                "version_changes:\n"
                "  - since: 9.2\n"  # YAML float，不是版本点字符串
                "    kind: overload_added\n"
                "    scope: page\n"
                '    evidence: "x"\n'
            ),
            "note_not_str": (
                "version_changes:\n"
                '  - since: "9.2.0"\n'
                "    kind: overload_added\n"
                "    scope: page\n"
                "    note: [1, 2]\n"
                '    evidence: "x"\n'
            ),
        }
        for name, block in invalid.items():
            with self.subTest(case=name):
                with self.assertRaises(build_index.BuildError) as ctx:
                    self._build({FIXTURE_REL: fixture_document(block)})
                self.assertIn("version_changes", str(ctx.exception))

    def test_version_changes_do_not_change_gating(self) -> None:
        """加入 version_changes 前后 doc_count / filtered_by_version 不变（不删文档）。"""
        without, _ = self._build(
            {FIXTURE_REL: fixture_document(), FIXTURE_REL_OLD: fixture_document(version=">=9.3.0")}
        )
        with_events, _ = self._build(
            {
                FIXTURE_REL: fixture_document(FIXTURE_EVENTS),
                FIXTURE_REL_OLD: fixture_document(FIXTURE_EVENTS, version=">=9.3.0"),
            }
        )
        for key in ("doc_count", "primary_count", "filtered_by_version", "filtered_by_arch", "nav_page_count"):
            self.assertEqual(without["meta"][key], with_events["meta"][key], key)
        self.assertEqual(1, with_events["meta"]["filtered_by_version"])
        self.assertEqual(1, with_events["meta"]["doc_count"])
        self.assertNotIn(FIXTURE_REL_OLD, with_events["documents"])

    def test_change_version_view(self) -> None:
        """建索引期视图 by_change_version = {since: [rel]}；空事件文档不入任何键。"""
        index, _ = self._build(
            {
                FIXTURE_REL: fixture_document(FIXTURE_EVENTS),
                FIXTURE_REL_PLAIN: fixture_document(),  # 无该字段
                FIXTURE_REL_OLD: fixture_document("version_changes: []\n"),  # 显式空数组
            }
        )
        view = index["views"]["by_change_version"]
        self.assertEqual([FIXTURE_REL], view["9.2.0"])
        self.assertEqual([FIXTURE_REL], view["9.1.0"])
        self.assertEqual(["9.1.0", "9.2.0"], sorted(view))
        self.assertIn(FIXTURE_REL_PLAIN, index["documents"])  # 未入视图但仍在索引里
        self.assertNotIn(FIXTURE_REL_PLAIN, view["9.2.0"])
        self.assertNotIn(FIXTURE_REL_OLD, view["9.2.0"])
        self.assertNotIn("", view)

    def test_changed_in_normalizes_legacy_spellings(self) -> None:
        """--changed-in 与 --version 同样归一化：9.2 / >=9.2.0 / 9.2.0 等价。"""
        index, _ = self._build({FIXTURE_REL: fixture_document(FIXTURE_EVENTS)})
        for value in ("9.2", ">=9.2.0", "9.2.0"):
            with self.subTest(value=value):
                hits = self._query(index, changed_in=value)
                self.assertEqual([FIXTURE_REL], [row["path"] for row in hits])
                self.assertEqual(["9.2.0"], [event["since"] for event in hits[0]["version_changes"]])

    def test_changed_in_flag(self) -> None:
        """--changed-in：命中该版本有事件的文档，并只带该版本的事件摘要。"""
        index, _ = self._build({FIXTURE_REL: fixture_document(FIXTURE_EVENTS)})
        hits = self._query(index, changed_in="9.2.0")
        self.assertEqual([FIXTURE_REL], [row["path"] for row in hits])
        self.assertIn("changed_in:9.2.0", hits[0]["why"])
        self.assertEqual(["9.2.0"], [event["since"] for event in hits[0]["version_changes"]])
        self.assertEqual("overload_added", hits[0]["version_changes"][0]["kind"])
        older = self._query(index, changed_in="9.1.0")
        self.assertEqual(["9.1.0"], [event["since"] for event in older[0]["version_changes"]])
        # 未命中：视图存在但该版本无键 → 空结果（CLI 会显式报出索引内已有的事件版本）
        self.assertEqual([], self._query(index, changed_in="9.3.0"))
        self.assertEqual([], self._query(index, changed_in="9.3.0", category="vector_compute"))

    def test_changed_in_no_hit_is_reported_not_swallowed(self) -> None:
        """0 命中时 CLI 显式说明，并把索引内已有的事件版本打出来（非静默）。"""
        import query as query_mod

        index, _ = self._build({FIXTURE_REL: fixture_document(FIXTURE_EVENTS)})
        gaps = dict(build_index.EMPTY_GAPS)
        with mock.patch.object(sys, "argv", ["query.py", "--changed-in", "9.3.0"]):
            with mock.patch.object(query_mod, "load", return_value=(index, gaps)):
                with mock.patch("sys.stdout", new=io.StringIO()) as out:
                    self.assertEqual(0, query_mod.main())
        text = out.getvalue()
        self.assertIn("9.3.0", text)
        self.assertIn("9.1.0, 9.2.0", text)
        self.assertNotEqual("no match\n", text)

    def test_version_flag_matches_first_support_range(self) -> None:
        """9.2.0 查询同时命中更早支持和恰好从 9.2.0 开始支持的页面。"""
        index, _ = self._build(
            {
                FIXTURE_REL: fixture_document(FIXTURE_EVENTS, version=">=9.0.0"),
                FIXTURE_REL_PLAIN: fixture_document(version=">=9.2.0"),
                FIXTURE_REL_OLD: fixture_document(version=">=9.3.0"),
            }
        )
        expected = {FIXTURE_REL, FIXTURE_REL_PLAIN}
        for target in ("9.2.0", "9.2", ">=9.2.0"):
            with self.subTest(target=target):
                hits = self._query(index, version=target)
                self.assertEqual(expected, {row["path"] for row in hits})
                self.assertTrue(all(f"version:{target.removeprefix('>=')}" in row["why"] for row in hits))
        self.assertEqual(
            [FIXTURE_REL], [row["path"] for row in self._query(index, version="9.2.0", changed_in="9.2.0")]
        )
        self.assertEqual([FIXTURE_REL], [row["path"] for row in self._query(index, version="9.2.0", api="asc_fixture")])

    def test_version_query_requires_index_for_target_version(self) -> None:
        index, _ = self._build({FIXTURE_REL: fixture_document()})
        for target in ("9.1.0", "9.3.0"):
            with self.subTest(target=target):
                with self.assertRaisesRegex(ValueError, "ensure_index.py --cann-version " + target):
                    self._query(index, version=target)
        for invalid in ("unknown", " ", "<=9.2.0", ">=9.0.0,<=9.2.0"):
            with self.subTest(invalid=invalid):
                with self.assertRaises(ValueError):
                    self._query(index, version=invalid)


@unittest.skipUnless(HAS_FULL_DATA, "annotated devkit data is not installed; set ASC_SEARCH_TEST_ROOT")
class DryrunTests(unittest.TestCase):
    """dryrun.yaml 查询清单回归。"""

    @classmethod
    def setUpClass(cls) -> None:
        import yaml

        cls.tmp = tempfile.TemporaryDirectory()
        cls.out = Path(cls.tmp.name)
        scope = build_index.SCOPES["c_api"]
        build_index.build(REPO_ROOT, scope, cls.out / "arch3510", "9.2.0", "3510")
        build_index.build(REPO_ROOT, scope, cls.out / "a3", "9.2.0", "A3")
        cls.cases = yaml.safe_load((Path(__file__).parent / "dryrun.yaml").read_text(encoding="utf-8"))

    @classmethod
    def tearDownClass(cls) -> None:
        cls.tmp.cleanup()

    def _run(self, arch: str, args: dict) -> list[dict]:
        sys.path.insert(0, str(SCRIPTS))
        import query as query_mod

        index, gaps = query_mod.load(self.out / arch)
        defaults = {
            "stage": None,
            "operation": None,
            "category": None,
            "api_class": None,
            "dtype": None,
            "kw": None,
            "convert": None,
            "feature": None,
            "version": None,
            "product": None,
            "api": None,
            "deps_of": None,
            "kind": None,
            "reverse": False,
            "param": None,
            "enum": None,
            "gap": None,
        }
        namespace = type("Args", (), {**defaults, **args, "json": True, "surface": "c_api", "index_dir": None})()
        return query_mod.run_query(namespace, index, gaps)

    def test_reverse_deps_by_family(self) -> None:
        # 族名与成员全路径应得到同一组反向入边
        by_family = self._run("arch3510", {"deps_of": "asc_copy_gm2ub", "reverse": True})
        by_path = self._run(
            "arch3510",
            {
                "deps_of": "docs/zh/api/SIMD-API/c_api/vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub_arch_3510.md",
                "reverse": True,
            },
        )
        self.assertGreaterEqual(len(by_family), 1)
        self.assertEqual({r["path"] for r in by_family}, {r["path"] for r in by_path})

    def test_unknown_named_query_returns_empty(self) -> None:
        self.assertEqual(self._run("arch3510", {"api": "asc_no_such_api"}), [])
        self.assertEqual(self._run("arch3510", {"deps_of": "asc_no_such_api"}), [])
        self.assertEqual(self._run("arch3510", {"operation": "no_such_operation"}), [])

    def test_dryrun_cases(self) -> None:
        for case in self.cases["cases"]:
            with self.subTest(case=case["name"]):
                arch = case.get("arch", "arch3510")
                results = self._run(arch, case["args"])
                self.assertGreaterEqual(
                    len(results), case.get("min", 1), f"{case['name']}: 期望 ≥{case.get('min', 1)}，实际 {len(results)}"
                )
                if "contains" in case:
                    joined = json.dumps(results, ensure_ascii=False)
                    for token in case["contains"]:
                        self.assertIn(token, joined, f"{case['name']}: 缺少 {token}")


@unittest.skipUnless(HAS_FULL_DATA, "annotated devkit data is not installed; set ASC_SEARCH_TEST_ROOT")
class EnsureTests(unittest.TestCase):
    """ensure_index 前置检测：筛选条件匹配、指纹漂移与原地重建决策。"""

    @classmethod
    def setUpClass(cls) -> None:
        import ensure_index

        cls.ensure_mod = ensure_index
        cls.tmp = tempfile.TemporaryDirectory()
        cls.out = Path(cls.tmp.name)
        build_index.build(REPO_ROOT, build_index.SCOPES["c_api"], cls.out / "inst", "9.2.0", "3510")

    @classmethod
    def tearDownClass(cls) -> None:
        cls.tmp.cleanup()

    def _copy_instance(self, name: str) -> Path:
        root = self.out / name
        shutil.copytree(self.out / "inst", root / "c_api")
        return root / "c_api"

    def _patch_build_forbidden(self):
        return mock.patch.object(
            self.ensure_mod.build_index, "build", side_effect=AssertionError("ensure must not rebuild")
        )

    def test_gate_reasons(self) -> None:
        ok = {"surface": "c_api", "cann_version": "9.2.0", "arch_label": "3510"}
        self.assertEqual(self.ensure_mod.gate_reasons(ok, "c_api", "9.2.0", "3510"), [])
        reasons = self.ensure_mod.gate_reasons(
            {"surface": "c_api", "cann_version": "9.1.0", "arch_label": "A3"}, "c_api", "9.2.0", "3510"
        )
        self.assertEqual(reasons, ["cann 9.1.0 != 9.2.0", "arch A3 != 3510"])

    def test_light_fresh_ok(self) -> None:
        ok, reasons = self.ensure_mod.light_fresh(self.out / "inst", REPO_ROOT, build_index.SCOPES["c_api"])
        self.assertTrue(ok, reasons)

    def test_light_fresh_detects_drift(self) -> None:
        inst = self._copy_instance("drift-meta")
        meta = json.loads((inst / "build-meta.json").read_text(encoding="utf-8"))
        meta["taxonomy_sha256"] = "0" * 64
        first_doc = next(iter(meta["document_sha256"]))
        meta["document_sha256"][first_doc] = "1" * 64
        (inst / "build-meta.json").write_text(json.dumps(meta), encoding="utf-8")
        ok, reasons = self.ensure_mod.light_fresh(inst, REPO_ROOT, build_index.SCOPES["c_api"])
        self.assertFalse(ok)
        self.assertIn("taxonomy_sha256 drift: tag-taxonomy.json changed since build", reasons)
        self.assertIn(f"document changed: {first_doc}", reasons)

    def test_light_fresh_detects_scope_growth(self) -> None:
        inst = self._copy_instance("drift-count")
        index = json.loads((inst / "index.json").read_text(encoding="utf-8"))
        index["meta"]["doc_count"] += 1
        (inst / "index.json").write_text(json.dumps(index), encoding="utf-8")
        ok, reasons = self.ensure_mod.light_fresh(inst, REPO_ROOT, build_index.SCOPES["c_api"])
        self.assertFalse(ok)
        self.assertTrue(any(r.startswith("scope document count") for r in reasons), reasons)

    def test_committed_instance_ready_noop(self) -> None:
        """committed c_api 实例与 9.2.0×3510 匹配且新鲜：ensure 零动作。"""
        with self._patch_build_forbidden():
            result = self.ensure_mod.ensure(
                REPO_ROOT, "c_api", "9.2.0", "3510", index_root=self._copy_instance("ready").parent
            )
        self.assertEqual(result["status"], "ready")
        self.assertEqual(result["actions"], [])
        # 全量轮：15 个分类全部入索引（含 defs/deprecated_interface/experimental）
        self.assertEqual(len(result["categories"]), 15)
        for marker in ("vector_compute", "deprecated_interface", "defs", "sync"):
            self.assertIn(marker, result["categories"])

    def test_skip_drift_avoids_rebuild(self) -> None:
        inst = self._copy_instance("skip")
        meta = json.loads((inst / "build-meta.json").read_text(encoding="utf-8"))
        meta["taxonomy_sha256"] = "0" * 64
        (inst / "build-meta.json").write_text(json.dumps(meta), encoding="utf-8")
        with self._patch_build_forbidden():
            result = self.ensure_mod.ensure(
                REPO_ROOT, "c_api", "9.2.0", "3510", skip_drift=True, index_root=inst.parent
            )
        self.assertEqual(result["actions"], [])

    def test_ensure_rebuilds_on_drift(self) -> None:
        inst = self._copy_instance("rebuild-drift")
        meta = json.loads((inst / "build-meta.json").read_text(encoding="utf-8"))
        meta["product_map_sha256"] = "0" * 64
        (inst / "build-meta.json").write_text(json.dumps(meta), encoding="utf-8")
        result = self.ensure_mod.ensure(REPO_ROOT, "c_api", "9.2.0", "3510", index_root=inst.parent)
        self.assertEqual(len(result["actions"]), 1)
        self.assertIn("product_map_sha256 drift", result["actions"][0])

    def test_ensure_rebuilds_on_gate_mismatch(self) -> None:
        inst = self._copy_instance("rebuild-a3")
        result = self.ensure_mod.ensure(REPO_ROOT, "c_api", "9.2.0", "A3", index_root=inst.parent)
        self.assertEqual(len(result["actions"]), 1)
        self.assertIn("arch 3510 != A3", result["actions"][0])
        self.assertEqual(result["arch"], "2201")

    def test_ensure_builds_missing_instance(self) -> None:
        result = self.ensure_mod.ensure(REPO_ROOT, "c_api", "9.2.0", "A3", index_root=self.out / "empty")
        self.assertEqual(result["status"], "ready")
        self.assertEqual(result["actions"], ["rebuilt: index.json missing"])
        self.assertGreater(result["doc_count"], 0)
        gaps = json.loads((self.out / "empty" / "c_api" / "gaps.json").read_text(encoding="utf-8"))
        self.assertIn("scope_out_gaps", gaps)


if __name__ == "__main__":
    unittest.main()
