# gossiping-broadcast

C++ implementation of the second [distributed systems challenge](https://fly.io/dist-sys/).  // FIXME

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
```

# init
{"id":0,"src":"c0","dest":"n0","body":{"type":"init","node_id":"n0","node_ids":["n0", "n1"],"msg_id":1}}
# topo
{"id":14,"src":"c9","dest":"n0","body":{"type":"topology","topology":{"n0":["n3","n1"]},"msg_id":1}}
# broadcast
{"id":4,"src":"c2","dest":"n0","body":{"type":"broadcast","message":0,"msg_id":1}}


