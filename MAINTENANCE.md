# Maintenance scripts

# Converting projects to build definitions

```
msbuild/convert.py "E:\Workspaces\Import\msvc\deploy.vcxproj"

msbuild_ext.visual_studio(
        project = "",
        files = [
                '..\code\Deploy\FileGlobBase.cpp',
                '..\code\Deploy\main.cpp',
                '..\code\Deploy\mainwindow.cpp',
        ],
        includes = [
                'E://Programming - VC Sdk//Qt\\5.5//msvc2012//include',
                'E://Programming - VC Sdk//Qt\\5.5//msvc2012//include//QtWidgets',
                'E://Programming - VC Sdk//Qt\\5.5//msvc2012//include//QtCore',
                'C://Python27-Clean\\include',
                'moc_deploy',
                '',
        ],
        libs = [
                'kernel32.lib',
                'user32.lib',
                'gdi32.lib',
                'winspool.lib',
                'comdlg32.lib',
                'advapi32.lib',
                'shell32.lib',
                'ole32.lib',
                'oleaut32.lib',
                'uuid.lib',
                'odbc32.lib',
                'odbccp32.lib',
                'C://Python27-Clean\libs/python27.lib',
                'E://Programming - VC Sdk//Qt\5.5/msvc2012/lib/Qt5Widgets.lib',
                'E://Programming - VC Sdk//Qt\5.5/msvc2012/lib/Qt5Core.lib',
                'E://Programming - VC Sdk//Qt\5.5/msvc2012/lib/Qt5Gui.lib',
                'E://Programming - VC Sdk//Qt\5.5/msvc2012/lib/qtmain.lib',
        ],
        name = 'new',
        defines = [
                'AAAA',
        ],
        outdir = r'..\Bin\Release\GraphIDE\',
        intdir = r'..\Intermediate\deploy\'
)
```