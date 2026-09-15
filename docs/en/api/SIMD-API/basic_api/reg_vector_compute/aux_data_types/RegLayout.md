# RegLayout<a name="ZH-CN_TOPIC_0000001956862301"></a>

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-08-31T12:49:57.852Z -->

When the source operand and destination operand have different bit widths, the computation amount of a single instruction is determined by the data type with the larger bit width. RegLayout is used to control the layout of elements with the smaller bit width in the register. For more feature descriptions, see [Cast](../Type Conversion/Cast-45.md).
```cpp
enum class RegLayout { 
    UNKNOWN = -1, 
    ZERO,         // Large-to-small conversion (smaller dst bit width): the result is placed at the 0th smaller-bit-width position within the dst group; small-to-large conversion (smaller src bit width): the 0th smaller-bit-width element within the src group is selected.
    ONE,          // Large-to-small conversion (smaller dst bit width): the result is placed at the 1st smaller-bit-width position within the dst group; small-to-large conversion (smaller src bit width): the 1st smaller-bit-width element within the src group is selected.
    TWO,          // Large-to-small conversion (smaller dst bit width): the result is placed at the 2nd smaller-bit-width position within the dst group; small-to-large conversion (smaller src bit width): the 2nd smaller-bit-width element within the src group is selected.
    THREE         // Large-to-small conversion (smaller dst bit width): the result is placed at the 3rd smaller-bit-width position within the dst group; small-to-large conversion (smaller src bit width): the 3rd smaller-bit-width element within the src group is selected.
};
```

When the source operand and destination operand have different type bit widths, there are four cases. You can select the corresponding RegLayout based on the actual scenario. In all cases, the validity of the mask is determined by the input data bit width.

- The source operand and destination operand have a bit width ratio of 1:2.

  For example, when the ExpSub API input data type is half (b16) and the output data type is float (b32), every 2 bits of the mask are valid. RegLayout determines the position of the data that participates in computation among every 2 half-type data in the source operand.

  <a id="fig1"></a>

  ![](../../../../figures/src_dst_ratio_1_to_2.png "Source operand to destination operand bit width ratio of 1:2")

- The source operand to destination operand bit width ratio is 2:1.

  For example, when the Cast API input data type is float(b32) and the output data type is half(b16), every 4 bits of the mask are valid, and RegLayout determines the position of the valid data of every 2 half-type data in the destination operand.

  <a id="fig2"></a>

  ![](../../../../figures/src_dst_ratio_2_to_1.png "Source operand to destination operand bit width ratio of 2:1")

- The source operand to destination operand bit width ratio is 1:4.

  For example, when the Cast API input data type is int8_t(b8) and the output data type is int32_t(b32), every 1 bit of the mask is valid, and RegLayout determines the position of the data of every 4 int8_t-type data that participate in the computation in the source operand.

  <a id="fig3"></a>

  ![](../../../../figures/src_dst_ratio_1_to_4.png "Source operand to destination operand bit width ratio of 1:4")

- The source operand to destination operand bit width ratio is 4:1.

  For example, when the **Cast** API input data type is **int32_t** (b32) and the output data type is **uint8_t** (b8), every 4 bits of **mask** are valid. **RegLayout** determines the position of the valid data of every four **uint8_t** type data in the destination operand.

  <a id="fig4"></a>

  ![](../../../../figures/src_dst_ratio_4_to_1.png "Source operand to destination operand bit width ratio of 4:1")
