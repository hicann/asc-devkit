# GetSysWorkSpacePtr<a name="ZH-CN_TOPIC_0000001666431622"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:15:25.700Z pushedAt=2026-10-26T07:41:44.747Z nl=2 -->

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
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling); // Initialize and pass in the system workspace pointer.
// CopyIn phase: Complete the data transfer from GM to LocalMemory.
mm.SetTensorA(gm_a);    // Set the left matrix A.
mm.SetTensorB(gm_b);    // Set the right matrix B.
mm.SetBias(gm_bias);    // Set the bias.
// Compute phase: Complete the matrix multiplication.
while (mm.Iterate()) { 
    // CopyOut phase: Complete the data transfer from LocalMemory to GM.
    mm.GetTensorC(gm_c); 
}
// End the matrix multiplication operation.
mm.End();
```
