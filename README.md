# gossiping-broadcast

C++ implementation of the second [distributed systems challenge](https://fly.io/dist-sys/) (broadcasting).

## Setup

Package management is done using [conan](https://docs.conan.io/2.0/tutorial/consuming_packages/build_simple_cmake_project.html). To install all dependencies, run:

```
conan install . --output-folder=build --build=missing
```

## Testing

To test the implementation, compile the binary and run:

```
# Single node
./maelstrom test -w broadcast --bin $GOSSIP_BIN --node-count 1 --time-limit 20 --rate 10

# Multiple nodes
./maelstrom test -w broadcast --bin $GOSSIP_BIN --node-count 5 --time-limit 20 --rate 10

# Fault tolerance
./maelstrom test -w broadcast --bin $GOSSIP_BIN --node-count 5 --time-limit 20 --rate 10 --nemesis partition

# Efficiency
./maelstrom test -w broadcast --bin $GOSSIP_BIN --node-count 25 --time-limit 20 --rate 100 --latency 100
```
