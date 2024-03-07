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

#include "FeaturePrimitive.h"

#include "PreCompiled.h"
#ifndef _PreComp_
#endif

#include <Base/Tools.h>

#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>


FC_LOG_LEVEL_INIT("NomAnor", true, true)


namespace NomAnor
{

// it turned out that OCC cannot e.g. create a box with a width of Precision::Confusion()
// with two times Precision::Confusion() all geometric primitives can be created
App::PropertyQuantityConstraint::Constraints const quantityRange = { 2 * Precision::Confusion(), FLT_MAX, 0.1 };
const App::PropertyQuantityConstraint::Constraints angleRangeU = { 0.0, 360.0, 1.0 };

PROPERTY_SOURCE_ABSTRACT_WITH_EXTENSIONS(NomAnor::FeaturePrimitive, NomAnor::FeatureToolShape)

FeaturePrimitive::FeaturePrimitive()
{
    Placement.setStatus(App::Property::Hidden, false);

    Part::AttachExtension::initExtension(this);
}

short FeaturePrimitive::mustExecute() const
{
    if (Placement.isTouched()) {
        return 1;
    }
    return NomAnor::FeatureToolShape::mustExecute();
}

auto FeaturePrimitive::getToolShapes() const -> std::vector<ToolShape>
{
    TopoDS_Shape shape = getToolShape();
    shape.Move(getLocation());
    
    return {ToolShape{getOperation(), shape}};
}


PROPERTY_SOURCE(NomAnor::FeatureBox, NomAnor::FeaturePrimitive)

FeatureBox::FeatureBox()
{
    ADD_PROPERTY_TYPE(Length,(10.0f),"Box",App::Prop_None,"The length of the box");
    ADD_PROPERTY_TYPE(Width ,(10.0f),"Box",App::Prop_None,"The width of the box");
    ADD_PROPERTY_TYPE(Height,(10.0f),"Box",App::Prop_None,"The height of the box");
    Length.setConstraints(&quantityRange);
    Width.setConstraints(&quantityRange);
    Height.setConstraints(&quantityRange);
}

short FeatureBox::mustExecute() const
{
    if (Length.isTouched() || Height.isTouched() || Width.isTouched()) {
        return 1;
    }
    return NomAnor::FeaturePrimitive::mustExecute();
}

TopoDS_Shape FeatureBox::getToolShape() const
{
    double const L = Length.getValue();
    double const W = Width.getValue();
    double const H = Height.getValue();

    if (L < Precision::Confusion()) {
        throw Base::ValueError(QT_TRANSLATE_NOOP("Exception", "Length of box too small"));
    }
    if (W < Precision::Confusion()) {
        throw Base::ValueError(QT_TRANSLATE_NOOP("Exception", "Width of box too small"));
    }
    if (H < Precision::Confusion()) {
        throw Base::ValueError(QT_TRANSLATE_NOOP("Exception", "Height of box too small"));
    }

    // Build a box using the dimension attributes
    BRepPrimAPI_MakeBox mkBox(L, W, H);
    return mkBox.Shape();
}



PROPERTY_SOURCE(NomAnor::FeatureCylinder, NomAnor::FeaturePrimitive)

FeatureCylinder::FeatureCylinder()
{
    ADD_PROPERTY_TYPE(Radius,(10.0f),"Cylinder",App::Prop_None,"The radius of the cylinder");
    ADD_PROPERTY_TYPE(Height,(10.0f),"Cylinder",App::Prop_None,"The height of the cylinder");
    ADD_PROPERTY_TYPE(Angle,(360.0f),"Cylinder",App::Prop_None,"The closing angle of the cylinder ");
    Radius.setConstraints(&quantityRange);
    Height.setConstraints(&quantityRange);
    Angle.setConstraints(&angleRangeU);
}

short FeatureCylinder::mustExecute() const
{
    if (Radius.isTouched() || Height.isTouched()) {
        return 1;
    }
    return NomAnor::FeaturePrimitive::mustExecute();
}

TopoDS_Shape FeatureCylinder::getToolShape() const
{
    double const R = Radius.getValue();
    double const H = Height.getValue();
    double const A = Angle.getValue();

    if (R < Precision::Confusion()) {
        throw Base::ValueError(QT_TRANSLATE_NOOP("Exception", "Radius of cylinder too small"));
    }
    if (H < Precision::Confusion()) {
        throw Base::ValueError(QT_TRANSLATE_NOOP("Exception", "Height of cylinder too small"));
    }
    if (A < Precision::Confusion())
        throw Base::ValueError(QT_TRANSLATE_NOOP("Exception", "Rotation angle of cylinder too small"));

    BRepPrimAPI_MakeCylinder mkCylinder(R, H, Base::toRadians<double>(A));
    return mkCylinder.Shape();
}

}  // namespace NomAnor
