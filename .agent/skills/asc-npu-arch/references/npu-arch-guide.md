# Ascend NPU 架构指南

本文档说明 Ascend NPU 的架构代际划分，以及在 `asc-devkit` 中进行 AscendC API 设计、实现和验证时需要关注的架构边界。

---

## 目录

1. [架构代际概述](#架构代际概述)
2. [统一维护原则](#统一维护原则)
3. [统一芯片调用名](#统一芯片调用名)
4. [完整映射表](#完整映射表)
5. [统一数据类型视图](#统一数据类型视图)
6. [条件编译边界](#条件编译边界)
7. [Ascend950 DAV_3510 相关能力](#ascend950-dav_3510-相关能力)
8. [API 兼容性检查清单](#api-兼容性检查清单)

---

## 架构代际概述

### 核心概念

| 概念 | 说明 |
|-----|------|
| **NpuArch** | 芯片架构号，定义指令集和微架构 |
| **SocVersion** | 片上系统版本，软件命名标识 |
| **__NPU_ARCH__** | Device 侧编译宏，四位数值，用于条件编译 |

### 架构代号别名

| 代号 | 对应 SocVersion | 说明 |
|-----|----------------|------|
| **A2** | ASCEND910B | Ascend910B1~B4, Ascend910B2C |
| **A3** | ASCEND910B (含 Ascend910_93) | 训练/推理芯片 |
| **A5** | ASCEND950 | Ascend950DT / Ascend950PR |

### 关键细节

- 一个 `NpuArch` 可以对应多个芯片型号或产品系列。
- `Ascend910_93` 的 SocVersion 字符串在运行时映射到 `SocVersion::ASCEND910B`，NpuArch 同为 `DAV_2201`。
- 源码中可能存在 `SocVersion::ASCEND910_93` 枚举值，但运行时字符串映射应以本地 `convertMap` 为准。

---

## 统一维护原则

- 芯片调用名、`NpuArch` / `SocVersion` / `__NPU_ARCH__` 映射、通用 dtype 视图和基础 dtype 大小在本 guide 中统一维护。
- 其他 skill 只保留场景特有的限制或约束，不再并行维护完整的芯片类型表、通用数据类型枚举和 dtype 大小表。
- 若某个 skill 需要新增统一调用名或基础 dtype 说明，应先更新本 guide，再回写相关脚本或测试。
- 脚本应读取同目录的 [`npu-arch-facts.json`](npu-arch-facts.json) 消费结构化事实；Markdown 表格用于人工说明，不作为脚本解析入口。

---

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

说明：

- 其他 skill 不应再维护并行的完整芯片调用名表和 dtype 大小表。
- 若某个 skill 只需要声明场景特有约束，可以只写限制条件，并回链到本节。
- 若出现新芯片调用名，应先在本 guide 中补映射，再同步更新相关脚本或测试。

---

## 完整映射表

| 产品系列 | SocVersion | __NPU_ARCH__ | 芯片型号 | 说明 |
|---------|-----------|:---:|---------|------|
| Atlas A2 训练/推理 | ASCEND910B | 2201 | Ascend910B1~B4, Ascend910B2C | 主流训练芯片 |
| Atlas A3 训练/推理 | ASCEND910B | 2201 | Ascend910_93 | 训练/推理芯片 |
| Atlas A5 训练 | ASCEND950 | 3510 | Ascend950DT | Decode |
| Atlas A5 推理 | ASCEND950 | 3510 | Ascend950PR | Prefill |
| Atlas 训练系列 | ASCEND910 | 1001 | Ascend910 | 初代训练架构 |
| Atlas 推理系列 | ASCEND310P | 2002 | Ascend310P1, Ascend310P3, Ascend610 | 推理芯片 |
| Atlas 200I/500 A2 推理 | ASCEND310B | 3002 | Ascend310B1~B4 | 推理芯片 |

---

## 统一数据类型视图

### 通用基础 dtype

本节依据仓内 [`docs/zh/api/context/内置数据类型.md`](../../../../docs/zh/api/SIMD-API/basic_api/data_structures/builtin_data_types.md) 维护 AscendC 内置 dtype 事实。脚本消费入口仍是 [`npu-arch-facts.json`](npu-arch-facts.json)，不要解析 Markdown 表格。

下表是各 skill 在未遇到特定 API 约束时，应优先复用的通用基础 dtype 视图。这里的“可直接初始化”只表示通用 UT 模板可以安全构造样例数据，不表示任意 API 都支持该类型。

| 类型组 | 典型类型名 | 说明 |
|-------|-----------|------|
| 可直接初始化 | `bool`, `int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `half`, `int32_t`, `uint32_t`, `float`, `int64_t`, `uint64_t` | 文档明确支持立即数赋值和初始化；通用 UT 生成器可直接使用 |
| 需 API 特化初始化 | `bfloat16_t`, `double`, `complex32`, `complex64` | 文档列为内置类型或产品支持类型，但通用模板不得仅凭 dtype 名直接初始化 |
| 低精度打包/8-bit 浮点 | `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `fp8_e8m0_t`, `int4x2_t` | 需要确认打包布局、转换 API 和目标 API 支持范围 |

### 基础 dtype 大小与 32B 对齐

下表是 UT 生成、样例 buffer 计算和内存对齐说明的统一 dtype 大小表。场景 guide 可以保留 API 特有的 dtype 支持矩阵，但不要复制本表。`fp4x2_*` 和 `int4x2_t` 均按 8-bit 存储单元计算，单个存储单元打包两个 4-bit 值。

| 数据类型 | sizeof | 32B 对齐元素数 | 说明 |
|---------|--------|---------------|------|
| `bool` | 1 字节 | 32 个元素 | 全 0 为 false，否则为 true |
| `int8_t` | 1 字节 | 32 个元素 | INT8 |
| `uint8_t` | 1 字节 | 32 个元素 | UINT8 |
| `fp4x2_e2m1_t` | 1 字节 | 32 个元素 | 两个 FP4 E2M1 值打包 |
| `fp4x2_e1m2_t` | 1 字节 | 32 个元素 | 两个 FP4 E1M2 值打包 |
| `hifloat8_t` | 1 字节 | 32 个元素 | HiFloat8 |
| `fp8_e5m2_t` | 1 字节 | 32 个元素 | FP8 E5M2 |
| `fp8_e4m3fn_t` | 1 字节 | 32 个元素 | FP8 E4M3FN |
| `fp8_e8m0_t` | 1 字节 | 32 个元素 | FP8 E8M0 |
| `int4x2_t` | 1 字节 | 32 个元素 | 两个 4-bit 整数打包，单 lane 范围 `[-8, 7]` |
| `half` | 2 字节 | 16 个元素 | FP16 |
| `bfloat16_t` | 2 字节 | 16 个元素 | BF16 |
| `int16_t` | 2 字节 | 16 个元素 | INT16 |
| `uint16_t` | 2 字节 | 16 个元素 | UINT16 |
| `float` | 4 字节 | 8 个元素 | FP32 |
| `int32_t` | 4 字节 | 8 个元素 | INT32 |
| `uint32_t` | 4 字节 | 8 个元素 | UINT32 |
| `complex32` | 4 字节 | 8 个元素 | `AscendC::Complex<half>` |
| `int64_t` | 8 字节 | 4 个元素 | INT64 |
| `uint64_t` | 8 字节 | 4 个元素 | UINT64 |
| `double` | 8 字节 | 4 个元素 | FP64 |
| `complex64` | 8 字节 | 4 个元素 | `AscendC::Complex<float>` |

32B 对齐的通用约束为 `count * sizeof(T) % 32 == 0`。

### 数据类型简写

文档给出的常用简写如下，生成 UT 时可以用于理解设计文档或表格，但代码里仍应使用目标 API 声明要求的真实 C++ 类型。

| 简写 | 类型 |
|-----|------|
| `S4` | `int4b_t`，通常通过 `int4x2_t` 打包存储 |
| `U8` / `S8` | `uint8_t` / `int8_t` |
| `U16` / `S16` | `uint16_t` / `int16_t` |
| `U32` / `S32` | `uint32_t` / `int32_t` |
| `U64` / `S64` | `uint64_t` / `int64_t` |
| `FP8_E4M3` | `fp8_e4m3fn_t` |
| `HiF8` | `hifloat8_t` |
| `FP16` / `BF16` / `FP32` | `half` / `bfloat16_t` / `float` |

### 产品支持范围

内置数据类型文档中的产品支持表给出以下 dtype 范围。本节只记录“产品到 dtype 集合”的事实矩阵，不展开低精度格式语义；具体格式说明见 [DAV_3510 扩展低精度 dtype](#dav_3510-扩展低精度-dtype)。未列出的芯片或产品系列不要按名称自行扩大 dtype 支持，必须回查目标 API 文档、声明、impl 和已有 UT。

| 产品范围 | 文档声明的支持 dtype |
|---------|----------------------|
| Ascend 950PR / Ascend 950DT | `bool`, `int8_t`, `uint8_t`, `fp4x2_e2m1_t`, `fp4x2_e1m2_t`, `hifloat8_t`, `fp8_e5m2_t`, `fp8_e4m3fn_t`, `fp8_e8m0_t`, `int4x2_t`, `int16_t`, `uint16_t`, `half`, `bfloat16_t`, `int32_t`, `uint32_t`, `float`, `complex32`, `int64_t`, `uint64_t`, `double`, `complex64` |
| Atlas A2 / A3 训练和推理系列 | `int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `int32_t`, `uint32_t`, `int64_t`, `uint64_t`, `half`, `bfloat16_t`, `float`, `double` |

复数章节还声明 `complex32` / `complex64` 当前仅昇腾 910_95 AI 处理器支持；如果与产品支持表或本地代码存在差异，生成 UT 前必须以目标 API 和本地实现为准。

### DAV_3510 扩展低精度 dtype

以下是 DAV_3510 低精度 dtype 的唯一详细说明位置；Ascend950 能力章节只回链到本节，不再逐项重复。只有设计文档和本地代码明确支持时，其他 skill 才能继续展开：

| 能力 | 说明 | 使用规则 |
|-----|------|---------|
| FP4 E2M1 / E1M2 | 两个 4-bit 浮点值打包到一个 8-bit 存储单元 | 必须确认打包顺序、转换 API 和 golden 生成方式 |
| FP8 E5M2 | 5 位指数、2 位尾数的 8-bit 浮点格式 | 仅在文档和本地代码显式声明时使用 |
| FP8 E4M3FN | 4 位指数、3 位尾数的 8-bit 浮点格式 | 仅在文档和本地代码显式声明时使用 |
| FP8 E8M0 | 8 位指数、0 位尾数的 8-bit 浮点格式 | 必须确认转换路径和特殊值语义 |
| HiFloat8 | 华为自定义 8-bit 浮点格式 | 先确认本地类型名和 API 行为 |
| INT4 | `int4x2_t` 打包两个 4-bit 整数；`S4` 简写对应 `int4b_t` | 先确认打包布局、声明和 UT 覆盖范围 |

---

## 条件编译边界

`__NPU_ARCH__` 是 Device 侧编译宏，适用于需要在编译期区分架构能力的代码。

使用要求：

- 不要把文档没有声明的芯片范围加入条件编译分支。
- 当已有代码对同类 API 有统一宏封装时，优先复用已有封装。
- Host 侧架构判断不在本 skill 中展开，按 `asc-devkit` 本地同类 API 模式处理。
- 如果文档要求的架构分支与本地代码模式冲突，先记录差异并向用户报告。

---

## Ascend950 DAV_3510 相关能力

| 能力 | 说明 | API 生成关注点 |
|-----|------|---------------|
| Regbase 编程 | 直接操作寄存器的编程能力 | 只有文档和现有接口明确支持时才生成相关分支 |
| SIMT 编程 | 线程级并行编程模型 | 关注 `include/simt_api`、`impl/simt_api`、UT 和目标芯片约束 |
| 低精度格式 | FP4、FP8、HiFloat8、INT4 等 dtype 能力 | 先按 [DAV_3510 扩展低精度 dtype](#dav_3510-扩展低精度-dtype) 确认格式，再检查类型定义、转换 API、精度边界和 UT 数据 |

注意事项：

- 能力存在不等于所有 API 自动支持，必须以设计文档和本地代码为准。
- 需要新增 dtype 支持时，应同步检查声明、实现、文档、UT 和示例。
- 新增 Ascend950 分支时，应明确说明是否影响 DAV_2201、DAV_3002 等已有架构。

---

## API 兼容性检查清单

涉及架构差异的 AscendC API 变更，应确认：

- [ ] 目标芯片、SocVersion 和 `__NPU_ARCH__` 映射一致。
- [ ] 文档声明的芯片范围与代码分支一致。
- [ ] Host 侧架构分支和 Device 侧条件编译符合本地已有模式。
- [ ] 新增 dtype 或特性支持有声明、实现、UT 和文档支撑。
- [ ] 失败返回值或不支持路径符合本地已有 API 风格。
- [ ] 未验证的芯片范围在最终报告中明确列出。
