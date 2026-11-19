#!/usr/bin/bash
for dir in ./2.* 
do
       	cd "$dir" ;
	echo "Building binaries for $dir" ;
       	make all 1> /dev/null;
	if [ $? != 0 ] ;then
		echo "failed to build binaries for $dir";
		exit 1;
	fi

	echo "Running binaries for $dir" ;
	./Expand-PoC ;
	if [ $? != 0 ] ;then
		echo "Expand-PoC failed for $dir";
		exit 1;
	fi

       	./Leak-PoC ;
	if [ $? != 0 ] ;then
		echo "Leak-PoC failed for $dir";
		exit 1;
	fi

       	./V1-PoC ;
	if [ $? != 0 ] ;then
		echo "V1-PoC failed for $dir";
		exit 1;
	fi

       	./V2-PoC ;
	if [ $? != 0 ] ;then
		echo "V2-PoC failed for $dir";
		exit 1;
	fi
	echo "All binaries succeeded for $dir";
	echo -e "Cleaning up...\n\n";
	make clean 1> /dev/null;
	cd ../ ; 
done
echo "All tests succeeded"
