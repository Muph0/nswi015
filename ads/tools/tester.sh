#!/bin/bash

RED='\e[31m'
GRN='\e[32m'
RST='\e[0m'

EXC_BUILDF=16
EXC_RUNF=32
EXC_OUTF=64

depth=0
indent=""
verbose=false

function echo_usage {
    echo -e "Usage:"
    echo ""
    local offset='\e[40G'
    echo -e "$0 run <test-name>                 Run particular test."
    echo -e "$0 suite <test-suite-file>         Run all tests in given test suite."
}
function error { echo -e "$0: Error:" $@ >&2; }
function info { echo -e "$0:" $@ >&2; }

function find_test {
    find -ipath "./$1/*.c"
}
function build_test {
    info "Locating test sources."
    local files=$(find_test "$1")
    local safe_name="$2"

    local src_base="../.."
    local test_dir="$(pwd)/build/test_$safe_name"

    info "Entering '$test_dir'."
    mkdir -vp "$test_dir"
    cd "$test_dir"

    info "Configuring build."

    #files=`echo "$files" | sed 's/\//\\\\\//g'`

    # Create Makefile
    cat > Makefile <<EOF

TEST_NAME = test_$safe_name
TEST_SRC = $files
TEST_BIN = build/\$(TEST_NAME)/test.out

\$(TEST_SRC):
	export TEST_NAME=\$(TEST_NAME); \\
	make -C ../.. \$(TEST_BIN)

test.out: \$(TEST_SRC)

EOF

    make test.out 2>&1 | tee build.log
    local exc=${PIPESTATUS[0]}
    return $exc
}

function run_test {
    ./test.out 2>&1 | tee run.log
    local exc=${PIPESTATUS[0]}

    if [ $exc != 0 ]; then
        return $EXC_RUNF
    fi

    local expect=()

    # Check output
    while read -r line; do
        if [ "${line:0:10}" == "[EXPECT]: " ]; then
            expect+=("${line:10}")
        else
            if [ "${#expect[@]}" != "0" ]; then
                local shouldBe="${expect[0]}"

                if [ "$line" != "$shouldBe" ]; then
                    echo -e "\n== INVALID OUTPUT ==" >> run.log
                    return $EXC_OUTF
                fi
            fi
        fi
    done < run.log
}

function run_suite {
    local suite="$1"

    echo "${indent}suite '$suite' {"
    local failedCount=0
    local passedCount=0
    local buildFailure=false
    local runFailure=false
    local stdoutFailure=false
    local resultCode=0

    while read -r kind name; do

        if [ "$kind" == "run" ]; then
            echo -n "${indent}    $kind $name "
        fi

        local t_start=$EPOCHREALTIME

        $0 -d $[$depth + 1] $kind $name
        local exc=$?

        local t_end=$EPOCHREALTIME
        local t_diff=$(bc <<<"scale=3; $t_end - $t_start")

        resultCode=$[$resultCode | $exc]

        if [ "$kind" == "run" ]; then
            if [ $exc == 0 ]; then
                echo -en "${GRN}OK${RST}"
                passedCount=$[$passedCount + 1]
            elif (( $[$exc & $EXC_BUILDF] != 0 )); then
                echo -en "${RED}BUILD FAILED${RST}"
                failedCount=$[$failedCount + 1]
                buildFailure=true
            elif (( $[$exc & $EXC_RUNF] != 0 )); then
                echo -en "${RED}ASSERT FAILED${RST}"
                failedCount=$[$failedCount + 1]
                runFailure=true
            elif (( $[$exc & $EXC_OUTF] != 0 )); then
                echo -en "${RED}INVALID${RST}"
                failedCount=$[$failedCount + 1]
                stdoutFailure=true
            else
                echo -en "${RED}(unspecified error)${RST}"
                failedCount=$[$failedCount + 1]
            fi
        fi

        echo -en ' '

        if [ `bc <<<"$t_diff > 60"` == 1 ]; then
            echo "(`date --date=@0$t_diff +%-Mm%S.%3Ns`)"
        else
            echo "(`date --date=@0$t_diff +%-S.%3Ns`)"
        fi

    done < "$suite"

    echo -n "${indent}} "

    if [ $failedCount == 0 ]; then
        echo -ne "$GRN$passedCount$RST test$([ $passedCount -ge 2 ] && echo s) passed."
        if [ "$depth" == 0 ]; then
            echo ""
        fi
        return 0
    else
        echo -ne "$RED$failedCount$RST test$([ $failedCount -ge 2 ] && echo s) failed, $GRN$passedCount$RST passed."

        if [ "$depth" == 0 ]; then
            echo ""
            $buildFailure && echo "A build has failed. For details see ./build/<test-name>/build.log"
            $runFailure && echo "A test assertion has failed. For details see ./build/<test-name>/run.log"
            $stdoutFailure && echo "A test has produced invalid output. For details see ./build/<test-name>/run.log."
        fi

        return $resultCode
    fi
}

function main {
    local repeat=true
    depth=0

    while $repeat; do
        repeat=false

        local command=$1
        local name="$2"

        case $command in
            run)
                test -z "$name" && error "missing test name." && exit 1
                test -z $(find_test "$name") && error "no test '$name'." && exit 1
                local safe_name=$(echo "$name" | tr ' ./?*' '_')
                if $verbose || [ "$depth" == "0" ]; then
                    build_test "$name" "$safe_name" || error "build failed." && exit $EXC_BUILDF
                    run_test "$safe_name"
                    return $?
                else
                    build_test "$name" "$safe_name" >/dev/null 2>&1 || { error "build failed." && exit $EXC_BUILDF; } >/dev/null 2>&1
                    run_test "$safe_name" >/dev/null 2>&1
                    return $?
                fi
                ;;
            suite)
                local suite="./$name"
                test -z "$name" && error "missing test suite name." && exit 1
                test -f "$suite" || { error "no such file: $suite" && exit 1; }
                run_suite "$suite"
                exit $?
                ;;
            -v|--verbose)
                shift
                verbose=true
                repeat=true
                ;;
            -d|--depth)
                shift
                shift
                depth=$name
                indent=`cat /dev/zero | tr '\0' ' ' | head --bytes $[$depth * 4]`
                repeat=true
                ;;
            *)
                error "unknown command '$command'."
                echo_usage && exit 1
                ;;
        esac
    done
}



main $@

