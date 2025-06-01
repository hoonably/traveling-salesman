#!/bin/bash

# C++ 설정
CXX=clang++
CXXFLAGS="-std=c++17 -O2"
OUTPUT_NAME=temp_exec

# C++ 소스 목록
CPP_FILES=(
  "Greedy.cpp"
  "MST_CLRS.cpp"
  # "MCMF.cpp"
  # "MCMF_knn.cpp"
)

# C++ 컴파일 및 실행
for file in "${CPP_FILES[@]}"; do
  # util2로 변경해서 진행 -> 100K 이상의 입력
  TEMP_FILE="temp_$file"
  sed 's/#include "util\.h"/#include "util2.h"/' "$file" > "$TEMP_FILE"

  $CXX $CXXFLAGS "$TEMP_FILE" -o $OUTPUT_NAME -lm
  if [ $? -ne 0 ]; then
    echo "❌ Compilation failed for modified $file"
  else
    ./$OUTPUT_NAME "dataset/$dataset"
    rm -f $OUTPUT_NAME
  fi

  rm -f "$TEMP_FILE"
done

# Python 후처리 스크립트 실행
echo "======== Running _result.py ========"
python3 _result.py

echo "======== Running _visualize_tsp_paths.py ========"
python3 _visualize_tsp_paths.py
