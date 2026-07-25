# AddDeqRelu<a name="ZH-CN_TOPIC_0000001815255356"></a>

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

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_vec_binary_intf.h"`。

依次计算按元素求和、结果进行deq量化后再进行relu计算（结果和0对比取较大值）。计算公式如下：

$$
dst_i = Relu(Deq(src0_i + src1_i))
$$

Deq的计算公式如下，采用RINT舍入方式，scale需要通过[SetDeqScale](../类型转换辅助配置接口/SetDeqScale.md)进行设置：

$$
Deq(x) = Cast(x \times scale)
$$

其中Relu的计算公式如下：

$$
Relu(x) = max(0, x)
$$

## 函数原型<a name="section620mcpsimp"></a>

- tensor前n个数据计算

  ```cpp
  __aicore__ inline void AddDeqRelu(const LocalTensor<half>& dst, const LocalTensor<int32_t>& src0, const LocalTensor<int32_t>& src1, const int32_t& count)
  ```

- tensor高维切分计算
  - mask逐bit模式

    ```cpp
    template <bool isSetMask = true>
    __aicore__ inline void AddDeqRelu(const LocalTensor<half>& dst, const LocalTensor<int32_t>& src0, const LocalTensor<int32_t>& src1, uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
    ```

  - mask连续模式

    ```cpp
    template <bool isSetMask = true>
    __aicore__ inline void AddDeqRelu(const LocalTensor<half>& dst, const LocalTensor<int32_t>& src0, const LocalTensor<int32_t>& src1, uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
    ```

操作数使用[TensorTrait](../../辅助数据结构/TensorTrait/TensorTrait.md)类型时，LocalTensor需要输入模板参数。提供支持操作数数据类型作为模板参数传入的接口如下：

- tensor前n个数据计算

  ```cpp
  template <typename T, typename U>
  __aicore__ inline void AddDeqRelu(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, const int32_t& count)
  ```

- tensor高维切分计算
  - mask逐bit模式

    ```cpp
    template <typename T, typename U, bool isSetMask = true>
    __aicore__ inline void AddDeqRelu(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
    ```

  - mask连续模式

    ```cpp
    template <typename T, typename U, bool isSetMask = true>
    __aicore__ inline void AddDeqRelu(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
    ```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| :----- | :--- |
| T | 目的操作数的数据类型。 |
| U | 源操作数的数据类型。 |
| isSetMask | 是否在接口内部设置mask。<br>&bull; true，表示在接口内部设置mask。<br>&bull; false，表示在接口外部设置mask，开发者需要使用[SetVectorMask](../掩码操作/SetVectorMask.md)接口设置mask值。这种模式下，接口入参中的mask值设置为占位符`MASK_PLACEHOLDER`，用于占位，无实际含义。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| :----- | :-------- | :--- |
| dst | 输出 | 目的操作数。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| src0、src1 | 输入 | 源操作数。<br>类型为LocalTensor，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| count | 输入 | 参与计算的元素个数。<br>**注：参数取值范围和操作数的数据类型有关，数据类型不同，能够处理的元素个数最大值不同，最大处理的数据量不能超过UB大小限制。** |
| mask[]/mask | 输入 | mask用于控制每次迭代内参与计算的元素。<br>设置详见[掩码操作](../SIMD计算说明/掩码.md)。 |
| repeatTime | 输入 | 重复迭代次数。<br>矢量计算单元，每次读取连续的256Bytes数据进行计算，为完成对输入数据的处理，必须通过多次迭代（repeat）才能完成所有数据的读取与计算。repeatTime表示迭代的次数。<br>关于该参数的具体描述请参考[高维切分](../SIMD计算说明/高维切分.md)。 |
| repeatParams | 输入 | 控制操作数地址步长的参数。[BinaryRepeatParams](../../辅助数据结构/BinaryRepeatParams.md)类型，包含操作数相邻迭代间相同datablock的地址步长，操作数同一迭代内不同datablock的地址步长等参数。<br>相邻迭代间的地址步长参数说明请参考[repeatStride](../SIMD计算说明/高维切分.md)；同一迭代内DataBlock的地址步长参数说明请参考[dataBlockStride](../SIMD计算说明/高维切分.md)。 |

## 数据类型

**表3**  数据类型组合情况

| src0、src1数据类型 | dst数据类型 |
| :----------------- | :---------- |
| int32_t            | half        |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md#section796754519912)。
- 操作数地址重叠约束请参考[通用地址重叠约束](../../../通用说明和约束.md#section668772811100)。
<!-- npu="A3,910b,950" id8 -->
- 当参数count或repeatTime取值为0时，该接口的行为如下：
  <!-- npu="A3,910b" id9 -->
  - 针对如下型号，该接口不会执行计算操作，不会对目的操作数进行写入，该接口将被视为NOP（空操作）。
    <!-- npu="A3" id10 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品
    <!-- end id10 -->
    <!-- npu="910b" id11 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品
    <!-- end id11 -->
  <!-- end id9 -->
  <!-- npu="950" id12 -->
  - 针对Ascend 950PR/Ascend 950DT，该接口通过VF调用[Reg矢量计算API](../../Reg矢量计算/Reg矢量计算.md)实现兼容，当参数count或repeatTime取值为0时，不保证该接口将被视为NOP（空操作）。
  <!-- end id12 -->
<!-- end id8 -->
- 该接口需要与`SetDeqScale`配合使用，关系映射表如下：

  | AddDeqRelu使用场景 | 对应使用的`SetDeqScale`函数原型                  |
  | :----------------- | :----------------------------------------------- |
  | 输入类型为int32_t  | `__aicore__ inline void SetDeqScale(half scale)` |

- 本接口涉及临时空间的使用，计算公式如下，当计算数据量小于等于2K（2048）时，使用的临时空间与计算数据量线性相关，当数据量大于2K时，临时空间大小固定为8KB：
  $$
  tmpSize =
  \begin{cases}
    count * sizeof(int32\_t), & \text{if } count \le 2K \\
    8K, & \text{if } count \gt 2K
  \end{cases}
  $$

## 关键特性

### 溢出保护

该接口在执行int32_t到half数据类型的反量化时，会进行防溢出保护处理：
$$Deq(x) = Cast(x \times (1/2^{17}) \times scale \times 2^{17}) = Cast(x \times scale)$$
中间缩小步骤将数值控制在安全范围内，有效避免了浮点溢出。

## 调用示例<a name="section837496171220"></a>

本样例的srcLocal为int32\_t类型，dstLocal为half类型，计算mask时以int32\_t为准。

- tensor高维切分计算样例-mask连续模式。

  ```cpp
  uint64_t mask = 256 / sizeof(int32_t); // 64
  // repeatTime = 4，一次迭代计算64个数，共计算256个数
  // dstBlkStride, src0BlkStride, src1BlkStride = 1，单次迭代内数据连续读取和写入
  // dstRepStride = 4，src0RepStride, src1RepStride = 8，相邻迭代间数据连续读取和写入
  half scale = 0.1;
  AscendC::SetDeqScale(scale);
  AscendC::AddDeqRelu(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 4, 8, 8 });
  ```

- tensor高维切分计算样例-mask逐bit模式。

  ```cpp
  uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
  // repeatTime = 4，一次迭代计算64个数，共计算256个数
  // dstBlkStride, src0BlkStride, src1BlkStride = 1，单次迭代内数据连续读取和写入
  // dstRepStride = 4，src0RepStride, src1RepStride = 8，相邻迭代间数据连续读取和写入
  half scale = 0.1;
  AscendC::SetDeqScale(scale);
  AscendC::AddDeqRelu(dstLocal, src0Local, src1Local, mask, 4, { 1, 1, 1, 4, 8, 8 });
  ```

- tensor前n个数据计算样例。

  ```cpp
  half scale = 0.1;
  AscendC::SetDeqScale(scale);
  AscendC::AddDeqRelu(dstLocal, src0Local, src1Local, 512);
  ```

结果示例如下：

```plain
输入数据src0Local：[70 36 43 54 28 49 27 82 95 ...]
输入数据src1Local：[19 33 34 50 42  2 97 93 99 ...]
输出数据dstLocal：[8.9 6.9 7.7 10.4 7.0 5.1 12.4 17.5 19.4 ...]
```
