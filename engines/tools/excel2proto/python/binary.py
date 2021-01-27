#!/usr/bin/python
# -*- coding: utf-8 -*-

import xlrd
import sys
import os
import imp

EXCEL_DIR = sys.argv[1]
PYTHON_DIR = sys.argv[2]
BINARY_DIR = sys.argv[3]

#EXCEL_DIR = u"../../../../share/excel"
#PYTHON_DIR = u"../config/out/python/"
#BINARY_DIR = u"../config/out/binary/"

CLIENT_FLAG = 1
SERVER_FLAG = 2
COMMON_FLAG = 3


def load_obj(pythonFileName, protoName, className):
    moduleName = PYTHON_DIR + pythonFileName + "_pb2.py"
    module = imp.load_source(protoName, moduleName)
    if hasattr(module, className):
        classzz = getattr(module, className)
        return classzz()


def is_string_type(cell):
    if cell.ctype == 1:
        return True
    else:
        return False


for root, dirs, files in os.walk(EXCEL_DIR):
    for excelFile in files:
        array = excelFile.split(".")
        if len(array) < 2:
            continue
        if array[1] != "xlsx":
            continue

        excelFileName = array[0]
        excel_full_path = os.path.join(EXCEL_DIR, excelFile)
        xlsfile = xlrd.open_workbook(excel_full_path)

        table = xlsfile.sheet_by_index(0)
        row_num = table.nrows     # 行
        col_num = table.ncols     # 列

        # row == 2  ===> client / server flag
        flags = []
        # flag_row = 2
        flag_row = 1
        for j in range(0, col_num):
            flag = table.cell_value(flag_row, j)
            flags.append(int(flag))

        # row == 0  ===> name
        names = []
        # name_row = 0
        name_row = 4
        for j in range(0, col_num):
            name = table.cell_value(name_row, j)
            names.append(name)

        # row == 3 ==> data type
        types = []
        # type_row = 3
        type_row = 2
        for j in range(0, col_num):
            types.append(table.cell_value(type_row, j))

        pythonFileName = excelFile.split(".")[0]
        protoName = excelFileName
        classCfgName = excelFileName + "cfg"
        fieldname = "fieldname"
        filedtype = "filedtype"
        try:
            cfgObj = load_obj(pythonFileName, protoName, classCfgName)
            units = getattr(cfgObj, "datas")

            data_start_line = 5
            for i in range(data_start_line, row_num):
                # vector
                # obj = units.add()

                # map
                id_cell = table.cell(i, 0)
                id = int(id_cell.value)
                obj = units[id]
                for j in range(0, col_num):
                    if flags[j] == COMMON_FLAG or flags[j] == SERVER_FLAG:
                        cell = table.cell(i, j)
                        fieldname = names[j]
                        filedtype = types[j]
                        if hasattr(obj, fieldname):
                            if filedtype == "int32" or filedtype == "uint32" or filedtype == "int64" or filedtype == "uint64":
                                if isinstance(cell.value, float):
                                    tempvalue = float(cell.value)
                                    fieldvalue = long(tempvalue)
                                    setattr(obj, fieldname, fieldvalue)
                                elif cell.value == "":
                                    setattr(obj, fieldname, 0)
                                else:
                                    setattr(obj, fieldname, long(cell.value))
                            elif filedtype == "bool":
                                setattr(obj, fieldname, bool(cell.value))
                            elif filedtype == "float":
                                setattr(obj, fieldname, float(cell.value))
                            elif filedtype == "string":
                                if isinstance(cell.value, float):
                                    tempvalue = float(cell.value)
                                    diff = tempvalue - int(tempvalue)
                                    if 0 == diff:
                                        value = int(tempvalue)
                                        setattr(obj, fieldname, str(value))
                                    else:
                                        setattr(obj, fieldname, str(tempvalue))
                                else:
                                    setattr(obj, fieldname, cell.value)
        except:
            print excelFileName, fieldname, filedtype, "error"
            raise Exception, "binary error"
        else:
            print excelFileName, "ok"
            print ""

        PbName = BINARY_DIR + excelFileName + ".pb"
        print PbName
        with open(PbName, 'wb') as dataFile:
            dataFile.write(cfgObj.SerializeToString())
