# Copy (Moving Data from L0C Buffer to Unified Buffer)

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T17:13:43.107Z -->

> [!NOTE] Note
> This API is experimental. It may be adjusted or improved in later versions, and backward compatibility is not guaranteed. Developers are advised to keep track of updates in later versions.

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
- Atlas 200I/500 A2 inference products: Not supported
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

The Tensor API uniformly performs data movement across different paths through the `Copy` API. This API is used to move the cube computation results in the L0C Buffer to the Unified Buffer. The data in the L0C Buffer is usually the output of `Mmad`, and its data format is `NZ`. When moving to the Unified Buffer, the API automatically selects the inline format conversion from `NZ` to `ND`, `NZ` to `DN`, or `NZ` to `NZ` based on the destination tensor format.

The L0C Buffer-to-Unified Buffer move supports non-quantized output, direct conversion output from `float` to `half` or `bfloat16_t`, and inline quantized output with scalar or tensor quantization parameters. The inline ReLU, dual-objective mode, and rounding mode are configured via `CopyL0C2UBTrait`. The `unitFlag` related to fine-grained parallelism between `Mmad` and `Fixpipe` is configured via `FixpipeParams`.

The API supports batch mode. Batch mode is used to complete the movement of multiple cube computation results at once. The layout adds a Batch dimension to the outermost layer of the original cube layout. The source tensor is in `NZ` format with a fixed fractal of 16×16, and can be constructed via `MakeFrameLayout<NZLayoutPtn>(batch, m, n)`. The destination tensor can be constructed via `MakeFrameLayout<NDLayoutPtn>(batch, m, n)`, `MakeFrameLayout<DNLayoutPtn>(batch, m, n)`, `MakeFrameLayout<NDExtLayoutPtn>(batch, m, n)`, `MakeFrameLayout<DNExtLayoutPtn>(batch, m, n)`, or `MakeFrameLayout<NZLayoutPtn, DstType>(batch, m, n)`. For the `NZ` format, the destination data type can be specified via the template parameter `DstType`. `C0` indicates the column fractal size of the `NZ` format, which defaults to 16.

The valid combinations, intermediate data types, and data paths of inline quantization, inline ReLU, inline format conversion, inline channel split, and inline channel merge are shown in the following figure. In the figure, F32 to F16 and F32 to BF16 are non-quantized modes that only perform cast. The remaining paths are non-quantized, inline scalar, or tensor quantization modes. For Ascend 950PR/Ascend 950DT, NZ2DN inline format conversion is also supported.

**Figure 1**  L0C2UB flowchart

![L0C2UB flowchart](../../../../figures/L0C2UB_Function_Combination_950.png)

## Prototype

- Performs non-quantized move from **L0C Buffer** to **Unified Buffer**.

    ```cpp
    template <typename AtomType, typename DstTensor, typename SrcTensor>
    __aicore__ inline void Copy(const CopyAtom<AtomType>& atomCopy, const DstTensor& dst, const SrcTensor& src)
    ```

- Performs quantized move from **L0C Buffer** to **Unified Buffer**.

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

**Table 1**  `Copy` API parameter description

|Parameter|Input/Output|Description|
|--------|--------|--------|
|atomCopy|Input|Move atomic object, which can be constructed via `MakeCopy(CopyL0C2UB{})` or `MakeCopy(CopyL0C2UB{}, CopyL0C2UBTraitDefault{})`.|
|dst|Output|Destination tensor, stored at `Location::UB`. The data format supports `ND`, `DN`, and `NZ`.|
|src|Input|Source tensor, stored at `Location::L0C`, with the data format `NZ`, typically the computation result of `Mmad`.|
|quant|Input|Optional quantization parameter. When a `uint64_t` is passed in, it indicates a scalar quantization parameter; when a tensor is passed in, it indicates a tensor quantization parameter. The tensor is located in the L1 Buffer, with the element type `uint64_t`.|
|fixpipeParams|Input|Optional move parameter, of the type `FixpipeParams`, bound to the move atomic object via the `with` API of `atomCopy`. The default value is used when it is not bound.|

**Table 2**  `MakeCopy` API parameter description

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| copyOperation | Input | Move operation object. For moving from the L0C Buffer to the Unified Buffer, use `CopyL0C2UB{}`. |
| copyTrait | Input | Move trait object. For moving from the L0C Buffer to the Unified Buffer, use `CopyL0C2UBTraitDefault{}` by default. |

### CopyL0C2UBTrait Description

`CopyL0C2UBTrait` is used to configure the static features of the L0C Buffer to Unified Buffer move.

```cpp
struct CopyL0C2UBTrait {
    RoundMode roundMode = RoundMode::DEFAULT;
    bool enableRelu = false;
    bool enableChannelSplit = false;
    DualDstMode dualDstCtl = DUAL_DST_DISABLE;
};
```

**Table 3**  `CopyL0C2UBTrait` member description

|Member|Default Value|Description|
|--------|--------|--------|
|roundMode|`RoundMode::DEFAULT`|Rounding mode. `RoundMode::HYBRID` is supported only in the quantized output scenario where the source type is `float` and the destination type is `hifloat8_t`.|
|enableRelu|`false`|Whether to enable inline Relu.|
|enableChannelSplit|`false`|Whether to enable output data channel splitting.|
|dualDstCtl|`DUAL_DST_DISABLE`|Dual-objective model control. The value can be `DUAL_DST_DISABLE`, `DUAL_DST_SPLIT_M`, or `DUAL_DST_SPLIT_N`. For details, see also [L0C Buffer to Unified Buffer dual-objective model](../cube move-out key feature description/l0c_to_ub_dual_dst.md).|

Example of using a custom trait:

```cpp
constexpr CopyL0C2UBTrait l0c2ubTrait = {
    RoundMode::DEFAULT,
    false,
    false,
    DUAL_DST_DISABLE
};

struct CopyL0C2UBTraitCustom {
    using TraitType = CopyL0C2UBTrait;
    static constexpr const TraitType value = l0c2ubTrait;
};
```

### **FixpipeParams**

`FixpipeParams` is used to configure runtime move parameters.

```cpp
struct FixpipeParams {
    uint8_t unitFlag = 0;
    bool subBlockId = false;
};
```

**Table 4**  `FixpipeParams` member description

|Member|Default Value|Description|
|--------|--------|--------|
|unitFlag|`0`|Controls fine-grained parallelism between the `Mmad` instruction and the `Fixpipe` instruction. `0` means disabled, `2` means enabled and the unit flag bit is not reset after execution, and `3` means enabled and the unit flag bit is reset after execution. When enabled, `unitFlag` in `MmadParams` and `FixpipeParams` must be set to `2` or `3` accordingly.|
|subBlockId|`false`|Indicates the number of the destination UB when single-objective mode is enabled.|

## Data Type

The move from the L0C Buffer to the Unified Buffer automatically selects the quantization mode based on whether a quantization parameter is passed in.

**Table 5**  Data type description

|Source Tensor Type (L0C Buffer)|Destination Tensor Type (Unified Buffer)|Call Form|Description|
|--------|--------|--------|--------|
|`int32_t`|`int8_t`, `uint8_t`|`Copy(atom, dst, src, quant)`|Scalar or tensor quantized output.|
|`float`|`int8_t`, `uint8_t`|`Copy(atom, dst, src, quant)`|Scalar or tensor quantized output.|
|`float`|`fp8_e4m3fn_t`, `hifloat8_t`|`Copy(atom, dst, src, quant)`|Scalar or tensor quantized output. `hifloat8_t` supports `RoundMode::DEFAULT` and `RoundMode::HYBRID`.|
|`int32_t`|`half`, `bfloat16_t`|`Copy(atom, dst, src, quant)`|Scalar or tensor quantized output.|
|`float`|`half`|`Copy(atom, dst, src)`|Direct conversion output, corresponding to F32 to F16.|
|`float`|`bfloat16_t`|`Copy(atom, dst, src)`|Direct conversion output, corresponding to F32 to BF16.|
|`float`|`half`, `bfloat16_t`|`Copy(atom, dst, src, quant)`|Scalar or tensor quantized output.|
|`int32_t`|`int32_t`|`Copy(atom, dst, src)`|Non-quantized output.|
|`float`|`float`|`Copy(atom, dst, src)`|Non-quantized output.|
|`float`|`float`|`Copy(atom, dst, src, quant)`|Scalar or tensor quantized output.|

When `quant` is not passed in, the API selects the non-quantized or direct conversion mode; when `uint64_t` is passed in, it selects the scalar quantization mode; and when a Tensor API tensor is passed in, it selects the tensor quantization mode.

## Return Value

`Copy` has no return value. `MakeCopy` returns a `CopyAtom` object.

## Constraints

- The source cube uses the `NZ` format, and its address requires 64-byte alignment. The destination cube address requires 32-byte alignment.
- When the destination cube is output in `NZ` format, the N dimension size must be a multiple of 16; for the `b8` type channel merge scenario, it must be a multiple of 32; for the `float` type with `enableChannelSplit` enabled for channel split, it must be a multiple of 8.
- The tensor quantization parameter tensor must reside in L1 Buffer, with an element type of `uint64_t` and an address requiring 32-byte alignment. For detailed constraints, see also [inline quantization](../cube_store_key_features/quant_pre.md).
- When `unitFlag` is enabled, `Mmad` must be enabled at the same time.
- When `RoundMode::HYBRID` is used, the source type must be `float` and the destination type must be `hifloat8_t`.
- When the destination layout is `NZ`, setting `dualDstCtl` to `DUAL_DST_SPLIT_N` is not supported. For detailed constraints on the dual-objective model, see also [L0C Buffer to Unified Buffer dual-objective model](../cube_store_key_features/l0c_to_ub_dual_dst.md).
- The channel merge feature is automatically enabled by hardware and cannot be configured via parameters. For detailed constraints, see also [Int8 Channel Merge](../cube_store_key_features/int8_channel_merge.md).

## Key Features

The move from L0C Buffer to Unified Buffer involves key features such as [inline quantization](../cube_store_key_features/quant_pre.md), [inline ReLU](../cube_store_key_features/relu_pre.md), [Int8 Channel Merge](../cube_store_key_features/int8_channel_merge.md), [L0C Buffer to Unified Buffer dual-objective mode](../cube_store_key_features/l0c_to_ub_dual_dst.md), and [batch move](../cube_store_key_features/batch_copy.md).

## Example

### Non-quantized Output

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

__aicore__ inline void CopyL0CToUB()
{
    auto l0c = MakeTensor(MakeMemPtr(l0cAddr), MakeFrameLayout<NZLayoutPtn, 16>(m, n));
    auto ub = MakeTensor(MakeMemPtr(ubAddr), MakeFrameLayout<NDExtLayoutPtn>(m, n));

    FixpipeParams params;
    auto atom = MakeCopy(CopyL0C2UB{}).with(params);
    Copy(atom, ub, l0c);
}
```

### Scalar Quantized Output

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

__aicore__ inline void CopyL0CToUB()
{
    auto l0c = MakeTensor(MakeMemPtr(l0cAddr), MakeFrameLayout<NZLayoutPtn, 16>(m, n));
    auto ub = MakeTensor(MakeMemPtr(ubAddr), MakeFrameLayout<NDExtLayoutPtn>(m, n));

    uint64_t quant = 1;
    auto atom = MakeCopy(CopyL0C2UB{}).with(FixpipeParams{});
    Copy(atom, ub, l0c, quant);
}
```

### Tensor Quantized Output

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

__aicore__ inline void CopyL0CToUB()
{
    auto l0c = MakeTensor(MakeMemPtr(l0cAddr), MakeFrameLayout<NZLayoutPtn, 16>(m, n));
    auto ub = MakeTensor(MakeMemPtr(ubAddr), MakeFrameLayout<NDExtLayoutPtn>(m, n));
    auto quant = MakeTensor(MakeMemPtr(l1QuantAddr), MakeFrameLayout<NDExtLayoutPtn>(1, n));

    auto atom = MakeCopy(CopyL0C2UB{}).with(FixpipeParams{});
    Copy(atom, ub, l0c, quant);
}
```

### Batch Mode Output

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

__aicore__ inline void CopyBatchL0CToUB()
{
    constexpr uint32_t batch = 3;
    constexpr uint32_t m = 32;
    constexpr uint32_t n = 64;

    __cc__ float l0cBuf[batch * m * n];
    __ubuf__ half ubBuf[batch * m * n];

    auto l0c = MakeTensor(MakeMemPtr(l0cBuf), MakeFrameLayout<NZLayoutPtn>(batch, m, n));
    auto ub = MakeTensor(MakeMemPtr(ubBuf), MakeFrameLayout<NDExtLayoutPtn>(batch, m, n));

    auto atom = MakeCopy(CopyL0C2UB{}).with(FixpipeParams{});
    Copy(atom, ub, l0c);
}
```
