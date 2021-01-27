0:Install python 2.7
0.1: cd phthon27\Scripts
0.2: easy_install pip
0.3: pip install xlrd

实现功能:
1:Excel2Csv.py:    实现excel 转换成csv
2:Excel2Code.py  实现excel 生成config.h/config.cpp代码
3:通过RELOAD_FLAG,UNRELOAD_FLAG来控制重新加载配置
4:ex_config.h/ex_config.cpp来实现二次解析管理