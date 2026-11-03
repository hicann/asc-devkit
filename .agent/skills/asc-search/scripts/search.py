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

"""Read-only Agent interface to a prepared Ascend C API index."""

from __future__ import annotations

import argparse
import json
import os
from pathlib import Path

import build_index
import query
import ensure_index

PLATFORMS = {
    "a2": ("2201", "910b"),
    "a3": ("2201", "A3"),
    "950": ("3510", "950"),
    "310b": ("3002", "310b"),
    "310p": ("2002", "310p"),
    "910": ("1001", "910"),
}


def final_paths(result: dict) -> list[str]:
    """noLLM：只保留候选文档路径。"""
    return [row.get("path", "") for row in result["results"]]


def final_snippet(paths: list[str]) -> str:
    """noLLM 最终输出：单候选 expected，多候选 expected_any（一组，任一项命中即可）。"""
    if not paths:
        return ""
    if len(paths) == 1:
        return "expected:\n  - " + paths[0]
    lines = ["expected_any:", "  - - " + paths[0]]
    lines += ["    - " + path for path in paths[1:]]
    return "\n".join(lines)


def search(args):
    if args.gap is not None and args.keywords:
        raise ValueError("--gap cannot be combined with --keywords; use the --gap query keyword")
    root, index_dir = resolve_paths(args)
    index, gaps = query.load(index_dir)
    fresh, reasons = ensure_index.light_fresh(index_dir, root, ensure_index.build_index.SCOPES[args.surface])
    if not fresh:
        raise ValueError("stale index; run preparation: " + "; ".join(reasons[:5]))
    if args.version:
        query.version_target(index, args.version)
    if args.platform:
        arch, product = PLATFORMS[args.platform]
        if index["meta"]["arch"] != arch:
            raise ValueError("platform conflicts with prepared index architecture")
        if args.product and product not in query.csv(args.product):
            raise ValueError("--product conflicts with --platform")
        if args.gap is None:
            args.product = product
    keywords = args.keywords or []
    has_query = bool(query._active_filter_dims(args) or args.api or args.deps_of or args.enum or args.param or args.gap)
    rows = (
        query.run_query(args, index, gaps)
        if has_query
        else [{"path": path, "why": []} for path in index["scope"]["primary_docs"]]
    )
    if not has_query and not keywords:
        raise ValueError("provide keywords or at least one structured query dimension")
    if keywords:
        filtered = []
        for row in rows:
            doc = index["documents"].get(row["path"], {})
            text = " ".join(str(doc.get(k, "")) for k in ("title", "description", "family", "tags")).casefold()
            if all(word.casefold() in text for word in keywords):
                row["why"] += ["keyword:" + word for word in keywords]
                filtered.append(row)
        rows = filtered
    total = len(rows)
    if args.limit < 1:
        raise ValueError("--limit must be positive")
    rows = rows[: args.limit]
    for row in rows:
        rel = row.get("path", "")
        target = (root / rel).resolve()
        target.relative_to(root)
        row["local_path"] = str(target)
        doc = index["documents"].get(rel)
        if doc:
            row.update(
                {
                    key: doc[key]
                    for key in ("title", "description", "version", "deprecated", "headers", "examples", "signatures")
                }
            )
            row["header_locations"] = [
                str(root / (p if p.startswith("include/") else "include/" + p)) for p in doc["headers"]
            ]
            row["example_locations"] = [
                dict(e, local_path=str(root / e["path"].lstrip("/") / e["file"])) for e in doc["examples"]
            ]
    return {
        "schema": "asc-search-results/1",
        "source": "asc-devkit",
        "source_commit": index["meta"]["source_commit"],
        "repository": args.repository,
        "root": str(root),
        "cann_version": index["meta"]["cann_version"],
        "arch": index["meta"]["arch"],
        "platform": args.platform,
        "count": len(rows),
        "matched_count": total,
        "truncated": total > len(rows),
        "results": rows,
    }


def build_parser() -> argparse.ArgumentParser:
    parser = query.parser()
    parser.description = __doc__
    parser.add_argument("--root", type=Path, default=build_index.REPO_ROOT, help="数据根（默认 skill 所在仓库根）")
    parser.add_argument("--keywords", nargs="+", help="short keywords; all must match title/description/family/tags")
    parser.add_argument("--platform", choices=sorted(PLATFORMS))
    parser.add_argument("--repository", default="https://gitcode.com/cann/asc-devkit.git")
    parser.add_argument("--limit", type=int, default=20)
    parser.add_argument(
        "--no-llm",
        dest="no_llm",
        action="store_true",
        default=os.environ.get("ASC_SEARCH_NO_LLM", "").strip().lower() in ("1", "true", "yes", "on"),
        help="noLLM：只输出检索用例期望片段（单候选 expected，多候选 expected_any），"
        "跳过调用方 LLM 整理（也可设 ASC_SEARCH_NO_LLM=1）",
    )
    return parser


def resolve_paths(args) -> tuple[Path, Path]:
    """数据根与索引目录：省略时默认取 skill 所在仓库根与 skill 内 index/<surface>。"""
    root = args.root.resolve()
    index_dir = args.index_dir if args.index_dir is not None else query.INDEX_DIR / args.surface
    return root, index_dir


def main(argv=None):
    try:
        args = build_parser().parse_args(argv)
        result = search(args)
        if getattr(args, "no_llm", False):
            snippet = final_snippet(final_paths(result))
            if snippet:
                print(snippet)
        else:
            print(json.dumps(result, ensure_ascii=False, separators=(",", ":")))
        return 0
    except (ValueError, OSError, KeyError, TypeError) as exc:
        print(json.dumps({"status": "error", "error": str(exc)}, ensure_ascii=False))
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
