//
// Created by fedor on 9/29/19.
//

#ifndef ROOTEXTRACTION_JSONWRITER_HPP
#define ROOTEXTRACTION_JSONWRITER_HPP

#include <ostream>

class JSONWriter {
public:
    explicit JSONWriter(std::ostream& stream);
    void BeginBlock();
    void EndBlock();
    void AddProperty(const std::string& name, const std::string& value);
    void AddProperty(const std::string& name, const int64_t& value);
    void AddProperty(const std::string& name);
private:
    std::ostream& stream;
    uint32_t current_depth = 0;
    const std::string block_begin = "{\n";
    const std::string block_end = "\n}\n";
    bool last_property = false;

    void CheckForComma();
    void Depth();
};


#endif //ROOTEXTRACTION_JSONWRITER_HPP
