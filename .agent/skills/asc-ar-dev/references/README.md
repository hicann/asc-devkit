# asc-ar-dev reference 导航

本目录保存 AscendC 需求分析、实现、评审和验证规划的可复用参考材料。维护时优先保持跨环境可复用，不沉淀个人路径、机器登录方式、私有凭据、一次性提交或问题复盘。

## 目录划分

| 文档 | 内容边界 |
|------|----------|
| [README.md](README.md) | 本 reference 导航和维护规则 |
| [00_format_template.md](00_format_template.md) | 默认四段式需求和方案输出结构 |
| [01_call_rules.md](01_call_rules.md) | Host、`__global__`、`__aicore__` 调用层级和代码结构规则 |
| [02_local_environment.md](02_local_environment.md) | `DEVKIT_PATH`、`CANN_PATH`、`SOC_ARCH` 的解析和验证方法 |
| [03_devkit_snippets.md](03_devkit_snippets.md) | 最小 kernel、向量流水、matmul、debug 和访存路径片段 |
| [04_debug_dump.md](04_debug_dump.md) | `printf`、`DumpTensor`、dump 数据解析和 unsupported buffer 场景 |
| [05_build_meta.md](05_build_meta.md) | `.ascend.meta`、TLV、DFX 和编译工程入口 |
| [06_task_goal_example.md](06_task_goal_example.md) | 中性的 `TASK_GOAL` 四段式输入样例 |
| [07_requirement_type_routing.md](07_requirement_type_routing.md) | API 类需求与编译工程类需求的路由规则 |
| [08_api_lookup.md](08_api_lookup.md) | kernel 代码引入新接口前的 devkit API 文档查询和证据记录流程 |

## 与其他 skill 的边界

- 芯片调用名、`SocVersion`、`__NPU_ARCH__`、dtype 事实由 [`../../asc-npu-arch/SKILL.md`](../../asc-npu-arch/SKILL.md) 维护。
- ASC 产品代码规范由 [`../../asc-code-dev-standards/SKILL.md`](../../asc-code-dev-standards/SKILL.md) 维护；产品实现不以测试目录为参考依据。
- API UT 生成、覆盖率扫描和 `tests/api/` 补齐由 [`../../asc-api-ut-gen/SKILL.md`](../../asc-api-ut-gen/SKILL.md) 维护。
- 本 skill 不维护完整芯片类型表、通用 dtype 表或 UT 生成器事实。

## 维护规则

- 新增 reference 后，同步更新本导航和 `SKILL.md` 的 reference 列表。
- 能从 `asc-npu-arch` 或 `asc-api-ut-gen` 复用的事实，不在本目录复制一份。
- debug/dump 经验必须写成可复用判断规则；不要写入具体 commit、个人分支、私有 PR 或一次性调试日志。
