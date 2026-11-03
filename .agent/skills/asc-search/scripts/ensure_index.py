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

"""OKF 索引前置保障：检索前确保存在与「本地 CANN × 架构 × 层级」匹配且新鲜的索引。

检索流程（references/retrieve.md）的步骤 0。检测链与 build_index 一致（--cann-version > $ASCEND_HOME_PATH >
version.cmake；--arch 默认 3510 经 product-map 校验）。committed `index/<surface>` 筛选条件
匹配且指纹新鲜时零动作；否则原地重建（产生本地 diff，是否提交由维护流程决定）。
指纹默认轻量档（build-meta 哈希 + scope 文档数），--strict 走全量重建比对。
"""

from __future__ import annotations

import argparse
import hashlib
import json
import subprocess
import sys
import tempfile
from pathlib import Path, PurePosixPath
from typing import Any

SCRIPTS_DIR = Path(__file__).resolve().parent
sys.path.insert(0, str(SCRIPTS_DIR))

import build_index  # noqa: E402
from derive import gate  # noqa: E402
from derive.slim_header import TAXONOMY_PATH  # noqa: E402

INDEX_DIR = build_index.INDEX_DIR
PRODUCT_MAP_PATH = build_index.PRODUCT_MAP_PATH
REPO_ROOT = build_index.REPO_ROOT


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def gate_reasons(meta: dict[str, Any], surface: str, cann_version: str, arch_label: str) -> list[str]:
    reasons = []
    if meta.get("surface") != surface:
        reasons.append(f"surface {meta.get('surface')!r} != {surface!r}")
    if meta.get("cann_version") != cann_version:
        reasons.append(f"cann {meta.get('cann_version')} != {cann_version}")
    if meta.get("arch_label") != arch_label:
        reasons.append(f"arch {meta.get('arch_label')} != {arch_label}")
    return reasons


def scope_documents(root: Path, scope: build_index.Scope) -> list[str]:
    """scope 内全部 .md（与 load_documents 同一范围语义，含未打头文档）。"""
    base = len(PurePosixPath(scope.root).parts)
    out = []
    for path in sorted((root / scope.root).rglob("*.md")):
        rel = path.relative_to(root).as_posix()
        parts = PurePosixPath(rel).parts
        if scope.categories is not None and (len(parts) <= base or parts[base] not in scope.categories):
            continue
        out.append(rel)
    return out


def light_fresh(instance: Path, root: Path, scope: build_index.Scope) -> tuple[bool, list[str]]:
    """轻量指纹：build-meta 哈希逐项 + scope 文档数，毫秒级。"""
    reasons: list[str] = []
    meta_path = instance / "build-meta.json"
    if not meta_path.is_file():
        return False, ["build-meta.json missing"]
    fingerprint = json.loads(meta_path.read_text(encoding="utf-8"))
    for key, path in (
        ("taxonomy_sha256", TAXONOMY_PATH),
        ("product_map_sha256", PRODUCT_MAP_PATH),
        ("gaps_sha256", instance / "gaps.json"),
    ):
        expected = fingerprint.get(key)
        if expected is None:
            reasons.append(f"build-meta missing {key}")
        elif expected != sha256(path):
            reasons.append(f"{key} drift: {path.name} changed since build")
    if fingerprint.get("input_sha256") != build_index.input_fingerprints(root):
        reasons.append("include/examples/scripts/overrides inputs changed since build")
    docs = fingerprint.get("document_sha256")
    if not docs:
        reasons.append("build-meta has no document_sha256")
        docs = {}
    for rel, digest in sorted(docs.items()):
        path = root / rel
        if not path.is_file():
            reasons.append(f"document removed: {rel}")
        elif sha256(path) != digest:
            reasons.append(f"document changed: {rel}")
    index_meta = json.loads((instance / "index.json").read_text(encoding="utf-8"))["meta"]
    expected_count = (
        index_meta["doc_count"]
        + index_meta["pending_no_okf"]
        + index_meta["filtered_by_version"]
        + index_meta["filtered_by_arch"]
    )
    actual = len(scope_documents(root, scope))
    if actual != expected_count:
        reasons.append(f"scope document count {actual} != {expected_count} (docs added/removed since build)")
    return (not reasons), reasons


def strict_fresh(
    instance: Path,
    root: Path,
    scope: build_index.Scope,
    cann_version: str,
    arch_label: str,
    source_commit: str | None = None,
) -> tuple[bool, list[str]]:
    """全量重建比对（与 build_index --check 同语义，按目标筛选条件参数化）。"""
    with tempfile.TemporaryDirectory() as tmp:
        for target in build_index.build(root, scope, Path(tmp), cann_version, arch_label, source_commit=source_commit):
            if target.name.endswith(".json"):
                committed = build_index.strip_informational(
                    json.loads((instance / target.name).read_text(encoding="utf-8"))
                )
                rebuilt = build_index.strip_informational(json.loads(target.read_text(encoding="utf-8")))
            else:
                committed = (instance / target.name).read_text(encoding="utf-8")
                rebuilt = target.read_text(encoding="utf-8")
            if committed != rebuilt:
                return False, [f"strict drift in {target.name}"]
    return True, []


def ensure(
    root: Path,
    surface: str,
    cann_version: str,
    arch_label: str,
    *,
    skip_drift: bool = False,
    strict: bool = False,
    index_root: Path | None = None,
    source_commit: str | None = None,
) -> dict[str, Any]:
    actions: list[str] = []
    scope = build_index.SCOPES[surface]
    instance = (index_root if index_root is not None else INDEX_DIR) / surface
    product_map = gate.load_product_map(PRODUCT_MAP_PATH)
    arch = gate.arch_label_to_number(arch_label, product_map)
    index_path = instance / "index.json"
    if index_path.is_file():
        meta = json.loads(index_path.read_text(encoding="utf-8"))["meta"]
        reasons = gate_reasons(meta, surface, cann_version, arch_label)
        if source_commit is not None and meta.get("source_commit") != source_commit:
            reasons.append("source commit differs from requested snapshot")
    else:
        reasons = ["index.json missing"]
    if not reasons and not skip_drift:
        if strict:
            ok, drift = strict_fresh(instance, root, scope, cann_version, arch_label, source_commit)
        else:
            ok, drift = light_fresh(instance, root, scope)
        if not ok:
            reasons = drift
    if reasons:
        build_index.build(root, scope, instance, cann_version, arch_label, source_commit=source_commit)
        actions.append("rebuilt: " + "; ".join(reasons))
    index = json.loads(index_path.read_text(encoding="utf-8"))
    return {
        "status": "ready",
        "index_dir": str(instance),
        "surface": surface,
        "cann_version": cann_version,
        "arch_label": arch_label,
        "arch": arch,
        "categories": sorted(k for k in index["views"]["by_category"] if k != "__root__"),
        "doc_count": index["meta"]["doc_count"],
        "pending_no_okf": index["meta"]["pending_no_okf"],
        "actions": actions,
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=REPO_ROOT)
    parser.add_argument("--surface", default="c_api", choices=sorted(build_index.SCOPES))
    parser.add_argument("--cann-version", default=None, help="默认自动检测（$ASCEND_HOME_PATH > version.cmake）")
    parser.add_argument("--arch", default="3510", help="架构号或检索标签（默认 3510，经 product-map 校验）")
    parser.add_argument("--skip-drift", action="store_true", help="只校验筛选条件匹配，跳过指纹漂移校验")
    parser.add_argument("--strict", action="store_true", help="全量重建比对（比轻量指纹精确，更慢）")
    parser.add_argument("--index-root", type=Path, default=None, help="索引实例根目录（默认 skill/index）")
    parser.add_argument("--source-commit", default=None, help="外部快照的固定提交")
    parser.add_argument("--json", action="store_true", help="机器可读输出")
    args = parser.parse_args()
    try:
        cann_version = gate.detect_cann_version(args.cann_version, args.root)
        result = ensure(
            args.root,
            args.surface,
            cann_version,
            args.arch,
            skip_drift=args.skip_drift,
            strict=args.strict,
            index_root=args.index_root,
            source_commit=args.source_commit,
        )
    except (build_index.BuildError, ValueError, OSError, KeyError, subprocess.CalledProcessError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1
    if args.json:
        print(json.dumps(result, ensure_ascii=False, indent=1))
        return 0
    scope_desc = "+".join(result["categories"]) or "all"
    note = f"; {'; '.join(result['actions'])}" if result["actions"] else ""
    print(
        f"[ensure] ready surface={result['surface']} cann={result['cann_version']} "
        f"arch={result['arch_label']}({result['arch']}) docs={result['doc_count']} "
        f"scope={scope_desc} index={result['index_dir']}{note}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
