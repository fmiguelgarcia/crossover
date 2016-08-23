# crossover
Crossover project

Requirement
  - CMake 2.8 or greater
  - Qt 5.3.2 or greater.
  - Linux platform on client: due to the functions used to load CPU, memory and number of processes.

How to build

  1. Go to the source code folder
    cd Source
  2. Create the external build folder, i.e. "build".
    mkdir build
  3. Use CMake to build the solution.
    3.1. On Release mode.
      cd build
      cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH=install ..
      make install

    3.2 On Debug mode
      cd build
      cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX:PATH=install ..
      make install

  4. Binaries will be installed into build/install/bin folder.
