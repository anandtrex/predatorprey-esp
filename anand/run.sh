#!/bin/bash
# export GLOG_log_dir=`pwd`/log/
# export GLOG_minloglevel=0
# export GLOG_stderrthreshold=2
export GLOG_logtostderr=1
export GLOG_v=2 # 4 is verbose debug log, 2 gives position updates every step
export LD_LIBRARY_PATH=`pwd`/glog/lib:`pwd`/libconfig/lib:$LD_LIBRARY_PATH
./esp-predprey -c esp_predprey.cfg
