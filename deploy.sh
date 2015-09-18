#!/bin/sh

uploader="./dropbox_uploader.sh -f .dropbox_uploader"
subfolder_name="$TRAVIS_BUILD_NUMBER-$TRAVIS_COMMIT_RANGE-$TRAVIS_BRANCH"
defaultoutfilename="platformake-$TRAVIS_BUILD_NUMBER-$COMPILER_NAME"
newestoutfilename="platformake-$COMPILER_NAME"


curl -Os "https://raw.githubusercontent.com/andreafabrizi/Dropbox-Uploader/master/dropbox_uploader.sh"
chmod +x dropbox_uploader.sh


for subfolder in "$subfolder_name" "0-newest"
do
	$uploader mkdir "$subfolder"
	if [ "$subfolder" == "0-newest" ]
	then
		outfilename="$newestoutfilename"
	else
		outfilename="$defaultoutfilename"
	fi
	$uploader upload out/platformake.out "$subfolder/$outfilename"
	for file in README.md LICENSE
	do
		$uploader -s upload "$file" "$subfolder/"
	done
done
