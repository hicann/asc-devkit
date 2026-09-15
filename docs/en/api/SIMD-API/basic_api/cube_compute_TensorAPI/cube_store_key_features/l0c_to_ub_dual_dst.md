# L0C Buffer to Unified Buffer Dual-Target Mode

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T17:09:59.352Z -->

The L0C Buffer to Unified Buffer dual-target mode is used to split the cube result in the L0C Buffer within the same AI Core and write the two parts to the Unified Buffers of two Vector Cores simultaneously. An AI Core contains one Cube Core and two Vector Cores. When the dual-target mode is enabled, the source cube is split into two parts along a specified dimension, with the first half written to SUB BLOCK0 and the second half written to SUB BLOCK1.

The Tensor API controls the dual-target mode through `CopyL0C2UBTrait::dualDstCtl`.

The values of `dualDstCtl` are as follows:

|Value|Description|
|--------|--------|
|`DUAL_DST_DISABLE`|Single-target mode. The entire cube is written to the default target Unified Buffer.|
|`DUAL_DST_SPLIT_M`|Dual-target mode, split along the M dimension. The source cube is split into two matrices of shape `M / 2 * N`, which are written to the two Unified Buffers respectively.|
|`DUAL_DST_SPLIT_N`|Dual-target mode, split along the N dimension. The source cube is split into two matrices of shape `M * N / 2`, which are written to the two Unified Buffers respectively.|

## Example

```cpp
constexpr CopyL0C2UBTrait l0c2ubTrait = {
    RoundMode::DEFAULT,
    false,
    false,
    DUAL_DST_SPLIT_M
};

struct CopyL0C2UBTraitCustom {
    using TraitType = CopyL0C2UBTrait;
    static constexpr const TraitType value = l0c2ubTrait;
};

FixpipeParams params;
auto atom = MakeCopy(CopyL0C2UB{}, CopyL0C2UBTraitCustom{}).with(params);
Copy(atom, ub, l0c);
```

## NZ2NZ Dual-Target Mode

In the NZ2NZ copy scenario, the dual-target mode supports splitting along the M dimension and splitting along the N dimension. Combined with the underlying copy API, the description is as follows:

Example of splitting along the N dimension:

- `nSize = 32`, indicating that the size of the cube to be copied in the source NZ cube along the N dimension is 32 elements.
- `mSize = 48`, indicating that the size of the cube to be copied in the source NZ cube along the M dimension is 48 elements.
- `srcStride = 64`, indicating that the starting address offset between adjacent Z layouts in the source NZ cube is `64 * C0_SIZE`.
- `dstStride = 64 * C0`, indicating that the starting address offset between adjacent Z layouts in the destination NZ cube is `64 * 16` elements.

Example of splitting along the M dimension:

- `nSize = 32`, indicating that the size of the cube to be copied in the source NZ cube along the N dimension is 32 elements.
- `mSize = 24`, indicating the size along the M dimension received by each destination Unified Buffer, where the total size of the source cube along the M dimension is 48 elements.
- `srcStride = 64`, indicating that the start address offset between adjacent Z layouts in the source NZ cube is `64 * C0_SIZE`.
- `dstStride = 40 * C0`, indicating that the start address offset between adjacent Z layouts in the destination NZ cube is `40 * 16` elements.

**Figure 1**  NZ2NZ dual-target copy mode setting diagram

![NZ2NZ dual-target copy mode setting diagram](../../../../figures/nz2nz_dual_target_movement_mode_setting.png)

## NZ2ND Dual-Target Mode

In the NZ2ND copy scenario, the dual-target mode also supports splitting by the M dimension and by the N dimension. The underlying copy APIs are described as follows:

- `ndNum = 2`, indicating that the number of source NZ matrices is 2.
- `nSize = 32`, indicating that the size of the source NZ cube in the N direction is 32 elements.
- `mSize = 48`, indicating that the size of the source NZ cube in the M direction is 48 elements.
- `srcStride = 64`, indicating that the start address offset between adjacent Z layouts in the source NZ cube is `64 * C0_SIZE`.
- `dstStride = 64`, indicating that the number of elements in each row of the destination ND cube is 64.
- `srcNdStride = 240`, indicating that the interval between the start addresses of different NZ matrices is `240 * C0_SIZE`.
- `dstNdStride = 4096`, indicating that the offset between the start addresses of adjacent destination ND matrices is 4096 elements.

**Figure 2**  NZ2ND dual-target copy mode setting

![NZ2ND dual-target copy mode setting diagram](../../../../figures/enable_nz2nd_param_dual_in_mode_setting.png)

## Constraints

- The dual-target mode supports only the L0C Buffer to Unified Buffer path.
- The dual-target mode supports the NZ2NZ and NZ2ND copy scenarios in normal copy mode.
- When splitting along the M dimension, M must be a multiple of 2.
- When splitting along the N dimension, N must be a multiple of 32.
