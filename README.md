libsnark knn circuit
a zero-knowledge proof of K-Nearest Neighbor Algorithm

### 1 init 
 ```
 git submodule update --init --recursive
 ```
### 2 compile
 ```
 mkdir build && cd build &&cmake ..
 ```
 ```
 make
 ```
 You can find the "knn" binary under the build/knn folder.

### 3 run
```
./knn/knn
```