# Setup Instructions

## Install Unreal Engine Source
### Cloning the repo
- Go to the [Unreal Engine website](https://www.unrealengine.com) and login with an Epic Games account
- Go to the [Connections tab](https://www.unrealengine.com/account/connections) and link your epic games account with your Github account
- This will add you to the Epic Games organization on github, and be able to clone the [engine source code](https://github.com/EpicGames/UnrealEngine)
### Installing Visual Studio
- Install [Visual Studio Community Edition](https://visualstudio.microsoft.com/vs/community)
- Add the following workflows and individual components
    - Workflows
        - .NET Desktop Development
        - Game Development with C++
    - Individual Components
        - C++ profiling tools
        - Windows 10/11 SDK (10.0.18362 or newer)
        - Unreal Engine Installer

### Setup Engine
- Run `git checkout 5.1.1-release` to checkout the correct version of the engine
- In the root of the unreal source run `Setup.bat`
- Then run GenerateProjectFiles.bat to create project files for the engine
- Open the UE5.sln file
- Set your solution configuration to Development Editor and your solution platform to Win64, then right click the UE5 target and select Build
- Run the engine from {Install_Location}\UnrealEngine\Engine\Binaries\Win64\UnrealEditor.exe (a shortcut to this is recommended)
 
### Run Project
- Right click Radiant.uproject -> Generate Visual Studio Project Files
- Open Radiant.sln and build the project
- Right click Radiant.uproject -> Switch to unreal engine version and select the source code build