set   excel_in="../../../share/excel"

set   temp_proto_out="./config/out/proto"
set   temp_python_out="./config/out/python"
set   temp_binary_out="./config/out/binary"
set   temp_go_out="./config/out/go"



set   temp_go_mgr_out="./config/out/go_cfg_mgr"
set   go_out="../../projects/config"

echo
rmdir /s /q  %temp_proto_out%
mkdir %temp_proto_out%
python python\excel2proto.pyc   %excel_in%    %temp_proto_out%
echo
pause 

echo
rmdir /s /q  %temp_python_out%
mkdir %temp_python_out%
rmdir /s /q  %temp_go_out%
mkdir %temp_go_out%
rmdir /s /q  %temp_binary_out%
mkdir %temp_binary_out%

protoc.exe  --plugin=protoc-gen-go=.\protoc-gen-go.exe  --proto_path %temp_proto_out%  --go_out   %temp_go_out%    %temp_proto_out%\*.proto
protoc.exe  --proto_path %temp_proto_out% --python_out=%temp_python_out%  %temp_proto_out%\*.proto
python  python\binary.pyc   %excel_in%\    %temp_python_out%\  %temp_binary_out%\
echo
pause

echo
rmdir /s /q  %temp_go_mgr_out%
mkdir %temp_go_mgr_out%
python  python\go_code.pyc  %temp_binary_out%    %temp_go_mgr_out%
xcopy  %temp_go_out%  %go_out%  /y
xcopy  %temp_go_mgr_out%  %go_out%  /y
echo
pause 
