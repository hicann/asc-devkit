# SetSubBlockIdx

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:56:18.145Z pushedAt=2026-10-27T11:31:49.079Z nl=2 -->

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
- Atlas inference products AI Core: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetSubBlockIdx_res.md#id1 -->

## Description

Sets the ID of the current AIV core. In the separated architecture, an AI Core is composed of Cube Core (AIC) and Vector Core (AIV) at a ratio of 1:N, where the IDs of the N AIV cores are 0, 1, ..., N-1.

## Prototype

```
__aicore__ inline void SetSubBlockIdx(uint8_t subBlockIdx)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **subBlockIdx** | Input | ID of the current AIV core. |

## Return Value

None

## Constraints

-   This API can be used only in the separated architecture.
-   In the separated architecture, the AIV core ID is automatically initialized and assigned inside the [REGIST\_MATMUL\_OBJ\(\)](REGIST_MATMUL_OBJ.md) API. If the REGIST\_MATMUL\_OBJ\(\) API is used in the operator program, calling this API is not recommended. If the REGIST\_MATMUL\_OBJ\(\) API is not used, call this API and set the sub-core ID to 0.

## Examples

```
#define ASCENDC_CUBE_ONLY
#include "lib/matmul_intf.h"

typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> bType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;

Matmul<aType, bType, cType, biasType, CFG_NORM> mm;
mm.SetSubBlockIdx(0); // Set the sub-core ID to 0.
```
