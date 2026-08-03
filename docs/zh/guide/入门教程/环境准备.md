# 环境准备<a name="ZH-CN_TOPIC_0000002532541127"></a>

-   进行Ascend C算子开发之前，需要安装**驱动固件**和**CANN软件包**，请参考[《CANN快速安装》](https://www.hiascend.com/cann/download)完成环境准备。


    > [!NOTE]说明
    > 安装CANN软件包后，使用CANN运行用户进行编译、运行时，需要以CANN运行用户登录环境，执行`source ${INSTALL_DIR}/set_env.sh`命令设置环境变量。`${INSTALL_DIR}`请替换为CANN软件安装后的文件存储路径。以root用户安装为例，安装后文件默认存储路径为：/usr/local/Ascend/cann。

-   安装CMake。通过CMake编译Ascend C算子时，要求安装3.16及以上版本的CMake，如果版本不符合要求，可以参考如下示例安装满足要求的版本。

    示例：安装3.16.0版本的CMake（x86\_64架构）。

    ```
    mkdir -p cmake-3.16 && wget -qO- "https://cmake.org/files/v3.16/cmake-3.16.0-Linux-x86_64.tar.gz" | tar --strip-components=1 -xz -C cmake-3.16
    export PATH=`pwd`/cmake-3.16/bin:$PATH
    ```

>[!NOTE]说明 
>对于Ascend C算子的开发，并非必须安装驱动固件。在非昇腾设备上，可以利用CPU仿真环境先行进行算子开发和测试，并在准备就绪后，利用昇腾设备进行加速计算。非昇腾设备的安装请参考 [《CANN安装指南》](https://www.hiascend.com/document/redirect/CannCommunityInstSoftware)中“附录B：常用操作 \> 在非昇腾设备上安装CANN”章节。
