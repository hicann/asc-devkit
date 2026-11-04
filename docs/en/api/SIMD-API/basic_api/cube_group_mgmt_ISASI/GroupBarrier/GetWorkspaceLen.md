# GetWorkspaceLen<a name="ZH-CN_TOPIC_0000001968485296"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T13:49:54.155Z pushedAt=2026-10-26T07:41:44.237Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR & 950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_group_mgmt_ISASI/GroupBarrier/GetWorkspaceLen_res.md#id1 -->


## Description<a name="zh-cn_topic_0000001526206862_section212607105720"></a>

Returns the size of the Global Memory message space occupied by the current **GroupBarrier**.

## Prototype<a name="section765814724715"></a>

```
__aicore__ inline uint64_t GetWorkspaceLen()
```

## Parameters<a name="zh-cn_topic_0000001526206862_section129451113125413"></a>

None

## Return Value<a name="section39217325237"></a>

Returns the size of the Global Memory message space currently occupied by **GroupBarrier**.

## Constraints<a name="zh-cn_topic_0000001526206862_section65498832"></a>

None

## Example<a name="zh-cn_topic_0000001526206862_section97001499599"></a>

```
// After 6 AIVs wait for 3 AIVs to arrive, subsequent operations start, requiring a total of 6*512B address space, with the starting address being the user-specified startAddr.
AscendC::GroupBarrier<AscendC::PipeMode::MTE3_MODE> barA(startAddr, 3, 6);
uint64_t offset = barA.GetWorkspaceLen(); // Returns the GlobalMemory space occupied by barA.
```

The result example is as follows:

```
The size of the occupied GlobalMemory space (offset) is: 3072
```
