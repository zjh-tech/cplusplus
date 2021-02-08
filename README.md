#cplusplus 库和框架

##库:
###Common:
    内存管理: 
            解决内存频繁创建销毁,导致内存碎片问题
            内存碎片:扩容时创建一块大的连续的内存
            内存占用大:缩减GC策略
            取出:解决脏数据问题==>内存memset,支持多参构造...
            放入:解决检查地址,多次放入等问题
            监控内存:定时打印内存使用情况
            不支持线程安全
    无锁队列
    反射
    类型转换转换    

####log: 
      {}代替解决手动类型转换错误,参数缺省宕机
####timer: 
    时间路定时器
    调时间: 时间回滚,前进
    Delay的API
    执行时间太长导致的误差越来越大
    attach使用Lambda代替vector<any>
####tcp:
    粘包，断包
    主动被动断开,主动断开时支持发送数据
    本地字节序和网络字节序转换
    监听地址端口重用    
    加解密,压缩,coder通用接口
####mysql:
    同步,异步API
    支持分库分表,事务
    自动拼接Sql语句工具
    liquibase:分库分表sql的管理
    DBRecordSet返回的VarType支持基础类型的转换

##框架:
    ###httpclient
    ###服务发现: 支持tcp/http二种方式
    ###策划配置加载和热更        
    ###信号: 
         Asan生成core文件
         策划配置热更
         GPerfTools对memory和cpu优化报告
         信号和主线程不是同一线程，存在锁竞争
    ###支持xml,json,yaml配置解析
    ###protobuf3.0以上版本使用
    ###唯一ID生成
    ###tcp:        
        心跳,验证
        白黑名单,验证超时待开发
        csclientsession: 用于Unity与Server之间
        ssclientsession: 用于SdkServer与Server之间
        ssserversession: 用于Server与Server之间
        Package: 包头 + 包体,包体: attach + other
        Coder: 自定义Package
        TimerMeter: 监控逻辑模块时间

##工具:
        打表工具: excel转protobuf 自动化代码生成,支持热更

##计划:    
redis: 
    hiredis
    C++与Lua: sol2
    组件QPS测试
    微服务