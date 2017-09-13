OpenBRAS
====================================

### 开源的宽带接入系统. OpenBRAS利用dpdk/加速网卡做底层转发面。是一款高性能的宽带接入系统。

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
******************************************************************************************
*
*  Copyright (C) 2016-2017, Nanjing YunEX Technology Co., Ltd
*  Welcome to OpenBRAS Server
*
******************************************************************************************
```
### GUI
* OpenBRAS登UI地址:
http://ip:7722/openbras/ui/index.html

* OpenBRAS开发手册文档地址:
 http://ip:7722/openbras/docs/index.html


