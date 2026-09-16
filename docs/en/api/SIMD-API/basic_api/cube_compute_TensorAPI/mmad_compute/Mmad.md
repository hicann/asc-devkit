# Mmad

<!-- md-trans-meta sourceCommit=8fad0c67f83ebd2b956accc16a2052471ba091cc translatedAt=2026-08-27T17:23:03.174Z -->

> [!NOTE]
> This API is experimental and may be adjusted or improved in subsequent versions, with no guarantee of backward compatibility. Developers are advised to keep track of updates in subsequent versions.

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

The `Mmad` API performs cube multiply-add between the left cube A on the L0A Buffer and the right cube B on the L0B Buffer, and writes the result to the result cube C on the L0C Buffer. The default mode is ordinary cube computation. Pipeline type: PIPE_M. The mathematical expression is:

```text
C = A * B
```

When a bias input is provided, the bias serves as the initial value of the result cube C and participates in the computation, which is equivalent to:

```text
C = A * B + Bias
```

The cube multiply-add relationship of Mmad is shown in the following figure:

**Figure 1**  Mmad cube multiply-add formula

![Mmad cube multiply-add formula](../../../../figures/mmad_formula.png)

`Mmad` uses the explicitly passed `MmadAtom` to configure the cube computation function. The left cube A, right cube B, and result cube C are illustrated as follows:

**Figure 2**  Mmad cube layout

![Mmad cube layout](../../../../figures/mmad_cube_layout.png)

## Cube Computation Description

| Cube | Physical Location | Dimension | Data Format | Data Type |
| :--- | :--- | :--- | :--- | :--- |
| Left cube A | L0A Buffer | m × k | NZ | See [data type](#data-type) |
| Right cube B | L0B Buffer | k × n | ZN | See [data type](#data-type) |
| Result cube C | L0C Buffer | m × n | NZ | See [data type](#data-type) |

## Prototype

- Perform cube computation without passing bias.

    ```cpp
    template <typename AtomType, typename DstTensor, typename FmTensor, typename FilterTensor>
    __aicore__ inline void Mmad(
        const MmadAtom<AtomType>& atomMmad, const DstTensor& dst, const FmTensor& fm, const FilterTensor& filter)
    ```

- Perform cube computation with bias passed in.

    ```cpp
    template <typename AtomType, typename DstTensor, typename FmTensor, typename FilterTensor, typename BiasTensor,
        Std::enable_if_t<IsAttrTensorV<BiasTensor>, int> Enable>
    __aicore__ inline void Mmad(
        const MmadAtom<AtomType>& atomMmad, const DstTensor& dst, const FmTensor& fm, const FilterTensor& filter,
        const BiasTensor& bias)
    ```

- Construct a default cube computation atom object.

    ```cpp
    template <typename MmadOperationType>
    __aicore__ inline constexpr auto MakeMmad(const MmadOperationType& mmadOperation)
    ```

- Construct a cube computation atom object with the specified Trait.

    ```cpp
    template <typename MmadOperationType, typename MmadTraitType>
    __aicore__ inline constexpr auto MakeMmad(
        const MmadOperationType& mmadOperation, const MmadTraitType& mmadTrait)
    ```

## Parameters

**Table 1** Parameters of the `Mmad` API

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| atomMmad | Input | Cube computation atom object. A default atom object can be constructed through `MakeMmad(MmadOperation{})` or `MakeMmad(MmadOperation{}, MmadTraitDefault{})`. |
| dst | Output | Result cube C, with the storage location `Location::L0C` and the data format NZ. |
| fm | Input | Left cube A, with the storage location `Location::L0A` and the data format NZ. |
| filter | Input | Right cube B, with the storage location `Location::L0B` and the data format ZN. |
| bias | Input | Bias tensor, with the storage location `Location::BIAS` and the data format ND. |

**Table 2** Parameters of the `MakeMmad` API

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| mmadOperation | Input | Cube computation operation object, constructed through `MmadOperation{}`. |
| mmadTrait | Input | Cube computation Trait object, used to specify the static characteristics of cube computation. `MmadTraitDefault{}` is used by default. |

**Table 3** Parameters of `MmadParams`

| Parameter | Type | Default Value | Description |
| :--- | :--- | :--- | :--- |
| m | `uint16_t` | `0` | Height of left cube A and height of result cube C. |
| n | `uint16_t` | `0` | Width of right cube B and width of result cube C. |
| k | `uint16_t` | `0` | Width of left cube A and height of right cube B. |
| unitFlag | `uint8_t` | `0` | Controls the fine-grained parallelism between `Mmad` and the subsequent cube data movement. `0` indicates that it is disabled, `2` indicates that it is enabled and the unit flag bit is not reset after execution, and `3` indicates that it is enabled and the unit flag bit is reset after execution. |
| ccubeInitVal | `bool` | `false` | When bias is not passed, controls whether to initialize result cube C. `true` indicates that cube C is initialized to 0 by default, and `false` indicates that no default operation is performed on cube C, which is initialized by setting the `ccubeSource` parameter. |

**Table 4** Parameters of `MmadTrait`

| Parameter | Type | Default Value | Description |
| :--- | :--- | :--- | :--- |
| fmOffset | `int32_t` | `0` | Left cube offset, retained as a compatibility parameter in the current Tensor API implementation. |
| kDirectionAlign | `bool` | `false` | K-direction alignment control, retained as a compatibility parameter in the current Tensor API implementation. |
| ccubeSource | `bool` | `false` | Configures whether the initial value of cube C comes from the BT Buffer. This configuration is invalid when bias is passed. `false` indicates that no initialization operation is performed on cube C, and `true` indicates that cube C is initialized using the data in the BT Buffer. |
| disableGemv | `bool` | `true` | Whether to disable GEMV mode in the M=1 scenario. `false` indicates that GEMV is enabled, and `true` indicates that GEMV is disabled. |
| mmadType | `MmadType` | `MmadType::NORMAL` | Cube computation type. `MmadType::NORMAL` is used by default. `MmadType::MX` indicates Mmad computation in the MX scenario. |

## Data Type

The following data type combinations of left cube A, right cube B, and result cube C are supported:

| Left Cube A | Right Cube B | Result Cube C |
| :--- | :--- | :--- |
| `int8_t` | `int8_t` | `int32_t` |
| `half` | `half` | `float` |
| `float` | `float` | `float` |
| `bfloat16_t` | `bfloat16_t` | `float` |
| `fp8_e4m3fn_t` | `fp8_e4m3fn_t` | `float` |
| `fp8_e4m3fn_t` | `fp8_e5m2_t` | `float` |
| `fp8_e5m2_t` | `fp8_e4m3fn_t` | `float` |
| `fp8_e5m2_t` | `fp8_e5m2_t` | `float` |
| `hifloat8_t` | `hifloat8_t` | `float` |

In scenarios where bias is passed in, the `int8_t * int8_t -> int32_t` combination requires the bias data type to be `int32_t`, while the other combinations require the bias data type to be `float`.

## Return Value

`Mmad` has no return value. `MakeMmad` returns an `MmadAtom` object.

## Constraints

- `dst` must be located in the L0C Buffer, `fm` must be located in the L0A Buffer, and `filter` must be located in the L0B Buffer.
- The start address of result cube C must meet the 64-byte address alignment requirement.
- The start addresses of left cube A and right cube B must meet the 512-byte address alignment requirement.
- The address alignment requirements above are hardware access constraints. The compiler and runtime do not check all scenarios individually, so users must ensure that the input parameters satisfy these constraints.
- `m`, `n`, and `k` must be consistent with the actual data dimensions involved in the `Mmad` computation.
- When the `unitFlag` feature is enabled, the `FixpipeParams::unitFlag` of the result cube C move-out API must be set in coordination with the `MmadParams::unitFlag` parameter of the `Mmad` computation.
- When two consecutive `Mmad` operations accumulate along the K direction, attention must be paid to the write-read dependency of result cube C. For the threshold relationship of synchronization optimization, see the figure in [result cube C initialization](#result-cube-c-initialization).
- When M, K, and N are not multiples of 16, the hardware still organizes data in 16×16 fractal blocks. Invalid data in the tail block occupies fractal block space but does not participate in valid computation. The layout of valid and invalid data is shown in the following figure:

**Figure 3**  Valid data layout of the tail block

![Tail block valid data layout](../../../../figures/mmad_tail_valid_data.png)

## Key Feature Description

### Result Cube C Initialization

When no bias is passed, `ccubeInitVal` controls whether to initialize the result cube C. Typically, it is set to `true` for the first accumulation along the K direction and to `false` for subsequent K-block accumulations. If `Mmad` is set to `false` for the first call in a multi-K-block accumulation scenario, the C cube will contain residual data from the L0C Buffer, resulting in incorrect computation results.

When two consecutive `Mmad` calls accumulate along the K direction, the write-read dependency of intermediate results usually needs to be considered. The threshold relationship for synchronization optimization is shown in the following figure:

**Figure 4** PipeBarrier threshold relationship

![PipeBarrier threshold relationship](../../../../figures/mmad_pipebarrier_threshold.png)

### UnitFlag

`unitFlag` controls the fine-grained parallelism between `Mmad` and the subsequent `Fixpipe`. When enabled, `MmadParams::unitFlag` and `FixpipeParams::unitFlag` for result move-out must be set in coordination, with common values of `2` and `3`.

### GEMV Mode

When M=1, whether to enable GEMV mode is controlled by `MmadTrait::disableGemv`. The default value is `true`, which indicates that GEMV is disabled.

### MX Mmad Mode

The MX Mmad mode does not have a separate function API; instead, it is configured through `MmadTrait::mmadType`. To execute MX Mmad, set `MmadTrait::mmadType` to `MmadType::MX` and construct the cube computation atom object through `MakeMmad(MmadOperation{}, MmadTraitMX{})`.

```cpp
struct MmadTraitMX {
    using TraitType = MmadTrait;
    static constexpr const TraitType value = MmadTrait(0, false, false, true, MmadType::MX);
};
```

MX Mmad supports the following data type combinations for the left cube A, right cube B, and result cube C:

| Left Cube A | Right Cube B | Result Cube C |
| :--- | :--- | :--- |
| `fp4x2_e2m1_t` | `fp4x2_e2m1_t` | `float` |
| `fp4x2_e2m1_t` | `fp4x2_e1m2_t` | `float` |
| `fp4x2_e1m2_t` | `fp4x2_e2m1_t` | `float` |
| `fp4x2_e1m2_t` | `fp4x2_e1m2_t` | `float` |
| `fp8_e4m3fn_t` | `fp8_e4m3fn_t` | `float` |
| `fp8_e4m3fn_t` | `fp8_e5m2_t` | `float` |
| `fp8_e5m2_t` | `fp8_e4m3fn_t` | `float` |
| `fp8_e5m2_t` | `fp8_e5m2_t` | `float` |

MX Mmad typically requires the movement of the left cube scaling data ScaleA and the right cube scaling data ScaleB to be completed before calling `Mmad`. For ScaleA movement, see [L1-to-L0 ScaleA data movement](../cube_compute_load/Copy_L1ToL0ScaleA.md); for ScaleB movement, see [L1-to-L0 ScaleB data movement](../cube_compute_load/Copy_L1ToL0ScaleB.md). The cube scaling data is not passed as a parameter of the `Mmad` function; instead, it is moved in advance to the L0ScaleA Buffer and L0ScaleB Buffer through the `Copy` API.

## Related APIs

- [UnitFlag](../mmad_compute_key_features/UnitFlag.md)
- [GEMV](../mmad_compute_key_features/GEMV.md)
- [HF32](../mmad_compute_key_features/HF32.md)
- [L1 to L0 ScaleA Data Movement](../cube_compute_load/Copy_L1ToL0ScaleA.md)
- [L1 to L0 ScaleB Data Movement](../cube_compute_load/Copy_L1ToL0ScaleB.md)
- [L0C to GM Data Movement](../cube_compute_store/Copy_L0CToGM.md)
- [L0C to UB Data Movement](../cube_compute_store/Copy_L0CToUB.md)

## Example

- **Mmad** example

    ```cpp
    #include "tensor_api/tensor.h"

    __aicore__ inline void NormalMmadExample()
    {
        using namespace AscendC::Te;

        constexpr uint16_t m = 16;
        constexpr uint16_t n = 16;
        constexpr uint16_t k = 16;

        __ca__ half l0aBuf[m * k];
        __cb__ half l0bBuf[k * n];
        __cc__ float l0cBuf[m * n];

        auto l0A = MakeTensor(MakeMemPtr(l0aBuf), MakeFrameLayout<NZLayoutPtn, half>(m, k));
        auto l0B = MakeTensor(MakeMemPtr(l0bBuf), MakeFrameLayout<ZNLayoutPtn, half>(k, n));
        auto l0C = MakeTensor(MakeMemPtr(l0cBuf), MakeFrameLayout<NZLayoutPtn, 16>(m, n));

        MmadParams params(m, n, k, 0, true);
        auto atom = MakeMmad(MmadOperation{}, MmadTraitDefault{}).with(params);
        Mmad(atom, l0C, l0A, l0B);
    }
    ```

- **Mmad** example in the **MX** scenario

    ```cpp
    #include "tensor_api/tensor.h"

    struct MmadTraitMX {
        using TraitType = AscendC::Te::MmadTrait;
        static constexpr const TraitType value = AscendC::Te::MmadTrait(0, false, false, true, AscendC::Te::MmadType::MX);
    };

    __aicore__ inline void MxMmadExample()
    {
        using namespace AscendC::Te;

        constexpr uint16_t m = 16;
        constexpr uint16_t n = 16;
        constexpr uint16_t k = 16;

        __ca__ fp8_e5m2_t l0aBuf[m * k];
        __cb__ fp8_e5m2_t l0bBuf[k * n];
        __cc__ float l0cBuf[m * n];

        auto l0A = MakeTensor(MakeMemPtr(l0aBuf), MakeFrameLayout<NZLayoutPtn, fp8_e5m2_t>(m, k));
        auto l0B = MakeTensor(MakeMemPtr(l0bBuf), MakeFrameLayout<ZNLayoutPtn, fp8_e5m2_t>(k, n));
        auto l0C = MakeTensor(MakeMemPtr(l0cBuf), MakeFrameLayout<NZLayoutPtn, 16>(m, n));

        MmadParams params(m, n, k, 0, true);
        auto atom = MakeMmad(MmadOperation{}, MmadTraitMX{}).with(params);
        Mmad(atom, l0C, l0A, l0B);
    }
    ```
