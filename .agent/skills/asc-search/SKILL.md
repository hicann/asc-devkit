---
name: asc-search
description: Ascend C API 的 Agent 底层检索。接受短关键词和 API、operation、dtype、版本、产品等结构化条件，返回带文档、声明和样例定位的 JSON 中间结果。
---

# Ascend C API Agent 检索

输入由调用 Agent 整理，输出是中间 JSON，不向人澄清、不组织最终答复。当前只覆盖 C API。
共享索引引擎位于本 skill，不能另造检索实现或复制知识事实。

## 上下文和准备

调用方显式平台/版本优先；缺失时运行 `scripts/detect_context.py` 懒探测，读取设备信息和包元数据。
无法确认产品或版本时返回 `needs_context`，不能默认 3510，也不能以 2201 猜 A2/A3。
本机探测每轮上下文复用；不覆盖远端任务目标。

调用方负责准备数据和索引；`--root`、`--out`、`--index-dir` 省略时分别默认取 skill 所在仓库根
与 skill 内 `index/<surface>`。构建入口为：

```bash
python3 <skill>/scripts/build_index.py --arch <arch> --cann-version <version>
```

非 Git 数据快照传 `--source-commit <commit>`。未提供时标记 `unversioned`，不会继承外层仓库提交。索引按来源提交、目标版本、架构分别存储。
查询只检查新鲜度，不联网、不写索引。原 `ensure_index.py` 仅供显式准备/维护使用，可指定
`--root`、`--index-root`、`--source-commit`。

## 查询

```bash
python3 <skill>/scripts/search.py --platform 950 --version 9.2.0 --operation copy --keywords 非对齐
```

短关键词以空格分隔，各词 AND，匹配 title/description/family/tags；结构化维度间 AND，
逗号值内 OR。维度包括 `--stage`、`--operation`、`--category`、`--api-class`、`--dtype`、
`--convert`、`--feature`、`--kw`、`--api`、`--deps-of`、`--param`、`--enum`、`--changed-in`、`--gap`。
`--gap` 使用自身查询词，不与 `--keywords` 或文档过滤维度组合。
CLI 细节见 [index.md](references/index.md)，三类头推导见 [契约](references/index_derivation_contract.md)。

`--version 9.2.0` 匹配首次支持 ≤ 目标版本及生命周期允许的接口，`>=9.0.0` 可命中。
`--changed-in` 与 `--version` 同样归一化，`9.2`、`>=9.2.0` 与 `9.2.0` 等价。
查询版本必须与索引一致。废弃仍可返回，移除事件排除；页面命中不代表全部重载已支持。
平台同时保留产品与架构条件，A2/A3 不能仅凭共享架构混为一谈。

## 返回

`search.py` 总是返回 JSON 对象：`schema/source/source_commit/repository/root/cann_version/arch/platform/count/results`。
每条候选包含原路径、绝对路径、命中原因，以及可用的摘要、版本、声明、include 和 examples 定位。
成功含零命中退出 0；缺参、索引缺失或 stale 等错误退出 2。旧 `query.py` 保留内部过滤接口和兼容 CLI。

结果仅为候选。消费 Agent 按需阅读实际文档/声明/样例，保留版本和条件，不能将空结果解释为能力不存在。
不写文档、注释或样例；查询失败的回退策略由调用方负责。

## noLLM 输出模式

调用方传入 `noLLM`（CLI 等价开关 `--no-llm`，也可设 `ASC_SEARCH_NO_LLM=1`）时跳过整理环节，
直接输出检索用例期望片段：单候选输出 `expected`（只有一项），多候选输出 `expected_any`
（同一组，任一项命中即可），路径相对数据根，不含 JSON、摘要、声明和样例。
调用方必须原样作为最终输出，不得改写、翻译或补充。未开启时仍返回 `asc-search-results/1` 中间结果，
由调用方组织答复。两种模式都只给候选，不构成能力结论。

维护参考：[目录导航](references/README.md)、[索引结构](references/index-structure.md)、[Agent 查询组合](references/retrieve.md)。
