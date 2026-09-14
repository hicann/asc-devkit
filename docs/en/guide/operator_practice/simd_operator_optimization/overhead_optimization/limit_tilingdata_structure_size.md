# Limiting the TilingData Structure Size<a name="ZH-CN_TOPIC_0000001846878548"></a>

<!-- md-trans-meta sourceCommit=51eb188d02e4be270d9dc56141153e3f75345dc0 translatedAt=2026-08-26T14:36:08.889Z -->

[Priority] Medium

[Description] The TilingData structure carries tiling segmentation information. After the Host side computes the tiling according to the tiling segmentation strategy, the operator passes the tiling segmentation information from the Host side to the Device side as an input parameter, and the tiling information is stored in the GM. After the GET_TILING_DATA macro is called, the tiling information is copied from the GM to the stack space of the AI processor, which incurs copy overhead. Because GM access is inefficient and the stack space is limited, the TilingData structure size must be limited. The copy takes a microsecond-level duration. In small-shape scenarios, such optimization yields more noticeable benefits.

To limit the TilingData structure size, consider the following aspects:

-   Reduce unnecessary TilingData structure variables.
-   Select appropriate variable types based on the data range of the tiling.
-   Arrange the TilingData structure properly.
-   The overall TilingData structure must be padded to 8-byte alignment.

[Negative Example]

-   The following example contains redundant TilingData structure variables: the NumBlocks information has already been set through the SetBlockDim API and can be obtained on the kernel side by calling the GetBlockNum API, so it does not need to be passed through the TilingData structure.
-   In addition, the data types of the variables are also unreasonable: formerNum and tailNum are the number of cores that compute the whole blocks and the number of cores that compute the tail block, respectively, and they never exceed the value of NUM\_BLOCKS, so the uint8\_t type is sufficient. Variables such as formerLength never exceed the range of uint32\_t according to their computation logic, so the uint32\_t type is sufficient.

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
// The Host-side Tiling function computes the Tiling structure information.
constexpr uint32_t NUM_BLOCKS = 8;
constexpr uint32_t SIZE_OF_HALF = 2;
constexpr uint32_t BLOCK_SIZE = 32;
constexpr uint32_t ALIGN_NUM = BLOCK_SIZE / SIZE_OF_HALF;
static ge::graphStatus TilingFunc(gert::TilingContext *context)
{
    TilingDataUnalign tiling;
    uint32_t totalLength = context->GetInputTensor(0)->GetShapeSize();
    // The NumBlocks information has been set through the SetBlockDim API.
    context->SetBlockDim(NUM_BLOCKS);
    uint32_t totalLengthAligned = ((totalLength + ALIGN_NUM - 1) / ALIGN_NUM) * ALIGN_NUM;
    // formerNum and tailNum are guaranteed not to exceed the data range of 0 to NUM_BLOCKS.
    uint32_t formerNum = (totalLengthAligned / ALIGN_NUM) % NUM_BLOCKS;
    uint32_t tailNum = NUM_BLOCKS - formerNum;
    // Variables such as formerLength never exceed the range of uint32_t according to their computation logic.
    uint32_t formerLength = ((totalLengthAligned / NUM_BLOCKS + ALIGN_NUM - 1) / ALIGN_NUM) * ALIGN_NUM;
    uint32_t tailLength = (totalLengthAligned / NUM_BLOCKS / ALIGN_NUM) * ALIGN_NUM;
    ...
}
```

[Positive Example]

The tiling variables have no redundancy, and the data types of the variables are minimized.

```
BEGIN_TILING_DATA_DEF(TilingDataUnalign)
  TILING_DATA_FIELD_DEF(uint8_t, formerNum);
  TILING_DATA_FIELD_DEF(uint8_t, tailNum); 
  TILING_DATA_FIELD_DEF(uint32_t, formerLength);
  TILING_DATA_FIELD_DEF(uint32_t, tailLength);
  TILING_DATA_FIELD_DEF(uint32_t, alignNum);
END_TILING_DATA_DEF;
```

[Negative Example]

The TilingData structure in the following example is unreasonable: because the AI processor requires 8-byte alignment for memory access, after the user defines the TilingData structure, the Ascend C engineering framework pads the bytes in an 8-byte aligned manner and ensures that the entire TilingData structure meets the 8-byte alignment requirement. In the following TilingData structure, both the formerNum and tailNum variables are padded with 3 bytes, and the entire TilingData structure is padded with another 4 bytes due to 8-byte alignment. As a result, a total of **10** bytes are padded in this TilingData structure.

```
BEGIN_TILING_DATA_DEF(TilingDataUnalign)
  TILING_DATA_FIELD_DEF(uint8_t, formerNum); // 3 bytes need to be padded so that the formerLength variable can be accessed correctly.
  TILING_DATA_FIELD_DEF(uint32_t, formerLength);
  TILING_DATA_FIELD_DEF(uint8_t, tailNum); // 3 bytes need to be padded so that the tailLength variable can be accessed correctly.
  TILING_DATA_FIELD_DEF(uint32_t, tailLength);
  TILING_DATA_FIELD_DEF(uint32_t, alignNum);// 4 bytes need to be padded so that the next TilingData structure can be accessed correctly.
END_TILING_DATA_DEF;
```

[Positive Example]

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

