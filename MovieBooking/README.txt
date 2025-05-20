Running Docker:
Execute
```
./docker_main.sh
```
In Git bash on Windows, or in Bash in Linux.

If you get this error:
```
error: building gtest:x64-linux failed with: BUILD_FAILED
See https://learn.microsoft.com/vcpkg/troubleshoot/build-failures?WT.mc_id=vcpkg_inproduct_cli for more information.
Elapsed time to handle gtest:x64-linux: 23 s
Please ensure you're using the latest port files with `git pull` and `vcpkg update`.
Then check for known issues at:
  https://github.com/microsoft/vcpkg/issues?q=is%3Aissue+is%3Aopen+in%3Atitle+gtest
You can submit a new issue at:
  https://github.com/microsoft/vcpkg/issues/new?title=[gtest]+Build+error+on+x64-linux&body=Copy%20issue%20body%20from%20%2Fproject%2Fvcpkg_installed%2Fvcpkg%2Fissue_body.md
```

then just run it again.

==========================================

Prerequisites:
On Windows, Install Ninja, and Visual Studio Community 2022 or Visual Studio Build Tools.

For Windows, you can open the folder with Visual Studio community, make sure you have "Win x64 Debug" as the preset (uses vcpkg) and build. The alternative would be to use the `conan-win64-debug` preset.

From command line (you might need to use VS Cross Tools), issue these commands:

cmake --preset=win-x64-debug
cmake --build --preset=win-x64-debug
ctest --preset=win-x64-debug

Please note that the release build is not up to date and may not work.

Please note that the conan preset generates a CMakeUserPresets json file, which may interfere with other preset types. It is preferable to delete it if switching to a different preset.

Read conan.md for details on configuring conan.

On Windows, if you build with the "Win x64 Debug" preset, then you need to first go to the MovieBooking/external directory
and execute:

```
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
git checkout 2025.04.09

.\bootstrap-vcpkg.bat
```


==================

Use `python3 client_zmq.py` to spawn a client that connects to the app zmq server.

You also have `start_zmq_clients.sh` and `start_zmq_clients.bat` if you want to run multiple clients in parallel.

