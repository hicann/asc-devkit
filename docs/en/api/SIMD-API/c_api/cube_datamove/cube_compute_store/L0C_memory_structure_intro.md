---
okftype: reference.concept
description: "Describes the equal-capacity storage structure of the L0C Buffer organized by Bank and Bank Group, provides the bit-field meanings of address encoding under two specifications: 128KB for the 2201 architecture and 256KB for the 3510 architecture, and points out the differences in C API addressing regarding result fractal alignment, move-out stride unit, and UnitFlag blocks."
---
# L0C Buffer Memory Structure

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:23:29.873Z pushedAt=2026-11-18T08:15:34.312Z nl=2 -->

To improve data access efficiency and throughput, the L0C Buffer adopts an equal-capacity memory module (Bank) structure design. The memory space is divided into multiple Banks, which are further grouped by Bank to form multiple Bank Groups (BG); each Bank Group contains several Banks, and each Bank is an independently operable storage unit submodule.

<!-- npu="A3,910b" id1 -->
Take the following product models as examples:
<!-- npu="A3" id2 -->
Atlas A3 products
<!-- end id2 -->
<!-- npu="910b" id3 -->
Atlas A2 products
<!-- end id3 -->
The total capacity of the L0C Buffer is 128K bytes, consisting of 16 Banks. Each Bank has a capacity of 8K bytes, consisting of 128 rows, with each row being 64 bytes wide. These 16 Banks are further organized into 16 Bank Groups. Each Bank Group contains 1 Bank.

The memory structure of the L0C Buffer is shown in Figure 1 below. The address encoding format of the L0C Buffer and the meaning of each bit field are described as follows:

```text
L0C_ADDR[16:0] = { BANK_DEPTH[6:0], BANK[3:0], BANK_WIDTH[5:0] }    // Bit order is from high to low, 17 bits in total
// BANK_DEPTH indicates the row number of the address within the Bank, occupying 7 bits, with a value range of [0, 127]
// BANK indicates the number of the Bank where the address is located, occupying 4 bits, with a value range of [0, 15]
// BANK_WIDTH indicates the offset of an address within a Bank row, occupying 6 bits, with a value range of [0, 63]
```

**Figure 1** L0C Buffer memory structure diagram ([NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))

![](../../../../figures/L0C_Memory_Structure.png)
<!-- end id1 -->

<!-- npu="950" id4 -->
Taking the Ascend 950PR&950DT products as an example, the total capacity of the L0C Buffer is 256K bytes, consisting of 16 Banks. Each Bank has a capacity of 16K bytes, consisting of 256 rows, with each row being 64 bytes wide. These 16 Banks are further organized into 16 Bank Groups. Each Bank Group contains 1 Bank.

The memory structure of the L0C Buffer is shown in Figure 2 below. The address encoding format of the L0C Buffer and the meaning of each bit field are described as follows:

```text
L0C_ADDR[17:0] = { BANK_DEPTH[7:0], BANK[3:0], BANK_WIDTH[5:0] }    // The bit order is from most significant to least significant, 18 bits in total
// BANK_DEPTH indicates the row number of an address within a Bank, occupying 8 bits, with a value range of [0, 255]
// BANK indicates the number of the Bank where the address resides, occupying 4 bits, with a value range of [0, 15]
// BANK_WIDTH indicates the offset of an address within a Bank row, occupying 6 bits, with a value range of [0, 63]
```

**Figure 2** L0C Buffer memory structure diagram ([NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))

![](../../../../figures/L0C_Memory_Structure_950.png)
<!-- end id4 -->

## C API Addressing and Synchronization

The L0C Buffer uses `__cc__ float*` or `__cc__ int32_t*` to store accumulation results. A 16×16 result fractal occupies 1024B, and its address must satisfy the alignment requirements of both the compute and move-out interfaces. The move-out start address is aligned to 64B, and `src_stride` is counted in units of 64B; the 512B input fractal unit of the L0A Buffer/L0B Buffer cannot be used as the stride unit.

[UnitFlag](../../cube_compute/mmad_compute_key_features/UnitFlag.md) manages read/write states in 512B blocks, and a complete 1024B result fractal involves two flag blocks. The Bank structure, result fractal, and UnitFlag block are different concepts.
