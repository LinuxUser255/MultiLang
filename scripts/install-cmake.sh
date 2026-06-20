#!/usr/bin/env bash

# Install a user-local CMake release (default: 4.3.3) when the system
# package is older than the project's cmake_minimum_required (3.28).
set -euo pipefail

CMAKE_VERSION="${CMAKE_VERSION:-4.3.3}"
INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX:-${HOME}/.local/cmake}"
ARCHIVE="cmake-${CMAKE_VERSION}-linux-x86_64.tar.gz"
URL="https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/${ARCHIVE}"

if [[ -x "${INSTALL_PREFIX}/bin/cmake" ]]; then
    installed_version="$("${INSTALL_PREFIX}/bin/cmake" --version | head -1 | grep -oE '[0-9]+\.[0-9]+\.[0-9]+')"
    if [[ "$(printf '%s\n' "3.28" "${installed_version}" | sort -V | head -1)" == "3.28" ]]; then
        echo "CMake ${installed_version} already installed at ${INSTALL_PREFIX}/bin/cmake"
        exit 0
    fi
fi

tmp_dir="$(mktemp -d)"
trap 'rm -rf "${tmp_dir}"' EXIT

echo "Downloading CMake ${CMAKE_VERSION}..."
curl -fsSL -o "${tmp_dir}/${ARCHIVE}" "${URL}"

mkdir -p "${INSTALL_PREFIX}"
tar -xzf "${tmp_dir}/${ARCHIVE}" -C "${INSTALL_PREFIX}" --strip-components=1

echo "Installed CMake $("${INSTALL_PREFIX}/bin/cmake" --version | head -1)"
echo "Use: CMAKE=${INSTALL_PREFIX}/bin/cmake ./build.sh"
echo "Or add ${INSTALL_PREFIX}/bin to your PATH."
