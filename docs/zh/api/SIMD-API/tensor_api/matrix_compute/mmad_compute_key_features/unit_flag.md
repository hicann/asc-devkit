# unit_flag

## 特性说明

unit_flag的核心功能体现为：mmad和copy接口引入了单元标志（unit-flag）机制，通过以内存块为粒度实现精细化的数据同步，从而有效降低同步延迟，提升系统整体性能。当unit_flag开关打开后，对于L0C Buffer中的每个内存块（512B），提供一个单元标志位，用于指示该块是否可读或可写。

mmad和copy接口将unit_flag设置为`unit_flag_mode::enable_keep`或`unit_flag_mode::enable_update`后，系统会启动单元标志位。

当`params.unit_flag`为`unit_flag_mode::enable_keep`（底层值为2）时，使能unit_flag功能，在硬件执行完指令后，不改变单元标志位。

- 对于写操作（mmad接口），如果单元标志位0，则硬件直接写入L0C Buffer，否则，如果单元标志位为1，则写操作会等待直到单元标志变为0，执行完成后将单元标志位保持为0。
- 对于读操作（copy接口），如果单元标志位1，则硬件直接读取L0C Buffer，否则，如果单元标志位为0，则读操作为等待直到单元标志变为1，执行完成后将单元标志位保持为1。

当`params.unit_flag`为`unit_flag_mode::enable_update`（底层值为3）时，使能unit_flag功能，在硬件执行完指令后，改变单元标志位。

- 对于写操作（mmad接口），如果单元标志位0，则硬件直接写入L0C Buffer，否则，如果单元标志位为1，则写操作会等待直到单元标志变为0，执行完成后将单元标志位设置成1。
- 对于读操作（copy接口），如果单元标志位1，则硬件直接读取L0C Buffer，否则，如果单元标志位为0，则读操作为等待直到单元标志变为1，执行完成后将单元标志位设置成0。

根据上述特性，如果用户在进行A矩阵维度为128×1024、B矩阵维度为1024×128的矩阵乘计算时，需要沿着K轴进行迭代循环，假设每次迭代K长度为128，则需要迭代8次，此时8次mmad指令对应1次Fixpipe指令。

- 前7次mmad的unit_flag都设置为`unit_flag_mode::enable_keep`，写入后将单元标志位始终为0，保证后续mmad可以写入L0C Buffer。
- 最后1次mmad设置为`unit_flag_mode::enable_update`，写入后将单元标志位设置成1，保证Fixpipe可以读取L0C Buffer。
- Fixpipe的unit_flag设置为`unit_flag_mode::enable_update`，读取后将单元标志位设置为0，保证后续mmad接口可以顺利写入L0C Buffer数据。

如果用户需要单次mmad的结果分多次搬出时，譬如mmad计算结果的L0C Buffer为M\(128\) × N\(256\)，沿N轴分两次搬出，这样一次mmad会对应两次Fixpipe。

- mmad的时候需要将unit_flag设置为`unit_flag_mode::enable_update`，保证Fixpipe可以读取L0C Buffer数据。
- 每一次Fixpipe的unit_flag都设置为`unit_flag_mode::enable_update`，读取后将单元标志位设置为0，保证后续其他mmad接口在复用这块L0C Buffer地址时可以顺利写入数据。

当开启unit_flag后，mmad和Fixpipe会对同一块分形的L0C Buffer进行读写操作，因此mmad计算和Fixpipe保持一致的读写顺序，有助于获得更优的性能表现。

在调用mmad接口时，需要通过[asc_set_mmad_direction_m](../../../c_api/cube_compute/asc_set_mmad_direction_m.md)或[asc_set_mmad_direction_n](../../../c_api/cube_compute/asc_set_mmad_direction_n.md)接口设置mmad的计算方向。当Fixpipe使能了NZ2ND或ChannelMerge等layout变换时，需将mmad的计算方向设置为N方向优先，即调用`asc_set_mmad_direction_n`。反之，若未使用这些特性，则应将计算方向设置为M方向优先，即调用`asc_set_mmad_direction_m`。

**图1**  mmad和Fixpipe同时沿M方向写/读

![mmad-Fixpipe读写示意图](../../../../figures/mmad_unitflag.png)

## 特性约束

- mmad和copy接口均提供了unit_flag参数来控制该功能的启用，需确保两者同步开启，才能正常生效。
- 当希望控制同一块L0C Buffer内存空间能持续只被多条mmad或多条Fixpipe指令操作时，需将对应的前n-1条指令的unit_flag值设置为`unit_flag_mode::enable_keep`，维持被操作内存空间的持续占用状态，最后一条指令设置为`unit_flag_mode::enable_update`，解除被占用状态。
- 当启用unit_flag功能后，建议mmad的计算数据量与Fixpipe搬出的数据量保持一致。若mmad计算了大块数据（M × N = 128 × 128），但Fixpipe只搬出了其中一部分数据（M × N = 64 × 64），则可能会导致执行异常，可以通过`SetFixPipeConfig`接口重置L0C Buffer的状态。

## 相关接口

- [mmad](../mmad_compute/mmad.md)
- [copy（L0C到GM数据搬运）](../cube_compute_store/copy_l0c_to_gm.md)
- [copy（L0C到UB数据搬运）](../cube_compute_store/copy_l0c_to_ub.md)
- [asc_set_mmad_direction_m](../../../c_api/cube_compute/asc_set_mmad_direction_m.md)
- [asc_set_mmad_direction_n](../../../c_api/cube_compute/asc_set_mmad_direction_n.md)

## 沿K轴迭代循环使用示例片段

以下示例仅展示`unit_flag`在K轴迭代循环中的设置方式，省略张量构造和参数初始化。

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

// 调用k_round次mmad。
for (auto k_index = 0; k_index < k_round; ++k_index) {
    if (k_index != k_round - 1) {
        // 前k_round-1次迭代保持单元标志位，保证mmad在K迭代循环中可以一直写入L0C Buffer。
        params.unit_flag = unit_flag_mode::enable_keep;
    } else {
        // 最后一次迭代更新单元标志位，保证Fixpipe可以读L0C Buffer。
        params.unit_flag = unit_flag_mode::enable_update;
    }
    auto mmad_atom = make_mmad(mmad_operation{}).with(params);
    mmad(mmad_atom, l0c, l0a, l0b);
}
// Fixpipe一次搬出。
// Fixpipe更新单元标志位，保证后续mmad接口可以顺利写入L0C Buffer数据。
l0c_to_gm_params fixpipe_params;
fixpipe_params.unit_flag = unit_flag_mode::enable_update;
auto fixpipe_atom = make_copy(copy_l0c_to_gm{}).with(fixpipe_params);
copy(fixpipe_atom, gm, l0c);

```

**表1**  性能示例说明（以输入数据类型half为例，无业务实测表示忽略指令的前后序操作，只考虑单指令性能，包含带宽延迟等开销）

| M | N | K | unit_flag | LOOP_COUNT | mmad理论值（cycle） | mmad无业务实测值（cycle） |
| --- | --- | --- | --- | --- | --- | --- |
| 128 | 256 | 512 | Enable | 8 | 4156 | 4209 |
| 128 | 256 | 512 | Disable | 8 | 4156 | 4225 |
| 128 | 128 | 512 | Enable | 8 | 2108 | 2172 |
| 128 | 128 | 512 | Disable | 8 | 2108 | 2172 |
