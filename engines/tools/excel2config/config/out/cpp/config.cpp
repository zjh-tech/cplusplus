/****************************************
code by tool general make ,can not modify
*****************************************/
                     
#include "SHLib/config/config.h"
                     

namespace Framework { 
namespace Config { 
                     

REGISTER_REFLECTOR(CHeroConfig);                     

void CHeroAttribute::FillAttribute(const CCsvParser& parser, uint32_t row) { 
    parser.GetField(row,"id",id);
    parser.GetField(row,"name",name);
    parser.GetField(row,"flag",flag);
    parser.GetField(row,"skill_id_1",skill_id_1);
    parser.GetField(row,"skill_id_2",skill_id_2);
    parser.GetField(row,"skill_id_3",skill_id_3);
    parser.GetField(row,"skill_id_4",skill_id_4);
    parser.GetField(row,"skill_id_5",skill_id_5);
    parser.GetField(row,"skill_id_6",skill_id_6);
    parser.GetField(row,"skill_id_7",skill_id_7);
    parser.GetField(row,"skill_id_8",skill_id_8);
    parser.GetField(row,"effect",effect);
 } 
                     
REGISTER_REFLECTOR(CTestConfig);                     

void CTestAttribute::FillAttribute(const CCsvParser& parser, uint32_t row) { 
    parser.GetField(row,"id",id);
    parser.GetField(row,"name",name);
    parser.GetField(row,"flag",flag);
    parser.GetField(row,"skill_id_1",skill_id_1);
    parser.GetField(row,"skill_id_2",skill_id_2);
    parser.GetField(row,"skill_id_3",skill_id_3);
    parser.GetField(row,"skill_id_4",skill_id_4);
    parser.GetField(row,"skill_id_5",skill_id_5);
    parser.GetField(row,"skill_id_6",skill_id_6);
    parser.GetField(row,"skill_id_7",skill_id_7);
    parser.GetField(row,"skill_id_8",skill_id_8);
    parser.GetField(row,"effect",effect);
 } 
                     
} // namespace Config
} // namespace Framework
