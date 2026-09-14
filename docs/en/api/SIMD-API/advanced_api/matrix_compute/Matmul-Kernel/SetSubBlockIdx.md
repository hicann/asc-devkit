# SetSubBlockIdx

<!-- md-trans-meta sourceCommit=29c4740ae8f0275cefcb220d1dc0bf18d9f0bd15 translatedAt=2026-09-09T07:02:18.194Z pushedAt=2026-09-12T09:55:18.145Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

Sets the ID of the current AIV core. In the separate architecture, an AI Core consists of a Cube Core (AIC) and Vector Cores (AIV) combined at a ratio of 1:N, where the IDs of the N AIV cores are 0, 1, ..., N-1.

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

-   This API is supported only in the separate architecture.
-   In the separate architecture, the ID of the AIV core is automatically initialized and assigned inside the [REGIST\_MATMUL\_OBJ\(\)](REGIST_MATMUL_OBJ.md) API. If the REGIST\_MATMUL\_OBJ\(\) API is used in the operator program, calling this API is not recommended. If the REGIST\_MATMUL\_OBJ\(\) API is not used, call this API and set the sub-block ID to 0.

## Examples

```
#define ASCENDC_CUBE_ONLY
#include "lib/matmul_intf.h"

typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> bType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;

Matmul<aType, bType, cType, biasType, CFG_NORM> mm;
mm.SetSubBlockIdx(0);  // Set the sub-block ID to 0.
```
