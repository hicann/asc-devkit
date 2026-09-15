# GetWithOffset<a name="ZH-CN_TOPIC_0000001810876828"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:25:43.481Z -->

## Applicable Products<a name="section1550532418810"></a>

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
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Supported
<!-- end id7 -->


## Description<a name="section954881431816"></a>

Uses **TBuf** as the base address, offsets backward by a specified length, and uses the offset address as the start address to extract a **Tensor** of the specified length.

## Prototype<a name="section1449617323189"></a>

```
template <typename T>
__aicore__ inline LocalTensor<T> GetWithOffset(uint32_t size, uint32_t bufOffset)
```

## Parameters<a name="section85121215142514"></a>

**Table 1** Template parameter description

<a name="table012895562310"></a>
<table><thead align="left"><tr id="row2128195532318"><th class="cellrowborder" valign="top" width="12.65%" id="mcps1.2.3.1.1"><p id="p1212885512232"><a name="p1212885512232"></a><a name="p1212885512232"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="87.35000000000001%" id="mcps1.2.3.1.2"><p id="p16129155552315"><a name="p16129155552315"></a><a name="p16129155552315"></a>Meaning</p></th>
</tr>
</thead>
<tbody><tr id="row6129355182310"><td class="cellrowborder" valign="top" width="12.65%" headers="mcps1.2.3.1.1 "><p id="p81291855102318"><a name="p81291855102318"></a><a name="p81291855102318"></a>T</p></td>
<td class="cellrowborder" valign="top" width="87.35000000000001%" headers="mcps1.2.3.1.2 "><p id="p14983161812418"><a name="p14983161812418"></a><a name="p14983161812418"></a>Data type of the Tensor to be obtained.</p></td>
</tr>
</tbody>
</table>

**Table 2** Parameter description

<a name="table1794522316251"></a>
<table><thead align="left"><tr id="row19456238252"><th class="cellrowborder" valign="top" width="12.36%" id="mcps1.2.4.1.1"><p id="p119458239258"><a name="p119458239258"></a><a name="p119458239258"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="12.379999999999999%" id="mcps1.2.4.1.2"><p id="p9945152332514"><a name="p9945152332514"></a><a name="p9945152332514"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="75.26%" id="mcps1.2.4.1.3"><p id="p1594552312513"><a name="p1594552312513"></a><a name="p1594552312513"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1694552372511"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p1094516239250"><a name="p1094516239250"></a><a name="p1094516239250"></a>**size**</p></td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.2.4.1.2 "><p id="p8945112312514"><a name="p8945112312514"></a><a name="p8945112312514"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p169454239253"><a name="p169454239253"></a><a name="p169454239253"></a>Number of **Tensor** elements to obtain.</p></td>
</tr>
<tr id="row165981554174"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p959810553176"><a name="p959810553176"></a><a name="p959810553176"></a>**bufOffset**</p></td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.2.4.1.2 "><p id="p1659820553174"><a name="p1659820553174"></a><a name="p1659820553174"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p125983554175"><a name="p125983554175"></a><a name="p125983554175"></a>Offset length from the start position, in bytes, which must be 32-byte aligned.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="section633mcpsimp"></a>

The value of **size** is the number of elements in the tensor. **size**\*sizeof(T) + **bufOffset** must not exceed the length specified when **TBuf** is initialized.

**bufOffset** must meet the 32-byte alignment requirement.

## Return Value<a name="section640mcpsimp"></a>

Returns the obtained [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md).

## Example<a name="section5725818154718"></a>

```
// Allocate memory for TBuf initialization, with a length of 1024 bytes.
AscendC::TPipe pipe;
AscendC::TBuf<AscendC::TPosition::VECCALC> calcBuf; // The template parameter is the VECCALC type in TPosition.
uint32_t byteLen = 1024;
pipe.InitBuffer(calcBuf, byteLen);
// Obtain a Tensor from calcBuf with an offset of 64 bytes. The Tensor has a memory size of 128 int32_t elements, that is, 512 bytes.
AscendC::LocalTensor<int32_t> tempTensor1 = calcBuf.GetWithOffset<int32_t>(128, 64);
```
