BEGIN {
    noFileNames=1;
}
/FILE: (.+\.[ch]) /  { 
    if (outputdir!="") {
	fileNameLst[noFileNames]=outputdir "/" $3;
    } else {
	fileNameLst[noFileNames]="./" $3;
    }
    noFileNames++
}
END {
    printf "cat %s | csplit -s - '/\\/\\* FILE: /' {*}\n", FILENAME;
    ndx=1;
    print "rm xx00"
    for (ndx=1; ndx <noFileNames; ndx++) {
	printf "mv xx%.2d %s\n", ndx, fileNameLst[ndx];
    }	
}
