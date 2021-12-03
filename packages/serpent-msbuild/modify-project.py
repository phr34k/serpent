import clr
clr.AddReference("Microsoft.Build")
clr.AddReference("Microsoft.Build.Framework")
import Microsoft.Build.Evaluation
import System

def main():
    collection = Microsoft.Build.Evaluation.ProjectCollection();
    collection.DefaultToolsVersion = "4.0"
    project = collection.LoadProject(r"test.vcxproj");
    
    buildTargetName = "Test"
    pixelShaderItemEntry = "Pixels"
    
    # Add references to project
    slItemGroup = project.Xml.CreateItemGroupElement();
    project.Xml.InsertAfterChild(slItemGroup, project.Xml.LastChild);
    slItemGroup.AddItem("Reference", "mscorlib");
    slItemGroup.AddItem("Reference", "System");
    slItemGroup.AddItem("Reference", "System.Core");
    slItemGroup.AddItem("Reference", "System.Windows");

    # Add using task
    usingTaskElement2 = project.Xml.CreateUsingTaskElement("Perspective.PixelShader.BuildTask.PixelShaderBuildTask", None, "Perspective.PixelShader.BuildTask");
    project.Xml.InsertAfterChild(usingTaskElement2, project.Xml.LastChild);

    # Add using import task
    projectImportElement1 = project.Xml.CreateImportElement(r"$(MSBuildExtensionsPath32)\Microsoft\Silverlight\v4.0\Microsoft.Silverlight.CSharp.targets");
    project.Xml.InsertAfterChild(projectImportElement1, project.Xml.LastChild);

    # Add build target
    buildTarget = project.Xml.CreateTargetElement(buildTargetName);
    project.Xml.InsertAfterChild(buildTarget, project.Xml.LastChild);
    buildTarget.Condition = "'@(" + pixelShaderItemEntry + ")' != ''";    

    # Add task
    buildTask = buildTarget.AddTask("PixelShaderBuildTask");
    buildTask.SetParameter("SourceFiles", "@(" + pixelShaderItemEntry + ")");
    buildTask.AddOutputItem("OutputFiles", "Resource");

    project.SetProperty("DefaultTargets", buildTargetName);
    project.Save(r"test-modified.vcxproj");    

if __name__ == '__main__':
    main()

