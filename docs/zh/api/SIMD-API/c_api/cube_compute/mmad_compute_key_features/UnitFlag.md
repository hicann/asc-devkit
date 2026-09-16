# UnitFlag

**特性说明：**

unitFlag的核心功能体现为：asc_mmad和L0C Buffer搬出接口引入了单元标志（unit-flag）机制，通过以内存块为粒度实现精细化的数据同步，从而有效降低同步延迟，提升系统整体性能。当UnitFlag开关打开后，对于L0C Buffer中的内存块，硬件提供单元标志位指示该块是否可读或可写，内存块的具体大小由硬件行为控制，用户无需感知。

asc_mmad通过`asc_unit_flag_mode::ENABLE_KEEP`或`ENABLE_UPDATE`启用单元标志。

当`unit_flag_mode == asc_unit_flag_mode::ENABLE_KEEP`，开启unitFlag功能，在硬件执行完指令后，不改变单元标志位；

- 对于写操作（asc_mmad接口），如果单元标志位0，则硬件直接写入L0C Buffer；否则，如果单元标志位为1，则写操作会等待直到单元标志变为0；执行完成后将单元标志位保持为0；
- 对于读操作（L0C Buffer搬出接口），如果单元标志位1，则硬件直接读取L0C Buffer；否则，如果单元标志位为0，则读操作会等待直到单元标志变为1；执行完成后将单元标志位保持为1；

当`unit_flag_mode == asc_unit_flag_mode::ENABLE_UPDATE`，开启unitFlag功能，在硬件执行完指令后，改变单元标志位；

- 对于写操作（asc_mmad接口），如果单元标志位0，则硬件直接写入L0C Buffer；否则，如果单元标志位为1，则写操作会等待直到单元标志变为0；执行完成后将单元标志位设置成1；
- 对于读操作（L0C Buffer搬出接口），如果单元标志位1，则硬件直接读取L0C Buffer；否则，如果单元标志位为0，则读操作会等待直到单元标志变为1；执行完成后将单元标志位设置成0；

根据上述特性，如果用户在进行A矩阵\[128, 1024\]、B矩阵为\[1024, 128\]的矩阵乘计算时，需要沿着K轴进行迭代循环，假设每次迭代K长度为128，则需要迭代8次，此时8次asc_mmad对应1次搬出操作；

- 前7次asc_mmad都设置成`asc_unit_flag_mode::ENABLE_KEEP`，写入后将单元标记位始终为0，保证后续asc_mmad可以写入L0C Buffer；
- 最后1次asc_mmad设置成`asc_unit_flag_mode::ENABLE_UPDATE`，写入后将单元标志位设置成1，保证Fixpipe可以读取L0C Buffer；
- 搬出接口设置为`asc_unit_flag_mode::ENABLE_UPDATE`，读取后将单元标记位设置为0，保证后续asc_mmad接口可以顺利写入L0C Buffer数据；

如果用户需要单次asc_mmad的结果分多次搬出时，譬如asc_mmad计算结果的L0C Buffer为M\(128\) x N\(256\)，沿N轴分两次搬出；这样一次asc_mmad会对应两次搬出操作；

- asc_mmad的时候需要设置`asc_unit_flag_mode::ENABLE_UPDATE`，保证搬出时可以读取L0C Buffer数据；
- 每一次搬出接口都设置为`asc_unit_flag_mode::ENABLE_UPDATE`，读取后将单元标记位设置为0，保证后续其他asc_mmad接口在复用这块L0C Buffer地址时可以顺利写入数据；

当开启unitFlag后，asc_mmad和L0C Buffer搬出接口会对同一块分形的L0C Buffer进行读写操作，因此asc_mmad计算和L0C Buffer搬出接口保持一致的读写顺序，有助于获得更优的性能表现。

在调用asc_mmad接口时，通过[asc_set_mmad_direction_m](../asc_set_mmad_direction_m.md)或[asc_set_mmad_direction_n](../asc_set_mmad_direction_n.md)配置计算方向，计算方向与推荐场景的具体说明请参考对应接口文档。

**图1** asc_mmad和L0C Buffer搬出接口同时沿M方向写/读

![asc_mmad和L0C Buffer搬出接口同时沿M方向写-读](../../../../figures/mmad_unitflag.png "asc_mmad和L0C Buffer搬出接口同时沿M方向写-读")

**特性约束：**

- asc_mmad和L0C Buffer搬出接口均提供了UnitFlag控制参数来控制该功能的启用，需确保两者同步开启，才能正常生效。
- 当希望控制同一块L0C Buffer内存空间能持续只被多条asc_mmad或多条搬出指令操作时，需将对应的前n-1条指令的unit_flag_mode设置为`asc_unit_flag_mode::ENABLE_KEEP`，维持被操作内存空间的持续占用状态，最后一条指令设置为`asc_unit_flag_mode::ENABLE_UPDATE`，解除被占用状态。
- 当启用unitFlag功能后，建议asc_mmad的计算数据量与搬出的数据量保持一致。若asc_mmad计算了大块数据（M × N = 128 × 128），但只搬出了其中一部分数据（M × N = 64 × 64），则可能会导致执行异常，可以通过`asc_set_l0c_copy_config`接口重置L0C Buffer的状态。

**沿K轴迭代循环使用示例片段：**

```cpp
asc_set_mmad_direction_n();
// 调用k_round次asc_mmad
for (uint16_t i = 0; i < k_round; ++i) {
    // 前k_round-1次迭代设置为ENABLE_KEEP，保证asc_mmad在K迭代循环中可以一直写入L0C Buffer
    // 最后一次迭代设置为ENABLE_UPDATE，保证asc_copy_l0c2gm可以读取L0C Buffer
    const auto flag = (i + 1 == k_round)
        ? asc_unit_flag_mode::ENABLE_UPDATE
        : asc_unit_flag_mode::ENABLE_KEEP;
    asc_mmad(c, a, b, m, k_tile, n, flag, false, false, i == 0 /* 仅首次清零 */);
    if ((m / 16) * (n / 16) < 10) {
        asc_sync_pipe(PIPE_M);
    }
}

asc_set_l0c_copy_nz_para(1, 0, 0);
asc_copy_l0c2gm(c_gm, c, n, m, n, m,
    asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM,
    asc_unit_flag_mode::ENABLE_UPDATE, asc_quant_mode::NoQuant,
    asc_relu_pre_mode::NONE, false, true, false, false);
```
