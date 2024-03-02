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

#include "FeatureToolShapes.h"

#include "PreCompiled.h"
#ifndef _PreComp_
#endif

#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepBuilderAPI_Transform.hxx>

#include <App/Application.h>
#include <Mod/Part/App/modelRefine.h>

FC_LOG_LEVEL_INIT("NomANor", true, true)


namespace NomAnor
{

PROPERTY_SOURCE(NomAnor::FeatureToolShapes, PartDesign::Feature)

FeatureToolShapes::FeatureToolShapes()
{
    ADD_PROPERTY_TYPE(Refine,
                      (0),
                      "Part Design",
                      (App::PropertyType)(App::Prop_None),
                      "Refine shape (clean up redundant edges) after adding/subtracting");

    // Initialize Refine property from preference
    Base::Reference<ParameterGrp> hGrp = App::GetApplication()
                                             .GetUserParameter()
                                             .GetGroup("BaseApp")
                                             ->GetGroup("Preferences")
                                             ->GetGroup("Mod/PartDesign");
    Refine.setValue(hGrp->GetBool("RefineModel", false));
}

short FeatureToolShapes::mustExecute() const
{
    if (Refine.isTouched()) {
        return 1;
    }
    return PartDesign::Feature::mustExecute();
}

App::DocumentObjectExecReturn* FeatureToolShapes::execute()
{
    if (!getFeatureBody()) {
        return new App::DocumentObjectExecReturn(QT_TRANSLATE_NOOP("Exception", "Cannot do boolean on feature which is not in a body"));
    }

    TopoDS_Shape result;
    try {
        result = getBaseShape();
    } catch (Base::RuntimeError&) {
        // Ignore, we are the first feature in the Body
    }

    auto const tools = getToolShapes();
    auto toolsIter = tools.begin();
    auto const toolsEnd = tools.end();

    // Use the first tool shape as base shape if it has a fuse operation
    if (result.IsNull()) {
        if (toolsIter == toolsEnd) {
            return new App::DocumentObjectExecReturn(QT_TRANSLATE_NOOP("Exception", "No tool shape to derive base shape"));
        }

        if (toolsIter->operation != Operation::Fuse) {
            return new App::DocumentObjectExecReturn(QT_TRANSLATE_NOOP("Exception", "Base shape cannot be derived from non fuse tool shape"));
        }
        result = toolsIter->shape;
        ++toolsIter;
    }

    gp_Trsf const baseToBodyTrsf = result.Location();
    gp_Trsf const bodyToBaseTrsf = baseToBodyTrsf.Inverted();
    // All operations are done in the LCS of the base shape
    result.Location(gp_Trsf());

#if OCC_VERSION_MAJOR > 7 || (OCC_VERSION_MAJOR == 7 && OCC_VERSION_MINOR >= 4)
    BRepAlgoAPI_BooleanOperation mkOp;
#else
    // These just set the operation in their constructor, we will use it like a base BRepAlgoAPI_BooleanOperation 
    // because the constructor is protected in this OCC version.
    BRepAlgoAPI_Common mkOp;
#endif  

    // Apply the tool shapes with the given operations to the base shape
    while (toolsIter != toolsEnd) {
        auto const operation = toolsIter->operation;
        switch (operation) {
        case Operation::Fuse:
            mkOp.SetOperation(BOPAlgo_FUSE);
            break;
        case Operation::Cut:
            mkOp.SetOperation(BOPAlgo_CUT);
            break;
        case Operation::Common:
            mkOp.SetOperation(BOPAlgo_COMMON);
            break;
        };     

        {
            // Accumulate all consecutive tools with the same operation
            TopTools_ListOfShape opTools;
            while (toolsIter != toolsEnd && toolsIter->operation == operation) {
                // Tool shapes are located in the Body LCS, transform them to the base shape LCS          
                auto shape = toolsIter->shape.Moved(bodyToBaseTrsf);
                opTools.Append(shape);
                ++toolsIter;
            }
            mkOp.SetTools(opTools); 
        }   
        {
            TopTools_ListOfShape args;
            args.Append(result);
            mkOp.SetArguments(args);
        }        

        mkOp.Build();
        if (!mkOp.IsDone()) {
            return new App::DocumentObjectExecReturn(QT_TRANSLATE_NOOP("Exception", "Boolean operation failed"));
        }

        auto resultShape = mkOp.Shape();
        if (resultShape.IsNull()) {
            return new App::DocumentObjectExecReturn(QT_TRANSLATE_NOOP("Exception", "Resulting shape is not a solid"));
        }

        result = std::move(resultShape);
    }

    if (Refine.getValue()) {
        try {
            Part::BRepBuilderAPI_RefineModel mkRefine(result);
            result = mkRefine.Shape();
        }
        catch (Standard_Failure&) {
            // ignore and use unrefined result
        }
    }

    if (countSolids(result) > 1) {
        return new App::DocumentObjectExecReturn(QT_TRANSLATE_NOOP("Exception", "Result has multiple solids: that is not currently supported."));
    }

    // Transform the resulting geometry in such a way that it remains at the same position (relative to the Body) as the base shape.
    // First apply the original base shape transformation and then negate our own placement.
    {
        BRepBuilderAPI_Transform mkTrsf(getLocation().Inverted() * baseToBodyTrsf);
        mkTrsf.Perform(result, true); // set copy=true to transform the geometry and not only the shape location
        if (!mkTrsf.IsDone()) {
            return new App::DocumentObjectExecReturn(QT_TRANSLATE_NOOP("Exception", "Transform operation failed"));
        }

        result = mkTrsf.Shape();
        if (result.IsNull()) {
            return new App::DocumentObjectExecReturn(QT_TRANSLATE_NOOP("Exception", "Resulting shape is not a solid"));
        }
    }

    Shape.setValue(result);
    return App::DocumentObject::StdReturn;
}

}  // namespace NomAnor
