//////////////////////////////////////////////////////////////////////
//
// This file is part of BeeBEEP.
//
// BeeBEEP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.
//
// BeeBEEP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with BeeBEEP.  If not, see <http://www.gnu.org/licenses/>.
//
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_VERSION_H
#define BEEBEEP_VERSION_H

const char BEEBEEP_NAME[] = "BeeBEEP";
const char BEEBEEP_ORGANIZATION[] = "MarcoMastroddiSW";
const char BEEBEEP_ORGANIZATION_DOMAIN[] = "beebeep.net";
const char BEEBEEP_DNS_RECORD[] = "_beebeep._tcp";
#ifdef BEEBEEP_DEBUG
  const char BEEBEEP_WEBSITE[] = "http://127.0.0.1/beebeep.net/";
#else
  const char BEEBEEP_WEBSITE[] = "https://www.beebeep.net/";
#endif
const char BEEBEEP_DOWNLOAD_WEBSITE[] = "download.php";
const char BEEBEEP_DONATE_WEBSITE[] = "donate.php";
const char BEEBEEP_HELP_WEBSITE[] = "help.php";
const char BEEBEEP_LANGUAGE_WEBSITE[] = "language.php";
const char BEEBEEP_NEWS_WEBSITE[] = "news.php";
const char BEEBEEP_CHECK_VERSION_WEBSITE[] = "check_version.php";
const char BEEBEEP_LAST_VERSION_FILE[] = "last_version.php";
const char BEEBEEP_GA_TRACKING_ID[] = "UA-57878696-1";  // VUEtNTc4Nzg2OTYtMQ==
const char BEEBEEP_GA_URL[] = "https://www.google-analytics.com/collect";
const char BEEBEEP_GA_EVENT_VERSION[] = "1";
const char HUNSPELL_VERSION[] = "1.6.1";
const char BEEBEEP_VERSION[] = "5.6.1";
const int BEEBEEP_PROTO_VERSION = 74;
const int BEEBEEP_SETTINGS_VERSION = 11;
const int BEEBEEP_BUILD = 1107;

#endif // BEEBEEP_VERSION_H

