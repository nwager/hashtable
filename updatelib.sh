#!/usr/bin/env bash

set -e
echo $#
if [ $# -ne 1 ]; then
  echo "USAGE: $0 <LINKEDLIST_LIB_DIR>"
  exit 1
fi

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

echo "Making linkedlist lib..."
cd $1
make lib
cd $SCRIPT_DIR
echo "Copying linkedlist lib into project..."
cp $1/libout/* lib
mv lib/linkedlist.h include
echo "Done"