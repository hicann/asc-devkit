# MmadBitMode<a name="ZH-CN_TOPIC_000000253823118101"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T13:23:55.460Z pushedAt=2026-10-26T07:41:44.186Z nl=2 -->

## Applicable Products

### Prototype without bias

<!-- npu="950" id1 -->
- Ascend 950PR & 950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/MmadBitMode_res.md#id1 -->

### Prototype with bias input

<!-- npu="950" id10 -->
- Ascend 950PR&950DT products: supported
<!-- end id10 -->
<!-- npu="A3" id11 -->
- Atlas A3 products: not supported
<!-- end id11 -->
<!-- npu="910b" id12 -->
- Atlas A2 products: not supported
<!-- end id12 -->
<!-- npu="310b" id13 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas inference products AI Core: not supported
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas inference products Vector Core: not supported
<!-- end id15 -->
<!-- npu="910" id16 -->
- Atlas training products: not supported
<!-- end id16 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/MmadBitMode_res.md#id2 -->

## Description

The header file path is `"basic_api/kernel_operator_mm_intf.h"`.

**MmadBitMode** optimizes the construction of the **MmadParams** structure. This API is suitable for scenarios where the scalar pipeline becomes a performance optimization bottleneck, and it supports the basic Mmad/MmadMx computation functions. The difference between this API and the Mmad/MmadMx APIs lies in the way parameters are passed: this API passes the union structure **MmadBitModeParams**. The design concept of the **MmadBitModeParams** class parameters is described as follows:

A union is a special data structure that allows different data types to be stored at the same memory location. All members of a union share the same memory space, whose size is determined by the largest member, and only one member can be used at a time.

A bit-field is a special class member that allows precise control over the number of memory bits occupied by a member variable in a structure. Member variables in a structure correspond to memory from low-order bits to high-order bits from top to bottom.

The **MmadBitModeParams** class uses the union and bit-field methods, expresses parameter types with bits, uses the bit-field structure to automatically process the number of bits of the input parameters, and leverages the union feature to implement multi-parameter fused passing. Only one input parameter needs to be passed to contain all the required information, and the corresponding underlying API only needs to receive one parameter. Meanwhile, when the value of a certain bit in a parameter needs to be modified, it can be achieved simply through loops and bit operations without re-passing the parameter, which reduces scalar computation and improves performance.

The **MmadBitModeParams** class can be initialized directly with an object of the **MmadBitModeParams** structure type:

```cpp
__aicore__ inline MmadBitModeParams(const MmadBitModeParams &mmadParams_);
```

You can also use the Set functions of each parameter to modify parameter values. In addition, because a union is used, you can also directly modify **config0** bit by bit to change the parameters.

## Prototype

- Without bias

  ```cpp
  template <typename T, typename U, typename S>
  __aicore__ inline void Mmad(const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const MmadBitModeParams& mmadParams)

  template <typename T, typename U, typename S>
  __aicore__ inline void MmadMx(const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const MmadBitModeParams& mmadParams)
  ```

- With bias

  ```cpp
  template <typename T, typename U, typename S, typename V>
  __aicore__ inline void Mmad(const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const LocalTensor<V>& bias, const MmadBitModeParams& mmadParams)

  template <typename T, typename U, typename S, typename V>
  __aicore__ inline void MmadMx(const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const LocalTensor<V>& bias, const MmadBitModeParams& mmadParams)
  ```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Meaning |
| ---------- | --------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| dst | Output | Destination operand, result matrix C, type is **LocalTensor**, supported physical storage location is **L0C Buffer** (**TPosition:CO1**).<br>The start address of **LocalTensor** must be 1024-byte aligned. |
| fm | Input | Source operand, left matrix A, type is **LocalTensor**, supported physical storage location is **L0A Buffer** (**TPosition:A2**).<br>The start address of the scale matrix corresponding to left matrix A is: the start address of matrix A / 16.<br>For the fp4 scenario, the start address of **LocalTensor** must be 512-byte aligned. For the fp8 scenario, the start address of **LocalTensor** must be 1024-byte aligned. |
| filter | Input | Source operand, right matrix B, type is **LocalTensor**, supported physical storage location is **L0B Buffer** (**TPosition:B2**).<br>The start address of the scale matrix corresponding to right matrix B is: the start address of matrix B / 16.<br>For the fp4 scenario, the start address of **LocalTensor** must be 512-byte aligned. For the fp8 scenario, the start address of **LocalTensor** must be 1024-byte aligned. |
| bias | Input | Source operand, Bias matrix, type is **LocalTensor**, supported physical storage location is **BT Buffer** (**TPosition:C2**).<br>The start address of **LocalTensor** must be 64-byte aligned. |
| mmadParams | Input | Matrix multiplication related parameters.<br>For the specific definition of this parameter type, see `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_mm.h`. Replace `${INSTALL_DIR}` with the file storage path after CANN software installation.<br>For the description of **MmadBitModeParams** parameters, see the following table. |

**Table 2** MmadBitModeParams class parameters

| Parameter | Meaning |
| -------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------- |
| config0 | **uint64_t** type, forming a union with the **MmadBitModeConfig0** bit-field structure type parameter **config0BitMode**, initialized to 0. You can use the **GetConfig0()** function of the class object to get its value. |
| config0BitMode | **MmadBitModeConfig0** bit-field structure type. For parameters, see [Table 3](#zh_cn_topic_mmadbitmode_section4_table3). It forms a union with **config0**. |

**Table 3** MmadBitModeConfig0 structure parameters<a id="zh_cn_topic_mmadbitmode_section4_table3"></a>

| Parameter | Meaning |
| -------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| m | Left matrix Height, value range: m∈[0, 4095]. Default value is 0.<br>This parameter is the lowest-order parameter of the bit-field structure, occupying 12 bits. You can use the **SetM()** function of the **MmadBitModeParams** class object to set its value, and the **GetM()** function to get its value. |
| k | Left matrix Width, right matrix Height, value range: k∈[0, 4095]. Default value is 0.<br>This parameter is the second lowest-order parameter of the bit-field structure, occupying 12 bits. You can use the **SetK()** function of the **MmadBitModeParams** class object to set its value, and the **GetK()** function to get its value. |
| n | Right matrix Width, value range: n∈[0, 4095]. Default value is 0.<br>This parameter is the third lowest-order parameter of the bit-field structure, occupying 12 bits. You can use the **SetN()** function of the **MmadBitModeParams** class object to set its value, and the **GetN()** function to get its value. |
| unitFlag | Reserved parameter. Reserved for future functionality; developers do not need to pay attention to it for now and can use the default value.<br>This parameter is the fourth lowest-order parameter of the bit-field structure, occupying 2 bits. You can use the **SetUnitFlag()** function of the **MmadBitModeParams** class object to set its value, and the **GetUnitFlag()** function to get its value. |
| disableGemv | When M = 1, configures whether GEMV is enabled for Mmad computation. When the input is false, GEMV is enabled; otherwise, when the input is true, GEMV is disabled.<br>GEMV (General Matrix-Vector Multiplication) implements the product of a matrix and a vector. After GEMV is enabled, when the Mmad API reads data from **L0A Buffer**, the data is read in ND format instead of being treated as ZZ format.<br>This parameter is the fifth lowest-order parameter of the bit-field structure, occupying 1 bit. You can use the **SetDisableGemv()** function of the **MmadBitModeParams** class object to set its value, and the **GetDisableGemv()** function to get its value. |
| cmatrixSource | Configures whether the initial value of matrix C comes from **BT Buffer** (**TPosition:C2**). Default value is false.<br>&nbsp;&nbsp;&bull; false: Comes from **L0C Buffer** (**TPosition:CO1**).<br>&nbsp;&nbsp;&bull; true: Comes from **BT Buffer** (**TPosition:C2**).<br>Note: For APIs with bias input, configuring this parameter is invalid. Whether the initial value of matrix C comes from **L0C Buffer** or **BT Buffer** is determined by the position of the bias input.<br>This parameter is the sixth lowest-order parameter of the bit-field structure, occupying 1 bit. You can use the **SetCmatrixSource()** function of the **MmadBitModeParams** class object to set its value, and the **GetCmatrixSource()** function to get its value. |
| cmatrixInitVal | Configures whether the initial value of matrix C is 0. Default value is true.<br>&nbsp;&nbsp;&bull; true: The initial value of matrix C is 0.<br>&nbsp;&nbsp;&bull; false: The initial value of matrix C is configured through the **cmatrixSource** parameter.<br>This parameter is the highest-order parameter of the bit-field structure, occupying 1 bit. You can use the **SetCmatrixInitVal()** function of the **MmadBitModeParams** class object to set its value, and the **GetCmatrixInitVal()** function to get its value. |

## Data Types<a id="zh_cn_topic_mmadbitmode_section_datatype"></a>

**Table 4** Supported precision type combinations of the left matrix, right matrix, Bias matrix, and result matrix for the Mmad API

| left matrix fm type | right matrix filter type | bias type | result matrix dst type |
| ------------- | ----------------- | --------- | ---------------- |
| int8_t | int8_t | int32_t | int32_t |
| half | half | float | float |
| float | float | float | float |
| bfloat16_t | bfloat16_t | float | float |
| fp8_e4m3fn_t | fp8_e4m3fn_t | float | float |
| fp8_e4m3fn_t | fp8_e5m2_t | float | float |
| fp8_e5m2_t | fp8_e4m3fn_t | float | float |
| fp8_e5m2_t | fp8_e5m2_t | float | float |
| hifloat8_t | hifloat8_t | float | float |

**Table 5** Supported precision type combinations of the left matrix, right matrix, Scale matrix, Bias matrix, and result matrix for the MmadMx API

| left matrix fm | right matrix filter | Scale matrix | Bias | result matrix dst |
| ------------ | ------------ | ---------- | -------- | ----------- |
| fp4x2_e1m2_t | fp4x2_e1m2_t | fp8_e8m0_t | float | float |
| fp4x2_e2m1_t | fp4x2_e1m2_t | fp8_e8m0_t | float | float |
| fp4x2_e1m2_t | fp4x2_e2m1_t | fp8_e8m0_t | float | float |
| fp4x2_e2m1_t | fp4x2_e2m1_t | fp8_e8m0_t | float | float |
| fp8_e4m3fn_t | fp8_e4m3fn_t | fp8_e8m0_t | float | float |
| fp8_e4m3fn_t | fp8_e5m2_t | fp8_e8m0_t | float | float |
| fp8_e5m2_t | fp8_e4m3fn_t | fp8_e8m0_t | float | float |
| fp8_e5m2_t | fp8_e5m2_t | fp8_e8m0_t | float | float |

## Return Value

None

## Constraints

- Constraints on the storage location for different matrices:
  - The result matrix C only supports the physical storage location **L0C Buffer** (**TPosition**: CO1), with a size of 256 KB.
  - The left matrix A only supports the physical storage location **L0A Buffer** (**TPosition**: A2), with a size of 64 KB.
  - The right matrix B only supports the physical storage location **L0B Buffer** (**TPosition**: B2), with a size of 64 KB.
  - The Bias matrix only supports the physical storage location **BT Buffer** (**TPosition**: C2), with a size of 4 KB.
- For address constraints, see Table 1.
- When any of **M**, **K**, and **N** is 0, the instruction is not executed, and this API is treated as a NOP (no operation).
- **K** must be a multiple of 64.
- For the fp4 scenario, the start addresses of the A/B matrices must be 512-byte aligned. For the fp8 scenario, the start addresses of the A/B matrices must be 1024-byte aligned.
- The start address of the scale matrix corresponding to the left matrix A/B is: the corresponding start address of matrix A/B divided by 16.
- For other special scenario constraints, see [Mmad API constraints](Mmad.md#constraints).

## Example

For samples, see [Mmad samples](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/mmad).
