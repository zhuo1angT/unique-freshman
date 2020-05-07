# 网络代理

你的任务是实现一个代理软件，使得它可以通过GFW的封锁。

时间为两周。

## 背景知识

- [防火长城 - 维基百科](https://zh.wikipedia.org/wiki/防火长城)

## 任务要求

### 前置知识

你首先需要考虑以下问题：

- GFW是怎么知道你访问的网站的
- GFW有哪些常用的阻断连接的手段
- 为什么现在无法使用狭义的“VPN”来翻墙
- 为什么现在无法使用hosts来翻墙

### 代码

#### 基本要求

你的代理软件应该分为服务端和客户端。服务端运行在你的VPS上，而客户端运行在你的电脑上。

客户端应当监听一个TCP端口，使得其它需要使用代理的软件可以通过 `socks5://127.0.0.1:port` 的形式使用你的代理。

客户端到服务端的传输过程应当经过加密和混淆，从而防止被审查。

你的程序应当正确使用和回收各种系统资源。

你的程序应当是多线程/进程的。

你的代理软件应当可以正常使用。

使用你喜欢的编程语言。

#### 高级特性

高级特性是选做的。实现一种即可。

- 通过使用相关密码学库，建立合理的加密体系，从而支持服务端与客户端的双向认证
- 通过合理的解耦合，实现多种可选的传输层协议，如（TCP QUIC 等）
- 抵抗[重放攻击](https://github.com/shadowsocks/shadowsocks-org/issues/44)
- 实现流量伪装

## 有用的提示

- [gwuhaolin](https://github.com/gwuhaolin)/**[lightsocks](https://github.com/gwuhaolin/lightsocks)** 值得参考的实现思路
- [RFC1928 - SOCKS Protocol Version 5](https://tools.ietf.org/html/rfc1928)
- go 语言可能会很适合编写本任务
- ***一定不要公开你的仓库链接，请使用私有仓库进行开发***

## 拓展阅读

- [Shadowsocks 是如何被检测和封锁的](https://gfw.report/blog/gfw_shadowsocks/zh.html)
- [rickyzhang82](https://github.com/rickyzhang82)/**[V2Ray-Deep-Packet-Inspection](https://github.com/rickyzhang82/V2Ray-Deep-Packet-Inspection)**

有问题请随时与出题人联系。敏感信息请使用 Keybase/Telegram 等 IM 软件。