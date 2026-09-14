# Overview<a name="ZH-CN_TOPIC_0000002534006769"></a>

<!-- md-trans-meta sourceCommit=44402ed0ea56d030eafed6c73227d7000e68bfa7 translatedAt=2026-08-26T14:23:44.163Z -->

[Priority] High

[Overview]

To improve the efficiency and throughput of data access, the Unified Buffer adopts a structure of equal-sized memory modules (banks). When multiple read/write instructions access the Unified Buffer simultaneously, these instructions cannot be executed at the same time due to hardware resource limitations, resulting in bank conflicts. In this case, the instructions need to queue for resources and cannot be completed within a single instruction cycle.

-   For [NPU architecture version 2201](../../../../programming_guide/language_extension/SIMD-BuiltIn_keyword.md#table65291052154114)

    ![](../../../../figures/matrix_programming_logical_position_diagram_65.png)

    The total UB size is 192KB, containing 16 bank groups, each of which contains 3 banks. Each bank is 4KB in size and consists of 128 rows, with each row being 32B in length.

    -   **Read-write conflict**: A read operation and a write operation attempt to access the same bank simultaneously.
    -   **Write-write conflict**: Multiple write operations attempt to access the same bank group simultaneously.
    -   **Read-read conflict**: Multiple read operations attempt to access the same bank group simultaneously.

-   For [NPU architecture version 3510](../../../../programming_guide/language_extension/SIMD-BuiltIn_keyword.md#table65291052154114)

    ![](../../../../figures/matrix_programming_logical_position_diagram_66.png)

    The total UB size is 256 KB, containing 8 bank groups, each of which contains 2 banks. Each bank is 16 KB in size and consists of 512 rows, with each row being 32 B long.

    -   **Read-write conflict**: A read operation and a write operation attempt to access the same bank simultaneously.
    -   **Write-write conflict**: Multiple write operations attempt to access the same bank group simultaneously.
    -   **Read-read conflict**: Two read operations attempt to access the same bank simultaneously, or more than two read operations attempt to access the same bank group simultaneously.

It can be seen that bank conflict scenarios are closely related to the Unified Buffer specifications, and changes in the specifications usually lead to changes in bank conflict scenarios.

-   Because the bank group of NPU architecture version 3510 has two sets of read ports and write ports, no conflict occurs when two read operations access different banks of the same bank group.
-   Assume that the address operated by a read instruction is 0x0000 (bank0) and the address operated by a write instruction is 0x10000. In NPU architecture version 2201, no read-write conflict occurs at address 0x10000 (bank16), whereas in NPU architecture version 3510, a read-write conflict occurs at address 0x10000 (bank0).

The following describes how to avoid bank conflicts under different hardware architectures.

