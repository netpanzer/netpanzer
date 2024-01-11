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

#include "iRect.hpp"

#include <algorithm>

iRect iRect::operator|(const iRect &a) {
  if (isEmpty()) return a;
  if (a.isEmpty()) return *this;

  return iRect(std::min(min.x, a.min.x), std::min(min.y, a.min.y),
               std::max(max.x, a.max.x), std::max(max.y, a.max.y));
}
