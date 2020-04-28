//
// Created by pbeerkens on 4/24/20.
//
#include <iostream>
#include "MsgFactory.h"
#include "ILink3Headers.h"
#include "ILink3MsgDefinition.h"
#include <Logger/LogMacros.h>
#include <tinyxml/tinyxml.h>
#include <map>


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
            ILink3FieldDefinition newField;

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
            newField.primitiveType_=primitiveType;

            //Add it to the list of known fields
            ilink3Fields_.emplace (newField.name_,newField);
        };
    }

    //TODO load composit fields and enums

    //Read messages
    {
        static std::string MESSAGE_TAG ("ns2:message");
        pElem = hRoot.FirstChild(MESSAGE_TAG).Element();
        for (; pElem; pElem = pElem->NextSiblingElement(MESSAGE_TAG)) {
            ILink3MsgDefinition newMsgDefinition;

            static std::string NAME ("name");
            static std::string DESCRIPTION ("description");
            static std::string BLOCK_LENGTH ("blockLength");
            static std::string ID ("id");

            const std::string*nameStr=pElem->Attribute(NAME);
            if (nameStr) {
                newMsgDefinition.name_ = *nameStr;
            } else {
                LOGERROR("Found entry in ns2:message without a name.");
                continue;
            }

            const auto*pDescription = pElem->Attribute (DESCRIPTION);
            if (pDescription) newMsgDefinition.description_=*pDescription;

            int blockLength;
            if (pElem->QueryIntAttribute (BLOCK_LENGTH, &blockLength)==TIXML_SUCCESS) {
                newMsgDefinition.blockLength_=static_cast <decltype(newMsgDefinition.blockLength_)>(blockLength);
            };

            int id;
            if (pElem->QueryIntAttribute (ID, &id)==TIXML_SUCCESS) {
                newMsgDefinition.id_=static_cast <decltype(newMsgDefinition.id_)>(id);
            };

            //TODO read fields next

            ilink3Msgs_.emplace (newMsgDefinition.id_, newMsgDefinition);
        }
    }

    return true;
}

void MsgFactory::WebRequestMsgFormat(const webserver::UrlRequest &, std::ostream &os) const {

    std::map<std::string, std::string> orderedRows; //sort them by field name
    for (const auto&field: ilink3Fields_) {
        std::stringstream ss;
        ss<<"<tr><td>"<<field.second.name_;
        ss<<"<td>"<<field.second.length_;
        ss<<"<td>"<<field.second.primitiveType_;
        ss<<"<td>"<<field.second.description_;
        orderedRows.emplace (field.first, ss.str ());
    }

    std::map<ILink3MsgDefinition::ID, std::string> orderedMsgRows;
    for (const auto&msg: ilink3Msgs_) {
        std::stringstream ss;
        ss<<"<table border='1' style='width:400px'>";
        ss<<"<tr><th>"<<msg.second.id_<<"<th colspan='3'>"<<msg.second.description_;
        ss<<"<tr><th colspan='4'>block length="<<msg.second.blockLength_;
        ss<<"</table>";
        orderedMsgRows.emplace (msg.second.id_, ss.str ());
    }

    os<<"<table><tr><td>";
    //print all messages on left hand side
    for (const auto& row: orderedMsgRows) {
        os<<row.second;
    }

    os<<"<td style='vertical-align:top'><table border='1'>";
    //print all fields on left hand side
    os<<"<tr><th colspan='4'>Fields";
    for (const auto&row: orderedRows){
        os<<row.second;
    }
    os<<"</table></table>";
}
