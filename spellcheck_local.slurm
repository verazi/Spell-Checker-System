#!/usr/bin/env bash
set -euo pipefail

DICT=${1:-files/dict100000.txt}
WORDS=${2:-files/words100000.txt}
MAX_THREADS=${3:-8}
NTASKS=${4:-1}

mkdir -p logs results

OS=$(uname -s)
if command -v nproc >/dev/null 2>&1; then
  TOTAL_CORES=$(nproc)
elif [[ "$OS" == "Darwin" ]]; then
  TOTAL_CORES=$(sysctl -n hw.ncpu)
else
  TOTAL_CORES=8
fi

compute_md5() {
  if command -v md5sum >/dev/null 2>&1; then
    md5sum "$1" | awk '{print $1}'
  elif command -v gmd5sum >/dev/null 2>&1; then
    gmd5sum "$1" | awk '{print $1}'
  elif command -v md5 >/dev/null 2>&1; then
    md5 -q "$1"
  else
    echo "ERROR: no md5 tool"; exit 2
  fi
}

peak_rss_kb() {
  local logfile="$1"
  local os="$OS"
  if [[ "$os" == "Darwin" ]]; then
    awk '/maximum resident set size/ {print $6}' "$logfile" 2>/dev/null || echo NA
  else
    awk '/Maximum resident set size/ {print $NF}' "$logfile" 2>/dev/null || echo NA
  fi
}

if ! command -v mpirun >/dev/null 2>&1; then
  echo "ERROR: mpirun not found. Install OpenMPI (or MPICH)."
  exit 2
fi

make spellcheck >/dev/null
echo "[build] ok"

echo "=== Inputs ==="
echo "DICT=$DICT"
echo "WORDS=$WORDS"
echo "NTASKS=$NTASKS"
echo "MAX_THREADS=$MAX_THREADS"
echo "TOTAL_CORES=$TOTAL_CORES"
echo "===================="

CSV="results/sweep_np${NTASKS}_omp1-${MAX_THREADS}_$(basename "$DICT")_$(basename "$WORDS").csv"
echo "np,omp,get_ms,total_ms,peak_mem_kb,md5_ok" > "$CSV"

export OMP_NUM_THREADS=1
export OMP_PROC_BIND=close
export OMP_PLACES=cores

BASE_RUNLOG=$(mktemp)
BASE_TIMELOG=$(mktemp)

if [[ "$OS" == "Darwin" ]]; then
  /usr/bin/time -l mpirun -np 1 ./build/spellcheck "$DICT" "$WORDS" \
    1> >(tee "$BASE_RUNLOG") 2> "$BASE_TIMELOG"
else
  /usr/bin/time -v mpirun -np 1 ./build/spellcheck "$DICT" "$WORDS" \
    1> >(tee "$BASE_RUNLOG") 2> "$BASE_TIMELOG"
fi

BASE_MD5=$(compute_md5 results/word_list_misspelled.txt)
echo "Baseline MD5 = $BASE_MD5"

BASE_PEAK=$(peak_rss_kb "$BASE_TIMELOG")
rm -f "$BASE_RUNLOG" "$BASE_TIMELOG"

for np in 1 2 4 8 16 32; do
  for omp in 1 2 4 8; do
    cores=$((np * omp))
    [[ $cores -gt $TOTAL_CORES ]] && continue
    [[ $omp -gt $MAX_THREADS ]] && continue

    echo ""
    echo "=== Run np=${np}, omp=${omp} (Total cores=${cores}) ==="

    export OMP_NUM_THREADS=$omp
    export OMP_PROC_BIND=close
    export OMP_PLACES=cores

    RUNLOG=$(mktemp)
    TIMELOG=$(mktemp)

    if [[ "$OS" == "Darwin" ]]; then
      /usr/bin/time -l mpirun -np "$np" ./build/spellcheck "$DICT" "$WORDS" \
        1> >(tee "$RUNLOG") 2> "$TIMELOG"
    else
      /usr/bin/time -v mpirun -np "$np" ./build/spellcheck "$DICT" "$WORDS" \
        1> >(tee "$RUNLOG") 2> "$TIMELOG"
    fi

    GET_MS=$(grep -E "Get misspelled" "$RUNLOG" | sed -E 's/.*\(([^ ]+) ms\).*/\1/' | tail -n1 || true)
    TOTAL_MS=$(grep -E "^[[:space:]]*- TOTAL:" "$RUNLOG" | sed -E 's/.*\(([^ ]+) ms\).*/\1/' | tail -n1 || true)
    if [[ -z "${TOTAL_MS:-}" ]]; then
      TOTAL_MS=$(grep -E "^\[\*\] Wall time:" "$RUNLOG" | awk -F';' '{print $2}' | tr -d ' ' | tail -n1 || echo "NA")
    fi
    if [[ -z "${GET_MS:-}" ]]; then GET_MS="NA"; fi

    PEAK=$(peak_rss_kb "$TIMELOG")

    rm -f "$RUNLOG" "$TIMELOG"

    MD5=$(compute_md5 results/word_list_misspelled.txt)
    OK="NO"; [[ "$MD5" == "$BASE_MD5" ]] && OK="YES"

    echo "${np},${omp},${GET_MS},${TOTAL_MS},${PEAK},${OK}" >> "$CSV"
  done
done

echo ""
echo "[*] Done. CSV -> $CSV"
