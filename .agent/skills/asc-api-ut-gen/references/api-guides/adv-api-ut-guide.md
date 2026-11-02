# 高阶 API (Advanced API) UT 生成指南

## 1. API 定位

高阶 API (Advanced API) 是 AscendC 在基础 API 之上封装的算子级接口。它通常把 shape、layout、tiling、workspace、策略枚举和多段基础 API 调用组合成一个可复用的高级算子能力。判断一个 API 是否属于高阶 API，优先看代码位置和实现边界，而不是 API 名称本身。

### 1.1 目录位置

| 类型 | 路径 | 说明 |
|------|------|------|
| 公开头文件 | `{ASC_DEVKIT_PATH}/include/adv_api/` | 高阶 API 的用户入口、tiling 入口、参数结构和策略枚举 |
| Kernel 实现 | `{ASC_DEVKIT_PATH}/impl/adv_api/detail/` | 高阶 API 在 AIC/AIV 上的实际实现，内部会调用 basic API |
| Tiling 实现 | `{ASC_DEVKIT_PATH}/impl/adv_api/tiling/` | host 侧 tiling、shape 推导、workspace 计算 |
| API 检查 | `{ASC_DEVKIT_PATH}/impl/adv_api/detail/api_check/` | 参数合法性、shape/layout/type 约束检查 |
| UT 目录 | `{ASC_DEVKIT_PATH}/tests/api/adv_api/` | 功能 UT、tiling UT、api_check UT |

### 1.2 和 basic API 的边界

| 维度 | 高阶 API | membase/basic API |
|------|----------|-------------------|
| 入口目录 | `include/adv_api/` | `include/basic_api/` |
| 实现目录 | `impl/adv_api/detail/`、`impl/adv_api/tiling/` | `impl/basic_api/dav_*`、`impl/basic_api/reg_compute/` |
| 抽象层级 | 算子级或子算子级封装，包含 shape、tiling、workspace、策略参数 | 单个基础指令或基础数据搬运/计算原语 |
| 典型调用 | `Softmax(...)`、`Quantize<config>(...)`、`TopK<...>(...)`、`Matmul` wrapper | `Add(...)`、`Mul(...)`、`DataCopy(...)`、`Mmad(...)` |
| UT 关注点 | API 合约、shape 边界、策略组合、workspace/tiling、结果语义 | overload、mask/repeat/stride、LocalTensor/GlobalTensor 数据通路 |

简单的 `Add`、`Sub`、`Mul`、`Div` 属于 membase/basic API，不应作为高阶 API 典型示例。`include/adv_api/math/` 下的数学函数属于高阶 API 时，通常会有对应的 tiling、参数结构、workspace 或算法分支；不能按“数学计算”这个名字直接归类。

### 1.3 典型 API 示例

| 类别 | 典型 API |
|------|----------|
| 激活和融合激活 | `Softmax`、`LogSoftMax`、`SoftmaxGrad`、`Gelu`、`Silu`、`Swish`、`Sigmoid`、`SwiGLU`、`GeGLU`、`ReGLU` |
| 归一化 | `LayerNorm`、`RmsNorm`、`BatchNorm`、`GroupNorm`、`DeepNorm`、`Normalize`、`WelfordFinalize` |
| Matmul / Conv | `Matmul`、`BatchMatmul`、`Conv3D`、`Conv3DBackpropInput`、`Conv3DBackpropFilter` |
| 量化/反量化 | `Quantize`、`AntiQuantize`、`Dequantize`、`AscendQuant`、`AscendDequant`、`AscendAntiQuant` |
| Reduce / Select / Sort | `ReduceSum`、`ReduceMean`、`TopK`、`Sort`、`SelectWithBytesMask` |
| Transpose / Pad / Filter / Index | `TransData`、`ConfusionTranspose`、`Pad`、`Broadcast`、`DropOut`、`ArithProgression` |
| 高阶 math | `Exp`、`Log`、`Sin`、`Cos`、`Tanh`、`Power`、`Clamp`、`Fma`、`CumSum`、`Where`，仅指 `include/adv_api/math/` 下的高阶封装 |
| 通信类 | `Hccl`、`Hcomm` 相关接口 |

---

## 2. 高阶 API 的实现特征

### 2.1 公开入口不只是一种形态

高阶 API 可能是函数模板、类封装、配置化入口或 tiling 入口。生成 UT 前先看公开头文件和 detail 实现，不要假设所有高阶 API 都是模板类。

常见形态：

```cpp
// 配置化函数模板
constexpr static QuantizeConfig config = {QuantizePolicy::PER_TENSOR, hasOffset};
Quantize<config>(dstLocal, srcLocal, sharedTmpBuffer, scale, offset, params);

// 策略模板
TopK<T, isInitIndex, false, isReuseSrc, topkMode, config>(
    dstValueLocal, dstIndexLocal, srcValueLocal, srcIndexLocal,
    finishLocal, tmp, k, tiling, topKInfo, isLargest);

// 对象生命周期封装
template <class A_TYPE, class B_TYPE, class C_TYPE,
          class BIAS_TYPE = half, class C_TYPE_2 = C_TYPE>
class MatmulImpl {
public:
    __aicore__ inline MatmulImpl() {}
    __aicore__ inline void Init(...) { }
    __aicore__ inline void Iterate(...) { }
    __aicore__ inline void GetTensorC(...) { }
    // ...
};
```

### 2.2 API 合约包含 shape、tiling 和 workspace

高阶 API 的正确性通常依赖这些信息：

- shape 参数：如 `m/n/k`、`height/width`、`axis`、`inner/outter`、batch 维度。
- layout/format：如 ND/NZ、NCHW/NDHWC、fractalz、transpose 标志。
- 策略枚举：如量化 policy、TopK mode/order/sorted、matmul tiling policy。
- workspace 或临时 buffer：如 `sharedTmpBuffer`、`TBuf<TPosition::...>`、`PopStackBuffer`。
- host tiling 数据：`*_tiling.h`、`*_tiling_intf.h`、`*_tilingdata.h` 中的 shape 推导和空间计算。

UT 要覆盖的是这些合约组合，而不是只验证某个基础 `DataCopy` 或 `Add` 是否能工作。

### 2.3 实现会复用 basic API

`impl/adv_api/detail/` 里经常调用 `DataCopy`、`LocalTensor`、`TQue`、`Mmad`、vector unary/binary 等 basic API。这是高阶 API 的实现方式，不代表 UT 要按 basic API 维度拆测。只有当分支行为由这些 basic API 的使用方式决定时，才把对应 shape、alignment、tail、dtype 或策略组合纳入高阶 API UT。

---

## 3. UT 设计要点

### 3.1 先定位测试类型

| 修改位置 | 需要的 UT |
|----------|-----------|
| `include/adv_api/<domain>/<api>.h` | 功能 UT，必要时补 api_check UT |
| `impl/adv_api/detail/<domain>/<api>/` | 功能 UT，覆盖 kernel 分支和结果语义 |
| `include/adv_api/<domain>/*_tiling*.h` 或 `impl/adv_api/tiling/<domain>/` | tiling UT，覆盖 shape、format、workspace 和错误返回 |
| `impl/adv_api/detail/api_check/kernel_check/` | api_check UT，覆盖合法/非法参数路径 |

### 3.2 功能 UT 模式

功能 UT 通常定义一个小型 wrapper kernel，把 GM 输入搬到 LocalTensor，调用目标高阶 API，再写回 GM：

```cpp
template <typename SrcT, typename DstT, typename ScaleT, bool hasOffset, bool hasWorkspace>
class KernelQuantizePerTensor {
public:
    __aicore__ inline void Init(GM_ADDR srcGm, GM_ADDR dstGm, GM_ADDR scaleGm,
                                GM_ADDR offsetGm, uint32_t m, uint32_t n) {
        srcGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ SrcT*>(srcGm), m * n);
        dstGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ DstT*>(dstGm), m * n);
        pipe.InitBuffer(inQueue, 1, m * n * sizeof(SrcT));
        pipe.InitBuffer(outQueue, 1, m * n * sizeof(DstT));
    }

    __aicore__ inline void Compute() {
        constexpr static QuantizeConfig config = {QuantizePolicy::PER_TENSOR, hasOffset};
        QuantizeParams params;
        params.m = m;
        params.n = n;
        Quantize<config>(dstLocal, srcLocal, scaleLocal.GetValue(0), offsetLocal.GetValue(0), params);
    }
};
```

实际 UT 需要按目标 API 调整：

- 多输入/多输出：如 `TopK` 同时验证 value、index、finish。
- workspace：有无 `sharedTmpBuffer` 或 `PopStackBuffer` 的分支要分别覆盖。
- 策略模板：policy、mode、order、sorted 等模板参数要来自 API 定义和已有同类 UT。
- shape 边界：覆盖 aligned、tail、broadcast、axis、batch、特殊 k/n/m 等分支。

### 3.3 Tiling UT 模式

当改动涉及 tiling 文件时，UT 重点不是执行 kernel，而是构造 shape/platform/format 参数，验证：

- tiling 返回值或错误码。
- blockDim、workspace、临时 buffer 大小。
- shape/layout 推导结果。
- 关键策略选择，如 Matmul schedule、TopK radix 参数、ReducePattern、QuantizePolicy。

### 3.4 API Check UT 模式

`api_check` UT 应覆盖参数合法性，不要只跑成功路径：

- tensor position、shape、dtype、format 不匹配。
- tmp buffer 位置或大小不满足要求。
- src/dst 复用、in-place、reuse 场景。
- 边界 shape、零维、非对齐、超过限制的参数。

### 3.5 架构和目录

高阶 API 有按架构拆分的实现和 UT，例如 `*_v200`、`*_v220`、`*_v300`、`*_c310`、`ascend950pr_9599`。生成或补充 UT 时：

- 先看目标实现文件里的 `__NPU_ARCH__`、文件名后缀和现有 UT 命名。
- 非 3510 高阶 API 通常走 `--adv_test` 相关 target。
- 3510 高阶 API 可能需要 `--adv_test_two` 或同时覆盖 AIC/AIV tiling target，具体 target 见 [自动化验证指南](../workflows/automation-guide.md)。

### 3.6 测试目录结构

```
tests/api/adv_api/
├── activation/
├── filter/
├── index/
├── math/
├── matmul/
├── normalization/
├── pad/
├── quantization/
├── reduce/
├── select/
├── sort/
├── transpose/
├── api_check/
└── utils/
```

新 UT 应放到与公开头文件和实现目录对应的领域目录下。`api_check` 与功能 UT 并行，不要混在普通功能用例里。

### 3.7 生成器输出的执行门槛

`ut_generator_cli.py --type adv` 不能退回按 `API<T1, T2>(dstLocal, srcLocal, {height, width})` 猜签名的泛化骨架。仓内不内置任何具体高阶 API profile；如果调用方需要复用一份已经验证过的可执行 UT，只能在本次 config 的 `kernel_params.adv_profile.source/output` 中显式传入。没有显式 profile 时生成器必须失败关闭。新增高阶 API 应先补 API 专属 wrapper/params/workspace/验证逻辑；不要把具体 API 路径写死到生成器代码或共享 reference 里，也不要生成看似成功但实际不可编译的文件。

`adv_profile` 是**精确源文件复制机制**，不是“同类 API 模板”：

- `source` 必须在生成时仍然存在，并且本身就已经包含目标 wrapper、params、workspace 和结果校验；
- `output` 只决定把这份已验证代码写到哪里，不会按 API 名、shape 或策略自动改写内容；
- 如果 `source == output`，而你先把这份唯一 UT 删除了，生成器无法凭空把它恢复出来；
- 仅仅找到“同领域”或“同类别”的另一份 UT 还不够，除非它已经就是你要复制的那份可执行代码，否则仍应写 API-specific UT。

生成后仍必须立即做编译或至少语法级验证，并检查以下条件：

- 必须生成 `cal_func` 对应的 `main_*` 或 `Entry` 包装函数，不能只在参数表里引用未定义函数。
- 目标 API 需要的 params/config/workspace 必须显式出现，例如 `QuantizeParams`、`QuantizePolicy`、`DropOutShapeInfo`、`TopKInfo`、tiling data 或 `sharedTmpBuffer`。
- 多输出 API 必须建模所有输出 GM/LocalTensor，例如 TopK 的 value/index 输出，不能退化成单 `dst`。
- 结果验证不能停留在 `TODO`；至少要校验目标 API 的核心语义或 api_check 的错误路径。

如果上述任一项缺失，应回到目标 API 的现有同类 UT 和 detail/tiling 实现重新设计，并让生成器失败关闭或补充可执行 profile，不要把泛化模板提交为高阶 API UT。

---

## 4. 分支覆盖要点

### 4.1 类型组合

高阶 API 的 dtype 组合必须从目标 API 声明、impl 分支、设计文档或已有同类 UT 确认。通用基础 dtype 和扩展低精度 dtype 的统一说明回链到 [`asc-npu-arch` 架构指南](../../../asc-npu-arch/references/npu-arch-guide.md#统一数据类型视图)，本 guide 不维护并行的常见 dtype 组合表。

不要把 basic API 的 dtype 支持直接套到高阶 API 上。例如量化类 API 常见 `half/float/bfloat16_t -> fp8/int8/hifloat8` 组合，和基础 vector `Add/Mul` 的 dtype 组合不是同一件事。

### 4.2 Shape、axis 和 tail

高阶 API 的覆盖率通常由 shape 分支决定：

- 对齐和非对齐：32B 对齐、block 对齐、tail 搬运。
- axis/broadcast：last axis、reduce axis、mask last axis、broadcast shape。
- batch/inner/outter：TopK、Reduce、Matmul、Norm 类常见。
- 特殊值：`k` padding、空/极小 shape、单 batch、非整除 shape。

### 4.3 策略和配置

对带 config 的 API，UT 参数化应覆盖真正影响分支的枚举或布尔项：

- `QuantizePolicy`、`has_offset`、`has_workspace`。
- `TopKMode`、`TopKOrder`、`sorted`、`isReuseSrc`、`isInitIndex`。
- Matmul `TilingPolicy`、`ScheduleType`、`IterateMode`、format/transpose。
- Reduce/Select/Pad/Transpose 的模式参数和 layout 参数。

### 4.4 Workspace 和临时 buffer

高阶 API 的临时空间通常是合约的一部分：

- `TBuf` / `PopStackBuffer` 分支需要确认空间大小和位置。
- workspace 大小来自 tiling 或 API 参数时，UT 应覆盖足够/不足两类路径。
- buffer 复用或 in-place 是 API 行为时，需要用结果校验或 api_check UT 固化。

这里不建议为了“优化 UT 性能”去启用 double buffer。double buffer 是算子在 NPU 上的通用流水优化方式，只有当目标高阶 API 的分支或配置明确包含 double buffer / n-buffer 策略时，才作为功能或策略分支覆盖。

---

## 5. 测试模板引用

通用 gtest、参数化测试和精度验证骨架见 [测试模板参考](../foundations/test-templates.md)。高阶 API guide 只维护高阶 API 特有约束：

- 按目标 API 的公开入口、参数结构和对象生命周期组织 `Init`、`Process`、`Compute` 和 `CopyOut`。
- 模板参数、policy、shape 和 workspace 组合必须来自目标 API 声明、impl 分支、tiling 逻辑、设计文档或已有同类 UT。
- 涉及 LocalTensor、TPipe、TQue 的基础内存申请规则回链 [LocalTensor 内存申请指南](../foundations/local-tensor-memory.md)。

---

## 6. 常见问题

### Q1: 高阶 API 改了 detail 实现，需要补 tiling UT 吗？

不一定。只改 kernel 计算分支时，优先补功能 UT 和结果校验；改到 `*_tiling*`、workspace 计算、shape 推导、format 推导或 platform 选择时，才需要补 tiling UT。改到 `api_check` 时补合法/非法参数 UT。

### Q2: 公共排障索引

API 类型判断、workspace 分支覆盖和 UT 执行时间统一查看 [常见问题与解决方案](../troubleshooting/faq.md)：

- [API 类型判断](../troubleshooting/faq.md#13-api-类型判断)
- [TmpBuffer / workspace 大小与分支覆盖](../troubleshooting/faq.md#6-tmpbuffer--workspace-大小与分支覆盖)
- [UT 执行时间过长](../troubleshooting/faq.md#12-ut-执行时间过长)

---

## 7. 检查清单

### 7.1 分析阶段

- [ ] 已确认 API 位于 `include/adv_api/`，不是 basic API。
- [ ] 已读取公开头文件、detail 实现和可能存在的 tiling/api_check 文件。
- [ ] 已确认需要的是功能 UT、tiling UT、api_check UT，或三者组合。
- [ ] 已从目标 API 或已有同类 UT 确认 dtype、shape、policy、workspace 组合。
- [ ] 已确认目标架构和对应测试 target。

### 7.2 编写阶段

- [ ] wrapper kernel 只封装目标高阶 API，不把 basic API 的独立行为当作验证目标。
- [ ] 参数化用例覆盖关键 shape、policy、workspace 和架构分支。
- [ ] 临时 buffer、workspace、TQue/TBuf 大小与 API 合约一致。
- [ ] 对多输出或 index 类 API 同时校验所有输出。
- [ ] api_check 用例包含失败路径和错误约束。

### 7.3 验证阶段

- [ ] 编译通过。
- [ ] 目标 gtest 通过。
- [ ] 结果校验覆盖核心语义，不只检查代码执行不崩。
- [ ] 需要时通过 cov_report 确认新增用例覆盖目标分支。

---

## 8. 常用枚举类

以下枚举只作为定位线索。生成 UT 时必须以目标 API 当前头文件和 impl 为准，不要把本节当作完整枚举取值表。

### 8.1 TPosition - 张量位置

**定义文件**: `include/adv_api/matmul/matmul_tiling_base.h`

```cpp
enum class TPosition : int32_t {
    GM,           // Global Memory
    A1, B1, C1,   // L1 缓冲区
    A2, B2, C2,   // L0 缓冲区
    CO1, CO2,     // Cube 输出缓冲区
    VECIN, VECOUT, VECCALC,  // Vector 核心缓冲区
    LCM, SPM, SHM, TSCM,     // 其他存储区域
    C2PIPE2GM, C2PIPE2LOCAL,
    MAX,
};
```

### 8.2 CubeFormat - Cube 格式

**定义文件**: `include/adv_api/matmul/matmul_tiling_base.h`

```cpp
enum class CubeFormat : int32_t {
    ND,   // ND 格式
    NZ,   // NZ 格式
    // ...
};
```

### 8.3 TilingPolicy - Tiling 策略

**定义文件**: `include/adv_api/matmul/matmul_tiling_base.h`

```cpp
enum class TilingPolicy : int32_t {
    // 具体值根据实现确定
};
```

### 8.4 DequantType - 反量化类型

**定义文件**: `include/adv_api/matmul/matmul_tiling_base.h`

```cpp
enum class DequantType : int32_t {
    // 具体值根据实现确定
};
```

### 8.5 ScheduleType - 调度类型

**定义文件**: `include/adv_api/matmul/matmul_tiling_base.h`

```cpp
enum class ScheduleType : int32_t {
    // 具体值根据实现确定
};
```

### 8.6 IterateMode - 迭代模式

**定义文件**: `include/adv_api/matmul/matmul_config.h`

```cpp
enum IterateMode : uint8_t {
    // 具体值根据实现确定
};
```

### 8.7 ReducePattern - 归约模式

**定义文件**: `include/adv_api/reduce/reduce_tiling.h`

```cpp
enum class ReducePattern : uint32_t {
    // 具体值根据实现确定
};
```

### 8.8 QuantizePolicy - 量化策略

**定义文件**: `include/adv_api/quantization/quantize_utils.h`

```cpp
enum class QuantizePolicy : int32_t {
    // 具体值根据实现确定
};
```

### 8.9 ConvFormat - 卷积格式

**定义文件**: `include/adv_api/conv/common/conv_common.h`

```cpp
enum class ConvFormat : uint32_t {
    // 具体值根据实现确定
};
```

---

## 9. 相关参考

| 文档 | 说明 |
|------|------|
| [API 目录映射表](../foundations/api-directory-map.md) | API 类型、源码目录和 UT 目录映射 |
| [分支覆盖分析指南](../foundations/branch-coverage-guide.md) | 详细分支分析方法 |
| [测试模板参考](../foundations/test-templates.md) | 通用测试骨架与模板选择索引 |
| [LocalTensor 内存申请指南](../foundations/local-tensor-memory.md) | 基础内存申请规则 |
| [自动化验证指南](../workflows/automation-guide.md) | 高阶 API build target 和执行方式 |
