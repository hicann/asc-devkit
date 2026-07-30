# asc_set_gm2ub_pad（废弃）

## 产品支持情况 

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
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

**此接口已废弃，源数据从GM非对齐搬运到UB设置填补数据值请使用[asc_set_copy_pad_val](./asc_set_copy_pad_val.md)进行设置。**

将数据从Global Memory (GM) 非对齐搬运到 Unified Buffer (UB)时，通过调用该接口设置连续搬运数据块左右两侧需要填补的数据值。 

## 函数原型 

```cpp 
__aicore__ inline void asc_set_gm2ub_pad(uint32_t pad_val)
``` 

## 参数说明 

|参数名|输入/输出|描述| 
|------------|------------|-----------| 
| pad_val    | 输入       | 左右两侧需要填补的数据值。| 

## 返回值说明 

无 

## 流水类型 

PIPE_S 

## 约束说明 

无 

## 调用示例 

```cpp 
asc_set_gm2ub_loop_size(2, 2); 
asc_set_gm2ub_loop1_stride(96, 128); 
asc_set_gm2ub_loop2_stride(192, 288); 
// 源操作数非对齐，需要填补数据 
asc_set_gm2ub_pad(0); 
asc_copy_gm2ub_align(dst, src, 2, 46 * sizeof(int8_t), 1, 1, true, 0, 48, 48); 
asc_set_gm2ub_loop_size(1, 1); 
```
