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


#ifndef NOMANOR_FeatureBoolean_H
#define NOMANOR_FeatureBoolean_H

#include <App/PropertyStandard.h>
#include <Mod/Part/App/AttachExtension.h>

#include "../NomAnorGlobal.h"
#include "FeatureToolShape.h"

namespace NomAnor
{

class NomAnorExport FeatureBoolean: public NomAnor::FeatureToolShape, public Part::AttachExtension
{
    PROPERTY_HEADER_WITH_EXTENSIONS(NomAnor::FeatureBoolean);

public:
    FeatureBoolean();

    App::PropertyLink Tool;
    App::PropertyBool UseToolPacement;

    short mustExecute() const override;

private:
    TopoDS_Shape getToolShape() const override;
};

}  // namespace NomAnor


#endif  // NOMANOR_FeatureBoolean_H
