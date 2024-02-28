# NomAnor gui init module
# (c) 2001 Juergen Riegel LGPL


class NomAnorWorkbench(Workbench):
    "NomAnor workbench object"
    MenuText = "NomAnor"
    ToolTip = "NomAnor workbench"
    Icon = FreeCAD.getResourceDir() + "Mod/NomAnor/Resources/icons/NomAnorWorkbench.svg"

    def Initialize(self):
        # load the module
        import NomAnorGui

    def GetClassName(self):
        return "NomAnorGui::Workbench"


Gui.addWorkbench(NomAnorWorkbench())
