rm -r files
mkdir files
cd files
for x in {0..24}; do
	base64 /dev/urandom | head -c 10000000 > file$x.txt
done