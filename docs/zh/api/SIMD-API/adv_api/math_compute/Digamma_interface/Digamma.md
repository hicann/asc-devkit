# Digamma

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Digamma_interface/Digamma_res.md#id1 -->

## 功能说明

按元素计算x的gamma函数的对数导数，计算公式如下，![](../../../../figures/zh-cn_formulaimage_0000001945339324.png)为伽玛函数。

![](../../../../figures/zh-cn_formulaimage_0000002182895154.png)

![](../../../../figures/zh-cn_formulaimage_0000002218495557.png)

## 函数原型

-   通过sharedTmpBuffer入参传入临时空间

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void Digamma(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
    ```

-   接口框架申请临时空间

    ```
    template<typename T, bool isReuseSource = false>
    __aicore__ inline void Digamma(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
    ```

由于该接口的内部实现中涉及复杂的数学计算，需要额外的临时空间来存储计算过程中的中间变量。临时空间支持开发者**通过sharedTmpBuffer入参传入**和**接口框架申请**两种方式。

-   通过sharedTmpBuffer入参传入，使用该tensor作为临时空间进行处理，接口框架不再申请。该方式开发者可以自行管理sharedTmpBuffer内存空间，并在接口调用完成后，复用该部分内存，内存不会反复申请释放，灵活性较高，内存利用率也较高。
-   接口框架申请临时空间，开发者无需申请，但是需要预留临时空间的大小。

通过sharedTmpBuffer传入的情况，开发者需要为tensor申请空间；接口框架申请的方式，开发者需要预留临时空间。临时空间大小BufferSize的获取方式如下：通过[GetDigammaMaxMinTmpSize](GetDigammaMaxMinTmpSize.md)中提供的接口获取需要预留空间范围的大小。

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数的数据类型。支持的数据类型为：half、float。 |
| isReuseSource | 是否允许修改源操作数，默认值为false。该参数仅在输入的数据类型为float时生效。<br><br>true：开发者允许源操作数被改写，可以设置该参数取值为true开启，开启后本接口内部计算时复用srcTensor的内存空间，节省部分内存空间；<br><br>false：本接口内部计算时不复用srcTensor的内存空间。<br><br>isReuseSource的使用样例请参考[更多样例](../more_examples.md#section639165323915)。 |

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstTensor | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| srcTensor | 输入 | 源操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>源操作数的数据类型需要与目的操作数保持一致。 |
| sharedTmpBuffer | 输入 | 临时缓存。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>用于Digamma内部复杂计算时存储中间变量，由开发者提供。<br><br>临时空间大小BufferSize的获取方式请参考[GetDigammaMaxMinTmpSize](GetDigammaMaxMinTmpSize.md)。 |
| calCount | 输入 | 参与计算的元素个数。 |

## 返回值说明

无

## 约束说明

-   **不支持源操作数与目的操作数地址重叠。**
-   不支持sharedTmpBuffer与源操作数和目的操作数地址重叠。
-   操作数地址对齐要求请参见[通用地址对齐约束](../../../general_description_and_constraints.md#section796754519912)。

<!-- npu="950" id7 -->
-   针对Ascend 950PR/Ascend 950DT，接口内部计算对Subnormal的处理方式受编译选项`--cce-ftz`控制（默认值为`true`）：
    -   配置为`false`时，计算过程中保留Subnormal，并按照其实际数值参与后续计算。
    -   配置为`true`时，启用FTZ（Flush-To-Zero）模式，计算过程中产生或参与运算的Subnormal将按0处理，可能导致计算结果与保留Subnormal时存在精度差异。
<!-- end id7 -->

## 调用示例

完整的调用样例请参考[更多样例](../more_examples.md)。

```
// dstLocal：输出Tensor
// srcLocal：输入Tensor
// sharedTmpBuffer：临时空间
// 输入tensor长度为1024,算子输入的数据类型为float,实际计算个数为1024
AscendC::Digamma<float, false>(dstLocal, srcLocal, sharedTmpBuffer, 1024);
```

结果示例如下：

```
输入数据(srcLocal): [5.3675685 0.26528683 -2.872628 ... 2.9387941 9.001339]
输出数据(dstLocal): [1.5843406 -3.978809 -6.2081366 ... 0.8983184 2.1407988]
```
