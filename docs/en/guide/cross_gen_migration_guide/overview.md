# Overview<a name="ZH-CN_TOPIC_0000002535192325"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T10:52:27.496Z pushedAt=2026-11-03T07:05:13.247Z nl=2 -->

One of the core design goals of Ascend C is to achieve cross-generation compatibility. However, each generation of chips has unique hardware characteristics in its microarchitecture, so it cannot be guaranteed that all APIs can achieve full cross-generation compatibility through recompilation alone. Based on this premise, this manual aims to provide systematic guidance for Ascend C operator developers, helping them smoothly migrate operator kernel code from the current chip to run on the target-generation chip, achieving efficient and controllable migration.

As described in [SIMD-Built-In Keywords](../programming_guide/language_extension/simd_builtin_keywords.md), **\_\_NPU\_ARCH\_\_** is a preprocessing macro in the device-side AI Core code, used to identify the architecture version of the AI processor. <!-- npu="950,A3,910b" id1 -->For example, the **\_\_NPU\_ARCH\_\_** of the Ascend 950PR&950DT products is 3510, while the **\_\_NPU\_ARCH\_\_** of the Atlas A2 products and Atlas A3 products is 2201. <!-- end id1 -->Therefore, the subsequent chapters will focus on **\_\_NPU\_ARCH\_\_** as the main line, highlighting the smooth migration methods between adjacent chip generations.

> [!NOTE]
> To ensure compatibility, developers are advised to avoid using the following APIs or programming methods as much as possible during development; otherwise, compatibility must be ensured by the developers themselves:
> - Non-public Ascend C APIs and structures, such as APIs and structures in the `impl` directory;
> - Hardcoding chip specifications, for example:
>   ```cpp
>   uint32_t MAX_UB_SIZE = 256 * 1024;  
>   ascendcPlatform.GetCoreMemSize(platform_ascendc::CoreMemType::UB, MAX_UB_SIZE);
>   ...
>   ```
> - Compiler built-in APIs, for example:
>   ```cpp
>   copy_gm_to_ubuf(input_1_local_UB, input_1, 0, 1, 8, 0, 0);
>   ```

Developers are advised to adopt the following practices:
- Use compatible APIs.
- If domain-specific extension features are involved, isolate them on the device side through the [`__NPU_ARCH__` compilation macro](../programming_guide/language_extension/simd_builtin_keywords.md), and on the host side through `SocVersion`.
- Use `<<<>>>` for direct calls or [Tiling template programming](../programming_guide/advanced_programming/aclnn_operator_development/design_and_implementation/multi_branch_strategy.md).
