#!/usr/bin python3
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


import argparse
import logging
import os
import shutil
import sys
from pathlib import Path

from .parser import parse_file, parse_string, render_html

logging.basicConfig(format="%(asctime)s [%(levelname)s] %(message)s", level=logging.INFO)
logger = logging.getLogger("mdparser")

_STATIC_EXTS = {".png", ".jpg", ".jpeg", ".gif", ".svg", ".ico", ".webp", ".bmp", ".pdf", ".zip", ".tar.gz"}


def _build_parser():
    parser = argparse.ArgumentParser(
        prog="mdparser", description="High-performance Markdown-to-HTML converter based on cmark-gfm (C)."
    )
    parser.add_argument("input", nargs="*", help="Markdown file(s) or directory to parse")
    parser.add_argument("-o", "--output", help="Output file or directory")
    parser.add_argument("--no-gfm", action="store_true", help="Disable GFM extensions")
    parser.add_argument("--stdin", action="store_true", help="Read from stdin")
    parser.add_argument("-s", "--style", action="store_true", help="Embed GitHub-style CSS")
    parser.add_argument("-r", "--recursive", action="store_true", help="Scan recursively")
    return parser


def _write_output(html, output):
    if output:
        Path(output).write_text(html, encoding="utf-8")
    else:
        sys.stdout.write(html)


def _handle_stdin(args):
    content = sys.stdin.buffer.read().decode("utf-8")
    html_body = parse_string(content, gfm=not args.no_gfm)
    _write_output(_build_html(html_body, styled=args.style), args.output)


def _handle_single(path, args):
    html_body = parse_file(path, gfm=not args.no_gfm)
    _write_output(_build_html(html_body, styled=args.style), args.output)


def _handle_batch(md_files, args):
    out_dir = Path(args.output) if args.output else Path.cwd()
    out_dir.mkdir(parents=True, exist_ok=True)
    common_root = _find_common_root(md_files)
    for src in md_files:
        html_body = parse_file(src, gfm=not args.no_gfm)
        html = _build_html(html_body, styled=args.style)
        rel = Path(src).relative_to(common_root).with_suffix(".html")
        dst = out_dir / rel
        dst.parent.mkdir(parents=True, exist_ok=True)
        dst.write_text(html, encoding="utf-8")
        logger.info("  %s -> %s", src, dst)
    _copy_static_files(common_root, out_dir, md_files)


def main():
    parser = _build_parser()
    args = parser.parse_args()
    md_files = _collect_files(args.input, args.recursive)

    if not md_files and not args.stdin:
        parser.print_help()
        return

    if args.stdin:
        _handle_stdin(args)
    elif len(md_files) == 1 and not (args.output and os.path.isdir(args.output)):
        _handle_single(md_files[0], args)
    else:
        _handle_batch(md_files, args)


def _copy_static_files(common_root, out_dir, md_files):
    md_set = {Path(f).relative_to(common_root) for f in md_files}
    for entry in sorted(common_root.rglob("*")):
        if not entry.is_file():
            continue
        if entry.suffix.lower() not in _STATIC_EXTS:
            continue
        rel = entry.relative_to(common_root)
        if rel in md_set:
            continue
        dst = out_dir / rel
        dst.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(entry, dst)
        logger.info("  %s -> %s", entry, dst)


def _find_common_root(paths):
    if not paths:
        return Path(".")
    root = Path(paths[0]).parent
    for p in paths[1:]:
        pp = Path(p).parent
        while root not in pp.parents and root != pp:
            root = root.parent
    return root


def _collect_files(paths, recursive):
    files = []
    for p in paths:
        pp = Path(p)
        if pp.is_dir():
            pattern = "**/*.md" if recursive else "*.md"
            files.extend(str(f) for f in sorted(pp.glob(pattern)) if f.is_file())
        elif pp.is_file() and pp.suffix == ".md":
            files.append(str(p))
    return files


def _build_html(body, styled=False):
    if not styled:
        return body
    return render_html(body)
