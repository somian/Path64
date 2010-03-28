# Replacement for SGI/Cray "caterr". Allows optional -c, but expects that
# input and output filenames are explicit and not "-"

trap "rm -f /tmp/caterr.$$" 0

if [ "$1" = "-c" ]
then
  shift
  echo '$set 1 nocomment' > /tmp/caterr.$$
  sed -n -e '/^\$msg/s/^\$msg[ \t]*//p' < "$2" >> /tmp/caterr.$$
  gencat "$1" /tmp/caterr.$$
else
  echo '$set 1 nocomment' > "$1"
  sed -n -e '/^\$msg/s/^\$msg[ \t]*//p' < "$2" >> "$1"
fi
