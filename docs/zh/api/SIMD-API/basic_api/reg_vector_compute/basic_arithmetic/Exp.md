# Exp<a name="ZH-CN_TOPIC_0000001929810838"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/basic_arithmetic/Exp_res.md#id1 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径：`"basic_api/reg_compute/kernel_reg_compute_vec_unary_intf.h"`。

该接口根据mask，对源操作数srcReg进行按元素指数操作，将结果写入目的操作数dstReg。计算公式如下：

$$
dstReg_i = e^{srcReg_i}
$$

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void Exp(U& dstReg, U& srcReg, MaskReg& mask)
```

## 参数说明<a name="section622mcpsimp"></a>

**表 1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |
| mode | 可配置为[MaskMergeMode](../aux_data_types/MaskMergeMode.md)枚举值或指向ExpSpecificMode结构体的指针。<br>&bull; 配置MaskMergeMode：选择MERGING模式或ZEROING模式。<br>&nbsp;&nbsp;&bull; ZEROING模式下，mask未筛选的元素在dstReg中置零。<br>&nbsp;&nbsp;&bull; MERGING模式当前不支持。<br>&bull; 配置ExpSpecificMode，定义如下：<br><code>enum class ExpAlgo {<br>    INTRINSIC = 0,<br>    PRECISION_1ULP_FTZ_TRUE,<br>    PRECISION_1ULP_FTZ_FALSE,<br>};<br>struct ExpSpecificMode {<br>    MaskMergeMode mrgMode = MaskMergeMode::ZEROING;<br>    ExpAlgo algo = ExpAlgo::INTRINSIC;<br>};</code><br>&bull; mrgMode：选择MERGING模式或ZEROING模式。<br>&bull; algo：用于选择Exp算法及配置Subnormal模式，详细说明请参考[关键特性说明](#关键特性说明)。<br>&nbsp;&nbsp;&bull; ExpAlgo::INTRINSIC：默认算法，最大精度误差为1ulp。对于half、float类型，Subnormal处理受编译选项--cce-ftz控制（默认值为true）。<br>&nbsp;&nbsp;&bull; ExpAlgo::PRECISION_1ULP_FTZ_TRUE：使用单指令计算，最大精度误差为1ulp。<br>&nbsp;&nbsp;&bull; ExpAlgo::PRECISION_1ULP_FTZ_FALSE：通过软件仿真实现，支持Subnormal数据计算，最大精度误差为1ulp。 |
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

无

## 关键特性说明

### 最大精度误差

ExpAlgo::INTRINSIC、ExpAlgo::PRECISION_1ULP_FTZ_TRUE和ExpAlgo::PRECISION_1ULP_FTZ_FALSE的最大精度误差均为1ulp。

### 配置Subnormal模式

FTZ（Flush-To-Zero）：一种浮点运算模式，当结果为Subnormal时，将其直接清零（近似为0），而非保留其精确的微小数值。

#### 默认算法

ExpAlgo::INTRINSIC为默认算法。--cce-ftz=false时保留Subnormal；--cce-ftz=true（默认值）时采用FTZ模式。

#### 显式指定的算法

- ExpAlgo::PRECISION_1ULP_FTZ_TRUE使用单指令计算，始终采用FTZ模式。
- ExpAlgo::PRECISION_1ULP_FTZ_FALSE通过软件仿真实现，支持Subnormal数据计算。

#### 使用建议

由于保留Subnormal的计算行为通过软件仿真实现，在--cce-ftz=true（默认值）时，一般场景建议使用默认的ExpAlgo::INTRINSIC或显式选择ExpAlgo::PRECISION_1ULP_FTZ_TRUE，以获得更好的性能；需要精确输出Subnormal时，使用ExpAlgo::PRECISION_1ULP_FTZ_FALSE。

**表 3**  Exp Subnormal示例

| 输出 | 输入 | 输出二进制 | 输入二进制 | 数据类型 |
| --- | --- | --- | --- | --- |
| 6.0975551605e-05 | -9.703125 | 0 00000 1111111111 | 1 10010 0011011010 | half |
| 5.9604644775e-08 | -16.640625 | 0 00000 0000000001 | 1 10011 0000101001 | half |
| 1.1754942107e-38 | -87.3365478515625 | 0 00000000 11111111111111111111111 | 1 10000101 01011101010110001010000 | float |
| 1.401298464324817e-45 | -103.2789306640625 | 0 00000000 00000000000000000000001 | 1 10000101 10011101000111011010000 | float |

## 调用示例<a name="section642mcpsimp"></a>

```cpp
template<typename T>
__simd_vf__ inline void ExpVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    // 1ulp精度及Subnormal模式
    // static constexpr AscendC::Reg::ExpSpecificMode mode = {
    //     AscendC::Reg::MaskMergeMode::ZEROING, AscendC::ExpAlgo::PRECISION_1ULP_FTZ_FALSE};
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        AscendC::Reg::Exp(dstReg, srcReg, mask);
        // 1ulp精度及Subnormal模式
        // AscendC::Reg::Exp<T, &mode>(dstReg, srcReg, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
