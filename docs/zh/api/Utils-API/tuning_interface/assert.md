# assert/ascendc_assert

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
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90：不支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：不支持
<!-- end id9 -->

不同产品对各编程场景的支持情况如下表所示。

**表1**  支持能力

<table>
    <tr>
        <th>产品</th>
        <th>SIMD</th>
        <th>SIMT VF</th>
        <th>SIMD VF</th>
    </tr>
    <!-- npu="950" id10 -->
    <tr>
        <td>Ascend 950PR/Ascend 950DT</td>
        <td>支持</td>
        <td>支持</td>
        <td>支持</td>
    </tr>
    <!-- end id10 -->
    <!-- npu="A3" id11 -->
    <tr>
        <td>Atlas A3 训练系列产品/Atlas A3 推理系列产品</td>
        <td>支持</td>
        <td>不支持</td>
        <td>不支持</td>
    </tr>
    <!-- end id11 -->
    <!-- npu="910b" id12 -->
    <tr>
        <td>Atlas A2 训练系列产品/Atlas A2 推理系列产品</td>
        <td>支持</td>
        <td>不支持</td>
        <td>不支持</td>
    </tr>
    <!-- end id12 -->
    <!-- npu="310p" id13 -->
    <tr>
        <td>Atlas 推理系列产品 AI Core</td>
        <td>支持</td>
        <td>不支持</td>
        <td>不支持</td>
    </tr>
    <!-- end id13 -->
    <!-- npu="310p" id14 -->
    <tr>
        <td>Atlas 推理系列产品 Vector Core</td>
        <td>支持</td>
        <td>不支持</td>
        <td>不支持</td>
    </tr>
    <!-- end id14 -->
</table>

## 功能说明

头文件路径为：`"utils/debug/asc_assert.h"`。

`assert`和`ascendc_assert`用于在Device侧代码中进行断言检查。断言条件成立时，后续代码继续执行；断言条件不成立时，接口打印断言失败信息并触发异常，算子执行失败。

需要在断言失败时输出自定义错误信息，可以在条件后传入格式字符串和对应参数。`assert`和`ascendc_assert`提供相同的断言能力，可以任选其一使用。

> [!CAUTION]注意
> 该接口用于调测，使用时会影响算子性能，生产环境建议通过设置[ASCENDC_DUMP=0](../../SIMD-API/basic_api/debug_interface/disable_ascendc_dump.md)关闭。

## 函数原型

```cpp
assert(expr)
assert(expr, fmt, args...)

ascendc_assert(expr)
ascendc_assert(expr, fmt, args...)
```

## 参数说明

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| expr | 输入 | 断言条件。为`true`时，Kernel继续运行；为`false`时，接口打印断言失败信息并触发异常。 |
| fmt | 输入 | 可选。自定义错误信息的格式控制字符串，包含普通字符和转换说明。普通字符原样输出；转换说明以百分号（%）开始，用于控制`args`中对应参数的转换和输出。支持的转换类型和数据类型参见[printf](printf.md)的参数说明。 |
| args | 输入 | 可选。与`fmt`中的转换说明对应的附加参数。每个参数替换`fmt`中对应的转换说明，参数数量和类型应与转换说明匹配。 |

## 返回值说明

无

## 约束说明

### 通用约束

- 在CMake工程的Release模式下，默认使能`NDEBUG`宏定义，本接口不生效。
- 使用`assert`时，如需包含标准库头文件`<cassert>`，请先包含`<cassert>`，再包含`"kernel_operator.h"`或`"utils/debug/asc_assert.h"`，避免标准库覆盖Ascend C的`assert`定义。

### SIMD编程场景

- 单次调用本接口打印的数据总量不可超过打印大小限制，默认为30KB。超出限制时，断言失败信息不会打印，但接口仍会触发异常。可以通过[aclInit接口](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/latest/API/runtimeapi/aclcppdevg_03_0022.html)中的`simd_printf_fifo_size_per_core`字段配置，配置范围为`[1KB,64MB]`。PyTorch调用和算子入图场景暂不支持该配置。

### SIMT VF编程场景

不输出`fmt`和`args`指定的自定义错误信息。

### SIMD VF编程场景

- `fmt`可以直接使用字符串字面量。使用字符串指针时，该指针必须指向UB中的字符串，示例如下：

    ```cpp
    __ubuf__ const char* fmt = "value is %d.\n";
    assert(value == 6, fmt, value);
    ```

- 每个AIV核在单次`asc_vf_call`执行期间使用2KB预留UB空间临时保存调测数据。同一次`asc_vf_call`中的`assert`、`ascendc_assert`、`printf`和`asc_dump`共享该空间。该空间中的数据传输完成后会被复用，因此上述接口产生的累计调测数据可以超过2KB。单条断言失败信息必须能完整保存在该空间中，否则该条信息不会打印，但接口仍会触发异常。
- SIMD VF场景下，本接口需要使用2KB预留UB空间传递断言信息。开启`--cce-disable-asc-reserved-ubuf`选项后，本接口不可用。
- SIMD VF场景下，`simd_printf_fifo_size_per_core`建议配置为3KB以上。配置过小且打印数据量较大时，断言失败信息不会被打印，但接口仍会触发异常。

## 调用示例

### SIMD编程场景

条件不满足时，接口打印AIC/AIV类型、Block号、源码位置、函数名和条件表达式。传入格式字符串和对应参数时，还会打印自定义错误信息。随后，接口触发异常。

以下示例中，`value`为7，不满足`value == 6`，因此触发断言。

```cpp
extern "C" __global__ __vector__ void AssertSimdKernel()
{
    int32_t value = 7;
    AscendC::printf("before assert, value is %d.\n", value);
    assert(value == 6, "value is %d.\n", value);
}
```

程序运行时会触发断言，实际打印如下：

```plain
[AIV Block 0/1] before assert, value is 7.
[AIV Block 0/1] [ASSERT] .../assert_simd.asc:10: void AssertSimdKernel(): Assertion 'value == 6' failed. value is 7.
```

### SIMT VF编程场景

条件不满足时，接口打印源码位置、函数名和条件表达式，然后触发异常。

-   SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(1024) inline void simt_kernel(float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        assert(!isnan(x[idx]));
    }
    ```

    程序运行时会触发assert，打印效果如下：

    ```plain
    [ASSERT] /home/.../simt_kernel.asc:44: void simt_kernel(float *): Assertion `!isnan(x[idx])' failed.
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void simt_kernel(__gm__ float* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        assert(!isnan(x[idx]));
    }
    ```

    程序运行时会触发assert，打印效果如下：

    ```plain
    [ASSERT] /home/.../simt_kernel.asc:44: void simt_kernel(__gm__ float *): Assertion `!isnan(x[idx])' failed.
    ```

### SIMD VF编程场景

条件不成立时，接口打印源码位置、函数名和条件表达式；如果传入了格式字符串和对应参数，自定义错误信息会追加在该断言失败信息之后。随后，接口触发异常。

```cpp
__simd_vf__ inline void AssertSimdVf()
{
    int32_t value = 7;
    assert(value == 6, "value is %d.\n", value);
}
```

程序运行时会触发断言，打印格式如下：

```plain
[ASSERT] .../assert_simd_vf.asc:12: void AssertSimdVf(): Assertion 'value == 6' failed. value is 7.
```
