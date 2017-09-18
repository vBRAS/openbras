OpenBRAS
====================================

### OpenBRAS社区
开源的宽带接入系统. OpenBRAS利用dpdk/加速网卡做底层转发面。是一款高性能的宽带接入系统。
OpenBRAS社区是由来自中国电信、Intel，Inspur，Mellanox，Netronome，Panath，YunEx等公司的技术专家共同创建，旨在构建一个开放、开源的宽带接入网络架构，推动vBRAS的发展
OpenBRAS社区致力构建一个开放，开源的vBRAS系统。( vBRAS，Virtualized Broadband Remote Access Server，即虚拟化宽带远程接入服务器)
vBRAS是传统BRAS的NFV(Network Function Virtualization)实现。NFV主要的思路是让网络功能和专用硬件解耦，在COTS(Commercial Off-The-Shelf)通用硬件上实现传统网络的系列功能。
vBRAS的系统主要包括业务控制面和数据转发面。具体实现方案有：一体化vBRAS系统或转控分离的vBRAS系统。在数据转发面的处理上，或采用既有的硬件设备，或采用纯CPU转发。
OpenBRAS社区致力于促进vBRAS系统的通用化，实现业务控制面开源，数据转发面开放，并实现一个通过智能加速网卡实现数据转发面offloading的完整解决方案。。
### 开始
### 安装第三方库
```
# git clone https://github.com/starosxyz/staroscontroller
# cd staroscontroller/tools/installstaros
# chmod a+x staros-install.sh
# ./staros-install.sh install
```
### 编译
```
# git clone https://github.com/vBRAS/openbras.git
# cd openbras/build
# source env.sh
# make debug=1或者0
# ./build.sh rpm打rpm包
```

### 运行OpenBRAS
```
# rpm -ihv openbras-1.0.0-1.x86_64.rpm
# cp /opt/yunex.com/openbras/scripts/startup.xml /opt/staros.xyz/dipc/scripts/
# dipcctl start或者stop
# 启动完成
```
### GUI
* OpenBRAS登UI地址:
http://ip:7722/openbras/ui/index.html

* OpenBRAS开发手册文档地址:
http://ip:7722/openbras/docs/index.html

欢迎访问我们的[官网](http://www.openbras.org)
