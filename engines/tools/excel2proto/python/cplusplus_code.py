#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import time
import sys
import xlrd
import string

PB_INPUTPATH = sys.argv[1]
CFG_MGR_OUTPATH = sys.argv[2]

#PB_INPUTPATH = u"..\\config\\out\\binary"
#CFG_MGR_OUTPATH = u"..\\config\\out\\cplusplus_cfg_mgr"

CODE = "utf8"

CLIENT_FLAG = 1
SERVER_FLAG = 2
COMMON_FLAG = 3

change_line = "\n"
empty_line = "                     \n"
tab_code = "   "


class CTool:
    def __init__(self):
        self.mCodeData = []

    def get_code_data(self):
        return self.mCodeData

    def generate_include(self):
        code = "#pragma once" + change_line
        code += "#include \"engine/inc/common/errorstring.h\"" + change_line
        code += "#include \"engine/inc/common/singleton.hpp\"" + change_line
        code += "#include <cinttypes>" + change_line
        code += empty_line
        code += "using namespace std;" + change_line
        code += "using namespace Framework;" + change_line
        code += empty_line
        code += "namespace google" + change_line
        code += "{" + change_line
        code += tab_code + "namespace protobuf" + change_line
        code += tab_code + "{" + change_line
        code += tab_code + tab_code + "class Message;" + change_line
        code += tab_code + "}" + change_line
        code += "}" + change_line
        code += empty_line

        self.mCodeData += code

    def generate_class_declare_start(self):
        code = "namespace config" + change_line
        code += "{" + change_line
        self.mCodeData += code

    def generate_class_declare(self, fileName):
        code = tab_code + "class" + tab_code + fileName + "cfg;" + change_line
        code += tab_code + "class" + tab_code + fileName + ";" + change_line
        self.mCodeData += code

    def generate_class_declare_end(self):
        code = "}" + change_line
        code += empty_line
        self.mCodeData += code

    def generate_start_class(self):
        code = "class StaticDataMgr : public Singleton<StaticDataMgr>" + change_line
        code += "{" + change_line
        code += "public:" + change_line
        code += tab_code + "~StaticDataMgr();" + change_line
        code += empty_line
        code += tab_code + \
            "ErrorStringPtr LoadAllCfg(const char *path);" + change_line
        code += empty_line
        code += "private:" + change_line
        code += tab_code + \
            "bool load_cfg(const char *path, google::protobuf::Message *msg);" + change_line
        code += empty_line
        code += "public:" + change_line
        self.mCodeData += code

    def generate_context_class(self, fileName):
        cap_file_name = fileName.capitalize()
        code = tab_code + "const config::" + fileName + \
            "cfg*  " + "Get" + cap_file_name + "Cfg();" + change_line
        code += tab_code + "const config::" + fileName + \
            "*  " + "Get" + cap_file_name + \
            "ByID(uint32_t id);" + change_line
        code += empty_line
        self.mCodeData += code

    def generate_define_start(self):
        code = "private:" + change_line
        self.mCodeData += code

    def generate_define_class(self, fileName):
        code = tab_code + "config::" + fileName + "cfg *" + \
            fileName + "_ptr = nullptr;" + change_line
        self.mCodeData += code

    def generate_end_class(self):
        code = "};" + change_line
        code += empty_line
        self.mCodeData += code

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

    def generate_cpp_include(self):
        code = "#include \"staticdatamgr.h\"" + change_line
        code += "#include \"engine/inc/common/utils.hpp\"" + change_line
        code += "#include \"engine/inc/log/logger.h\"" + change_line
        code += "#include \"google/protobuf/message.h\"" + change_line
        code += "#include \"tip.pb.h\"" + change_line
        code += "#include <fstream>" + change_line
        code += "#include <iostream>" + change_line
        code += "#include <sstream>" + change_line
        code += empty_line
        code += "using namespace Framework;" + change_line
        code += empty_line
        self.mCodeData += code

    def generate_cpp_destruct_start(self):
        code = "StaticDataMgr::~StaticDataMgr()" + change_line
        code += "{" + change_line
        self.mCodeData += code

    def generate_cpp_destruct(self, fileName):
        code = tab_code + "SAFE_DELETE(" + fileName + "_ptr);" + change_line
        self.mCodeData += code

    def generate_cpp_destruct_end(self):
        code = "}" + change_line
        code += empty_line
        self.mCodeData += code

    def generate_cpp_load_template(self):
        code = "bool StaticDataMgr::load_cfg(const char *path, google::protobuf::Message *msg)" + change_line
        code += "{" + change_line
        code += tab_code + "ifstream fs(path);" + change_line
        code += tab_code + "if (!fs.is_open())" + change_line
        code += tab_code + "{" + change_line
        code += tab_code + tab_code + "return false;" + change_line
        code += tab_code + "}" + change_line
        code += tab_code + "stringstream buffer;" + change_line
        code += tab_code + "buffer << fs.rdbuf();" + change_line
        code += tab_code + \
            "return msg->ParseFromArray(buffer.str().c_str(), buffer.str().length());" + change_line
        code += "}" + change_line
        code += empty_line
        self.mCodeData += code

    def generate_load_all_cfg_start(self):
        code = "ErrorStringPtr StaticDataMgr::LoadAllCfg(const char *path)" + \
            change_line
        code += "{" + change_line
        self.mCodeData += code

    def generate_load_all_cfg(self, fileName):
        cap_file_name = fileName.capitalize()
        code = tab_code + fileName + "_ptr = NEW config::" + fileName + "cfg();" + \
            change_line
        code += tab_code + "string prefix_path(path);" + change_line

        code += tab_code + "string " + fileName + "_path = prefix_path + \"/" + \
            fileName + ".pb\";" + change_line

        code += tab_code + \
            "if (!load_cfg(" + fileName + "_path.c_str(), " + \
            fileName + "_ptr))" + change_line
        code += tab_code + "{" + change_line
        code += tab_code + tab_code + "return CREATE_ERROR_STRING(\"load " + \
            fileName + "  file error..\"); " + change_line
        code += tab_code + "}" + change_line
        code += tab_code + "LogInfoA(\"[StaticDataMgr] " + cap_file_name + \
            "Cfg={}\"," + fileName + "_ptr->DebugString());" + change_line
        code += empty_line

        self.mCodeData += code

    def generate_load_all_cfg_end(self):
        code = tab_code + "return nullptr;" + change_line
        code += "}" + change_line
        code += empty_line
        self.mCodeData += code

    def generate_get_cfg(self, fileName):
        cap_file_name = fileName.capitalize()
        code = "const config::" + fileName + "cfg* StaticDataMgr::Get" + \
            cap_file_name + "Cfg()" + change_line
        code += "{" + change_line
        code += tab_code + "return " + fileName + "_ptr;" + change_line
        code += "}" + change_line
        code += empty_line

        code += "const config::" + fileName + "*  StaticDataMgr::Get" + \
            cap_file_name + "ByID(uint32_t id) {" + change_line
        code += tab_code + "if (" + fileName + "_ptr) {" + change_line
        code += tab_code + tab_code + "auto iter = " + \
            fileName + "_ptr->datas().find(id);" + change_line
        code += tab_code + tab_code + "if (iter != " + fileName + \
            "_ptr->datas().end()) {" + change_line
        code += tab_code + tab_code + tab_code + \
            "return &(iter->second);" + change_line
        code += tab_code + tab_code + "}" + change_line
        code += tab_code + "}" + change_line
        code += tab_code + "return nullptr;" + change_line
        code += "}" + change_line

        code += empty_line
        self.mCodeData += code


def Start():
    # staticdatamgr.h generate
    tool = CTool()
    tool.generate_include()

    tool.generate_class_declare_start()
    for _, _, files in os.walk(PB_INPUTPATH):
        for filename in files:
            file_full_path = os.path.join(PB_INPUTPATH, filename)
            if os.path.exists(file_full_path):
                pb_name = filename.replace(".pb", "")
                tool.generate_class_declare(pb_name)

    tool.generate_class_declare_end()

    tool.generate_start_class()
    for _, _, files in os.walk(PB_INPUTPATH):
        for filename in files:
            file_full_path = os.path.join(PB_INPUTPATH, filename)
            if os.path.exists(file_full_path):
                pb_name = filename.replace(".pb", "")
                tool.generate_context_class(pb_name)

    tool.generate_define_start()

    for _, _, files in os.walk(PB_INPUTPATH):
        for filename in files:
            file_full_path = os.path.join(PB_INPUTPATH, filename)
            if os.path.exists(file_full_path):
                pb_name = filename.replace(".pb", "")
                tool.generate_define_class(pb_name)

    tool.generate_end_class()

    h_filename = "staticdatamgr.h"
    # tool.del_file(CFG_MGR_OUTPATH)
    tool.write(CFG_MGR_OUTPATH, h_filename, tool.get_code_data())
    print "ok"

    # staticdatamgr.cpp generate
    cpp_tool = CTool()
    cpp_tool.generate_cpp_include()
    cpp_tool.generate_cpp_destruct_start()
    for _, _, files in os.walk(PB_INPUTPATH):
        for filename in files:
            file_full_path = os.path.join(PB_INPUTPATH, filename)
            if os.path.exists(file_full_path):
                pb_name = filename.replace(".pb", "")
                cpp_tool.generate_cpp_destruct(pb_name)
    cpp_tool.generate_cpp_destruct_end()

    cpp_tool.generate_cpp_load_template()

    cpp_tool.generate_load_all_cfg_start()
    for _, _, files in os.walk(PB_INPUTPATH):
        for filename in files:
            file_full_path = os.path.join(PB_INPUTPATH, filename)
            if os.path.exists(file_full_path):
                pb_name = filename.replace(".pb", "")
                cpp_tool.generate_load_all_cfg(pb_name)
    cpp_tool.generate_load_all_cfg_end()

    for _, _, files in os.walk(PB_INPUTPATH):
        for filename in files:
            file_full_path = os.path.join(PB_INPUTPATH, filename)
            if os.path.exists(file_full_path):
                pb_name = filename.replace(".pb", "")
                cpp_tool.generate_get_cfg(pb_name)

    cpp_filename = "staticdatamgr.cpp"
    cpp_tool.write(CFG_MGR_OUTPATH, cpp_filename, cpp_tool.get_code_data())
    print "ok"


if __name__ == '__main__':
    Start()
