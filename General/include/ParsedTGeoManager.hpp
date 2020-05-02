//
// Created by fedor on 2/05/20.
//

#ifndef ROOTEXTRACTION_PARSEDTGEOMANAGER_HPP
#define ROOTEXTRACTION_PARSEDTGEOMANAGER_HPP

#include "JSON/JSONWriter.hpp"
#include "TGeoManagerParser.hpp"
#include "ParsedShape.hpp"
#include <unordered_set>
#include <string>

struct ParsedMaterial {
    ParsedMaterial(const std::string& name, const std::string& colour) :
    name(name), colour(colour) {}


    std::string name;
    std::string colour;
};

struct ParsedPhysVolume;

struct ParsedVolume {
    ParsedVolume(
            const std::string& name,
            ParsedShape* shape,
            const std::string& material_name)
            : name(name), shape(shape), material_name(material_name) {}

    std::string name = "";
    ParsedShape* shape = nullptr;
    std::string material_name = "";
    std::vector<ParsedPhysVolume*> children;
};

struct ParsedPhysVolume {
    std::string name;
    std::string volume_name;
    Translation tr;
    Rotation rot;
    Scale scl;
};

class ParsedTGeoManager {
public:
    ParsedTGeoManager();
    void write(JSONWriter& writer);
private:

};

#endif //ROOTEXTRACTION_PARSEDTGEOMANAGER_HPP

