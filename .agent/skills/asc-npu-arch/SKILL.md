---
name: asc-npu-arch
description: Ascend NPU 架构知识技能。触发：当用户需要确认芯片型号、NpuArch、SocVersion、__NPU_ARCH__、架构差异、特性支持、API 可用范围或条件编译分支时使用。
---

# Ascend NPU 架构知识

## 使用边界

本 skill 面向 `asc-devkit` 中的 AscendC API 设计、实现、UT 和验证工作，用于确认芯片架构与 API 能力边界。非 API 能力边界相关的工程模板、执行流程和优化策略不属于本 skill 范围。

## 统一维护原则

- 芯片调用名、`NpuArch` / `SocVersion` / `__NPU_ARCH__` 映射、通用 dtype 视图和基础 dtype 大小在本 skill 中统一维护。
- 脚本需要消费这些事实时，必须读取 [`references/npu-arch-facts.json`](references/npu-arch-facts.json)，不要在脚本中并行维护芯片或 dtype 表。
- 其他 AscendC skill 只保留场景特有的限制或约束，不再并行维护完整的芯片类型表、通用数据类型枚举和 dtype 大小表。
- 当其他 skill 需要说明芯片范围、架构映射、低精度格式、通用基础 dtype 或 dtype 大小时，应显式引用本 skill 或 `references/npu-arch-guide.md`。

## 架构代际概述

| 概念 | 说明 |
|-----|------|
| **NpuArch** | 芯片架构号，定义指令集和微架构 |
| **SocVersion** | 片上系统版本，软件命名标识 |
| **__NPU_ARCH__** | Device 侧编译宏，四位数值，用于条件编译 |

## 统一芯片调用名

下表中的“统一调用名”是各 skill、脚本和测试中应优先复用的入参名或文档名。

| 统一调用名 | __NPU_ARCH__ | SocVersion | 产品系列 / 常见芯片 | 说明 |
|-----------|:------------:|------------|---------------------|------|
| `ascend910` | 1001 | ASCEND910 | Atlas 训练系列 / Ascend910 | 初代训练架构 |
| `ascend910b1` | 2201 | ASCEND910B | Atlas A2 / Ascend910B1~B4, Ascend910B2C | 与 A3 共用 2201 |
| `ascend310p` | 2002 | ASCEND310P | Atlas 推理系列 / Ascend310P1, Ascend310P3 | 与 `ascend610` 共用 2002 |
| `ascend610` | 2002 | ASCEND310P | Atlas 推理系列 / Ascend610 | 与 `ascend310p` 共用 2002 |
| `ascend310b1` | 3002 | ASCEND310B | Atlas 200I/500 A2 / Ascend310B1~B4 | 推理架构 |
| `ascend950pr_9599` | 3510 | ASCEND950 | Atlas A5 推理 / Ascend950PR | 当前 regbase / SIMT 主入口 |

> **注意**：`Ascend910_93` 的 SocVersion 字符串在运行时映射到 `SocVersion::ASCEND910B`，不是独立运行时 SocVersion；NpuArch 同为 `DAV_2201`。

## 完整映射表

| __NPU_ARCH__ | SocVersion | 产品系列 | 芯片型号 |
|-------------|------------|---------|---------|
| 1001 | ASCEND910 | Atlas 训练系列 | Ascend910 |
| 2002 | ASCEND310P | Atlas 推理系列 | Ascend310P1, Ascend310P3, Ascend610 |
| 2201 | ASCEND910B | Atlas A2 训练/推理系列 | Ascend910B1~B4, Ascend910B2C |
| 2201 | ASCEND910B | Atlas A3 训练/推理系列 | Ascend910_93 |
| 3002 | ASCEND310B | Atlas 200I/500 A2推理产品 | Ascend310B1~B4 |
| 3510 | ASCEND950 | Atlas A5 训练系列 | Ascend950DT |
| 3510 | ASCEND950 | Atlas A5 推理系列 | Ascend950PR |

## 统一数据类型视图

### 通用基础 dtype

本节依据仓内 `docs/zh/api/context/内置数据类型.md` 维护 AscendC 内置 dtype 事实。脚本消费入口是 [`references/npu-arch-facts.json`](references/npu-arch-facts.json)，不要解析 Markdown 表格。

下表是各 skill 在未遇到特定 API 约束时，应优先复用的通用基础 dtype 视图。这里的“可直接初始化”只表示通用 UT 模板可以安全构造样例数据，不表示任意 API 都支持该类型。

| 类型组 | 典型类型名 | 说明 |
|-------|-----------|------|
| 可直接初始化 | `bool`, `int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `half`, `int32_t`, `uint32_t`, `float`, `int64_t`, `uint64_t` | 文档明确支持立即数赋值和初始化；通用 UT 生成器可直接使用 |
| 需 API 特化初始化 | `bfloat16_t`, `double`, `complex32`, `complex64` | 文档列为内置类型或产品支持类型，但通用模板不得仅凭 dtype 名直接初始化 |
| 低精度打包/8-bit 浮点 | `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `fp8_e8m0_t`, `int4x2_t` | 需要确认打包布局、转换 API 和目标 API 支持范围 |

基础 dtype 的 `sizeof`、32B 对齐元素数、简写和产品支持范围统一见 [Ascend NPU 架构指南](references/npu-arch-guide.md#基础-dtype-大小与-32b-对齐)。

### DAV_3510 扩展低精度 dtype

以下 dtype 或格式能力集中归口到本 skill；只有设计文档和本地代码明确支持时，其他 skill 才能继续展开：

| 能力 | 说明 | 使用规则 |
|-----|------|---------|
| FP4 E2M1 / E1M2 | 两个 4-bit 浮点值打包到一个 8-bit 存储单元 | 必须确认打包顺序、转换 API 和 golden 生成方式 |
| FP8 E5M2 | 5 位指数、2 位尾数的 8-bit 浮点格式 | 仅在文档和本地代码显式声明时使用 |
| FP8 E4M3FN | 4 位指数、3 位尾数的 8-bit 浮点格式 | 仅在文档和本地代码显式声明时使用 |
| FP8 E8M0 | 8 位指数、0 位尾数的 8-bit 浮点格式 | 必须确认转换路径和特殊值语义 |
| HiFloat8 | 华为自定义 8-bit 浮点格式 | 先确认本地类型名和 API 行为 |
| INT4 | `int4x2_t` 打包两个 4-bit 整数；`S4` 简写对应 `int4b_t` | 先确认打包布局、声明和 UT 覆盖范围 |

## Ascend950 (DAV_3510) 相关能力

| 能力 | 说明 | API 生成关注点 |
|-----|------|---------------|
| Regbase 编程 | 直接操作寄存器的编程能力 | 仅在文档或已有代码明确支持时生成相关 API 分支 |
| SIMT 编程 | 线程级并行编程模型 | 检查 `include/simt_api`、`impl/simt_api` 和目标芯片范围 |
| 低精度格式 | FP8、FP4、INT4 等数据格式 | 检查 dtype 声明、类型转换和 UT 覆盖范围 |

## API 生成使用规则

- 当 API 行为、dtype、芯片范围或条件编译与架构相关时，先使用本 skill 确认边界。
- 不要只根据芯片名称推断 API 是否支持，必须结合设计文档和本地已有代码模式。
- `__NPU_ARCH__` 适用于 Device 侧条件编译；Host 侧架构判断应遵循本地已有代码模式。
- 如果目标芯片与文档、代码或 UT 约束不一致，先输出冲突点，不要直接扩大支持范围。

## 场景

- 用户指定芯片型号，需要确认对应的 `NpuArch`、`SocVersion` 和 `__NPU_ARCH__`。
- 设计文档要求某个 dtype 或 API 只在部分芯片上可用，需要确认支持边界。
- API 实现或 UT 中出现架构条件分支，需要确认分支是否覆盖目标芯片。
- 最终验证报告需要说明哪些芯片范围已覆盖，哪些范围未验证。
- 其他 skill 需要引用通用芯片调用名或基础 dtype 集合时，需要回到本 skill 统一取值。

## 错误处理

- 文档中的芯片型号无法映射到本表时，输出缺口并要求补充来源，不要自行扩展映射。
- 文档声明的支持范围与本地已有代码不一致时，先报告冲突，不要直接扩大 API 能力。
- 缺少目标芯片或架构信息时，按未知项记录在最终报告中，不要默认选择某个架构。
- 需要确认运行时实现细节时，回到本地同类 API 模式或设计文档，本 skill 不展开具体实现方式。

## 详细文档

- [Ascend NPU 架构指南](references/npu-arch-guide.md)
- [结构化架构事实](references/npu-arch-facts.json)
