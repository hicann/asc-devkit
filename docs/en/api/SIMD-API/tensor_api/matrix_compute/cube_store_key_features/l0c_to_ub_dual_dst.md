# L0C Buffer to Unified Buffer Dual-Target Mode

<!-- md-trans-meta sourceCommit=46a50d69bd092786e44495d519df57743eefbfc6 translatedAt=2026-10-27T03:06:35.803Z pushedAt=2026-10-27T13:03:35.348Z nl=2 -->

The L0C Buffer to Unified Buffer (UB) dual-target mode is used to split the cube result in the L0C Buffer within the same AI Core and write it to the UBs of two Vector Cores simultaneously. An AI Core contains one Cube Core and two Vector Cores. When the dual-target mode is enabled, the source cube is split into two parts along the specified dimension, with the first half written to SUB BLOCK0 and the second half written to SUB BLOCK1.

The Tensor API controls the dual-target mode through `l0c_to_ub_trait::dual_dst_ctl`.

The values of `dual_dst_ctl` are as follows:

**Table 1**  Description of dual_dst_mode values

|Value|Description|
|--------|--------|
|`dual_dst_mode::disable`|Single-target mode. The entire cube is written to the default target UB.|
|`dual_dst_mode::split_m`|Dual-target mode, split along the M dimension. The source cube is split into two cubes of shape `M / 2 * N`, which are written to the two UBs respectively.|
|`dual_dst_mode::split_n`|Dual-target mode, split along the N dimension. The source cube is split into two cubes of shape `M * N / 2`, which are written to the two UBs respectively.|

## Example

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

constexpr l0c_to_ub_trait l0c_to_ub_trait_value = {
    round_mode::default_round,
    false,
    false,
    dual_dst_mode::split_m
};

struct copy_l0c_to_ub_trait_custom {
    using trait_type = l0c_to_ub_trait;
    static constexpr const trait_type value = l0c_to_ub_trait_value;
};

l0c_to_ub_params params;
auto atom = make_copy(copy_l0c_to_ub{}, copy_l0c_to_ub_trait_custom{}).with(params);
copy(atom, ub, l0c);
```

## NZ2NZ Dual-Target Mode

In the NZ2NZ movement scenario, the dual-target mode supports splitting by the M dimension and splitting by the N dimension. Combined with the underlying copy API, the details are described as follows:

N-direction splitting example:

- `n_size = 32`, indicating that the size of the cube to be copied in the source NZ cube in the N direction is 32 elements.
- `m_size = 48`, indicating that the size of the cube to be copied in the source NZ cube in the M direction is 48 elements.
- `src_stride = 64`, indicating that the start address offset of adjacent Z layouts in the source NZ cube is `64 * c0_size`.
- `dst_stride = 64 * C0`, indicating that the start address offset of adjacent Z layouts in the destination NZ cube is `64 * 16` elements.

M-direction splitting example:

- `n_size = 32`, indicating that the size of the cube to be copied in the source NZ cube in the N direction is 32 elements.
- `m_size = 24`, indicating the M-direction size received by each target UB. The total M-direction size of the source cube is 48 elements.
- `src_stride = 64`, indicating that the start address offset of adjacent Z layouts in the source NZ cube is `64 * c0_size`.
- `dst_stride = 40 * C0`, indicating that the start address offset between adjacent Z layouts in the destination NZ cube is `40 * 16` elements.

**Figure 1**  NZ2NZ dual-target movement mode setting diagram

![NZ2NZ dual-target copy mode setting diagram](../../../../figures/nz2nz_dual_target_movement_mode_setting.png)

## NZ2ND Dual-Target Mode

In the NZ2ND movement scenario, the dual-target mode also supports splitting by the M dimension and by the N dimension. The underlying copy APIs are described as follows:

- `nd_num = 2`, indicating that the number of source NZ cubes is 2.
- `n_size = 32`, indicating that the size of the source NZ cube in the N direction is 32 elements.
- `m_size = 48`, indicating that the size of the source NZ cube in the M direction is 48 elements.
- `src_stride = 64`, indicating that the start address offset of adjacent Z layouts in the source NZ cube is `64 * c0_size`.
- `dst_stride = 64`, indicating that the number of elements in each row of the destination ND cube is 64.
- `src_nd_stride = 240`, indicating that the interval between the start addresses of different NZ cubes is `240 * c0_size`.
- `dst_nd_stride = 4096`, indicating that the offset between the start addresses of adjacent destination ND cubes is 4096 elements.

**Figure 2** NZ2ND dual-target movement mode setting diagram

![NZ2ND dual-target copy mode setting diagram](../../../../figures/enable_nz2nd_param_dual_in_mode_setting.png)

## Constraints

- The dual-target mode supports only the L0C Buffer-to-UB path.
- The dual-target mode supports the NZ2NZ and NZ2ND movement scenarios in normal movement mode.
- When splitting along the M dimension, M must be a multiple of 2.
- When splitting along the N dimension, N must be a multiple of 32.
