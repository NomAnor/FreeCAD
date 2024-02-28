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

#include "Feature.h"

#include "PreCompiled.h"
#ifndef _PreComp_
#endif

#include <App/Application.h>

FC_LOG_LEVEL_INIT("NomANor", true, true)


namespace NomAnor
{

PROPERTY_SOURCE(NomAnor::Feature, PartDesign::Feature)

Feature::Feature()
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

short Feature::mustExecute() const
{
    if (Refine.isTouched()) {
        return 1;
    }
    return PartDesign::Feature::mustExecute();
}

}  // namespace NomAnor
