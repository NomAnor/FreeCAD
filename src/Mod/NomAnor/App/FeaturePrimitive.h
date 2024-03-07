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


#ifndef NOMANOR_FeaturePrimitive_H
#define NOMANOR_FeaturePrimitive_H

#include <App/PropertyUnits.h>
#include <Mod/Part/App/AttachExtension.h>

#include "../NomAnorGlobal.h"
#include "FeatureToolShape.h"

namespace NomAnor
{

/** PartDesign feature with a primitive shape
 * Base class of all PartDesign features that create primitive shapes
 */
class NomAnorExport FeaturePrimitive: public NomAnor::FeatureToolShape, public Part::AttachExtension
{
    PROPERTY_HEADER_WITH_EXTENSIONS(NomAnor::FeaturePrimitive);

public:
    FeaturePrimitive();

    short mustExecute() const override;

    std::vector<ToolShape> getToolShapes() const override;
};

class NomAnorExport FeatureBox: public NomAnor::FeaturePrimitive
{
    PROPERTY_HEADER_WITH_OVERRIDE(NomAnor::FeatureBox);

public:
    FeatureBox();

    App::PropertyLength Length, Height, Width;

    short mustExecute() const override;

    TopoDS_Shape getToolShape() const override;
};

class NomAnorExport FeatureCylinder: public NomAnor::FeaturePrimitive
{
    PROPERTY_HEADER_WITH_OVERRIDE(NomAnor::FeatureCylinder);

public:
    FeatureCylinder();

    App::PropertyLength Radius, Height;
    App::PropertyAngle Angle;

    short mustExecute() const override;

    TopoDS_Shape getToolShape() const override;
};

}  // namespace NomAnor


#endif  // NOMANOR_FeaturePrimitive_H
