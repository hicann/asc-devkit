# asc\_dcci\_entire

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-28T07:16:28.036Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

This API is used to flush the entire Data Cache within a core to ensure cache consistency during data reads.

In SIMT programming, written data is immediately written to Global Memory, making it visible to other cores, so there is no inter-core consistency issue. When reading data from Global Memory, the data may have been modified by other cores. In this case, use the **dcci** API to directly access Global Memory to obtain the latest data.

Note that this API is time-consuming, so **use it with caution in performance-sensitive scenarios**.

## Prototype

```
inline void asc_dcci_entire(void *dst)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Input | Address in Global Memory. |

## Return Value

None

## Constraints

The input must be a valid Global Memory address. When a non-Global Memory address or an invalid address is passed in, the Cache refresh behavior is not guaranteed to be effective.

## Required Header Files

To use this API, include the **simt_api/device_functions.h** header file.

```
#include "simt_api/device_functions.h"
```

## Example

-   SIMT programming scenario:

    ```
    __global__ __launch_bounds__(1024) void kernelDcci_entire(uint32_t* data, uint32_t* output) {
        // Thread 0 of block 0 writes data.
        if (blockIdx.x == 0 && threadIdx.x == 0) {
            data[1] = 10;      // Write data.
            asc_threadfence(); // Ensure that the data write is complete.
            data[0] = 1;       // Flag indicating that the data is ready.
        }

        // Thread 0 of block 1 reads data.
        if (blockIdx.x == 1 && threadIdx.x == 0) {
            while (data[0] != 1) {
                asc_dcci_entire((void*)(data)); // Read the flag from global memory.
            }
            output[0] = data[1]; // The data is ready; obtain the data.
        }
    }
    ```

-   SIMD and SIMT hybrid programming scenario:

    ```
    __simt_vf__ __launch_bounds__(1024) inline void kernelDcci_entire(__gm__ uint32_t* data, __gm__ uint32_t* output)
    {
        // Thread 0 of thread block 0 performs the data write operation.
        if (blockIdx.x == 0 && threadIdx.x == 0) {
            data[1] = 10;      // Write the data.
            asc_threadfence(); // Ensure that the data write is complete.
            data[0] = 1;       // Flag indicating that the data is ready.
        }

        // Thread 0 of thread block 1 performs the data read operation.
        if (blockIdx.x == 1 && threadIdx.x == 0) {
            while (data[0] != 1) {
                asc_dcci_entire((__gm__ void*)(data)); // Read the flag from Global Memory.
            }
            output[0] = data[1]; // The data is ready; obtain the data.
        }
    }
    ```
