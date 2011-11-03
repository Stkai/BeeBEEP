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

#ifndef BEEBEEP_GUINETWORKLOGIN_H
#define BEEBEEP_GUINETWORKLOGIN_H

#include "Config.h"
#include "NetworkAccount.h"
#include "ui_GuiNetworkLogin.h"


class GuiNetworkLogin : public QDialog, private Ui::GuiNetworkLogin
{
  Q_OBJECT

public:
  explicit GuiNetworkLogin( QWidget *parent );

  void setNetworkAccount( const NetworkAccount&, const QString& network_service );
  inline const NetworkAccount& account() const;

protected slots:
  void doLogin();

private:
  NetworkAccount m_account;

};


// Inline Functions
inline const NetworkAccount& GuiNetworkLogin::account() const { return m_account; }

#endif // BEEBEEP_GUINETWORKLOGIN_H
