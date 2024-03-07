/***************************************************************************
 *   Copyright (c) YEAR YOUR NAME <Your e-mail address>                    *
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


#include "PreCompiled.h"

#ifndef _PreComp_
#endif

#include "Workbench.h"
#include <Gui/MenuManager.h>
#include <Gui/ToolBarManager.h>

using namespace NomAnorGui;

/// @namespace NomAnorGui @class Workbench
TYPESYSTEM_SOURCE(NomAnorGui::Workbench, Gui::StdWorkbench)

Workbench::Workbench()
{}

Workbench::~Workbench()
{}

Gui::MenuItem* Workbench::setupMenuBar() const
{
    Gui::MenuItem* root = StdWorkbench::setupMenuBar();
    Gui::MenuItem* item = root->findItem("&Windows");

    Gui::MenuItem* nomanor = new Gui::MenuItem;
    root->insertItem(item, nomanor);
    nomanor->setCommand("NomAnor");
    *nomanor << "NomAnor_FeatureBox"
            << "NomAnor_FeatureCylinder"
            << "NomAnor_FeatureExtrude"
            << "NomAnor_FeatureBoolean"
            << "NomAnor_FeatureMirrored"
            << "NomAnor_FeatureLinearPattern"
            << "NomAnor_FeaturePolarPattern";

    return root;
}

Gui::ToolBarItem* Workbench::setupToolBars() const
{
    Gui::ToolBarItem* root = StdWorkbench::setupToolBars();

    Gui::ToolBarItem* features = new Gui::ToolBarItem(root);
    features->setCommand("NomAnor Features");
    *features << "NomAnor_FeatureBox"
            << "NomAnor_FeatureCylinder"
            << "NomAnor_FeatureExtrude"
            << "NomAnor_FeatureBoolean"
            << "NomAnor_FeatureMirrored"
            << "NomAnor_FeatureLinearPattern"
            << "NomAnor_FeaturePolarPattern";

    return root;
}
