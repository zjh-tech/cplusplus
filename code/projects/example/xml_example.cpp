#include <iostream>
#include <string>

#include "tinyxml2.h"

using namespace tinyxml2;
using namespace std;

bool ReadSundryXml()
{
    XMLDocument doc;
    if (doc.LoadFile("config/xml/sundry.xml") != XML_SUCCESS)
    {
        return false;
    }

    XMLElement* root = doc.RootElement();
    if (!root)
    {
        return false;
    }

    if (root->FirstChildElement("default_hero_id"))
    {
        string default_hero_id_str = root->FirstChildElement("default_hero_id")->GetText();
        cout << "default_hero_id: " << default_hero_id_str << endl;
    }

    // Attribute with  XMLElement difference
    XMLElement* persons_ele = root->FirstChildElement("persons");
    if (persons_ele)
    {
        for (XMLElement* node = persons_ele->FirstChildElement(); node; node = node->NextSiblingElement())
        {
            cout << endl;

            cout << "name: " << node->Attribute("name") << endl;
            cout << "id: " << node->Attribute("id") << endl;

            cout << "sex: " << node->FirstChildElement("sex")->GetText() << endl;
            cout << "age: " << node->FirstChildElement("age")->GetText() << endl;

            cout << endl;
        }
    }

    return true;
}

int main(void)
{
    ReadSundryXml();

    return 0;
}