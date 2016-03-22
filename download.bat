powershell -Command "(New-Object Net.WebClient).DownloadFile('https://github.com/phr34k/serpent/releases/download/0.0.98/serpent-release-x86-windows.zip', 'serpent-release-x86-windows.zip')"
python -m zipfile -e serpent-release-x86-windows.zip bin\release
del serpent-release-x86-windows.zip