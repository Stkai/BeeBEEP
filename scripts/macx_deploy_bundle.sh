#!/bin/sh
#
# This file is part of BeeBEEP.
#
# BeeBEEP is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published
# by the Free Software Foundation, either version 3 of the License,
# or (at your option) any later version.
#
# BeeBEEP is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with BeeBEEP.  If not, see <http:#www.gnu.org/licenses/>.
#
# Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
#
# $Id$
#
######################################################################

BEEBEEP_VERSION=5.8.5-1535

echo "Making BeeBEEP Bundle version ${BEEBEEP_VERSION}"

SOURCE_DIR=".."
echo "Source folder:" $SOURCE_DIR

BUNDLE_APP="BeeBEEP.app"
BUNDLE_FOLDER="${BUNDLE_APP}"
echo "Bundle folder:" $BUNDLE_FOLDER

BUNDLE_DMG="BeeBEEP.dmg"
echo "Bundle:" $BUNDLE_DMG

MACDEPLOY_APP=../../Qt/latest/clang_64/bin/macdeployqt
echo "Mac Deploy App:" $MACDEPLOY_APP

# delete previous bundle folder
printf "Delete previous bundle folder ... "
rm -rf $BUNDLE_FOLDER
echo "Done"

# delete previous bundle dmg
printf "Delete previous bundle ... "
rm -f *.dmg
echo "Done"

#copy beebeep.app from release
printf "Copy beebeep.app ... "
cp -R $SOURCE_DIR/test/$BUNDLE_APP .
echo "Done"

#clean up
printf "Clean up folders and files ... "
rm -f $BUNDLE_FOLDER/Contents/Resources/beehosts.ini
rm -f $BUNDLE_FOLDER/Contents/Resources/beebeep.ini
rm -f $BUNDLE_FOLDER/Contents/Resources/beebeep.dat
rm -f $BUNDLE_FOLDER/Contents/Resources/beebeep.rc
rm -f $BUNDLE_FOLDER/Contents/Resources/qt.conf
echo "Done"

#create folders
printf "Create folders ... "
mkdir $BUNDLE_FOLDER/Contents/Frameworks
mkdir $BUNDLE_FOLDER/Contents/PlugIns
echo "Done"

#copy Info.plist
printf "Copy Info.plist ... "
cp $SOURCE_DIR/misc/Info.plist $BUNDLE_FOLDER/Contents/.
echo "Done"

#copy beep.wav
printf "Copy beep.wav ... "
cp $SOURCE_DIR/misc/beep.wav $BUNDLE_FOLDER/Contents/Resources/.
echo "Done"

#copy beehosts.ini
printf "Copy beehosts_example.ini ... "
cp $SOURCE_DIR/misc/beehosts_example.ini $BUNDLE_FOLDER/Contents/Resources/
echo "Done"

#copy beebeep.rc
printf "Copy beebeep_example.rc ... "
cp $SOURCE_DIR/misc/beebeep_example.rc $BUNDLE_FOLDER/Contents/Resources/
echo "Done"

#copy locale
printf "Copy translations ... "
cp $SOURCE_DIR/locale/*.qm $BUNDLE_FOLDER/Contents/Resources/.
printf "and removing xx locale..."
rm -f $BUNDLE_FOLDER/Contents/Resources/beebeep_xx.qm
echo "Done"

#copy plugins
printf "Copy plugins ... "
cp $SOURCE_DIR/test/*.dylib $BUNDLE_FOLDER/Contents/PlugIns/.
echo "Done"

#mac deploy frameworks
printf "MacOS X deploy and create APP file ... "
$MACDEPLOY_APP $BUNDLE_FOLDER -dmg -always-overwrite
echo "Done"

#sign the APP
printf "Sign the APP ..."
codesign --deep --timestamp --options runtime -s "Developer ID Application: Marco Mastroddi (3F9FLBSUAJ)" $BUNDLE_FOLDER
echo "Done"

#create DMG source folder
rm -rf *.dmg
printf "Create DMG ..."
rm -rf macosx_dmg
mkdir macosx_dmg
ln -s /Applications macosx_dmg
cp -a $BUNDLE_FOLDER macosx_dmg/
hdiutil create -volname "BeeBEEP ${BEEBEEP_VERSION}" -srcfolder macosx_dmg -ov -format UDZO beebeep-${BEEBEEP_VERSION}.dmg
codesign --deep --timestamp --options runtime -s "Developer ID Application: Marco Mastroddi (3F9FLBSUAJ)" beebeep-${BEEBEEP_VERSION}.dmg

printf "Making alzo ZIP version ... "
ditto -c -k --sequesterRsrc --keepParent $BUNDLE_FOLDER beebeep-${BEEBEEP_VERSION}-osx.zip
echo "Done"

echo "BeeBEEP to Bundle - Process completed."

