#!/usr/bin/env bash
set -euo pipefail

BIN="../test-mergesort"
OUTDIR="../logs"
mkdir -p "$OUTDIR"

N=100000000
SEED=1234
REPS=3
DATE_TAG=$(date +"%Y%m%d_%H%M%S")
LOGFILE="$OUTDIR/all_runs_${DATE_TAG}.log"

if [ ! -x "$BIN" ]; then
  echo "Building..."
  make
fi

# --- header ---
{
echo "==============================================="
echo " Parallel Merge Sort Batch Run"
echo "==============================================="
echo "Date: $(date)"
echo "Binary: $BIN"
echo "Input size: $N"
echo "Seed: $SEED"
echo "Repetitions: $REPS"
echo "-----------------------------------------------"
} | tee "$LOGFILE"

# --- main loop ---
for cutoff in 0 1 2 3 4 5 6 7 8; do
  echo "" | tee -a "$LOGFILE"
  echo ">>> Cutoff = $cutoff" | tee -a "$LOGFILE"
  echo "-----------------------------------------------" | tee -a "$LOGFILE"

  for rep in $(seq 1 $REPS); do
    echo "Run $rep/$REPS (n=$N, cutoff=$cutoff, seed=$SEED)" | tee -a "$LOGFILE"
    "$BIN" "$N" "$cutoff" "$SEED" 2>&1 | tee -a "$LOGFILE"
    echo "" | tee -a "$LOGFILE"
  done
done

# --- footer ---
{
echo "==============================================="
echo "✅ All runs complete."
echo "Log file saved to: $LOGFILE"
echo "==============================================="
} | tee -a "$LOGFILE"
