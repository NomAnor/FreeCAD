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


#ifndef NOMANOR_FeatureToolShapes_H
#define NOMANOR_FeatureToolShapes_H

#include <vector>

#include <App/PropertyStandard.h>
#include <Mod/PartDesign/App/Feature.h>

#include "../NomAnorGlobal.h"

namespace NomAnor
{

/** PartDesign feature based on tool shapes
 * Base class of all PartDesign features that use boolean operations with tools shapes.
 */
class NomAnorExport FeatureToolShapes: public PartDesign::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(NomAnor::FeatureToolShapes);

public:
    FeatureToolShapes();

    App::PropertyBool Refine;

    short mustExecute() const override;

    enum class Operation {
        Fuse,
        Cut,
        Common
    };

    struct ToolShape {
        Operation operation;
        TopoDS_Shape shape;
    };

    virtual std::vector<ToolShape> getToolShapes() const
    {
        return {};
    }

protected:
    App::DocumentObjectExecReturn* execute() override;
};

}  // namespace NomAnor


#endif  // NOMANOR_FeatureToolShapes_H
