# GetReduceRepeatMaxMinSpr(ISASI)

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
- Atlas 200I/500 A2 推理产品：不支持
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

## 功能说明

头文件路径为：`"basic_api/kernel_operator_vec_reduce_intf.h"`。

本接口用于获取调用[ReduceRepeat<MAX/MIN>](../归约计算/ReduceRepeat.md)时所有repeat内的最值及其索引，或获取调用[ReduceMax](../归约计算/ReduceMax.md)/[ReduceMin](../归约计算/ReduceMin.md)得到的最值。计算结果以全局变量形式存储，可以随时调用获取。

## 函数原型

<!-- npu="A3,910b" id8 -->

- 获取调用[ReduceRepeat<MAX/MIN>](../归约计算/ReduceRepeat.md)时所有repeat内的最值及其索引，或获取调用[ReduceMax](../归约计算/ReduceMax.md)/[ReduceMin](../归约计算/ReduceMin.md)得到的最值（此时获取的索引不准确）。该函数原型仅支持如下型号：

    <!-- npu="A3" id9 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品
    <!-- end id9 -->

    <!-- npu="910b" id10 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品
    <!-- end id10 -->

    ```cpp
    template <typename T>
    __aicore__ inline void GetReduceRepeatMaxMinSpr(T &maxMinValue, T &maxMinIndex)
    ```
    
<!-- end id8 -->

<!-- npu="310p" id11 -->

- 获取调用[ReduceRepeat<MAX/MIN>](../归约计算/ReduceRepeat.md)时所有repeat内的最值，或获取调用[ReduceMax](../归约计算/ReduceMax.md)/[ReduceMin](../归约计算/ReduceMin.md)得到的最值。该函数原型仅支持Atlas 推理系列产品AI Core。

    ```cpp
    template <typename T>
    __aicore__ inline void GetReduceRepeatMaxMinSpr(T &maxMinValue)
    ```

<!-- end id11 -->

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数数据类型。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| maxMinValue | 输出 | 调用[ReduceRepeat<MAX/MIN>](../归约计算/ReduceRepeat.md)时所有repeat内的最值，或调用[ReduceMax](../归约计算/ReduceMax.md)/[ReduceMin](../归约计算/ReduceMin.md)得到的最值。 |
| maxMinIndex | 输出 | 调用[ReduceRepeat<MAX/MIN>](../归约计算/ReduceRepeat.md)时所有repeat内的最值索引。 |

## 数据类型

操作数支持的数据类型为：`half`、`float`。

## 返回值说明

无

## 约束说明

- 仅支持`ReduceRepeat<MAX/MIN>`、`ReduceMax`、`ReduceMin`的连续计算场景，即调用上述归约接口时参数需满足`srcRepStride=8`、`srcBlkStride=1`。
- 不支持获取归约接口使用[Mask Counter模式](../SIMD计算说明/掩码.md#mask-mode)时的结果。
- 用于[ReduceMax](../归约计算/ReduceMax.md)/[ReduceMin](../归约计算/ReduceMin.md)场景时，仅可获取准确最值，不能获取对应的准确索引；如需准确索引，以[ReduceMax](../归约计算/ReduceMax.md)/[ReduceMin](../归约计算/ReduceMin.md)自身输出为准。
- 索引`maxMinIndex`按操作数数据类型存储，比如操作数使用`half`类型时，`maxMinIndex`是按照`half`类型进行存储的，如果按照`half`格式进行读取，`maxMinIndex`的值是不对的，因此`maxMinIndex`的读取需要使用`reinterpret_cast`方法转换到整数类型，若输入数据类型是`half`，需要使用`reinterpret_cast<uint16_t*>`，若输入是`float`，需要使用`reinterpret_cast<uint32_t*>`。
- 操作数数据类型T，需要与所使用的归约接口目的操作数数据类型一致，否则会导致精度错误。

## 调用示例

具体可参考[ReduceRepeat系列归约指令样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce_repeat)。

1. 调用`ReduceRepeat<MAX>`后，所有repeat内的最大值及索引。

    <!-- npu="A3,910b" id12 -->

    - 针对如下型号：
      <!-- npu="A3" id13 -->
      - Atlas A3 训练系列产品/Atlas A3 推理系列产品
      <!-- end id13 -->
      <!-- npu="910b" id14 -->
      - Atlas A2 训练系列产品/Atlas A2 推理系列产品
      <!-- end id14 -->

      ```cpp
      AscendC::LocalTensor<float> src;
      AscendC::LocalTensor<float> dst;
      int32_t mask = 64;
      float val = 0;   // 最大值
      float idx = 0;   // 最大值索引
      AscendC::ReduceRepeat<AscendC::ReduceType::MAX>(dst, src, mask, 1, 1, 1, 8);
      AscendC::GetReduceRepeatMaxMinSpr<float>(val, idx); // 保证和ReduceRepeat的调用次序，而且要配对调用
      ```

    <!-- end id12 -->

    <!-- npu="310p" id15 -->

    - 针对Atlas 推理系列产品AI Core：

        ```cpp
        AscendC::LocalTensor<float> src;
        AscendC::LocalTensor<float> dst;
        int32_t mask = 64;
        AscendC::ReduceRepeat<AscendC::ReduceType::MAX>(dst, src, mask, 1, 1, 1, 8);
        float val = 0;   // 最大值
        AscendC::GetReduceRepeatMaxMinSpr<float>(val); // 保证和ReduceRepeat的调用次序，而且要配对调用
        ```

    <!-- end id15 -->

2. 调用`ReduceMax`后，获取最大值。

    <!-- npu="A3,910b" id16 -->

    - 针对如下型号，`GetReduceRepeatMaxMinSpr`仅用于获取最大值，不能用于获取`ReduceMax`对应的准确索引。
      <!-- npu="A3" id17 -->
      - Atlas A3 训练系列产品/Atlas A3 推理系列产品
      <!-- end id17 -->
      <!-- npu="910b" id18 -->
      - Atlas A2 训练系列产品/Atlas A2 推理系列产品
      <!-- end id18 -->

      ```cpp
      AscendC::LocalTensor<float> src;
      AscendC::LocalTensor<float> work;
      AscendC::LocalTensor<float> dst;
      int32_t mask = 64;
      AscendC::ReduceMax(dst, src, work, mask, 1, 8, true); // 连续场景，srcRepStride = 8，且calIndex = true
      float val = 0;   // 最大值
      float idx = 0;   // 不用于获取ReduceMax对应的准确索引
      AscendC::GetReduceRepeatMaxMinSpr<float>(val, idx); // 保证和ReduceMax的调用次序，而且要配对调用
      ```

    <!-- end id16 -->

    <!-- npu="310p" id19 -->

    - 针对Atlas 推理系列产品AI Core版本，可在调用`ReduceMax`后直接调用`GetReduceRepeatMaxMinSpr`指令获取最大值。

        ```cpp
        AscendC::LocalTensor<float> src;
        AscendC::LocalTensor<float> work;
        AscendC::LocalTensor<float> dst;
        int32_t mask = 64;
        AscendC::ReduceMax(dst, src, work, mask, 1, 8, true);
        float val = 0;   // 最大值
        AscendC::GetReduceRepeatMaxMinSpr<float>(val); // 保证和ReduceMax的调用次序，而且要配对调用
        ```

    <!-- end id19 -->
