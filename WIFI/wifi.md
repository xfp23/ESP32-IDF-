**本频道只提供idf编程方法，不提供知识点注解**

# 版权信息

 ©  2024 . 未经许可不得复制、修改或分发。 此文献为 [小風的藏書閣](https://t.me/xfp2333)  所有。

# WIFI编程指南

# 简介

- ESP32-S3 支持以下 Wi-Fi 功能：

1. 支持 4 个虚拟接口，即 STA、AP、Sniffer 和 reserved

2. 支持仅 station 模式、仅 AP 模式、station/AP 共存模式

3. 支持使用 IEEE 802.11b、IEEE 802.11g、IEEE 802.11n 和 API 配置协议模式

4. 支持 WPA/WPA2/WPA3/WPA2-企业版/WPA3-企业版/WAPI/WPS 和 DPP

5. 支持 AMSDU、AMPDU、HT40、QoS 以及其它主要功能

6. 支持 Modem-sleep

7. 支持乐鑫专属协议，可实现 1 km 数据通信量

8. 空中数据传输最高可达 20 MBit/s TCP 吞吐量和 30 MBit/s UDP 吞吐量

9. 支持 Sniffer

10. 支持快速扫描和全信道扫描

11. 支持多个天线

12. 支持获取信道状态信息


# wifi STA 模式

- [STA模式编程指南](wifiSTA.md)
- [AP模式编程指南](wifiAP.md)

# 程序示例

- [WIFI STA示例](wifista.c)

- [WIFI AP示例](wifiap.c)

- [WIFI STA+AP示例](wifiapsta.c)  **混合模式**