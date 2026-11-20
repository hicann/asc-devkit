# L1 Buffer, L0A Buffer/L0B Buffer Memory Structure Introduction<a name="ZH-CN_TOPIC_0000002538231122"></a>

<!-- md-trans-meta sourceCommit=7f211052f814e6d340cc6bf2dfc44c8393b5424f translatedAt=2026-11-02T03:15:45.540Z pushedAt=2026-11-18T01:25:06.703Z nl=2 -->

To improve data access efficiency and throughput, the Buffer adopts a memory module (Bank) structure design with equal capacity. The memory space is divided into multiple Banks, which are further grouped by Bank as a unit to form multiple Bank Groups (BG). Each Bank Group contains several Banks, and each Bank is an independently operable storage unit submodule.

The following describes the detailed structure of the Cube-side memories (L1 Buffer, L0A Buffer, L0B Buffer).

## L1 Buffer Memory Structure<a name="zh-cn_topic_0000002545385271_section133445171205"></a>

The total capacity of the L1 Buffer is 512 KB, consisting of 16 Banks. Each Bank has a capacity of 32 KB, consisting of 1024 rows, with each row being 32 bytes wide. **A single Bank allows at most one read or one write at the same time**.

These 16 Banks are further organized into 8 Bank Groups. Each Bank Group contains 2 Banks (Bank0 and Bank1). **Two Banks belonging to the same Bank Group support one Bank reading while the other writes, but do not support simultaneous reads or simultaneous writes**.

The memory structure of the L1 Buffer is shown in [Figure 1](#zh-cn_topic_0000002545385271_fig5383229131020), and the address encoding format is as follows:

```cpp
L1_ADDR[18:0] = {BANK[0:0], BANK_DEPTH[9:0], BG[2:0], BANK_WIDTH[4:0]}    // Bits are ordered from most significant to least significant, 19 bits in total.
// BANK indicates the number of the bank where the address resides, occupying 1 bit, with a value range of [0, 1].
// BANK_DEPTH indicates the row number of the address in the bank, occupying 10 bits, with a value range of [0, 1023].
// BG indicates the number of the bank group where the address resides, occupying 3 bits, with a value range of [0, 7].
// BANK_WIDTH indicates the offset of the address within a bank row, occupying 5 bits, with a value range of [0, 31].
```

**Figure 1** L1 Buffer memory structure diagram<a name="zh-cn_topic_0000002545385271_fig5383229131020"></a>  
![](../../../../figures/l1_memory_bank.png "L1 memory structure diagram")

## L0A Buffer/L0B Buffer Memory Structure<a name="zh-cn_topic_0000002545385271_section6770135142516"></a>

The total capacity of L0A Buffer/L0B Buffer is 64 KB, containing only one Bank with 128 rows, and each row is 512 bytes wide. **Simultaneous read and write to the same address is not allowed. LoadData is allowed to write to and Mmad is allowed to read from different addresses of the same Bank at the same time.**

The memory structure of L0A Buffer/L0B Buffer is shown in [Figure 2](#zh-cn_topic_0000002545385271_fig2046019020496). L0A Buffer and L0B Buffer use the same address encoding format:

```cpp
L0A_ADDR[15:0] = { BANK_DEPTH[6:0], BANK_WIDTH[8:0] }    // Bits are ordered from most significant to least significant, 16 bits in total.
L0B_ADDR[15:0] = { BANK_DEPTH[6:0], BANK_WIDTH[8:0] }
// BANK_DEPTH indicates the row number of the address in the bank, occupying 7 bits, with a value range of [0, 127].
// BANK_WIDTH indicates the offset of the address within a bank row, occupying 9 bits, with a value range of [0, 511].
```

**Figure 2** L0A Buffer/L0B Buffer memory structure diagram<a name="zh-cn_topic_0000002545385271_fig2046019020496"></a>  
![](../../../../figures/l0al0b_memory_bank.png "L0A-B memory structure diagram")
