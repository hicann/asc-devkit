# UB Bank Conflict and Address Overlap Scenario Analysis Example

## Overview

This example is based on the Add instruction. By configuring different LocalTensor addresses, it verifies and analyzes the impact of bank conflicts and address overlaps in UB (Unified Buffer) on performance. The example uses the compile-time parameter `SCENARIO_NUM` to select different address configuration scenarios, with a total of 8 scenarios covering typical cases such as no conflict, read-read conflict, read-write conflict, and complete address overlap.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|---------|----------------------|
| Atlas A3 Training Series Products / Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products / Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

``` text
├── bank_conflict_ub
│   ├── scripts
│   │   ├── gen_data.py                    // Input data and ground truth data generation script
│   │   └── verify_result.py               // Verification script to check if output data matches ground truth
│   ├── figures                            // UB memory layout diagrams and read/write illustrations for each scenario
│   ├── CMakeLists.txt                     // Build project file
│   ├── config.h                           // Address configurations for each SCENARIO_NUM
│   ├── data_utils.h                       // Data read/write functions
│   ├── bank_conflict_ub.asc               // Ascend C example implementation and usage sample
│   ├── README.md                          // Example documentation (Chinese)
│   └── README_en.md                       // Example documentation (English)
```

## UB (Unified Buffer) Memory Structure

![UB Memory Layout](./figures/ub_memory_layout.png)

- On the A2/A3 architecture, the UB space size is 192KB;
- There are 48 **banks** in total, forming 16 **bank groups**, with each bank group containing 3 banks (`bank_id` = 0, 1, 2);
- Each bank consists of 128 rows (`BANK_DEPTH = 128`), with each row being 32B long (`BANK_WIDTH = 32B`), making each bank 4KB in size;
- **Bank global numbering rule**: `bank_number = bank_group_id + bank_id * 16`;
- The same bank_number indicates the same bank; the same bank_group_id indicates the same bank group.

## UB Address Encoding Rules

The UB 18-bit address encoding follows the rules below:

`UB_ADDR[17:0] = { BANK[1:0], BANK_DEPTH[6:0], BG[3:0], BANK_WIDTH[4:0] }`

| Bit Field | BANK[1:0] | BANK_DEPTH[6:0] | BG[3:0] | BANK_WIDTH[4:0] |
| --- | --- | --- | --- | --- |
| Bits | 2 bits | 7 bits | 4 bits | 5 bits |
| Meaning | bank_id | row_id (row number within bank) | bank_group_id (bank group number) | offset (intra-row offset) |

**Decoding example**: Address `0x10020`

Convert the address to 18-bit binary, divided by bit fields: `01 0000000 0001 00000`

| Bit Field | Binary Value | Decimal | Meaning |
| --- | --- | --- | --- |
| BANK[1:0] | `01` | 1 | bank_id = 1 |
| BANK_DEPTH[6:0] | `0000000` | 0 | row_id = 0 |
| BG[3:0] | `0001` | 1 | bank_group_id = 1 |
| BANK_WIDTH[4:0] | `00000` | 0 | offset = 0 |

Based on the UB memory layout diagram and bank encoding rules above, this address is located in the bank with bank_id = 1 within bank group 1, i.e., global bank 17 (`1 + 1 x 16 = 17`), at row 0.

## Bank Conflict Types

Based on the memory structure, bank conflicts are mainly classified into the following three types:

- Read-read conflict: Multiple read operations simultaneously attempt to access the same bank group;
- Write-write conflict: Multiple write operations simultaneously attempt to access the same bank group;
- Read-write conflict: A read operation and a write operation simultaneously attempt to access the same bank.

## Scenario Details

This example conducts verification and analysis from two perspectives: bank conflicts and address overlaps. The input data type for all scenarios is `float`, with a data length of 4096, using continuous read/write mode (`BlkStride = 1`, `RepStride = 8`, `mask = 64`). Special configurations are described separately in the corresponding scenario. The kernel function name is `add_custom`.

> **Performance note**: The "Benchmark Performance" in the table below represents the cycle count of the Add instruction executed 1000 consecutive times. Lower values indicate better performance.

| Scenario | Formula | Description | Bank Conflict Status | src0 Base Addr | src1 Base Addr | dst Base Addr | Benchmark Perf | Remarks |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | dst = src0 + src1 | src0/src1/dst base addresses distributed across different banks in different bank groups | No read-read or read-write conflict | 0 | 0x10100 | 0x20000 | 2390 | Baseline performance reference (no bank conflict) |
| 2 | dst = src0 + src1 | src0/src1 base addresses in the same bank | **Read-read conflict** | 0 | 0x4000 | 0x10000 | 4167 | Verify read-read conflict (multiple reads accessing the same bank) |
| 3 | dst = src0 + src1 | src0/src1 base addresses in the same bank group | **Read-read conflict** | 0 | 0x10000 | 0x14100 | 4167 | Verify read-read conflict (multiple reads accessing the same bank group) |
| 4 | dst = src0 + src1 | src0/dst base addresses in the same bank | No read-write conflict (hardware optimization) | 0 | 0x10100 | 0x4000 | 2389 | Baseline reference for scenario 5 |
| 5 | dst = src0 + src1 | src0/dst base addresses not in the same bank | **Read-write conflict** | 0 | 0x10100 | 0x4020 | 3751 | Verify read-write conflict (compare with scenario 4) |
| 6 | dst = src0 + src1 | src1/dst base addresses in the same bank,<br>src1/dst addresses completely overlap | No read-write conflict (hardware optimization) | 0 | 0x10100 | 0x10100 | 2390 | Verify src/dst address overlap (no read-write conflict, hardware optimization) |
| 7 | dst = src0 + src1 | src0 repeatedly reads the same datablock on each repeat (src0BlkStride=0, src0RepStride=1),<br>src1/dst addresses completely overlap | **Read-read conflict** | 0 | 0x10100 | 0x10100 | 3584 | Compare with scenario 6/scenario 8, verify correlation between src0RepStride configuration and bank conflicts |
| 8 | dst = src0 + src1 | src0 repeatedly reads the same datablock on each repeat (src0BlkStride=0, src0RepStride=8),<br>src1/dst addresses completely overlap | No read-read or read-write conflict | 0 | 0x10100 | 0x10100 | 2389 | Compare with scenario 7, verify correlation between src0RepStride configuration and bank conflicts |

Detailed analysis for each scenario is provided in the corresponding sections below.

### Scenario 1: No Read-Read Conflict, No Read-Write Conflict

#### Address Decoding

| Operand | Base Address | 18-bit Binary (BANK DEPTH BG WIDTH) | bank_id | bank_group_id | Global Bank Number |
| --- | --- | --- | --- | --- | --- |
| src0 | `0x0` | `00 0000000 0000 00000` | 0 | 0 | 0 |
| src1 | `0x10100` | `01 0000000 1000 00000` | 1 | 8 | 24 |
| dst | `0x20000` | `10 0000000 0000 00000` | 2 | 0 | 32 |

#### UB Read/Write Illustration

![scenario1](./figures/scenario1.png)

- Every 8 green data blocks (32B) represent the data read by src0 per repeat (256B);
- Every 8 yellow data blocks (32B) represent the data read by src1 per repeat (256B);
- Every 8 blue data blocks (32B) represent the data written by dst per repeat (256B);
- Number of iterations required to process all data: `repeat_times = data_length * sizeof(float) / bytes_per_repeat = 4096 * 4 / 256 = 64`.

#### Analysis Conclusion

- Read-read conflict analysis: In the 1st repeat, src0's (bank_id=0) 8 datablocks are in bg 0-7, src1's (bank_id=1) 8 datablocks are in bg 8-15, with completely non-overlapping bank groups; subsequent repeats follow the same pattern, so **no read-read conflict**.
- Read-write conflict analysis: In the 1st repeat, dst's (bank_id=2) 8 datablocks are in bg 0-7, with bank_id different from both src0 (bank_id=0) and src1 (bank_id=1), so they are not in the same bank; subsequent repeats follow the same pattern, so **no read-write conflict**.

### Scenario 2: src0/src1 Base Addresses in the Same Bank — Read-Read Conflict

#### Address Decoding

| Operand | Base Address | 18-bit Binary (BANK DEPTH BG WIDTH) | bank_id | bank_group_id | Global Bank Number |
| --- | --- | --- | --- | --- | --- |
| src0 | `0x0` | `00 0000000 0000 00000` | 0 | 0 | 0 |
| src1 | `0x4000` | `00 0100000 0000 00000` | 0 | 0 | 0 |
| dst | `0x10000` | `01 0000000 0000 00000` | 1 | 0 | 16 |

#### UB Read/Write Illustration

![scenario2](./figures/scenario2.png)

- Every 8 green data blocks (32B) represent the data read by src0 per repeat (256B);
- Every 8 yellow data blocks (32B) represent the data read by src1 per repeat (256B);
- Every 8 blue data blocks (32B) represent the data written by dst per repeat (256B);
- Number of iterations required to process all data: `repeat_times = 4096 * 4 / 256 = 64`.

#### Analysis Conclusion

- Read-read conflict analysis: In the 1st repeat, src0's (bank_id=0) and src1's (bank_id=0) 8 datablocks are both in bg 0-7, with the same bank_id and completely overlapping bank groups; subsequent repeats follow the same pattern, so **read-read conflict exists**.
- Read-write conflict analysis: In the 1st repeat, src0/src1 reads (bank_id=0) and dst writes (bank_id=1) have different bank_ids, so they are not in the same bank; subsequent repeats follow the same pattern, so **no read-write conflict**.

### Scenario 3: src0/src1 Base Addresses in the Same Bank Group — Read-Read Conflict

#### Address Decoding

| Operand | Base Address | 18-bit Binary (BANK DEPTH BG WIDTH) | bank_id | bank_group_id | Global Bank Number |
| --- | --- | --- | --- | --- | --- |
| src0 | `0x0` | `00 0000000 0000 00000` | 0 | 0 | 0 |
| src1 | `0x10000` | `01 0000000 0000 00000` | 1 | 0 | 16 |
| dst | `0x14100` | `01 0100000 1000 00000` | 1 | 8 | 24 |

#### UB Read/Write Illustration

![scenario3](./figures/scenario3.png)

- Every 8 green data blocks (32B) represent the data read by src0 per repeat (256B);
- Every 8 yellow data blocks (32B) represent the data read by src1 per repeat (256B);
- Every 8 blue data blocks (32B) represent the data written by dst per repeat (256B);
- Number of iterations required to process all data: `repeat_times = 4096 * 4 / 256 = 64`.

#### Analysis Conclusion

- Read-read conflict analysis: In the 1st repeat, src0's (bank_id=0) and src1's (bank_id=1) 8 datablocks are both in bg 0-7; although bank_ids differ, the bank groups completely overlap; subsequent repeats follow the same pattern, so **read-read conflict exists**.
- Read-write conflict analysis: In the 1st repeat, src0/src1 reads (bank_id=0/1, bg 0-7) and dst writes (bank_id=1, bg 8-15) have non-overlapping bank groups, so they are not in the same bank; subsequent repeats follow the same pattern, so **no read-write conflict**.

> **Comparison with scenario 2**: In scenario 2, src0/src1 are in the same bank (both bank_id and bank_group_id are the same); in scenario 3, they are in the same bank group (only bank_group_id is the same). Both scenarios trigger read-read conflicts with similar performance overhead.

### Scenario 4: src0/dst Base Addresses in the Same Bank — No Read-Write Conflict (Hardware Optimization)

#### Address Decoding

| Operand | Base Address | 18-bit Binary (BANK DEPTH BG WIDTH) | bank_id | bank_group_id | Global Bank Number |
| --- | --- | --- | --- | --- | --- |
| src0 | `0x0` | `00 0000000 0000 00000` | 0 | 0 | 0 |
| src1 | `0x10100` | `01 0000000 1000 00000` | 1 | 8 | 24 |
| dst | `0x4000` | `00 0100000 0000 00000` | 0 | 0 | 0 |

#### UB Read/Write Illustration

![scenario4](./figures/scenario4.png)

- Every 8 green data blocks (32B) represent the data read by src0 per repeat (256B);
- Every 8 yellow data blocks (32B) represent the data read by src1 per repeat (256B);
- Every 8 blue data blocks (32B) represent the data written by dst per repeat (256B);
- Number of iterations required to process all data: `repeat_times = 4096 * 4 / 256 = 64`.

#### Analysis Conclusion

- Read-read conflict analysis: In the 1st repeat, src0's (bank_id=0, bg 0-7) and src1's (bank_id=1, bg 8-15) bank groups do not overlap at all; subsequent repeats follow the same pattern, so **no read-read conflict**.
- Read-write conflict analysis: In the 1st repeat, src0 reads (bank_id=0) and dst writes (bank_id=0) have 8 datablocks both in bg 0-7 with the same bank_id, theoretically causing a read-write conflict; however, the hardware automatically delays dst writes by 1 repeat, staggering the read and write operations; subsequent repeats follow the same pattern, so **no actual read-write conflict**.

**Theoretical execution timing** (unoptimized):

| | 1st repeat | 2nd repeat | 3rd repeat | ... |
| --- | --- | --- | --- | --- |
| src0 read | bank 0-7 (bg 0-7) | bank 8-15 (bg 8-15) | bank 0-7 (bg 0-7) | ... |
| src1 read | bank 24-31 (bg 8-15) | bank 16-23 (bg 0-7) | bank 24-31 (bg 8-15) | ... |
| dst write | bank 0-7 (bg 0-7) | bank 8-15 (bg 8-15) | bank 0-7 (bg 0-7) | ... |

Theoretical result: src0 and dst access the same bank every repeat — read-write conflict exists.

**Actual execution timing** (hardware optimized):

| | 1st repeat | 2nd repeat | 3rd repeat | ... |
| --- | --- | --- | --- | --- |
| src0 read | bank 0-7 (bg 0-7) | bank 8-15 (bg 8-15) | bank 0-7 (bg 0-7) | ... |
| src1 read | bank 24-31 (bg 8-15) | bank 16-23 (bg 0-7) | bank 24-31 (bg 8-15) | ... |
| dst write | — | bank 0-7 (bg 0-7) | bank 8-15 (bg 8-15) | ... |

Actual result: the hardware delays dst writes by 1 repeat, staggering them with src0 reads to avoid read-write conflicts.

### Scenario 5: src0/dst Base Addresses Not in the Same Bank — Read-Write Conflict

#### Address Decoding

| Operand | Base Address | 18-bit Binary (BANK DEPTH BG WIDTH) | bank_id | bank_group_id | Global Bank Number |
| --- | --- | --- | --- | --- | --- |
| src0 | `0x0` | `00 0000000 0000 00000` | 0 | 0 | 0 |
| src1 | `0x10100` | `01 0000000 1000 00000` | 1 | 8 | 24 |
| dst | `0x4020` | `00 0100000 0001 00000` | 0 | 1 | 1 |

#### UB Read/Write Illustration

![scenario5](./figures/scenario5.png)

- Every 8 green data blocks (32B) represent the data read by src0 per repeat (256B);
- Every 8 yellow data blocks (32B) represent the data read by src1 per repeat (256B);
- Every 8 blue data blocks (32B) represent the data written by dst per repeat (256B);
- Number of iterations required to process all data: `repeat_times = 4096 × 4 ÷ 256 = 64`.

#### Analysis Conclusion

- Read-read conflict analysis: In the 1st repeat, src0's (bank_id=0, bg 0-7) and src1's (bank_id=1, bg 8-15) bank groups do not overlap at all; subsequent repeats follow the same pattern, so **no read-read conflict**.
- Read-write conflict analysis: In the 1st repeat, src0 reads (bank_id=0, bg 0-7) and dst writes (bank_id=0, bg 1-8) have the same bank_id with partially overlapping bank groups (bg 1-7), accessing the same banks; subsequent repeats follow the same pattern, so **read-write conflict exists**.

**UB Read/Write Process Illustration**:

| | 1st repeat | 2nd repeat | 3rd repeat | ... |
| --- | --- | --- | --- | --- |
| src0 read | bank 0-7 (bg 0-7) | bank 8-15 (bg 8-15) | bank 0-7 (bg 0-7) | ... |
| src1 read | bank 24-31 (bg 8-15) | bank 16-23 (bg 0-7) | bank 24-31 (bg 8-15) | ... |
| dst write | bank 1-8 (bg 1-8) | bank 9-15-0 (bg 9-15-0) | bank 1-8 (bg 1-8) | ... |

Analysis result: src0 and dst access the same banks every repeat, causing read-write conflicts.

### Scenario 6: src1/dst Base Addresses in the Same Bank, src1/dst Addresses Overlap — No Read-Write Conflict (Hardware Optimization)

#### Address Decoding

| Operand | Base Address | 18-bit Binary (BANK DEPTH BG WIDTH) | bank_id | bank_group_id | Global Bank Number |
| --- | --- | --- | --- | --- | --- |
| src0 | `0x0` | `00 0000000 0000 00000` | 0 | 0 | 0 |
| src1 | `0x10100` | `01 0000000 1000 00000` | 1 | 8 | 24 |
| dst | `0x10100` | `01 0000000 1000 00000` | 1 | 8 | 24 |

#### UB Read/Write Illustration

![scenario6](./figures/scenario6.png)

- Every 8 green data blocks (32B) represent the data read by src0 per repeat (256B);
- Every 8 yellow data blocks represent the data read by src1 and written by dst to the same memory space per repeat (256B), i.e., src1 and dst addresses completely overlap;
- Number of iterations required to process all data: `repeat_times = 4096 × 4 ÷ 256 = 64`.

#### Analysis Conclusion

- Read-read conflict analysis: In the 1st repeat, src0's (bank_id=0, bg 0-7) and src1's (bank_id=1, bg 8-15) bank groups do not overlap at all; subsequent repeats follow the same pattern, so **no read-read conflict**.
- Read-write conflict analysis: In the 1st repeat, src1 reads (bank_id=1) and dst writes (bank_id=1) have completely overlapping addresses, theoretically posing a read-write conflict risk; however, the hardware automatically delays dst writes by 1 repeat, staggering the read and write operations; subsequent repeats follow the same pattern, so **no actual read-write conflict**.

**Theoretical execution timing** (unoptimized):

| | 1st repeat | 2nd repeat | 3rd repeat | ... |
| --- | --- | --- | --- | --- |
| src0 read | bank 0-7 (bg 0-7) | bank 8-15 (bg 8-15) | bank 0-7 (bg 0-7) | ... |
| src1 read | bank 24-31 (bg 8-15) | bank 16-23 (bg 0-7) | bank 24-31 (bg 8-15) | ... |
| dst write | bank 24-31 (bg 8-15) | bank 16-23 (bg 0-7) | bank 24-31 (bg 8-15) | ... |

Theoretical result: src1 and dst access the same bank every repeat — read-write conflict would occur.

**Actual execution timing** (hardware optimized):

| | 1st repeat | 2nd repeat | 3rd repeat | ... |
| --- | --- | --- | --- | --- |
| src0 read | bank 0-7 (bg 0-7) | bank 8-15 (bg 8-15) | bank 0-7 (bg 0-7) | ... |
| src1 read | bank 24-31 (bg 8-15) | bank 16-23 (bg 0-7) | bank 24-31 (bg 8-15) | ... |
| dst write | — | bank 24-31 (bg 8-15) | bank 16-23 (bg 0-7) | ... |

Actual result: the hardware delays dst writes by 1 repeat, staggering them with src1 reads to avoid read-write conflicts.

### Scenario 7: src0 Repeatedly Reads the Same Datablock (src0BlkStride=0, src0RepStride=1), src1/dst Addresses Overlap — Read-Read Conflict

> **Special configuration**: src0's `src0BlkStride=0` (repeatedly reads the same datablock within a single repeat), `src0RepStride=1` (each repeat is spaced by 1 datablock, i.e., 32B).

#### Address Decoding

| Operand | Base Address | 18-bit Binary (BANK DEPTH BG WIDTH) | bank_id | bank_group_id | Global Bank Number |
| --- | --- | --- | --- | --- | --- |
| src0 | `0x0` | `00 0000000 0000 00000` | 0 | 0 | 0 |
| src1 | `0x10100` | `01 0000000 1000 00000` | 1 | 8 | 24 |
| dst | `0x10100` | `01 0000000 1000 00000` | 1 | 8 | 24 |

#### UB Read/Write Illustration

![scenario7](./figures/scenario7.png)

- The same 1 green data block (32B) is repeatedly read 8 times per repeat (src0BlkStride=0);
- Every 8 yellow data blocks represent each repeat, where src1 reads and dst writes the same data space (256B), i.e., src1 and dst addresses completely overlap;
- Number of iterations required to process all data: `repeat_times = 4096 * 4 / 256 = 64`.

#### Analysis Conclusion

- Read-read conflict analysis: In the 1st repeat, src0 (bank_id=0) reads only a single datablock (bg 0), which is not in the same bank group as src1 (bank_id=1, bg 8-15), so the first beat has no conflict; however, in subsequent repeats src0's bank group increments sequentially (bg 1, 2, ...), and on every even-numbered repeat it overlaps with src1's bank group range — overall **read-read conflict exists**.
- Read-write conflict analysis: In the 1st repeat, src1 reads (bank_id=1) and dst writes (bank_id=1) have completely overlapping addresses; the hardware automatically delays dst writes by 1 repeat, staggering the read and write operations; subsequent repeats follow the same pattern, so **no read-write conflict**.

**UB Read/Write Process Illustration**:

| | 1st repeat | 2nd repeat | 3rd repeat | 4th repeat | ... |
| --- | --- | --- | --- | --- | --- |
| src0 read | bank 0 (bg 0) | bank 1 (bg 1) | bank 2 (bg 2) | bank 3 (bg 3) | ... |
| src1 read | bank 24-31 (bg 8-15) | bank 16-23 (bg 0-7) | bank 24-31 (bg 8-15) | bank 16-23 (bg 0-7) | ... |
| dst write | - | bank 24-31 (bg 8-15) | bank 16-23 (bg 0-7) | bank 24-31 (bg 8-15) | ... |

Analysis results:
- 2nd repeat: src0 reads bg 1, src1 reads bg 0-7 → bg 1 overlaps — read-read conflict;
- 4th repeat: src0 reads bg 3, src1 reads bg 0-7 → bg 3 overlaps — read-read conflict;
- ...
- dst writes are delayed by 1 repeat, so no read-write conflict with src1.

### Scenario 8: src0 Repeatedly Reads the Same Datablock (src0BlkStride=0, src0RepStride=8), src1/dst Addresses Overlap — No Conflict

> **Special configuration**: src0's `src0BlkStride=0` (repeatedly reads the same datablock within a single repeat), `src0RepStride=8` (each repeat is spaced by 8 datablocks, i.e., 256B).

#### Address Decoding

| Operand | Base Address | 18-bit Binary (BANK DEPTH BG WIDTH) | bank_id | bank_group_id | Global Bank Number |
| --- | --- | --- | --- | --- | --- |
| src0 | `0x0` | `00 0000000 0000 00000` | 0 | 0 | 0 |
| src1 | `0x10100` | `01 0000000 1000 00000` | 1 | 8 | 24 |
| dst | `0x10100` | `01 0000000 1000 00000` | 1 | 8 | 24 |

#### UB Read/Write Illustration

![scenario8](./figures/scenario8.png)

- The same 1 green data block (32B) is repeatedly read 8 times per repeat (src0BlkStride=0);
- Every 8 yellow data blocks represent each repeat, where src1 reads and dst writes the same data space (256B), i.e., src1 and dst addresses completely overlap;
- Number of iterations required to process all data: `repeat_times = 4096 * 4 / 256 = 64`.

#### Analysis Conclusion

- Read-read conflict analysis: In the 1st repeat, src0 (bank_id=0) reads only a single datablock, which is not in the same bank group as src1 (bank_id=1, bg 8-15); in subsequent repeats src0's bank group alternates as bg 0, 8, 0, 8, ..., always staggered from src1's bank group range — overall **no read-read conflict**.
- Read-write conflict analysis: In the 1st repeat, src1 reads (bank_id=1) and dst writes (bank_id=1) have completely overlapping addresses; the hardware automatically delays dst writes by 1 repeat, staggering the read and write operations; subsequent repeats follow the same pattern, so **no read-write conflict**.

**UB Read/Write Process Illustration**:

| | 1st repeat | 2nd repeat | 3rd repeat | 4th repeat | ... |
| --- | --- | --- | --- | --- | --- |
| src0 read | bank 0 (bg 0) | bank 8 (bg 8) | bank 0 (bg 0) | bank 8 (bg 8) | ... |
| src1 read | bank 24-31 (bg 8-15) | bank 16-23 (bg 0-7) | bank 24-31 (bg 8-15) | bank 16-23 (bg 0-7) | ... |
| dst write | - | bank 24-31 (bg 8-15) | bank 16-23 (bg 0-7) | bank 24-31 (bg 8-15) | ... |

Analysis results:
- src0 (bg 0 or 8) and src1 (bg 0-7 or 8-15) never overlap: when src0 is at bg 0, src1 is at bg 8-15; when src0 is at bg 8, src1 is at bg 0-7. Therefore, no read-read conflict.
- dst writes are delayed by 1 repeat, so no read-write conflict with src1.

> **Comparison with Scenario 7**: In Scenario 7, `src0RepStride=1` causes src0 to traverse bank groups incrementally with stride 1 (bg 0, 1, 2, 3, ...), frequently overlapping with src1's bank group range; in Scenario 8, `src0RepStride=8` makes src0 alternate between bg 0 and 8, perfectly staggered from src1. This effectively eliminates read-read conflicts.

### Summary

- Read-read conflict: as long as two read operations access the same bank group (regardless of whether bank_id is the same), a read-read conflict is triggered.
- Read-write conflict: when read and write operations access the same bank, a read-write conflict is triggered (a single bank can only perform 1 read or 1 write per cycle). Exception: when the first datablock of the read and write operations in a repeat are in the same bank, and the subsequent 7 datablocks are also each in the same bank respectively, the hardware can delay writes by 1 repeat to avoid the read-write conflict.
- blkStride=0 scenario: no repeated reads within a repeat — only 1 read is performed. repStride=0 scenario: no repeated reads across repeats — only 1 read is performed.

## Sample Specifications

**Table 1**  Sample Input/Output Specifications

| Category | name | shape | data type | format |
| --- | --- | --- | --- | --- |
| Sample Input | x | [1, 4096] | float | ND |
| Sample Input | y | [1, 4096] | float | ND |
| Sample Output | z | [1, 4096] | float | ND |
| Kernel Function | add_custom | - | - | - |

## Build and Run

Execute the following steps in the root directory of this sample to build and run it.

- Configure environment variables

  Based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit package in your current environment, configure the environment variables.

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation path is specified, the default installation location is `/usr/local/Ascend`.

- Single scenario test

  ```bash
  SCENARIO_NUM=1    # Set scenario number
  mkdir -p build && cd build;                              # Create and enter build directory
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;  # Build the project
  python3 ../scripts/gen_data.py --scenario $SCENARIO_NUM          # Generate test input data
  ./demo                                                   # Execute the compiled program to run the sample
  python3 ../scripts/verify_result.py output/output_z.bin output/golden.bin  # Verify whether the output is correct
  ```

- Build option description

  | Option | Valid Values | Description |
  |--------|--------------|-------------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` | NPU architecture: dav-2201 corresponds to Atlas A2 training series products / Atlas A2 inference series products and Atlas A3 training series products / Atlas A3 inference series products |
  | `SCENARIO_NUM` | `1`~`8` | Scenario number: 1 (no-conflict baseline), 2 (same-bank read-read conflict), 3 (same-bank-group read-read conflict), 4 (same-bank read-write conflict — hardware optimized), 5 (read-write conflict), 6 (address overlap — hardware optimized), 7 (src0RepStride=1 read-read conflict), 8 (src0RepStride=8 no conflict) |

- Execution result

  The execution result is as follows, indicating that the accuracy comparison passed successfully.

  ```bash
  test pass!
  ```
