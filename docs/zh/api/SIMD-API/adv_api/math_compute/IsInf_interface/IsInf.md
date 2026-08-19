# IsInf

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/IsInf_interface/IsInf_res.md#id1 -->

## 功能说明

按元素判断输入的浮点数是否为±INF，输出结果为浮点数或布尔值。当输出为浮点类型时，对于±INF的输入数据，对应位置的结果为浮点类型的1，反之为0；当输出为bool类型时，对于±INF的输入数据，对应位置的结果为true，反之为false。计算公式如下：

![](../../../../figures/zh-cn_formulaimage_0000002382902989.png)

-   当输出为浮点类型时：

    ![](../../../../figures/zh-cn_formulaimage_0000002386808777.png)

-   当输出为bool类型时：

    ![](../../../../figures/zh-cn_formulaimage_0000002386809017.png)

## 函数原型

-   通过sharedTmpBuffer入参传入临时空间

    ```
    template <const IsInfConfig& config = DEFAULT_IS_INF_CONFIG, typename T, typename U>
    __aicore__ inline void IsInf(const LocalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t count)
    ```

-   接口框架申请临时空间

    ```
    template <const IsInfConfig& config = DEFAULT_IS_INF_CONFIG, typename T, typename U>
    __aicore__ inline void IsInf(const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint32_t count)
    ```

由于该接口的内部实现中涉及精度转换。需要额外的临时空间来存储计算过程中的中间变量。临时空间支持开发者**通过sharedTmpBuffer入参传入**和**接口框架申请**两种方式。

-   通过sharedTmpBuffer入参传入，使用该tensor作为临时空间进行处理，接口框架不再申请。该方式开发者可以自行管理sharedTmpBuffer内存空间，并在接口调用完成后，复用该部分内存，内存不会反复申请释放，灵活性较高，内存利用率也较高。
-   接口框架申请临时空间，开发者无需申请，但是需要预留临时空间的大小。

通过sharedTmpBuffer传入的情况，开发者需要为tensor申请空间；接口框架申请的方式，开发者需要预留临时空间。临时空间大小BufferSize的获取方式如下：通过[GetIsInfMaxMinTmpSize](GetIsInfMaxMinTmpSize.md)中提供的接口获取需要预留空间的大小。

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| IsInfConfig | IsInf算法的相关配置。此参数可选配，IsInfConfig类型，具体定义如下方代码所示，其中参数的含义为：<br><br>isReuseSource：是否允许修改源操作数。该参数预留，传入默认值false即可。 |
| T | 目的操作数的数据类型。支持的数据类型为：bool、half、float。 |
| U | 源操作数的数据类型。支持的数据类型为：half、float。 |

```
struct IsInfConfig {
    bool isReuseSource;
};
```

**表2**  接口参数说明

| 参数名称 | 输入/输出 | 含义 |
| --- | --- | --- |
| dst | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>目的操作数的数据类型和源操作数相同或者为bool类型。当前支持的数据类型组合请见[表3](#tab3)。 |
| src | 输入 | 源操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| sharedTmpBuffer | 输入 | 临时缓存。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>用于IsInf内部复杂计算时存储中间变量，由开发者提供。<br><br>临时空间大小BufferSize的获取方式请参考[GetIsInfMaxMinTmpSize](GetIsInfMaxMinTmpSize.md)。 |
| count | 输入 | 参与计算的元素个数。 |

**表3**  输入输出支持的数据类型组合<a name="tab3"></a>

| srcDtype | dstDtype |
| --- | --- |
| half | half |
| half | bool |
| float | float |
| float | bool |

## 返回值说明

无

## 约束说明

-   **不支持源操作数与目的操作数地址重叠。**
-   操作数地址偏移对齐要求请参见[通用说明和约束](../../../general_description_and_constraints.md)。

## 调用示例

-   通过sharedTmpBuffer入参传入

    ```
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECCALC, 1> tmpQue;
    pipe.InitBuffer(tmpQue, 1, bufferSize); // bufferSize通过Host侧tiling参数获取
    AscendC::LocalTensor<uint8_t> sharedTmpBuffer = tmpQue.AllocTensor<uint8_t>();
    // 输入tensor长度为1024,算子输入的数据类型为half,实际计算个数为512
    static constexpr AscendC::IsInfConfig isInfConfig = {false}; // 不修改源操作数
    // dst为bool类型的LocalTensor，src为half类型的LocalTensor
    AscendC::IsInf<isInfConfig, bool, half>(dst, src, sharedTmpBuffer, 512);
    ```

-   接口框架申请临时空间

    ```
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECCALC, 1> tmpQue;
    pipe.InitBuffer(tmpQue, 1, bufferSize); // bufferSize通过Host侧tiling参数获取
    // 输入tensor长度为1024,算子输入的数据类型为half,实际计算个数为512
    static constexpr AscendC::IsInfConfig isInfConfig = {false}; // 不修改源操作数
    // dst为bool类型的LocalTensor，src为half类型的LocalTensor
    AscendC::IsInf<isInfConfig, bool, half>(dst, src, 512);
    ```

结果示例如下：

```
输入的数据类型为half，输出的数据类型为bool
输入数据(src):[1.0 inf 3.0 4.0 inf 6.0 -inf 8.0]
输出数据(dst):[false true false false true false true false]
```
