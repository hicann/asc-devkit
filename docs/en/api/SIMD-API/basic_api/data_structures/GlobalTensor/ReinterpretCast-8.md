# ReinterpretCast<a name="ZH-CN_TOPIC_0000002132450370"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T12:57:07.973Z -->

## Applicable Products<a name="section1550532418810"></a>

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


## Description<a name="section618mcpsimp"></a>

Reinterprets the current **GlobalTensor** as a new type specified by the user. The converted **Tensor** has exactly the same address and content as the original **Tensor**, and the memory size (in bits) of the **Tensor** remains unchanged.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename CAST_T>
__aicore__ inline GlobalTensor<CAST_T> ReinterpretCast() const
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="17.77%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="82.23%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row18835145716587"><td class="cellrowborder" valign="top" width="17.77%" headers="mcps1.2.3.1.1 "><p id="p1383515717581"><a name="p1383515717581"></a><a name="p1383515717581"></a>CAST_T</p></td>
<td class="cellrowborder" valign="top" width="82.23%" headers="mcps1.2.3.1.2 "><p id="p18689719202918"><a name="p18689719202918"></a><a name="p18689719202918"></a>Specifies the new type after reinterpretation.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

Returns the reinterpreted **GlobalTensor**.

## Constraints<a name="section633mcpsimp"></a>

After the data type is converted, the number of elements may not be divisible. For example, when three **int4b_t** elements are converted to **uint32_t**, calling the **GetSize** API after the conversion returns only the floor integer value. In this scenario, a corresponding warning message is displayed when running in the CPU state.

## Example<a name="section17531157161314"></a>

```cpp
uint64_t dataSize = 256; //Set the size of input_global to 256.

AscendC::GlobalTensor<int32_t> inputGlobal; // The type is int32_t.
inputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ int32_t *>(src_gm), dataSize); // Set the start address of the source operand in Global Memory to src_gm, with an external storage size of 256 int32_t elements.

AscendC::LocalTensor<int32_t> inputLocal = inQueueX.AllocTensor<int32_t>();    
AscendC::DataCopy(inputLocal, inputGlobal, dataSize); // Copy inputGlobal in Global Memory to inputLocal in Local Memory.
...
// Assume inputGlobal is of type int32_t and contains 16 elements (64 bytes).
// Call ReinterpretCast to reinterpret inputGlobal as int16_t.
AscendC::GlobalTensor<int16_t> interpreTensor = inputGlobal.template ReinterpretCast<int16_t>();
// The example result is as follows: the data of the two is completely identical, they share the same physical memory address, and they are merely reinterpreted according to different types.
// inputGlobal:0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
// interpreTensor:0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0 10 0 11 0 12 0 13 0 14 0 15 0
```

