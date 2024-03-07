/***************************************************************************
 *   Copyright (c) 2011 Juergen Riegel <FreeCAD@juergen-riegel.net>        *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#include "FeatureExtrude.h"

#include "PreCompiled.h"
#ifndef _PreComp_
#endif

#include <TopExp_Explorer.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>

#include <Base/Tools.h>
#include <Mod/Part/App/Part2DObject.h>
#include <Mod/Part/App/FaceMakerCheese.h>


FC_LOG_LEVEL_INIT("NomAnor", true, true)


namespace NomAnor
{

App::PropertyQuantityConstraint::Constraints const signedLengthConstraint = { -DBL_MAX, DBL_MAX, 1.0 };
double const maxAngle = 90 - Base::toDegrees<double>(Precision::Angular());
App::PropertyAngle::Constraints const floatAngle = { -maxAngle, maxAngle, 1.0 };

PROPERTY_SOURCE(NomAnor::FeatureExtrude, NomAnor::FeatureToolShape)

char const* FeatureExtrude::TypeEnums[] = {"Length", "TwoLengths", nullptr};

FeatureExtrude::FeatureExtrude()
{
    ADD_PROPERTY_TYPE(Profile, (nullptr), "Extrude", App::Prop_None, "Reference to sketch");
    ADD_PROPERTY_TYPE(Midplane, (0), "Extrude", App::Prop_None, "Extrude symmetric to sketch face");
    ADD_PROPERTY_TYPE(Reversed, (0), "Extrude", App::Prop_None, "Reverse extrusion direction");

    ADD_PROPERTY_TYPE(Type, (0L), "Pad", App::Prop_None, "Pad type");
    Type.setEnums(TypeEnums);
    ADD_PROPERTY_TYPE(Length, (10.0), "Pad", App::Prop_None, "Pad length");
    ADD_PROPERTY_TYPE(Length2, (10.0), "Pad", App::Prop_None, "Pad length in 2nd direction");
    ADD_PROPERTY_TYPE(Offset, (0.0), "Pad", App::Prop_None, "Offset from face in which pad will end");
    Offset.setConstraints(&signedLengthConstraint);
}

short FeatureExtrude::mustExecute() const
{
    if (Profile.isTouched() ||
        Midplane.isTouched() ||
        Reversed.isTouched() ||
        
        Type.isTouched() ||
        Length.isTouched() ||
        Length2.isTouched() ||
        Offset.isTouched()) {
        return 1;
    }
    return NomAnor::FeatureToolShape::mustExecute();
}

App::DocumentObjectExecReturn* FeatureExtrude::execute()
{
    std::string const method = Type.getValueAsString();

    // disable settings that are not valid on the current method
    // disable everything unless we are sure we need it
    bool isLengthEnabled = false;
    bool isLength2Enabled = false;
    bool isOffsetEnabled = false;
    bool isMidplaneEnabled = false;
    bool isReversedEnabled = false;
    if (method == "Length") {
        isLengthEnabled = true;
        isMidplaneEnabled = true;
        isReversedEnabled = !Midplane.getValue();
    }
    else if (method == "TwoLengths") {
        isLengthEnabled = true;
        isLength2Enabled = true;
        isReversedEnabled = true;
    }

    Length.setReadOnly(!isLengthEnabled);
    Length2.setReadOnly(!isLength2Enabled);
    Offset.setReadOnly(!isOffsetEnabled);
    Midplane.setReadOnly(!isMidplaneEnabled);
    Reversed.setReadOnly(!isReversedEnabled);

    return FeatureToolShape::execute();
}

TopoDS_Shape FeatureExtrude::getToolShape() const
{
    double L = Length.getValue();
    double L2 = Length2.getValue();
    bool const hasMidplane = Midplane.getValue();
    bool const hasReversed = Reversed.getValue();

    std::string const method(Type.getValueAsString());

    TopoDS_Shape sketchshape = getVerifiedFace();
    Base::Vector3d SketchVector = getProfileNormal();

    auto invObjLoc = getLocation().Inverted();

    Base::Vector3d paddingDirection = SketchVector;

    // create vector in padding direction with length 1
    gp_Dir dir(paddingDirection.x, paddingDirection.y, paddingDirection.z);

    // The length of a gp_Dir is 1 so the resulting pad would have
    // the length L in the direction of dir. But we want to have its height in the
    // direction of the normal vector.
    // Therefore we must multiply L by the factor that is necessary
    // to make dir as long that its projection to the SketchVector
    // equals the SketchVector.
    // This is the scalar product of both vectors.
    // Since the pad length cannot be negative, the factor must not be negative.
    double factor = fabs(dir * gp_Dir(SketchVector.x, SketchVector.y, SketchVector.z));

    // factor would be zero if vectors are orthogonal
    if (factor < Precision::Confusion()) {
        throw Base::ValueError(QT_TRANSLATE_NOOP("Exception", "Pad: Creation failed because direction is orthogonal to sketch's normal vector"));
    }

    L = L / factor;
    L2 = L2 / factor;

    dir.Transform(invObjLoc.Transformation());

    if (sketchshape.IsNull()) {
        throw Base::RuntimeError(QT_TRANSLATE_NOOP("Exception", "Pad: Creating a face from sketch failed"));
    }
    sketchshape.Move(invObjLoc);

    double Ltotal = L;
    double Loffset = 0.;

    if (method == "TwoLengths") {
        Ltotal += L2;
        if (hasReversed)
            Loffset = -L;
        else
            Loffset = -L2;
    }
    else if (hasReversed) {
        Loffset = -Ltotal / 2;
    }

    TopoDS_Shape from = sketchshape;
    if (method == "TwoLengths" || hasMidplane) {
        gp_Trsf mov;
        mov.SetTranslation(Loffset * gp_Vec(dir));
        TopLoc_Location loc(mov);
        from = sketchshape.Moved(loc);
    }
    else if (hasReversed) {
        Ltotal *= -1.0;
    }

    if (fabs(Ltotal) < Precision::Confusion()) {
        throw Base::ValueError("Cannot create an extrude with a height of zero.");
    }

    BRepPrimAPI_MakePrism PrismMaker(from, Ltotal * gp_Vec(dir), Standard_False, Standard_True); // finite prism
    if (!PrismMaker.IsDone()) {
        throw Base::RuntimeError("ProfileBased: Length: Could not extrude the sketch!");
    }
    TopoDS_Shape prism = PrismMaker.Shape();

    if (prism.IsNull()) {
        throw Base::RuntimeError(QT_TRANSLATE_NOOP("Exception", "Pad: Resulting shape is empty"));
    }
    prism.Move(invObjLoc.Inverted());
    return prism;
}

Base::Vector3d FeatureExtrude::getProfileNormal() const 
{
    Base::Vector3d SketchVector(0, 0, 1);
    auto obj = getVerifiedObject(true);
    if (!obj) {
        return SketchVector;
    }

    // get the Sketch plane
    if (obj->isDerivedFrom<Part::Part2DObject>()) {
        Base::Placement SketchPos = obj->Placement.getValue();
        Base::Rotation SketchOrientation = SketchPos.getRotation();
        SketchOrientation.multVec(SketchVector, SketchVector);
    }

    return SketchVector;
}

Part::Feature* FeatureExtrude::getVerifiedObject(bool silent) const 
{
    App::DocumentObject* result = Profile.getValue();
    const char* err = nullptr;

    if (!result) {
        err = "No object linked";
    }
    else {
        if (!result->isDerivedFrom<Part::Feature>())
            err = "Linked object is not a Sketch, Part2DObject or Feature";
    }

    if (!silent && err) {
        throw Base::RuntimeError(err);
    }

    return static_cast<Part::Feature*>(result);
}

Part::Part2DObject* FeatureExtrude::getVerifiedSketch(bool silent) const
{
    App::DocumentObject* result = Profile.getValue();
    const char* err = nullptr;

    if (!result) {
        err = "No profile linked at all";
    }
    else {
        if (!result->isDerivedFrom<Part::Part2DObject>()) {
            err = "Linked object is not a Sketch or Part2DObject";
            result = nullptr;
        }
    }

    if (!silent && err) {
        throw Base::RuntimeError(err);
    }

    return static_cast<Part::Part2DObject*>(result);
}

TopoDS_Shape FeatureExtrude::getVerifiedFace(bool silent) const 
{
    App::DocumentObject* result = Profile.getValue();
    const char* err = nullptr;
    std::string _err;

    if (!result) {
        err = "No profile linked";
    }
    else {
        if (result->isDerivedFrom<Part::Part2DObject>()) {

            auto wires = getProfileWires();
            return Part::FaceMakerCheese::makeFace(wires);
        }
        else
            err = "Linked object is neither Sketch, Part2DObject or Part::Feature";
    }

    if (!silent && err) {
        throw Base::RuntimeError(err);
    }

    return TopoDS_Face();
}

std::vector<TopoDS_Wire> FeatureExtrude::getProfileWires() const 
{
    std::vector<TopoDS_Wire> result;

    if (!Profile.getValue() || !Profile.getValue()->isDerivedFrom<Part::Feature>())
        throw Base::TypeError("No valid profile linked");

    TopoDS_Shape shape;
    if (Profile.getValue()->isDerivedFrom(Part::Part2DObject::getClassTypeId()))
        shape = Profile.getValue<Part::Part2DObject*>()->Shape.getValue();
    else {
        if (Profile.getSubValues().empty())
            throw Base::ValueError("No valid subelement linked in Part::Feature");

        shape = Profile.getValue<Part::Feature*>()->Shape.getShape().getSubShape(Profile.getSubValues().front().c_str());
    }

    if (shape.IsNull())
        throw Base::ValueError("Linked shape object is empty");

    // this is a workaround for an obscure OCC bug which leads to empty tessellations
    // for some faces. Making an explicit copy of the linked shape seems to fix it.
    // The error almost happens when re-computing the shape but sometimes also for the
    // first time
    BRepBuilderAPI_Copy copy(shape);
    shape = copy.Shape();
    if (shape.IsNull())
        throw Base::ValueError("Linked shape object is empty");

    TopExp_Explorer ex;
    for (ex.Init(shape, TopAbs_WIRE); ex.More(); ex.Next()) {
        result.push_back(TopoDS::Wire(ex.Current()));
    }
    if (result.empty()) // there can be several wires
        throw Base::ValueError("Linked shape object is not a wire");

    return result;
}

}