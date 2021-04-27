## Linux环境编程：从应用到内核



### 1.基础知识

#### 1.1. 一个Linux程序的诞生

一个C程序从编写到执行的过程：

预处理->编译->汇编->链接	 	GCC作为一个工具集帮我们处理了这些过程



预处理

- 处理预处理命令（#include展开 -> 将内容包含到文件中

- ```shell
  gcc -E file.c -> file.i
  ```

  

- 最后得到预处理后的文件

编译

- 对源码进行语法分析，并优化对应的汇编代码的过程

- ```shell
  gcc -S file.c -o file.s 
  ```

  (-S选项会让gcc在编译完成后停止后面的工作)

- 产生对应编译文件

汇编

- 将源代码翻译成可执行的指令，生成目标文件

- ```shell
  gcc -c file.c -o file.o
  ```

  

链接

- 生成可执行程序的最后一个步骤
- 将各个目标文件-包含库文件(库文件也是一种目标文件)链接成一个可执行程序
- 地址和空间的分配，符号解析，重定位
- GNU的链接器ld完成



执行下面命令可看到整个过程

```shell
gcc -g -Wall -v 0_1_hello_word.c -o hello_world 
```





#### 1.2. 程序的构成

Linux 下二进制可执行文件的格式一般为ELF格式

- 由section和symbol表组成
- section表
  - text段 （代码段，只读）
  - data段 （数据段，包含已经初始化的全局变量以及静态变量）
  - bss段 （未初始化数据段，包含未初始化的全局变量及静态变量）
  - 其他
    - .debug段 （用于调试信息）
    - .dynamic段 （用于保存动态链接信息）
    - .fini段 （用于保存进程退出时的执行程序，当进程结束时，系统会自动执行这段代码）
    - .rodata段 (用于保存制度数据，如const修饰的全局变量，字符串常量)
    - .symtab段（保存符号表）
- symbol表



#### 1.3 程序是如何跑的

- strace 命令追踪程序运行

```shell
strace ./hello_world 
execve("./hello_world", ["./hello_world"], 0x7fff59c39970 /* 67 vars */) = 0
brk(NULL)                               = 0x55d7176fa000
arch_prctl(0x3001 /* ARCH_??? */, 0x7ffea30703e0) = -1 EINVAL (Invalid argument)
access("/etc/ld.so.preload", R_OK)      = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
fstat(3, {st_mode=S_IFREG|0644, st_size=67391, ...}) = 0
mmap(NULL, 67391, PROT_READ, MAP_PRIVATE, 3, 0) = 0x7fe3ef6ed000
close(3)                                = 0
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libc.so.6", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\360q\2\0\0\0\0\0"..., 832) = 832
pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
pread64(3, "\4\0\0\0\20\0\0\0\5\0\0\0GNU\0\2\0\0\300\4\0\0\0\3\0\0\0\0\0\0\0", 32, 848) = 32
pread64(3, "\4\0\0\0\24\0\0\0\3\0\0\0GNU\0\t\233\222%\274\260\320\31\331\326\10\204\276X>\263"..., 68, 880) = 68
fstat(3, {st_mode=S_IFREG|0755, st_size=2029224, ...}) = 0
mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7fe3ef6eb000
pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
pread64(3, "\4\0\0\0\20\0\0\0\5\0\0\0GNU\0\2\0\0\300\4\0\0\0\3\0\0\0\0\0\0\0", 32, 848) = 32
pread64(3, "\4\0\0\0\24\0\0\0\3\0\0\0GNU\0\t\233\222%\274\260\320\31\331\326\10\204\276X>\263"..., 68, 880) = 68
mmap(NULL, 2036952, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x7fe3ef4f9000
mprotect(0x7fe3ef51e000, 1847296, PROT_NONE) = 0
mmap(0x7fe3ef51e000, 1540096, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x25000) = 0x7fe3ef51e000
mmap(0x7fe3ef696000, 303104, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x19d000) = 0x7fe3ef696000
mmap(0x7fe3ef6e1000, 24576, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1e7000) = 0x7fe3ef6e1000
mmap(0x7fe3ef6e7000, 13528, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0x7fe3ef6e7000
close(3)                                = 0
arch_prctl(ARCH_SET_FS, 0x7fe3ef6ec540) = 0
mprotect(0x7fe3ef6e1000, 12288, PROT_READ) = 0
mprotect(0x55d7162d0000, 4096, PROT_READ) = 0
mprotect(0x7fe3ef72b000, 4096, PROT_READ) = 0
munmap(0x7fe3ef6ed000, 67391)           = 0
fstat(1, {st_mode=S_IFCHR|0620, st_rdev=makedev(0x88, 0x1), ...}) = 0
brk(NULL)                               = 0x55d7176fa000
brk(0x55d71771b000)                     = 0x55d71771b000
write(1, "Hello World\n", 12Hello World
)           = 12
exit_group(0)                           = ?
+++ exited with 0 +++

```

- Linux环境中，执行一个程序
  - shell 调用 fork 创建一个子进程
  - 然后在子进程中真正执行这个命令
  - execve 加载可执行文件 hello_world
  - Ld 分别检查Ld.so.nohwcap 和 Ld.so.preload
    - Ld.so.nohwcap存在 加载其中未优化版本的库
    - Id.so.preload 存在 加载其中的库
      - 用于拦截或替换系统调用或C库
  - 之后利用mmap将LD.so.cache映射到内存中
    - ld.cache中保存了库的路径
  - LD加载c库 --libc.so.6 利用mmap及mprotect设置程序的各个内存区域 运行环境已经准备完成
  - 后面write会向文件描述符1（即标准输出）输出“Hello world!\n”  返回之13 表示成功write的字符个数
  - 最后调用exit_group推出程序 此时参数为0 表示程序退出状态



#### 1.4. 背景概念介绍

##### 1.4.1 系统调用

- 为系统提供服务 应用程序与内核通信接口 （系统调用性能消耗巨大，追求极致性能的程序中 尽力避免系统调用 -> C库gettimeofday）

- 用户空间的程序默认通过栈来传递参数 

  - 系统调用中 用户台和内核台使用的是不同的栈
  - 系统调用的参数通过寄存器进行传递

  问题： 编译器如何知道调用的函数是普通函数,还是系统调用呢？

##### 1.4.2 C库函数

- 一般都是glibc -> 封装了几乎所有的系统调用

- 位于用户台态 编译器可以统一处理所有的函数调用 而不用区分函数到底是不是系统调用

- Linux 平台下 系统调用的约定是使用寄存器eax来传递系统调用号，至于参数传递 详细可参见

  ```shell
  sysdeps/unix/sysv/linux/i386/sysdep.h
  ```

##### 1.4.3 线程安全

- 多线程环境下"安全"地执行

- 使用局部变量或资源 锁等同步机制 实现全局变量或资源的串行访问

  ```c
  #include <pthread.h>
  #include <stdio.h>
  #include <stdlib.h>
  static int counter = 0;
  #define LOOPS 10000000
  
  static void * thread(void * unused) {
  	int i;
  	for (i = 0; i < LOOPS; ++i) {
  		++counter;
  	}
  	return NULL;
  }
  
  int main(void) {
  	pthread_t t1, t2;
  	pthread_create(&t1, NULL, thread, NULL);
  	pthread_create(&t2, NULL, thread, NULL);
  	pthread_join(t1, NULL);
  	pthread_join(t2, NULL);
  	printf("Counter is %d by threads\n", counter);
  	return 0;
  }
  ```

  

- 反汇编程序如下

```assembly
0000000000000000 <thread>:
   0:	f3 0f 1e fa          	endbr64 
   4:	55                   	push   %rbp
   5:	48 89 e5             	mov    %rsp,%rbp
   8:	48 89 7d e8          	mov    %rdi,-0x18(%rbp)
   c:	c7 45 fc 00 00 00 00 	movl   $0x0,-0x4(%rbp)
  13:	eb 13                	jmp    28 <thread+0x28>
  15:	8b 05 00 00 00 00    	mov    0x0(%rip),%eax        # 1b <thread+0x1b>
  1b:	83 c0 01             	add    $0x1,%eax
  1e:	89 05 00 00 00 00    	mov    %eax,0x0(%rip)        # 24 <thread+0x24>
  24:	83 45 fc 01          	addl   $0x1,-0x4(%rbp)
  28:	81 7d fc 7f 96 98 00 	cmpl   $0x98967f,-0x4(%rbp)
  2f:	7e e4                	jle    15 <thread+0x15>
  31:	b8 00 00 00 00       	mov    $0x0,%eax
  36:	5d                   	pop    %rbp
  37:	c3                   	retq  
```

假设目前counter = 0

那么两个线程同时执行++counter会有以下情况：

1. 每个线程会有独立的上下文执行环境
2. 可以视为每个线程都有一个独立的EAX 或则和RBP

- 为了避免多个线程counter的并法访问导致错误时序而产生错误结果 其本质原因是因为++counter的执行指令并不是原子的 因此需要利用🔓来保证counter自增指令的串行化

  ```shell
  
  thread1                         thread2
  lock                            lock
  eax =0                          eax = counter => eax = 1
  eax = eax +1 => eax = 1         eax = eax+1 => eax = 2        
  counter = eax => counter = 1    counter = eax => counter = 2
  unlock                          unlock
  ```

  

##### 1.4.4 原子性

- 不可分割的属性
- 要么成功 要么失败 （数据库 事务）

##### 1.4.5 可重入函数

- 不仅表示可重复进入，还需要进入后能成功执行
- 可重入 -> 线程安全  充分不必要条件
  - 信号处理
  - longjump
- 死🔓示例

```C
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static const char* const caller[2] = {"mutex_thread", "signal handler"};
static pthread_t mutex_tid;
static pthread_t sleep_tid;
static volatile int signal_handler_exit = 0;

static void hold_mutex(int c) {
    printf("enter hold_mutex [caller %s]\n", caller[c]);
    pthread_mutex_lock(&mutex);
    while (!signal_handler_exit && c != 1) {
        sleep(5);
    }
    pthread_mutex_unlock(&mutex);
    printf("leave hold_mutex [caller %s]\n", caller[c]);
}

static void* mutex_thread(void* arg) {
    hold_mutex(0);
}

static void* sleep_thread(void* arg) {
    sleep(10);
}

static void signal_handler(int signum) {
    hold_mutex(1);
    signal_handler_exit = 1;
}

int main () {
    signal(SIGUSR1, signal_handler);
    pthread_create(&mutex_tid, NULL, mutex_thread, NULL);
    pthread_create(&sleep_tid, NULL, sleep_thread, NULL);
    pthread_kill(sleep_tid, SIGUSR1);
    pthread_join(mutex_tid, NULL);
    pthread_join(sleep_tid, NULL);
    return 0;
}
```

- ```shell
  ming@ming-virtual-machine:~/Documents/Linux/Linux环境编程/PageResume$ ./dead_locak 
  enter hold_mutex [caller mutex_thread]
  enter hold_mutex [caller signal handler]
  ```

  

##### 1.4.6 阻塞与非阻塞

- Linux 环境下 所有的 I/O 系统调用默认都是阻塞的
- 阻塞的系统调用是指 当进行系统调用时 除非出错(被信号打断也视为出错) 进程将会一直陷入内核态直到调用完成
- 非阻塞的系统调用是指无论 I/O 操作成功与否 调用都会立刻返
  回

##### 1.4.7 同步与非同步

- 以系统调用 read 为例。阻塞的 read 会一直陷入内核态直到 read 返回;而非阻塞的 read 在数据未准备
  好的情况下,会直接返回错误,而当有数据时,非阻塞的 read 同样会一直陷入内核态,直到 read 完成。
  这个 read 就是同步的操作,即 I/O 的完成是在当前执行流程下同步完成的
- 如果是非同步即异步,则 I/O 操作不是随系统调用同步完成的。调用返回后, I/O 操作并没有完成,
  而是由操作系统或者某个线程负责真正的 I/O 操作,等完成后通知原来的线程



### 2. 文件I/O

> 文件 I/O 是操作系统不可或缺的部分,也是实现数据持久化的手段。对于 Linux 来说,其 “ 一切皆是 文件 ” 的思想,更是突出了文件在 Linux 内核中的重要地位

#### 2.1 文件、文件描述符、文件表

- Linux 内核将一切视为文件
  - 物理文件 -> 狭义
  - 设备、管道、内存 -> 广义  VFS机制 -> 以文件系统的形式挂载在内核中，对外提供一致的文件操作接口
- 文件描述符
  - 数值（本质是句柄） 通过他得到用于管理文件的真正的数据结构
  - 好处
    - 增加安全性
    - 增加可扩展性
  - 每个进程都维护一个文件表 以便维护该进程打开文件的信息，包括打开的文件个数、每个文件的偏移量等信息



#### 2.2 内核文件表的实现

![image-20210427140421287](/home/ming/snap/typora/33/.config/Typora/typora-user-images/image-20210427140421287.png)

```c

struct files_struct init_files = {
    .count = ATOMIC_INIT(1),
    .fdt = &init_files.fdtab,
    .fdtab = {
        .max_fds = NR_OPEN_DEFAULT,
        .fd = &init_files.fd_array[0],.close_on_exec = (fd_set *)&init_files.close_on_exec_init,
        .open_fds = (fd_set *)&init_files.open_fds_init,
    },
    .file_lock = __SPIN_LOCK_UNLOCKED(init_task.file_lock),
};
```

```c
newf = kmem_cache_alloc(files_cachep, GFP_KERNEL);
if (!newf)
    goto out;
atomic_set(&newf->count, 1);
spin_lock_init(&newf->file_lock);
newf->next_fd = 0;
new_fdt = &newf->fdtab;
new_fdt->max_fds = NR_OPEN_DEFAULT;
new_fdt->close_on_exec = (fd_set *)&newf->close_on_exec_init;
new_fdt->open_fds = (fd_set *)&newf->open_fds_init;
new_fdt->fd = &newf->fd_array[0];
new_fdt->next = NULL;
```

#### 2.3 打开文件

##### 2.3.1 open

```c
int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);
```

示例

```c
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
int main(void) {
    int fd = open("test_open.txt", O_CREAT, 0644, "test");
    close(fd);
    return 0;
}
```

##### 2.3.2 close

```

```

> "Linux 文件描述符策略永远选择最小的可用的文件描述符 " ——这也是 POSIX 标准规定的

> 内核可以尽早地让错误发生 对于应用开发人员来说,会是一个福 音 因为调试bug的时候  bug 距离造成错误的地点越近 时间发生得越早  就越容易找到根本原因 这也是为什么释放内存以后,要将指针置为 NULL 的原因

- 遗忘close造成的问题

  - 文件描述符始终没有释放

  - 用于文件管理的某些内存结构没有被释放

    - 对于普通进程来说，即使忘记关闭文件，当进程退出时，Linux内核也会自动关闭文件，释放内存，但是对于一个常驻进程，问题就会变得严重

  - 两种情况

    - 第一种情况

      - ```c
        int expand_files(struct files_struct *files, int nr) {
            struct fdtable *fdt;
            fdt = files_fdtable(files);
            /*
            * N.B. For clone tasks sharing a files structure, this test
            * will limit the total number of files that can be opened.
            */
            if (nr >= rlimit(RLIMIT_NOFILE))
            return -EMFILE;
            /* Do we need to expand? */
            if (nr < fdt->max_fds)
            return 0;
            /* Can we expand? */
            if (nr >= sysctl_nr_open)
            return -EMFILE;
            /* All good, so we try */
            return expand_fdtable(files, nr);
        }
        ```

        

    - 第二种情况

      - ```c
        struct file *get_empty_filp(void) {
            const struct cred *cred = current_cred();
            static long old_max;
            struct file * f;
            /*
            * Privileged users can go above max_files
            */
            /* 这里对打开文件的个数进行检查,非特权用户不能超过系统的限制
            */
            if (get_nr_files() >= files_stat.max_files && !capable(CAP_SYS_ADMIN)) {
            /*
            再次检查per cpu 的文件个数的总和,
            为什么要做两次检查呢。后文会详细介绍
            */
            if (percpu_counter_sum_positive(&nr_files) >= files_stat.max_files)
            goto over;
            }
            /* 未到达上限,申请一个新的
            file 结构
            */
            f = kmem_cache_zalloc(filp_cachep, GFP_KERNEL);
            if (f == NULL)
            goto fail;
            /* 增加
            file 结构计数
            */
            percpu_counter_inc(&nr_files);
            f->f_cred = get_cred(cred);
            if (security_file_alloc(f))
            goto fail_sec;
            INIT_LIST_HEAD(&f->f_u.fu_list);
            atomic_long_set(&f->f_count, 1);
            rwlock_init(&f->f_owner.lock);
            spin_lock_init(&f->f_lock);
            eventpoll_init_file(f);
            /* f->f_version: 0 */
            return f;
            over:
            /* 用完了
            file 配额,打印
            log 报错
            */
            /* Ran out of filps - report that */
            if (get_nr_files() > old_max) {
            pr_info("VFS: file-max limit %lu reached\n", get_max_files());
            old_max = get_nr_files();
            }
            goto fail;
            fail_sec:
            file_free(f);
            fail:
            return NULL;
        }
        ```

        为什么要做两次检查？

        > 对于 file 的个数, Linux 内核使用两种方式来计数。一是使用全局变量,另外一个是使用 percpu 变量。更新全局变量时,为了避免竞争,不得不使用锁,所以 Linux 使用了一种折中的解决方案。当percpu 变量的个数变化不超过正负 percpu_counter_batch (默认为 32 )的范围时,就不更新全局变量。这样就减少了对全局变量的更新,可是也造成了全局变量的值不准确的问题。于是在全局变量的 file 个数超过限制时,会再对所有的 percpu 变量求和,再次与系统的限制相比较。想了解这个计数手段的详细信息,可以阅读 percpu_counter_add 的相关代码



#### 2.4 查找文件资源泄漏

- 错误程序示例

  ```c
  #include <stdlib.h>
  #include <stdio.h>
  #include <unistd.h>
  
  int main(void) {
      int cnt = 0;
      while (1) {
      char name[64];
      snprintf(name, sizeof(name),"%d.txt", cnt);
      int fd = creat(name, 644);
      sleep(10);
      ++cnt;
      }
      return 0;
  }
  ```

- 运行程序 得到当前程序在后台运行的pid

  ```shell
  ./file.elf &
  [1] 3000
  ```

- 利用lsof 查看相关信息

```shell
[fgao@fgao chapter1]#lsof -p 3000
COMMAND    PID USER   FD   TYPE             DEVICE SIZE/OFF    NODE NAME
error_fil 4289 ming  cwd    DIR                8,5     4096 1718805 /home/ming/Documents/Linux/Linux环境编程/File
error_fil 4289 ming  rtd    DIR                8,5     4096       2 /
error_fil 4289 ming  txt    REG                8,5    16800 1722052 /home/ming/Documents/Linux/Linux环境编程/File/error_file_create
error_fil 4289 ming  mem    REG                8,5  2029224 2365272 /usr/lib/x86_64-linux-gnu/libc-2.31.so
error_fil 4289 ming  mem    REG                8,5   191472 2365056 /usr/lib/x86_64-linux-gnu/ld-2.31.so
error_fil 4289 ming    0u   CHR              136,1      0t0       4 /dev/pts/1
error_fil 4289 ming    1u   CHR              136,1      0t0       4 /dev/pts/1
error_fil 4289 ming    2u   CHR              136,1      0t0       4 /dev/pts/1
error_fil 4289 ming    3w   REG                8,5        0 1722053 /home/ming/Documents/Linux/Linux环境编程/File/0.txt
error_fil 4289 ming    4w   REG                8,5        0 1722054 /home/ming/Documents/Linux/Linux环境编程/File/1.txt
error_fil 4289 ming    5w   REG                8,5        0 1722055 /home/ming/Documents/Linux/Linux环境编程/File/2.txt
error_fil 4289 ming    6w   REG                8,5        0 1722056 /home/ming/Documents/Linux/Linux环境编程/File/3.txt
error_fil 4289 ming   21r   REG                7,0   172330    3343 /snap/code/62/usr/share/code/v8_context_snapshot.bin
error_fil 4289 ming   31r   REG               0,23        0   57093 /proc/3048/statm
error_fil 4289 ming   32r   REG               0,23        0   58025 /proc/3048/status
error_fil 4289 ming   33u   REG                8,5    10543 2228243 /tmp/.org.chromium.Chromium.jZyP5t (deleted)
error_fil 4289 ming   34u  unix 0x0000000000000000      0t0   57095 type=STREAM
error_fil 4289 ming   40r   REG                7,0  5462571    3067 /snap/code/62/usr/share/code/resources/app/node_modules.asar
error_fil 4289 ming   41r   REG                7,0  5462571    3067 /snap/code/62/usr/share/code/resources/app/node_modules.asar
error_fil 4289 ming   42r   REG                8,5   389764  525943 /usr/share/fonts/truetype/liberation2/LiberationSerif-Regular.ttf
error_fil 4289 ming  103r   REG   
```



#### 2.5 文件偏移

> 文件偏移是基于某个打开文件来说的,一般情况下,读写操作都会从当前的偏移位置开始读写(所以 read 和 write 都没有显式地传入偏移量),并且在读写结束后更新偏移量

##### 2.5.1 lseek

- 原型分析

```c
off_t lseek(int fd, off_t offset, int whence);
offset 表示偏移位置 正负值都可以
whence 可以为三个值
    SEEK_SET	文件的起始位置
    SEEK_CUR	文件的当前位置
    SEEK_END	文件的末尾
成功执行，返回新的文件偏移量
Linux 3.1 以后 增加了两个whence属性
    SEEK_DATA	查找文件中的数据
    SEEL_HOLE	查找文件中的空洞
```

- 小心lseek的返回值
  - 一般情况下,对于普通文件来说, lseek 都是返回非负的整数,但是对于某些设备文件 来说,是允许返回负的偏移量。因此要想判断 lseek 是否真正出错,必须在调用 lseek 前将 errno 重置为 0 , 然后再调用 lseek ,同时检查返回值是否为 -1 及 errno 的值。只有当两个同时成立时,才表明 lseek 真正出
    错了
  - 文件偏移都是内核的概念,所以 lseek 并不会引起任何真正的 I/O 操作

> 注意：
> 当 lseek 执行成功时,它会返回最终以文件起始位置为起点的偏移位置。如果出错,则返回 -1 ,同时 errno 被设置为对应的错误值

- 源码分析
  - lseek -> read_write.c -> SYSCALL_DEFINE3(...) -> vfs_llseek(...) -> default_llseek(...)
- 更新...



#### 2.6 读取文件

- 原型

  ```c
  ssize_t read(int fd, void *buf, size_t count);
  //read 尝试从 fd 中读取 count 个字节到 buf 中,并返回成功读取的字节数,同时将文件偏移向前移动相同的字节数。返回 0 的时候则表示已经到了"文件尾"。 read 还有可能读取比 count 小的字节数
  ```

- 正确处理错误事件

  ```c
  error
      EAGAIN
      EWOULDBLOCK
      EINTR
  前两者是由于当前fd为非阻塞且没有可读数据时返回的
  后者是由于read被信号中断所造成的
  ```

  

- 源码跟踪

```
read() -> SYSCALL_DEFINE3(...) -> vfs_read()
```

- 部分读取

  - 调用链 (socket文件系统中UDP协议的read实现)

    ```c
    sock_aio_read -> do_sock_read -> __sock_recvmsg -> __sock_recvmsg_nose -> udp_recvmsg
    // 只定义了aio_read操作，没有定义普通的read函数
    // do_sync_read 会利用aio_read来实现同步
    ```

    

  - 当UDP报文的数据长度小于len时，就只会复制真正的长度，对于read操作来说，返回的读取字节小于参数count

  - socket 文件系统, TCP 套接字的读取操作也会与 UDP 不同，当 TCP 的 fd 的数据不足时, read 操作极可能会阻塞,而不是直接返回

    > 注意：
    >
    > - TCP 是否阻塞,取决于当前缓存区可用数据多少,要读取的字节数,以及套接字设置的接收低水位大小
    > - 在调用 read 的时候,只能根据 read 接口的说明,小心处理所有的情况,而不能主观臆测内核的实现

#### 2.7 写文件（可参考read源码进行分析）

- 文件追加功能 （待更新）

- 文件的原子读写

  - 源码分析

    ```c
    SYSCALL_DEFINE(pread64)(unsigned int fd, char __user *buf,size_t count, loff_t pos) {
        struct file *file;
        ssize_t ret = -EBADF;
        int fput_needed;
        if (pos < 0)
        	return -EINVAL;
        file = fget_light(fd, &fput_needed);
        if (file) {
            ret = -ESPIPE;
            if (file->f_mode & FMODE_PREAD)
            ret = vfs_read(file, buf, count, &pos);
            fput_light(file, fput_needed);
        }
        return ret;
    }
    ```

    

#### 2.8 文件描述符的复制

- 系统调用原型

  ```c
  int dup(int oldfd);
  int dup2(int oldfd, int newfd);
  int dup3(int oldfd, int newfd, int flags);
  /*
  ·dup 会使用一个最小的未用文件描述符作为复制后的文件描述符。
  ·dup2 是使用用户指定的文件描述符 newfd 来复制 oldfd 的。如果 newfd 已经是打开的文件描述
  符, Linux 会先关闭 newfd 然后再复制 oldfd
  .dup3 只有定义了feature 宏"_GNU_SOURCE" 才可以使用,它比 dup2 多了一个参数,可以指定标志 —— 不过目前仅仅支持 O_CLOEXEC 标志,可在 newfd 上设置 O_CLOEXEC 标志。定义 dup3 的原因与 open 类似,可以在进行 dup 操作的同时原子地将 fd 设置为 O_CLOEXEC ,从而避免将文件内容暴露给子进程
  */
  ```

  

- 源码分析（待更新）

#### 2.9 文件数据的同步

- 系统调用原型

```c
void sync(void);
int fsync(int fd);
int fdatasync(int fd);
SYSCALL_DEFINE0(sync) {
    ...
}
SYSCALL_DEFINE1(fsync, unsigned int, fd) {
	return do_fsync(fd, 0);
}
SYSCALL_DEFINE1(fdatasync, unsigned int, fd) {
	return do_fsync(fd, 1);
}
```

- 作用
  涉及程序效率问题时需要注意文件数据的同步问题，以及缓冲区的处理

#### 2.10 文件的元数据

> 其包括文件的访问权限、上次访问的时间戳、所有者、所有组、文件大小等信息

##### 2.10.1 获取文件的元数据

- 原型

  ```c
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <unistd.h>
  int stat(const char *path, struct stat *buf);
  int fstat(int fd, struct stat *buf);
  int lstat(const char *path, struct stat *buf);
  // stat 结构如下
  struct stat {
      dev_t st_dev;           // ID of device containing file
      ino_t st_ino;           // inode number
      mode_t st_mode;         // protection
      nlink_t st_nlink;       // number of hard links
      uid_t st_uid;           // user ID of owner
      gid_t st_gid;           // group ID of owner
      dev_t st_rdev;          // device ID (if special file)
      off_t st_size;          // total size, in bytes
      blksize_t st_blksize;   // blocksize for file system I/O
      blkcnt_t st_blocks;     // number of 512B blocks allocated
      time_t st_atime;        // time of last access
      time_t st_mtime;        // time of last modification
      time_t st_ctime;        // time of last status change
  };
  ```

  > 注意：st_mode 不只用于权限管理，还包含文件类型 普通文件或是目录 

##### 2.10.2 内核如何维护文件的元数据

##### 2.10.3 权限位解析

- SUID权限位
- SGID权限位
- Stricky权限位

#### 2.11 文件截断

##### 2.11.1 truncate 与 ftruncate 介绍

```c
#include <unistd.h>
#include <sys/types.h>
int truncate(const char *path, off_t length);
int ftruncate(int fd, off_t length);
// 两者之间的唯一区别在于, truncate 截断的是路径 path 指定的文件, ftruncate 截断的是 fd 引用的文件
```

##### 2.11.2 源码分析 （待更新）