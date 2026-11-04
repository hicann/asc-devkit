# L0C Buffer Memory Structure Introduction<a id="ZH-CN_TOPIC_0000002538071268"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T12:37:18.997Z pushedAt=2026-10-26T07:41:44.124Z nl=2 -->

To improve data access efficiency and throughput, the L0C Buffer adopts an equal-capacity memory module (Bank) structure design. The memory space is divided into multiple Banks, which are further grouped into multiple Bank Groups (BGs) by Bank. Each Bank Group contains several Banks, and each Bank is a storage unit submodule that can be operated independently.

<!-- npu="A3,910b" id2 -->
Take the following product models as an example:
<!-- npu="A3" id3 -->
Atlas A3 products
<!-- end id3 -->
<!-- npu="910b" id4 -->
Atlas A2 products
<!-- end id4 -->
The total capacity of the L0C Buffer is 128 KB, consisting of 16 Banks. Each Bank has a capacity of 8 KB, consisting of 128 rows, with each row being 64 bytes wide. These 16 Banks are further organized into 16 Bank Groups. Each Bank Group contains 1 Bank.

The memory structure of the L0C Buffer is shown in [Figure 1](#zh-cn_topic_0000002555148561_fig1715315610160). The address encoding format of the L0C Buffer and the meanings of its bits are described as follows:

```text
L0C_ADDR[16:0] = { BANK_DEPTH[6:0], BANK[3:0], BANK_WIDTH[5:0] }    // The bit order is from high to low, 17 bits in total.
// BANK_DEPTH indicates the row number of the address in the Bank, occupying 7 bits, with a value range of [0, 127].
// BANK indicates the number of the Bank where the address is located, occupying 4 bits, with a value range of [0, 15].
// BANK_WIDTH indicates the offset of an address within a Bank row, occupying 6 bits, with a value range of [0, 63].
```

**Figure 1** L0C Buffer memory structure diagram ([NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))<a id="zh-cn_topic_0000002555148561_fig1715315610160"></a>  

![](../../../../figures/L0C_Memory_Structure.png)
<!-- end id2 -->

<!-- npu="950" id1 -->
Taking the Ascend 950PR&950DT products as an example, the total capacity of the L0C Buffer is 256K bytes, consisting of 16 Banks. Each Bank has a capacity of 16K bytes and consists of 256 rows, with each row being 64 bytes wide. These 16 Banks are further organized into 16 Bank Groups. Each Bank Group contains 1 Bank.

The memory structure of the L0C Buffer is shown in [Figure 2](#fig179810301543). The address encoding format of the L0C Buffer and the meaning of each bit are described as follows:

```text
L0C_ADDR[17:0] = { BANK_DEPTH[7:0], BANK[3:0], BANK_WIDTH[5:0] }    // The bit order is from high to low, totaling 18 bits.
// BANK_DEPTH indicates the row number of an address within a Bank, occupying 8 bits, with a value range of [0, 255].
// BANK indicates the number of the Bank where the address is located, occupying 4 bits, with a value range of [0, 15].
// BANK_WIDTH indicates the offset of an address within a Bank row, occupying 6 bits, with a value range of [0, 63].
```

**Figure 2** L0C Buffer memory structure diagram ([NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))<a id="fig179810301543"></a>  

![](../../../../figures/L0C_Memory_Structure_950.png)
<!-- end id1 -->
