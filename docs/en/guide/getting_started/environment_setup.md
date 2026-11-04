# Environment Setup<a name="ZH-CN_TOPIC_0000002532541127"></a>

<!-- md-trans-meta sourceCommit=f83e857605e3844db8b9de0442007cc115278a47 translatedAt=2026-10-23T11:08:02.338Z pushedAt=2026-11-03T07:05:13.256Z nl=2 -->

-   Before developing Ascend C operators, you need to install the **driver and firmware** and the **CANN software package**. For details, see the [CANN quick installation guide](https://www.hiascend.com/cann/download).


    > [!NOTE] Note
    > After installing the CANN software package, when you use the CANN running user to compile and run, log in to the environment as the CANN running user and run the `source ${INSTALL_DIR}/set_env.sh` command to set environment variables. Replace `${INSTALL_DIR}` with the storage path of the installed CANN files. For example, if you install it as the **root** user, the default storage path is **/usr/local/Ascend/cann**.

-   Install CMake. To compile Ascend C operators with CMake, CMake 3.16 or later is required. If the installed version does not meet the requirement, install a compliant version by referring to the following example.

    Example: Install CMake 3.16.0 (x86\_64 architecture).

    ```
    mkdir -p cmake-3.16 && wget -qO- "https://cmake.org/files/v3.16/cmake-3.16.0-Linux-x86_64.tar.gz" | tar --strip-components=1 -xz -C cmake-3.16
    export PATH=`pwd`/cmake-3.16/bin:$PATH
    ```

> [!NOTE] Note 
> For Ascend C operator development, installing the driver and firmware is not mandatory. On non-Ascend devices, you can use the CPU simulation environment to develop and test operators first, and then use Ascend devices for accelerated computation after everything is ready. For installation on non-Ascend devices, see the "Appendix B: Common Operations \> Installing CANN on Non-Ascend Devices" section in [CANN Software Installation](https://www.hiascend.com/document/detail/en/CANNCommunityEdition/920/softwareinst/instg/instg_0000.html?OS=openEuler&InstallType=netyum).
