#!/usr/bin/env bash
#SBATCH --job-name=spellcheck-sweep
#SBATCH --time=15:00:00
#SBATCH --nodes=1
#SBATCH --ntasks=32
#SBATCH --cpus-per-task=1
#SBATCH --partition=sapphire
#SBATCH --account=punim0520
#SBATCH --hint=nomultithread
#SBATCH --output=logs/%x.%j.out
#SBATCH --error=logs/%x.%j.err

set -euo pipefail
cd "$SLURM_SUBMIT_DIR"
mkdir -p logs results

DICT=${DICT:-/data/projects/punim0520/2025/Project2/dict10000.txt}
WORDS=${WORDS:-/data/projects/punim0520/2025/Project2/words10000.txt}
MAX_THREADS=${MAX_THREADS:-8}
BASE_MD5=${BASE_MD5:-""}

module purge
module load GCC/13.3.0
module load OpenMPI/5.0.3

make clean >/dev/null 2>&1 || true
make spellcheck >/dev/null
echo "[build] ok"

CSV="results/sweep_np1-32_omp1-${MAX_THREADS}_$(basename "$DICT")_$(basename "$WORDS").csv"
echo "np,omp,get_ms,total_ms,peak_mem_kb_sum,peak_mem_kb_max,md5_ok" > "$CSV"

if [[ -z "$BASE_MD5" ]]; then
  export OMP_NUM_THREADS=1
  export OMP_PROC_BIND=close
  export OMP_PLACES=cores
  srun -n 1 --cpus-per-task=1 --cpu-bind=cores ./build/spellcheck "$DICT" "$WORDS"
  BASE_MD5=$(md5sum results/word_list_misspelled.txt | awk '{print $1}')
fi
echo "Baseline MD5 = $BASE_MD5"

TOTAL_CORES=${SLURM_CPUS_ON_NODE:-32}

for np in 1 2 4 8 16 32; do
  for omp in 1 2 4 8 16 32; do
    cores=$((np * omp))
    [[ $cores -gt $TOTAL_CORES ]] && continue

    echo ""
    echo "=== Run np=${np}, omp=${omp} (Total cores=${cores}) ==="

    export OMP_NUM_THREADS=$omp
    export OMP_PROC_BIND=close
    export OMP_PLACES=cores

    OUTDIR="logs/mem_${np}_${omp}"
    mkdir -p "$OUTDIR"
    
    srun -n "$np" --cpus-per-task="$omp" --cpu-bind=cores \
    --output="$OUTDIR/run.%t.out" --error="$OUTDIR/run.%t.err" \
    bash -lc '
    /usr/bin/time -f "%M" -o "'"$OUTDIR"'/maxrss.$SLURM_PROCID" \
    ./build/spellcheck "'"$DICT"'" "'"$WORDS"'"
  '

    PEAK_SUM=$(awk '{s+=$1} END{print s+0}' "$OUTDIR"/maxrss.*)
    PEAK_MAX=$(awk 'max<$1{max=$1} END{print max+0}' "$OUTDIR"/maxrss.*)

    LOG0="$OUTDIR/run.0.out"
    GET_MS=$(grep -E "Get misspelled" "$LOG0" | sed -E 's/.*\(([^ ]+) ms\).*/\1/' | tail -n1 || echo "NA")
    TOTAL_MS=$(grep -E "^[[:space:]]*- TOTAL:" "$LOG0" | sed -E 's/.*\(([^ ]+) ms\).*/\1/' | tail -n1 || echo "NA")

    MD5=$(md5sum results/word_list_misspelled.txt | awk '{print $1}')
    OK="NO"; [[ "$MD5" == "$BASE_MD5" ]] && OK="YES"

    echo "${np},${omp},${GET_MS},${TOTAL_MS},${PEAK_SUM},${PEAK_MAX},${OK}" >> "$CSV"
  done
done

echo ""
echo "[*] Done. CSV -> $CSV"
