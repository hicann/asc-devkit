# 约束说明<a name="ZH-CN_TOPIC_0000002507564342"></a>

- 在同一个编译单元，若存在多个核函数，暂不支持自动推导Kernel类型，需要开发者手动设置Kernel类型。
    - 特别地，针对如下型号，无论是否是同一个编译单元多个核函数的场景，均不支持在开发者未设置Kernel类型时进行自动推导。建议开发者手动设置Kernel类型。
        <!-- npu="950" id1 -->
        - Ascend 950PR/Ascend 950DT
        <!-- end id1 -->
        <!-- npu="310p" id2 -->
        - Atlas 推理系列产品
        <!-- end id2 -->

- Kernel函数推荐使用 \_\_cube\_\_ / \_\_vector\_\_ / \_\_mix\_\_(cube, vec)进行标记，KERNEL\_TASK\_TYPE\_DEFAULT接口即将废弃。

- 纯Scalar算子无法实现自动推导
    需手动标记Kernel函数类型，推荐设置为纯Vector类型，添加\_\_vector\_\_ attribute进行标记：

    ```cpp
    __global__ __vector__ __aicore__ void func0(__gm__ uint8_t* Addr) {
        Addr[1] = Addr[0];
        AscendC::printf("Hello world");
    }
    ```

- bfloat16\_t等数据类型在Host端仅支持以C++模板的形式进行定义与声明，具体数据类型如下：

    <!-- npu="950" id3 -->
    **Ascend 950PR/Ascend 950DT**：bfloat16\_t、hifloat8\_t、fp8\_e5m2\_t、fp8\_e4m3fn\_t、fp8\_e8m0\_t、fp4x2\_e2m1\_t、fp4x2\_e1m2\_t、int4x2\_t。
    <!-- end id3 -->

    <!-- npu="910b" id4 -->
    **Atlas A2 训练系列产品/Atlas A2 推理系列产品**：bfloat16\_t。
    <!-- end id4 -->

    <!-- npu="A3" id5 -->
    **Atlas A3 训练系列产品/Atlas A3 推理系列产品**：bfloat16\_t。
    <!-- end id5 -->

- 暂不支持在函数空间外定义和使用device侧的字符串字面量。例如：

    ```cpp
    // 全局空间
    const __gm__ char* g_str = "g_string"; // 不支持

    __aicore__ void func() // device侧函数
    {
        const __gm__ char* str = "device_string"; // 支持
    }
    ```
