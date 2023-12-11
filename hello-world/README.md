# Hello World! An Introduction to Socket Programming

## Communication Model

在計算機的世界裡頭，我們常常需要利用多個 process 來完成一個 task，但這時，這些 process 需要資料之間的交換，需要提供一個溝通的管道給這些 process 來溝通，傳遞之間的資料。

而這裡，主要的溝通模型可以分成兩種：

1. shared memory
2. message passing

shared memory利用memory的address來取得資料，不同的process間會共享一塊記憶體空間，而其最大的缺點就是同步化的問題，好處就是因為是直接利用memory access，所以效率高。

message passing必須要透過system call，而且要做copy，會有kernel的介入，所以通常會比較慢一些，但是當資料量不多的時候，有時候用 message passing 可以避開 synchronization 的問題，相對可能會快一些。

## Socket

而 message passing 由作業系統主要有兩種方式：socket 和 remote procedure call。

這裡主要要介紹的就是在 linux 作業系統中，如何使用 socket 來做到同一台主機的 processes 間溝通，或是網路間的主機上的 process 的溝通。

透過 linux 系統中已經定義好的 socket API ，來使用不同的網路協定來幫助我們傳輸資料。

首先，在 linux 作業系統中，要使用 socket API，需要先引入標頭檔，

```
#include <sys/socket.h>
```

在該標頭檔定義了下面的 socket prototype 如下，幫助我們在 kernel 中建立一個 socket：

```
int socket(int domain, int type, int protocol);
```

其中，裡面的 argument 分別有，詳見 [man page](https://man7.org/linux/man-pages/man2/socket.2.html)：

### Domain

定義了該 socket 中溝通的範圍和協定，主要的 protocol family 都會定義在 ```<sys/socket.h>```，常見的主要有：

- AX_LOCAL：主要使用於同一台主機間不同 process 的傳輸
- AF_INET：主要使用於不同主機上不同 process 的傳輸，使用 IPv4 協定
- AF_INET6 ：同上，差別在於使用  IPv6 協定

### Type

定義了該 socket 資料傳輸的方式，主要常見的有：
- SOCK_STREAM：資料會以 byte stream 傳輸。
- SOCK_DGRAM：資料會以 datagram 傳輸。

### Protocol

定義了該 socket 使用的 protocol，但因為通常 domain 會和 protocol 綁定，因此一般來說可以直接給 ```0``` 值，直接讓 kernel 判定 protocol 為何。

### Return Value

而如果成功， ```socket``` 的回傳值會為創建 socket 的 file descriptor，失敗會回傳 -1 ，可以使用 ```perror``` 來捕捉 errno print 出 error message。

### Example

```
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

int main(void) {
	int socket_fd;
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}
	printf("socket_fd = %d\n", socket_fd);
	return 0;
}
```

# References

- [socket man page](https://man7.org/linux/man-pages/man2/socket.2.html)
- [TCP Socket Programming 學習筆記](http://zake7749.github.io/2015/03/17/SocketProgramming/)
- [socket programming](https://github.com/davidleitw/socket)
- [清大作業系統課程](https://ocw.nthu.edu.tw/ocw/index.php?page=course&cid=141)