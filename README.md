Build project
```
mkdir build && cd build
cmake -S .. -B . -DCMAKE_BUILD_TYPE=Release
make -j
```
Run with Gosper's glider pattern by default for 350 generations
```
./src/main
```
Or run with other patterns: `bee_hive`, `toad`, `glider`, `switch_engine`, `switch_engine_2` and optionally specify the number of generations
```
./src/main glider 50
```
