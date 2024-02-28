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

#include <App/Document.h>
#include <Base/Console.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Mod/PartDesign/App/Body.h>
#include <Mod/PartDesign/Gui/Utils.h>

#include <Mod/NomAnor/App/Feature.h>


DEF_STD_CMD_A(CmdNomAnorFeature)

CmdNomAnorFeature::CmdNomAnorFeature()
    : Command("NomAnor_Feature")
{
    sAppModule = "NomAnor";
    sGroup = QT_TR_NOOP("NomAnor");
    sMenuText = QT_TR_NOOP("Feature");
    sToolTipText = QT_TR_NOOP("Feature add function");
    sWhatsThis = "NomAnor_Feature";
    sStatusTip = sToolTipText;
    sPixmap = "PartDesign_Pad";
}

void CmdNomAnorFeature::activated(int)
{
    PartDesign::Body* pcActiveBody = PartDesignGui::getBody(/*messageIfNot = */ true);
    if (!pcActiveBody) {
        return;
    }

    openCommand(QT_TRANSLATE_NOOP("Command", "Create Feature"));

    std::string featureName = getUniqueObjectName("Feature", pcActiveBody);
    FCMD_OBJ_CMD(pcActiveBody, "newObject('NomAnor::Feature', '" << featureName << "')");

    //auto feature =
    //    static_cast<NomAnor::Feature*>(pcActiveBody->getDocument()->getObject(featureName.c_str()));
}

bool CmdNomAnorFeature::isActive()
{
    if (getActiveGuiDocument()) {
        return true;
    }
    else {
        return false;
    }
}


void CreateNomAnorCommands(void)
{
    Gui::CommandManager& rcCmdMgr = Gui::Application::Instance->commandManager();

    rcCmdMgr.addCommand(new CmdNomAnorFeature());
}
