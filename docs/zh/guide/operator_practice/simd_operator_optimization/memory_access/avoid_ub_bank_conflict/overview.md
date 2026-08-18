# 概述<a name="ZH-CN_TOPIC_0000002534006769"></a>

【优先级】高

【概述】

为了提高数据访问的效率和吞吐量，Unified Buffer（UB）采用了大小相等的内存模块（bank）结构设计。当多条读写指令同时访问UB时，由于硬件资源的限制，这些指令不能同时执行，从而引发bank冲突。在这种情况下，指令需要排队等待资源，无法在一个指令周期内完成。

-   针对[NPU架构版本2201](../../../../programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)

    ![](../../../../figures/mat_pos_65.png)

    UB总大小为192KB，包含16个bank group，每个bank group包含3个bank。每个bank大小为4KB，由128行组成，每行长度为32B。

    -   **读写冲突**：读操作和写操作同时尝试访问同一个bank。
    -   **写写冲突**：多个写操作同时尝试访问同一个bank group。
    -   **读读冲突**：多个读操作同时尝试访问同一个bank group。

-   针对[NPU架构版本3510](../../../../programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)

    ![](../../../../figures/mat_pos_66.png)

    UB总大小为256KB，包含8个bank group，每个bank group包含2个bank。每个bank大小为16KB，由512行组成，每行长度为32B。

    -   **读写冲突**：读操作和写操作同时尝试访问同一个bank。
    -   **写写冲突**：多个写操作同时尝试访问同一个bank group。
    -   **读读冲突**：两个读操作同时尝试访问同一个bank，或者两个以上读操作同时尝试访问同一个bank group。

可以看出bank冲突的场景与UB的规格密切相关，规格的变化通常会导致bank冲突场景的变化。

-   由于NPU架构版本3510的bank group上有两组读口和写口，因此两次读操作访问同一个bank group的不同bank时，不会引起冲突。
-   假设读指令操作的地址为0x0000（bank0），写指令操作的地址为0x10000 ，在NPU架构版本2201中，地址0x10000（bank16）不会发生读写冲突，而在NPU架构版本3510中，这个地址0x10000（bank0）会引发读写冲突。

下文介绍不同硬件架构下如何避免bank冲突。

