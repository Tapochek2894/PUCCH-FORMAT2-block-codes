#!/bin/bash

# PUCCH F2 Codec — Test Runner

BINARY="../../pucch_codes_modeling.elf"
PASSED=0
FAILED=0

echo "========================================"
echo "  PUCCH F2 Codec — Test Suite"
echo "========================================"
echo ""

for test_file in *.json; do
    rm -fr result.json
    echo -n "Testing $test_file ... "
    
    output=$(cat "$test_file" | $BINARY 2>&1)
    exit_code=$?
    
    if [[ "$test_file" == *"invalid"* ]]; then
        if [ $exit_code -ne 0 ]; then
            echo "✓ PASS (expected error)"
            ((PASSED++))
        else
            echo "✗ FAIL (should have failed)"
            ((FAILED++))
        fi
    else
        if [ $exit_code -eq 0 ]; then
            echo "✓ PASS"
            ((PASSED++))
        else
            echo "✗ FAIL"
            echo "  Output: $output"
            ((FAILED++))
        fi
    fi
done

echo ""
echo "========================================"
echo "  Results: $PASSED passed, $FAILED failed"
echo "========================================"

[ $FAILED -eq 0 ]