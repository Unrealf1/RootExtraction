//
// Created by fedor on 9/29/19.
//

#include "JSON/JSONWriter.hpp"

JSONWriter::JSONWriter(std::ostream &stream): stream(stream) {

}

void JSONWriter::BeginBlock() {
    stream << block_begin;
    last_property = false;
    ++current_depth;
}

void JSONWriter::EndBlock() {
    --current_depth;
    Depth();
    stream << block_end;
}

void JSONWriter::AddProperty(const std::string &name, const std::string& value) {
    CheckForComma();
    Depth();
    stream << '\"' << name << "\": \"" << value << '\"';
    last_property = true;
}

void JSONWriter::AddProperty(const std::string &name, const int64_t& value) {
    CheckForComma();
    Depth();
    stream << '\"' << name << "\": " << std::to_string(value);
    last_property = true;
}

void JSONWriter::AddProperty(const std::string &name) {
    CheckForComma();
    Depth();
    stream << '\"' << name << "\": ";
    last_property = true;
}

void JSONWriter::CheckForComma() {
    if (last_property) {
        stream << ",\n";
    } else {
        stream << '\n';
    }
}

void JSONWriter::Depth() {
    stream << std::string(4 * current_depth, ' ');
}
