Install python 2.7
cd phthon27\Scripts
easy_install pip
pip install xlrd

python protobuf安装
pip install protobuf==3.6.1
pip list
验证是否成功：打开cmd，输入python，输入import google.protobuf，如果不报错则安装成功

步骤:
(1)excel模板:
第1行==>英文名称(建议驼峰格式)
第2行==>服务器,客户端是否共用
第3行==>支持proto基本数据类型(bool,int32,uint32,int64,uint64,float,double,string)
第4/n行==>每条记录

(2)excel2proto
excel转成对应语言的proto

(3)binary
生成proto反序列化的数据

(4)一级管理
生成相应语言的一级管理配置代码
不同语言生成不同的管理器


