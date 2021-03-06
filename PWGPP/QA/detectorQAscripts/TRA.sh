#available variables:
#  $dataType     e.g. data or sim
#  $year         e.g. 2011
#  $period       e.g. LHC13g
#  $runNumber     e.g. 169123
#  $pass         e.g. cpass1,pass1,passMC
#  #ocdbStorage  e.g. "raw://", "local://./OCDB"

runLevelQA()
{
  qaFile=$1

  cp $ALICE_PHYSICS/PWGPP/macros/PlotESDtrackQA.C .
  root -b -q -l "PlotESDtrackQA.C(\"$qaFile\",\"QA\",${runNumber})"
}

periodLevelQA()
{
  trendingFile=$1

  cp $ALICE_PHYSICS/PWGPP/macros/DrawTrendingESDTrackQA.C .
  aliroot -b -q -l "DrawTrendingESDTrackQA.C(\"trending.root\")"
}
