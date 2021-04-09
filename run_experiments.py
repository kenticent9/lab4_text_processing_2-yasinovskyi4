import math
import os
import platform
import subprocess
import sys


def main():
    if len(sys.argv) != 3:
        raise ValueError("Usage: python run_experiments.py num_times conf_file")
    try:
        num_times = int(sys.argv[1])
    except ValueError:
        raise ValueError("Usage: python run_experiments.py num_times conf_file")

    conf_file = sys.argv[2]

    cur_work_dir = os.getcwd()
    build_dir = os.path.join(cur_work_dir, 'build')
    os.makedirs(build_dir, exist_ok=True)

    # It's unnecessary to add the .exe ending on Windows, but I did it
    # for clarity
    if platform.system() == 'Windows':
        exe = os.path.join(build_dir, 'process_text.exe')
    else:
        exe = os.path.join(build_dir, 'process_text')

    cmake_flags = ['-DCMAKE_BUILD_TYPE=Release', '-G', 'Unix Makefiles']
    subprocess.run(['cmake', '..'] + cmake_flags, cwd=build_dir, check=True)
    subprocess.run(['cmake', '--build', '.'], cwd=build_dir, check=True)

    print("Testing text processing, microseconds:")

    max_threads = 4
    last_out_by_a = None
    for indexing_threads in range(1, max_threads+1):
        with open(conf_file, 'r') as f:
            lines = f.readlines()
            lines[3] = f'indexing_threads={indexing_threads}\n'
        with open(conf_file, 'w') as f:
            f.writelines(lines)

        min_time = float('inf')
        for _ in range(num_times):
            p = subprocess.run([exe, conf_file], capture_output=True, text=True, check=True)
            output = p.stdout.split()
            time = float(output[1])
            if time < min_time:
                min_time = time

        # We're only interested in checking the results for race
        # conditions, therefore it is sufficient to check only one
        # output file
        out_by_a_name = lines[1].strip().split('=')[1]
        with open(out_by_a_name, 'r', encoding='UTF-8') as f:
            out_by_a = f.read()
        if last_out_by_a is not None and last_out_by_a != out_by_a:
            raise Exception("The results are not the same.")
        last_out_by_a = out_by_a

        print(f'{indexing_threads}: {min_time}')

    print("\nAll results are the same.")


if __name__ == '__main__':
    main()
