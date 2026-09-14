# How to Enable Vector Core in Vector Programming<a name="ZH-CN_TOPIC_0000001883181813"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-26T11:20:12.952Z pushedAt=2026-08-31T11:26:06.877Z -->

For Atlas inference products, in addition to AI Core, the hardware architecture also provides a separate Vector Core as a supplement to the vector computing unit in AI Core, thereby alleviating the vector computing bottleneck. Vector Core contains only two types of basic computing resources: the Vector Unit and the Scalar Unit, which are used to perform vector and scalar data computation respectively. During vector operator development, when Vector Core is enabled, both AI Core and Vector Core are started simultaneously during operator execution, and these cores execute the same kernel function code in parallel.

This section focuses on how to enable Vector Core in Atlas inference products. Before learning this section, you are advised to familiarize yourself with [Operator Implementation](../../../operator_practice/simd_operator_impl/vector_programming/overview.md), [Kernel Launch Based on a Sample Project](../completing_kernel_launch_based_on_sample_project.md), and [Engineering-based Operator Development](../../advanced_programming/aclnn_operator_development/overview.md), and master the end-to-end operator development process based on AI Core. On this basis, this chapter focuses on the differences when Vector Core is enabled. The details are as follows:

1.  When developing the kernel side of an operator, you need to enable Vector Core through the macro [KERNEL\_TASK\_TYPE\_DEFAULT](../../../../api/SIMD-API/basic_api/Kernel-Tiling/setting_kernel_type.md). During operator execution, both AI Core and Vector Core are started simultaneously, and AI Core is used as Vector Core. The following code sample shows how to enable Vector Core:

    ```
    extern "C" __global__ __aicore__ void add_custom(__gm__ uint8_t *x, __gm__ uint8_t *y, __gm__ uint8_t *z, __gm__ uint8_t *workspace, __gm__ uint8_t *tiling)
    {
        GET_TILING_DATA(tilingData, tiling);
        if (workspace == nullptr) {
            return;
        }
        GM_ADDR usr = AscendC::GetUserWorkspace(workspace);
        KernelAdd op;
        op.Init(x, y, z, tilingData.numBlocks, tilingData.totalLength, tilingData.tileNum);
        KERNEL_TASK_TYPE_DEFAULT(KERNEL_TYPE_MIX_VECTOR_CORE); // Enable VectorCore
        if (TILING_KEY_IS(1)) {
            op.Process1();
        } else if (TILING_KEY_IS(2)) {
            op.Process2();
        }
        // ...
    }
    ```

2.  When developing the host-side tiling, the numBlocks you set represents the total number of AI Cores and Vector Cores. For example, if you set numBlocks to 10 on the host side, a total of 10 AI Cores and Vector Cores will be started. To ensure that Vector Core is started, the value should be greater than the number of AI Cores. You can obtain the number of AI Cores through the [GetCoreNumAic](../../../../api/Utils-API/platform_info/PlatformAscendC/GetCoreNumAic.md) API and the number of Vector Cores through the [GetCoreNumVector](../../../../api/Utils-API/platform_info/PlatformAscendC/GetCoreNumVector.md) API. The following code snippets show the setting examples when using the kernel launch project and the custom operator project respectively. Here, the value is set to the sum of AI Cores and Vector Cores, indicating that all AI Cores and Vector Cores are started.
    -   Kernel launch project

        ```
        auto ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
        auto totalCoreNum = ascendcPlatform.GetCoreNumAic();
        // Replace ASCENDXXX with the actual version model
        if (ascendcPlatform.GetSocVersion() == platform_ascendc::SocVersion::ASCENDXXX) {
           totalCoreNum = totalCoreNum + ascendcPlatform.GetCoreNumVector();
        }
        ...
        kernel_name<<<totalCoreNum, 0, stream>>>(argument list);
        ```

    -   Custom operator project

        ```
        // Example of the corresponding host-side tiling function:
        ge::graphStatus TilingFunc(gert::TilingContext* context)
        {	
            // Enable Vector Core and set numBlocks to the number of vector cores in AI Core plus the number of vector cores in Vector Core.
            auto ascendcPlatform = platform_ascendc::PlatformAscendC(platformInfo);
            auto totalCoreNum = ascendcPlatform.GetCoreNumAic();
            // Replace ASCENDXXX with the actual version model.
            if (ascendcPlatform.GetSocVersion() == platform_ascendc::SocVersion::ASCENDXXX) {
               totalCoreNum = totalCoreNum + ascendcPlatform.GetCoreNumVector();
            }
            context->SetBlockDim(totalCoreNum);
        }
        ```

>[!NOTE] Note
>- Refer to the models supported by specific APIs in [Ascend C API](../../../../api/api_list.md) to determine whether the API supports Vector Core on Atlas inference products.
>- After Vector Core is enabled, AI Core and Vector Core are executed separately and scheduled through different tasks. Therefore, inter-core synchronization instructions such as IBSet, IBWait, and SyncAll are not supported.
>- When operator computation overflows (input inf/nan or the computation result exceeds the range), note that the results of AI Core and Vector Core are inconsistent. AI Core supports only the saturation mode, while Vector Core supports only the inf/nan mode.
