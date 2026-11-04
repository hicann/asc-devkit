# L0C Buffer Memory Structure

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-22T13:39:33.435Z pushedAt=2026-10-28T12:45:45.498Z nl=2 -->

To improve data access efficiency and throughput, the L0C Buffer adopts a memory structure composed of equal-capacity memory modules (Banks). The memory space is divided into multiple Banks, which are further grouped into multiple Bank Groups (BGs). Each Bank Group contains several Banks, and each Bank is an independently operable storage submodule.

<!-- npu="A3,910b" id1 -->
The following product models are used as examples:
<!-- npu="A3" id2 -->
Atlas A3 products
<!-- end id2 -->
<!-- npu="910b" id3 -->
Atlas A2 products
<!-- end id3 -->
The total capacity of the L0C Buffer is 128 KB, consisting of 16 Banks. Each Bank has a capacity of 8 KB and consists of 128 rows, with each row being 64 bytes wide. These 16 Banks are further organized into 16 Bank Groups, with each Bank Group containing 1 Bank.

The memory structure of the L0C Buffer is shown in Figure 1 below. The address encoding format of the L0C Buffer and the meaning of each bit are described as follows:

```text
L0C_ADDR[16:0] = { BANK_DEPTH[6:0], BANK[3:0], BANK_WIDTH[5:0] }    // Bit order is from most significant to least significant, 17 bits in total.
// BANK_DEPTH indicates the row number of the address within a Bank, occupying 7 bits, with a value range of [0, 127].
// BANK indicates the number of the Bank where the address resides, occupying 4 bits, with a value range of [0, 15].
// BANK_WIDTH indicates the offset of the address within a row of a Bank, occupying 6 bits, with a value range of [0, 63].
```

**Figure 1** L0C Buffer memory structure diagram ([NPU architecture version 2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))

![](../../../figures/L0C_Memory_Structure.png)
<!-- end id1 -->

<!-- npu="950" id4 -->
Taking the Ascend 950PR&950DT products as an example, the L0C Buffer has a total capacity of 256K bytes and consists of 16 banks. Each bank has a capacity of 16K bytes and consists of 256 rows, with each row being 64 bytes wide. These 16 banks are further organized into 16 bank groups. Each bank group contains one bank.

The memory structure of the L0C Buffer is shown in Figure 2 below. The address encoding format of the L0C Buffer and the meaning of each bit are described as follows:

```text
L0C_ADDR[17:0] = { BANK_DEPTH[7:0], BANK[3:0], BANK_WIDTH[5:0] }    // The bit order is from the most significant bit to the least significant bit, 18 bits in total.
// BANK_DEPTH indicates the row number of the address within a bank, occupying 8 bits, with a value range of [0, 255].
// BANK indicates the number of the bank where the address resides, occupying 4 bits, with a value range of [0, 15].
// BANK_WIDTH indicates the offset of the address within a row of a bank, occupying 6 bits, with a value range of [0, 63].
```

**Figure 2** L0C Buffer memory structure diagram ([NPU architecture version 3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))

![](../../../figures/L0C_Memory_Structure_950.png)
<!-- end id4 -->

## C API Addressing and Synchronization

The L0C Buffer stores accumulation results using `__cc__ float*` or `__cc__ int32_t*`. A 16×16 result fractal occupies 1024B, and its address must satisfy the alignment requirements of both the compute and the move-out interfaces. The move-out start address is aligned to 64B, and `src_stride` is counted in units of 64B. The 512B input fractal unit of the L0A Buffer/L0B Buffer cannot be used as the stride unit.

[UnitFlag](../cube_compute/mmad_compute_key_features/UnitFlag.md) manages read/write states in 512B blocks, and a complete 1024B result fractal involves two flag blocks. The Bank structure, the result fractal, and the UnitFlag block are distinct concepts.
