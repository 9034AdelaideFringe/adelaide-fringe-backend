# Adelaide Fringe

Adelaide Fringe

## Prerequisites and Evironment

* crow installed [Link](https://github.com/CrowCpp/Crow/releases/download/v1.2.1.2/Crow-1.2.1-Linux.deb)

* gcc

* make

* cmake

### tested on Ubuntu22.04LTS

## Config
add a `config.json` file in the root dir, and add
```json
{
    "database": "your database here",
    "JWTSecret": "jwt secret here"
}
```

## Build
change to the root dir and run
```shell
cmake -B build && cmake --build build
```

## Run
```shell
./server
```