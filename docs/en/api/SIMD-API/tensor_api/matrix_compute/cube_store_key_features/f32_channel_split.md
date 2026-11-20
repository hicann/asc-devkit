# F32 Channel Split

<!-- md-trans-meta sourceCommit=46a50d69bd092786e44495d519df57743eefbfc6 translatedAt=2026-10-27T03:05:15.634Z pushedAt=2026-10-27T13:03:35.344Z nl=2 -->

F32 Channel Split is a channel splitting capability used when moving cube computation results out. When both the source type and the destination type are `float`, the destination format is NZ, and `l0c_to_gm_trait::enable_channel_split` or `l0c_to_ub_trait::enable_channel_split` is set to `true`, the hardware splits a 16×16 fractal cube into two independent 16×8 small-Z fractal cubes.

After this capability is enabled, the C0 of the destination NZ output is processed as 8, and the size in the N direction must be a multiple of 8. As shown in the following figures, when the source operand shape is `[64, 32]`, the moved-out result is split into 16 independent 16×8 fractal cubes. When the source operand shape is `[64, 24]`, the moved-out result is split into 3 columns of 16×8 fractal cubes.

**Figure 1**  F32 Channel Split diagram 1

![F32 Channel Split diagram 1](../../../../figures/Fixpipe_Channel_Split.png)

**Figure 2**  F32 Channel Split diagram 2

![F32 Channel Split diagram 2](../../../../figures/Fixpipe_Channel_Split_with_dirty.png)

## Call Form

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

auto l0c = make_tensor(make_mem_ptr(l0c_addr), make_frame_layout<nz_layout_ptn, 16>(m, n));
auto gm = make_tensor(make_mem_ptr(gm_addr), make_frame_layout<nz_layout_ptn, float>(m, n));

constexpr l0c_to_gm_trait l0c_to_gm_trait_value = {
    round_mode::default_round,
    false,
    true
};

struct copy_l0c_to_gm_trait_custom {
    using trait_type = l0c_to_gm_trait;
    static constexpr const trait_type value = l0c_to_gm_trait_value;
};

l0c_to_gm_params params;
auto atom = make_copy(copy_l0c_to_gm{}, copy_l0c_to_gm_trait_custom{}).with(params);
copy(atom, gm, l0c);
```

## Constraints

- F32 Channel Split supports only `float` for both the source type and the destination type, and the destination format is NZ.
- When F32 Channel Split is enabled, `unit_flag` cannot be enabled at the same time, and the size of the N dimension must be a multiple of 8.
