//
// Created by fedor on 9/29/19.
//

#ifndef ROOTEXTRACTION_JSONWRITER_HPP
#define ROOTEXTRACTION_JSONWRITER_HPP

#include <ostream>
#include <TROOT.h>

class JSONWriter {
public:
    explicit JSONWriter(std::ostream& stream);
    void BeginBlock();
    void BeginBlock(const std::string& name);
    void EndBlock();
    void AddProperty(const std::string& name, const std::string& value);
    void AddProperty(const std::string& name, const int64_t& value);
    void AddProperty(const std::string& name, const double& value);
private:
    std::ostream& stream;
    uint32_t current_depth = 0;
    const std::string block_begin = "{\n";
    const std::string block_end = "}";
    bool last_property = false;

    void CheckForComma();
    void Depth();

    //ClassDef(JSONWriter, 2222);
};


#endif //ROOTEXTRACTION_JSONWRITER_HPP
