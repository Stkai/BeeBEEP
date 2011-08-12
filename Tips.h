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
// Author: Marco Mastroddi (marco.mastroddi(AT)gmail.com)
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_TIPS_H
#define BEEBEEP_TIPS_H

const int BeeBeepTipsSize = 4;

static const char* BeeBeepTips[ BeeBeepTipsSize ] =
{
  QT_TRANSLATE_NOOP( "Tips", "You can switch between chats with CTRL+TAB if there are new messages availables." ),
  QT_TRANSLATE_NOOP( "Tips", "If you want a <b>bold text</b> write a *bold text*." ),
  QT_TRANSLATE_NOOP( "Tips", "If you want an <i>italic text</i> write a /italic text/." ),
  QT_TRANSLATE_NOOP( "Tips", "If you want an <i>underlined text</i> write a _underlined text_." )
};

#endif // BEEBEEP_TIPS_H
