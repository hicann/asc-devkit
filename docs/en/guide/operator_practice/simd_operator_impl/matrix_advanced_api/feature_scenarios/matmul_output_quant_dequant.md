# Quantization/Dequantization of Matmul Output

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T12:09:17.036Z pushedAt=2026-11-03T07:05:13.305Z nl=2 -->

## Feature Overview

For [specific input and output data types](#zh-cn_topic_0000002298767893_table1996113269499), Matmul supports quantizing or dequantizing the output cube C elements when moving the computation result from the L0C Buffer (CO1) to Global Memory.

-   Matmul quantization scenario: During Matmul computation, the left cube A and right cube B are of the half or bfloat16\_t data type, and the output cube C is of the int8\_t data type. In this scenario, when the data of the cube C is moved from the L0C Buffer (CO1) to Global Memory, a quantization operation is performed to quantize the final result into the int8\_t type, as shown in the following figure.

    **Figure 1**  Matmul quantization scenario  
    ![](../../../../figures/mm_quant.png "Matmul quantization scenario")

-   Matmul dequantization scenario: During Matmul computation, the left cube A and right cube B are of the int8\_t or int4b\_t data type and the output cube C is of the half data type, or the left cube A and right cube B are of the int8\_t data type and the output cube C is of the int8\_t data type. In this scenario, when the data of the cube C is moved from the L0C Buffer (CO1) to Global Memory, a dequantization operation is performed to dequantize the final result into the corresponding half type or int8\_t type, as shown in the following figure.

    **Figure 2**  Matmul dequantization scenario  
    ![](../../../../figures/mm_dequant.png "Matmul dequantization scenario")

Matmul quantization/dequantization includes two modes: the same-coefficient quantization/dequantization mode and the vector quantization/dequantization mode. Developers call the [SetDequantType](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Tiling/SetDequantType.md) API on the operator Tiling side to set the quantization or dequantization mode. The specific differences between the two modes are as follows:

-   Same-coefficient quantization/dequantization mode (PER\_TENSOR mode): The entire cube C corresponds to one quantization parameter, and the shape of the quantization parameter is \[1\]. Developers call the [SetQuantScalar](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetQuantScalar.md) API on the operator kernel function side to set the quantization parameter.
-   Vector quantization/dequantization mode (PER\_CHANNEL mode): The shape of the cube C is \[m, n\], and each channel dimension, that is, each column of the cube C, corresponds to one quantization parameter, whose shape is \[n\]. Developers call the [SetQuantVector](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetQuantVector.md) API on the operator kernel function side to set the quantization parameter.

**Table 1**  Interface configuration corresponding to the quantization/dequantization modes

| Mode | Tiling Side Interface | Kernel Function (Kernel) Side Interface |
| --- | --- | --- |
| Quantization/dequantization with the same coefficient | SetDequantType(DequantType::SCALAR) | SetQuantScalar(gmScalar) |
| Quantization/dequantization with vectors | SetDequantType(DequantType::TENSOR) | SetQuantVector(gmTensor) |

## Scenarios

This section applies to scenarios where the cube computation result needs to be quantized or dequantized. In these scenarios, the data types supported by the Matmul input and output cubes are listed in the following table.

**Table 2** Data types supported by Matmul quantization/dequantization

<a name="zh-cn_topic_0000002298767893_table1996113269499"></a>

| Cube A | Cube B | Cube C | Supported Platform |
| --- | --- | --- | --- |
| half | half | int8_t | <!-- npu="950" id1 -->Ascend 950PR&950DT products<br><br><!-- end id1 --><!-- npu="A3" id2 -->Atlas A3 products<br><br><!-- end id2 --><!-- npu="910b" id3 -->Atlas A2 products<!-- end id3 --> |
| bfloat16_t | bfloat16_t | int8_t | <!-- npu="950" id4 -->Ascend 950PR&950DT products<br><br><!-- end id4 --><!-- npu="A3" id5 -->Atlas A3 products<br><br><!-- end id5 --><!-- npu="910b" id6 -->Atlas A2 products<!-- end id6 --> |
| int8_t | int8_t | half | <!-- npu="950" id7 -->Ascend 950PR&950DT products<br><br><!-- end id7 --><!-- npu="A3" id8 -->Atlas A3 products<br><br><!-- end id8 --><!-- npu="910b" id9 -->Atlas A2 products<!-- end id9 --> |
| int4b_t | int4b_t | half | <!-- npu="A3" id10 -->Atlas A3 products<br><br><!-- end id10 --><!-- npu="910b" id11 -->Atlas A2 products<!-- end id11 --> |
| int8_t | int8_t | int8_t | <!-- npu="950" id12 -->Ascend 950PR&950DT products<br><br><!-- end id12 --><!-- npu="A3" id13 -->Atlas A3 products<br><br><!-- end id13 --><!-- npu="910b" id14 -->Atlas A2 products<!-- end id14 --> |
| int8_t | int8_t | bfloat16_t | <!-- npu="950" id15 -->Ascend 950PR&950DT products<!-- end id15 --> |
| fp8_e4m3fn_t/fp8_e5m2_t | fp8_e4m3fn_t/fp8_e5m2_t | fp8_e4m3fn_t/half/bfloat16_t/float | <!-- npu="950" id16 -->Ascend 950PR&950DT products<!-- end id16 --> |
| hifloat8_t | hifloat8_t | hifloat8_t/half/bfloat16_t/float | <!-- npu="950" id17 -->Ascend 950PR&950DT products<br><br><!-- end id17 -->Note:<br><br>When the output is hifloat8_t, the Half to Away Round method is used for quantization.<br><br>When the output of the quantization scenario is of the float type, the precision of this quantization mode cannot reach 2/10000 but can reach 2/1000. If a precision of 2/10000 is required, use the advanced AscendDeQuant API. |

## Constraints

-   The [SetQuantScalar](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetQuantScalar.md) and [SetQuantVector](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetQuantVector.md) APIs must be called before the [Iterate](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/Iterate.md) or [IteratAll](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/IterateAll.md) API.

-   The quantization/dequantization modes set on the kernel function side and the Tiling side must be consistent:
    -   On the kernel function side, call the **SetQuantScalar** API to set the same-coefficient quantization/dequantization mode, and on the Tiling side, call the **SetDequantType** API to configure the mode as DequantType::SCALAR.
    -   On the kernel function side, call the **SetQuantVector** API to set the vector quantization/dequantization mode, and on the Tiling side, call the **SetDequantType** API to configure the mode as DequantType::TENSOR.

-   When cubes A and B are of the int8\_t or int4b\_t type and cube C is half, the output result of this feature does not support the INF\_NAN mode. If the result needs to be output as INF\_NAN, it is recommended that you output the result to TPosition::VECIN when calling the Matmul API, set the output data type to int32\_t, and then use the advanced [AscendDequant](../../../../../api/SIMD-API/adv_api/quantization/AscendDequant.md) API on the AIV core to dequantize the result to the half type.

## Example

For the complete operator sample, see [matmul\_quant sample](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/00_matmul/matmul_quant).

-   Tiling implementation

    Call the [SetDequantType](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Tiling/SetDequantType.md) API to set the quantization or dequantization mode. Other implementation content is the same as that in the basic scenario.

    ```
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
    tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_INT8);
    tiling.SetBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_INT8);
    tiling.SetCType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    tiling.SetBiasType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_INT32);
    tiling.SetShape(M, N, K);
    tiling.SetOrgShape(M, N, K);
    tiling.EnableBias(true);
    tiling.SetDequantType(DequantType::SCALAR);  // Set the same-coefficient quantization/dequantization mode
    // tiling.SetDequantType(DequantType::TENSOR);  // Set the vector quantization/dequantization mode
    ... // Perform other configurations
    ```

-   Kernel function implementation

    Based on the specific quantization mode scenario, call the [SetQuantScalar](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetQuantScalar.md) or [SetQuantVector](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetQuantVector.md) API to set the quantization parameters. Other implementation content is the same as that in the basic scenario.

    -   Same-coefficient quantization/dequantization mode

        ```
        REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
        float tmp = 0.1;  // Multiply by 0.1 when outputting to GM
        uint64_t ans = static_cast<uint64_t>(*reinterpret_cast<int32_t*>(&tmp)); // Convert the floating-point quantization coefficient to uint64_t for setting
        mm.SetQuantScalar(ans);
        mm.SetTensorA(gm_a);
        mm.SetTensorB(gm_b);
        mm.SetBias(gm_bias);
        mm.IterateAll(gm_c);
        ```

    -   Vector quantization/dequantization mode

        ```
        GlobalTensor gmQuant;
        ...
        REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
        mm.SetQuantVector(gmQuant);
        mm.SetTensorA(gm_a);
        mm.SetTensorB(gm_b);
        mm.SetBias(gm_bias);
        mm.IterateAll(gm_c);
        ```
