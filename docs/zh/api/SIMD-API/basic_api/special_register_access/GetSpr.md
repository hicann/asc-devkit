# GetSpr<a name="ZH-CN_TOPIC_0000002537557502"></a>

## 产品支持情况<a name="section364964311912"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/special_register_access/GetSpr_res.md#id1 -->

## 功能说明<a name="section11971757181915"></a>

头文件路径为：`"basic_api/kernel_operator_sys_var_intf.h"`。

获取指定特殊寄存器的值。当前支持[表SpecialPurposeReg模板参数说明](#table37531617424)。

## 函数原型<a name="section113251712205"></a>

```cpp
template <SpecialPurposeReg spr>
__aicore__ inline int64_t GetSpr()
```

## 参数说明<a name="section1116018170208"></a>

**表1** 模板参数说明

| 参数名 | 描述 |
|--------|------|
| spr | 特殊寄存器，类型为SpecialPurposeReg枚举类，具体的取值请参考[表SpecialPurposeReg模板参数说明](#table37531617424)。 |

**表2** SpecialPurposeReg模板参数说明<a name="table37531617424"></a>

| 取值 | 含义 |
|------|------|
| AR | AR寄存器通常配合Reg矢量计算API [Squeeze](../reg_vector_compute/compare_and_select/Squeeze.md)和[连续非对齐搬出](../reg_vector_compute/reg_data_store/StoreUnAlign_continuous.md)的场景3（无需显式传入偏移量）接口一起使用，寄存器会存储有效元素的总字节数。 |

## 数据类型<a name="section3853852112218"></a>

接口返回数据类型为int64\_t。

## 返回值说明<a name="section16895132314202"></a>

返回int64\_t类型的特殊寄存器中的数值。

## 约束说明<a name="section162221734202016"></a>

本接口只能在VF函数外调用，命名空间为AscendC，函数标记符为\_\_aicore\_\_。

## 调用示例<a name="section849174212202"></a>

如下示例中[Squeeze](../reg_vector_compute/compare_and_select/Squeeze.md)，Reg矢量计算API会存储有效元素的总字节数到AR寄存器中，宏函数结束后通过GetSpr获取AR寄存器的值（单位为字节）。

```cpp
template <typename T>
__simd_vf__ inline void SqueezeVF(__ubuf__ T* xAddr, __ubuf__ T* yAddr, uint32_t repeatTimes, uint32_t oneRepeatSize)
{
    AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::M4>();
    AscendC::Reg::RegTensor<T> xReg;
    AscendC::Reg::RegTensor<T> yReg;
    AscendC::Reg::UnalignRegForStore ureg;
    AscendC::Reg::ClearSpr<AscendC::SpecialPurposeReg::AR>();
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        AscendC::Reg::LoadAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(xReg, xAddr, oneRepeatSize);
        AscendC::Reg::Squeeze<T, AscendC::Reg::GatherMaskMode::STORE_REG>(yReg, xReg, mask);
        AscendC::Reg::StoreUnAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(yAddr, yReg, ureg);
    }
    AscendC::Reg::StoreUnAlignPost(yAddr, ureg);
}

__aicore__ inline void Process()
{
    AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
    AscendC::LocalTensor<float> xLocal = ubAllocator.Alloc<float, 256>();
    AscendC::LocalTensor<float> yLocal = ubAllocator.Alloc<float, 64>();

    AscendC::DataCopy(xLocal, xGm, 256);
    AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

    __ubuf__ float* xAddr = reinterpret_cast<__ubuf__ float*>(xLocal.GetPhyAddr());
    __ubuf__ float* yAddr = reinterpret_cast<__ubuf__ float*>(yLocal.GetPhyAddr());
    constexpr uint32_t oneRepeatSize = AscendC::GetVecLen() / sizeof(float);
    uint16_t repeatTimes = DivCeil(256, oneRepeatSize);

    asc_vf_call<SqueezeVF<float>>(xAddr, yAddr, repeatTimes, oneRepeatSize);
    AscendC::SetFlag<AscendC::HardEvent::V_S>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::V_S>(EVENT_ID0);
    int64_t arNum = AscendC::GetSpr<AscendC::SpecialPurposeReg::AR>();
    // 可通过printf打印
    AscendC::printf("arNum的值为:%lld\n", arNum);

    AscendC::SetFlag<AscendC::HardEvent::S_MTE3>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::S_MTE3>(EVENT_ID0);
    AscendC::DataCopy(yGm, yLocal, 64);
}
```

结果示例如下：

```cpp
输入256个float的数据(xLocal): [1.0 1.0 1.0 ... ] // 数据为全1.0的数
输入64个float的数据(yLocal): [1.0 1.0 1.0 ... ] // 数据为全1.0的数
arNum的值为:256
```
