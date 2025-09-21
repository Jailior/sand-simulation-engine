import argparse
import subprocess

parser = argparse.ArgumentParser(description="Build script for the Sand Simulation Engine.")
parser.add_argument('--clean', help='Clean or not', action='store_true')
args = parser.parse_args()

if args.clean:
    print("Cleaning previous build artifacts...")
    subprocess.run(['rm', '-rf', 'build/'])
    result = subprocess.run(['ls'], shell=True, capture_output=True, text=True)
    if 'build' not in result.stdout:
        print("Clean completed.")
    else:
        print("ERROR: Clean failed. Please check the output for errors.")
else:
    print("Starting build process...")
    subprocess.run(['mkdir', '-p', 'build/'])
    subprocess.run(['cd build/ && cmake ..'], shell=True)
    subprocess.run(['cd build/ && make'], shell=True)
    result = subprocess.run(['cd build/ && ls'], shell=True, capture_output=True, text=True)
    if 'SandSimulationEngine' in result.stdout:
        print("Build completed successfully.")
    else:
        print("ERROR: Build failed. Please check the output for errors.")