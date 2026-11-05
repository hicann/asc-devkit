# GetSubBlockIdx

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:25:25.597Z pushedAt=2026-10-27T11:24:31.243Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetSubBlockIdx_res.md#id1 -->

## Description

Obtains the ID of the current AIV core. In the decoupled architecture, an AI Core consists of Cube Core (AIC) and Vector Core (AIV) combined at a ratio of 1:N, where the sub-core IDs of the N AIVs are 0, 1, ..., N-1.

The difference between Matmul::GetSubBlockIdx\(\) and the basic API [AscendC::GetSubBlockIdx\(\)](../../../basic_api/tool_interface/system_resources_and_variables/GetSubBlockIdx_ISASI.md) is that Matmul::GetSubBlockIdx\(\) obtains the ID of the current AIV core within the current AI Core group, while AscendC::GetSubBlockIdx\(\) obtains the logical ID of the AIV core across all AI Core groups. For example, with 10 AI Core groups and an AIC-to-AIV ratio of 1:2, there are 20 AIV cores in total. When Matmul::GetSubBlockIdx\(\) is called, the results for the 20 AIVs are 0, 1, 0, 1, 0, 1, ..., 0, 1 in sequence. When AscendC::GetSubBlockIdx\(\) is called, the results for the 20 AIVs are 0, 1, 2, 3, 4, 5, ..., 18, 19 in sequence.

## Prototype

```
__aicore__  inline uint8_t GetSubBlockIdx()
```

## Parameters

None

## Return Value

ID of the current AIV core.

## Constraints

-   This API is supported only in the decoupled architecture; otherwise, it returns a random value.
-   In the decoupled architecture, the ID of the AIV core is automatically initialized and assigned inside the [REGIST\_MATMUL\_OBJ\(\)](REGIST_MATMUL_OBJ.md) API. Therefore, call this API only after calling the REGIST\_MATMUL\_OBJ\(\) API to obtain the correct ID.
-   If [SetSubBlockIdx\(\)](SetSubBlockIdx.md) is called in the operator program, the GetSubBlockIdx\(\) API returns the ID value set by the **SetSubBlockIdx** API.

## Examples

```
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> bType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;

AscendC::Matmul<aType, bType, cType, biasType, CFG_NORM> mm;
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
mm.GetSubBlockIdx(); // Obtain the sub-core ID.
```
