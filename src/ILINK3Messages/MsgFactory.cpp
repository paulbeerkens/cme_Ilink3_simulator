//
// Created by pbeerkens on 4/24/20.
//
#include <iostream>
#include "MsgFactory.h"
#include "ILink3Headers.h"
#include <Logger/LogMacros.h>
#include <tinyxml/tinyxml.h>


std::unique_ptr<ILink3Msg> MsgFactory::processMessage([[maybe_unused]] MessageBuffer &msgBuffer) {
    const SBEHeader* sbeHeader= reinterpret_cast<const SBEHeader*>(msgBuffer.getRdPtr());
    std::cout<<sbeHeader->blockLength_<<std::endl;
    std::cout<<sbeHeader->schemaId_<<std::endl;

    return nullptr;
}

bool MsgFactory::initialize(MsgFactorySettings &msgFactorySettings) {
    LOGINFO ("Loading ILINK3 message formats from "<<msgFactorySettings.msgFormatFile_);

    TiXmlDocument doc;
    if (!doc.LoadFile(msgFactorySettings.msgFormatFile_)) {
        std::cout<<"Failed to load the ILINK3 message format xml document from "<<msgFactorySettings.msgFormatFile_<<std::endl;
        return 1;
    }

    TiXmlHandle hDoc(&doc);
    TiXmlElement* pElem;
    TiXmlHandle hRoot(0);

    {
        pElem=hDoc.FirstChildElement().Element();
        // should always have a valid root but handle gracefully if it does
        if (!pElem) return false;

        // save this for later
        hRoot=TiXmlHandle(pElem);
    }

    //First read the field types
    {
        pElem = hRoot.FirstChild("types").FirstChild("type").Element();
        for (; pElem; pElem = pElem->NextSiblingElement("type")) {
            ILink3Field newField;
            //const char *pKey = pElem->Value ();

            static std::string NAME ("name");
            static std::string DESCRIPTION ("description");
            static std::string LENGTH ("length");
            static std::string PRIMITIVE_TYPE ("primitiveType");

            const std::string*nameStr=pElem->Attribute(NAME);
            if (nameStr) {
                newField.name_ = *nameStr;
            } else {
                LOGERROR("Found entry in types|type node without a name.");
                continue;
            }

            const auto*pDescription = pElem->Attribute (DESCRIPTION);
            if (pDescription) newField.description_=*pDescription;

            int length;
            if (pElem->QueryIntAttribute (LENGTH, &length)==TIXML_SUCCESS) {
                newField.length_=static_cast <decltype(newField.length_)>(length);
            };

            const auto*pPrimitiveType = pElem->Attribute (PRIMITIVE_TYPE);
            if (!pPrimitiveType) {
                LOGERROR ("Found entry in types|type without primitiveType with name "<<newField.name_);
                continue;
            }
            auto primitiveType=PrimitiveTypeFromString (*pPrimitiveType);
            if (primitiveType==PrimitiveType::none_t) {
                LOGERROR ("Found entry in types|type with unrecognized primitiveType "<<*pPrimitiveType<<" with name "<<newField.name_);
                continue;
            }

            //Add it to the list of known fields
            ilink3Fields_.emplace (newField.name_,newField);
        };
    }

    //TODO add to web page and load other components

    return true;
}
