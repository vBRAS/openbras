#!/bin/bash
#
# Copyright (C) 2013-2014, Nanjing YunEx Technology Co., Ltd
#
source ./env.sh

help()
{
echo ===============================================================================
echo build usage:
echo ===============================================================================
echo "./build.sh [type]"
echo "[type]:"
echo "          help    		: help info" 
echo "          debug     		: build a debug"
echo "          release		    : build a release"
echo "          rpm 		    : build a rpm"
echo "          all 		    : build a debug and release"
echo ===============================================================================
}

# Build an RPM package
function build_rpm() {

    sudo rm -fr $OPENBRAS_RPM_ROOT

    mkdir -p $OPENBRAS_RPM_ROOT/{BUILD,RPMS,SOURCES/openbras-$OPENBRAS_RPM_VERSION/opt/yunex.com,SPECS,SRPMS}

    cp -r $OPENBRAS_STAGE $OPENBRAS_RPM_ROOT/SOURCES/openbras-$OPENBRAS_RPM_VERSION/opt/yunex.com/openbras

    cd $OPENBRAS_RPM_ROOT/SOURCES
    COPYFILE_DISABLE=1 tar zcf openbras-$OPENBRAS_RPM_VERSION.tar.gz openbras-$OPENBRAS_RPM_VERSION

    cp $CURRENT/rpm/openbras.spec $OPENBRAS_RPM_ROOT/SPECS/
    sed -i'' -E "s/@OPENBRAS_RPM_VERSION/$OPENBRAS_RPM_VERSION/g" $OPENBRAS_RPM_ROOT/SPECS/openbras.spec

    rpmbuild --define "_topdir $OPENBRAS_RPM_ROOT" -bb $OPENBRAS_RPM_ROOT/SPECS/openbras.spec

    cp $OPENBRAS_RPM_ROOT/RPMS/x86_64/openbras-$OPENBRAS_RPM_VERSION-1.x86_64.rpm $CURRENT && ls -l $CURRENT/openbras-$OPENBRAS_RPM_VERSION-1.x86_64.rpm
}

function clean_stage_dir() {
    [ -d "$OPENBRAS_RPM_ROOT" ] && rm -r $OPENBRAS_RPM_ROOT || :
}

if [ "$1" == "help" ] || [ "$1" == "" ]
then
	help
	exit 1
fi

if [ "$1" == "debug" ]
then
	make -f Makefile debug=1 optimize=0
elif [ "$1" == "release" ]
then
	make -f Makefile debug=0 optimize=1
elif [ "$1" == "rpm" ]
then
	build_rpm
elif [ "$1" == "all" ]
then
	make -f Makefile cleanall
	make -f Makefile debug=0 optimize=1
	build_rpm
	clean_stage_dir
else
	help
	exit 1
fi

