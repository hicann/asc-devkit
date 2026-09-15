# GetWorkspaceLen<a name="ZH-CN_TOPIC_0000001968485296"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T17:39:46.360Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
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
// Six AIVs wait for three AIVs to arrive before starting subsequent operations. A total of 6*512B address space is required, with the start address being the user-specified startAddr.
AscendC::GroupBarrier<AscendC::PipeMode::MTE3_MODE> barA(startAddr, 3, 6);
uint64_t offset = barA.GetWorkspaceLen(); // Returns the Global Memory space occupied by barA.
```

The result is as follows:

```
Size of occupied GlobalMemory (offset): 3072
```

