//
// Created by fedor on 5/2/20.
//

#include <TGeoManager.h>
#include <iostream>
#include <TGeoBBox.h>
#include <unordered_map>
#include "TGeoManagerParser.hpp"
#include "RootMagic.hpp"

ParsedTGeoManager TGeoManagerParser::parse(TGeoManager *geoManager) {
    //This is dark magic.
    StructLst* fAccPatt = new StructLst;
    fAccPatt->fLst["TGeoPatternX"] = kTRUE;
    fAccPatt->fLst["TGeoPatternY"] = kTRUE;
    fAccPatt->fLst["TGeoPatternZ"] = kTRUE;
    fAccPatt->fLst["TGeoPatternCylR"] = kTRUE;
    fAccPatt->fLst["TGeoPatternCylPhi"] = kTRUE;
    StructLst* fRejShape = new StructLst;
    fRejShape->fLst["TGeoTrd1"] = kTRUE;
    fRejShape->fLst["TGeoTrd2"] = kTRUE;

    ParsedTGeoManager parsed;

    TGeoNode* top_node = geoManager->GetTopNode();

    TList* materials = geoManager->GetListOfMaterials();
    std::unordered_set<ParsedMaterial*> parsed_materials = parseMaterials(materials);
    std::unordered_map<std::string, ParsedVolume *> parsed_volumes;
    parseVolumes(top_node, parsed_volumes, fAccPatt, fRejShape);

    UnsetTemporaryBits(geoManager);
    delete fAccPatt;
}

std::unordered_set<ParsedMaterial *> TGeoManagerParser::parseMaterials(TList *materials) {
    TIter next(materials);
    TGeoMaterial *material;

    std::unordered_set<ParsedMaterial *> result;

    while ((material = (TGeoMaterial *)next())) {
        TString lname = material->GetName();

        if (material->IsMixture()) {
            TGeoMixture  *lmixture = (TGeoMixture *)material;
            std::cout << "Can't work with mixtures yet" << std::endl;
        } else {
            result.insert(parseMaterial(material));
        }
    }
    return result;
}

ParsedMaterial *TGeoManagerParser::parseMaterial(TGeoMaterial *material) {
    return new ParsedMaterial(material->GetName(), "");
}

void TGeoManagerParser::parseVolumes(
        TGeoNode *node,
        std::unordered_map<std::string, ParsedVolume *>& parsedVolumes,
        StructLst* fAccPatt,
        StructLst* fRejShape) {

    TGeoVolume * volume = node->GetVolume();
    std::string volname = volume->GetName();

    TGeoPatternFinder* pattFinder = nullptr;
    Bool_t isPattern = kFALSE;
    std::string pattClsName;

    ParsedVolume* parsedVolume = nullptr;


    if (volume->IsAssembly()) {
        std::cout << "Can't work with assembly yet\n";
    }
    //} else {

        std::string material_name = volume->GetName();
        ParsedShape* shape = parseShape(volume->GetShape());
        if (shape == nullptr) {
            std::cout << "Can't add volume: invalid shape\n";
        }
        parsedVolume = new ParsedVolume(volname, shape, material_name);

        pattFinder = volume->GetFinder();
        if (pattFinder) {
            pattClsName = TString::Format("%s", pattFinder->ClassName());
            TString shapeCls = TString::Format("%s", volume->GetShape()->ClassName());
            if ((fAccPatt->fLst[pattClsName] == kTRUE) &&
                (fRejShape->fLst[shapeCls] != kTRUE)) {
                isPattern = kTRUE;
            }
        }
    //}

    TObjArray *nodeLst = volume->GetNodes();
    TIter next(nodeLst);
    TGeoNode *geoNode;
    while ((geoNode = (TGeoNode *) next())) {
        ParsedPhysVolume* physVolume = new ParsedPhysVolume;
        physVolume->name = geoNode->GetName();
        physVolume->volume_name = volname;

        TGeoVolume * subvol = geoNode->GetVolume();
        if (subvol->TestAttBit(fgkProcBitVol) == kFALSE) {
            subvol->SetAttBit(fgkProcBitVol);
            parseVolumes(geoNode, parsedVolumes, fAccPatt, fRejShape);
        }
        std::string subvolName = geoNode->GetVolume()->GetName();
        if (isPattern == kFALSE) {
            std::string nodename = geoNode->GetName();
            nodename = nodename + "in" + volname;
            const Double_t * pos = geoNode->GetMatrix()->GetTranslation();
            Translation tr;
            tr.x = pos[0];
            tr.y = pos[1];
            tr.z = pos[2];
            physVolume->tr = tr;

            Double_t lx, ly, lz;
            Double_t xangle = 0;
            Double_t zangle = 0;
            lx = geoNode->GetMatrix()->GetRotationMatrix()[0];
            ly = geoNode->GetMatrix()->GetRotationMatrix()[4];
            lz = geoNode->GetMatrix()->GetRotationMatrix()[8];
            if (geoNode->GetMatrix()->IsReflection()
                && TMath::Abs(lx) == 1 &&  TMath::Abs(ly) == 1 && TMath::Abs(lz) == 1) {
                Scale scl;
                scl.x = lx;
                scl.y = ly;
                scl.z = lz;
                physVolume->scl = scl;

                //this magic is so dark and powerful, that no one should ever try to understand it
                if (lx == -1) {
                    zangle = 180;
                }
                if (lz == -1) {
                    xangle = 180;
                }
            }

            Rotation rot = GetXYZangles(geoNode->GetMatrix()->GetRotationMatrix());
            rot.x -= xangle;
            rot.z -= zangle;
            physVolume->rot = rot;

            parsedVolume->children.push_back(physVolume);
        }
    }

    if (isPattern && pattFinder) {
        Int_t ndiv, divaxis;
        Double_t offset, width, xlo, xhi;
        TString axis, unit;

        ndiv = pattFinder->GetNdiv();
        width = pattFinder->GetStep();

        divaxis = pattFinder->GetDivAxis();
        volume->GetShape()->GetAxisRange(divaxis, xlo, xhi);

        //compute relative start (not positional)
        offset = pattFinder->GetStart() - xlo;

        //TODO Finish this function
        std::cerr << "Can't work with divisions" << std::endl;
        /*axis = GetPattAxis(divaxis, pattClsName, unit);

        //create division node
        childN = CreateDivisionN(offset, width, ndiv, axis.Data(), unit.Data(), nodeVolNameBak.Data());
        fGdmlE->AddChild(volumeN, childN);*/
    }

    if (parsedVolume != nullptr) {
        parsedVolumes[volname] = parsedVolume;
    }
}

ParsedShape* TGeoManagerParser::parseShape(TGeoShape* shape) {
    if (shape == nullptr) {
        return nullptr;
    }
    std::string class_name = shape->ClassName();

    if (class_name == "TGeoBox") {
        return new ParsedBox(dynamic_cast<TGeoBBox*>(shape));
    } else if (class_name == "TGeoCompositeShape") {
        return new ParsedComposite(dynamic_cast<TGeoCompositeShape*>(shape));
    } else if (class_name == "TGeoTube") {
        return new ParsedTube(dynamic_cast<TGeoTube*>(shape));
    }

}

void TGeoManagerParser::UnsetTemporaryBits(TGeoManager *geoMng) {
    TIter next(geoMng->GetListOfVolumes());
    TGeoVolume *vol;
    while ((vol = (TGeoVolume *)next())) {
        ((TObject *)vol->GetShape())->SetBit(fgkProcBit, kFALSE);
        vol->SetAttBit(fgkProcBitVol, kFALSE);
    }
}

