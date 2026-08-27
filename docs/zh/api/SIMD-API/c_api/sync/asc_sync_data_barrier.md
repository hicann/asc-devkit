# asc_sync_data_barrier

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

阻塞后续的指令执行，直到所有之前的内存访问指令（需要等待的内存位置可通过参数控制）执行结束，用于解决Scalar的内存依赖。

## 函数原型

```cpp
__aicore__ inline void asc_sync_data_barrier(mem_dsb_t arg)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :---  | :--- | :--- |
| arg | 输入 | 表示需要等待的内存位置，类型为mem_dsb_t，可取值为： <br>&bull; DSB_ALL：等待所有内存访问指令。 <br>&bull; DSB_DDR：等待GM访问指令。<br>&bull; DSB_UB：等待UB访问指令。<br>&bull; DSB_SEQ：预留参数，暂未启动，为后续的功能扩展做保留。|

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- mem_dsb_t类型的入参必须直接传递枚举值字面量。
- 等待的内存位置取值为DSB_ALL时，只能等待GM和Unified Buffer（UB）的访问指令，不包括其他内存位置，例如L0C Buffer、L1 Buffer。

## 调用示例

由于[asc_store_dev](../scalar_compute/scalar_store/asc_store_dev.md)接口向GM写数据时不经过DCache，因此开发者需要考虑如下场景：当GM上地址addr已经在DCache缓存并且其对应的Cache Line标记为"脏"（dirty，表示该数据已被修改但尚未写回到GM）时，开发者应该在asc_store_dev接口之前调用[asc_dcci](../cache_ctrl/asc_dcci.md)接口将addr对应的Cache Line立即写回GM，否则asc_store_dev接口写入addr的数据后续可能被DCache写回的脏数据覆盖。

asc_dcci接口与asc_store_dev接口向GM写数据时硬件不能保证两个接口的执行顺序，因此开发者应该在asc_store_dev接口之前调用asc_sync_data_barrier接口对这两个接口进行同步，否则asc_store_dev接口写入addr的数据依然可能被DCache写回的脏数据覆盖。

根据以上的描述，为了简化编程（开发者无需关心addr是否在DCache缓存以及是否被标记为"脏"），建议开发者在使用asc_store_dev接口时采用如下代码片段：

```cpp
asc_sync_data_barrier(mem_dsb_t::DSB_ALL);
```
