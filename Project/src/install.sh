cd ..
cp conf/proj /etc/system.conf.d
mkdir /home/snaktionary
cp -vr res/ /home/snaktionary
chmod 777 src/compile.sh
chmod 777 src/run.sh