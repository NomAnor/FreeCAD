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


#ifndef NOMANOR_FeatureToolShape_H
#define NOMANOR_FeatureToolShape_H

#include <vector>

#include <App/PropertyStandard.h>

#include "../NomAnorGlobal.h"
#include "FeatureToolShapes.h"

namespace NomAnor
{

/** PartDesign feature based on tool shapes
 * Base class of all PartDesign features that use boolean operations with tools shapes.
 */
class NomAnorExport FeatureToolShape: public NomAnor::FeatureToolShapes
{
    PROPERTY_HEADER_WITH_OVERRIDE(NomAnor::FeatureToolShape);

public:
    FeatureToolShape();

    App::PropertyEnumeration Operation;

    short mustExecute() const override;

    std::vector<ToolShape> getToolShapes() const override;

protected:
    FeatureToolShapes::Operation getOperation() const;

    virtual TopoDS_Shape getToolShape() const = 0;

private:
    static char const* OperationEnums[];
};

}  // namespace NomAnor


#endif  // NOMANOR_FeatureToolShape_H
