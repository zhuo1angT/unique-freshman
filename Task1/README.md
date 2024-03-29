# 联创团队Lab组2020春第一期新人任务

**Lab组传统艺能**（大嘘

c++练手

## 描述

你需要实现两种C++ STL中的基本容器，set与priority queue。

方法定义如下：

- Set
  - bool empty() const;
  - size_type size() const;
  - void clear();
  - void insert(const value_type& value);
  - SizeType erase(const value_type& value);
  - SizeType count(const value_type& value);
- Priority Queue
  - const_reference top() const;
  - bool empty() const;
  - size_type size() const;
  - void push(const value_type& value);
  - void pop();

更多的信息可以在 [C++ reference](http://www.cplusplus.com/reference) 找到

你大可以放轻松，因为这只是用来提高你对C++语言的掌握程度，但是这里也有一些要求。

## 要求

- 你被要求使用LLRB（左偏红黑树）作为 Set 的物理结构，斐波那契堆作为Priority Queue的物理结构。
- 一个正常的工程师应当明白如何检验自己的程序是否正常通过，所以你被要求自己生成足够强度的数据集来对你的数据结构进行检测。
- 编程语言：你可以使用C++，C，汇编语言，机器码中的任意一种语言来实现题目的要求。

- 编程风格：**NO OI STYLE**，使用OI风格进行编程是非常不推荐的，请尽量使用贴近工程的风格编写，此外，良好的编程习惯会很有帮助，这里有更多信息 [Google Coding Style Guide](https://google.github.io/styleguide/cppguide.html#C++_Version)
- 其他：首先你需要明白自己在写什么，还要明白为什么这样写，这意味这你最好明白上述数据结构复杂度的证明~~，即使这不是必要的~~。

## 评估

- Basic:
  - 实现所有上述的数据结构与功能
  - 自己生成数据进行测试
- Advance:
  - 为Set实现iterator，并且为Set增添以下操作：
    - iterator find(const value_type& value);
    - iterator lower_bound(const value_type& value);
    - iterator upper_bound(const value_type& value);
  - 使用Template实现上述要求
  - 使你的数据结构线程安全(Thread-Safe)

我们也许会提供简单的测试文件或者是测试程序，也有可能不提供。如果简单的测试文件和你的程序毫不兼容，你也可以自己想一些奇妙的测试方法。

## DDL

下次组会

## 参考

这里有你需要的更多信息，大概

[Learn C++ in Y miniutes](https://learnxinyminutes.com/docs/c++/)

[Left Leaning Red-Black Trees, Princeton](https://www.cs.princeton.edu/~rs/talks/LLRB/RedBlack.pdf)

[Priority Queue](https://en.wikipedia.org/wiki/Priority_queue)

[GNU libstdc++](https://gcc.gnu.org/onlinedocs/libstdc++/)

**尽信书不如无书**，上面的资料也许存在错误，需要你自己思考、自己排查，或者是自己寻找更棒的资料。