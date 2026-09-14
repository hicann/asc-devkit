# DCache Access Optimization

<!-- md-trans-meta sourceCommit=d1fe9e831a69eaae67fc1f463ea1c4ea17a8f417 translatedAt=2026-08-26T14:48:22.391Z -->

[Priority] Medium

[Description] In SIMT programming mode, Global Memory data is accessed through DCache. When an operator contains data with multiple different access patterns, such as input data that is traversed only once and hot data that needs to be accessed repeatedly, all data competes for DCache space if not distinguished. As a result, hot data is evicted from DCache by other data, and subsequent accesses to the hot data require reloading from GM, increasing memory access time. In this case, different cache policies can be specified for different types of data through different memory access functions, so that hot data preferentially resides in DCache and unnecessary GM accesses are reduced.

The following memory access functions are provided in SIMT programming mode:

- **`asc_ldcg`**: Loads data from Global Memory, suitable for input data that is traversed only once, reducing its occupation of DCache space.
- **`asc_ldca`**: Loads data preferentially from DCache, suitable for hot data that needs to be accessed frequently (such as lookup tables), ensuring that hot data resides in DCache and reducing the number of reloads from Global Memory.
- **`asc_stcg`**: Writes data directly to Global Memory without going through DCache, suitable for data that will not be read from DCache after being written to GM, preventing output data from occupying DCache space and affecting the caching of hot data.

[Example Introduction] Take the [sin lookup table operator](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/03_simt_api/03_best_practices/00_memory_optimizations/cache_hint) as an example. It uses the lookup table method to calculate sin values and improves precision through linear interpolation. The input data length is 65536 (256KB), and the sin lookup table length is 8192 (32KB). In the algorithm implementation, each thread calculates the lookup table index based on the input value, reads the data at the corresponding position and its subsequent position from the sin table according to the index, and obtains the result through linear interpolation. Each input data is accessed only once and is therefore non-hot data; the sin lookup table is accessed repeatedly and is therefore hot data.

[Negative Example] All data is loaded using the default method, and the input, output, and sin table data compete for DCache space together.

```
__global__ void sin_table_lookup_baseline(float* input, float* sin_table, float* output,
                                            uint32_t input_length, uint32_t table_length)
{
    for (int idx = threadIdx.x; idx < input_length; idx += blockDim.x) {
        float x = input[idx];
        ...
        float low_val = sin_table[n];
        float high_val = 0.0f;
        if (n + 1 >= table_length) {
            high_val = sin_table[0];
        } else {
            high_val = sin_table[n + 1];
        }
        output[idx] = sign * (low_val + frac * (high_val - low_val));
    }
}
```

In the implementation above, `input[idx]`, `sin_table[n]`, `sin_table[n+1]`, and `output[idx]` all access Global Memory using the default method. Although the input data is accessed only once, its loading occupies DCache space; the output data may also reside in DCache after being written. When a large amount of input and output data is loaded, the sin lookup table data is evicted from DCache, causing subsequent threads to reload it from Global Memory when performing table lookups, which increases the number of DCache Read GM operations.

The performance data of the negative example operator is as follows:

| Task Duration(us) | DCache Read GM(times) | DCache Read Vector(times) | DCache Write Vector(times) |
|:-----------------:|:-----------------:|:---------------------:|:----------------------:|
|       56.82       |       5064        |         2048          |          6144          |

[Positive Example] Use memory access functions to distinguish cache policies for different types of data.

```
__global__ void sin_table_lookup_optimized(float* input, float* sin_table, float* output,
                                             uint32_t input_length, uint32_t table_length)
{
    for (int idx = threadIdx.x; idx < input_length; idx += blockDim.x) {
        float x = asc_ldcg(&input[idx]);
        ...
        float low_val = asc_ldca(&sin_table[n]);
        float high_val = 0.0f;
        if (n + 1 >= table_length) {
            high_val = asc_ldca(&sin_table[0]);
        } else {
            high_val = asc_ldca(&sin_table[n + 1]);
        }
        float y = sign * (low_val + frac * (high_val - low_val));
        asc_stcg(&output[idx], y);
    }
}
```

In the preceding implementation:
- The input data is loaded using `asc_ldcg`. The input data is read directly from Global Memory and does not need to occupy DCache space, avoiding the occupation of DCache by the input data.
- The sin lookup table is loaded using `asc_ldca`, which prioritizes allocating DCache space for the sin table data and ensures that the sin table resides in DCache.
- The output data is written using `asc_stcg`, which writes directly to Global Memory without going through the DCache, avoiding the occupation of DCache by the output data that would affect the caching of hot data.

The performance data of the positive example operator is as follows:

| Task Duration(us) | DCache Read GM(times) | DCache Read Vector(times) | DCache Write Vector(times) |
|:-----------------:|:-----------------:|:---------------------:|:----------------------:|
|      50.895       |       3531        |         2048          |          6144          |

In terms of task duration, the optimized execution time is 50.895us, which is about 10.4% lower than the 56.82us before optimization. In terms of DCache Read GM, it is 3531 times after optimization, which is about 30.2% fewer than the 5064 times before optimization (a reduction of 1533 GM accesses). This indicates that the main bottleneck before optimization came from all data competing for DCache space, causing the hot data (the sin lookup table) to be evicted from DCache and then requiring a reload from GM. After distinguishing cache policies through memory access functions, the sin lookup table resides in DCache with priority, reducing the number of reloads from GM, improving the DCache hit rate, and improving the overall performance.

[Summary] When a SIMT operator contains data with multiple different access patterns, analyze the access characteristics of each type of data and specify an appropriate cache policy for each type through the memory access functions. Use `asc_ldcg` for data that is traversed only once to lower its cache occupancy priority. Use `asc_ldca` for hot data that is accessed frequently to keep it resident in the DCache. Use `asc_stcg` for output data that does not need to remain in the DCache after being written, to optimize cache allocation after the write. This reduces the eviction of hot data from the DCache, lowers the number of GM accesses, and improves the overall performance of the operator.
