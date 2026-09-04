# Quantize

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
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/quantization/Quantize_res.md#id1 -->

## 功能说明

按元素做量化计算，将高精度数据转换为低精度数据。本接口的量化策略包括PER\_TENSOR，PER\_CHANNEL，PER\_TOKEN和PER\_GROUP四种，每种量化策略均支持配置舍入模式。**本接口最多支持输入为二维数据，不支持更高维度的输入。**

Quantize与[AscendQuant](AscendQuant.md)的功能类似，Quantize在PER\_TENSOR、PER\_CHANNEL量化场景，扩展了配置舍入模式的功能，因此推荐使用本接口。

-   PER\_TENSOR量化：整个srcTensor对应一个量化参数，scale和offset的shape为\[1\]。

    ![](../../../figures/zh-cn_formulaimage_0000002292405593.png)

-   PER\_CHANNEL量化：srcTensor的shape为\[m, n\]，每个channel维度对应一个量化参数，scale和offset的shape为\[1, n\]。

    ![](../../../figures/zh-cn_formulaimage_0000002257766684.png)

-   PER\_TOKEN量化：srcTensor的每组token（token为n方向，共有m组token）中的元素共享一个量化参数，srcTensor的shape为\[m, n\]时，scale和offset的shape为\[m, 1\]。

    ![](../../../figures/zh-cn_formulaimage_0000002257767428.png)

-   PER\_GROUP量化：定义group的计算方向为k方向，srcTensor在k方向上每groupSize个元素共享一组scale和offset。srcTensor的shape为\[m, n\]时，如果kDim=0，表示k是m方向，scale和offset的shape为\[\(m + groupSize - 1\) / groupSize, n\]；如果kDim=1，表示k是n方向，scale和offset的shape为\[m，\(n + groupSize - 1\) / groupSize\]。

    根据输出数据类型的不同，PER\_GROUP量化分为两种场景：fp4x2\_e2m1\_t/fp4x2\_e1m2\_t场景（后续内容中简称为float4场景）和int8\_t/hifloat8\_t/fp8\_e5m2\_t/fp8\_e4m3fn\_t场景（后续内容中简称为b8场景）。

    -   fp4x2\_e2m1\_t/fp4x2\_e1m2\_t场景（float4场景）
        -   kDim = 0:

            ![](../../../figures/zh-cn_formulaimage_0000002257871838.png)

        -   kDim = 1:

            ![](../../../figures/zh-cn_formulaimage_0000002292420797.png)

    -   int8\_t/hifloat8\_t/fp8\_e5m2\_t/fp8\_e4m3fn\_t场景（b8场景）
        -   kDim = 0：

            ![](../../../figures/zh-cn_formulaimage_0000002292412281.png)

        -   kDim = 1:

            ![](../../../figures/zh-cn_formulaimage_0000002292420789.png)

## 函数原型

-   通过sharedTmpBuffer入参传入临时空间

    ```
    template <const QuantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
    __aicore__ inline void Quantize(const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const ScaleT& scale, const OffsetT& offset, const QuantizeParams& params)
    ```

-   接口框架申请临时空间

    ```
    template <const QuantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
    __aicore__ inline void Quantize(const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor,const ScaleT& scale, const OffsetT& offset, const QuantizeParams& params)
    ```

由于该接口的内部实现中涉及复杂的数学计算，需要额外的临时空间来存储计算过程中的中间变量。临时空间支持**接口框架申请**和开发者**通过sharedTmpBuffer入参传入**两种方式。

-   接口框架申请临时空间，开发者无需申请，但是需要预留临时空间的大小。

-   通过sharedTmpBuffer入参传入，使用该tensor作为临时空间进行处理，接口框架不再申请。该方式开发者可以自行管理sharedTmpBuffer内存空间，并在接口调用完成后，复用该部分内存，内存不会反复申请释放，灵活性较高，内存利用率也较高。

接口框架申请的方式，开发者需要预留临时空间；通过sharedTmpBuffer传入的情况，开发者需要为sharedTmpBuffer申请空间。临时空间大小BufferSize的获取方式如下：通过[GetQuantizeMaxMinTmpSize](GetQuantizeMaxMinTmpSize.md)中提供的接口获取需要预留空间的范围大小。

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| config | 用于配置量化计算相关信息，QuantizeConfig类型，具体定义如下方代码所示，其中参数的含义如下。<br>policy：用于配置量化策略，枚举类型，具体定义如下方代码所示。<br>hasOffset：用于配置offset是否参与计算。true：表示offset参与计算。false：表示offset不参与计算。<br>roundMode：量化过程中，数据由高精度数据类型转换为低精度数据类型的舍入模式，支持的取值有：CAST_RINT、CAST_ROUND、CAST_FLOOR、CAST_CEIL、CAST_TRUNC、CAST_HYBRID，各个舍入模式的详细介绍请参考[精度转换规则](../../../SIMD-API/basic_api/data_structures/precision_conversion.md#tab1)。不同数据类型的量化支持不同的舍入模式，当量化过程中使用了不支持的舍入模式时，将回退到默认的舍入模式；例如，bfloat16_t数据类型量化为hifloat8_t数据类型时，如果配置的roundMode为不支持的CAST_RINT，实际执行量化时将回退到默认的roundMode（CAST_ROUND）。不同数据类型支持的舍入模式请见下方表格。<br>kDim：group的计算方向，即k方向。仅在PER_GROUP场景有效，支持的取值如下：0：k轴是第0轴，即m方向为group的计算方向。1：k轴是第1轴，即n方向为group的计算方向。 |
| DstT | 目的操作数的数据类型。接口内根据入参dstTensor自动推导数据类型，开发者无需配置该参数，保证dstTensor符合输入输出支持的数据类型组合即可。 |
| SrcT | 源操作数的数据类型。接口内根据入参srcTensor自动推导数据类型，开发者无需配置该参数，保证srcTensor符合输入输出支持的数据类型组合即可。 |
| ScaleT | 缩放因子scale的数据类型。接口内根据入参scale自动推导数据类型，开发者无需配置该参数。ScaleT可以为标量数据类型或LocalTensor类型。<br><br>注意：<br>对于PER_TENSOR量化策略，scale为标量，ScaleT只能为标量数据类型。<br>对于PER_CHANNEL、PER_TOKEN、PER_GROUP量化策略，scale为矢量，ScaleT只能为LocalTensor类型。 |
| OffsetT | offset的数据类型。接口内根据入参offset自动推导数据类型，开发者无需配置该参数。OffsetT可以为标量数据类型或LocalTensor类型。<br><br>注意：<br>对于PER_TENSOR量化策略，offset为标量，OffsetT只能为标量数据类型。<br>对于PER_CHANNEL、PER_TOKEN、PER_GROUP量化策略，offset可以是标量或者矢量，OffsetT可以为标量数据类型，也可以为LocalTensor类型。 |

```
struct QuantizeConfig {
    QuantizePolicy policy;
    bool hasOffset;
    RoundMode roundMode = RoundMode::CAST_RINT;
    int32_t kDim = 1;
};
enum class QuantizePolicy : int32_t { PER_TENSOR, PER_CHANNEL, PER_TOKEN, PER_GROUP };
```

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstTensor | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| srcTensor | 输入 | 源操作数。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| scale | 输入 | 输入数据量化时的缩放因子。 |
| offset | 输入 | 输入数据量化时的偏移量。对于PER_GROUP量化的float4场景，offset不生效。 |
| sharedTmpBuffer | 输入 | 临时缓存。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>临时空间大小BufferSize的获取方式请参考[GetQuantizeMaxMinTmpSize](GetQuantizeMaxMinTmpSize.md)。 |
| params | 输入 | 量化接口的参数，QuantizeParams类型，具体定义如下方代码所示，其中参数的含义如下。<br>m：m方向元素个数。<br>n：n方向元素个数。n值对应的数据大小需满足32字节对齐的要求，即shape最后一维为n的输入或输出均需要满足该维度上32字节对齐的要求。<br>groupSize：PER_GROUP场景有效，表示groupSize行/列数据共用一个scale/offset。groupSize的取值必须大于0且是32的整倍数。 |

```
struct QuantizeParams {
    uint32_t m;
    uint32_t n;
    uint32_t groupSize = 0;
};
```

输入输出支持的数据类型组合如下：

**表3**  DstT为fp8_e5m2_t/fp8_e4m3fn_t支持的数据类型组合

| SrcT | ScaleT/OffsetT |  roundMode |
| --- | --- | --- |
| half | half | CAST_RINT(默认) |
| bfloat16_t | bfloat16_t | CAST_RINT(默认) |
| float | float | CAST_RINT(默认) |
| half | float | CAST_RINT(默认) |
| bfloat16_t | float | CAST_RINT(默认) |

**表4**  DstT为hifloat8_t支持的数据类型组合

| SrcT | ScaleT/OffsetT |  roundMode |
| --- | --- | --- |
| half | half | CAST_ROUND(默认)<br>CAST_HYBRID |
| bfloat16_t | bfloat16_t | CAST_ROUND(默认)<br>CAST_HYBRID |
| float | float | CAST_ROUND(默认)<br>CAST_HYBRID |
| half | float | CAST_ROUND(默认)<br>CAST_HYBRID |
| bfloat16_t | float | CAST_ROUND(默认)<br>CAST_HYBRID |

**表5**  DstT为int8_t支持的数据类型组合

| SrcT | ScaleT/OffsetT |  roundMode |
| --- | --- | --- |
| half | half | CAST_RINT(默认)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |
| bfloat16_t | bfloat16_t | CAST_RINT(默认)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |
| float | float | CAST_RINT(默认)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |
| half | float | CAST_RINT(默认)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |
| bfloat16_t | float | CAST_RINT(默认)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |

**表6**  DstT为fp4x2_e1m2_t/fp4x2_e2m1_t支持的数据类型组合（当前均只支持PER_GROUP场景）

| SrcT | ScaleT/OffsetT |  roundMode |
| --- | --- | --- |
| half | half | CAST_RINT(默认)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |
| bfloat16_t | bfloat16_t | CAST_RINT(默认)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |
| float | float | CAST_RINT(默认)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |
| half | float | CAST_RINT(默认)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |
| bfloat16_t | float | CAST_RINT(默认)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |

## 返回值说明

无

## 约束说明

-   **不支持源操作数与目的操作数地址重叠。**
-   操作数地址对齐要求请参见[通用地址对齐约束](../../general_description_and_constraints.md#section796754519912)。
-   输入输出操作数参与计算的数据长度要求32字节对齐。
-   连续计算方向（即n方向）的数据量要求32字节对齐。
-   PER\_GROUP量化的float4场景不支持offset，该场景下模板参数config中的hasOffset参数必须配置为false。

## 调用示例

-   PER\_CHANNEL、PER\_TOKEN、PER\_GROUP模式

    ```
    constexpr static QuantizePolicy tokenPolicy = QuantizePolicy::PER_TOKEN;
    constexpr static QuantizePolicy channelPolicy = QuantizePolicy::PER_CHANNEL;
    constexpr static QuantizePolicy groupPolicy = QuantizePolicy::PER_GROUP;
    // 此处以PER_TOKEN模式为例，启用offset，舍入模式为CAST_ROUND；kDim仅PER_GROUP场景有效，表示group计算方向为n方向
    constexpr static QuantizeConfig config = {tokenPolicy, true, RoundMode::CAST_ROUND, 1};
    QuantizeParams params;
    // m,n为外部传入参数，表示srcLocal实际参与的m、n方向的元素个数
    params.m = m;
    params.n = n;
    params.groupSize = n; // 仅PER_GROUP场景有效，此处表示n方向所有元素共享一组scale和offset
    // dstLocal为int8_t类型的LocalTensor，srcLocal、scale、offset为half类型的LocalTensor
    Quantize<config>(dstLocal, srcLocal, scale, offset, params);
    ```

-   PER\_TENSOR模式

    ```
    constexpr static QuantizePolicy tensorPolicy = QuantizePolicy::PER_TENSOR;
    // 启用offset，舍入模式为CAST_ROUND
    constexpr static QuantizeConfig config = {tensorPolicy, true, RoundMode::CAST_ROUND, -1};
    QuantizeParams params;
    // m,n为外部传入参数，表示srcLocal实际参与的m、n方向的元素个数
    params.m = m;
    params.n = n;
    params.groupSize = 0; // 仅PER_GROUP场景有效
    // dstLocal为int8_t类型的LocalTensor，srcLocal为half类型的LocalTensor，scale、offset为half类型的标量
    Quantize<config>(dstLocal, srcLocal, scale, offset, params);
    ```

结果示例如下：

```
输入数据（srcLocal）:
[-4.4, 2.5, -2.9, -3.1, -1.5, -4.8, 1.8, 3.5, 4.5, 1.1, -2.7, 0.5, ... 1.6]
输入数据（scale矢量）:
[1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, ... 1]
输入数据（scale标量）:
[1]
输入数据（offset矢量）:
[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ... 0]
输入数据（offset标量）:
[0]
输出数据（dstLocal）:
[-4, 3, -3, -3, -1, -5, 2, 4, 5, 1, -3, 1, ... 2]
```
