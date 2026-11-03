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

"""OKF 索引查询 CLI。

索引实例按 surface 隔离（默认 c_api），构建时已按 CANN 版本 × 架构筛选条件。
过滤查询各维度 AND、逗号值内 OR；独立查询（--api/--deps-of/--enum/--gap）返回结构化卡片。
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path, PurePosixPath
from typing import Any

SCRIPTS = Path(__file__).resolve().parent
sys.path.insert(0, str(SCRIPTS))
from build_index import EMPTY_GAPS, INDEX_DIR, SCOPES, normalize_dtype  # noqa: E402
from derive.gate import version_covers, version_key  # noqa: E402
from derive.versioning import page_available, signature_available  # noqa: E402

FILTER_KEYS = ("stage", "operation", "category", "api_class", "dtype", "kw", "convert", "feature", "version", "product")


def load(index_dir: Path) -> tuple[dict[str, Any], dict[str, Any]]:
    index = json.loads((index_dir / "index.json").read_text(encoding="utf-8"))
    gaps_path = index_dir / "gaps.json"
    gaps = json.loads(gaps_path.read_text(encoding="utf-8")) if gaps_path.is_file() else dict(EMPTY_GAPS)
    return index, gaps


def csv(value: str | None) -> list[str]:
    return [item.strip() for item in value.split(",") if item.strip()] if value else []


def strip_version_prefix(value: str) -> str:
    """去掉旧 CLI 拼写的前缀 '>='，只保留目标版本号。"""
    text = value.strip()
    return text[2:].strip() if text.startswith(">=") else text


def canonical_change_version(index: dict[str, Any], value: str) -> str:
    """把 --changed-in 归一化到索引内实际的事件版本键（兼容 '>=9.2.0' 与 '9.2'）。"""
    target = version_key(strip_version_prefix(value))
    for since in index["views"].get("by_change_version", {}):
        if version_key(since) == target:
            return since
    return strip_version_prefix(value)


def version_target(index: dict[str, Any], value: str) -> str:
    """Resolve a target version; an index for another version cannot answer availability."""
    target = strip_version_prefix(value)
    target_key = version_key(target)
    indexed = index["meta"]["cann_version"]
    if target_key != version_key(indexed):
        raise ValueError(
            f"--version {target} 与索引 CANN {indexed} 不一致；请先运行 "
            f"ensure_index.py --cann-version {target}（保留目标架构），"
            "或用 build_index.py 构建对应版本的 --index-dir"
        )
    return target


def available_at(index: dict[str, Any], path: str, target: str) -> bool:
    """Match first support and lifecycle events, retaining deprecated APIs."""
    doc = index["documents"][path]
    if not doc.get("version") or not version_covers(doc["version"], target):
        return False
    events = doc.get("version_changes") or []
    if not page_available(events, target):
        return False
    signatures = doc.get("signatures") or []
    arch = index["meta"]["arch"]
    return not signatures or any(signature_available(s, events, target, arch) for s in signatures)


def resolve_api(index: dict[str, Any], name: str) -> str | None:
    """API 名 / 文档全路径 / 文件名 → 索引文档路径；未命中返回 None（由调用方空结果处理）。"""
    if name in index["documents"]:
        return name
    view = index["views"]["by_api"]
    if name in view:
        candidates = view[name]
        # 家族 canonical 页（<family>/<family>.md）优先于废弃/变体成员作 owner
        for path in candidates:
            if PurePosixPath(path).stem == name and PurePosixPath(path).parent.name == name:
                return path
        return candidates[0]
    for path in index["documents"]:
        if Path(path).stem == name or Path(path).name == f"{name}.md":
            return path
    return None


def why_tags(index: dict[str, Any], path: str, dims: dict[str, list[str]]) -> list[str]:
    reasons = []
    for stage in dims.get("stage", []):
        if path in index["views"]["by_stage"].get(stage, []):
            reasons.append(f"stage:{stage}")
    for op in dims.get("operation", []):
        if path in index["views"]["by_operation"].get(op, []):
            reasons.append(f"operation:{op}")
    for cat in dims.get("category", []):
        if path in index["views"]["by_category"].get(cat, []):
            reasons.append(f"category:{cat}")
    for cls in dims.get("api_class", []):
        if path in index["views"]["by_api_class"].get(cls, []):
            reasons.append(f"api_class:{cls}")
    for dtype in dims.get("dtype", []):
        if path in index["views"]["by_dtype"].get(dtype, []):
            reasons.append(f"dtype:{dtype}")
    for pair in dims.get("pair", []):
        if path in index["views"]["by_dtype_pair"].get(pair, []):
            reasons.append(f"convert:{pair}")
    for feature in dims.get("feature", []):
        if path in index["views"]["by_feature"].get(feature, []):
            reasons.append(f"feature:{feature}")
    for version in dims.get("version", []):
        if available_at(index, path, version):
            reasons.append(f"version:{version}")
    for since in dims.get("change_version", []):
        if path in index["views"]["by_change_version"].get(since, []):
            reasons.append(f"changed_in:{since}")
    for product in dims.get("product", []):
        if path in index["views"]["by_product"].get(product, []):
            reasons.append(f"product:{product}")
    for kw in dims.get("kw", []):
        doc = index["documents"][path]
        haystack = f"{doc['title']} {doc['description']}".lower()
        if kw.lower() in haystack:
            reasons.append(f"kw:{kw}")
    return reasons


def filter_query(index: dict[str, Any], args: argparse.Namespace) -> list[dict[str, Any]]:
    dims: dict[str, list[str]] = {
        "stage": [s for s in csv(args.stage) if s] if args.stage else [],
        "operation": csv(args.operation),
        "category": csv(args.category),
        "api_class": csv(args.api_class),
        "dtype": [normalize_dtype(d) for d in csv(args.dtype)],
        "kw": csv(args.kw),
        "pair": [],
        "feature": csv(args.feature),
        "version": [version_target(index, args.version)] if args.version else [],
        "change_version": [canonical_change_version(index, args.changed_in)]
        if getattr(args, "changed_in", None)
        else [],
        "product": csv(args.product),
    }
    if args.convert:
        src = normalize_dtype(args.convert[0])
        dims["pair"] = [f"{src}->{normalize_dtype(dst)}" for dst in csv(args.convert[1])]
    if not any(dims[key] for key in dims):
        raise ValueError(
            "no filter given; use --stage/--operation/--category/--api-class/--dtype/--kw/--convert"
            "/--feature/--version/--changed-in/--product"
        )
    # 维度值不合法时不报错：视作空结果，交由 Agent 走降级链并用同义词重试
    candidates: set[str] | None = None
    for key, view_name in (
        ("stage", "by_stage"),
        ("operation", "by_operation"),
        ("category", "by_category"),
        ("api_class", "by_api_class"),
        ("dtype", "by_dtype"),
        ("pair", "by_dtype_pair"),
        ("feature", "by_feature"),
        ("change_version", "by_change_version"),
        ("product", "by_product"),
    ):
        values = dims[key]
        if not values:
            continue
        matched: set[str] = set()
        for value in values:
            matched.update(index["views"][view_name].get(value, []))
        candidates = matched if candidates is None else candidates & matched
    if dims["version"]:
        candidates = {
            path
            for path in (candidates if candidates is not None else index["documents"])
            if available_at(index, path, dims["version"][0])
        }
    if dims["kw"]:
        matched = {
            path
            for path in (candidates if candidates is not None else index["documents"])
            if any(
                kw.lower() in f"{index['documents'][path]['title']} {index['documents'][path]['description']}".lower()
                for kw in dims["kw"]
            )
        }
        candidates = matched
    return [{"path": path, "why": why_tags(index, path, dims)} for path in sorted(candidates or set())]


def api_card(index: dict[str, Any], name: str) -> list[dict[str, Any]]:
    owner = resolve_api(index, name)
    if owner is None:
        return []
    doc = index["documents"][owner]
    results = [{"path": owner, "why": ["api:owner"]}]
    for member in index["views"]["by_api"].get(doc["family"], []):
        if member != owner:
            results.append({"path": member, "why": ["family-member"]})
    if doc["nav_page"] != owner and doc["nav_page"] in index["documents"]:
        results.append({"path": doc["nav_page"], "why": ["category-page"]})
    return results


def deps_query(index: dict[str, Any], target: str, kind: str | None, reverse: bool) -> list[dict[str, Any]]:
    path = resolve_api(index, target)
    if path is None:
        return []
    doc = index["documents"][path]
    results: list[dict[str, Any]] = []
    if reverse:
        # 反向按解析到的家族聚合：族名/成员名/全路径都能查到全族入边
        family_paths = set(index["views"]["by_api"].get(doc["family"], []))
        family_paths.add(path)
        for edge in index["graph"]["edges"]:
            if edge["to"] in family_paths and (not kind or edge["kind"] == kind):
                results.append(
                    {
                        "path": edge["from"],
                        "why": [f"reverse:{edge['kind']}"],
                        "condition": edge["condition"],
                        "out_of_scope": edge["from"] not in index["documents"],
                    }
                )
    else:
        for dep in doc["dependencies"]:
            if kind and dep["kind"] != kind:
                continue
            results.append(
                {
                    "path": dep["path"],
                    "why": [f"dep:{dep['kind']}"],
                    "condition": dep["condition"],
                    "out_of_scope": dep["path"] not in index["documents"],
                }
            )
    return results


def param_query(index: dict[str, Any], token: str) -> list[dict[str, Any]]:
    """参数名或枚举类型反查：--param round_mode / --param asc_cast_round_mode。"""
    results = []
    for path, doc in sorted(index["documents"].items()):
        hits = [
            p
            for sig in doc.get("signatures", [])
            for p in sig.get("params", [])
            if p.get("name") == token or p.get("type") == token
        ]
        if hits:
            param = hits[0]
            reasons = [f"param:{param.get('name', '')}"]
            if str(param.get("type", "")).startswith("asc_"):
                reasons.append(f"type:{param['type']}")
            results.append({"path": path, "why": reasons})
    if not results and token in index.get("defs", {}).get("enums", {}):
        enum = index["defs"]["enums"][token]
        results.append(
            {
                "path": enum["header"],
                "why": [f"enum:{token}"],
                "enum": {"values": enum["values"][:20], "header": enum["header"], "total_values": len(enum["values"])},
            }
        )
    return results


def enum_query(index: dict[str, Any], token: str) -> list[dict[str, Any]]:
    """枚举名或枚举值反查：--enum asc_cast_round_mode / --enum ASC_CAST_RN。"""
    enums = index.get("defs", {}).get("enums", {})
    constants = {c["name"]: c for c in index.get("defs", {}).get("constants", [])}
    results = []
    if token in enums:
        users = index["views"].get("by_enum", {}).get(token, [])
        enum = enums[token]
        results.append(
            {
                "path": f"include/{enum['header']}",
                "why": [f"enum:{token}"],
                "enum": {"values": enum["values"], "used_by": users},
            }
        )
    value_owner = None
    for name, enum in enums.items():
        if token in enum["values"]:
            value_owner = name
            break
    if value_owner is None and token in constants:
        value_owner = next(
            (e for e, info in enums.items() if any(c["name"] == token for c in constants.values() if True) and False),
            None,
        )
        # 常量名 ASC_CAST_RN → 归属枚举（defs 常量与枚举同名族）
        stem = token[4:] if token.startswith("ASC_") else token
        for name in enums:
            if name.lower().endswith(stem.split("_")[0].lower()):
                value_owner = name
                break
    if value_owner:
        users = index["views"].get("by_enum", {}).get(value_owner, [])
        results.append(
            {
                "path": f"include/{enums[value_owner]['header']}",
                "why": [f"enum-value:{token}", f"enum:{value_owner}"],
                "enum": {"value": token, "used_by": users},
            }
        )
    if not results:
        raise ValueError(f"unknown enum or value: {token}")
    return results


def gap_query(gaps: dict[str, Any], keyword: str) -> list[dict[str, Any]]:
    keywords = [k.strip().lower() for k in keyword.split(",") if k.strip()]
    if not keywords:
        raise ValueError("--gap requires a non-empty keyword")

    def hit(haystack: str) -> bool:
        return any(kw in haystack.lower() for kw in keywords)

    results = []
    for gap in gaps["scope_out_gaps"]:
        haystack = " ".join([gap["id"], gap["conclusion"], *gap["keywords"]])
        if hit(haystack):
            results.append({"gap_id": gap["id"], "conclusion": gap["conclusion"], "hints": gap["hints"]})
    for gap in gaps["relation_gaps"]:
        haystack = f"{gap['from']} {gap['note']}"
        if hit(haystack):
            results.append(
                {
                    "gap_id": f"relation:{Path(gap['from']).stem}",
                    "conclusion": gap["note"],
                    "hints": [{"path": gap["from"], "note": "范围内部文档"}],
                }
            )
    return results


def change_summary(event: dict[str, Any]) -> str:
    """单条版本变更事件的一行人读摘要（kind/scope 原样，不做词表校验）。"""
    parts = [f"{event.get('since', '')} {event.get('kind', '')} {event.get('scope', '')}".strip()]
    note = event.get("note")
    if note:
        parts.append(f"（{note}）")
    return "".join(parts)


def attach_change_events(index: dict[str, Any], results: list[dict[str, Any]], version: str) -> list[dict[str, Any]]:
    """--changed-in：给候选附上**该版本**的变更事件摘要（since/kind/scope/note）。

    只在显式使用 --changed-in 时附加，其他查询的输出结构不变。
    """
    target = version_key(strip_version_prefix(version))
    for row in results:
        doc = index["documents"].get(row["path"])
        if doc is None:
            continue
        events = [
            event
            for event in doc.get("version_changes") or []
            if event.get("since") and version_key(event["since"]) == target
        ]
        if events:
            row["version_changes"] = events
    return results


def run_query(args: argparse.Namespace, index: dict[str, Any], gaps: dict[str, Any]) -> list[dict[str, Any]]:
    if args.gap is not None:
        # 缺口登记描述的是"范围外"，没有文档维度可与过滤条件求交：
        # 与其静默忽略，不如明确拒绝（SKILL.md 承诺维度间 AND）。
        dims = _active_filter_dims(args)
        if dims:
            raise ValueError(
                "--gap 不能与过滤维度组合（"
                + "/".join("--" + name.replace("_", "-") for name in dims)
                + "）：缺口是范围外登记，无法与文档维度求交"
            )
        return gap_query(gaps, args.gap)
    independent = bool(args.api or args.deps_of or args.param or args.enum)
    if not independent:
        return _attach_changes(index, filter_query(index, args), args)
    if args.api:
        results = api_card(index, args.api)
    elif args.deps_of:
        results = deps_query(index, args.deps_of, args.kind, args.reverse)
    elif args.param:
        results = param_query(index, args.param)
    else:
        results = enum_query(index, args.enum)
    # 独立查询是"展开"而非"过滤"：只传它时输出不变；一旦同时给了过滤维度，
    # 必须与 filter_query 求交，否则就是静默吞掉用户条件（曾经的 A11 缺陷）。
    dims = _active_filter_dims(args)
    if dims:
        allowed = {row["path"] for row in filter_query(index, args) if "path" in row}
        results = [row for row in results if row.get("path") in allowed]
    return _attach_changes(index, results, args)


def _attach_changes(
    index: dict[str, Any], results: list[dict[str, Any]], args: argparse.Namespace
) -> list[dict[str, Any]]:
    """--changed-in：给候选附上该版本的变更事件摘要（过滤本身由 filter_query 完成）。"""
    for row in results:
        doc = index["documents"].get(row.get("path"))
        if doc and doc.get("version_changes"):
            row["version_context"] = {"cann_version": index["meta"]["cann_version"], "changes": doc["version_changes"]}
    changed_in = getattr(args, "changed_in", None)
    if changed_in:
        results = attach_change_events(index, results, changed_in.strip())
    return results


# 过滤维度（filter_query 消费的那些）；独立查询必须与它们求交，不能静默忽略。
FILTER_DIM_ATTRS = (
    "stage",
    "operation",
    "category",
    "api_class",
    "dtype",
    "kw",
    "convert",
    "feature",
    "version",
    "product",
)


def _active_filter_dims(args: argparse.Namespace) -> list[str]:
    """本次调用实际给出的过滤维度名（含 --changed-in）。"""
    active = [name for name in FILTER_DIM_ATTRS if getattr(args, name, None)]
    if getattr(args, "changed_in", None):
        active.append("changed_in")
    return active


def render(index: dict[str, Any], results: list[dict[str, Any]]) -> str:
    if not results:
        return "no match"
    lines = []
    for row in results:
        if "gap_id" in row:
            hints = "; ".join(h["path"] for h in row["hints"])
            lines.append(f"{row['gap_id']}\n  {row['conclusion']}\n  hints: {hints}")
            continue
        if "enum" in row:
            enum = row["enum"]
            detail = f"值: {', '.join(enum['values'][:12])}" if "value" not in enum else f"枚举值 {enum['value']}"
            users = f"；使用方 {len(enum['used_by'])} 篇" if enum.get("used_by") else ""
            lines.append(f"{row['path']} | {','.join(row['why'])}\n  {detail}{users}")
            continue
        doc = index["documents"].get(row["path"])
        name = doc["title"] if doc else Path(row["path"]).stem
        desc = (doc["description"] if doc else "")[:60]
        flags = " [范围外]" if row.get("out_of_scope") else ""
        condition = f"（{row['condition']}）" if row.get("condition") else ""
        lines.append(f"{row['path']}{flags} | {name} | {','.join(row['why'])}{condition}\n  {desc}")
        context = row.get("version_context")
        if context and not row.get("version_changes"):
            lines.append(
                f"  目标 CANN {context['cann_version']}，需核对重载/行为变更: "
                + "；".join(change_summary(e) for e in context["changes"])
            )
        changes = row.get("version_changes") or []
        if changes:
            lines.append("  版本变更: " + "；".join(change_summary(event) for event in changes))
    return "\n".join(lines)


def parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument(
        "--surface",
        default="c_api",
        choices=sorted(SCOPES),
        help="API surface 索引实例（默认 c_api）；索引构建时已按 CANN×架构筛选条件",
    )
    p.add_argument("--index-dir", type=Path, default=None, help="显式索引目录（默认 index/<surface>）")
    p.add_argument("--json", action="store_true", help="机器可读输出")
    p.add_argument("--stage", help="逗号列表: in,out,compute")
    p.add_argument("--operation", help="逗号列表（如 copy,divide,gather）")
    p.add_argument("--category", help="逗号列表（如 vector_compute,vector_datamove）")
    p.add_argument("--api-class", dest="api_class", help="API 分类=include 子目录（如 vector_datamove,reg_compute）")
    p.add_argument("--dtype", help="逗号列表（如 half,fp8_e4m3fn）")
    p.add_argument("--kw", help="title+description 子串")
    p.add_argument("--convert", nargs=2, metavar=("SRC", "DST"), help="类型转换对，DST 可为逗号列表")
    p.add_argument("--feature", help="特性（如 low_precision,low_precision_matrix,int4）")
    p.add_argument(
        "--version",
        help="目标 CANN 版本（如 9.2.0）：按首次支持范围及生命周期匹配，>=9.0.0 的接口可命中 9.2.0；"
        "需使用对应版本的索引。兼容旧写法 '>=9.2.0'，同样表示目标 9.2.0。查版本变更用 --changed-in",
    )
    p.add_argument(
        "--changed-in",
        dest="changed_in",
        help="版本变更事件维度（如 9.2.0）：命中 version_changes[].since 等于该版本的文档，"
        "并输出事件摘要（since/kind/scope/note）；空事件文档不入该视图",
    )
    p.add_argument("--product", help="产品 npu 标记（如 950,A3,910b）")
    p.add_argument("--api", help="家族卡：主页+同族成员+分类页")
    p.add_argument("--deps-of", dest="deps_of", help="依赖图遍历（API 名或路径）")
    p.add_argument("--kind", choices=["requires", "configures", "constrained_by", "related", "replaced_by"])
    p.add_argument("--reverse", action="store_true", help="配合 --deps-of：反向边")
    p.add_argument("--param", help="参数名或枚举类型反查（如 round_mode,asc_cast_round_mode）")
    p.add_argument("--enum", help="枚举名或枚举值反查（如 asc_cast_round_mode,ASC_CAST_RN）")
    p.add_argument("--gap", help="缺口登记检索")
    return p


def main() -> int:
    args = parser().parse_args()
    try:
        index, gaps = load(args.index_dir if args.index_dir is not None else INDEX_DIR / args.surface)
        results = run_query(args, index, gaps)
        if args.gap is not None and not results and not gaps.get("scope_out_gaps"):
            print("[]" if args.json else "无登记缺口：原范围外缺口已全部纳入索引范围")
            return 0
        if args.changed_in and not results:
            known = ", ".join(sorted(index["views"].get("by_change_version", {}))) or "（无）"
            target = canonical_change_version(index, args.changed_in)
            print(
                "[]"
                if args.json
                else f"无命中：索引中没有 since = {target} 的版本变更事件（索引内已有事件版本：{known}）"
            )
            return 0
        print(json.dumps(results, ensure_ascii=False, indent=1) if args.json else render(index, results))
        return 0
    except (ValueError, KeyError, OSError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
