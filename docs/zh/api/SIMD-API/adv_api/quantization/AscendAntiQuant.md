# AscendAntiQuant

## 产品支持情况

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
- Atlas 推理系列产品AI Core：支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id15 -->
<!-- npu="910" id16 -->
- Atlas 训练系列产品：不支持
<!-- end id16 -->
<!-- npu="x90" id1 -->
- Kirin X90：支持
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin 9030：支持
<!-- end id2 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/quantization/AscendAntiQuant_res.md#id1 -->

## 功能说明

按元素做伪量化计算，比如将int8\_t数据类型伪量化为half数据类型，计算公式如下：

-   PER\_CHANNEL场景（按通道量化）
    -   不开启输入转置

        groupSize = src.shape\[0\] / offset.shape\[0\]

        **dst\[i\]\[j\] = scale\[i / groupSize\]\[j\] \* \(src\[i\]\[j\] + offset\[i / groupSize\]\[j\]\)**

    -   开启输入转置

        groupSize = src.shape\[1\] / offset.shape\[1\]

        **dst\[i\]\[j\] = scale\[i\]\[j / groupSize\] \* \(src\[i\]\[j\] + offset\[i\]\[j / groupSize\]\)**

-   PER\_TENSOR场景（按张量量化）

    **dst\[i\]\[j\] = scale \* \(src\[i\]\[j\] + offset\)**

-   PER\_TOKEN场景（按token量化）

    ![](../../../figures/zh-cn_formulaimage_0000002069677288.png)

-   PER\_GROUP场景（按组量化）

    根据输入数据类型的不同，当前PER\_GROUP分为两种场景：fp4x2\_e2m1\_t/fp4x2\_e1m2\_t场景（后续内容中简称为float4场景）和int8\_t/hifloat8\_t/fp8\_e5m2\_t/fp8\_e4m3fn\_t场景（后续内容中简称为b8场景）。

    -   fp4x2\_e2m1\_t/fp4x2\_e1m2\_t场景（float4场景）
        -   groupSize可配置接口

            定义group的计算方向为k方向，src在k方向上每groupSize个元素共享一组scale。src的shape为\[m, n\]时，如果kDim=0，表示k是m方向，scale的shape为\[\(m + groupSize - 1\) / groupSize, n\]；如果kDim=1，表示k是n方向，scale的shape为\[m，\(n + groupSize - 1\) / groupSize\]。isTranspose为True表示src，scale，dst都是转置的矩阵。

            -   k为m方向，即公式中i轴为group的计算方向：（kDim=0同时isTranspose=False）或者（kDim=1同时isTranspose=True）

                **dst\[i\]\[j\] = scale\[i / groupSize\]\[j\] \* src\[i\]\[j\]**

            -   k为n方向，即公式中j轴为group的计算方向：（kDim=0同时isTranspose=True）或者（kDim=1同时isTranspose=False）

                **dst\[i\]\[j\] = scale\[i\]\[j / groupSize\] \* src\[i\]\[j\]**

        -   groupSize固定为32

            isTranspose为True表示src，scale，dst都是转置的矩阵。

            -   不开启输入转置（isTranspose=False）

                **dst\[i\]\[j\] = scale\[i / groupSize\]\[j\] \* src\[i\]\[j\]**

            -   开启输入转置（isTranspose=True）

                **dst\[i\]\[j\] = scale\[i\]\[j / groupSize\] \* src\[i\]\[j\]**

    -   int8\_t/hifloat8\_t/fp8\_e5m2\_t/fp8\_e4m3fn\_t场景（b8场景）

        定义group的计算方向为k方向，src在k方向上每groupSize个元素共享一组scale和offset。src的shape为\[m, n\]时，如果kDim=0，表示k是m方向，scale和offset的shape为\[\(m + groupSize - 1\) / groupSize, n\]；如果kDim=1，表示k是n方向，scale和offset的shape为\[m，\(n + groupSize - 1\) / groupSize\]。offset是可选输入。isTranspose为True表示src，scale，dst都是转置的矩阵。

        -   k为m方向，即公式中i轴为group的计算方向：（kDim=0同时isTranspose=False）或者（kDim=1同时isTranspose= True）

            ![](../../../figures/zh-cn_formulaimage_0000002106594713.png)

        -   k为n方向，即公式中j轴为group的计算方向：（kDim=0同时isTranspose=True）或者（kDim=1同时isTranspose =False）

            ![](../../../figures/zh-cn_formulaimage_0000002106474749.png)

## 实现原理

**图1**  AscendAntiQuant算法框图  
![](../../../figures/ascendantiquant_diagram.png "AscendAntiQuant算法框图")

如上图所示，为AscendAntiQuant的典型场景算法框图，计算过程分为如下几步，均在Vector上进行：

1.  精度转换：将输入src转换为half类型；
2.  计算offset：当offset为向量时做Add计算，当offset为scalar时做Adds计算；
3.  计算scale：当scale为向量时做Mul计算，当scale为scalar时做Muls计算。

**图2**  isTranspose为False且输出为bfloat16的AscendAntiQuant算法框图  
![](../../../figures/ascendantiquant_no_transpose_bfloat16.png "isTranspose为False且输出为bfloat16的AscendAntiQuant算法框图")

在Atlas A2 训练系列产品/Atlas A2 推理系列产品上，当输出为bfloat16时，计算过程分为如下几步：

1.  src精度转换：将输入的src转换为half类型，再转换为float类型，存放到tmp1；
2.  offset精度转换：当输入的offset为向量时转换为float类型，存放到tmp2，为scalar时做Cast转换为float类型；
3.  计算offset：当输入的offset为向量时与tmp2做Add计算，为scalar时做Adds计算；
4.  scale精度转换：当输入的scale为向量时转换为float类型，存放到tmp2，为scalar时做Cast转换为float类型；
5.  计算scale：当输入的scale为向量时用tmp2做Mul计算，为scalar时做Muls计算；
6.  dst精度转换：将tmp1转换为bf16类型。

**图3**  AscendAntiQuant PER\_TOKEN/PER\_GROUP算法框图  
![](../../../figures/ascendantiquant_per_token_per_group.png "AscendAntiQuant-PER_TOKEN-PER_GROUP算法框图")

PER\_TOKEN/PER\_GROUP b8/float4场景的计算逻辑如下：

1.  读取数据：连续读取输入src；根据不同的场景，对输入scale和offset，采用不同的读取方式；例如，PER\_TOKEN场景做Broadcast处理，PER\_GROUP场景做Gather处理；
2.  精度转换：根据不同输入的数据类型组合，对src/scale/offset进行相应的数据类型转换；
3.  计算：对类型转换后的数据做加乘操作；
4.  精度转换：将上述加乘操作得到的计算结果转换成dstT类型，得到最终输出。

## 函数原型

-   通过sharedTmpBuffer入参传入临时空间
    -   PER\_CHANNEL场景（按通道量化）

        ```
        template <typename InputDataType, typename OutputDataType, bool isTranspose>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<OutputDataType>& dst, const LocalTensor<InputDataType>& src, const LocalTensor<OutputDataType>& offset, const LocalTensor<OutputDataType>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t k, const AntiQuantShapeInfo& shapeInfo = {})
        ```

    -   PER\_CHANNEL场景（按通道量化，不带offset）

        ```
        template <typename InputDataType, typename OutputDataType, bool isTranspose>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<OutputDataType>& dst, const LocalTensor<InputDataType>& src, const LocalTensor<OutputDataType>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t k, const AntiQuantShapeInfo& shapeInfo = {})
        ```

    -   PER\_TENSOR场景（按张量量化）

        ```
        template <typename InputDataType, typename OutputDataType, bool isTranspose>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<OutputDataType>& dst, const LocalTensor<InputDataType>& src, const OutputDataType offset, const OutputDataType scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t k, const AntiQuantShapeInfo& shapeInfo = {})
        ```

    -   PER\_TENSOR场景（按张量量化，不带offset）

        ```
        template <typename InputDataType, typename OutputDataType, bool isTranspose>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<OutputDataType>& dst, const LocalTensor<InputDataType>& src, const OutputDataType scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t k, const AntiQuantShapeInfo& shapeInfo = {})
        ```

    -   PER\_GROUP float4场景（按组量化）

        <!-- npu="950" id34 -->
        仅支持Ascend 950PR/Ascend 950DT
        <!-- end id34 -->

        ```
        template <typename InputDataType, typename OutputDataType, bool isTranspose>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<OutputDataType>& dst, const LocalTensor<InputDataType>& src, const LocalTensor<fp8_e8m0_t>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t k, const AntiQuantShapeInfo& shapeInfo = {})
        ```

    -   PER\_TOKEN/PER\_GROUP b8/float4场景（按token量化）/（按组量化）

        <!-- npu="950" id35 -->
        仅支持Ascend 950PR/Ascend 950DT
        <!-- end id35 -->

        ```
        template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config, const AscendAntiQuantPolicy& policy>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor, const LocalTensor<scaleT>& offsetTensor,const LocalTensor<uint8_t>& sharedTmpBuffer, const AscendAntiQuantParam& para)
        ```

-   接口框架申请临时空间
    -   PER\_CHANNEL场景

        ```
        template <typename InputDataType, typename OutputDataType, bool isTranspose>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<OutputDataType>& dst, const LocalTensor<InputDataType>& src, const LocalTensor<OutputDataType>& offset, const LocalTensor<OutputDataType>& scale, const uint32_t k, const AntiQuantShapeInfo& shapeInfo = {})
        ```

    -   PER\_TENSOR场景

        ```
        template <typename InputDataType, typename OutputDataType, bool isTranspose>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<OutputDataType>& dst, const LocalTensor<InputDataType>& src, const OutputDataType offset, const OutputDataType scale, const uint32_t k, const AntiQuantShapeInfo& shapeInfo = {})
        ```

    -   PER\_GROUP float4场景（groupSize固定为32）

        <!-- npu="950" id36 -->
        仅支持Ascend 950PR/Ascend 950DT
        <!-- end id36 -->

        ```
        template <typename InputDataType, typename OutputDataType, bool isTranspose>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<OutputDataType>& dst, const LocalTensor<InputDataType>& src, const LocalTensor<fp8_e8m0_t>& scale, const uint32_t k, const AntiQuantShapeInfo& shapeInfo = {})
        ```

    -   PER\_TOKEN/PER\_GROUP b8/float4场景（groupSize可配置）

        <!-- npu="950" id37 -->
        仅支持Ascend 950PR/Ascend 950DT
        <!-- end id37 -->

        ```
        template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config, const AscendAntiQuantPolicy& policy>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor, const LocalTensor<scaleT>& offsetTensor,const AscendAntiQuantParam& para)
        ```

由于该接口的内部实现中涉及复杂的数学计算，需要额外的临时空间来存储计算过程中的中间变量。临时空间支持**接口框架申请**和开发者**通过sharedTmpBuffer入参传入**两种方式。

-   接口框架申请临时空间，开发者无需申请，但是需要预留临时空间的大小。

-   通过sharedTmpBuffer入参传入，使用该tensor作为临时空间进行处理，接口框架不再申请。该方式开发者可以自行管理sharedTmpBuffer内存空间，并在接口调用完成后，复用该部分内存，内存不会反复申请释放，灵活性较高，内存利用率也较高。

接口框架申请的方式，开发者需要预留临时空间；通过sharedTmpBuffer传入的情况，开发者需要为sharedTmpBuffer申请空间。临时空间大小BufferSize的获取方式如下：通过[GetAscendAntiQuantMaxMinTmpSize](GetAscendAntiQuantMaxMinTmpSize.md)中提供的接口获取需要预留空间的范围大小。

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| InputDataType | 输入的数据类型。 |
| OutputDataType | 输出的数据类型。 |
| isTranspose | 是否开启输入数据转置。 |

**表2**  PER\_TOKEN/PER\_GROUP b8/float4场景模板参数说明

| 参数名 | 描述 |
| --- | --- |
| dstT | 目的操作数的数据类型。 |
| srcT | 源操作数的数据类型。 |
| scaleT | 缩放因子scale参数的数据类型。 |
| config | 量化接口配置参数，定义如下方代码所示，其中参数的含义如下。<br>hasOffset：量化参数offset是否参与计算。True：表示offset参数参与计算。False：表示offset参数不参与计算。<br>isTranspose：表示是否开启输入src转置。True：表示输入src转置。False：表示输入src不转置。<br>kDim：group的计算方向，即k方向。仅在PER_GROUP场景有效，支持的取值如下。0：k轴是第0轴，即m方向为group的计算方向；1：k轴是第1轴，即n方向为group的计算方向。 |
| policy | 量化策略配置参数，枚举类型，可取值如下：<br>PER_TENSOR：预留参数，暂不支持。<br>PER_CHANNEL：预留参数，暂不支持。<br>PER_TOKEN：配置为PER_TOKEN场景。<br>PER_GROUP：配置为PER_GROUP场景。<br>PER_CHANNEL_PER_GROUP：预留参数，暂不支持。<br>PER_TOKEN_PER_GROUP：预留参数，暂不支持。 |

```
struct AscendAntiQuantConfig {
    bool hasOffset;
    bool isTranspose;
    int32_t kDim = 1;
};
```

**表3**  PER\_TOKEN/PER\_GROUP b8/float4场景支持的数据类型组合

| srcDtype | scaleDtype/offsetDtype | dstDtype |
| --- | --- | --- |
| int8_t | half | half |
| int8_t | bfloat16_t | bfloat16_t |
| int8_t | float | float |
| int8_t | float | half |
| int8_t | float | bfloat16_t |
| hifloat8_t | half | half |
| hifloat8_t | bfloat16_t | bfloat16_t |
| hifloat8_t | float | float |
| hifloat8_t | float | half |
| hifloat8_t | float | bfloat16_t |
| fp8_e5m2_t/fp8_e4m3fn_t | half | half |
| fp8_e5m2_t/fp8_e4m3fn_t | bfloat16_t | bfloat16_t |
| fp8_e5m2_t/fp8_e4m3fn_t | float | float |
| fp8_e5m2_t/fp8_e4m3fn_t | float | half |
| fp8_e5m2_t/fp8_e4m3fn_t | float | bfloat16_t |
| fp4x2_e1m2_t/fp4x2_e2m1_t<br><br>（当前均只支持PER_GROUP场景） | fp8_e8m0_t | half |
| fp4x2_e1m2_t/fp4x2_e2m1_t<br><br>（当前均只支持PER_GROUP场景） | fp8_e8m0_t | bfloat16_t |

**表4**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的操作数。不同型号支持的数据类型请参考[dst支持的数据类型](#li117871685514)。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| src | 输入 | 源操作数。不同型号支持的数据类型请参考[src支持的数据类型](#li13336025165520)。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| offset | 输入 | 输入数据伪量化时的偏移量。不同型号支持的数据类型请参考[offset支持的数据类型](#li5382122225718)。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| scale | 输入 | 输入数据伪量化时的缩放因子。不同型号支持的数据类型请参考[scale支持的数据类型](#li103341450906)。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| sharedTmpBuffer | 输入 | 临时缓存。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>临时空间大小BufferSize的获取方式请参考[GetAscendAntiQuantMaxMinTmpSize](GetAscendAntiQuantMaxMinTmpSize.md)。 |
| k | 输入 | isTranspose为true时，src的shape为[N,K]；isTranspose为false时，src的shape为[K,N]。<br><br>参数k对应其中的K值。 |
| shapeInfo | 输入 | 设置参数offset和scale的shape信息，仅PER_CHANNEL场景（按通道量化）需要配置。<br><br>可选参数。在PER_CHANNEL场景，如果未传入该参数或者结构体中数据设置为0，将从offset和scale的[ShapeInfo](../../basic_api/aux_data_structures/ShapeInfo.md)中获取offset和scale的shape信息。<br><br>AntiQuantShapeInfo类型，定义如下方代码所示。 |

```
struct AntiQuantShapeInfo {
    uint32_t offsetHeight{0}; // offset的高
    uint32_t offsetWidth{0};  // offset的宽
    uint32_t scaleHeight{0};  // scale的高
    uint32_t scaleWidth{0};   // scale的宽
};
```

**表5**  PER\_TOKEN/PER\_GROUP b8/float4场景接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstTensor | 输出 | 目的操作数。支持的数据类型为：half、bfloat16_t、float。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| srcTensor | 输入 | 源操作数。支持的数据类型为：int8_t、fp8_e4m3fn_t、fp8_e5m2_t、hifloat8_t、fp4x2_e1m2_t、fp4x2_e2m1_t。注意，对于fp4x2_e1m2_t、fp4x2_e2m1_t数据类型，仅在PER_GROUP场景下支持。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| sharedTmpBuffer | 输入 | 临时缓存。支持的数据类型为：uint8_t。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>临时空间大小BufferSize的获取方式请参考[GetAscendAntiQuantMaxMinTmpSize](GetAscendAntiQuantMaxMinTmpSize.md)。 |
| scaleTensor | 输入 | 量化参数scale。支持的数据类型为：half、float、bfloat16_t、fp8_e8m0_t。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| offsetTensor | 输入 | 量化参数offset。支持的数据类型和scaleTensor保持一致。对于float4场景，offsetTensor不生效。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| para | 输入 | 量化接口的参数，AscendAntiQuantParam类型，具体定义如下方代码所示，其中参数的含义如下。<br>m：m方向元素个数。<br>n：n方向元素个数。n值对应的数据大小需满足32B对齐的要求，即shape最后一维为n的输入输出均需要满足该维度上32B对齐的要求。<br>calCount：参与计算的元素个数。calCount必须是n的整数倍。<br>groupSize：PER_GROUP场景有效，表示groupSize行/列数据共用一个scale/offset。groupSize的取值必须大于0且是32的整倍数。 |

```
struct AscendAntiQuantParam {
    uint32_t m;
    uint32_t n;
    uint32_t calCount;
    uint32_t groupSize = 0;
};
```

## 返回值说明

无

## 约束说明

-   **不支持源操作数与目的操作数地址重叠。**
-   操作数地址对齐要求请参见[通用地址对齐约束](../../general_description_and_constraints.md#section796754519912)。
-   输入输出操作数参与计算的数据长度要求32B对齐。
-   输入带转置场景，k需要32B对齐。
-   调用接口前，确保输入数据的size正确，offset和scale的size和shape正确。

-   PER\_TOKEN/PER\_GROUP b8/float4场景支持情况如下：

    <!-- npu="950" id33 -->
    - Ascend 950PR/Ascend 950DT：支持。
    <!-- end id33 -->

    <!-- npu="A3" id38 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持。
    <!-- end id38 -->

    <!-- npu="910b" id39 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持。
    <!-- end id39 -->

    <!-- npu="310p" id40 -->
    - Atlas 推理系列产品AI Core：不支持。
    <!-- end id40 -->

-   PER\_TOKEN/PER\_GROUP b8/float4场景，连续计算方向（即n方向）的数据量要求32B对齐。
-   非PER\_TOKEN/PER\_GROUP b8/float4场景dst支持的数据类型<a id="li117871685514"></a>

    <!-- npu="950" id17 -->
    - Ascend 950PR/Ascend 950DT，支持的数据类型为：half、bfloat16\_t。
    <!-- end id17 -->

    <!-- npu="A3" id18 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为：half、bfloat16\_t。
    <!-- end id18 -->

    <!-- npu="910b" id19 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为：half、bfloat16\_t。
    <!-- end id19 -->

    <!-- npu="310p" id20 -->
    - Atlas 推理系列产品AI Core，支持的数据类型为：half。
    <!-- end id20 -->

    <!-- npu="x90" id3 -->
    - Kirin X90，支持的数据类型为：half。
    <!-- end id3 -->

    <!-- npu="9030" id4 -->
    - Kirin 9030，支持的数据类型为：half。
    <!-- end id4 -->

-   非PER\_TOKEN/PER\_GROUP b8/float4场景src支持的数据类型<a id="li13336025165520"></a>

    <!-- npu="950" id21 -->
    - Ascend 950PR/Ascend 950DT，PER\_CHANNEL和PER\_TENSOR场景下支持的数据类型为：int8\_t、fp8\_e4m3fn\_t、fp8\_e5m2\_t、hifloat8\_t；PER\_GROUP float4场景下支持的数据类型为：fp4x2\_e2m1\_t、fp4x2\_e1m2\_t。
    <!-- end id21 -->

    <!-- npu="A3" id22 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为：int8\_t、int4b\_t。
    <!-- end id22 -->

    <!-- npu="910b" id23 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为：int8\_t、int4b\_t。
    <!-- end id23 -->

    <!-- npu="310p" id24 -->
    - Atlas 推理系列产品AI Core，支持的数据类型为：int8\_t。
    <!-- end id24 -->

    <!-- npu="x90" id5 -->
    - Kirin X90，支持的数据类型为：int8\_t。
    <!-- end id5 -->

    <!-- npu="9030" id6 -->
    - Kirin 9030，支持的数据类型为：int8\_t。
    <!-- end id6 -->

-   非PER\_TOKEN/PER\_GROUP b8/float4场景offset支持的数据类型<a id="li5382122225718"></a>

    <!-- npu="950" id25 -->
    - Ascend 950PR/Ascend 950DT，支持的数据类型为：half、bfloat16\_t。
    <!-- end id25 -->

    <!-- npu="A3" id26 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为：half、bfloat16\_t。
    <!-- end id26 -->

    <!-- npu="910b" id27 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为：half、bfloat16\_t。
    <!-- end id27 -->

    <!-- npu="310p" id28 -->
    - Atlas 推理系列产品AI Core，支持的数据类型为：half。
    <!-- end id28 -->

    <!-- npu="x90" id7 -->
    - Kirin X90，支持的数据类型为：half。
    <!-- end id7 -->

    <!-- npu="9030" id8 -->
    - Kirin 9030，支持的数据类型为：half。
    <!-- end id8 -->

-   非PER\_TOKEN/PER\_GROUP b8/float4场景scale支持的数据类型<a id="li103341450906"></a>

    <!-- npu="950" id29 -->
    - Ascend 950PR/Ascend 950DT，PER\_CHANNEL和PER\_TENSOR场景下支持的数据类型为：half、bfloat16\_t；PER\_GROUP float4场景下支持的数据类型为：fp8\_e8m0\_t。
    <!-- end id29 -->

    <!-- npu="A3" id30 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为：half、bfloat16\_t。
    <!-- end id30 -->

    <!-- npu="910b" id31 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为：half、bfloat16\_t。
    <!-- end id31 -->

    <!-- npu="310p" id32 -->
    - Atlas 推理系列产品AI Core，支持的数据类型为：half。
    <!-- end id32 -->

    <!-- npu="x90" id9 -->
    - Kirin X90，支持的数据类型为：half。
    <!-- end id9 -->

    <!-- npu="9030" id10 -->
    - Kirin 9030，支持的数据类型为：half。
    <!-- end id10 -->

## 调用示例

```
// dstLocal：结果张量
// srcLocal：量化输入
// offsetLocal：偏移参数
// scaleLocal：缩放参数
// sharedTmpBuffer：开发者管理的临时缓冲区，用于存放内部计算中的中间变量
// k：k轴长度
// shapeInfo：offsetLocal和scaleLocal张量的shape信息
AscendC::AntiQuantShapeInfo shapeInfo = {1, elementCountOfOffset, 1, elementCountOfOffset};
AscendC::AscendAntiQuant<InputType, OutType, false>(
    dstLocal, srcLocal, offsetLocal, scaleLocal, sharedTmpBuffer, k, shapeInfo);
```

结果示例如下：

```
输入数据src（shape为[2,64]，非转置场景）:
[1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1]
offset（shape为[1,64]）:
[2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2.
 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2.
 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2.]
scale（shape为[1,64]）:
[3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3.
 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3.
 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3.]
输出数据dstLocal（shape为[2,64]）:
[9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9.
 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9.
 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9.
 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9.
 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9.
 9. 9. 9. 9. 9. 9. 9. 9.]
```

PER\_TOKEN/PER\_GROUP b8场景调用示例如下。

```
// 注意m,n需从外部传入
constexpr static bool isReuseSource = false;
constexpr static AscendAntiQuantConfig config = {has_offset, has_transpose, -1};
constexpr static AscendAntiQuantPolicy policy = AscendAntiQuantPolicy::PER_TOKEN;
AscendAntiQuantParam para;
para.m = m;
para.n = n;
para.calCount = calCount;
AscendAntiQuant<dstType, srcType, scaleType, config, policy>(dstLocal, srcLocal, scaleLocal, offsetLocal, para);
```
