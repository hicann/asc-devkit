# GetSysWorkSpacePtr<a name="ZH-CN_TOPIC_0000001666431622"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T15:01:59.474Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Supported
<!-- end id7 -->


## Description<a name="section618mcpsimp"></a>

Obtains the system workspace pointer. Some high-level APIs such as **Matmul** require the system workspace, and the related APIs need to pass in the system workspace pointer, which can be obtained through this API. When using the system workspace, developers on the host side need to apply for the system workspace space by themselves, and the reserved space size can be obtained through the [GetLibApiWorkSpaceSize](../../../../Utils-API/platform_info/PlatformAscendC/GetLibApiWorkSpaceSize.md) API.

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
mm.SetTensorA(gm_a);    // Set the left cube A.
mm.SetTensorB(gm_b);    // Set the right cube B.
mm.SetBias(gm_bias);    // Set the bias.
// Compute phase: Complete the cube multiplication.
while (mm.Iterate()) { 
    // CopyOut phase: Complete the data transfer from LocalMemory to GM.
    mm.GetTensorC(gm_c); 
}
// End the cube multiplication operation.
mm.End();
```
