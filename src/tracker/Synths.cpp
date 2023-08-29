#include "Synth.h" 
#include "SynthFM.h"

#include "DialogSliders.h"
#include "FilterParameters.h"
#include "XModule.h"
#include "SampleEditor.h"
#include <math.h>

/*
 * Lightweight synths from the milkyverse.
 *
 * Rules for new synths:
 *   1. don't include new classes/libraries/FFT libraries e.g., KISS.
 *   2. re-use SampleEditor operations (recursive sample-editing, CyclePaint is an example)  
 *   3. please use volume as first slider and ADSR after that (if any)
 */


void Synth::init(){
	CyclePaint(true); 
	JamToyFM(true);
}

void Synth::process( MSynth *s, PPString *preset ){

	if( s == NULL ){  //fetch current synth from param 0
		pp_uint32 ID = (int)getParam(0).value;
		s = &(synths[ID]);
	}

	// if slider 0 changed, change synth + relaunch dialog
	if( s->ID != synth->ID ){     
		synth  = s;                 
		synth->param[0].value = (float)synth->ID;
		dialog(NULL,NULL,NULL);
	}

	switch( s->ID ){
		case SYNTH_CYCLE_PAINT:  CyclePaint();   break;
		case SYNTH_JAMTOY_FM:    JamToyFM();     break;
	}
}

void Synth::CyclePaint( bool init ){
	pp_int32 ID = SYNTH_CYCLE_PAINT;

	if( init ){
		synths[ID].nparams = 6;
		synths[ID].ID      = ID;
		synths[ID].param[0].name  = "cycle paint";
		synths[ID].param[0].value = 0.0f;
		synths[ID].param[0].min   = 0;
		synths[ID].param[0].max   = SYNTH_LAST;

		synths[ID].param[1].name  = "cycle amp";
		synths[ID].param[1].value = 70.0f;
		synths[ID].param[1].min   = 0;
		synths[ID].param[1].max   = (float)SYN_PARAM_MAX_VALUE;

		synths[ID].param[2].name  = "wave";
		synths[ID].param[2].value = 1.0f;
		synths[ID].param[2].min   = 1.0f;
		synths[ID].param[2].max   = 9.0f;

		synths[ID].param[3].name  = "harmonic";
		synths[ID].param[3].value = 0.0f;
		synths[ID].param[3].min   = 0;
		synths[ID].param[3].max   = (float)SYN_PARAM_MAX_VALUE;

		synths[ID].param[4].name  = "feedback";
		synths[ID].param[4].value = 0;
		synths[ID].param[4].min   = 0;
		synths[ID].param[4].max   = (float)SYN_PARAM_MAX_VALUE;

		synths[ID].param[5].name  = "volume";
		synths[ID].param[5].value = ((float)SYN_PARAM_MAX_VALUE)/2.0f;
		synths[ID].param[5].min   = 0;
		synths[ID].param[5].max   = (float)SYN_PARAM_MAX_VALUE;
		return;
	}

	// determine duration
	TXMSample *sample = sampleEditor->isEmptySample() ? prepareSample(100) : sampleEditor->getSample();

	// synthesize!
	FilterParameters parWave(2);
	parWave.setParameter(0, FilterParameters::Parameter( SYN_PARAM_NORMALIZE(synth->param[1].value) ));
	parWave.setParameter(1, FilterParameters::Parameter( 1.0f * (1.0f+synth->param[3].value) ) );
	switch( (int)synth->param[2].value ){
		case 1: sampleEditor->tool_generateSine(&parWave);         break;
		case 2: sampleEditor->tool_generateSquare(&parWave);       break;
		case 3: sampleEditor->tool_generateTriangle(&parWave);     break;
		case 4: sampleEditor->tool_generateSawtooth(&parWave);     break;
		case 5: sampleEditor->tool_generateHalfSine(&parWave);     break;
		case 6: sampleEditor->tool_generateAbsoluteSine(&parWave); break;
		case 7: 
		case 8:
		case 9: {
					parWave.setParameter(1, FilterParameters::Parameter( ((pp_int32)synth->param[2].value)-7 ) );
					sampleEditor->tool_generateNoise(&parWave);
					break;
				}
	}

	// scale volume
	float scale    = 2.0f * SYN_PARAM_NORMALIZE(synth->param[5].value);
	float foldback = 1.0f + synth->param[4].value*2.0f; 
	for( int i = 0; i < sample->samplen; i++ )
		sampleEditor->setFloatSampleInWaveform( i, sin( sampleEditor->getFloatSampleFromWaveform(i) * foldback ) * scale );

	// force loop 
	sampleEditor->setLoopType( 1 );
	sample->loopstart = 0;
	sample->looplen   = sample->samplen;

}

void Synth::JamToyFM( bool init ){
	pp_int32 ID = SYNTH_JAMTOY_FM;

	if( init ){
		synths[ID].nparams = 17;
		synths[ID].ID      = ID;
		synths[ID].param[0].name  = "FM JamToy";
		synths[ID].param[0].value = 0.0f;
		synths[ID].param[0].min   = 0;
		synths[ID].param[0].max   = SYNTH_LAST;

		synths[ID].param[1].name  = "volume";
		synths[ID].param[1].value = 38.0f;
		synths[ID].param[1].min   = 0.0f;
		synths[ID].param[1].max   = (float)SYN_PARAM_MAX_VALUE;

		synths[ID].param[2].name  = "size";
		synths[ID].param[2].value = 1.0f;
		synths[ID].param[2].min   = 1.0f;
		synths[ID].param[2].max   = 10.0f;

		synths[ID].param[3].name  = "attack";
		synths[ID].param[3].value = 0.0f;
		synths[ID].param[3].min   = 0;
		synths[ID].param[3].max   = (float)SYN_PARAM_MAX_VALUE;

		synths[ID].param[4].name  = "decay";
		synths[ID].param[4].value = 4.0f;
		synths[ID].param[4].min   = 1.0f;
		synths[ID].param[4].max   = (float)SYN_PARAM_MAX_VALUE;

		synths[ID].param[5].name  = "sustain";
		synths[ID].param[5].value = 4.0f;
		synths[ID].param[5].min   = 0;
		synths[ID].param[5].max   = (float)SYN_PARAM_MAX_VALUE;

		synths[ID].param[6].name  = "release";
		synths[ID].param[6].value = 2.0f;
		synths[ID].param[6].min   = 0;
		synths[ID].param[6].max   = (float)SYN_PARAM_MAX_VALUE;

		synths[ID].param[7].name  = "carrier freq";
		synths[ID].param[7].value = 66.0f;
		synths[ID].param[7].min   = 0;
		synths[ID].param[7].max   = (float)SYN_PARAM_MAX_VALUE;

		synths[ID].param[8].name  = "carrier wave";
		synths[ID].param[8].value = 1.0f;
		synths[ID].param[8].min   = 0;
		synths[ID].param[8].max   = 5.0f;

		synths[ID].param[9].name  = "mod freq";
		synths[ID].param[9].value = 73.0f;
		synths[ID].param[9].min   = 0;
		synths[ID].param[9].max   = (float)SYN_PARAM_MAX_VALUE;

		synths[ID].param[10].name  = "mod wave";
		synths[ID].param[10].value = 1.0f;
		synths[ID].param[10].min   = 0;
		synths[ID].param[10].max   = 5.0f; 

		synths[ID].param[11].name  = "mod amp";
		synths[ID].param[11].value = 79.0f; 
		synths[ID].param[11].min   = 0;
		synths[ID].param[11].max   = (float)SYN_PARAM_MAX_VALUE;

		synths[ID].param[12].name  = "mod type";
		synths[ID].param[12].value = 3.0f;
		synths[ID].param[12].min   = 0.0f;
		synths[ID].param[12].max   = 5.0f;

		synths[ID].param[13].name  = "pluck amp";
		synths[ID].param[13].value = 92.0f;
		synths[ID].param[13].min   = 0;
		synths[ID].param[13].max   = (float)SYN_PARAM_MAX_VALUE;

		synths[ID].param[14].name  = "pluck size";
		synths[ID].param[14].value = 8.0f;
		synths[ID].param[14].min   = 1.0f;
		synths[ID].param[14].max   = (float)SYN_PARAM_MAX_VALUE;

		synths[ID].param[15].name  = "feedback";
		synths[ID].param[15].value = 5.0f;
		synths[ID].param[15].min   = 0.0f;
		synths[ID].param[15].max   = (float)SYN_PARAM_MAX_VALUE;

		synths[ID].param[16].name  = "looptype";
		synths[ID].param[16].value = 0.0f;
		synths[ID].param[16].min   = 0.0f;
		synths[ID].param[16].max   = 3.0f;
		return;
	}

	// setup synth
	fm_control_t controls;
	fm_t instrument;
	pp_uint32 srate = 44100;

	switch( (int)synth->param[12].value ){
		case 0: controls.modulation = MODULATION_NONE;      break; 
		case 1: controls.modulation = MODULATION_AMPLITUDE; break;
		case 2: controls.modulation = MODULATION_FREQUENCY; break;
		case 3: controls.modulation = MODULATION_RING;      break;
		case 4: controls.modulation = MODULATION_TREMOLO;   break;
		case 5: controls.modulation = MODULATION_VIBRATO;   break;
	}

	// init carrier wave
	switch( (int)synth->param[8].value ){
		case 0: controls.carrier = OSCILLATOR_ZERO;     break;
		case 1: controls.carrier = OSCILLATOR_SINE;     break;
		case 2: controls.carrier = OSCILLATOR_SQUARE;   break;
		case 3: controls.carrier = OSCILLATOR_SAWTOOTH; break;
		case 4: controls.carrier = OSCILLATOR_TRIANGLE; break;
		case 5: controls.carrier = OSCILLATOR_NOISE;    break;
	}
	controls.carrier_amplitude = 1.0f;

	// init modulator wave
	switch( (int)synth->param[10].value ){
		case 0: controls.modulator = OSCILLATOR_ZERO;     break;
		case 1: controls.modulator = OSCILLATOR_SINE;     break;
		case 2: controls.modulator = OSCILLATOR_SQUARE;   break;
		case 3: controls.modulator = OSCILLATOR_SAWTOOTH; break;
		case 4: controls.modulator = OSCILLATOR_TRIANGLE; break;
		case 5: controls.modulator = OSCILLATOR_NOISE;    break;
	}
	controls.modulator_amplitude = SYN_PARAM_NORMALIZE( synth->param[11].value );
	controls.modulator_freq = NOTE2HZ( (int)synth->param[9].value );

	controls.attack  = SYN_PARAM_NORMALIZE(synth->param[3].value);
	controls.decay   = SYN_PARAM_NORMALIZE(synth->param[4].value);
	controls.sustain = SYN_PARAM_NORMALIZE(synth->param[5].value);
	controls.release = SYN_PARAM_NORMALIZE(synth->param[6].value) * 0.5f;

	controls.filter = FILTER_NONE;
	controls.filter_freq = 500.0;
	controls.filter_resonance = 0.5;
	controls.filter_gain = 0.5;

	controls.echo_delay = 1.0;
	controls.echo_feedback = 0.4;
	controls.echo_level = 0.5;

	instrument.modulator.phase = 0;
	instrument.carrier.phase = 0;

	// determine duration
	pp_uint32 samples = (srate/6) * (int)synth->param[2].value; // 300ms * param
	TXMSample *sample = prepareSample(samples,true);


	// exponential positive drive into sin() function (produces foldback/freq multiply)
	// see curve @ https://graphtoy.com/?f1(x,t)=max(0,(x*10*x*x)%20)%20+x&v1=true 
	float scale    = 2.0f * SYN_PARAM_NORMALIZE(synth->param[1].value);
	float feedback = 1.0f + (100.0f * SYN_PARAM_NORMALIZE(synth->param[15].value));

	// synthesize! 
	float x;
	for( pp_int32 i = 0; i < (int)samples; i++ ){

		// apply plucks to freq controllers (see pluck @ https://graphtoy.com/?f1(x,t)=-0.5*tanh((x*92)-3)+0.5&v1=true)
		pp_uint32 pluckSamples = (pp_uint32)( (float(srate)/100) * SYN_PARAM_NORMALIZE(synth->param[14].value ) ); 
		float offset   = (1.0f/(float)pluckSamples) * float(i);
		float pluckAmp = SYN_PARAM_NORMALIZE(synth->param[13].value) * float(srate/4); 
		float c_pluck  = fmax( 0, pluckAmp * (1.0f+(-offset*offset) ) );
		instrument.carrier.freq   = NOTE2HZ( (int)synth->param[7].value );
		instrument.carrier.freq   += c_pluck;
		SynthFM::instrument_control( &instrument, &controls, srate );

		// trigger note
		if( i == 0 ) SynthFM::adsr_trigger( &(instrument.adsr) );

		SynthFM::instrument_play( &instrument, srate, &x);
		sampleEditor->setFloatSampleInWaveform( i, sin( x * feedback ) * scale );
	}


	// force loop 
	pp_uint32 looptype = (pp_uint32)synth->param[16].value;
	sampleEditor->setLoopType( looptype );
	if( looptype > 0 ){
		sample->loopstart = 0;
		sample->looplen   = sample->samplen;
	}

	sampleEditor->notifyListener(SampleEditor::NotificationChanges); // update UI
}
