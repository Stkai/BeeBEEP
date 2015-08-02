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

#include "EmoticonManager.h"


EmoticonManager* EmoticonManager::mp_instance = NULL;


EmoticonManager::EmoticonManager()
  : m_emoticons(), m_maxTextSize( 2 )
#ifdef BEEBEEP_USE_EMOJI
  , m_emojis()
#endif
{
  addEmoticon( ":)", "smile" );
  addEmoticon( ":-)", "smile" );
  addEmoticon( ":p", "tongue" );
  addEmoticon( ":-p", "tongue" );
  addEmoticon( ":P", "big-tongue" );
  addEmoticon( ":-P", "big-tongue" );
  addEmoticon( ":'", "cry" );
  addEmoticon( ":'(", "cry" );
  addEmoticon( ":D", "laugh" );
  addEmoticon( ":-D", "laugh" );
  addEmoticon( ":@", "angry" );
  addEmoticon( ":x", "kiss" );
  addEmoticon( ":*", "big-kiss" );
  addEmoticon( ":-*", "big-kiss" );
  addEmoticon( ":z", "sleep" );
  addEmoticon( ":o", "surprise" );
  addEmoticon( ":O", "big-surprise" );
  addEmoticon( ":|", "pouty" );
  addEmoticon( ":L", "love" );
  addEmoticon( ":w", "whistle" );
  addEmoticon( ":$", "bandit" );
  addEmoticon( ":!", "wizard" );
  addEmoticon( ";)", "wink" );
  addEmoticon( ";-)", "wink" );
  addEmoticon( ":(", "disappointed" );
  addEmoticon( ":-(", "disappointed" );
  addEmoticon( ":T", "slurp" );
  addEmoticon( ":%", "bazinga" );
  addEmoticon( "B)", "cool" );
  addEmoticon( "B-)", "cool" );
  addEmoticon( "<3", "heart" );
  addEmoticon( "&lt;3", "heart" );  // for html
  addEmoticon( "</3", "heart-broken" );
  addEmoticon( "&lt;/3", "heart-broken" ); // for html
  addEmoticon( "=)", "sideways" );
  addEmoticon( "}:)", "devil" );
  addEmoticon( "o:)", "angel" );
  addEmoticon( "x(", "sick" );
  addEmoticon( "x-(", "sick" );
  addEmoticon( "X|", "pinched" );
  addEmoticon( "X-|", "pinched" );
  addEmoticon( "^_^", "happy" );
  addEmoticon( "o.O", "confused" );

#ifdef BEEBEEP_USE_EMOJI
#ifdef BEEBEEP_DEBUG
  createEmojiFile();
#endif
  addEmoticon( "✂", "2702", Emoticon::Objects );
  addEmoticon( "✅", "2705", Emoticon::Symbols );
  addEmoticon( "✈", "2708", Emoticon::Places );
  addEmoticon( "✉", "2709", Emoticon::Objects );
  addEmoticon( "✏", "270f", Emoticon::Objects );
  addEmoticon( "✒", "2712", Emoticon::Objects );
  addEmoticon( "✔", "2714", Emoticon::Symbols );
  addEmoticon( "✖", "2716", Emoticon::Symbols );
  addEmoticon( "✳", "2733", Emoticon::Symbols );
  addEmoticon( "✴", "2734", Emoticon::Symbols );
  addEmoticon( "❄", "2744", Emoticon::Nature );
  addEmoticon( "❇", "2747", Emoticon::Symbols );
  addEmoticon( "❌", "274c", Emoticon::Symbols );
  addEmoticon( "❎", "274e", Emoticon::Symbols );
  addEmoticon( "❓", "2753", Emoticon::Symbols );
  addEmoticon( "❔", "2754", Emoticon::Symbols );
  addEmoticon( "❕", "2755", Emoticon::Symbols );
  addEmoticon( "❗", "2757", Emoticon::Symbols );
  addEmoticon( "➕", "2795", Emoticon::Symbols );
  addEmoticon( "➖", "2796", Emoticon::Symbols );
  addEmoticon( "➗", "2797", Emoticon::Symbols );
  addEmoticon( "➡", "27a1", Emoticon::Symbols );
  addEmoticon( "➰", "27b0", Emoticon::Symbols );
  addEmoticon( "🚩", "1f6a9", Emoticon::Places );
  addEmoticon( "🚪", "1f6aa", Emoticon::Objects );
  addEmoticon( "🚫", "1f6ab", Emoticon::Symbols );
  addEmoticon( "⛔", "26d4", Emoticon::Symbols );
  addEmoticon( "🚬", "1f6ac", Emoticon::Objects );
  addEmoticon( "🚭", "1f6ad", Emoticon::Symbols );
  addEmoticon( "🚹", "1f6b9", Emoticon::Symbols );
  addEmoticon( "🚺", "1f6ba", Emoticon::Symbols );
  addEmoticon( "🚻", "1f6bb", Emoticon::Symbols );
  addEmoticon( "🚼", "1f6bc", Emoticon::Symbols );
  addEmoticon( "🚽", "1f6bd", Emoticon::Objects );
  addEmoticon( "🚾", "1f6be", Emoticon::Symbols );
  addEmoticon( "🛀", "1f6c0", Emoticon::Objects );
  addEmoticon( "⤴", "2934", Emoticon::Symbols );
  addEmoticon( "⤵", "2935", Emoticon::Symbols );
  addEmoticon( "⬅", "2b05", Emoticon::Symbols );
  addEmoticon( "⬆", "2b06", Emoticon::Symbols );
  addEmoticon( "⬇", "2b07", Emoticon::Symbols );
  addEmoticon( "🔀", "1f500", Emoticon::Symbols );
  addEmoticon( "🔁", "1f501", Emoticon::Symbols );
  addEmoticon( "🔂", "1f502", Emoticon::Symbols );
  addEmoticon( "🔄", "1f504", Emoticon::Symbols );
  addEmoticon( "⌚", "231a", Emoticon::Objects );
  addEmoticon( "⌛", "231b", Emoticon::Objects );
  addEmoticon( "⏰", "23f0", Emoticon::Objects );
  addEmoticon( "⏳", "23f3", Emoticon::Objects );
  addEmoticon( "☀", "2600", Emoticon::Nature );
  addEmoticon( "☁", "2601", Emoticon::Nature );
  addEmoticon( "☎", "260e", Emoticon::Objects );
  addEmoticon( "☔", "2614", Emoticon::Nature );
  addEmoticon( "☕", "2615", Emoticon::Objects );
  addEmoticon( "♈", "2648", Emoticon::Symbols );
  addEmoticon( "♉", "2649", Emoticon::Symbols );
  addEmoticon( "♊", "264a", Emoticon::Symbols );
  addEmoticon( "♋", "264b", Emoticon::Symbols );
  addEmoticon( "♌", "264c", Emoticon::Symbols );
  addEmoticon( "♍", "264d", Emoticon::Symbols );
  addEmoticon( "♎", "264e", Emoticon::Symbols );
  addEmoticon( "♏", "264f", Emoticon::Symbols );
  addEmoticon( "♐", "2650", Emoticon::Symbols );
  addEmoticon( "♑", "2651", Emoticon::Symbols );
  addEmoticon( "♒", "2652", Emoticon::Symbols );
  addEmoticon( "♓", "2653", Emoticon::Symbols );
  addEmoticon( "♠", "2660", Emoticon::Symbols );
  addEmoticon( "♣", "2663", Emoticon::Symbols );
  addEmoticon( "♥", "2665", Emoticon::Symbols );
  addEmoticon( "♦", "2666", Emoticon::Symbols );
  addEmoticon( "♨", "2668", Emoticon::Symbols );
  addEmoticon( "♻", "267b", Emoticon::Symbols );
  addEmoticon( "♿", "267f", Emoticon::Symbols );
  addEmoticon( "⚓", "2693", Emoticon::Objects );
  addEmoticon( "⚠", "26a0", Emoticon::Objects );
  addEmoticon( "⚡", "26a1", Emoticon::Nature );
  addEmoticon( "⚽", "26bd", Emoticon::Objects );
  addEmoticon( "⚾", "26be", Emoticon::Objects );
  addEmoticon( "⛄", "26c4", Emoticon::Nature );
  addEmoticon( "⛅", "26c5", Emoticon::Nature );
  addEmoticon( "⛪", "26ea", Emoticon::Places );
  addEmoticon( "⛲", "26f2", Emoticon::Places );
  addEmoticon( "⛳", "26f3", Emoticon::Places );
  addEmoticon( "⛵", "26f5", Emoticon::Places );
  addEmoticon( "⛺", "26fa", Emoticon::Places );
  addEmoticon( "⛽", "26fd", Emoticon::Places );
  addEmoticon( "🚲", "1f6b2", Emoticon::Places );
  addEmoticon( "🚀", "1f680", Emoticon::Places );
  addEmoticon( "🚃", "1f683", Emoticon::Places );
  addEmoticon( "🚄", "1f684", Emoticon::Places );
  addEmoticon( "🚅", "1f685", Emoticon::Places );
  addEmoticon( "🚇", "1f687", Emoticon::Places );
  addEmoticon( "🚉", "1f689", Emoticon::Places );
  addEmoticon( "🚌", "1f68c", Emoticon::Places );
  addEmoticon( "🚏", "1f68f", Emoticon::Places );
  addEmoticon( "🚑", "1f691", Emoticon::Places );
  addEmoticon( "🚒", "1f692", Emoticon::Places );
  addEmoticon( "🚓", "1f693", Emoticon::Places );
  addEmoticon( "🚕", "1f695", Emoticon::Places );
  addEmoticon( "🚗", "1f697", Emoticon::Places );
  addEmoticon( "🚙", "1f699", Emoticon::Places );
  addEmoticon( "🚚", "1f69a", Emoticon::Places );
  addEmoticon( "🚢", "1f6a2", Emoticon::Places );
  addEmoticon( "🚤", "1f6a4", Emoticon::Places );
  addEmoticon( "🚥", "1f6a5", Emoticon::Places );
  addEmoticon( "🚦", "1f6a6", Emoticon::Places );
  addEmoticon( "🚨", "1f6a8", Emoticon::Places );
  addEmoticon( "🚁", "1f681", Emoticon::Places );
  addEmoticon( "🚂", "1f682", Emoticon::Places );
  addEmoticon( "🚆", "1f686", Emoticon::Places );
  addEmoticon( "🚈", "1f688", Emoticon::Places );
  addEmoticon( "🚊", "1f68a", Emoticon::Places );
  addEmoticon( "🚍", "1f68d", Emoticon::Places );
  addEmoticon( "🚎", "1f68e", Emoticon::Places );
  addEmoticon( "🚐", "1f690", Emoticon::Places );
  addEmoticon( "🚔", "1f694", Emoticon::Places );
  addEmoticon( "🚖", "1f696", Emoticon::Places );
  addEmoticon( "🚘", "1f698", Emoticon::Places );
  addEmoticon( "🚛", "1f69b", Emoticon::Places );
  addEmoticon( "🚜", "1f69c", Emoticon::Places );
  addEmoticon( "🚝", "1f69d", Emoticon::Places );
  addEmoticon( "🚞", "1f69e", Emoticon::Places );
  addEmoticon( "🚟", "1f69f", Emoticon::Places );
  addEmoticon( "🚠", "1f6a0", Emoticon::Places );
  addEmoticon( "🚡", "1f6a1", Emoticon::Places );
  addEmoticon( "🚣", "1f6a3", Emoticon::Places );
  addEmoticon( "⭐", "2b50", Emoticon::Nature );
  addEmoticon( "⭕", "2b55", Emoticon::Symbols );
  addEmoticon( "〰", "3030", Emoticon::Symbols );
  addEmoticon( "〽", "303d", Emoticon::Symbols );
  addEmoticon( "㊗", "3297", Emoticon::Symbols );
  addEmoticon( "㊙", "3299", Emoticon::Symbols );
  addEmoticon( "🀄", "1f004", Emoticon::Symbols );
  addEmoticon( "🃏", "1f0cf", Emoticon::Symbols );
  addEmoticon( "🌀", "1f300", Emoticon::Nature );
  addEmoticon( "🌁", "1f301", Emoticon::Nature );
  addEmoticon( "🌂", "1f302", Emoticon::Objects );
  addEmoticon( "🌃", "1f303", Emoticon::Places );
  addEmoticon( "🌄", "1f304", Emoticon::Places );
  addEmoticon( "🌅", "1f305", Emoticon::Places );
  addEmoticon( "🌆", "1f306", Emoticon::Places );
  addEmoticon( "🌇", "1f307", Emoticon::Places );
  addEmoticon( "🌈", "1f308", Emoticon::Nature );
  addEmoticon( "🌉", "1f309", Emoticon::Places );
  addEmoticon( "🌊", "1f30a", Emoticon::Nature );
  addEmoticon( "🌋", "1f30b", Emoticon::Nature );
  addEmoticon( "🌌", "1f30c", Emoticon::Nature );
  addEmoticon( "🌰", "1f330", Emoticon::Nature );
  addEmoticon( "🌱", "1f331", Emoticon::Nature );
  addEmoticon( "🌴", "1f334", Emoticon::Nature );
  addEmoticon( "🌵", "1f335", Emoticon::Nature );
  addEmoticon( "🌷", "1f337", Emoticon::Nature );
  addEmoticon( "🌸", "1f338", Emoticon::Nature );
  addEmoticon( "🌹", "1f339", Emoticon::Nature );
  addEmoticon( "🌺", "1f33a", Emoticon::Nature );
  addEmoticon( "🌻", "1f33b", Emoticon::Nature );
  addEmoticon( "🌼", "1f33c", Emoticon::Nature );
  addEmoticon( "🌽", "1f33d", Emoticon::Nature );
  addEmoticon( "🌾", "1f33e", Emoticon::Nature );
  addEmoticon( "🌿", "1f33f", Emoticon::Nature );
  addEmoticon( "🍀", "1f340", Emoticon::Nature );
  addEmoticon( "🍁", "1f341", Emoticon::Nature );
  addEmoticon( "🍂", "1f342", Emoticon::Nature );
  addEmoticon( "🍃", "1f343", Emoticon::Nature );
  addEmoticon( "🍄", "1f344", Emoticon::Objects );
  addEmoticon( "🍅", "1f345", Emoticon::Objects );
  addEmoticon( "🍆", "1f346", Emoticon::Objects );
  addEmoticon( "🍇", "1f347", Emoticon::Objects );
  addEmoticon( "🍈", "1f348", Emoticon::Objects );
  addEmoticon( "🍉", "1f349", Emoticon::Objects );
  addEmoticon( "🍊", "1f34a", Emoticon::Objects );
  addEmoticon( "🍌", "1f34c", Emoticon::Objects );
  addEmoticon( "🍍", "1f34d", Emoticon::Objects );
  addEmoticon( "🍎", "1f34e", Emoticon::Objects );
  addEmoticon( "🍏", "1f34f", Emoticon::Objects );
  addEmoticon( "🍑", "1f351", Emoticon::Objects );
  addEmoticon( "🍒", "1f352", Emoticon::Objects );
  addEmoticon( "🍓", "1f353", Emoticon::Objects );
  addEmoticon( "🍔", "1f354", Emoticon::Objects );
  addEmoticon( "🍕", "1f355", Emoticon::Objects );
  addEmoticon( "🍖", "1f356", Emoticon::Objects );
  addEmoticon( "🍗", "1f357", Emoticon::Objects );
  addEmoticon( "🍘", "1f358", Emoticon::Objects );
  addEmoticon( "🍙", "1f359", Emoticon::Objects );
  addEmoticon( "🍚", "1f35a", Emoticon::Objects );
  addEmoticon( "🍛", "1f35b", Emoticon::Objects );
  addEmoticon( "🍜", "1f35c", Emoticon::Objects );
  addEmoticon( "🍝", "1f35d", Emoticon::Objects );
  addEmoticon( "🍞", "1f35e", Emoticon::Objects );
  addEmoticon( "🍟", "1f35f", Emoticon::Objects );
  addEmoticon( "🍠", "1f360", Emoticon::Objects );
  addEmoticon( "🍡", "1f361", Emoticon::Objects );
  addEmoticon( "🍢", "1f362", Emoticon::Objects );
  addEmoticon( "🍣", "1f363", Emoticon::Objects );
  addEmoticon( "🍤", "1f364", Emoticon::Objects );
  addEmoticon( "🍥", "1f365", Emoticon::Objects );
  addEmoticon( "🍦", "1f366", Emoticon::Objects );
  addEmoticon( "🍧", "1f367", Emoticon::Objects );
  addEmoticon( "🍨", "1f368", Emoticon::Objects );
  addEmoticon( "🍩", "1f369", Emoticon::Objects );
  addEmoticon( "🍪", "1f36a", Emoticon::Objects );
  addEmoticon( "🍫", "1f36b", Emoticon::Objects );
  addEmoticon( "🍬", "1f36c", Emoticon::Objects );
  addEmoticon( "🍭", "1f36d", Emoticon::Objects );
  addEmoticon( "🍮", "1f36e", Emoticon::Objects );
  addEmoticon( "🍯", "1f36f", Emoticon::Objects );
  addEmoticon( "🍰", "1f370", Emoticon::Objects );
  addEmoticon( "🍱", "1f371", Emoticon::Objects );
  addEmoticon( "🍲", "1f372", Emoticon::Objects );
  addEmoticon( "🍳", "1f373", Emoticon::Objects );
  addEmoticon( "🍴", "1f374", Emoticon::Objects );
  addEmoticon( "🍵", "1f375", Emoticon::Objects );
  addEmoticon( "🍶", "1f376", Emoticon::Objects );
  addEmoticon( "🍷", "1f377", Emoticon::Objects );
  addEmoticon( "🍸", "1f378", Emoticon::Objects );
  addEmoticon( "🍹", "1f379", Emoticon::Objects );
  addEmoticon( "🍺", "1f37a", Emoticon::Objects );
  addEmoticon( "🍻", "1f37b", Emoticon::Objects );
  addEmoticon( "🎀", "1f380", Emoticon::Objects );
  addEmoticon( "🎁", "1f381", Emoticon::Objects );
  addEmoticon( "🎂", "1f382", Emoticon::Objects );
  addEmoticon( "🎃", "1f383", Emoticon::Objects );
  addEmoticon( "🎄", "1f384", Emoticon::Objects );
  addEmoticon( "🎅", "1f385", Emoticon::Objects );
  addEmoticon( "🎆", "1f386", Emoticon::Objects );
  addEmoticon( "🎇", "1f387", Emoticon::Objects );
  addEmoticon( "🎈", "1f388", Emoticon::Objects );
  addEmoticon( "🎉", "1f389", Emoticon::Objects );
  addEmoticon( "🎊", "1f38a", Emoticon::Objects );
  addEmoticon( "🎋", "1f38b", Emoticon::Objects );
  addEmoticon( "🎌", "1f38c", Emoticon::Objects );
  addEmoticon( "🎍", "1f38d", Emoticon::Objects );
  addEmoticon( "🎎", "1f38e", Emoticon::Objects );
  addEmoticon( "🎏", "1f38f", Emoticon::Objects );
  addEmoticon( "🎐", "1f390", Emoticon::Objects );
  addEmoticon( "🎑", "1f391", Emoticon::Objects );
  addEmoticon( "🎒", "1f392", Emoticon::Objects );
  addEmoticon( "🎓", "1f393", Emoticon::Objects );
  addEmoticon( "🎠", "1f3a0", Emoticon::Places );
  addEmoticon( "🎡", "1f3a1", Emoticon::Places );
  addEmoticon( "🎢", "1f3a2", Emoticon::Places );
  addEmoticon( "🎣", "1f3a3", Emoticon::Objects );
  addEmoticon( "🎤", "1f3a4", Emoticon::Objects );
  addEmoticon( "🎥", "1f3a5", Emoticon::Objects );
  addEmoticon( "🎦", "1f3a6", Emoticon::Symbols );
  addEmoticon( "🎧", "1f3a7", Emoticon::Objects );
  addEmoticon( "🎨", "1f3a8", Emoticon::Objects );
  addEmoticon( "🎩", "1f3a9", Emoticon::Objects );
  addEmoticon( "🎪", "1f3aa", Emoticon::Places );
  addEmoticon( "🎫", "1f3ab", Emoticon::Objects );
  addEmoticon( "🎬", "1f3ac", Emoticon::Objects );
  addEmoticon( "🎭", "1f3ad", Emoticon::Objects );
  addEmoticon( "🎮", "1f3ae", Emoticon::Objects );
  addEmoticon( "🎯", "1f3af", Emoticon::Objects );
  addEmoticon( "🎰", "1f3b0", Emoticon::Objects );
  addEmoticon( "🎱", "1f3b1", Emoticon::Objects );
  addEmoticon( "🎲", "1f3b2", Emoticon::Objects );
  addEmoticon( "🎳", "1f3b3", Emoticon::Objects );
  addEmoticon( "🎴", "1f3b4", Emoticon::Objects );
  addEmoticon( "🎵", "1f3b5", Emoticon::Objects );
  addEmoticon( "🎶", "1f3b6", Emoticon::Objects );
  addEmoticon( "🎷", "1f3b7", Emoticon::Objects );
  addEmoticon( "🎸", "1f3b8", Emoticon::Objects );
  addEmoticon( "🎼", "1f3bc", Emoticon::Objects );
  addEmoticon( "🎾", "1f3be", Emoticon::Objects );
  addEmoticon( "🎿", "1f3bf", Emoticon::Objects );
  addEmoticon( "🏁", "1f3c1", Emoticon::Objects );
  addEmoticon( "🏂", "1f3c2", Emoticon::Objects );
  addEmoticon( "🏄", "1f3c4", Emoticon::Objects );
  addEmoticon( "🏆", "1f3c6", Emoticon::Objects );
  addEmoticon( "🏈", "1f3c8", Emoticon::Objects );
  addEmoticon( "🏊", "1f3ca", Emoticon::Objects );
  addEmoticon( "🏠", "1f3e0", Emoticon::Places );
  addEmoticon( "🏡", "1f3e1", Emoticon::Places );
  addEmoticon( "🏢", "1f3e2", Emoticon::Places );
  addEmoticon( "🏣", "1f3e3", Emoticon::Places );
  addEmoticon( "🏥", "1f3e5", Emoticon::Places );
  addEmoticon( "🏦", "1f3e6", Emoticon::Places );
  addEmoticon( "🏧", "1f3e7", Emoticon::Places );
  addEmoticon( "🏨", "1f3e8", Emoticon::Places );
  addEmoticon( "🏩", "1f3e9", Emoticon::Places );
  addEmoticon( "🏪", "1f3ea", Emoticon::Places );
  addEmoticon( "🏫", "1f3eb", Emoticon::Places );
  addEmoticon( "🏬", "1f3ec", Emoticon::Places );
  addEmoticon( "🏭", "1f3ed", Emoticon::Places );
  addEmoticon( "🏮", "1f3ee", Emoticon::Places );
  addEmoticon( "🏯", "1f3ef", Emoticon::Places );
  addEmoticon( "🏰", "1f3f0", Emoticon::Places );
  addEmoticon( "🐌", "1f40c", Emoticon::Nature );
  addEmoticon( "🐍", "1f40d", Emoticon::Nature );
  addEmoticon( "🐎", "1f40e", Emoticon::Nature );
  addEmoticon( "🐑", "1f411", Emoticon::Nature );
  addEmoticon( "🐒", "1f412", Emoticon::Nature );
  addEmoticon( "🐔", "1f414", Emoticon::Nature );
  addEmoticon( "🐗", "1f417", Emoticon::Nature );
  addEmoticon( "🐘", "1f418", Emoticon::Nature );
  addEmoticon( "🐙", "1f419", Emoticon::Nature );
  addEmoticon( "🐚", "1f41a", Emoticon::Nature );
  addEmoticon( "🐛", "1f41b", Emoticon::Nature );
  addEmoticon( "🐜", "1f41c", Emoticon::Nature );
  addEmoticon( "🐝", "1f41d", Emoticon::Nature );
  addEmoticon( "🐞", "1f41e", Emoticon::Nature );
  addEmoticon( "🐟", "1f41f", Emoticon::Nature );
  addEmoticon( "🐠", "1f420", Emoticon::Nature );
  addEmoticon( "🐡", "1f421", Emoticon::Nature );
  addEmoticon( "🐢", "1f422", Emoticon::Nature );
  addEmoticon( "🐣", "1f423", Emoticon::Nature );
  addEmoticon( "🐦", "1f426", Emoticon::Nature );
  addEmoticon( "🐧", "1f427", Emoticon::Nature );
  addEmoticon( "🐨", "1f428", Emoticon::Nature );
  addEmoticon( "🐩", "1f429", Emoticon::Nature );
  addEmoticon( "🐫", "1f42b", Emoticon::Nature );
  addEmoticon( "🐬", "1f42c", Emoticon::Nature );
  addEmoticon( "🐭", "1f42d", Emoticon::Nature );
  addEmoticon( "🐮", "1f42e", Emoticon::Nature );
  addEmoticon( "🐯", "1f42f", Emoticon::Nature );
  addEmoticon( "🐰", "1f430", Emoticon::Nature );
  addEmoticon( "🐱", "1f431", Emoticon::Nature );
  addEmoticon( "🐲", "1f432", Emoticon::Nature );
  addEmoticon( "🐳", "1f433", Emoticon::Nature );
  addEmoticon( "🐴", "1f434", Emoticon::Nature );
  addEmoticon( "🐵", "1f435", Emoticon::Nature );
  addEmoticon( "🐶", "1f436", Emoticon::Nature );
  addEmoticon( "🐷", "1f437", Emoticon::Nature );
  addEmoticon( "🐸", "1f438", Emoticon::Nature );
  addEmoticon( "🐹", "1f439", Emoticon::Nature );
  addEmoticon( "🐺", "1f43a", Emoticon::Nature );
  addEmoticon( "🐻", "1f43b", Emoticon::Nature );
  addEmoticon( "🐼", "1f43c", Emoticon::Nature );
  addEmoticon( "🐽", "1f43d", Emoticon::Nature );
  addEmoticon( "🐾", "1f43e", Emoticon::Nature );
  addEmoticon( "👻", "1f47b", Emoticon::Objects );
  addEmoticon( "👾", "1f47e", Emoticon::Objects );
  addEmoticon( "💈", "1f488", Emoticon::Places );
  addEmoticon( "💉", "1f489", Emoticon::Objects );
  addEmoticon( "💊", "1f48a", Emoticon::Objects );
  addEmoticon( "💐", "1f490", Emoticon::Nature );
  addEmoticon( "💒", "1f492", Emoticon::Places );
  addEmoticon( "💟", "1f49f", Emoticon::Symbols );
  addEmoticon( "💠", "1f4a0", Emoticon::Symbols );
  addEmoticon( "💡", "1f4a1", Emoticon::Objects );
  addEmoticon( "💢", "1f4a2", Emoticon::Symbols );
  addEmoticon( "💣", "1f4a3", Emoticon::Objects );
  addEmoticon( "💮", "1f4ae", Emoticon::Symbols );
  addEmoticon( "💯", "1f4af", Emoticon::Symbols );
  addEmoticon( "💰", "1f4b0", Emoticon::Objects );
  addEmoticon( "💱", "1f4b1", Emoticon::Symbols );
  addEmoticon( "💲", "1f4b2", Emoticon::Symbols );
  addEmoticon( "💳", "1f4b3", Emoticon::Objects );
  addEmoticon( "💴", "1f4b4", Emoticon::Objects );
  addEmoticon( "💵", "1f4b5", Emoticon::Objects );
  addEmoticon( "💸", "1f4b8", Emoticon::Objects );
  addEmoticon( "💺", "1f4ba", Emoticon::Objects );
  addEmoticon( "💻", "1f4bb", Emoticon::Objects );
  addEmoticon( "💼", "1f4bc", Emoticon::Objects );
  addEmoticon( "💽", "1f4bd", Emoticon::Objects );
  addEmoticon( "💾", "1f4be", Emoticon::Objects );
  addEmoticon( "💿", "1f4bf", Emoticon::Objects );
  addEmoticon( "📀", "1f4c0", Emoticon::Objects );
  addEmoticon( "📁", "1f4c1", Emoticon::Objects );
  addEmoticon( "📂", "1f4c2", Emoticon::Objects );
  addEmoticon( "📃", "1f4c3", Emoticon::Objects );
  addEmoticon( "📄", "1f4c4", Emoticon::Objects );
  addEmoticon( "📅", "1f4c5", Emoticon::Objects );
  addEmoticon( "📆", "1f4c6", Emoticon::Objects );
  addEmoticon( "📇", "1f4c7", Emoticon::Objects );
  addEmoticon( "📈", "1f4c8", Emoticon::Objects );
  addEmoticon( "📉", "1f4c9", Emoticon::Objects );
  addEmoticon( "📊", "1f4ca", Emoticon::Objects );
  addEmoticon( "📋", "1f4cb", Emoticon::Objects );
  addEmoticon( "📌", "1f4cc", Emoticon::Objects );
  addEmoticon( "📍", "1f4cd", Emoticon::Objects );
  addEmoticon( "📎", "1f4ce", Emoticon::Objects );
  addEmoticon( "📏", "1f4cf", Emoticon::Objects );
  addEmoticon( "📐", "1f4d0", Emoticon::Objects );
  addEmoticon( "📑", "1f4d1", Emoticon::Objects );
  addEmoticon( "📒", "1f4d2", Emoticon::Objects );
  addEmoticon( "📓", "1f4d3", Emoticon::Objects );
  addEmoticon( "📔", "1f4d4", Emoticon::Objects );
  addEmoticon( "📕", "1f4d5", Emoticon::Objects );
  addEmoticon( "📖", "1f4d6", Emoticon::Objects );
  addEmoticon( "📗", "1f4d7", Emoticon::Objects );
  addEmoticon( "📘", "1f4d8", Emoticon::Objects );
  addEmoticon( "📙", "1f4d9", Emoticon::Objects );
  addEmoticon( "📚", "1f4da", Emoticon::Objects );
  addEmoticon( "📛", "1f4db", Emoticon::Objects );
  addEmoticon( "📜", "1f4dc", Emoticon::Objects );
  addEmoticon( "📝", "1f4dd", Emoticon::Objects );
  addEmoticon( "📞", "1f4de", Emoticon::Objects );
  addEmoticon( "📟", "1f4df", Emoticon::Objects );
  addEmoticon( "📠", "1f4e0", Emoticon::Objects );
  addEmoticon( "📡", "1f4e1", Emoticon::Objects );
  addEmoticon( "📢", "1f4e2", Emoticon::Objects );
  addEmoticon( "📣", "1f4e3", Emoticon::Objects );
  addEmoticon( "📤", "1f4e4", Emoticon::Objects );
  addEmoticon( "📥", "1f4e5", Emoticon::Objects );
  addEmoticon( "📦", "1f4e6", Emoticon::Objects );
  addEmoticon( "📧", "1f4e7", Emoticon::Objects );
  addEmoticon( "📨", "1f4e8", Emoticon::Objects );
  addEmoticon( "📩", "1f4e9", Emoticon::Objects );
  addEmoticon( "📪", "1f4ea", Emoticon::Objects );
  addEmoticon( "📫", "1f4eb", Emoticon::Objects );
  addEmoticon( "📮", "1f4ee", Emoticon::Objects );
  addEmoticon( "📰", "1f4f0", Emoticon::Objects );
  addEmoticon( "📱", "1f4f1", Emoticon::Objects );
  addEmoticon( "📲", "1f4f2", Emoticon::Objects );
  addEmoticon( "📳", "1f4f3", Emoticon::Symbols );
  addEmoticon( "📴", "1f4f4", Emoticon::Symbols );
  addEmoticon( "📶", "1f4f6", Emoticon::Symbols );
  addEmoticon( "📷", "1f4f7", Emoticon::Objects );
  addEmoticon( "📹", "1f4f9", Emoticon::Objects );
  addEmoticon( "📺", "1f4fa", Emoticon::Objects );
  addEmoticon( "📻", "1f4fb", Emoticon::Objects );
  addEmoticon( "📼", "1f4fc", Emoticon::Objects );
  addEmoticon( "🔃", "1f503", Emoticon::Symbols );
  addEmoticon( "🔊", "1f50a", Emoticon::Objects );
  addEmoticon( "🔋", "1f50b", Emoticon::Objects );
  addEmoticon( "🔌", "1f50c", Emoticon::Objects );
  addEmoticon( "🔍", "1f50d", Emoticon::Objects );
  addEmoticon( "🔎", "1f50e", Emoticon::Objects );
  addEmoticon( "🔏", "1f50f", Emoticon::Objects );
  addEmoticon( "🔐", "1f510", Emoticon::Objects );
  addEmoticon( "🔑", "1f511", Emoticon::Objects );
  addEmoticon( "🔒", "1f512", Emoticon::Objects );
  addEmoticon( "🔓", "1f513", Emoticon::Objects );
  addEmoticon( "🔔", "1f514", Emoticon::Objects );
  addEmoticon( "🔖", "1f516", Emoticon::Objects );
  addEmoticon( "🔗", "1f517", Emoticon::Symbols );
  addEmoticon( "🔘", "1f518", Emoticon::Symbols );
  addEmoticon( "🔙", "1f519", Emoticon::Symbols );
  addEmoticon( "🔚", "1f51a", Emoticon::Symbols );
  addEmoticon( "🔛", "1f51b", Emoticon::Symbols );
  addEmoticon( "🔜", "1f51c", Emoticon::Symbols );
  addEmoticon( "🔝", "1f51d", Emoticon::Symbols );
  addEmoticon( "🔞", "1f51e", Emoticon::Symbols );
  addEmoticon( "🔟", "1f51f", Emoticon::Symbols );
  addEmoticon( "🔠", "1f520", Emoticon::Symbols );
  addEmoticon( "🔡", "1f521", Emoticon::Symbols );
  addEmoticon( "🔢", "1f522", Emoticon::Symbols );
  addEmoticon( "🔣", "1f523", Emoticon::Symbols );
  addEmoticon( "🔤", "1f524", Emoticon::Symbols );
  addEmoticon( "🔦", "1f526", Emoticon::Objects );
  addEmoticon( "🔧", "1f527", Emoticon::Objects );
  addEmoticon( "🔨", "1f528", Emoticon::Objects );
  addEmoticon( "🔩", "1f529", Emoticon::Objects );
  addEmoticon( "🔪", "1f52a", Emoticon::Objects );
  addEmoticon( "🔫", "1f52b", Emoticon::Objects );
  addEmoticon( "🔮", "1f52e", Emoticon::Objects );
  addEmoticon( "🔯", "1f52f", Emoticon::Symbols );
  addEmoticon( "🔰", "1f530", Emoticon::Symbols );
  addEmoticon( "🔱", "1f531", Emoticon::Symbols );
  addEmoticon( "🗻", "1f5fb", Emoticon::Places );
  addEmoticon( "🗼", "1f5fc", Emoticon::Places );
  addEmoticon( "🗽", "1f5fd", Emoticon::Places );
  addEmoticon( "🗾", "1f5fe", Emoticon::Places );
  addEmoticon( "🗿", "1f5ff", Emoticon::Places );
  addEmoticon( "🚮", "1f6ae", Emoticon::Symbols );
  addEmoticon( "🚯", "1f6af", Emoticon::Symbols );
  addEmoticon( "🚰", "1f6b0", Emoticon::Symbols );
  addEmoticon( "🚱", "1f6b1", Emoticon::Symbols );
  addEmoticon( "🚳", "1f6b3", Emoticon::Symbols );
  addEmoticon( "🚴", "1f6b4", Emoticon::Objects );
  addEmoticon( "🚵", "1f6b5", Emoticon::Objects );
  addEmoticon( "🚷", "1f6b7", Emoticon::Symbols );
  addEmoticon( "🚸", "1f6b8", Emoticon::Symbols );
  addEmoticon( "🚿", "1f6bf", Emoticon::Objects );
  addEmoticon( "🛁", "1f6c1", Emoticon::Objects );
  addEmoticon( "🛂", "1f6c2", Emoticon::Symbols );
  addEmoticon( "🛃", "1f6c3", Emoticon::Symbols );
  addEmoticon( "🛄", "1f6c4", Emoticon::Symbols );
  addEmoticon( "🛅", "1f6c5", Emoticon::Symbols );
  addEmoticon( "🌞", "1f31e", Emoticon::Nature );
  addEmoticon( "🌲", "1f332", Emoticon::Nature );
  addEmoticon( "🌳", "1f333", Emoticon::Nature );
  addEmoticon( "🍋", "1f34b", Emoticon::Nature );
  addEmoticon( "🍐", "1f350", Emoticon::Nature );
  addEmoticon( "🍼", "1f37c", Emoticon::Objects );
  addEmoticon( "🏇", "1f3c7", Emoticon::Objects );
  addEmoticon( "🏉", "1f3c9", Emoticon::Objects );
  addEmoticon( "🏤", "1f3e4", Emoticon::Places );
  addEmoticon( "🐀", "1f400", Emoticon::Nature );
  addEmoticon( "🐁", "1f401", Emoticon::Nature );
  addEmoticon( "🐂", "1f402", Emoticon::Nature );
  addEmoticon( "🐃", "1f403", Emoticon::Nature );
  addEmoticon( "🐄", "1f404", Emoticon::Nature );
  addEmoticon( "🐅", "1f405", Emoticon::Nature );
  addEmoticon( "🐇", "1f407", Emoticon::Nature );
  addEmoticon( "🐈", "1f408", Emoticon::Nature );
  addEmoticon( "🐉", "1f409", Emoticon::Nature );
  addEmoticon( "🐊", "1f40a", Emoticon::Nature );
  addEmoticon( "🐋", "1f40b", Emoticon::Nature );
  addEmoticon( "🐏", "1f40f", Emoticon::Nature );
  addEmoticon( "🐐", "1f410", Emoticon::Nature );
  addEmoticon( "🐓", "1f413", Emoticon::Nature );
  addEmoticon( "🐕", "1f415", Emoticon::Nature );
  addEmoticon( "🐖", "1f416", Emoticon::Nature );
  addEmoticon( "🐪", "1f42a", Emoticon::Nature );
  addEmoticon( "💶", "1f4b6", Emoticon::Objects );
  addEmoticon( "💷", "1f4b7", Emoticon::Objects );
  addEmoticon( "📬", "1f4ec", Emoticon::Objects );
  addEmoticon( "📭", "1f4ed", Emoticon::Objects );
  addEmoticon( "📯", "1f4ef", Emoticon::Objects );
  addEmoticon( "📵", "1f4f5", Emoticon::Symbols );
  addEmoticon( "🔅", "1f505", Emoticon::Objects );
  addEmoticon( "🔆", "1f506", Emoticon::Objects );
  addEmoticon( "🔇", "1f507", Emoticon::Objects );
  addEmoticon( "🔉", "1f509", Emoticon::Objects );
  addEmoticon( "🔕", "1f515", Emoticon::Objects );
  addEmoticon( "🔬", "1f52c", Emoticon::Objects );
  addEmoticon( "🔭", "1f52d", Emoticon::Objects );
  addEmoticon( "🕐", "1f550", Emoticon::Places );
  addEmoticon( "🌑", "1f311", Emoticon::Nature );
  addEmoticon( "🌚", "1f31a", Emoticon::Nature );
  addEmoticon( "🌓", "1f313", Emoticon::Nature );
  addEmoticon( "🌗", "1f317", Emoticon::Nature );
  addEmoticon( "🌘", "1f318", Emoticon::Nature );
  addEmoticon( "🌔", "1f314", Emoticon::Nature );
  addEmoticon( "🌖", "1f316", Emoticon::Nature );
  addEmoticon( "🌒", "1f312", Emoticon::Nature );
  addEmoticon( "🌕", "1f315", Emoticon::Nature );
  addEmoticon( "🌙", "1f319", Emoticon::Nature );
  addEmoticon( "🌛", "1f31b", Emoticon::Nature );
  addEmoticon( "🌜", "1f31c", Emoticon::Nature );
  addEmoticon( "🌝", "1f31d", Emoticon::Nature );
  addEmoticon( "🌏", "1f30f", Emoticon::Nature );
  addEmoticon( "🌍", "1f30d", Emoticon::Nature );
  addEmoticon( "🌎", "1f30e", Emoticon::Nature );
  addEmoticon( "🌐", "1f310", Emoticon::Nature );
  addEmoticon( "🇩🇪", "1f1e9-1f1ea", Emoticon::Objects );
  addEmoticon( "🇬🇧", "1f1ec-1f1e7", Emoticon::Objects );
  addEmoticon( "🇨🇳", "1f1e8-1f1f3", Emoticon::Objects );
  addEmoticon( "🇯🇵", "1f1ef-1f1f5", Emoticon::Objects );
  addEmoticon( "🇰🇷", "1f1f0-1f1f7", Emoticon::Objects );
  addEmoticon( "🇫🇷", "1f1eb-1f1f7", Emoticon::Objects );
  addEmoticon( "🇮🇹", "1f1ee-1f1f9", Emoticon::Objects );
  addEmoticon( "🇺🇸", "1f1fa-1f1f8", Emoticon::Objects );
  addEmoticon( "🇷🇺", "1f1f7-1f1fa", Emoticon::Objects );
#endif

  qDebug() << "Emoticon manager loads" << emoticons( true ).size() << "images with" << m_emoticons.size() << "keys";
}

void EmoticonManager::addEmoticon( const QString& e_text, const QString& e_name, int emoticon_group )
{
  m_emoticons.insert( e_text.at( 0 ), Emoticon( e_text, e_name, emoticon_group ) );
  if( e_text.size() > m_maxTextSize )
    m_maxTextSize = e_text.size();
}

static bool EmoticonForName( const Emoticon& e1, const Emoticon& e2 )
{
  return e1.name() < e2.name();
}

QList<Emoticon> EmoticonManager::emoticons( bool remove_duplicates ) const
{
  QList<Emoticon> emoticon_list;
  bool emoticon_to_add = false;
  QMultiHash<QChar, Emoticon>::const_iterator it = m_emoticons.begin();
  while( it != m_emoticons.end() )
  {
    if( !remove_duplicates )
      emoticon_list << *it;
    else
    {
      emoticon_to_add = true;
      QList<Emoticon>::iterator it2 = emoticon_list.begin();
      while( it2 != emoticon_list.end() )
      {
        if( (*it2).name() == (*it).name() )
        {
          emoticon_to_add = false;
          if( (*it2).textToMatch().size() > (*it).textToMatch().size() )
            *it2 = *it;
        }
        ++it2;
      }
      if( emoticon_to_add )
        emoticon_list << *it;
    }
    ++it;
  }

  qSort( emoticon_list.begin(), emoticon_list.end(), EmoticonForName );

  return emoticon_list;
}

Emoticon EmoticonManager::emoticon( const QString& e_text ) const
{
  if( e_text.size() > 1 )
  {
    QChar c = e_text.at( 0 );
    QList<Emoticon> emoticon_list = m_emoticons.values( c );
    QList<Emoticon>::const_iterator it = emoticon_list.begin();
    while( it != emoticon_list.end() )
    {
      if( (*it).textToMatch() == e_text )
        return *it;
      ++it;
    }
  }
  return Emoticon();
}

QString EmoticonManager::parseEmoticons( const QString& msg ) const
{
  QString s = "";
  QString text_to_match = "";
  QChar c;
  bool parse_emoticons = true;

  for( int pos = 0; pos < msg.size(); pos++ )
  {
    c = msg[ pos ];

    if( c.isSpace() )
    {
      if( text_to_match.size() > 0 )
      {
        s += text_to_match;
        text_to_match = "";
      }
      s += c;
      parse_emoticons = true;
    }
    else if( text_to_match.size() > 0 )
    {
      text_to_match += c;
      Emoticon e = emoticon( text_to_match );
      if( e.isValid() )
      {
        s += e.toHtml();
        text_to_match = "";
        parse_emoticons = true;
      }
      else
      {
        if( text_to_match.size() >= m_maxTextSize )
        {
          s += text_to_match;
          text_to_match = "";
          parse_emoticons = false;
        }
      }
    }
    else if( m_emoticons.contains( c ) )
    {
      if( parse_emoticons )
      {
        text_to_match = c;
        parse_emoticons = false;
      }
      else
        s += c;
    }
    else
    {
      s += c;
      parse_emoticons = false;
    }
  }

  if( text_to_match.size() > 0 )
    s += text_to_match;

  return s;
}

#ifdef BEEBEEP_USE_EMOJI
void EmoticonManager::createEmojiFile()
{
  QFile emoji_list_file( "emoji_list.txt" );
  if( !emoji_list_file.open( QFile::ReadOnly | QFile::Text ) )
    return;

  QStringList emoji_parts;
  QString emoji_key;
  QString emoji_file;
  QString emoji_line;
  QList<Emoticon> emoji_list;

  QTextStream text_stream_in( &emoji_list_file );
  text_stream_in.setCodec( "UTF-8" );
  while( !text_stream_in.atEnd() )
  {
    emoji_line = text_stream_in.readLine();
    emoji_parts = emoji_line.split( "\t", QString::SkipEmptyParts );
    if( emoji_parts.count() < 2 )
      continue;

    emoji_file = emoji_parts.at( 0 ).trimmed();
    emoji_file.remove( ".png" );
    emoji_key = emoji_parts.at( 1 ).trimmed();

    emoji_list.append( Emoticon( emoji_key, emoji_file ) );
#ifdef BEEBEEP_DEBUG
    qDebug() << "Load Emoji: char" << qPrintable( emoji_key ) << "and file" << emoji_file;
#endif
  }

  emoji_list_file.close();
  qDebug() << emoji_list.size() << "emojis load";

  QStringList emoji_folders;
  emoji_folders << "";
  emoji_folders << "";
  emoji_folders << "../src/emojis/smiley/";
  emoji_folders << "../src/emojis/objects/";
  emoji_folders << "../src/emojis/nature/";
  emoji_folders << "../src/emojis/places/";
  emoji_folders << "../src/emojis/symbols/";

  QStringList emoji_group_names;
  emoji_group_names << "";
  emoji_group_names << "";
  emoji_group_names << "Smiley";
  emoji_group_names << "Objects";
  emoji_group_names << "Nature";
  emoji_group_names << "Places";
  emoji_group_names << "Symbols";

  QList<Emoticon>::iterator emoji_it = emoji_list.begin();
  QString emoji_file_name;
  while( emoji_it != emoji_list.end() )
  {
    for( int i = Emoticon::Smiley; i < Emoticon::NumGroups; i++ )
    {
      emoji_file_name = emoji_folders.at( i ) + emoji_it->name() + ".png";
      if( QFile::exists( emoji_file_name ) )
      {
        qDebug() << "Found emoji" << qPrintable( emoji_it->textToMatch() ) << "in file" << emoji_file_name;
        emoji_it->setGroup( i );
      }
    }
    ++emoji_it;
  }

  int emoji_not_in_group = 0;
  foreach( Emoticon e, emoji_list )
  {
    if( e.group() < Emoticon::Smiley )
      emoji_not_in_group++;
  }

  qDebug() << "Emoji not in group:" << emoji_not_in_group;

  QFile file_to_save( "emoji_save_file.txt" );
  if( file_to_save.exists() )
    file_to_save.remove();

  if( !file_to_save.open( QFile::ReadWrite ) )
  {
    qWarning() << file_to_save.fileName() << "is not writeable";
    return;
  }

  QTextStream text_stream_out( &file_to_save );
  text_stream_out.setCodec( "UTF-8" );

  foreach( Emoticon e, emoji_list )
  {
    if( e.group() > Emoticon::Recent )
      //text_stream_out << " " << e.name() << "\t" << e.group() << "\t" << e.textToMatch() << "\n";
      text_stream_out << "  addEmoticon( \"" << e.textToMatch() << "\", \"" << e.name() << "\", Emoticon::" << emoji_group_names.at( e.group() ) << " ); \n";
  }

  file_to_save.close();
}
#endif

