import time
import numpy as np

def bench(n, runs=5):
    a = np.random.rand(n, n).astype(np.float32)
    b = np.random.rand(n, n).astype(np.float32)
    a @ b  # warm-up
    times = []
    for _ in range(runs):
        t0 = time.perf_counter()
        a @ b
        times.append(time.perf_counter() - t0)
    times.sort()
    med = times[len(times) // 2]
    return med, 2.0 * n**3 / med / 1e9

if __name__ == "__main__":
    np.show_config() 
    print()
    for n in [64, 128, 256, 512, 1024, 2048]:
        med, gflops = bench(n)
        print(f"numpy matmul(n={n:<5}) : {med*1000:10.3f} ms | {gflops:7.2f} GFLOPS")