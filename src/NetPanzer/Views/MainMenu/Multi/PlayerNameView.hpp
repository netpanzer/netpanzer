/*
Copyright (C) 1998 Pyrosoft Inc. (www.pyrosoftgames.com), Matthew Bogue

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef __PlayerNameView_hpp__
#define __PlayerNameView_hpp__

#include "2D/Surface.hpp"
#include "Views/Components/View.hpp"

//---------------------------------------------------------------------------
class PlayerNameView : public View {
 public:
  PlayerNameView();
  virtual ~PlayerNameView();

  virtual void doDraw(Surface &windowArea, Surface &clientArea);

  void init();

 private:
  static void textChanged(cInputField *input);

  enum { BORDER_SPACE = 4 };
  // irc nick is 16 chars max, 2 = "np", 1 spare for digit if nick is taken
  enum { INPUT_FIELD_CHARACTERS = 20 };

  cInputFieldString playerName;
};  // end PlayerNameView

#endif  // end __PlayerNameView_hpp__
