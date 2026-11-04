# Deleting Redundant Workspace Operations in the Kernel Function<a name="ZH-CN_TOPIC_0000002499260290"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T12:51:01.353Z pushedAt=2026-11-03T07:05:13.346Z nl=2 -->

[Priority] Medium

[Description] In an Ascend C operator project, the workspace parameter passed in when writing the kernel function is already directly assigned as the user workspace. Therefore, you do not need to call SetSysWorkspace and GetUserWorkspace to set and obtain the workspace. After these redundant checks are removed, the compiler can further optimize the unused workspace variable when this parameter is not used.

[Negative example]

The workspace parameter of the fast\_gelu function is equivalent to the user workspace and is not null, but the code still performs a null check on workspace and calls SetSysWorkspace and GetUserWorkspace to obtain the user workspace.

```
template <uint64_t schMode, uint64_t dType>
__global__ __aicore__ void fast_gelu(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* workspace, __gm__ uint8_t* tiling)
{
    // Negative example: redundant check
    if (workspace == nullptr) {
        return;
    }
    SetSysWorkspace(workspace);
    __gm__ uint8_t* userWS = GetUserWorkspace(workspace);
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

[Positive example]

In the fast\_gelu function, remove the null pointer check on the workspace parameter. There is also no need to call **SetSysWorkspace** or obtain the workspace through **GetUserWorkspace**.

```
template <uint64_t schMode, uint64_t dType>
__global__ __aicore__ void fast_gelu(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* workspace, __gm__ uint8_t* tiling)
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

