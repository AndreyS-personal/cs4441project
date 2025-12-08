#!/bin/bash

repeat_num=3
db_names=("bwtree")

trap 'kill $(jobs -p)' SIGINT

if [ $# -ne 1 ]; then
  echo "Usage: $0 [dir of workload specs]"
  exit 1
fi

workload_dir=$1

# Output CSV header
echo "workload,db,threads,throughput" > results.csv

for file_name in $workload_dir/workload*.spec; do
    workload=$(basename "$file_name")
    # cant do range scans
    if [[ "$workload" == "workloade.spec" || "$workload" == "workloadf.spec" ]]; then
        echo "Skipping $workload"
        continue
    fi

    for ((tn=1; tn<=32; tn=tn*2)); do
        for db_name in "${db_names[@]}"; do
        for ((i=1; i<=repeat_num; i++)); do

            echo "Running $db_name with $tn threads for $workload"
            # Skip unwanted workloads

            # Capture output (stdout and stderr for some reason)
            output=$(./ycsbc -db "$db_name" -threads "$tn" -P "$file_name" 2>&1)

            throughput=$(echo "$output" | awk -v db="$db_name" '$1==db {print $NF; exit}')

            # Fallback in case parsing fails
            if [[ -z "$throughput" ]]; then
            throughput=0
            fi

            echo "$workload,$db_name,$tn,$throughput" >> results.csv

        done
        done
    done
done
