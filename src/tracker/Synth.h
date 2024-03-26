/*
 *  tracker/Synth.h 
 *
 *  Copyright 2023 Leon van Kammen (coderofsalvation)
 * 
 *
 *  This file is part of Milkytracker.
 *
 *  Milkytracker is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Milkytracker is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Milkytracker.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __SYN_H
#define __SYN_H

#include "BasicTypes.h"
#include "XModule.h"
#include "Screen.h"
#include "Event.h"
#include "XModule.h"

#define SYN_PREFIX_V1 "M1"                           // samplename 'M<version><params>' hints XM editors that sample was created with milkysynth <version> using <params> 
#define SYN_PREFIX_CHARS 2                           // "M*"
#define SYN_PARAMS_MAX MP_MAXTEXT-SYN_PREFIX_CHARS   // max samplechars minus "M*" (32-2=30)     
#define SYN_OFFSET_CHAR 33                           // printable chars only 33..127 = 0..91 (this allows ascii copy/paste of synths in the future)
#define SYN_PARAM_MAX_VALUE 92                       // 92 printable chars
#define SYN_PARAM_NORMALIZE(x) (1.0f/(float)SYN_PARAM_MAX_VALUE)*x
#define NOTE2HZ(m) (440.0 * pow(2, (m - 69) / 12.0)) 
#define NOTE_START 60                                // C3
                                                    
// synth ID's
#define SYNTH_FM_PAINT    0                  //
#define SYNTH_CYCLE_PAINT 1                  // incremental numbers
#define SYNTH_LAST        SYNTH_CYCLE_PAINT  // update this when adding a synth

#ifndef M_PI
#define M_PI   3.14159265358979323846264338327950288
#endif

struct MSynthParam{
  PPString name;
  float value;
  int min;
  int max;
};

struct MSynth{
  PPString name;
  MSynthParam param[SYN_PARAMS_MAX];
  int nparams;
  pp_uint32 ID;
  bool inited;
};
	
class SampleEditor; // forward
class DialogSliders;                    
class DialogResponder;

class Synth
{

  private:
	int samplerate;
    MSynth *synth;
    MSynth synths[SYNTH_LAST+1];
    DialogSliders *sliders;

    SampleEditor *sampleEditor;
    PPScreen *screen;
    DialogResponder *dr;

  public:
    Synth(int samplerate);
    ~Synth();
    DialogSliders * dialog( SampleEditor *s, PPScreen *screen, DialogResponder *dr);

    void setParam( int i, float v);
    MSynthParam& getParam( int i ){ return synth->param[i]; }
    int getMaxParam(){ return synth->nparams; }
    
	// spec here: https://github.com/coderofsalvation/ASCIISYNTH
	PPString ASCIISynthExport( );
    bool ASCIISynthImport( PPString preset );

    void reset();
    void init();
    void process( MSynth *s, PPString *preset );
	TXMSample * prepareSample(pp_uint32 duration, bool force = false);

    // synths
    void CyclePaint( bool init = false );
    void FMPaint( bool init = false );

};

#endif