# SetAtomicMax\(ISASI\)<a name="ZH-CN_TOPIC_0000001685721321"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id11 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id11 -->
<!-- npu="A3" id12 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id12 -->
<!-- npu="910b" id13 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id13 -->
<!-- npu="310b" id14 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id15 -->
<!-- npu="310p" id16 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id16 -->
<!-- npu="910" id17 -->
- Atlas 训练系列产品：不支持
<!-- end id17 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_set_atomic_intf.h"`。

设置后续搬运到GM的数据是否执行原子比较：将待拷贝的内容和GM已有内容进行比较，然后将最大值写入GM。SetAtomicMax接口可通过设置模板参数来设定不同的数据类型。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void SetAtomicMax() 
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
|--------|------|
| T | 设定不同的原子最大操作数据类型。 |

## 数据类型

支持的数据类型为int8_t、int16_t、half、bfloat16_t、int32_t、float。

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT，支持的数据通路为UB/L0C Buffer->GM。
<!-- end id1 -->

<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据通路为UB/L0C Buffer/L1 Buffer->GM。
<!-- end id2 -->

<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据通路为UB/L0C Buffer/L1 Buffer->GM。
<!-- end id3 -->

- 使用完后，建议通过[DisableDmaAtomic](DisableDmaAtomic.md)关闭原子最大操作，以免影响后续相关功能。
- 该接口执行前不会自动清零GM数据。开发者需根据算子逻辑判断是否清零，如需清零，请在执行前手动完成。
- SetAtomicMax内部已集成与[SetAtomicType](SetAtomicType.md)相同的功能。建议调用上述接口时，通过设置模板参数显式指定原子操作的数据类型，无需额外调用SetAtomicType接口。

## 调用示例<a name="section177231425115410"></a>

```cpp
#include "kernel_operator.h"

constexpr uint32_t SIZE = 256;
__aicore__ inline void CopyIn()
{
    AscendC::LocalTensor<T> srcLocal = queueSrc.AllocTensor<T>();
    // 清空原子操作的状态
    AscendC::DisableDmaAtomic();
    AscendC::DataCopy(srcLocal, srcGlobal, SIZE);
    queueSrc.EnQue(srcLocal);
    // 核间同步
    AscendC::SyncAll();
}

__aicore__ inline void CopyOut()
{
    AscendC::LocalTensor<T> srcLocal = queueSrc.DeQue<T>();
    // 开启原子MAX模式，3个核取最大值
    AscendC::SetAtomicMax<T>();
    AscendC::DataCopy(dstGlobal, srcLocal, SIZE);
    // 关闭原子操作
    AscendC::DisableDmaAtomic();
    queueSrc.FreeTensor(srcLocal);
}

/*
每个核的输入数据为: 
Src0: [1,1,1,1,1,...,1] // 256个1
Src1: [2,2,2,2,2,...,2] // 256个2
最终输出数据: [2,2,2,2,2,...,2] // 256个2
*/
```

完整样例请参考[DataMovementWithAtomicOperations样例](../../../../../../examples/01_simd_cpp_api/03_basic_api/06_atomic/data_movement_with_atomic_operations)。
