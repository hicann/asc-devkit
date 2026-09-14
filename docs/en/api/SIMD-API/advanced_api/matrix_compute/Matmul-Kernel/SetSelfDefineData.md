# SetSelfDefineData

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T06:59:42.751Z pushedAt=2026-09-12T09:55:18.138Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

When the template parameter [MatmulCallBackFunc](matmul_template_parameters.md#matmulcallbackfunc) (custom callback function) is enabled, this API sets the required computation data or the address of data stored in GM, which is used by the callback function. When the same Matmul object is reused, you can call this API multiple times to reset the corresponding data information.

## Prototype

```
__aicore__ inline void SetSelfDefineData(const uint64_t dataPtr)
```

```
__aicore__ inline void SetSelfDefineData(T dataPtr)
```

Atlas A3 training products/Atlas A3 inference products do not support the SetSelfDefineData\(T dataPtr\) API prototype.

Atlas A2 training products/Atlas A2 inference products do not support the SetSelfDefineData\(T dataPtr\) API prototype.

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **dataPtr** | Input | Information such as the computation data required by the operator callback function or the address of the data stored in GM. The type **T** supports user-defined base structures. |

## Return Value

None

## Constraints

-   If the **dataPtr** parameter is used in the callback function, this API must be called; if the callback function does not use the **dataPtr** parameter, there is no need to call this API.
-   When the MixDualMaster (dual-master mode) scenario is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to **true**, this API is not supported.
-   This API must be called before the **SetTensorA** and **SetTensorB** APIs.

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
GlobalTensor<SrcT> dataGM; // GM that stores the computation data required by the callback function.
uint64_t dataGMPtr = reinterpret_cast<uint64_t>(dataGM.address_);
// When the dataPtr parameter is required in the callback function, this API must be called.
mm.SetSelfDefineData(dataGMPtr);
mm.SetTensorA(gmA);
mm.SetTensorB(gmB);
mm.IterateAll();
```
