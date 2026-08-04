# Copy（L1 Buffer到L0A Buffer数据搬运）

> [!NOTE]说明
>本接口为试验接口，在后续版本中可能会调整或改进，不保证后续兼容性。请开发者在使用过程中关注后续版本更新。

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

头文件路径为：`tensor_api/tensor.h`。

Tensor API通过`Copy`接口统一执行不同通路数据搬运。该接口用于将L1 Buffer中的左矩阵数据搬运到L0A Buffer。

该通路以512Byte数据分形为基本搬运单位。不同数据类型对应的分形矩阵形态如下：

| 数据位宽 | 分形矩阵形态 |
| :--- | :--- |
| b4 | 16 * 64 |
| b8 | 16 * 32 |
| b16 | 16 * 16 |
| b32 | 16 * 8 |

接口支持非转置搬运、转置搬运和Img2Col卷积数据搬运。源张量和目的张量的Layout需要匹配当前通路支持的格式组合。Img2Col场景中，源张量为`NC1HWC0`格式，目的张量为`NZ`格式，搬运过程中将卷积特征图展开为矩阵。

接口支持Batch模式。Batch模式下，源张量和目的张量的Layout需要在原有分形Layout最前面增加Batch维，Shape形态为`(B, 单矩阵Shape)`，其中`B`表示Batch数量。用户可使用`MakeFrameLayout<NZLayoutPtn, DataType>(B, m, k)`或`MakeFrameLayout<ZNLayoutPtn, DataType>(B, m, k)`构造带Batch维的Layout。

## 函数原型

- 执行L1 Buffer到L0A Buffer的数据搬运。

    ```cpp
    template <typename AtomType, typename DstTensor, typename SrcTensor>
    __aicore__ inline void Copy(const CopyAtom<AtomType>& atomCopy, const DstTensor& dst, const SrcTensor& src)
    ```

- 构造默认搬运原子对象。

    ```cpp
    template <typename CopyOperationType>
    __aicore__ inline constexpr auto MakeCopy(const CopyOperationType& copyOperation)
    ```

- 构造指定Trait的搬运原子对象。

    ```cpp
    template <typename CopyOperationType, typename CopyTraitType>
    __aicore__ inline constexpr auto MakeCopy(
        const CopyOperationType& copyOperation, const CopyTraitType& copyTrait)
    ```

Img2Col搬运通过`CopyAtom::with`绑定`Img2ColParams`后，使用相同的`Copy(atomCopy, dst, src)`函数原型执行。

## 参数说明

**表1**  `Copy`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| atomCopy | 输入 | 搬运原子对象。L1 Buffer到L0A Buffer通路可通过`MakeCopy(CopyL12L0A{})`或`MakeCopy(CopyL12L0A{}, CopyL12L0ATraitDefault{})`构造。 |
| dst | 输出 | 目的张量，存储位置为`Location::L0A`。 |
| src | 输入 | 源张量，存储位置为`Location::L1`。 |

**表2**  `MakeCopy`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| copyOperation | 输入 | 搬运操作对象。L1 Buffer到L0A Buffer通路使用`CopyL12L0A{}`。 |
| copyTrait | 输入 | 搬运Trait对象，用于指定搬运特性。L1 Buffer到L0A Buffer默认Trait使用`CopyL12L0ATraitDefault{}`。 |

### Img2ColParams说明

`Img2ColParams<T>`用于配置Img2Col搬运的目的矩阵范围、卷积核、滑动步长、膨胀和padding。源特征图的H、W和通道数由`NC1HWC0`源Layout推导。

```cpp
template <typename T>
struct Img2ColParams {
    uint16_t mExtension = 0;
    uint16_t kExtension = 0;
    uint16_t mStartPt = 0;
    uint16_t kStartPt = 0;
    uint8_t padList[4] = {0, 0, 0, 0};
    uint8_t strideW = 1;
    uint8_t strideH = 1;
    uint8_t filterW = 1;
    uint8_t filterH = 1;
    uint8_t dilationFilterW = 1;
    uint8_t dilationFilterH = 1;
    bool filterSizeW = false;
    bool filterSizeH = false;
    bool transpose = false;
    bool fMatrixCtrl = false;
    T padValue = 0;
};
```

**表3**  `Img2ColParams`成员说明

| 成员 | 默认值 | 描述 |
| :--- | :--- | :--- |
| mExtension | `0` | 目的矩阵M轴的搬运元素数，取值范围为[0, 65535]，为0时不执行搬运。搬运范围未覆盖目的矩阵最下侧分形时，b8和b16数据类型要求取值为16的倍数，b32数据类型无倍数要求；覆盖最下侧分形时无倍数要求。 |
| kExtension | `0` | 目的矩阵K轴的搬运元素数，取值范围为[0, 65535]，为0时不执行搬运。搬运范围未覆盖目的矩阵最右侧分形时，b8、b16和b32数据类型分别要求取值为32、16和8的倍数；覆盖最右侧分形时无倍数要求。 |
| mStartPt | `0` | 目的矩阵M轴的搬运起点，取值范围[0, 32767]。默认为0。 |
| kStartPt | `0` | 该指令在目的操作数width维度的起点，对于b32类型，应为8的倍数；对于b16类型，应为16的倍数；对于b8类型，应为32的倍数；对于b4类型，应为64的倍数。取值范围[0, 65535]。默认为0。|
| padList | `{0, 0, 0, 0}` | padding大小，依次为左、右、上、下，每个值的取值范围为[0, 255]。 |
| strideW | `1` | 卷积核在源特征图W轴的滑动步长，取值范围为[0, 63]。 |
| strideH | `1` | 卷积核在源特征图H轴的滑动步长，取值范围为[0, 63]。 |
| filterW | `1` | 卷积核的宽度，取值范围为[0, 255]。 |
| filterH | `1` | 卷积核的高度，取值范围为[0, 255]。 |
| dilationFilterW | `1` | 卷积核W轴的膨胀系数，取值范围为[0, 255]。 |
| dilationFilterH | `1` | 卷积核H轴的膨胀系数，取值范围为[0, 255]。 |
| filterSizeW | `false` | 是否在`filterW`基础上将卷积核宽度增加256。 |
| filterSizeH | `false` | 是否在`filterH`基础上将卷积核高度增加256。 |
| transpose | `false` | 是否对展开后的目的矩阵进行转置。 |
| fMatrixCtrl | `false` | FeatureMap属性选择标识，当前仅支持`false`。 |
| padValue | `0` | padding区域的填充值。模板参数T需要与目的张量的元素类型一致。 |

## 数据类型

非Img2Col搬运支持的数据类型包括：

`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`int8_t`、`uint8_t`、`hifloat8_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`half`、`bfloat16_t`、`int16_t`、`uint16_t`、`int32_t`、`uint32_t`、`float`。

源张量和目的张量的数据类型需要保持一致。

Img2Col搬运不支持b4数据类型，即不支持`fp4x2_e2m1_t`和`fp4x2_e1m2_t`。

## 返回值说明

`Copy`无返回值。`MakeCopy`返回`CopyAtom`对象。

## 约束说明

- 目的地址位于L0A Buffer时，起始地址需要满足512Byte对齐要求。
- 源地址位于L1 Buffer时，起始地址需要满足32Byte对齐要求。
- 特殊数据类型约束：L1 Buffer到L0A Buffer通路使能转置时，即`ZN2NZ`，支持的数据类型约束如下：
  - b32数据类型要求源矩阵Shape在K轴方向16对齐。
  - b8数据类型要求源矩阵Shape在M轴方向32对齐。
  - b4数据类型要求源矩阵Shape在M轴方向64对齐。
- Batch模式要求源张量和目的张量的Batch数量一致，且在L1 Buffer和L0A Buffer上张量数据连续排布。
- Img2Col搬运当前仅支持N为1的`NC1HWC0`源张量，目的张量需要为`NZ`格式。
- `Img2ColParams<T>`的T需要与L0A Buffer目的张量的元素类型一致。

## 关键特性说明

### 非转置搬运

当源张量和目的张量数据格式为`NZ`时，接口按非转置方式从L1 Buffer搬运到L0A Buffer。

**图1**  非转置搬运示意图

![非转置搬运示意图](../../../../figures/notranspose.png)

### 转置搬运

当源张量数据格式为`ZN`、目的张量数据格式为`NZ`时，接口按转置方式搬运。转置搬运会对源张量中每个分形矩阵进行转置。

不同数据位宽的转置搬运示意如下。

#### b4转置搬运

**图2**  b4转置搬运示意图

![b4转置搬运示意图](../../../../figures/transpose_b4.png)

#### b8转置搬运

**图3**  b8转置搬运示意图

![b8转置搬运示意图](../../../../figures/transpose_b8.png)

#### b16转置搬运

**图4**  b16转置搬运示意图

![b16转置搬运示意图](../../../../figures/transpose_b16.png)

#### b32转置搬运

**图5**  b32转置搬运示意图

![b32转置搬运示意图](../../../../figures/transpose_b32.png)

### Batch搬运

当源张量和目的张量均使用带Batch维的Layout时，`Copy`接口会按照Batch维描述的数据范围完成L1 Buffer到L0A Buffer的数据搬运。非转置搬运时源张量、目的张量均为`NZ`格式，转置搬运时源张量为`ZN`格式，目的张量为`NZ`格式。

### Img2Col搬运

Img2Col搬运将`NC1HWC0`格式特征图按卷积核窗口展开为`NZ`格式矩阵。展开后矩阵的M轴对应输出特征图的空间位置，K轴对应卷积核的H、W和输入通道。用户通过`Img2ColParams`配置展开范围和卷积参数，再通过`copyAtom.with(params)`将参数绑定到搬运原子对象。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

__aicore__ inline void CopyL1ToL0AExample()
{
    using AType = half;
    constexpr uint32_t m = 16;
    constexpr uint32_t k = 16;

    __cbuf__ AType l1Buf[m * k];
    __ca__ AType l0aBuf[m * k];

    auto l1Tensor = MakeTensor(MakeMemPtr(l1Buf), MakeFrameLayout<NZLayoutPtn, AType>(m, k));

    auto l0aTensor = MakeTensor(MakeMemPtr(l0aBuf), MakeFrameLayout<NZLayoutPtn, AType>(m, k));

    auto copyAtom = MakeCopy(CopyL12L0A{}, CopyL12L0ATraitDefault{});
    Copy(copyAtom, l0aTensor, l1Tensor);
}
```

Batch模式示例如下。

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

__aicore__ inline void CopyL1ToL0ABatchExample()
{
    using AType = half;
    constexpr uint32_t batch = 2;
    constexpr uint32_t m = 16;
    constexpr uint32_t k = 16;

    __cbuf__ AType l1Buf[batch * m * k];
    __ca__ AType l0aBuf[batch * m * k];

    auto layout = MakeFrameLayout<NZLayoutPtn, AType>(batch, m, k);
    auto l1Tensor = MakeTensor(MakeMemPtr(l1Buf), layout);
    auto l0aTensor = MakeTensor(MakeMemPtr(l0aBuf), layout);

    auto copyAtom = MakeCopy(CopyL12L0A{}, CopyL12L0ATraitDefault{});
    Copy(copyAtom, l0aTensor, l1Tensor);
}
```

Img2Col搬运示例如下。输入特征图Shape为`(1, 2, 5, 5, 16)`，卷积核为3×3，stride为1，padding为1，展开后矩阵的逻辑Shape为`(25, 288)`。

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

__aicore__ inline void CopyImg2ColL1ToL0A()
{
    constexpr uint32_t N = 1;
    constexpr uint32_t C1 = 2;
    constexpr uint32_t H = 5;
    constexpr uint32_t W = 5;
    constexpr uint32_t C0 = 16;
    constexpr uint32_t M = 25;
    constexpr uint32_t K = 3 * 3 * C1 * C0;
    constexpr uint32_t M_ALIGN = 32;

    __cbuf__ half l1Buf[N * C1 * H * W * C0];
    __ca__ half l0aBuf[M_ALIGN * K];

    auto l1Feature = MakeTensor(
        MakeMemPtr(l1Buf), MakeFrameLayout<NC1HWC0LayoutPtn>(N, C1, H, W, C0));
    auto l0aMatrix = MakeTensor(
        MakeMemPtr(l0aBuf), MakeFrameLayout<NZLayoutPtn, half>(M, K));

    Img2ColParams<half> params;
    params.mExtension = M;
    params.kExtension = K;
    prams.mStartPt = 0;
    prams.kStartPt = 0;
    params.filterW = 3;
    params.filterH = 3;
    params.strideW = 1;
    params.strideH = 1;
    params.padList[0] = 1;
    params.padList[1] = 1;
    params.padList[2] = 1;
    params.padList[3] = 1;

    auto copyL1ToL0A = MakeCopy(CopyL12L0A{}, CopyL12L0ATraitDefault{});
    Copy(copyL1ToL0A.with(params), l0aMatrix, l1Feature);
}
```
