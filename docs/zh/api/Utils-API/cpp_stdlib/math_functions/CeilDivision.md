# CeilDivision

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

## 功能说明

计算两个整数num1和num2相除后向上取整结果。

## 函数原型

```cpp
__aicore__ constexpr inline int32_t CeilDivision(int32_t num1, int32_t num2)
```

## 参数说明

**表1** 参数说明

| 参数名 | 描述 |
| --- | --- |
| num1 | 参数1，被除数。 |
| num2 | 参数2，除数。 |

## 返回值说明

两个整数相除的向上取整结果。

## 约束说明

- 当num2为0时，结果为0。
- 该接口仅支持在num1和num2为正数场景下使用。

## 调用示例

本示例中使用CeilDivision计算迭代次数repeatTimes，通过对数据量count与单次处理数据量进行向上取整除法，确保所有数据（包括尾块）均被完整处理。

```cpp
template <typename T>
__aicore__ inline void AddCustomImpl(__local_mem__ T *dst, __local_mem__ T *src0, __local_mem__ T *src1,
    uint32_t count)
{
    AscendC::Reg::RegTensor<T> srcReg0;
    AscendC::Reg::RegTensor<T> srcReg1;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    constexpr uint32_t oneRepeatSize = AscendC::GetVecLen() / sizeof(T);
    uint16_t repeatTime = AscendC::CeilDivision(count, oneRepeatSize);
    for (uint16_t i = 0; i < repeatTime; ++i) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg0, src0 + i * oneRepeatSize );
        AscendC::Reg::LoadAlign(srcReg1, src1 + i * oneRepeatSize );
        AscendC::Reg::Add(dstReg, srcReg0, srcReg1, mask);
        AscendC::Reg::StoreAlign(dst + i * oneRepeatSize, dstReg, mask);
    }
}
```
