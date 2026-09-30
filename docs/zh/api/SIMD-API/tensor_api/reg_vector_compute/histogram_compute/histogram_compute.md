# 直方图计算

本节介绍寄存器直方图统计接口。

## 接口列表

| 接口 | 功能 |
| --- | --- |
| [histograms](histograms.md) | • 频率统计：<br>$dst_i = dst_i + \sum_{j:\,src.mask_j=1}\mathbf{1}(src_j=h+i)$<br>• 累计统计：<br>$dst_i = dst_i + \sum_{j:\,src.mask_j=1}\mathbf{1}(src_j\le h+i)$<br>其中，低位模式下$h=0$，高位模式下$h=128$。 |

输入数据的有效范围由`src.mask`决定，返回对象继承`dst.mask`。
