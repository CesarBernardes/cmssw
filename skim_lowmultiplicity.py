import FWCore.ParameterSet.Config as cms
process = cms.Process("ANASKIM")

process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('HeavyIonsAnalysis.Configuration.collisionEventSelection_cff')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_PostLS1_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.MessageLogger.cerr.FwkReport.reportEvery = 100

process.source = cms.Source("PoolSource",
   fileNames = cms.untracked.vstring(
'/store/hidata/PARun2016C/PAMinimumBias1/AOD/PromptReco-v1/000/286/496/00000/FC7A9DA0-A9BD-E611-AC04-02163E0146AA.root'
)
)

# =============== Other Statements =====================
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(1000))
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))
process.GlobalTag.globaltag = '80X_dataRun2_Prompt_v15'


# =============== Import Sequences =====================

# filter in NtrkOffline
process.trackMultiplicityFilter = cms.EDFilter("TrackMultiplicityFilter",
   vertexCollection = cms.InputTag("offlinePrimaryVertices"),      
   generalTrackTag  = cms.InputTag("generalTracks"),
   nTrackMinCut = cms.int32(0),
   nTrackMaxCut = cms.int32(40)  
)

process.PAcollisionEventSelection = cms.Sequence(
process.trackMultiplicityFilter
)

process.eventFilter_LM = cms.Sequence( 
process.PAcollisionEventSelection
)

process.eventFilter_LM_step = cms.Path( process.eventFilter_LM )

process.output_LM = cms.OutputModule("PoolOutputModule",
    outputCommands = process.AODEventContent.outputCommands,
    fileName = cms.untracked.string('pPb_skim_LM.root'),
    SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('eventFilter_LM_step')),
    dataset = cms.untracked.PSet(
      dataTier = cms.untracked.string('AOD')
    )
)

process.output_LM_step = cms.EndPath(process.output_LM)

process.schedule = cms.Schedule(
    process.eventFilter_LM_step,
    process.output_LM_step
)
