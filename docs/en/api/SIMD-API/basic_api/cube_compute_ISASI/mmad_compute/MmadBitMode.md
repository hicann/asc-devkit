# MmadBitMode<a name="ZH-CN_TOPIC_000000253823118101"></a>

<!-- md-trans-meta sourceCommit=5afce9079f40f95eb7b22e64d4da4de38a1a5378 translatedAt=2026-08-27T16:50:40.616Z -->

## Applicable Products

### Prototype Without Bias

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
### Prototype with bias input

<!-- npu="950" id10 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id10 -->
<!-- npu="A3" id11 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id11 -->
<!-- npu="910b" id12 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id12 -->
<!-- npu="310b" id13 -->
- Atlas 200I/500 A2 inference product: Not supported
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas inference products AI Core: Not supported
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas inference products Vector Core: Not supported
<!-- end id15 -->
<!-- npu="910" id16 -->
- Atlas training products: Not supported
<!-- end id16 -->
## Description

The header file path is `"basic_api/kernel_operator_mm_intf.h"`.

**MmadBitMode** optimizes the construction of the **MmadParams** structure. This API is suitable for scenarios where the scalar pipeline becomes a performance optimization bottleneck, and it supports basic **Mmad**/**MmadMx** computation. The difference between this API and the **Mmad**/**MmadMx** APIs lies in how parameters are passed: this API passes the union structure **MmadBitModeParams**. The design concept of the **MmadBitModeParams** class parameters is described as follows:

A union is a special data structure that allows different data types to be stored at the same memory location. All members of a union share the same memory space, whose size is determined by the largest member, and only one member can be used at a time.

A bit-field is a special class member that allows precise control over the number of memory bits occupied by a member variable in a structure. Member variables in a structure correspond to memory from low-order bits to high-order bits from top to bottom.

The **MmadBitModeParams** class uses the union and bit-field methods. It expresses parameter types with bits, uses a bit-field structure to automatically handle the bit count of input parameters, and leverages the union feature to implement multi-parameter fused passing, so that only one input parameter is needed to contain all required information, and the corresponding underlying API only needs to receive one parameter. In addition, when the value of a certain bit in a parameter needs to be modified, it can be achieved simply through loops and bit operations without re-passing the parameter, which reduces scalar computation and improves performance.

The **MmadBitModeParams** class can be initialized directly using an object of the **MmadBitModeParams** structure type:

```cpp
__aicore__ inline MmadBitModeParams(const MmadBitModeParams &mmadParams_);
```

You can also use the Set function of each parameter to modify parameter values. In addition, because a union is used, you can directly modify **config0** bit by bit to modify parameters.

## Prototype

- Without passing in **bias**

  ```cpp
  template <typename T, typename U, typename S>
  __aicore__ inline void Mmad(const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const MmadBitModeParams& mmadParams)

  template <typename T, typename U, typename S>
  __aicore__ inline void MmadMx(const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const MmadBitModeParams& mmadParams)
  ```

- Passing in **bias**

  ```cpp
  template <typename T, typename U, typename S, typename V>
  __aicore__ inline void Mmad(const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const LocalTensor<V>& bias, const MmadBitModeParams& mmadParams)

  template <typename T, typename U, typename S, typename V>
  __aicore__ inline void MmadMx(const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const LocalTensor<V>& bias, const MmadBitModeParams& mmadParams)
  ```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| ---------- | --------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| dst | Output | Destination operand, that is, the result cube C, of the **LocalTensor** type. The supported physical storage location is the L0C Buffer (TPosition:CO1).<br>The start address of the **LocalTensor** must be aligned to 1024 bytes. |
| fm | Input | Source operand, that is, the left cube A, of the **LocalTensor** type. The supported physical storage location is the L0A Buffer (TPosition:A2).<br>The start address of the scale cube corresponding to the left cube A is: corresponding address of the cube start of cube A/16.<br>For the fp4 scenario, the start address of the **LocalTensor** must be aligned to 512 bytes. For the fp8 scenario, the start address of the **LocalTensor** must be aligned to 1024 bytes. |
| filter | Input | Source operand, that is, the right cube B, of the **LocalTensor** type. The supported physical storage location is the L0B Buffer (TPosition:B2).<br>The start address of the scale cube corresponding to the right cube B is: corresponding address of the cube start of cube B/16.<br>For the fp4 scenario, the start address of the **LocalTensor** must be aligned to 512 bytes. For the fp8 scenario, the start address of the **LocalTensor** must be aligned to 1024 bytes. |
| bias | Input | Source operand, that is, the Bias cube, of the **LocalTensor** type. The supported physical storage location is the BT Buffer (TPosition:C2).<br>The start address of the **LocalTensor** must be aligned to 64 bytes. |
| mmadParams | Input | Cube multiplication related parameters.<br>For the specific definition of this parameter type, see ${INSTALL_DIR}/include/ascendc/basic_api/API/kernel_struct_mm.h. Replace ${INSTALL_DIR} with the file storage path after CANN software installation.<br>For the **MmadBitModeParams** parameter description, see the following table. |

**Table 2** MmadBitModeParams class parameter description

| Parameter | Description |
| -------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------- |
| config0 | Of the uint64_t type, forms a union with the **config0BitMode** parameter of the **MmadBitModeConfig0** bit-field structure type. It is initialized to 0, and its value can be obtained using the **GetConfig0()** function of the class object. |
| config0BitMode | Of the **MmadBitModeConfig0** bit-field structure type. For the parameters, see [Table 3](#zh_cn_topic_mmadbitmode_section4_table3). It forms a union with **config0**. |

**Table 3** MmadBitModeConfig0 structure parameter description<a id="zh_cn_topic_mmadbitmode_section4_table3"></a>

| Parameter | Description |
| -------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| m | Height of the left cube. Value range: m ∈ [0, 4095]. Default value: 0.<br>This parameter is the lowest-order parameter of the bit-field structure and occupies 12 bits. Its value can be set using the **SetM()** function of the **MmadBitModeParams** class object and obtained using the **GetM()** function. |
| k | Width of the left cube and height of the right cube. Value range: k ∈ [0, 4095]. Default value: 0.<br>This parameter is the second low-order parameter of the bit-field structure and occupies 12 bits. Its value can be set using the **SetK()** function of the **MmadBitModeParams** class object and obtained using the **GetK()** function. |
| n | Width of the right cube. Value range: n ∈ [0, 4095]. Default value: 0.<br>This parameter is the third low-order parameter of the bit-field structure and occupies 12 bits. Its value can be set using the **SetN()** function of the **MmadBitModeParams** class object and obtained using the **GetN()** function. |
| unitFlag | Reserved parameter. It is reserved for future functions, and developers do not need to pay attention to it for now. Use the default value.<br>This parameter is the fourth low-order parameter of the bit-field structure and occupies 2 bits. Its value can be set using the **SetUnitFlag()** function of the **MmadBitModeParams** class object and obtained using the **GetUnitFlag()** function. |
| disableGemv | When M = 1, configures whether GEMV is enabled for **Mmad** computation. When the input is false, GEMV is enabled; conversely, when the input is true, GEMV is disabled.<br>GEMV (General Cube-Vector Multiplication) implements the product of a cube and a vector. After GEMV is enabled, when the **Mmad** API reads data from the L0A Buffer, the data is read in ND format instead of being treated as ZZ format.<br>This parameter is the fifth low-order parameter of the bit-field structure and occupies 1 bit. Its value can be set using the **SetDisableGemv()** function of the **MmadBitModeParams** class object and obtained using the **GetDisableGemv()** function. |
| ccubeSource | Configures whether the initial value of the C cube comes from the BT Buffer (TPosition:C2). Default value: false.<br>&nbsp;&nbsp;&bull; false: Comes from the L0C Buffer (TPosition:CO1).<br>&nbsp;&nbsp;&bull; true: Comes from the BT Buffer (TPosition:C2).<br>Note: For APIs with bias input, configuring this parameter is invalid. Whether the initial value of the C cube comes from the L0C Buffer or the BT Buffer is determined based on the position of the bias input.<br>This parameter is the sixth low-order parameter of the bit-field structure and occupies 1 bit. Its value can be set using the **SetCcubeSource()** function of the **MmadBitModeParams** class object and obtained using the **GetCcubeSource()** function. |
| ccubeInitVal | Configures whether the initial value of the C cube is 0. Default value: true.<br>&nbsp;&nbsp;&bull; true: The initial value of the C cube is 0.<br>&nbsp;&nbsp;&bull; false: The initial value of the C cube is configured through the **ccubeSource** parameter.<br>This parameter is the highest-order parameter of the bit-field structure and occupies 1 bit. Its value can be set using the **SetCcubeInitVal()** function of the **MmadBitModeParams** class object and obtained using the **GetCcubeInitVal()** function. |

## Data Type<a id="zh_cn_topic_mmadbitmode_section_datatype"></a>

**Table 4** Precision type combinations supported by the Mmad API for the left cube, right cube, bias cube, and result cube

| left cube fm type | right cube filter type | bias type | result cube dst type |
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

**Table 5** Precision type combinations supported by the MmadMx API for the left cube, right cube, scale cube, bias cube, and result cube

| left cube fm | right cube filter | Scale cube | Bias | result cube dst |
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

- Constraints on the storage locations of different matrices:
  - The result cube C is supported only at the physical storage location L0C Buffer (TPosition:CO1), with a size of 256 KB.
  - The left cube A is supported only at the physical storage location L0A Buffer (TPosition:A2), with a size of 64 KB.
  - The right cube B is supported only at the physical storage location L0B Buffer (TPosition:B2), with a size of 64 KB.
  - The Bias cube is supported only at the physical storage location BT Buffer (TPosition:C2), with a size of 4 KB.
- For address constraints, see Table 1.
- When any of M, K, and N is 0, the instruction is not executed, and this API is treated as a NOP (no operation).
- K must be a multiple of 64.
- For the fp4 scenario, the start addresses of matrices A and B must be aligned to 512 bytes. For the fp8 scenario, the start addresses of matrices A and B must be aligned to 1024 bytes.
- The start address of the scale cube corresponding to the left cube A/B is: the corresponding address of the cube start of A/B divided by 16.
- For other special scenario constraints, see [Mmad API constraints](Mmad.md#constraints).

## Example

For examples, see [Mmad example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/mmad).
