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

#ifndef BEEBEEP_HISTORYMANAGER_H
#define BEEBEEP_HISTORYMANAGER_H

#include "HistoryMessage.h"


class HistoryManager
{
// Singleton Object
  static HistoryManager* mp_instance;

public:
  bool moveHistoryUp();
  bool moveHistoryDown();
  inline int historySize() const;
  inline void setTemporaryMessage( const HistoryMessage& );
  inline bool hasTemporaryMessage() const;
  inline void clearTemporaryMessage();
  void addMessage( const HistoryMessage& );

  HistoryMessage message() const;

  static HistoryManager& instance()
  {
    if( !mp_instance )
      mp_instance = new HistoryManager();
    return *mp_instance;
  }

  static void close()
  {
    if( mp_instance )
    {
      delete mp_instance;
      mp_instance = NULL;
    }
  }

protected:
  HistoryManager();
  inline int minIndex() const;
  inline int maxIndex() const;

private:
  QList<HistoryMessage> m_history;
  int m_index;
  HistoryMessage m_temporayMessage;

};

// Inline Functions
inline int HistoryManager::historySize() const { return m_history.size(); }
inline void HistoryManager::setTemporaryMessage( const HistoryMessage& new_value ) { m_temporayMessage = new_value; }
inline bool HistoryManager::hasTemporaryMessage() const { return !m_temporayMessage.isEmpty(); }
inline void HistoryManager::clearTemporaryMessage() { m_temporayMessage.clear(); }
inline int HistoryManager::minIndex() const { return 0; }
inline int HistoryManager::maxIndex() const { return m_history.size() - 1; }

#endif // BEEBEEP_HISTORYMANAGER_H
