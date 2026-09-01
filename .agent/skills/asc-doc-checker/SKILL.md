---
name: doc-checker
description: |
  检查 Ascend C API Markdown 文档是否符合当前仓库写作规范。用户要求检查、审查或修改
  `docs/zh/api` 下的 API 文档、索引文档或相关 Markdown 格式、章节、芯片标签、术语、
  链接、示例、接口变更时使用。
---

# Ascend C 文档规范检查

## 权威资料

开始检查前，完整读取 `docs/zh/asc_api_writing_guidelines.md`。该文件是规则的主要权威来源；
API 页面必需与条件章节由 `docs/zh/asc_doc_contributing.md` 补充。与本技能的示例、参考资料或
现有文档存在冲突时，以这两份现行规范为准。

按检查对象按需读取以下现行资料：

- API 内容页：`docs/zh/asc_api_writing_guidelines.md` 中对应的 `API-*` 条款。
- API 页面章节清单：`docs/zh/asc_doc_contributing.md` 中的“API参考页面编写规范”。
- 导航或索引页：`docs/zh/api/README.md` 及 `API-STRUCT-*` 条款。
- 术语：`docs/zh/guide/technical_appendix/concepts_and_terms/glossary.md`。
- 已更名接口：`docs/zh/api/appendix/interface_change_description.md`。
- 已废弃接口：`docs/zh/api/appendix/deprecated_interface.md`。
- 芯片标签：`references/芯片与标签映射关系.md`。

不要将旧的 `<cann-filter>` 语法作为 API 内容页的检查标准。它只在 `docs/zh/api/README.md`
等导航配置中出现；API 内容页使用 `<!-- npu="950" id1 -->` 和 `<!-- end id1 -->`。

## 检查流程

1. 确认对象类型：API 内容页、索引页、概述页或编程指南。仅对 API 内容页执行 API 专属章节、
   产品支持、原型、参数、返回值和调用示例检查。
2. 读取权威资料和目标文件，并保留行号。若检查函数原型、参数或数据类型，读取目标 API 的头文件；
   无法定位头文件时，将该项标为“未验证”，不要报告为违规。
3. 先进行结构化扫描，再人工确认每个候选问题。扫描标点时排除代码围栏、行内代码、链接目标、
   图片路径、URL、HTML 标签和芯片过滤标签，避免把合法 Markdown 或代码误报为英文标点。
4. 按适用条款检查并记录问题。区分“强制”和“建议”；建议项不得表述为必须修改。
5. 输出检查报告。未经用户明确确认，不修改目标文档。

## 必检项

### API 内容页

- 按 `API-STRUCT-06` 和 `API-STRUCT-07` 检查一级标题与导航名称、二级章节顺序。产品支持、
  功能说明、函数原型、参数说明、返回值说明、约束说明和调用示例为必需项；数据类型、需要包含的
  头文件、关键特性说明和流水类型按接口实际情况检查。
- 按 `API-PROD-00` 至 `API-PROD-02` 检查产品支持章节位置、每个产品系列的支持状态和过滤标签。
- 按 `API-FUNC-*`、`API-PROTO-*`、`API-PARAM-*`、`API-RET-*`、`API-CONST-*` 和
  `API-EXAMPLE-*` 检查内容完整性、原型/参数/返回值与头文件的一致性，以及示例代码的有效性。
- 按 `API-SOFTWARE-*` 检查数据类型写法、排序、位宽表达和 NAN/INF 用法。
- 按 `API-TERM-00` 检查术语；只有能从术语表或上下文确定的错误才报告，歧义术语标为需确认。
- 按 `API-DEPRECATED-*` 检查废弃标识、替代接口链接、废弃接口清单和已更名接口引用。解析当前
  `interface_change_description.md` 表格获得旧名到新名映射，不要使用硬编码映射。

### 格式与链接

- 按文档低错 CheckList 检查中文标点、括号、中文与英文混排、语病、预览后的缩进/换行和链接。
- 按 `API-FORMAT-00` 至 `API-FORMAT-05` 检查列表下代码块缩进、表格、`<br>`、相对链接、单项
  列表和锚点。HTML 表格是例外场景允许的格式，不能一律报错。
- 验证本地 Markdown 链接和图片相对路径是否可解析。图片可以位于 `docs/zh/api/figures/` 或文档
  就近的 `figures/` 目录；根据实际相对路径验证，不强制单一目录或文件命名风格。
- 代码围栏检查语言标识和上下文缩进。函数原型和调用示例优先检查 `cpp`，纯文本输出可使用 `text`。

### 芯片过滤标签

- 使用 `<!-- npu="950" id2 -->` 与对应的 `<!-- end id2 -->` 成对包裹需要按芯片过滤的内容；
  根据实际产品替换 `npu` 值和数字 ID。
- 验证 `npu` 值、标签内产品名称和支持状态的对应关系；多个产品系列可用逗号分隔。
- 验证同一文件内 `idN` 唯一。不要要求 ID 从 `id1` 开始、连续递增，也不要要求标签只能包裹无序列表。
- 标签可包裹段落、列表、代码块或表格单元格。仅在标签不成对、ID 重复或产品映射错误时报告问题。

## 报告格式

按严重度排序，使用以下格式。位置使用当前客户端可访问的绝对路径 Markdown 链接。

```markdown
## 文档检查报告

文件：`<路径>`

| # | 级别 | 规则 | 位置 | 问题 | 修改建议 |
|---|---|---|---|---|---|
| 1 | 必须 | API-PROTO-01 | [第 42 行](/绝对路径:42) | 原型参数与头文件不一致 | 与头文件保持一致。 |

### 未验证项

- `API-PROTO-01`：未找到对应头文件。

### 合格项

- `API-FORMAT-03`：仓内链接均为相对路径。
```

只列出已实际检查且合格的项目。报告后先等待用户决定是否修改；用户确认后，再逐项或成组执行已确认的修改并复查。

## 参考资料

- [现行规范索引](references/writing_specs.md)
- [芯片标签参考](references/芯片与标签映射关系.md)
- [术语参考](references/terminology.md)
