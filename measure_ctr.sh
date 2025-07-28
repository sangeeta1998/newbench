#!/bin/bash

# Usage check
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <number_of_times>"
    exit 1
fi

n=$1
output_file="timing_results.csv"

echo "Image,Average Pull Time (s)" > $output_file

pull_image() {
    local image=$1
    local platform=$2
    local accumulated_time=0

    for ((i=1; i<=n; i++))
    do
        # Remove image if present and clear containerd cache
        sudo ctr image rm $image 2>/dev/null
        sudo ctr image prune --all

        # Pull and time
        start_time=$(date +%s.%3N)
        if [ -z "$platform" ]; then
            sudo ctr image pull $image
        else
            sudo ctr image pull --platform $platform $image
        fi
        end_time=$(date +%s.%3N)

        elapsed=$(echo "$end_time - $start_time" | bc)
        accumulated_time=$(echo "$accumulated_time + $elapsed" | bc)
    done

    avg_time=$(echo "scale=3; $accumulated_time / $n" | bc)
    echo "$image,$avg_time" >> $output_file
}

# Pull and measure
pull_image "docker.io/sangeetakakati/newbench:wasm" "wasm"
pull_image "docker.io/sangeetakakati/newbench:native" "linux/arm64"

echo "Timing results have been recorded in $output_file" 
