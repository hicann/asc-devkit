# Fill

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
- Atlas 推理系列产品AI Core：支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->

## 功能说明

将Global Memory上的数据初始化为指定值。该接口可用于对workspace地址或输出数据进行清零。

## 函数原型

```
template <typename T>
__aicore__ inline void Fill(GlobalTensor<T>& gmWorkspaceAddr, const uint64_t size, const T value)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 含义 |
| --- | --- |
| T | 操作数的数据类型。不同型号支持的数据类型请参考[支持的数据类型](#li194619420173)。 |

**表2**  接口参数说明

| 参数名 | 输入/输出 | 含义 |
| --- | --- | --- |
| gmWorkspaceAddr | 输入 | gmWorkspaceAddr为用户定义的全局空间，是需要被初始化的空间，类型为GlobalTensor。GlobalTensor数据结构的定义请参考[GlobalTensor](../../基础API/数据结构/GlobalTensor/GlobalTensor.md)。 |
| size | 输入 | 需要初始化的空间大小，单位为元素个数。 |
| value | 输入 | 初始化的值，数据类型与gmWorkspaceAddr保持一致。 |

## 返回值说明

无

## 约束说明

-   单核调用此接口时，如果后续操作涉及Unified Buffer的使用，则需要在调用接口后，设置MTE2流水等待MTE3流水（[MTE3\_MTE2](../../基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md#section622mcpsimp)）的同步。
-   单核上连续多次调用此接口且初始化的值不一致时，需要在接口之间设置V流水等待MTE3流水（[MTE3\_V](../../基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md#section622mcpsimp)）的同步。由于该接口内部使用同一块Unified Buffer作为中转空间进行值初始化，连续调用接口时若不进行流水同步，后一次写操作可能覆盖前一次未完成写入的数据，导致前一次初始化Global Memory的结果非预期。
-   当多个核调用此接口对Global Memory进行初始化时，所有核对Global Memory的初始化未必会同时结束，也可能存在核之间读后写、写后读以及写后写等数据依赖问题。这种使用场景下，可以在本接口后调用[SyncAll](../../基础API/同步控制/核间同步/SyncAll.md)接口保证多核间同步正确。
-   该接口仅支持在程序内存分配[InitBuffer](../../基础API/资源管理/TPipe/InitBuffer.md)接口前使用。
-   支持的数据类型<a id="li194619420173"></a>

    <!-- npu="950" id7 -->
    Ascend 950PR/Ascend 950DT，支持的数据类型为：uint8\_t、int8\_t、uint16\_t、int16\_t、bfloat16\_t、half、uint32\_t、int32\_t、float、uint64\_t、int64\_t。
    <!-- end id7 -->
    <!-- npu="A3" id8 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为：uint16\_t、int16\_t、half、uint32\_t、int32\_t、float。
    <!-- end id8 -->
    <!-- npu="910b" id9 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为：uint16\_t、int16\_t、half、uint32\_t、int32\_t、float。
    <!-- end id9 -->
    <!-- npu="310p" id10 -->
    Atlas 推理系列产品AI Core，支持的数据类型为：uint16\_t、int16\_t、half、uint32\_t、int32\_t、float。
    <!-- end id10 -->

## 调用示例

本调用示例使用8个核，每个核用当前blockIdx的值初始化zGm上的65536个数。

```
// 带初始化的元素个数
constexpr int32_t INIT_SIZE = 65536;

// 设置源操作数在Global Memory上的起始地址为z+INIT_SIZE*blockIdx，所占外部存储的大小为INIT_SIZE个float
zGm.SetGlobalBuffer((__gm__ float*)z + INIT_SIZE * AscendC::GetBlockIdx(), INIT_SIZE);
// 将起始地址为z+INIT_SIZE*blockIdx和大小为INIT_SIZE个float源操作数zGm的值，初始化为每个核用当前blockIdx
AscendC::Fill(zGm, INIT_SIZE, (float)(AscendC::GetBlockIdx()));
```

结果示例如下：

```
初始化后zGm数据：
[0. 0. 0. ... 0. 0. 0.
 1. 1. 1. ... 1. 1. 1.
 2. 2. 2. ... 2. 2. 2.
 ...
 5. 5. 5. ... 5. 5. 5.
 6. 6. 6. ... 6. 6. 6.
 7. 7. 7. ... 7. 7. 7.]
```
