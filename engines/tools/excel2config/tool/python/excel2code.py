#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import time
import sys
import xlrd
import csv
import string

EXCEL_INPUTPATH = sys.argv[1]
SERVER_CODE_OUTPATH = sys.argv[2]

#input path.
#EXCEL_INPUTPATH= u"..\..\Config\excel"

#output code path.
#SERVER_CODE_OUTPATH = u"..\..\Config\out\cpp"

CODE = "utf8"

CLIENT_FLAG = 1
SERVER_FLAG = 2
COMMON_FLAG = 3

empty_line = "                     \n"
small_empty = "  "
change_line = "\n"        
public_with_colon = "public:"
start_scope = " { "
end_scope   = " } "        
class_code = "class "
semicolon = ";"
colon       = " : "
comma       = ","
tab_code    ="      "
one_tab_code = "    " 

def check_data_type(data_type):
    if data_type == "int8" or data_type == "uint8":
        return True
    elif data_type == "int16" or data_type == "uint16": 
        return True
    elif data_type == "int32" or data_type == "uint32":
        return True
    elif data_type == "int64" or data_type == "uint64": 
        return True
    elif data_type == "bool":
        return True
    elif data_type == "string":
        return True
    else:
        return False

def get_value_typ(data_type):
    if data_type == "int8":
        return "int8_t"
    elif data_type == "uint8":
        return "uint8_t"    
    elif data_type == "int16":
        return "int16_t"
    elif data_type == "uint16":
        return "uint16_t"    
    elif data_type == "int32":
        return "int32_t"  
    elif data_type == "uint32": 
        return "uint32_t"
    elif data_type == "int64":
        return "int64_t"
    elif data_type == "uint64": 
        return "uint64_t"
    elif data_type == "bool":
        return "bool"
    elif data_type == "string":
        return "std::string"         
    else:
        assert(False)

def get_default_value(data_type):
    if data_type == "int8" or data_type == "uint8":
        return "0"    
    elif data_type == "int16" or data_type == "uint16":
        return "0"    
    elif data_type == "int32" or data_type == "uint32": 
        return "0"
    elif data_type == "int64" or data_type == "uint64": 
        return "0"
    elif data_type == "bool":
        return "false"
    elif data_type == "string":
        return "\"\""         
    else:
        assert(False)

class CExcelToCsvTool:    
    def __init__(self):
        # server data
        self.mHCodeData = []
        self.mCppCodeData = []
    
    def get_h_code_data(self):
        return self.mHCodeData

    def get_cpp_code_data(self):
        return self.mCppCodeData

    def h_common_start(self):
        #commented
        commented_code =  "/****************************************"  +   change_line 
        commented_code += "code by tool general make ,can not modify"  +   change_line 
        commented_code += "*****************************************/" +   change_line         
        commented_code += empty_line

        #head
        head_code =   "#pragma once" + change_line        
        head_code +=  "#include \"SHLib/common/noncopyable.hpp\"" + change_line 
        head_code +=  "#include \"SHLib/config/config_template.hpp\"" + change_line        
        head_code += empty_line

        #namespace_start
        namespace_start_code =  "namespace Framework { " + change_line 
        namespace_start_code += "namespace Config { " + change_line
        namespace_start_code += empty_line

        other_code = "class CCsvParser;" + change_line
        other_code += empty_line

        self.mHCodeData.append(commented_code)         
        self.mHCodeData.append(head_code)    
        self.mHCodeData.append(namespace_start_code)                    
        self.mHCodeData.append(other_code)

        print "generate h_common start"

    def h_common_end(self):
        #namespace_end
        namespace_end_code =  "} // namespace Config" + change_line  
        namespace_end_code += "} // namespace Framework" + change_line

        self.mHCodeData.append(namespace_end_code)

        print "generate h_common end"

    def h_generate(self,filename,field_name_array,types):
        #class_code  generate        
        class_attr_name = "C"+ filename + "Attribute"        
        
        class_attribute_code = class_code + class_attr_name + start_scope + change_line
        class_attribute_code += public_with_colon + change_line
        class_attribute_code += tab_code + "void FillAttribute(const CCsvParser& parser, uint32_t row);" + change_line
        class_attribute_code += change_line
        class_attribute_code += public_with_colon + change_line
        for j in range(0,len(types)):
            var_code = tab_code + get_value_typ(types[j]) + small_empty + field_name_array[j] + small_empty +  "= " + get_default_value(types[j])  + semicolon + change_line
            class_attribute_code += var_code
            
        class_attribute_code += end_scope + semicolon + change_line
        class_attribute_code += empty_line        

        class_cfg_name = "C"+ filename + "Config"
        class_cfg_code = class_code + class_cfg_name + colon  + "public CConfigTemplate<"  + class_attr_name + comma + "uint32_t>" + comma + "Noncopyable" + start_scope + change_line
        class_cfg_code = class_cfg_code + end_scope + semicolon + change_line
        class_cfg_code += empty_line        
                            
        self.mHCodeData.append(class_attribute_code)
        self.mHCodeData.append(class_cfg_code)

        print  filename,"h generate ok"

    def cpp_common_start(self):
        #commented
        commented_code =  "/****************************************"  +   change_line 
        commented_code += "code by tool general make ,can not modify"  +   change_line 
        commented_code += "*****************************************/" +   change_line         
        commented_code += empty_line

        #include_code             
        include_code =  "#include \"SHLib/config/config.h\"" + change_line
        include_code += empty_line + change_line

        #namespace_start
        namespace_start_code =  "namespace Framework { " + change_line 
        namespace_start_code += "namespace Config { " + change_line
        namespace_start_code += empty_line + change_line

        self.mCppCodeData.append(commented_code)
        self.mCppCodeData.append(include_code)
        self.mCppCodeData.append(namespace_start_code)

        print "generate cpp_common start"

    def cpp_common_end(self):
        #namespace_end
        namespace_end_code =  "} // namespace Config" + change_line  
        namespace_end_code += "} // namespace Framework" + change_line
        self.mCppCodeData.append(namespace_end_code)

        print "generate cpp_common end"

    def cpp_generate(self,filename,field_name_array):        
        #register_code
        class_cfg_name = "C"+ filename + "Config"
        register_code = "REGISTER_REFLECTOR(" + class_cfg_name + ")" + semicolon        

        #class_cfg_code
        class_attr_name = "C"+ filename + "Attribute"        
        class_cfg_code = empty_line + change_line
        class_cfg_code += "void "  + class_attr_name  + "::FillAttribute(const CCsvParser& parser, uint32_t row)" + start_scope + change_line
        for j in range(0,len(field_name_array)):
            class_cfg_code += one_tab_code + "parser.GetField(row,\"" + field_name_array[j] + "\"" + comma +  field_name_array[j] +")" + semicolon + change_line
        class_cfg_code += end_scope + change_line
        class_cfg_code += empty_line                       
                            
        self.mCppCodeData.append(register_code)
        self.mCppCodeData.append(class_cfg_code)

        print  filename,"cpp generate ok"
                                 
    def read(self, file_full_path,filename,h_flag):
        xlsfile = xlrd.open_workbook(file_full_path)
        #print xlsfile.sheet_names()

        table = xlsfile.sheet_by_index(0)  # excel 选择第0个sheet
        row_num = table.nrows     # 行
        col_num = table.ncols     # 列  
        
        if row_num <= 1:
            return False

        # row == 1  ===> filed name 
        field_name_array = []
        field_name_row = 0 
        for j in range(0, col_num):            
            field_name_array.append(table.cell_value(field_name_row, j))            
     
        # row == 3 ==> data type
        type_row = 3 
        types = []
        for j in range(0, col_num):
            types.append(table.cell_value(type_row, j))

        for j in range(0,len(types)):
            if check_data_type(types[j]) == False:
                print "read",filename,"type error"
                return False

        if len(types) != len(field_name_array):
            return

        if h_flag:
            self.h_generate(filename,field_name_array,types)
        else:
            self.cpp_generate(filename,field_name_array)
        
        return True

    def del_file(self,path):
        ls = os.listdir(path)
        for i in ls:
            c_path = os.path.join(path, i)
            if os.path.isdir(c_path):
                self.del_file(c_path)
            else:
                os.remove(c_path)

    def write(self, path,filename,content):     
        if not os.path.exists(path):         
            os.makedirs(path)

        all_path_filename = path +"\\" + filename
        code_file = open(all_path_filename, 'w')
        code_file.write(''.join(content))
        code_file.close()        

def Start():
    tool = CExcelToCsvTool()
    print ""
    tool.h_common_start()
    tool.cpp_common_start()
    print ""

    for root, dirs, files in os.walk(EXCEL_INPUTPATH):
        for filename in files:
            file_full_path = os.path.join(EXCEL_INPUTPATH, filename)        
            csv_name = filename.replace(".xlsx", "")
            if os.path.exists(file_full_path):                                
                success = tool.read(file_full_path,csv_name,True)
                success = tool.read(file_full_path,csv_name,False)
                print ""
                assert(success)
            else:
                print file_full_path ,root,dirs," don't exist"

    tool.h_common_end()
    tool.cpp_common_end()

    tool.del_file(SERVER_CODE_OUTPATH)

    tool.write(SERVER_CODE_OUTPATH, "config.h", tool.get_h_code_data())
    tool.write(SERVER_CODE_OUTPATH, "config.cpp", tool.get_cpp_code_data())

if __name__ == '__main__':
    Start()
