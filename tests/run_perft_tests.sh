#!/usr/bin/env bash
set -euo pipefail

declare -A expected=( [1]=20 [2]=400 [3]=8902 [4]=197281 [5]=4865609 [6]=119060324 )
for depth in 1 2 3 4 5 6; do
    output=$(./chessperft $depth)
    nodes=$(echo "$output" | grep -oP '(?<=: )\d+')
    if [[ "$nodes" != "${expected[$depth]}" ]]; then
        echo "Depth $depth: expected ${expected[$depth]}, got $nodes"
        exit 1
    fi
done
echo "All perft tests passed"