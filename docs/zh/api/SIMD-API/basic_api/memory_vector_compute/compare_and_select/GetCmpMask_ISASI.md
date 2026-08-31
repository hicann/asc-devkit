# GetCmpMask\(ISASI\)<a name="ZH-CN_TOPIC_0000001836766209"></a>

## 产品支持情况<a name="section1550532418810"></a>

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
- Atlas 推理系列产品 AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品 Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/compare_and_select/GetCmpMask_ISASI_res.md#id1 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_vec_cmpsel_intf.h"`。

此接口用于获取[Compare（结果存入寄存器）](Compare_store_to_register.md)指令的比较结果。

[Compare（结果存入寄存器）](Compare_store_to_register.md)指令会将比较后的结果写入CmpMask寄存器中，使用GetCmpMask接口可以获取到CmpMask寄存器的值从而得到Compare的结果。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template<typename T>
__aicore__ inline void GetCmpMask(const LocalTensor<T>& dst)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数的数据类型。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | [Compare（结果存入寄存器）](Compare_store_to_register.md)指令的比较结果。<br>类型为[LocalTensor](../../data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要16字节对齐。 |

## 数据类型

支持数据类型为：b8、b16、b32。

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

dst的空间大小不能少于128字节。

## 调用示例<a name="section837496171220"></a>

[Compare（结果存入寄存器）](Compare_store_to_register.md)指令的结果使用uint8\_t类型数据存储，因此dstLocal使用uint8\_t类型。完整的示例参考[Compare类样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/compare)。

```cpp
AscendC::LocalTensor<float> src0Local;
AscendC::LocalTensor<float> src1Local;
AscendC::LocalTensor<uint8_t> dstLocal;
uint64_t mask = 256 / sizeof(float); // 256为每个迭代处理的字节数，结果为64
AscendC::BinaryRepeatParams repeatParams = { 1, 1, 1, 8, 8, 8 };
AscendC::Compare(src0Local, src1Local, AscendC::CMPMODE::LT, mask, repeatParams);
AscendC::GetCmpMask(dstLocal); // mask为0x40，比较数据类型为float，则每次迭代的32B里只有第7个float数字参与compare
```

```plain
输出示例：
src0Local:   [1, 2, 3, 4, 5, 6, 7, 8, 9, ...256]
src1Local:   [2, 3, 4, 5, 6, 7, 8, 9, ...257]
mask后参与比较的数
src0Local:   [1, 8, 16, ...256]
src1Local:   [2, 10, 18, ...257]
GetCmpMask结果：[256, 256, 256, 256, 256, 256, 256, 256]
```
