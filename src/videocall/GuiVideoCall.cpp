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

#include "GuiVideoCall.h"
#include "Settings.h"


GuiVideoCall::GuiVideoCall( QWidget* parent )
  : QMainWindow( parent ), m_isVideoCallActive( false )
{
}


void GuiVideoCall::startVideoCall()
{
  m_isVideoCallActive = true;
  emit videoCallStarted();
}

void GuiVideoCall::closeVideoCall()
{
  m_isVideoCallActive = false;
  emit videoCallClosed();
}


void GuiVideoCall::closeEvent( QCloseEvent* e )
{
  if( isVideoCallActive() )
  {
    if( QMessageBox::question( this, Settings::instance().programName(), tr("Do you want to end the video call?"),
                               tr( "Yes" ), tr( "No" ), QString::null, 1, 1 ) == 1 )
    {
      e->ignore();
      return;
    }
  }

  closeVideoCall();
  QMainWindow::closeEvent( e );
  e->accept();
}
