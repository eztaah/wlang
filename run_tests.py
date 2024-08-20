#!/bin/python3

import os
import subprocess

# Output directory for the compiled executables
OUT_DIR = "./out"
EXECUTABLE_NAME = "prog"

# Create the output directory if it doesn't exist
os.makedirs(OUT_DIR, exist_ok=True)

# Initialize counters
pass_count = 0
fail_count = 0

# ANSI color codes
RED = "\033[91m"
GREEN = "\033[92m"
YELLOW = "\033[93m"
BOLD = "\033[1m"
RESET = "\033[0m"

def format_test_path(test_file):
    """
    Format the test file path to hide the 'compiler/tests' and 'stdlib/tests' prefixes.
    """
    if test_file.startswith("./compiler/tests/"):
        return test_file[len("./compiler/tests/"):]
    elif test_file.startswith("./stdlib/tests/"):
        return test_file[len("./stdlib/tests/"):]
    return test_file

def run_test(test_file):
    global pass_count, fail_count

    with open(test_file, 'r') as file:
        lines = file.readlines()

    # Determine if the test checks exit code or output
    if not lines:
        print(f"{BOLD}{RED}{format_test_path(test_file)}: FAIL{RESET}")
        print(f"    {YELLOW}Test specification missing{RESET}")
        fail_count += 1
        return

    last_line = lines[-1].strip()

    if ': expected exit code :' in last_line:
        expected_output = last_line.split(': expected exit code : ')[-1].strip()
        check_exit_code = True
    elif ': expected output :' in last_line:
        # Capturing exactly what is written after ": expected output :"
        expected_output = last_line.split(': expected output : ', 1)[-1]
        # Replace escaped characters like \\n with \n
        expected_output = expected_output.replace("\\n", "\n").replace("\\t", "\t").replace("\\0", "\0")
        check_exit_code = False
    else:
        print(f"{BOLD}{RED}{format_test_path(test_file)}: FAIL{RESET}")
        print(f"    {YELLOW}Test specification missing{RESET}")
        fail_count += 1
        return

    # Compile and link the .w file using the compiler
    compile_command = ["./wlangc", test_file]
    result = subprocess.run(compile_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    if result.returncode != 0:
        print(f"{BOLD}{RED}{format_test_path(test_file)}: FAIL{RESET}")

        error_message = result.stderr.decode().strip().splitlines()
        formatted_error_message = "\n".join([f"    {line}" for line in error_message])
        print(formatted_error_message)
        fail_count += 1
        return

    if expected_output == "SKIP":
        print(f"{BOLD}{GREEN}{format_test_path(test_file)}: PASS{RESET}")
        pass_count += 1
        return

    # Run the compiled executable and capture the exit code or output
    try:
        run_command = [os.path.join(OUT_DIR, EXECUTABLE_NAME)]
        result = subprocess.run(run_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        
        if check_exit_code:
            # Compare the actual exit code with the expected one
            actual_output = str(result.returncode)
            if actual_output == expected_output.strip():
                print(f"{BOLD}{GREEN}{format_test_path(test_file)}: PASS{RESET}")
                pass_count += 1
            else:
                print(f"{BOLD}{RED}{format_test_path(test_file)}: FAIL{RESET}")
                print(f"    {YELLOW}expected exit code:{RESET} {repr(expected_output)}")
                print(f"    {YELLOW}actual exit code:{RESET} {repr(actual_output)}")
                fail_count += 1

        else:
            # Compare the actual output with the expected output (both are raw text)
            actual_output = result.stdout.decode('utf-8', errors='replace')
            if actual_output == expected_output:
                print(f"{BOLD}{GREEN}{format_test_path(test_file)}: PASS{RESET}")
                pass_count += 1
            else:
                print(f"{BOLD}{RED}{format_test_path(test_file)}: FAIL{RESET}")
                print(f"    {YELLOW}expected output:{RESET} {repr(expected_output)}")
                print(f"    {YELLOW}actual output:{RESET} {repr(actual_output)}")
                fail_count += 1

    except FileNotFoundError as e:
        print(f"{BOLD}{RED}Failed to run {EXECUTABLE_NAME} in {format_test_path(test_file)}. Error: {e}{RESET}")
        fail_count += 1

    # Clean up the executable for the next test
    executable_path = os.path.join(OUT_DIR, EXECUTABLE_NAME)
    if os.path.exists(executable_path):
        os.remove(executable_path)

def run_tests_in_directory(directory):
    global pass_count, fail_count
    
    # Traverse all directories and run tests in alphabetical order
    for root, dirs, files in os.walk(directory, topdown=True):
        # Sort directories and files alphabetically
        dirs.sort()
        files.sort()

        # Skip the 'out' directory
        if os.path.basename(root) == "out":
            continue

        for file in files:
            if file.endswith(".w"):
                test_file = os.path.join(root, file)
                run_test(test_file)

def main():
    global pass_count, fail_count

    # Run tests in 'compiler/tests' first
    print(f"{BOLD}COMPILER TESTS{RESET}")
    run_tests_in_directory("./compiler/tests")

    # Then run tests in 'stdlib/tests'
    print(f"\n{BOLD}STDLIB TESTS{RESET}")
    run_tests_in_directory("./stdlib/tests")

    # Print summary
    print(f"\nSummary:")
    print(f"Total tests passed: {pass_count}")
    print(f"Total tests failed: {fail_count}")

    # Exit with a success code if all tests passed, otherwise fail
    if fail_count == 0:
        exit(0)
    else:
        exit(1)

if __name__ == "__main__":
    main()

