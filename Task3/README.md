## Introduction

当你在一台主机上同时开很多虚拟机，虽然生产力++，但存储空间却减少很多。在小规模的虚拟化下，也许挺少关心存储空间的问题。

而在生产环境中，为了追求隔离性，一台服务器上启动非常多的虚拟机跑不同应用时，这些应用之间本来可以大量共用的系统文件（如动态链接库），就成为产生冗余和浪费的罪魁祸首。

为解决这一问题，我们可以使用OverlayFS和AUFS为代表的多层文件系统（Multi-Layered Unification Filesystem）。就像脱胎于Linux Container技术的docker所做的那样。以OverlayFS为例，直观上它是这样的：

![img](https://arkingc.github.io/img/in-post/post-kernel-overlayfs/mount_tree.png)

在Reference中有更多的资料帮助你了解这一技术。

当然，诸如此类Union Filesystem（Union FS)所挂载的目录的文件系统是其他单独的文件系统，如ext4。但在这个任务中，不希望在分层中依赖ext4等其他现有文件系统，而仅仅借用分层的思想，来解决虚拟化中空间浪费的简单问题。

## Task

实现一个**用户态虚拟多层文件系统**，要求演示时，做到两个不同程序`program1`与`program2`共用多个文件（在一个`lower`目录下，**只读层**），同时有各自独立文件（分别位于`upper1`和`upper2`下，**可读写层**），最终呈现在**合并层**`merge1`和`merge2`下。

除此之外，你还需要实现：

- 写时复制（copy-on-write）
- 仅需要实现一层lower
- 支持shell下`ls` `touch` `cat` `cp` `mv` `rm` `cd` `mkdir` `rmdir` `chmod` 命令

具体来说，任务可分两步完成: 

1.  先实现一个用户态的虚拟文件系统作为雏形
2. 在雏形较为稳定的情况下，改造成具有分层特性的文件系统Multi-layer FS

注：这个任务与传统Union FS有显著区别，尽情发挥你的创造性（~~尽情魔改不是~~），文件系统毕竟都由你手搓。另外，如果你从一开始就以分层特性为目的实现一个文件系统也未尝不可。

#### 虚拟文件系统的雏形

在这里只需要你使用合适的数据结构和索引机制进行文件的存储。在内核接口方面，你可以使用**libfuse**，这意味着不要求进行内核驱动方面的编程。

你只需要在用户空间实现libfuse封装的部分操作，来支持shell的文件管理。其中，libfuse库提供的example下有样例参考，同时推荐[Github-πfs](https://github.com/philipl/pifs)以了解libfuse使用的基本框架。

文件如何组织是非常重要的，你需要了解**inode, block, superblock**（如下图）。这方面网上有大量的资料，你可以参照主流的文件系统模式，或者自己设计你认为合理的管理方式。

![img](https://www3.nd.edu/~pbui/teaching/cse.30341.fa17/static/img/project06-layout.png)

你的文件系统雏形应该尽可能的简单，这样在改造成分层时不至于过于痛苦。

#### 分层化的改造

你可以参考overlayFS的思想直接在上面的文件系统雏形内改造。任务要求`lower` `upper` `merge`三层均要支持多级目录，这也意味着在inode中魔改是一个不错的选择。

当然还有一种效率不高的办法：把上述文件系统独立出来，另外用libfuse弄一套和UnionFS差不多的东西，就像建立在ext4上的overlayFS一样。如果你在查阅资料中了解到Linux FUSE的实现机制时，你就能想象经过FUSE套娃后的效率有多么低下。

## Extra

为了让任务更加有趣，同时你的时间充裕的话，你可以为文件系统增加一些features，如支持加密，文件完整性校验，线程安全等。

## DDL

预计两周时间，中期会有Code Review，希望你已经完成文件系统的雏形了。

## Reference

善用搜索：https://www.google.com/

#### Filesystem

[Linux 文件系统剖析](https://www.ibm.com/developerworks/cn/linux/l-linux-filesystem/index.html)

#### FUSE

https://en.wikipedia.org/wiki/Filesystem_in_Userspace

https://www.cs.nmsu.edu/~pfeiffer/fuse-tutorial/

https://www.cs.hmc.edu/~geoff/classes/hmc.cs135.201109/homework/fuse/fuse_doc.html

http://libfuse.github.io/doxygen/index.html

#### UnionFS & OverlayFS

[How the overlay driver works](https://docs.docker.com/storage/storagedriver/overlayfs-driver/#how-the-overlay-driver-works)

[OverlayFS初识与概述](https://blog.csdn.net/luckyapple1028/article/details/77916194)

[OverlayFS使用与原理分析](https://blog.csdn.net/luckyapple1028/article/details/78075358)

[mergerfs - a featureful union filesystem](https://github.com/trapexit/mergerfs)