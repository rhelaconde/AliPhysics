///
/// \file  AliFemtoTrio.h
/// \author Jeremi Niedziela

#ifndef AliFemtoTrio_H
#define AliFemtoTrio_H

#include "AliFemtoParticle.h"
#include "AliFemtoTypes.h"

class AliFemtoTrio {
public:
  enum EPart { kKaonPlus , kKaonMinus , kPionPlus , kPionMinus , kUnknown};
  
  AliFemtoTrio();
  ~AliFemtoTrio();

  // track Gets:
  inline AliFemtoParticle* Track1(){return fTrack1;}
  inline AliFemtoParticle* Track2(){return fTrack2;}
  inline AliFemtoParticle* Track3(){return fTrack3;}
  
  inline EPart GetTrack1type(){return fTrack1type;}
  inline EPart GetTrack2type(){return fTrack2type;}
  inline EPart GetTrack3type(){return fTrack3type;}
  
  // track Sets:
  inline void SetTrack1(AliFemtoParticle *track, EPart type){fTrack1 = track;fTrack1type=type;}
  inline void SetTrack2(AliFemtoParticle *track, EPart type){fTrack2 = track;fTrack2type=type;}
  inline void SetTrack3(AliFemtoParticle *track, EPart type){fTrack3 = track;fTrack3type=type;}

  double MInv(); // returns invariant mass of the trio
  
private:
  AliFemtoParticle* fTrack1;  // first particle in the trio
  AliFemtoParticle* fTrack2;  // second particle in the trio
  AliFemtoParticle* fTrack3;  // third particle in the trio
  
  EPart fTrack1type;
  EPart fTrack2type;
  EPart fTrack3type;
};

#endif
