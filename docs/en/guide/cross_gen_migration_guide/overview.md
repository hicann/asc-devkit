# Overview<a name="ZH-CN_TOPIC_0000002535192325"></a>

<!-- md-trans-meta sourceCommit=91b3c8a73f4af2b1be3f97d77021140cdbb42bc1 translatedAt=2026-08-26T12:45:51.786Z pushedAt=2026-09-11T07:06:44.685Z -->

One of the core design goals of Ascend C is to achieve cross-generation compatibility. However, each generation of chips has unique hardware characteristics in its microarchitecture, so it cannot be guaranteed that all APIs can achieve full cross-generation compatibility simply through recompilation. Based on this premise, this manual aims to provide systematic guidance for Ascend C operator developers, helping them smoothly migrate operator kernel code from the current chip to run on the target-generation chip, achieving efficient and controllable migration.

As described in [SIMD built-in keywords](../programming_guide/language_extension/SIMD-BuiltIn_keyword.md), \_\_NPU\_ARCH\_\_ is a preprocessing macro in the device-side AI Core code, used to identify the architecture version of the AI processor. For example, the \_\_NPU\_ARCH\_\_ of Ascend 950PR/Ascend 950DT is 3510, while that of Atlas A2 training products/Atlas A2 inference products and Atlas A3 training products/Atlas A3 inference products is 2201. Therefore, the subsequent chapters will focus on \_\_NPU\_ARCH\_\_ as the main thread, highlighting the smooth migration methods between adjacent chip generations.

> [!NOTE] Note
> To ensure compatibility, developers are advised to avoid using the following APIs or programming approaches during development as much as possible; otherwise, compatibility must be ensured by the developers themselves:
> - Non-public Ascend C APIs and structures, such as the APIs and structures in the `impl` directory;
> - Hardcoding chip specifications, for example:
>   ```cpp
>   constexpr uint32_t MAX_UB_SIZE = 256 * 1024;  
>   // Recommended way to obtain the value:
>   // ascendcPlatform.GetCoreMemSize(platform_ascendc::CoreMemType::UB, MAX_UB_SIZE);
>   ...
>   ```
> - Compiler built-in APIs, for example:
>   ```cpp
>   copy_gm_to_ubuf(input_1_local_UB, input_1, 0, 1, 8, 0, 0);
>   ```

Developers are advised to adopt the following practices:
- Use compatible APIs.
- If extension features in specific domains are involved, isolate them on the device side through the [`__NPU_ARCH__` compilation macro](../programming_guide/language_extension/SIMD-BuiltIn_keyword.md), and on the host side through `SocVersion`.
- Use `<<<>>>` for direct call or [tiling template programming](../programming_guide/advanced_programming/aclnn_operator_development/host_tiling_implementation/tiling_template_programming.md).
