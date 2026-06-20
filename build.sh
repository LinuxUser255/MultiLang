#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${BUILD_DIR:-${ROOT}/build}"
BUILD_TYPE="${BUILD_TYPE:-Debug}"
CMAKE_MIN_VERSION="3.28"
JOBS="${JOBS:-$(nproc 2>/dev/null || echo 4)}"

resolve_cmake() {
    local candidates=()

    if [[ -n "${CMAKE:-}" ]]; then
        candidates+=("${CMAKE}")
    fi
    if [[ -x "${HOME}/.local/cmake/bin/cmake" ]]; then
        candidates+=("${HOME}/.local/cmake/bin/cmake")
    fi
    if command -v cmake >/dev/null 2>&1; then
        candidates+=("$(command -v cmake)")
    fi

    local cmake_bin version min
    min="$(printf '%s\n' "${CMAKE_MIN_VERSION}.0" | cut -d. -f1-3)"

    for cmake_bin in "${candidates[@]}"; do
        [[ -x "${cmake_bin}" ]] || continue
        version="$("${cmake_bin}" --version | head -1 | grep -oE '[0-9]+\.[0-9]+\.[0-9]+')"
        if [[ "$(printf '%s\n' "${min}" "${version}" | sort -V | head -1)" == "${min}" ]]; then
            echo "${cmake_bin}"
            return 0
        fi
    done

    echo "CMake ${CMAKE_MIN_VERSION}+ is required." >&2
    echo "Run: ${ROOT}/scripts/install-cmake.sh" >&2
    echo "Then: CMAKE=\$HOME/.local/cmake/bin/cmake ${ROOT}/build.sh" >&2
    return 1
}

CMAKE_BIN="$(resolve_cmake)"

echo "Using ${CMAKE_BIN}"
"${CMAKE_BIN}" -S "${ROOT}" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
"${CMAKE_BIN}" --build "${BUILD_DIR}" --target MultiLang -j "${JOBS}"

echo "Built: ${BUILD_DIR}/MultiLang"
