#ifndef SAMPLEDITOR_SCRIPT_H
#define SAMPLEDITOR_SCRIPT_H

/* 
 * A gateway to external scripting languages rather than embedding our own.
 * The idea is to allow ANY language to process samples, not embedding a full scripting language.
 * Choosing a trendy language X (Chaiscript/Lua/micropython/Javascript e.g.) would cause 
 * future compatibility/maintenance issues between a changing c++ codebase & scripts.
 * Lets forward wav-files to our own preferred scripting languages / cli tools.
 */

#include <stdio.h>
#include <stdlib.h>
#include "Tracker.h"
#include "ModuleEditor.h"
#include "ControlIDs.h"

#define BUFSIZE 255

class SampleEditorScript{
  private:
    FILE *f;
	char cdir[BUFSIZE];


  public:
	Tracker* tracker;
	ModuleEditor* moduleEditor;
    char err[BUFSIZE];
    int exec( PPString file, PPString fin, PPString fout ){
      char s[BUFSIZ]={ 0 };
      int l=0;
      f=fopen(file.getStrBuffer(),"r");
      if (f==0) {
        tracker->showMessageBoxSized(MESSAGEBOX_UNIVERSAL, "could not open script", Tracker::MessageBox_OK);
        return 1;
      }
      fgets(s,BUFSIZ,f);
      while ( l < BUFSIZ ) {
        if (s[l]=='\r'||s[l]=='\n'||s[l]==EOF) {
          s[l]='\0';
          break;
        }
        l++;
      }
      fclose(f);

      char launcher[BUFSIZ]={ 0 };
      char cmd[BUFSIZ]={ 0 };
      if(s[0]=='#' && s[1]=='!') {
        snprintf(launcher,BUFSIZ,"%s",s+2);
		if (strstr(launcher, "%s") != NULL) {
			snprintf(cmd, BUFSIZ, launcher, fin.getStrBuffer(), fout.getStrBuffer());
		}else {
			snprintf(cmd, BUFSIZ, "%s \"%s\" \"%s\" \"%s\"", launcher, file.getStrBuffer(), fin.getStrBuffer(), fout.getStrBuffer());
		}
		// remove last produced out.wav file since certain utilities don't like leftovers
		remove(fout.getStrBuffer());
		int ok = system(cmd);
		sprintf(err, ":( script error: %i", ok);
		if( ok != 0) tracker->showMessageBoxSized(MESSAGEBOX_UNIVERSAL, err, Tracker::MessageBox_OK);
		return ok;
      } else {
       tracker->showMessageBoxSized(MESSAGEBOX_UNIVERSAL, "script is missing shebang", Tracker::MessageBox_OK);
        return 1;
      }
    }

	int init(PPString fin, PPString fout) {
		// create new in.wav
		int selected_instrument;
		int selected_sample;
		tracker->getSelectedInstrument(&selected_instrument, &selected_sample);
		int res = moduleEditor->saveSample(
			fin, 
			selected_instrument, 
			selected_sample,
			ModuleEditor::SampleFormatTypeWAV
		);
		return res;
	}

	void update(PPString fin, PPString fout) {
		int selected_instrument;
		int selected_sample;
		
		// return if fout does not exist 
		FILE* ffout = fopen(fout.getStrBuffer(), "r");
		if ( ffout  == NULL) return;
		fclose(ffout);

		tracker->getSelectedInstrument(&selected_instrument, &selected_sample);
		moduleEditor->loadSample(
			fout,
			selected_instrument,
			selected_sample,
			ModuleEditor::SampleFormatTypeWAV
		);
	}
};

#endif
