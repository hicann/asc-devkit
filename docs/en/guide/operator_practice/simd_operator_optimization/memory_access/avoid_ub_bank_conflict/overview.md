# Overview<a name="ZH-CN_TOPIC_0000002534006769"></a>

<!-- md-trans-meta sourceCommit=49eda737d7d7268a9ddb02fcaba4732176bfc076 translatedAt=2026-11-09T10:35:31.619Z pushedAt=2026-11-19T06:32:19.771Z nl=2 -->

[Priority] High

[Overview]

To improve the efficiency and throughput of data access, the Unified Buffer (UB) adopts a structure of equal-sized memory modules (banks). When multiple read/write instructions access the UB at the same time, these instructions cannot be executed simultaneously due to hardware resource limitations, resulting in bank conflicts. In this case, the instructions need to queue for resources and cannot be completed within a single instruction cycle.

-   For [NPU architecture version 2201](../../../../programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)

    <img src="../../../../figures/mat_pos_65.png" alt="" width="920px" height="497px">

    The total UB size is 192 KB, containing 16 bank groups, each of which contains 3 banks. Each bank is 4 KB in size and consists of 128 rows, with each row being 32B in length.

    -   **Read-write conflict**: A read operation and a write operation attempt to access the same bank at the same time.
    -   **Write-write conflict**: Multiple write operations attempt to access the same bank group at the same time.
    -   **Read-read conflict**: Multiple read operations attempt to access the same bank group at the same time.

-   For [NPU architecture version 3510](../../../../programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)

    <img src="../../../../figures/mat_pos_66.png" alt="" width="920px" height="670px">

    The total UB size is 256 KB, containing 8 bank groups, each of which contains 2 banks. Each bank is 16 KB in size and consists of 512 rows, with each row being 32B in length.

    -   **Read-write conflict**: A read operation and a write operation attempt to access the same bank at the same time.
    -   **Write-write conflict**: Multiple write operations attempt to access the same bank group at the same time.
    -   **Read-read conflict**: Two read operations attempt to access the same bank at the same time, or more than two read operations attempt to access the same bank group at the same time.

It can be seen that bank conflict scenarios are closely related to the UB specifications. Changes in the specifications usually lead to changes in bank conflict scenarios.

-   In NPU architecture version 3510, the same bank group supports two read operations per cycle. Therefore, when two read operations access different banks of the same bank group, no conflict occurs. If the two read operations access the same bank, or more than two read operations occur within the same bank group, a conflict occurs.
-   Assume that the address operated by a read instruction is 0x0000 (bank0) and the address operated by a write instruction is 0x10000. In NPU architecture version 2201, the address 0x10000 (bank16) does not cause a read-write conflict, whereas in NPU architecture version 3510, the address 0x10000 (bank0) causes a read-write conflict.

The following describes how to avoid bank conflicts under different hardware architectures.
