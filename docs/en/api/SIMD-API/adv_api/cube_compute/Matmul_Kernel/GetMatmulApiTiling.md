# GetMatmulApiTiling

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T12:19:20.553Z pushedAt=2026-10-27T11:19:57.805Z nl=2 -->

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

- Atlas inference products AI Core: Not supported

- Vector Core of Atlas inference products: Not supported

<!-- end id6 -->
<!-- npu="910" id7 -->

- Atlas training products: Not supported

<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetMatmulApiTiling_res.md#id1 -->

## Description

This API is used to obtain constantized Matmul Tiling parameters during compilation.

The Matmul Tiling constantization feature obtains constantized Matmul Tiling parameters during compilation and compiles the operator, thereby reducing Scalar computation overhead and improving overall operator performance. Specifically, when obtaining the [Matmul template](Matmul_template_params.md), you can determine the singleCore Shape (singleCoreM/singleCoreN/singleCoreK in [MatmulConfig](MatmulConfig.md)) and Base Shape (basicM/basicN/basicK in [MatmulConfig](MatmulConfig.md)) parameters of [MatmulConfig](MatmulConfig.md), or determine only the Base Shape parameters. By specifying the singleCore Shape and Base Shape parameters in the API for obtaining the template, or by specifying only the Base Shape parameters, you can obtain a custom template. Then call this API to obtain the constantized Matmul Tiling parameters.

When calling the [API for obtaining the MatmulConfig template](MatmulConfig.md), if only \(baseM, baseN, baseK\) are set to constant values, it is called partial constantization. In this case, \(singleCoreM, singleCoreN, singleCoreK\) all keep their default value of 0. In the partial constantization scenario, when using [REGIST\_MATMUL\_OBJ](REGIST_MATMUL_OBJ.md) to initialize the Matmul object on the kernel side, Tiling is still required. If \(baseM, baseN, baseK, singleCoreM, singleCoreN, singleCoreK\) are all set to constant values, it is called full constantization. In this case, you can pass a null pointer in place of the Tiling parameter in the input parameters of [REGIST\_MATMUL\_OBJ](REGIST_MATMUL_OBJ.md).














After the above partial constantization or full constantization, a MatmulConfig template with constantized parameters is obtained. Then, use this API to constantize the Tiling parameters. The return value of this API contains the constantized Matmul Tiling parameters and the MatmulConfig template.

## Prototype

```
template<class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE>
__aicore__ constexpr MatmulApiStaticTiling GetMatmulApiTiling(const MatmulConfig& mmCFG, int32_t l1Size = Impl::L1_SIZE)
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| A_TYPE | Type information of cube A, defined by [MatmulType](Matmul_usage.md#table1188045714378). |
| B_TYPE | Type information of cube B, defined by [MatmulType](Matmul_usage.md#table1188045714378). |
| C_TYPE | Type information of cube C, defined by [MatmulType](Matmul_usage.md#table1188045714378). |
| BIAS_TYPE | Type information of the BIAS cube, defined by [MatmulType](Matmul_usage.md#table1188045714378). |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| mmCFG | Input | The obtained [MatmulConfig](MatmulConfig.md#matmulconfig-params) template.<!-- npu="950" id8 --><br><br>For Ascend 950PR&950DT products, all templates support constantization: Norm, IBShare, and MDL templates.<!-- end id8 --><!-- npu="A3" id9 --><br><br>For Atlas A3 products, the templates that support constantization are: Norm and MDL templates.<!-- end id9 --><!-- npu="910b" id10 --><br><br>For Atlas A2 products, the templates that support constantization are: Norm and MDL templates.<!-- end id10 --> |
| l1Size | Input | Available L1 size. The default value is L1_SIZE. |

## Return Value

[MatmulApiStaticTiling](Matmul_template_params.md#table7939847143412)

## Constraints

-   Input parameter mmCFG: When calling the API for obtaining the MatmulConfig template, you need to specify \(baseM, baseN, baseK\) or \(baseM, baseN, baseK, singleCoreM, singleCoreN, singleCoreK\) with constant values, and the specified parameter values must be consistent with the values calculated by Tiling.

-   The Batch Matmul scenario supports full constantization, but does not support using a null pointer in place of the Tiling input parameter of REGIST\_MATMUL\_OBJ.

<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetMatmulApiTiling_res.md#id2 -->

## Examples

```
// Define the Matmul object
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> bType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
// Here CFG is obtained using the GetNormalConfig API, with the known singleShape information and baseM, baseN, baseK specified; the specified values must be consistent with the runtime tiling
// The available L1 Buffer size in this example is 512 KB
constexpr auto staticTiling = GetMatmulApiTiling<aType, bType, cType, biasType>(CFG, 524288);
AscendC::Matmul<aType, bType, cType, biasType, staticTiling> mm;
```