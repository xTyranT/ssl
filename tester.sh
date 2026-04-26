#!/bin/bash

BIN="./ft_ssl"
FAILS=0
TOTAL=0

OUT="/tmp/ft_ssl_test_out"
ERR="/tmp/ft_ssl_test_err"

run_test() {
    NAME="$1"
    EXPECT="$2"
    shift 2

    TOTAL=$((TOTAL + 1))

    "$BIN" "$@" >"$OUT" 2>"$ERR"
    STATUS=$?

    if [ "$EXPECT" = "OK" ] && [ "$STATUS" -eq 0 ]; then
        echo "[OK]   $NAME"
    elif [ "$EXPECT" = "ERR" ] && [ "$STATUS" -ne 0 ]; then
        echo "[OK]   $NAME"
    else
        echo "[FAIL] $NAME"
        echo "       command: $BIN $*"
        echo "       expected: $EXPECT"
        echo "       status: $STATUS"
        echo "       stdout:"
        cat "$OUT"
        echo "       stderr:"
        cat "$ERR"
        FAILS=$((FAILS + 1))
    fi
}

make >/dev/null || exit 1

echo "hello" > /tmp/ft_ssl_file1
echo "world" > /tmp/ft_ssl_file2
echo "test"  > /tmp/ft_ssl_file3

# Files with flag-looking names in current directory
touch -- -p -q -r

# Basic valid
run_test "md5 no args stdin" OK md5
run_test "md5 -p" OK md5 -p
run_test "md5 -q" OK md5 -q
run_test "md5 -r" OK md5 -r
run_test "md5 all flags" OK md5 -p -q -r

# Invalid flags before files
run_test "invalid -x before file" ERR md5 -x
run_test "invalid combined -pq before file" ERR md5 -pq
run_test "invalid dash only before file" ERR md5 -
run_test "invalid double dash --p before file" ERR md5 --p
run_test "invalid long --help before file" ERR md5 --help

# -s behavior
run_test "-s simple" OK md5 -s hello
run_test "-s empty string" OK md5 -s ""
run_test "-s spaces" OK md5 -s "hello world"
run_test "-s flag-looking string" OK md5 -s -p
run_test "-s invalid-looking string" OK md5 -s -x
run_test "-s missing value" ERR md5 -s
run_test "multiple -s" OK md5 -s one -s two -s three

# Files
run_test "single file" OK md5 /tmp/ft_ssl_file1
run_test "multiple files" OK md5 /tmp/ft_ssl_file1 /tmp/ft_ssl_file2 /tmp/ft_ssl_file3
run_test "missing file" ERR md5 /tmp/does_not_exist_123

# Once file parsing starts, flag-looking args are files
run_test "file then -p treated as file" OK md5 /tmp/ft_ssl_file1 -p
run_test "file then -q treated as file" OK md5 /tmp/ft_ssl_file1 -q
run_test "file then -r treated as file" OK md5 /tmp/ft_ssl_file1 -r

run_test "file then missing -x treated as file" ERR md5 /tmp/ft_ssl_file1 -x
run_test "file then missing -qr treated as file" ERR md5 /tmp/ft_ssl_file1 -qr

# -s before file still consumes string
run_test "-s then file" OK md5 -s hello /tmp/ft_ssl_file1
run_test "-p -s then file" OK md5 -p -s hello /tmp/ft_ssl_file1
run_test "multiple strings and files" OK md5 -s one /tmp/ft_ssl_file1 -s two /tmp/ft_ssl_file2

# OpenSSL-style: after first file, -s is also a file, not a string flag
run_test "file then -s treated as file if file exists" ERR md5 /tmp/ft_ssl_file1 -s hello

# Command errors
run_test "unknown command" ERR unknown
run_test "sha typo" ERR sh256

# SHA256 / Whirlpool basic command routing
run_test "sha256 no args" OK sha256
run_test "sha256 -s" OK sha256 -s hello
run_test "sha256 invalid flag" ERR sha256 -x

run_test "whirlpool no args" OK whirlpool
run_test "whirlpool -s" OK whirlpool -s hello
run_test "whirlpool invalid flag" ERR whirlpool -x

# Stress-ish parser tests
run_test "many strings" OK md5 \
    -s a -s b -s c -s d -s e -s f -s g -s h -s i -s j

run_test "many files" OK md5 \
    /tmp/ft_ssl_file1 /tmp/ft_ssl_file2 /tmp/ft_ssl_file3 \
    /tmp/ft_ssl_file1 /tmp/ft_ssl_file2 /tmp/ft_ssl_file3

run_test "many mixed before file phase" OK md5 \
    -p -q -r -s a -s b -s c /tmp/ft_ssl_file1 /tmp/ft_ssl_file2

rm -f "$OUT" "$ERR"
rm -f /tmp/ft_ssl_file1 /tmp/ft_ssl_file2 /tmp/ft_ssl_file3
rm -f -- -p -q -r

echo
echo "Total: $TOTAL"
echo "Failures: $FAILS"

if [ "$FAILS" -eq 0 ]; then
    echo "All tests passed ✅"
else
    echo "Some tests failed ❌"
fi

exit "$FAILS"