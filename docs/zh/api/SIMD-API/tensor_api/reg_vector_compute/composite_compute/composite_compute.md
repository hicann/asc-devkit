# 复合计算

本节介绍将多个计算步骤融合为一次调用的寄存器计算接口。

## 接口列表

| 接口 | 描述 |
| --- | --- |
| [axpy](axpy.md) | $dst_i = scalar \times src_i + dst_i$ |
| [abs_diff](abs_diff.md) | $dst_i = \lvert src0_i - src1_i \rvert$ |
| [exp_diff](exp_diff.md) | • 输入数据类型为`float`时：<br>$dst_i = e^{src0_i - src1_i}$<br>• 输入数据类型为`half`时：<br>$dst_i = e^{cast\_f16\_to\_f32(src0_i) - cast\_f16\_to\_f32(src1_i)}$ |
| [fma](fma.md) | $dst_i = src0_i \times src1_i + src2_i$ |
| [madd](madd.md) | $dst_i = dst_i \times src0_i + src1_i$ |
| [mula](mula.md) | $dst_i = src0_i \times src1_i + dst_i$ |
| [muls_cast](muls_cast.md) | $dst_i = cast\_round\_to\_f16(src_i \times scalar)$ |
