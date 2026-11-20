# GetSysWorkSpacePtr<a name="ZH-CN_TOPIC_0000001666431622"></a>

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-05T04:40:45.236Z pushedAt=2026-11-18T01:25:31.091Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: supported
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90 processor products: Supported
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030 processor products: Supported
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/resource_management/workspace/GetSysWorkSpacePtr_res.md#id1 -->


## Description<a name="section618mcpsimp"></a>

Obtains the system workspace pointer. Some advanced APIs such as **Matmul** require the system workspace, and the related APIs need to receive the system workspace pointer, which can be obtained through this API. When using the system workspace, host-side developers need to apply for the system workspace space by themselves, and the reserved space size can be obtained through the [GetLibApiWorkSpaceSize](../../../../Utils-API/platform_info/PlatformAscendC/GetLibApiWorkSpaceSize.md) API.

## Prototype<a name="section620mcpsimp"></a>

```
__aicore__ inline __gm__ uint8_t* __gm__ GetSysWorkSpacePtr()
```

## Parameters<a name="section622mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Return Value<a name="section640mcpsimp"></a>

Returns the system workspace pointer.

## Example<a name="section642mcpsimp"></a>

```
...
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling); // Initialize and pass the system workspace pointer.
// CopyIn stage: Copy data from GM to LocalMemory.
mm.SetTensorA(gm_a);    // Set left matrix A.
mm.SetTensorB(gm_b);    // Set right matrix B.
mm.SetBias(gm_bias);    // Set the bias.
// Compute stage: Perform matrix multiplication.
while (mm.Iterate()) { 
    // CopyOut stage: Copy data from LocalMemory to GM.
    mm.GetTensorC(gm_c); 
}
// End the matrix multiplication operation.
mm.End();
```
