# ceil\_div

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
- Atlas 推理系列产品 AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品 Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`"utils/std/cmath.h"`。

计算两个整数a和b相除后向上取整的结果。该接口可在Host侧和AI Core侧使用。

## 函数原型

```cpp
template <typename T, typename U>
__host__ __aicore__ inline constexpr auto ceil_div(const T& a, const U& b);
```

## 参数说明

**表 1** 模板参数说明

| 参数名 | 含义 |
| --- | --- |
| T | 参数a的数据类型，必须为整型。 |
| U | 参数b的数据类型，必须为整型。 |

**表 2** 接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| a | 输入 | 被除数。 |
| b | 输入 | 除数。 |

## 返回值说明

返回两个整型数相除的向上取整结果。一般情况下，返回值类型由`decltype(T{} / U{})`推导得到。

## 约束说明

- T和U必须为整型，支持的数据类型：int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t、int64_t、uint64_t。
- 当b为0时，结果为0。
- 对于有符号输入，调用方需确保a >= 0且b > 0。
<!-- npu="950" id8 -->
- 针对Ascend 950PR/Ascend 950DT，vector侧使用时，该接口入参仅支持`uint32_t`类型，在VF场景下返回值需使用`static_cast`转换为`uint16_t`类型并确保向上取整结果在`uint16_t`可表示范围内；该约束源于VF Hardware Loop编码规范，更多规范内容可参考[《Hardware Loop编码规范》](../../../../guide/operator_practice/simd_operator_optimization/vector_compute/vf_optimization/vf_loop_optimization.md#section11326136133217)。
<!-- end id8 -->

## 调用示例

本示例中使用ceil\_div计算迭代次数repeatTime，通过对数据量count与单次处理数据量进行向上取整除法，确保所有数据（包括尾块）均被完整处理。

```cpp
template <typename T>
__simd_vf__ inline void AddCustom(__local_mem__ T *dst, __local_mem__ T *src0, __local_mem__ T *src1,
    uint32_t count)
{
    AscendC::Reg::RegTensor<T> srcReg0;
    AscendC::Reg::RegTensor<T> srcReg1;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    constexpr uint32_t oneRepeatSize = AscendC::GetVecLen() / sizeof(T);
    uint16_t repeatTime = static_cast<uint16_t>(AscendC::Std::ceil_div(count, oneRepeatSize));
    for (uint16_t i = 0; i < repeatTime; ++i) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg0, src0 + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1 + i * oneRepeatSize);
        AscendC::Reg::Add(dstReg, srcReg0, srcReg1, mask);
        AscendC::Reg::StoreAlign(dst + i * oneRepeatSize, dstReg, mask);
    }
}
```
