# Basic Knowledge<a name="ZH-CN_TOPIC_0000002500468254"></a>

<!-- md-trans-meta sourceCommit=dd293a1c6b873d149bf289bb2b8098dadf9f5cbd translatedAt=2026-10-23T13:06:14.430Z pushedAt=2026-11-03T07:05:13.365Z nl=2 -->

This section guides you through SIMT programming using the SIMT APIs.

Unlike SIMD programming, in SIMT programming, data in Global Memory can be directly read and used. SIMT programming typically implements data partitioning by organizing the thread hierarchy, and uses [SIMT Built-In keywords](../../programming_guide/language_extension/simt_builtin_keywords.md) such as **threadIdx** to compute the data index that each thread should process, thereby completing the computation on the corresponding data and simplifying the function implementation into scalar computation.

SIMT is a single-instruction, multiple-thread programming abstraction that allows one instruction to drive multiple threads, with each thread independently addressing its own data, which is more flexible. As shown in the following figure, SIMT programming is suitable for operators with scattered access, such as Scatter and Gather operators. In addition, threads can execute independently, and each thread is highly flexible and can execute different logic branches to implement complex logic.

![Access diagram](../../figures/repeat_stride1_51.png)
