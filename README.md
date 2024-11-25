This is a game made in 3 days as part of the GMTK Game Jam 2024 with the theme "Built to Scale". It is about a little pufferfish that wants to save the ocean using his puffing powers. 

You can get the game at: https://xerca.itch.io/overblown

# Cmake build

## Web
emcmake cmake -G "Ninja" -S . -B build -DCMAKE_BUILD_TYPE=Release

## Windows
cmake -S . -B build-windows/release -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++
cmake -S . -B build-windows/debug -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=g++

# Project build

## Web
emmake ninja

## Windows
ninja

# Serve
python serve.py
