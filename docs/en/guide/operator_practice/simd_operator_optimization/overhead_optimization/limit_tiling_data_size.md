# Limiting the TilingData Structure Size<a name="ZH-CN_TOPIC_0000001846878548"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T12:50:37.937Z pushedAt=2026-11-03T07:05:13.347Z nl=2 -->

[Priority] Medium

[Description] The **TilingData** structure is the carrier of tiling information. After the Host side computes the tiling based on the tiling strategy, the operator passes the tiling information from the Host side to the Device side as an input parameter. At this point, the tiling information is stored in the GM. After the **GET\_TILING\_DATA** macro is called, the tiling information is copied from the GM to the stack space of the AI processor, which incurs copy overhead. Because GM access is inefficient and the stack space is limited, the **TilingData** structure size must be limited. The copy takes microseconds. In small-shape scenarios, this optimization yields more noticeable benefits.

You can limit the **TilingData** structure size from the following aspects:

-   Remove unnecessary **TilingData** structure variables.
-   Select an appropriate variable type based on the data range of the tiling.
-   Arrange the **TilingData** structure properly.
-   The overall **TilingData** structure must be padded to 8-byte alignment.

[Negative example]

-   The following example shows redundant TilingData structure variables: the NumBlocks information has already been set through the SetSimdNumBlocks API and can be obtained by calling the GetBlockNum API on the kernel function side, so it does not need to be passed through the TilingData structure.
-   In addition, the data types of the variables are also unreasonable: formerNum and tailNum are the number of cores for computing the whole block data and the tail block data respectively, and will not exceed the value of NUM\_BLOCKS, so the uint8\_t type is sufficient; variables such as formerLength will not exceed the range of uint32\_t according to their computation logic, so the uint32\_t type is sufficient.

```
// Tiling structure definition
BEGIN_TILING_DATA_DEF(TilingDataUnalign)
  TILING_DATA_FIELD_DEF(uint64_t, numBlocks);
  TILING_DATA_FIELD_DEF(uint64_t, formerNum);
  TILING_DATA_FIELD_DEF(uint64_t, tailNum);
  TILING_DATA_FIELD_DEF(uint64_t, formerLength);
  TILING_DATA_FIELD_DEF(uint64_t, tailLength);
  TILING_DATA_FIELD_DEF(uint64_t, alignNum);
END_TILING_DATA_DEF;
```

```
// The Tiling function on the host side computes the Tiling structure information
constexpr uint32_t NUM_BLOCKS = 8;
constexpr uint32_t SIZE_OF_HALF = 2;
constexpr uint32_t BLOCK_SIZE = 32;
constexpr uint32_t ALIGN_NUM = BLOCK_SIZE / SIZE_OF_HALF;
static ge::graphStatus TilingFunc(gert::TilingContext *context)
{
    TilingDataUnalign tiling;
    uint32_t totalLength = context->GetInputTensor(0)->GetShapeSize();
    // The NumBlocks information has been set through the SetSimdNumBlocks API
    context->SetSimdNumBlocks(NUM_BLOCKS);
    uint32_t totalLengthAligned = ((totalLength + ALIGN_NUM - 1) / ALIGN_NUM) * ALIGN_NUM;
    // Ensure that formerNum and tailNum do not exceed the 0-NUM_BLOCKS data range
    uint32_t formerNum = (totalLengthAligned / ALIGN_NUM) % NUM_BLOCKS;
    uint32_t tailNum = NUM_BLOCKS - formerNum;
    // Variables such as formerLength will not exceed the uint32_t range according to their computation logic   
    uint32_t formerLength = ((totalLengthAligned / NUM_BLOCKS + ALIGN_NUM - 1) / ALIGN_NUM) * ALIGN_NUM;
    uint32_t tailLength = (totalLengthAligned / NUM_BLOCKS / ALIGN_NUM) * ALIGN_NUM;
    ...
}
```

[Positive example]

The Tiling variables have no redundancy, and the data types of the variables are minimized.

```
BEGIN_TILING_DATA_DEF(TilingDataUnalign)
  TILING_DATA_FIELD_DEF(uint8_t, formerNum);
  TILING_DATA_FIELD_DEF(uint8_t, tailNum); 
  TILING_DATA_FIELD_DEF(uint32_t, formerLength);
  TILING_DATA_FIELD_DEF(uint32_t, tailLength);
  TILING_DATA_FIELD_DEF(uint32_t, alignNum);
END_TILING_DATA_DEF;
```

[Negative example]

In the following example, the TilingData structure is unreasonable: because the AI processor requires 8-byte alignment for memory access, after the user defines the TilingData structure, the Ascend C project framework pads the bytes in an 8-byte aligned manner and ensures that the entire TilingData structure meets the 8-byte alignment requirement. In the following TilingData structure, both the formerNum and tailNum variables are padded with 3 bytes, and the entire TilingData structure is padded with another 4 bytes due to 8-byte alignment, so the TilingData structure is padded with a total of **10** bytes.

```
BEGIN_TILING_DATA_DEF(TilingDataUnalign)
  TILING_DATA_FIELD_DEF(uint8_t, formerNum); // 3 bytes need to be padded so that the formerLength variable can be accessed correctly.
  TILING_DATA_FIELD_DEF(uint32_t, formerLength);
  TILING_DATA_FIELD_DEF(uint8_t, tailNum); // 3 bytes need to be padded so that the tailLength variable can be accessed correctly.
  TILING_DATA_FIELD_DEF(uint32_t, tailLength);
  TILING_DATA_FIELD_DEF(uint32_t, alignNum);// 4 bytes need to be padded so that the next TilingData structure can be accessed correctly.
END_TILING_DATA_DEF;
```

[Positive example]

In the following example, the layout of the Tiling parameters is adjusted so that the byte layout is reasonable and only **2** bytes need to be padded, thereby reducing the TilingData structure size.

```
BEGIN_TILING_DATA_DEF(TilingDataUnalign)
  TILING_DATA_FIELD_DEF(uint8_t, formerNum);
  TILING_DATA_FIELD_DEF(uint8_t, tailNum); // 2 bytes need to be padded so that the formerLength variable can be accessed correctly.
  TILING_DATA_FIELD_DEF(uint32_t, formerLength);
  TILING_DATA_FIELD_DEF(uint32_t, tailLength);
  TILING_DATA_FIELD_DEF(uint32_t, alignNum);
END_TILING_DATA_DEF;
```

