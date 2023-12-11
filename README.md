# An Introduction to Socket Programming

該篇文章的目的在於簡介 socket 的觀念和實作出一個透過 socket 使用 TCP protocol 來從 client 傳輸 "Hello, world!" 字串至 server 的簡易範例。

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

## Server 和 Client 端的連線

### sockaddr

開啟 socket 後，下一步就是要設定 server 的 address 相關資訊等，才有辦法進行連線。

而在 ```<netinet/in.h>``` 裡，有定義了 ```sockaddr``` 資料結構，來幫助我們儲存如地址的相關資訊。

```sockaddr``` 的定義參考 [man page](https://man7.org/linux/man-pages/man3/sockaddr.3type.html) 如下：

```
struct sockaddr {
    sa_family_t     sa_family;      /* 2 bytes, Address family */
    char            sa_data[14];      /* 14 bytes, Socket address */
};
```
### sockaddr_in

但為了可讀性的增加，通常使用另一個將 socket address 定義的更細的 structure 為 ```sockaddr_in``` 用來儲存和 IPv4 協定相關的結構如下：

```
struct sockaddr_in {
    // sa_family_t sin_family
    unsigned short int sin_family; // 2 bytes
    unsigned short int sin_port;   // 2 bytes
    struct in_addr sin_addr;       // 4 bytes
    unsigned char sin_zero[8];
};

struct in_addr {
    unsigned long s_addr;          // load with inet_pton()
};
```

可以注意到的是，為了讓 ```sockaddr_in``` 和 ```sockaddr``` 長度相同，最後加入了 ```sin_zero``` 來作為填充。

### Example 

下面為使用 ```sockaddr_in``` 的一例：

```
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define IP "0.0.0.0"
#define PORT 8080

int main(void) {
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(IP);
	addr.sin_port = htons(PORT);
}
```
我們在這裡就建立了一個 ```sockaddr_in``` 結構，用作儲存 server 的相關資料。

可以看到這裡指定 sin_family 為 ```AF_INET```，代表這裡使用的是 IPv4 的網路協定傳輸，

### ip address 轉換

再來是我們要給定 server 的 IP address，因為在 IPv4 協定裡頭， address 為 32 bits 的數值，但通常我們為了方便使用，會給定一個字串的 IP，我們需要將字串轉換為 IPv4 協定中的數值表示，

這裡我們用了定義在 ```<arpa/inet.h>``` 裡的 ```inet_addr``` 來將給定的 server IP 字串轉成 ```in_addr_t``` 的數值形式。

如果 inet_addr 轉成功，會回傳型態為 ```in_addr_t``` 的數值，並且會依照 network order，但如果給定字串為不合法的 address，會回傳 ```INADDR_NONE```。

下面為一例子：

```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

int main(void) {
	char *ip = "0.0.0.0";
	in_addr_t addr;
	if ((addr = inet_addr(ip)) == INADDR_NONE) {
		perror("inet_addr");
		exit(1);
	}
	printf("inet_addr: %s\n", ip);
	printf("inet_addr: %u\n", ntohl(addr));

	char *invalid_ip = "0.0.0.256";
	if ((addr = inet_network(invalid_ip)) == INADDR_NONE) {
		perror("inet_network");
		exit(1);
	}
	return 0;
}
```

### Byte Order

再來是我們要使用 ```htons``` 來解決 byte order 的問題， ```htons``` 全寫為 host to network short，

原因為各個 host 可能使用不同的 byte order 來表示資料，如 big-edian 或是 little-edian 等等，這依照 host 的機器型號和實作而定，

為了讓資料傳輸不會受到 host byte order 的影響，我們在網路傳輸時，會使用統一的 byte order 形式，稱為 network byte order，來避免相容性的問題。

這裡的 ```htons``` 就用做將 host byte order 轉成 network byte order。

### Server: Bind

再來，建立好 ```sockaddr_in``` 資料結構，我們表達好 server 端的資訊後，我們要在 server 端將前面創建好的 socket 綁定在主機的某個 port 上。

這樣一來才能接收 client 端傳送來的資料，並在那個 port 上處理。

而 bind 的 prototype 一樣定義在 ```<sys/socket.h>``` 中，詳細可以看 [man page](https://man7.org/linux/man-pages/man2/bind.2.html)。

其 prototype 如下：

```
int bind(int sockfd, const struct sockaddr *addr,
                socklen_t addrlen);
```

其中：

- sockfd：剛剛創建的 socket 的 file descriptor
- addr：剛剛建立好的 ```sockaddr_in```，用作提供 bind 要綁定的 address 還有 port，注意他 prototype 是使用 ```sockaddr```，但因為我們 ```sockaddr_in``` 有做填零成相同大小，可以直接轉型使用
- addrlen：```sockaddr_in``` 的大小

回傳值如果成功為 0，否則為 -1。

#### Example

```
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080

int main(void) {
	int sock_fd;

	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_port = htons(PORT),
		.sin_addr.s_addr = INADDR_ANY
	};

	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	if (bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		exit(1);
	}
	printf("sock_fd: %d\n", sock_fd);
	printf("bind success\n");
	return 0;
}
```

這樣一來，server 就能成功綁定到主機上給定的 port。

注意到的是，在設定 ```sockaddr_in``` 時，我們設定 ```addr.sin_addr.s_addr``` 為 ```INADDR_ANY```，即我們沒有指名 IP 位址，直接讓 kernel 決定。

### Server: Listen

再來，我們綁定 server 這端到指定的位址或是 port 之後，我們必須要讓 server 持續監聽這個 port，來知道說是否有資料進來。

這裡就使用了

### Client: Connect

再來是 client 這一端，同樣需要

## Envirnoment Setup

### Build the Docker Image

1. Place your ```server.c``` and ```client.c``` in the same directory as the ```Dockerfile```.
2. Build the Docker image by running the following command in the directory containing your Dockerfile:

```
docker build -t socket-programming .
```

### Running the Containers

1. Run the Server Container:

- Start the server container:
```
docker run -it --rm --name server-container -p 8080:8080 socket-programming
```
This command starts the server and exposes port 8080 for communication.

2. Run the Client Container:

- In a new terminal, start the client container:

```
docker run -it --rm --name client-container --link server-container socket-programming ./client
```

## References

- [socket man page](https://man7.org/linux/man-pages/man2/socket.2.html)
- [TCP Socket Programming 學習筆記](http://zake7749.github.io/2015/03/17/SocketProgramming/)
- [socket programming](https://github.com/davidleitw/socket)
- [清大作業系統課程](https://ocw.nthu.edu.tw/ocw/index.php?page=course&cid=141)