测试文件使用gtest，需要自行安装gtest

将你的程序写在同目录的`PriorityQueue.h`里

编译命令（有多文件就自己搞编译命令）

```bash
g++ -o test test.cpp -Wall -lgtest -lpthread
```

然后

```bash
./test
```

测试