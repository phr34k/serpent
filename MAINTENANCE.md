# Maintenance scripts

## Converting projects to build definitions

```
msbuild/convert_project.py "E:\Workspaces\Import\msvc\deploy.vcxproj"
---------------------------------------------------------------------
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

## Overriding properties in a msbuild file

```
python msbuild/change_project.py "E:\Workspaces\Import\msvc\deploy.vcxproj" "E:\Workspaces\Import\msvc\deploy-modified.vcxproj" --c:'$(Configuration)'=='ReleaseDebug' --p:indexer=2
---------------------------------------------------------------------
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.default.props" />
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />  
		<ItemGroup>
---------------------------------------------------------------------
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.default.props" />
  <PropertyGroup Condition="'$(Configuration)'=='ReleaseDebug'" Label="test">
    <indexer>2</indexer>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
```

## Formatting the project

```
python msbuild/format_project.py "E:\Workspaces\Import\msvc\deploy.vcxproj"
```

## Walking over all dependecies of a project

```
python msbuild/refs_project.py "E:\Workspaces\Import\Code\Aurora.Acl.Texture\source\extern\freeimage\FreeImageLib\FreeImageLib.2013.vcxproj"
---------------------------------------------------------------------
{5e1d4e5f-e10c-4ba3-b663-f33014fd21d9} E:\Workspaces\Import\Code\Aurora.Acl.Texture\source\extern\freeimage\LibJPEG\LibJPEG.2013.vcxproj
{244455e0-5f25-4451-9540-f317883e52a8} E:\Workspaces\Import\Code\Aurora.Acl.Texture\source\extern\freeimage\LibJXR\LibJXR.2013.vcxproj
{e3536c28-a7f1-4b53-8e52-7d2232f9e098} E:\Workspaces\Import\Code\Aurora.Acl.Texture\source\extern\freeimage\LibOpenJPEG\LibOpenJPEG.2013.vcxproj
{5e1d4e5f-e10c-4ba3-b663-f33014fd21d9} E:\Workspaces\Import\Code\Aurora.Acl.Texture\source\extern\freeimage\LibJPEG\LibJPEG.2013.vcxproj
{33134f61-c1ad-4b6f-9cea-503a9f140c52} E:\Workspaces\Import\Code\Aurora.Acl.Texture\source\extern\freeimage\ZLib\ZLib.2013.vcxproj
{ec085cbd-e9c3-477f-9a97-cb9d5da30e27} E:\Workspaces\Import\Code\Aurora.Acl.Texture\source\extern\freeimage\LibTIFF4\LibTIFF4.2013.vcxproj
{097d9f6c-fd0e-4cbc-9676-009012aaeca8} E:\Workspaces\Import\Code\Aurora.Acl.Texture\source\extern\freeimage\LibWebP\LibWebP.2013.vcxproj
{33134f61-c1ad-4b6f-9cea-503a9f140c52} E:\Workspaces\Import\Code\Aurora.Acl.Texture\source\extern\freeimage\ZLib\ZLib.2013.vcxproj
{17a4874b-0606-4687-90b6-f91f8cb3b8af} E:\Workspaces\Import\Code\Aurora.Acl.Texture\source\extern\freeimage\OpenEXR\OpenEXR.2013.vcxproj
{33134f61-c1ad-4b6f-9cea-503a9f140c52} E:\Workspaces\Import\Code\Aurora.Acl.Texture\source\extern\freeimage\ZLib\ZLib.2013.vcxproj
```
