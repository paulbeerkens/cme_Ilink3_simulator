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


    doc.ToText();
    return 0;

}