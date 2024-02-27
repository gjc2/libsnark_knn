knn circuit using libsnark

a zero-knowledge proof of K-Nearest Neighbor Algorithm

### 1 init 
complete installation of libsnark 
### 2 compile
complete build 

you can make change and design your circuit and compile
 ```
 rm -rf build
 ```
 ```
 mkdir build && cd build &&cmake ..
 ```
 ```
 make
 ```
 You can find the "knn" binary under the build/knn folder.

### 3 run
```
cd build
```
```
./knn/knn
```