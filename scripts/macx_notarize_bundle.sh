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

response_tmp_file="beebeep_notarization_response.txt"

dmg_files=`ls ./*.dmg`
for dmg_files_tmp in $dmg_files
do
   dmg_file=$dmg_files_tmp
   break
done

if [ -z $dmg_file ]; then
  echo "DMG file not found."
  exit
fi

rm -f $response_tmp_file

echo "Making notarization request for file $dmg_file ..."
xcrun altool --notarize-app --primary-bundle-id "net.beebeep.app" --username "marco.mastroddi@gmail.com" --password "@keychain:BeeBEEP_DEV_PW" --file $dmg_file &> $response_tmp_file
echo "Done."

uuid=`cat $response_tmp_file | grep -Eo '\w{8}-(\w{4}-){3}\w{12}$'`
echo "Response UUID: $uuid"
rm -f $response_tmp_file

while true; do
  echo "Checking for notarization ..."
 
  xcrun altool --notarization-info "$uuid" --username "marco.mastroddi@gmail.com" --password "@keychain:BeeBEEP_DEV_PW" &> $response_tmp_file
    
  r=`cat $response_tmp_file`
  rm -f $response_tmp_file
  t=`echo "$r" | grep "success"`
  f=`echo "$r" | grep "invalid"`
  if [[ "$t" != "" ]]; then
    echo "Notarization done."
    xcrun stapler staple $dmg_file
    echo "Stapler done."
    break
  fi
  if [[ "$f" != "" ]]; then
    echo "$r"
    return 1
  fi
  echo "... not finish yet, sleep 2m then check again..."
  sleep 120
done

echo "Done."

