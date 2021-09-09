#!/bin/sh

DBG_PROFILE="${DEBUG_PROFILE:-$(uname -s)-$(uname -m)-Debug}"
REL_PROFILE="${RELEASE_PROFILE:-$(uname -s)-$(uname -m)-Release}"

function num_cpus() {
    local os=$(uname -s)
    if [[ "Darwin" == "$os"  ]]; then
	echo "$(sysctl -n hw.ncpu)"
    else
	echo "$(nproc)"
    fi
}

function usage() {
    echo "xsd-tools conan build script"
    echo ""
    echo "Usage"
    echo "build-conan.sh [debug|release|clean]"
    echo "\t\t builds either the debug or release configuraitons or"
    echo "\t\t it cleans the repo."
    echo ""
    echo "Examples"
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
		echo "\tprofile: $DBG_PROFILE"
		echo "\tncpus  : $(num_cpus)"
		conan install . \
		      -pr:h $REL_PROFILE \
		      -pr:b $REL_PROFILE
		source activate.sh
		scons conf=release -j$(num_cpus)
		build_error=$?
		source deactivate.sh
		exit $build_error
		;;
	    Debug)
		echo "Compiling Debug"
		echo "\tprofile: $DBG_PROFILE"
		echo "\tncpus  : $(num_cpus)"
		conan install . \
		      -pr:h $REL_PROFILE \
		      -pr:b $DBG_PROFILE
		source activate.sh
		scons conf=debug -j$(num_cpus)
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
