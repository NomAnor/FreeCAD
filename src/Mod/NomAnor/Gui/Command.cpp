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


DEF_STD_CMD_A(CmdNomAnorFeatureBox)

CmdNomAnorFeatureBox::CmdNomAnorFeatureBox()
    : Command("NomAnor_FeatureBox")
{
    sAppModule = "NomAnor";
    sGroup = QT_TR_NOOP("NomAnor");
    sMenuText = QT_TR_NOOP("Box");
    sToolTipText = QT_TR_NOOP("Add box feature");
    sWhatsThis = "NomAnor_FeatureBox";
    sStatusTip = sToolTipText;
    sPixmap = "PartDesign_AdditiveBox";
}

void CmdNomAnorFeatureBox::activated(int)
{
    PartDesign::Body* pcActiveBody = PartDesignGui::getBody(/*messageIfNot = */ true);
    if (!pcActiveBody) {
        return;
    }

    openCommand(QT_TRANSLATE_NOOP("Command", "Create Feature"));

    std::string featureName = getUniqueObjectName("Box", pcActiveBody);
    FCMD_OBJ_CMD(pcActiveBody, "newObject('NomAnor::FeatureBox', '" << featureName << "')");
    Gui::Command::updateActive();

    //auto feature =
    //    static_cast<NomAnor::Feature*>(pcActiveBody->getDocument()->getObject(featureName.c_str()));
}

bool CmdNomAnorFeatureBox::isActive()
{
    return getActiveGuiDocument();
}


DEF_STD_CMD_A(CmdNomAnorFeatureCylinder)

CmdNomAnorFeatureCylinder::CmdNomAnorFeatureCylinder()
    : Command("NomAnor_FeatureCylinder")
{
    sAppModule = "NomAnor";
    sGroup = QT_TR_NOOP("NomAnor");
    sMenuText = QT_TR_NOOP("Cylinder");
    sToolTipText = QT_TR_NOOP("Add cylinder feature");
    sWhatsThis = "NomAnor_FeatureCylinder";
    sStatusTip = sToolTipText;
    sPixmap = "PartDesign_AdditiveCylinder";
}

void CmdNomAnorFeatureCylinder::activated(int)
{
    PartDesign::Body* pcActiveBody = PartDesignGui::getBody(/*messageIfNot = */ true);
    if (!pcActiveBody) {
        return;
    }

    openCommand(QT_TRANSLATE_NOOP("Command", "Create Feature"));

    std::string featureName = getUniqueObjectName("Cylinder", pcActiveBody);
    FCMD_OBJ_CMD(pcActiveBody, "newObject('NomAnor::FeatureCylinder', '" << featureName << "')");
    Gui::Command::updateActive();

    //auto feature =
    //    static_cast<NomAnor::Feature*>(pcActiveBody->getDocument()->getObject(featureName.c_str()));
}

bool CmdNomAnorFeatureCylinder::isActive()
{
    return getActiveGuiDocument();
}


DEF_STD_CMD_A(CmdNomAnorFeatureExtrude)

CmdNomAnorFeatureExtrude::CmdNomAnorFeatureExtrude()
    : Command("NomAnor_FeatureExtrude")
{
    sAppModule = "NomAnor";
    sGroup = QT_TR_NOOP("NomAnor");
    sMenuText = QT_TR_NOOP("Extrude");
    sToolTipText = QT_TR_NOOP("Add an extrude feature");
    sWhatsThis = "NomAnor_FeatureExtrude";
    sStatusTip = sToolTipText;
    sPixmap = "PartDesign_Pad";
}

void CmdNomAnorFeatureExtrude::activated(int)
{
    PartDesign::Body* pcActiveBody = PartDesignGui::getBody(/*messageIfNot = */ true);
    if (!pcActiveBody) {
        return;
    }

    openCommand(QT_TRANSLATE_NOOP("Command", "Create Feature"));

    std::string featureName = getUniqueObjectName("Extrusion", pcActiveBody);
    FCMD_OBJ_CMD(pcActiveBody, "newObject('NomAnor::FeatureExtrude', '" << featureName << "')");
    Gui::Command::updateActive();

    //auto feature =
    //    static_cast<NomAnor::Feature*>(pcActiveBody->getDocument()->getObject(featureName.c_str()));
}

bool CmdNomAnorFeatureExtrude::isActive()
{
    return getActiveGuiDocument();
}


DEF_STD_CMD_A(CmdNomAnorFeatureBoolean)

CmdNomAnorFeatureBoolean::CmdNomAnorFeatureBoolean()
    : Command("NomAnor_FeatureBoolean")
{
    sAppModule = "NomAnor";
    sGroup = QT_TR_NOOP("NomAnor");
    sMenuText = QT_TR_NOOP("Boolean");
    sToolTipText = QT_TR_NOOP("Add a boolean feature");
    sWhatsThis = "NomAnor_FeatureBoolean";
    sStatusTip = sToolTipText;
    sPixmap = "PartDesign_Boolean";
}

void CmdNomAnorFeatureBoolean::activated(int)
{
    PartDesign::Body* pcActiveBody = PartDesignGui::getBody(/*messageIfNot = */ true);
    if (!pcActiveBody) {
        return;
    }

    openCommand(QT_TRANSLATE_NOOP("Command", "Create Feature"));

    std::string featureName = getUniqueObjectName("Boolean", pcActiveBody);
    FCMD_OBJ_CMD(pcActiveBody, "newObject('NomAnor::FeatureBoolean', '" << featureName << "')");
    Gui::Command::updateActive();

    //auto feature =
    //    static_cast<NomAnor::Feature*>(pcActiveBody->getDocument()->getObject(featureName.c_str()));
}

bool CmdNomAnorFeatureBoolean::isActive()
{
    return getActiveGuiDocument();
}


void CreateNomAnorCommands(void)
{
    Gui::CommandManager& rcCmdMgr = Gui::Application::Instance->commandManager();

    rcCmdMgr.addCommand(new CmdNomAnorFeatureBox());
    rcCmdMgr.addCommand(new CmdNomAnorFeatureCylinder());

    rcCmdMgr.addCommand(new CmdNomAnorFeatureExtrude());
    
    rcCmdMgr.addCommand(new CmdNomAnorFeatureBoolean());
}
