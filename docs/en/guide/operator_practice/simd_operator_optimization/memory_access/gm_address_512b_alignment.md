# Aligning the GM Address to 512 Bytes Wherever Possible<a name="ZH-CN_TOPIC_0000001892919369"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T14:22:32.451Z -->

[Priority] High

[Description] Due to the internal design constraints of the AI processor, when data is moved from Global Memory to Local Memory, ensuring that the GM address is 512-byte aligned can maximize bandwidth efficiency. The following figures show the single-core bandwidth efficiency under 512-byte alignment and 32-byte alignment: when moving the same amount of data, in the case of the largest bandwidth difference, the 32-byte alignment scenario can only achieve 70% of the bandwidth of the 512-byte alignment scenario.

>[!NOTE] Note
>- This performance optimization method takes effect only on Atlas A2 training products/Atlas A2 inference products.
>- The test data is related to the processor model, and slight jitter may occur during actual testing. The specific bandwidth values may not be strictly consistent with the test data below.

**Figure 1**  Comparison of the measured bandwidth difference between 512-byte alignment and 32-byte alignment in the GM-\>UB direction<a name="fig5376636154310"></a>  
![](../../../figures/gm_ub_direction_512b_alignment_and_32b_alignment_measured_bandwidth_difference.png "GM--UB direction 512-byte alignment and 32-byte alignment measured bandwidth difference")

**Figure 2**  Comparison of the measured bandwidth difference between 512-byte alignment and 32-byte alignment in the UB-\>GM direction<a name="fig13744224715"></a>  
![](../../../figures/ub_gm_direction_512b_alignment_and_32b_alignment_measured_bandwidth_difference.png "UB--GM direction 512-byte alignment and 32-byte alignment measured bandwidth difference")

