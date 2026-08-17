# Log2<a name="ZH-CN_TOPIC_0000002008231220"></a>

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

## 功能说明<a name="section618mcpsimp"></a>

头文件路径：`"basic_api/reg_compute/kernel_reg_compute_vec_unary_intf.h"`。

该接口用于根据mask对输入数据srcReg进行求以2为底的对数操作，将结果写入dstReg。

计算公式如下：

$$
dstReg_i = \log_2(srcReg_i)
$$

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void Log2(U& dstReg, U& srcReg, MaskReg& mask)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |
| mode | 可配置为[MaskMergeMode](../aux_data_types/MaskMergeMode.md)枚举类型或Log2SpecificMode的结构体实例。<br>&bull; 配置MaskMergeMode枚举类型选择MERGING模式或ZEROING模式。<br>&nbsp;&nbsp;&bull; ZEROING模式下，mask未筛选的元素在dstReg中置零。<br>&nbsp;&nbsp;&bull; MERGING模式当前不支持。<br>&bull; Log2SpecificMode，定义如下：<br><code>enum class Log2Algo {<br>    INTRINSIC = 0,<br>    PRECISION_1ULP_FTZ_TRUE,<br>    PRECISION_1ULP_FTZ_FALSE,<br>};<br>struct Log2SpecificMode{<br>    MaskMergeMode mrgMode = MaskMergeMode::ZEROING,<br>    Log2Algo algo = Log2Algo::INTRINSIC;<br>};</code><br>&bull; mrgMode：选择MERGING模式或ZEROING模式。<br>&bull; algo：用于配置Subnormal模式。<br>&nbsp;&nbsp;&bull; Log2Algo::INTRINSIC、Log2Algo::PRECISION_1ULP_FTZ_TRUE，所有Subnormal被近似为0。<br>&nbsp;&nbsp;&bull; Log2Algo::PRECISION_1ULP_FTZ_FALSE，支持Subnormal数据计算。 |
| U | 目的操作数的RegTensor类型，例如RegTensor&lt;half&gt;，由编译器自动推导，用户不需要填写。 |

**表2**  参数说明

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

**精度提升**

当源操作数数据类型为half时，将half提升精度到float后进行计算，再将结果转为half后输出。

**最大精度误差**

- Log2Algo::INTRINSIC、Log2Algo::PRECISION_1ULP_FTZ_TRUE，最大精度误差为1ulp。
- Log2Algo::PRECISION_1ULP_FTZ_FALSE，最大精度误差为1ulp。

**配置Subnormal模式**：
<br>
FTZ（Flush To Zero）：一种浮点运算模式，当结果为Subnormal时，将其直接清零（近似为0），而非保留其精确的微小数值。
<br>
只有将algo设置为Log2Algo::PRECISION_1ULP_FTZ_FALSE时，Log2接口才会保留并正确输出Subnormal结果；其他模式下Subnormal均被FTZ。
<br>
一般场景推荐使用性能更好的Log2Algo::INTRINSIC、Log2Algo::PRECISION_1ULP_FTZ_TRUE；需要精确Subnormal输出的场景（如特定数据精度要求的算法、避免除零错误）使用Log2Algo::PRECISION_1ULP_FTZ_FALSE。

## 调用示例<a name="section642mcpsimp"></a>

```cpp
template<typename T>
__simd_vf__ inline void Log2VF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    // Subnormal模式
    // static constexpr AscendC::Reg::Log2SpecificMode mode = {MaskMergeMode::ZEROING, Log2Algo::PRECISION_1ULP_FTZ_FALSE};
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        AscendC::Reg::Log2(dstReg, srcReg, mask);
        // Subnormal模式
        // AscendC::Reg::Log2<T, &mode>(dstReg, srcReg, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
