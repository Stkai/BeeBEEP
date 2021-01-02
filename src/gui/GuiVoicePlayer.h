//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2021 Marco Mastroddi
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
// along with BeeBEEP. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_GUIVOICEPLAYER_H
#define BEEBEEP_GUIVOICEPLAYER_H

#include "Config.h"
#include "ui_GuiVoicePlayer.h"


class GuiVoicePlayer : public QWidget, private Ui::GuiVoicePlayerWidget
{
  Q_OBJECT

public:
  explicit GuiVoicePlayer( QWidget *parent = Q_NULLPTR );

public slots:
  void setFilePath( const QString&, VNumber );

protected slots:
  void onPlayClicked();
  void onSliderPositionPressed();
  void onSliderPositionMoved( int );
  void onSliderPositionReleased();
  void onPlaying( const QString&, VNumber );
  void onPaused( const QString&, VNumber );
  void onFinished( const QString&, VNumber );
  void setFileDuration( const QString&, VNumber, qint64 );
  void setFilePosition( const QString&, VNumber, qint64 );
  void onCloseClicked();

protected:
  void setPaused( bool );

private:
  QString m_filePath;
  VNumber m_chatId;

};


#endif // BEEBEEP_GUIVOICEPLAYER_H
