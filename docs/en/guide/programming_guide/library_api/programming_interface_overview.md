# Programming API Overview<a name="ZH-CN_TOPIC_0000001976190182"></a>

<!-- md-trans-meta sourceCommit=05e70f75a396a9000a410ffbd40bee3d156ac8b4 translatedAt=2026-10-23T15:20:56.807Z pushedAt=2026-11-03T07:05:13.471Z nl=2 -->

>[!NOTE] Note 
>This chapter provides an overview of the programming APIs. For details about specific APIs, see [Ascend C API](../../../api/api_list.md).

Ascend C provides a set of library APIs. Developers use standard C++ syntax and library APIs for programming. The following figure shows the Ascend C programming library APIs, which are classified into:

-   **Language extension layer C API**: Opens up the complete chip programming capabilities, supports array memory allocation, is generally pointer-based, and provides a C language programming experience consistent with the industry.

-   **Basic API**: Abstracts hardware capabilities and opens up chip capabilities to ensure completeness and compatibility. APIs marked as Instruction Set Architecture Special Interface (ISASI) cannot guarantee cross-hardware-version compatibility.
-   **Advanced API**: Implements some common computing algorithms to improve programming and development efficiency, and usually calls multiple basic APIs for implementation. Advanced APIs include math libraries, Matmul, Softmax, and other APIs. Advanced APIs guarantee compatibility.
-   **Utils API (public helper functions)**: A rich set of general-purpose utility classes covering standard libraries, platform information acquisition, runtime compilation, and log output, helping developers efficiently implement operator development and performance optimization.

<img src="../../figures/architecture_ascendc.png" alt="Architecture diagram" width="1000px">

> [!NOTE] Note
> The header file directories of the Ascend C APIs are as follows:
> - Language extension layer C API: `${INSTALL_DIR}/asc/include/c_api`
> - Basic API: `${INSTALL_DIR}/asc/include/basic_api`
> - Advanced API: (Note that if the interfaces contained in the header files in the following directories are not declared in the documentation, they are indirect call interfaces and developers do not need to pay attention to them.)
>    - `${INSTALL_DIR}/asc/include/adv_api`
>    - `${INSTALL_DIR}/asc/include/tiling`
> - Utils API: `${INSTALL_DIR}/asc/include/utils`
> Replace `${INSTALL_DIR}` with the file storage path after CANN software installation. For example, when installed as the root user, the default file storage path after installation is /usr/local/Ascend/cann.
> 
> The library files that the Ascend C APIs depend on are described as follows:
> - Language extension layer C API: not involved 
> - Basic API: not involved
> - Advanced API: because the Advanced API is used together with the Host Tiling interface, it needs to be linked to libtiling\_api.a.
> - Utils API: not involved
