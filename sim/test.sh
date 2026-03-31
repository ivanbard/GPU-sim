#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cd "${script_dir}"
make

default_out="$(./sim)"
printf '%s\n' "${default_out}" | grep -q "Warps completed: 4"
printf '%s\n' "${default_out}" | grep -q "Diverged cycles: 16"

memory_out="$(./sim ../kernels/test.kern)"
printf '%s\n' "${memory_out}" | grep -q "Total cycles: 13"
printf '%s\n' "${memory_out}" | grep -q "Stall cycles: 1"
printf '%s\n' "${memory_out}" | grep -q "Instructions issued: 12"

if ./sim ../kernels/invalid_missing_operand.kern >/tmp/gpu-sim-invalid.out 2>&1; then
    echo "expected invalid kernel to fail"
    exit 1
fi

grep -q "Kernel parse error" /tmp/gpu-sim-invalid.out

echo "All regression checks passed."
