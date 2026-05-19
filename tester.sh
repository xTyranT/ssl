#!/usr/bin/env bash

EXE="./ft_ssl"
PASS=0
FAIL=0
LEAK_FAIL=0

GREEN="\033[32m"
RED="\033[31m"
YELLOW="\033[33m"
RESET="\033[0m"

TMP_FILE="/tmp/ft_ssl_test_file"
TMP_EMPTY="/tmp/ft_ssl_empty_file"
TMP_OUT="/tmp/ft_ssl_out"
TMP_ERR="/tmp/ft_ssl_err"
TMP_B64_IN="/tmp/ft_ssl_b64_in"
TMP_B64_OUT="/tmp/ft_ssl_b64_out"
TMP_INVALID_B64="/tmp/ft_ssl_invalid_b64"

run_test() {
	name="$1"
	expected="$2"
	actual="$3"

	if [ "$expected" = "$actual" ]; then
		echo -e "${GREEN}[PASS]${RESET} $name"
		PASS=$((PASS + 1))
	else
		echo -e "${RED}[FAIL]${RESET} $name"
		echo "Expected: [$expected]"
		echo "Actual:   [$actual]"
		FAIL=$((FAIL + 1))
	fi
}

run_error_test() {
	name="$1"
	shift

	"$EXE" "$@" > "$TMP_OUT" 2> "$TMP_ERR"
	status=$?

	if [ "$status" -ne 0 ]; then
		echo -e "${GREEN}[PASS]${RESET} $name"
		PASS=$((PASS + 1))
	else
		echo -e "${RED}[FAIL]${RESET} $name"
		echo "Expected non-zero exit status"
		FAIL=$((FAIL + 1))
	fi
}

hash_expected() {
	algo="$1"
	input="$2"

	if [ "$algo" = "md5" ]; then
		printf "%s" "$input" | openssl md5 | awk '{print $2}'
	elif [ "$algo" = "sha256" ]; then
		printf "%s" "$input" | openssl sha256 | awk '{print $2}'
	elif [ "$algo" = "whirlpool" ]; then
		if [ "$input" = "test" ]; then
			echo "b913d5bbb8e461c2c5961cbe0edcdadfd29f068225ceb37da6defcf89849368f8c6c2eb6a4c4ac75775d032a0ecfdfe8550573062b653fe92fc7b8fb3b7be8d6"
		elif [ "$input" = "" ]; then
			echo "19fa61d75522a4669b44e39c1d2e1726c530232130d407f89afee0964997f7a73e83be698b288febcf88e3e03c4f0757ea8964e59b63d93708b138cc42a66eb3"
		fi
	fi
}

display_name() {
	algo="$1"

	if [ "$algo" = "md5" ]; then
		echo "MD5"
	else
		echo "$algo"
	fi
}

check_algo_output() {
	algo="$1"
	input="test"
	hash=$(hash_expected "$algo" "$input")
	display=$(display_name "$algo")

	echo -e "\n${YELLOW}Output tests: $algo${RESET}"

	echo -n "$input" > "$TMP_FILE"

	run_test "$algo -s default" \
		"$display (\"$input\") = $hash" \
		"$($EXE "$algo" -s "$input")"

	run_test "$algo -s quiet" \
		"$hash" \
		"$($EXE "$algo" -q -s "$input")"

	run_test "$algo -s reverse" \
		"$hash $input" \
		"$($EXE "$algo" -r -s "$input")"

	run_test "$algo stdin" \
		"$display (stdin) = $hash" \
		"$(printf "%s" "$input" | $EXE "$algo")"

	run_test "$algo -p stdin" \
		"$input$display (stdin) = $hash" \
		"$(printf "%s" "$input" | $EXE "$algo" -p | tr -d '\n')"

	run_test "$algo file default" \
		"$display ($TMP_FILE) = $hash" \
		"$($EXE "$algo" "$TMP_FILE")"

	run_test "$algo file quiet" \
		"$hash" \
		"$($EXE "$algo" -q "$TMP_FILE")"

	run_test "$algo file reverse" \
		"$hash $TMP_FILE" \
		"$($EXE "$algo" -r "$TMP_FILE")"

	empty_hash=$(hash_expected "$algo" "")
	echo -n "" > "$TMP_EMPTY"

	if [ -n "$empty_hash" ]; then
		run_test "$algo empty -s" \
			"$display (\"\") = $empty_hash" \
			"$($EXE "$algo" -s "")"

		run_test "$algo empty file" \
			"$display ($TMP_EMPTY) = $empty_hash" \
			"$($EXE "$algo" "$TMP_EMPTY")"
	fi
}

check_base64_output() {
	echo -e "\n${YELLOW}Base64 output tests${RESET}"

	run_test "base64 encode empty" \
		"" \
		"$(printf "" | $EXE base64)"

	run_test "base64 encode M" \
		"TQ==" \
		"$(printf "M" | $EXE base64)"

	run_test "base64 encode Ma" \
		"TWE=" \
		"$(printf "Ma" | $EXE base64)"

	run_test "base64 encode Man" \
		"TWFu" \
		"$(printf "Man" | $EXE base64)"

	run_test "base64 encode test" \
		"dGVzdA==" \
		"$(printf "test" | $EXE base64)"

	run_test "base64 encode Hello world" \
		"SGVsbG8gd29ybGQ=" \
		"$(printf "Hello world" | $EXE base64)"

	run_test "base64 explicit -e" \
		"dGVzdA==" \
		"$(printf "test" | $EXE base64 -e)"

	run_test "base64 last mode wins -d -e" \
		"dGVzdA==" \
		"$(printf "test" | $EXE base64 -d -e)"

	run_test "base64 decode empty" \
		"" \
		"$(printf "" | $EXE base64 -d)"

	run_test "base64 decode TQ==" \
		"M" \
		"$(printf "TQ==" | $EXE base64 -d)"

	run_test "base64 decode TWE=" \
		"Ma" \
		"$(printf "TWE=" | $EXE base64 -d)"

	run_test "base64 decode TWFu" \
		"Man" \
		"$(printf "TWFu" | $EXE base64 -d)"

	run_test "base64 decode dGVzdA==" \
		"test" \
		"$(printf "dGVzdA==" | $EXE base64 -d)"

	run_test "base64 decode Hello world" \
		"Hello world" \
		"$(printf "SGVsbG8gd29ybGQ=" | $EXE base64 -d)"

	run_test "base64 last mode wins -e -d" \
		"test" \
		"$(printf "dGVzdA==" | $EXE base64 -e -d)"

	echo -n "test" > "$TMP_B64_IN"
	$EXE base64 -i "$TMP_B64_IN" -o "$TMP_B64_OUT"

	run_test "base64 -i -o encode file" \
		"dGVzdA==" \
		"$(cat "$TMP_B64_OUT")"

	echo -n "dGVzdA==" > "$TMP_B64_IN"
	$EXE base64 -d -i "$TMP_B64_IN" -o "$TMP_B64_OUT"

	run_test "base64 -d -i -o decode file" \
		"test" \
		"$(cat "$TMP_B64_OUT")"

	echo -n "test" > "$TMP_B64_IN"
	run_test "base64 -i stdout" \
		"dGVzdA==" \
		"$($EXE base64 -i "$TMP_B64_IN")"

	echo -n "dGVzdA==" > "$TMP_B64_IN"
	run_test "base64 -d -i stdout" \
		"test" \
		"$($EXE base64 -d -i "$TMP_B64_IN")"
}

run_leak_test() {
	name="$1"
	stdin_data="$2"
	shift 2

	if [ -n "$stdin_data" ]; then
		printf "%s" "$stdin_data" | valgrind \
			--leak-check=full \
			--show-leak-kinds=all \
			--errors-for-leak-kinds=definite,indirect,possible \
			--error-exitcode=42 \
			"$EXE" "$@" > "$TMP_OUT" 2> "$TMP_ERR"
	else
		valgrind \
			--leak-check=full \
			--show-leak-kinds=all \
			--errors-for-leak-kinds=definite,indirect,possible \
			--error-exitcode=42 \
			"$EXE" "$@" > "$TMP_OUT" 2> "$TMP_ERR"
	fi

	status=$?

	if [ "$status" -eq 42 ]; then
		echo -e "${RED}[LEAK]${RESET} $name"
		grep -E "definitely lost|indirectly lost|possibly lost|ERROR SUMMARY" "$TMP_ERR"
		LEAK_FAIL=$((LEAK_FAIL + 1))
	else
		echo -e "${GREEN}[NO LEAK]${RESET} $name"
	fi
}

check_algo_leaks() {
	algo="$1"

	echo -e "\n${YELLOW}Leak tests: $algo${RESET}"

	echo -n "test" > "$TMP_FILE"

	run_leak_test "$algo -s" "" "$algo" -s "test"
	run_leak_test "$algo -q -s" "" "$algo" -q -s "test"
	run_leak_test "$algo -r -s" "" "$algo" -r -s "test"
	run_leak_test "$algo file" "" "$algo" "$TMP_FILE"
	run_leak_test "$algo stdin" "test" "$algo"
	run_leak_test "$algo -p stdin" "test" "$algo" -p
}

check_base64_leaks() {
	echo -e "\n${YELLOW}Leak tests: base64${RESET}"

	echo -n "test" > "$TMP_B64_IN"

	run_leak_test "base64 stdin encode" "test" base64
	run_leak_test "base64 stdin decode" "dGVzdA==" base64 -d
	run_leak_test "base64 -e stdin encode" "test" base64 -e
	run_leak_test "base64 -i encode" "" base64 -i "$TMP_B64_IN"
	run_leak_test "base64 -i -o encode" "" base64 -i "$TMP_B64_IN" -o "$TMP_B64_OUT"

	echo -n "dGVzdA==" > "$TMP_B64_IN"
	run_leak_test "base64 -d -i decode" "" base64 -d -i "$TMP_B64_IN"
	run_leak_test "base64 -d -i -o decode" "" base64 -d -i "$TMP_B64_IN" -o "$TMP_B64_OUT"
}

check_error_cases() {
	echo -e "\n${YELLOW}Error tests${RESET}"

	echo -n "abc" > "$TMP_INVALID_B64"

	run_error_test "no command"
	run_error_test "unknown command" unknown

	run_error_test "md5 unknown flag" md5 -x
	run_error_test "sha256 unknown flag" sha256 -x
	run_error_test "whirlpool unknown flag" whirlpool -x

	run_error_test "md5 missing -s argument" md5 -s
	run_error_test "sha256 missing -s argument" sha256 -s
	run_error_test "whirlpool missing -s argument" whirlpool -s

	run_error_test "file does not exist" md5 /tmp/ft_ssl_does_not_exist_123

	run_error_test "base64 unknown flag" base64 -x
	run_error_test "base64 missing -i argument" base64 -i
	run_error_test "base64 missing -o argument" base64 -o
	run_error_test "base64 unexpected argument" base64 file.txt
	run_error_test "base64 invalid decode length" base64 -d -i "$TMP_INVALID_B64"
}

cleanup() {
	rm -f "$TMP_FILE" "$TMP_EMPTY" "$TMP_OUT" "$TMP_ERR" \
		"$TMP_B64_IN" "$TMP_B64_OUT" "$TMP_INVALID_B64"
}

if [ ! -x "$EXE" ]; then
	echo -e "${RED}Error:${RESET} $EXE not found or not executable"
	exit 1
fi

check_algo_output md5
check_algo_output sha256
check_algo_output whirlpool
check_base64_output
check_error_cases

if command -v valgrind >/dev/null 2>&1; then
	check_algo_leaks md5
	check_algo_leaks sha256
	check_algo_leaks whirlpool
	check_base64_leaks
else
	echo -e "\n${YELLOW}Skipping leak tests:${RESET} valgrind not installed"
fi

cleanup

echo
echo "Output/Error tests passed: $PASS"
echo "Output/Error tests failed: $FAIL"
echo "Leak tests failed: $LEAK_FAIL"

if [ "$FAIL" -eq 0 ] && [ "$LEAK_FAIL" -eq 0 ]; then
	echo -e "${GREEN}All tests passed.${RESET}"
	exit 0
else
	echo -e "${RED}Some tests failed.${RESET}"
	exit 1
fi