# LocalTensor constructor<a name="ZH-CN_TOPIC_0000002330767510"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T13:07:44.029Z -->

## Applicable Products<a name="section1550532418810"></a>

### Whether Pipe Framework Is Supported

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Supported
<!-- end id7 -->

### Whether Static Tensor Programming Is Supported

<!-- npu="950" id8 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 inference product: Not supported
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas inference products AI Core: Supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products Vector Core: Supported
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas training products: Not supported
<!-- end id14 -->


## Description<a name="section618mcpsimp"></a>

**LocalTensor** constructor.

## Prototype<a name="section620mcpsimp"></a>

-   Applicable to the [Pipe programming framework](../../../../../guide/programming_guide/programming_model/ai_core_simd_programming/tpipe_tque_programming/tpipe_tque_framework_programming_paradigm.md). Normally, developers do not call this function directly. This function does not initialize the member variables of **LocaTensor**, which remain random values.

    ```cpp
    __aicore__ inline LocalTensor<T>() {}
    ```

-   Applicable to [static Tensor programming](../../../../../guide/programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md). Returns a **Tensor** object based on the specified logical position, address, and length.

    ```cpp
    __aicore__ inline LocalTensor<T>(TPosition pos, uint32_t addr, uint32_t tileSize)
    __aicore__ inline LocalTensor<T>(uint32_t addr)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001429830437_row118356578583"><th class="cellrowborder" valign="top" width="16.28%" id="mcps1.2.3.1.1"><p id="zh-cn_topic_0000001429830437_p48354572582"><a name="zh-cn_topic_0000001429830437_p48354572582"></a><a name="zh-cn_topic_0000001429830437_p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="83.72%" id="mcps1.2.3.1.2"><p id="zh-cn_topic_0000001429830437_p583535795817"><a name="zh-cn_topic_0000001429830437_p583535795817"></a><a name="zh-cn_topic_0000001429830437_p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001429830437_row1835857145817"><td class="cellrowborder" valign="top" width="16.28%" headers="mcps1.2.3.1.1 "><p id="zh-cn_topic_0000001429830437_p5835457165816"><a name="zh-cn_topic_0000001429830437_p5835457165816"></a><a name="zh-cn_topic_0000001429830437_p5835457165816"></a>T</p></td>
<td class="cellrowborder" valign="top" width="83.72%" headers="mcps1.2.3.1.2 "><a name="ul171781281310"></a><a name="ul171781281310"></a><ul id="ul171781281310"><li>Prototype applicable to the Pipe programming framework, supporting basic data types and the <a href="../../aux_data_structures/TensorTrait/TensorTrait.md">TensorTrait</a> type.</li><li>Prototype applicable to static Tensor programming, supporting the following data types:<pre class="screen" id="screen148501140172610"><a name="screen148501140172610"></a><a name="screen148501140172610"></a>// Only basic data types are supported.
__aicore__ inline LocalTensor&lt;T&gt;(TPosition pos, uint32_t addr, uint32_t tileSize)
// Only the TensorTrait type is supported.
__aicore__ inline LocalTensor&lt;T&gt;(uint32_t addr)</pre>
</li></ul></td>
</tr>
</tbody>
</table>

**Table 2** Parameter description

<a name="zh-cn_topic_0235751031_table33761356"></a>
<table><thead align="left"><tr id="zh-cn_topic_0235751031_row27598891"><th class="cellrowborder" valign="top" width="16.53%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0235751031_p20917673"><a name="zh-cn_topic_0235751031_p20917673"></a><a name="zh-cn_topic_0235751031_p20917673"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="10.4%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0235751031_p16609919"><a name="zh-cn_topic_0235751031_p16609919"></a><a name="zh-cn_topic_0235751031_p16609919"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="73.07000000000001%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0235751031_p59995477"><a name="zh-cn_topic_0235751031_p59995477"></a><a name="zh-cn_topic_0235751031_p59995477"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="16.53%" headers="mcps1.2.4.1.1 "><p id="p284425844311"><a name="p284425844311"></a><a name="p284425844311"></a>**pos**</p></td>
<td class="cellrowborder" valign="top" width="10.4%" headers="mcps1.2.4.1.2 "><p id="p158449584436"><a name="p158449584436"></a><a name="p158449584436"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.07000000000001%" headers="mcps1.2.4.1.3 "><p id="p17825114223"><a name="p17825114223"></a><a name="p17825114223"></a>Logical position where the LocalTensor resides.</p></td>
</tr>
<tr id="row74161018112218"><td class="cellrowborder" valign="top" width="16.53%" headers="mcps1.2.4.1.1 "><p id="p1041641811229"><a name="p1041641811229"></a><a name="p1041641811229"></a>**addr**</p></td>
<td class="cellrowborder" valign="top" width="10.4%" headers="mcps1.2.4.1.2 "><p id="p241611832219"><a name="p241611832219"></a><a name="p241611832219"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.07000000000001%" headers="mcps1.2.4.1.3 "><p id="p241615183223"><a name="p241615183223"></a><a name="p241615183223"></a>Start address of the LocalTensor, ranging from [0, the maximum value of the corresponding physical memory). The start address must be 32-byte aligned.</p></td>
</tr>
<tr id="row583211182317"><td class="cellrowborder" valign="top" width="16.53%" headers="mcps1.2.4.1.1 "><p id="p1083210116237"><a name="p1083210116237"></a><a name="p1083210116237"></a>**tileSize**</p></td>
<td class="cellrowborder" valign="top" width="10.4%" headers="mcps1.2.4.1.2 "><p id="p183281112316"><a name="p183281112316"></a><a name="p183281112316"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="73.07000000000001%" headers="mcps1.2.4.1.3 "><p id="p20832711172314"><a name="p20832711172314"></a><a name="p20832711172314"></a>The sum of the number of elements in **LocalTensor**, **addr**, and **tileSize** (converted to the number of bytes occupied) must not exceed the range of the corresponding physical memory.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section17531157161314"></a>

This section provides usage examples of the LocalTensor constructor and call examples of all its member functions.

```cpp
// srcLen = 256, num = 100, M=50
// Example 1
for (int32_t i = 0; i < srcLen; ++i) {
    inputLocal.SetValue(i, num); // Assign num to the i-th position in inputLocal.
}
// The result of Example 1 is as follows:
// Data (inputLocal): [100 100 100  ... 100]

// Example 2
for (int32_t i = 0; i < srcLen; ++i) {
    auto element = inputLocal.GetValue(i); // Obtain the value at the i-th position in inputLocal.
}
// The result of Example 2 is as follows:
// element is 100.

// Example 3
for (int32_t i = 0; i < srcLen; ++i) {
    inputLocal(i) = num; // Assign num to the i-th position in inputLocal.
}
// The result of Example 3 is as follows:
// Data (inputLocal): [100 100 100  ... 100]

// Example 4
for (int32_t i = 0; i < srcLen; ++i) {
    auto element = inputLocal(i); // Obtain the value at the i-th position in inputLocal.
}
// The result of Example 4 is as follows:
// element is 100.

// Example 5
auto size = inputLocal.GetSize(); // Obtain the length of inputLocal. The size is the number of elements in inputLocal.
// The result of Example 5 is as follows:
// The size is srcLen, which is 256.

// Example 6
// Usage of operator[]. inputLocal[16] is a new tensor with an offset of 16 from the start address.
AscendC::Add(outputLocal[16], inputLocal[16], inputLocal2[16], M);
// The result of Example 6 is as follows:
// Input data (inputLocal): [100 100 100 ... 100]
// Input data (inputLocal2): [1 2 3 ... 66]
// Output data (outputLocal): [... 117 118 119 ... 166]

// Example 7
AscendC::TTagType tag = 10;
inputLocal.SetUserTag(tag); // Set the tag information for the LocalTensor.

// Example 8
AscendC::LocalTensor<half> tensor1 = que1.DeQue<half>();
AscendC::TTagType tag1 = tensor1.GetUserTag();
AscendC::LocalTensor<half> tensor2 = que2.DeQue<half>();
AscendC::TTagType tag2 = tensor2.GetUserTag();
AscendC::LocalTensor<half> tensor3 = que3.AllocTensor<half>();
/*Use Tag to control conditional statement execution.*/
if ((tag1 <= 10) && (tag2 >= 9)) {
    AscendC::Add(tensor3, tensor1, tensor2, TILE_LENGTH); // The addition operation can be performed only when tag1 is less than or equal to 10 and tag2 is greater than or equal to 9.
}
// Example 9
// input_local is of the int32_t type and contains 16 elements (64 bytes).
for (int32_t i = 0; i < 16; ++i) {
    inputLocal.SetValue(i, i); // Assign the value i to the i-th position in inputLocal.
}

// Call ReinterpretCast to reinterpret input_local as the int16_t type.
AscendC::LocalTensor<int16_t> interpreTensor = inputLocal.ReinterpretCast<int16_t>();
// The result of Example 9 is as follows. The data of the two is completely identical, and they share the same physical memory address, with only reinterpretation performed based on different types.
// inputLocal:0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
// interpreTensor:0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0 10 0 11 0 12 0 13 0 14 0 15 0

// Example 10
// Call GetPhyAddr() to return the LocalTensor address. On the CPU, a pointer type (T*) is returned; on the NPU, the physical storage address (uint64_t) is returned.
#ifdef ASCENDC_CPU_DEBUG
float *inputLocalCpuPtr = inputLocal.GetPhyAddr();
uint64_t realAddr = (uint64_t)inputLocalCpuPtr - (uint64_t)(GetTPipePtr()->GetBaseAddr(static_cast<int8_t>(AscendC::TPosition::VECCALC)));
#else
uint64_t realAddr = inputLocal.GetPhyAddr();
#endif

// Example 11.
AscendC::TPosition srcPos = (AscendC::TPosition)inputLocal.GetPosition();
if (srcPos == AscendC::TPosition::VECCALC) {
    // Handle logic 1.
} else if (srcPos == AscendC::TPosition::A1) {
    // Handle logic 2.
} else {
    // Handle logic 3.
}

// Example 12.
// Obtain the length of localTensor (in bytes). Since the data type is int32_t, the length is 16*sizeof(int32_t).
uint32_t len = inputLocal.GetLength();
// inputLocal:0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
// len: 64

// Example 13: Set the ShapeInfo information of the Tensor.
AscendC::LocalTensor<float> maxUb = softmaxMaxBuf.template Get<float>();
uint32_t shapeArray[] = {16, 1024};
maxUb.SetShapeInfo(AscendC::ShapeInfo(2, shapeArray, AscendC::DataFormat::ND));

// Example 14: Obtain the ShapeInfo information of the Tensor.
AscendC::ShapeInfo maxShapeInfo = maxUb.GetShapeInfo();
uint32_t orgShape0 = maxShapeInfo.originalShape[0];
uint32_t orgShape1 = maxShapeInfo.originalShape[1];
uint32_t orgShape2 = maxShapeInfo.originalShape[2];
uint32_t orgShape3 = maxShapeInfo.originalShape[3];
uint32_t shape2 = maxShapeInfo.shape[2];

// Example 15: SetAddrWithOffset is used to quickly obtain and define a Tensor while specifying the offset of the new Tensor relative to the base address of the old Tensor.
// Note that the offset length is the number of elements in the old Tensor.
AscendC::LocalTensor<float> tmpBuffer1 = tempBmm2Queue.AllocTensor<float>();
AscendC::LocalTensor<half> tmpHalfBuffer;
tmpHalfBuffer.SetAddrWithOffset(tmpBuffer1, calcSize * 2);

// Example 16: SetBufferLen. The following example changes the length of the applied tensor to 1024 (in bytes).
AscendC::LocalTensor<float> tmpBuffer2 = tempBmm2Queue.AllocTensor<float>();
tmpBuffer2.SetBufferLen(1024);

// Example 17: SetSize. The following example changes the length of the applied tensor to 256 (in elements).
AscendC::LocalTensor<float> tmpBuffer3 = tempBmm2Queue.AllocTensor<float>();
tmpBuffer3.SetSize(256);

#ifdef ASCENDC_CPU_DEBUG
// Example 18: Only for CPU debugging. Dump LocalTensor data to a file for precision debugging. The file is saved in the execution directory.
AscendC::LocalTensor<float> tmpTensor = softmaxMaxBuf.template Get<float>();
tmpTensor.ToFile("tmpTensor.bin");

// Example 19: Only for CPU debugging. Print LocalTensor data in the debug window for precision debugging. Each line prints the data of one datablock (32 bytes).
AscendC::LocalTensor<int32_t> inputLocal = softmaxMaxBuf.template Get<int32_t>();
for (int32_t i = 0; i < 16; ++i) {
    inputLocal.SetValue(i, i); // Assign the value i to the i-th position in input_local.
}
inputLocal.Print();
// 0000: 0 1 2 3 4 5 6 7 8
// 0008: 9 10 11 12 13 14 15
#endif

// Example 20: Used in static Tensor programming scenarios. Construct a Tensor object based on the passed logical position VECIN, start address 128, element count 32, and data type float.
uint32_t addr = 128;
uint32_t tileSize = 32;
AscendC::LocalTensor<float> tensor1 = AscendC::LocalTensor<float>(AscendC::TPosition::VECIN, addr, tileSize);
// Construct a Tensor object based on the passed TensorTrait information and start address 128.
// Its logical position is VECIN, the data type is float, and the number of Tensor elements is 16*16*16.
template <uint32_t v>
using UIntImm = Std::integral_constant<uint32_t, v>;
...
auto shape = AscendC::MakeShape(UIntImm<16>{}, UIntImm<16>{}, UIntImm<16>{});
auto stride = AscendC::MakeStride(UIntImm<0>{}, UIntImm<0>{}, UIntImm<0>{});
auto layoutMake = AscendC::MakeLayout(shape, stride);
auto tensorTraitMake = AscendC::MakeTensorTrait<float, AscendC::TPosition::VECIN>(layoutMake);
uint32_t addr = 128;
auto tensor1 = AscendC::LocalTensor<decltype(tensorTraitMake)>(addr);
```
