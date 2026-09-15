# 概述<a name="ZH-CN_TOPIC_0000002535192325"></a>

Ascend C的核心设计目标之一是实现跨代兼容。然而，每一代芯片在微架构上都存在独特的硬件特性，因此无法保证所有接口仅通过重编译就能达成完全的跨代兼容。基于这一前提，本手册旨在为Ascend C算子开发者提供系统化指导，帮助将算子核函数（Kernel）代码从当前芯片平滑移植到目标代际芯片上运行，实现高效、可控的迁移。

如[SIMD-BuiltIn关键字](../programming_guide/language_extension/simd_builtin_keywords.md)中所述，\_\_NPU\_ARCH\_\_是Device侧AI Core代码中的一个预处理宏，用于标识AI处理器的架构版本。<!-- npu="950,A3,910b" id1 -->例如，Ascend 950PR/Ascend 950DT的\_\_NPU\_ARCH\_\_为3510，而Atlas A2 训练系列产品/Atlas A2 推理系列产品和Atlas A3 训练系列产品/Atlas A3 推理系列产品的\_\_NPU\_ARCH\_\_则为2201。<!-- end id1 -->因此，后续章节将以\_\_NPU\_ARCH\_\_为主线，重点介绍相邻代际芯片间的平滑迁移方法。

> [!NOTE]
> 为保证兼容性，建议开发者在开发过程中尽量避免使用以下接口或编程方式，否则需自行保证兼容性：
> - 非Ascend C公开接口、结构体，例如`impl`目录下的接口、结构体等；
> - 对芯片规格进行硬编码，例如：
>   ```cpp
>   uint32_t MAX_UB_SIZE = 256 * 1024;  
>   ascendcPlatform.GetCoreMemSize(platform_ascendc::CoreMemType::UB, MAX_UB_SIZE);
>   ...
>   ```
> - 编译器内置API（BuiltIn API），例如：
>   ```cpp
>   copy_gm_to_ubuf(input_1_local_UB, input_1, 0, 1, 8, 0, 0);
>   ```

建议开发者采用以下实践：
- 使用兼容的API接口。
- 若涉及特定领域的扩展特性，应在Device侧通过 [`__NPU_ARCH__`编译宏](../programming_guide/language_extension/simd_builtin_keywords.md)进行隔离，在Host侧通过`SocVersion`进行隔离。
- 使用`<<<>>>`直调或 [Tiling模板编程](../programming_guide/advanced_programming/aclnn_operator_development/design_and_implementation/multi_branch_strategy.md)。
