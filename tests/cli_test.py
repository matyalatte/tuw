"""Tests for command-line features of Tuw"""
import os
import subprocess
import difflib

def run_command(command, should_succeed=True):
    """Runs a shell command."""
    print(f"Running: {command}")
    result = subprocess.run(command, shell=True, capture_output=True, text=True)
    if should_succeed and result.returncode != 0:
        raise RuntimeError(f"Error running command: {command}\n{result.stderr}")
    return result


def load_text(file_path):
    """Loads a file as text."""
    with open(file_path, "r", encoding="utf-8") as f:
        return f.readlines()


def compare_text(text1, text2):
    """Compares two text files and prints differences."""
    diff = list(difflib.unified_diff(text1, text2, fromfile="test.json", tofile="test2.json", lineterm=""))

    if diff:
        print("Differences found:")
        for line in diff:
            print(line, end="")
        raise RuntimeError("Failed to embed JSON.")
    else:
        print("Succeed in embedding JSON.")


if __name__ == "__main__":
    # Test if merge and split commands preserve JSON.
    if os.name == "nt":
        ext = ".exe"
        sep = "\\"
    else:
        ext = ""
        sep = "/"
    json_path = f"json{sep}help.json"
    json_out_path = f"out.json"

    run_command(f"..{sep}Tuw{ext} merge -j {json_path} -e Tuw.new{ext} -f")
    run_command(f".{sep}Tuw.new{ext} split -j {json_out_path} -e Tuw.orig{ext} -f")

    # Check if both files were the same or not
    json1 = load_text(json_path)
    json2 = load_text(json_out_path)
    if json2[-1][-1] != "\n":
        json2[-1] += "\n"
    compare_text(json1, json2)
