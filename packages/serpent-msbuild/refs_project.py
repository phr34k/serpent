import clr, sys, re, os
clr.AddReference("Microsoft.Build")
clr.AddReference("Microsoft.Build.Framework")
import Microsoft.Build.Evaluation
import System

def main(project):
    directory = os.path.dirname(os.path.abspath(project))
    collection = Microsoft.Build.Evaluation.ProjectCollection();
    collection.DefaultToolsVersion = "4.0"
    project = collection.LoadProject(project, "4.0");
    for x in project.GetItems("ProjectReference"):
    	guid = x.GetMetadataValue("Project") 
    	absolute_project = os.path.abspath(os.path.join(directory, x.EvaluatedInclude))
    	main(absolute_project)
    	print guid, absolute_project
    

if __name__ == '__main__':
    main(sys.argv[1])

