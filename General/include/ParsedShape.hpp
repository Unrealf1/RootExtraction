//
// Created by fedor on 5/2/20.
//

#ifndef ROOTEXTRACTION_PARSEDSHAPE_HPP
#define ROOTEXTRACTION_PARSEDSHAPE_HPP

#include <TGeoTube.h>
#include <TGeoBoolNode.h>
#include <TGeoCompositeShape.h>
#include <TGDMLWrite.h>
#include "TGeoManagerParser.hpp"

struct Translation {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
};

struct Rotation {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
};

struct Scale {
    double x = 1.0;
    double y = 1.0;
    double z = 1.0;
};

struct ParsedShape {
    std::string className = "UnknownClass";
    virtual void write(JSONWriter& wr) = 0;
    virtual ~ParsedShape() = default;
};

struct ParsedBox : public ParsedShape {
    explicit ParsedBox(TGeoBBox* box) {
        xSize = box->GetDX() * 2.0;
        ySize = box->GetDY() * 2.0;
        zSize = box->GetDZ() * 2.0;
    }

    std::string className = "3d.box";
    double xSize = 0;
    double ySize = 0;
    double zSize = 0;
    void write(JSONWriter& wr) override {}
    ~ParsedBox() override = default;
};

struct ParsedTube : public ParsedShape {
    explicit ParsedTube(TGeoTube* tube) {
        rmin = tube->GetRmin();
        rmax = tube->GetRmax();
        height = tube->GetDz();
    }

    std::string className = "3d.tube";
    double rmin = 0;
    double rmax = 0;
    double height = 0;
    void write(JSONWriter& wr) override {}
    ~ParsedTube() override = default;
};

struct ParsedComposite : public ParsedShape {
    explicit ParsedComposite(TGeoCompositeShape* composite) {
        switch (composite->GetBoolNode()->GetBooleanOperator()) {
            case TGeoBoolNode::kGeoUnion:
                operation = "UNION";
                break;
            case TGeoBoolNode::kGeoSubtraction:
                operation = "SUBTRACT";
                break;
            case TGeoBoolNode::kGeoIntersection:
                operation = "INTERSECT";
                break;
        }
        const Double_t  *ltr = composite->GetBoolNode()->GetLeftMatrix()->GetTranslation();
        leftTranslation.x = ltr[0];
        leftTranslation.y = ltr[1];
        leftTranslation.z = ltr[2];

        const Double_t  *rtr = composite->GetBoolNode()->GetLeftMatrix()->GetTranslation();
        rightTranslation.x = rtr[0];
        rightTranslation.y = rtr[1];
        rightTranslation.z = rtr[2];
        
        left = TGeoManagerParser::parseShape(composite->GetBoolNode()->GetLeftShape());
        right = TGeoManagerParser::parseShape(composite->GetBoolNode()->GetRightShape());
    }

    std::string className = "3d.composite";
    std::string operation = "unknown";
    ParsedShape* left = nullptr;
    ParsedShape* right = nullptr;
    ~ParsedComposite() override {
        delete left;
        delete right;
    }
    Translation leftTranslation;
    Translation rightTranslation;
    void write(JSONWriter& wr) override {}
};

#endif //ROOTEXTRACTION_PARSEDSHAPE_HPP
