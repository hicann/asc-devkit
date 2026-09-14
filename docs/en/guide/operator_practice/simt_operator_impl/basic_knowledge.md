# Basic Knowledge<a name="ZH-CN_TOPIC_0000002500468254"></a>

<!-- md-trans-meta sourceCommit=5cd1ca67d9a3fb2031f31c875e7c2cd5c29578f1 translatedAt=2026-08-26T14:42:42.648Z -->

This section guides you through SIMT programming using the SIMT API.

Unlike SIMD programming, in SIMT programming, data in Global Memory can be directly read and used. SIMT programming typically implements data partitioning by organizing the thread hierarchy. It uses [SIMT built-in keywords](../../programming_guide/language_extension/SIMT-BuiltIn_keyword.md) such as threadIdx to compute the data index that each thread should process, and then performs the corresponding computation, thereby simplifying the function implementation into scalar computation.

SIMT is a single-instruction, multiple-thread programming abstraction that allows one instruction to drive multiple threads. Each thread can independently address its own data, which provides greater flexibility. As shown in the following figure, for operators with scattered access, SIMT programming is suitable, such as Scatter and Gather operators. In addition, threads can execute independently, and each thread has high flexibility to execute different logic branches to implement complex logic.

![Access diagram](../../figures/repeat_stride1-51.png)
