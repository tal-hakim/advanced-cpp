#!/bin/bash

# Paths
MAP_FOLDERS=(../maps ../more_maps)   # Add more as needed
GAME_MANAGERS=(../GameManager/GameManager_322213836_212054837.so ../SharedManagers/GameManager_322996059_211779582.so)
SIMULATOR="./Simulator"              # Adjust to your real executable if needed

# Thread counts to test
THREAD_COUNTS=(1 2 4 8 16 30 64 128)

for MAP_FOLDER in "${MAP_FOLDERS[@]}"; do
  for GAME_MANAGER in "${GAME_MANAGERS[@]}"; do
    for NUM_THREADS in "${THREAD_COUNTS[@]}"; do
      CMD="$SIMULATOR -competition game_maps_folder=$MAP_FOLDER game_manager=$GAME_MANAGER algorithms_folder=../SharedAlgs num_threads=$NUM_THREADS"
      echo "Running: $CMD"
      $CMD > /dev/null

      # Optionally save output:
      # $CMD > "out_${MAP_FOLDER//\//_}_${GAME_MANAGER##*/}_t${NUM_THREADS}.txt"
    done
  done
done
