# UnitFlag

## 特性说明

unitFlag的核心功能体现为：Mmad和Copy接口引入了单元标志（unit-flag）机制，通过以内存块为粒度实现精细化的数据同步，从而有效降低同步延迟，提升系统整体性能。当UnitFlag开关打开后，对于L0C Buffer中的每个内存块（512B），提供一个单元标志位，用于指示该块是否可读或可写。

Mmad和Copy接口设置unitFlag值为2/3后，系统会启动单元标志位。

当`mmadParams.unitFlag`为`2（0b10）`时，使能unitFlag功能，在硬件执行完指令后，不改变单元标志位。

- 对于写操作（Mmad接口），如果单元标志位0，则硬件直接写入L0C Buffer，否则，如果单元标志位为1，则写操作会等待直到单元标志变为0，执行完成后将单元标志位保持为0。
- 对于读操作（Copy接口），如果单元标志位1，则硬件直接读取L0C Buffer，否则，如果单元标志位为0，则读操作为等待直到单元标志变为1，执行完成后将单元标志位保持为1。

当`mmadParams.unitFlag`为`3（0b11）`时，使能unitFlag功能，在硬件执行完指令后，改变单元标志位。

- 对于写操作（Mmad接口），如果单元标志位0，则硬件直接写入L0C Buffer，否则，如果单元标志位为1，则写操作会等待直到单元标志变为0，执行完成后将单元标志位设置成1。
- 对于读操作（Copy接口），如果单元标志位1，则硬件直接读取L0C Buffer，否则，如果单元标志位为0，则读操作为等待直到单元标志变为1，执行完成后将单元标志位设置成0。

根据上述特性，如果用户在进行A矩阵维度为128×1024、B矩阵维度为1024×128的矩阵乘计算时，需要沿着K轴进行迭代循环，假设每次迭代K长度为128，则需要迭代8次，此时8次Mmad指令对应1次Fixpipe指令。

- 前7次Mmad的unitFlag都设置成2，写入后将单元标志位始终为0，保证后续Mmad可以写入L0C Buffer。
- 最后1次Mmad设置成3，写入后将单元标志位设置成1，保证Fixpipe可以读取L0C Buffer。
- Fixpipe的unitFlag设置为3，读取后将单元标志位设置为0，保证后续Mmad接口可以顺利写入L0C Buffer数据。

如果用户需要单次Mmad的结果分多次搬出时，譬如Mmad计算结果的L0C Buffer为M\(128\) × N\(256\)，沿N轴分两次搬出，这样一次Mmad会对应两次Fixpipe。

- Mmad的时候需要设置unitFlag = 3，保证Fixpipe可以读取L0C Buffer数据。
- 每一次Fixpipe的unitFlag都设置为3，读取后将单元标志位设置为0，保证后续其他Mmad接口在复用这块L0C Buffer地址时可以顺利写入数据。

当开启unitFlag后，Mmad和Fixpipe会对同一块分形的L0C Buffer进行读写操作，因此Mmad计算和Fixpipe保持一致的读写顺序，有助于获得更优的性能表现。

在调用Mmad接口时，需要通过[SetMMColumnMajor/SetMMRowMajor](../../矩阵计算（ISASI）/Mmad计算辅助配置接口/SetMMColumnMajor-SetMMRowMajor.md)接口设置Mmad的计算方向。当Fixpipe使能了NZ2ND或ChannelMerge等layout变换时，需将Mmad的计算方向设置为N方向优先，即调用`SetMMRowMajor`。反之，若未使用这些特性，则应将计算方向设置为M方向优先，即调用`SetMMColumnMajor`。

**图1**  Mmad和Fixpipe同时沿M方向写/读

![Mmad-Fixpipe读写示意图](../../../../figures/mmad_unitflag.png)

## 特性约束

- Mmad和Copy接口均提供了unitFlag参数来控制该功能的启用，需确保两者同步开启，才能正常生效。
- 当希望控制同一块L0C Buffer内存空间能持续只被多条Mmad或多条Fixpipe指令操作时，需将对应的前n-1条指令的unitFlag值设置为2，维持被操作内存空间的持续占用状态，最后一条指令设置为3，解除被占用状态。
- 当启用unitFlag功能后，建议Mmad的计算数据量与Fixpipe搬出的数据量保持一致。若Mmad计算了大块数据（M × N = 128 × 128），但Fixpipe只搬出了其中一部分数据（M × N = 64 × 64），则可能会导致执行异常，可以通过`SetFixPipeConfig`接口重置L0C Buffer的状态。

## 相关接口

- [Mmad](../Mmad计算/Mmad.md)
- [Copy（L0C到GM数据搬运）](../Copy（L0C到GM数据搬运）.md/../矩阵计算的搬出/Copy（L0C到GM数据搬运）.md)
- [Copy（L0C到UB数据搬运）](../矩阵计算的搬出/Copy（L0C到UB数据搬运）.md)
- [SetFixPipeConfig](../../矩阵计算（ISASI）/矩阵搬出辅助配置接口/SetFixPipeConfig.md)
- [SetMMColumnMajor/SetMMRowMajor](../../矩阵计算（ISASI）/Mmad计算辅助配置接口/SetMMColumnMajor-SetMMRowMajor.md)

## 沿K轴迭代循环使用示例片段

以下示例仅展示`unitFlag`在K轴迭代循环中的设置方式，省略张量构造和参数初始化。

```cpp
using namespace AscendC::Te;

// 调用kRound次Mmad。
for (auto kIndex = 0; kIndex < kRound; ++kIndex) {
    if (kIndex != kRound - 1) {
        // 前kRound-1次迭代设置为2，保证Mmad在K迭代循环中可以一直写入L0C Buffer。
        mmadParams.unitFlag = 2;
    } else {
        // 最后一次迭代设置为3，将单元标志位设成1，保证Fixpipe可以读L0C Buffer。
        mmadParams.unitFlag = 3;
    }
    auto mmadAtom = MakeMmad(MmadOperation{}).with(mmadParams);
    Mmad(mmadAtom, l0C, l0A, l0B);
}
// Fixpipe一次搬出。
// Fixpipe的unitFlag设置为3，读取后将单元标志位设置为0，保证后续Mmad接口可以顺利写入L0C Buffer数据。
FixpipeParams fixpipeParams;
fixpipeParams.unitFlag = 3;
auto fixpipeAtom = MakeCopy(CopyL0C2GM{}).with(fixpipeParams);
Copy(fixpipeAtom, gm, l0C);

```

**表1**  性能示例说明（以输入数据类型half为例，无业务实测表示忽略指令的前后序操作，只考虑单指令性能，包含带宽延迟等开销）

| M | N | K | UnitFlag | LOOP_COUNT | Mmad理论值（cycle） | Mmad无业务实测值（cycle） |
| --- | --- | --- | --- | --- | --- | --- |
| 128 | 256 | 512 | Enable | 8 | 4156 | 4209 |
| 128 | 256 | 512 | Disable | 8 | 4156 | 4225 |
| 128 | 128 | 512 | Enable | 8 | 2108 | 2172 |
| 128 | 128 | 512 | Disable | 8 | 2108 | 2172 |
