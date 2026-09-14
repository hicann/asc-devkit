# GetMatmulApiTiling

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T06:26:36.796Z pushedAt=2026-09-12T09:55:18.096Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference product: Not supported
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Parameter

This API is used to obtain the constant Matmul tiling parameters during compilation.

<a id="matmul-tiling-constant"></a>
The Matmul Tiling constant feature can be used to obtain constant Matmul Tiling parameters during compilation and compily the operator accordingly, thereby reducing Scalar computation overhead and improving the overall operator performance. Specifically, when obtaining the [Matmul template](matmul_template_parameters.md), you can determine the **singleCore** shape parameters ([MatmulConfig](MatmulConfig.md) in singleCoreM/singleCoreN/singleCoreK) and **Base** shape parameters (**basicM**, **basicN**, and **basicK** in [MatmulConfig](MatmulConfig.md)), or determine only the **Base** shape parameters. By specifying the **singleCore** shape and **Base** shape parameters in the API for obtaining the template, or by specifying only the **Base** shape parameters, you can obtain a custom template; then call this API to obtain the constant Matmul Tiling parameters.

<a id="partial-constant"></a>
When calling the [API for obtaining the MatmulConfig template](MatmulConfig.md#get-matmulconfig-api), if only **baseM**, **baseN**, and **baseK** are set to constant values, this is called partial constant compilation. In this case, **singleCoreM**, **singleCoreN**, and **singleCoreK** are all set to their default value of **0**. In the partial constant compilation scenario, when initializing the **Matmul** object on the Kernel using [REGIST\_MATMUL\_OBJ](REGIST_MATMUL_OBJ.md), tiling is still required. When **baseM**, **baseN**, **baseK**, **singleCoreM**, **singleCoreN**, and singleCoreK are all set to constant values, this is called full constant compilation. In this case, a null pointer can be used in place of the tiling parameter in the input parameters of [REGIST\_MATMUL\_OBJ](REGIST_MATMUL_OBJ.md).

After the above partial or full constant compilation, a **MatmulConfig** template with constant parameters is obtained, and then this API is used to perform constant compilation of the tiling parameters. The return value of this API contains the constant Matmul tiling parameters and the **MatmulConfig** template.

## Prototype

```
template<class A_TYPE, class B_TYPE, class C_TYPE, class BIAS_TYPE>
__aicore__ constexpr MatmulApiStaticTiling GetMatmulApiTiling(const MatmulConfig& mmCFG, int32_t l1Size = Impl::L1_SIZE)
```

## Parameter Description

**Table 1**  Template parameters

| Parameter | Description |
| --- | --- |
| A_TYPE | Type information of matrix A, defined by [MatmulType](matmul_usage.md#table1188045714378). |
| B_TYPE | Type information of matrix B, defined by [MatmulType](matmul_usage.md#table1188045714378). |
| C_TYPE | Type information of matrix C, defined by [MatmulType](matmul_usage.md#table1188045714378). |
| BIAS_TYPE | Type information of the BIAS matrix, defined by [MatmulType](matmul_usage.md#table1188045714378). |

**Table 2**  Parameter description

| Parameter Name | Input/Output | Description |
| --- | --- | --- |
| mmCFG | Input | The [MatmulConfig](MatmulConfig.md#matmulconfig-params) template to be obtained.<br><br>For Ascend 950PR/Ascend 950DT, all templates support constant compilation, including the **Norm**, **IBShare**, and **MDL** templates.<br><br>For Atlas A3 training products/Atlas A3 inference products, the templates that support constant compilation include **Norm** and **MDL**.<br><br>For Atlas A2 training products/Atlas A2 inference products, the templates that support constant compilation include **Norm** and **MDL**. |
| l1Size | Input | Available L1 size. The default value is **L1_SIZE**. |

## Return Value

[MatmulApiStaticTiling](matmul_template_parameters.md#table7939847143412)

## Constraints

-   When the input parameter **mmCFG** is obtained by calling the API for obtaining the **MatmulConfig** template, you need to use constant values to specify **baseM, **baseN**, and baseK** or specify **baseM**, **baseN**, **baseK**, **singleCoreM**, **singleCoreN**, and **singleCoreK**, and the specified parameter values must be consistent with the values computed by tiling.
-   In the batch Matmul scenario, full constant compilation is supported, but it is not allowed to use a null pointer to replace the input parameter **Tiling** of **REGIST\_MATMUL\_OBJ**.

## Examples

```
//Define the Matmul object.
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> aType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, half> bType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType;
typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
// CFG is obtained using the GetNormalConfig API, with the known single-shape information and baseM, baseN, and baseK specified. The specified values must be consistent with the runtime tiling.
constexpr auto staticTiling = GetMatmulApiTiling<aType, bType, cType, biasType>(CFG, 524288); // In this example, the available L1 buffer size is 512 KB.
AscendC::Matmul<aType, bType, cType, biasType, staticTiling > mm;
```
