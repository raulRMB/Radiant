set enginepath=C:/Source/UE/
set uprojectpath=C:/Radiant/Radiant.uproject
set outputpath=C:/Assets/RadiantServer

copy Dockerfile "%outputpath%/Dockerfile"
"%enginepath%Engine/Build/BatchFiles/RunUAT.bat"  -ScriptsForProject="%uprojectpath%" Turnkey -command=VerifySdk -platform=Linux -UpdateIfNeeded -EditorIO -EditorIOPort=52026  -project="%uprojectpath%" BuildCookRun -nop4 -utf8output -nocompileeditor -skipbuildeditor -cook  -project="%uprojectpath%" -target=RadiantServer  -unrealexe="%enginepath%Engine/Binaries/Win64/UnrealEditor-Cmd.exe" -platform=Linux -stage -archive -package -build -pak -iostore -compressed -prereqs -applocaldirectory="%enginepath%/Engine/Binaries/ThirdParty/AppLocalDependencies" -archivedirectory="%outputpath%" -CrashReporter -server -noclient -serverconfig=Development