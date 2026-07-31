# CheckLocalMemoryIA\(ISASI\)<a name="ZH-CN_TOPIC_0000001834740621"></a>

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：不支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_common_intf.h"`。
check设定范围内的UB读写行为，如果有设定范围的读写行为则会出现EXCEPTION报错，无设定范围的读写行为则不会报错。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void CheckLocalMemoryIA(const CheckLocalMemoryIAParam& checkParams)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  参数说明

| 参数名称 | 输入/输出 | 描述 |
| ------ | ------ | ------ |
| checkParams | 输入 | 用于配置对UB访问的检查行为，类型为CheckLocalMemoryIAParam。<br>具体定义请参考`${INSTALL_DIR}`/include/ascendc/basic_api/interface/kernel_struct_mm.h，`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。<br>参数说明请参考[表2](#table15780447181917)。 |

**表2**  CheckLocalMemoryIAParam结构体内参数说明<a name="table15780447181917"></a>

| 参数名称 | 含义 |
| ------ | ------ |
| enableBit | 配置的异常寄存器，取值范围：enableBit∈[0,3]，默认为0。<br>• 0：异常寄存器0。<br>• 1：异常寄存器1。<br>• 2：异常寄存器2。<br>• 3：异常寄存器3。 |
| startAddr | Check的起始地址，32B对齐，取值范围：startAddr∈[0, 65535]，默认值为0。比如，可通过LocalTensor.GetPhyAddr()/32来获取startAddr。 |
| endAddr | Check的结束地址，32B对齐，取值范围：endAddr∈[0, 65535]。默认值为0。 |
| isScalarRead | Check标量读访问。<br>•false：不开启，默认为false。<br>•true：开启。 |
| isScalarWrite | Check标量写访问。<br>•false：不开启，默认为false。<br>•true：开启。 |
| isVectorRead | Check矢量读访问。<br>•false：不开启，默认为false。<br>•true：开启。 |
| isVectorWrite | Check矢量写访问。<br>•false：不开启，默认为false。<br>•true：开启。 |
| isMteRead | Check MTE读访问。<br>•false：不开启，默认为false。<br>•true：开启。 |
| isMteWrite | Check MTE写访问。<br>•false：不开启，默认为false。<br>•true：开启。 |
| isEnable | 是否使能enableBit参数配置的异常寄存器。<br>•false：不使能，默认为false。<br>•true：使能。 |
| reserved | 预留参数。为后续的功能做保留，开发者暂时无需关注，使用默认值即可。 |

## 约束说明<a name="section633mcpsimp"></a>

- startAddr/endAddr的单位是32B，check的范围不包含startAddr，包含endAddr，即(startAddr,endAddr]。
- 每次调用完该接口需要进行复位（配置isEnable为false进行复位）。
- 操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md#section796754519912)。

## 调用示例<a name="section642mcpsimp"></a>

该示例check矢量写访问是否在设定的(startAddr, endAddr]范围内。当前示例check到矢量写在设定的范围内，结果会报错（ACL_ERROR_RT_VECTOR_CORE_EXCEPTION）。

```cpp
AscendC::TPipe pipe;
AscendC::TQue<AscendC::TPosition::VECIN, 1> inQueueSrc0, inQueueSrc1;
AscendC::TQue<AscendC::TPosition::VECOUT, 1> outQueueDst;
pipe.InitBuffer(inQueueSrc0, 1, 512 * sizeof(half));
pipe.InitBuffer(inQueueSrc1, 1, 512 * sizeof(half));
pipe.InitBuffer(outQueueDst, 1, 512 * sizeof(half));
AscendC::LocalTensor<half> src0Local = inQueueSrc0.DeQue<half>();
AscendC::LocalTensor<half> src1Local = inQueueSrc1.DeQue<half>();
AscendC::LocalTensor<half> dstLocal = outQueueDst.AllocTensor<half>();
AscendC::CheckLocalMemoryIA({ 0, (uint32_t)(dstLocal.GetPhyAddr() / 32),(uint32_t)((dstLocal.GetPhyAddr() + 512 * sizeof(half)) / 32), false, false, false, true, false, false,
true });
```
