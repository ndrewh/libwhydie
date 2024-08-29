libwhydie
====

## Build

```
apt install -y build-essential clang libunwind-dev
git clone git@github.com:ndrewh/libwhydie.git
cd libwhydie && git submodule update --init
make
```

## Usage

```
LD_PRELOAD=/path/to/libwhydie.so ./crashing_process
```
