#!/bin/sh

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
	local host_plaftorm=$(lsb_release -i | \
				  sed 's/Distributor ID://g' | \
				  tr '[:upper:]' '[:lower:]')
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
    echo "\tUsage"
    echo "\tbuild-conan.sh [debug|release|clean]"
    echo "\t\t builds either the debug or release configuraitons or"
    echo "\t\t it cleans the repo."
    echo ""
    echo "\tEnvironment Variables"
    echo "\tHOST_PROFILE"
    echo "\t\tThe conan profile to be used to define the host"
    echo "\t\tenvironment."
    echo "\tBUILD_PROFILE"
    echo "\t\tThe conan profile to be used to define the build"
    echo "\t\tenvironment."
    echo ""
    echo "\tExamples"
    echo "\t\t./build-conan.sh debug"
    echo "\t\t./build-conan.sh clean"    
}

if [[ $# -eq 0 ]]; then
    usage
    exit 0
else
    while [[ $# -gt 0 ]]; do
	case "$1" in
	    Release)
		echo "Compiling Release"
		echo "\thost profile : $HOST_PROFILE"
		echo "\tbuild profile: $BUILD_PROFILE_REL"
		echo "\tncpus        : $(num_cpus)"
		echo "\ttarget       : $(target)"
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
		echo "\thost profile : $HOST_PROFILE"
		echo "\tbuild profile: $BUILD_PROFILE_DBG"
		echo "\tncpus        : $(num_cpus)"
		echo "\ttarget       : $(target)"
		conan install . \
		      -pr:h $HOST_PROFILE \
		      -pr:b $BUILD_PROFILE_DBG
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
