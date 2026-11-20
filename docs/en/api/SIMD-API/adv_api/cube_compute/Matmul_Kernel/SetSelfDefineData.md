# SetSelfDefineData

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-02T06:48:02.007Z pushedAt=2026-11-18T04:53:56.901Z nl=2 -->

## Applicable Products

<!-- npu="950" id3 -->
- Ascend 950PR&950DT products: Supported
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 products: Supported
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 products: Supported
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->
<!-- npu="x90" id1 -->
- Kirin X90 processor products: Supported
<!-- end id1 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetSelfDefineData_res.md#id1 -->

## Description

When the template parameter [MatmulCallBackFunc](Matmul_template_params.md#matmul-callback-func) (custom callback function) is enabled, set information such as the required computation data or the address of data stored in GM for use by the callback function. When reusing the same **Matmul** object, you can call this API multiple times to reset the corresponding data information.

## Prototype

```
__aicore__ inline void SetSelfDefineData(const uint64_t dataPtr)
```

```
__aicore__ inline void SetSelfDefineData(T dataPtr)
```

<!-- npu="A3" id9 -->
Atlas A3 products do not support the SetSelfDefineData\(T dataPtr\) API prototype.
<!-- end id9 -->

<!-- npu="910b" id10 -->
Atlas A2 products do not support the SetSelfDefineData\(T dataPtr\) API prototype.
<!-- end id10 -->

<!-- npu="x90" id2 -->
Kirin X90 processor products do not support the SetSelfDefineData\(T dataPtr\) API prototype.
<!-- end id2 -->

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dataPtr | Input | Information such as the computation data required by the operator callback function or the data address stored on the GM. The type T supports user-defined basic structures. |

## Return Value

None

## Constraints

-   If the callback function needs to use the dataPtr parameter, this API must be called. If the callback function does not use the dataPtr parameter, there is no need to call this API.
-   When MixDualMaster (dual-master mode) is enabled, that is, when the template parameter [enableMixDualMaster](MatmulConfig.md#matmulconfig-params) is set to true, this API is not supported.
-   This API must be called before the SetTensorA and SetTensorB APIs.

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
GlobalTensor<SrcT> dataGM; // GM that stores the computation data required by the callback function.
uint64_t dataGMPtr = reinterpret_cast<uint64_t>(dataGM.address_);
// Call this API when the dataPtr parameter is required in the callback function.
mm.SetSelfDefineData(dataGMPtr);
mm.SetTensorA(gmA);
mm.SetTensorB(gmB);
mm.IterateAll();
```
