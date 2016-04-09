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
//---------------------------------------------------------------------------

#ifndef __Particle2D_hpp__
#define __Particle2D_hpp__

#include "Types/iRect.hpp"
#include "Types/iXY.hpp"
#include "Types/fXYZ.hpp"
#include "Classes/Sprite.hpp"
#include "Classes/SpriteSorter.hpp"
#include "ParticleSystemGlobals.hpp"


/////////////////////////////////////////////////////////////////////////////
// Forwards.
/////////////////////////////////////////////////////////////////////////////

class ColorTable;
class Surface;
class PackedSurface;


//--------------------------------------------------------------------------
class Particle2D
{
public:
    Particle2D(const fXYZ &pos);
    virtual ~Particle2D();

    static  void setBltTo(int bltTo)
    {
        Particle2D::bltTo = bltTo;
    }

    static  void drawAll(const Surface &dest, SpriteSorter &sorter);
    static  void removeAll();
    static  void simAll();

    virtual void draw(const Surface &dest, SpriteSorter &sorter);
    virtual void sim();

    void *operator new(size_t numBytes);  // Overloaded for static array.
    void operator delete(void *block);    // Overloaded for static array.

    static int getFrameCount()
    {
        return frameCount;
    }
    static int getPeakCount()
    {
        return peakCount;
    }
    static int getBltTo()
    {
        return bltTo;
    }
    static int getCreateParticles()
    {
        return createParticles;
    }
    static int getDrawParticles()
    {
        return drawParticles;
    }

    static void setCreateParticles(bool value)
    {
        Particle2D::createParticles = value;
    }

    // Return the resulting random FPS.
    int getFPS(int FPSmin, int FPSrand);

    // Returns the pak index depending on the the specified scale.
    static int getPakIndex(float scale, int pakImageCount);

    static float getScale(float scaleMin, float scaleRand);

    static float getLifetime(float lifetimeMin, float lifetimeRand);

    // Check to see if this particle needs to be reduced, because the distance
    // is very far from the world view.
    static int getFarAway(const fXYZ &worldPos);

protected:
    static int   frameCount;          // How many are alive this frame?
    static int   peakCount;           // How many were alive max?
    static int   bltTo;               // What is the destination renderer?
    static int   createParticles;     // Create any new particles?
    static int   drawParticles;       // Draw any particles?

    void reset();
    void insertMe();
    void removeMe();

    float age;          // How long have I been alive?
    float lifetime;     // How long will I live?
    bool  isAlive;      // Am I alive?
    fXYZ  direction;    // Where am I headed?
    fXYZ  pos;          // Where am I at in the world?
    fXYZ  velocity;     // How fast am I traveling?
    fXYZ  acceleration; // At what rate am I accelerating?
    char  layer;        // What layer to draw me.
    char  shadowLayer;  // What layer to draw my shadow.
    float scale;        // What size to draw?
    int   index;        // Used to with scale to define frame of surface.
    int   FPSMin;       // Minimum frames per second.
    int   FPSRand;      // Random frames per second adjustment.
    int   isFarAway;    // Flag denoting this is far from the world view or not.

    SpritePacked packedSurface;           // My graphic.
    SpritePacked packedSurfaceShadow;     // My shadow's graphic.

    static Particle2D *const zParticle2D; // The list-loop particle.
    Particle2D *next;              // Next in the list.
    Particle2D *prev;              // Previous in the list.

}
; // end Particle2D

//// SpriteBltNormal
////--------------------------------------------------------------------------
//class SpriteBltNormal : public SpriteSurface
//{
//public:
//	virtual void blit(Surface *surface, iRect &world_win);
//
//}; // end SpriteBltNormal
//
//// SpriteBlt
////--------------------------------------------------------------------------
//class SpriteBlt
//{
//public:
//	virtual void blit(Surface *surface, iRect &world_win);
//
//	ColorTable *colorTable;
//
//	float scale;
//
//}; // end SpriteBltNormal
//
//// SpriteBltTrans
////--------------------------------------------------------------------------
//class SpriteBltTrans : public SpriteSurface
//{
//public:
//	virtual void blit(Surface *surface, iRect &world_win);
//
//}; // end SpriteBltTrans
//
//// SpriteBltBrightness
////--------------------------------------------------------------------------
//class SpriteBltBrightness : public SpriteSurface
//{
//public:
//	float brightness;
//
//	virtual void blit(Surface *surface, iRect &world_win);
//
//}; // end SpriteBltTrans
//
//// SpriteBltBlendRandom
////--------------------------------------------------------------------------
//class SpriteBltBlendRandom : public SpriteSurface
//{
//private:
//	int blendTable;
//
//public:
//	SpriteBltBlendRandom();
//
//	virtual void blit(Surface *surface, iRect &world_win);
//
//}; // end SpriteBltBlendRandom
//
//// SpriteBltBlendRandomScale
////--------------------------------------------------------------------------
//class SpriteBltBlendRandomScale : public SpriteSurface
//{
//private:
//	int blendTable;
//
//public:
//	SpriteBltBlendRandomScale();
//	float yPos;
//
//	virtual void blit(Surface *surface, iRect &world_win);
//
//}; // end SpriteBltBlendRandomScale
//
//// SpriteBltBlendRandomScale
////--------------------------------------------------------------------------
//class SpriteBltBlendScale : public SpriteSurface
//{
//public:
//	SpriteBltBlendScale()
//	{
//		colorTable = 0;
//	}
//
//	virtual void blit(Surface *surface, iRect &world_win);
//
//	ColorTable *colorTable;
//	float       scale;
//
//}; // end SpriteBltBlendScale
//
//// SpriteBltBlend
////--------------------------------------------------------------------------
//class SpriteBltBlend : public SpriteSurface
//{
//public:
//	SpriteBltBlend()
//	{
//		colorTable = 0;
//		useBlend   = 1;
//	}
//
//	virtual void blit(Surface *surface, iRect &world_win);
//
//	ColorTable *colorTable;
//	char        useBlend;
//
//}; // end SpriteBltBlend
//
//// SpriteBltScale
////--------------------------------------------------------------------------
//class SpriteBltScale : public SpriteSurface
//{
//public:
//	SpriteBltScale() {}
//
//	virtual void blit(Surface *surface, iRect &world_win);
//
//	float scale;
//
//}; // end SpriteBltScale
//
//// PackedSurfacePreScaled
////--------------------------------------------------------------------------
//class PackedSurfacePreScaled : SpriteSurface
//{
//public:
//	PackedSurfacePreScaled() {}
//
//	virtual void blit(Surface *surface, iRect &world_win);
//	virtual void setTo(const PackedSurface &source);
//
//}; // end PackedSurfacePreScaled
//
//// SpritePackedBlt
////--------------------------------------------------------------------------
//class SpritePackedBlt : public SpritePacked
//{
//public:
//	SpritePackedBlt() {}
//
//	virtual void blit( Surface *surface, iRect &world_win );
//
//}; // end SpritePackedBlt

#endif // __Particle2D_hpp__
