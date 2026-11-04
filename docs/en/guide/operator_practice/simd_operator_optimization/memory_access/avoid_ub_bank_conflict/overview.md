# Overview<a name="ZH-CN_TOPIC_0000002534006769"></a>

<!-- md-trans-meta sourceCommit=ee355529a5d6098b1c84bfe577b9776e4ec7583b translatedAt=2026-10-23T12:45:59.263Z pushedAt=2026-11-03T07:05:13.342Z nl=2 -->

[Priority] High

[Description]

To improve the efficiency and throughput of data access, the Unified Buffer (UB) adopts a structure of equal-sized memory modules (banks). When multiple read/write instructions access the UB at the same time, these instructions cannot be executed simultaneously due to hardware resource limitations, resulting in bank conflicts. In this case, the instructions need to wait in a queue for resources and cannot be completed within one instruction cycle.

-   For [NPU architecture version 2201](../../../../programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)

    ![](../../../../figures/mat_pos_65.png)

    The total UB size is 192KB, containing 16 bank groups, and each bank group contains 3 banks. Each bank is 4KB in size and consists of 128 rows, with each row being 32B in length.

    -   **Read-write conflict**: A read operation and a write operation attempt to access the same bank at the same time.
    -   **Write-write conflict**: Multiple write operations attempt to access the same bank group at the same time.
    -   **Read-read conflict**: Multiple read operations attempt to access the same bank group at the same time.

-   For [NPU architecture version 3510](../../../../programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)

    ![](../../../../figures/mat_pos_66.png)

    The total size of the UB is 256 KB, which contains 8 bank groups, and each bank group contains 2 banks. Each bank is 16 KB in size and consists of 512 rows, with each row being 32B in length.

    -   **Read-write conflict**: A read operation and a write operation attempt to access the same bank at the same time.
    -   **Write-write conflict**: Multiple write operations attempt to access the same bank group at the same time.
    -   **Read-read conflict**: Two read operations attempt to access the same bank at the same time, or more than two read operations attempt to access the same bank group at the same time.

It can be seen that bank conflict scenarios are closely related to the UB specifications, and changes in the specifications usually lead to changes in the bank conflict scenarios.

-   In NPU architecture version 3510, a single bank group supports two read operations in a single beat. Therefore, when two read operations access different banks of the same bank group, no conflict occurs; if the two read operations access the same bank, or more than two read operations occur within the same bank group, a conflict occurs.
-   Assume that the address operated by a read instruction is 0x0000 (bank0) and the address operated by a write instruction is 0x10000. In NPU architecture version 2201, the address 0x10000 (bank16) does not cause a read-write conflict, whereas in NPU architecture version 3510, this address 0x10000 (bank0) causes a read-write conflict.

The following describes how to avoid bank conflicts under different hardware architectures.
