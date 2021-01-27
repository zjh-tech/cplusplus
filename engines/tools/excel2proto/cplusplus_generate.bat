set   excel_in="../../../share/excel"

set   temp_proto_out="./config/out/proto"
set   temp_python_out="./config/out/python"
set   temp_binary_out="./config/out/binary"
set   temp_cplusplus_out="./config/out/cplusplus"
set   temp_cplusplus_mgr_out="./config/out/cplusplus_cfg_mgr"
set   cplusplus_out="../../projects/staticdata"

echo
rmdir /s /q  %temp_proto_out%
mkdir %temp_proto_out%
python python\excel2proto.pyc   %excel_in%    %temp_proto_out%
echo
pause 

echo
rmdir /s /q  %temp_python_out%
mkdir %temp_python_out%
rmdir /s /q  %temp_cplusplus_out%
mkdir %temp_cplusplus_out%
rmdir /s /q  %temp_binary_out%
mkdir %temp_binary_out%

protoc.exe  --proto_path %temp_proto_out% --python_out=%temp_python_out%  %temp_proto_out%\*.proto
protoc.exe  --proto_path %temp_proto_out% --cpp_out  %temp_cplusplus_out%  %temp_proto_out%\*.proto
python  python\binary.pyc   %excel_in%\    %temp_python_out%\  %temp_binary_out%\
echo
pause

echo
rmdir /s /q  %temp_cplusplus_mgr_out%
mkdir %temp_cplusplus_mgr_out%
python  python\cplusplus_code.pyc  %temp_binary_out%    %temp_cplusplus_mgr_out%
xcopy  %temp_cplusplus_out%  %cplusplus_out%  /y
xcopy  %temp_cplusplus_mgr_out%  %cplusplus_out%  /y
echo
pause 
