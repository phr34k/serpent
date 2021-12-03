import clr, sys
clr.AddReference("Microsoft.Build")
clr.AddReference("Microsoft.Build.Framework")
import Microsoft.Build.Evaluation
import System

def main(project):
    collection = Microsoft.Build.Evaluation.ProjectCollection();
    collection.DefaultToolsVersion = "4.0"
    project = collection.LoadProject(project);

    includes = ''
    libs = ''
    defines = ''
    outdir = project.GetPropertyValue('OutDir')
    intdir = project.GetPropertyValue('IntDir')
    for y in project.ItemDefinitions:
        if y.ItemType == "ClCompile":
            includes = y.GetMetadataValue("AdditionalIncludeDirectories")            
            defines = y.GetMetadataValue("PreprocessorDefinitions")
        if y.ItemType == "Link":
            libs = y.GetMetadataValue("AdditionalDependencies")        

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
    for y in libs.split(';'):
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

