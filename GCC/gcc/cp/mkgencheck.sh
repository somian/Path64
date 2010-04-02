#!/bin/sh

EXECDIR=`dirname $0`
BASEDIR=`dirname ${EXECDIR}`
for f in ${EXECDIR}/cp-tree.def
do
  echo "#include \"$f\"";
done
# TODO: Is this needed?
# sed "s|${BASEDIR}/||"
