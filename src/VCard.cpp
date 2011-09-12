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

#include "VCard.h"


VCard::VCard()
  : m_nickName( "" ), m_firstName( "" ), m_lastName( "" ),
    m_birthday(), m_email( "" ), m_photo()
{}

VCard::VCard( const VCard& vc )
{
  (void)operator=( vc );
}

VCard& VCard::operator=( const VCard& vc )
{
  if( this != &vc )
  {
    m_nickName = vc.m_nickName;
    m_firstName = vc.m_firstName;
    m_lastName = vc.m_lastName;
    m_birthday = vc.m_birthday;
    m_email = vc.m_email;
    m_photo = vc.m_photo;
  }
  return *this;

}
