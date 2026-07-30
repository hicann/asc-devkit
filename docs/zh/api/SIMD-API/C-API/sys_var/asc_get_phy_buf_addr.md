# asc_get_phy_buf_addr

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

基于偏移量offset获取片上实际物理地址。

## 函数原型

```cpp
__aicore__ inline uint64_t asc_get_phy_buf_addr(uint64_t offset)
```

## 参数说明

|参数名|输入/输出|描述|
| :------ | :--- | :------------ |
|offset   |输入   |片上内存的地址偏移量，范围是0~物理内存大小。|

## 返回值说明

物理内存的地址。

## 流水类型

PIPE_S

## 约束说明

不能和[以数组方式申请内存的方法](../通用说明和约束.md#以数组方式申请内存)混用。

## 调用示例

```cpp
// 假设src0，src1，dst三个操作数均包含64个half类型的数据。
uint64_t offset = 0;                                   // 首先为src0申请内存，从0开始。
__ubuf__ half* src0 = (__ubuf__ half*)asc_get_phy_buf_addr(offset);    // 获取src0的地址，通过__ubuf__关键字指定该地址指向UB内存。
offset += 64 * sizeof(half);                           // 通过offset将src1的起始地址设置在src0之后。
__ubuf__ half* src1 = (__ubuf__ half*)asc_get_phy_buf_addr(offset);    // 获取src1的地址，通过__ubuf__关键字指定该地址指向UB内存。
offset += 64 * sizeof(half);                           // 通过offset将dst的起始地址设置在src1之后。
__ubuf__ half* dst = (__ubuf__ half*)asc_get_phy_buf_addr(offset);     // 获取dst的地址，通过__ubuf__关键字指定该地址指向UB内存。
...... // 使用src0、src1、dst中的数据进行后续计算或数据搬运操作。
```
