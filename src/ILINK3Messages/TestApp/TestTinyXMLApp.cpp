//
// Created by pbeerkens on 4/27/20.
//
#include <tinyxml/tinyxml.h>
#include <iostream>

int main () {
    TiXmlDocument doc ("/home/pbeerkens/source/cme_Ilink3_simulator/data/Cme.Futures.iLink3.Sbe.v8.5.xml");
    if (!doc.LoadFile()) {
        std::cout<<"Failed to load the xml document"<<std::endl;
        return 1;
    }

    TiXmlHandle hDoc(&doc);
    TiXmlElement* pElem;
    TiXmlHandle hRoot(0);

    {
        pElem=hDoc.FirstChildElement().Element();
        // should always have a valid root but handle gracefully if it does
        if (!pElem) return 1;
        std::cout<<pElem->Value()<<std::endl;

        // save this for later
        hRoot=TiXmlHandle(pElem);
    }

    {
        pElem = hRoot.FirstChild("types").FirstChild().Element();
        for (; pElem; pElem = pElem->NextSiblingElement()) {
            const char *pKey = pElem->Value();
            const char *pName = pElem->Attribute ("name");
            const char *pDescription = pElem->Attribute ("description");
            const char *pText = pElem->GetText();
            std::cout<<pKey<<":";
            if (pName) std::cout<<pName<<" ";
            if (pDescription) std::cout<<"("<<pDescription<<") ";
            if (pText) std::cout<<"|"<<pText;
            std::cout<<std::endl;
        };
    }

    {
        pElem = hRoot.FirstChild("ns2:message").Element();
        for (; pElem; pElem = pElem->NextSiblingElement("ns2:message")) {
            const char *pKey = pElem->Value();
            const char *pName = pElem->Attribute ("name");
            std::cout<<pKey<<":";
            if (pName) std::cout<<pName<<" ";
            std::cout<<std::endl;
        }
    }

            doc.ToText();
    return 0;

}