# RegLayout<a name="ZH-CN_TOPIC_0000001956862301"></a>

<!-- md-trans-meta sourceCommit=b6142b1cb0ae3dfa4b9063f8bd59db99b19fc426 translatedAt=2026-11-05T02:17:03.405Z pushedAt=2026-11-18T01:25:30.790Z nl=2 -->

When the source operand and destination operand have different bit widths, the computation amount of a single instruction is determined by the data type with the larger bit width. RegLayout is used to control the layout of elements with the smaller bit width in registers. For more feature descriptions, see [Cast](../type_conversion/Cast.md).

```cpp
enum class RegLayout { 
    UNKNOWN = -1, 
    ZERO,         // Large-to-small (smaller dst bit width): the result is placed at the 0th smaller-bit-width position in the dst group; small-to-large (smaller src bit width): the 0th smaller-bit-width element in the src group is selected.
    ONE,          // Large-to-small (smaller dst bit width): the result is placed at the 1st smaller-bit-width position in the dst group; small-to-large (smaller src bit width): the 1st smaller-bit-width element in the src group is selected.
    TWO,          // Large-to-small (smaller dst bit width): the result is placed at the 2nd smaller-bit-width position in the dst group; small-to-large (smaller src bit width): the 2nd smaller-bit-width element in the src group is selected.
    THREE         // Large-to-small (smaller dst bit width): the result is placed at the 3rd smaller-bit-width position in the dst group; small-to-large (smaller src bit width): the 3rd smaller-bit-width element in the src group is selected.
};
```

When the source operand and destination operand have different type bit widths, there are four cases. You can select the corresponding RegLayout based on the actual scenario. The validity of the mask is determined by the input data bit width.

- The bit width ratio of the source operand to the destination operand is 1:2.

  For example, when the ExpSub API input data type is half (b16) and the output data type is float (b32), every 2 bits of mask are valid. RegLayout determines the positions of the data that participate in computation for every 2 half-type data in the source operand.

  <a id="fig1"></a>

  ![](../../../../figures/src_dst_bitwidth_1to2.png "Source operand and destination operand bit width ratio is 1:2")

- The source operand and destination operand bit width ratio is 2:1.

  For example, the Cast API input data type is float(b32), the output data type is half(b16), and every 4 bits of the mask are valid. RegLayout determines the position of the valid data in every 2 half-type data in the destination operand.

  <a id="fig2"></a>

  ![](../../../../figures/src_dst_bitwidth_2to1.png "Source operand and destination operand bit width ratio is 2:1")

- The bit width ratio of the source operand to the destination operand is 1:4.

  For example, the Cast API input data type is int8_t(b8), the output data type is int32_t(b32), and each bit of the mask is valid. RegLayout determines the positions of the data that participate in the computation among every 4 int8_t data in the source operand.

  <a id="fig3"></a>

  ![](../../../../figures/src_dst_bitwidth_1to4.png "Source operand and destination operand bit width ratio is 1:4")

- The source operand and destination operand bit width ratio is 4:1.

  For example, when the Cast API input data type is int32_t (b32) and the output data type is uint8_t (b8), every 4 bits of the mask are valid, and RegLayout determines the position of the valid data in every four uint8_t data in the destination operand.

  <a id="fig4"></a>

  ![](../../../../figures/src_dst_bitwidth_4to1.png "Source operand and destination operand bit width ratio is 4:1")

The pseudocode for the data layout of Cast under different RegLayout values is as follows, where the mapping between the layout parameter and the enumeration values is: 0 corresponds to RegLayout::ZERO, 1 corresponds to RegLayout::ONE, 2 corresponds to RegLayout::TWO, and 3 corresponds to RegLayout::THREE:

```python
import numpy as np

# MaskReg behavior: MaskReg is 256 bits long. When the lowest bit of each 4-bit group is 1, the b32 element at the corresponding position is selected for conversion.
def cast_b32_to_b16(src, dst, layout):
    dst[:] = 0
    dst[layout:128:2] = src[0:64].astype(np.int16)

# MaskReg behavior: MaskReg is 256 bits long. When the lowest bit of every 2 bits is 1, the b16 element at the corresponding position is selected for conversion.
def cast_b16_to_b32(src, dst, layout):
    dst[0:64] = src[layout:128:2].astype(np.int32)

# MaskReg behavior: MaskReg is 256 bits long. When the lowest bit of every 4 bits is 1, the b32 element at the corresponding position is selected for conversion.
def cast_b32_to_b8(src, dst, layout):
    dst[:] = 0
    dst[layout:256:4] = src[0:64].astype(np.uint8)

# MaskReg behavior: MaskReg is 256 bits long. When every 1 bit is 1, the b8 element at the corresponding position is selected for conversion.
def cast_b8_to_b32(src, dst, layout):
    dst[0:64] = src[layout:256:4].astype(np.int32)

# MaskReg behavior: MaskReg is 256 bits long. When the lowest bit of every 4 bits is 1, two consecutive b16 elements at the corresponding position are selected for conversion.
def cast_b16_to_b4(src, dst, layout):
    dst[:] = 0
    dst[2*layout    : 512 : 8] = src[0:128:2].astype(np.uint8)
    dst[2*layout + 1: 512 : 8] = src[1:128:2].astype(np.uint8)

# MaskReg behavior: MaskReg is 256 bits long. When every 1 bit is 1, two consecutive b4 elements at the corresponding position are selected for conversion.
def cast_b4_to_b16(src, dst, layout):
    dst[0:128:2] = src[2*layout    : 512 : 8].astype(np.int16)
    dst[1:128:2] = src[2*layout + 1: 512 : 8].astype(np.int16)
```
