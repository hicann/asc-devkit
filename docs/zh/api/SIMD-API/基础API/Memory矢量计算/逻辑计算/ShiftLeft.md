# ShiftLeft<a name="ZH-CN_TOPIC_0000001474880641"></a>

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
- Atlas 200I/500 A2 推理产品：支持
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

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_vec_binary_scalar_intf.h"`。

对源操作数中的每个元素进行左移操作，左移的位数由标量scalarValue决定。根据源操作数的数据类型，左移操作分为以下两种情况：

- 数据类型为无符号类型：执行逻辑左移。逻辑左移会将二进制数整体向左移动指定的位数，最高位被丢弃，最低位用0填充。例如，二进制数1010101010101010（uint16\_t类型）逻辑左移1位后，结果为0101010101010100。
- 数据类型为有符号类型：执行算术左移。算术左移会将二进制数整体向左移动指定的位数，次高位被丢弃，最低位用0填充。例如，二进制数1010101010101010（int16\_t类型）算术左移1位后，结果为1101010101010100；算术左移3位后，结果为1101010101010000。

计算公式如下：

$dst_i = src_i \ll scalarValue$

## 函数原型<a name="section620mcpsimp"></a>

- tensor前n个数据连续计算

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void ShiftLeft(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, const int32_t& count)
    ```

- tensor高维切分计算
    - mask逐bit模式

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void ShiftLeft(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    - mask连续模式

        ```cpp
        template <typename T, bool isSetMask = true>
        __aicore__ inline void ShiftLeft(const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

dst和src使用[TensorTrait](../../辅助数据结构/TensorTrait/TensorTrait.md)类型时，其数据类型TensorTrait和scalarValue的数据类型（对应TensorTrait中的LiteType类型）不一致。因此新增模板类型U表示scalarValue的数据类型，并通过std::enable\_if检查T中萃取出的LiteType和U是否完全一致，一致则接口通过编译，否则编译失败。接口原型定义如下：

- tensor前n个数据计算

    ```cpp
    template <typename T, typename U, bool isSetMask = true, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
    __aicore__ inline void ShiftLeft(const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const int32_t& count)
    ```

- tensor高维切分计算
    - mask逐bit模式

        ```cpp
        template <typename T, typename U, bool isSetMask = true, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
        __aicore__ inline void ShiftLeft(const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    - mask连续模式

        ```cpp
        template <typename T, typename U, bool isSetMask = true, typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
        __aicore__ inline void ShiftLeft(const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

## 参数说明<a name="section622mcpsimp"></a>

### 模板参数及接口参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
|---|---|
| T | 操作数数据类型。 |
| U | scalarValue的数据类型。 |
| isSetMask | 是否在接口内部设置mask。<br>&bull; true，表示在接口内部设置mask。<br>&bull; false，表示在接口外部设置mask，开发者需要使用[SetVectorMask](../掩码操作/SetVectorMask.md)接口设置mask值。这种模式下：<br>&nbsp;&nbsp;&bull; 针对tensor前n个数据计算接口，接口入参中的count不生效，建议设置成1。<br>&nbsp;&nbsp;&bull; 针对tensor高维切分计算接口，接口入参中的mask值设置为占位符`MASK_PLACEHOLDER`，用于占位，无实际含义。<br><!-- npu="950,310b" id8 -->针对以下型号，tensor前n个数据计算API中的isSetMask参数不生效，保持默认值即可。<br>&bull; <!-- npu="950" id9 -->Ascend 950PR/Ascend 950DT<!-- end id9 --><br>&bull; <!-- npu="310b" id10 -->Atlas 200I/500 A2 推理产品<!-- end id10 --><!-- end id8 --> |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
|---|---|---|
| dst | 输出 | 目的操作数。<br>类型为LocalTensor，支持的TPosition为VECIN/VECCALC/VECOUT。<br>地址对齐约束参考[通用地址对齐约束](../../../通用说明和约束.md)。 |
| src | 输入 | 源操作数。<br>类型为LocalTensor，支持的TPosition为VECIN/VECCALC/VECOUT。<br>地址对齐约束参考[通用地址对齐约束](../../../通用说明和约束.md)。<br>源操作数的数据类型需要与目的操作数保持一致。 |
| scalarValue | 输入 | 左移的位数，数据类型需要与目的操作数中的元素数据类型保持一致。具体取值说明请参考下面的[scalarValue取值说明](#scalarvalue取值说明)。 |
| mask[]/mask | 输入 | mask用于控制每次迭代内参与计算的元素。详细设置参考[掩码](../SIMD计算说明/掩码.md)。 |
| repeatTime | 输入 | 重复迭代次数。矢量计算单元，每次读取连续的256Bytes数据进行计算，为完成对输入数据的处理，必须通过多次迭代（repeat）才能完成所有数据的读取与计算。repeatTime表示迭代的次数。<br>关于该参数的具体说明请参考[高维切分](../SIMD计算说明/高维切分.md)。 |
| repeatParams | 输入 | 控制操作数地址步长的参数。[UnaryRepeatParams](../../辅助数据结构//UnaryRepeatParams.md)类型，包含操作数相邻迭代间相同DataBlock的地址步长，操作数同一迭代内不同DataBlock的地址步长等参数。<br>相邻迭代间的地址步长参数说明请参考[repeatStride](../SIMD计算说明/高维切分.md)；同一迭代内DataBlock的地址步长参数说明请参考[dataBlockStride](../SIMD计算说明/高维切分.md)。 |

### scalarValue取值说明

<!-- npu="950" id11 -->
- 针对Ascend 950PR/Ascend 950DT，scalarValue的取值应大于等于0，如果左移的位数大于src数据类型位宽，dst的全部元素被赋值为0。
<!-- end id11 -->
<!-- npu="A3" id12 -->
- 针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，当src为uint16_t或int16_t类型时，scalarValue的取值范围为[0, 16]；当src为uint32_t或int32_t类型时，scalarValue的取值范围为[0, 32]。
<!-- end id12 -->
<!-- npu="910b" id13 -->
- 针对Atlas A2 训练系列产品/Atlas A2 推理系列产品，当src为uint16_t或int16_t类型时，scalarValue的取值范围为[0, 16]；当src为uint32_t或int32_t类型时，scalarValue的取值范围为[0, 32]。
<!-- end id13 -->
<!-- npu="310b" id14 -->
- 针对Atlas 200I/500 A2 推理产品，当src为uint16_t或int16_t类型时，scalarValue的取值范围为[0, 16]；当src为uint32_t或int32_t类型时，scalarValue的取值范围为[0, 32]。
<!-- end id14 -->

## 数据类型

<!-- npu="950" id15 -->
- 针对Ascend 950PR/Ascend 950DT，T和U支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t、int64_t、uint64_t。数据类型int8_t、uint8_t、int64_t、uint64_t仅支持tensor前n个数据计算接口。
<!-- end id15 -->
<!-- npu="A3" id16 -->
- 针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，T和U支持的数据类型为：int16_t、uint16_t、int32_t、uint32_t。
<!-- end id16 -->
<!-- npu="910b" id17 -->
- 针对Atlas A2 训练系列产品/Atlas A2 推理系列产品，T和U支持的数据类型为：int16_t、uint16_t、int32_t、uint32_t。
<!-- end id17 -->
<!-- npu="310b" id18 -->
- 针对Atlas 200I/500 A2 推理产品，T和U支持的数据类型为：int16_t、uint16_t、int32_t、uint32_t。
<!-- end id18 -->

## 返回值说明<a name="section194321251175110"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md)。
- 操作数地址重叠约束请参考[通用地址重叠约束](../../../通用说明和约束.md)。
<!-- npu="A3,910b,950" id19 -->
- 当参数count或repeatTime取值为0时，该接口的行为如下：
  <!-- npu="A3,910b" id20 -->
  - 针对如下型号，该接口不会执行计算操作，不会对目的操作数进行写入，该接口将被视为NOP（空操作）。
    <!-- npu="A3" id21 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品
    <!-- end id21 -->
    <!-- npu="910b" id22 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品
    <!-- end id22 -->
  <!-- end id20 -->
  <!-- npu="950" id23 -->
  - 针对Ascend 950PR/Ascend 950DT，该接口通过VF调用[Reg矢量计算API](../../Reg矢量计算/Reg矢量计算.md)实现兼容，当参数count或repeatTime取值为0时，不保证该接口将被视为NOP（空操作）。
  <!-- end id23 -->
<!-- end id19 -->
<!-- npu="950" id24 -->
- 针对Ascend 950PR/Ascend 950DT，tensor前n个数据计算API中的isSetMask参数不生效，保持默认值即可。
<!-- end id24 -->

## 调用示例<a name="section132384819392"></a>

- tensor高维切分计算样例-mask连续模式

    ```cpp
    uint64_t mask = 128;
    int16_t scalar = 2;
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::ShiftLeft(dstLocal, srcLocal, scalar, mask, 4, { 1, 1, 8, 8 });
    ```

- tensor高维切分计算样例-mask逐bit模式

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    int16_t scalar = 2;
    // repeatTime = 4, 128 elements one repeat, 512 elements total.
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat.
    // dstRepStride, srcRepStride = 8, no gap between repeats.
    AscendC::ShiftLeft(dstLocal, srcLocal, scalar, mask, 4, {1, 1, 8, 8});
    ```

- tensor前n个数据计算样例

    ```cpp
    int16_t scalar = 2;
    AscendC::ShiftLeft(dstLocal, srcLocal, scalar, 512);
    ```

结果示例如下：

```plain
输入数据srcLocal: [1 2 3 ... 512]
输入数据scalar = 2
输出数据dstLocal: [4 8 12 ... 2048]
```
