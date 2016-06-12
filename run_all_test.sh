#!/bin/bash

for i in $(ls testcase/src/*.c | sed -- 's/.*\///g' | sed -- 's/\.c//g'); do
  export BUILD_OBJ=./obj/testcase/$i;
  (echo -ne 'c\nq\n' | make run > $i-log.txt) || (echo "Aborted" >> $i-log.txt);
  echo "$i done.";
done;

echo "TODO: ";
IFS='
';
set -f;
res=$(find ./ -type f -name *-log.txt -exec grep invalid {} +);
for i in $res; do
  addr=$(echo "$i" | sed 's/^.*eip\s=\s0x00\([0-9a-z]*\).*$/\1/g');
  filepath=$(echo "$i" | sed 's|^\./\(.*\)-log.txt:.*$|\1|g');
  echo "$addr $filepath";
  objdump -d "./obj/testcase/$filepath" | grep "$addr" --color;
done;
set +f;
unset IFS;

#grep invalid ./*-log.txt --color;
echo "Result: ";
grep GOOD ./*-log.txt --color;
grep BAD ./*-log.txt --color;
pcregrep -M 'c\nAborted' ./*-log.txt;
