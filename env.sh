#!/bin/bash

export TOP_DIR=$(pwd)

if [ "$1" ];then
	cp $1 $TOP_DIR/scripts/config.sh
fi
source $TOP_DIR/scripts/config.sh

export INC_DIR=$TOP_DIR/include
export CFG_DIR=$TOP_DIR/config
export APP_DIR=$TOP_DIR/demo
export SRC_DIR=$TOP_DIR/src
export INSTALL_DIR=$TOP_DIR/install_$target

export BUILD_DIR=$TOP_DIR/build
export BIN_DIR=$BUILD_DIR/bin
export LIB_DIR=$BUILD_DIR/lib


#go env set
export GOPATH=$TOP_DIR
export GOARCH=$targetarch
export CGO_ENABLED=1

if [ $java_support == 'yes' ]; then
	export JAVA_SRC_DIR=$TOP_DIR/src/src_java
	export CLASSPATH=$LIB_DIR/device.jar:$CLASSPATH
	
	if [ $hum_sensor_support == 'yes' ]; then
		export CLASSPATH=$LIB_DIR/RSNetDevice-2.2.0.jar:$CLASSPATH
	fi
	
	if [ $rfid_support == 'yes' ]; then
		export CLASSPATH=$LIB_DIR/UhfApi.jar:$CLASSPATH
	fi
fi

# if [ $python_support == 'yes' ]; then
	# export PYTHON_SRC_DIR=$TOP_DIR/src/src_python
# fi

if [ $pkg_proto == 'protobuf' ]; then
	export PATH=/opt/environment/nanopb-0.4.3-linux-x86/generator-bin:$PATH
fi

export LD_LIBRARY_PATH=$LIB_DIR

MAKE_PARAM="-DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DSYSTEM_NAME=${target}"

if [ -n "$sw_version" ]; then
	MAKE_PARAM="${MAKE_PARAM} -DSW_VERSION=${sw_version}"
fi

if [ -n "$protocol_version" ]; then
	MAKE_PARAM="${MAKE_PARAM} -DPROTOCOL_VERSION=${protocol_version}"
fi

if [ -n "$root_dir" ]; then
	MAKE_PARAM="${MAKE_PARAM} -DCFG_DIR=${root_dir}"
fi

if [ -n "$aes_key" ]; then
	MAKE_PARAM="${MAKE_PARAM} -DDEV_AES_KEY=${aes_key}"
fi

if [ $cross_compile = 'yes' ]; then
	MAKE_PARAM="${MAKE_PARAM} -DCROSS_COMPILE=ON -DTARGET_HOST=${target}"
fi

if [ $samurai_support = 'yes' ]; then
	MAKE_PARAM="${MAKE_PARAM} -DUSE_SAMURAI_CLIENT=ON"
fi

if [ $bulletproof_support = 'yes' ]; then
	MAKE_PARAM="${MAKE_PARAM} -DBULLETPROOF_LIB=ON"
fi

if [ $java_support = 'yes' ]; then
	MAKE_PARAM="${MAKE_PARAM} -DJAVA_SUPPORT=ON"
	if [ $hum_sensor_support == 'yes' ]; then
		has_device=yes
		MAKE_PARAM="${MAKE_PARAM} -DHUMSENSOR=ON"
	else
		MAKE_PARAM="${MAKE_PARAM} -DHUMSENSOR=OFF"
	fi

	if [ $temp_sensor_support == 'yes' ]; then
		has_device=yes
		MAKE_PARAM="${MAKE_PARAM} -DTEMPSENSOR=ON"
	else
		MAKE_PARAM="${MAKE_PARAM} -DTEMPSENSOR=OFF"
	fi

	if [ $rfid_support == 'yes' ]; then
		has_device=yes
		MAKE_PARAM="${MAKE_PARAM} -DRFID=ON"
	else
		MAKE_PARAM="${MAKE_PARAM} -DRFID=OFF"
	fi
fi

if [ $plc_support = 'yes' ]; then
	has_device=yes
	MAKE_PARAM="${MAKE_PARAM} -DPLC=ON"
else
	MAKE_PARAM="${MAKE_PARAM} -DPLC=OFF"
fi

if [ $ipcamera_support = 'yes' ]; then
	has_device=yes
	MAKE_PARAM="${MAKE_PARAM} -DIPCAMERA=ON"
else
	MAKE_PARAM="${MAKE_PARAM} -DIPCAMERA=OFF"
fi

if [ $mqtt_support == 'yes' ]; then
	MAKE_PARAM="${MAKE_PARAM} -DMQTT_SUPPORT=ON"
	if [ $mqtt_tls_support == 'yes' ]; then
		MAKE_PARAM="${MAKE_PARAM} -DMQTT_TLS_SUPPORT=ON"
		MAKE_PARAM="${MAKE_PARAM} -DMQTT_TLS_AUTH=${mqtt_tls_auth}"
	fi
else
	MAKE_PARAM="${MAKE_PARAM} -DMQTT_SUPPORT=OFF"
fi

if [ $http_support == 'yes' ]; then
	MAKE_PARAM="${MAKE_PARAM} -DHTTP_SUPPORT=ON"
fi

# if [ $goahead_support = 'yes' ]; then
	# MAKE_PARAM="${MAKE_PARAM} -DGOAHEAD_SUPPORT=ON"
# fi

if [ $modbus_support = 'yes' ]; then
	MAKE_PARAM="${MAKE_PARAM} -DMODBUS_SUPPORT=ON"
fi

if [ $xls_support = 'yes' ]; then
	MAKE_PARAM="${MAKE_PARAM} -DXLS_SUPPORT=ON"
fi


MAKE_PARAM="${MAKE_PARAM} -DPKG_PROTO=${pkg_proto}"

export COMPILE_PARAM=$MAKE_PARAM

if [ $has_device = 'yes' ]; then
	export DEVICE_SUPPORT=yes
fi

if [ ! -d $BUILD_DIR ]; then
	mkdir -p $BUILD_DIR
fi





