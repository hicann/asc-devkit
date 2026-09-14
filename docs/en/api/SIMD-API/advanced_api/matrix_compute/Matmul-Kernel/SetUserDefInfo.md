# SetUserDefInfo

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T07:06:10.259Z pushedAt=2026-09-12T09:55:18.151Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

When the template parameter [MatmulCallBackFunc](MatmulCallBackFunc.md) (custom callback function) is enabled, this API sets the operator tiling address for use by the callback function. It needs to be called only once.

## Prototype

```
__aicore__ inline void SetUserDefInfo(const uint64_t tilingPtr)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **tilingPtr** | Input | Tiling address of the operator to be set. |

## Return Value

None

## Constraints

- If the callback function needs to use the `tilingPtr` parameter, you must call this API. If the callback function does not use the `tilingPtr` parameter, you do not need to call this API.
- When MixDualMaster (dual-master mode) is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to true, this API is not supported.

## Examples

```
//User-defined callback function.
void DataCopyOut(const __gm__ void *gm, const LocalTensor<int8_t> &co1Local, const void *dataCopyOutParams, const uint64_t tilingPtr, const uint64_t dataPtr);
void CopyA1(const LocalTensor<int8_t> &aMatrix, const __gm__ void *gm, int row, int col, int useM, int useK, const uint64_t tilingPtr, const uint64_t dataPtr);
void CopyB1(const LocalTensor<int8_t> &bMatrix, const __gm__ void *gm, int row, int col, int useK, int useN, const uint64_t tilingPtr, const uint64_t dataPtr);

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
