#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import time
import sys
import xlrd
import string

EXCEL_INPUTPATH = sys.argv[1]
PROTO_OUTPATH = sys.argv[2]

#EXCEL_INPUTPATH = u"../../../../share/excel"
#PROTO_OUTPATH = u"../config/out/proto"

CODE = "utf8"

CLIENT_FLAG = 1
SERVER_FLAG = 2
COMMON_FLAG = 3

change_line = "\n"
empty_line = "                     \n"
start_scope = "{ "
end_scope = "} "
two_tab_code = "        "
tab_code = "   "
semicolon = ";"
small_empty = "  "
equal_number = "="


class CTool:
    def __init__(self):
        # server data
        self.mCodeData = []

    def get_code_data(self):
        return self.mCodeData

    def check_data_type(self, data_type):
        if data_type == "int32":
            return True
        elif data_type == "uint32":
            return True
        elif data_type == "int64":
            return True
        elif data_type == "uint64":
            return True
        elif data_type == "bool":
            return True
        elif data_type == "float":
            return True
        elif data_type == "double":
            return True
        elif data_type == "string":
            return True
        else:
            return False

    def proto_generate(self, classname, names, types):
        code = "syntax = " + "\"proto3\"" + semicolon + change_line
        code += empty_line
        code += "package" + tab_code + "config;" + change_line
        code += empty_line
        code += "message" + tab_code + classname + change_line
        code += start_scope + change_line

        line_num = 1

        for j in range(0, len(types)):
            field_code = tab_code + types[j] + "  " + names[j] + small_empty + \
                equal_number + str(line_num) + semicolon + change_line

            line_num += 1
            code += field_code
        code += end_scope

        self.mCodeData = code
        code = change_line

    def proto_list_generate(self, classname):
        code = change_line
        code += "message" + tab_code + classname + "cfg" + change_line
        code += start_scope + change_line
        line_num = 1
        # vector
        # code += tab_code + "repeated " + classname + tab_code + \
        #    "datas" + equal_number + tab_code + str(line_num) + semicolon

        # map
        code += tab_code + "map<uint32," + classname + ">  datas" + \
            equal_number + tab_code + str(line_num) + semicolon

        code += change_line
        code += end_scope
        self.mCodeData += code

    def read(self, filename, proto_name):
        xlsfile = xlrd.open_workbook(filename)
        # print xlsfile.sheet_names()

        table = xlsfile.sheet_by_index(0)  # excel 选择第0个sheet
        row_num = table.nrows     # 行
        col_num = table.ncols     # 列

        if row_num <= 1:
            return False

        # row == 2  ===> client / server flag
        flags = []
        #flag_row = 2
        flag_row = 1
        for j in range(0, col_num):
            flag = table.cell_value(flag_row, j)
            flags.append(int(flag))

        # row == 0  ===> name
        names = []
        #name_row = 0
        name_row = 4
        for j in range(0, col_num):
            if flags[j] == COMMON_FLAG or flags[j] == SERVER_FLAG:
                name = table.cell_value(name_row, j)
                names.append(name)

        # row == 3 ==> data type
        types = []
        #type_row = 3
        type_row = 2
        for j in range(0, col_num):
            if flags[j] == COMMON_FLAG or flags[j] == SERVER_FLAG:
                types.append(table.cell_value(type_row, j))

        for j in range(0, len(types)):
            if self.check_data_type(types[j]) == False:
                print "read", filename, types[j], "type error"
                return False

        self.proto_generate(proto_name, names, types)
        self.proto_list_generate(proto_name)
        return True

    def del_file(self, path):
        ls = os.listdir(path)
        for i in ls:
            c_path = os.path.join(path, i)
            if os.path.isdir(c_path):
                self.del_file(c_path)
            else:
                os.remove(c_path)

    def write(self, path, filename, content):
        if not os.path.exists(path):
            os.makedirs(path)

        all_path_filename = path + "\\" + filename
        code_file = open(all_path_filename, 'w')
        code_file.write(''.join(content))
        code_file.close()


def Start():
    print ""

    for root, dirs, files in os.walk(EXCEL_INPUTPATH):
        for filename in files:
            file_full_path = os.path.join(EXCEL_INPUTPATH, filename)
            if os.path.exists(file_full_path):
                proto_name = filename.replace(".xlsx", "")
                tool = CTool()
                success = tool.read(file_full_path, proto_name)
                if success == True:
                    # tool.del_file(PROTO_OUTPATH)
                    proto_name = proto_name + ".proto"
                    tool.write(PROTO_OUTPATH, proto_name, tool.get_code_data())
            else:
                print root, dirs, filename, " don't exist"


if __name__ == '__main__':
    Start()
