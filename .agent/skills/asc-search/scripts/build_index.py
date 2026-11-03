#!/usr/bin/env python3
# -----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# -----------------------------------------------------------------------------------------------------------

"""OKF 索引构建：slim 头 + include 头 + examples 头 → 按适用条件筛选的持久索引。

管线：解析文档头 → 三源推导（include 声明/Doxygen、examples marker、正文）→
筛选条件（CANN 版本 × 架构 × 层级）→ 视图 → 依赖图 → 落盘。
构建以脚本为主，残留进 unresolved.json，由 derive-overrides 裁决后确定性消费；
同输入两次构建字节一致（meta 中的信息性字段除外）。
"""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import subprocess
import sys
import tempfile
from dataclasses import dataclass
from pathlib import Path, PurePosixPath
from typing import Any

SCRIPTS_DIR = Path(__file__).resolve().parent
SKILL_ROOT = SCRIPTS_DIR.parent
REPO_ROOT = SKILL_ROOT.parents[2]
sys.path.insert(0, str(SCRIPTS_DIR))

from derive import docparse, gate, overrides as overrides_mod  # noqa: E402
from derive.versioning import page_available, signature_available, deprecation  # noqa: E402
from derive.examples_db import build_examples_db  # noqa: E402
from derive.include_db import Declaration, build_include_db, declarations_for  # noqa: E402
from derive.slim_header import (  # noqa: E402
    DEPENDENCY_KINDS,
    TAXONOMY_PATH,
    ValidationError,
    split_front_matter,
    taxonomy,
)

SKILL_ROOT = SCRIPTS_DIR.parent
INDEX_DIR = SKILL_ROOT / "index"
PRODUCT_MAP_PATH = SKILL_ROOT / "config" / "product-map.yaml"
STAGES = ("in", "out", "compute")
# 数据位置深度：数值越大越靠近计算核，用于按参数方向判定搬入/搬出。
LOCATION_DEPTH = {
    "global_memory": 0,
    "l1_buffer": 1,
    "l0a_buffer": 2,
    "l0b_buffer": 2,
    "l0c_buffer": 2,
    "unified_buffer": 3,
    "bias_table_buffer": 3,
    "fixpipe_buffer": 3,
    "vector_register": 4,
    "mask_register": 4,
    "register_tensor": 4,
}
API_CLASS_TO_MODEL = {
    "vector_compute": "memory_vector",
    "vector_datamove": "memory_vector",
    "reg_compute": "register_vector",
    "scalar_compute": "scalar",
    "cube_compute": "matrix_compute",
    "cube_datamove": "matrix_compute",
}
EXECUTION_MODEL_AREAS = {
    "vector_compute",
    "vector_datamove",
    "reg_compute",
    "scalar_compute",
    "cube_compute",
    "cube_datamove",
}
DTYPE_ALIASES = {
    "f16": "half",
    "fp16": "half",
    "f32": "float",
    "fp32": "float",
    "bf16": "bfloat16",
    "int": "int32",
    "uint": "uint32",
    "int4b": "int4",
    "int4x2": "int4",
    "b8": "int8",
    "e4m3": "fp8_e4m3fn",
    "e5m2": "fp8_e5m2",
    "e8m0": "fp8_e8m0",
    "hif8": "hifloat8",
    "e1m2x2": "fp4x2_e1m2",
    "e2m1x2": "fp4x2_e2m1",
}
CONVERT_RE = re.compile(r"asc_(?P<src>[a-z0-9]+)2(?P<dst>[a-z0-9]+)$")
PAIR_TOKENS = {
    "half",
    "float",
    "bfloat16",
    "int4",
    "int8",
    "uint8",
    "int16",
    "uint16",
    "int32",
    "uint32",
    "int64",
    "uint64",
    "fp8_e4m3fn",
    "fp8_e5m2",
    "fp8_e8m0",
    "hifloat8",
    "fp4x2_e1m2",
    "fp4x2_e2m1",
}
LOW_PRECISION = {"fp8_e4m3fn", "fp8_e5m2", "fp8_e8m0", "hifloat8", "fp4x2_e1m2", "fp4x2_e2m1"}


@dataclass(frozen=True)
class Scope:
    name: str
    root: str
    root_nav: str
    categories: tuple[str, ...] | None  # None = 全量；试点期限定子目录


SCOPES: dict[str, Scope] = {
    # c_api 全量（vector_* 试点轮已完成，categories=None 放开全部子目录）
    "c_api": Scope("c_api", "docs/zh/api/SIMD-API/c_api", "c_api.md", None)
}


class BuildError(ValueError):
    pass


def normalize_dtype(value: str) -> str:
    token = value[:-2] if value.endswith("_t") else value
    if token == "int4b":
        token = "int4"
    return DTYPE_ALIASES.get(token, token)


def is_arch_page(path: str) -> bool:
    return re.fullmatch(r".+_arch_\d+(?:_deprecated)?", PurePosixPath(path).stem) is not None


def family_of(stem: str) -> str:
    """家族名：剥离 _arch_NNNN(_deprecated) 变体后缀与纯 _deprecated 后缀。

    纯后缀页（如 asc_set_gm2ub_pad_deprecated.md）按真实 API 名回查 include 声明。
    """
    stripped = re.sub(r"_arch_\d+(?:_deprecated)?$", "", stem)
    if stripped == stem:
        stripped = re.sub(r"_deprecated$", "", stem)
    return stripped


# ---------------------------------------------------------------- 文档加载


VERSION_CHANGE_REQUIRED_KEYS = ("since", "kind", "scope")
VERSION_CHANGE_OPTIONAL_KEYS = ("note", "scope_basis")
# 索引内是**降维副本**：事实源在文档头；索引只保留倒排与展示字段。
VERSION_CHANGE_INDEX_KEYS = VERSION_CHANGE_REQUIRED_KEYS + VERSION_CHANGE_OPTIONAL_KEYS
VERSION_CHANGE_KEYS = VERSION_CHANGE_INDEX_KEYS


def normalize_version_changes(rel: str, value: Any, errors: list[str]) -> list[dict[str, Any]]:
    """front matter 第 6 字段 version_changes 的结构校验 + 透传（宽进严出）。

    只校验**结构**：必须是 list、每项必须是 mapping、必填键存在且类型为 str
    （可选键给 null 视为缺省）。**不校验 kind 取值**（冻结词表只做展示/一致性副本，
    未知 kind 透传）——否则建设侧新增 kind 会让检索侧整次构建 BuildError。
    结构不合法计入 errors，由 build() 统一拒绝出索引。
    """
    if value is None:
        return []
    if not isinstance(value, list):
        errors.append(f"{rel}: version_changes must be a list, got {type(value).__name__}")
        return []
    events: list[dict[str, Any]] = []
    for index, item in enumerate(value):
        where = f"{rel}: version_changes[{index}]"
        if not isinstance(item, dict):
            errors.append(f"{where} must be a mapping, got {type(item).__name__}")
            continue
        missing = sorted(key for key in VERSION_CHANGE_REQUIRED_KEYS if item.get(key) is None)
        if missing:
            errors.append(f"{where} required key(s) missing or null: {missing}")
            continue
        bad_types = sorted(
            key for key in VERSION_CHANGE_INDEX_KEYS if item.get(key) is not None and not isinstance(item.get(key), str)
        )
        if bad_types:
            errors.append(f"{where} value(s) must be a string: {bad_types}")
            continue
        try:
            gate.version_key(item["since"])
        except ValueError as exc:
            errors.append(f"{where}: {exc}")
            continue
        events.append({key: item[key] for key in VERSION_CHANGE_INDEX_KEYS if item.get(key) is not None})
    return events


def load_documents(root: Path, scope: Scope, errors: list[str]) -> tuple[dict[str, dict[str, Any]], list[str]]:
    namespaces, tax_values = taxonomy(TAXONOMY_PATH)
    documents: dict[str, dict[str, Any]] = {}
    pending: list[str] = []
    for path in sorted((root / scope.root).rglob("*.md")):
        rel = path.relative_to(root).as_posix()
        parts = PurePosixPath(rel).parts
        base = len(PurePosixPath(scope.root).parts)
        if scope.categories is not None and (len(parts) <= base or parts[base] not in scope.categories):
            continue
        fm, _, body = split_front_matter(path.read_text(encoding="utf-8"))
        if not fm:
            pending.append(rel)
            continue
        allowed = {"okftype", "description", "tag", "dependencies", "version", "version_changes"}
        extra = sorted(set(fm) - allowed)
        if extra:
            errors.append(
                f"{rel}: fields outside slim header (violates references/index_derivation_contract.md): {extra}"
            )
            continue
        tags: dict[str, list[str]] = {}
        for item in fm.get("tag") or []:
            for ns, values in item.items():
                if ns not in namespaces:
                    errors.append(f"{rel}: unknown tag namespace {ns}")
                    continue
                if not values:
                    errors.append(f"{rel}: empty tag namespace rendered: {ns}")
                    continue
                unknown = sorted(set(values) - tax_values.get(ns, set()))
                if unknown:
                    errors.append(f"{rel}: tag {ns} values outside taxonomy: {unknown}")
                tags[ns] = sorted(set(values))
        if "roles" in fm.get("tag", [{}]) or tags:
            pass
        deps = []
        for dep in fm.get("dependencies") or []:
            if dep.get("kind") not in DEPENDENCY_KINDS:
                errors.append(f"{rel}: invalid dependency kind {dep.get('kind')!r}")
                continue
            deps.append({"path": dep["path"], "kind": dep["kind"], "condition": dep.get("condition", "")})
        version = fm.get("version", "")
        normalized_version = version
        if version:
            try:
                normalized_version = gate.first_support(version, allow_legacy=True)
            except ValueError as exc:
                errors.append(f"{rel}: {exc}")
        stem = PurePosixPath(rel).stem
        documents[rel] = {
            "okftype": fm["okftype"],
            "description": fm.get("description", ""),
            "tags": tags,
            "dependencies": deps,
            "version": normalized_version,
            "legacy_version": version if version != normalized_version else "",
            "version_changes": normalize_version_changes(rel, fm.get("version_changes"), errors),
            "title": docparse.doc_title(body, stem),
            "navigation": docparse.doc_navigation(body),
            "product_support": docparse.doc_product_support(body),
            "body": body,
        }
    return documents, pending


# ---------------------------------------------------------------- 三源推导


PROTOTYPE_BLOCK_RE = re.compile(r"```(?:cpp|c)\n(?P<body>.*?)```", re.DOTALL)
PROTO_NAME_RE = re.compile(r"\b(asc_[a-z0-9_]+)\s*\(")
PROTOTYPE_SECTION_RE = re.compile(r"(?ms)^#{2,6}\s+函数(?:原型|声明)(?P<section>.*?)(?=^#{1,6}\s|\Z)")


def _prototype_api_names(body: str) -> list[str]:
    names: set[str] = set()
    for section in PROTOTYPE_SECTION_RE.finditer(body):
        for block in PROTOTYPE_BLOCK_RE.finditer(section.group("section")):
            names.update(PROTO_NAME_RE.findall(block.group("body")))
    return sorted(names)


def derive_signatures(
    rel: str,
    doc: dict[str, Any],
    include_db: dict[str, Any],
    area: str,
    examples_db: dict[str, Any],
    unresolved: list[dict[str, Any]],
    page_arch: str = "",
) -> dict[str, Any]:
    stem = PurePosixPath(rel).stem
    family = family_of(stem)
    declarations, mode = declarations_for(family, include_db, area)
    if mode == "none":
        # 回退：家族页名不带序号（如 asc_set_ndim_loop_stride 覆盖 loop0-3_stride），
        # 从正文函数原型代码块提取真实 API 名再查 include。
        proto_names = [n for n in _prototype_api_names(doc["body"]) if n in include_db["functions"]]
        if proto_names:
            declarations = []
            for name in proto_names:
                declarations.extend(include_db["functions"][name])
            mode = "body_prototypes"
    derived: dict[str, Any] = {
        "api": family,
        "api_class": area,
        "signatures": [],
        "archs": [],
        "headers": [],
        "examples": [],
        "params": [],
        "enum_types": [],
        "deprecated": None,
    }
    if mode == "none":
        unresolved.append({"document": rel, "field": "signatures", "issue": "no declaration found in include/c_api"})
        # 无声明也继续：archs 交给正文 npu 标记兜底（保守保留改为按证据筛选）。
    silent = 0
    for declaration in declarations:
        raw_archs = list(declaration.archs)
        if page_arch:
            # 架构变体页只呈现本架构的重载：无 @archs 声明继承页面架构，
            # 带其他架构注解的声明不属于本页。
            if raw_archs and page_arch not in raw_archs:
                continue
            if not raw_archs:
                declaration = Declaration(
                    name=declaration.name,
                    header=declaration.header,
                    signature=declaration.signature,
                    return_type=declaration.return_type,
                    params=declaration.params,
                    override_id=declaration.override_id,
                    archs=[page_arch],
                    ingroup=declaration.ingroup,
                    deprecated=declaration.deprecated,
                )
        entry = declaration.to_dict()
        if not entry["override_id"]:
            silent += 1
        derived["signatures"].append(entry)
        derived["archs"].extend(entry["archs"])
        derived["headers"].append(declaration.header)
        if entry["deprecated"]:
            derived["deprecated"] = entry["deprecated"]
        for param in entry["params"]:
            derived["params"].append(
                {
                    "name": param["name"],
                    "type": param["type"],
                    "address_space": param.get("address_space", ""),
                    "direction": param.get("direction", ""),
                    "override_id": entry["override_id"],
                }
            )
            if param["type"] in include_db["enums"]:
                derived["enum_types"].append(param["type"])
        if entry["override_id"]:
            for example in examples_db.get(entry["override_id"], []):
                derived["examples"].append({**example, "override_id": entry["override_id"]})
    if silent:
        unresolved.append(
            {
                "document": rel,
                "field": "signatures",
                "issue": f"{silent} declaration(s) without @override_id (no example link; dtypes/params still derived)",
            }
        )
    derived["archs"] = sorted(set(derived["archs"]))
    if page_arch and derived["signatures"]:
        # 架构变体页只呈现本架构视图：声明 @archs 含多架构（如废弃接口 2201+3510）
        # 时也按页面架构筛选，防止变体页漏入其他架构索引。
        derived["archs"] = [page_arch]
    derived["headers"] = sorted(set(derived["headers"]))
    derived["params"] = list({(p["name"], p["type"], p["override_id"]): p for p in derived["params"]}.values())
    derived["enum_types"] = sorted(set(derived["enum_types"]))
    derived["examples"] = list({(e["path"], e["file"]): e for e in derived["examples"]}.values())
    if not derived["archs"]:
        fallback = []
        for marker, info in doc["product_support"].items():
            if info["support"] == "支持":
                arch = gate.npu_to_arch(marker, gate.load_product_map(PRODUCT_MAP_PATH))
                if arch:
                    fallback.append(arch)
        if fallback:
            derived["archs"] = sorted(set(fallback))
            unresolved.append(
                {
                    "document": rel,
                    "field": "archs",
                    "issue": f"archs derived from body npu markers (include has no @archs): {sorted(set(fallback))}",
                }
            )
        else:
            unresolved.append({"document": rel, "field": "archs", "issue": "archs unknown"})
    return derived


def classify_documents(documents: dict[str, dict[str, Any]], scope: Scope) -> None:
    base = len(PurePosixPath(scope.root).parts)
    for rel, doc in documents.items():
        parts = PurePosixPath(rel).parts
        stem = parts[-1][: -len(".md")]
        doc["category"] = parts[base] if len(parts) > base + 1 else "__root__"
        if len(parts) == base + 1:
            doc["nav_page"] = f"{scope.root}/{scope.root_nav}"
        elif len(parts) == base + 2:
            doc["nav_page"] = f"{scope.root}/{parts[base]}/{parts[base]}.md"
        else:
            directory = str(PurePosixPath(rel).parent)
            landing = [
                other
                for other in documents
                if str(PurePosixPath(other).parent) == directory
                and documents[other]["okftype"] == "reference.index"
                and PurePosixPath(other).stem == PurePosixPath(directory).name
            ]
            doc["nav_page"] = landing[0] if landing else f"{scope.root}/{parts[base]}/{parts[base]}.md"
        if is_arch_page(rel):
            match = re.fullmatch(r"(?P<family>.+)_arch_(?P<arch>\d+)(?:_deprecated)?", stem)
            doc["family"] = doc["arch_of"] = match.group("family")
            doc["page_arch"] = match.group("arch")
        elif doc["okftype"] == "reference.index":
            doc["family"] = stem if len(parts) > base + 2 else "__nav__"
        else:
            doc["family"] = family_of(stem)
        doc["standalone_primary"] = False


def param_location(param: dict[str, Any]) -> str:
    if param.get("address_space"):
        return param["address_space"]
    if param.get("type") == "ub_addr8_t":
        return "unified_buffer"
    if param.get("is_vector"):
        return "mask_register" if param.get("is_mask") else "vector_register"
    return ""


def signature_stages(signature: dict[str, Any]) -> set[str]:
    """单个声明：按参数方向（方向缺失时按 dst 首参约定）判定数据流动方向。"""
    located = [(param, param_location(param)) for param in signature.get("params", [])]
    located = [(param, loc) for param, loc in located if loc]
    if not located:
        return set()
    in_locs = [loc for param, loc in located if "in" in (param.get("direction") or "")]
    out_locs = [loc for param, loc in located if "out" in (param.get("direction") or "")]
    if not in_locs and not out_locs:
        out_locs, in_locs = [located[0][1]], [loc for _, loc in located[1:]]
    if not in_locs or not out_locs:
        return set()
    in_depth = [LOCATION_DEPTH[loc] for loc in in_locs if loc in LOCATION_DEPTH]
    out_depth = [LOCATION_DEPTH[loc] for loc in out_locs if loc in LOCATION_DEPTH]
    if not in_depth or not out_depth:
        return set()
    stages = set()
    if max(out_depth) > min(in_depth):
        stages.add("in")
    if min(out_depth) < max(in_depth):
        stages.add("out")
    return stages


def stage_of(doc: dict[str, Any]) -> list[str]:
    roles = set(doc["tags"].get("roles", []))
    stages: set[str] = set()
    if "data_movement" in roles:
        for signature in doc.get("derived_signatures", []):
            stages |= signature_stages(signature)
    if "compute" in roles:
        stages.add("compute")
    return sorted(stages)


def conversion_pair(rel: str, doc: dict[str, Any]) -> str | None:
    if "conversion" not in doc["tags"].get("operations", []):
        return None
    stem = family_of(PurePosixPath(rel).stem)
    body = stem[len("asc_") :] if stem.startswith("asc_") else ""
    for cut in (i for i, ch in enumerate(body) if ch == "2"):
        src, dst = normalize_dtype(body[:cut]), normalize_dtype(body[cut + 1 :])
        if src in PAIR_TOKENS and dst in PAIR_TOKENS:
            return f"{src}->{dst}"
    return None


def features_of(doc: dict[str, Any]) -> list[str]:
    dtypes = set(doc.get("derived_dtypes", []))
    operations = set(doc["tags"].get("operations", []))
    features = []
    low = dtypes & LOW_PRECISION
    if low:
        features.append("low_precision")
        if "matrix_multiply_accumulate" in operations:
            features.append("low_precision_matrix")
        if "conversion" in operations:
            features.append("low_precision_conversion")
    if "int4" in dtypes:
        features.append("int4")
    return sorted(features)


def check_graph(documents: dict[str, dict[str, Any]], errors: list[str], root: Path) -> list[dict[str, Any]]:
    edges: dict[tuple[str, str], dict[str, Any]] = {}
    for rel, doc in documents.items():
        for dep in doc["dependencies"]:
            edges[(rel, dep["path"])] = {
                "from": rel,
                "to": dep["path"],
                "kind": dep["kind"],
                "condition": dep["condition"],
            }
    for rel, doc in documents.items():
        for dep in doc["dependencies"]:
            target = dep["path"]
            if target not in documents and not (root / target).is_file():
                errors.append(f"{rel}: dependency target not found: {target}")
    return [edges[key] for key in sorted(edges)]


def build_views(documents: dict[str, dict[str, Any]]) -> dict[str, Any]:
    views: dict[str, dict[str, list[str]]] = {
        "by_stage": {stage: [] for stage in STAGES},
        "by_role": {},
        "by_operation": {},
        "by_category": {},
        "by_api_class": {},
        "by_dtype": {},
        "by_dtype_pair": {},
        "by_override_id": {},
        "by_header": {},
        "by_api": {},
        "by_param": {},
        "by_enum": {},
        "by_feature": {},
        "by_version": {},
        "by_change_version": {},
        "by_product": {},
    }
    for rel, doc in documents.items():
        if doc["family"] == "__nav__":
            continue
        for stage in stage_of(doc):
            views["by_stage"][stage].append(rel)
        for role in doc["tags"].get("roles", []):
            views["by_role"].setdefault(role, []).append(rel)
        for op in doc["tags"].get("operations", []):
            views["by_operation"].setdefault(op, []).append(rel)
        views["by_category"].setdefault(doc["category"], []).append(rel)
        if doc.get("derived_api_class"):
            views["by_api_class"].setdefault(doc["derived_api_class"], []).append(rel)
        for dtype in doc.get("derived_dtypes", []):
            views["by_dtype"].setdefault(dtype, []).append(rel)
        for signature in doc.get("derived_signatures", []):
            if signature["override_id"]:
                views["by_override_id"].setdefault(signature["override_id"], []).append(rel)
        for header in doc.get("derived_headers", []):
            views["by_header"].setdefault(header, []).append(rel)
        for param in doc.get("derived_params", []):
            views["by_param"].setdefault(param["name"], []).append(rel)
            if param["type"].startswith("asc_"):
                views["by_param"].setdefault(param["type"], []).append(rel)
        for enum_type in doc.get("derived_enum_types", []):
            views["by_enum"].setdefault(enum_type, []).append(rel)
        for feature in features_of(doc):
            views["by_feature"].setdefault(feature, []).append(rel)
        if doc.get("version"):
            views["by_version"].setdefault(doc["version"], []).append(rel)
        # 版本变更事件视图：{since: [文档]}。只收非空事件（version_changes: [] 不入任何键）；
        # 与 by_version（version 区间原串）语义不同，不参与门控。
        for event in doc.get("version_changes", []):
            since = event.get("since", "")
            if since:
                views["by_change_version"].setdefault(since, []).append(rel)
        for marker, info in doc.get("product_support", {}).items():
            if info["support"] == "支持":
                views["by_product"].setdefault(marker, []).append(rel)
        pair = conversion_pair(rel, doc)
        if pair:
            views["by_dtype_pair"].setdefault(pair, []).append(rel)
        views["by_api"].setdefault(doc["family"], []).append(rel)
    return {name: {key: sorted(set(paths)) for key, paths in view.items()} for name, view in views.items()}


# ---------------------------------------------------------------- 构建主流程


def git_output(args: list[str], root: Path = REPO_ROOT) -> str:
    return subprocess.run(["git", *args], cwd=root, capture_output=True, text=True, check=True).stdout.strip()


def source_identity(root: Path, source_commit: str | None) -> tuple[str, str]:
    if source_commit:
        return source_commit, ""
    try:
        if Path(git_output(["rev-parse", "--show-toplevel"], root)).resolve() != root.resolve():
            return "unversioned", ""
        return git_output(["rev-parse", "HEAD"], root), git_output(["log", "-1", "--format=%cI"], root)
    except (subprocess.CalledProcessError, OSError):
        return "unversioned", ""


def overview(index: dict[str, Any], enums: dict[str, Any]) -> str:
    meta = index["meta"]
    lines = [
        "# OKF 索引概览（构建副产物，勿手改）",
        "",
        f"- 筛选条件：CANN {meta['cann_version']} × 架构 {meta['arch_label']}（{meta['arch']}）× 层级 {meta['surface']}",
        f"- 索引文档：{meta['doc_count']}（主文档 {meta['primary_count']}、导航页 {meta['nav_page_count']}）",
        f"- 版本过滤：{meta['filtered_by_version']} 篇；架构过滤：{meta['filtered_by_arch']} 篇",
        f"- 范围内无 slim 头的文档：{meta['pending_no_okf']} 篇",
        f"- 推导残留（unresolved）：{meta['unresolved_count']} 条（derive-overrides 裁决）",
        f"- defs 目录：枚举 {len(enums)} 个；参数反查视图 {len(index['views']['by_param'])} 项",
        "",
        "## 分阶段候选规模",
        "",
        "| 阶段 | 主文档数 |",
        "| --- | ---: |",
    ]
    for stage, paths in index["views"]["by_stage"].items():
        lines.append(f"| {stage} | {len(paths)} |")
    lines += ["", "## 分类视图", "", "| 分类 | 主文档数 |", "| --- | ---: |"]
    for category, paths in index["views"]["by_category"].items():
        lines.append(f"| {category} | {len(paths)} |")
    lines += ["", "## 特性视图", "", "| 特性 | 主文档数 |", "| --- | ---: |"]
    for feature, paths in index["views"]["by_feature"].items():
        lines.append(f"| {feature} | {len(paths)} |")
    lines.append("")
    return "\n".join(lines)


def input_fingerprints(root: Path) -> dict[str, str]:
    sources = list((root / "include/c_api").rglob("*.h"))
    sources += [
        p
        for p in (root / "examples").rglob("*")
        if p.is_file() and p.suffix.lower() in {".asc", ".c", ".cc", ".cpp", ".cxx", ".h", ".hpp"}
    ]
    scripts = list((SKILL_ROOT / "scripts").rglob("*.py"))
    scripts += list((SKILL_ROOT / "derive-overrides").glob("*.json"))
    result = {
        "repo/" + p.relative_to(root).as_posix(): hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(sources)
    }
    result.update(
        {
            "skill/" + p.relative_to(SKILL_ROOT).as_posix(): hashlib.sha256(p.read_bytes()).hexdigest()
            for p in sorted(scripts)
        }
    )
    return result


def build(
    root: Path,
    scope: Scope,
    out_dir: Path,
    cann_version: str,
    arch_label: str,
    *,
    product_map_path: Path | None = None,
    gaps_path: Path | None = None,
    source_commit: str | None = None,
) -> list[Path]:
    gate.version_key(cann_version)
    errors: list[str] = []
    unresolved: list[dict[str, Any]] = []
    product_map = gate.load_product_map(product_map_path or PRODUCT_MAP_PATH)
    arch = gate.arch_label_to_number(arch_label, product_map)

    documents, pending = load_documents(root, scope, errors)
    classify_documents(documents, scope)
    include_db = build_include_db(root / "include" / "c_api")
    # examples DB 覆盖整棵 examples/：marker 由建设侧按 canonical `override_id` 写入，
    # 而查表是「文档侧 include DB 的 override_id → examples DB」，没有 marker 的样例
    # 不可能被查到 —— 因此放宽根目录不会把别的 API 家族（如 SIMT 同名接口）关联进来。
    # 实测：全仓仅 2 个样例带 marker，SIMT 样例（03_simt_api/05_simd_simt_hybrid）0 个。
    examples_db = build_examples_db(root / "examples")
    override_map = overrides_mod.load_overrides(SKILL_ROOT / "derive-overrides" / f"{scope.name}.json")

    gated: dict[str, dict[str, Any]] = {}
    filtered_by_version = filtered_by_arch = 0
    for rel, doc in sorted(documents.items()):
        area = doc["category"] if doc["category"] in EXECUTION_MODEL_AREAS else None
        page_arch = doc.get("page_arch", "")
        derived = (
            derive_signatures(rel, doc, include_db, area, examples_db, unresolved, page_arch=page_arch)
            if doc["okftype"] == "reference.api"
            else {
                "api": "",
                "api_class": "",
                "signatures": [],
                "archs": [],
                "headers": [],
                "examples": [],
                "params": [],
                "enum_types": [],
                "deprecated": None,
            }
        )
        applied = overrides_mod.apply_overrides(rel, derived, override_map)
        if doc["legacy_version"]:
            unresolved.append(
                {
                    "document": rel,
                    "field": "version",
                    "issue": f"legacy upper bound ignored: {doc['legacy_version']}; review first support and lifecycle events",
                }
            )
        is_entity = doc["okftype"] != "reference.index"
        if is_entity:
            if (doc["version"] and not gate.version_covers(doc["version"], cann_version)) or not page_available(
                doc["version_changes"], cann_version
            ):
                filtered_by_version += 1
                continue
            if derived["archs"] and arch not in derived["archs"]:
                filtered_by_arch += 1
                continue
            if derived["signatures"]:
                eligible = [s for s in derived["signatures"] if not s["archs"] or arch in s["archs"]]
                derived["signatures"] = [
                    s for s in eligible if signature_available(s, doc["version_changes"], cann_version, arch)
                ]
                if eligible and not derived["signatures"]:
                    filtered_by_version += 1
                    continue
                for signature in derived["signatures"]:
                    signature["deprecated"] = deprecation(signature, doc["version_changes"], cann_version, arch)
            ids = {s["override_id"] for s in derived["signatures"]}
            derived["examples"] = [e for e in derived["examples"] if e["override_id"] in ids]
            derived["params"] = [p for s in derived["signatures"] for p in s["params"]]
            derived["enum_types"] = sorted({p["type"] for p in derived["params"] if p["type"] in include_db["enums"]})
            derived["headers"] = sorted({s["header"] for s in derived["signatures"]})
            derived["deprecated"] = next((s["deprecated"] for s in derived["signatures"] if s["deprecated"]), None)
        doc["derived"] = derived
        doc["derived_applied_overrides"] = applied
        doc["derived_api_class"] = derived["api_class"] or (area if area else ("defs" if "/defs/" in rel else ""))
        doc["derived_execution_model"] = API_CLASS_TO_MODEL.get(doc["derived_api_class"], "")
        doc["derived_signatures"] = derived["signatures"]
        doc["derived_dtypes"] = sorted({d for s in derived["signatures"] for d in s["dtypes"]})
        doc["derived_locations"] = sorted({loc for s in derived["signatures"] for loc in s["locations"]})
        doc["derived_masked"] = any(s["masked"] for s in derived["signatures"])
        doc["derived_deprecated"] = bool(derived["deprecated"]) or "deprecated" in doc["tags"].get("traits", [])
        doc["derived_examples"] = derived["examples"]
        doc["derived_headers"] = derived["headers"]
        doc["derived_params"] = derived["params"]
        doc["derived_enum_types"] = derived["enum_types"]

        gated[rel] = doc

    edges = check_graph(gated, errors, root)
    gaps, gaps_raw = load_gaps(scope, gaps_path=gaps_path)
    validate_gaps(gaps, errors, root)
    if errors:
        raise BuildError(f"{len(errors)} validation error(s):\n" + "\n".join(f"  - {e}" for e in errors[:30]))

    nav_pages = sorted(rel for rel, doc in gated.items() if doc["family"] == "__nav__")
    primary = sorted(rel for rel, doc in gated.items() if doc["family"] != "__nav__" or doc["standalone_primary"])
    views = build_views(gated)
    reverse: dict[str, dict[str, list[str]]] = {kind: {} for kind in sorted(DEPENDENCY_KINDS)}
    for edge in edges:
        if edge["to"] in gated:
            reverse[edge["kind"]].setdefault(edge["to"], []).append(edge["from"])

    resolved_source_commit, commit_date = source_identity(root, source_commit)
    index = {
        "schema": "okf-index/5",
        "meta": {
            "surface": scope.name,
            "source_commit": resolved_source_commit,
            "commit_date": commit_date,
            "cann_version": cann_version,
            "arch_label": arch_label,
            "arch": arch,
            "doc_count": len(gated),
            "primary_count": len(primary),
            "nav_page_count": len(nav_pages),
            "filtered_by_version": filtered_by_version,
            "filtered_by_arch": filtered_by_arch,
            "pending_no_okf": len(pending),
            "unresolved_count": len(unresolved),
            "gap_ids": [gap["id"] for gap in gaps.get("scope_out_gaps", [])],
        },
        "documents": {
            rel: {
                "okftype": doc["okftype"],
                "title": doc["title"],
                "description": doc["description"],
                "tags": doc["tags"],
                "dependencies": doc["dependencies"],
                "version": doc["version"],
                "version_changes": doc["version_changes"],
                "navigation": doc["navigation"],
                "product_support": {m: i["support"] for m, i in doc["product_support"].items()},
                "api_class": doc["derived_api_class"],
                "execution_model": doc["derived_execution_model"],
                "signatures": [{k: v for k, v in s.items() if k != "unresolved"} for s in doc["derived_signatures"]],
                "dtypes": doc["derived_dtypes"],
                "locations": doc["derived_locations"],
                "masked": doc["derived_masked"],
                "deprecated": doc["derived_deprecated"],
                "headers": doc["derived_headers"],
                "examples": doc["derived_examples"],
                "enum_types": doc["derived_enum_types"],
                "family": doc["family"],
                "category": doc["category"],
                "nav_page": doc["nav_page"],
                "applied_overrides": doc["derived_applied_overrides"],
            }
            # 注意：不落盘 per-doc params（与 signatures[].params 冗余）；
            # 参数反查经 by_param 视图 / signatures 推导。
            for rel, doc in sorted(gated.items())
        },
        "views": views,
        "graph": {"edges": edges, "reverse": reverse},
        "defs": {
            "enums": include_db["enums"],
            "constants": include_db["constants"],
            "macros": [m for m in include_db["macros"] if m["name"].startswith("ASC_")],
            "aliases": include_db["aliases"],
            "records": include_db["records"],
        },
        "scope": {"primary_docs": primary, "navigation_pages": nav_pages},
    }
    build_meta = {
        "schema": "okf-index-meta/2",
        "source_commit": index["meta"]["source_commit"],
        "taxonomy_sha256": hashlib.sha256(TAXONOMY_PATH.read_bytes()).hexdigest(),
        "product_map_sha256": hashlib.sha256(PRODUCT_MAP_PATH.read_bytes()).hexdigest(),
        "gaps_sha256": hashlib.sha256(gaps_raw).hexdigest(),
        "input_sha256": input_fingerprints(root),
        "document_sha256": {
            rel: hashlib.sha256((root / rel).read_bytes()).hexdigest() for rel in sorted(set(documents) | set(pending))
        },
    }
    out_dir.mkdir(parents=True, exist_ok=True)
    # gaps.json 可选（人工维护）：缺失时落盘空台账快照，保证 --out 构建的实例目录
    # 可被 query.py 直接加载。
    (out_dir / "gaps.json").write_bytes(gaps_raw)
    written: list[Path] = [out_dir / "gaps.json"]
    for name, payload in (
        # index.json 紧凑序列化（全量 592 篇 + 6550 声明，indent 会突破远端 10 MiB 单文件限制）
        ("index.json", json.dumps(index, ensure_ascii=False, separators=(",", ":")) + "\n"),
        ("build-meta.json", json.dumps(build_meta, ensure_ascii=False, indent=1) + "\n"),
        ("概览.md", overview(index, include_db["enums"])),
    ):
        target = out_dir / name
        target.write_text(payload, encoding="utf-8")
        written.append(target)
    overrides_mod.write_unresolved(out_dir, overrides_mod.collect_unresolved(unresolved))
    return written


EMPTY_GAPS: dict[str, Any] = {"scope_out_gaps": [], "relation_gaps": []}


def load_gaps(scope: Scope, *, gaps_path: Path | None = None) -> tuple[dict[str, Any], bytes]:
    """gaps.json 可选：缺失视为空台账；返回值同时用于指纹与快照落盘。"""
    path = gaps_path or (INDEX_DIR / scope.name / "gaps.json")
    raw = (
        path.read_text(encoding="utf-8")
        if path.is_file()
        else (json.dumps(EMPTY_GAPS, ensure_ascii=False, indent=1) + "\n")
    )
    return json.loads(raw), raw.encode("utf-8")


def validate_gaps(gaps: dict[str, Any], errors: list[str], root: Path = REPO_ROOT) -> None:
    for section in ("scope_out_gaps", "relation_gaps"):
        if section not in gaps:
            errors.append(f"gaps.json: missing section {section}")
    for gap in gaps.get("scope_out_gaps", []):
        for key in ("id", "keywords", "conclusion", "hints", "evidence"):
            if not gap.get(key):
                errors.append(f"gaps.json: gap {gap.get('id')} missing {key}")
        for hint in gap.get("hints", []):
            if not (root / hint["path"]).exists():
                errors.append(f"gaps.json: hint path not found: {hint['path']}")
        for ev in gap.get("evidence", []):
            if not (root / ev["path"]).exists():
                errors.append(f"gaps.json: evidence path not found: {ev['path']}")
    for gap in gaps.get("relation_gaps", []):
        if not (root / gap["from"]).is_file():
            errors.append(f"gaps.json: relation gap source not found: {gap['from']}")


INFORMATIONAL_META = ("source_commit", "commit_date")


def strip_informational(payload: dict) -> dict:
    stripped = json.loads(json.dumps(payload))
    for key in INFORMATIONAL_META:
        stripped.pop(key, None)
        stripped.get("meta", {}).pop(key, None)
    return stripped


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=REPO_ROOT)
    parser.add_argument("--scope", default="c_api", choices=sorted(SCOPES))
    parser.add_argument("--out", type=Path, default=None)
    parser.add_argument("--cann-version", default=None, help="默认自动检测（$ASCEND_HOME_PATH > version.cmake）")
    parser.add_argument("--arch", default="3510", help="架构号或检索标签（默认 3510，经 product-map 校验）")
    parser.add_argument("--source-commit", default=None, help="外部快照的固定提交；非 git 快照必须提供")
    parser.add_argument("--check", action="store_true", help="校验已提交索引与重建结果一致")
    args = parser.parse_args()
    scope = SCOPES[args.scope]
    out_dir = args.out if args.out is not None else INDEX_DIR / scope.name
    try:
        cann_version = gate.detect_cann_version(args.cann_version, args.root)
        if args.check:
            with tempfile.TemporaryDirectory() as tmp:
                fresh = build(args.root, scope, Path(tmp), cann_version, args.arch, source_commit=args.source_commit)
                for target in fresh:
                    if target.name.endswith(".json"):
                        committed = strip_informational(json.loads((out_dir / target.name).read_text(encoding="utf-8")))
                        rebuilt = strip_informational(json.loads(target.read_text(encoding="utf-8")))
                    else:
                        committed = (out_dir / target.name).read_text(encoding="utf-8")
                        rebuilt = target.read_text(encoding="utf-8")
                    if committed != rebuilt:
                        print(f"drift detected in {target.name}: index is stale, rerun build_index.py", file=sys.stderr)
                        return 1
            print("index is up to date")
            return 0
        for target in build(args.root, scope, out_dir, cann_version, args.arch, source_commit=args.source_commit):
            print(f"written: {target}")
        return 0
    except (BuildError, ValidationError, ValueError, subprocess.CalledProcessError, OSError, KeyError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
