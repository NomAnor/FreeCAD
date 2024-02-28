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


#ifndef NOMANOR_Feature_H
#define NOMANOR_Feature_H

#include <Mod/NomAnor/NomAnorGlobal.h>
#include <App/PropertyStandard.h>
#include <Mod/PartDesign/App/Feature.h>

namespace NomAnor
{

/** PartDesign feature
 *   Base class of all PartDesign features.
 *   This kind of features only produce solids or fail.
 */
class NomAnorExport Feature: public PartDesign::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(NomAnor::Feature);

public:
    Feature();

    App::PropertyBool Refine;

    short mustExecute() const override;
};

}  // namespace NomAnor


#endif  // NOMANOR_Feature_H
