# Constraints<a name="ZH-CN_TOPIC_0000002507564342"></a>

<!-- md-trans-meta sourceCommit=3c788322939ff72c11a21859147c7a4b4b0d42d9 translatedAt=2026-11-09T10:59:17.811Z pushedAt=2026-11-19T06:32:19.934Z nl=2 -->

- In the same compilation unit, if multiple kernels exist, automatic inference of the kernel type is not supported. You need to manually set the kernel type.
    - In particular, for the following models, automatic inference is not supported when the kernel type is not set by the developer, regardless of whether multiple kernels exist in the same compilation unit. You are advised to manually set the kernel type.
        <!-- npu="950" id1 -->
        - Ascend 950PR&950DT products
        <!-- end id1 -->
        <!-- npu="310p" id2 -->
        - Atlas inference products
        <!-- end id2 -->

- The kernel is marked with \_\_cube\_\_ / \_\_vector\_\_ / \_\_mix\_\_(cube, vec).

- Automatic inference cannot be implemented for scalar-only operators.
    You need to manually mark the kernel type. You are advised to set it to the vector-only type and add the **\_\_vector\_\_** attribute for marking:

    ```cpp
    __global__ __vector__ __aicore__ void func0(__gm__ uint8_t* Addr) {
        Addr[1] = Addr[0];
        AscendC::printf("Hello world");
    }
    ```

- The Ascend C data types supported on the host side are as follows:
  - Data types that support direct declaration or use.
    |  Category  |  Data Type  | Support Status |
    |  ----  | ----  | ----  |
    | Basic data type  | int4b_t, bool, int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float, int64_t, uint64_t, double. | Support declaration and use |
    | Basic data type |int4x2_t.| Support declaration only |
    | Vector data type  | vector_bool, vector_int8_t, vector_uint8_t, vector_int16_t, vector_uint16_t, vector_half, vector_int32_t, vector_uint32_t, vector_float, vector_int64_t, vector_uint64_t, addr_reg, vector_store_unalign, vector_load_unalign.| Support declaration and use |
    | Complex | complex32, complex64.| Support declaration only |
    
    The following is an example:

      ```cpp
      // Global
      bool flag = true;
      vector_bool mask = {true, false};
      complex32 i; // Support declaration only

      // Inside a function
      void f()
      {
        int8_t x = 0;
        vector_int8_t y = {0, 1};
        int8_t y1 = y[0] + 1; // y1 == 1
      }
      ```

  - Data types that support declaration as pointers.
    |  Category  |  Data Type  |
    |  ----  | ----  |
    | Extended data type  | fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e4m3fn_t, fp8_e5m2_t, fp8_e8m0_t, bfloat16_t. |
    | Extended vector data type  | vector_hifloat8_t, vector_fp8_e4m3fn_t, vector_fp8_e5m2_t, vector_fp8_e8m0_t, vector_bfloat16_t.|

    The following is a usage example:

      ```cpp
      // Global declaration
      hifloat8_t *flag = nullptr;
      vector_hifloat8_t *mask = nullptr;

      // Declaration inside a function
      void f()
      {
        fp8_e4m3fn_t *x = nullptr;
        vector_fp8_e5m2_t *y = nullptr;
      }
      ```

- The use of templates to pass `extended vector data type`, such as `vector_fp8_e4m3fn_t`, is not supported yet on the host side.

- Defining and using device-side string literals outside the function scope is not supported. For example:

    ```cpp
    // Global space
    const __gm__ char* g_str = "g_string"; // Not supported

    __aicore__ void func() // Device-side function
    {
        const __gm__ char* str = "device_string"; // Supported
    }
    ```
