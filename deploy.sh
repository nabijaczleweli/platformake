#!/bin/sh

uploader="./dropbox_uploader.sh -f .dropbox_uploader"
subfolder_name="$TRAVIS_BUILD_NUMBER-$TRAVIS_COMMIT_RANGE-$TRAVIS_BRANCH"
outfilename="platformake-$TRAVIS_BUILD_NUMBER-$COMPILER_NAME"


curl -Os "https://raw.githubusercontent.com/andreafabrizi/Dropbox-Uploader/master/dropbox_uploader.sh"
chmod +x dropbox_uploader.sh


$uploader mkdir "$subfolder_name"
$uploader upload out/platformake.out "$subfolder_name/$outfilename"
for file in README.md LICENSE
do
	$uploader -s upload "$file" "$subfolder_name/"
done
