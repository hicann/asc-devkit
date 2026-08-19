# Axpy

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Axpy_interface/Axpy_res.md#id1 -->

## 功能说明

源操作数\(srcTensor\)中每个元素与标量求积后和目的操作数\(dstTensor\)中的对应元素相加，计算公式如下：

![](../../../../figures/zh-cn_formulaimage_0000002188267514.png)

![](../../../../figures/zh-cn_formulaimage_0000002188272234.png)

该接口功能同基础API Axpy，区别在于此接口指令是通过Muls和Add组合计算，从而提供更优的精度。

## 函数原型

```
template <typename T, typename U, bool isReuseSource = false>
__aicore__ inline void Axpy(const LocalTensor<T>& dstTensor, const LocalTensor<U>& srcTensor, const U scalarValue, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 目的操作数的数据类型。支持的数据类型为：half、float。 |
| U | 源操作数的数据类型。支持的数据类型为：half、float。 |
| isReuseSource | 是否允许修改源操作数。该参数预留，传入默认值false即可。 |

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstTensor | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| srcTensor | 输入 | 源操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| scalarValue | 输入 | scalar标量。支持的数据类型为：half、float。scalar操作数的类型需要和srcTensor保持一致。 |
| sharedTmpBuffer | 输入 | 临时缓存。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>由于该接口的内部实现中涉及复杂的数学计算，需要额外的临时空间来存储计算过程中的中间变量。临时空间需要开发者通过sharedTmpBuffer入参传入。临时空间大小BufferSize的获取方式请参考[GetAxpyMaxMinTmpSize](GetAxpyMaxMinTmpSize.md)。 |
| calCount | 输入 | 参与计算的元素个数。 |

## 返回值说明

无

## 约束说明

-   **不支持源操作数与目的操作数地址重叠。**
-   不支持sharedTmpBuffer与源操作数和目的操作数地址重叠。
-   操作数地址对齐要求请参见[通用地址对齐约束](../../../general_description_and_constraints.md#section796754519912)。
-   该接口支持的精度组合如下：
    -   half精度组合：srcLocal数据类型=half；scalar数据类型=half；dstLocal数据类型=half；PAR=128
    -   float精度组合：srcLocal数据类型=float；scalar数据类型=float；dstLocal数据类型=float；PAR=64
    -   mix精度组合：srcLocal数据类型=half；scalar数据类型=half；dstLocal数据类型=float；PAR=64

## 调用示例

```
// dstLocal: 存放Axpy计算结果的Tensor
// srcLocal: 存放Axpy计算输入的Tensor
// sharedTmpBuffer: 存放Axpy计算过程中临时缓存的Tensor

// 算子输入的数据类型为half,需要参与计算的元素个数为512
AscendC::Axpy(dstLocal, srcLocal, static_cast<half>(3.0), sharedTmpBuffer, 512);
```

结果示例如下：

```
输入数据(srcLocal):
[1. 2. 3. 4. 5. 6. ... 512.]
输入数据(scalarValue): 3.0
输出数据(dstLocal)初始值:
[0. 0. 0. 0. 0. 0. ... 0.]
进行Axpy计算后，输出数据(dstLocal):
[3. 6. 9. 12. 15. 18. ... 1536.]
```
