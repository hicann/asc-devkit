# Copy (Moving Data from L0C Buffer to Global Memory)

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T17:13:02.208Z -->

> [!NOTE]Note
> This API is experimental and may be adjusted or improved in subsequent versions, with no guarantee of backward compatibility. Developers are advised to keep track of updates in subsequent versions when using it.

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

The header file path is `tensor_api/tensor.h`.

The Tensor API uses the `Copy` API to uniformly move data over different paths. This API moves the cube computation results in the L0C Buffer to Global Memory. The data in the L0C Buffer is usually the output of `Mmad`, and its data format is `NZ`. When moving to Global Memory, the API automatically selects the inline format conversion from `NZ` to `ND`, `NZ` to `DN`, or `NZ` to `NZ` based on the destination tensor layout.

Moving from the L0C Buffer to Global Memory supports non-quantized output, direct conversion output from `float` to `half` or `bfloat16_t`, and inline quantized output with scalar or tensor quantization parameters. Inline Relu, channel split, and quantization rounding mode are configured via `CopyL0C2GMTrait`. The `unitFlag` related to fine-grained parallelism between `Mmad` and `Fixpipe` is configured via `FixpipeParams`.

The API supports batch mode. Batch mode is used to move the computation results of multiple matrices at once. The Layout adds a Batch dimension to the outermost layer of the original cube Layout. The source tensor is in `NZ` format with a fixed fractal of 16×16, and can be constructed via `MakeFrameLayout<NZLayoutPtn>(batch, m, n)`. The destination tensor can be constructed via `MakeFrameLayout<NDLayoutPtn>(batch, m, n)`, `MakeFrameLayout<DNLayoutPtn>(batch, m, n)`, `MakeFrameLayout<NDExtLayoutPtn>(batch, m, n)`, `MakeFrameLayout<DNExtLayoutPtn>(batch, m, n)`, or `MakeFrameLayout<NZLayoutPtn, DstType>(batch, m, n)`. For the `NZ` format, the destination data type can be specified via the template parameter `DstType`. `C0` indicates the column fractal size of the `NZ` format, which defaults to 16.

The valid combinations, intermediate data types, and data paths of inline quantization, inline Relu, inline format conversion, inline channel split, and inline channel merge are shown in the following figure. In the figure, F32 to F16 and F32 to BF16 are non-quantization modes that only perform cast. The remaining paths are non-quantization mode, inline scalar quantization mode, or inline tensor quantization mode.

For Ascend 950PR/Ascend 950DT:

**Figure 1**  L0C2GM flowchart

![L0C2GM flowchart](../../../../figures/L0C2GM_Function_Combination_950.png)

## Prototype

- Performs a non-quantized move from the L0C Buffer to Global Memory.

    ```cpp
    template <typename AtomType, typename DstTensor, typename SrcTensor>
    __aicore__ inline void Copy(const CopyAtom<AtomType>& atomCopy, const DstTensor& dst, const SrcTensor& src)
    ```

- Performs a quantized move from the L0C Buffer to Global Memory.

    ```cpp
    template <typename AtomType, typename DstTensor, typename SrcTensor, typename QuantParam,
        Std::enable_if_t<IsCopyQuantParamV<QuantParam>, int> Enable>
    __aicore__ inline void Copy(const CopyAtom<AtomType>& atomCopy, const DstTensor& dst, const SrcTensor& src,
        const QuantParam& quant)
    ```

- Constructs a move atomic object using the default trait.

    ```cpp
    template <typename CopyOperationType>
    __aicore__ inline constexpr auto MakeCopy(const CopyOperationType& copyOperation)
    ```

- Constructs a move atomic object using the specified trait.

    ```cpp
    template <typename CopyOperationType, typename CopyTraitType>
    __aicore__ inline constexpr auto MakeCopy(const CopyOperationType& copyOperation, const CopyTraitType& copyTrait)
    ```

## Parameters

**Table 1**  Parameter description of the `Copy` API

|Parameter|Input/Output|Description|
|--------|--------|--------|
|atomCopy|Input|Move atomic object, which can be constructed via `MakeCopy(CopyL0C2GM{})` or `MakeCopy(CopyL0C2GM{}, CopyL0C2GMTraitDefault{})`.|
|dst|Output|Destination tensor, stored at `Location::GM`. Supports `ND`, `DN`, and `NZ` data formats.|
|src|Input|Source tensor, stored at `Location::L0C`, in `NZ` data format, typically the computation result of `Mmad`.|
|quant|Input|Optional quantization parameter. When `uint64_t` is passed, it indicates a scalar quantization parameter; when a tensor is passed, it indicates a tensor quantization parameter. The tensor is located in the L1 Buffer with an element type of `uint64_t`.|
|fixpipeParams|Input|Optional move parameter of type `FixpipeParams`, bound to the move atomic object via the `with` API of `atomCopy`. The default value is used when it is not bound.|

**Table 2**  Parameter description of the `MakeCopy` API

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| copyOperation | Input | Move operation object. For moving from the L0C Buffer to Global Memory, `CopyL0C2GM{}` is used. |
| copyTrait | Input | Move static trait object. For moving from the L0C Buffer to Global Memory, `CopyL0C2GMTraitDefault{}` is used by default. |

### **CopyL0C2GMTrait** Description

`CopyL0C2GMTrait` is used to configure the static traits for moving data from the L0C Buffer to Global Memory.

```cpp
struct CopyL0C2GMTrait {
    RoundMode roundMode = RoundMode::DEFAULT;
    bool enableRelu = false;
    bool enableChannelSplit = false;
};
```

**Table 3**  `CopyL0C2GMTrait` member description

|Member|Default Value|Description|
|--------|--------|--------|
|roundMode|`RoundMode::DEFAULT`|Rounding mode. `RoundMode::HYBRID` is supported only in the quantized output scenario where the source type is `float` and the destination type is `hifloat8_t`.|
|enableRelu|`false`|Whether to enable inline Relu.|
|enableChannelSplit|`false`|Whether to enable output data channel split. Only the NZ format scenario where both the source type and the destination type are `float` is supported.|

Example of using a custom trait:

```cpp
constexpr CopyL0C2GMTrait l0c2gmTrait = {
    RoundMode::DEFAULT,
    false,
    true
};

struct CopyL0C2GMTraitCustom {
    using TraitType = CopyL0C2GMTrait;
    static constexpr const TraitType value = l0c2gmTrait;
};
```

### FixpipeParams Description

`FixpipeParams` is used to configure runtime move parameters.

```cpp
struct FixpipeParams {
    uint8_t unitFlag = 0;
    bool subBlockId = false;
};
```

**Table 4** `FixpipeParams` member description

|Member|Default Value|Description|
|--------|--------|--------|
|unitFlag|`0`|Controls the fine-grained parallelism between Mmad instructions and Fixpipe instructions. `0` indicates disabled, `2` indicates enabled without resetting the unit flag bit after execution, and `3` indicates enabled with the unit flag bit reset after execution. When enabled, `unitFlag` in both `MmadParams` and `FixpipeParams` must be set to `2` or `3` accordingly.|
|subBlockId|`false`|Indicates the number of the destination UB when single-destination mode is enabled. This parameter is invalid for L0C Buffer to Global Memory data move.|

## Data Type

For the move from the L0C Buffer to Global Memory, the quantization mode is automatically selected based on whether a quantization parameter is passed in.

**Table 5**  Data type description

|Source tensor type (L0C Buffer)|Destination tensor type (Global Memory)|Call form|Description|
|--------|--------|--------|--------|
|`int32_t`|`int8_t`, `uint8_t`|`Copy(atom, dst, src, quant)`|Scalar or tensor quantized output.|
|`float`|`int8_t`, `uint8_t`|`Copy(atom, dst, src, quant)`|Scalar or tensor quantized output.|
|`float`|`fp8_e4m3fn_t`, `hifloat8_t`|`Copy(atom, dst, src, quant)`|Scalar or tensor quantized output. `hifloat8_t` supports `RoundMode::DEFAULT` and `RoundMode::HYBRID`.|
|`int32_t`|`half`, `bfloat16_t`|`Copy(atom, dst, src, quant)`|Scalar or tensor quantized output.|
|`float`|`half`|`Copy(atom, dst, src)`|Direct conversion output.|
|`float`|`bfloat16_t`|`Copy(atom, dst, src)`|Direct conversion output.|
|`float`|`half`, `bfloat16_t`|`Copy(atom, dst, src, quant)`|Scalar or tensor quantized output.|
|`int32_t`|`int32_t`|`Copy(atom, dst, src)`|Non-quantized output.|
|`float`|`float`|`Copy(atom, dst, src)`|Non-quantized output.|
|`float`|`float`|`Copy(atom, dst, src, quant)`|Scalar or tensor quantized output.|

When `quant` is not passed in, the API selects the non-quantized or direct conversion mode; when `uint64_t` is passed in, it selects the scalar quantization mode; and when a tensor is passed in, it selects the tensor quantization mode.

## Return Value

`Copy` has no return value. `MakeCopy` returns a `CopyAtom` object.

## Constraints

- The source cube uses the `NZ` format, with an address requirement of 64-byte alignment. When the destination cube uses the `DN` or `ND` format, the address requirement is 1-byte alignment; when it uses the `NZ` format, the address requirement is 32-byte alignment.
- When the destination cube is output in the `NZ` format, the size in the N direction must be a multiple of 16; in the `b8` type channel merge scenario, it must be a multiple of 32; and in the `float` scenario with `enableChannelSplit` channel split enabled, it must be a multiple of 8.
- The tensor quantization parameter tensor must be located in the L1 Buffer, with an element type of `uint64_t` and an address requirement of 32-byte alignment. See details in [inline quantization](../cube move-out key feature/quant_pre.md).
- When `unitFlag` is enabled, `Mmad` must be enabled at the same time.
- When `RoundMode::HYBRID` is used, the source type must be `float` and the destination type must be `hifloat8_t`.
- `enableChannelSplit` takes effect only when both the source type and the destination type are `float` and the destination format is `NZ`. See details in [F32 Channel Split](../cube move-out key feature/f32_channel_split.md).
- The channel merge feature is automatically enabled by hardware and cannot be configured through parameters. See details in [Int8 Channel Merge](../cube move-out key feature/int8_channel_merge.md).

## Key Features

Moving data from the L0C Buffer to Global Memory involves the following key features: [inline quantization](../cube_store_key_features/quant_pre.md), [inline Relu](../cube_store_key_features/relu_pre.md), [F32 Channel Split](../cube_store_key_features/f32_channel_split.md), [Int8 Channel Merge](../cube_store_key_features/int8_channel_merge.md), and [batch move](../cube_store_key_features/batch_copy.md).

## Call Example

### Non-quantized Output

```cpp

#include "tensor_api/tensor.h"

using namespace AscendC::Te;

__aicore__ inline void CopyL0CToGM()
{
    auto l0c = MakeTensor(MakeMemPtr(l0cAddr), MakeFrameLayout<NZLayoutPtn, 16>(m, n));
    auto gm = MakeTensor(MakeMemPtr(gmAddr), MakeFrameLayout<NDExtLayoutPtn>(m, n));

    FixpipeParams params;
    auto atom = MakeCopy(CopyL0C2GM{}).with(params);
    Copy(atom, gm, l0c);
}
```

### Scalar Quantized Output

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;
__aicore__ inline void CopyL0CToGM()
{
    auto l0c = MakeTensor(MakeMemPtr(l0cAddr), MakeFrameLayout<NZLayoutPtn, 16>(m, n));
    auto gm = MakeTensor(MakeMemPtr(gmAddr), MakeFrameLayout<NDExtLayoutPtn>(m, n));

    uint64_t quant = 1;
    auto atom = MakeCopy(CopyL0C2GM{}).with(FixpipeParams{});
    Copy(atom, gm, l0c, quant);
}
```

### Tensor Quantized Output

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

__aicore__ inline void CopyL0CToGM()
{
    auto l0c = MakeTensor(MakeMemPtr(l0cAddr), MakeFrameLayout<NZLayoutPtn, 16>(m, n));
    auto gm = MakeTensor(MakeMemPtr(gmAddr), MakeFrameLayout<NDExtLayoutPtn>(m, n));
    auto quant = MakeTensor(MakeMemPtr(l1QuantAddr), MakeFrameLayout<NDExtLayoutPtn>(1, n));

    auto atom = MakeCopy(CopyL0C2GM{}).with(FixpipeParams{});
    Copy(atom, gm, l0c, quant);
}
```

### Batch Mode Output

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

__aicore__ inline void CopyBatchL0CToGM(__gm__ half* gmAddr)
{
    constexpr uint32_t srcBatch = 3;
    constexpr uint32_t dstBatch = 9;
    constexpr uint32_t m = 32;
    constexpr uint32_t n = 64;

    __cc__ float l0cBuf[srcBatch * m * n];

    auto l0c = MakeTensor(MakeMemPtr(l0cBuf), MakeFrameLayout<NZLayoutPtn>(srcBatch, m, n));
    auto gm = MakeTensor(MakeMemPtr(gmAddr), MakeFrameLayout<NDExtLayoutPtn>(dstBatch, m, n));

    auto atom = MakeCopy(CopyL0C2GM{}).with(FixpipeParams{});
    for (uint32_t i = 0; i < dstBatch / srcBatch; ++i) {
        auto gmSlice = gm.Slice(MakeCoord(i * srcBatch, MakeCoord(0, 0)), MakeShape(srcBatch, MakeShape(m, n)));
        Copy(atom, gmSlice, l0c);
    }
}
```
