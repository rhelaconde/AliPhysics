/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* AliAnaysisTaskMyTask
 *
 * empty task which can serve as a starting point for building an analysis
 * as an example, one histogram is filled
 */

#include <iostream>
#include "TChain.h"
#include "TH1F.h"
#include "TList.h"
#include "AliAnalysisTask.h"
#include "AliAnalysisManager.h"
#include "AliAODEvent.h"
#include "AliAODInputHandler.h"
#include "AliAnalysisTaskCaloHFEpPbRun2.h"
#include "AliPIDResponse.h"
#include "AliAODPid.h"
#include "AliPID.h"
#include "AliKFParticle.h"

//header for MC
#include "AliAODMCParticle.h"
#include "AliAODMCHeader.h"
#include "AliMCEventHandler.h"
#include "AliMCEvent.h"
#include "AliMCParticle.h"
#include "AliGenHijingEventHeader.h"
#include "AliGenPythiaEventHeader.h"

using std::cout;
using std::endl;

class AliAnalysisTaskCaloHFEpPbRun2;    // your analysis class

using namespace std;            // std namespace: so you can do things like 'cout'

ClassImp(AliAnalysisTaskCaloHFEpPbRun2) // classimp: necessary for root

//##################### Standard Cut Parameters (No Systematic Parameters) ##################### //
//---Event Cut
Int_t CutNcontV = 1;
Double_t CutZver = 10;
//---Track Cut
Int_t CutITSchi2 = 25;
Int_t CutTPCchi2perNDF = 4;
//---PID Cut
Double_t CutNsigmaH[1] = {-3.5};
//---Associate track cut
Double_t CutAssoTPCchi2perNDF = 4;
Double_t CutPairEEta[2] = {-0.9,0.9};
Double_t CutPairENsigma[2] = {-3.,3.};
//############################################################################################## //


AliAnalysisTaskCaloHFEpPbRun2::AliAnalysisTaskCaloHFEpPbRun2() : AliAnalysisTaskSE(),
    //----- Analysis Parameters -----
    TrackEtaLow(0),
    TrackEtaHigh(0),
    NTPCClust(0),
    NITSClust(0),
    NCrossedRow(0),
    DCAxy(0),
    DCAz(0),
    TrackMatchPhi(0),
    TrackMatchEta(0),
    NsigmaLow(0),
    NsigmaHigh(0),
    M20Low(0),
    M20High(0),
    EopLow(0),
    EopHigh(0),
    AssoMinpT(0),
    AssoNTPCClust(0),
    MassCut(0),
    fAOD(0),
    fOutputList(0),
    fVevent(0),
    fEventCuts(0),
    fPIDresponse(0),
    //-----Tender------
    fTracks_tender(0),
    fCaloClusters_tender(0),
    //------for MC-----
    fMCparticle(0),
    fMCheader(0),
    fMCarray(0),
    //------EMCal&DCal flag-----
    fFlagClsTypeEMCal(kTRUE),
    fFlagClsTypeDCal(kTRUE),
    //------EMCalCorrection flag-----
    fFlagEMCalCorrection(kTRUE),
    //------Trigger Selection flag-----
    fFlagEG1(kFALSE),
    fFlagEG2(kFALSE),
    fFlagDG1(kFALSE),
    fFlagDG2(kFALSE),
    //##################### Real Data ##################### //
    //-----Vertex------
    fvtxZ(0),
    fvtxZ_NoCut(0),
    fvtxZ_NcontCut(0),
    fNcontV(0),
    fNcontVSPD(0),
    //-----EMCal(&DCal) Cluster------
    fCaloClusterE(0),
    fCaloClusterEAfterMatch(0),
    fCaloClustEtaphi(0),
    fCaloClustEtaPhiAfterMatch(0),
    fCaloClustTiming(0),
    fCaloTrackDiff(0),
    //-----Track------
    fTrackPt(0),
    fTrackPtAfterMatch(0),
    fTrackphieta(0),
    fTrackphietaAfterMatch(0),
    fTrackCluster(0),
    fTrackChi2(0),
    fDCAxy(0),
    fDCAz(0),
    //-----PID plots-----
    fdEdx(0),
    fNSigmaTPC(0),
    fNsigmaEta(0),
    fNSigmaTPCelectron(0),
    fNSigmaTPChadron(0),
    fNsigmaEtaElectron(0),
    fM02(0),
    fM20(0),
    fEop(0),
    fNSigmaEop(0),
    fEopElectron(0),
    fEopHadron(0),
    fElectronphieta(0),
    //-----select photonic electron------
    fInvmassLS(0),
    fInvmassULS(0),
    fPHEpTLS(0),
    fPHEpTULS(0),

    //##################### Monte Carlo ##################### //
    //----- particle information -----
    fMCPDGcodeM(0),
    fMCPDGcodeGM(0),
    //----- efficiency correction -----
    fMCTrackPtelectron(0),
    fMCTrackPtelectron_reco(0),
    fMCTrackPtelectron_wTPCPID(0),
    fMCTrackPtelectron_wmatch(0),
    fMCTrackPtelectron_wEMCALPID(0),
    fMCTrackPtHFE(0),
    fMCTrackPtHFE_reco(0),
    fMCTrackPtHFE_wTPCPID(0),
    fMCTrackPtHFE_wmatch(0),
    fMCTrackPtHFE_wEMCALPID(0),
    fMCHFEResponceMatrix(0),
    // --- PID check ---
    fMCTPCNSigmaelectron(0),
    fMCNsigmaEtaElectron(0),
    fMCHFEEop(0),
    //----- DCA for c/b separation -----
    fMCDCAinclusive(0),
    fMCDCAconv(0),
    fMCDCAdalitz(0),
    fMCDCAcharm(0),
    fMCDCAbeauty(0),
    fMCDCAPHE(0),
    fMCDCAPHEULS(0),
    fMCDCAPHELS(0),
    //----- Non-HFE tagging efficiency -----
    PionWeight(0),
    EtaWeight(0),
    fMCPionInput(0),
    fMCEtaInput(0),
    fMCTrackPtPHEHijing(0),
    fMCTrackPtPHEnoweighting(0),
    fMCTrackPtPHEaftweighting(0),
    fMCTrackPtPHEHijingwPID(0),
    fMCTrackPtPHEHijingwMasscut(0),
    fMCTrackPtPHEwPIDnoweighting(0),
    fMCTrackPtPHEwPIDaftweighting(0),
    fMCTrackPtPHEwMasscutnoweighting(0),
    fMCTrackPtPHEwMasscutaftweighting(0),
    //----- D meson & B meson -----
    fMCHFhadronpT(0),
    //----- c->e & b->e -----
    fMCDdecayE(0),
    fMCBdecayE(0)
{
    // default constructor, don't allocate memory here!
    // this is used by root for IO purposes, it needs to remain empty
}

//_____________________________________________________________________________
AliAnalysisTaskCaloHFEpPbRun2::AliAnalysisTaskCaloHFEpPbRun2(const char* name) : AliAnalysisTaskSE(name),
    //----- Analysis Parameters -----
    TrackEtaLow(0),
    TrackEtaHigh(0),
    NTPCClust(0),
    NITSClust(0),
    NCrossedRow(0),
    DCAxy(0),
    DCAz(0),
    TrackMatchPhi(0),
    TrackMatchEta(0),
    NsigmaLow(0),
    NsigmaHigh(0),
    M20Low(0),
    M20High(0),
    EopLow(0),
    EopHigh(0),
    AssoMinpT(0),
    AssoNTPCClust(0),
    MassCut(0),
    fAOD(0),
    fOutputList(0),
    fVevent(0),
    fEventCuts(0),
    fPIDresponse(0),
    //-----Tender------
    fTracks_tender(0),
    fCaloClusters_tender(0),
    //------for MC-----
    fMCparticle(0),
    fMCheader(0),
    fMCarray(0),
    //------EMCal&DCal flag-----
    fFlagClsTypeEMCal(kTRUE),
    fFlagClsTypeDCal(kTRUE),
    //------EMCalCorrection flag-----
    fFlagEMCalCorrection(kTRUE),
    //------Trigger Selection flag-----
    fFlagEG1(kFALSE),
    fFlagEG2(kFALSE),
    fFlagDG1(kFALSE),
    fFlagDG2(kFALSE),
    //##################### Real Data ##################### //
    //-----Vertex------
    fvtxZ(0),
    fvtxZ_NoCut(0),
    fvtxZ_NcontCut(0),
    fNcontV(0),
    fNcontVSPD(0),
    //-----EMCal(&DCal) Cluster------
    fCaloClusterE(0),
    fCaloClusterEAfterMatch(0),
    fCaloClustEtaphi(0),
    fCaloClustEtaPhiAfterMatch(0),
    fCaloClustTiming(0),
    fCaloTrackDiff(0),
    //-----Track------
    fTrackPt(0),
    fTrackPtAfterMatch(0),
    fTrackphieta(0),
    fTrackphietaAfterMatch(0),
    fTrackCluster(0),
    fTrackChi2(0),
    fDCAxy(0),
    fDCAz(0),
    //-----PID plots-----
    fdEdx(0),
    fNSigmaTPC(0),
    fNsigmaEta(0),
    fNSigmaTPCelectron(0),
    fNSigmaTPChadron(0),
    fNsigmaEtaElectron(0),
    fM02(0),
    fM20(0),
    fEop(0),
    fNSigmaEop(0),
    fEopElectron(0),
    fEopHadron(0),
    fElectronphieta(0),
    //-----select photonic electron------
    fInvmassLS(0),
    fInvmassULS(0),
    fPHEpTLS(0),
    fPHEpTULS(0),
    //##################### Monte Carlo ##################### //
    //----- particle information -----
    fMCPDGcodeM(0),
    fMCPDGcodeGM(0),
    //----- efficiency correction -----
    fMCTrackPtelectron(0),
    fMCTrackPtelectron_reco(0),
    fMCTrackPtelectron_wTPCPID(0),
    fMCTrackPtelectron_wmatch(0),
    fMCTrackPtelectron_wEMCALPID(0),
    fMCTrackPtHFE(0),
    fMCTrackPtHFE_reco(0),
    fMCTrackPtHFE_wTPCPID(0),
    fMCTrackPtHFE_wmatch(0),
    fMCTrackPtHFE_wEMCALPID(0),
    fMCHFEResponceMatrix(0),
    // --- PID check ---
    fMCTPCNSigmaelectron(0),
    fMCNsigmaEtaElectron(0),
    fMCHFEEop(0),
    //----- DCA for c/b separation -----
    fMCDCAinclusive(0),
    fMCDCAconv(0),
    fMCDCAdalitz(0),
    fMCDCAcharm(0),
    fMCDCAbeauty(0),
    fMCDCAPHE(0),
    fMCDCAPHEULS(0),
    fMCDCAPHELS(0),
    //----- Non-HFE tagging efficiency -----
    PionWeight(0),
    EtaWeight(0),
    fMCPionInput(0),
    fMCEtaInput(0),
    fMCTrackPtPHEHijing(0),
    fMCTrackPtPHEnoweighting(0),
    fMCTrackPtPHEaftweighting(0),
    fMCTrackPtPHEHijingwPID(0),
    fMCTrackPtPHEHijingwMasscut(0),
    fMCTrackPtPHEwPIDnoweighting(0),
    fMCTrackPtPHEwPIDaftweighting(0),
    fMCTrackPtPHEwMasscutnoweighting(0),
    fMCTrackPtPHEwMasscutaftweighting(0),
    //----- D meson & B meson -----//
    fMCHFhadronpT(0),
    //----- c->e & b->e -----
    fMCDdecayE(0),
    fMCBdecayE(0)
{
    // constructor
    DefineInput(0, TChain::Class());    // define the input of the analysis: in this case we take a 'chain' of events
                                        // this chain is created by the analysis manager, so no need to worry about it,
                                        // it does its work automatically
    DefineOutput(1, TList::Class());    // define the ouptut of the analysis: in this case it's a list of histograms
                                        // you can add more output objects by calling DefineOutput(2, classname::Class())
                                        // if you add more output objects, make sure to call PostData for all of them, and to
                                        // make changes to your AddTask macro!
}
//_____________________________________________________________________________
AliAnalysisTaskCaloHFEpPbRun2::~AliAnalysisTaskCaloHFEpPbRun2()
{
    // destructor
    if(fOutputList) {
        delete fOutputList;     // at the end of your task, it is deleted from memory by calling this function
    }
    delete fTracks_tender;
    delete fCaloClusters_tender;
}
//_____________________________________________________________________________
void AliAnalysisTaskCaloHFEpPbRun2::UserCreateOutputObjects()
{
    // create output objects
    //
    // this function is called ONCE at the start of your analysis (RUNTIME)
    // here you ceate the histograms that you want to use
    //
    // the histograms are in this case added to a tlist, this list is in the end saved
    // to an output file

    //
    fOutputList = new TList();          // this is a list which will contain all of your histograms
                                        // at the end of the analysis, the contents of this list are written
                                        // to the output file
    fOutputList->SetOwner(kTRUE);       // memory stuff: the list is owner of all objects it contains and will delete them
                                        // if requested (dont worry about this now)
    fEventCuts = new AliEventCuts();


    //-----Vertex------
    fvtxZ = new TH1F("fvtxZ","Z vertex position;Vtx_{z} (cm.);counts",300,-30,30);
    fOutputList -> Add(fvtxZ);

    fvtxZ_NoCut = new TH1F("fvtxZ_NoCut","Z vertex position (No cut);Vtx_{z} (cm.);counts",300,-30,30);
    fOutputList -> Add(fvtxZ_NoCut);

    fvtxZ_NcontCut = new TH1F("fvtxZ_NcontCut",Form("Z vertex position, Ncont_{global} >= %d);Vtx_{z} (cm.);counts",CutNcontV),300,-30,30);
    fOutputList -> Add(fvtxZ_NcontCut);

    // fNcontV = new TH1F("fNcontV","Number of contributors (ITS+TPC);# of contributors;counts",500,0,500);
    // fOutputList -> Add(fNcontV);
    //
    // fNcontVSPD = new TH1F("fNcontVSPD","Number of contributors (SPD);# of contributors;counts",500,0,500);
    // fOutputList -> Add(fNcontVSPD);

    //-----EMCal(&DCal) Cluster------
    fCaloClusterE = new TH1F("fCaloClusterE", "cluster energy distribution;Energy (GeV.);counts",500,0,50);
    fOutputList->Add(fCaloClusterE);

    fCaloClusterEAfterMatch = new TH1F("fCaloClusterEAfterMatch", "cluster energy distribution w/trackmatch);Energy (GeV.);counts",500,0,50);
    fOutputList->Add(fCaloClusterEAfterMatch);

    fCaloClustEtaphi = new TH2F("fCaloClustEtaphi","cluster #phi-#eta distribution;#eta;#phi",100,-0.9,0.9,200,0,6.3);
    fOutputList -> Add(fCaloClustEtaphi);

    fCaloClustEtaPhiAfterMatch = new TH2F("fCaloClustEtaPhiAfterMatch","cluster #phi-#eta distribution w/trackmatch;#eta;#phi",100,-0.9,0.9,200,0,6.3);
    fOutputList -> Add(fCaloClustEtaPhiAfterMatch);

    fCaloClustTiming = new TH2F("fCaloClustTiming",Form("Cluster timing, %3.2f < n_{#sigma}^{TPC} < %3.2f;energy (GeV);t (ns)",NsigmaLow,NsigmaHigh),100,0,50,500,-100,100);
    fOutputList -> Add(fCaloClustTiming);

    fCaloTrackDiff = new TH2F("fCaloTrackDiff","cluster-track diff;#Delta #phi;#Delta #eta",300,-0.06,0.06,300,-0.06,0.06);
    fOutputList -> Add(fCaloTrackDiff);

    //-----Track------
    fTrackPt = new TH1F("fTrackPt","p_{T} ditribution;p_{T} (GeV/c.);counts",500,0,50);       // create your histogra
    fOutputList->Add(fTrackPt);          // don't forget to add it to the list! the list will be written to file, so if you want
                                        // your histogram in the output file, add it to the list!
    fTrackPtAfterMatch = new TH1F("fTrackPtAfterMatch", "p_{T} ditribution w/Colomatch;p_{T} (GeV/c.);counts", 500,0,50);
    fOutputList->Add(fTrackPtAfterMatch);

    fTrackphieta = new TH2F("fTrackphieta","Charged track #phi-#eta;#eta;#phi",100,-0.9,0.9,200,0,6.3);
    fOutputList -> Add(fTrackphieta);

    fTrackphietaAfterMatch = new TH2F("fTrackphietaAfterMatch","Calo matched track #phi-#eta;#eta;#phi",100,-0.9,0.9,200,0,6.3);
    fOutputList ->  Add(fTrackphietaAfterMatch);

    fDCAxy = new TH2F("fDCAxy","DCAxy distribution;p_{T} (GeV/c.);DCA #times charge (cm.)",500,0,50,400,-0.2,0.2);
    fOutputList -> Add(fDCAxy);

    fDCAz = new TH2F("fDCAz","DCAz distribution;p_{T} (GeV/c.);DCAz (cm.)",500,0,50,400,-0.2,0.2);
    fOutputList -> Add(fDCAz);

    fTrackCluster = new TH2F("fTrackCluster","Track Clusters;;# of clusters",3,0,3,165,0,165);
    fTrackCluster -> GetXaxis() -> SetBinLabel(1,"ITS cluster");
    fTrackCluster -> GetXaxis() -> SetBinLabel(2,"TPC cluster");
    fTrackCluster -> GetXaxis() -> SetBinLabel(3,"Crossed rows");
    fOutputList -> Add(fTrackCluster);

    fTrackChi2 = new TH2F("fTrackChi2","Track #chi^{2}",2,0,2,200,0,100);
    fTrackChi2 -> GetXaxis() -> SetBinLabel(1,"ITS #chi^{2}");
    fTrackChi2 -> GetXaxis() -> SetBinLabel(2,"TPC #chi^{2}/ndf");
    fOutputList -> Add(fTrackChi2);

    //-----PID plots------
    // fdEdx = new TH2F("fdEdx","dE/dx vs. p;p (GeV/c.);dE/dx",500,0,50,500,0,160);
    // fOutputList -> Add(fdEdx);

    fNSigmaTPC = new TH2F("fNSigmaTPC","n_{#sigma}^{TPC} vs. p;p (GeV/c.);n_{#sigma}^{TPC}",500,0,50,500,-10,10);
    fOutputList -> Add(fNSigmaTPC);

    fNsigmaEta = new TH2F("fNsigmaEta","n_{#sigma}^{TPC};#eta;n_{#sigma}^{TPC}",100,-0.9,0.9,500,-10,10);
    fOutputList -> Add(fNsigmaEta);

    fNSigmaTPCelectron = new TH2F("fNSigmaTPCelectron","n_{#sigma}^{TPC};p_{T} (GeV/c);n_{#sigma}^{TPC}",500,0,50,500,-10,10);
    fOutputList -> Add(fNSigmaTPCelectron);

    fNSigmaTPChadron = new TH2F("fNSigmaTPChadron","n_{#sigma}^{TPC};p_{T} (GeV/c);n_{#sigma}^{TPC}",500,0,50,500,-10,10);
    fOutputList -> Add(fNSigmaTPChadron);

    fNsigmaEtaElectron = new TH2F("fNsigmaEtaElectron","n_{#sigma}^{TPC};#eta;n_{#sigma}^{TPC}",100,-0.9,0.9,500,-10,10);
    fOutputList -> Add(fNsigmaEtaElectron);

    fM02 = new TH2F("fM02","M02 vs p_{T} distribution;p_{T} (GeV/c.);long axis of ellipse (cm.)",500,0,50,400,0,2);
    fOutputList -> Add(fM02);

    fM20 = new TH2F("fM20","M20 vs p_{T} distribution;p_{T} (GeV/c.);short axis of ellipse (cm.)",500,0,50,400,0,2);
    fOutputList -> Add(fM20);

    fEop = new TH2F("fEop","E/p distribution;p_{T} (GeV/c.);E/p",500,0,50,300,0.,3.);
    fOutputList -> Add(fEop);

    fNSigmaEop = new TH2F("fNSigmaEop","E/p vs. n_{#sigma}^{TPC}, p_{T} > 3 GeV/c.;E/p;#sigma_{TPC-dE/dx}",300,0.,3.,200,-10,10);
    fOutputList -> Add(fNSigmaEop);

    // fEopElectron = new TH2F("fEopElectron",Form("E/p vs. p_{T}, %3.1f < n_{#sigma}^{TPC} < %3.1f;p_{T} (GeV/c.);E/p",CutNsigmaE[0],CutNsigmaE[1]),500,0,50,300,0.,3.);
    fEopElectron = new TH2F("fEopElectron",Form("E/p vs. p_{T}, %3.2f < n_{#sigma}^{TPC} < %3.2f;p_{T} (GeV/c.);E/p",NsigmaLow,NsigmaHigh),500,0,50,300,0.,3.);
    fOutputList -> Add(fEopElectron);

    fEopHadron = new TH2F("fEopHadron",Form("E/p vs. p_{T}, n_{#sigma}^{TPC} < %3.1f;p_{T} (GeV/c.);E/p",CutNsigmaH[0]),500,0,50,300,0.,3.);
    fOutputList -> Add(fEopHadron);

    fElectronphieta = new TH2F("fElectronphieta","Charged track #phi-#eta;#eta;#phi",100,-0.9,0.9,200,0,6.3);
    fOutputList -> Add(fElectronphieta);

    //-----select photonic electron------
    fInvmassLS = new TH2F("fInvmassLS","Invariant-mass (like-sign);p_{T} (GeV/c.);M_{ee} (GeV/c^{2}.)",500,0,50,500,0,0.5);
    fOutputList -> Add(fInvmassLS);

    fInvmassULS = new TH2F("fInvmassULS","Invariant-mass (unlike-sign);p_{T} (GeV/c.);M_{ee} (GeV/c^{2}.)",500,0,50,500,0,0.5);
    fOutputList -> Add(fInvmassULS);

    fPHEpTLS = new TH1F("fPHEpTLS",Form("electros p_{T},M_{ee} < %3.2f (GeV/c^{2}), like-sign;p_{T} (GeV/c.);counts",MassCut),500,0,50);
    fOutputList -> Add(fPHEpTLS);

    fPHEpTULS = new TH1F("fPHEpTULS",Form("electros p_{T},M_{ee} < %3.2f (GeV/c^{2}), Unlike-sign;p_{T} (GeV/c.);counts",MassCut),500,0,50);
    fOutputList -> Add(fPHEpTULS);

    //----- MC info ------
    if(fFlagMC)
    {
      //----- particle information ------
      fMCPDGcodeM = new TH1F("fMCPDGcodeM","PDG code of mother particle;PDG code;counts;",1000,0,1000);
      fOutputList -> Add(fMCPDGcodeM);

      fMCPDGcodeGM = new TH1F("fMCPDGcodeGM","PDG code of mother particle;PDG code;counts;",1000,0,1000);
      fOutputList -> Add(fMCPDGcodeGM);

      //----- efficiency correction ------
      fMCTrackPtelectron = new TH1F("fMCTrackPtelectron","MC track p_{T} all electron;p_{T} (GeV/c.);counts",500,0,50);
      fOutputList -> Add(fMCTrackPtelectron);

      fMCTrackPtelectron_reco = new TH1F("fMCTrackPtelectron_reco","MC track p_{T} reconstructed electron;p_{T} (GeV/c.);counts",500,0,50);
      fOutputList -> Add(fMCTrackPtelectron_reco);

      fMCTrackPtelectron_wTPCPID = new TH1F("fMCTrackPtelectron_wTPCPID","MC track p_{T} electron w/TPCPID;p_{T} (GeV/c.);counts",500,0,50);
      fOutputList -> Add(fMCTrackPtelectron_wTPCPID);

      fMCTrackPtelectron_wmatch = new TH1F("fMCTrackPtelectron_wmatch","MC track p_{T} EMCal matched electron;p_{T} (GeV/c.);counts",500,0,50);
      fOutputList -> Add(fMCTrackPtelectron_wmatch);

      fMCTrackPtelectron_wEMCALPID = new TH1F("fMCTrackPtelectron_wEMCALPID","MC track p_{T} electron w/EMCalPID;p_{T} (GeV/c.);counts",500,0,50);
      fOutputList -> Add(fMCTrackPtelectron_wEMCALPID);

      fMCTrackPtHFE = new TH1F("fMCTrackPtHFE","MC track p_{T} all HFE;p_{T} (GeV/c.);counts",500,0,50);
      fOutputList -> Add(fMCTrackPtHFE);

      fMCTrackPtHFE_reco = new TH1F("fMCTrackPtHFE_reco","MC track p_{T} reconstructed HFE;p_{T} (GeV/c.);counts",500,0,50);
      fOutputList -> Add(fMCTrackPtHFE_reco);

      fMCTrackPtHFE_wTPCPID = new TH1F("fMCTrackPtHFE_wTPCPID","MC track p_{T} HFE w/TPCPID;p_{T} (GeV/c.);counts",500,0,50);
      fOutputList -> Add(fMCTrackPtHFE_wTPCPID);

      fMCTrackPtHFE_wmatch = new TH1F("fMCTrackPtHFE_wmatch","MC track p_{T} EMCal matched HFE;p_{T} (GeV/c.);counts",500,0,50);
      fOutputList -> Add(fMCTrackPtHFE_wmatch);

      fMCTrackPtHFE_wEMCALPID = new TH1F("fMCTrackPtHFE_wEMCALPID","MC track p_{T} HFE w/EMCalPID;p_{T} (GeV/c.);counts",500,0,50);
      fOutputList -> Add(fMCTrackPtHFE_wEMCALPID);

      // --- Responce Matriz --- //
      Double_t pTbin[16] = {0.,1.5,2.,3.,4.,5.,6.,8.,10.,12.,14.,16.,20.,26.,35.,50};
      fMCHFEResponceMatrix = new TH2F("fMCHFEResponceMatrix","HFE Responce Matrix;p_{T}^{gene} (GeV/c);p_{T}^{reco} (GeV/c)",15,pTbin,15,pTbin);
      fOutputList -> Add(fMCHFEResponceMatrix);

      // --- PID check --- //
      fMCTPCNSigmaelectron = new TH2F("fMCTPCNSigmaelectron","n_{#sigma}^{TPC} (|PDG| = 11);p_{T} (GeV/c);n_{#sigma}^{TPC}",500,0,50,500,-10,10);
      fOutputList -> Add(fMCTPCNSigmaelectron);

      fMCNsigmaEtaElectron = new TH2F("fMCNsigmaEtaElectron","n_{#sigma}^{TPC} (|PDG| = 11);#eta;n_{#sigma}^{TPC}",100,-0.9,0.9,500,-10,10);
      fOutputList -> Add(fMCNsigmaEtaElectron);

      fMCHFEEop = new TH2F("fMCHFEEop","PYTHIA e^{HF} E/p vs p_{T};p_{T} (GeV/c);E/p",500,0,50,300,0,3);
      fOutputList -> Add(fMCHFEEop);

      //----- DCA for c/b separation ------
      // fMCDCAinclusive = new TH2F("fMCDCAinclusive","inclusive electrons;p_{T} (GeV/c.);DCA #times charge (cm.)",500,0,50,400,-0.2,0.2);
      // fOutputList -> Add(fMCDCAinclusive);
      //
      // fMCDCAconv = new TH2F("fMCDCAconv","conversion electrons;p_{T} (GeV/c.);DCA #times charge (cm.)",500,0,50,400,-0.2,0.2);
      // fOutputList -> Add(fMCDCAconv);
      //
      // fMCDCAdalitz = new TH2F("fMCDCAdalitz","Dalitz decay electrons;p_{T} (GeV/c.);DCA #times charge (cm.)",500,0,50,400,-0.2,0.2);
      // fOutputList -> Add(fMCDCAdalitz);
      //
      // fMCDCAcharm = new TH2F("fMCDCAcharm","charm hadrons decay electrons;p_{T} (GeV/c.);DCA #times charge (cm.)",500,0,50,400,-0.2,0.2);
      // fOutputList -> Add(fMCDCAcharm);
      //
      // fMCDCAbeauty = new TH2F("fMCDCAbeauty","beauty hadrons decay electrons;p_{T} (GeV/c.);DCA #times charge (cm.)",500,0,50,400,-0.2,0.2);
      // fOutputList -> Add(fMCDCAbeauty);
      //
      // fMCDCAPHE = new TH2F("fMCDCAPHE","PHE w/PID before mass method;p_{T} (GeV/c.);DCA #times charge (cm.)",500,0,50,400,-0.2,0.2);
      // fOutputList -> Add(fMCDCAPHE);
      //
      // fMCDCAPHEULS = new TH2F("fMCDCAPHEULS","ULS-pair w/ mass cut;p_{T} (GeV/c.);DCA #times charge (cm.)",500,0,50,400,-0.2,0.2);
      // fOutputList -> Add(fMCDCAPHEULS);
      //
      // fMCDCAPHELS = new TH2F("fMCDCAPHELS","ULS-pair w/ mass cut;p_{T} (GeV/c.);DCA #times charge (cm.)",500,0,50,400,-0.2,0.2);
      // fOutputList -> Add(fMCDCAPHELS);

      //----- Non-HFE tagging efficiency ------
      Bool_t fFlagweightfunc = kTRUE;
      if(fFlagweightfunc)
      {
        PionWeight = new TF1("PionWeight","[0]/pow(exp(-x/[1]-x*x/[2])+x/[3],[4])",0,50);
        // LHC17i5a //
        //PionWeight -> SetParameters(11714.8,-7.77972,27.3542,0.977768,5.39899);
        // LHC17i5b //
        PionWeight -> SetParameters(10461,-8.99845,30.1231,0.994821,5.38202);
      }
      else PionWeight = new TF1("PionWeight","1",0,50);
      fOutputList -> Add(PionWeight);

      if(fFlagweightfunc)
      {
        EtaWeight = new TF1("EtaWeight","[0]/pow(exp(-x/[1]-x*x/[2])+x/[3],[4])",0,50);
        // LHC17i5a //
        //EtaWeight -> SetParameters(1342.46,-12.351,44.6781,1.53057,5.82068);
        // LHC17i5b //
        EtaWeight -> SetParameters(1484.13,-12.3383,42.4992,1.39306,5.60478);
      }
      else EtaWeight = new TF1("EtaWeight","1",0,50);
      fOutputList -> Add(EtaWeight);

      fMCPionInput = new TH2F("fMCPionInput","MC #pi^{0};;p_{T} (GeV/c)",4,0,4,500,0,50);
      fMCPionInput -> GetXaxis() -> SetBinLabel(1,"all");
      fMCPionInput -> GetXaxis() -> SetBinLabel(2,"Hijing");
      fMCPionInput -> GetXaxis() -> SetBinLabel(3,"embedding");
      fMCPionInput -> GetXaxis() -> SetBinLabel(4,"others");
      fOutputList -> Add(fMCPionInput);

      fMCEtaInput = new TH2F("fMCEtaInput","MC #eta;;p_{T} (GeV/c)",4,0,4,500,0,50);
      fMCEtaInput -> GetXaxis() -> SetBinLabel(1,"all");
      fMCEtaInput -> GetXaxis() -> SetBinLabel(2,"Hijing");
      fMCEtaInput -> GetXaxis() -> SetBinLabel(3,"embedding");
      fMCEtaInput -> GetXaxis() -> SetBinLabel(4,"others");
      fOutputList -> Add(fMCEtaInput);

      fMCTrackPtPHEHijing = new TH1F("fMCTrackPtPHEHijing","MC track p_{T} PHE from Hijing;p_{T} (GeV/c.);counts",500,0,50);
      fOutputList -> Add(fMCTrackPtPHEHijing);

      fMCTrackPtPHEnoweighting = new TH1F("fMCTrackPtPHEnoweighting","MC track p_{T} PHE;p_{T} (GeV/c.);counts",500,0,50);
      fMCTrackPtPHEnoweighting -> Sumw2();
      fOutputList -> Add(fMCTrackPtPHEnoweighting);

      fMCTrackPtPHEaftweighting = new TH1F("fMCTrackPtPHEaftweighting","MC track p_{T} PHE w/PHE weighting;p_{T} (GeV/c.);counts",500,0,50);
      fMCTrackPtPHEaftweighting -> Sumw2();
      fOutputList -> Add(fMCTrackPtPHEaftweighting);

      fMCTrackPtPHEHijingwPID = new TH1F("fMCTrackPtPHEHijingwPID","Photonic electrons from Hijing w/PID;p_{T} (GeV/c.);counts",500,0,50);
      fMCTrackPtPHEHijingwPID -> Sumw2();
      fOutputList -> Add(fMCTrackPtPHEHijingwPID);

      fMCTrackPtPHEHijingwMasscut = new TH1F("fMCTrackPtPHEHijingwMasscut","Photonic electrons from Hijing w/PID & w/masscut;p_{T} (GeV/c.);counts",500,0,50);
      fMCTrackPtPHEHijingwMasscut -> Sumw2();
      fOutputList -> Add(fMCTrackPtPHEHijingwMasscut);

      fMCTrackPtPHEwPIDnoweighting = new TH1F("fMCTrackPtPHEwPIDnoweighting","Photonic electrons p_{T} w/PID & wo/weight;p_{T} (GeV/c.);counts",500,0,50);
      fMCTrackPtPHEwPIDnoweighting -> Sumw2();
      fOutputList -> Add(fMCTrackPtPHEwPIDnoweighting);

      fMCTrackPtPHEwPIDaftweighting = new TH1F("fMCTrackPtPHEwPIDaftweighting","Photonic electrons p_{T} w/PID & w/weight;p_{T} (GeV/c.);counts",500,0,50);
      fMCTrackPtPHEwPIDaftweighting -> Sumw2();
      fOutputList -> Add(fMCTrackPtPHEwPIDaftweighting);

      fMCTrackPtPHEwMasscutnoweighting = new TH1F("fMCTrackPtPHEwMasscutnoweighting","Photonic electrons p_{T} w/PID & w/masscut & wo/weight;p_{T} (GeV/c.);counts",500,0,50);
      fMCTrackPtPHEwMasscutnoweighting -> Sumw2();
      fOutputList -> Add(fMCTrackPtPHEwMasscutnoweighting);

      fMCTrackPtPHEwMasscutaftweighting = new TH1F("fMCTrackPtPHEwMasscutaftweighting","Photonic electrons p_{T} w/PID & w/masscut & w/weight;p_{T} (GeV/c.);counts",500,0,50);
      fMCTrackPtPHEwMasscutaftweighting -> Sumw2();
      fOutputList -> Add(fMCTrackPtPHEwMasscutaftweighting);

      //----- D meson & B meson -----
      fMCHFhadronpT = new TH2F("fMCHFhadronpT","heavy-flavor hadrons;;p_{T} (GeV/c)",4,0,4,500,0,50);
      fMCHFhadronpT -> GetXaxis() -> SetBinLabel(1,"Hjjing D");
      fMCHFhadronpT -> GetXaxis() -> SetBinLabel(2,"Hijing B");
      fMCHFhadronpT -> GetXaxis() -> SetBinLabel(3,"embedding D");
      fMCHFhadronpT -> GetXaxis() -> SetBinLabel(4,"embedding B");
      fOutputList -> Add(fMCHFhadronpT);

      //----- c->e & b->e -----
      fMCDdecayE = new TH1F("fMCDdecayE","MC electrons form charm hadron decays;p_{T} (GeV/c.);counts.",500,0,50);
      fOutputList -> Add(fMCDdecayE);

      fMCBdecayE = new TH1F("fMCBdecayE","MC electrons form beauty hadron decays;p_{T} (GeV/c.);counts.",500,0,50);
      fOutputList -> Add(fMCBdecayE);
    }
    //-----Others------

    PostData(1, fOutputList);           // postdata will notify the analysis manager of changes / updates to the
                                        // fOutputList object. the manager will in the end take care of writing your output to file
                                        // so it needs to know what's in the output
}
//_____________________________________________________________________________
void AliAnalysisTaskCaloHFEpPbRun2::UserExec(Option_t *)
{

  Bool_t Isdebug = kFALSE;
  //##################### Systematic Parameters ##################### //
  //---Track Cut
  Double_t CutTrackEta[2] = {TrackEtaLow,TrackEtaHigh};
  Int_t CutTPCNCls = NTPCClust;
  Int_t CutITSNCls = NITSClust;
  Int_t CutNCrossedRow = NCrossedRow;
  Double_t CutDCAxy = DCAxy;
  Double_t CutDCAz = DCAz;
  Double_t CutPhidiff = TrackMatchPhi;
  Double_t CutEtadiff = TrackMatchEta;
  //---PID Cut
  Double_t CutNsigmaE[2] = {NsigmaLow,NsigmaHigh};
  Double_t CutM20[2] = {M20Low,M20High};
  Double_t CutEopE[2] = {EopLow,EopHigh};
  //---Associate track cut
  Double_t CutPairEPt = AssoMinpT;
  Int_t CutAssoTPCNCls = AssoNTPCClust;
  Double_t CutPhotEMass = MassCut;
  //################################################################# //

    Bool_t IsPamaCheck = kTRUE;
    if(IsPamaCheck)
    {
      cout << " ############### Analysis Parameters ###############" << endl;
      cout << " MC analysis >> " << fFlagMC << endl;
      cout << " EMCal Correction >> " << fFlagEMCalCorrection << endl;
      cout << " --- Track cut ---" << endl;
      cout << " Rapidity : " << CutTrackEta[0] << " <  y < " << CutTrackEta[1] << endl;
      cout << " # of TPC cluster = " << CutTPCNCls << endl;
      cout << " # of ITS cluster = " << CutITSNCls << endl;
      cout << " # of CrossedRow = " << CutITSNCls << endl;
      cout << " DCAxy = " << CutDCAxy << ", DCAz = " << CutDCAz << endl;
      cout << " Delta phi = " << CutPhidiff << ", Delta Eta = " << CutEtadiff << endl;
      cout << " --- electron PID ---" << endl;
      cout << " Nsigma : " << CutNsigmaE[0] << " < n sigma < " << CutNsigmaE[1] << endl;
      cout << " M20 :" << CutM20[0] << " < M20 < " << CutM20[1] << endl;
      cout << " E/p : " << CutEopE[0] << " < E/p < " << CutEopE[1] << endl;
      cout << " --- Photonic method ---" << endl;
      cout << " min pT = " << CutPairEPt << endl;
      cout << " # of TPC cluster = " << CutAssoTPCNCls << endl;
      cout << " mass = " << CutPhotEMass << endl;
      cout << " ###################################################" << endl;
    }

    if(Isdebug) cout << "Event loop start" << endl;
    // user exec
    // this function is called once for each event
    // the manager will take care of reading the events from file, and with the static function InputEvent() you
    // have access to the current event.
    // once you return from the UserExec function, the manager will retrieve the next event from the chain
    fAOD = dynamic_cast<AliAODEvent*>(InputEvent());    // get an event (called fAOD) from the input file
                                                        // there's another event format (ESD) which works in a similar wya
                                                        // but is more cpu/memory unfriendly. for now, we'll stick with aod's
    if(!fAOD) return;                                   // if the pointer to the event is empty (getting it failed) skip this event
        // example part: i'll show how to loop over the tracks in an event
        // and extract some information from them which we'll store in a histogram

    fMCarray = dynamic_cast<TClonesArray*>(fAOD->FindListObject(AliAODMCParticle::StdBranchName()));
    fMCheader = dynamic_cast<AliAODMCHeader*>(fAOD->GetList()->FindObject(AliAODMCHeader::StdBranchName()));

    fVevent = dynamic_cast<AliVEvent*>(InputEvent());
    if (!fVevent)
    {
        printf("ERROR: fVEvent not available\n");
        return;
    }

    // --- Switch for EMCal Correction --- //
    if(fFlagEMCalCorrection)
    {
      fTracks_tender = dynamic_cast<TClonesArray*>(InputEvent()->FindListObject("tracks"));
      fCaloClusters_tender = dynamic_cast<TClonesArray*>(InputEvent()->FindListObject("caloClusters"));
    }

    // --- PID initialized --- //
    fPIDresponse = fInputHandler -> GetPIDResponse();

    //########################## Trigger Selection ##########################//
    TString firedTrigger;
    TString TriggerEG1 = "EG1", TriggerEG2 = "EG2", TriggerDG1 = "DG1", TriggerDG2 = "DG2";
    fVevent -> GetFiredTriggerClasses();
    if(fAOD) firedTrigger = fAOD -> GetFiredTriggerClasses();
    // --- EMCAL + DCAL analysis --- //
    if(fFlagEG2 && fFlagDG2) if(!firedTrigger.Contains(TriggerEG2) && !firedTrigger.Contains(TriggerDG2)) return;
    if(fFlagEG1 && fFlagDG1) if(!firedTrigger.Contains(TriggerEG1) && !firedTrigger.Contains(TriggerDG1)) return;
    // --- separate EMCAL and DCAL --- //
    // if(fFlagEG1) if(!firedTrigger.Contains(TriggerEG1)) return;
    // if(fFlagEG2) if(!firedTrigger.Contains(TriggerEG2)) return;
    // if(fFlagDG1) if(!firedTrigger.Contains(TriggerDG1)) return;
    // if(fFlagDG2) if(!firedTrigger.Contains(TriggerDG2)) return;
    //#######################################################################//

    //########################## Event Selection ##########################//
    // --- SPD Vtx --- //
    const AliVVertex *pVtxSPD = fVevent -> GetPrimaryVertexSPD();
    Double_t NcontVSPD = pVtxSPD -> GetNContributors();
    // fNcontVSPD -> Fill(NcontVSPD);
    // --- Global Vtx --- //
    const AliVVertex *pVtx = fVevent -> GetPrimaryVertex();
    Double_t NcontV = pVtx -> GetNContributors();
    Double_t Zvertex = pVtx -> GetZ();
    // fNcontV -> Fill(NcontV);
    fvtxZ_NoCut -> Fill(Zvertex);
    if(fVevent->IsPileupFromSPDInMultBins()) return; // pile up removal //
    if(!fEventCuts->GoodPrimaryAODVertex(fVevent)) return; // remove Vtx with only TPC track //
    if(!(NcontV >= CutNcontV)) return; // # of contributors cut //
    fvtxZ_NcontCut -> Fill(Zvertex);
    if(TMath::Abs(Zvertex) > CutZver) return; // Z vertex position cut //
    fvtxZ -> Fill(Zvertex);
    //#####################################################################//

    if(Isdebug) cout << "event selection finished" << endl;

    // --- MC simulation data --- //
    AliAODMCParticle* fMCALLparticleM;
    AliAODMCParticle* fMCALLparticleGM;
    AliAODMCParticle* fMCALLparticleGGM;
    AliAODMCParticle* fMCALLparticleGGGM;
    // electrons ID //
    Bool_t flagconv = kFALSE, flagDalitz = kFALSE, flagCharm = kFALSE, flagBeauty = kFALSE;
    // for weighting the PHE //
    Bool_t flagpionweight = kFALSE, flagetaweight = kFALSE;
    Bool_t flagpionfromenhancedeta= kFALSE;
    // for enhance D meson & B meson //
    Bool_t flagHFenhance = kFALSE;
    // MC particles parameters//
    Double_t MCTrackPt, MCTrackPtM, MCTrackPtGM;
    Double_t MCTrackEta;
    Int_t MCTrackpdg, MCTrackpdgM, MCTrackpdgGM, MCTrackpdgGGM, MCTrackpdgGGGM;
    Int_t MCTracklabel, MCTracklabelM, MCTracklabelGM, MCTracklabelGGM, MCTracklabelGGGM;
    Int_t NMCTrack = -999.;
    Int_t NpureMC = -999;
    if(fFlagMC)
    {
      // --- Generared particles Loop --- //
      NMCTrack = fMCarray -> GetEntries();
      NpureMC = GetNpureMCParticle(fMCheader);
      for(Int_t jMCTrack = 0 ; jMCTrack < NMCTrack ; jMCTrack++)
      {
        MCTrackpdg = -999, MCTrackpdgM = -999, MCTrackpdgGM = -999, MCTrackpdgGGM = -999, MCTrackpdgGGGM = -999;
        MCTracklabel = -999, MCTracklabelM = -999, MCTracklabelGM = -999, MCTracklabelGGM = -999, MCTracklabelGGGM = -999;
        MCTrackPt = -999., MCTrackPtM = -999., MCTrackPtGM = -999.;
        MCTrackEta = -999.;
        flagpionweight = kFALSE, flagetaweight = kFALSE, flagpionfromenhancedeta = kFALSE;
        flagHFenhance = kFALSE;
        AliAODMCParticle* fMCALLparticle = (AliAODMCParticle*)fMCarray -> At(jMCTrack);
        MCTrackpdg = fMCALLparticle -> GetPdgCode();
        MCTrackEta = fMCALLparticle -> Eta();
        MCTrackPt = fMCALLparticle -> Pt();
        MCTracklabelM = fMCALLparticle -> GetMother();
        // --- eta cut --- //
        if(MCTrackEta < CutTrackEta[0] || CutTrackEta[1] < MCTrackEta) continue;

        //########################## get particle info ##########################//
        if(MCTracklabelM >= 0)
        {
          fMCALLparticleM = (AliAODMCParticle*)fMCarray -> At(MCTracklabelM);
          MCTrackPtM = fMCALLparticleM -> Pt();
          MCTrackpdgM = fMCALLparticleM -> GetPdgCode();
          MCTracklabelGM = fMCALLparticleM -> GetMother();
        }
        if(MCTracklabelGM >= 0)
        {
          fMCALLparticleGM = (AliAODMCParticle*)fMCarray -> At(MCTracklabelGM);
          MCTrackPtGM = fMCALLparticleGM -> Pt();
          MCTrackpdgGM = fMCALLparticleGM -> GetPdgCode();
          MCTracklabelGGM = fMCALLparticleGM -> GetMother();
        }
        if(MCTracklabelGGM >= 0)
        {
          fMCALLparticleGGM = (AliAODMCParticle*)fMCarray -> At(MCTracklabelGGM);
          MCTrackpdgGGM = fMCALLparticleGGM -> GetPdgCode();
          MCTracklabelGGGM = fMCALLparticleGGM -> GetMother();
        }
        if(MCTracklabelGGGM >= 0)
        {
          fMCALLparticleGGGM = (AliAODMCParticle*)fMCarray -> At(MCTracklabelGGGM);
          MCTrackpdgGGGM = fMCALLparticleGGGM -> GetPdgCode();
        }
        //#######################################################################//

        // --- flag enhance HF --- //
        if(jMCTrack >= NpureMC) flagHFenhance = kTRUE;

        // --- choose only electrons --- //
        if(TMath::Abs(MCTrackpdg) == 11)
        {
          // --- <efficiency caluculation> All electrons --- //
          fMCTrackPtelectron -> Fill(MCTrackPt);

          //########################## PID via PDG code ##########################//
          flagCharm = EnhanceCharmCheck(MCTrackpdgM,MCTrackpdgGM,NpureMC,MCTracklabelM,MCTracklabelGM);
          flagBeauty = EnhanceBeautyCheck(MCTrackpdgM,MCTrackpdgGM,NpureMC,MCTracklabelM,MCTracklabelGM);
          flagconv = ConversionCheck(MCTrackpdgM);
          flagDalitz = DalitzCheck(MCTrackpdgM);
          // --- flag enhance elctrons from conversion or Dalitz decay --- //
          flagpionweight = PionWeighingCheck(MCTrackpdgM,MCTrackpdgGM,MCTracklabelM,MCTracklabelGM,MCTracklabelGGM,NpureMC);
          flagetaweight = EtaWeighingCheck(MCTrackpdgM,MCTrackpdgGM,MCTracklabelM,MCTracklabelGM,MCTracklabelGGM,NpureMC);
          // --- flag enhance electrons from pion from enhance eta (pion -> 3 eta) --- //
          if(MCTrackpdgM == 22 & MCTrackpdgGM == 111 && MCTrackpdgGGM == 221 && MCTracklabelGGGM < 0) flagpionfromenhancedeta = kTRUE;
          else if(MCTrackpdgM == 111 && MCTrackpdgGM == 221 && MCTracklabelGGM < 0) flagpionfromenhancedeta = kTRUE;
          //######################################################################//

          //########################## choose HFE ##########################//
          // --- <efficiency caluculation> All HFE --- //
          if(flagCharm || flagBeauty) fMCTrackPtHFE -> Fill(MCTrackPt);
          if(flagCharm) fMCDdecayE -> Fill(MCTrackPt);
          if(flagBeauty) fMCBdecayE -> Fill(MCTrackPt);
          //################################################################//
          //########################## choose PHE from Hijing ##########################//
          if(flagconv)
          {
              if(MCTrackpdgGM == 111 && MCTracklabelGM < NpureMC) fMCTrackPtPHEHijing -> Fill(MCTrackPt);
              else if(MCTrackpdgGM == 221 && MCTracklabelGM < NpureMC) fMCTrackPtPHEHijing -> Fill(MCTrackPt);
          }
          else if(flagDalitz)
          {
            if(MCTrackpdgM == 111 && MCTracklabelM < NpureMC) fMCTrackPtPHEHijing -> Fill(MCTrackPt);
            else if(MCTrackpdgM == 221 && MCTracklabelM < NpureMC) fMCTrackPtPHEHijing -> Fill(MCTrackPt);
          }
          //############################################################################//
          //########################## choose PHE from enhanced events + no mohter ##########################//
          if(flagconv)
          {
            if(flagpionweight)
            {
              fMCTrackPtPHEnoweighting -> Fill(MCTrackPt);
              fMCTrackPtPHEaftweighting -> Fill(MCTrackPt,PionWeight->Eval(MCTrackPtGM));
            }
            else if(flagetaweight)
            {
              fMCTrackPtPHEnoweighting -> Fill(MCTrackPt);
              fMCTrackPtPHEaftweighting -> Fill(MCTrackPt,EtaWeight->Eval(MCTrackPtGM));
            }
          }
          else if(flagDalitz)
          {
            if(flagpionweight)
            {
              fMCTrackPtPHEnoweighting -> Fill(MCTrackPt);
              fMCTrackPtPHEaftweighting -> Fill(MCTrackPt,PionWeight->Eval(MCTrackPtM));
            }
            else if(flagetaweight)
            {
              fMCTrackPtPHEnoweighting -> Fill(MCTrackPt);
              fMCTrackPtPHEaftweighting -> Fill(MCTrackPt,EtaWeight->Eval(MCTrackPtM));
            }
          }
          //#################################################################################################//
        }
        //########################## choose pion ##########################//
        else if(TMath::Abs(MCTrackpdg) == 111)
        {
          fMCPionInput -> Fill(0.,MCTrackPt);
          if(jMCTrack < NpureMC) fMCPionInput -> Fill(1.,MCTrackPt);
          else if(MCTracklabelM < 0) fMCPionInput -> Fill(2.,MCTrackPt);
          else fMCPionInput -> Fill(3.,MCTrackPt);
        }
        //#################################################################//
        //########################## choose eta ##########################//
        else if(TMath::Abs(MCTrackpdg) == 221)
        {
          fMCEtaInput -> Fill(0.,MCTrackPt);
          if(jMCTrack < NpureMC) fMCEtaInput -> Fill(1.,MCTrackPt);
          else if(MCTracklabelM < 0) fMCEtaInput -> Fill(2.,MCTrackPt);
          else fMCEtaInput -> Fill(3.,MCTrackPt);
        }
        //################################################################//
        //########################## choose D meson  ##########################//
        else if(TMath::Abs(MCTrackpdg) == 411 || TMath::Abs(MCTrackpdg) == 421 || TMath::Abs(MCTrackpdg) == 431 || TMath::Abs(MCTrackpdg) == 413 || TMath::Abs(MCTrackpdg) == 423 || TMath::Abs(MCTrackpdg) == 433)
        {
          if(flagHFenhance) fMCHFhadronpT -> Fill(2.,MCTrackPt);
          else fMCHFhadronpT -> Fill(0.,MCTrackPt);
        }
        //#####################################################################//
        //########################## choose B meson  ##########################//
        else if(TMath::Abs(MCTrackpdg) == 511 || TMath::Abs(MCTrackpdg) == 521 || TMath::Abs(MCTrackpdg) == 531 || TMath::Abs(MCTrackpdg) == 513 || TMath::Abs(MCTrackpdg) == 523 || TMath::Abs(MCTrackpdg) == 533)
        {
          if(flagHFenhance) fMCHFhadronpT -> Fill(3.,MCTrackPt);
          else fMCHFhadronpT -> Fill(1.,MCTrackPt);
        }
        //#####################################################################//
      }
    }

    if(Isdebug && fFlagMC) cout << "MC loop finished" << endl;


    // --- Calo Cluster Loop (EMCal/DCal/PHOS) --- //
    Int_t Nclust;
    if(!fFlagEMCalCorrection) Nclust = fVevent->GetNumberOfCaloClusters();
    if(fFlagEMCalCorrection) Nclust = fCaloClusters_tender->GetEntries();
    Bool_t fClsTypeEMCal = kFALSE, fClsTypeDCal = kFALSE;
    for(Int_t icl=0; icl<Nclust; icl++)
    {
        AliVCluster *clust = 0x0;
        if(!fFlagEMCalCorrection) clust = (AliVCluster*)fVevent->GetCaloCluster(icl);
        if(fFlagEMCalCorrection) clust = dynamic_cast<AliVCluster*>(fCaloClusters_tender->At(icl));
        if(!clust) printf("ERROR: Could not receive cluster matched calibrated from track %d\n", icl);
        fClsTypeEMCal = kFALSE; fClsTypeDCal = kFALSE;
        if(clust && clust->IsEMCAL())
        {
            Float_t EMCalpos[3];
            clust -> GetPosition(EMCalpos);
            TVector3 clustpos(EMCalpos[0],EMCalpos[1],EMCalpos[2]);
            Double_t EMCalphi = clustpos.Phi();
            if(EMCalphi < 0) EMCalphi = EMCalphi + (2*TMath::Pi());
            Double_t EMCaleta = clustpos.Eta();
            //########################## choose EMCal or DCal ##########################//
            //----selects EMCAL+DCAL clusters when fFlagClsTypeEMCal and fFlagClsTypeDCal is kTRUE
            if(EMCalphi > 1.39 && EMCalphi < 3.265) fClsTypeEMCal = kTRUE; //EMCAL : 80 < phi < 187
            else if(EMCalphi > 4.53 && EMCalphi < 5.708) fClsTypeDCal = kTRUE;  //DCAL  : 260 < phi < 327
            if(fFlagClsTypeEMCal && !fFlagClsTypeDCal)
            {
              if(!fClsTypeEMCal)continue; //selecting only EMCAL clusters
            }
            else if(fFlagClsTypeDCal && !fFlagClsTypeEMCal)
            {
              if(!fClsTypeDCal) continue; //selecting only DCAL clusters
            }
            else{};
            //##########################################################################//

            fCaloClustEtaphi -> Fill(EMCaleta,EMCalphi);
            Double_t EMCalclusterE = clust -> E();
            Int_t NCells = clust -> GetNCells();
	          fCaloClusterE -> Fill(EMCalclusterE);
        }
    }

    if(Isdebug) cout << "cluster loop finished" << endl;

    ////////////////////////////////
    //Look for kink mother for AOD//
    ////////////////////////////////
    Int_t numberofvertices = 100;
    if(fAOD) numberofvertices = fAOD->GetNumberOfVertices();
    Double_t listofmotherkink[numberofvertices];
    Int_t numberofmotherkink = 0;
    for(Int_t ivertex=0; ivertex < numberofvertices; ivertex++) {
      AliAODVertex *aodvertex = fAOD->GetVertex(ivertex);
      if(!aodvertex) continue;
      if(aodvertex->GetType()==AliAODVertex::kKink) {
        AliAODTrack *mother = (AliAODTrack *) aodvertex->GetParent();
        if(!mother) continue;
        Int_t idmother = mother->GetID();
        listofmotherkink[numberofmotherkink] = idmother;
        numberofmotherkink++;
      }
    }

    // --- Track Loop --- //
    // MC parameters //
    Bool_t flagpidele;
    Int_t pdg, pdgM, pdgGM, pdgGGM, pdgGGGM;
    Int_t ilabel, ilabelM, ilabelGM, ilabelGGM, ilabelGGGM;
    Double_t TrackPtM = -999., TrackPtGM = -999.;
    // track parameters //
    Double_t DCA[2] = {-999.,-999.};
    Double_t TrackPt = -999., TrackP = -999.;
    Double_t TrackdEdx = -999., TrackNSigma  = -999., TrackNSigmaPi = -999.;
    Double_t TrackPhi = -999., TrackEta = -999.;
    Double_t TrackCharge = -999.;
    Double_t ITSchi2 = -999., TPCchi2NDF = -999.;
    Int_t NTracks = -999;
    if(!fFlagEMCalCorrection) NTracks = fAOD ->GetNumberOfTracks();
    if(fFlagEMCalCorrection)  NTracks = fTracks_tender->GetEntries();           // see how many tracks there are in the event
    for(Int_t i = 0; i < NTracks; i++)
    {
        //################## electron ID flag ##################//
        Bool_t flagNsigmaECut = kFALSE;
        Bool_t flagNsigmaHCut = kFALSE;
        Bool_t flagShowerShapeCut = kFALSE;
        Bool_t flagEopCut = kFALSE;
        //######################################################//

        // loop ove rall these tracks
        AliAODTrack* track;
        if(fFlagEMCalCorrection)
        {
          AliVParticle* Vtrack = 0x0;
          Vtrack = dynamic_cast<AliVTrack*>(fTracks_tender->At(i));
          track = dynamic_cast<AliAODTrack*>(Vtrack);
        }
        if(!fFlagEMCalCorrection) track = static_cast<AliAODTrack*>(fAOD -> GetTrack(i));
              // get a track (type AliAODTrack) from the event
        if(!track) continue;         // if we failed, skip this track

        // tracks parameters //
        TrackP = track -> P();
        TrackPt = track -> Pt();
        TrackCharge = track -> Charge();
        TrackPhi = track -> Phi();
        TrackEta = track -> Eta();
        TrackdEdx = track -> GetTPCsignal();
        TrackNSigma = fPIDresponse -> NumberOfSigmasTPC(track,AliPID::kElectron);
        ITSchi2 = track -> GetITSchi2();
        TPCchi2NDF = track -> Chi2perNDF();
        Int_t TPCNCls = 0, ITSNCls = 0, TPCCrossedRows = 0;
        TPCNCls = track -> GetTPCNcls();
        for(Int_t l = 0 ; l < 6 ; l++) if(TESTBIT(track ->GetITSClusterMap(),l)) ITSNCls++;
        TPCCrossedRows = track -> GetTPCCrossedRows();

        //########################## Standard Track Cut ##########################//
        // successful track fitting in TPC and ITS //
        if(!(track->GetStatus()&AliAODTrack::kITSrefit) || !(track->GetStatus()&AliAODTrack::kTPCrefit)) continue;
        //---minimum cut (filter bit 4)
        if(!track->TestFilterMask(AliAODTrack::kTrkGlobalNoDCA)) continue;
        //---TPC & ITS cluster cut
        if((TPCNCls < CutTPCNCls) || (ITSNCls < CutITSNCls)) continue;
        //---NCrossedRow
        if(TPCCrossedRows < CutNCrossedRow) continue;
        //---chi2 cuts
        if((ITSchi2 >= CutITSchi2) || (TPCchi2NDF >= CutTPCchi2perNDF)) continue;
        //---Require the hit in SPD layer
        if(!(track -> HasPointOnITSLayer(0) || track -> HasPointOnITSLayer(1))) continue;
        //---DCA cut
        Double_t DCA[2] = {-999.,-999.}, CovarianceMatrix[3];
        if(track -> PropagateToDCA(pVtx,fVevent -> GetMagneticField(),20.,DCA,CovarianceMatrix))
        {
            if(TMath::Abs(DCA[0]) > CutDCAxy || TMath::Abs(DCA[1]) > CutDCAz) continue;
        }
        //---Eta Cut
        if(TrackEta < CutTrackEta[0] || CutTrackEta[1] < TrackEta) continue;
        //---reject kink
        Bool_t kinkmotherpass = kTRUE;
        for(Int_t kinkmother = 0; kinkmother < numberofmotherkink; kinkmother++) {
          if(track->GetID() == listofmotherkink[kinkmother]) {
            kinkmotherpass = kFALSE;
            continue;
          }
        }
        if(!kinkmotherpass) continue;
        //########################################################################//

        fTrackPt -> Fill(TrackPt);
        fTrackphieta -> Fill(TrackEta,TrackPhi);
        // fdEdx -> Fill(TrackP,TrackdEdx);
        fNSigmaTPC -> Fill(TrackP,TrackNSigma);
        fNsigmaEta -> Fill(TrackEta,TrackNSigma);
        fDCAxy -> Fill(TrackPt,DCA[0]*TrackCharge);
        fDCAz -> Fill(TrackPt,DCA[1]*TrackCharge);
        fTrackChi2 -> Fill(0.,ITSchi2);
        fTrackChi2 -> Fill(1.,TPCchi2NDF);
        fTrackCluster -> Fill(0.,ITSNCls);
        fTrackCluster -> Fill(1.,TPCNCls);
        fTrackCluster -> Fill(2.,TPCCrossedRows);

        // --- Nsigma flag --- //
        if((TrackNSigma > CutNsigmaE[0]) && (TrackNSigma < CutNsigmaE[1])) flagNsigmaECut = kTRUE;
        if(TrackNSigma < CutNsigmaH[0]) flagNsigmaHCut = kTRUE;

        // --- MC information --- //
        flagpidele = kFALSE;
        pdg = -999, pdgM = -999, pdgGM = -999, pdgGGM = -999, pdgGGGM = -999;
        ilabelM = -999, ilabelGM = -999, ilabelGGM = -999, ilabelGGGM = -999;
        if(fFlagMC)
        {
          ilabel = track -> GetLabel();
          if(ilabel > 0 && fMCarray)
          {
            //########################## get particle info ##########################//
            fMCparticle = (AliAODMCParticle*)fMCarray -> At(ilabel);
            pdg = fMCparticle -> GetPdgCode();
            ilabelM = fMCparticle -> GetMother();
            if(ilabelM >= 0)
            {
              AliAODMCParticle* fMCparticleM = (AliAODMCParticle*)fMCarray -> At(ilabelM);
              TrackPtM = fMCparticleM -> Pt();
              pdgM = fMCparticleM -> GetPdgCode();
              ilabelGM = fMCparticleM -> GetMother();
            }
            if(ilabelGM >= 0)
            {
              AliAODMCParticle* fMCparticleGM = (AliAODMCParticle*)fMCarray -> At(ilabelGM);
              TrackPtGM = fMCparticleGM -> Pt();
              pdgGM = fMCparticleGM -> GetPdgCode();
            }
            //#######################################################################//
            if(TMath::Abs(pdg) == 11) flagpidele = kTRUE;
            if(flagpidele)
            {
              // --- check mother speicies --- //
              fMCPDGcodeM -> Fill(pdgM), fMCPDGcodeGM -> Fill(pdgGM);
              //########################## PID via PDG code ##########################//
              flagCharm = EnhanceCharmCheck(pdgM,pdgGM,NpureMC,ilabelM,ilabelGM);
              flagBeauty = EnhanceBeautyCheck(pdgM,pdgGM,NpureMC,ilabelM,ilabelGM);
              flagconv = ConversionCheck(pdgM);
              flagDalitz = DalitzCheck(pdgM);
              // --- flag enhance electrons from conversion or Dalitz decay --- //
              flagpionweight = PionWeighingCheck(pdgM,pdgGM,ilabelM,ilabelGM,ilabelGGM,NpureMC);
              flagetaweight = EtaWeighingCheck(pdgM,pdgGM,ilabelM,ilabelGM,ilabelGGM,NpureMC);
              // --- flag enhance electrons from pion from enhance eta (pion -> 3 eta) --- //
              if(pdgM == 22 & pdgGM == 111 && pdgGGM == 221 && ilabelGGGM < 0) flagpionfromenhancedeta = kTRUE;
              if(pdgM == 111 && pdgGM == 221 && ilabelGGM < 0) flagpionfromenhancedeta = kTRUE;
              //######################################################################//
              // --- <effeciency caluculation> reco track --- //
              fMCTrackPtelectron_reco -> Fill(TrackPt);
              if(flagCharm || flagBeauty)
              {
                fMCTrackPtHFE_reco -> Fill(TrackPt);
                fMCHFEResponceMatrix -> Fill(fMCparticle->Pt(),TrackPt);
              }
              // ---  <effeciency caluculation> electrons w/TPCPID --- //
              if(flagNsigmaECut) fMCTrackPtelectron_wTPCPID -> Fill(TrackPt);
              if((flagCharm || flagBeauty) && flagNsigmaECut) fMCTrackPtHFE_wTPCPID -> Fill(TrackPt);
              // --- PID check --- //
              fMCTPCNSigmaelectron -> Fill(TrackPt,TrackNSigma);
              fMCNsigmaEtaElectron -> Fill(TrackEta,TrackNSigma);
            }
          }
        }

        // --- calo clusters matched with tracks --- //
        Int_t EMCalIndex = -1;
        Double_t EMCalClusterE = -999., EMCalphi = -999, EMCaleta = -999, EMCalTOF = -999.;
        Double_t TrackphionEMCal = -999., TracketaonEMCal = -999.;
        Double_t EMCalTrackDiffphi = -999., EMCalTrackDiffeta = -999.;
        fClsTypeEMCal = kFALSE, fClsTypeDCal = kFALSE;
        EMCalIndex = track -> GetEMCALcluster(); // get index of EMCal cluster which matched to track
        AliVCluster *clustMatch=0x0;
        if(fFlagEMCalCorrection) if(EMCalIndex >= 0) clustMatch = dynamic_cast<AliVCluster*>(fCaloClusters_tender->At(EMCalIndex)); // address cluster matched to track
        if(!fFlagEMCalCorrection) if(EMCalIndex >= 0) clustMatch = (AliVCluster*)fVevent->GetCaloCluster(EMCalIndex);
        if(clustMatch && clustMatch->IsEMCAL())
        {
            Float_t EMCalpos[3];
            clustMatch -> GetPosition(EMCalpos);
            TVector3 clustpos(EMCalpos[0],EMCalpos[1],EMCalpos[2]);
            EMCalphi = clustpos.Phi();
            if(EMCalphi < 0) EMCalphi = EMCalphi + (2*TMath::Pi());
            EMCaleta = clustpos.Eta();
            EMCalClusterE = clustMatch -> E();
            EMCalTOF = clustMatch -> GetTOF()*1e+9;
            Double_t M02 = -999., M20 = -999.;
            M02 = clustMatch -> GetM02(); // long axis
            M20 = clustMatch -> GetM20(); // short axis
            Double_t Eop = -1.0;
            if(TrackP > 0) Eop = EMCalClusterE / TrackP;

            //########################## choose EMCal or DCal ##########################//
            //selects EMCAL+DCAL clusters when fFlagClsTypeEMCal and fFlagClsTypeDCal is kTRUE
            if(EMCalphi > 1.39 && EMCalphi < 3.265) fClsTypeEMCal = kTRUE; //EMCAL : 80 < phi < 187
            else if(EMCalphi > 4.53 && EMCalphi < 5.708) fClsTypeDCal = kTRUE;  //DCAL  : 260 < phi < 327
            if(fFlagClsTypeEMCal && !fFlagClsTypeDCal)
            {
              if(!fClsTypeEMCal)continue; //selecting only EMCAL clusters
            }
            else if(fFlagClsTypeDCal && !fFlagClsTypeEMCal)
            {
              if(!fClsTypeDCal) continue; //selecting only DCAL clusters
            }
            else{};
            //##########################################################################//

            // --- EMCalTrackDiffCut --- //
            TrackphionEMCal = track -> GetTrackPhiOnEMCal();
            TracketaonEMCal = track -> GetTrackEtaOnEMCal();
            EMCalTrackDiffphi = TVector2::Phi_mpi_pi(TrackphionEMCal - EMCalphi);
            EMCalTrackDiffeta = TracketaonEMCal - EMCaleta;
            if(TMath::Abs(EMCalTrackDiffphi) > CutPhidiff || TMath::Abs(EMCalTrackDiffeta) > CutEtadiff) continue;
            fCaloTrackDiff -> Fill(EMCalTrackDiffphi,EMCalTrackDiffeta);

            // only MC //
            if(fFlagMC && flagpidele)
            {
              // ---  <effeciency caluculation> electrons matched with Calo --- //
              if(flagNsigmaECut) fMCTrackPtelectron_wmatch -> Fill(TrackPt);
              if((flagCharm || flagBeauty) && flagNsigmaECut) fMCTrackPtHFE_wmatch -> Fill(TrackPt);
              // --- E/p check --- //
              if((flagCharm || flagBeauty) && TrackP > 0) fMCHFEEop -> Fill(TrackPt,Eop);
            }

            // --- matched tracks parameters --- //
            fTrackPtAfterMatch -> Fill(TrackPt);
            fTrackphietaAfterMatch -> Fill(TrackEta,TrackPhi);

            //--- matched clusters parameters --- //
            fCaloClustEtaPhiAfterMatch -> Fill(EMCaleta,EMCalphi);
    	      fCaloClusterEAfterMatch-> Fill(EMCalClusterE);

            // electron timing //
            if(flagNsigmaECut) fCaloClustTiming -> Fill(EMCalClusterE,EMCalTOF);

            fM02 -> Fill(TrackPt,M02);
            fM20 -> Fill(TrackPt,M20);
            fEop -> Fill(TrackPt,Eop);
            if(TrackPt > 3.0) fNSigmaEop -> Fill(Eop,TrackNSigma);

            if(M20 > CutM20[0] && M20 < CutM20[1]) flagShowerShapeCut = kTRUE;
            if(Eop > CutEopE[0] && Eop < CutEopE[1]) flagEopCut = kTRUE;

            // --- electron candadates
            if(flagNsigmaECut && flagShowerShapeCut) fEopElectron -> Fill(TrackPt,Eop);
            // --- estimating hadron contamination
            if(flagNsigmaHCut && flagShowerShapeCut) fEopHadron -> Fill(TrackPt,Eop);
            // --- TPC energy loss for electron, hadrons --- //
            if(flagEopCut && flagShowerShapeCut)
            {
              fNsigmaEtaElectron -> Fill(TrackEta,TrackNSigma);
              fNSigmaTPCelectron -> Fill(TrackPt,TrackNSigma);
            }
            if(Eop < 0.6 && flagShowerShapeCut) fNSigmaTPChadron -> Fill(TrackPt,TrackNSigma);

            //########################## PID cut in TPC & EMCal/DCal ##########################//
            if(!flagNsigmaECut || !flagShowerShapeCut || !flagEopCut) continue;
            //#################################################################################//
            fElectronphieta -> Fill(TrackEta,TrackPhi);

            // only MC //
            if(fFlagMC && flagpidele)
            {
              // ---  <effeciency caluculation> electrons w/CaloPID --- //
              fMCTrackPtelectron_wEMCALPID -> Fill(TrackPt);
              if(flagCharm || flagBeauty) fMCTrackPtHFE_wEMCALPID -> Fill(TrackPt);
              //########################## choose PHE from Hijing ##########################//
              if(flagconv)
              {
                  if(pdgGM == 111 && ilabelGM < NpureMC) fMCTrackPtPHEHijingwPID -> Fill(TrackPt);
                  else if(pdgGM == 221 && ilabelGM < NpureMC) fMCTrackPtPHEHijingwPID -> Fill(TrackPt);
              }
              else if(flagDalitz)
              {
                if(pdgM == 111 && ilabelM < NpureMC) fMCTrackPtPHEHijingwPID -> Fill(TrackPt);
                else if(pdgM == 221 && ilabelM < NpureMC) fMCTrackPtPHEHijingwPID -> Fill(TrackPt);
              }
              //############################################################################//

              //########################## choose PHE from enhanced events + no mohter ##########################//
              if(!flagpionfromenhancedeta)
              {
                if(flagconv)
                {
                  if(flagpionweight)
                  {
                    fMCTrackPtPHEwPIDnoweighting -> Fill(TrackPt);
                    fMCTrackPtPHEwPIDaftweighting -> Fill(TrackPt,PionWeight->Eval(TrackPtGM));
                  }
                  else if(flagetaweight)
                  {
                    fMCTrackPtPHEwPIDnoweighting -> Fill(TrackPt);
                    fMCTrackPtPHEwPIDaftweighting -> Fill(TrackPt,EtaWeight->Eval(TrackPtGM));
                  }
                }
                else if(flagDalitz)
                {
                  if(flagpionweight)
                  {
                    fMCTrackPtPHEwPIDnoweighting -> Fill(TrackPt);
                    fMCTrackPtPHEwPIDaftweighting -> Fill(TrackPt,PionWeight->Eval(TrackPtM));
                  }
                  else if(flagetaweight)
                  {
                    fMCTrackPtPHEwPIDnoweighting -> Fill(TrackPt);
                    fMCTrackPtPHEwPIDaftweighting -> Fill(TrackPt,EtaWeight->Eval(TrackPtM));
                  }
                }
              }
              //#################################################################################################//

              // //########################## DCA distribution for templete fit ##########################//
              // fMCDCAinclusive -> Fill(TrackPt,DCA[0]*TrackCharge);
              // if(flagconv) fMCDCAconv -> Fill(TrackPt,DCA[0]*TrackCharge);
              // else if(flagDalitz) fMCDCAdalitz -> Fill(TrackPt,DCA[0]*TrackCharge);
              // else if(flagCharm) fMCDCAcharm -> Fill(TrackPt,DCA[0]*TrackCharge);
              // else if(flagBeauty) fMCDCAbeauty -> Fill(TrackPt,DCA[0]*TrackCharge);
              // if(flagconv || flagDalitz) fMCDCAPHE -> Fill(TrackPt,DCA[0]*TrackCharge);
              // //#######################################################################################//d
            }

            // --- Invariant mass caluculation --- //
            Bool_t fFlagPHEMassULS = kFALSE, fFlagPHEMassLS = kFALSE;
            // --- associated track loop --- //
            Int_t ntracks = -999;
            if(fFlagEMCalCorrection) ntracks = fTracks_tender->GetEntries();
            if(!fFlagEMCalCorrection) ntracks = fVevent -> GetNumberOfTracks();
            for(Int_t jtrack = 0 ; jtrack < ntracks ; jtrack++)
            {
                AliAODTrack *assotrack;
                if(fFlagEMCalCorrection)
                {
                  AliVParticle* VAssotrack = 0x0;
                  VAssotrack = dynamic_cast<AliVTrack*>(fTracks_tender->At(jtrack));
                  assotrack = dynamic_cast<AliAODTrack*>(VAssotrack);
                }
                if(!fFlagEMCalCorrection) assotrack = static_cast<AliAODTrack*>(fAOD -> GetTrack(jtrack));
                if(!assotrack) continue;  //if we failed, skip this track

                //trigE : triggered electron, assoE : associated elctron
                Double_t Mass = -999., SigmaMass = -999.;
                Int_t PDGtrigE = 11, PDGassoE = 11;
                Int_t TrackCharge = track -> Charge(), AssotrackCharge = assotrack -> Charge();
                Double_t AssotrackEta = assotrack -> Eta();
                Double_t AssotrackPt = assotrack -> Pt();
                Double_t AssotrackNsigma = fPIDresponse -> NumberOfSigmasTPC(assotrack,AliPID::kElectron);
                Bool_t flagLS = kFALSE, flagULS = kFALSE;
                if(TrackCharge > 0) PDGtrigE = -11;
                if(AssotrackCharge > 0) PDGassoE = -11;
                if(TrackCharge == AssotrackCharge) flagLS = kTRUE;
                if(TrackCharge != AssotrackCharge) flagULS = kTRUE;
                Int_t AssoTPCNCls = 0, AssoITSNCls = 0;
                AssoTPCNCls = assotrack -> GetTPCNcls();
                for(Int_t l = 0 ; l < 6 ; l++) if(TESTBIT(assotrack -> GetITSClusterMap(),l)) AssoITSNCls++;
                Double_t AssoTPCchi2perNDF = assotrack -> Chi2perNDF();

                //########################## Associated Track Cut ##########################//
                //---reject same track
                if(i == jtrack) continue;
                // successful track fitting in TPC and ITS //
                if(!(assotrack->GetStatus()&AliAODTrack::kITSrefit) || !(assotrack->GetStatus()&AliAODTrack::kTPCrefit)) continue;
                //---filter bit
                if(!assotrack -> TestFilterMask(AliAODTrack::kTrkTPCOnly)) continue;
                //---TPC cluster cut
                if(AssoTPCNCls < CutAssoTPCNCls) continue;
                //---chi2 cut
                if(AssoTPCchi2perNDF >= CutAssoTPCchi2perNDF) continue;
                //---reject kink
                Bool_t kinkmotherpass = kTRUE;
                for(Int_t kinkmother = 0; kinkmother < numberofmotherkink; kinkmother++) {
                  if(assotrack->GetID() == listofmotherkink[kinkmother]) {
                    kinkmotherpass = kFALSE;
                    continue;
                  }
                }
                if(!kinkmotherpass) continue;
                //---loose cut on pair electron
                if(AssotrackPt < CutPairEPt) continue;
                if(AssotrackEta < CutPairEEta[0] || AssotrackEta > CutPairEEta[1]) continue;
                if(AssotrackNsigma < CutPairENsigma[0] || AssotrackNsigma > CutPairENsigma[1]) continue;
                //##########################################################################//

                AliKFParticle::SetField(fVevent -> GetMagneticField());
                AliKFParticle trigE = AliKFParticle(*track,PDGtrigE);
                AliKFParticle assoE = AliKFParticle(*assotrack,PDGassoE);
                AliKFParticle recg(trigE,assoE);

                //quality cut of two-pairs of electrons
                if(recg.GetNDF() < 1) continue;
                if(TMath::Sqrt(TMath::Abs(recg.GetChi2()/recg.GetNDF())) > 3.) continue;

                // --- get Mass & SigmaMass --- //
                Int_t GetMassInfo = recg.GetMass(Mass,SigmaMass);
                if(flagLS) fInvmassLS -> Fill(TrackPt,Mass);
                else if(flagULS) fInvmassULS -> Fill(TrackPt,Mass);

                // --- check if a triggered electron has a photonic pair from associated tracks --- //
                if(flagLS && Mass < CutPhotEMass) fFlagPHEMassLS = kTRUE;
                if(flagULS && Mass < CutPhotEMass) fFlagPHEMassULS = kTRUE;
            }
            if(fFlagPHEMassLS)
            {
              fPHEpTLS -> Fill(TrackPt);
              // if(fFlagMC && flagpidele && (flagconv || flagDalitz)) fMCDCAPHELS -> Fill(TrackPt,DCA[0]*TrackCharge);
            }
            if(fFlagPHEMassULS)
            {
              fPHEpTULS -> Fill(TrackPt);
              if(fFlagMC && flagpidele)
              {
                // if(flagconv || flagDalitz) fMCDCAPHEULS -> Fill(TrackPt,DCA[0]*TrackCharge);

                //########################## choose PHE from Hijing ##########################//
                if(flagconv)
                {
                    if(pdgGM == 111 && ilabelGM < NpureMC) fMCTrackPtPHEHijingwMasscut -> Fill(TrackPt);
                    else if(pdgGM == 221 && ilabelGM < NpureMC) fMCTrackPtPHEHijingwMasscut -> Fill(TrackPt);
                }
                else if(flagDalitz)
                {
                  if(pdgM == 111 && ilabelM < NpureMC) fMCTrackPtPHEHijingwMasscut -> Fill(TrackPt);
                  else if(pdgM == 221 && ilabelM < NpureMC) fMCTrackPtPHEHijingwMasscut -> Fill(TrackPt);
                }
                //############################################################################//

                //########################## choose PHE from enhanced events + no mohter ##########################//
                if(!flagpionfromenhancedeta)
                {
                  if(flagconv)
                  {
                    if(flagpionweight)
                    {
                      fMCTrackPtPHEwMasscutnoweighting -> Fill(TrackPt);
                      fMCTrackPtPHEwMasscutaftweighting -> Fill(TrackPt,PionWeight->Eval(TrackPtGM));
                    }
                    else if(flagetaweight)
                    {
                      fMCTrackPtPHEwMasscutnoweighting -> Fill(TrackPt);
                      fMCTrackPtPHEwMasscutaftweighting -> Fill(TrackPt,EtaWeight->Eval(TrackPtGM));
                    }
                  }
                  else if(flagDalitz)
                  {
                    if(flagpionweight)
                    {
                      fMCTrackPtPHEwMasscutnoweighting -> Fill(TrackPt);
                      fMCTrackPtPHEwMasscutaftweighting-> Fill(TrackPt,PionWeight->Eval(TrackPtM));
                    }
                    else if(flagetaweight)
                    {
                      fMCTrackPtPHEwMasscutnoweighting -> Fill(TrackPt);
                      fMCTrackPtPHEwMasscutaftweighting -> Fill(TrackPt,EtaWeight->Eval(TrackPtM));
                    }
                  }
                }
                //#################################################################################################//
              }
            }
        }
    }                                                   // continue until all the tracks are processed

    if(Isdebug) cout << "track loop finished" << endl;

    PostData(1, fOutputList);                           // stream the results the analysis of this event to
                                                        // the output manager which will take care of writing
                                                        // it to a file
}


// #################################################################################
// Functions for MC analyis
// #################################################################################
//_____________________________________________________________________________
void AliAnalysisTaskCaloHFEpPbRun2::FindMother(AliAODMCParticle* part, Int_t &label, Int_t &pid)
{
  // Find mother in case of MC

  if(part->GetMother()>-1)
  {
    label = part->GetMother();
    AliAODMCParticle *partM = (AliAODMCParticle*) fMCarray->At(label);
    pid = partM->GetPdgCode();
  }
  else
  {
    pid = -1;
  }
}

//_____________________________________________________________________________
Bool_t AliAnalysisTaskCaloHFEpPbRun2::ConversionCheck(Int_t &pdgM)
{
  if(pdgM == 22) return kTRUE;
  else return kFALSE;
}

//_____________________________________________________________________________
Bool_t AliAnalysisTaskCaloHFEpPbRun2::DalitzCheck(Int_t &pdgM)
{
  if(pdgM == 111 || pdgM == 221) return kTRUE;
  else return kFALSE;
}

//_____________________________________________________________________________
Bool_t AliAnalysisTaskCaloHFEpPbRun2::CharmCheck(Int_t &pdgM, Int_t &pdgGM)
{
  if(TMath::Abs(pdgM) == 411 || TMath::Abs(pdgM) == 421 || TMath::Abs(pdgM) == 431 || TMath::Abs(pdgM) == 413 || TMath::Abs(pdgM) == 423 || TMath::Abs(pdgM) == 433)
  {
    if(TMath::Abs(pdgGM) == 511 || TMath::Abs(pdgGM) == 521 || TMath::Abs(pdgGM) == 531 || TMath::Abs(pdgGM) == 541 || TMath::Abs(pdgGM) == 513 || TMath::Abs(pdgGM) == 523 || TMath::Abs(pdgGM) == 533 || TMath::Abs(pdgGM) == 543)
    {
      return kFALSE;
    }
    else return kTRUE;
  }
  else return kFALSE;
}

//_____________________________________________________________________________
Bool_t AliAnalysisTaskCaloHFEpPbRun2::EnhanceCharmCheck(Int_t &pdgM, Int_t &pdgGM, Int_t &NpureMC, Int_t &labelM, Int_t &labelGM)
{
  Int_t PDGM = TMath::Abs(pdgM);
  Int_t PDGGM = TMath::Abs(pdgGM);
  if((labelM >= NpureMC) && (PDGM == 411 || PDGM == 421 || PDGM == 431 || PDGM == 413 || PDGM == 423 || PDGM == 433))
  {
    if(PDGGM == 511 || PDGGM == 521 || PDGGM == 531 || PDGGM == 513 || PDGGM == 523 || PDGGM == 533)
    {
      return kFALSE;
    }
    else return kTRUE;
  }
  else return kFALSE;
}

//_____________________________________________________________________________
Bool_t AliAnalysisTaskCaloHFEpPbRun2::BeautyCheck(Int_t &pdgM, Int_t &pdgGM)
{
  if(TMath::Abs(pdgM) == 511 || TMath::Abs(pdgM) == 521 || TMath::Abs(pdgM) == 531 || TMath::Abs(pdgM) == 541 || TMath::Abs(pdgM) == 513 || TMath::Abs(pdgM) == 523 || TMath::Abs(pdgM) == 533 || TMath::Abs(pdgM) == 543)
  {
    return kTRUE;
  }
  else if(TMath::Abs(pdgM) == 411 || TMath::Abs(pdgM) == 421 || TMath::Abs(pdgM) == 431 || TMath::Abs(pdgM) == 413 || TMath::Abs(pdgM) == 423 || TMath::Abs(pdgM) == 433)
  {
    if(TMath::Abs(pdgGM) == 511 || TMath::Abs(pdgGM) == 521 || TMath::Abs(pdgGM) == 531 || TMath::Abs(pdgGM) == 541 || TMath::Abs(pdgGM) == 513 || TMath::Abs(pdgGM) == 523 || TMath::Abs(pdgGM) == 533 || TMath::Abs(pdgGM) == 543)
    {
      return kTRUE;
    }
    else return kFALSE;
  }
  else return kFALSE;
}


//_____________________________________________________________________________
Bool_t AliAnalysisTaskCaloHFEpPbRun2::EnhanceBeautyCheck(Int_t &pdgM, Int_t &pdgGM, Int_t &NpureMC, Int_t &labelM, Int_t &labelGM)
{
  Int_t PDGM = TMath::Abs(pdgM);
  Int_t PDGGM = TMath::Abs(pdgGM);
  if((labelM >= NpureMC) && (PDGM == 511 || PDGM == 521 || PDGM == 531 || PDGM == 513 || PDGM == 523 || PDGM == 533))
  {
    return kTRUE;
  }
  else if(PDGM == 411 || PDGM == 421 || PDGM == 431 || PDGM == 413 || PDGM == 423 || PDGM == 433)
  {
    if((labelGM >= NpureMC) && (PDGGM == 511 || PDGGM == 521 || PDGGM == 531 || PDGGM == 513 || PDGGM == 523 || PDGGM == 533))
    {
      return kTRUE;
    }
    else return kFALSE;
  }
  else return kFALSE;
}

//_____________________________________________________________________________
Bool_t AliAnalysisTaskCaloHFEpPbRun2::PionWeighingCheck(Int_t &pdgM, Int_t &pdgGM, Int_t &labelM, Int_t &labelGM , Int_t &labelGGM, Int_t &NpureMC)
{
  if(pdgM == 111 && labelM >= NpureMC && labelGM < 0) return kTRUE; // Dalitz
  else if(pdgM == 22 && pdgGM == 111 && labelGM >= NpureMC && labelGGM < 0) return kTRUE; //conversion
  else return kFALSE;
}

//_____________________________________________________________________________
Bool_t AliAnalysisTaskCaloHFEpPbRun2::EtaWeighingCheck(Int_t &pdgM, Int_t &pdgGM, Int_t &labelM, Int_t &labelGM , Int_t &labelGGM, Int_t &NpureMC)
{
  if(pdgM == 221 && labelM >= NpureMC && labelGM < 0) return kTRUE; // Dalitz
  else if(pdgM == 22 && pdgGM == 221 && labelGM >= NpureMC && labelGGM < 0) return kTRUE; //conversion
  else return kFALSE;
}


//_____________________________________________________________________________
Int_t AliAnalysisTaskCaloHFEpPbRun2::GetNpureMCParticle(AliAODMCHeader* fMCheader)
{
  TList *lh = fMCheader->GetCocktailHeaders();
  Int_t NpureMC = 0;
  if(lh)
  {
    for(Int_t igene = 0; igene < lh -> GetEntries(); igene++)
    {
      AliGenEventHeader* gh = (AliGenEventHeader*)lh -> At(igene);
      if(gh && igene == 0)
      {
         NpureMC = gh -> NProduced(); // generate by PYTHIA or HIJING
      }
    }
  }
  return NpureMC;
}


//_____________________________________________________________________________
void AliAnalysisTaskCaloHFEpPbRun2::Terminate(Option_t *)
{
    // terminate
    // called at the END of the analysis (when all events are processed)
}
//_____________________________________________________________________________
