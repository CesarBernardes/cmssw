import FWCore.ParameterSet.Config as cms

process = cms.Process('TRACKVARDATA')
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.Geometry.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')


process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)

process.options = cms.untracked.PSet(
wantSummary = cms.untracked.bool(True),
###SkipEvent = cms.untracked.vstring('ProductNotFound') ###cesar
)

process.TFileService = cms.Service("TFileService",
    fileName = cms.string('trkVar_DATA.root')
)

###from GO - begin
process.load("RecoVertex.PrimaryVertexProducer.OfflinePrimaryVerticesRecovery_cfi")
process.load('HeavyIonsAnalysis.EventAnalysis.skimanalysis_cfi')
process.load('HeavyIonsAnalysis.Configuration.collisionEventSelection_cff')
process.load('HeavyIonsAnalysis.Configuration.hfCoincFilter_cff')

#Centrality
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '103X_dataRun2_Prompt_v3', '')
print('\n\033[31m~*~ USING CENTRALITY TABLE FOR PbPb 2018 ~*~\033[0m\n')
process.GlobalTag.snapshotTime = cms.string("9999-12-31 23:59:59.000")
process.GlobalTag.toGet.extend([
    cms.PSet(record = cms.string("HeavyIonRcd"),
        tag = cms.string("CentralityTable_HFtowers200_DataPbPb_periHYDJETshape_run2v1031x02_offline"),
        connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS"),
        label = cms.untracked.string("HFtowers")
    ),
])

process.load("RecoHI.HiCentralityAlgos.CentralityBin_cfi")
process.centralityBin.Centrality = cms.InputTag("hiCentrality")
process.centralityBin.centralityVariable = cms.string("HFtowers")

###from GO - end


# Input source
process.source = cms.Source("PoolSource",
    duplicateCheckMode = cms.untracked.string("noDuplicateCheck"),
    fileNames =  cms.untracked.vstring(
'/store/hidata/HIRun2018A/HIMinimumBias1/AOD/PromptReco-v1/000/326/586/00000/34B8A132-7E18-BC48-A164-07A4E727FD33.root'
    ),
    eventsToProcess = cms.untracked.VEventRange('326586:55459824')
)


process.trackingAnalyzer = cms.EDAnalyzer("TrackingAnalyzer",
        centralitySrc     = cms.InputTag("centralityBin","HFtowers"),
        vertexSrc         = cms.InputTag("offlinePrimaryVerticesRecovery"), 
        ###vertexSrc         = cms.InputTag("offlinePrimaryVertices"),
        generalTracksSrc  = cms.InputTag("generalTracks"),
        pixelTracksSrc    = cms.InputTag("hiConformalPixelTracks"),
        useGeneralTracks  = cms.bool(True)
)

process.p = cms.Path(
                     process.offlinePrimaryVerticesRecovery * ###from GO event selection
                     process.primaryVertexFilter * ###from GO event selection 
                     process.hfCoincFilter2Th4 * ###from GO event selection
                     process.clusterCompatibilityFilter * ###from GO event selection
                     process.centralityBin * ###from GO event selection
                     process.trackingAnalyzer 
)

###from GO - new pv
from HLTrigger.Configuration.CustomConfigs import MassReplaceInputTag
process = MassReplaceInputTag(process,"offlinePrimaryVertices","offlinePrimaryVerticesRecovery")
process.offlinePrimaryVerticesRecovery.oldVertexLabel = "offlinePrimaryVertices"


