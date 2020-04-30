#include <iostream>
#include <fstream>
#include <tinyxml/tinyxml.h>
#include <ILink3Common/ILink3FieldDefinition.h>
#include <ILink3Common/ILink3EnumDefinition.h>
#include <map>

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

    std::map <std::string, ILink3EnumDefinition> enumDefinitions; //map so that they show up sorted
    {
        pElem = hRoot.FirstChild("types").FirstChild("enum").Element();
        for (; pElem; pElem = pElem->NextSiblingElement("enum")) {
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
            newEnum.encodingType=&fieldItr->second;
            enumDefinitions.emplace (newEnum.name_, newEnum);
        }
    }

    std::ofstream enumFile;
    enumFile.open ("Ilink3EnumsGen.h");
    enumFile<<std::endl;
    enumFile<<"namespace IL3Enum {"<<std::endl;

    enumFile<<"//File generated by CodeGenerator"<<std::endl;
    enumFile<<"//Generated from: "<<argv [1]<<std::endl;
    enumFile<<std::endl;
    enumFile<<"#ifndef CMESIMULATOR_ILINK3_ENUMS_GEN_H"<<std::endl;
    enumFile<<"#define CMESIMULATOR_ILINK3_ENUMS_GEN_H"<<std::endl;

    enumFile<<"#include <cstdint>"<<std::endl;
    enumFile<<std::endl;

    for (const auto& anEnum: enumDefinitions) {
        //enum class FTI: std::uint8_t {Backup=0, Primary=1, Null=255};
        enumFile<<"enum class "<<anEnum.second.name_;
        enumFile<<": "<<anEnum.second.encodingType->primitiveType_;
        enumFile<<"{";
        if (anEnum.second.encodingType->nullValue.has_value()) {
            enumFile<<"Null="<<*anEnum.second.encodingType->nullValue;
        }
        enumFile<<"};"<<std::endl;
    }
    enumFile<<"} //end of namespace IL3Enum"<<std::endl;


        enumFile<<"#endif //CMESIMULATOR_ILINK3_ENUMS_GEN_H"<<std::endl;
    enumFile.close ();
    return 0;
}