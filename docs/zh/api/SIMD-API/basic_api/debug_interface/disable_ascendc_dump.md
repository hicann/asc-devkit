# 关闭ASCENDC_DUMP说明

## 特性说明

调试接口（如本章的上板打印、异常检测接口）开启后会对算子性能产生一定影响，通常在调测阶段使用，生产环境建议关闭。ASCENDC_DUMP宏定义是控制调试接口（）打印的开关。

## 使用示例

- CMake编译选项控制方式
    - Kernel直调工程
        修改CMakeList.txt文件中中ascendc_compile_definitions命令增加ASCENDC_DUMP=0宏定义来关闭ASCENDC_DUMP开关。示例如下：
        ```plain
        // 关闭所有算子的打印功能
        ascendc_compile_definitions({kernel_target_name} PRIVATE
            ASCENDC_DUMP = 0
        )
        ```

    - 自定义算子工程
        修改算子工程op_kernel目录下的CMakeLists.txt文件中的add_ops_compile_options或npu_op_kernel_options，首行增加编译选项-DASCENDC_DUMP=0，关闭ASCENDC_DUMP开关，示例如下：
        ```plain
        // 使用add_ops_compile_options关闭所有算子的打印功能
        ascendc_compile_definitions(ALL OPTIONS -DASCENDC_DUMP=0)
        // 使用npu_op_kernel_options关闭所有算子的打印功能
        ascendc_compile_definitions(ascendc_kernels ALL OPTIONS -DASENDC_DUMP=0)
        ```

    - 宏定义控制方式
        在kernel代码文件的开头位置添加#define ASCENDC_DUMP 0,示例如下：
        ```cpp
        #define ASCENDC_DUMP 0 // 在包含头文件之前添加
        #include "..." // 包含所需的头文件
        ```
