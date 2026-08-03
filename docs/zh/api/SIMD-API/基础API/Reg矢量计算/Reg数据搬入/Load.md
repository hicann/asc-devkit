# Load<a name="ZH-CN_TOPIC_0000002581346834"></a>

## 产品支持情况<a name="section129404119114"></a>

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

## 功能说明<a name="section12921612161519"></a>

头文件路径为：`"basic_api/reg_compute/kernel_reg_compute_datacopy_intf.h"`。

Reg矢量计算数据搬运接口，支持从UB非32字节对齐的源地址srcAddr搬运至RegTensor，搬运量为VL（256B）。连续搬运时，用户需手动更新srcAddr地址。

该接口封装了[LoadUnAlignPre和LoadUnAlign](LoadUnAlign_continuous.md)。

## 函数原型<a name="section429122319217"></a>

```cpp
template <typename T = DefaultType, typename U>
__simd_callee__ inline void Load(U& dstReg, __ubuf__ T* srcAddr)
```

## 参数说明<a name="section99612571362"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |
| U | 目的操作数的RegTensor类型，例如RegTensor\<half>，由编译器自动推导，用户不需要手动填写。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstReg | 输出 | 目的操作数，类型为[RegTensor](../寄存器数据类型/RegTensor.md)。 |
| srcAddr | 输入 | 源操作数，UB起始地址，不需要32字节对齐。 |

## 数据类型

目的操作数与源操作数的数据类型需要保持一致。支持的数据类型为：b8、b16、b32、b64。

## 返回值说明

无

## 约束说明

- dstReg不支持RegTraitNumTwo。
- 接口内部定义了一个[UnalignRegForLoad](../寄存器数据类型/UnalignRegForLoad-UnalignRegForStore.md)，该寄存器数量上限为4。

## 调用示例

```cpp
template<typename T>
__simd_vf__ inline void LoadStoreVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t count, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::Load(srcReg, srcAddr + i * count);
        AscendC::Reg::Store(dstAddr + i * count, srcReg);
    }
}
```
