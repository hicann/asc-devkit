# asc-search reference 导航

本目录保存 asc-search 两个工作流（建立索引、使用索引）的流程文档与推导契约。运行所需配置、
脚本与测试在各同级目录。

## 目录划分

| 文档 | 内容边界 |
|------|----------|
| [README.md](README.md) | 本 reference 导航和维护规则 |
| [index.md](index.md) | 建立索引：三类头 → 按适用条件筛选的持久索引的构建流程、资产结构与查询 CLI |
| [index-structure.md](index-structure.md) | `index.json` 的实际 JSON 结构、字段含义、倒排视图、依赖图和示例 |
| [retrieve.md](retrieve.md) | 使用索引：意图分析、多轮查询、输出格式与检索策略 |
| [index_derivation_contract.md](index_derivation_contract.md) | 推导规则唯一事实源：推导维度、LLM 兜底边界、索引筛选条件 |
| [三类知识标注与检索工具链框架图](asc-search-knowledge-toolchain-9.2.0.svg) | 三类头的标准格式、真实样例、关联关系及工具链消费流程 |

## 维护规则

- asc-devkit 是机器检索 asc-search 的规范源；cannbot-knowledge 分发同一核心，人类包装由知识仓维护。
  交付前运行 `scripts/check_mirrors.py --source <devkit-repo> --target <knowledge-repo>`，asc-search（排除运行缓存）须字节一致。
  目录或 manifest 缺失、源和目标指向同一目录时返回错误，不能当作一致。
  运行时自动同步由知识仓前置 skill 管理，不能覆盖本地未提交修改，不自动提交或推送。
- 推导规则只在《index_derivation_contract.md》维护；流程文档只引用，不复述推导规则。
- 构建与前置保障只依赖三类头与本 skill `config/`，不得引用其他 skill 的文件。
- 文档头/include/examples 变化后重跑 `scripts/build_index.py --scope c_api` 并提交索引 diff；
  检索触发的本地重建 diff 只在维护流程确认后提交。
