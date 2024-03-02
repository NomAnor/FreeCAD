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

#include "FeatureBoolean.h"

#include "PreCompiled.h"
#ifndef _PreComp_
#endif

#include <Mod/PartDesign/App/Body.h>


FC_LOG_LEVEL_INIT("NomAnor", true, true)


namespace NomAnor
{

PROPERTY_SOURCE_WITH_EXTENSIONS(NomAnor::FeatureBoolean, NomAnor::FeatureToolShape)

FeatureBoolean::FeatureBoolean()
{
    Placement.setStatus(App::Property::Status::Hidden, false);
    
    ADD_PROPERTY_TYPE(Tool, (nullptr), "Boolean", App::Prop_None, "Reference to tool");
    Tool.setScope(App::LinkScope::Global);
    ADD_PROPERTY_TYPE(UseToolPacement, (false), "Boolean", App::Prop_None, "Use the tool placement instead of the feature placement");

    Part::AttachExtension::initExtension(this);
}

short FeatureBoolean::mustExecute() const
{
    if (Placement.isTouched() || Tool.isTouched() || UseToolPacement.isTouched()) {
        return 1;
    }
    return NomAnor::FeatureToolShape::mustExecute();
}

TopoDS_Shape FeatureBoolean::getToolShape() const
{
    App::DocumentObject const* const tool = Tool.getValue();
    if (!tool) {
        return {};
    }
    if (!tool->isDerivedFrom<Part::Feature>()) {
        throw Base::RuntimeError(QT_TRANSLATE_NOOP("Exception", "Cannot do boolean with anything but Part::Feature and its derivatives"));
    }

    auto feature = static_cast<Part::Feature const*>(tool);
    TopoDS_Shape shape = feature->Shape.getShape().getShape();

    TopLoc_Location location;
    if (UseToolPacement.getValue()) {
        PartDesign::Body const* const body = getFeatureBody();
        if (!body) {
            throw Base::RuntimeError(QT_TRANSLATE_NOOP("Exception", "Boolean feature has no body"));
        }
        
        auto const featurePlacement = feature->globalPlacement();        
        auto const bodyPlacement = body->globalPlacement();
        auto const matrix = (bodyPlacement.inverse() * featurePlacement).toMatrix();

        gp_Trsf trsf;
        Part::TopoShape::convertTogpTrsf(matrix, trsf);  
        location = trsf;      
    }
    else {
        location = getLocation();
    }
    shape.Location(location);

    return shape;
}

}
