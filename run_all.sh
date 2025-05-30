#!/bin/bash

# C++ 설정
CXX=clang++
CXXFLAGS="-std=c++17 -O2"
OUTPUT_NAME=temp_exec

# C++ 소스 목록
CPP_FILES=(
  "Greedy.cpp"
  "MCMF.cpp"
  "MCMF_knn.cpp"
  "MST_CLRS.cpp"
)

# C++ 컴파일 및 실행
for file in "${CPP_FILES[@]}"; do
  $CXX $CXXFLAGS "$file" -o $OUTPUT_NAME -lm
  if [ $? -ne 0 ]; then
    echo "❌ Compilation failed for $file"
    continue
  fi

  echo "======== Processing $file ========"
  ./$OUTPUT_NAME "dataset/$dataset"

  rm -f $OUTPUT_NAME
done

# Python 후처리 스크립트 실행
echo "======== Running _result.py ========"
python3 _result.py

echo "======== Running _visualize_tsp_paths.py ========"
python3 _visualize_tsp_paths.py
