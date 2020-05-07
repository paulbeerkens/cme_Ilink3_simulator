#include <iostream>
#include <fstream>
#include <tinyxml/tinyxml.h>
#include <ILink3Common/ILink3FieldDefinition.h>
#include <ILink3Common/ILink3EnumDefinition.h>
#include <map>
#include <ILink3Common/ILink3MsgDefinition.h>

int main (int argc, [[maybe_unused]]char** argv) {

    if (argc<2) {
        std::cout<<"CodeGenerator failed to run."<<std::endl;
        std::cout<<"Please specify the CME ILINK3 Message spec as the first paramater."<<std::endl;
        return 1;
    };

    std::cout<< "CodeGenerator: Loading ILINK3 message formats from "<<argv [1]<<std::endl;

    static std::string NAME_TAG("name");
    static std::string DESCRIPTION_TAG("description");
    static std::string BLOCK_LENGTH_TAG("blockLength");
    static std::string ID_TAG("id");
    static std::string FIELD_TAG("field");
    static std::string LENGTH_TAG("length");
    static std::string PRIMITIVE_TYPE_TAG("primitiveType");
    static std::string MESSAGE_TAG("ns2:message");
    static std::string NULL_VALUE_TAG ("nullValue");
    static std::string ENCODING_TYPE_TAG ("encodingType");
    static std::string TYPE_TAG ("type");


    TiXmlDocument doc;
    if (!doc.LoadFile(argv [1])) {
        std::cout<<"CodeGenerator: Failed to load the ILINK3 message format xml document from "<<argv [1]<<std::endl;
        return 1;
    }

    TiXmlHandle hDoc(&doc);
    TiXmlElement* pElem;
    TiXmlHandle hRoot(0);

    {
        pElem=hDoc.FirstChildElement().Element();
        // should always have a valid root but handle gracefully if it does
        if (!pElem) return 1;

        // save this for later
        hRoot=TiXmlHandle(pElem);
    }

    //Get Major version (Schema ID)
    const char* idPtr=pElem->Attribute("id");
    std::string id;
    if (idPtr) id=*idPtr;

    //Get minor version
    const char* versionPtr=pElem->Attribute("version");
    std::string version;
    if (versionPtr) version=*versionPtr;


    std::unordered_map<std::string, ILink3FieldDefinition> ilink3Fields;

    //First read the field types
    {
        pElem = hRoot.FirstChild("types").FirstChild("type").Element();
        for (; pElem; pElem = pElem->NextSiblingElement("type")) {
            ILink3FieldDefinition newField;

            const std::string*nameStr=pElem->Attribute(NAME_TAG);
            if (nameStr) {
                newField.name_ = *nameStr;
            } else {
                std::cout<<"CodeGenerator: Found entry in types|type node without a name."<<std::endl;
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
                std::cout<<"CodeGenerator: Found entry in types|type without primitiveType with name "<<newField.name_<<std::endl;
                continue;
            }
            auto primitiveType=PrimitiveTypeFromString (*pPrimitiveType);
            if (primitiveType==PrimitiveType::none_t) {
                std::cout<<"CodeGenerator: Found entry in types|type with unrecognized primitiveType "<<*pPrimitiveType<<" with name "<<newField.name_<<std::endl;
                continue;
            }
            newField.primitiveType_=primitiveType;

            const auto* nullValueStr=pElem->Attribute (NULL_VALUE_TAG);
            if (nullValueStr) {
                try {
                    newField.nullValue = std::stoul(*nullValueStr);
                } catch (const std::exception& e) {};
            }

            //Add it to the list of known fields
            ilink3Fields.emplace (newField.name_,newField);
        };
    }
    std::cout<<"CodeGenerator: Loaded "<<ilink3Fields.size ()<<" fields."<<std::endl;

    std::size_t enums {0};
    std::size_t validValues {0};
    std::map <std::string, ILink3EnumDefinition> enumDefinitions; //map so that they show up sorted
    {
        pElem = hRoot.FirstChild("types").FirstChild("enum").Element();
        for (; pElem; pElem = pElem->NextSiblingElement("enum")) {
            ++enums;
            ILink3EnumDefinition newEnum;

            const std::string*nameStr=pElem->Attribute(NAME_TAG);
            if (nameStr) {
                newEnum.name_ = *nameStr;
            } else {
                std::cout<<"CodeGenerator: Found entry in types|enum node without a name."<<std::endl;
                continue;
            }

            const auto*pEncodingType = pElem->Attribute (ENCODING_TYPE_TAG);
            if (!pEncodingType) {
                std::cout<<"CodeGenerator: Enum without encoding type in node: "<<newEnum.name_;
                continue;
            }

            auto fieldItr=ilink3Fields.find (*pEncodingType);
            if (fieldItr==ilink3Fields.end ()) {
                std::cout<<"CodeGenerator: Unknown type "<<*pEncodingType<<" for enum "<<newEnum.name_;
                continue;
            }

            //link the enum to its encoding type
            newEnum.encodingType_=&fieldItr->second;

            TiXmlElement* pValues= pElem->FirstChildElement("validValue");
            for (; pValues; pValues = pValues->NextSiblingElement("validValue")) {
                ++validValues;
                ILink3EnumValidValues newValidValue;

                const std::string*valueNameStr=pValues->Attribute(NAME_TAG);
                if (!valueNameStr) {
                    std::cout<<"CodeGenerator: Found validValue in types|enum|validValue node without a name for enum."<<newEnum.name_<<std::endl;
                    continue;
                }
                newValidValue.name_=*valueNameStr;

                const auto*pDescription = pValues->Attribute (DESCRIPTION_TAG);
                if (pDescription) newValidValue.description_=*pDescription;

                newValidValue.value_=pValues->GetText();
                newEnum.validValues_.emplace_back(newValidValue);
            }

            enumDefinitions.emplace (newEnum.name_, newEnum);
        }
    }
    std::cout<<"CodeGenerator: Loaded "<<enums<<" enums with a total of "<<validValues<<" valid values"<<std::endl;

    std::size_t fields {0};
    std::map <MsgId, ILink3MsgDefinition> msgDefinitions; //map so that they show up sorted
    {
        pElem = hRoot.FirstChild("ns2:message").Element();
        for (; pElem; pElem = pElem->NextSiblingElement("ns2:message")) {
            ILink3MsgDefinition newMsgDef;

            //We use the descriptionStr rather than the name because the name has the
            //id in it and description is the name without. Probably looks slightly better.
            const std::string *descriptionStr = pElem->Attribute(DESCRIPTION_TAG);
            if (descriptionStr) {
                newMsgDef.name_=*descriptionStr;
            } else {
                std::cout << "CodeGenerator: Found entry in ns2:message node without a description." << std::endl;
                continue;
            }

            const std::string *idStr = pElem->Attribute(ID_TAG);
            if (idStr) {
                try {
                    newMsgDef.msgId_ = static_cast<MsgId>(std::stoll(*idStr));
                } catch (std::exception& e) {
                    std::cout<< "CodeGenerator: Found entry in ns2:message node "<<newMsgDef.name_<<" with an invalid id."<<std::endl;
                    continue;
                };
            } else {
                std::cout<< "CodeGenerator: Found entry in ns2:message node "<<newMsgDef.name_<<" without an id."<<std::endl;
                continue;
            }

            const std::string *blockLengthStr = pElem->Attribute(BLOCK_LENGTH_TAG);
            if (blockLengthStr) {
                try {
                    newMsgDef.blockLength_ = std::stoll(*blockLengthStr);
                } catch (const std::exception& e) {
                    std::cout<<"CodeGenerator: ound entry in ns2:message node "<<newMsgDef.name_<<" with an invalid blockLength."<<std::endl;
                    continue;
                }
            }

            TiXmlElement* pValues= pElem->FirstChildElement(FIELD_TAG);
            for (; pValues; pValues = pValues->NextSiblingElement(FIELD_TAG)) {
                ILink3FieldInMsgDefinition newField;

                const std::string*nameStr=pValues->Attribute(NAME_TAG);
                if (!nameStr) {
                    std::cout<<"CodeGenerator: Found entry in ns2:message|field node without a name."<<std::endl;
                    continue;
                }
                newField.name_=*nameStr;

                const std::string*typeStr = pValues->Attribute (TYPE_TAG);
                if (!typeStr) {
                    std::cout<<"CodeGenerator: Found entry in ns2:message|field node without type in node: "<<newMsgDef.name_<<std::endl;
                    continue;
                }

                auto fieldItr=ilink3Fields.find (*typeStr);
                auto enumItr=enumDefinitions.find (*typeStr);

                if (fieldItr==ilink3Fields.end ()&&enumItr==enumDefinitions.end ()) {
                    std::cout<<"CodeGenerator: Unknown type "<<*typeStr<<" for msg "<<newMsgDef.name_<<std::endl;
                    continue;
                }
                if (fieldItr!=ilink3Fields.end ()){
                    newField.fieldDefinition_=&fieldItr->second;
                }
                if (enumItr!=enumDefinitions.end ()) {
                    newField.enumDefinition_=&enumItr->second;
                }

            }

            msgDefinitions [newMsgDef.msgId_]=newMsgDef;
        }
    }
    std::cout<<"CodeGenerator: Loaded "<<msgDefinitions.size ()<<" msgs with a total of "<<fields<<" fields."<<std::endl;


    //****************************** Start of writing to files *******************************

    std::ofstream constFile;
    constFile.open ("Generated/ILink3ConstGen.h");
    constFile<<"//File generated by CodeGenerator"<<std::endl;
    constFile<<"//Generated from: "<<argv [1]<<std::endl;
    constFile<<std::endl;

    constFile<<"#ifndef CMESIMULATOR_ILINK3_CONST_GEN_H"<<std::endl;
    constFile<<"#define CMESIMULATOR_ILINK3_CONST_GEN_H"<<std::endl;
    constFile<<std::endl;
    constFile<<"namespace IL3Const {"<<std::endl;
    constFile<<std::endl;

    constFile<<"inline static const std::uint16_t SCHEMA_ID="<<id<<";"<<std::endl;
    constFile<<"inline static const std::uint16_t VERSION="<<version<<";"<<std::endl;

    constFile<<"} //end of namespace IL3Const"<<std::endl;

    constFile<<"#endif //CMESIMULATOR_ILINK3_CONST_GEN_H"<<std::endl;
    constFile.close ();

    //***************** enum file ***********************************
    //The enum file will have entries like this:
    //enum class TimeInForce: std::uint8_t{
    //      Day=0 //Day
    //    , GoodTillCancel=1 //Good Till Cancel
    //    , FillAndKill=3 //Fill And Kill
    //    , FillOrKill=4 //Fill Or Kill
    //    , GoodTillDate=6 //Good Till Date
    //};

    std::ofstream enumFile;
    enumFile.open ("Generated/ILink3EnumsGen.h");

    enumFile<<"//File generated by CodeGenerator"<<std::endl;
    enumFile<<"//Generated from: "<<argv [1]<<std::endl;
    enumFile<<std::endl;

    enumFile<<"#ifndef CMESIMULATOR_ILINK3_ENUMS_GEN_H"<<std::endl;
    enumFile<<"#define CMESIMULATOR_ILINK3_ENUMS_GEN_H"<<std::endl;

    enumFile<<"#include <cstdint>"<<std::endl;
    enumFile<<std::endl;

    enumFile<<"namespace IL3Enum {"<<std::endl;
    enumFile<<std::endl;

    for (const auto& anEnum: enumDefinitions) {
        //enum class FTI: std::uint8_t {Backup=0, Primary=1, Null=255};
        enumFile<<"enum class "<<anEnum.second.name_;
        enumFile<<": "<<anEnum.second.encodingType_->primitiveType_;
        enumFile<<"{"<<std::endl;
        std::string preFix {"  "};
        for (const auto&validValue: anEnum.second.validValues_) {
            enumFile<<preFix<<validValue.name_<<"=";
            if (anEnum.second.encodingType_->primitiveType_==PrimitiveType::char_t) {
                enumFile << "'"<<validValue.value_<<"'";
            } else {
                enumFile << validValue.value_;
            }
            enumFile<<" //"<<validValue.description_<<std::endl;
            preFix=", ";
        }
        if (anEnum.second.encodingType_->nullValue.has_value()) {
            enumFile<<preFix<<"Null="<<*anEnum.second.encodingType_->nullValue;
        }
        enumFile<<"};"<<std::endl<<std::endl;
    }
    enumFile<<"} //end of namespace IL3Enum"<<std::endl;

    enumFile<<"#endif //CMESIMULATOR_ILINK3_ENUMS_GEN_H"<<std::endl;
    enumFile.close ();

    //****************** Msg File ************************
    std::ofstream msgFile;
    msgFile.open ("Generated/ILink3MsgsGen.h");

    msgFile<<"//File generated by CodeGenerator"<<std::endl;
    msgFile<<"//Generated from: "<<argv [1]<<std::endl;
    msgFile<<std::endl;

    msgFile<<"#ifndef CMESIMULATOR_ILINK3_MSGS_GEN_H"<<std::endl;
    msgFile<<"#define CMESIMULATOR_ILINK3_MSGS_GEN_H"<<std::endl;

    msgFile<<"#include <cstdint>"<<std::endl;
    msgFile<<std::endl;

    msgFile<<"namespace IL3Msg {"<<std::endl;
    msgFile<<std::endl;

    for (const auto& msgDef: msgDefinitions) {
        msgFile<<"class "<<msgDef.second.name_<<" {"<<std::endl;
        msgFile<<"public:"<<std::endl;
        msgFile<<"\tinline static const std::size_t id{"<<msgDef.second.msgId_<<"};"<<std::endl;

        msgFile<<"protected:"<<std::endl;
        msgFile<<"\tstruct __attribute__ ((packed))  BlockData {"<<std::endl;
        msgFile<<"\t};"<<std::endl;
        msgFile<<std::endl;
        msgFile<<"\tconst BlockData *blockData_{nullptr};"<<std::endl;

        msgFile<<"};"<<std::endl;
        msgFile<<std::endl;
    }

    msgFile<<"} //end of namespace IL3Msg"<<std::endl;

    msgFile<<"#endif //CMESIMULATOR_ILINK3_MSGS_GEN_H"<<std::endl;
    msgFile.close ();

    return 0;
}