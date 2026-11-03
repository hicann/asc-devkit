# -----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# -----------------------------------------------------------------------------------------------------------

"""include/c_api 头文件解析：声明、Doxygen、架构守卫与 defs 目录。

以 C++ 声明为主源（类型/地址空间/参数名都在声明里），Doxygen 块补充
@override_id/@archs/@ingroup/@param，`__NPU_ARCH__` 守卫兜底架构。
解析不了的事实不猜测，进 unresolved（由 derive-overrides 裁决）。
"""

from __future__ import annotations

import re
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any

ADDRESS_SPACES = {
    "__gm__": "global_memory",
    "__ubuf__": "unified_buffer",
    "__cbuf__": "l1_buffer",
    "__fbuf__": "fixpipe_buffer",
    "__ca__": "l0a_buffer",
    "__cb__": "l0b_buffer",
    "__cc__": "l0c_buffer",
}
# 地址包装类型 → 数据位置（语义来自类型定义，defs/type.h）。
WRAPPER_LOCATIONS = {"ub_addr8_t": "unified_buffer"}
DTYPE_ALIASES = {
    "int4b_t": "int4",
    "int4x2_t": "int4",
    "int8_t": "int8",
    "uint8_t": "uint8",
    "int16_t": "int16",
    "uint16_t": "uint16",
    "int32_t": "int32",
    "uint32_t": "uint32",
    "int64_t": "int64",
    "uint64_t": "uint64",
    "half": "half",
    "bfloat16_t": "bfloat16",
    "float": "float",
    "double": "double",
    "hifloat8_t": "hifloat8",
    "fp4x2_e1m2_t": "fp4x2_e1m2",
    "fp4x2_e2m1_t": "fp4x2_e2m1",
    "fp8_e4m3fn_t": "fp8_e4m3fn",
    "fp8_e5m2_t": "fp8_e5m2",
    "fp8_e8m0_t": "fp8_e8m0",
}
DOCTYPE_BLOCK_RE = re.compile(r"/\*\*(?P<body>.*?)\*/", re.DOTALL)
INGROUP_RE = re.compile(r"@ingroup\s+(\S+)")
OVERRIDE_ID_RE = re.compile(r"@override_id\s+(\S+)")
ARCHS_RE = re.compile(r"@archs\s+([\d ]+)")
PARAM_DOC_RE = re.compile(r"@param\s+\[(?P<direction>(?:in|out)\s*(?:,\s*(?:in|out))?)\]\s+(?P<name>\S+)")
DECL_START_RE = re.compile(r"^(?:__aicore__|__simd_callee__)\s+inline\s+(?P<ret>.+?)\s+(?P<name>asc_\w+)\s*\(")
ARCH_GUARD_RE = re.compile(r"#\s*(?:if|elif)\s+defined\(__NPU_ARCH__\)\s*&&\s*\(__NPU_ARCH__\s*==\s*(?P<arch>\d+)\)")
ENUM_RE = re.compile(r"enum class (?P<name>\w+)\s*(?::\s*(?P<underlying>\w+))?\s*\{(?P<body>.*?)\}", re.DOTALL)
ENUM_VALUE_RE = re.compile(r"(?P<name>\w+)\s*(?:=\s*(?P<value>[^,}]+))?")
CONSTEXPR_RE = re.compile(r"^constexpr[^;]*?\b(?P<name>ASC_\w+)\s*;", re.MULTILINE)
DEFINE_RE = re.compile(r"^#\s*define\s+(?P<name>\w+)(?P<args>\([^)]*\)|)\s+(?P<body>.+)$", re.MULTILINE)
ALIAS_DEFINE_RE = re.compile(r"^#\s*define\s+(asc_[a-z0-9_]+)\s+([a-z_][a-z0-9_]*)\s*$")
ALIAS_RE = re.compile(r"^using\s+(?P<name>\w+)\s*=\s*(?P<target>[^;]+);", re.MULTILINE)
RECORD_RE = re.compile(r"^(?:struct|union)\s+(?P<name>\w+)\s*\{", re.MULTILINE)
DEPRECATED_RE = re.compile(r"ASC_DEPRECATED\(\s*(?P<version>[\d.]+)\s*,\s*\"[^\"]*\"\s*,\s*(?P<replacement>[^)]+)\)")


@dataclass
class Param:
    name: str = ""
    type_text: str = ""
    address_space: str = ""
    base_type: str = ""
    is_pointer: bool = False
    is_vector: bool = False
    dtype: str = ""
    is_mask: bool = False
    direction: str = ""

    def to_dict(self) -> dict[str, Any]:
        # 只序列化非默认键：参数条目数量大，空值键会显著膨胀 index.json
        out: dict[str, Any] = {"name": self.name, "type": self.base_type or self.type_text}
        if self.address_space:
            out["address_space"] = self.address_space
        if self.dtype:
            out["dtype"] = self.dtype
        if self.is_pointer:
            out["is_pointer"] = True
        if self.is_vector:
            out["is_vector"] = True
        if self.is_mask:
            out["is_mask"] = True
        if self.direction:
            out["direction"] = self.direction
        return out


@dataclass
class Declaration:
    name: str = ""
    header: str = ""
    signature: str = ""
    return_type: str = ""
    params: list[Param] = field(default_factory=list)
    override_id: str = ""
    archs: list[str] = field(default_factory=list)
    ingroup: str = ""
    deprecated: dict[str, str] | None = None
    unresolved: list[str] = field(default_factory=list)

    def dtypes(self) -> list[str]:
        values = {p.dtype for p in self.params if p.dtype}
        match = re.match(r"(?:const\s+)?vector_(\w+)", self.return_type)
        if match:
            token = DTYPE_ALIASES.get(match.group(1))
            if token:
                values.add(token)
        return sorted(values)

    def locations(self) -> list[str]:
        result = set()
        for param in self.params:
            if param.address_space:
                result.add(param.address_space)
            if param.base_type in WRAPPER_LOCATIONS:
                result.add(WRAPPER_LOCATIONS[param.base_type])
            if param.is_vector:
                result.add("mask_register" if param.is_mask else "vector_register")
        if re.match(r"(?:const\s+)?vector_", self.return_type):
            result.add("mask_register" if self.return_type.startswith("vector_bool") else "vector_register")
        return sorted(result)

    def masked(self) -> bool:
        return any(p.is_mask for p in self.params) or self.return_type.startswith("vector_bool")

    def to_dict(self) -> dict[str, Any]:
        return {
            "override_id": self.override_id,
            "archs": sorted(self.archs),
            "header": self.header,
            "dtypes": self.dtypes(),
            "locations": self.locations(),
            "masked": self.masked(),
            "params": [p.to_dict() for p in self.params],
            "return_type": self.return_type,
            "deprecated": self.deprecated,
            "unresolved": self.unresolved,
        }


def _split_params(text: str) -> list[str]:
    parts: list[str] = []
    depth_angle = depth_paren = 0
    current: list[str] = []
    for ch in text:
        if ch == "<":
            depth_angle += 1
        elif ch == ">":
            depth_angle = max(0, depth_angle - 1)
        elif ch == "(":
            depth_paren += 1
        elif ch == ")":
            depth_paren = max(0, depth_paren - 1)
        if ch == "," and depth_angle == 0 and depth_paren == 0:
            parts.append("".join(current).strip())
            current = []
        else:
            current.append(ch)
    tail = "".join(current).strip()
    if tail:
        parts.append(tail)
    return [p for p in parts if p and p != "void"]


def _parse_param(text: str, directions: dict[str, str]) -> Param:
    param = Param(type_text=text)
    for token, location in ADDRESS_SPACES.items():
        if token in text:
            param.address_space = location
            text = text.replace(token, "")
    template = re.match(r"std::integral_constant<\s*(\w+)", text)
    if template:
        param.base_type = template.group(1)
    name_match = re.search(r"(?P<name>\w+)\s*$", text)
    if name_match:
        param.name = name_match.group("name")
        text = text[: name_match.start()].strip()
    if "*" in text:
        param.is_pointer = True
        text = text.replace("*", "").strip()
    text = re.sub(r"\bconst\b|\bvolatile\b|\brestrict\b|\binline\b|&", " ", text).strip()
    vector = re.match(r"(?:const\s+)?vector_(\w+)", text)
    if vector:
        param.is_vector = True
        token = vector.group(1)
        if token == "bool":
            param.is_mask = True
            param.base_type = "vector_bool"
        else:
            param.base_type = f"vector_{token}"
            param.dtype = DTYPE_ALIASES.get(token, "")
    elif not template:
        base = re.match(r"(\w+)", text)
        if base:
            param.base_type = base.group(1)
            param.dtype = DTYPE_ALIASES.get(param.base_type, "")
    param.direction = directions.get(param.name, "")
    return param


def _normalize_direction(value: str) -> str:
    """@param [in,out] 等写法归一：去空格，顺序统一为 in,out。"""
    compact = re.sub(r"\s+", "", value)
    return "in,out" if compact in {"in,out", "out,in"} else compact


def _parse_doxygen(text: str) -> list[dict[str, Any]]:
    blocks: list[dict[str, Any]] = []
    for match in DOCTYPE_BLOCK_RE.finditer(text):
        body = match.group("body")
        entry: dict[str, Any] = {
            "start": match.start(),
            "end": match.end(),
            "ingroup": "",
            "override_id": "",
            "archs": [],
            "deprecated": None,
            "params": {
                m.group("name"): _normalize_direction(m.group("direction")) for m in PARAM_DOC_RE.finditer(body)
            },
        }
        group = INGROUP_RE.search(body)
        if group:
            entry["ingroup"] = group.group(1)
        override = OVERRIDE_ID_RE.search(body)
        if override:
            entry["override_id"] = override.group(1)
        archs = ARCHS_RE.search(body)
        if archs:
            entry["archs"] = archs.group(1).split()
        dep = DEPRECATED_RE.search(body)
        if dep:
            entry["deprecated"] = {"version": dep.group("version"), "replacement": dep.group("replacement").strip()}
        blocks.append(entry)
    return blocks


def _strip_comments(text: str) -> str:
    text = re.sub(r"/\*.*?\*/", "", text, flags=re.DOTALL)
    text = re.sub(r"//[^\n]*", "", text)
    return text


def _bind_deprecated_macros(raw: str, parsed: list[tuple[int, "Declaration"]]) -> None:
    """代码级 ASC_DEPRECATED(v, date, replacement) 位于被废弃声明之前，按位置绑定到后续声明。

    标注数据把宏写在 Doxygen 块之外（`ASC_DEPRECATED(9.2.0, "...", asc_x)` 后接文档块与声明），
    `_parse_doxygen` 只能看到注释体内部，因此在这里补一次位置绑定，保留替代目标。
    """
    comment_spans = [(m.start(), m.end()) for m in re.finditer(r"/\*.*?\*/", raw, re.DOTALL)]
    ordered = sorted(parsed, key=lambda item: item[0])
    for macro in DEPRECATED_RE.finditer(raw):
        if any(start <= macro.start() < end for start, end in comment_spans):
            continue
        for offset, declaration in ordered:
            if offset >= macro.end():
                if declaration.deprecated is None:
                    declaration.deprecated = {
                        "version": macro.group("version"),
                        "replacement": macro.group("replacement").strip(),
                    }
                break


def parse_header(path: Path, rel_root: Path) -> dict[str, Any]:
    """解析单个头文件：函数声明 + defs 目录（枚举/常量/宏/别名/记录）。

    逐原始行遍历（跟踪块注释状态），保证声明偏移与 Doxygen 块偏移同坐标系。
    """
    raw = path.read_text(encoding="utf-8")
    header_rel = path.relative_to(rel_root).as_posix()
    doxygen = _parse_doxygen(raw)

    functions: dict[str, list[Declaration]] = {}
    parsed: list[tuple[int, Declaration]] = []
    unresolved: list[dict[str, str]] = []
    guard_stack: list[str] = []
    lines = raw.splitlines()
    line_offsets: list[int] = []
    cursor = 0
    for line in lines:
        line_offsets.append(cursor)
        cursor += len(line) + 1
    in_comment = False
    prev_decl_end = -1
    index = 0
    while index < len(lines):
        line = lines[index]
        offset = line_offsets[index]
        stripped = line.strip()
        opens = line.count("/*")
        closes = line.count("*/")
        line_is_comment = in_comment or stripped.startswith("//") or (opens > 0 and closes == 0)
        if not line_is_comment:
            guard = ARCH_GUARD_RE.search(line)
            alias_match = ALIAS_DEFINE_RE.match(stripped)
            if guard:
                if stripped.startswith("#if"):
                    guard_stack.append(guard.group("arch"))
                elif guard_stack:
                    guard_stack[-1] = guard.group("arch")
            elif stripped.startswith("#endif"):
                if guard_stack:
                    guard_stack.pop()
            elif alias_match:
                # 对象式宏别名（#define asc_mem_bar mem_bar）：C API 名 → 内部函数。
                # 产出无参 Declaration，archs 依赖守卫/Doxygen，否则交正文 npu 兜底。
                declaration = _build_alias_declaration(
                    alias_match.group(1), alias_match.group(2), header_rel, offset, doxygen, guard_stack, prev_decl_end
                )
                functions.setdefault(declaration.name, []).append(declaration)
                parsed.append((offset, declaration))
                prev_decl_end = offset + len(stripped)
            elif DECL_START_RE.match(stripped) or (
                stripped.startswith(("__aicore__", "__simd_callee__"))
                and index + 1 < len(lines)
                and DECL_START_RE.match((stripped + " " + lines[index + 1].strip()).strip())
            ):
                decl_text = stripped
                start = index
                while not decl_text.rstrip().endswith(");") and index + 1 < len(lines) and index - start < 30:
                    index += 1
                    decl_text += " " + lines[index].strip()
                match = DECL_START_RE.match(decl_text)
                if match:
                    declaration = _build_declaration(
                        match, decl_text, header_rel, offset, doxygen, guard_stack, prev_decl_end
                    )
                    prev_decl_end = offset + len(decl_text)
                    functions.setdefault(declaration.name, []).append(declaration)
                    parsed.append((offset, declaration))
                    for note in declaration.unresolved:
                        unresolved.append({"header": header_rel, "function": declaration.name, "issue": note})
        if in_comment:
            if closes > 0:
                in_comment = False
        elif opens > 0 and closes == 0:
            in_comment = True
        index += 1

    _bind_deprecated_macros(raw, parsed)

    code = _strip_comments(raw)
    enums: dict[str, dict[str, Any]] = {}
    for match in ENUM_RE.finditer(code):
        values = [
            m.group("name")
            for m in ENUM_VALUE_RE.finditer(match.group("body"))
            if m.group("name") not in {"uint8_t", "int8_t"}
        ]
        enums[match.group("name")] = {
            "values": values,
            "underlying": match.group("underlying") or "",
            "header": header_rel,
        }
    constants = [{"name": m.group("name"), "header": header_rel} for m in CONSTEXPR_RE.finditer(code)]
    macros = [
        {"name": m.group("name"), "function_like": bool(m.group("args")), "header": header_rel}
        for m in DEFINE_RE.finditer(code)
    ]
    aliases = [
        {"name": m.group("name"), "target": m.group("target").strip(), "header": header_rel}
        for m in ALIAS_RE.finditer(code)
    ]
    records = [
        {"name": m.group("name"), "kind": "union" if m.group(0).startswith("union") else "struct", "header": header_rel}
        for m in RECORD_RE.finditer(code)
    ]
    return {
        "functions": functions,
        "unresolved": unresolved,
        "enums": enums,
        "constants": constants,
        "macros": macros,
        "aliases": aliases,
        "records": records,
    }


def _build_declaration(
    match: re.Match,
    decl_text: str,
    header_rel: str,
    offset: int,
    doxygen: list[dict[str, Any]],
    guard_stack: list[str],
    prev_decl_end: int,
) -> Declaration:
    name = match.group("name")
    params_text = decl_text[match.end() :]
    params_text = params_text[: params_text.rfind(");")] if ");" in params_text else params_text
    declaration = Declaration(
        name=name,
        header=header_rel,
        signature=re.sub(r"\s+", " ", decl_text).rstrip(");"),
        return_type=re.sub(r"\s+", " ", match.group("ret")).strip(),
    )
    for raw_param in _split_params(params_text):
        declaration.params.append(_parse_param(raw_param, {}))
    nearest = max((block for block in doxygen if block["end"] <= offset), key=lambda block: block["end"], default=None)
    owned = (
        nearest is not None
        and nearest["override_id"]
        and nearest["end"] > prev_decl_end
        and (nearest["override_id"] == name or nearest["override_id"].startswith(name + "__"))
    )
    if owned:
        declaration.override_id = nearest["override_id"]
        declaration.archs = list(nearest["archs"])
        declaration.ingroup = nearest["ingroup"]
        declaration.deprecated = nearest["deprecated"]
        for param in declaration.params:
            param.direction = nearest["params"].get(param.name, "")
    else:
        declaration.archs = list(guard_stack)
        declaration.unresolved.append("declaration without @override_id")
        if not declaration.archs:
            declaration.unresolved.append("archs unknown (no @archs and no __NPU_ARCH__ guard)")
    return declaration


def _build_alias_declaration(
    name: str,
    target: str,
    header_rel: str,
    offset: int,
    doxygen: list[dict[str, Any]],
    guard_stack: list[str],
    prev_decl_end: int,
) -> Declaration:
    declaration = Declaration(name=name, header=header_rel, signature=f"#define {name} {target}", return_type="")
    nearest = max((block for block in doxygen if block["end"] <= offset), key=lambda block: block["end"], default=None)
    owned = (
        nearest is not None
        and nearest["override_id"]
        and nearest["end"] > prev_decl_end
        and (nearest["override_id"] == name or nearest["override_id"].startswith(name + "__"))
    )
    if owned:
        declaration.override_id = nearest["override_id"]
        declaration.archs = list(nearest["archs"])
        declaration.ingroup = nearest["ingroup"]
        declaration.deprecated = nearest["deprecated"]
    else:
        declaration.archs = list(guard_stack)
    return declaration


def build_include_db(include_root: Path) -> dict[str, Any]:
    """解析 include/c_api 全部头文件为统一数据库。"""
    db: dict[str, Any] = {
        "functions": {},
        "unresolved": [],
        "enums": {},
        "constants": [],
        "macros": [],
        "aliases": [],
        "records": [],
    }
    for path in sorted(include_root.rglob("*.h")):
        payload = parse_header(path, include_root.parent)
        for name, declarations in payload["functions"].items():
            db["functions"].setdefault(name, []).extend(declarations)
        db["unresolved"].extend(payload["unresolved"])
        db["enums"].update(payload["enums"])
        db["constants"].extend(payload["constants"])
        db["macros"].extend(payload["macros"])
        db["aliases"].extend(payload["aliases"])
        db["records"].extend(payload["records"])
    return db


def declarations_for(api: str, db: dict[str, Any], area: str | None = None) -> tuple[list[Declaration], str]:
    """文档 API 名 → 声明集合；优先精确名 + 同区域头文件，再家族前缀兜底。"""

    def in_area(declarations: list[Declaration]) -> list[Declaration]:
        if area is None:
            return declarations
        return [d for d in declarations if d.header.startswith(f"c_api/{area}/")]

    if api in db["functions"]:
        matched = in_area(db["functions"][api]) or db["functions"][api]
        return matched, "exact"
    family = sorted(name for name in db["functions"] if name.startswith(api + "_"))
    if family:
        declarations: list[Declaration] = []
        for name in family:
            declarations.extend(db["functions"][name])
        matched = in_area(declarations) or declarations
        return matched, "family"
    return [], "none"
