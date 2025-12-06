import argparse
import subprocess
import os

def get_build_number():
    num = 0
    while os.path.exists(f'builds/build_{num}'):
        num += 1
    return num

parser = argparse.ArgumentParser(description="Build script for the Sand Simulation Engine.")
parser.add_argument('--clean', help='Clean or not', action='store_true')
parser.add_argument('--build_number', type=int, help='Specify build number', default=None)
args = parser.parse_args()

if args.clean:
    if args.build_number == -1:
        print("Cleaning all build directories...")
        subprocess.run(['rm', '-rf', 'builds/*'])
        result = subprocess.run(['ls', 'builds/'], shell=True, capture_output=True, text=True)
        if result.stdout.strip() == '':
            print("All builds cleaned.")
        else:
            print("ERROR: Some build directories could not be deleted. Please check the output for errors.")
    elif args.build_number is not None:
        build_dir = f'builds/build_{args.build_number}'
        print(f"Cleaning build directory: {build_dir}...")
        subprocess.run(['rm', '-rf', build_dir])
        if not os.path.exists(build_dir):
            print(f"Build directory {build_dir} cleaned.")
        else:
            print(f"ERROR: Build directory {build_dir} could not be deleted. Please check the output for errors.")
    else:
        build_num = get_build_number() - 1
        if os.path.exists(f'builds/build_{build_num}'):
            build_dir = f'builds/build_{build_num}'
            print(f"Cleaning latest build directory: {build_dir}...")
            subprocess.run(['rm', '-rf', build_dir])
            if not os.path.exists(build_dir):
                print(f"Latest build directory {build_dir} cleaned.")
            else:
                print(f"ERROR: Latest build directory {build_dir} could not be deleted. Please check the output for errors.")
        else:
            print("No build directories found to clean.")
else:
    if args.build_number is not None:
        build_dir = f'builds/build_{args.build_number}'
    else:
        build_num = get_build_number()
        build_dir = f'builds/build_{build_num}'
    os.makedirs('builds', exist_ok=True)
    print("Starting build process at directory:", build_dir)
    subprocess.run(['mkdir', '-p', f'{build_dir}/'])
    subprocess.run([f'cd {build_dir}/ && cmake ../../'], shell=True)
    subprocess.run([f'cd {build_dir}/ && make'], shell=True)
    result = subprocess.run([f'cd {build_dir} && ls'], shell=True, capture_output=True, text=True)
    if 'SandSimulationEngine' in result.stdout:
        print("Build completed successfully.")
    else:
        print("ERROR: Build failed. Please check the output for errors.")