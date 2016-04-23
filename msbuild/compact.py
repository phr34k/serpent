import os, sys, re, shutil

def verify(path):
    if os.path.isdir(path):
        shutil.rmtree(path)
    else:
        os.remove(path)

def scan(path):
    for subdir, dirs, files in os.walk(path):
        for dir in dirs:
            filepath = subdir + os.sep + dir
            if filepath.endswith("_UpgradeReport_Files"):
                verify(filepath)
            if filepath.endswith("Backup"):
                verify(filepath)
        for file in files:
            filepath = subdir + os.sep + file
            if filepath.endswith(".ncb"):
                verify(filepath)
            if filepath.endswith(".sdf"):
                verify(filepath)
            if filepath.endswith(".opensdf"):
                verify(filepath)
            if filepath.endswith("UpgradeLog.XML"):
                verify(filepath)
            if filepath.endswith("UpgradeLog.htm"):
                verify(filepath)


if __name__ == "__main__":
    if len(sys.argv) > 1:
        scan(sys.argv[1])
    else:
        scan(".")
