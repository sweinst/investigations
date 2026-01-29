import os
import sys
import platform
import statistics
from subprocess import Popen, PIPE
from tqdm import trange

project_dir = os.path.dirname(os.path.abspath(__file__))
caches_path = os.path.join(project_dir, '..', 'bin', platform.system(), 'Release', 'caches', 'caches')

if not os.path.exists(caches_path):
    print(f"caches executable does not exist: {caches_path}\n", file=sys.stderr)
    sys.exit(1)

n_iterations = 10_000_000
m_size_max = 30
n_repeats = 100
result = []
for size2 in trange(1, m_size_max + 1, leave=True, desc="Array size"):
    cmd = [caches_path, '-n', str(n_iterations), "-s", str(size2)]
    tmp = []
    for _ in trange(n_repeats, leave=False, desc="Sampling..."):
        process = Popen(cmd, stdout=PIPE, stderr=PIPE)
        stdout, stderr = process.communicate()
        if process.returncode != 0:
            print(f"Error executing command: {' '.join(cmd)} ({stderr.decode()})", file=sys.stderr)
            print(stderr.decode(), file=sys.stderr)
            sys.exit(1)
    tmp.append(float(stdout.decode()))
    result.append((size2, statistics.median(tmp)))

print(result)


