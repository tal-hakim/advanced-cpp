#!/bin/bash

SIMULATOR="./Simulator"
MAP_FOLDERS=(../maps ../more_maps)
GAME_MANAGERS_FOLDER="../GameManager"
NUM_THREADS=8  # or any number you prefer

MY_ALG="../Algorithm/Algorithm_322213836_212054837.so"
SHARED_ALGS_FOLDER="../SharedAlgs"

SHARED_ALGS=(
    Algorithm_322996059_211779582.so
    AlgorithmFake.so
    AlwaysForward.so
    LookPause.so
    SmartEvader.so
    SmartHunt.so
    SmartHybridAlg.so
    SpinShooter.so
)

for MAP_FOLDER in "${MAP_FOLDERS[@]}"; do
  for MAP_PATH in "$MAP_FOLDER"/*.txt; do
    [ -e "$MAP_PATH" ] || continue
    for ALG in "${SHARED_ALGS[@]}"; do
      ALG_PATH="$SHARED_ALGS_FOLDER/$ALG"
      # Pass 1: Our alg as algorithm1, theirs as algorithm2
      CMD1="$SIMULATOR -comparative game_map=$MAP_PATH game_managers_folder=$GAME_MANAGERS_FOLDER algorithm1=$MY_ALG algorithm2=$ALG_PATH num_threads=$NUM_THREADS"
      echo "Running: $CMD1"
      $CMD1 > /dev/null

      # Pass 2: Their alg as algorithm1, ours as algorithm2
      CMD2="$SIMULATOR -comparative game_map=$MAP_PATH game_managers_folder=$GAME_MANAGERS_FOLDER algorithm1=$ALG_PATH algorithm2=$MY_ALG num_threads=$NUM_THREADS"
      echo "Running: $CMD2"
      $CMD2 > /dev/null
    done
  done
done
