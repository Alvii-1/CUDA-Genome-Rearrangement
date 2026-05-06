HOW TO COMPILE, BUILD AND RUN THE PROJECT:

If this is the C++/CUDA template, please copy the entire folder, rename it, and continue.

Note: 
    - Update any mention of "project" in the CMakeLists.txt file
    - If the template project has a "build" folder, delete it before building.
    - Also copy the .vscode folder between projects.

Organization:
    - /src: main c++ files
    - /include: header files, both c++ and cu declarations
    - /kernels: all GPU code files

--- | Manual Way
1. Open VS Code
2. Click the dropdown arrow by the terminal, select "Command Prompt"
3. Type "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" into cmd
4. To compile code "cl main.cpp" to run it "main.exe"

--- | CMake Way 
1. Ensure the CMakeLists.txt is properly updated with a new project name
2. Type "cmake -S . -B build" into PS to configure the project
3. Type "cmake --build build" into PS to build the project
4. Type ".\build\Debug\project.exe" to run the project, make sure to change "project" to match txt

--- | Powershell Function (Simplest)
1. Type ".\run.ps1" in the terminal and it should accomplish everything from CMake