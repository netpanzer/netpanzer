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
#ifndef __cInputField_hpp__
#define __cInputField_hpp__

#include "Types/iXY.hpp"
#include "Types/iRect.hpp"
#include "2D/Surface.hpp"
#include "Util/NoCopy.hpp"

//--------------------------------------------------------------------------
class cInputFieldString : public NoCopy
{
    friend class cInputField;

private:
    int   maxCharCount;
    int     maxWidth;         // max width in the gui
    char *string;

public:
    cInputFieldString()
    {
        reset();
    }

    ~cInputFieldString()
    {
        free();
    }

    void free()
    {
        if (string != 0) {
            delete [] string;
            string = 0;
        }
    }

    void init(const char *string, int maxCharCount,int maxWidth);
    void init(const char *string, int maxCharCount) {
        init(string,maxCharCount,maxCharCount);
    }
    void setString(const std::string& string);
    void reset();

    const char *getString()
    {
        return string;
    }
}
; // end cInputFieldString


//--------------------------------------------------------------------------
class cInputField
{
public:
    cInputField();
    cInputField(const cInputField& other);
    ~cInputField();

    void operator= (const cInputField& other);

    iXY    getPos() const
    {
        return pos;
    }
    int    getMaxCharCount() const
    {
        return maxCharCount;
    }
    char  *getDestString() const
    {
        return destString;
    }
    char  *getExcludedCharacters() const
    {
        return excludedCharacters;
    }

    bool contains(const iXY &pos) const
    {
        return bounds.contains(pos);
    }

    typedef void (*ACTION_FUNC_PTR)(cInputField* inputfield);

    void setPos(iXY pos);
    void setInputFieldString(cInputFieldString *string);
    void setTextAction(ACTION_FUNC_PTR func);
    void setExcludedCharacters(const char *exChars);
    void setmaxChar(const int maxChar)
    {
        maxCharCount = maxChar;
    }
    void addChar(int newChar);
    void addExtendedChar(int newExtendedChar);
    void draw(Surface &dest);
    void drawHighlighted(Surface &dest);
    void checkCursor();
    void resetString();     // reset string position

private:
    iXY      pos;
    size_t   maxCharCount;
    int     strDisplayStart;  // which char to start displaying from
    int     maxWidth;         // max width in the gui
    bool    insertMode;
    char    *destString;
    char    *excludedCharacters;
    iRect    bounds;
    size_t   cursorPos;
    Surface inputFieldSurface;
    ACTION_FUNC_PTR textaction;

    void reset();
}; // end cInputField

#endif // end __cInputField_hpp__
