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


std::unique_ptr<ILink3Msg> MsgFactory::processMessage(MessageBuffer &msgBuffer) {
    const SBEHeader* sbeHeader= reinterpret_cast<const SBEHeader*>(msgBuffer.getRdPtr());
    msgBuffer.moveRdPtr (sizeof (SBEHeader));
    std::cout<<sbeHeader->blockLength_<<std::endl;
    std::cout<<sbeHeader->templateId_<<std::endl;

    switch (sbeHeader->templateId_) {
        case NegotiateMsg::id:
            NegotiateMsg newMsg;
            newMsg.readFromBuffer (msgBuffer);
            if (msgBuffer.bad ()) {
                LOGERROR ("Failed to read msg with id "<<NegotiateMsg::id);
                return nullptr;
            }

            std::cout<<newMsg.getAccessKeyID()<<std::endl;
            std::cout<<newMsg.getFirm()<<std::endl;
            std::cout<<newMsg.getSession()<<std::endl;

    }
/*
    const Negotiate500* msg=reinterpret_cast<const Negotiate500*>(msgBuffer.getRdPtr());
    std::cout<<msg->UUID<<std::endl;
    msgBuffer.moveRdPtr (sizeof (Negotiate500));
    std::cout<<msgBuffer.getLeftToRead()<<std::endl;
*/
    return nullptr;
}

bool MsgFactory::initialize(MsgFactorySettings &msgFactorySettings) {
    static std::string NAME_TAG("name");
    static std::string DESCRIPTION_TAG("description");
    static std::string BLOCK_LENGTH_TAG("blockLength");
    static std::string ID_TAG("id");
    static std::string FIELD_TAG("field");
    static std::string LENGTH_TAG("length");
    static std::string PRIMITIVE_TYPE_TAG("primitiveType");
    static std::string MESSAGE_TAG("ns2:message");

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

            const std::string*nameStr=pElem->Attribute(NAME_TAG);
            if (nameStr) {
                newField.name_ = *nameStr;
            } else {
                LOGERROR("Found entry in types|type node without a name.");
                continue;
            }

            const auto*pDescription = pElem->Attribute (DESCRIPTION_TAG);
            if (pDescription) newField.description_=*pDescription;

            int length;
            if (pElem->QueryIntAttribute (LENGTH_TAG, &length) == TIXML_SUCCESS) {
                newField.length_=static_cast <decltype(newField.length_)>(length);
            };

            const auto*pPrimitiveType = pElem->Attribute (PRIMITIVE_TYPE_TAG);
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

    //TODO load composite fields and enums

    //Read messages
    {

        pElem = hRoot.FirstChild(MESSAGE_TAG).Element();
        for (; pElem; pElem = pElem->NextSiblingElement(MESSAGE_TAG)) {
            ILink3MsgDefinition newMsgDefinition;

            const std::string*nameStr=pElem->Attribute(NAME_TAG);
            if (nameStr) {
                newMsgDefinition.name_ = *nameStr;
            } else {
                LOGERROR("Found entry in ns2:message without a name.");
                continue;
            }

            const auto*pDescription = pElem->Attribute (DESCRIPTION_TAG);
            if (pDescription) newMsgDefinition.description_=*pDescription;

            int blockLength;
            if (pElem->QueryIntAttribute (BLOCK_LENGTH_TAG, &blockLength) == TIXML_SUCCESS) {
                newMsgDefinition.blockLength_=static_cast <decltype(newMsgDefinition.blockLength_)>(blockLength);
            };

            int id;
            if (pElem->QueryIntAttribute (ID_TAG, &id) == TIXML_SUCCESS) {
                newMsgDefinition.id_=static_cast <decltype(newMsgDefinition.id_)>(id);
            };

            //Read Fields
            TiXmlElement* pFieldElem=pElem->FirstChildElement (FIELD_TAG);
            for (; pFieldElem; pFieldElem = pFieldElem->NextSiblingElement(FIELD_TAG)) {

                const std::string*fldNameStr=pFieldElem->Attribute(NAME_TAG);
                if (fldNameStr) {
                    newMsgDefinition.name_ = *fldNameStr;
                } else {
                    LOGERROR("Found entry in ns2:message without a name.");
                    continue;
                }
            }



//            ilink3Msgs_.emplace (newMsgDefinition.id_, newMsgDefinition);
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
