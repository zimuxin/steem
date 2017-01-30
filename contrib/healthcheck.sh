#!/bin/bash

BLOCKCHAIN_TIME=`curl --max-time 3 --data '{"id":39,"method":"get_dynamic_global_properties","params":[]}' localhost:8090 | jq -r .result.time`

if [[ ! -z "$BLOCKCHAIN_TIME" ]]; then
  BLOCKCHAIN_SECS=`date -d $BLOCKCHAIN_TIME +%s`
  CURRENT_SECS=`date +%s`

  # if we're within 60 seconds of current time, call it synced and report healthy
  BLOCK_AGE=$((${CURRENT_SECS} - ${BLOCKCHAIN_SECS}))
  if [[ ${BLOCK_AGE} -le 60 ]]; then
    echo Status: 200
    echo Content-type:text/plain
    echo
    echo Block age is less than 60 seconds old, this node is considered healthy.
  else
    echo Status: 503
    echo Content-type:text/plain
    echo
    echo The node is responding but block chain age is $BLOCK_AGE seconds old
  fi
else
  echo Status: 502
  echo Content-type:text/plain
  echo
  echo The node is currently not responding.
fi
