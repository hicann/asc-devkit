# AntiQuantize

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/quantization/AntiQuantize_res.md#id1 -->

## 功能说明

按元素做伪量化计算，比如将int8\_t数据类型伪量化为half数据类型。**本接口最多支持输入为二维数据，不支持更高维度的输入。**

AntiQuantize与[AscendAntiQuant](AscendAntiQuant.md)的功能类似，本接口在不同量化场景下的形式更统一，因此推荐使用本接口。

本接口的伪量化策略包括PER\_TENSOR，PER\_CHANNEL，PER\_TOKEN，PER\_GROUP四种，伪量化系数scale、offset在PER\_TENSOR场景下为标量，其余场景下为矢量，计算公式如下：

-   PER\_TENSOR场景（按张量量化）：scale和offset的shape为\[1\]。

    ![](../../../figures/zh-cn_formulaimage_0000002268883076.png)

-   PER\_CHANNEL场景（按通道量化）：srcTensor的shape为\[m, n\]，每个channel维度对应一个量化参数，scale和offset的shape为\[1, n\]。

    ![](../../../figures/zh-cn_formulaimage_0000002295275093.png)

-   PER\_TOKEN场景（按token量化）：srcTensor的每组token（token为n方向，共有m组token）中的元素共享一个量化参数，srcTensor的shape为\[m, n\]时，scale和offset的shape为\[m, 1\]。

    ![](../../../figures/zh-cn_formulaimage_0000002295277697.png)

-   PER\_GROUP场景（按组量化）：定义group的计算方向为k方向，srcTensor在k方向上每groupSize个元素共享一组scale和offset。srcTensor的shape为\[m, n\]时，如果kDim=0，表示k是m方向，scale和offset的shape为\[\(m + groupSize - 1\) / groupSize, n\]；如果kDim=1，表示k是n方向，scale和offset的shape为\[m，\(n + groupSize - 1\) / groupSize\]。

    根据输入数据类型的不同，PER\_GROUP分为两种场景：fp4x2\_e2m1\_t/fp4x2\_e1m2\_t场景（后续内容中简称为float4场景）和int8\_t/hifloat8\_t/fp8\_e5m2\_t/fp8\_e4m3fn\_t场景（后续内容中简称为b8场景）。

    -   fp4x2\_e2m1\_t/fp4x2\_e1m2\_t场景（float4场景）
        -   k为m方向，即公式中i轴为group的计算方向（kDim=0）：

            ![](../../../figures/zh-cn_formulaimage_0000002260652804.png)

        -   k为n方向，即公式中j轴为group的计算方向（kDim=1）：

            ![](../../../figures/zh-cn_formulaimage_0000002260755858.png)

    -   int8\_t/hifloat8\_t/fp8\_e5m2\_t/fp8\_e4m3fn\_t场景（b8场景）
        -   k为m方向，即公式中i轴为group的计算方向（kDim=0）：

            ![](../../../figures/zh-cn_formulaimage_0000002295285957.png)

        -   k为n方向，即公式中j轴为group的计算方向（kDim=1）：

            ![](../../../figures/zh-cn_formulaimage_0000002260755850.png)

## 函数原型

-   通过sharedTmpBuffer入参传入临时空间

    ```
    template <const AntiQuantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
    __aicore__ inline void AntiQuantize(const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset, const LocalTensor<uint8_t>& sharedTmpBuffer, const AntiQuantizeParams& params)
    ```

-   接口框架申请临时空间

    ```
    template <const AntiQuantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
    __aicore__ inline void AntiQuantize(const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset, const AntiQuantizeParams& params)
    ```

由于该接口的内部实现中涉及复杂的数学计算，需要额外的临时空间来存储计算过程中的中间变量。临时空间支持**接口框架申请**和开发者**通过sharedTmpBuffer入参传入**两种方式。

-   接口框架申请临时空间，开发者无需申请，但是需要预留临时空间的大小。

-   通过sharedTmpBuffer入参传入，使用该tensor作为临时空间进行处理，接口框架不再申请。该方式开发者可以自行管理sharedTmpBuffer内存空间，并在接口调用完成后，复用该部分内存，内存不会反复申请释放，灵活性较高，内存利用率也较高。

接口框架申请的方式，开发者需要预留临时空间；通过sharedTmpBuffer传入的情况，开发者需要为sharedTmpBuffer申请空间。临时空间大小BufferSize的获取方式如下：通过[GetAntiQuantizeMaxMinTmpSize](GetAntiQuantizeMaxMinTmpSize.md)中提供的接口获取需要预留空间的范围大小。

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| config | 用于配置伪量化相关信息，AntiQuantizeConfig类型，具体定义如下方代码所示，其中参数的含义如下。<br>policy：用于配置量化策略，枚举类型，具体定义如下方代码所示。<br>hasOffset：用于配置offset是否参与计算。true：表示offset参与计算。false：表示offset不参与计算。<br>kDim：group的计算方向，即k方向。仅在PER_GROUP场景有效，支持的取值如下。0：k轴是第0轴，即m方向为group的计算方向。1：k轴是第1轴，即n方向为group的计算方向。 |
| DstT | 目的操作数的数据类型。接口内根据入参dstTensor自动推导数据类型，开发者无需配置该参数，保证dstTensor满足表3和表4中输入输出支持的数据类型组合即可。 |
| SrcT | 源操作数的数据类型。接口内根据入参srcTensor自动推导数据类型，开发者无需配置该参数，保证srcTensor满足表3和表4中输入输出支持的数据类型组合即可。 |
| ScaleT | scale的数据类型。接口内根据入参scale自动推导数据类型，开发者无需配置该参数。ScaleT可以为标量数据类型或LocalTensor类型。<br><br>注意：<br>对于PER_TENSOR场景，scale为标量，ScaleT只能为标量数据类型。<br>对于PER_CHANNEL、PER_TOKEN、PER_GROUP场景，scale为矢量，ScaleT只能为LocalTensor类型。 |
| OffsetT | offset的数据类型。接口内根据入参offset自动推导数据类型，开发者无需配置该参数。OffsetT可以为标量数据类型或LocalTensor类型。<br><br>注意：<br>对于PER_TENSOR量化策略，offset为标量，OffsetT只能为标量数据类型。<br>对于PER_CHANNEL、PER_TOKEN、PER_GROUP量化策略，offset为矢量，OffsetT只能为LocalTensor类型。 |

```
struct AntiQuantizeConfig {
    AntiQuantizePolicy policy;
    bool hasOffset;
    int32_t kDim = 1;
};
enum class AntiQuantizePolicy : int32_t { PER_TENSOR, PER_CHANNEL, PER_TOKEN, PER_GROUP };
```

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstTensor | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| srcTensor | 输入 | 源操作数。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| scale | 输入 | 输入数据伪量化时的缩放因子。 |
| offset | 输入 | 输入数据伪量化时的偏移量。对于PER_GROUP量化的float4场景，offset不生效。 |
| sharedTmpBuffer | 输入 | 临时缓存。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>临时空间大小BufferSize的获取方式请参考[GetAntiQuantizeMaxMinTmpSize](GetAntiQuantizeMaxMinTmpSize.md)。 |
| params | 输入 | 量化接口的参数，AntiQuantizeParams类型，具体定义如下方代码所示，其中参数的含义如下。<br>m：m方向元素个数。<br>n：n方向元素个数。n值对应的数据大小需满足32字节对齐的要求，即shape最后一维为n的输入或输出均需要满足该维度上32字节对齐的要求。<br>groupSize：PER_GROUP场景有效，表示groupSize行/列数据共用一个scale/offset。groupSize的取值必须大于0且是32的整倍数。 |

```
struct AntiQuantizeParams {
    uint32_t m;
    uint32_t n;
    uint32_t groupSize = 0;
};
```

输入输出支持的数据类型组合如下：

**表3**  PER_TENSOR/PER_CHANNEL量化策略支持的数据类型组合

| SrcT | ScaleT/OffsetT |  DstT |
| --- | --- | --- |
| fp8_e4m3fn_t | half | half |
| fp8_e5m2_t | half | half |
| hifloat8_t | half | half |
| int8_t | half | half |
| fp8_e4m3fn_t | bfloat16_t | bfloat16_t |
| fp8_e5m2_t | bfloat16_t | bfloat16_t |
| hifloat8_t | bfloat16_t | bfloat16_t |
| int8_t | bfloat16_t | bfloat16_t |

**表4**  PER_TOKEN/PER_GROUP量化策略支持的数据类型组合

| SrcT | ScaleT/OffsetT |  DstT |
| --- | --- | --- |
| int8_t | half | half |
| int8_t | bfloat16_t | bfloat16_t |
| int8_t | float | float |
| int8_t | float | half |
| int8_t | float | bfloat16_t |
| hifloat8_t | half | half |  |
| hifloat8_t | bfloat16_t | bfloat16_t |
| hifloat8_t | float | float |
| hifloat8_t | float | half |
| hifloat8_t | float | bfloat16_t |
| fp8_e5m2_t/fp8_e4m3fn_t | half | half |
| fp8_e5m2_t/fp8_e4m3fn_t| bfloat16_t | bfloat16_t |
| fp8_e5m2_t/fp8_e4m3fn_t| float | float |
| fp8_e5m2_t/fp8_e4m3fn_t| float | half |
| fp8_e5m2_t/fp8_e4m3fn_t| float | bfloat16_t |
| fp4x2_e1m2_t/fp4x2_e2m1_t<br>（当前均只支持PER_GROUP场景） | fp8_e8m0_t | half |
| fp4x2_e1m2_t/fp4x2_e2m1_t<br>（当前均只支持PER_GROUP场景） | fp8_e8m0_t | bfloat16_t |

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
    constexpr static AntiQuantizePolicy tokenPolicy = AntiQuantizePolicy::PER_TOKEN;
    constexpr static AntiQuantizePolicy channelPolicy = AntiQuantizePolicy::PER_CHANNEL;
    constexpr static AntiQuantizePolicy groupPolicy = AntiQuantizePolicy::PER_GROUP;
    // 此处以PER_TOKEN模式为例，启用offset；kDim仅PER_GROUP场景有效，表示group计算方向为n方向
    constexpr static AntiQuantizeConfig config = {tokenPolicy, true, 1};
    AntiQuantizeParams params;
    // m,n为外部传入参数，表示srcLocal实际参与的m、n方向的元素个数
    params.m = m;
    params.n = n;
    params.groupSize = n; // 仅PER_GROUP场景有效，此处表示n方向所有元素共享一组scale和offset
    // srcLocal为int8_t类型的LocalTensor，dstLocal、scale、offset为half类型的LocalTensor
    AntiQuantize<config>(dstLocal, srcLocal, scale, offset, params);
    ```

-   PER\_TENSOR模式

    ```
    constexpr static AntiQuantizePolicy tensorPolicy = AntiQuantizePolicy::PER_TENSOR;
    // 启用offset
    constexpr static AntiQuantizeConfig config = {tensorPolicy, true, -1};
    AntiQuantizeParams params;
    // m,n为外部传入参数，表示srcLocal实际参与的m、n方向的元素个数
    params.m = m;
    params.n = n;
    params.groupSize = 0; // 仅PER_GROUP场景有效
    // srcLocal为int8_t类型的LocalTensor，dstLocal为half类型的LocalTensor，scale、offset为half类型的标量
    AntiQuantize<config>(dstLocal, srcLocal, scale, offset, params);
    ```

结果示例如下：

```
输入数据（srcLocal）:
[-4, 2, -2, -3, -1, -4, 1, 3, 4, 1, -2, 0, ... 1]
输入数据（scale矢量）:
[1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, ... 1]
输入数据（scale标量）:
[1]
输入数据（offset矢量）:
[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ... 0]
输入数据（offset标量）:
[0]
输出数据（dstLocal），此时dstLocal = srcLocal:
[-4, 2, -2, -3, -1, -4, 1, 3, 4, 1, -2, 0, ... 1]
```
