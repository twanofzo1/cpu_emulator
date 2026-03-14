if [ ! -d "build" ]; then
  mkdir build
fi

if [ "$1" == "clean" ]; then
  rm -rf build
  exit 0
fi

if [ "$1" == "rebuild" ]; then
  rm -rf build
  mkdir build
fi

if [ "$1" == "run" ]; then
  cd build
  ./cpu_emulator
  exit 0
fi

if [ "$1" == "debug" ]; then
  cmake -DCMAKE_BUILD_TYPE=Debug ..
  make
  exit 0
else
  echo "do you want to build in debug mode? (y/n)"
  read answer
  if [ "$answer" == "y" ]; then
    cmake -DCMAKE_BUILD_TYPE=Debug ..
  else
    cmake ..
  fi
fi

cd build 
cmake ..
make
