# RegLayout<a name="ZH-CN_TOPIC_0000001956862301"></a>

源操作数和目的操作数位宽不同时，单条指令计算量以位宽更大的数据类型为准，RegLayout用于控制位宽小的元素在寄存器中的排布方式。更多特性说明请参考[Cast](../type_conversion/Cast.md)。
```cpp
enum class RegLayout { 
    UNKNOWN = -1, 
    ZERO,         // 大转小（dst位宽小）：结果位于dst组内第0个位宽小位置；小转大（src位宽小）：选取src组内第0个位宽小元素
    ONE,          // 大转小（dst位宽小）：结果位于dst组内第1个位宽小位置；小转大（src位宽小）：选取src组内第1个位宽小元素
    TWO,          // 大转小（dst位宽小）：结果位于dst组内第2个位宽小位置；小转大（src位宽小）：选取src组内第2个位宽小元素
    THREE         // 大转小（dst位宽小）：结果位于dst组内第3个位宽小位置；小转大（src位宽小）：选取src组内第3个位宽小元素
};
```

源操作数与目的操作数类型位宽不同时，可以分为四种情况，用户可以根据实际场景选择对应的RegLayout，其中，mask的有效情况均以输入数据位宽为准。

- 源操作数与目的操作数位宽比为1:2。

  例如，ExpSub接口输入数据类型为half(b16)，输出数据类型为float(b32)时，mask每2位有效，RegLayout确定源操作数中，每2个half类型数据参与计算的数据的位置。

  <a id="fig1"></a>

  ![](../../../../figures/src_dst_bitwidth_1to2.png "源操作数与目的操作数位宽比为1:2")

- 源操作数与目的操作数位宽比为2:1。

  例如，Cast接口输入数据类型为float(b32)，输出数据类型为half(b16)，mask每4位有效，RegLayout确定目的操作数中，每2个half类型数据有效数据的位置。

  <a id="fig2"></a>

  ![](../../../../figures/src_dst_bitwidth_2to1.png "源操作数与目的操作数位宽比为2:1")

- 源操作数与目的操作数位宽比为1:4。

  例如，Cast接口输入数据类型为int8_t(b8)，输出数据类型为int32_t(b32)，mask每1位有效，RegLayout确定源操作数中，每4个int8_t类型数据参与计算的数据的位置。

  <a id="fig3"></a>

  ![](../../../../figures/src_dst_bitwidth_1to4.png "源操作数与目的操作数位宽比为1:4")

- 源操作数与目的操作数位宽比为4:1。

  例如，Cast接口输入数据类型为int32_t(b32)，输出数据类型为uint8_t(b8)，mask每4位有效，RegLayout确定目的操作数中，每4个uint8_t类型数据有效数据的位置。

  <a id="fig4"></a>

  ![](../../../../figures/src_dst_bitwidth_4to1.png "源操作数与目的操作数位宽比为4:1")

Cast在不同RegLayout取值下的数据排布伪代码如下，其中layout参数与枚举值的映射关系为：0对应RegLayout::ZERO，1对应RegLayout::ONE，2对应RegLayout::TWO，3对应RegLayout::THREE：
```python
import numpy as np

# MaskReg行为：MaskReg长度为256bit，每4bit最低位为1时选取对应位置的b32元素进行转换。
def cast_b32_to_b16(src, dst, layout):
    dst[:] = 0
    dst[layout:128:2] = src[0:64].astype(np.int16)

# MaskReg行为：MaskReg长度为256bit，每2bit最低位为1时选取对应位置的b16元素进行转换。
def cast_b16_to_b32(src, dst, layout):
    dst[0:64] = src[layout:128:2].astype(np.int32)

# MaskReg行为：MaskReg长度为256bit，每4bit最低位为1时选取对应位置的b32元素进行转换。
def cast_b32_to_b8(src, dst, layout):
    dst[:] = 0
    dst[layout:256:4] = src[0:64].astype(np.uint8)

# MaskReg行为：MaskReg长度为256bit，每1bit为1时选取对应位置的b8元素进行转换。
def cast_b8_to_b32(src, dst, layout):
    dst[0:64] = src[layout:256:4].astype(np.int32)

# MaskReg行为：MaskReg长度为256bit，每4bit最低位为1时选取对应位置连续的两个b16元素进行转换。
def cast_b16_to_b4(src, dst, layout):
    dst[:] = 0
    dst[2*layout    : 512 : 8] = src[0:128:2].astype(np.uint8)
    dst[2*layout + 1: 512 : 8] = src[1:128:2].astype(np.uint8)

# MaskReg行为：MaskReg长度为256bit，每1bit为1时选取对应位置连续的两个b4元素进行转换。
def cast_b4_to_b16(src, dst, layout):
    dst[0:128:2] = src[2*layout    : 512 : 8].astype(np.int16)
    dst[1:128:2] = src[2*layout + 1: 512 : 8].astype(np.int16)
```
