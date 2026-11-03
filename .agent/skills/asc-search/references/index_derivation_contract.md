# 索引推导契约

本契约是 asc-search 的唯一推导规则文档：索引是三类头的纯派生数据，本文定义推导维度、残留
裁决边界与筛选条件规则。文档头如何写入、标注证据规则属于建设侧契约，本 skill 不复制。
当前实例为 c_api 全量（SCOPES categories=None）。

## 1. 三类头（推导输入）

| 头类型 | 载体 | 提供的事实 |
| --- | --- | --- |
| 文档头 | Markdown front matter（`okftype/description/tag/dependencies/version/version_changes`） | 语义信息：页面种类、摘要、角色/操作/特性标签、关联文档、接口首次支持版本、版本变更事件时间线 |
| include 头 | 头文件 Doxygen（`@ingroup/@brief/@override_id/@archs/@param`）与声明 | 重载功能摘要与结构事实：逐重载编号、适用架构、参数方向与类型、原型与地址空间 |
| examples 头 | Agent 选定的源文件语义范围首尾的 `//! [id1, id2, ...]` marker（兼容单 ID） | 同一展示范围与多个重载的关联 |

三类头都是只读输入；索引构建不修改 `docs/` 正文、include 或样例。文档头中的 tag 值合法性
以本 skill `config/tag-taxonomy.json`（随包分发的冻结词表）校验。
include 的 `@brief` 是面向开发者的英文重载摘要，当前解析器不将它写入索引；
文档语义仍来自 Markdown 头的 `description/tag/dependencies`，索引不额外消费 brief。

## 2. 推导契约（索引推导期，脚本执行）

下表全部由本 skill 的推导引擎（`scripts/derive/`）从三类头与仓内数据生成，不写入文档头或标签：

| 推导维度 | 来源 | 规则 |
| --- | --- | --- |
| `title` | 文档 H1 | 一级标题 |
| `api_class` | include 路径 / `@ingroup` | `include/c_api/<子目录>` 即 API 分类（vector_compute/reg_compute/cube_compute/cube_datamove/scalar_compute/atomic/cache_ctrl/sync/spr/utils/defs/composite/…） |
| `execution_model` | include 路径 + 原型 mangling | vector_compute→memory_vector；reg_compute→register_vector；scalar_compute→scalar；cube_*→matrix_compute；mangling 形态佐证（`Rv*` 寄存器侧、`PU*` 内存侧） |
| `source/destination_locations` | 原型 mangling 地址空间码 | `G`=GM、`U`=UB、`L1`=L1、`FB`=fixpipe、`CA/CB/CC`=L0A/L0B/L0C；`v`=矢量寄存器（vreg）、`vb`=掩码寄存器（preg）；mangling 解析实现见 `scripts/derive/include_db.py` |
| `signature/override_id/dtype` | include Doxygen + 声明 | `@override_id` 逐重载枚举；dtype 从 mangling 类型码还原 |
| `archs` | include `@archs` + `__NPU_ARCH__` 守卫 | 逐重载架构，文档级取并集 |
| `header` | include 声明位置 | 唯一声明头文件路径 |
| `masked` | mangling `_vb` 后缀 | 任一重载含掩码参数即支持掩码模式 |
| `deprecated` | include `ASC_DEPRECATED(v,date,replacement)` + 文档声明 | 替代目标同步推导 |
| `params`（参数反查） | include 声明 + `@param` | 参数名/类型（含枚举类型形参）→ API 反查视图 |
| `enums/macros 目录` | `include/c_api/defs/*.h` | 枚举值、常量、宏、union、类型别名全量目录 |
| `examples` | 样例成对 marker 的 ID 列表，展开为独立 `override_id` | 记录 `file/start_line/end_line`（行号不含 marker）；每个重载关联同一展示范围；支持 `.asc/.c/.cc/.cpp/.cxx/.h/.hpp`。扫描根为**整棵 `examples/`**：marker 由建设侧按 canonical `override_id` 写入，查表是「文档侧 include DB 的 `override_id` → examples DB」，没有 marker 的样例不可能被查到，因此放宽根目录不会把同名但不同家族的接口（如 SIMT）关联进来 |
| `navigation` | 正文链接 | 仓内相对链接保序去重 |
| `products` | 正文 `<!-- npu="X" -->` 标记 × `config/product-map.yaml` | 产品级支持/不支持矩阵 |
| `feature 视图` | dtype 家族 × roles/operations | 低精度（fp8_*/hifloat8/fp4x2_*）、int4 等特性族 |
| `by_version 视图与版本查询` | 文档头 `version` 与生命周期事件 | 视图保留 `{首次支持字段原串: [文档]}`；`--version 9.2.0` 按范围及生命周期判断可用性，`>=9.0.0` 可命中，已废弃但未移除仍保留。查询目标必须与索引构建版本一致；无 version 的导航页不命中 |
| `by_change_version 视图` | 文档头 `version_changes[].since` | `{since: [文档]}`：只收**非空**事件（`version_changes: []` 与无该字段的文档不入视图），`since` 为版本**点**（如 `9.2.0`）。该视图供 `--changed-in` 查询，首次支持范围及生命周期由版本可用性筛选消费。**索引内是降维副本**：只留 `since/kind/scope/note/scope_basis`；带 `evidence` 的旧头接受并忽略 |
| `api_surfaces` | 文档路径 | `SIMD-API/<surface>/` 即层级 |

## 3. LLM 辅助边界（索引构建）

1. 确定性脚本管线承载第 2 节全部主推导。
2. 脚本无法裁决的残留（复杂模板声明、Release Notes 歧义条目、文档与 include 冲突、
   缺口归类）写入 `index/<surface>/unresolved.json`。
3. 单轮 LLM 裁决残留，结果固化为 `derive-overrides/<surface>.json`（落盘、可审查、可回放）。
4. 构建确定性消费 overrides；同输入两次构建字节一致；构建过程不内联调用 LLM。

## 4. 索引筛选条件（CANN 版本 × 架构 × 层级）

- CANN 版本检测链：`--cann-version` 显式参数 > `$ASCEND_HOME_PATH` 安装目录版本 >
  仓 `version.cmake`。
- 构建参数：`--scope`（默认 c_api）、`--arch`（默认 3510，经 `config/product-map.yaml` 映射架构号）。
- 只索引同时满足三条件的 API：目标 CANN 已达到 version 首次支持 ∧ 明确的生命周期事件允许 ∧ archs 含目标架构 ∧ 属于目标层级；被过滤 API 计入 `meta.filtered_by_version/arch` 供核对。
- 检索前置（`scripts/ensure_index.py`，检索流程步骤 0）：按同一检测链得出目标筛选条件，
  committed 索引筛选条件匹配且指纹新鲜（build-meta 文档/源码/脚本/裁决哈希 + scope 文档数；`--strict` 全量重建比对）
  才直接使用，否则原地重建后再检索。

## 5. 首次支持与变更事件消费

- `version` 新格式仅为 `>=首次支持版本`。不合法的版本拒绝构建，不静默过滤。
  存量 `>=x,<=y` 兼容读取为 `>=x`，在 unresolved 明示旧上限被忽略，待建设侧核实事件；
  不能把废弃时间当成移除时间。
- `version_changes` 独立表达版本事实，since 比较支持数字补齐及预发布版本。
  evidence 可为字符串或源记录列表，索引均不复制；kind 扩展仍宽进透传。
- 具体 `overload:<id>[@<arch>]` 的 introduced/overload_added 确定该重载的首次出现，
  明确 removed 确定移除生效点；按目标版本/架构筛选后才生成 dtype、参数、样例等视图。
  page removed 只有 scope_basis 非 fallback 时才表示整个接口移除。
- deprecated 是状态，版本未到不提前标废弃，已废弃仍保留候选。
  已生效证据按声明自身、指定架构重载、普通重载、非 fallback 整页事件的优先级选择；
  同一优先级取最早废弃版本，不受事件顺序影响。声明自身的替代目标保留。
  返回值和参数变化不由名字匹配猜测旧签名；query 返回目标版本与事件上下文，由检索 Agent
  核对具体用法。page version 命中不能证明所有重载已支持。
- Agent 选择的 marker 范围原样关联，search 不按调用位置重新裁剪；多个 ID 关联同一范围。
- ensure 的输入指纹同时覆盖 include、examples、推导脚本和 overrides，工具链升级后旧索引须重建。
