# In-line ReLU

<!-- md-trans-meta sourceCommit=46a50d69bd092786e44495d519df57743eefbfc6 translatedAt=2026-10-27T03:07:35.024Z pushedAt=2026-10-27T13:03:35.351Z nl=2 -->

In-line ReLU is an in-line activation capability applied when the cube computation result is moved out from the L0C Buffer to Global Memory or the Unified Buffer (UB). The Tensor API controls whether to enable this capability through `l0c_to_gm_trait::enable_relu` or `l0c_to_ub_trait::enable_relu`.

## Description

The default value of `enable_relu` is `false`, indicating that in-line ReLU is disabled. When set to `true`, Normal ReLU activation is performed on the output data during the move-out process.

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

constexpr l0c_to_gm_trait l0c_to_gm_trait_value = {
    round_mode::default_round,
    true,
    false
};

struct copy_l0c_to_gm_trait_custom {
    using trait_type = l0c_to_gm_trait;
    static constexpr const trait_type value = l0c_to_gm_trait_value;
};

l0c_to_gm_params params;
auto atom = make_copy(copy_l0c_to_gm{}, copy_l0c_to_gm_trait_custom{}).with(params);
copy(atom, gm, l0c);
```

The move from the L0C Buffer to the UB is likewise controlled by `l0c_to_ub_trait::enable_relu`:

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

constexpr l0c_to_ub_trait l0c_to_ub_trait_value = {
    round_mode::default_round,
    true,
    false,
    dual_dst_mode::disable
};

struct copy_l0c_to_ub_trait_custom {
    using trait_type = l0c_to_ub_trait;
    static constexpr const trait_type value = l0c_to_ub_trait_value;
};

l0c_to_ub_params params;
auto atom = make_copy(copy_l0c_to_ub{}, copy_l0c_to_ub_trait_custom{}).with(params);
copy(atom, ub, l0c);
```

## Support Scope

Based on the capabilities of the underlying Fixpipe, in-line ReLU includes modes such as Normal ReLU, Leaky ReLU, and PRelu. Currently, the L0C Buffer-to-Global Memory/UB movement in the Tensor API exposes the Normal ReLU capability through `enable_relu`, and in-line PRelu is not supported.

## Constraints

- `enable_relu` is a compile-time trait configuration and is disabled by default.
- The quantization and ReLU parameters cannot be `inf`, `nan`, or denormal numbers.
- The Tensor API does not support in-line PRelu.
