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


#include "ParticleSystemGlobals.hpp"


float ExplosionSystem::puffSmallScaleMin   = 0.0f;
float ExplosionSystem::puffSmallScaleRand  = 0.3f;
float ExplosionSystem::puffMediumScaleMin  = 0.2f;
float ExplosionSystem::puffMediumScaleRand = 0.2f;
float ExplosionSystem::puffLargeScaleMin   = 0.4f;
float ExplosionSystem::puffLargeScaleRand  = 0.2f;

float ExplosionSystem::puffSmallWaitMin    = 0.05f;
float ExplosionSystem::puffSmallWaitRand   = 0.15f;
float ExplosionSystem::puffMediumWaitMin   = 0.2f;
float ExplosionSystem::puffMediumWaitRand  = 0.3f;
float ExplosionSystem::puffLargeWaitMin    = 0.5f;
float ExplosionSystem::puffLargeWaitRand   = 0.5f;

int   ExplosionSystem::flameLayer = 5;
int   ExplosionSystem::flashLayer = 4;
/*
	Particle Layers

		0 - in the ground
		1 - particle shadows
		2 - particles
		3 - vehicles
		4 - particle shadows
		5 - particles
		6 - particle shadows
		7 - particles
*/

// The following are base values.  You can modify them through the
// text files associated with each section.  When the game ships,
// make sure and modify these values to what we have found to be
// the best and remove the file loading.
//--------------------------------------------------------------------------

// All Particles - particleSystem.dat
//--------------------------------------------------------------------------
float arcScale             = 0.2f;
float shadowDistanceScale  = 0.7f;

// Cloud Particle - cloudParticle.dat
//--------------------------------------------------------------------------
int   cloudLayer           = 7;
int   cloudShadowLayer     = 6;
float cloudWindMinPercent  = 0.5f;
float cloudWindRandPercent = 1.0f;
float speedAbsoluteMin     = 15;

// Cloud Coverage - environment.dat
//--------------------------------------------------------------------------
int   baseTileCountPerCloud        = 500;
float randomDivisorOfBase          = 2.0f;
float clearPercentOfBase           = 0.0f; //0.05f
float brokenPercentOfBase          = 0.2f;
float partlyCloudyPercentOfBase    = 0.45f;
float overcastPercentOfBase        = 0.75f;
float extremelyCloudyPercentOfBase = 1.0f;

// Wind Speeds - environment.dat
//--------------------------------------------------------------------------
int   baseWindSpeed               = 120;
float calmWindsPercentOfBase      = 0.1f;
float breezyWindsPercentOfBase    = 0.25f;
float briskWindsPercentOfBase     = 0.45f;
float heavyWindsPercentOfBase     = 0.75f;
float typhoonWindsPercentOfBase   = 1.0f;

// Smoke Particles - smokeParticle.dat
//--------------------------------------------------------------------------
int   smokeLayer        =  5;
int   smokeShadowLayer  =  4;
int   smokeFPSMin       =  6;
int   smokeFPSRand      = 14;
float smokeWindScale    = 0.35f;

// Muzzle System statics -
//--------------------------------------------------------------------------
float MuzzleSystem::windScale   = 0.1f;
int   MuzzleSystem::layer       = 5;
int   MuzzleSystem::shadowLayer = 4;
int   MuzzleSystem::flashLayer  = 4;


//int smokeFPSMin       = 10;
//int smokeFPSRand      = 10;
//int smokeLayer        = 3;
//int smokeShadowLayer  = 2;
//
//int   cloudLayer                       = 7;
//int   cloudShadowLayer                 = 6;
//float cloudWindMin                     = 0.5f;
//float cloudWindRand                    = 1.0f;
//
//// Chunk Particles.
//float explosionParticleMaxCount              = 16;
//float explosionSmallParticleMaxCountPercent  = 1.0f;
//float explosionMediumParticleMaxCountPercent = 0.0f;
//float explosionLargeParticleMaxCountPercent  = 0.0f;
//float explosionSmallSmokeParticleMaxSpeed    = 400;
//float explosionMediumSmokeParticleMaxSpeed   = 160;
//float explosionLargeSmokeParticleMaxSpeed    = 120;
//float explosionSmallSmokeParticleScaleMin    = 0.0f;
//float explosionSmallSmokeParticleScaleRand   = 0.15f;
//float explosionMediumSmokeParticleScaleMin   = 0.2f;
//float explosionMediumSmokeParticleScaleRand  = 0.15f;
//float explosionLargeSmokeParticleScaleMin    = 0.3f;
//float explosionLargeSmokeParticleScaleRand   = 0.2f;
//float explosionParticleArc                   = 0.4f;
//int   explosionParticleShadowLayer           = 4;
//float explosionSmallSmokeParticleWaitMin     = 0.1f;
//float explosionSmallSmokeParticleWaitRand    = 0.2f;
//float explosionMediumSmokeParticleWaitMin    = 0.2f;
//float explosionMediumSmokeParticleWaitRand   = 0.35f;
//float explosionLargeSmokeParticleWaitMin     = 0.5f;
//float explosionLargeSmokeParticleWaitRand    = 0.5f;
//
//// Smoking Particles.
//int   smokingTrajectoryParticleMaxCount  = 50;
//int   smokingTrajectoryParticleMaxSpeed  = 50;
//float smokingTrajectoryParticleArc       = 0.15f;
//
//float smolderMinScale                  = 0.1f;
//float smolderRandScale                 = 0.5f;
//int   smolderMinFPS                    = 8;
//int   smolderRandFPS                   = 12;
//int   smolderLayer                     = 6;
//int   smolderShadowLayer               = 4;
//float smolderWindScale                 = 0.5f;
//float smolderIncrementScale            = 0.0f;
//int   smolderExplosionMaxParticleCount = 14;
//
//float missleFlightPuffScaleMin         = 0.1f;
//float missleFlightPuffScaleRand        = 0.1f;
//float missleFlightPuffWaitMin          = 0.05f;
//float missleFlightPuffWaitRand         = 0.05f;
//int   missleFlightPuffFPSMin           = 8;
//int   missleFlightPuffFPSRand          = 8;
//int   missleFlightPuffLayer            = 6;
//int   missleFlightShadowPuffLayer      = 4;
//float missleFlightPuffWindScale        = 0.7f;
//float missleFlightPuffIncrementScale   = 0.3f;
//
//int   missleLaunchPuffSpeedMin       = 100;
//int   missleLaunchPuffSpeedRand      = 50;
//int   missleLaunchPuffFPSMin         = 30;
//int   missleLaunchPuffFPSRand        = 20;
//float missleLaunchPuffWaitMin        = 0.075f;
//float missleLaunchPuffWaitRand       = 0.075f;
//float missleLaunchPuffLifetimeMin    = 0.7f;
//float missleLaunchPuffLifetimeRand   = 0.3f;
//int   missleLaunchPuffLayer          = 5;
//float missleLaunchPuffWindScale      = 0.0f;
//float missleLaunchPuffIncrementScale = 0.3f;
//float missleLaunchPuffMinScale       = 0.02f;
//float missleLaunchPuffRandScale      = 0.05f;
//float missleLaunchPuffExpandRadius   = 0.5f;
//
//float movePuffMinScale                 = 0.07f;
//float movePuffRandScale                = 0.1f;
//float movePuffMinWait                  = 0.2f;
//float movePuffRandWait                 = 0.1f;
//int   movePuffMinFPS                   = 10;
//int   movePuffRandFPS                  = 10;
//int   movePuffLayer                    = 2;
//int   moveShadowPuffLayer              = 1;
//float movePuffWindScale                = 0.2f;
//
//float missleExplosionFlameMinScale     = 0.15f;
//float missleExplosionFlameRandScale    = 0.15f;
//float missleExplosionFlashLifetime     = 0.5f;
//int   missleExplosionFlameLayer	       = 6;
//
//float missleExplosionFlashScaleMin     = 0.25f;
//float missleExplosionFlashScaleRand    = 0.15f;
//int   missleExplosionFlashLayer        = 5;
//
//int   missleExplosionSmokeMaxParticleCount = 14;
//
//float vehicleExplosionFlameMinScale    = 0.3f;
//float vehicleExplosionFlameRandScale   = 0.25f;
//int   vehicleExplosionFlameLayer       = 6;
//
//float vehicleExplosionFlashScaleMin    = 0.6f;
//float vehicleExplosionFlashScaleRand   = 0.4f;
//float vehicleExplosionFlashLifetime    = 0.8f;
//int   vehicleExplosionFlashLayer       = 5;
//
//int   vehicleExplosionMaxParticleCount = 5;
//int   vehicleExplosionSmokeMaxParticleCount = 14;
//
//int   dirtExplosionMaxParticleCount    = 14;
//int   dirtPuffMaxParticleCount         = 8;
//
//iXY lightTankMoveParticleRadius        = iXY(10, 10);
//iXY mediumTankMoveParticleRadius       = iXY(10, 10);
//iXY heavyTankMoveParticleRadius        = iXY(10, 10);
//iXY missleLauncherMoveParticleRadius   = iXY(10, 10);
//iXY humveeMoveParticleRadius           = iXY(10, 10);
//iXY hoverCraftMoveParticleRadius       = iXY(10, 10);
//iXY commKillerMoveParticleRadius       = iXY(10, 10);
//iXY refuelerMoveParticleRadius         = iXY(10, 10);
//
//int   muzzlePuffSpeedMin       = 100;
//int   muzzlePuffSpeedRand      = 50;
//int   muzzlePuffFPSMin         = 14;
//int   muzzlePuffFPSRand        = 4;
//float muzzlePuffWaitMin        = 0.05f;
//float muzzlePuffWaitRand       = 0.05f;
//float muzzlePuffLifetimeMin    = 0.2f;
//float muzzlePuffLifetimeRand   = 0.1f;
//int   muzzlePuffLayer          = 5;
//float muzzlePuffWindScale      = 0.2f;
//float muzzleFlashMinScale      = 0.3f;
//float muzzleFlashRandScale     = 0.2f;
//int   muzzleFlashLayer         = 6;
//float muzzlePuffIncrementScale = 0.3f;
//float muzzlePuffMinScale       = 0.1f;
//float muzzlePuffRandScale      = 0.1f;
//float muzzlePuffExpandRadius   = 0.5f;
//
//PARTICLE_FILE_TYPE gParticleFileType = PAK;
