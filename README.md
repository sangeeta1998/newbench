# Benchx

A synthetic C++ benchmark designed to stress CPU and memory with a variety of computational kernels (Mandelbrot, matrix multiplication, FFT, blur, sorting, and large static data). It is intended for benchmarking container image pull times and runtime performance across native and WebAssembly (Wasm) platforms, including RISC-V.

---

## Prerequisites

- **C++ compiler** (g++ or clang++)
- **wasi-sdk** (for Wasm build)
- **Docker** (with buildx and multi-arch support)
- **containerd** and `ctr` (for direct image pull/run/measurements)
---

## Building

### Native (amd64, arm64, riscv64)

#### 1. On your target system (or with cross-compiler):
```sh
g++ -O3 -static -o bench bench.cpp
```
Or, with clang++:
```sh
clang++ -O3 -static -fno-exceptions -o bench bench.cpp
```


### WebAssembly (Wasm)

#### 1. Install wasi-sdk (https://github.com/WebAssembly/wasi-sdk/releases)
```sh
export WASI_SDK_PATH=$HOME/wasi-sdk-25.0-x86_64-linux
$WASI_SDK_PATH/bin/clang++ --sysroot=$WASI_SDK_PATH/share/wasi-sysroot -O3 -fno-exceptions -o bench.wasm bench.cpp
```

---


**Build for platforms:**
```sh
docker buildx build --platform linux/amd64,linux/arm64 -f Dockerfile.native -t sangeetakakati/newbench:native --output type=image,push=true .
```

```sh
docker buildx build --platform wasm -f Dockerfile.wasm -t sangeetakakati/newbench:wasm --output type=image,push=true .
```

---

## Running the Benchmark

```sh
docker run --rm --platform linux/amd64 sangeetakakati/newbench:native
```

```sh
docker run --rm --platform wasm --runtime io.containerd.wasmtime.v1 sangeetakakati/newbench:wasm
```


## Measuring Image Pull Times with containerd (ctr)

```sh
chmod +x measure_ctr.sh
./measure_ctr.sh 10
```

This will output average pull times to `timing_results.csv`.

---

## References
- [wasi-sdk](https://github.com/WebAssembly/wasi-sdk)
- [Docker Buildx](https://docs.docker.com/build/buildx/working-with-buildx/)
- [containerd](https://containerd.io/)
