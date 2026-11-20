# SetUserDefInfo

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-02T06:56:42.018Z pushedAt=2026-11-18T04:54:18.976Z nl=2 -->

## Applicable Products

<!-- npu="950" id2 -->
- Ascend 950PR&950DT products: Supported
<!-- end id2 -->
<!-- npu="A3" id3 -->
- Atlas A3 products: Supported
<!-- end id3 -->
<!-- npu="910b" id4 -->
- Atlas A2 products: Supported
<!-- end id4 -->
<!-- npu="310b" id5 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
<!-- npu="x90" id1 -->
- Kirin X90 processor products: Supported
<!-- end id1 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetUserDefInfo_res.md#id1 -->

## Description

When the template parameter [MatmulCallBackFunc](MatmulCallBackFunc.md) (custom callback function) is enabled, set the operator tiling address for use by the callback function. This API needs to be called only once.

## Prototype

```
__aicore__ inline void SetUserDefInfo(const uint64_t tilingPtr)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| tilingPtr | Input | Operator tiling address to be set. |

## Return Value

None

## Constraints

-   If the callback function needs to use the **tilingPtr** parameter, this API must be called; if the callback function does not use the **tilingPtr** parameter, there is no need to call this API.
-   When MixDualMaster (dual-master mode) is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to true, this API is not supported.

## Examples

```
// User-defined callback function.
void DataCopyOut(
    const __gm__ void* gm, const LocalTensor<int8_t>& co1Local, const void* dataCopyOutParams, const uint64_t tilingPtr,
    const uint64_t dataPtr);
void CopyA1(
    const LocalTensor<int8_t>& aMatrix, const __gm__ void* gm, int row, int col, int useM, int useK,
    const uint64_t tilingPtr, const uint64_t dataPtr);
void CopyB1(
    const LocalTensor<int8_t>& bMatrix, const __gm__ void* gm, int row, int col, int useK, int useN,
    const uint64_t tilingPtr, const uint64_t dataPtr);

typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> bType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
AscendC::Matmul<aType, bType, cType, biasType, CFG_NORM, MatmulCallBackFunc<DataCopyOut, CopyA1, CopyB1>> mm;
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
uint64_t tilingPtr = reinterpret_cast<uint64_t>(tiling);
// Set the operator tiling address for use by the callback function. This API needs to be called only once.
mm.SetUserDefInfo(tilingPtr);
mm.SetTensorA(gmA);
mm.SetTensorB(gmB);
mm.IterateAll();
```
