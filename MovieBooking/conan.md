Setting up conan:

*) Install Python3 (I used the Python3 from Windows Store)
*) `python -m pip install --upgrade pip` to make sure that you have the latest pip
*) `pip install pipx` to install pipx
*) `python -m pipx ensurepath` to add python tools (such as pipx) to PATH and thus be able to run `pipx` and `conan` without providing full path (May be required only for Python from Windows Store)
*) Restart Git Bash / shell, to reload the PATH
*) `pip install conan` or `pipx install conan`
*) `conan --version` --- make sure you can call conan
*) conan profile detect --force --name=win64-profile
*) Go to `~/.conan2/profiles/win64-profile` and make changes -- I updated `compiler.cppstd` to `17` from the default generated (`14`)
*) In the project directory, execute:
     conan install . --profile:host=win64-profile --profile:build=win64-profile --output-folder=conan-cmake-modules --build=missing

     cmake -B out/conan-build -S . -DCMAKE_TOOLCHAIN_FILE=conan-cmake-modules/conan_toolchain.cmake
     cmake --build out/conan-cmake-build
