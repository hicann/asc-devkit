# 连续计算

**连续计算API**，支持Tensor前n个数据计算。针对源操作数的连续n个数据进行计算并连续写入目的操作数，解决一维Tensor的连续计算问题。

以Add接口为例，接口调用形式如下：

```cpp
Add(dst, src1, src2, n);
```

下图以矢量加法为例，展示了连续计算API的特点：

- 源操作数src1、src2和目的操作数dst地址连续。
- 接口简洁，仅需指定计算元素个数n。

**图1** **连续计算API**  
![连续计算API](../../../../figures/continuous_calc_api.png)
