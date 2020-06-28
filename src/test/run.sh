#!/usr/bin/env bash

TEST_BINARY=$1
TEST_NAME=$2
TIME_FORMAT=$3
CAND_LIMIT=$4

GOLD_INPUT="test/.gold/${TEST_NAME}/input.txt"
GOLD_OUTPUT="test/.gold/${TEST_NAME}/output.txt"
TEST_OUTPUT="test/output_${TEST_NAME}.txt"

die() {
    echo "Error: $*" >> /dev/stderr
    exit 1
}

if [ ! -x "${TEST_BINARY}" ]; then
    die "no executable ${TEST_BINARY} found"
fi

if [ -f "${GOLD_INPUT}" ]; then
    mkdir -p `dirname "$TEST_OUTPUT"`
    cmd="$TEST_BINARY $GOLD_INPUT '$TIME_FORMAT' $CAND_LIMIT > $TEST_OUTPUT"
    eval $cmd
else
    die "test input ${GOLD_INPUT} not found"
fi

diff -q $GOLD_OUTPUT $TEST_OUTPUT &> /dev/null
if [ $? -ne 0 ] ; then
    echo "[Test $TEST_NAME]: FAIL"
    exit 1
else
    echo "[Test $TEST_NAME]: PASS"
    exit 0
fi

