# 约束说明<a name="ZH-CN_TOPIC_0000002507564342"></a>

- 在同一个编译单元，若存在多个核函数（Kernel），暂不支持自动推导核函数（Kernel）类型，需要开发者手动设置核函数（Kernel）类型。
    - 特别地，针对如下型号，无论是否是同一个编译单元多个核函数（Kernel）的场景，均不支持在开发者未设置核函数（Kernel）类型时进行自动推导。建议开发者手动设置核函数（Kernel）类型。
        <!-- npu="950" id1 -->
        - Ascend 950PR/Ascend 950DT
        <!-- end id1 -->
        <!-- npu="310p" id2 -->
        - Atlas 推理系列产品
        <!-- end id2 -->

- 核函数（Kernel）推荐使用 \_\_cube\_\_ / \_\_vector\_\_ / \_\_mix\_\_(cube, vec)进行标记，KERNEL\_TASK\_TYPE\_DEFAULT接口即将废弃。

- 纯Scalar算子无法实现自动推导
    需手动标记核函数（Kernel）类型，推荐设置为纯Vector类型，添加\_\_vector\_\_ attribute进行标记：

    ```cpp
    __global__ __vector__ __aicore__ void func0(__gm__ uint8_t* Addr) {
        Addr[1] = Addr[0];
        AscendC::printf("Hello world");
    }
    ```

- Host端支持Ascend C数据类型情况如下：
  - 支持直接声明或使用的数据类型。
    |  类别   |  数据类型  | 支持情况 |
    |  ----  | ----  | ----  |
    | 基本数据类型  | int4b_t、bool、int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float、int64_t、uint64_t、double。 | 支持声明和使用 |
    | 基本数据类型 |int4x2_t。| 仅支持声明 |
    | 向量数据类型  | vector_bool、vector_int8_t、vector_uint8_t、vector_int16_t、vector_uint16_t、vector_half、vector_int32_t、vector_uint32_t、vector_float、vector_int64_t、vector_uint64_t、addr_reg、vector_store_unalign、vector_load_unalign。| 支持声明和使用 |
    | 复数 | complex32、complex64。| 仅支持声明 |
    
    使用示例如下：
      ```cpp
      // 全局
      bool flag = true;
      vector_bool mask = {true, false};
      complex32 i; // 仅支持声明

      //函数内
      void f()
      {
        int8_t x = 0;
        vector_int8_t y = {0, 1};
        int8_t y1 = y[0] + 1; // y1 == 1
      }
      ```

  - 支持以指针方式声明的数据类型。
    |  类别  |  数据类型  |
    |  ----  | ----  |
    | 扩展数据类型  | fp4x2_e2m1_t、fp4x2_e1m2_t、hifloat8_t、fp8_e4m3fn_t、fp8_e5m2_t、fp8_e8m0_t、bfloat16_t。 |
    | 扩展向量数据类型  | vector_hifloat8_t、vector_fp8_e4m3fn_t、vector_fp8_e5m2_t、vector_fp8_e8m0_t、vector_bfloat16_t。|

    使用示例如下：
      ```cpp
      // 全局声明
      hifloat8_t *flag = nullptr;
      vector_hifloat8_t *mask = nullptr;

      //函数内声明
      void f()
      {
        fp8_e4m3fn_t *x = nullptr;
        vector_fp8_e5m2_t *y = nullptr;
      }
      ```

- 暂不支持在函数空间外定义和使用device侧的字符串字面量。例如：

    ```cpp
    // 全局空间
    const __gm__ char* g_str = "g_string"; // 不支持

    __aicore__ void func() // device侧函数
    {
        const __gm__ char* str = "device_string"; // 支持
    }
    ```
