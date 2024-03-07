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


#ifndef NOMANOR_FeaturePattern_H
#define NOMANOR_FeaturePattern_H

#include <App/PropertyStandard.h>
#include <App/PropertyUnits.h>

#include "../NomAnorGlobal.h"
#include "FeatureToolShapes.h"

namespace NomAnor
{

class NomAnorExport FeaturePattern: public NomAnor::FeatureToolShapes
{
    PROPERTY_HEADER_WITH_OVERRIDE(NomAnor::FeaturePattern);

public:
    FeaturePattern();

    App::PropertyLinkList Features;

    short mustExecute() const override;

    std::vector<ToolShape> getToolShapes() const override;

    /// Apply this feature's transformation to the given shapes and return a list of new shapes
    virtual std::vector<ToolShape> applyTransformation(std::vector<ToolShape> shapes) const
    {
        return shapes;
    }
};

class NomAnorExport FeatureMirrored: public NomAnor::FeaturePattern
{
    PROPERTY_HEADER_WITH_OVERRIDE(NomAnor::FeatureMirrored);

public:
    FeatureMirrored();

    App::PropertyLinkSub MirrorPlane;

    short mustExecute() const override;

    std::vector<ToolShape> applyTransformation(std::vector<ToolShape> shapes) const override;

private:
    gp_Trsf calculateTransformation() const;
};

class NomAnorExport FeatureLinearPattern: public NomAnor::FeaturePattern
{
    PROPERTY_HEADER_WITH_OVERRIDE(NomAnor::FeatureLinearPattern);

public:
    enum class PatternMode
    {
        Length,
        Offset
    };

    FeatureLinearPattern();

    App::PropertyLinkSub Direction;
    App::PropertyBool Reversed;
    App::PropertyEnumeration Mode;
    App::PropertyLength Length;
    App::PropertyLength Offset;
    App::PropertyIntegerConstraint Occurrences;

    short mustExecute() const override;

    std::vector<ToolShape> applyTransformation(std::vector<ToolShape> shapes) const override;

private:
    App::DocumentObjectExecReturn* execute() override;

    void setReadWriteStatusForMode(PatternMode mode);
    std::vector<gp_Trsf> calculateTransformations() const;
};

class NomAnorExport FeaturePolarPattern: public NomAnor::FeaturePattern
{
    PROPERTY_HEADER_WITH_OVERRIDE(NomAnor::FeaturePolarPattern);

public:
    enum class PatternMode
    {
        Angle,
        Offset
    };

    FeaturePolarPattern();

    App::PropertyLinkSub Axis;
    App::PropertyBool Reversed;
    App::PropertyEnumeration Mode;
    App::PropertyAngle Angle;
    App::PropertyAngle Offset;
    App::PropertyIntegerConstraint Occurrences;

    short mustExecute() const override;

    std::vector<ToolShape> applyTransformation(std::vector<ToolShape> shapes) const override;

private:
    App::DocumentObjectExecReturn* execute() override;
    
    void setReadWriteStatusForMode(PatternMode mode);
    std::vector<gp_Trsf> calculateTransformations() const;
};

}  // namespace NomAnor


#endif  // NOMANOR_FeaturePattern_H
