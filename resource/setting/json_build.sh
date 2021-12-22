#super user use this file
#install
sudo apt install git
sudo apt install cmake

git clone https://github.com/json-c/json-c.git
mkdir json-c-build
cd json-c-build
cmake ../json-c

make
make test
make USE_VALGRIND=0 test
sudo make install

# ~/bashrc or ~/bash_profile add
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
# source ~/bash..
