set   temp_solution="./solution"
rmdir /s /q  %temp_solution%
mkdir %temp_solution%
cd %temp_solution%
call cmake -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
pause