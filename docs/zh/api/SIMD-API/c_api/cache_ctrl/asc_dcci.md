# asc_dcci

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

在AI Core内部，Scalar单元和DMA单元都可能对Global Memory进行访问。

![](../figures/dcci.png "DataCache内存层次示意图") \\

如上图所示：

DMA搬运单元读写Global Memory，数据通过[asc_copy_ub2gm](../vector_datamove/asc_copy_ub2gm/asc_copy_ub2gm.md)和[asc_copy_gm2ub](../vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub.md)等接口在Unified Buffer（UB）等Local Memory和Global Memory间交互，没有Cache一致性问题；
Scalar单元访问Global Memory，首先会访问每个核内的Data Cache，因此存在Data Cache与Global Memory的Cache一致性问题。
该接口用来刷新Cache，保证Cache的一致性，使用场景如下：

读取Global Memory的数据，但该数据可能在外部被其余核修改，此时需要使用asc_dcci接口，直接访问Global Memory，获取最新数据；
用户通过Scalar单元写Global Memory的数据，希望立刻写出，也需要使用asc_dcci接口。

Scalar单元访问UB数据时，该接口需配合[asc_set_ctrl()](../spr/asc_set_ctrl.md)接口使用，将CTRL[49]设置为1'b1，开启datacache模式。

<!-- npu="950" id8 -->
针对Ascend 950PR/Ascend 950DT，不支持asc_dcci_entire_ub接口。
<!-- end id8 -->

## 函数原型

   ```cpp
   // 刷新传入地址所在的单个Cache Line
   __aicore__ inline void asc_dcci_single(__gm__ void* dst)

   // 刷新传入的UB地址空间地址所在的单个Cache Line
   __aicore__ inline void asc_ub_dcci_single(__ubuf__ void* dst)

   // 刷新所有Cache Line
   __aicore__ inline void asc_dcci_entire_all()
   __aicore__ inline void asc_dcci_entire_ub()
   __aicore__ inline void asc_dcci_entire_out()
   __aicore__ inline void asc_dcci_entire_atomic()
   ```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| dst | 输入 | 需要刷新缓存的向量的起始地址。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

无

## 调用示例

```cpp
// 示例：SINGLE_CACHE_LINE模式（x_gm 64B对齐场景）
// 假设x为外部传入的地址，每个核计算16个数据，block_idx为内置变量，通过block_idx计算偏移实现多核间数据隔离
__gm__ float* x_gm = x + block_idx * 16;
for (int i = 0; i < 16; i++) {
    x_gm[i] = static_cast<float>(i + block_idx);
}
// 由于首地址64B对齐，调用asc_dcci_single指令后，数据会立刻刷新到Global Memory
asc_dcci_single(reinterpret_cast<__gm__ uint64_t*>(x_gm));
```
