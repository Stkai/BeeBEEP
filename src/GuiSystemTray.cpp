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
// $Id: GuiTransferFile.cpp 213 2013-03-10 21:41:40Z mastroddi $
//
//////////////////////////////////////////////////////////////////////

#include "GuiSystemTray.h"


GuiSystemTray::GuiSystemTray( QObject *parent )
  : QSystemTrayIcon( parent ), m_iconStatus( -1 ), m_unreadMessages( 0 )
{
  setDefaultIcon();
}

void GuiSystemTray::addUnreadMessage( int ur)
{
  m_unreadMessages += ur;
  showIcon();
}

void GuiSystemTray::setUnreadMessages( int ur )
{
  m_unreadMessages = ur;
  showIcon();
}

void GuiSystemTray::setDefaultIcon()
{
  if( m_iconStatus != GuiSystemTray::Default )
  {
    setIcon( QIcon( ":/images/beebeep.png" ) );
    m_iconStatus = GuiSystemTray::Default;
  }
}

void GuiSystemTray::setMessageIcon()
{
  if( m_iconStatus != GuiSystemTray::Message )
  {
    setIcon( QIcon( ":/images/beebeep-message.png" ) );
    m_iconStatus = GuiSystemTray::Default;
  }
}

void GuiSystemTray::showIcon()
{
  if( m_unreadMessages > 0 )
    setMessageIcon();
  else
    setDefaultIcon();
}
