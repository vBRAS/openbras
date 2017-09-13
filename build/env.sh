#!/bin/sh
#
# Copyright (C) 2013-2014, Nanjing StarOS Technology Co., Ltd
#
CURRENT=`pwd`
export ROOTDIR=$CURRENT/../

export USER_ROOT=$CURRENT/../

export BUILD_NUMBER=1

export OPENBRAS_VERSION="1.0.0"

export OPENBRAS_STAGE_ROOT=/tmp
export OPENBRAS_RPM_ROOT=$OPENBRAS_STAGE_ROOT/rpm
export OPENBRAS_RPM_VERSION=${OPENBRAS_VERSION//-/.}
export OPENBRAS_STAGE=$CURRENT/openbras

export PLATFORM_ROOT=/opt/staros.xyz/stardlls/
export PLATFORM_INCLUDE=$PLATFORM_ROOT/include/
export PLATFORM_LIB_PATH=$PLATFORM_ROOT/libs/

export DIPC_INCLUDE=/opt/staros.xyz/dipc/include/dipc
export DIPC_LIB_PATH=/opt/staros.xyz/dipc/libs/

export PROTOCOL_INCLUDE=/opt/staros.xyz/protocol/include
export PROTOCOL_LIB_PATH=/opt/staros.xyz/protocol/libs/


export STARLANG_INCLUDE=/opt/staros.xyz/starlang/include
export STARLANG_LIB_PATH=/opt/staros.xyz/starlang/libs/
export CORE_INCLUDE=/opt/staros.xyz/starcore/include/starcore
export COREUTILS_INCLUDE=/opt/staros.xyz/starcore/include/coreutils
export CORE_LIB_PATH=/opt/staros.xyz/starcore/libs

export DPDK_INCLUDE=/opt/staros.xyz/stardlls/include/dpdk

#################PLATFORM_LIB###############
export ACE_INCLUDE=$PLATFORM_INCLUDE
export CFGPARSER_INCLUDE=$PLATFORM_INCLUDE/cfgparser
export DISPATCH_INCLUDE=$PLATFORM_INCLUDE/dispatch
export EXPAT_INCLUDE=$PLATFORM_INCLUDE/expat
export FASTDB_INCLUDE=$PLATFORM_INCLUDE/fastdb
export GLOG_INCLUDE=$PLATFORM_INCLUDE/
export JSON_INCLUDE=$PLATFORM_INCLUDE/json
export EVENT_INCLUDE=$PLATFORM_INCLUDE/event2
export SSL_INCLUDE=$PLATFORM_INCLUDE
export PROTOBUF_INCLUDE=$PLATFORM_INCLUDE/
export SCRYPTO_INCLUDE=$PLATFORM_INCLUDE/scrypto
export SHTTP_INCLUDE=$PLATFORM_INCLUDE/shttp
export SOAP_INCLUDE=$PLATFORM_INCLUDE/soap
export JS_INCLUDE=$PLATFORM_INCLUDE/js
export STARBASE_INCLUDE=$PLATFORM_INCLUDE/starbase
export TBB_INCLUDE=$PLATFORM_INCLUDE/tbb
export RTC_INCLUDE=$PLATFORM_INCLUDE/
export WFACE_INCLUDE=$PLATFORM_INCLUDE/wface
export WFDB_INCLUDE=$PLATFORM_INCLUDE/wfdb
export WOCAL_INCLUDE=$PLATFORM_INCLUDE/wocal
export ZLIB_INCLUDE=$PLATFORM_INCLUDE/zlib
export VPPINFRA_INCLUDE=$PLATFORM_INCLUDE/include
export LIBCLI_INCLUDE=$PLATFORM_INCLUDE/staroscli

export OPENBRAS_LIB_PATH=$USER_ROOT/libs
LD_LIBRARY_PATH=$PLATFORM_LIB_PATH:$OPENBRAS_LIB_PATH:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH