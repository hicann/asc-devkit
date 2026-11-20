# L0C Buffer Memory Structure Introduction<a id="ZH-CN_TOPIC_0000002538071268"></a>

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T03:57:40.048Z pushedAt=2026-11-18T01:25:06.741Z nl=2 -->

To improve data access efficiency and throughput, the L0C Buffer adopts an equal-capacity memory module (Bank) structure design. The memory space is divided into multiple Banks, which are further grouped into multiple Bank Groups (BGs) by Bank. Each Bank Group contains several Banks, and each Bank is a storage unit submodule that can be operated independently.

<!-- npu="A3,910b" id2 -->
The following product models are used as examples:
<!-- npu="A3" id3 -->
Atlas A3 products
<!-- end id3 -->
<!-- npu="910b" id4 -->
Atlas A2 products
<!-- end id4 -->
The total capacity of the L0C Buffer is 128 KB, consisting of 16 Banks. Each Bank has a capacity of 8 KB, consisting of 128 rows, with each row being 64 bytes wide. These 16 Banks are further organized into 16 Bank Groups. Each Bank Group contains 1 Bank.

The memory structure of the L0C Buffer is shown in [Figure 1](#zh-cn_topic_0000002555148561_fig1715315610160). The address encoding format of the L0C Buffer and the meanings of its bits are described as follows:

```text
L0C_ADDR[16:0] = { BANK_DEPTH[6:0], BANK[3:0], BANK_WIDTH[5:0] }    // Bits are ordered from high to low, 17 bits in total.
// BANK_DEPTH indicates the number of rows of the address in the bank, occupying 7 bits, with a value range of [0, 127].
// BANK indicates the number of the bank where the address resides, occupying 4 bits, with a value range of [0, 15].
// BANK_WIDTH indicates the offset of the address within a bank row, occupying 6 bits, with a value range of [0, 63].
```

**Figure 1** L0C Buffer memory structure diagram ([NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))<a id="zh-cn_topic_0000002555148561_fig1715315610160"></a>  

![](../../../../figures/L0C_Memory_Structure.png)
<!-- end id2 -->

<!-- npu="950" id1 -->
Taking the Ascend 950PR&950DT products as an example, the total capacity of the L0C Buffer is 256K bytes, consisting of 16 Banks. Each Bank has a capacity of 16K bytes and consists of 256 rows, with each row being 64 bytes wide. These 16 Banks are further organized into 16 Bank Groups. Each Bank Group contains 1 Bank.

The memory structure of the L0C Buffer is shown in [Figure 2](#fig179810301543). The address encoding format of the L0C Buffer and the meaning of each bit are described as follows:

```text
L0C_ADDR[17:0] = { BANK_DEPTH[7:0], BANK[3:0], BANK_WIDTH[5:0] }    // Bits are ordered from high to low, 18 bits in total.
// BANK_DEPTH indicates the number of rows of the address in the bank, occupying 8 bits, with a value range of [0, 255].
// BANK indicates the number of the bank where the address resides, occupying 4 bits, with a value range of [0, 15].
// BANK_WIDTH indicates the offset of the address within a bank row, occupying 6 bits, with a value range of [0, 63].
```

**Figure 2** L0C Buffer memory structure diagram ([NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))<a id="fig179810301543"></a>  

![](../../../../figures/L0C_Memory_Structure_950.png)
<!-- end id1 -->
