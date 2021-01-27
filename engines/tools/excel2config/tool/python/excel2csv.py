#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import time
import sys
import xlrd
import csv
import string

EXCEL_INPUTPATH = sys.argv[1]
CLIENT_CSV_OUTPATH = sys.argv[2]
SERVER_CSV_OUTPATH = sys.argv[3]

#input path.
#EXCEL_INPUTPATH= u"..\..\Config\excel"

#output path.
#CLIENT_CSV_OUTPATH= u"..\..\Config\out\csv\client"
#SERVER_CSV_OUTPATH = u"..\..\Config\out\csv\server"

CODE = "utf8"

CLIENT_FLAG = 1
SERVER_FLAG = 2
COMMON_FLAG = 3

class CExcelToCsvTool:    
    def __init__(self):        
        # server data
        self.mServerData = []
                
        # client data
        self.mClientData = []

    def getclientdata(self):
        return self.mClientData

    def getserverdata(self):
        return self.mServerData

    # ctype： 0 empty,1 string, 2 number, 3 date, 4 boolean, 5 error

    def is_int_type(self,cell):
        if cell.ctype == 2 and cell.value % 1 == 0:
           return True
        else:
            return False
    
    def is_float_type(self,cell):
        if cell.ctype == 2:
               return True
        else:
            return False

    def is_string_type(self,cell):
        if cell.ctype == 1:
            return True
        else:
            return False

    def is_date_type(self,cell):
        if cell.ctype == 3:
               return True
        else:
            return False

    def str_to_date(self,str):
        if(str==""):
            str="0"
        
        str=str.strip()
        strArr=str.split('/')
        temp=["0","0","0","0","0","0"]
        for i in range(0,len(temp)):
            if(i<len(strArr)):
                temp[i]=strArr[i]

        result=''
        if(temp[0]=="0"):
            return 0
        else:
            for i in range(0,len(temp)):
                if(i<2):
                    result=result+temp[i]+'-'
                elif(i==2):
                    result=result+temp[i]+' '
                elif(i<5):
                    result=result+temp[i]+':'
                else:
                    result=result+temp[i]
        
        #print result
        timeStamp=time.mktime(time.strptime(result,'%Y-%m-%d %H:%M:%S'))
        return int(timeStamp)

    def check_data_type(self,data_type):
        if data_type == "int8":
            return True
        elif data_type == "uint8": 
            return True
        elif data_type == "int16": 
            return True
        elif data_type == "uint16": 
            return True
        elif data_type == "int32": 
            return True
        elif data_type == "uint32": 
            return True
        elif data_type == "int64": 
            return True
        elif data_type == "uint64":
            return True
        elif data_type == "bool":
            return True
        elif data_type == "string":
            return True
        else:
            return False    

    def read(self, filename):
        xlsfile = xlrd.open_workbook(filename)
        #print xlsfile.sheet_names()

        table = xlsfile.sheet_by_index(0)  # excel 选择第0个sheet
        row_num = table.nrows     # 行
        col_num = table.ncols     # 列  
        
        if row_num <= 1:
            return False

        # row == 2  ===> client / server flag         
        flags = [] 
        flag_row = 2 
        for j in range(0, col_num):
            flag = table.cell_value(flag_row, j)
            flags.append(int(flag))

        # row == 3 ==> data type
        type_row = 3 
        types = []
        for j in range(0, col_num):
            types.append(table.cell_value(type_row, j))

        for j in range(0,len(types)):
            if self.check_data_type(types[j]) == False:
                print "read",filename,"type error"
                return False

        for i in range(0, row_num):                        
            s_row_list = []
            c_row_list = []  
            for j in range(0, col_num):
                cell = table.cell(i,j)
                value = None
                                
                if self.is_int_type(cell): 
                    value = int(cell.value)
                elif self.is_float_type(cell): 
                    value = float(cell.value)
                elif self.is_string_type(cell):
                    value = cell.value    
                    value = value.encode(CODE)
                elif self.is_date_type(cell):                    
                    value = self.str_to_date(value)

                if value == None: 
                    print "read",filename, i, j,"error"
                    return False
                                
                if flags[j] == COMMON_FLAG or flags[j] == CLIENT_FLAG:
                    c_row_list.append(value)
                if flags[j] == COMMON_FLAG or flags[j] == SERVER_FLAG:
                    s_row_list.append(value)         
            
            self.mServerData.append(tuple(s_row_list))
            self.mClientData.append(tuple(c_row_list))  

        return True

    def del_file(self,path):
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

        csvfile = file("tmp", "wb")   
        writer = csv.writer(csvfile)       
        writer.writerows(content)      
        csvfile.close()

        os.rename("tmp", os.path.join(path, filename))     
        print "write",filename, " ok"


def Start():
    print ""

    for root, dirs, files in os.walk(EXCEL_INPUTPATH):
        for filename in files:        
            file_full_path = os.path.join(EXCEL_INPUTPATH, filename)        
            if os.path.exists(file_full_path):
                tool = CExcelToCsvTool()
                success = tool.read(file_full_path)
                if success == True:                     
                    csv_name = filename.replace(".xlsx", "")
                    
                    tool.del_file(SERVER_CSV_OUTPATH)
                    tool.del_file(CLIENT_CSV_OUTPATH)
                    tool.write(SERVER_CSV_OUTPATH, csv_name+".csv", tool.getserverdata())
                    tool.write(CLIENT_CSV_OUTPATH, csv_name+".csv", tool.getclientdata())
                    print ""
            else:
                print  root,dirs,filename," don't exist"


if __name__ == '__main__':
    Start()
