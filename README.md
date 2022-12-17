# random값 생성을 위한 radamsa install
sudo apt-get install gcc make git wget\n
git clone https://gitlab.com/akihe/radamsa.git && cd radamsa && make && sudo make install
echo "HAL 9000" | radamsa   #설치 확인

# fuzzer download
git clone https://github.com/bob11vrdp/vBoxFuzzer/
cd vBoxFuzzer/rdpusb/client-1.8.4
chmod 777 ./bootstrap
chmod 777 ./configure

# librdesktop.so build
./bootstrap
./configure --disable-credssp --disable-smartcard
cp ../Makefile .   # Makefile이 자동으로 생성되는데, ../Makefile으로 교체
gedit Makefile     # MY_ROOT 변수 값을 내 VirtualBox 위치로 지정
make librdesktop   # so파일 생성

# librdesktop.so Test
python3 ./remoate_fuzzer.py <ip>   #원격 테스트

# Server 
./loadall.sh    # Building VirtualBox kernel modules
./VBoxHeadless -s <uuid> --vrde on    # VBoxHeadless start
