import os
import subprocess
import difflib

# Directory containing the test files
TEST_DIR = "./tests"
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

def run_test(test_file):
    global pass_count, fail_count

    base_name = os.path.splitext(os.path.basename(test_file))[0]
    expected_file = os.path.join(TEST_DIR, f"{base_name}.expected")
    output_file = os.path.join(OUT_DIR, f"{base_name}.output")

    # Compile the .w file
    compile_command = ["./wlangc", test_file]
    result = subprocess.run(compile_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    if result.returncode != 0:
        print(f"{BOLD}{RED}Compilation failed for {test_file}{RESET}")
        print(result.stderr.decode())
        fail_count += 1
        return

    # Run the compiled executable and capture its output
    try:
        run_command = [os.path.join(OUT_DIR, EXECUTABLE_NAME)]
        with open(output_file, "w") as f:
            subprocess.run(run_command, stdout=f, stderr=subprocess.PIPE)

        # Compare the output with the expected output
        with open(output_file, "r") as out_f, open(expected_file, "r") as exp_f:
            actual_output = out_f.read().rstrip('\n')
            expected_output = exp_f.read().rstrip('\n')

        if actual_output == expected_output:
            print(f"{BOLD}{GREEN}{base_name}: PASS{RESET}")
            pass_count += 1
        else:
            print(f"{BOLD}{RED}{base_name}: FAIL{RESET}")
            print(f"{YELLOW}expected output:{RESET}")
            print("".join(expected_output))
            print(f"{YELLOW}actual output:{RESET}")
            print("".join(actual_output))
            fail_count += 1
    except FileNotFoundError as e:
        print(f"{BOLD}{RED}Failed to run {EXECUTABLE_NAME}. Error: {e}{RESET}")
        fail_count += 1

    # Clean up the executable for the next test
    executable_path = os.path.join(OUT_DIR, EXECUTABLE_NAME)
    if os.path.exists(executable_path):
        os.remove(executable_path)

def main():
    for filename in os.listdir(TEST_DIR):
        if filename.endswith(".w"):
            test_file = os.path.join(TEST_DIR, filename)
            run_test(test_file)

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
