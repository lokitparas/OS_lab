for x in {0..9999}; do
  dd if=/dev/zero ibs=2M count=1 of=foo$x.txt 
done