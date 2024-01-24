#!env sh

if [ "x${1}" = "x" ]; then
    echo "No source parameter";
    exit 1
fi
if [ "x${2}" = "x" ]; then
    echo "No dest parameter";
    exit 1
fi
if [ "x${3}" = "x" ]; then
    echo "No patchdir parameter";
    exit 1
fi

SOURCE="$1"
DEST="$2"
PATCHDIR="$3"

LS="ls"
MV="mv"
MKTEMP="mktemp"
PATCH="patch"
TAR="tar"
TAREXTRACT="tar"

curdir=`pwd`
tmpdir=`"${MKTEMP}" -d`
tmpfile=`"${MKTEMP}" --suffix .tar.gz`

echo "=== Extracting ${SOURCE} in ${tmpdir}"
${TAR} -C ${tmpdir} -zxf ${SOURCE}
for i in `ls ${PATCHDIR}/*.patch`; do echo "=== Applying $i" && ${PATCH} -d ${tmpdir}/* -p1 < $i; done
cd ${tmpdir}
echo "=== Producing new tarball in ${tmpfile}"
${TAR} -zcf ${tmpfile} *
cd ${curdir}
echo "=== Moving ${tmpfile} to ${DEST}"
${MV} ${tmpfile} ${DEST}

exit 0
