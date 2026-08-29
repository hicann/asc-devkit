# L0C Buffer到Unified Buffer双目标模式

L0C Buffer到Unified Buffer（UB）双目标模式用于将同一AI Core内L0C Buffer的矩阵结果拆分后，同时写入两个Vector Core各自的UB。一个AI Core内包含一个Cube Core和两个Vector Core，启用双目标模式后，源矩阵会按指定维度拆分为两部分，前半部分写入SUB BLOCK0，后半部分写入SUB BLOCK1。

Tensor API通过`l0c_to_ub_trait::dual_dst_ctl`控制双目标模式。

`dual_dst_ctl`取值如下：

**表1**  dual_dst_mode取值说明

|取值|说明|
|--------|--------|
|`dual_dst_mode::disable`|单目标模式。整个矩阵写入默认目标UB。|
|`dual_dst_mode::split_m`|双目标模式，按M维度拆分。源矩阵拆分为两个形状为`M / 2 * N`的矩阵，分别写入两个UB。|
|`dual_dst_mode::split_n`|双目标模式，按N维度拆分。源矩阵拆分为两个形状为`M * N / 2`的矩阵，分别写入两个UB。|

## 使用示例

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

## NZ2NZ双目标模式

在NZ2NZ搬运场景下，双目标模式支持按M维度拆分和按N维度拆分。结合底层搬运接口说明如下：

N方向切分示例：

- `n_size = 32`，表示源NZ矩阵中待搬运矩阵在N方向上的大小为32个元素。
- `m_size = 48`，表示源NZ矩阵中待搬运矩阵在M方向上的大小为48个元素。
- `src_stride = 64`，表示源NZ矩阵中相邻Z排布的起始地址偏移为`64 * c0_size`。
- `dst_stride = 64 * C0`，表示目的NZ矩阵中相邻Z排布的起始地址偏移为`64 * 16`个元素。

M方向切分示例：

- `n_size = 32`，表示源NZ矩阵中待搬运矩阵在N方向上的大小为32个元素。
- `m_size = 24`，表示每个目标UB接收的M方向大小，源矩阵M方向总大小为48个元素。
- `src_stride = 64`，表示源NZ矩阵中相邻Z排布的起始地址偏移为`64 * c0_size`。
- `dst_stride = 40 * C0`，表示目的NZ矩阵中相邻Z排布的起始地址偏移为`40 * 16`个元素。

**图1**  NZ2NZ双目标搬运模式设置示意图

![NZ2NZ双目标搬运模式设置示意图](../../../../figures/nz2nz_dual_target_movement_mode_setting.png)

## NZ2ND双目标模式

在NZ2ND搬运场景下，双目标模式也支持按M维度拆分和按N维度拆分。结合底层搬运接口说明如下：

- `nd_num = 2`，表示源NZ矩阵的数目为2。
- `n_size = 32`，表示源NZ矩阵在N方向上的大小为32个元素。
- `m_size = 48`，表示源NZ矩阵在M方向上的大小为48个元素。
- `src_stride = 64`，表示源NZ矩阵中相邻Z排布的起始地址偏移为`64 * c0_size`。
- `dst_stride = 64`，表示目的ND矩阵每一行中的元素个数为64。
- `src_nd_stride = 240`，表示不同NZ矩阵起始地址之间的间隔为`240 * c0_size`。
- `dst_nd_stride = 4096`，表示目的相邻ND矩阵起始地址之间的偏移为4096个元素。

**图2**  NZ2ND双目标搬运模式设置示意图

![NZ2ND双目标搬运模式设置示意图](../../../../figures/enable_nz2nd_param_dual_in_mode_setting.png)

## 约束说明

- 双目标模式仅支持L0C Buffer到UB通路。
- 双目标模式支持普通搬运模式NZ2NZ和NZ2ND搬运场景。
- 按M维度拆分时，M必须为2的倍数。
- 按N维度拆分时，N必须为32的倍数。
