BEGIN {
	noFileNames=1;
	targetSrcRoot=outputdir;
}
/FILE: (.+\.java) /  { 
	fileNameLst[noFileNames]=$3;
}
/package (.+);/ {
	pkgName = $2
	gsub(/\.|;/, "/", pkgName);
	pkgNameLst[noFileNames++] = pkgName;
}
END {
	printf "cat %s | csplit -s - '/\\/\\* FILE: /' {*}\n", FILENAME;
	ndx=1;
	print "rm xx00"
	for (ndx=1; ndx <noFileNames; ndx++) {
		printf "mkdir -p %s/%s\n", targetSrcRoot, pkgNameLst[ndx];
		printf "mv xx%.2d %s/%s%s\n", ndx, targetSrcRoot, pkgNameLst[ndx], fileNameLst[ndx];
	}
	
}
