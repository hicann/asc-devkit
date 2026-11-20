---
okftype: reference.concept
description: "Introduces the Bank and Bank Group memory structure of the Cube-side L1 Buffer, L0A Buffer, and L0B Buffer, provides the capacity, number of Banks, row width, and address encoding bit-field division of each Buffer, and explains the read/write concurrency restrictions within the same Bank and the same Bank Group."
---
# L1 Buffer, L0A Buffer/L0B Buffer Memory Structure

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:08:14.104Z pushedAt=2026-11-18T08:15:34.295Z nl=2 -->

To improve data access efficiency and throughput, the Buffer adopts a structure of equal-capacity memory modules (Banks). The memory space is divided into multiple Banks, which are further grouped by Bank into multiple Bank Groups (BGs); each Bank Group contains several Banks, and each Bank is an independently operable storage unit submodule.

The following describes the detailed structure of the Cube-side memories (L1 Buffer, L0A Buffer, and L0B Buffer).

## L1 Buffer Memory Structure

The total capacity of the L1 Buffer is 512K bytes, consisting of 16 Banks. Each Bank has a capacity of 32K bytes, consisting of 1024 rows, with each row being 32 bytes wide. **The same Bank allows at most one read or one write at the same time**.

These 16 Banks are further organized into 8 Bank Groups. Each Bank Group contains 2 Banks (Bank0 and Bank1). **Two Banks belonging to the same Bank Group support one read and the other write, but do not support simultaneous reads or simultaneous writes**.

The memory structure of the L1 Buffer is shown in Figure 1 below, and the address encoding format is as follows:

```cpp
L1_ADDR[18:0] = {BANK[0:0], BANK_DEPTH[9:0], BG[2:0], BANK_WIDTH[4:0]}    // The bit order is from high to low, 19 bits in total.
// BANK indicates the number of the Bank where the address is located, occupying 1 bit, with a value range of [0, 1].
// BANK_DEPTH indicates the row number of the address within the Bank, occupying 10 bits, with a value range of [0, 1023].
// BG indicates the number of the Bank Group where the address is located, occupying 3 bits, with a value range of [0, 7].
// BANK_WIDTH indicates the offset of the address within a row of the Bank, occupying 5 bits, with a value range of [0, 31].
```

**Figure 1** L1 Buffer memory structure diagram  
![](../../../../figures/l1_memory_bank.png "L1 Buffer memory structure diagram")

## L0A Buffer/L0B Buffer Memory Structure

The L0A Buffer and L0B Buffer each have a capacity of 64K bytes, contain only 1 Bank, have 128 rows, and each row is 512 bytes wide. **Simultaneous read and write to the same address is not allowed. Cube load-in writing and asc_mmad reading different addresses of the same Bank at the same time are allowed.**

The memory structure of the L0A Buffer/L0B Buffer is shown in Figure 2 below. The L0A Buffer and L0B Buffer use the same address encoding format:

```cpp
L0A_ADDR[15:0] = { BANK_DEPTH[6:0], BANK_WIDTH[8:0] }    // Bit order is from high to low, 16 bits in total.
L0B_ADDR[15:0] = { BANK_DEPTH[6:0], BANK_WIDTH[8:0] }
// BANK_DEPTH indicates the row number of the address in the Bank, occupying 7 bits, with a value range of [0, 127].
// BANK_WIDTH indicates the offset of the address within a Bank row, occupying 9 bits, with a value range of [0, 511].
```

**Figure 2** L0A Buffer/L0B Buffer memory structure diagram  
![](../../../../figures/l0al0b_memory_bank.png "L0A Buffer/L0B Buffer memory structure diagram")

For layout constraints and load-in interface selection, see [Cube Compute Input Move Constraint](cube_compute_input_move_constraint.md) and [Cube Load-in Overall Description](cube_compute_load_overall_description.md) respectively; for the fractal layout of cube data, see [Key Fractal Format Details](../../cube_compute/cube_compute_fractal_intro/key_fractal_format_details.md).
