# Div<a name="ZH-CN_TOPIC_0000001929668264"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/basic_arithmetic/Div_res.md#id1 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径：`"basic_api/reg_compute/kernel_reg_compute_vec_binary_intf.h"`。

该接口根据mask，对源操作数srcReg0、srcReg1进行按元素相除操作，将结果写入目的操作数dstReg。计算公式如下：

$$
dstReg_i = srcReg0_i / srcReg1_i
$$

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void Div(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
```

## 参数说明<a name="section622mcpsimp"></a>

**表 1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |
| mode | 可配置为[MaskMergeMode](../aux_data_types/MaskMergeMode.md)枚举值或指向DivSpecificMode结构体的指针。<br>&bull; 配置MaskMergeMode：选择MERGING模式或ZEROING模式。<br>&nbsp;&nbsp;&bull; ZEROING模式下，mask未筛选的元素在dstReg中置零。<br>&nbsp;&nbsp;&bull; MERGING模式当前不支持。<br>&bull; 配置DivSpecificMode，定义如下：<br><code>enum class DivAlgo {<br>    INTRINSIC = 0,<br>    DIFF_COMPENSATION,<br>    PRECISION_1ULP_FTZ_TRUE,<br>    PRECISION_0ULP_FTZ_TRUE,<br>    PRECISION_0ULP_FTZ_FALSE,<br>    PRECISION_1ULP_FTZ_FALSE<br>};<br>struct DivSpecificMode {<br>    MaskMergeMode mrgMode = MaskMergeMode::ZEROING;<br>    bool precisionMode = false;<br>    DivAlgo algo = DivAlgo::INTRINSIC;<br>};</code><br>&bull; precisionMode：用于配置高精度Div计算。设置为true时，使用差值补偿算法，最大精度误差为0ulp，支持float、complex64类型。<br>&bull; algo：用于选择Div算法及配置Subnormal模式，详细说明请参考[关键特性说明](#关键特性说明)。<br>&nbsp;&nbsp;&bull; DivAlgo::INTRINSIC：默认算法，最大精度误差为1ulp。对于half、float类型，Subnormal处理受编译选项--cce-ftz控制（默认值为true）。<br>&nbsp;&nbsp;&bull; DivAlgo::PRECISION_1ULP_FTZ_TRUE：使用单指令计算，最大精度误差为1ulp。<br>&nbsp;&nbsp;&bull; DivAlgo::DIFF_COMPENSATION：使用差值补偿算法，最大精度误差为0ulp。对于float、complex64类型，Subnormal处理受编译选项--cce-ftz控制（默认值为true）。<br>&nbsp;&nbsp;&bull; DivAlgo::PRECISION_0ULP_FTZ_TRUE：使用差值补偿算法，最大精度误差为0ulp。<br>&nbsp;&nbsp;&bull; DivAlgo::PRECISION_0ULP_FTZ_FALSE：使用差值补偿算法，支持Subnormal数据计算，最大精度误差为0ulp。<br>&nbsp;&nbsp;&bull; DivAlgo::PRECISION_1ULP_FTZ_FALSE：支持Subnormal数据计算，最大精度误差为1ulp。 |
| U | 源操作数和目的操作数的RegTensor类型，例如RegTensor&lt;half&gt;，由编译器自动推导，用户不需要填写。 |

**表 2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstReg | 输出 | 目的操作数。<br>类型为[RegTensor](../register_data_types/RegTensor.md)。 |
| srcReg0 | 输入 | 源操作数。<br>类型为[RegTensor](../register_data_types/RegTensor.md)。 |
| srcReg1 | 输入 | 源操作数。<br>类型为[RegTensor](../register_data_types/RegTensor.md)。 |
| mask | 输入 | 源操作数元素操作的有效指示，详细说明请参考[MaskReg](../register_data_types/MaskReg.md)。 |

## 数据类型

目的操作数与源操作数的数据类型需要保持一致。

- 当模式为DivAlgo::INTRINSIC或DivAlgo::PRECISION_1ULP_FTZ_TRUE时，支持的数据类型为：int16_t、uint16_t、half、int32_t、uint32_t、float、complex32、int64_t、uint64_t、complex64。
- 当模式为DivAlgo::DIFF_COMPENSATION或DivAlgo::PRECISION_0ULP_FTZ_TRUE时，支持的数据类型为：float、complex64。
- 当模式为DivAlgo::PRECISION_0ULP_FTZ_FALSE时，支持的数据类型为：float。
- 当模式为DivAlgo::PRECISION_1ULP_FTZ_FALSE时，支持的数据类型为：half、float。

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

无

## 关键特性说明

### 最大精度误差

- precisionMode设置为true时，使用差值补偿算法进行高精度计算，最大精度误差为0ulp。
- precisionMode设置为false时，DivAlgo::INTRINSIC、DivAlgo::PRECISION_1ULP_FTZ_TRUE和DivAlgo::PRECISION_1ULP_FTZ_FALSE的最大精度误差为1ulp；DivAlgo::DIFF_COMPENSATION、DivAlgo::PRECISION_0ULP_FTZ_TRUE和DivAlgo::PRECISION_0ULP_FTZ_FALSE的最大精度误差为0ulp。

### 配置Subnormal模式

FTZ（Flush-To-Zero）：一种浮点运算模式，当结果为Subnormal时，将其直接清零（近似为0），而非保留其精确的微小数值。

#### 默认算法

DivAlgo::INTRINSIC支持整数、浮点和复数类型，具体支持范围请参考[数据类型](#数据类型)。对于half和float类型，--cce-ftz=false时保留Subnormal，等效于DivAlgo::PRECISION_1ULP_FTZ_FALSE；--cce-ftz=true（默认值）时采用FTZ模式，等效于DivAlgo::PRECISION_1ULP_FTZ_TRUE。对于整数类型，计算结果不涉及Subnormal，--cce-ftz对其无影响；对于complex32和complex64类型，实部和虚部分别为half和float，Subnormal处理遵循对应元素类型的--cce-ftz配置。

#### 显式指定的算法

- DivAlgo::DIFF_COMPENSATION支持float和complex64类型。--cce-ftz=false时保留Subnormal；--cce-ftz=true（默认值）时采用FTZ模式。
- 名称中包含FTZ_FALSE的算法始终保留Subnormal。
- 名称中包含FTZ_TRUE的算法始终采用FTZ模式。

#### 使用建议

一般场景建议使用性能更优的DivAlgo::PRECISION_1ULP_FTZ_TRUE或DivAlgo::PRECISION_0ULP_FTZ_TRUE；对于需要精确输出Subnormal的场景（如算法对数据精度有特殊要求或需避免除零错误），可将算法配置为DivAlgo::PRECISION_0ULP_FTZ_FALSE或DivAlgo::PRECISION_1ULP_FTZ_FALSE。

**表 3**  Div Subnormal示例

| 被除数输入 | 除数输入 | 输出（配置Subnormal模式） | 输出（不配置Subnormal模式） |
| --- | --- | --- | --- |
| 1（0x3F80 0000） | -5.01923249665106e-39（0x8036 A794） | -1.9923364304124947e+38（0xFF15 E301） | -inf（0xFF80 0000） |
| 1.8611191411098982e-39（0x0014 440B） | 7.70406850625423e-39（0x0053 E3CF） | 0.24157613515853882（0x3E77 5FBC） | NaN（0x7FFF FFFF） |

## 调用示例<a name="section642mcpsimp"></a>

```cpp
template<typename T>
__simd_vf__ inline void DivVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg0;
    AscendC::Reg::RegTensor<T> srcReg1;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    // 高精度模式
    // static constexpr AscendC::Reg::DivSpecificMode mode = {AscendC::Reg::MaskMergeMode::ZEROING, true};
    // 0ulp精度及Subnormal模式
    // static constexpr AscendC::Reg::DivSpecificMode mode = {
    //     AscendC::Reg::MaskMergeMode::ZEROING, true, AscendC::DivAlgo::PRECISION_0ULP_FTZ_FALSE};
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
        AscendC::Reg::Div(dstReg, srcReg0, srcReg1, mask);
        // 高精度模式/Subnormal模式
        // AscendC::Reg::Div<T, &mode>(dstReg, srcReg0, srcReg1, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
