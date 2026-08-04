# GetSsbufBaseAddr

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

头文件路径为：`"basic_api/kernel_operator_sys_var_intf.h"`。

该接口用于获取SSBuffer的基地址。

## 函数原型

```cpp
__aicore__ inline __ssbuf__ void*  GetSsbufBaseAddr()
```

## 参数说明

无

## 返回值说明

返回指向SSBuffer基地址的指针。

## 约束说明

- SSBuffer中存在脏数据，读取时数据不保证全为0。
- AIC和AIV启动不同的任务时，不能访问SSBuffer。
- 访问超过最末端的地址存在异常。每个核在非MIX模式下运行时，可以独立占用1KB的空间（AIC，AIV0，AIV1各占据1KB）；或者在MIX模式下运行时共享整个3KB的空间(AIC:AIV = 1:2)。目前SSBuffer的大小为3KB。
- SSBuffer只支持通过读写指令32字节的对齐访问。

## 调用示例

```cpp
 __ssbuf__ void* ssbuf = GetSsbufBaseAddr(); // NPU域中返回(void*) 0 ,CPU域中返回CPU模拟分配的地址
AscendC::printf("Ssbuf基地址指针：%p\n", ssbuf); // %p为打印指针格式符，NPU环境中，将特殊的0指针值显示为nil
```
