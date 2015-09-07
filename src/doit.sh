#!/bin/bash

#TESTS="128 256 512 1024"
TESTS="2 4 8 16 32 64 128 256 512 1024"
#TESTNUM=10
TESTFOLDER="/home/alessio/Dropbox/UNI/MECS/Tesi/linux/performanceAndTest/taskset/"
OUTPUTFOLDER="/home/alessio/performanceAndTest/results/"
#OUTPUTFOLDER="/home/alessio/Dropbox/UNI/MECS/Tesi/linux/performanceAndTest/results/"

mkdir -p $OUTPUTFOLDER`uname -r`


# Check if all the functions are available for tracing


for t in $TESTS
do
  echo "Performing test $t"

#  for i in $(seq 1 $TESTNUM)
#  do
#    echo "# $i"
    
    echo nop > /sys/kernel/debug/tracing/current_tracer
    echo 0 > /sys/kernel/debug/tracing/options/sleep-time

    echo dl_task_timer > /sys/kernel/debug/tracing/set_ftrace_filter
    echo update_curr_dl >> /sys/kernel/debug/tracing/set_ftrace_filter
    echo enqueue_task_dl >> /sys/kernel/debug/tracing/set_ftrace_filter
    echo dequeue_task_dl >> /sys/kernel/debug/tracing/set_ftrace_filter

    FUNCTIONSNUMBER=`grep -E -w 'ss_queue_timer_elapsed' \
        /sys/kernel/debug/tracing/available_filter_functions \
        | wc -l`
    if [ $FUNCTIONSNUMBER -eq "1" ]
    then
        echo ss_queue_timer_elapsed >> /sys/kernel/debug/tracing/set_ftrace_filter
    fi

    echo 0 > /sys/kernel/debug/tracing/function_profile_enabled
    echo 1 > /sys/kernel/debug/tracing/function_profile_enabled

    ./spawner < "$TESTFOLDER$t.json" > /dev/null

    cat /sys/kernel/debug/tracing/trace_stat/function0 \
      | grep -E "(dl_task_timer|update_curr_dl|enqueue_task_dl|dequeue_task_dl|ss_queue_timer_elapsed)" \
      | grep -v init \
      > "$OUTPUTFOLDER`uname -r`"/"$t".txt
#  done

done

exit 0


