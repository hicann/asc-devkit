# L0C Buffer Memory Structure<a id="ZH-CN_TOPIC_0000002538071268"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T15:49:03.451Z -->

To improve data access efficiency and throughput, the L0C Buffer adopts a memory module (**Bank**) structure design with equal capacity. The memory space is divided into multiple **Banks**, which are further grouped into multiple **Bank Groups** (BGs). Each **Bank Group** contains several **Banks**, and each **Bank** is an independently operable storage unit submodule.

<!-- npu="A3,910b" id2 -->
The following product models are used as examples:
<!-- npu="A3" id3 -->
Atlas A3 training products/Atlas A3 inference products
<!-- end id3 -->
<!-- npu="910b" id4 -->
Atlas A2 training products/Atlas A2 inference products
<!-- end id4 -->
The total capacity of the L0C Buffer is 128 KB, consisting of 16 **Banks**. Each **Bank** has a capacity of 8 KB, consisting of 128 rows, with each row being 64 bytes wide. These 16 **Banks** are further organized into 16 **Bank Groups**. Each **Bank Group** contains 1 **Bank**.

The memory structure of the L0C Buffer is shown in [Figure 1](#zh-cn_topic_0000002555148561_fig1715315610160). The address encoding format of the L0C Buffer and the meanings of its bits are described as follows:

```text
L0C_ADDR[16:0] = { BANK_DEPTH[6:0], BANK[3:0], BANK_WIDTH[5:0] }    // The bit order is from high to low, with a total of 17 bits.
// BANK_DEPTH indicates the number of rows of the address in the Bank, occupying 7 bits, with a value range of [0, 127].
// BANK indicates the number of the Bank where the address resides, occupying 4 bits, with a value range of [0, 15].
// BANK_WIDTH indicates the offset of the address within a row of the Bank, occupying 6 bits, with a value range of [0, 63].
```

**Figure 1** L0C Buffer memory structure diagram ([NPU architecture version 2201](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMD-BuiltIn Keywords.md))<a id="zh-cn_topic_0000002555148561_fig1715315610160"></a>  

![](../../../../figures/L0C_Memory_Structure.png)
<!-- end id2 -->

<!-- npu="950" id1 -->
Taking Ascend 950PR/Ascend 950DT as an example, the total capacity of the L0C Buffer is 256 KB, consisting of 16 Banks. Each Bank has a capacity of 16 KB, consisting of 256 rows, with each row being 64 bytes wide. These 16 Banks are further organized into 16 Bank Groups. Each Bank Group contains 1 Bank.

The memory structure of the L0C Buffer is shown in [Figure 2](#fig179810301543). The address encoding format of the L0C Buffer and the meaning of each bit are described as follows:

```text
L0C_ADDR[17:0] = { BANK_DEPTH[7:0], BANK[3:0], BANK_WIDTH[5:0] }    // The bit order is from the most significant bit to the least significant bit, totaling 18 bits.
// BANK_DEPTH indicates the number of rows of the address within the Bank, occupying 8 bits, with a value range of [0, 255].
// BANK indicates the number of the Bank where the address is located, occupying 4 bits, with a value range of [0, 15].
// BANK_WIDTH indicates the offset of the address within a row of the Bank, occupying 6 bits, with a value range of [0, 63].
```

**Figure 2** L0C Buffer memory structure diagram ([NPU architecture version 3510](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMD-BuiltIn Keywords.md))<a id="fig179810301543"></a>  

![](../../../../figures/L0C_Memory_Structure_950.png)
<!-- end id1 -->
