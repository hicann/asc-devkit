# 编程示例<a name="ZH-CN_TOPIC_0000002563469531"></a>

本节以[gather & adds混合编程样例](../../../../../../../examples/05_simd_simt_hybrid/00_introduction/simd_simt_gather_and_adds)为例，展示SIMD与SIMT混合编程的关键语法。

## 场景说明<a name="zh-cn_topic_0000002563469531_section164411381518"></a>

样例从长度为100000的一维输入`input`中，按照`index`数组指定的位置离散读取8192个元素，并对读取结果执行加1操作：

```cpp
output[i] = input[index[i]] + 1
```

其中，离散读取`input[index[i]]`适合使用SIMT线程并行完成；加1操作面向连续UB数据，适合使用SIMD向量计算完成。

| 数据 | Shape | 数据类型 | 说明 |
| --- | --- | --- | --- |
| input | [100000] | float | 被离散读取的输入数据 |
| index | [8192] | uint32_t | 每个输出元素对应的input下标 |
| output | [8192] | float | gather后加1的输出结果 |

样例启动8个Vector Core，每个Vector Core处理1024个输出元素。每个核内通过`asc_vf_call`启动一个SIMT VF函数，`dim3(THREAD_COUNT)`配置1024个SIMT线程，每个线程负责一个`index`元素。

## 定义SIMT VF函数<a name="zh-cn_topic_0000002563469531_section1998615411431"></a>

SIMT VF函数使用`__simt_vf__`修饰，函数内可以使用`blockIdx`、`blockDim`、`threadIdx`等SIMT内置变量。下面代码中，每个线程根据自己的`threadIdx.x`读取一个`index`，再从GM输入`input`中读取对应数据并写入UB缓存`local_output`。

```cpp
constexpr uint32_t THREAD_COUNT = 1024;

__simt_vf__ __launch_bounds__(THREAD_COUNT) inline void simt_gather(
    __gm__ float* input, __gm__ uint32_t* index, __ubuf__ float* local_output,
    uint32_t input_total_length, uint32_t index_total_length, uint32_t output_total_length)
{
    if (threadIdx.x >= output_total_length) {
        return;
    }

    uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx >= index_total_length) {
        return;
    }

    uint32_t gather_idx = index[idx];
    if (gather_idx >= input_total_length) {
        return;
    }

    local_output[threadIdx.x] = input[gather_idx];
}
```

关键点如下：

-   `__simt_vf__`标识该函数在SIMT单元执行。
-   `__launch_bounds__(THREAD_COUNT)`声明该SIMT VF函数的最大线程数。
-   `__gm__`用于标识GM指针，`__ubuf__`用于标识UB指针。
-   `threadIdx.x`表示当前线程在线程块内的索引，用于让不同线程处理不同元素。

## 调用VF函数<a name="zh-cn_topic_0000002563469531_section199351451451"></a>

在`__global__ __vector__`核函数中，申请UB空间作为共享内存，再通过`asc_vf_call<simt_gather>`调用SIMT VF函数完成gather计算，而后通过`asc_vf_call<simd_adds>`调用SIMD VF函数执行后续的加法计算。在调用SIMT VF函数时，`asc_vf_call`的第一个参数`dim3(THREAD_COUNT)`表示本次SIMT VF调用启动1024个线程。

```cpp
__global__ __vector__ void gather_and_adds_kernel(
    __gm__ float* input, __gm__ uint32_t* index, __gm__ float* output,
    uint32_t input_total_length, uint32_t index_total_length)
{
    uint32_t index_total_length_per_block = index_total_length / block_num;
    extern __ubuf__ float local_output[];

    asc_vf_call<simt_gather>(
        dim3(THREAD_COUNT), input, index, local_output,
        input_total_length, index_total_length, index_total_length_per_block);

    uint32_t one_repeat_size = asc_get_vf_len() / sizeof(float);
    uint16_t repeat_times = (index_total_length_per_block + one_repeat_size - 1) / one_repeat_size;

    // 启动SIMD VF函数，对Gather结果执行加1操作。
    asc_vf_call<simd_adds>(
        local_output, local_output,
        index_total_length_per_block, one_repeat_size, repeat_times);
}
```

`asc_vf_call<simt_gather>`调用后，当前Vector Core内会启动一个SIMT线程块。以本样例为例，`index_total_length_per_block`为1024，`THREAD_COUNT`也为1024，因此一个线程块即可处理当前核负责的全部输出元素。

## 配置核函数启动<a name="zh-cn_topic_0000002563469531_section1680714261755"></a>

Host侧通过`<<<>>>`内核调用符启动`__global__ __vector__`核函数。下面代码中，`num_blocks`表示启动8个Vector Core，`dyn_ub_size`表示动态UB大小，本样例取值为`THREAD_COUNT * sizeof(float)`，`stream`用于维护异步执行顺序。

```cpp
constexpr uint32_t input_total_length = 100000;
constexpr uint32_t index_total_length = 8 * 1024;
constexpr uint32_t num_blocks = 8;
constexpr uint32_t dyn_ub_size = THREAD_COUNT * sizeof(float);

gather_and_adds_kernel<<<num_blocks, dyn_ub_size, stream>>>(
    input_device, index_device, output_device, input_total_length, index_total_length);
```

`<<<>>>`中的三个配置参数含义如下：

| 参数 | 本样例取值 | 说明 |
| --- | --- | --- |
| num_blocks | 8 | 启动8个Vector Core，每个核处理1024个输出元素 |
| dyn_ub_size | THREAD_COUNT * sizeof(float) | 为核函数配置动态UB空间 |
| stream | stream | 指定ACL运行时流 |

## 数据同步<a name="zh-cn_topic_0000002563469531_section_data_sync"></a>

SIMT VF和SIMD VF属于Vector流水，与MTE数据搬运异步执行，因此需要确保数据搬运操作与VF函数调用之间正确同步。本例中，SIMT VF函数直接读取GM输入数据，不涉及数据搬入；SIMD VF执行完后，需要将数据从UB写出到GM，此时需要加上同步指令保证操作顺序。整体流程如下：

```cpp
asc_vf_call<simt_gather>(...);
asc_vf_call<simd_adds>(...);
// 同步Vector计算流水和MTE3搬出流水，确保Adds结果写入UB后再搬运到GM。
asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
asc_copy_ub2gm_align(...);
```

## 结果说明<a name="zh-cn_topic_0000002563469531_section1184165711616"></a>

样例执行后会生成`output/output.bin`，输出中第`i`个元素满足：

```cpp
output[i] = input[index[i]] + 1
```

完整工程包含输入生成脚本和结果校验脚本，执行精度校验通过时输出如下：

```text
test pass!
```
