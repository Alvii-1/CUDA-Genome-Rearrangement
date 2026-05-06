if (Test-Path build) { Remove-Item -Recurse -Force build }
cmake -S . -B build
cmake --build build 
.\build\Debug\project.exe