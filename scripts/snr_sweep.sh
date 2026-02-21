#!/bin/bash

BINARY="./pucch_codes_modeling.elf"
OUTPUT_FILE="results/full_snr_sweep.json"
PLOT_SCRIPT="scripts/plot_full_sweep.py"
ITERATIONS=${1:-20000}
SNR_START=${2:--10}
SNR_END=${3:-4}
SNR_STEP=${4:-2}
SKIP_PLOT=${5:-0}

CODE_LENGTHS=(2 4 6 8 11)

if [ ! -f "$BINARY" ]; then
    echo "Error: Binary '$BINARY' not found. Run 'make' first."
    exit 1
fi

mkdir -p results

echo "========================================"
echo "  PUCCH F2 Codec — Full SNR Sweep"
echo "========================================"
echo "Code lengths: ${CODE_LENGTHS[*]} bits"
echo "Iterations:   $ITERATIONS per point"
echo "SNR range:    $SNR_START to $SNR_END dB (step $SNR_STEP)"
echo "Output:       $OUTPUT_FILE"
echo "========================================"
echo ""

ALL_RESULTS=()
TOTAL_POINTS=$((${#CODE_LENGTHS[@]} * (($SNR_END - $SNR_START) / $SNR_STEP + 1)))
CURRENT_POINT=0

for CODE_LENGTH in "${CODE_LENGTHS[@]}"; do
    echo ">>> Processing code length: $CODE_LENGTH bits"
    
    for (( SNR=SNR_START; SNR<=SNR_END; SNR+=SNR_STEP )); do
    ((CURRENT_POINT++))
    echo -n "  [$CURRENT_POINT/$TOTAL_POINTS] SNR = $SNR dB ... "
    
    INPUT_JSON=$(cat <<EOF
{
    "mode": "channel simulation",
    "num_of_pucch_f2_bits": $CODE_LENGTH,
    "snr_db": $SNR,
    "iterations": $ITERATIONS
}
EOF
)
    
    OUTPUT=$(PUCCH_DISABLE_FILE_OUTPUT=1 $BINARY "$INPUT_JSON" 2>&1)
    EXIT_CODE=$?
    
    if [ $EXIT_CODE -ne 0 ]; then
        echo "✗ FAILED"
        echo "    Error: $OUTPUT"
        continue
    fi
    
    echo "✓ Done"
    ALL_RESULTS+=("$OUTPUT")
done
    echo ""
done

echo "Saving results to $OUTPUT_FILE ..."

{
    echo "{"
    echo "  \"metadata\": {"
    echo "    \"code_lengths\": [$(IFS=,; echo "${CODE_LENGTHS[*]}")],"
    echo "    \"snr_range\": {\"start\": $SNR_START, \"end\": $SNR_END, \"step\": $SNR_STEP},"
    echo "    \"iterations\": $ITERATIONS,"
    echo "    \"timestamp\": \"$(date -Iseconds)\""
    echo "  },"
    echo "  \"results\": ["
    
    for i in "${!ALL_RESULTS[@]}"; do
        if [ $i -gt 0 ]; then
            echo "    ,"
        fi
        echo "${ALL_RESULTS[$i]}" | sed 's/^/    /'
    done
    
    echo "  ]"
    echo "}"
} > "$OUTPUT_FILE"

echo ""
echo "========================================"
echo "  Sweep Complete!"
echo "  Results: ${#ALL_RESULTS[@]} points saved"
echo "  Output:  $OUTPUT_FILE"
echo "========================================"

echo ""
echo "Summary by code length:"
echo "======================="

for CODE_LENGTH in "${CODE_LENGTHS[@]}"; do
    echo ""
    echo "Code length: $CODE_LENGTH bits"
    echo "----------------------"
    printf "%-10s %-10s %-10s %-10s\n" "SNR(dB)" "BLER" "Success" "Failed"
    echo "----------------------"
    
    for result in "${ALL_RESULTS[@]}"; do
        RESULT_LEN=$(echo "$result" | grep -o '"num_of_pucch_f2_bits": [0-9]*' | grep -o '[0-9]*$')
        if [ "$RESULT_LEN" = "$CODE_LENGTH" ]; then
            SNR=$(echo "$result" | grep -o '"snr_db": [0-9.-]*' | grep -o '[0-9.-]*$')
            BLER=$(echo "$result" | grep -o '"bler": [0-9.]*' | grep -o '[0-9.]*$')
            SUCCESS=$(echo "$result" | grep -o '"success": [0-9]*' | grep -o '[0-9]*$')
            FAILED=$(echo "$result" | grep -o '"failed": [0-9]*' | grep -o '[0-9]*$')
            printf "%-10s %-10s %-10s %-10s\n" "$SNR" "$BLER" "$SUCCESS" "$FAILED"
        fi
    done
done

echo "----------------------"

echo ""
if [ "$SKIP_PLOT" -eq 0 ]; then
    if [ -f "$PLOT_SCRIPT" ]; then
        echo "Generating plot..."        
        if command -v python3 &> /dev/null; then
            python3 "$PLOT_SCRIPT" "$OUTPUT_FILE"
            if [ $? -eq 0 ]; then
                echo "✓ Plot generated successfully"
            else
                echo "⚠ Plot script failed (check matplotlib installation)"
                echo "  Install: pip3 install matplotlib numpy"
            fi
        else
            echo "⚠ Python3 not found. Skipping plot generation."
            echo "  Install Python3 to enable automatic plotting."
        fi
    else
        echo "⚠ Plot script '$PLOT_SCRIPT' not found. Skipping plot generation."
    fi
else
    echo "Plot generation skipped (SKIP_PLOT=1)"
fi

echo ""
echo "Done!"