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


import html as html_mod
import pathlib
import re
from urllib.parse import unquote
import cmarkgfm
from pygments import highlight
from pygments.lexers import get_lexer_by_name
from pygments.formatters import HtmlFormatter
from pygments.util import ClassNotFound

_CSS = None
_PYGMENTS_FORMATTER = None
_PYGMENTS_CSS = None


def _load_css():
    global _CSS
    if _CSS is not None:
        return _CSS
    candidates = [
        pathlib.Path(__file__).parent.parent.parent
        / "node_modules"
        / "github-markdown-css"
        / "github-markdown.css",
        pathlib.Path(__file__).parent.parent
        / "node_modules"
        / "github-markdown-css"
        / "github-markdown.css",
        pathlib.Path(
            "/home/zjj/md-parser/node_modules/github-markdown-css/github-markdown.css"
        ),
    ]
    for p in candidates:
        if p.exists():
            _CSS = p.read_text(encoding="utf-8")
            return _CSS
    _CSS = ""
    return _CSS


_OPTIONS = cmarkgfm.Options.CMARK_OPT_UNSAFE

_MD_LINK_RE = re.compile(r'(href|src)="((?!https?:|//)[^"]+)\.md(#[^"]*)?"')

_CANN_OPEN_RE = re.compile(
    r'<cann-filter\b[^>]*npu[_-]type\s*=\s*"([^"]+)"[^>]*>',
    re.IGNORECASE,
)

_CANN_CLOSE_RE = re.compile(
    r"</cann-filter\b[^>]*>",
    re.IGNORECASE,
)

_OTHER_TAG_NAMES = r"term|ph|__gm__|__ubuf__"

_OTHER_TAG_LINE_RE = re.compile(
    r"^[ \t]*</?(" + _OTHER_TAG_NAMES + r")\b[^>]*>[ \t]*$",
    re.IGNORECASE,
)

_OTHER_TAG_WRAP_RE = re.compile(
    r"<(" + _OTHER_TAG_NAMES + r")\b[^>]*>([\s\S]*?)</\1>",
    re.IGNORECASE,
)

_OTHER_TAG_ANY_RE = re.compile(
    r"</?(" + _OTHER_TAG_NAMES + r")\b[^>]*>",
    re.IGNORECASE,
)


_FENCE_OPEN_RE = re.compile(r"^[ \t]{0,3}(`{3,}|~{3,})", re.MULTILINE)

_INLINE_CODE_SPAN_RE = re.compile(r"`+[^\n]*?`+")

_LONE_TILDE_RE = re.compile(r"(?<!\\)(?<!~)~(?!~)")


def _escape_lone_tildes_in_line(line: str) -> str:
    result = []
    last_end = 0
    for m in _INLINE_CODE_SPAN_RE.finditer(line):
        before = line[last_end : m.start()]
        result.append(_LONE_TILDE_RE.sub(r"\\~", before))
        result.append(m.group())
        last_end = m.end()
    remaining = line[last_end:]
    result.append(_LONE_TILDE_RE.sub(r"\\~", remaining))
    return "".join(result)


def _escape_lone_tildes(md_text: str) -> str:
    lines = md_text.split("\n")
    result = []
    in_fence = False
    fence_char = None

    for line in lines:
        stripped = line.lstrip(" \t")

        if in_fence:
            result.append(line)
            if fence_char and re.match(
                rf"^{re.escape(fence_char)}{{3,}}[ \t]*$", stripped
            ):
                in_fence = False
                fence_char = None
            continue

        fence_match = re.match(r"^(`{3,}|~{3,})", stripped)
        if fence_match:
            in_fence = True
            fence_char = fence_match.group(1)[0]
            result.append(line)
            continue

        result.append(_escape_lone_tildes_in_line(line))

    return "\n".join(result)


_MATH_BLOCK_RE = re.compile(r"(?<!\\)\$\$[\s\S]*?(?<!\\)\$\$")

_MATH_INLINE_RE = re.compile(r"(?<!\\)(?<!\$)\$(?!\$)[^\n$]+?(?<!\\)\$(?!\$)")

_MATH_PLACEHOLDER_FMT = "@@MATH{n}@@"


def _split_by_fences(md_text: str):
    lines = md_text.split("\n")
    segments = []
    current = []
    is_code = False
    fence_char = None

    for line in lines:
        stripped = line.lstrip(" \t")

        if is_code:
            current.append(line)
            if fence_char and re.match(
                rf"^{re.escape(fence_char)}{{3,}}[ \t]*$", stripped
            ):
                is_code = False
                fence_char = None
                segments.append((True, "\n".join(current)))
                current = []
            continue

        fence_match = re.match(r"^(`{3,}|~{3,})", stripped)
        if fence_match:
            if current:
                segments.append((False, "\n".join(current)))
                current = []
            is_code = True
            fence_char = fence_match.group(1)[0]
            current.append(line)
            continue

        current.append(line)

    if current:
        segments.append((is_code, "\n".join(current)))

    return segments


def _make_code_stasher():
    spans = []

    def _stash(m):
        spans.append(m.group(0))
        return f"\x00CODE{len(spans) - 1}\x00"

    return _stash, spans


def _make_math_stasher(blocks):
    def _stash(m):
        blocks.append(m.group(0))
        return _MATH_PLACEHOLDER_FMT.format(n=len(blocks) - 1)

    return _stash


def _extract_math_blocks(md_text: str) -> tuple:
    segments = _split_by_fences(md_text)
    blocks = []
    result = []
    for is_code, content in segments:
        if is_code:
            result.append(content)
            continue

        stash_code, code_spans = _make_code_stasher()
        text = _INLINE_CODE_SPAN_RE.sub(stash_code, content)

        stash_math = _make_math_stasher(blocks)
        text = _MATH_BLOCK_RE.sub(stash_math, text)
        text = _MATH_INLINE_RE.sub(stash_math, text)

        for i, code in enumerate(code_spans):
            text = text.replace(f"\x00CODE{i}\x00", code)

        result.append(text)

    return "\n".join(result), blocks


def _restore_math_blocks(html: str, blocks: list) -> str:
    for i, block in enumerate(blocks):
        placeholder = _MATH_PLACEHOLDER_FMT.format(n=i)
        encoded = block.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")
        html = html.replace(placeholder, encoded)
    return html


def _convert_cann_filter(md_text: str) -> str:
    """将所有 cann-filter 标签转为 HTML 注释，保留筛选信息。

    <cann-filter> 标签（无论独立行还是行内）会被 cmarkgfm 视为 raw HTML，
    若跨表格单元格/列表项边界则产生非法 HTML 结构。转为注释后 cmarkgfm
    透传为无害注释，不破坏结构。后续由 config.mjs 将注释还原为 div[data-filter]。
    """
    md_text = _CANN_OPEN_RE.sub(r"<!--cann-filter:\1-->", md_text)
    md_text = _CANN_CLOSE_RE.sub(r"<!--/cann-filter-->", md_text)
    return md_text


def _strip_other_tags(md_text: str) -> str:
    """清除 term / ph / __gm__ / __ubuf__ 等不参与筛选的标签。

    这些标签同样会被 cmarkgfm 视为 raw HTML block，破坏 markdown 结构。
    """
    lines = md_text.split("\n")
    result = []
    for line in lines:
        if _OTHER_TAG_LINE_RE.match(line):
            continue
        result.append(line)
    md_text = "\n".join(result)
    md_text = _OTHER_TAG_WRAP_RE.sub(r"\2", md_text)
    md_text = _OTHER_TAG_ANY_RE.sub("", md_text)
    return md_text


_CALLOUT_RE = re.compile(
    r"<blockquote>\s*\n<p>\[!(NOTE|TIP|IMPORTANT|WARNING|CAUTION)\](.*?)</p>\s*\n(.*?)</blockquote>",
    re.DOTALL,
)

_CALLOUT_ICONS = {
    "NOTE": (
        '<svg class="callout-icon" viewBox="0 0 16 16" width="16" height="16">'
        '<path d="M0 8a8 8 0 1 1 16 0A8 8 0 0 1 0 8Zm8-6.5a6.5 6.5 0 1 0 0 13 6.5 6.5 0 0 0 0-13Z'
        "M6.5 7.75A.75.75 0 0 1 7.25 7h1a.75.75 0 0 1 .75.75v2.75h.25a.75.75 0 0 1 0 1.5h-2"
        'a.75.75 0 0 1 0-1.5h.25v-2h-.25a.75.75 0 0 1-.75-.75ZM8 6a1 1 0 1 1 0-2 1 1 0 0 1 0 2Z"/>'
        "</svg>"
    ),
    "TIP": (
        '<svg class="callout-icon" viewBox="0 0 16 16" width="16" height="16">'
        '<path d="M8 1.5c-2.363 0-4 1.69-4 3.75 0 .984.424 1.625.984 2.304l.214.253'
        "c.223.264.47.556.673.848.284.411.537.896.621 1.49a.75.75 0 0 1-1.484.21"
        "c-.132-.932-.5-1.583-.83-2.06A10.5 10.5 0 0 0 3.5 7.8c-.574-.686-1-1.441-1-2.55"
        "C2.5 3.11 4.637 0 8 0s5.5 3.11 5.5 5.25c0 1.109-.426 1.864-1 2.55-.202.241-.39.452"
        "-.557.645-.328.38-.534.821-.534 1.305 0 .553-.448 1-1 1s-1-.448-1-1"
        "c0-.984.424-1.625.984-2.304l.214-.253c.223-.264.47-.556.673-.848"
        "C11.549 6.26 12 5.38 12 4.5 12 2.69 10.363 1.5 8 1.5Zm2 12.75"
        "a.75.75 0 0 1-.75.75h-2.5a.75.75 0 0 1 0-1.5h2.5a.75.75 0 0 1 .75.75"
        'ZM8 12a1 1 0 1 0 0 2 1 1 0 0 0 0-2Z"/>'
        "</svg>"
    ),
    "IMPORTANT": (
        '<svg class="callout-icon" viewBox="0 0 16 16" width="16" height="16">'
        '<path d="M0 1.75C0 .784.784 0 1.75 0h12.5C15.216 0 16 .784 16 1.75v9.5'
        "A1.75 1.75 0 0 1 14.25 13H8.06l-2.573 2.574A1.458 1.458 0 0 1 3 14.543V13"
        "H1.75A1.75 1.75 0 0 1 0 11.25Zm1.75-.25a.25.25 0 0 0-.25.25v9.5"
        "c0 .138.112.25.25.25h2a.75.75 0 0 1 .75.75v2.19l2.72-2.72a.749.749 0 0 1 .53-.22h6.5"
        "a.25.25 0 0 0 .25-.25v-9.5a.25.25 0 0 0-.25-.25Zm7 2.25v2.5"
        'a.75.75 0 0 1-1.5 0v-2.5a.75.75 0 0 1 1.5 0ZM9 9a1 1 0 1 1-2 0 1 1 0 0 1 2 0Z"/>'
        "</svg>"
    ),
    "WARNING": (
        '<svg class="callout-icon" viewBox="0 0 16 16" width="16" height="16">'
        '<path d="M6.457 1.047c.659-1.234 2.427-1.234 3.086 0l6.082 11.378'
        "A1.75 1.75 0 0 1 14.082 15H1.918a1.75 1.75 0 0 1-1.543-2.575Z"
        "M8 5a.75.75 0 0 0-.75.75v2.5a.75.75 0 0 0 1.5 0v-2.5A.75.75 0 0 0 8 5Z"
        'm1 6a1 1 0 1 0-2 0 1 1 0 0 0 2 0Z"/>'
        "</svg>"
    ),
    "CAUTION": (
        '<svg class="callout-icon" viewBox="0 0 16 16" width="16" height="16">'
        '<path d="M4.47.22A.749.749 0 0 1 5 0h6c.199 0 .389.079.53.22l4.25 4.25'
        "c.141.14.22.331.22.53v6a.749.749 0 0 1-.22.53l-4.25 4.25A.749.749 0 0 1 11 16H5"
        "a.749.749 0 0 1-.53-.22L.22 11.53A.749.749 0 0 1 0 11V5"
        "c0-.199.079-.389.22-.53Zm.84 1.28L1.5 5.31v5.38l3.81 3.81h5.38l3.81-3.81V5.31"
        "L10.69 1.5ZM8 4a.75.75 0 0 1 .75.75v3.5a.75.75 0 0 1-1.5 0v-3.5"
        'A.75.75 0 0 1 8 4Zm0 8a1 1 0 1 1 0-2 1 1 0 0 1 0 2Z"/>'
        "</svg>"
    ),
}

_CALLOUT_CSS = """
.markdown-body .callout {
  padding: 0 1rem;
  margin-bottom: 1rem;
  border-left: 0.25em solid;
  border-radius: 0;
}
.markdown-body .callout .callout-title {
  display: flex;
  align-items: center;
  font-weight: 600;
  line-height: 1.5;
  margin-bottom: 0;
  padding-top: 0.5rem;
}
.markdown-body .callout .callout-title .callout-icon {
  margin-right: 0.5rem;
  flex-shrink: 0;
}
.markdown-body .callout > p:last-child {
  padding-bottom: 0.5rem;
  margin-bottom: 0;
}
.markdown-body .callout > p {
  margin-top: 0.25rem;
}
.markdown-body .callout-note {
  border-left-color: #0969da;
}
.markdown-body .callout-note .callout-title { color: #0969da; }
.markdown-body .callout-note .callout-icon { fill: #0969da; }
.markdown-body .callout-tip {
  border-left-color: #1a7f37;
}
.markdown-body .callout-tip .callout-title { color: #1a7f37; }
.markdown-body .callout-tip .callout-icon { fill: #1a7f37; }
.markdown-body .callout-important {
  border-left-color: #8250df;
}
.markdown-body .callout-important .callout-title { color: #8250df; }
.markdown-body .callout-important .callout-icon { fill: #8250df; }
.markdown-body .callout-warning {
  border-left-color: #9a6700;
}
.markdown-body .callout-warning .callout-title { color: #9a6700; }
.markdown-body .callout-warning .callout-icon { fill: #9a6700; }
.markdown-body .callout-caution {
  border-left-color: #cf222e;
}
.markdown-body .callout-caution .callout-title { color: #cf222e; }
.markdown-body .callout-caution .callout-icon { fill: #cf222e; }
"""
_TEMPLATE = """<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
{style}
</style>
</head>
<body>
<article class="markdown-body">
{body}
</article>
</body>
</html>
"""


def markdown_to_html(text: str) -> str:
    return cmarkgfm.markdown_to_html(text, options=_OPTIONS)


def gfm_to_html(text: str) -> str:
    return cmarkgfm.github_flavored_markdown_to_html(text, options=_OPTIONS)


def _is_within(path: pathlib.Path, parent: pathlib.Path) -> bool:
    try:
        path.relative_to(parent)
        return True
    except ValueError:
        return False


def _repository_roots(source_path):
    if source_path is None:
        return None, None
    source = pathlib.Path(source_path).resolve()
    for parent in source.parents:
        if parent.name in ("zh", "en") and parent.parent.name == "docs":
            return parent.parent.parent, parent.parent
    return None, None


def _fix_links(html: str, source_path=None) -> str:
    html = re.sub(
        r'(href|src)="https://gitcode\.com/cann/asc-devkit/blob/master/docs/(api|guide)/([^"]+)\.md(#[^"]*)?"',
        r'\1="/\2/\3.html\4"',
        html,
        flags=re.IGNORECASE,
    )

    repo_root, docs_root = _repository_roots(source_path)

    def _replace_markdown_link(match):
        link_path = match.group(2)
        if match.group(1).lower() == "href" and repo_root is not None:
            target = (
                pathlib.Path(source_path).resolve().parent / unquote(link_path)
            ).resolve()
            if _is_within(target, repo_root) and not _is_within(target, docs_root):
                return match.group(0)
        return f'{match.group(1)}="{link_path}.html{match.group(3) or ""}"'

    return _MD_LINK_RE.sub(_replace_markdown_link, html)


def _fix_callouts(html: str) -> str:
    def _replace(m):
        callout_type = m.group(1)
        callout_lower = callout_type.lower()
        title = m.group(2).strip()
        body_content = m.group(3).strip()
        icon = _CALLOUT_ICONS.get(callout_type, "")
        type_label = dict(
            NOTE="Note",
            TIP="Tip",
            IMPORTANT="Important",
            WARNING="Warning",
            CAUTION="Caution",
        ).get(callout_type, callout_type)
        if not body_content and "\n" in title:
            title, body_content = title.split("\n", 1)
            title = title.strip()
            body_content = f"<p>{body_content.strip()}</p>"
        title_text = title if title else type_label
        parts = [f'<p class="callout-title">{icon}{title_text}</p>']
        if body_content:
            parts.append(body_content)
        body_html = "\n".join(parts)
        return f'<div class="callout callout-{callout_lower}">\n{body_html}\n</div>'

    return _CALLOUT_RE.sub(_replace, html)


_PYGMENTS_LANG_LABELS = {
    "py": "Python",
    "python": "Python",
    "cpp": "C++",
    "c++": "C++",
    "cxx": "C++",
    "c": "C",
    "bash": "Bash",
    "sh": "Shell",
    "shell": "Shell",
    "js": "JavaScript",
    "javascript": "JavaScript",
    "ts": "TypeScript",
    "typescript": "TypeScript",
    "java": "Java",
    "go": "Go",
    "rust": "Rust",
    "json": "JSON",
    "yaml": "YAML",
    "yml": "YAML",
    "xml": "XML",
    "html": "HTML",
    "css": "CSS",
    "sql": "SQL",
    "makefile": "Makefile",
    "mk": "Makefile",
    "cmake": "CMake",
    "dockerfile": "Dockerfile",
    "docker": "Dockerfile",
    "diff": "Diff",
    "text": "Text",
    "ini": "INI",
    "toml": "TOML",
}

_PYGMENTS_FORMATTER = None
_PYGMENTS_CSS = None


def _get_pygments_formatter():
    global _PYGMENTS_FORMATTER
    if _PYGMENTS_FORMATTER is None:
        _PYGMENTS_FORMATTER = HtmlFormatter(style="default", nowrap=True)
    return _PYGMENTS_FORMATTER


def _get_pygments_css() -> str:
    global _PYGMENTS_CSS
    if _PYGMENTS_CSS is not None:
        return _PYGMENTS_CSS
    _PYGMENTS_CSS = HtmlFormatter(style="default").get_style_defs(
        ".markdown-body .highlight"
    )
    return _PYGMENTS_CSS


_HIGHLIGHT_CSS = """
.markdown-body .code-block {
  margin-bottom: 1rem;
  border: 1px solid #d0d7de;
  border-radius: 6px;
  overflow: hidden;
}
.markdown-body .code-block .code-header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 0.25rem 1rem;
  background: #f6f8fa;
  border-bottom: 1px solid #d0d7de;
  font-size: 12px;
  color: #57606a;
}
.markdown-body .code-block .code-header .lang-label {
  font-weight: 500;
  text-transform: uppercase;
}
.markdown-body .code-block pre {
  margin: 0;
  border: 0;
  border-radius: 0;
  background: #f6f8fa;
}
.markdown-body .code-block pre code {
  background: transparent;
  border: 0;
}
.markdown-body .code-header .copy-btn {
  display: flex;
  align-items: center;
  padding: 2px 6px;
  border: 1px solid #d0d7de;
  border-radius: 4px;
  background: #fff;
  color: #57606a;
  cursor: pointer;
  font-size: 12px;
}
.markdown-body .code-header .copy-btn:hover {
  background: #eaeef2;
}
.markdown-body .code-header .copy-btn svg {
  fill: #57606a;
}
.markdown-body .code-header .copy-btn.copied {
  color: #1a7f37;
  border-color: #1a7f37;
}
.markdown-body .code-header .copy-btn.copied svg {
  fill: #1a7f37;
}
"""

_CODEFENCE_RE = re.compile(
    r'<pre(?:\s+lang="([^"]*)")?>\s*<code>(.*?)</code>\s*</pre>',
    re.DOTALL,
)


def _highlight_code(html: str) -> str:
    def _replace(m):
        lang = m.group(1)
        code = m.group(2)
        text = html_mod.unescape(code)
        text = re.sub(r"<br\s*/?>", "\n", text)
        if not lang:
            lang = "text"
        label = _PYGMENTS_LANG_LABELS.get(lang.lower(), lang.capitalize())
        try:
            lexer = get_lexer_by_name(lang, stripall=True)
        except ClassNotFound:
            return f'<div class="code-block"><pre class="highlight"><code>{code}</code></pre></div>'
        formatter = _get_pygments_formatter()
        highlighted = highlight(text, lexer, formatter)
        return (
            f'<div class="code-block">'
            f'<div class="code-header"><span class="lang-label">{label}</span>'
            f'<button class="copy-btn" title="复制代码">'
            f'<svg width="16" height="16" viewBox="0 0 16 16">'
            f'<path d="M0 6.75C0 5.784.784 5 1.75 5h1.5a.75.75 0 0 1 0 1.5h-1.5'
            f"a.25.25 0 0 0-.25.25v7.5c0 .138.112.25.25.25h7.5a.25.25 0 0 0 .25-.25"
            f"v-1.5a.75.75 0 0 1 1.5 0v1.5A1.75 1.75 0 0 1 9.25 16h-7.5"
            f'A1.75 1.75 0 0 1 0 14.25Z"/>'
            f'<path d="M5 1.75C5 .784 5.784 0 6.75 0h7.5C15.216 0 16 .784 16 1.75'
            f"v7.5A1.75 1.75 0 0 1 14.25 11h-7.5A1.75 1.75 0 0 1 5 9.25Z"
            f"m1.75-.25a.25.25 0 0 0-.25.25v7.5c0 .138.112.25.25.25h7.5"
            f'a.25.25 0 0 0 .25-.25v-7.5a.25.25 0 0 0-.25-.25Z"/>'
            f"</svg>"
            f"</button>"
            f"</div>"
            f'<pre class="highlight"><code>{highlighted}</code></pre>'
            f"</div>"
        )

    return _CODEFENCE_RE.sub(_replace, html)


def parse_string(text: str, gfm: bool = True, source_path=None) -> str:
    text = _convert_cann_filter(text)
    text = _escape_lone_tildes(text)
    text, math_blocks = _extract_math_blocks(text)
    if gfm:
        html = gfm_to_html(text)
    else:
        html = markdown_to_html(text)
    html = _restore_math_blocks(html, math_blocks)
    html = _fix_links(html, source_path=source_path)
    html = _fix_callouts(html)
    html = _highlight_code(html)
    return html


def parse_file(filepath: str, gfm: bool = True) -> str:
    with open(filepath, "r", encoding="utf-8") as f:
        content = f.read()
    return parse_string(content, gfm=gfm, source_path=filepath)


def render_html(body: str) -> str:
    html = _TEMPLATE
    html = html.replace("{style}", _load_css())
    html = html.replace("{body}", body)
    return html
