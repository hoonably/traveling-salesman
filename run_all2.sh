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
  echo "======== Processing $file ========"

  TEMP_FILE="temp_$file"
  
  # "util.h" → "util2.h" 로 변경하여 임시 파일 생성
  sed 's/#include "util\.h"/#include "util2.h"/' "$file" > "$TEMP_FILE"

  # 컴파일
  $CXX $CXXFLAGS "$TEMP_FILE" -o $OUTPUT_NAME -lm
  if [ $? -ne 0 ]; then
    echo "❌ Compilation failed for $file"
    rm -f "$TEMP_FILE"
    continue
  fi

  # 실행
  ./$OUTPUT_NAME "dataset/$dataset"

  # 임시 파일 및 실행 파일 삭제
  rm -f "$TEMP_FILE" "$OUTPUT_NAME"
done

# Python 후처리 스크립트 실행
echo "======== Running _result.py ========"
python3 _result.py

echo "======== Running _visualize_tsp_paths.py ========"
python3 _visualize_tsp_paths.py
