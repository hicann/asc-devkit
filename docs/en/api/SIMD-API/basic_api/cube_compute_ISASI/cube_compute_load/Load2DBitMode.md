# LoadData (BitMode 2D Cube Transfer)<a id="ZH-CN_TOPIC_0000002517418694"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T16:27:40.633Z -->

## Applicable Products<a id="section1550532418810"></a>

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
## Description<a id="section618mcpsimp"></a>

It transfers 2D-format data required for ordinary cube computation, in units of 512-byte data fractals. The supported data paths are L1 Buffer->L0A Buffer and L1 Buffer->L0B Buffer.

The difference between this API and the Load2DV2 API lies in how parameters are passed. This API passes a union structure **Load2DBitModeParam**.

## Prototype<a id="section620mcpsimp"></a>

```cpp
template <TPosition Dst, TPosition Src, typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src,const Load2DBitModeParam& loadDataParams)
```

## Parameters<a id="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ---------- | ---------- |
| T | Data type of the source operand and destination operand. |
| Src | Logical position (**TPosition**) where the source operand is stored. The supported values are **A1** and **B1**, used only by the **Load2DBitMode** API. |
| Dst | Logical position (**TPosition**) where the destination operand is stored. The supported values are **A2** and **B2**, used only by the **Load2DBitMode** API. |

**Table 2** General parameter description

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, of the **LocalTensor** type.<br>The continuous arrangement order of data is determined by the **TPosition** where the destination operand resides. The specific constraints are as follows:<br>&nbsp;&nbsp;&bull; **A2**: ZZ format/NZ format; the corresponding fractal size is 16 \* (32 bytes / sizeof(T)).<br>&nbsp;&nbsp;&bull; **B2**: ZN format; the corresponding fractal size is (32 bytes / sizeof(T)) \* 16.<br>&nbsp;&nbsp;&bull; **A1**/**B1**: No format requirement, generally NZ format. In NZ format, the corresponding fractal size is 16 * (32 bytes / sizeof(T)). |
| src | Input | Source operand, of the **LocalTensor** type.<br>The data type must be consistent with **dst**. |
| loadDataParams | Input | **LoadData** parameter structure, of the **Load2DBitModeParam** type. For details, see [Table 3](#table10539223195311).<br>For the definition of the above structure parameters, see \$\{INSTALL\_DIR\}/include/ascendc/basic\_api/API/kernel\_struct\_mm.h. Replace \$\{INSTALL\_DIR\} with the file storage path after CANN software installation. |

**Table 3** Load2DBitModeParam class parameter description<a id="table10539223195311"></a>

| Parameter | Description |
| ---------- | ---------- |
| config0 | **uint64_t** type. Forms a union with the **Load2DBitModeConfig0** bit-field structure type parameter **config0BitMode**. Initialized to 0. You can use the **GetConfig0()** function of the class object to obtain its value. |
| config0BitMode | **Load2DBitModeConfig0** bit-field structure type. For details, see [Table 4](#table4109172132317). Forms a union with **config0**. |
| config1 | **uint64_t** type. Forms a union with the **Load2DBitModeConfig1** bit-field structure type parameter **config1BitMode**. Initialized to 0. You can use the **GetConfig1()** function of the class object to obtain its value. |
| config1BitMode | **Load2DBitModeConfig1** bit-field structure type. For details, see [Table 5](#table122891852142311). Forms a union with **config1**. |
| ifTranspose | Whether to enable the transpose function, which transposes each fractal cube. Defaults to **false**. Its meaning is the same as that of the parameter with the same name in the **LoadData2DParamsV2** structure. For details, see [LoadData (2D cube transfer)](Load2D.md).<br>&nbsp;&nbsp;&bull; **true**: Enable.<br>&nbsp;&nbsp;&bull; **false**: Disable.<br>When the transpose function is enabled, the supported data type constraints are as follows:<br>The source operand and destination operand support the **b4**, **b8**, **b16**, and **b32** data types. |

Design concept of the **Load2DBitModeParam** class parameters:

A union is a special data structure that allows different data types to be stored at the same memory location. All members of a union share the same memory space, whose size is determined by the largest member. Only one member can be used at a time.

A bit field is a special class member that allows precise control over the number of memory bits occupied by member variables in the structure. Member variables in the structure correspond to memory from low bits to high bits from top to bottom.

The **Load2DBitModeParam** class uses the union and bit-field methods to express parameter types in bits, uses a bit-field structure to automatically process the bit count of input parameters, and leverages the characteristics of the union to implement multi-parameter fused passing. Only one input parameter needs to be passed to contain all required information, and the corresponding underlying API only needs to receive one parameter. In addition, when the value of a certain bit in a parameter needs to be modified, this can be achieved simply through loops and bit operations, without the need to pass the parameter again.

Compared with the **Load2D** API that uses the **LoadData2DParamsV2** structure, this reduces scalar computation and improves performance.

The **Load2DBitModeParam** class can be initialized directly with an object of the **LoadData2DParamsV2** structure type:

```cpp
LoadData2DParamsV2 loadDataParams;
loadDataParams.mStartPosition = 0;
loadDataParams.kStartPosition = 0;
loadDataParams.mStep = 2;
loadDataParams.kStep = 2;
loadDataParams.srcStride = 2;
loadDataParams.dstStride = 2;
loadDataParams.sid = 0;
loadDataParams.ifTranspose = false;
Load2DBitModeParam params(loadDataParams);  // Initialize directly with an object of the LoadData2DParamsV2 structure type.
```

You can also use the Set functions of each parameter to modify parameter values. In addition, because a union is used, you can also directly modify **config0** and **config1** bit by bit to modify parameters.

**Table 4** Load2DBitModeConfig0 structure parameter description<a name="table4109172132317"></a>

| Parameter | Meaning |
| ---------- | ---------- |
| mStartPosition | Take an M*K cube as an example, the start position of the source cube along the M axis, in units of 16 elements.<br>This parameter is the lowest bit parameter of the bit-field structure and occupies 16 bits. You can use the **SetMStartPosition()** function of the **Load2DBitModeParam** class object to set its value and the **GetMStartPosition()** function to obtain its value. For details, see [Table 6](#table1123714295457). |
| kStartPosition | Take an M*K cube as an example, the start position of the source cube along the K axis, in units of 32 bytes.<br>This parameter is the second-lowest bit parameter of the bit-field structure and occupies 16 bits. You can use the **SetKStartPosition()** function of the **Load2DBitModeParam** class object to set its value and the **GetKStartPosition()** function to obtain its value. For details, see [Table 6](#table1123714295457). |
| mStep | Take an M*K cube as an example, the transfer length of the source cube along the M axis, in units of 16 elements. Value range: mStep∈[0, 255].<br>When the transpose function is enabled through the **ifTranspose** parameter, in addition to satisfying the value range [0, 255], **mStep** must also satisfy the following additional constraints:<br>&nbsp;&nbsp;&bull; When the data type is b4, **mStep** must be a multiple of 4;<br>&nbsp;&nbsp;&bull; When the data type is b8, **mStep** must be a multiple of 2;<br>&nbsp;&nbsp;&bull; When the data type is b16, **mStep** must be a multiple of 1;<br>&nbsp;&nbsp;&bull; When the data type is b32, **mStep** has no additional constraint.<br>This parameter is the third-lowest bit parameter of the bit-field structure and occupies 8 bits. You can use the **SetMStep()** function of the **Load2DBitModeParam** class object to set its value and the **GetMStep()** function to obtain its value. For details, see [Table 6](#table1123714295457). |
| kStep | Take an M*K cube as an example, the transfer length of the source cube along the K axis, in units of 32 bytes. Value range: kStep∈[0, 255].<br>When the transpose function is enabled through the **ifTranspose** parameter, in addition to satisfying the value range [0, 255], **kStep** must also satisfy the following additional constraints:<br>&nbsp;&nbsp;&bull; When the data type is b4, b8, or b16, **kStep** has no additional constraint;<br>&nbsp;&nbsp;&bull; When the data type is b32, **kStep** must be a multiple of 2.<br>This parameter is the highest bit parameter of the bit-field structure and occupies 8 bits. You can use the **SetKStep()** function of the **Load2DBitModeParam** class object to set its value and the **GetKStep()** function to obtain its value. For details, see [Table 6](#table1123714295457). |

The meanings of the parameters in the **Load2DBitModeConfig0** structure are the same as those of the parameters with the same names in the **LoadData2DParamsV2** structure. For details, see [LoadData (2D cube transfer)](Load2D.md).

**Table 5** Load2DBitModeConfig1 structure parameter description<a name="table122891852142311"></a>

| Parameter | Meaning |
| ---------- | ---------- |
| srcStride | Take an M*K cube as an example, the interval between the start address of the previous fractal and the start address of the next fractal along the K direction of the source cube, in units of 512 bytes.<br>This parameter is the lowest bit parameter of the bit-field structure and occupies 16 bits. You can use the **SetSrcStride()** function of the **Load2DBitModeParam** class object to set its value and the **GetSrcStride()** function to obtain its value. For details, see [Table 6](#table1123714295457). |
| dstStride | Take an M*K cube as an example, the interval between the start address of the previous fractal and the start address of the next fractal along the K direction of the destination cube, in units of 512 bytes.<br>This parameter is the highest bit parameter of the bit-field structure and occupies 16 bits. You can use the **SetDstStride()** function of the **Load2DBitModeParam** class object to set its value and the **GetDstStride()** function to obtain its value. For details, see [Table 6](#table1123714295457). |

The meanings of the parameters in the **Load2DBitModeConfig1** structure are the same as those of the parameters with the same names in the **LoadData2DParamsV2** structure. For details, see [LoadData (2D cube transfer)](Load2D.md).

**Table 6** Description of the member functions of the Load2DBitModeParam class<a name="table1123714295457"></a>

| Function Name | Function |
| ---------- | ---------- |
| void SetMStartPosition(uint32_t mStartPosition_) | Sets the value of the **mStartPosition** parameter in the **Load2DBitModeConfig0** structure to **mStartPosition_**. |
| void SetKStartPosition(uint32_t kStartPosition_) | Sets the value of the **kStartPosition** parameter in the **Load2DBitModeConfig0** structure to **kStartPosition_**. |
| void SetMStep(uint16_t mStep_) | Sets the value of the **mStep** parameter in the **Load2DBitModeConfig0** structure to **mStep_**. |
| void SetKStep(uint16_t kStep_) | Sets the value of the **kStep** parameter in the **Load2DBitModeConfig0** structure to **kStep_**. |
| void SetSrcStride(int32_t srcStride_) | Sets the value of the **srcStride** parameter in the **Load2DBitModeConfig1** structure to **srcStride_**. |
| void SetDstStride(uint16_t dstStride_) | Sets the value of the **dstStride** parameter in the **Load2DBitModeConfig1** structure to **dstStride_**. |
| uint32_t GetMStartPosition() const | Obtains the value of the **mStartPosition** parameter in the **Load2DBitModeConfig0** structure. |
| uint32_t GetKStartPosition() const | Obtains the value of the **kStartPosition** parameter in the **Load2DBitModeConfig0** structure. |
| uint16_t GetMStep() const | Obtains the value of the **mStep** parameter in the **Load2DBitModeConfig0** structure. |
| uint16_t GetKStep() const | Obtains the value of the **kStep** parameter in the **Load2DBitModeConfig0** structure. |
| int32_t GetSrcStride() const | Obtains the value of the **srcStride** parameter in the **Load2DBitModeConfig1** structure. |
| uint16_t GetDstStride() const | Obtains the value of the **dstStride** parameter in the **Load2DBitModeConfig1** structure. |

## Data Types

Supported data types: **int8_t**, **uint8_t**, **fp4x2_e2m1_t**, **fp4x2_e1m2_t**, **hifloat8_t**, **fp8_e5m2_t**, **fp8_e4m3fn_t**, **half**, **bfloat16_t**, **int32_t**, **uint32_t**, **float**.

## Return Value<a id="section640mcpsimp"></a>

None

## Constraints<a id="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).
<!-- npu="950" id10 -->
- For Ascend 950PR/Ascend 950DT, only the L1 Buffer->L0A Buffer and L1 Buffer->L0B Buffer data paths are supported.
<!-- end id10 -->

## Example<a id="section6461234123118"></a>

For the complete transfer process, see [Load2DV2 sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/03_cube_compute/load_data_2dv2_l12l0). For the use of **Load2DBitMode**, refer to the following example, where cube A on L1 uses NZ data layout with shape [M,K], and the **LoadData** instruction is called to complete the Nz2Nz transfer from L1 Buffer to L0A Buffer.

- Example 1: Use **LoadData2DParamsV2** to construct **Load2DBitModeParam**

```cpp
constexpr uint32_t fractalElemCount = 256;
constexpr uint32_t tensorElemCount = 4 * fractalElemCount;

// Source operand: L1 Buffer, which stores a 32x32 half cube.
AscendC::LocalTensor<half> srcLocal(AscendC::TPosition::A1, a1Addr, tensorElemCount);

// Destination operand: L0A Buffer, which reserves four 512B fractals to receive the complete 32x32 half cube.
AscendC::LocalTensor<half> dstLocal(AscendC::TPosition::A2, a2Addr, tensorElemCount);

AscendC::LoadData2DParamsV2 loadDataParams;
// Start the transfer from the 0th 16-element position on the M axis of the source cube.
loadDataParams.mStartPosition = 0;
// Start the transfer from the 0th 32B position on the K axis of the source cube.
loadDataParams.kStartPosition = 0;
// Transfer 2 * 16 elements in the M direction.
loadDataParams.mStep = 2;
// Transfer 32 half elements in the K direction, that is, 2 * 32B.
loadDataParams.kStep = 2;
// The source L1 has 2 fractals in the M direction, and the start address interval between adjacent source fractals in the K direction is 2 * 512B.
loadDataParams.srcStride = 2;
// The destination L0A is arranged contiguously by 2 M-direction fractals, and the start address interval between adjacent destination fractals in the K direction is 2 * 512B.
loadDataParams.dstStride = 2;
// Do not transpose each fractal; keep the fractal direction in L1 when transferring into L0A.
loadDataParams.ifTranspose = false;
// Reserved parameter, fixed to 0.
loadDataParams.sid = 0;

// Use LoadData2DParamsV2 to construct the bit mode parameters, and pass the preceding transfer configuration in bit form at the underlying layer.
AscendC::Load2DBitModeParam bitModeParams(loadDataParams);
AscendC::LoadData<AscendC::TPosition::A2, AscendC::TPosition::A1, half>(dstLocal, srcLocal, bitModeParams);
```

- Example 2: Use Set functions to modify Load2DBitModeParam

```cpp
AscendC::Load2DBitModeParam bitModeParams;
// Start transferring from the 0th 16-element block on the M axis of the source cube.
bitModeParams.SetMStartPosition(0);
// Start transferring from the 0th 32B block on the K axis of the source cube.
bitModeParams.SetKStartPosition(0);
// Transfer 2 * 16 elements in the M direction.
bitModeParams.SetMStep(2);
// Transfer 32 half elements in the K direction, that is, 2 * 32B.
bitModeParams.SetKStep(2);
// The source L1 has 2 fractals in the M direction, and the start address interval between adjacent source fractals in the K direction is 2 * 512B.
bitModeParams.SetSrcStride(2);
// The destination L0A is arranged contiguously by 2 M-direction fractals, and the start address interval between adjacent destination fractals in the K direction is 2 * 512B.
bitModeParams.SetDstStride(2);
// Do not transpose each fractal; keep the fractal direction in L1 when transferring into L0A.
bitModeParams.SetIfTranspose(false);

AscendC::LoadData<AscendC::TPosition::A2, AscendC::TPosition::A1, half>(dstLocal, srcLocal, bitModeParams);
```
