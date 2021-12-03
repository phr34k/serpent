import clr, sys
SWF = clr.AddReference("Microsoft.Build")
clr.AddReference("Microsoft.Build.Framework")
print SWF.Location
import Microsoft.Build.Evaluation
import Microsoft.Build.Construction
import System

def main(project):
    collection = Microsoft.Build.Evaluation.ProjectCollection();
    collection.DefaultToolsVersion = "4.0"


    print project
    solution = Microsoft.Build.Construction.SolutionFile.Parse(project)
    print solution

    project = collection.LoadProject(project);

    includes = ''
    libs = ''
    defines = ''
    libdirs = ''
    outdir = project.GetPropertyValue('OutDir')
    intdir = project.GetPropertyValue('IntDir')
    for y in project.ItemDefinitions:
        if y.ItemType == "ClCompile":
            includes = y.GetMetadataValue("AdditionalIncludeDirectories")            
            defines = y.GetMetadataValue("PreprocessorDefinitions")
        if y.ItemType == "Link":
            libs = y.GetMetadataValue("AdditionalDependencies")
            libdirs = y.GetMetadataValue("AdditionalLibraryDirectories")

    print "msbuild_ext.visual_studio("
    print "\tproject = \"\","
    print "\tfiles = ["            
    for x in project.GetItems("ClCompile"):
        print "\t\t'" + x.EvaluatedInclude.replace("\\","/") + "',"
    print "\t],"

    print "\tincludes = ["            
    for y in includes.split(';'):
        print "\t\t'" + y.replace("\\","/") + "',"
    print "\t],"

    print "\tlibs = ["            
    for y in libdirs.split(';'):
        print "\t\t'" + y.replace("\\","/") + "',"
    print "\t],"    

    print "\tlibdirs = ["            
    for y in includes.split(';'):
        print "\t\t'" + y.replace("\\","/") + "',"
    print "\t],"    

    print "\tname = 'new',"
    print "\tdefines = ["            
    for y in defines.split(';'):
        print "\t\t'" + y + "',"
    print "\t],"  
    print "\toutdir = r'%s'," % outdir.replace("\\","/")
    print "\tintdir = r'%s'" % intdir.replace("\\","/")
    print ")"

if __name__ == '__main__':
    main(sys.argv[1])

