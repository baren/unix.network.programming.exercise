# unix.network.programming.exercise
exercise
## linux下关闭socket连接端口占用问题。

在linux开发socket应用时，经常遇到刚关闭的网络端口，立即打开，会出现端口占用问题，必须等一段时间才能再次启动同一端口下的应用。

这有时对调试不方便。其原因在于网络连接的关闭方式。如果服务器先关闭，则先关闭的一方会处于“TIME_WAIT”状态，这个状态下，需要经过2MSL的超时，网络连接状态才能转换到CLOSE状态。因此为了避免服务端连接总出现端口占用情况，可以先关闭客户端。比如：

服务端启动9090端口。并有一个客户端连接进来，通过netstat命令可以看出：

```shell
@ubuntu:~/work/unpproj$ netstat -a | grep 9090
tcp        0      0 *:9090                  *:*                     LISTEN     
tcp        0      0 localhost:9090          localhost:47727         ESTABLISHED
tcp        0      0 localhost:47727         localhost:9090          ESTABLISHED
```

这时，如果关闭服务端，则服务端是主动关闭方，因此会处于TIME_WAIT状态。比如：

```shell
@ubuntu:~/work/unpproj$ netstat -a | grep 9090
tcp        0      0 localhost:9090          localhost:47727         TIME_WAIT
```

一旦处于TIME——WAIT状态，则根据tcp规范，需要等2MSL超时，才会释放端口，因此必须等待一段时间再启动才可以，不然一直报错端口占用。

如果先关闭客户端连接，等所有客户端连接都关闭了，再关闭服务端连接，这时就不会出现这种情况，可以立即启动服务端而不会发生端口占用情况。

比如：初始两个客户端连接进来。

```shell
@ubuntu:~/work/unpproj$ netstat -a | grep 9090
tcp        0      0 *:9090                  *:*                     LISTEN     
tcp        0      0 localhost:47788         localhost:9090          ESTABLISHED
tcp        0      0 localhost:47787         localhost:9090          ESTABLISHED
tcp        0      0 localhost:9090          localhost:47788         ESTABLISHED
tcp        0      0 localhost:9090          localhost:47787         ESTABLISHED
```

关闭一个客户端：

```shell
@ubuntu:~/work/unpproj$ netstat -a | grep 9090
tcp        0      0 *:9090                  *:*                     LISTEN     
tcp        0      0 localhost:47788         localhost:9090          TIME_WAIT  
tcp        0      0 localhost:47787         localhost:9090          ESTABLISHED
tcp        0      0 localhost:9090          localhost:47787         ESTABLISHED
```

会发现主动关闭方（客户端）处于TIME_WAIT状态，而被动关闭方（服务端）则立即关闭了。

关闭第二个客户端：

```shell
@ubuntu:~/work/unpproj$ netstat -a | grep 9090
tcp        0      0 *:9090                  *:*                     LISTEN     
tcp        0      0 localhost:47787         localhost:9090          TIME_WAIT
```

同样，处于TIME_WAIT状态。

最后关闭服务端：

```shell
@ubuntu:~/work/unpproj$ netstat -a | grep 9090
@ubuntu:~/work/unpproj$
```
则没有任何连接了，因此这种情况下立即启动服务端，由于没有端口占用，所以能够立即启动。

