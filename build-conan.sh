#!/bin/bash

HOST_PROFILE="${HOST_PROFILE:-$(uname -s)-$(uname -m)-Release}"
BUILD_PROFILE_DBG="${BUILD_PROFILE:-$(uname -s)-$(uname -m)-Debug}"
BUILD_PROFILE_REL="${BUILD_PROFILE:-$(uname -s)-$(uname -m)-Release}"

function num_cpus() {
    local os=$(uname -s)
    if [[ "Darwin" == "$os"  ]]; then
	echo "$(sysctl -n hw.ncpu)"
    else
	echo "$(nproc)"
    fi
}

function target() {
    local os=$(uname -s | tr '[:upper:]' '[:lower:]')
    if [[ "darwin" == "$os" ]]; then
	echo "$os"
    elif [[ "linux" == "$os" ]]; then
	local host_platform=$(lsb_release -i | \
				  sed 's/\s*Distributor ID:\s*//g' | \
				  tr '[:upper:]' '[:lower:]'| \
				  sed 's/\s+//g')
	if [[ "ubuntu" == "$host_platform" ]]; then
	    echo "$os-$host_platform"
	else
	    echo "$os-default"
	fi
    else
	echo "linux-default"
    fi
}

function usage() {
    echo "xsd-tools conan build script"
    echo ""
    echo "    Usage"
    echo "    build-conan.sh [Debug|Release|clean]"
    echo "         builds either the debug or release configuraitons or"
    echo "         it cleans the repo."
    echo ""
    echo "    Environment Variables"
    echo "    HOST_PROFILE"
    echo "        The conan profile to be used to define the host"
    echo "        environment."
    echo "    BUILD_PROFILE"
    echo "        The conan profile to be used to define the build"
    echo "        environment."
    echo ""
    echo "    Examples"
    echo "        ./build-conan.sh debug"
    echo "        ./build-conan.sh clean"    
}

if [[ $# -eq 0 ]]; then
    usage
    exit 0
else
    while [[ $# -gt 0 ]]; do
	case "$1" in
	    Release)
		echo "Compiling Release"
		echo "    host profile : $HOST_PROFILE"
		echo "    build profile: $BUILD_PROFILE_REL"
		echo "    ncpus        : $(num_cpus)"
		echo "    target       : $(target)"
		conan install . \
		      -pr:h $HOST_PROFILE \
		      -pr:b $BUILD_PROFILE_REL
		source activate.sh
		scons conf=release target=$(target) -j$(num_cpus)
		build_error=$?
		source deactivate.sh
		exit $build_error
		;;
	    Debug)
		echo "Compiling Debug"
		echo "    host profile : $HOST_PROFILE"
		echo "    build profile: $BUILD_PROFILE_DBG"
		echo "    ncpus        : $(num_cpus)"
		echo "    target       : $(target)"
		conan install . \
		      -pr:h $HOST_PROFILE \
		      -pr:b $BUILD_PROFILE_DBG \
		      --build=outdated \
		      --build=missing
		source activate.sh
		scons conf=debug target=$(target) -j$(num_cpus)
		build_error=$?
		source deactivate.sh
		exit $build_error
		;;
	    clean)
		scons -c
		git clean -d -f
		exit 0
		;;
	    *)
		usage
		exit 0
		;;
	esac
	shift
    done
fi
