import sys
import time
from subprocess import call


def calculate_speedup(run_log_name: str) -> None:
    gains = []
    with open(run_log_name, 'r') as f:
        while True:
            name = f.readline()
            if not name:
                break
            if name[0] == "#":
                print(name)
                break

            ref = float(f.readline().split(' ')[-1])

            cand_line = f.readline()
            if "Ran out of fuel" in cand_line:
                print(f'Found bad result: {cand_line}')
                continue

            cand = float(cand_line.split(' ')[-1])

            perct_change = ((cand - ref) / ref) * 100
            gains.append(perct_change)

        print(f'Average percent change in candidate solution: {sum(gains) / len(gains):0.4f}%')


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('Not enough args')
        exit(1)

    print()
    run_log_name = sys.argv[1]
    calculate_speedup(run_log_name)
