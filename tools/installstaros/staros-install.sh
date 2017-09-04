#!/bin/bash

if [ "$(id -u)" != "0" ]; then
   echo "You need to be 'root' dude." 1>&2
   exit 1
fi
source ./version.sh
help()
{
echo ===============================================================================
echo StarOS installer usage:
echo ===============================================================================
echo "./staros-install.sh [type]"
echo "[type]:"
echo "          install    		: Install StarOS Platform" 
echo "          uninstall       : Uninstall StarOS Platform"
echo ===============================================================================
}

if [ "$1" == "help" ] || [ "$1" == "" ]
then
	help
	exit 1
fi

function install_rpm() {
	#check stardlls
	if [ -d /opt/staros.xyz/stardlls ];then
		echo Stardlls check!
	else
		if [ -f ../package/centos/stardlls-$STARDLLS_VERSION-1.x86_64.rpm ];then
			rpm -ihv ../package/centos/stardlls-$STARDLLS_VERSION-1.x86_64.rpm --nodeps
		else
			wget -P ../package/centos/ https://github.com/starosxyz/stardlls/raw/master/stardlls-$STARDLLS_VERSION-1.x86_64.rpm
			rpm -ihv ../package/centos/stardlls-$STARDLLS_VERSION-1.x86_64.rpm --nodeps
		fi
	fi
	if [ -d /opt/staros.xyz/protocol ];then
		echo protocol check!
	else
		if [ -f ../package/centos/protocol-$PROTOCOL_VERSION-1.x86_64.rpm ];then
			rpm -ihv ../package/centos/protocol-$PROTOCOL_VERSION-1.x86_64.rpm --nodeps
		else
			wget -P ../package/centos/ https://github.com/starosxyz/protocol/raw/master/protocol-$PROTOCOL_VERSION-1.x86_64.rpm
			rpm -ihv ../package/centos/protocol-$PROTOCOL_VERSION-1.x86_64.rpm --nodeps
		fi
	fi
	if [ -d /opt/staros.xyz/starlang ];then
		echo starlang check!
	else
		if [ -f ../package/centos/starlang-$STARLANG_VERSION-1.x86_64.rpm ];then
			rpm -ihv ../package/centos/starlang-$STARLANG_VERSION-1.x86_64.rpm --nodeps
		else
			wget -P ../package/centos/ https://github.com/starosxyz/starlang/raw/master/starlang-$STARLANG_VERSION-1.x86_64.rpm
			rpm -ihv ../package/centos/starlang-$STARLANG_VERSION-1.x86_64.rpm --nodeps
		fi
	fi
	#check dipc
	if [ -d /opt/staros.xyz/dipc ];then
		echo DIPC check!
	else
		if [ -f ../package/centos/dipc-$DIPC_VERSION-1.x86_64.rpm ];then
			rpm -ihv ../package/centos/dipc-$DIPC_VERSION-1.x86_64.rpm --nodeps
		else
			wget -P ../package/centos/ https://github.com/starosxyz/DIPC/raw/master/dipc-$DIPC_VERSION-1.x86_64.rpm
			rpm -ihv ../package/centos/dipc-$DIPC_VERSION-1.x86_64.rpm --nodeps
		fi
	fi
	#check core
	if [ -d /opt/staros.xyz/starcore ];then
		echo StarCore check!
	else
		if [ -f ../package/centos/starcore-$STARCORE_VERSION-1.x86_64.rpm ];then
			rpm -ihv ../package/centos/starcore-$STARCORE_VERSION-1.x86_64.rpm --nodeps
		else
			wget -P ../package/centos/ https://github.com/starosxyz/starcore/raw/master/starcore-$STARCORE_VERSION-1.x86_64.rpm
			rpm -ihv ../package/centos/starcore-$STARCORE_VERSION-1.x86_64.rpm --nodeps
		fi
	fi
	
	#check staros
	if [ -d /opt/staros.xyz/staros ];then
		echo StarOS check!
	else
		if [ -f ../package/centos/staros-$STAROS_VERSION-1.x86_64.rpm ];then
			rpm -ihv ../package/centos/staros-$STAROS_VERSION-1.x86_64.rpm --nodeps
		else
			wget -P ../package/centos/ https://github.com/starosxyz/staros/raw/master/staros-$STAROS_VERSION-1.x86_64.rpm
			rpm -ihv ../package/centos/staros-$STAROS_VERSION-1.x86_64.rpm --nodeps
		fi
	fi
}

function get_rpm() {
	#check stardlls
	if [ -f ../package/centos/stardlls-$STARDLLS_VERSION-1.x86_64.rpm ];then
		echo "check"
	else
		wget -P ../package/centos/ https://github.com/starosxyz/stardlls/raw/master/stardlls-$STARDLLS_VERSION-1.x86_64.rpm
	fi
	if [ -f ../package/centos/protocol-$PROTOCOL_VERSION-1.x86_64.rpm ];then
		echo "check"
	else
		wget -P ../package/centos/ https://github.com/starosxyz/protocol/raw/master/protocol-$PROTOCOL_VERSION-1.x86_64.rpm
	fi
	if [ -f ../package/centos/starlang-$STARLANG_VERSION-1.x86_64.rpm ];then
		echo "check"
	else
		wget -P ../package/centos/ https://github.com/starosxyz/starlang/raw/master/starlang-$STARLANG_VERSION-1.x86_64.rpm
	fi
	#check dipc
	if [ -f ../package/centos/dipc-$DIPC_VERSION-1.x86_64.rpm ];then
		echo "check"
	else
		wget -P ../package/centos/ https://github.com/starosxyz/DIPC/raw/master/dipc-$DIPC_VERSION-1.x86_64.rpm
	fi
	#check core
	if [ -f ../package/centos/starcore-$STARCORE_VERSION-1.x86_64.rpm ];then
		echo "check"
	else
		wget -P ../package/centos/ https://github.com/starosxyz/starcore/raw/master/starcore-$STARCORE_VERSION-1.x86_64.rpm
	fi
	#check staros
	if [ -f ../package/centos/staros-$STAROS_VERSION-1.x86_64.rpm ];then
		echo "check"
	else
		wget -P ../package/centos/ https://github.com/starosxyz/staros/raw/master/staros-$STAROS_VERSION-1.x86_64.rpm
	fi
	cp /opt/staros.xyz/staros/scripts/startup.xml /opt/staros.xyz/dipc/scripts/
}

function uninstall_rpm() {
	echo uninstall platform will cause application not avaliable, are you sure want uninstall platform?
	read -s -n1 -p "Press any key to continue ..."
	echo "\n"
	rpm -e staros --nodeps
	rpm -e starcore --nodeps
	rpm -e dipc --nodeps
	rpm -e starlang --nodeps
	rpm -e protocol --nodeps
	rpm -e stardlls --nodeps
}

if [ "$1" == "get" ]
then
	get_rpm
elif [ "$1" == "install" ]
then
	install_rpm
elif [ "$1" == "uninstall" ]
then
	uninstall_rpm
else
	help
	exit 1
fi
