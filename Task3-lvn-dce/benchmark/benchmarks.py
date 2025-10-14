import subprocess
import json
import pathlib
# # [envs.dynamic]
# command = "bril2json < {filename} | brili -p {args} 2>&1 | grep total_dyn_inst > {filename}.dynamic"


LVN = "./lvn_exec"
DCE = "./dce_exec"

TEST_DIR = pathlib.Path("tests")

results = {}

for bril_file in TEST_DIR.glob("*.bril"):
    name = bril_file.name

    # no opt
    baseline_cmd = f"bril2json < {bril_file} | brili -p"
    baseline_proc = subprocess.run(
        baseline_cmd, shell=True, capture_output=True, text=True
    )
    baseline_out = baseline_proc.stderr.strip().splitlines()
    baseline_dyn = None
    for line in baseline_out:
        if line.startswith("total_dyn_inst:"):
            baseline_dyn = int(line.split(":")[1].strip())
    baseline_lines = sum(1 for _ in open(bril_file))

    # optimized
    opt_cmd = f"bril2json < {bril_file} | {LVN} | {DCE} | brili -p"
    opt_proc = subprocess.run(
        opt_cmd, shell=True, capture_output=True, text=True
    )
    opt_out = opt_proc.stderr.strip().splitlines()
    opt_dyn = None
    for line in opt_out:
        if line.startswith("total_dyn_inst:"):
            opt_dyn = int(line.split(":")[1].strip())

    # To count static lines of optimized program, run pipeline but stop before brili
    opt_code_cmd = f"bril2json < {bril_file} | {LVN} | {DCE} | bril2txt"
    opt_code_proc = subprocess.run(
        opt_code_cmd, shell=True, capture_output=True, text=True
    )
    opt_code_lines = len(opt_code_proc.stdout.strip().splitlines())

    results[name] = {
        "original_lines": baseline_lines,
        "original_dyn_inst": baseline_dyn,
        "optimized_lines": opt_code_lines,
        "optimized_dyn_inst": opt_dyn,
    }

with open("lvn_results.json", "w") as f:
    json.dump(results, f, indent=4)
