#!/bin/bash
# export GLOG_log_dir=`pwd`/log/
# export GLOG_minloglevel=0
# export GLOG_stderrthreshold=2
export GLOG_logtostderr=1
export LD_LIBRARY_PATH=`pwd`/glog/lib:`pwd`/libconfig/lib:$LD_LIBRARY_PATH
build/esp-predprey config_file.txt