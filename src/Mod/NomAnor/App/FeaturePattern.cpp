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

#include "FeaturePattern.h"

#include "PreCompiled.h"
#ifndef _PreComp_
#endif

#include <gp_Pln.hxx>
#include <gp_Lin.hxx>
#include <gp_Circ.hxx>
#include <TopoDS.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBuilderAPI_Transform.hxx>

#include <Base/Tools.h>
#include <App/OriginFeature.h>
#include <Mod/PartDesign/App/DatumPlane.h>
#include <Mod/PartDesign/App/DatumLine.h>
#include <Mod/Part/App/TopoShape.h>
#include <Mod/Part/App/Part2DObject.h>


FC_LOG_LEVEL_INIT("NomAnor", true, true)


namespace NomAnor
{

PROPERTY_SOURCE(NomAnor::FeaturePattern, NomAnor::FeatureToolShapes)

FeaturePattern::FeaturePattern()
{
    ADD_PROPERTY_TYPE(Features, (nullptr), "Pattern", App::Prop_None, "Reference to the source features");
}

short FeaturePattern::mustExecute() const
{
    if (Features.isTouched()) {
        return 1;
    }
    return NomAnor::FeatureToolShapes::mustExecute();
}

// Recursivly collect all features from a pattern and all sub-patterns.
static void gatherFeatures(FeaturePattern const* pattern, std::vector<App::DocumentObject const*>& features)
{
    for (auto const* feature : pattern->Features.getValues()) {
        features.push_back(feature);
        if (feature->isDerivedFrom<FeaturePattern>()) {
            auto pattern = static_cast<NomAnor::FeaturePattern const*>(feature);
            gatherFeatures(pattern, features);
        }
    }
}

auto FeaturePattern::getToolShapes() const -> std::vector<ToolShape>
{
    auto const features = [&]() {
        std::vector<App::DocumentObject const*> result;
        gatherFeatures(this, result);
        return result;
    }();

    std::vector<ToolShape> result;
    auto addResults = [&](std::vector<ToolShape> shapes) {
        for (auto& shape : applyTransformation(std::move(shapes))) {
            result.push_back(std::move(shape));
        }
    };
    
    if (!features.empty()) {
        for (auto feature : features) {
            if (!feature->isDerivedFrom<NomAnor::FeatureToolShapes>()) {
                throw Base::RuntimeError(QT_TRANSLATE_NOOP("Exception", "Only features with tool shapes can be patterned"));
            }
            addResults(static_cast<NomAnor::FeatureToolShapes const*>(feature)->getToolShapes());
        }
    } else {
        // Without features we use the base shape itself
        addResults({ToolShape{Operation::Fuse, getBaseShape()}});
    }

    return result;
}


PROPERTY_SOURCE(NomAnor::FeatureMirrored, NomAnor::FeaturePattern)

FeatureMirrored::FeatureMirrored()
{
    ADD_PROPERTY_TYPE(MirrorPlane, (nullptr), "Mirrored", App::Prop_None, "Mirror plane");
}

short FeatureMirrored::mustExecute() const
{
    if (MirrorPlane.isTouched()) {
        return 1;
    }
    return NomAnor::FeaturePattern::mustExecute();
}

gp_Trsf FeatureMirrored::calculateTransformation() const
{
    App::DocumentObject* refObject = MirrorPlane.getValue();
    if (!refObject) {
        throw Base::ValueError("No mirror plane reference specified");
    }
    std::vector<std::string> subStrings = MirrorPlane.getSubValues();

    gp_Pnt axbase;
    gp_Dir axdir;
    if (refObject->isDerivedFrom<Part::Part2DObject>()) {
        if (subStrings.empty()) {
            throw Base::ValueError("No mirror plane reference specified");
        }

        Part::Part2DObject* refSketch = static_cast<Part::Part2DObject*>(refObject);
        Base::Axis axis;
        if (subStrings[0] == "H_Axis") {
            axis = refSketch->getAxis(Part::Part2DObject::V_Axis);
        }
        else if (subStrings[0] == "V_Axis") {
            axis = refSketch->getAxis(Part::Part2DObject::H_Axis);
        }
        else if (subStrings[0].empty()) {
            axis = refSketch->getAxis(Part::Part2DObject::N_Axis);
        }
        else if (subStrings[0].compare(0, 4, "Axis") == 0) {
            int AxId = std::atoi(subStrings[0].substr(4, 4000).c_str());
            if (AxId >= 0 && AxId < refSketch->getAxisCount()) {
                axis = refSketch->getAxis(AxId);
                axis.setBase(axis.getBase() + 0.5 * axis.getDirection());
                axis.setDirection(Base::Vector3d(-axis.getDirection().y,
                                                 axis.getDirection().x,
                                                 axis.getDirection().z));
            }
        }
        axis *= refSketch->Placement.getValue();
        axbase = gp_Pnt(axis.getBase().x, axis.getBase().y, axis.getBase().z);
        axdir = gp_Dir(axis.getDirection().x, axis.getDirection().y, axis.getDirection().z);
    }
    else if (refObject->isDerivedFrom<PartDesign::Plane>()) {
        PartDesign::Plane const* plane = static_cast<PartDesign::Plane const*>(refObject);
        Base::Vector3d base = plane->getBasePoint();
        axbase = gp_Pnt(base.x, base.y, base.z);
        Base::Vector3d dir = plane->getNormal();
        axdir = gp_Dir(dir.x, dir.y, dir.z);
    }
    else if (refObject->isDerivedFrom<App::Plane>()) {
        App::Plane const* plane = static_cast<App::Plane const*>(refObject);
        Base::Vector3d base = plane->Placement.getValue().getPosition();
        axbase = gp_Pnt(base.x, base.y, base.z);
        Base::Rotation rot = plane->Placement.getValue().getRotation();
        Base::Vector3d dir(0, 0, 1);
        rot.multVec(dir, dir);
        axdir = gp_Dir(dir.x, dir.y, dir.z);
    }
    else if (refObject->isDerivedFrom<Part::Feature>()) {
        if (subStrings.empty()) {
            throw Base::ValueError("No mirror plane reference specified");
        }
        if (subStrings[0].empty()) {
            throw Base::ValueError("No direction reference specified");
        }
        Part::TopoShape baseShape = static_cast<Part::Feature const*>(refObject)->Shape.getShape();
        // TODO: Check for multiple mirror planes?
        TopoDS_Shape shape = baseShape.getSubShape(subStrings[0].c_str());
        TopoDS_Face face = TopoDS::Face(shape);
        if (face.IsNull()) {
            throw Base::ValueError("Failed to extract mirror plane");
        }
        BRepAdaptor_Surface adapt(face);
        if (adapt.GetType() != GeomAbs_Plane) {
            throw Base::TypeError("Mirror face must be planar");
        }

        axbase = getPointFromFace(face);
        axdir = adapt.Plane().Axis().Direction();
    }
    else {
        throw Base::ValueError(
            "Mirror plane reference must be a sketch axis, a face of a feature or a datum plane");
    }

    TopLoc_Location invObjLoc = this->getLocation().Inverted();
    axbase.Transform(invObjLoc.Transformation());
    axdir.Transform(invObjLoc.Transformation());

    gp_Ax2 mirrorAxis(axbase, axdir);

    gp_Trsf trans;
    trans.SetMirror(mirrorAxis);
    return trans;
}

auto FeatureMirrored::applyTransformation(std::vector<ToolShape> tools) const -> std::vector<ToolShape>
{
    BRepBuilderAPI_Transform mkMirror(calculateTransformation());

    // Replace each shape with its mirrored counterpart
    for (auto& tool : tools) {
        mkMirror.Perform(tool.shape);
        if (!mkMirror.IsDone()) {
            throw Base::CADKernelError(QT_TRANSLATE_NOOP("Exception", "Transformation failed"));
        }
        tool.shape = mkMirror;
    }

    return tools;
}


PROPERTY_SOURCE(NomAnor::FeatureLinearPattern, NomAnor::FeaturePattern)

static App::PropertyIntegerConstraint::Constraints const intOccurrences = {1, INT_MAX, 1};
static char const* LinearModeEnums[] = {"Length", "Offset", nullptr};

FeatureLinearPattern::FeatureLinearPattern()
{
    auto initial_mode = PatternMode::Length;
    ADD_PROPERTY_TYPE(Mode, (static_cast<int>(initial_mode)), "LinearPattern", App::Prop_None, "Mode");
    Mode.setEnums(LinearModeEnums);
    ADD_PROPERTY_TYPE(Direction, (nullptr), "LinearPattern", App::Prop_None, "Direction");
    ADD_PROPERTY_TYPE(Reversed, (0), "LinearPattern", App::Prop_None, "Reversed");
    ADD_PROPERTY_TYPE(Length, (100.0), "LinearPattern", App::Prop_None, "Length");
    ADD_PROPERTY_TYPE(Offset, (10.0), "LinearPattern", App::Prop_None, "Offset");
    ADD_PROPERTY_TYPE(Occurrences, (3), "LinearPattern", App::Prop_None, "Occurrences");
    Occurrences.setConstraints(&intOccurrences);
    
    setReadWriteStatusForMode(initial_mode);
}

short FeatureLinearPattern::mustExecute() const
{
    if (Direction.isTouched() || Reversed.isTouched() || Mode.isTouched() ||
        // Length and Offset are mutually exclusive, only one could be updated at once
        Length.isTouched() || Offset.isTouched() || Occurrences.isTouched()) {
        return 1;
    }
    return NomAnor::FeaturePattern::mustExecute();
}

App::DocumentObjectExecReturn* FeatureLinearPattern::execute()
{
    auto mode = static_cast<PatternMode>(Mode.getValue());
    setReadWriteStatusForMode(mode);

    return NomAnor::FeaturePattern::execute();
}

void FeatureLinearPattern::setReadWriteStatusForMode(PatternMode mode)
{
    Length.setReadOnly(mode != PatternMode::Length);
    Offset.setReadOnly(mode != PatternMode::Offset);
}

std::vector<gp_Trsf> FeatureLinearPattern::calculateTransformations() const
{
    int const occurrences = Occurrences.getValue();

    if (occurrences < 1) {
        throw Base::ValueError("At least one occurrence required");
    }

    if (occurrences == 1) {
        return {};
    }

    double const distance = Length.getValue();
    if (distance < Precision::Confusion()) {
        throw Base::ValueError("Pattern length too small");
    }
    bool const reversed = Reversed.getValue();

    App::DocumentObject const* refObject = Direction.getValue();
    if (!refObject) {
        throw Base::ValueError("No direction reference specified");
    }

    std::vector<std::string> const subStrings = Direction.getSubValues();

    gp_Dir dir;
    if (refObject->isDerivedFrom<Part::Part2DObject>()) {
        if (subStrings.empty()) {
            throw Base::ValueError("No direction reference specified");
        }
        Part::Part2DObject const* refSketch = static_cast<Part::Part2DObject const*>(refObject);
        Base::Axis axis;
        if (subStrings[0] == "H_Axis") {
            axis = refSketch->getAxis(Part::Part2DObject::H_Axis);
            axis *= refSketch->Placement.getValue();
        }
        else if (subStrings[0] == "V_Axis") {
            axis = refSketch->getAxis(Part::Part2DObject::V_Axis);
            axis *= refSketch->Placement.getValue();
        }
        else if (subStrings[0] == "N_Axis") {
            axis = refSketch->getAxis(Part::Part2DObject::N_Axis);
            axis *= refSketch->Placement.getValue();
        }
        else if (subStrings[0].compare(0, 4, "Axis") == 0) {
            int AxId = std::atoi(subStrings[0].substr(4, 4000).c_str());
            if (AxId >= 0 && AxId < refSketch->getAxisCount()) {
                axis = refSketch->getAxis(AxId);
                axis *= refSketch->Placement.getValue();
            }
        }
        else if (subStrings[0].compare(0, 4, "Edge") == 0) {
            Part::TopoShape refShape = refSketch->Shape.getShape();
            TopoDS_Shape ref = refShape.getSubShape(subStrings[0].c_str());
            TopoDS_Edge refEdge = TopoDS::Edge(ref);
            if (refEdge.IsNull()) {
                throw Base::ValueError("Failed to extract direction edge");
            }
            BRepAdaptor_Curve adapt(refEdge);
            if (adapt.GetType() != GeomAbs_Line) {
                throw Base::TypeError("Direction edge must be a straight line");
            }

            gp_Pnt p = adapt.Line().Location();
            gp_Dir d = adapt.Line().Direction();

            // the axis is not given in local coordinates and mustn't be multiplied with the
            // placement
            axis.setBase(Base::Vector3d(p.X(), p.Y(), p.Z()));
            axis.setDirection(Base::Vector3d(d.X(), d.Y(), d.Z()));
        }
        dir = gp_Dir(axis.getDirection().x, axis.getDirection().y, axis.getDirection().z);
    }
    else if (refObject->isDerivedFrom<PartDesign::Plane>()) {
        PartDesign::Plane const* plane = static_cast<PartDesign::Plane const*>(refObject);
        Base::Vector3d d = plane->getNormal();
        dir = gp_Dir(d.x, d.y, d.z);
    }
    else if (refObject->isDerivedFrom<PartDesign::Line>()) {
        PartDesign::Line const* line = static_cast<PartDesign::Line const*>(refObject);
        Base::Vector3d d = line->getDirection();
        dir = gp_Dir(d.x, d.y, d.z);
    }
    else if (refObject->isDerivedFrom<App::Line>()) {
        App::Line const* line = static_cast<App::Line const*>(refObject);
        Base::Rotation rot = line->Placement.getValue().getRotation();
        Base::Vector3d d(1, 0, 0);
        rot.multVec(d, d);
        dir = gp_Dir(d.x, d.y, d.z);
    }
    else if (refObject->isDerivedFrom<Part::Feature>()) {
        if (subStrings.empty()) {
            throw Base::ValueError("No direction reference specified");
        }
        if (subStrings[0].empty()) {
            throw Base::ValueError("No direction reference specified");
        }
        Part::Feature const* refFeature = static_cast<Part::Feature const*>(refObject);
        Part::TopoShape refShape = refFeature->Shape.getShape();
        TopoDS_Shape ref = refShape.getSubShape(subStrings[0].c_str());

        if (ref.ShapeType() == TopAbs_FACE) {
            TopoDS_Face refFace = TopoDS::Face(ref);
            if (refFace.IsNull()) {
                throw Base::ValueError("Failed to extract direction plane");
            }
            BRepAdaptor_Surface adapt(refFace);
            if (adapt.GetType() != GeomAbs_Plane) {
                throw Base::TypeError("Direction face must be planar");
            }

            dir = adapt.Plane().Axis().Direction();
        }
        else if (ref.ShapeType() == TopAbs_EDGE) {
            TopoDS_Edge refEdge = TopoDS::Edge(ref);
            if (refEdge.IsNull()) {
                throw Base::ValueError("Failed to extract direction edge");
            }
            BRepAdaptor_Curve adapt(refEdge);
            if (adapt.GetType() != GeomAbs_Line) {
                throw Base::ValueError("Direction edge must be a straight line");
            }

            dir = adapt.Line().Direction();
        }
        else {
            throw Base::ValueError("Direction reference must be edge or face");
        }
    }
    else {
        throw Base::ValueError(
            "Direction reference must be edge/face of a feature or a datum line/plane");
    }

    gp_Vec offset(dir.X(), dir.Y(), dir.Z());

    switch (static_cast<PatternMode>(Mode.getValue())) {
        case PatternMode::Length:
            offset *= distance / (occurrences - 1);
            break;

        case PatternMode::Offset:
            offset *= Offset.getValue();
            break;

        default:
            throw Base::ValueError("Invalid mode");
    }

    if (reversed) {
        offset.Reverse();
    }

    std::vector<gp_Trsf> transformations;
    gp_Trsf trans;

    // Note: The original shape is already included in the list of shapes!
    // Therefore we  need no transformation for it and start with occurrence number 1
    for (int i = 1; i < occurrences; i++) {
        trans.SetTranslation(offset * i);
        transformations.push_back(trans);
    }

    return transformations;
}

auto FeatureLinearPattern::applyTransformation(std::vector<ToolShape> tools) const -> std::vector<ToolShape>
{
    std::vector<ToolShape> result;

    for (auto const& transformation : calculateTransformations()) {
        for (auto const& tool : tools) {
            result.push_back(ToolShape{tool.operation, tool.shape.Moved(transformation)});
        }
    }

    return result;
}


PROPERTY_SOURCE(NomAnor::FeaturePolarPattern, NomAnor::FeaturePattern)

static App::PropertyAngle::Constraints const floatAngle = {Base::toDegrees<double>(Precision::Angular()), 360.0, 1.0};
static char const* PatternModeEnums[] = {"Length", "Offset", nullptr};

FeaturePolarPattern::FeaturePolarPattern()
{
    auto initial_mode = PatternMode::Angle;
    ADD_PROPERTY_TYPE(Mode, (static_cast<int>(initial_mode)), "PolarPattern", App::Prop_None, "Mode");
    Mode.setEnums(PatternModeEnums);
    ADD_PROPERTY_TYPE(Axis, (nullptr), "PolarPattern", App::Prop_None, "Axis");
    ADD_PROPERTY_TYPE(Reversed, (0), "PolarPattern", App::Prop_None, "Reversed");
    ADD_PROPERTY_TYPE(Angle, (360.0), "PolarPattern", App::Prop_None, "Angle");
    Angle.setConstraints(&floatAngle);
    ADD_PROPERTY_TYPE(Offset, (120.0), "PolarPattern", App::Prop_None, "Offset");
    ADD_PROPERTY_TYPE(Occurrences, (3), "PolarPattern", App::Prop_None, "Occurrences");
    Occurrences.setConstraints(&intOccurrences);
    
    setReadWriteStatusForMode(initial_mode);
}

short FeaturePolarPattern::mustExecute() const
{
    if (Axis.isTouched() || Reversed.isTouched() || Mode.isTouched() ||
        // Length and Offset are mutually exclusive, only one could be updated at once
        Angle.isTouched() || Offset.isTouched() || Occurrences.isTouched()) {
        return 1;
    }
    return NomAnor::FeaturePattern::mustExecute();
}

App::DocumentObjectExecReturn* FeaturePolarPattern::execute()
{
    auto mode = static_cast<PatternMode>(Mode.getValue());
    setReadWriteStatusForMode(mode);

    return NomAnor::FeaturePattern::execute();
}

void FeaturePolarPattern::setReadWriteStatusForMode(PatternMode mode)
{
    Angle.setReadOnly(mode != PatternMode::Angle);
    Offset.setReadOnly(mode != PatternMode::Offset);
}

std::vector<gp_Trsf> FeaturePolarPattern::calculateTransformations() const
{
    int const occurrences = Occurrences.getValue();

    if (occurrences < 1) {
        throw Base::ValueError("At least one occurrence required");
    }

    if (occurrences == 1) {
        return {};
    }

    bool const reversed = Reversed.getValue();

    App::DocumentObject const* refObject = Axis.getValue();
    if (!refObject) {
        throw Base::ValueError("No axis reference specified");
    }
    std::vector<std::string> subStrings = Axis.getSubValues();

    gp_Pnt axbase;
    gp_Dir axdir;
    if (refObject->isDerivedFrom<Part::Part2DObject>()) {
        if (subStrings.empty()) {
            throw Base::ValueError("No axis reference specified");
        }
        Part::Part2DObject const* refSketch = static_cast<Part::Part2DObject const*>(refObject);
        Base::Axis axis;
        if (subStrings[0] == "H_Axis") {
            axis = refSketch->getAxis(Part::Part2DObject::H_Axis);
        }
        else if (subStrings[0] == "V_Axis") {
            axis = refSketch->getAxis(Part::Part2DObject::V_Axis);
        }
        else if (subStrings[0] == "N_Axis") {
            axis = refSketch->getAxis(Part::Part2DObject::N_Axis);
        }
        else if (subStrings[0].compare(0, 4, "Axis") == 0) {
            int AxId = std::atoi(subStrings[0].substr(4, 4000).c_str());
            if (AxId >= 0 && AxId < refSketch->getAxisCount()) {
                axis = refSketch->getAxis(AxId);
            }
        }
        axis *= refSketch->Placement.getValue();
        axbase = gp_Pnt(axis.getBase().x, axis.getBase().y, axis.getBase().z);
        axdir = gp_Dir(axis.getDirection().x, axis.getDirection().y, axis.getDirection().z);
    }
    else if (refObject->isDerivedFrom<PartDesign::Line>()) {
        PartDesign::Line const* line = static_cast<PartDesign::Line const*>(refObject);
        Base::Vector3d base = line->getBasePoint();
        axbase = gp_Pnt(base.x, base.y, base.z);
        Base::Vector3d dir = line->getDirection();
        axdir = gp_Dir(dir.x, dir.y, dir.z);
    }
    else if (refObject->isDerivedFrom<App::Line>()) {
        App::Line const* line = static_cast<App::Line const*>(refObject);
        Base::Rotation rot = line->Placement.getValue().getRotation();
        Base::Vector3d d(1, 0, 0);
        rot.multVec(d, d);
        axdir = gp_Dir(d.x, d.y, d.z);
    }
    else if (refObject->isDerivedFrom<Part::Feature>()) {
        if (subStrings.empty()) {
            throw Base::ValueError("No axis reference specified");
        }
        if (subStrings[0].empty()) {
            throw Base::ValueError("No axis reference specified");
        }
        Part::Feature const* refFeature = static_cast<Part::Feature const*>(refObject);
        Part::TopoShape refShape = refFeature->Shape.getShape();
        TopoDS_Shape ref = refShape.getSubShape(subStrings[0].c_str());

        if (ref.ShapeType() == TopAbs_EDGE) {
            TopoDS_Edge refEdge = TopoDS::Edge(ref);
            if (refEdge.IsNull()) {
                throw Base::ValueError("Failed to extract axis edge");
            }
            BRepAdaptor_Curve adapt(refEdge);
            if (adapt.GetType() == GeomAbs_Line) {
                axbase = adapt.Line().Location();
                axdir = adapt.Line().Direction();
            }
            else if (adapt.GetType() == GeomAbs_Circle) {
                axbase = adapt.Circle().Location();
                axdir = adapt.Circle().Axis().Direction();
            }
            else {
                throw Base::TypeError(
                    "Rotation edge must be a straight line, circle or arc of circle");
            }
        }
        else {
            throw Base::TypeError("Axis reference must be an edge");
        }
    }
    else {
        throw Base::TypeError("Axis reference must be edge of a feature or datum line");
    }

    gp_Ax2 axis(axbase, axdir);

    if (reversed) {
        axis.SetDirection(axis.Direction().Reversed());
    }

    double angle;

    switch (static_cast<PatternMode>(Mode.getValue())) {
        case PatternMode::Angle:
            angle = Angle.getValue();

            if (std::fabs(angle - 360.0) < Precision::Confusion()) {
                angle /= occurrences;  // Because e.g. two occurrences in 360 degrees need to be 180
                                       // degrees apart
            }
            else {
                angle /= occurrences - 1;
            }

            break;

        case PatternMode::Offset:
            angle = Offset.getValue();
            break;

        default:
            throw Base::ValueError("Invalid mode");
    }

    double const offset = Base::toRadians<double>(angle);

    if (offset < Precision::Angular()) {
        throw Base::ValueError("Pattern angle too small");
    }

    std::vector<gp_Trsf> transformations;
    gp_Trsf trans;

    // Note: The original shape is already included in the list of shapes!
    // Therefore we  need no transformation for it and start with occurrence number 1
    for (int i = 1; i < occurrences; i++) {
        trans.SetRotation(axis.Axis(), i * offset);
        transformations.push_back(trans);
    }

    return transformations;
}

auto FeaturePolarPattern::applyTransformation(std::vector<ToolShape> tools) const -> std::vector<ToolShape>
{
    std::vector<ToolShape> result;

    for (auto const& transformation : calculateTransformations()) {
        for (auto const& tool : tools) {
            result.push_back(ToolShape{tool.operation, tool.shape.Moved(transformation)});
        }
    }

    return result;
}

}
