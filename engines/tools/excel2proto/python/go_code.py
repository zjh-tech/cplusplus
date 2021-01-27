#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import time
import sys
import xlrd
import string

PB_INPUTPATH = sys.argv[1]
CFG_MGR_OUTPATH = sys.argv[2]

# PB_INPUTPATH = u"..\\config\\out\\binary"
# CFG_MGR_OUTPATH = u"..\\config\\out\\go_cfg_mgr"

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

    def generate_package(self):
        code = "package config" + change_line
        code += empty_line
        self.mCodeData += code

    def generate_import(self):
        code = "import (" + change_line
        code += tab_code + "\"projects/go-engine/elog\"" + change_line
        code += tab_code + "\"io/ioutil\"" + change_line
        code += tab_code + "\"os\"" + change_line
        code += tab_code + "\"github.com/golang/protobuf/proto\"" + change_line
        code += ")" + change_line
        code += empty_line
        self.mCodeData += code

    def generate_start_struct(self):
        code = "type ConfigMgr struct {" + change_line
        code += tab_code + "DirPath" + tab_code + "string" + change_line
        self.mCodeData += code

    def generate_context_struct(self, fileName):
        code = tab_code + fileName + tab_code + fileName + "cfg" + change_line
        self.mCodeData += code

    def generate_end_struct(self):
        code = "}" + change_line
        code += empty_line
        self.mCodeData += code

    def generate_load_cfg(self):
        code = "func (c *ConfigMgr) LoadCfg(dirPath string, fileName string, message proto.Message) error {" + change_line
        code += tab_code + "PbName := dirPath + \"/\" + fileName + \".pb\"" + change_line
        code += tab_code + "file, openErr := os.Open(PbName)" + change_line
        code += tab_code + "if openErr != nil {" + change_line
        code += tab_code + tab_code + "return openErr" + change_line
        code += tab_code + "}" + change_line
        code += tab_code + "defer file.Close()" + change_line
        code += tab_code + \
            "datas, readErr := ioutil.ReadAll(file)" + change_line
        code += tab_code + "if readErr != nil {" + change_line
        code += tab_code + tab_code + "return readErr" + change_line
        code += tab_code + "}" + change_line
        code += tab_code + \
            "return proto.Unmarshal(datas, message)" + change_line
        code += "}" + change_line
        code += empty_line
        self.mCodeData += code

    def generate_load_all_cfg_start(self):
        code = "func (c *ConfigMgr) LoadAllCfg(dirPath string) error {" + change_line
        code += tab_code + "c.DirPath" + tab_code + \
            "=" + tab_code + "dirPath" + change_line
        self.mCodeData += code

    def generate_load_all_cfg_context(self, fileName):
        FirstBigFileName = fileName.capitalize()
        code = tab_code + \
            "if err := c.LoadCfg(dirPath," + "\"" + fileName + "\"" + ",&c." + \
            FirstBigFileName + "); err != nil {" + change_line
        code += tab_code + tab_code + "return err" + change_line
        code += tab_code + "}" + change_line

        code += tab_code + \
            "elog.InfoA(\"" + FirstBigFileName + \
            " Config Start\")" + change_line

        code += tab_code + "for _, info := range c." + \
            FirstBigFileName + ".Datas {" + change_line
        code += tab_code + tab_code + \
            "elog.InfoAf(\"" + fileName + "=%+v \\n\", info)" + change_line
        code += tab_code + "}" + change_line

        code += tab_code + \
            "elog.InfoA(\"" + FirstBigFileName + \
            " Config End\")" + change_line

        self.mCodeData += code

    def generate_load_all_cfg_end(self):
        code = tab_code + "return nil" + change_line
        code += "}" + change_line
        code += empty_line
        self.mCodeData += code

    def generate_get_by_id(self, filename):
        className = filename
        code = "func (c *ConfigMgr) Get" + className + \
            "ByID(id uint32) *" + className + " {" + change_line
        code += tab_code + "if  c." + className + \
            ".Datas != nil {" + change_line
        code += tab_code + tab_code + "if info, ok := c." + \
            className + ".Datas[id]; ok {" + change_line
        code += tab_code + tab_code + tab_code + "return info" + change_line
        code += tab_code + tab_code + "}" + change_line
        code += tab_code + "}" + change_line
        code += tab_code + "return nil" + change_line
        code += "}" + change_line
        code += change_line
        self.mCodeData += code

    def generate_global_var(self):
        code = "var GConfigMgr *ConfigMgr" + change_line
        code += empty_line
        code += "func init() {" + change_line
        code += tab_code + tab_code + "GConfigMgr = &ConfigMgr{}" + change_line
        code += "}" + change_line

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


def Start():
    tool = CTool()
    tool.generate_package()

    tool.generate_import()
    tool.generate_start_struct()

    for _, _, files in os.walk(PB_INPUTPATH):
        for filename in files:
            file_full_path = os.path.join(PB_INPUTPATH, filename)
            if os.path.exists(file_full_path):
                pb_name = filename.replace(".pb", "")
                tool.generate_context_struct(pb_name.capitalize())
                # tool.generate_context_struct(pb_name)

    tool.generate_end_struct()
    tool.generate_load_cfg()
    tool.generate_load_all_cfg_start()
    for _, _, files in os.walk(PB_INPUTPATH):
        for filename in files:
            file_full_path = os.path.join(PB_INPUTPATH, filename)
            if os.path.exists(file_full_path):
                pb_name = filename.replace(".pb", "")
                tool.generate_load_all_cfg_context(pb_name)

    tool.generate_load_all_cfg_end()

    for _, _, files in os.walk(PB_INPUTPATH):
        for filename in files:
            file_full_path = os.path.join(PB_INPUTPATH, filename)
            if os.path.exists(file_full_path):
                pb_name = filename.replace(".pb", "")
                tool.generate_get_by_id(pb_name.capitalize())

    tool.generate_global_var()
    tool.del_file(CFG_MGR_OUTPATH)
    tool.write(CFG_MGR_OUTPATH, "configmgr.go", tool.get_code_data())
    print "ok"


if __name__ == '__main__':
    Start()
