#!/bin/bash

TESTS_DIR="./func_tests/data"

if [ ! -d "$TESTS_DIR" ]; then
    echo "Directory with tests does not exist"
    exit 1
fi

pos_tests=0
neg_tests=0
result=0

for input_file in "$TESTS_DIR"/pos_*_in.txt; do
    number=$(echo "$input_file" | grep -oP '[0-9]*')
    output_file="$TESTS_DIR/pos_${number}_out.txt"
    if [ -f "$output_file" ]; then
        ./func_tests/scripts/pos_case.sh "${input_file}" "${output_file}"
        test_result=$?
        if [[ "$1" == "-v" && "$test_result" -ne 0 ]]; then
            echo "Pos test ${number} not passed: expected output $(cat "${output_file}"), got file app_output_pos_${number}"
        fi
        pos_tests=$((pos_tests + test_result))
    else
        echo "Output for positive test ${number} not found"
        exit 1
    fi
done

for input_file in "$TESTS_DIR"/neg_*_in.txt; do
    ./func_tests/scripts/neg_case.sh "${input_file}"
    test_result=$?
    if [[ "$1" == "-v" && "$test_result" -ne 0 ]]; then
        number=$(echo "$input_file" | grep -oP '[0-9]*')
        echo "Neg test ${number} not passed"
    fi
    neg_tests=$((neg_tests + test_result))
done

result=$((neg_tests + pos_tests))

if [[ "$result" -eq 0 ]]; then
    echo "All tests passed!"
else
    echo "Total of ${result} tests not passed"
    echo "${pos_tests} positive tests not passed and ${neg_tests} negative tests not passed"
fi

exit $result
