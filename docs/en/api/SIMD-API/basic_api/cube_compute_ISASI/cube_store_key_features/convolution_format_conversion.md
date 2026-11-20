# Convolution Format Conversion

<!-- md-trans-meta sourceCommit=dacba287b3c94417af1e532d9cbd29c398889dea translatedAt=2026-11-02T04:23:02.530Z pushedAt=2026-11-18T01:25:06.782Z nl=2 -->

In convolution operations, the data stored in the L0C Buffer is in the NC1HWC0 format. Through the [NZ2ND](NZ2ND.md) capability, the Fixpipe instruction supports converting the data in the L0C Buffer to the NHWC format for output. The conversion diagram is as follows:

**Figure 1** NC1HWC0 converted to NHWC format<a name="zh-cn_topic_0000002515830464_fig63620361414"></a>  

![](../../../../figures/Fixpipe_NC1HWC0_NHWC.png)

<!-- npu="950" id1 -->
In particular, for the Ascend 950PR&950DT products, the Fixpipe instruction also supports converting the data in the L0C Buffer to the NCHW format for output through the [NZ2DN](NZ2DN.md) capability. The conversion diagram is as follows:

**Figure 2**  NC1HWC0 converted to NCHW format<a name="fig188671630134414"></a>  

![](../../../../figures/Fixpipe_NC1HWC0_NCHW.png)
<!-- end id1 -->
