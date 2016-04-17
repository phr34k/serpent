import clr, sys, re
clr.AddReference("Microsoft.Build")
clr.AddReference("Microsoft.Build.Framework")
import Microsoft.Build.Evaluation
import System

def main(project, project2, arguments):
    collection = Microsoft.Build.Evaluation.ProjectCollection();
    collection.DefaultToolsVersion = "4.0"
    project = collection.LoadProject(project);
    project.Save(project);

if __name__ == '__main__':
    main(sys.argv[1])

