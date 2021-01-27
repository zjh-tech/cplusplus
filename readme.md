<!--
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-12-17 11:28:59
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-26 14:52:38
 -->
C#:
组件和框架跨平台: .Net Core 3.1 
软件环境: 
        window==>Visual Studio 2019
        linux==> dotnet(yum install dotnet-sdk-3.1)   
        
组件: 日志,定时器,Tcp网络等

框架: 服务发现,业务Session,逻辑LogicServer等

工具类: 打表工具

引用的Dll:  
    需求: Debug调用Debug的Dll,Release调用Release的Dll   
    解决方案: *.csproj将Debug/Release改为$(Configuration)

Go:
组件和框架跨平台
软件环境：
        go1.14.2

组件:
    日志,定时器,数据库,Redis,Tcp网络,Http(Https)等
    数据库:同步异步API,支持分库分表,事务,数据库语句自动拼接Api
框架:
    服务发现,业务Session,逻辑LogicServer,信号,gops性能监控,唯一ID生成,等
    
工具类: 打表工具
    
C++:
组件
    日志,定时器,数据库,内存池,Tcp网络等    
    内存池: 内存碎片,性能问题等
    数据库: 同步异步API,支持分库分表,事务,数据库语句自动拼接Api,业务使用索引,管理sql工具(liquibase)
框架:
    服务发现,业务Session,逻辑LogicServer,信号,唯一ID生成,core宕机Asan,配置热更,
    GPerfTools优化内存和CPU等
    解析xml,json,yaml等
工具类: 打表工具

日志:
    {}代替解决手动类型转换,参数缺省宕机    
网络模块:
    断包,粘包，主动被动断开,主动断开支持发送数据后断开,本地字节序和网络字节序转换
    加解密,压缩,地址端口重用,心跳,连接秘钥验证,白黑名单,无锁队列等
定时器:
    时间轮
    Timer用内存池来分配
    计算Delay的API
    执行时间太长导致误差越来越大
    attach数据重复使用
    时间回滚或者时间前进
内存池: (内存频繁创建销毁)
    内存碎片,扩容时创建一块大的连续的内存
    内存占用大:缩减GC策略
    取出:解决脏数据问题==>内存memset,支持多参构造等  
    放入:解决检查地址,多次放入等问题
    监控内存:定时打印使用情况
    是否支持线程安全
信号:
    core生产,加载配置,关闭服务器,生产CPU,内存性能报告,信号和主线程不是在同一线程，存在锁竞争

打表工具:
    支持C#,Go,C++语言
    excel生成proto
    binary生成(反射)
    根据不同的开发语言生成不同的配置管理器代码

protobuf(版本号:3.11.4):
下载地址: 
    https://github.com/protocolbuffers/protobuf/releases
编译静态库:
    linux:   cmake -DCMAKE_BUILD_TYPE=Release ../cmake/ 或者 cmake -DCMAKE_BUILD_TYPE=Debug ../cmake/
    window:  下载cmake,使用cmake生成sln功能,编译sln
             下载https://cmake.org/download/
             参照https://blog.csdn.net/hp_cpp/article/details/81561310

mysql:
下载 https://dev.mysql.com/downloads/mysql/
    
mysql: mysqlserver mysqlclient 版本要对应
redis: redisserver redisclient 版本要对应

protobuf安装
yum install autoconf automake libtool curl make g++ unzip
git clone https://github.com/protocolbuffers/protobuf.git
cd protobuf
git submodule update --init --recursive
./autogen.sh
./configure --prefix=/usr/local/protobuf
make && make install
vim /etc/profile
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/protobuf/lib
export LIBRARY_PATH=$LIBRARY_PATH:/usr/local/protobuf/lib
export PATH=$PATH:/usr/local/protobuf/bin
source /etc/profile

C++ 嵌套调用C# 
netcore
linux : dotnet publish -c release --self-contained  -r centos.7-x64
参考https://docs.microsoft.com/zh-cn/dotnet/core/tutorials/netcore-hosting#prerequisites
参考https://yizhang82.dev/hosting-coreclr

优化:
    内存对齐,减少内存消耗，提交访问效率    

待开发:
微服务: 排行榜跳表
屏蔽字: 字典树
数据冗余
集群
C++/Lua Sol2
Static被Dll代替
openssl被boringssl代替
protobuf和json 互转
kafka
进程热更
Udp: Kcp接入
#C++和Lua交互: 
    C++调用Lua通用接口 
    tolua++生成Lua调用C++的接口
    tolua++不支持智能指针,sol2支持智能指针,sol2没此工具
    纯lua开发适合用sol2
常见问题:
服务器与客户端共用一套逻辑: C#服务器,嵌套调用(C++),脚本语言
    帧同步和状态同步
    九宫格或十字链表视野同步
    全球同服/区服
    进程热更

主线程Run: 模块之间运行时间均匀,添加循环次数限制

C++模板类支持多参: tuple


加密 boringssl openssl 
cmake生成vs2019解决方案
C++与C++交互: tolua++ Sol2(3.0)
https://github.com/ThePhD/sol2
http://www.lua.org/versions.html#5.4


框架是通过服务发现配置动态设计的


第三方插件,编译成静态库,动态库问题?
如果编译成动态库也会自带一个静态库:通常静态库作为连接,动态库作为运行

Window Vscode:
方法一:测试可行
生成visual studio 工程
属性==>配置属性==>常规==>配置类型==>选择静态库
属性==>配置属性==>C/C++==>代码生成==>运行时 Debug:/MTd Release:/MT 跟windows.cmake里面一样

方法二:测试有问题
CMakeLists.txt中仅可以修改生成共享库标志,Debug前缀,不能修改/MTd /MT
option(BUILD_SHARED_LIBS "build as shared library" OFF)
set(CMAKE_DEBUG_POSTFIX "")

curl编译
vs2019打开projects\Windows\VC15\curl-all.sln
选择Lib Debug/Release编译动态库, lib用于连接,dll用于运行
