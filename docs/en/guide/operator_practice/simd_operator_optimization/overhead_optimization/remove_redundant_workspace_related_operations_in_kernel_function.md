# Removing Redundant Workspace Operations in the Kernel Function<a name="ZH-CN_TOPIC_0000002499260290"></a>

<!-- md-trans-meta sourceCommit=51eb188d02e4be270d9dc56141153e3f75345dc0 translatedAt=2026-08-26T14:32:37.108Z -->

[Priority] Medium

[Description] In an Ascend C operator project, the workspace parameter passed when writing a kernel function is already directly assigned to the user workspace. Therefore, there is no need to call SetSysWorkspace and GetUserWorkspace to set and obtain the workspace. After removing these redundant checks, the compiler can further optimize the unused workspace variable when the parameter is not used.

[Negative Example]

The workspace parameter of the fast\_gelu function is equivalent to the user workspace and is not null. However, the function still performs a null check on workspace and calls SetSysWorkspace and GetUserWorkspace to obtain the user workspace.

```
template <uint64_t schMode, uint64_t dType>
__global__ __aicore__ void fast_gelu(GM_ADDR x, GM_ADDR y, GM_ADDR workspace, GM_ADDR tiling)
{
    // Counterexample: redundant check
    if (workspace == nullptr) {
        return;
    }
    SetSysWorkspace(workspace);
    GM_ADDR userWS = GetUserWorkspace(workspace);
    if (userWS == nullptr) {
        return;
    }
    REGISTER_TILING_DEFAULT(EleBaseTilingDataV2);
    GET_TILING_DATA_WITH_STRUCT(EleBaseTilingDataV2, tilingData, tiling);
    KERNEL_TASK_TYPE_DEFAULT(KERNEL_TYPE_AIV_ONLY);
    TPipe pipe;
    if constexpr (dType == static_cast<uint64_t>(TPL_FP16)) {
        ElementwiseSch<schMode, FastGeluDag::FastGeluNeedCast<half>::OpDag> sch(&tilingData, &pipe);
        sch.Init(x, y);
        sch.Process();
    } else if constexpr (dType == static_cast<uint64_t>(TPL_BF16)) {
        ElementwiseSch<schMode, FastGeluDag::FastGeluNeedCast<bfloat16_t>::OpDag> sch(&tilingData, &pipe);
        sch.Init(x, y);
        sch.Process();
    } else if constexpr (dType == static_cast<uint64_t>(TPL_FP32)) {
        ElementwiseSch<schMode, FastGeluDag::FastGeluNoCast<float>::OpDag> sch(&tilingData, &pipe);
        sch.Init(x, y);
        sch.Process();
    }
}
```

[Positive Example]

In the fast\_gelu function, remove the null pointer check on the workspace parameter. There is also no need to call SetSysWorkspace or obtain the workspace through GetUserWorkspace.

```
template <uint64_t schMode, uint64_t dType>
__global__ __aicore__ void fast_gelu(GM_ADDR x, GM_ADDR y, GM_ADDR workspace, GM_ADDR tiling)
{
    REGISTER_TILING_DEFAULT(EleBaseTilingDataV2);
    GET_TILING_DATA_WITH_STRUCT(EleBaseTilingDataV2, tilingData, tiling);
    KERNEL_TASK_TYPE_DEFAULT(KERNEL_TYPE_AIV_ONLY);
    TPipe pipe;
    if constexpr (dType == static_cast<uint64_t>(TPL_FP16)) {
        ElementwiseSch<schMode, FastGeluDag::FastGeluNeedCast<half>::OpDag> sch(&tilingData, &pipe);
        sch.Init(x, y);
        sch.Process();
    } else if constexpr (dType == static_cast<uint64_t>(TPL_BF16)) {
        ElementwiseSch<schMode, FastGeluDag::FastGeluNeedCast<bfloat16_t>::OpDag> sch(&tilingData, &pipe);
        sch.Init(x, y);
        sch.Process();
    } else if constexpr (dType == static_cast<uint64_t>(TPL_FP32)) {
        ElementwiseSch<schMode, FastGeluDag::FastGeluNoCast<float>::OpDag> sch(&tilingData, &pipe);
        sch.Init(x, y);
        sch.Process();
    }
}
```

