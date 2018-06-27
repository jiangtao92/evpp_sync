## evpp_sync
在evpp网络库的基础上,实现同步请求的发送接收封装

## 依赖
* evpp: 使用vcpkg安装: vcpkg.exe install evpp
* c++11: visual stuido 2015以上

## 底层报文结构

```
|size|tag|type|session|reqidx|data|
size: 报文长度,用于拆分数据流
tag: 特殊标记,校验使用
session: 会话编号
reqidx: 请求编号
data: 消息体
```

## TODO
* 优化,减少内存的memcpy
* server只能可单个客户端通信
* 修改tag值位校验位,确保传输正确
* 添加线程池支持,当前请求处理在io线程中
