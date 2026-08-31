# Sqrt<a name="ZH-CN_TOPIC_0000001929668256"></a>

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/basic_arithmetic/Sqrt_res.md#id1 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径：`"basic_api/reg_compute/kernel_reg_compute_vec_unary_intf.h"`。

该接口根据mask，对源操作数srcReg逐元素做平方根运算，将结果写入目的操作数dstReg。计算公式如下：

$$
dstReg_i = (srcReg_i)^{1/2}
$$

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void Sqrt(U& dstReg, U& srcReg, MaskReg& mask)
```

## 参数说明<a name="section622mcpsimp"></a>

**表 1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |
| mode | 可配置为[MaskMergeMode](../aux_data_types/MaskMergeMode.md)枚举值或指向SqrtSpecificMode结构体的指针。<br>&bull; 配置MaskMergeMode：选择MERGING模式或ZEROING模式。<br>&nbsp;&nbsp;&bull; ZEROING模式下，mask未筛选的元素在dstReg中置零。<br>&nbsp;&nbsp;&bull; MERGING模式当前不支持。<br>&bull; 配置SqrtSpecificMode，定义如下：<br><code>enum class SqrtAlgo {<br>    INTRINSIC = 0,<br>    FAST_INVERSE,<br>    PRECISION_1ULP_FTZ_TRUE,<br>    PRECISION_0ULP_FTZ_FALSE,<br>    PRECISION_1ULP_FTZ_FALSE,<br>};<br>struct SqrtSpecificMode {<br>    MaskMergeMode mrgMode = MaskMergeMode::ZEROING;<br>    bool precisionMode = false;<br>    SqrtAlgo algo = SqrtAlgo::INTRINSIC;<br>};</code><br>&bull; mrgMode：选择MERGING模式或ZEROING模式。<br>&bull; precisionMode：用于配置精度模式。当precisionMode为true时，使能更高精度的Sqrt计算，使用快速求逆算法得出结果。该算法目前只针对float数据类型生效。<br>&bull; algo：用于选择Sqrt算法及配置Subnormal模式，详细说明请参考[关键特性说明](#关键特性说明)。<br>&nbsp;&nbsp;&bull; SqrtAlgo::INTRINSIC：默认算法，最大精度误差为1ulp。对于half、float类型，Subnormal处理受编译选项--cce-ftz控制（默认值为true）。<br>&nbsp;&nbsp;&bull; SqrtAlgo::PRECISION_1ULP_FTZ_TRUE：使用单指令计算，最大精度误差为1ulp。<br>&nbsp;&nbsp;&bull; SqrtAlgo::FAST_INVERSE和SqrtAlgo::PRECISION_0ULP_FTZ_FALSE：使用快速求逆算法得出结果。目前，该算法仅支持float类型，并在该模式下支持Subnormal数据计算。<br>&nbsp;&nbsp;&bull; SqrtAlgo::PRECISION_1ULP_FTZ_FALSE：最大精度误差为1ulp，支持half类型的Subnormal数据计算。 |
| U | 源操作数和目的操作数的RegTensor类型，例如RegTensor&lt;half&gt;，由编译器自动推导，用户不需要填写。 |

**表 2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstReg | 输出 | 目的操作数。<br>类型为[RegTensor](../register_data_types/RegTensor.md)。 |
| srcReg | 输入 | 源操作数。<br>类型为[RegTensor](../register_data_types/RegTensor.md)。 |
| mask | 输入 | 源操作数元素操作的有效指示，详细说明请参考[MaskReg](../register_data_types/MaskReg.md)。 |

## 数据类型

目的操作数与源操作数的数据类型需要保持一致。支持的数据类型为：half、float。

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 输入为负数时结果为nan。
- 输入为-0时结果为-0。

## 关键特性说明

### 最大精度误差

- precisionMode设置为true时，使用快速求逆算法进行高精度计算，最大精度误差为0ulp。
- precisionMode设置为false时：
  - 最大精度误差为1ulp：SqrtAlgo::INTRINSIC、SqrtAlgo::PRECISION_1ULP_FTZ_TRUE和SqrtAlgo::PRECISION_1ULP_FTZ_FALSE。
  - 最大精度误差为0ulp：SqrtAlgo::FAST_INVERSE和SqrtAlgo::PRECISION_0ULP_FTZ_FALSE。

### 配置Subnormal模式

FTZ（Flush-To-Zero）：一种浮点运算模式，当结果为Subnormal时，将其直接清零（近似为0），而非保留其精确的微小数值。

#### 默认算法

SqrtAlgo::INTRINSIC为默认算法，支持half和float类型。--cce-ftz=false时保留Subnormal；--cce-ftz=true（默认值）时采用FTZ模式。

#### 显式指定的算法

- SqrtAlgo::PRECISION_1ULP_FTZ_TRUE使用单指令计算，始终采用FTZ模式。
- SqrtAlgo::FAST_INVERSE和SqrtAlgo::PRECISION_0ULP_FTZ_FALSE使用快速求逆算法，仅支持float类型，并支持Subnormal数据计算。
- SqrtAlgo::PRECISION_1ULP_FTZ_FALSE支持half类型的Subnormal数据计算。

#### 使用建议

由于保留Subnormal的计算行为通过软件仿真实现，在--cce-ftz=true（默认值）时，一般场景建议使用默认的SqrtAlgo::INTRINSIC或显式选择SqrtAlgo::PRECISION_1ULP_FTZ_TRUE，以获得更好的性能；需要精确输出Subnormal时，使用SqrtAlgo::FAST_INVERSE、SqrtAlgo::PRECISION_0ULP_FTZ_FALSE或SqrtAlgo::PRECISION_1ULP_FTZ_FALSE。

**表 3**  Sqrt Subnormal示例

| 输出 | 输入 | algo |
| --- | --- | --- |
| 1.0471472598529991e-19（0x1FF7 402B） | 1.0965174496326337e-38（0x0077 6672） | SqrtAlgo::FAST_INVERSE、SqrtAlgo::PRECISION_0ULP_FTZ_FALSE |
| 1.0471473244764844e-19（0x1FF7 402C） | 1.0965174496326337e-38（0x0077 6672） | SqrtAlgo::PRECISION_1ULP_FTZ_FALSE |

## 调用示例<a name="section642mcpsimp"></a>

```cpp
template<typename T>
__simd_vf__ inline void SqrtVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    // 高精度模式
    // static constexpr AscendC::Reg::SqrtSpecificMode mode = {AscendC::Reg::MaskMergeMode::ZEROING, true};
    // 0ulp精度及Subnormal模式
    // static constexpr AscendC::Reg::SqrtSpecificMode mode = {
    //     AscendC::Reg::MaskMergeMode::ZEROING, true, AscendC::SqrtAlgo::PRECISION_0ULP_FTZ_FALSE};
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        AscendC::Reg::Sqrt(dstReg, srcReg, mask);
        // 高精度模式/Subnormal模式
        // AscendC::Reg::Sqrt<T, &mode>(dstReg, srcReg, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
