# L1 Buffer, L0A Buffer/L0B Buffer Memory Structure<a name="ZH-CN_TOPIC_0000002538231122"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T16:23:57.451Z -->

To improve data access efficiency and throughput, the Buffer adopts a structure design of equal-capacity memory modules (**Bank**). The memory space is divided into multiple **Bank**s, which are further grouped by **Bank** into multiple **Bank Group**s (**BG**). Each **Bank Group** contains several **Bank**s, and each **Bank** is an independently operable storage unit submodule.

The following describes the detailed structure of the Cube-side related memory (**L1 Buffer**, **L0A Buffer**, and **L0B Buffer**).

## L1 Buffer Memory Structure<a name="zh-cn_topic_0000002545385271_section133445171205"></a>

The total capacity of **L1 Buffer** is 512 KB, consisting of 16 **Bank**s. Each **Bank** has a capacity of 32 KB, consisting of 1024 rows, with each row being 32 bytes wide. **A single Bank allows at most one read or one write at a time.**

These 16 **Bank**s are further organized into 8 **Bank Group**s. Each **Bank Group** contains 2 **Bank**s (**Bank0** and **Bank1**). **Two Banks belonging to the same Bank Group support one read and the other write, but do not support simultaneous reads or simultaneous writes.**

The memory structure of **L1 Buffer** is shown in [Figure 1](#zh-cn_topic_0000002545385271_fig5383229131020), and the address encoding format is as follows:

```cpp
L1_ADDR[18:0] = {BANK[0:0], BANK_DEPTH[9:0], BG[2:0], BANK_WIDTH[4:0]}    // The bit order is from the most significant bit to the least significant bit, 19 bits in total.
// BANK indicates the number of the Bank where the address resides, occupying 1 bit, with a value range of [0, 1].
// BANK_DEPTH indicates the row number of the address within the Bank, occupying 10 bits, with a value range of [0, 1023].
// BG indicates the number of the Bank Group where the address resides, occupying 3 bits, with a value range of [0, 7].
// BANK_WIDTH indicates the offset of the address within a row of the Bank, occupying 5 bits, with a value range of [0, 31].
```

**Figure 1** L1 Buffer memory structure diagram<a name="zh-cn_topic_0000002545385271_fig5383229131020"></a>  
![](../../../../figures/l1_memory_bank.png "L1 memory structure diagram")

## L0A Buffer/L0B Buffer Memory Structure<a name="zh-cn_topic_0000002545385271_section6770135142516"></a>

The total capacity of L0A Buffer/L0B Buffer is 64 KB, containing only one Bank with 128 rows, each row being 512 bytes wide. **Simultaneous read and write to the same address is not allowed. LoadData is allowed to write to and Mmad is allowed to read from different addresses of the same Bank at the same time.**

The memory structure of L0A Buffer/L0B Buffer is shown in [Figure 2](#zh-cn_topic_0000002545385271_fig2046019020496). L0A Buffer and L0B Buffer use the same address encoding format:

```cpp
L0A_ADDR[15:0] = { BANK_DEPTH[6:0], BANK_WIDTH[8:0] }    // The bit order is from high to low, with a total of 16 bits.
L0B_ADDR[15:0] = { BANK_DEPTH[6:0], BANK_WIDTH[8:0] }
// BANK_DEPTH indicates the number of rows of the address in the Bank, occupying 7 bits, with a value range of [0, 127].
// BANK_WIDTH indicates the offset of the address within a row of the Bank, occupying 9 bits, with a value range of [0, 511].
```

**Figure 2** L0A Buffer/L0B Buffer memory structure diagram<a name="zh-cn_topic_0000002545385271_fig2046019020496"></a>  
![](../../../../figures/l0al0b_memory_bank.png "L0A-B memory structure diagram")
