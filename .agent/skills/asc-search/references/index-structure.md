# 索引结构说明

`asc-search` 的索引是按 `surface`、CANN 版本和 NPU 架构生成的 JSON 实例，不是全文倒排文件，也不包含模型向量。默认实例位于：

```text
.agent/skills/asc-search/index/c_api/
```

构建命令：

```bash
python3 .agent/skills/asc-search/scripts/build_index.py \
  --scope c_api --arch 3510 --cann-version 9.2.0
```

## 文件布局

```text
index/c_api/
├── index.json       # 查询主数据：文档、倒排视图、依赖图、defs
├── build-meta.json  # 输入文件指纹和构建信息
├── gaps.json        # 范围外缺口登记
├── unresolved.json  # 推导过程中待人工裁决的问题
└── 概览.md           # 从 index.json 生成的人读统计
```

`ensure_index.py` 读取 `build-meta.json`、输入文档和源码指纹，确认实例可用；索引缺失、筛选条件不匹配或输入发生漂移时，会调用构建流程重新生成实例。

## `index.json` 顶层结构

```json
{
  "schema": "okf-index/5",
  "meta": {},
  "documents": {},
  "views": {},
  "graph": {},
  "defs": {},
  "scope": {}
}
```

### `meta`

`meta` 描述索引实例本身，而不是某个 API：

```json
{
  "surface": "c_api",
  "source_commit": "3304966e54d9f9e25d3b093a5c594fac27c12ae4",
  "commit_date": "2026-10-26T16:04:53+08:00",
  "cann_version": "9.2.0",
  "arch_label": "3510",
  "arch": "3510",
  "doc_count": 616,
  "primary_count": 602,
  "nav_page_count": 14,
  "filtered_by_version": 0,
  "filtered_by_arch": 243,
  "pending_no_okf": 0,
  "unresolved_count": 90,
  "gap_ids": []
}
```

查询使用的 CANN 版本必须与 `meta.cann_version` 一致；不同版本要先构建或准备对应索引实例。

### `documents`

`documents` 是以仓库相对路径为键的文档实体库。导航页也会进入这里，但没有 API 重载时其 `signatures` 为空。

```json
{
  "docs/zh/api/SIMD-API/c_api/reg_compute/reg_arith/asc_abs.md": {
    "okftype": "reference.api",
    "title": "asc_abs",
    "description": "按 mask 对矢量寄存器中的元素逐元素取绝对值…",
    "tags": {
      "roles": ["compute"],
      "operations": ["absolute"],
      "traits": ["elementwise"]
    },
    "dependencies": [
      {
        "path": "docs/.../data_type_definition.md",
        "kind": "related",
        "condition": "正文据此说明矢量数据寄存器"
      }
    ],
    "version": ">=9.0.0",
    "version_changes": [
      {
        "since": "9.2.0",
        "kind": "overload_added",
        "scope": "section:函数原型"
      }
    ],
    "api_class": "reg_compute",
    "execution_model": "register_vector",
    "signatures": [
      {
        "override_id": "asc_abs__Rvi8_vi8_vb",
        "archs": ["3510"],
        "header": "c_api/reg_compute/compute/reg_arith.h",
        "dtypes": ["int8"],
        "locations": ["mask_register", "vector_register"],
        "masked": true,
        "params": [
          {"name": "src", "type": "vector_int8_t", "direction": "in"}
        ],
        "return_type": "void",
        "deprecated": null
      }
    ],
    "headers": ["c_api/reg_compute/compute/reg_arith.h"],
    "examples": [],
    "product_support": {"950": "支持"},
    "family": "asc_abs",
    "category": "reg_compute",
    "nav_page": "docs/.../reg_compute.md",
    "applied_overrides": []
  }
}
```

其中：

- `tags` 来自文档 slim 头；
- `signatures` 来自 include 声明和 Doxygen，按 overload 保存架构、dtype、参数、位置和掩码信息；
- `version_changes` 用于判断页面或具体 overload 在目标版本是否存在、废弃或移除；
- `dependencies` 是文档声明的依赖边；
- `family` 用于把同一 API 的架构页、deprecated 页和重载页聚合起来；
- `applied_overrides` 记录人工裁决文件对自动推导结果的影响。

### `views`

`views` 是查询用的倒排视图。视图的值都是文档路径数组，查询先在这些数组上做集合运算，再回到 `documents` 读取完整记录。

| 视图 | 索引内容 |
| --- | --- |
| `by_stage` | `in`、`out`、`compute` 阶段 |
| `by_role` | API 角色 |
| `by_operation` | `copy`、`absolute`、`matrix_multiply_accumulate` 等操作 |
| `by_category` | 文档目录分类 |
| `by_api_class` | include API 分类 |
| `by_dtype` | 输入或输出数据类型 |
| `by_dtype_pair` | 类型转换对，例如 `half->int4` |
| `by_override_id` | include overload 标识 |
| `by_header` | 声明所在头文件 |
| `by_api` | API family 到成员文档 |
| `by_param` | 参数名或枚举类型到使用方文档 |
| `by_enum` | 枚举类型到使用方文档 |
| `by_feature` | `low_precision`、`int4` 等派生特性 |
| `by_version` | 文档原始 `version` 字段 |
| `by_change_version` | `version_changes[].since` |
| `by_product` | 产品支持标记 |

例如：

```json
{
  "by_operation": {
    "absolute": [
      "docs/.../reg_compute/reg_arith/asc_abs.md"
    ]
  },
  "by_dtype": {
    "int8": [
      "docs/.../reg_compute/reg_arith/asc_abs.md"
    ]
  }
}
```

不同过滤维度之间求交，同一维度的逗号值求并；`--kw` 在候选集上对 `title + description` 做大小写不敏感的子串匹配。

### `graph`

`graph.edges` 保存正向依赖边，`graph.reverse` 按依赖类型保存反向边：

```json
{
  "edges": [
    {
      "from": "docs/.../asc_abs.md",
      "to": "docs/.../data_type_definition.md",
      "kind": "related",
      "condition": "正文据此说明矢量数据寄存器"
    }
  ],
  "reverse": {
    "related": {
      "docs/.../data_type_definition.md": ["docs/.../asc_abs.md"]
    }
  }
}
```

`--deps-of` 查询正向边，增加 `--reverse` 查询反向边；依赖目标在当前索引之外时，结果会标记 `out_of_scope`。

### `defs`

`defs` 保存从头文件推导出的可反查定义：

```json
{
  "enums": {},
  "constants": [],
  "macros": [],
  "aliases": [],
  "records": []
}
```

`--enum` 可以从枚举名或枚举值找到头文件和使用方；`--param` 可以从参数名或枚举类型反查 API 文档。

### `scope`

`scope` 记录本次实例实际纳入的主文档和导航页路径。它用于区分“当前索引范围内没有结果”和“文档根本不在本次索引范围内”。

## 三类输入到索引字段

```text
Markdown slim header ────────┐
include Doxygen + declaration ├─> documents + views + defs
examples marker ─────────────┘             │
文档依赖声明 ──────────────────────────────┴─> graph
```

索引是纯派生产物。修改文档头、include 注解、样例 marker、词表、产品映射或推导脚本后，需要重新运行构建并由 `ensure_index.py` 检查指纹是否新鲜。
