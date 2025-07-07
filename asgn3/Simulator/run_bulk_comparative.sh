#!/bin/bash

SIMULATOR="./Simulator"
MAP_FOLDERS=(../maps ../more_maps)
GAME_MANAGERS_FOLDER="../SharedManagers"
THREAD_COUNTS=(1 2 4 8 16 30 64 128)

MY_ALG="../Algorithm/Algorithm_322213836_212054837.so"
SHARED_ALGS_FOLDER="../SharedAlgs"

# List of all algorithms
ALL_ALGS=(
    $MY_ALG
    $SHARED_ALGS_FOLDER/Algorithm_322996059_211779582.so
    $SHARED_ALGS_FOLDER/AlgorithmFake.so
    $SHARED_ALGS_FOLDER/AlwaysForward.so
    $SHARED_ALGS_FOLDER/LookPause.so
    $SHARED_ALGS_FOLDER/SmartEvader.so
    $SHARED_ALGS_FOLDER/SmartHunt.so
    $SHARED_ALGS_FOLDER/SmartHybridAlg.so
    $SHARED_ALGS_FOLDER/SpinShooter.so
)

for MAP_FOLDER in "${MAP_FOLDERS[@]}"; do
  for MAP_PATH in "$MAP_FOLDER"/*.txt; do
    [ -e "$MAP_PATH" ] || continue  # skip if no txt files in folder
    for NUM_THREADS in "${THREAD_COUNTS[@]}"; do
      for ((i=0; i<${#ALL_ALGS[@]}; ++i)); do
        for ((j=0; j<${#ALL_ALGS[@]}; ++j)); do
          if [ $i -ne $j ]; then
            ALG1="${ALL_ALGS[$i]}"
            ALG2="${ALL_ALGS[$j]}"
            CMD="$SIMULATOR -comparative game_map=$MAP_PATH game_managers_folder=$GAME_MANAGERS_FOLDER algorithm1=$ALG1 algorithm2=$ALG2 num_threads=$NUM_THREADS"
            echo "Running: $CMD"
            $CMD > /dev/null
            # Uncomment to save outputs:
            # $CMD > "comparative_$(basename "$MAP_PATH")_$(basename "$ALG1")_$(basename "$ALG2")_threads${NUM_THREADS}.txt"
          fi
        done
      done
    done
  done
done
