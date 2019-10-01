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

#ifndef BEEBEEP_VOICERECORDER_H
#define BEEBEEP_VOICERECORDER_H

#include "Config.h"


class VoiceRecorder : public QObject
{
  Q_OBJECT
public:
  VoiceRecorder( QObject* parent = Q_NULLPTR );

  inline const QString& currentFilePath() const;

public slots:
  void start();
  void pause();
  void resume();
  void stop();

private:
  QString m_currentFilePath;

};

// Inline Functions
inline const QString& VoiceRecorder::currentFilePath() const { return m_currentFilePath; }

#endif // BEEBEEP_VOICERECORDER_H
