# 随路Relu

随路Relu是矩阵计算结果从L0C Buffer搬出到Global Memory或Unified Buffer（UB）过程中的随路激活能力。Tensor API通过`l0c_to_gm_trait::enable_relu`或`l0c_to_ub_trait::enable_relu`控制是否使能该能力。

## 功能说明

`enable_relu`默认值为`false`，表示不使能随路Relu。设置为`true`时，搬出过程中对输出数据执行Normal Relu激活。

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

L0C Buffer到UB搬运同样通过`l0c_to_ub_trait::enable_relu`控制：

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

## 支持范围

根据底层Fixpipe的能力，随路Relu包含Normal Relu、Leaky Relu和PRelu等模式。当前Tensor API的L0C Buffer到Global Memory/UB搬运通过`enable_relu`暴露Normal Relu能力，当前不支持随路PRelu。

## 约束说明

- `enable_relu`为编译期trait配置，默认不使能。
- 量化和Relu参数不能为`inf`、`nan`或非规格化数。
- 当前Tensor API不支持随路PRelu。
