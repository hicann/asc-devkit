# Squeeze<a name="ZH-CN_TOPIC_0000002009320090"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id1  -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1  -->
<!-- npu="A3" id2  -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2  -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4  -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4  -->
<!-- npu="310p" id5  -->
- Atlas 推理系列产品 AI Core：不支持
<!-- end id5  -->
<!-- npu="310p" id6  -->
- Atlas 推理系列产品 Vector Core：不支持
<!-- end id6  -->
<!-- npu="910" id7  -->
- Atlas 训练系列产品：不支持
<!-- end id7  -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/reg_compute/kernel_reg_compute_gather_mask_intf.h"`。

将传入的srcReg中被mask选择的有效元素依次复制到dstReg中，有效元素在dstReg中从低到高连续排列，剩余位置元素置为0，如[图1](#figure1)所示。

**图1**  Squeeze计算示意图<a id="figure1"></a>

![Squeeze计算示意图](../../../../figures/reg_squeeze.png)

特别地，当模板参数store取值为`GatherMaskMode::STORE_REG`时，`Squeeze`会将有效元素的总字节数存入AR特殊寄存器。此时配合使用[连续非对齐搬出](../Reg数据搬出/连续非对齐搬出（StoreUnAlign）.md)的场景3接口（无需显式传入偏移量），`StoreUnAlign`会自动从AR寄存器读取有效字节数作为地址偏移，参考[调用示例](#调用示例)。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, GatherMaskMode store = GatherMaskMode::NO_STORE_REG, typename U>
__simd_callee__ inline void Squeeze(U& dstReg, U& srcReg, MaskReg& mask)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| :----- | :--- |
| T | 目的操作数和源操作数的数据类型。 |
| store | GatherMaskMode选择是否将有效元素的总字节数存入AR寄存器，AR寄存器描述参考[表 模板参数说明](../../特殊寄存器访问/GetSpr.md#table37531617424)。<br>• NO_STORE_REG，有效元素的总字节数不存入AR寄存器；<br>• STORE_REG，有效元素的总字节数存入AR寄存器。 |
| U | 目的操作数和源操作数的`RegTensor`类型，例如`RegTensor<uint32_t>`，由编译器自动推导，用户不需要填写。 |

**表2**  函数参数说明

| 参数名 | 输入/输出 | 描述 |
| :----- | :-------- | :--- |
| dstReg | 输出 | 目的操作数。<br>类型为[RegTensor](../寄存器数据类型/RegTensor.md)。 |
| srcReg | 输入 | 源操作数。<br>类型为[RegTensor](../寄存器数据类型/RegTensor.md)。 |
| mask | 输入 | mask用于控制每次迭代内参与计算的元素。 |

## 返回值说明

无

## 数据类型<a name="zh-cn_topic_0000002568284054_section1962315623118"></a>

支持的数据类型为：int8\_t、uint8\_t、int16\_t、uint16\_t、half、int32\_t、uint32\_t、float。

## 约束说明<a name="section177921451558"></a>

- 当模板参数store取值为`GatherMaskMode::STORE_REG`时，由于硬件约束，`StoreUnAlign`指令和`Squeeze`指令必须交替使用，例如：

  ```cpp
  Squeeze(dstVreg, srcVreg, mask);
  StoreUnAlign(dstAddr, dstVreg, ureg);
  Squeeze(dstVreg, srcVreg, mask);
  StoreUnAlign(dstAddr, dstVreg, ureg);
  ```

- 当store取值为`GatherMaskMode::NO_STORE_REG`时，不涉及AR寄存器，`Squeeze`和`StoreUnAlign`不强制交替。

## 调用示例<a name="section642mcpsimp"></a>

以下示例展示了`Squeeze`与`StoreUnAlign`的典型配合方式（`Squeeze`的STORE_REG模式，`StoreUnAlign`使用AR寄存器中的有效元素字节数作为偏移量）：

```cpp
template<typename T>
__simd_vf__ inline void SqueezeVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg0;
    AscendC::Reg::RegTensor<T> srcReg1;
    AscendC::Reg::UnalignRegForStore ureg;
    AscendC::Reg::MaskReg sqzMask = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::H>();
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        AscendC::Reg::LoadAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(srcReg0, srcAddr, oneRepeatSize);
        AscendC::Reg::Squeeze<T, AscendC::Reg::GatherMaskMode::STORE_REG>(srcReg1, srcReg0, sqzMask);
        AscendC::Reg::StoreUnAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(dstAddr, srcReg1, ureg);
     }
     AscendC::Reg::StoreUnAlignPost(dstAddr, ureg);
}
```
