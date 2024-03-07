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


#ifndef NOMANOR_FeatureExtrude_H
#define NOMANOR_FeatureExtrude_H

#include <App/PropertyUnits.h>
#include <Mod/Part/App/Part2DObject.h>

#include "../NomAnorGlobal.h"
#include "FeatureToolShape.h"

namespace NomAnor
{

class NomAnorExport FeatureExtrude: public NomAnor::FeatureToolShape
{
    PROPERTY_HEADER_WITH_OVERRIDE(NomAnor::FeatureExtrude);

public:
    FeatureExtrude();

    App::PropertyLinkSub Profile;
    App::PropertyBool    Reversed;
    App::PropertyBool    Midplane;

    App::PropertyEnumeration Type;
    App::PropertyLength      Length;
    App::PropertyLength      Length2;
    App::PropertyLength      Offset;

    short mustExecute() const override;

private:
    App::DocumentObjectExecReturn* execute() override;
    TopoDS_Shape getToolShape() const override;

    Base::Vector3d getProfileNormal() const;
    Part::Feature* getVerifiedObject(bool silent = false) const;
    TopoDS_Shape getVerifiedFace(bool silent = false) const;
    std::vector<TopoDS_Wire> getProfileWires() const;
    Part::Part2DObject* getVerifiedSketch(bool silent = false) const;

private:
    static char const* TypeEnums[];
};

}  // namespace NomAnor


#endif  // NOMANOR_FeatureExtrude_H
