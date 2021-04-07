#include <errno.h>

#include <iostream>

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

using namespace std;

bool Read()
{
    FILE* fp = fopen("config/json/test.json", "r");
    if (!fp)
    {
        cout << "read json file error" << strerror(errno) << endl;
        return false;
    }

    char buf[0XFFFF];
    rapidjson::FileReadStream rstream(fp, buf, sizeof(buf));
    rapidjson::Document doc;
    doc.ParseStream(rstream);
    fclose(fp);

    // Int
    if (doc.HasMember("Int") && doc["Int"].IsInt())
    {
        cout << "Int: " << doc["Int"].GetInt() << endl;
    }

    // Object
    if (doc.HasMember("Object") && doc["Object"].IsObject())
    {
        const rapidjson::Value& object = doc["Object"];

        if (object.HasMember("name") && object["name"].IsString())
        {
            cout << "object name: " << object["name"].GetString() << endl;
        }

        if (object.HasMember("age") && object["age"].IsInt())
        {
            cout << "object age: " << object["age"].GetInt() << endl;
        }
    }

    // Array
    if (doc.HasMember("IntArray") && doc["IntArray"].IsArray())
    {
        const rapidjson::Value& array = doc["IntArray"].GetArray();
        size_t len                    = array.Size();
        for (size_t i = 0; i < len; ++i)
        {
            if (array[i].IsInt())
            {
                cout << "IntArray: " << array[i].GetInt() << endl;
            }
        }
    }

    // Object, Array Mix
    if (doc.HasMember("People") && doc["People"].IsArray())
    {
        const rapidjson::Value& array = doc["People"].GetArray();
        size_t len                    = array.Size();
        for (size_t i = 0; i < len; ++i)
        {
            if (array[i].IsObject())
            {
                const rapidjson::Value& object = array[i];
                if (object.HasMember("name") && object["name"].IsString())
                {
                    cout << "object name: " << object["name"].GetString() << endl;
                }

                if (object.HasMember("age") && object["age"].IsInt())
                {
                    cout << "object age: " << object["age"].GetInt() << endl;
                }

                if (object.HasMember("sex") && object["sex"].IsBool())
                {
                    cout << "object sex: " << object["sex"].GetBool() << endl;
                }
            }
        }
    }
    return true;
}

void Write()
{
    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

    // Int
    doc.AddMember("Int", 1, allocator);

    // Object
    rapidjson::Value object(rapidjson::kObjectType);
    object.AddMember("name", "qq", allocator);
    object.AddMember("age", 25, allocator);
    doc.AddMember("Object", object, allocator);

    // Array
    rapidjson::Value intarray(rapidjson::kArrayType);
    intarray.PushBack(10, allocator);
    intarray.PushBack(20, allocator);
    intarray.PushBack(30, allocator);
    doc.AddMember("IntArray", intarray, allocator);

    // Object, Array Mix
    rapidjson::Value ObjectArray(rapidjson::kArrayType);
    for (int i = 1; i < 3; i++)
    {
        rapidjson::Value obj(rapidjson::kObjectType);
        obj.AddMember("name", "qq", allocator);
        obj.AddMember("age", 0, allocator);
        obj.AddMember("sex", true, allocator);
        ObjectArray.PushBack(obj, allocator);
    }

    doc.AddMember("People", ObjectArray, allocator);

    rapidjson::StringBuffer strBuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strBuf);
    doc.Accept(writer);
    string data = strBuf.GetString();
    cout << data.c_str() << endl;
}

int main(void)
{
    Read();
    Write();

    return 0;
}