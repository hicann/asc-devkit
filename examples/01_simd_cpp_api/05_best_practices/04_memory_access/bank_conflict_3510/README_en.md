# UB Bank Conflict Scenario Verification and Analysis (Architecture 3510)


## UB Memory Structure Introduction

![UB Architecture](./figures/ub_arch.png)

- In Ascend 950PR&950DT products, the UB space size is 256KB;
- There are 16 **banks**, forming 8 **bank groups**, each bank group contains 2 banks (`bank_id` = 0, 1);
- Each bank consists of 512 rows (`BANK_DEPTH = 512`), each row is 32B long (`BANK_WIDTH = 32B`), i.e., each bank size is 16KB;
- Each bank group supports up to two reads or one read and one write.
- The same `bank_number` indicates the same bank; the same `bank_group_id` indicates the same bank group.

Specifically, each bank (32B wide) is divided into 4 subbanks, each 8B wide × 512 rows deep. **The subbank memory structure only needs attention when using scatter and gather APIs**.
```
bank internal layout (one row 32B):
┌────────┬────────┬────────┬────────┐
│ sub0   │ sub1   │ sub2   │ sub3   │
│  8B    │  8B    │  8B    │  8B    │
└────────┴────────┴────────┴────────┘
```

## UB Address Encoding Rules

The Ascend 950PR&950DT products UB 18-bit address uses **low-bit interleaving** encoding rules:

`UB_ADDR[17:0] = { DEPTH[8:0], BANK[0], BG[2:0], WIDTH[4:0] }`

| Bit Field | DEPTH[8:0] | BANK[0] | BG[2:0] | WIDTH[4:0] |
| --- | --- | --- | --- | --- |
| Bits | 9 bits | 1 bit | 3 bits | 5 bits |
| Meaning | row_id (row number within bank) | bank_id (bank number within bank group) | bank_group_id | offset (offset within row) |

![UB Address](./figures/ub_address.png)

**Decoding Example**: Address `0x4100`

Convert the address to 18-bit binary and divide by bit fields: `00 0100 0001 0000 0000`

| Bit Field | Binary Value | Decimal | Meaning |
| --- | --- | --- | --- |
| DEPTH[8:0] | `00 0100 000` | 32 | row_id = 32 |
| BANK[0] | `1` | 1 | bank_id = 1 |
| BG[2:0] | `000` | 0 | bank_group_id = 0 |
| WIDTH[4:0] | `00000` | 0 | offset = 0 |

## DataBlock Mapping

One read/write instruction processes 256B = 8 DataBlocks each time. Each DataBlock = 32B.

The 8 DataBlocks starting from the `base` address are mapped to 8 different bank groups:

| DataBlock | Offset | Physical Bank | BG |
|:--:|:------:|:---------------:|:--:|
| 0 | +0x00 | b | b%7 |
| 1 | +0x20 | b+1 | (b+1)%7 |
| ... | ... | ... | ... |
| 7 | +0xE0 | b+7 | (b+7)%7 |

Where `b = (base >> 5) & 0xF`. The 8 DataBlocks cover BG 0-7 once each.

## Bank Conflict Types

Based on the memory structure, bank conflicts are mainly divided into the following three types:
- **Read-read conflict**: Multiple read operations accessing the same bank simultaneously, or more than two read operations accessing the same bank group simultaneously will cause a conflict (in Atlas A2/A3 series products, multiple read operations accessing the same bank group will cause a conflict).
- **Write-write conflict**: Multiple write operations accessing the same bank group simultaneously.
- **Read-write conflict**: A read operation and a write operation accessing the same bank simultaneously, or two read operations and one write operation accessing the same bank group simultaneously.

**Specifically, when using scatter and gather APIs, you need to pay attention to the subbank conflict rules**:
- Scatter data with the same starting address in the same subbank has no conflict (hardware merges it into one read).
- Scatter data with the same depth address in the same subbank has no conflict. For example, data1 and data2 in the figure below do not conflict.
    ```
    bank0:
    ┌─────────────┬───────────┬───────────┬───────────┐
    │ sub0        │ sub1      │ sub2      │ sub3      │
    │ data1 data2 │           │           │           │
    └─────────────┴───────────┴───────────┴───────────┘
    ```
- Scatter data with different depth addresses in the same subbank causes conflict. For example, data1 and data2 in the figure below will conflict.
    ```
    bank0:
    ┌────────┬────────┬────────┬────────┐
    │ sub0   │ sub1   │ sub2   │ sub3   │
    │ data1  │        │        │        │
    │ data2  |        │        │        │
    └────────┴────────┴────────┴────────┘
    ```
- Scatter data with the same subbank number in the same bank group causes conflict. data1 and data2 in the figure below will conflict.
    ```
    bank0:
    ┌────────┬────────┬────────┬────────┐
    │ sub0   │ sub1   │ sub2   │ sub3   │
    │ data1  │        │        │        │
    └────────┴────────┴────────┴────────┘
    bank1:
    ┌────────┬────────┬────────┬────────┐
    │ sub0   │ sub1   │ sub2   │ sub3   │
    │ data2  |        │        │        │
    └────────┴────────┴────────┴────────┘
    ```

## Test Scenario Overview

This document focuses on verification and analysis of bank conflict scenarios for three types of APIs: continuous aligned load/store, scatter and gather, and non-continuous aligned load/store. All scenarios use `float` as input data type (index is `uint32_t` in scatter and gather scenarios), executed 1000 times to amplify performance differences.

> **Performance Notes**: The "Benchmark Performance" in the table below is the total cycles of VF executed 1000 times continuously (using `GetSystemCycle` for timing), lower values indicate better performance. "LDU Conflict Ratio" and "STU Conflict Ratio" come from the `ResourceConflictRatio` metric collected by `msopprof --aic-metrics=Default`, representing the load unit conflict ratio and store unit conflict ratio of the vector unit respectively (unit: %).

### Continuous Aligned Load/Store Bank Conflict Scenarios (Case 1-8)

**Note: In Ascend 950PR&950DT products, the scenario where src0/src1/dst starting addresses are distributed in different banks of different bank groups does not exist, and the read-write conflict cannot be avoided in two-read-one-write scenarios**

| Case | Formula | Scenario Description | Bank Conflict | src0 Address | src1 Address | dst Address | Single VF Data Volume (B) | Benchmark Performance (cycles) | LDU Conflict Ratio (%) | STU Conflict Ratio (%) | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | dst = src0 + src1 | src0/src1 starting addresses in the same bank, dst starting address in a different bank | Read-read conflict + Read-write conflict | 0x0000 | 0x5000 | 0xA100 | 16384 | 1,332,802 | 40.9 | 17.6 | Significant LDU conflict |
| 2 | dst = src0 + src1 | src0/dst starting addresses in the same bank, src0/src1 starting addresses in the same bank group | Read-write conflict | 0x0000 | 0x5100 | 0xA000 | 16384 | 1,470,520 | 0.0 | 27.1 | No LDU conflict, high STU conflict ratio |
| 3 | dst = src0 + src1 | src0/dst starting addresses in the same bank with address overlap, src1 in a different bank | Read-write conflict | 0x0000 | 0x5100 | 0x0000 | 16384 | 1,470,753 | 0.0 | 27.2 | Compared to case2, address overlap did not improve STU conflict |
| 4 | dst = src0 + src1 | src1/dst starting addresses in the same bank, src0/src1 starting addresses in the same bank group | Read-write conflict | 0x0000 | 0x5100 | 0xA100 | 16384 | 1,440,590 | 0.0 | 25.6 | Compared to case2, same conflict characteristics |
| 5 | dst = src0 + src1 | src1/dst starting addresses in the same bank with address overlap, src0 in a different bank | Read-write conflict | 0x0000 | 0x5100 | 0x5100 | 16384 | 1,440,708 | 0.0 | 25.8 | Compared to case3, address overlap did not improve STU conflict |
| 6 | dst = src0 + src1 | src0/src1/dst in the same bank, both read-read conflict and read-write conflict | Read-read conflict + Read-write conflict | 0x0000 | 0x5000 | 0xA000 | 16384 | 1,420,560 | 4.1 | 23.8 | Significant STU conflict |
| 7 | dst = src0 + src1 | Same conflict characteristics as case6, **loop unrolling applied to instructions** | Read-read conflict + Read-write conflict | 0x0000 | 0x5000 | 0xA000 | 16384 | 1,330,688 | 40.8 | 18.2 | Compared to case6, LDU conflict increased, STU conflict decreased, performance improved by 6.3% |
| 8 | dst = src0 + src1 | Same addresses as Case6, kernel adds heavy computation, changing kernel from memory-bound to compute-bound | Read-read conflict + Read-write conflict | 0x0000 | 0x5000 | 0xA000 | 16384 | 1,810,355,431 | 0.1 | 0.0 | Under compute-bound, LDU/STU conflicts almost disappear. Compared to case6, cycles are ~1274x higher |

### Scatter and Gather Scenarios (Case 9-10)

**Note: Subbank conflicts are a lower-level storage conflict and are not captured by LDU/STU conflict ratio metrics**

| Case | Formula | Scenario Description | Subbank Conflict | index Address | data Address | index Pattern | Single VF Data Volume (B) | Benchmark Performance (cycles) | LDU Conflict Ratio (%) | STU Conflict Ratio (%) | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 9 | Gather + Add + Scatter | Gather data indices [0..255] continuous | Cross-bank dispersed, no conflict | 0x0000 | 0x0400 | Indices [0..255] continuous | 1024 | 650,670 | NA | NA | Data dispersed across different subbanks of 16 banks, low conflict ratio |
| 10 | Gather + Add + Scatter | Gather data indices 0, 128 alternating repeat | Same subbank different row conflict | 0x0000 | 0x0400 | Indices 0, 128 alternating | 1024 | **750,707** | NA | NA | Compared to Case 9, performance degraded by 15.4% |

### Non-Continuous Load/Store Stride Comparison (Case 11-14)

| Case | Formula | Scenario Description | src0 Address | dst Address | Load/Store Method | Single VF Data Volume (B) | Benchmark Performance (cycles) | LDU Conflict Ratio (%) | STU Conflict Ratio (%) | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 11 | dst = src0 | 512 elements computed by row, 8 DataBlocks within repeat are continuously arranged, 256B gap between repeats | 0x0000 | 0x4100 | DataBlock continuous load/store | 2048 | 443,135 | 0 | 0 | No LDU/STU conflict |
| 12 | dst = src0 | 512 elements computed by column, 256B gap between 8 DataBlocks within repeat, 32B gap between repeats | 0x0000 | 0x4100 | DataBlock non-continuous load/store | 2048 | 1,510,497 | 0 | **53.5** | Compared to case11, extremely high STU conflict ratio, significant performance degradation |
| 13 | dst = src0 | 8 DataBlocks within the same repeat all read data from the same starting address | 0x0000 | 0x4100 | DataBlock repeated read | 2048 | 440,911 | 0.0 | 0.0 | Compared to case11, no degradation, same address merged, no extra overhead |
| 14 | dst = src0 | 8 iterations, each iteration reads data from the same starting address | 0x0000 | 0x4100 | Repeat repeated read | 2048 | 440,847 | 10.1 | 0.0 | Compared to case11, no degradation, same address merged, no extra overhead |


**Detailed analysis of each case is in the corresponding sections below.**

## Case 1: Two Reads One Write, src0/src1 Same Bank

### Address Decoding

| Operand | Starting Address | 18-bit Binary (DEPTH BANK BG WIDTH) | bank_id | bank_group_id |
| --- | --- | --- | --- | --- |
| src0 | `0x0000` | `000000000 0 000 00000` | 0 | 0 |
| src1 | `0x5000` | `000101000 0 000 00000` | 0 | 0 |
| dst | `0xA100` | `001010000 1 000 00000` | 1 | 0 |

### UB Read/Write Diagram

![case1](./figures/case1.png)

### Performance Data

- Benchmark Performance: 1,332,802 cycles
- LDU Conflict Ratio: 40.9%
- STU Conflict Ratio: 17.6%

### Analysis Conclusion

- src0 and src1 starting addresses both fall in bank0, causing a read-read conflict. Although dst starting address is in bank1, read-write conflict still occurs due to the two-read-one-write limitation.
- Performance is better than Case2/Case3. When a read instruction conflict occurs within the same repeat, since there is no data dependency between reads, other non-conflicting read instructions from different repeats can be executed simultaneously, alleviating the read-read conflict.


## Case 2: Two Reads One Write, src0/dst Same Bank, src0/src1 Same Bank Group

### Address Decoding

| Operand | Starting Address | 18-bit Binary (DEPTH BANK BG WIDTH) | bank_id | bank_group_id |
| --- | --- | --- | --- | --- |
| src0 | `0x0000` | `000000000 0 000 00000` | 0 | 0 |
| src1 | `0x5100` | `000101000 1 000 00000` | 1 | 0 |
| dst | `0xA000` | `001010000 0 000 00000` | 0 | 0 |

### UB Read/Write Diagram

![case2](./figures/case2.png)

### Performance Data

- Benchmark Performance: 1,470,520 cycles
- LDU Conflict Ratio: 0.0%
- STU Conflict Ratio: 27.1%

### Analysis Conclusion

- src0/dst fall in bank0, src1 falls in bank1, all three are in the same bank group. Read-write conflict occurs due to the two-read-one-write limitation.
- LDU=0% indicates no direct read conflict; STU=27.1% indicates the write port is queued due to insufficient total bank group bandwidth, which is a different conflict pattern from Case1 (read-read conflict dominated).
- Performance is lower than Case1 (1,470,520 vs 1,332,802 cycles, ~10.3% slower), indicating that read-write conflict overhead is higher than read-read conflict.

## Case 3: Two Reads One Write, src0 and dst Address Overlap

### Address Decoding

| Operand | Starting Address | 18-bit Binary (DEPTH BANK BG WIDTH) | bank_id | bank_group_id |
| --- | --- | --- | --- | --- |
| src0 | `0x0000` | `000000000 0 000 00000` | 0 | 0 |
| src1 | `0x5100` | `000101000 1 000 00000` | 1 | 0 |
| dst | `0x0000` | `000000000 0 000 00000` | 0 | 0 |

### UB Read/Write Diagram

![case3](./figures/case3.png)

### Performance Data

- Benchmark Performance: 1,470,753 cycles
- LDU Conflict Ratio: 0.0%
- STU Conflict Ratio: 27.2%

### Analysis Conclusion

- src0 and dst starting addresses are completely identical (0x0000), src1 is in bank1 (0x5100), all three belong to BG=0.
- Compared to Case2, performance is basically the same, indicating that on the Ascend 950PR&950DT products architecture, read-write address overlap does not eliminate bank conflicts, which differs from some older architectures.


## Case 4: Two Reads One Write, src1/dst Same Bank, src0/src1 Same Bank Group

### Address Decoding

| Operand | Starting Address | 18-bit Binary (DEPTH BANK BG WIDTH) | bank_id | bank_group_id |
| --- | --- | --- | --- | --- |
| src0 | `0x0000` | `000000000 0 000 00000` | 0 | 0 |
| src1 | `0x5100` | `000101000 1 000 00000` | 1 | 0 |
| dst | `0xA100` | `001010000 1 000 00000` | 1 | 0 |

### UB Read/Write Diagram

![case4](./figures/case4.png)

### Performance Data

- Benchmark Performance: 1,440,590 cycles
- LDU Conflict Ratio: 0.0%
- STU Conflict Ratio: 25.6%

### Analysis Conclusion

- src1/dst both fall in bank1, src0 is in bank0, all three belong to BG=0. Performance is similar, with the same conflict pattern as Case2.

## Case 5: Two Reads One Write, src1 and dst Address Overlap

### Address Decoding

| Operand | Starting Address | 18-bit Binary (DEPTH BANK BG WIDTH) | bank_id | bank_group_id |
| --- | --- | --- | --- | --- |
| src0 | `0x0000` | `000000000 0 000 00000` | 0 | 0 |
| src1 | `0x5100` | `000101000 1 000 00000` | 1 | 0 |
| dst | `0x5100` | `000101000 1 000 00000` | 1 | 0 |

### UB Read/Write Diagram

![case5](./figures/case5.png)

### Performance Data

- Benchmark Performance: 1,440,708 cycles
- LDU Conflict Ratio: 0.0%
- STU Conflict Ratio: 25.8%

### Analysis Conclusion

- src1 and dst starting addresses are completely identical, src0 is in bank1, all three belong to BG=0.
- Compared to Case4, performance is basically the same, indicating that on the Ascend 950PR&950DT products architecture, read-write address overlap does not eliminate bank conflicts, which differs from some older architectures.


## Case 6: Two Reads One Write, src0/src1/dst Same Bank

### Address Decoding

| Operand | Starting Address | 18-bit Binary (DEPTH BANK BG WIDTH) | bank_id | bank_group_id |
| --- | --- | --- | --- | --- |
| src0 | `0x0000` | `000000000 0 000 00000` | 0 | 0 |
| src1 | `0x5000` | `000101000 0 000 00000` | 0 | 0 |
| dst | `0xA000` | `001010000 0 000 00000` | 0 | 0 |

### UB Read/Write Diagram

![case6——7](./figures/case6_7.png)

### Performance Data

- Benchmark Performance: 1,420,560 cycles
- LDU Conflict Ratio: 4.1%
- STU Conflict Ratio: 23.8%

### Analysis Conclusion

- All three operands fall in bank0 (BG=0). There is both a read-read conflict between src0/src1 and a read-write conflict.

## Case 7: Two Reads One Write, src0/src1/dst Same Bank, Loop Unrolling

### Address Decoding

Same as Case 6.

### UB Read/Write Diagram

Same as Case 6.

### Performance Data

- Benchmark Performance: 1,330,688 cycles
- LDU Conflict Ratio: 40.8%
- STU Conflict Ratio: 18.2%

### Analysis Conclusion

- The address configuration is exactly the same as Case 6. A 2-way loop unrolling is adopted, with the first half and second half alternately issuing instructions, increasing the probability of instruction dual-issue. LDU increased from 4.1% → 40.2%, while STU decreased from 24.2% → 18.0%, indicating that instruction parallelism released some of the competition pressure on the storage pipeline.
- Total cycles decreased from 1,420,560 → 1,330,688, a 6.3% performance improvement.

## Case 8: Two Reads One Write, src0/src1/dst Same Bank, Compute-Bound

### Address Decoding

Same as Case 6.

### UB Read/Write Diagram

Same as Case 6.

### Performance Data

- Benchmark Performance: 1,810,355,431 cycles
- LDU Conflict Ratio: 0.1%
- STU Conflict Ratio: 0.0%

### Analysis Conclusion

- The address configuration is exactly the same as Case 6 (all three operands fall in bank0, BG=0). The difference is that the kernel internally adds an additional 1000 iterations of `Adds` computation loop, changing the kernel from memory-bound to compute-bound, used for comparative analysis of LDU/STU conflict ratio metric changes under compute-bound scenarios.
- Measured LDU conflict ratio dropped from 4.1% in Case 6 to 0.1%, and STU conflict ratio dropped from 23.8% to 0.0%, indicating that after the proportion of computation time increases significantly, the storage instruction wait time is diluted by a large number of computation instructions, making bank conflicts almost invisible. Total cycles increased by about 1274x, proportional to the additional computation.

## Case 9: Scatter and Gather, Gather Data Indices Continuous, Cross-Bank Dispersed

### Address Decoding

| Operand | Starting Address | 18-bit Binary (DEPTH BANK BG WIDTH) | bank_id | bank_group_id |
| --- | --- | --- | --- | --- |
| index (src0) | `0x0000` | `000000000 0 000 00000` | 0 | 0 |
| data (src1) | `0x0400` | `000001000 0 000 00000` | 0 | 0 |
| dst | `0x0400` | `000001000 0 000 00000` | 0 | 0 |

### UB Read/Write Diagram

![case9](./figures/case8.png)

### Performance Data

- Benchmark Performance: 650,670 cycles
- LDU Conflict Ratio: NA
- STU Conflict Ratio: NA

### Case Analysis

- Indices take values [0..255] continuously incrementing (256 unique values in total). The 256 elements are dispersed across different depth rows of different banks/subbanks. Each element is dispersed across banks, and both Gather read and Scatter write have no obvious bank/subbank conflicts.
- LDU/STU conflict ratio metrics are ineffective in this scenario, and diagnosis needs to be done through benchmark performance differences.


## Case 10: Scatter and Gather, Gather Data Indices Alternating Repeat, Same Subbank Conflict

### Address Decoding

| Operand | Starting Address | 18-bit Binary (DEPTH BANK BG WIDTH) | bank_id | bank_group_id |
| --- | --- | --- | --- | --- |
| index (src0) | `0x0000` | `000000000 0 000 00000` | 0 | 0 |
| data (src1) | `0x0400` | `000001000 0 000 00000` | 0 | 0 |
| dst | `0x0400` | `000001000 0 000 00000` | 0 | 0 |

### UB Read/Write Diagram

![case10](./figures/case9.png)

### Performance Data

- Benchmark Performance: 750,707 cycles
- LDU Conflict Ratio: NA
- STU Conflict Ratio: NA

### Case Analysis

- The address configuration is exactly the same as Case 9. The difference is that the index pattern is 0, 128 alternating repeat (within each repeat, half the elements are read via index=0, the other half via index=128). Both fall in the same subbank (sub0) of the same bank, but at different depth rows. The scatter read needs to switch between different rows of the same subbank, triggering a subbank conflict.
- Compared to Case 9, performance degraded by 15.4%, indicating that subbank same-address row conflicts have a significant impact on scatter and gather API performance.


## Case 11: One Read One Write, Row-wise Load/Store, DataBlock Continuous Arrangement, No Bank Conflict

### Address Decoding

| Operand | Starting Address | 18-bit Binary (DEPTH BANK BG WIDTH) | bank_id | bank_group_id |
| --- | --- | --- | --- | --- |
| src0 | `0x0000` | `000000000 0 000 00000` | 0 | 0 |
| dst | `0x4100` | `000100000 1 000 00000` | 1 | 0 |

### UB Read/Write Diagram

![case11](./figures/case10.png)

### Performance Data

- Benchmark Performance: 443,135 cycles
- LDU Conflict Ratio: 0%
- STU Conflict Ratio: 0%

### Analysis Conclusion

- Within each repeat, 8 DataBlocks are arranged continuously with 32B intervals, incrementing along the UB address. Since 32B is exactly the bank group rotation period (switching one bank group every 32B), the 8 DataBlocks naturally hit BG 0-7 once each, with no bank group competition.
- src0 and dst belong to different banks (bank0 vs bank1), and one read one write is within the bank group capability, so there is no conflict. LDU=0%/STU=0%.

## Case 12: One Read One Write, Column-wise Load/Store, DataBlock Non-Continuous Arrangement, Severe Bank Conflict

### Address Decoding

| Operand | Starting Address | 18-bit Binary (DEPTH BANK BG WIDTH) | bank_id | bank_group_id |
| --- | --- | --- | --- | --- |
| src0 | `0x0000` | `000000000 0 000 00000` | 0 | 0 |
| dst | `0x4100` | `000100000 1 000 00000` | 1 | 0 |

### UB Read/Write Diagram

![case12](./figures/case11.png)

### Performance Data

- Benchmark Performance: 1,510,497 cycles
- LDU Conflict Ratio: 0%
- STU Conflict Ratio: 53.5%

### Analysis Conclusion

- Within each repeat, 8 DataBlocks are spaced 8 × 32B = 256B apart, which is exactly the complete rotation period of 8 bank groups. Therefore, all 8 DataBlocks return to **the same bank of the same bank group**, with only different depth (depth+8 every 256B).
- STU conflict ratio is as high as 53.5% (write conflict is the main bottleneck), and performance degraded about 3.4x compared to Case 11.

## Case 13: One Read One Write, Same Address Repeated Read Within Repeat

### Address Decoding

| Operand | Starting Address | 18-bit Binary (DEPTH BANK BG WIDTH) | bank_id | bank_group_id |
| --- | --- | --- | --- | --- |
| src0 | `0x0000` | `000000000 0 000 00000` | 0 | 0 |
| dst | `0x4100` | `000100000 1 000 00000` | 1 | 0 |

### UB Read/Write Diagram

![case13](./figures/case12.png)

### Performance Data

- Benchmark Performance: 440,911 cycles
- LDU Conflict Ratio: 0.0%
- STU Conflict Ratio: 0.0%

### Analysis Conclusion

- Set blockStride=0. All 8 DataBlocks within the same repeat read from the same 32B base address. The hardware automatically detects the same address read requests within the same repeat and merges them into a single UB read. The 8 DataBlocks share the same data.
- Same address repeated read within a repeat does not require additional optimization. The hardware automatically merges to avoid redundant access. LDU=0%/STU=0%, consistent with Case11.

## Case 14: One Read One Write, Same Starting Address Repeated Read Across Repeats

### Address Decoding

| Operand | Starting Address | 18-bit Binary (DEPTH BANK BG WIDTH) | bank_id | bank_group_id |
| --- | --- | --- | --- | --- |
| src0 | `0x0000` | `000000000 0 000 00000` | 0 | 0 |
| dst | `0x4100` | `000100000 1 000 00000` | 1 | 0 |

### UB Read/Write Diagram

![case14](./figures/case13.png)

### Performance Data

- Benchmark Performance: 440,847 cycles
- LDU Conflict Ratio: 10.1%
- STU Conflict Ratio: 0.0%

### Analysis Conclusion

- All repeat iterations load data from the same starting address, and dst writes with different offsets. The hardware cache hits, with no extra UB access overhead.
- LDU 10.1%/STU 0%, on par with the Case 11 baseline.

## Summary

1. On Ascend 950PR&950DT products, two-read-one-write inevitably has bank conflicts. In actual scenarios, since the Regbase architecture is adopted, UB read/write frequency is reduced, and bank conflicts are often not the bottleneck.
7. When using scatter and gather APIs, continuous indices dispersed across subbanks can reduce conflicts.
8. Loop unrolling can optimize bank conflicts, as shown in the following trace analysis.
    | case6 pipeline | case7 pipeline |
    | ---   | --- |
    | ![case6_trace](./figures/case6_trace.png) | ![case7_trace](./figures/case7_trace.png) |
9. When there are a large number of Reg computation instructions between read and write instructions, read-write instruction conflicts are reduced, as shown in the following trace analysis.
    | case6 pipeline | case8 pipeline |
    | ---   | --- |
    | ![case6_trace](./figures/case6_trace.png) | ![case8_trace](./figures/case8_trace.png) |
10. In two-read-one-write scenarios, the impact of read-write conflicts is greater than that of read-read conflicts, as shown in the following trace analysis.
    | case1 pipeline | case3 pipeline |
    | ---   | --- |
    | ![case1_trace](./figures/case1_trace.png) | ![rw_conflict](./figures/case3_trace.png) |
11. Optimization suggestions: When using continuous aligned load/store interfaces, loop unrolling can be used to alleviate conflict bottlenecks. When using non-continuous aligned load/store interfaces, it is recommended that DataBlocks fall in different bank groups. When using scatter and gather interfaces, data should be dispersed to different subbanks of different bank groups.


## Directory Structure

```text
├── bank_conflict.asc             // Operator host + device code
├── CMakeLists.txt                // Build project
├── config.h                      // Address/data type configuration for 14 cases
├── data_utils.h                  // Data read/write utilities
├── figures/                      // UB memory layout diagrams for each case
│   ├── ub_arch.png               // UB architecture diagram
│   ├── ub_address.png            // UB address encoding diagram
│   ├── case1.png
│   ├── case1_trace.png           // Case1 pipeline trace
│   ├── case2.png
│   ├── case3.png
│   ├── case3_trace.png           // Case3 pipeline trace (read-write conflict)
│   ├── case4.png
│   ├── case5.png
│   ├── case6_7.png
│   ├── case6_trace.png           // Case6 pipeline trace
│   ├── case7_trace.png           // Case7 pipeline trace
│   ├── case8.png
│   ├── case9.png
│   ├── case10.png
│   ├── case11.png
│   ├── case12.png
│   └── case13.png
├── README.md                     // This document
├── scripts/
│   ├── gen_data.py               // Test data generation
│   └── verify_result.py          // Accuracy verification
└── vf_func.h                     // VF function implementation
```

## How to Run

### Environment Preparation

```bash
source /path/to/cann/set_env.sh
```

### Build and Run

```bash
export SCENARIO_NUM=1          # Valid values: 1-14
mkdir -p build && cd build
cmake .. -DSCENARIO_NUM=${SCENARIO_NUM}
make -j
python3 ../scripts/gen_data.py --case ${SCENARIO_NUM}
./demo                     # Accuracy verification + benchmark performance (outputs cycle count to stdout)
python3 ../scripts/verify_result.py output/output_z.bin output/golden.bin
```

### CMake Options

| Option | Valid Values | Description |
|:------:|:------------:|:------------|
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture version |
| `SCENARIO_NUM` | 1-14 | Switch test scenario |
| `CMAKE_ASC_RUN_MODE` | `npu` | On-board run mode |

### Conflict Performance Collection

Use `msopprof` to collect hardware performance counters to quantify bank conflict severity:

```bash
# After building a specific case, execute in the build directory
msopprof --aic-metrics=Default ./demo
```

The monitoring results are saved in the `OPPROF_<timestamp>/` directory. Key metrics are as follows:

#### ResourceConflictRatio.csv

| Metric | Meaning | Description |
|--------|---------|-------------|
| `aiv_vec_ldu_cflt_ratio` | Vector Load unit conflict ratio | The proportion of time read operations are blocked due to bank conflicts. Higher values indicate more severe read conflicts |
| `aiv_vec_stu_cflt_ratio` | Vector Store unit conflict ratio | The proportion of time write operations are blocked due to bank conflicts. Higher values indicate more severe write conflicts |
