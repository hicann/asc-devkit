# 建立索引工作流

Agent 公共入口为 `scripts/search.py --root <data-root> --index-dir <instance> --platform <id>`，
支持 `--keywords <词...>` 与现有维度组合，统一 JSON 输出。数据根与 skill 根独立，
非 Git 快照构建时传 `--source-commit`；`ensure_index.py --index-root` 仅用于显式准备，
查询不自动建索引。自然语言交互由调用方负责。

把已打标文档构建为持久索引，支撑检索流程（[retrieve.md](retrieve.md)）。索引是三类头的
**纯派生数据**：文档 slim 头（语义）+ include Doxygen/声明（结构）+ examples marker（关联），
推导规则以 [index_derivation_contract.md](index_derivation_contract.md) 为唯一契约。
索引 JSON 的字段和实例示例见 [index-structure.md](index-structure.md)。
当前构建范围为 c_api（SCOPES categories=None），默认架构为 3510；索引文档数随已标注范围和筛选条件变化，以 `ensure_index.py` 输出的 `doc_count` 为准。

## 资产结构

```
asc-search/
  scripts/build_index.py        # 构建 + 校验 + 漂移检测（--check）；--arch/--cann-version 筛选条件
  scripts/ensure_index.py       # 前置保障（检索步骤 0）：筛选条件匹配 + 指纹新鲜 → 按需原地重建
  scripts/query.py              # 查询 CLI（唯一检索入口）
  scripts/derive/               # 推导引擎：include_db / docparse / examples_db / gate / versioning / overrides / slim_header
  config/tag-taxonomy.json      # 随包分发的冻结词表（tag 合法值校验）
  config/product-map.yaml       # 架构标签 ↔ 架构号、产品支持映射
  derive-overrides/c_api.json   # LLM/人工裁决残留（unresolved.json 的固化结论，可审查可回放）
  index/c_api/index.json        # 索引实例（构建产物）：documents + 16 视图 + 依赖图 + defs 目录
  index/c_api/build-meta.json   # 源 commit、文档 sha256、词表/映射指纹
  index/c_api/gaps.json         # 缺口登记（可选、人工维护；缺失视为空台账，随实例落盘快照）
  index/c_api/概览.md            # 人读概览（构建产物）
  tests/test_index.py           # 筛选条件 + 推导正确性 + 确定性 + ensure 决策 + dryrun 回归
  tests/dryrun.yaml             # 查询清单（3510/A3 双实例）
```

## 构建筛选条件与流程

```bash
python3 scripts/build_index.py --scope c_api [--arch 3510] [--cann-version 9.2.0] [--check]
```

1. **CANN 版本检测链**：`--cann-version` > `$ASCEND_HOME_PATH` 安装目录 > 仓 `version.cmake`。
2. **架构映射**：`--arch`（默认 3510）由本 skill `config/product-map.yaml` 校验，其他标签映射为架构号。
3. 只索引同时满足三条件的 API：目标版本达到头内 `version` 首次支持 ∧ 明确生命周期事件允许 ∧ 推导 archs 含目标架构 ∧ 属于目标层级；被过滤 API 计入 `meta.filtered_by_version/arch`。
4. 推导（脚本为主）：
   - include 声明 + Doxygen → 签名（override_id/archs/dtype/位置/参数/掩码/废弃）、
     `api_class`（include 子目录）、`execution_model`、defs 目录（枚举/常量/宏/别名/union）；
   - examples `//! [override_id]` marker → 样例关联；
   - 正文 → 标题、导航链接、npu 产品标记 → 产品支持；
   - 架构变体页只保留本架构声明（无 @archs 声明继承页面架构）；家族页名不带序号时从正文
     函数原型提取真实 API 名回查 include。
5. 残留进 `unresolved.json`（静默重载无示例链接、archs 需 npu 兜底等）；单轮 LLM/人工裁决后
   固化为 `derive-overrides/<surface>.json`，构建确定性消费——**构建过程不内联调用 LLM**。
6. 构建内联全量校验，任一失败拒绝出索引；输出不含时钟，同输入两次构建字节一致；
   `--check` 做漂移检测（文档改动未重建时退出码 1）。

## 查询接口

`query.py` 过滤维度间 AND、逗号值内 OR。

**展开类查询**（`--api` / `--deps-of` / `--param` / `--enum`）是"展开"而非"过滤"：
单独使用时按各自的语义返回，**一旦同时给出过滤维度（含 `--changed-in`），结果会与
过滤维度求交**（例如 `--api asc_x --changed-in 9.2.0` 只保留 9.2.0 有变更事件的候选）。
`--gap` 描述的是范围外缺口，没有文档维度可求交，**与过滤维度组合会直接报错**（exit 1），
不会静默忽略条件。

```
--surface c_api                # 索引实例（默认；--index-dir 显式指定目录）
--stage in,out,compute         # 分阶段候选（roles + 推导位置判定）
--operation copy,divide        # operations 维度
--category vector_compute      # 文档目录分类
--api-class vector_datamove    # API 分类 = include 子目录（reg_compute/vector_datamove/...）
--dtype half,fp8_e4m3fn        # dtype（别名归一：f32→float、e4m3→fp8_e4m3fn）
--kw 非对齐,填充                # title+description 子串
--convert half int4            # 类型转换对
--feature low_precision        # 特性（low_precision/low_precision_matrix/int4...）
--version 9.2.0               # 目标版本可用性：首次支持 ≤ 9.2.0 且生命周期允许；
                              #  '>=9.0.0' 页面可命中。兼容 '>=9.2.0'，同样表示目标 9.2.0
--changed-in 9.2.0            # 版本变更事件维度：version_changes[].since == 9.2.0 的文档，
                              #  输出事件摘要（since/kind/scope/note）；空事件文档不入该视图
--product 950,A3               # 产品 npu 标记
--api asc_copy_gm2ub           # 家族卡：主页+同族成员+分类页
--deps-of <api|path> [--kind K] [--reverse]   # 依赖图遍历
--param round_mode             # 参数名或枚举类型反查（如 asc_cast_round_mode）
--enum asc_cast_round_mode     # 枚举名或枚举值反查（如 ASC_CAST_RN → 使用方 API）
--gap 同步,寄存器               # 缺口登记检索
--json                         # 机器可读输出
```

输出统一为 `path | API 名 | 命中原因`；枚举反查返回值目录与使用方。`--changed-in` 命中的候选另起一行
输出「版本变更: since kind scope（note）」。普通候选也携带目标 CANN 与版本事件上下文，
便于核对重载新增、参数/返回值变化、废弃与移除。`--version 9.2.0` 返回在目标版本可用的候选，
包括更早首次支持的接口；已废弃但尚未移除的接口仍可命中。答"某 CANN 版本新增/改了什么"用
`--changed-in`。没有首次支持版本的导航页不参加 `--version` 匹配。

索引在构建时已按版本筛掉页面及重载，无法用另一个版本的索引完整回答目标版本可用性。
`--version` 与索引 `meta.cann_version` 不一致时直接报错；先用
`ensure_index.py --cann-version 9.2.0 --arch 3510` 准备对应版本的索引，或构建临时实例并指定
`--index-dir`。版本比较支持预发布版本（例如 `9.3.0-beta.1` 早于 `9.3.0`）。

## 前置保障（检索步骤 0）

```bash
python3 scripts/ensure_index.py [--arch 3510] [--cann-version X] [--skip-drift] [--strict] [--json]
```

检测链与构建同构（surface 默认 c_api）。committed 实例筛选条件匹配且指纹新鲜 → 零动作；
缺失、筛选条件不符或漂移 → 原地重建。指纹默认轻量档（build-meta `document_sha256` 逐文件 +
scope 文档数 + 词表/映射/gaps 哈希 + include/examples/scripts/overrides 指纹），`--strict` 走全量重建比对；`--skip-drift`
只查筛选条件。输出 JSON（`cann_version/arch_label/categories/doc_count/actions`）即检索
上下文；失败时退出码 1，由检索侧如实转告。

## 维护边界

- **自含性**：构建与前置保障只依赖三类头与本 skill（脚本 + `config/`），不得 import 或读取
  其他 skill 的文件。
- 文档头变化（建设侧重新打标）或 include/examples 变化后：重跑
  `build_index.py --scope c_api` 并提交索引 diff；`--check` 报漂移属预期报警。
  检索侧触发 ensure 重建产生的本地 diff 同样只在维护流程确认后提交。
- `derive-overrides/<surface>.json` 只登记脚本无法裁决、经人工核对的事实，每条附 reason；
  新增推导规则可消除的条目应删除。
- gaps.json 可选：人工按证据增补（hint/evidence 必须是真实仓库路径）；缺失或为空时构建照常
  出索引（`--gap` 返回空），`--out` 构建的实例目录始终落盘快照供 query 直接加载。
- 词表合法性以本 skill `config/tag-taxonomy.json`（随包分发的冻结词表）校验；
  接入新范围时在 `SCOPES` 登记并扩充 dryrun 清单（c_api 全量已放开 categories=None）。
