# GetSubBlockIdx

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-09-09T06:35:27.559Z pushedAt=2026-09-12T09:55:18.106Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Functional Description

Gets the ID of the current AIV core. Under the separate architecture, an AI Core consists of Cube Cores (AIC) and Vector Cores (AIV) combined at a ratio of 1:N, where the sub-core IDs of the N AIVs are 0, 1, ..., N-1.

The difference between Matmul::GetSubBlockIdx\(\) and the basic API [AscendC::GetSubBlockIdx\(\)](../../../basic_api/tool_interface/system_resources_and_variables/GetSubBlockIdx_ISASI.md) is that Matmul::GetSubBlockIdx\(\) is used to get the ID of the current AIV core within the current AI Core group, while AscendC::GetSubBlockIdx\(\) gets the logical ID of the AIV core across all AI Core groups. For example, with 10 AI Core groups and an AIC-to-AIV ratio of 1:2, there are 20 AIV cores in total. When calling Matmul::GetSubBlockIdx\(\), the results for the 20 AIVs are 0, 1, 0, 1, 0, 1, ..., 0, 1 in order. When calling AscendC::GetSubBlockIdx\(\), the results for the 20 AIVs are 0, 1, 2, 3, 4, 5, ..., 18, 19 in order.

## Function Prototype

```
__aicore__  inline uint8_t GetSubBlockIdx()
```

## Parameters

None

## Return Value

Returns the ID of the current AIV core.

## Constraints

-   This API is supported only in the separate architecture; otherwise, it returns a random value.
-   In the separate architecture, the ID of the AIV core is automatically initialized and assigned within the [REGIST\_MATMUL\_OBJ\(\)](REGIST_MATMUL_OBJ.md) API. Therefore, call this API after calling the REGIST\_MATMUL\_OBJ\(\) API to obtain the correct ID.
-   If [SetSubBlockIdx\(\)](SetSubBlockIdx.md) is called in the operator program, the GetSubBlockIdx\(\) API returns the ID value set by the **SetSubBlockIdx** API.

## Examples

```
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> bType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;

AscendC::Matmul<aType, bType, cType, biasType, CFG_NORM> mm;
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
mm.GetSubBlockIdx(); // Obtain the sub-block ID.
```
