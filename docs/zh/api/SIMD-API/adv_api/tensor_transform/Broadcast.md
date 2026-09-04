# Broadcast

## 产品支持情况

<!-- npu="950" id5 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id5 -->
<!-- npu="A3" id6 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id6 -->
<!-- npu="910b" id7 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id7 -->
<!-- npu="310b" id8 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id8 -->
<!-- npu="310p" id9 -->
- Atlas 推理系列产品AI Core：支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id9 -->
<!-- npu="910" id10 -->
- Atlas 训练系列产品：不支持
<!-- end id10 -->
<!-- npu="x90" id1 -->
- Kirin X90：支持
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin 9030：支持
<!-- end id2 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/tensor_transform/Broadcast_res.md#id1 -->

## 功能说明

将输入按照输出shape进行广播。

比如A的shape为\(2,1\)，广播的目标shape为\(2,16\)，则会将原来的一列扩展为相同的16列。

```
输入数据：
[[ 1]
 [ 2]]
输出数据：
[[ 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1]
 [ 2  2  2  2  2  2  2  2  2  2  2  2  2  2  2  2]]
```

## 实现原理

以float类型，ND格式，\[m, 1\]广播到\[m, k\]为例，描述Broadcast高阶API内部算法框图，如下图所示。

**图1**  Broadcast算法框图  
![](../../../figures/broadcast_algorithm_diagram.png "Broadcast算法框图")

计算过程分为如下几步，均在Vector上进行：

1.  brcb步骤：将每个元素广播为一个datablock；
2.  Copy步骤：将每个datablock均复制为多个datablock，k对齐场景下即为结果y；
3.  对于k非对齐的场景，再使用GatherMask截取\[m, k\]个元素，其中k'表示k向上对齐32B的大小。

## 函数原型

-   通过sharedTmpBuffer入参传入临时空间

    ```
    template <typename T, int32_t dim, int32_t axis, bool isReuseSource = false>
    __aicore__ inline void Broadcast(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dstShape[dim], const uint32_t srcShape[dim], LocalTensor<uint8_t>& sharedTmpBuffer)
    ```

-   接口框架申请临时空间

    ```
    template <typename T, int32_t dim, int32_t axis, bool isReuseSource = false>
    __aicore__ inline void Broadcast(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dstShape[dim], const uint32_t srcShape[dim])
    ```

-   支持动态shape

    <!-- npu="950" id11 -->
    仅在Ascend 950PR/Ascend 950DT上支持。
    <!-- end id11 -->

    ```
    template<class T, int constRank=-1, uint32_t* constDstShape = nullptr, uint32_t* constSrcShape = nullptr, bool constSrcInnerPad = false>
    __aicore__ inline void Broadcast(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t* dstShape, const uint32_t* srcShape, BroadcastTiling* tiling)
    ```

该接口需要额外的临时空间来存储计算过程中的中间变量。临时空间支持开发者**通过sharedTmpBuffer入参传入**和**接口框架申请**两种方式。

-   通过sharedTmpBuffer入参传入，使用该tensor作为临时空间进行处理，接口框架不再申请。该方式开发者可以自行管理sharedTmpBuffer内存空间，并在接口调用完成后，复用该部分内存，内存不会反复申请释放，灵活性较高，内存利用率也较高。
-   接口框架申请临时空间，开发者无需申请，但是需要预留临时空间的大小。

通过sharedTmpBuffer传入的情况，开发者需要为tensor申请空间；接口框架申请的方式，开发者需要预留临时空间。临时空间大小BufferSize的获取方式如下：通过[GetBroadCastMaxMinTmpSize](GetBroadCastMaxMinTmpSize.md)中提供的接口获取需要预留空间范围的大小。

另外，提供了一个核函数（Kernel）侧计算Tiling的接口，针对Broadcast的实现计算Tiling，获取Tiling结果。该接口的模板参数功能与支持动态shape的Broadcast接口模板参数相同，其余参数说明请参见[表5](#table5458981523)。

-   **kernel侧tiling计算接口**

    <!-- npu="950" id12 -->
    仅在Ascend 950PR/Ascend 950DT上支持。
    <!-- end id12 -->

    ```
    template<class T, int constRank=-1, uint32_t* constDstShape = nullptr, uint32_t* constSrcShape = nullptr>
    __aicore__ inline void GetBroadcastTilingInfo(uint32_t rank, const uint32_t* dstShape, const uint32_t* srcShape, bool srcInnerPad, BroadcastTiling& tiling)
    ```

## 参数说明

**表1**  模板参数说明

| 参数名称 | 功能 |
| --- | --- |
| T | 操作数的数据类型。不同型号支持的数据类型请参考[支持的数据类型](#li12616155731718)。 |
| dim | 输入/输出tensor的维度，目前仅支持1维和2维。 |
| axis | 要广播的维度，目前仅支持0和1。参数取值为0代表要广播第1维，取值为1代表要广播第2维。 |
| isReuseSource | 是否允许修改源操作数。该参数预留，传入默认值false即可。 |

**表2**  支持动态shape接口的模板参数说明

| 参数名称 | 功能 |
| --- | --- |
| T | 操作数的数据类型，目前支持int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t。 |
| constRank | 输入/输出tensor的维度数目。<br>默认值-1为动态shape场景，内部按照GetBroadcastTilingInfo接口中的参数rank计算；<br>constRank大于0时，必须与GetBroadcastTilingInfo接口中的参数rank取值相同。rank当前支持的范围为[1, 9]。 |
| constDstShape | 输出tensor的shape。uint32_t类型的数组。<br>该数组中任一维度取值为0，表示该维度为动态场景，该维度实际取值由参数dstShape对应维度取值决定。<br>该数组中任一维度取值大于0，表示该维度为静态场景，该维度取值与参数dstShape中对应维度取值相同。<br><br>该参数预留，传入默认值nullptr即可。 |
| constSrcShape | 输入tensor的shape。uint32_t类型的数组。<br>该数组中任一维度取值为0，表示该维度为动态场景，实际shape由参数srcShape决定。<br>该数组中任一维度取值大于0，表示该维度为静态场景，与参数srcShape中对应维度取值相同。<br><br>该参数预留，传入默认值nullptr即可。 |
| constSrcInnerPad | 表示输入的最后一维srcShape[rank-1]是否32B对齐，其中rank为输入/输出tensor的维度数目。<br><br>该参数预留，传入默认值false即可。 |

**表3**  接口参数说明

| 参数名称 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstLocal | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| srcLocal | 输入 | 源操作数。<br><br>源操作数的数据类型需要与目的操作数保持一致。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| dstShape | 输入 | 输出tensor的shape：uint32_t类型的数组，长度为1或者2，输入/输出的shape维度数目必须一致。 |
| srcShape | 输入 | 输入tensor的shape：uint32_t类型的数组，长度为1或者2，输入/输出的shape维度数目必须一致。 |
| sharedTmpBuffer | 输入 | 临时缓存。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>用于Broadcast内部复杂计算时存储中间变量，由开发者提供。<br><br>临时空间大小BufferSize的获取方式请参考[GetBroadCastMaxMinTmpSize](GetBroadCastMaxMinTmpSize.md)。 |

**表4**  支持动态shape接口的参数说明

| 参数名称 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| src | 输入 | 源操作数。<br><br>源操作数的数据类型需要与目的操作数保持一致。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| dstShape | 输入 | 输出tensor的shape：uint32_t类型的数组，长度取值范围为[1, 9]。输入/输出的shape维度数目必须一致，且满足条件dstShape[i] >= srcShape[i]。 |
| srcShape | 输入 | 输入tensor的shape：uint32_t类型的数组，长度取值范围为[1, 9]。输入/输出的shape维度数目必须一致，且满足条件dstShape[i] >= srcShape[i]。<br><br>当srcShape[i]的值为1，且dstShape[i]不等于srcShape[i]时，表示i轴为广播轴。 |
| tiling | 输入 | Broadcast接口所需的Tiling信息。BroadcastTiling*类型，通过调用核函数（Kernel）侧的tiling计算接口GetBroadcastTilingInfo获取。 |

**表5**  kernel侧tiling计算接口参数说明

<a name="table5458981523"></a>

| 参数名称 | 输入/输出 | 功能 |
| --- | --- | --- |
| rank | 输入 | 输入/输出tensor的维度数目，目前支持的取值为[1, 9]。 |
| dstShape | 输入 | 输出tensor的shape：uint32_t类型的数组，长度取值范围为[1, 9]。输入/输出的shape维度数目必须一致，且满足条件dstShape[i] >= srcShape[i]。 |
| srcShape | 输入 | 输入tensor的shape：uint32_t类型的数组，长度取值范围为[1, 9]。输入/输出的shape维度数目必须一致，且满足条件dstShape[i] >= srcShape[i]。<br><br>当srcShape[i]的值为1，且dstShape[i]不等于srcShape[i]时，表示i轴为广播轴。 |
| srcInnerPad | 输入 | 表示输入的最后一维srcShape[rank-1]是否32B对齐。<br><br>当前仅支持取值为false。 |
| tiling | 输出 | 计算返回的Tiling信息。BroadcastTiling&类型。 |

## 返回值说明

无

## 约束说明

-   操作数地址对齐要求请参见[通用地址对齐约束](../../general_description_and_constraints.md#section796754519912)。
-   **不支持源操作数与目的操作数地址重叠。**
-   当前仅支持ND格式的输入，不支持其他格式。
-   dim目前仅支持1或者2， axis目前仅支持0或者1。
<!-- npu="310p" id13 -->
-   对于Atlas 推理系列产品AI Core，在dim=2且axis=1时，srcShape\[0\]必须为32B对齐，即当输入/输出Tensor的维度为2维，且广播的维度为1时，输入Tensor的第0维数据需为32字节的倍数。
<!-- end id13 -->
-   在dim=2，axis=0时，要求srcShape\[1\]必须32B对齐。
<!-- npu="950" id14 -->
-   对于Ascend 950PR/Ascend 950DT，输入/输出tensor支持的维度数目，即rank支持的取值范围为\[1, 9\]。
<!-- end id14 -->
-   支持的数据类型<a id="li12616155731718"></a>

    <!-- npu="950" id15 -->
    Ascend 950PR/Ascend 950DT，支持的数据类型为：b8、b16、b32、b64位宽对应的数据类型，具体数据类型请参考[不同位宽对应的数据类型](../../basic_api/data_structures/builtin_data_types.md#section16395539499)。
    <!-- end id15 -->
    <!-- npu="A3" id16 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为：int8\_t、uint8\_t、half、float。
    <!-- end id16 -->
    <!-- npu="910b" id17 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为：int8\_t、uint8\_t、half、float。
    <!-- end id17 -->
    <!-- npu="310p" id18 -->
    Atlas 推理系列产品AI Core，支持的数据类型为：int8\_t、uint8\_t、half、float。
    <!-- end id18 -->
    <!-- npu="x90" id3 -->
    Kirin X90，支持的数据类型为：int8\_t、uint8\_t、half、float。
    <!-- end id3 -->
    <!-- npu="9030" id4 -->
    Kirin 9030，支持的数据类型为：int8\_t、uint8\_t、half、float。
    <!-- end id4 -->

## 调用示例

```
// dstLocal：输出Tensor
// srcLocal：输入Tensor
const uint32_t srcShape[2] = {16, 1};  // 源数据shape
const uint32_t dstShape[2] = {16, 16}; // broadcast数据shape
AscendC::Broadcast<float, 2, 1>(
    dstLocal, srcLocal, dstShape, srcShape); // broadcast数据类型为float，源数据是2维， broadcast第1维度
```

结果示例如下：

```
输入数据（srcLocal）:
[[ 1]
 [ 2]
 [ 3]
 [ 4]
 [ 5]
 [ 6]
 [ 7]
 [ 8]
 [ 9]
 [10]
 [11]
 [12]
 [13]
 [14]
 [15]
 [16]]
dim：2
axis：1
输出数据（dstLocal）:
[[ 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1]
 [ 2  2  2  2  2  2  2  2  2  2  2  2  2  2  2  2]
 [ 3  3  3  3  3  3  3  3  3  3  3  3  3  3  3  3]
 [ 4  4  4  4  4  4  4  4  4  4  4  4  4  4  4  4]
 [ 5  5  5  5  5  5  5  5  5  5  5  5  5  5  5  5]
 [ 6  6  6  6  6  6  6  6  6  6  6  6  6  6  6  6]
 [ 7  7  7  7  7  7  7  7  7  7  7  7  7  7  7  7]
 [ 8  8  8  8  8  8  8  8  8  8  8  8  8  8  8  8]
 [ 9  9  9  9  9  9  9  9  9  9  9  9  9  9  9  9]
 [10 10 10 10 10 10 10 10 10 10 10 10 10 10 10 10]
 [11 11 11 11 11 11 11 11 11 11 11 11 11 11 11 11]
 [12 12 12 12 12 12 12 12 12 12 12 12 12 12 12 12]
 [13 13 13 13 13 13 13 13 13 13 13 13 13 13 13 13]
 [14 14 14 14 14 14 14 14 14 14 14 14 14 14 14 14]
 [15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15]
 [16 16 16 16 16 16 16 16 16 16 16 16 16 16 16 16]]
```
