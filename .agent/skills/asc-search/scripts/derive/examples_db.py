# -----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# -----------------------------------------------------------------------------------------------------------

"""样例目录解析：单 ID 或集中 ID 列表的成对 marker → 样例文件映射。"""

from __future__ import annotations

import re
from pathlib import Path

MARKER_RE = re.compile(
    r"^[ \t]*//![ \t]*\[(?P<ids>[A-Za-z0-9_.:-]+(?:[ \t]*,[ \t]*[A-Za-z0-9_.:-]+)*)\][ \t]*$", re.MULTILINE
)


def build_examples_db(examples_root: Path) -> dict[str, list[dict[str, str | int]]]:
    """override_id → [{path, file, start_line, end_line}]，行号不含 marker。"""
    db: dict[str, list[dict[str, str | int]]] = {}
    if not examples_root.is_dir():
        return db
    for path in sorted(examples_root.rglob("*")):
        if not path.is_file() or path.suffix.lower() not in {".asc", ".c", ".cc", ".cpp", ".cxx", ".h", ".hpp"}:
            continue
        active = None
        source = path.read_text(encoding="utf-8", errors="replace")
        for match in MARKER_RE.finditer(source):
            group = frozenset(part.strip() for part in match.group("ids").split(","))
            line = source.count("\n", 0, match.start()) + 1
            if active is None:
                active = (group, line)
            elif group == active[0]:
                entry = {
                    "path": "/" + path.parent.relative_to(examples_root.parent).as_posix(),
                    "file": path.name,
                    "start_line": active[1] + 1,
                    "end_line": line - 1,
                }
                for identifier in sorted(group):
                    db.setdefault(identifier, []).append(entry)
                active = None
            else:
                raise ValueError(f"unpaired or nested example markers: {path}")
        if active is not None:
            raise ValueError(f"unclosed example markers: {path}")
    return db
