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
#ifndef __Color_hpp__
#define __Color_hpp__

#include <SDL.h>

#include "Core/CoreTypes.hpp"

//---------------------------------------------------------------------------
class Color {
 public:
  // Color for netPanzer players.
  static Uint8 unitAqua;
  static Uint8 unitYellow;
  static Uint8 unitRed;
  static Uint8 unitBlue;
  static Uint8 unitDarkBlue;
  static Uint8 unitLightGreen;
  static Uint8 unitGreen;
  static Uint8 unitBlueGray;
  static Uint8 unitDarkRed;
  static Uint8 unitBlack;
  static Uint8 unitDarkGreen;
  static Uint8 unitWhite;
  static Uint8 unitLightOrange;
  static Uint8 unitOrange;
  static Uint8 unitGray;
  static Uint8 unitDarkGray;

  // Normal Colors.
  static Uint8 black;
  static Uint8 blue;
  static Uint8 brown;
  static Uint8 cyan;
  static Uint8 gray;
  static Uint8 green;
  static Uint8 magenta;
  static Uint8 orange;
  static Uint8 pink;
  static Uint8 red;
  static Uint8 white;
  static Uint8 yellow;

  // Color specifically for netPanzer.
  static Uint8 tan;
  static Uint8 chartreuse;
  static Uint8 cobaltGreen;
  static Uint8 emeraldGreen;
  static Uint8 forestGreen;
  static Uint8 darkOliveGreen;
  static Uint8 terreVerte;

  // Dark Colors.
  static Uint8 darkBlue;
  static Uint8 darkBrown;
  static Uint8 darkCyan;
  static Uint8 darkGray;
  static Uint8 darkGreen;
  static Uint8 darkMagenta;
  static Uint8 darkOrange;
  static Uint8 darkPink;
  static Uint8 darkRed;
  static Uint8 darkYellow;

  // Light Colors.
  static Uint8 lightBlue;
  static Uint8 lightBrown;
  static Uint8 lightCyan;
  static Uint8 lightGray;
  static Uint8 lightGreen;
  static Uint8 lightMagenta;
  static Uint8 lightOrange;
  static Uint8 lightPink;
  static Uint8 lightRed;
  static Uint8 lightYellow;

  // Gray scales.
  static Uint8 gray32;
  static Uint8 gray64;
  static Uint8 gray96;
  static Uint8 gray128;
  static Uint8 gray160;
  static Uint8 gray192;
  static Uint8 gray224;

 private:
  friend class ScriptManager;
  static void registerScript(const NPString& table_name);
};  // end Color

#endif  // end __Color_hpp__

// This is the baddest thing since sliced bread.  This is some list my brother
// found on the internet of the RGB color values of a freaking crayon box.

// # Whites
// antique_white         250 235 215   0.9804 0.9216 0.8431
// azure                 240 255 255   0.9412 1.0000 1.0000
// bisque                255 228 196   1.0000 0.8941 0.7686
// blanched_almond       255 235 205   1.0000 0.9216 0.8039
// cornsilk              255 248 220   1.0000 0.9725 0.8627
// eggshell              252 230 201   0.9900 0.9000 0.7900
// floral_white          255 250 240   1.0000 0.9804 0.9412
// gainsboro             220 220 220   0.8627 0.8627 0.8627
// ghost_white           248 248 255   0.9725 0.9725 1.0000
// honeydew              240 255 240   0.9412 1.0000 0.9412
// ivory                 255 255 240   1.0000 1.0000 0.9412
// lavender              230 230 250   0.9020 0.9020 0.9804
// lavender_blush        255 240 245   1.0000 0.9412 0.9608
// lemon_chiffon         255 250 205   1.0000 0.9804 0.8039
// linen                 250 240 230   0.9804 0.9412 0.9020
// mint_cream            245 255 250   0.9608 1.0000 0.9804
// misty_rose            255 228 225   1.0000 0.8941 0.8824
// moccasin              255 228 181   1.0000 0.8941 0.7098
// navajo_white          255 222 173   1.0000 0.8706 0.6784
// old_lace              253 245 230   0.9922 0.9608 0.9020
// papaya_whip           255 239 213   1.0000 0.9373 0.8353
// peach_puff            255 218 185   1.0000 0.8549 0.7255
// seashell              255 245 238   1.0000 0.9608 0.9333
// snow                  255 250 250   1.0000 0.9804 0.9804
// thistle               216 191 216   0.8471 0.7490 0.8471
// titanium_white        252 255 240   0.9900 1.0000 0.9400
// wheat                 245 222 179   0.9608 0.8706 0.7020
// white                 255 255 255   1.0000 1.0000 1.0000
// white_smoke           245 245 245   0.9608 0.9608 0.9608
// zinc_white            253 248 255   0.9900 0.9700 1.0000

// # Greys
// cold_grey             128 138 135   0.5000 0.5400 0.5300
// dim_grey              105 105 105   0.4118 0.4118 0.4118
// grey                  192 192 192   0.7529 0.7529 0.7529
// light_grey            211 211 211   0.8275 0.8275 0.8275
// slate_grey            112 128 144   0.4392 0.5020 0.5647
// slate_grey_dark        47  79  79   0.1843 0.3098 0.3098
// slate_grey_light      119 136 153   0.4667 0.5333 0.6000
// warm_grey             128 128 105   0.5000 0.5000 0.4100

// # Blacks
// black                   0   0   0   0.0000 0.0000 0.0000
// ivory_black            41  36  33   0.1600 0.1400 0.1300
// lamp_black             46  71  59   0.1800 0.2800 0.2300

// # Reds
// alizarin_crimson      227  38  54   0.8900 0.1500 0.2100
// brick                 156 102  31   0.6100 0.4000 0.1200
// cadmium_red_deep      227  23  13   0.8900 0.0900 0.0500
// coral                 255 127  80   1.0000 0.4980 0.3137
// coral_light           240 128 128   0.9412 0.5020 0.5020
// deep_pink             255  20 147   1.0000 0.0784 0.5765
// english_red           212  61  26   0.8300 0.2400 0.1000
// firebrick             178  34  34   0.6980 0.1333 0.1333
// geranium_lake         227  18  48   0.8900 0.0700 0.1900
// hot_pink              255 105 180   1.0000 0.4118 0.7059
// indian_red            176  23  31   0.6900 0.0900 0.1200
// light_salmon          255 160 122   1.0000 0.6275 0.4784
// madder_lake_deep      227  46  48   0.8900 0.1800 0.1900
// maroon                176  48  96   0.6902 0.1882 0.3765
// pink                  255 192 203   1.0000 0.7529 0.7961
// pink_light            255 182 193   1.0000 0.7137 0.7569
// raspberry             135  38  87   0.5300 0.1500 0.3400
// red                   255   0   0   1.0000 0.0000 0.0000
// rose_madder           227  54  56   0.8900 0.2100 0.2200
// salmon                250 128 114   0.9804 0.5020 0.4471
// tomato                255  99  71   1.0000 0.3882 0.2784
// venetian_red          212  26  31   0.8300 0.1000 0.1200

// # Browns
// beige                 163 148 128   0.6400 0.5800 0.5000
// brown                 128  42  42   0.5000 0.1647 0.1647
// brown_madder          219  41  41   0.8600 0.1600 0.1600
// brown_ochre           135  66  31   0.5300 0.2600 0.1200
// burlywood             222 184 135   0.8706 0.7216 0.5294
// burnt_sienna          138  54  15   0.5400 0.2100 0.0600
// burnt_umber           138  51  36   0.5400 0.2000 0.1400
// chocolate             210 105  30   0.8235 0.4118 0.1176
// deep_ochre            115  61  26   0.4500 0.2400 0.1000
// flesh                 255 125  64   1.0000 0.4900 0.2500
// flesh_ochre           255  87  33   1.0000 0.3400 0.1300
// gold_ochre            199 120  38   0.7800 0.4700 0.1500
// greenish_umber        255  61  13   1.0000 0.2400 0.0500
// khaki                 240 230 140   0.9412 0.9020 0.5490
// khaki_dark            189 183 107   0.7412 0.7176 0.4196
// light_beige           245 245 220   0.9608 0.9608 0.8627
// peru                  205 133  63   0.8039 0.5216 0.2471
// rosy_brown            188 143 143   0.7373 0.5608 0.5608
// raw_sienna            199  97  20   0.7800 0.3800 0.0800
// raw_umber             115  74  18   0.4500 0.2900 0.0700
// sepia                  94  38  18   0.3700 0.1500 0.0700
// sienna                160  82  45   0.6275 0.3216 0.1765
// saddle_brown          139  69  19   0.5451 0.2706 0.0745
// sandy_brown           244 164  96   0.9569 0.6431 0.3765
// tan                   210 180 140   0.8235 0.7059 0.5490
// van_dyke_brown         94  38   5   0.3700 0.1500 0.0200

// # Oranges
// cadmium_orange        255  97   3   1.0000 0.3800 0.0100
// cadmium_red_light     255   3  13   1.0000 0.0100 0.0500
// carrot                237 145  33   0.9300 0.5700 0.1300
// dark_orange           255 140   0   1.0000 0.5490 0.0000
// mars_orange           150  69  20   0.5900 0.2700 0.0800
// mars_yellow           227 112  26   0.8900 0.4400 0.1000
// orange                255 128   0   1.0000 0.5000 0.0000
// orange_red            255  69   0   1.0000 0.2706 0.0000
// yellow_ochre          227 130  23   0.8900 0.5100 0.0900

// # Yellows
// aureoline_yellow      255 168  36   1.0000 0.6600 0.1400
// banana                227 207  87   0.8900 0.8100 0.3400
// cadmium_lemon         255 227   3   1.0000 0.8900 0.0100
// cadmium_yellow        255 153  18   1.0000 0.6000 0.0700
// cadmium_yellow_light  255 176  15   1.0000 0.6900 0.0600
// gold                  255 215   0   1.0000 0.8431 0.0000
// goldenrod             218 165  32   0.8549 0.6471 0.1255
// goldenrod_dark        184 134  11   0.7216 0.5255 0.0431
// goldenrod_light       250 250 210   0.9804 0.9804 0.8235
// goldenrod_pale        238 232 170   0.9333 0.9098 0.6667
// light_goldenrod       238 221 130   0.9333 0.8667 0.5098
// melon                 227 168 105   0.8900 0.6600 0.4100
// naples_yellow_deep    255 168  18   1.0000 0.6600 0.0700
// yellow                255 255   0   1.0000 1.0000 0.0000
// yellow_light          255 255 224   1.0000 1.0000 0.8784

// # Greens
// chartreuse            127 255   0   0.4980 1.0000 0.0000
// chrome_oxide_green    102 128  20   0.4000 0.5000 0.0800
// cinnabar_green         97 179  41   0.3800 0.7000 0.1600
// cobalt_green           61 145  64   0.2400 0.5700 0.2500
// emerald_green           0 201  87   0.0000 0.7900 0.3400
// forest_green           34 139  34   0.1333 0.5451 0.1333
// green                   0 255   0   0.0000 1.0000 0.0000
// green_dark              0 100   0   0.0000 0.3922 0.0000
// green_pale            152 251 152   0.5961 0.9843 0.5961
// green_yellow          173 255  47   0.6784 1.0000 0.1843
// lawn_green            124 252   0   0.4863 0.9882 0.0000
// lime_green             50 205  50   0.1961 0.8039 0.1961
// mint                  189 252 201   0.7400 0.9900 0.7900
// olive                  59  94  43   0.2300 0.3700 0.1700
// olive_drab            107 142  35   0.4196 0.5569 0.1373
// olive_green_dark       85 107  47   0.3333 0.4196 0.1843
// permanent_green        10 201  43   0.0400 0.7900 0.1700
// sap_green              48 128  20   0.1900 0.5000 0.0800
// sea_green              46 139  87   0.1804 0.5451 0.3412
// sea_green_dark        143 188 143   0.5608 0.7373 0.5608
// sea_green_medium       60 179 113   0.2353 0.7020 0.4431
// sea_green_light        32 178 170   0.1255 0.6980 0.6667
// spring_green            0 255 127   0.0000 1.0000 0.4980
// spring_green_medium     0 250 154   0.0000 0.9804 0.6039
// terre_verte            56  94  15   0.2200 0.3700 0.0600
// viridian_light        110 255 112   0.4300 1.0000 0.4400
// yellow_green          154 205  50   0.6039 0.8039 0.1961

// # Cyans
// aquamarine            127 255 212   0.4980 1.0000 0.8314
// aquamarine_medium     102 205 170   0.4000 0.8039 0.6667
// cyan                    0 255 255   0.0000 1.0000 1.0000
// cyan_white            224 255 255   0.8784 1.0000 1.0000
// turquoise              64 224 208   0.2510 0.8784 0.8157
// turquoise_dark          0 206 209   0.0000 0.8078 0.8196
// turquoise_medium       72 209 204   0.2824 0.8196 0.8000
// turquoise_pale        175 238 238   0.6863 0.9333 0.9333

// # Blues
// alice_blue            240 248 255   0.9412 0.9725 1.0000
// blue                    0   0 255   0.0000 0.0000 1.0000
// blue_light            173 216 230   0.6784 0.8471 0.9020
// blue_medium             0   0 205   0.0000 0.0000 0.8039
// cadet                  95 158 160   0.3725 0.6196 0.6275
// cobalt                 61  89 171   0.2400 0.3500 0.6700
// cornflower            100 149 237   0.3922 0.5843 0.9294
// cerulean                5 184 204   0.0200 0.7200 0.8000
// dodger_blue           30 144 255   0.1176 0.5647 1.0000
// indigo                  8  46  84   0.0300 0.1800 0.3300
// manganese_blue          3 168 158   0.0100 0.6600 0.6200
// midnight_blue          25  25 112   0.0980 0.0980 0.4392
// navy                    0   0 128   0.0000 0.0000 0.5020
// peacock                51 161 201   0.2000 0.6300 0.7900
// powder_blue           176 224 230   0.6902 0.8784 0.9020
// royal_blue             65 105 225   0.2549 0.4118 0.8824
// slate_blue            106  90 205   0.4157 0.3529 0.8039
// slate_blue_dark        72  61 139   0.2824 0.2392 0.5451
// slate_blue_light      132 112 255   0.5176 0.4392 1.0000
// slate_blue_medium     123 104 238   0.4824 0.4078 0.9333
// sky_blue              135 206 235   0.5294 0.8078 0.9216
// sky_blue_deep           0 191 255   0.0000 0.7490 1.0000
// sky_blue_light        135 206 250   0.5294 0.8078 0.9804
// steel_blue             70 130 180   0.2745 0.5098 0.7059
// steel_blue_light      176 196 222   0.6902 0.7686 0.8706
// turquoise_blue          0 199 140   0.0000 0.7800 0.5500
// ultramarine            18  10 143   0.0700 0.0400 0.5600

// # Magentas
// blue_violet           138  43 226   0.5412 0.1686 0.8863
// cobalt_violet_deep    145  33 158   0.5700 0.1300 0.6200
// magenta               255   0 255   1.0000 0.0000 1.0000
// orchid                218 112 214   0.8549 0.4392 0.8392
// orchid_dark           153  50 204   0.6000 0.1961 0.8000
// orchid_medium         186  85 211   0.7294 0.3333 0.8275
// permanent_red_violet  219  38  69   0.8600 0.1500 0.2700
// plum                  221 160 221   0.8667 0.6275 0.8667
// purple                160  32 240   0.6275 0.1255 0.9412
// purple_medium         147 112 219   0.5765 0.4392 0.8588
// ultramarine_violet     92  36 110   0.3600 0.1400 0.4300
// violet                143  94 153   0.5600 0.3700 0.6000
// violet_dark           148   0 211   0.5804 0.0000 0.8275
// violet_red            208  32 144   0.8157 0.1255 0.5647
// violet_red_medium     199  21 133   0.7804 0.0824 0.5216
// violet_red_pale       219 112 147   0.8588 0.4392 0.5765
