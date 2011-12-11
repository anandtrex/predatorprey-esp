if [ ! -d "glog-0.3.1" ]; then 
	wget http://google-glog.googlecode.com/files/glog-0.3.1-1.tar.gz && tar xzvf glog-0.3.1-1.tar.gz
fi
if [ ! -d "glog" ]; then
	cd glog-0.3.1 && ./configure --prefix=`pwd`/../glog && make -j 2 && make install
fi

if [ ! -d "libconfig-1.4.8" ]; then 
	wget http://www.hyperrealm.com/libconfig/libconfig-1.4.8.tar.gz  && tar xzvf libconfig-1.4.8.tar.gz
fi
if [ ! -d "libconfig" ]; then
	cd libconfig-1.4.8 && ./configure --prefix=`pwd`/../libconfig && make -j 2 && make install
fi



